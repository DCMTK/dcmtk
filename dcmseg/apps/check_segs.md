
# DICOM Processing Tool – Manual

## Overview

This tool processes a directory of DICOM files in two passes:

1. **With segmentation digest (`segdigest`)**
2. **Without segmentation digest**

Each file is transformed, dumped, and the results of both processing paths are compared.
The transformation includes decompressing from RLE, writing with explicit lengths, i.e.
with the goal to normalize the files for comparison.

However, right now, only the pixel data is compared.

---

## Requirements

- Bash shell
- Executables available in `PATH`:
  - `dcmdrle` – decompresses DICOM files
  - `dcmconv` – converts DICOM transfer syntax
  - `dcmdump` – dumps DICOM metadata
- Executable `segdigest` available at `./bin/segdigest`

---

## Usage

```bash
./process_dicom.sh /path/to/dicom/files
```

---

## Processing Steps

### 1. Initial Setup
- Creates an `output/` directory to store `dcmdump` results.

### 2. Pass 1: With `segdigest`

For each DICOM file:
- `segdigest`: creates a transformed copy (`converted_<filename>`)
- `dcmdrle`: decompresses the result (`decompressed_<filename>`)
- `dcmconv +e`: converts syntax (`final_<filename>`)
- `dcmdump +W output/`: dumps metadata to the `output/` folder

### 3. Pass 2: Without `segdigest`

For the original file:
- `dcmdrle`: decompresses the original file (`orig_decompressed_<filename>`)
- `dcmconv +e`: converts syntax (`orig_final_<filename>`)
- `dcmdump +W output/`: dumps metadata to the same `output/` folder

### 4. Comparison

- Compares each `final_<filename>` (with `segdigest`) to `orig_final_<filename>` (without `segdigest`) using `cmp`
- Prints whether the files are **identical** or **different**

---

## Output Summary

- Intermediate files are created in the working directory:
  - `converted_...`, `decompressed_...`, `final_...`
  - `orig_decompressed_...`, `orig_final_...`
- Metadata dumps are saved to the `output/` directory.
- Comparison results are printed to the terminal.

---

## Example Output

```text
========== Comparison: With vs. Without segdigest ==========
Comparing final_file1.dcm <-> orig_final_file1.dcm ... identical
Comparing final_file2.dcm <-> orig_final_file2.dcm ... different
```