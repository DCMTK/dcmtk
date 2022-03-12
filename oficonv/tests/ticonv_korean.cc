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
 *  Purpose: oficonv unit tests for Korean character sets
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/oficonv/iconv.h"
#include "oficonv_tests.h"
#include "tables_ksx_1001.h"

OFTEST(oficonv_euc_kr_to_utf8)
{
  convert_euc_to_utf8("EUC-KR", 0xa1, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_01_converted_to_utf8, sizeof(ksx_1001_row_01_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xa2, ksx_1001_row_02, sizeof(ksx_1001_row_02), ksx_1001_row_02_converted_to_utf8, sizeof(ksx_1001_row_02_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xa3, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_03_converted_to_utf8, sizeof(ksx_1001_row_03_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xa4, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_04_converted_to_utf8, sizeof(ksx_1001_row_04_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xa5, ksx_1001_row_05, sizeof(ksx_1001_row_05), ksx_1001_row_05_converted_to_utf8, sizeof(ksx_1001_row_05_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xa6, ksx_1001_row_06, sizeof(ksx_1001_row_06), ksx_1001_row_06_converted_to_utf8, sizeof(ksx_1001_row_06_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xa7, ksx_1001_row_07, sizeof(ksx_1001_row_07), ksx_1001_row_07_converted_to_utf8, sizeof(ksx_1001_row_07_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xa8, ksx_1001_row_08, sizeof(ksx_1001_row_08), ksx_1001_row_08_converted_to_utf8, sizeof(ksx_1001_row_08_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xa9, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_09_converted_to_utf8, sizeof(ksx_1001_row_09_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xaa, ksx_1001_row_10, sizeof(ksx_1001_row_10), ksx_1001_row_10_converted_to_utf8, sizeof(ksx_1001_row_10_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xab, ksx_1001_row_11, sizeof(ksx_1001_row_11), ksx_1001_row_11_converted_to_utf8, sizeof(ksx_1001_row_11_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xac, ksx_1001_row_12, sizeof(ksx_1001_row_12), ksx_1001_row_12_converted_to_utf8, sizeof(ksx_1001_row_12_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xb0, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_16_converted_to_utf8, sizeof(ksx_1001_row_16_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xb1, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_17_converted_to_utf8, sizeof(ksx_1001_row_17_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xb2, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_18_converted_to_utf8, sizeof(ksx_1001_row_18_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xb3, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_19_converted_to_utf8, sizeof(ksx_1001_row_19_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xb4, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_20_converted_to_utf8, sizeof(ksx_1001_row_20_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xb5, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_21_converted_to_utf8, sizeof(ksx_1001_row_21_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xb6, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_22_converted_to_utf8, sizeof(ksx_1001_row_22_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xb7, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_23_converted_to_utf8, sizeof(ksx_1001_row_23_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xb8, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_24_converted_to_utf8, sizeof(ksx_1001_row_24_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xb9, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_25_converted_to_utf8, sizeof(ksx_1001_row_25_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xba, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_26_converted_to_utf8, sizeof(ksx_1001_row_26_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xbb, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_27_converted_to_utf8, sizeof(ksx_1001_row_27_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xbc, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_28_converted_to_utf8, sizeof(ksx_1001_row_28_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xbd, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_29_converted_to_utf8, sizeof(ksx_1001_row_29_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xbe, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_30_converted_to_utf8, sizeof(ksx_1001_row_30_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xbf, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_31_converted_to_utf8, sizeof(ksx_1001_row_31_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xc0, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_32_converted_to_utf8, sizeof(ksx_1001_row_32_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xc1, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_33_converted_to_utf8, sizeof(ksx_1001_row_33_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xc2, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_34_converted_to_utf8, sizeof(ksx_1001_row_34_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xc3, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_35_converted_to_utf8, sizeof(ksx_1001_row_35_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xc4, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_36_converted_to_utf8, sizeof(ksx_1001_row_36_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xc5, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_37_converted_to_utf8, sizeof(ksx_1001_row_37_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xc6, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_38_converted_to_utf8, sizeof(ksx_1001_row_38_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xc7, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_39_converted_to_utf8, sizeof(ksx_1001_row_39_converted_to_utf8));
  convert_euc_to_utf8("EUC-KR", 0xc8, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_40_converted_to_utf8, sizeof(ksx_1001_row_40_converted_to_utf8));
}

OFTEST(oficonv_utf8_to_euc_kr)
{
  convert_utf8_to_euc("EUC-KR", 0xa1, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_01_converted_to_utf8, sizeof(ksx_1001_row_01_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xa2, ksx_1001_row_02, sizeof(ksx_1001_row_02), ksx_1001_row_02_converted_to_utf8, sizeof(ksx_1001_row_02_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xa3, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_03_converted_to_utf8, sizeof(ksx_1001_row_03_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xa4, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_04_converted_to_utf8, sizeof(ksx_1001_row_04_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xa5, ksx_1001_row_05, sizeof(ksx_1001_row_05), ksx_1001_row_05_converted_to_utf8, sizeof(ksx_1001_row_05_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xa6, ksx_1001_row_06, sizeof(ksx_1001_row_06), ksx_1001_row_06_converted_to_utf8, sizeof(ksx_1001_row_06_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xa7, ksx_1001_row_07, sizeof(ksx_1001_row_07), ksx_1001_row_07_converted_to_utf8, sizeof(ksx_1001_row_07_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xa8, ksx_1001_row_08, sizeof(ksx_1001_row_08), ksx_1001_row_08_converted_to_utf8, sizeof(ksx_1001_row_08_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xa9, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_09_converted_to_utf8, sizeof(ksx_1001_row_09_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xaa, ksx_1001_row_10, sizeof(ksx_1001_row_10), ksx_1001_row_10_converted_to_utf8, sizeof(ksx_1001_row_10_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xab, ksx_1001_row_11, sizeof(ksx_1001_row_11), ksx_1001_row_11_converted_to_utf8, sizeof(ksx_1001_row_11_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xac, ksx_1001_row_12, sizeof(ksx_1001_row_12), ksx_1001_row_12_converted_to_utf8, sizeof(ksx_1001_row_12_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xb0, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_16_converted_to_utf8, sizeof(ksx_1001_row_16_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xb1, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_17_converted_to_utf8, sizeof(ksx_1001_row_17_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xb2, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_18_converted_to_utf8, sizeof(ksx_1001_row_18_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xb3, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_19_converted_to_utf8, sizeof(ksx_1001_row_19_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xb4, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_20_converted_to_utf8, sizeof(ksx_1001_row_20_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xb5, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_21_converted_to_utf8, sizeof(ksx_1001_row_21_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xb6, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_22_converted_to_utf8, sizeof(ksx_1001_row_22_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xb7, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_23_converted_to_utf8, sizeof(ksx_1001_row_23_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xb8, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_24_converted_to_utf8, sizeof(ksx_1001_row_24_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xb9, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_25_converted_to_utf8, sizeof(ksx_1001_row_25_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xba, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_26_converted_to_utf8, sizeof(ksx_1001_row_26_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xbb, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_27_converted_to_utf8, sizeof(ksx_1001_row_27_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xbc, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_28_converted_to_utf8, sizeof(ksx_1001_row_28_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xbd, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_29_converted_to_utf8, sizeof(ksx_1001_row_29_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xbe, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_30_converted_to_utf8, sizeof(ksx_1001_row_30_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xbf, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_31_converted_to_utf8, sizeof(ksx_1001_row_31_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xc0, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_32_converted_to_utf8, sizeof(ksx_1001_row_32_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xc1, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_33_converted_to_utf8, sizeof(ksx_1001_row_33_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xc2, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_34_converted_to_utf8, sizeof(ksx_1001_row_34_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xc3, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_35_converted_to_utf8, sizeof(ksx_1001_row_35_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xc4, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_36_converted_to_utf8, sizeof(ksx_1001_row_36_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xc5, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_37_converted_to_utf8, sizeof(ksx_1001_row_37_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xc6, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_38_converted_to_utf8, sizeof(ksx_1001_row_38_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xc7, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_39_converted_to_utf8, sizeof(ksx_1001_row_39_converted_to_utf8));
  convert_utf8_to_euc("EUC-KR", 0xc8, ksx_1001_row_01_03_04_09_16ff, sizeof(ksx_1001_row_01_03_04_09_16ff), ksx_1001_row_40_converted_to_utf8, sizeof(ksx_1001_row_40_converted_to_utf8));
}

OFTEST(oficonv_ascii_to_euc_kr)
{
  check_euc_ascii_characters("EUC-KR");
}

