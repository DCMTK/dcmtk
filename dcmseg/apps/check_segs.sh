#!/bin/bash

set -e

# Option for dump comparison
COMPARE_DUMP=false
VERBOSE=false
IGNORE_MISSING=false

# Argument parsing
FILES=()

while [[ $# -gt 0 ]]; do
    case "$1" in
        -d)
            COMPARE_DUMP=true
            shift
            ;;
        -v)
            VERBOSE=true
            shift
            ;;
        -i)
            IGNORE_MISSING=true
            echo "Warning: Using --ignore-missing will skip checks for missing input files."
            shift
            ;;
        *)
            FILES+=("$1")
            shift
            ;;
    esac
done

if $VERBOSE; then
    set -x
fi

if [ ${#FILES[@]} -eq 0 ]; then
    echo "Usage: $0 [-d] [-v] [-i] <dicom-files-or-pattern>"
    echo "Example: $0 -d -v -i /path/to/files/*.dcm"
    exit 1
fi

OUTPUT_DIR="/tmp/DIGEST/output"
TEMP_DIR="/tmp/DIGEST"

echo "Using temporary directory: ${TEMP_DIR}"
rm -rf "${TEMP_DIR}"
mkdir -p "$TEMP_DIR"
rm -rf "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR"

echo "========== Pass 1: With segdigest =========="

for file in "${FILES[@]}"; do
    if [ -f "$file" ]; then
        base=$(basename "$file")
        decompressed_file="${TEMP_DIR}/decompressed_${base}"
        converted_file="${TEMP_DIR}/converted_${base}"
        final_file="${TEMP_DIR}/final_${base}"
        dump_file="${TEMP_DIR}/dump_${base}.txt"

        echo "Processing (with segdigest): $file"

        # Decompress input file first
        dcmdrle "$file" "$decompressed_file"
        # Now process with segdigest
        if $IGNORE_MISSING; then
            ./bin/segdigest --ignore-missing "$decompressed_file" "$converted_file"
        else
            ./bin/segdigest "$decompressed_file" "$converted_file"
        fi
        dcmconv +e "$converted_file" "$final_file"

        if $COMPARE_DUMP; then
            dcmdump "$final_file" > "$dump_file" >& /dev/null
        else
            dcmdump "$final_file" +W "$OUTPUT_DIR" >& /dev/null
        fi
    fi
done

echo "========== Pass 2: Without segdigest =========="

for file in "${FILES[@]}"; do
    if [ -f "$file" ]; then
        base=$(basename "$file")
        orig_decompressed_file="${TEMP_DIR}/orig_decompressed_${base}"
        orig_final_file="${TEMP_DIR}/orig_final_${base}"
        orig_dump_file="${TEMP_DIR}/orig_dump_${base}.txt"

        echo "Processing (without segdigest): $file"

        dcmdrle "$file" "$orig_decompressed_file"
        dcmconv +e "$orig_decompressed_file" "$orig_final_file"

        if $COMPARE_DUMP; then
            dcmdump "$orig_final_file" > "$orig_dump_file" >& /dev/null
        else
            dcmdump "$orig_final_file" +W "$OUTPUT_DIR" >& /dev/null
        fi
    fi
done

echo "========== Comparison =========="

total_files=0
summary_table=()

for file in "${FILES[@]}"; do
    if [ -f "$file" ]; then
        total_files=$((total_files+1))
        base=$(basename "$file")
        raw_file="$OUTPUT_DIR/final_${base}.0.raw"
        orig_raw_file="$OUTPUT_DIR/orig_final_${base}.0.raw"

        bin_result="not tested"
        text_result="not tested"

        echo "Comparing pixel data: $raw_file <-> $orig_raw_file ..."

        if [ -f "$raw_file" ] && [ -f "$orig_raw_file" ]; then
            if cmp -s "$raw_file" "$orig_raw_file"; then
                echo "→ Pixel data: identical"
                bin_result="identical"
        else
            echo "→ Pixel data: DIFFERENT"
            echo "  Byte differences (first 10):"
            cmp -l "$raw_file" "$orig_raw_file" | head -n 10 | while read offset val1 val2; do
                # Convert offset to decimal
                offset_dec=$offset
                # Convert values to decimal
                val1_dec=$((8#$val1))
                val2_dec=$((8#$val2))
                # Convert values to binary (8 bits)
                val1_bin=$(printf "%08d" "$(echo "obase=2;$val1_dec" | bc)")
                val2_bin=$(printf "%08d" "$(echo "obase=2;$val2_dec" | bc)")
                printf "    %6d | %3d (%s) | %3d (%s)\n" "$offset_dec" "$val1_dec" "$val1_bin" "$val2_dec" "$val2_bin"
            done
            bin_result="different"
        fi
        else
            echo "One or both pixel data files do not exist: $raw_file $orig_raw_file"
            bin_result="not tested"
        fi

        if $COMPARE_DUMP; then
            dump_file="${TEMP_DIR}/dump_${base}.txt"
            orig_dump_file="${TEMP_DIR}/orig_dump_${base}.txt"
            echo "→ Dump comparison:"
            if [ -f "$dump_file" ] && [ -f "$orig_dump_file" ]; then
                if diff -q "$dump_file" "$orig_dump_file" > /dev/null; then
                    echo "  Text dumps: identical"
                    text_result="identical"
                else
                    echo "  Text dumps: DIFFERENT (first few lines):"
                    diff -u "$dump_file" "$orig_dump_file" | head -n 20
                    text_result="different"
                fi
            else
                echo "  One or both dump files do not exist: $dump_file $orig_dump_file"
                text_result="not tested"
            fi
        fi

        summary_table+=("$base|$bin_result|$text_result")
        echo
    fi
done

echo "========== Summary =========="
printf "%-45s | %-15s | %-15s\n" "File" "Binary" "Textual"
printf "%-45s-+-%-15s-+-%-15s\n" "---------------------------------------------" "---------------" "---------------"
for entry in "${summary_table[@]}"; do
    IFS="|" read -r base bin_result text_result <<< "$entry"
    printf "%-45s | %-15s | %-15s\n" "$base" "$bin_result" "$text_result"
done
echo "Total files compared: $total_files"
