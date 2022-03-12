/*
 *
 *  Copyright (C) 2022, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: oficonv unit tests for Japanese character sets
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofstring.h"
#include "oficonv_tests.h"

#include "tables_jis_x0201.h"
#include "tables_jis_x0208.h"
#include "tables_jis_x0212.h"

// Conversion from Shift-JIS (Katakana) to ISO_IR 192 (Unicode)
// For all pratical purposes, Shift-JIS is identical to ISO-IR-13 (JIS_X0201)
OFTEST(oficonv_shift_jis_to_utf8)
{
  convert_to_utf8("Shift_JIS", jis_x0201_characters, sizeof(jis_x0201_characters), jis_x0201_to_utf8, sizeof(jis_x0201_to_utf8));
}

OFTEST(oficonv_utf8_to_shift_jis)
{
  convert_from_utf8("Shift_JIS", jis_x0201_characters, sizeof(jis_x0201_characters), jis_x0201_to_utf8, sizeof(jis_x0201_to_utf8));
}

OFTEST(oficonv_jis_x0201_to_ascii)
{
  // the transliteration function in oficonv is rather stupid and will simply map all extended
  // characters to '?' when converting to ASCII. We can at least check that this works
  convert_jis_x0201_to_ascii("Shift_JIS", jis_x0201_characters, sizeof(jis_x0201_characters), jis_x0201_to_ascii, sizeof(jis_x0201_to_ascii));
}

OFTEST(oficonv_jis_x0208_to_utf8)
{
  // Non-Kanji rows
  convert_euc_to_utf8("ISO-IR-87", 0x21, jis_x0208_row_01, sizeof(jis_x0208_row_01), jis_x0208_row_01_converted_to_utf8, sizeof(jis_x0208_row_01_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x22, jis_x0208_row_02, sizeof(jis_x0208_row_02), jis_x0208_row_02_converted_to_utf8, sizeof(jis_x0208_row_02_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x23, jis_x0208_row_03, sizeof(jis_x0208_row_03), jis_x0208_row_03_converted_to_utf8, sizeof(jis_x0208_row_03_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x24, jis_x0208_row_04, sizeof(jis_x0208_row_04), jis_x0208_row_04_converted_to_utf8, sizeof(jis_x0208_row_04_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x25, jis_x0208_row_05, sizeof(jis_x0208_row_05), jis_x0208_row_05_converted_to_utf8, sizeof(jis_x0208_row_05_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x26, jis_x0208_row_06, sizeof(jis_x0208_row_06), jis_x0208_row_06_converted_to_utf8, sizeof(jis_x0208_row_06_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x27, jis_x0208_row_07, sizeof(jis_x0208_row_07), jis_x0208_row_07_converted_to_utf8, sizeof(jis_x0208_row_07_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x28, jis_x0208_row_08, sizeof(jis_x0208_row_08), jis_x0208_row_08_converted_to_utf8, sizeof(jis_x0208_row_08_converted_to_utf8));

  // Kanji Level 1
  convert_euc_to_utf8("ISO-IR-87", 0x30, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_16_converted_to_utf8, sizeof(jis_x0208_row_16_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x31, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_17_converted_to_utf8, sizeof(jis_x0208_row_17_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x32, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_18_converted_to_utf8, sizeof(jis_x0208_row_18_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x33, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_19_converted_to_utf8, sizeof(jis_x0208_row_19_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x34, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_20_converted_to_utf8, sizeof(jis_x0208_row_20_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x35, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_21_converted_to_utf8, sizeof(jis_x0208_row_21_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x36, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_22_converted_to_utf8, sizeof(jis_x0208_row_22_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x37, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_23_converted_to_utf8, sizeof(jis_x0208_row_23_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x38, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_24_converted_to_utf8, sizeof(jis_x0208_row_24_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x39, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_25_converted_to_utf8, sizeof(jis_x0208_row_25_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x3a, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_26_converted_to_utf8, sizeof(jis_x0208_row_26_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x3b, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_27_converted_to_utf8, sizeof(jis_x0208_row_27_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x3c, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_28_converted_to_utf8, sizeof(jis_x0208_row_28_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x3d, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_29_converted_to_utf8, sizeof(jis_x0208_row_29_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x3e, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_30_converted_to_utf8, sizeof(jis_x0208_row_30_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x3f, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_31_converted_to_utf8, sizeof(jis_x0208_row_31_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x40, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_32_converted_to_utf8, sizeof(jis_x0208_row_32_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x41, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_33_converted_to_utf8, sizeof(jis_x0208_row_33_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x42, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_34_converted_to_utf8, sizeof(jis_x0208_row_34_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x43, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_35_converted_to_utf8, sizeof(jis_x0208_row_35_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x44, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_36_converted_to_utf8, sizeof(jis_x0208_row_36_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x45, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_37_converted_to_utf8, sizeof(jis_x0208_row_37_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x46, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_38_converted_to_utf8, sizeof(jis_x0208_row_38_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x47, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_39_converted_to_utf8, sizeof(jis_x0208_row_39_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x48, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_40_converted_to_utf8, sizeof(jis_x0208_row_40_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x49, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_41_converted_to_utf8, sizeof(jis_x0208_row_41_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x4a, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_42_converted_to_utf8, sizeof(jis_x0208_row_42_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x4b, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_43_converted_to_utf8, sizeof(jis_x0208_row_43_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x4c, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_44_converted_to_utf8, sizeof(jis_x0208_row_44_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x4d, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_45_converted_to_utf8, sizeof(jis_x0208_row_45_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x4e, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_46_converted_to_utf8, sizeof(jis_x0208_row_46_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x4f, jis_x0208_row_47, sizeof(jis_x0208_row_47),             jis_x0208_row_47_converted_to_utf8, sizeof(jis_x0208_row_47_converted_to_utf8));

  // Kanji Level 2
  convert_euc_to_utf8("ISO-IR-87", 0x50, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_48_converted_to_utf8, sizeof(jis_x0208_row_48_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x51, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_49_converted_to_utf8, sizeof(jis_x0208_row_49_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x52, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_50_converted_to_utf8, sizeof(jis_x0208_row_50_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x53, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_51_converted_to_utf8, sizeof(jis_x0208_row_51_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x54, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_52_converted_to_utf8, sizeof(jis_x0208_row_52_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x55, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_53_converted_to_utf8, sizeof(jis_x0208_row_53_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x56, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_54_converted_to_utf8, sizeof(jis_x0208_row_54_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x57, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_55_converted_to_utf8, sizeof(jis_x0208_row_55_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x58, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_56_converted_to_utf8, sizeof(jis_x0208_row_56_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x59, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_57_converted_to_utf8, sizeof(jis_x0208_row_57_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x5a, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_58_converted_to_utf8, sizeof(jis_x0208_row_58_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x5b, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_59_converted_to_utf8, sizeof(jis_x0208_row_59_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x5c, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_60_converted_to_utf8, sizeof(jis_x0208_row_60_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x5d, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_61_converted_to_utf8, sizeof(jis_x0208_row_61_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x5e, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_62_converted_to_utf8, sizeof(jis_x0208_row_62_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x5f, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_63_converted_to_utf8, sizeof(jis_x0208_row_63_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x60, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_64_converted_to_utf8, sizeof(jis_x0208_row_64_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x61, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_65_converted_to_utf8, sizeof(jis_x0208_row_65_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x62, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_66_converted_to_utf8, sizeof(jis_x0208_row_66_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x63, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_67_converted_to_utf8, sizeof(jis_x0208_row_67_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x64, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_68_converted_to_utf8, sizeof(jis_x0208_row_68_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x65, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_69_converted_to_utf8, sizeof(jis_x0208_row_69_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x66, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_70_converted_to_utf8, sizeof(jis_x0208_row_70_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x67, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_71_converted_to_utf8, sizeof(jis_x0208_row_71_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x68, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_72_converted_to_utf8, sizeof(jis_x0208_row_72_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x69, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_73_converted_to_utf8, sizeof(jis_x0208_row_73_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x6a, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_74_converted_to_utf8, sizeof(jis_x0208_row_74_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x6b, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_75_converted_to_utf8, sizeof(jis_x0208_row_75_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x6c, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_76_converted_to_utf8, sizeof(jis_x0208_row_76_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x6d, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_77_converted_to_utf8, sizeof(jis_x0208_row_77_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x6e, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_78_converted_to_utf8, sizeof(jis_x0208_row_78_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x6f, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_79_converted_to_utf8, sizeof(jis_x0208_row_79_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x70, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_80_converted_to_utf8, sizeof(jis_x0208_row_80_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x71, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_81_converted_to_utf8, sizeof(jis_x0208_row_81_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x72, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_82_converted_to_utf8, sizeof(jis_x0208_row_82_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x73, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_83_converted_to_utf8, sizeof(jis_x0208_row_83_converted_to_utf8));
  convert_euc_to_utf8("ISO-IR-87", 0x74, jis_x0208_row_84, sizeof(jis_x0208_row_84),             jis_x0208_row_84_converted_to_utf8, sizeof(jis_x0208_row_84_converted_to_utf8));
}

OFTEST(oficonv_utf8_to_jis_x0208)
{
  // Non-Kanji rows
  convert_utf8_to_euc("ISO-IR-87", 0x21, jis_x0208_row_01, sizeof(jis_x0208_row_01), jis_x0208_row_01_converted_to_utf8, sizeof(jis_x0208_row_01_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x22, jis_x0208_row_02, sizeof(jis_x0208_row_02), jis_x0208_row_02_converted_to_utf8, sizeof(jis_x0208_row_02_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x23, jis_x0208_row_03, sizeof(jis_x0208_row_03), jis_x0208_row_03_converted_to_utf8, sizeof(jis_x0208_row_03_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x24, jis_x0208_row_04, sizeof(jis_x0208_row_04), jis_x0208_row_04_converted_to_utf8, sizeof(jis_x0208_row_04_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x25, jis_x0208_row_05, sizeof(jis_x0208_row_05), jis_x0208_row_05_converted_to_utf8, sizeof(jis_x0208_row_05_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x26, jis_x0208_row_06, sizeof(jis_x0208_row_06), jis_x0208_row_06_converted_to_utf8, sizeof(jis_x0208_row_06_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x27, jis_x0208_row_07, sizeof(jis_x0208_row_07), jis_x0208_row_07_converted_to_utf8, sizeof(jis_x0208_row_07_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x28, jis_x0208_row_08, sizeof(jis_x0208_row_08), jis_x0208_row_08_converted_to_utf8, sizeof(jis_x0208_row_08_converted_to_utf8));

  // Kanji Level 1
  convert_utf8_to_euc("ISO-IR-87", 0x30, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_16_converted_to_utf8, sizeof(jis_x0208_row_16_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x31, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_17_converted_to_utf8, sizeof(jis_x0208_row_17_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x32, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_18_converted_to_utf8, sizeof(jis_x0208_row_18_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x33, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_19_converted_to_utf8, sizeof(jis_x0208_row_19_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x34, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_20_converted_to_utf8, sizeof(jis_x0208_row_20_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x35, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_21_converted_to_utf8, sizeof(jis_x0208_row_21_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x36, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_22_converted_to_utf8, sizeof(jis_x0208_row_22_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x37, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_23_converted_to_utf8, sizeof(jis_x0208_row_23_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x38, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_24_converted_to_utf8, sizeof(jis_x0208_row_24_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x39, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_25_converted_to_utf8, sizeof(jis_x0208_row_25_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x3a, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_26_converted_to_utf8, sizeof(jis_x0208_row_26_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x3b, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_27_converted_to_utf8, sizeof(jis_x0208_row_27_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x3c, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_28_converted_to_utf8, sizeof(jis_x0208_row_28_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x3d, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_29_converted_to_utf8, sizeof(jis_x0208_row_29_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x3e, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_30_converted_to_utf8, sizeof(jis_x0208_row_30_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x3f, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_31_converted_to_utf8, sizeof(jis_x0208_row_31_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x40, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_32_converted_to_utf8, sizeof(jis_x0208_row_32_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x41, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_33_converted_to_utf8, sizeof(jis_x0208_row_33_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x42, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_34_converted_to_utf8, sizeof(jis_x0208_row_34_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x43, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_35_converted_to_utf8, sizeof(jis_x0208_row_35_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x44, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_36_converted_to_utf8, sizeof(jis_x0208_row_36_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x45, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_37_converted_to_utf8, sizeof(jis_x0208_row_37_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x46, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_38_converted_to_utf8, sizeof(jis_x0208_row_38_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x47, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_39_converted_to_utf8, sizeof(jis_x0208_row_39_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x48, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_40_converted_to_utf8, sizeof(jis_x0208_row_40_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x49, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_41_converted_to_utf8, sizeof(jis_x0208_row_41_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x4a, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_42_converted_to_utf8, sizeof(jis_x0208_row_42_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x4b, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_43_converted_to_utf8, sizeof(jis_x0208_row_43_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x4c, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_44_converted_to_utf8, sizeof(jis_x0208_row_44_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x4d, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_45_converted_to_utf8, sizeof(jis_x0208_row_45_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x4e, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_46_converted_to_utf8, sizeof(jis_x0208_row_46_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x4f, jis_x0208_row_47, sizeof(jis_x0208_row_47),             jis_x0208_row_47_converted_to_utf8, sizeof(jis_x0208_row_47_converted_to_utf8));

  // Kanji Level 2
  convert_utf8_to_euc("ISO-IR-87", 0x50, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_48_converted_to_utf8, sizeof(jis_x0208_row_48_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x51, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_49_converted_to_utf8, sizeof(jis_x0208_row_49_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x52, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_50_converted_to_utf8, sizeof(jis_x0208_row_50_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x53, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_51_converted_to_utf8, sizeof(jis_x0208_row_51_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x54, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_52_converted_to_utf8, sizeof(jis_x0208_row_52_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x55, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_53_converted_to_utf8, sizeof(jis_x0208_row_53_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x56, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_54_converted_to_utf8, sizeof(jis_x0208_row_54_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x57, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_55_converted_to_utf8, sizeof(jis_x0208_row_55_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x58, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_56_converted_to_utf8, sizeof(jis_x0208_row_56_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x59, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_57_converted_to_utf8, sizeof(jis_x0208_row_57_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x5a, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_58_converted_to_utf8, sizeof(jis_x0208_row_58_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x5b, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_59_converted_to_utf8, sizeof(jis_x0208_row_59_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x5c, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_60_converted_to_utf8, sizeof(jis_x0208_row_60_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x5d, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_61_converted_to_utf8, sizeof(jis_x0208_row_61_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x5e, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_62_converted_to_utf8, sizeof(jis_x0208_row_62_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x5f, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_63_converted_to_utf8, sizeof(jis_x0208_row_63_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x60, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_64_converted_to_utf8, sizeof(jis_x0208_row_64_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x61, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_65_converted_to_utf8, sizeof(jis_x0208_row_65_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x62, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_66_converted_to_utf8, sizeof(jis_x0208_row_66_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x63, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_67_converted_to_utf8, sizeof(jis_x0208_row_67_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x64, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_68_converted_to_utf8, sizeof(jis_x0208_row_68_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x65, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_69_converted_to_utf8, sizeof(jis_x0208_row_69_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x66, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_70_converted_to_utf8, sizeof(jis_x0208_row_70_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x67, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_71_converted_to_utf8, sizeof(jis_x0208_row_71_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x68, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_72_converted_to_utf8, sizeof(jis_x0208_row_72_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x69, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_73_converted_to_utf8, sizeof(jis_x0208_row_73_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x6a, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_74_converted_to_utf8, sizeof(jis_x0208_row_74_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x6b, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_75_converted_to_utf8, sizeof(jis_x0208_row_75_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x6c, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_76_converted_to_utf8, sizeof(jis_x0208_row_76_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x6d, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_77_converted_to_utf8, sizeof(jis_x0208_row_77_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x6e, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_78_converted_to_utf8, sizeof(jis_x0208_row_78_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x6f, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_79_converted_to_utf8, sizeof(jis_x0208_row_79_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x70, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_80_converted_to_utf8, sizeof(jis_x0208_row_80_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x71, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_81_converted_to_utf8, sizeof(jis_x0208_row_81_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x72, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_82_converted_to_utf8, sizeof(jis_x0208_row_82_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x73, jis_x0208_row_16_to_83, sizeof(jis_x0208_row_16_to_83), jis_x0208_row_83_converted_to_utf8, sizeof(jis_x0208_row_83_converted_to_utf8));
  convert_utf8_to_euc("ISO-IR-87", 0x74, jis_x0208_row_84, sizeof(jis_x0208_row_84),             jis_x0208_row_84_converted_to_utf8, sizeof(jis_x0208_row_84_converted_to_utf8));
}


OFTEST(oficonv_jis_x0212_to_utf8)
{
  // Non-Kanji rows
  convert_euc_to_utf8("JIS_X0212", 0x22, jis_x0212_row_02, sizeof(jis_x0212_row_02), jis_x0212_row_02_converted_to_utf8, sizeof(jis_x0212_row_02_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x26, jis_x0212_row_06, sizeof(jis_x0212_row_06), jis_x0212_row_06_converted_to_utf8, sizeof(jis_x0212_row_06_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x27, jis_x0212_row_07, sizeof(jis_x0212_row_07), jis_x0212_row_07_converted_to_utf8, sizeof(jis_x0212_row_07_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x29, jis_x0212_row_09, sizeof(jis_x0212_row_09), jis_x0212_row_09_converted_to_utf8, sizeof(jis_x0212_row_09_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x2a, jis_x0212_row_10, sizeof(jis_x0212_row_10), jis_x0212_row_10_converted_to_utf8, sizeof(jis_x0212_row_10_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x2b, jis_x0212_row_11, sizeof(jis_x0212_row_11), jis_x0212_row_11_converted_to_utf8, sizeof(jis_x0212_row_11_converted_to_utf8));

  // Kanji Level 1
  convert_euc_to_utf8("JIS_X0212", 0x30, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_16_converted_to_utf8, sizeof(jis_x0212_row_16_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x31, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_17_converted_to_utf8, sizeof(jis_x0212_row_17_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x32, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_18_converted_to_utf8, sizeof(jis_x0212_row_18_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x33, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_19_converted_to_utf8, sizeof(jis_x0212_row_19_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x34, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_20_converted_to_utf8, sizeof(jis_x0212_row_20_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x35, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_21_converted_to_utf8, sizeof(jis_x0212_row_21_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x36, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_22_converted_to_utf8, sizeof(jis_x0212_row_22_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x37, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_23_converted_to_utf8, sizeof(jis_x0212_row_23_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x38, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_24_converted_to_utf8, sizeof(jis_x0212_row_24_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x39, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_25_converted_to_utf8, sizeof(jis_x0212_row_25_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x3a, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_26_converted_to_utf8, sizeof(jis_x0212_row_26_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x3b, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_27_converted_to_utf8, sizeof(jis_x0212_row_27_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x3c, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_28_converted_to_utf8, sizeof(jis_x0212_row_28_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x3d, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_29_converted_to_utf8, sizeof(jis_x0212_row_29_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x3e, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_30_converted_to_utf8, sizeof(jis_x0212_row_30_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x3f, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_31_converted_to_utf8, sizeof(jis_x0212_row_31_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x40, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_32_converted_to_utf8, sizeof(jis_x0212_row_32_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x41, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_33_converted_to_utf8, sizeof(jis_x0212_row_33_converted_to_utf8));

  convert_euc_to_utf8("JIS_X0212", 0x42, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_34_converted_to_utf8, sizeof(jis_x0212_row_34_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x43, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_35_converted_to_utf8, sizeof(jis_x0212_row_35_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x44, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_36_converted_to_utf8, sizeof(jis_x0212_row_36_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x45, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_37_converted_to_utf8, sizeof(jis_x0212_row_37_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x46, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_38_converted_to_utf8, sizeof(jis_x0212_row_38_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x47, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_39_converted_to_utf8, sizeof(jis_x0212_row_39_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x48, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_40_converted_to_utf8, sizeof(jis_x0212_row_40_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x49, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_41_converted_to_utf8, sizeof(jis_x0212_row_41_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x4a, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_42_converted_to_utf8, sizeof(jis_x0212_row_42_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x4b, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_43_converted_to_utf8, sizeof(jis_x0212_row_43_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x4c, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_44_converted_to_utf8, sizeof(jis_x0212_row_44_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x4d, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_45_converted_to_utf8, sizeof(jis_x0212_row_45_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x4e, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_46_converted_to_utf8, sizeof(jis_x0212_row_46_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x4f, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_47_converted_to_utf8, sizeof(jis_x0212_row_47_converted_to_utf8));

  // Kanji Level 2
  convert_euc_to_utf8("JIS_X0212", 0x50, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_48_converted_to_utf8, sizeof(jis_x0212_row_48_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x51, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_49_converted_to_utf8, sizeof(jis_x0212_row_49_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x52, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_50_converted_to_utf8, sizeof(jis_x0212_row_50_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x53, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_51_converted_to_utf8, sizeof(jis_x0212_row_51_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x54, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_52_converted_to_utf8, sizeof(jis_x0212_row_52_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x55, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_53_converted_to_utf8, sizeof(jis_x0212_row_53_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x56, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_54_converted_to_utf8, sizeof(jis_x0212_row_54_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x57, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_55_converted_to_utf8, sizeof(jis_x0212_row_55_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x58, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_56_converted_to_utf8, sizeof(jis_x0212_row_56_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x59, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_57_converted_to_utf8, sizeof(jis_x0212_row_57_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x5a, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_58_converted_to_utf8, sizeof(jis_x0212_row_58_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x5b, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_59_converted_to_utf8, sizeof(jis_x0212_row_59_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x5c, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_60_converted_to_utf8, sizeof(jis_x0212_row_60_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x5d, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_61_converted_to_utf8, sizeof(jis_x0212_row_61_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x5e, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_62_converted_to_utf8, sizeof(jis_x0212_row_62_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x5f, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_63_converted_to_utf8, sizeof(jis_x0212_row_63_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x60, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_64_converted_to_utf8, sizeof(jis_x0212_row_64_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x61, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_65_converted_to_utf8, sizeof(jis_x0212_row_65_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x62, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_66_converted_to_utf8, sizeof(jis_x0212_row_66_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x63, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_67_converted_to_utf8, sizeof(jis_x0212_row_67_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x64, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_68_converted_to_utf8, sizeof(jis_x0212_row_68_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x65, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_69_converted_to_utf8, sizeof(jis_x0212_row_69_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x66, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_70_converted_to_utf8, sizeof(jis_x0212_row_70_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x67, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_71_converted_to_utf8, sizeof(jis_x0212_row_71_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x68, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_72_converted_to_utf8, sizeof(jis_x0212_row_72_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x69, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_73_converted_to_utf8, sizeof(jis_x0212_row_73_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x6a, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_74_converted_to_utf8, sizeof(jis_x0212_row_74_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x6b, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_75_converted_to_utf8, sizeof(jis_x0212_row_75_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x6c, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_76_converted_to_utf8, sizeof(jis_x0212_row_76_converted_to_utf8));
  convert_euc_to_utf8("JIS_X0212", 0x6d, jis_x0212_row_77, sizeof(jis_x0212_row_77),             jis_x0212_row_77_converted_to_utf8, sizeof(jis_x0212_row_77_converted_to_utf8));
}


OFTEST(oficonv_utf8_to_jis_x0212)
{
  // Non-Kanji rows
  convert_utf8_to_euc("JIS_X0212", 0x22, jis_x0212_row_02, sizeof(jis_x0212_row_02), jis_x0212_row_02_converted_to_utf8, sizeof(jis_x0212_row_02_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x26, jis_x0212_row_06, sizeof(jis_x0212_row_06), jis_x0212_row_06_converted_to_utf8, sizeof(jis_x0212_row_06_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x27, jis_x0212_row_07, sizeof(jis_x0212_row_07), jis_x0212_row_07_converted_to_utf8, sizeof(jis_x0212_row_07_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x29, jis_x0212_row_09, sizeof(jis_x0212_row_09), jis_x0212_row_09_converted_to_utf8, sizeof(jis_x0212_row_09_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x2a, jis_x0212_row_10, sizeof(jis_x0212_row_10), jis_x0212_row_10_converted_to_utf8, sizeof(jis_x0212_row_10_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x2b, jis_x0212_row_11, sizeof(jis_x0212_row_11), jis_x0212_row_11_converted_to_utf8, sizeof(jis_x0212_row_11_converted_to_utf8));

  // Kanji Level 1
  convert_utf8_to_euc("JIS_X0212", 0x30, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_16_converted_to_utf8, sizeof(jis_x0212_row_16_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x31, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_17_converted_to_utf8, sizeof(jis_x0212_row_17_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x32, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_18_converted_to_utf8, sizeof(jis_x0212_row_18_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x33, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_19_converted_to_utf8, sizeof(jis_x0212_row_19_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x34, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_20_converted_to_utf8, sizeof(jis_x0212_row_20_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x35, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_21_converted_to_utf8, sizeof(jis_x0212_row_21_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x36, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_22_converted_to_utf8, sizeof(jis_x0212_row_22_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x37, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_23_converted_to_utf8, sizeof(jis_x0212_row_23_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x38, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_24_converted_to_utf8, sizeof(jis_x0212_row_24_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x39, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_25_converted_to_utf8, sizeof(jis_x0212_row_25_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x3a, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_26_converted_to_utf8, sizeof(jis_x0212_row_26_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x3b, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_27_converted_to_utf8, sizeof(jis_x0212_row_27_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x3c, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_28_converted_to_utf8, sizeof(jis_x0212_row_28_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x3d, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_29_converted_to_utf8, sizeof(jis_x0212_row_29_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x3e, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_30_converted_to_utf8, sizeof(jis_x0212_row_30_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x3f, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_31_converted_to_utf8, sizeof(jis_x0212_row_31_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x40, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_32_converted_to_utf8, sizeof(jis_x0212_row_32_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x41, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_33_converted_to_utf8, sizeof(jis_x0212_row_33_converted_to_utf8));

  convert_utf8_to_euc("JIS_X0212", 0x42, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_34_converted_to_utf8, sizeof(jis_x0212_row_34_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x43, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_35_converted_to_utf8, sizeof(jis_x0212_row_35_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x44, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_36_converted_to_utf8, sizeof(jis_x0212_row_36_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x45, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_37_converted_to_utf8, sizeof(jis_x0212_row_37_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x46, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_38_converted_to_utf8, sizeof(jis_x0212_row_38_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x47, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_39_converted_to_utf8, sizeof(jis_x0212_row_39_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x48, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_40_converted_to_utf8, sizeof(jis_x0212_row_40_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x49, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_41_converted_to_utf8, sizeof(jis_x0212_row_41_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x4a, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_42_converted_to_utf8, sizeof(jis_x0212_row_42_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x4b, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_43_converted_to_utf8, sizeof(jis_x0212_row_43_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x4c, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_44_converted_to_utf8, sizeof(jis_x0212_row_44_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x4d, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_45_converted_to_utf8, sizeof(jis_x0212_row_45_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x4e, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_46_converted_to_utf8, sizeof(jis_x0212_row_46_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x4f, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_47_converted_to_utf8, sizeof(jis_x0212_row_47_converted_to_utf8));

  // Kanji Level 2
  convert_utf8_to_euc("JIS_X0212", 0x50, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_48_converted_to_utf8, sizeof(jis_x0212_row_48_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x51, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_49_converted_to_utf8, sizeof(jis_x0212_row_49_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x52, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_50_converted_to_utf8, sizeof(jis_x0212_row_50_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x53, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_51_converted_to_utf8, sizeof(jis_x0212_row_51_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x54, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_52_converted_to_utf8, sizeof(jis_x0212_row_52_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x55, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_53_converted_to_utf8, sizeof(jis_x0212_row_53_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x56, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_54_converted_to_utf8, sizeof(jis_x0212_row_54_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x57, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_55_converted_to_utf8, sizeof(jis_x0212_row_55_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x58, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_56_converted_to_utf8, sizeof(jis_x0212_row_56_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x59, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_57_converted_to_utf8, sizeof(jis_x0212_row_57_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x5a, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_58_converted_to_utf8, sizeof(jis_x0212_row_58_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x5b, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_59_converted_to_utf8, sizeof(jis_x0212_row_59_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x5c, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_60_converted_to_utf8, sizeof(jis_x0212_row_60_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x5d, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_61_converted_to_utf8, sizeof(jis_x0212_row_61_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x5e, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_62_converted_to_utf8, sizeof(jis_x0212_row_62_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x5f, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_63_converted_to_utf8, sizeof(jis_x0212_row_63_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x60, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_64_converted_to_utf8, sizeof(jis_x0212_row_64_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x61, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_65_converted_to_utf8, sizeof(jis_x0212_row_65_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x62, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_66_converted_to_utf8, sizeof(jis_x0212_row_66_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x63, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_67_converted_to_utf8, sizeof(jis_x0212_row_67_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x64, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_68_converted_to_utf8, sizeof(jis_x0212_row_68_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x65, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_69_converted_to_utf8, sizeof(jis_x0212_row_69_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x66, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_70_converted_to_utf8, sizeof(jis_x0212_row_70_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x67, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_71_converted_to_utf8, sizeof(jis_x0212_row_71_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x68, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_72_converted_to_utf8, sizeof(jis_x0212_row_72_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x69, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_73_converted_to_utf8, sizeof(jis_x0212_row_73_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x6a, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_74_converted_to_utf8, sizeof(jis_x0212_row_74_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x6b, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_75_converted_to_utf8, sizeof(jis_x0212_row_75_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x6c, jis_x0212_row_16_to_76, sizeof(jis_x0212_row_16_to_76), jis_x0212_row_76_converted_to_utf8, sizeof(jis_x0212_row_76_converted_to_utf8));
  convert_utf8_to_euc("JIS_X0212", 0x6d, jis_x0212_row_77, sizeof(jis_x0212_row_77),             jis_x0212_row_77_converted_to_utf8, sizeof(jis_x0212_row_77_converted_to_utf8));
}
