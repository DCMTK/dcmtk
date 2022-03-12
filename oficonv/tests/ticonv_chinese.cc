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
 *  Purpose: oficonv unit tests for Chinese character sets
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofstring.h"
#include "oficonv_tests.h"

#include "tables_gb18030.h"
#include "tables_gb2312.h"
#include "tables_gbk.h"

OFTEST(oficonv_gb2312_to_utf8)
{
  convert_euc_to_utf8("GB2312", 0xa1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_01_converted_to_utf8, sizeof(gb2312_row_01_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xa2, gb2312_row_02, sizeof(gb2312_row_02),     gb2312_row_02_converted_to_utf8, sizeof(gb2312_row_02_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xa3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_03_converted_to_utf8, sizeof(gb2312_row_03_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xa4, gb2312_row_04, sizeof(gb2312_row_04),     gb2312_row_04_converted_to_utf8, sizeof(gb2312_row_04_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xa5, gb2312_row_05, sizeof(gb2312_row_05),     gb2312_row_05_converted_to_utf8, sizeof(gb2312_row_05_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xa6, gb2312_row_06, sizeof(gb2312_row_06),     gb2312_row_06_converted_to_utf8, sizeof(gb2312_row_06_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xa7, gb2312_row_07, sizeof(gb2312_row_07),     gb2312_row_07_converted_to_utf8, sizeof(gb2312_row_07_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xa8, gb2312_row_08, sizeof(gb2312_row_08),     gb2312_row_08_converted_to_utf8, sizeof(gb2312_row_08_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xa9, gb2312_row_09, sizeof(gb2312_row_09),     gb2312_row_09_converted_to_utf8, sizeof(gb2312_row_09_converted_to_utf8));

  // level 1 chinese characters
  convert_euc_to_utf8("GB2312", 0xb0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_16_converted_to_utf8, sizeof(gb2312_row_16_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xb1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_17_converted_to_utf8, sizeof(gb2312_row_17_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xb2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_18_converted_to_utf8, sizeof(gb2312_row_18_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xb3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_19_converted_to_utf8, sizeof(gb2312_row_19_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xb4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_20_converted_to_utf8, sizeof(gb2312_row_20_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xb5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_21_converted_to_utf8, sizeof(gb2312_row_21_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xb6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_22_converted_to_utf8, sizeof(gb2312_row_22_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xb7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_23_converted_to_utf8, sizeof(gb2312_row_23_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xb8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_24_converted_to_utf8, sizeof(gb2312_row_24_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xb9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_25_converted_to_utf8, sizeof(gb2312_row_25_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xba, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_26_converted_to_utf8, sizeof(gb2312_row_26_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xbb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_27_converted_to_utf8, sizeof(gb2312_row_27_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xbc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_28_converted_to_utf8, sizeof(gb2312_row_28_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xbd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_29_converted_to_utf8, sizeof(gb2312_row_29_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xbe, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_30_converted_to_utf8, sizeof(gb2312_row_30_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xbf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_31_converted_to_utf8, sizeof(gb2312_row_31_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xc0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_32_converted_to_utf8, sizeof(gb2312_row_32_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xc1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_33_converted_to_utf8, sizeof(gb2312_row_33_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xc2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_34_converted_to_utf8, sizeof(gb2312_row_34_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xc3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_35_converted_to_utf8, sizeof(gb2312_row_35_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xc4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_36_converted_to_utf8, sizeof(gb2312_row_36_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xc5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_37_converted_to_utf8, sizeof(gb2312_row_37_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xc6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_38_converted_to_utf8, sizeof(gb2312_row_38_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xc7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_39_converted_to_utf8, sizeof(gb2312_row_39_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xc8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_40_converted_to_utf8, sizeof(gb2312_row_40_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xc9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_41_converted_to_utf8, sizeof(gb2312_row_41_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xca, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_42_converted_to_utf8, sizeof(gb2312_row_42_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xcb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_43_converted_to_utf8, sizeof(gb2312_row_43_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xcc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_44_converted_to_utf8, sizeof(gb2312_row_44_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xcd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_45_converted_to_utf8, sizeof(gb2312_row_45_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xce, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_46_converted_to_utf8, sizeof(gb2312_row_46_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xcf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_47_converted_to_utf8, sizeof(gb2312_row_47_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xd0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_48_converted_to_utf8, sizeof(gb2312_row_48_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xd1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_49_converted_to_utf8, sizeof(gb2312_row_49_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xd2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_50_converted_to_utf8, sizeof(gb2312_row_50_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xd3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_51_converted_to_utf8, sizeof(gb2312_row_51_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xd4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_52_converted_to_utf8, sizeof(gb2312_row_52_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xd5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_53_converted_to_utf8, sizeof(gb2312_row_53_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xd6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_54_converted_to_utf8, sizeof(gb2312_row_54_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xd7, gb2312_row_55, sizeof(gb2312_row_55), gb2312_row_55_converted_to_utf8, sizeof(gb2312_row_55_converted_to_utf8));

  // level 2 chinese characters
  convert_euc_to_utf8("GB2312", 0xd8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_56_converted_to_utf8, sizeof(gb2312_row_56_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xd9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_57_converted_to_utf8, sizeof(gb2312_row_57_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xda, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_58_converted_to_utf8, sizeof(gb2312_row_58_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xdb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_59_converted_to_utf8, sizeof(gb2312_row_59_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xdc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_60_converted_to_utf8, sizeof(gb2312_row_60_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xdd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_61_converted_to_utf8, sizeof(gb2312_row_61_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xde, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_62_converted_to_utf8, sizeof(gb2312_row_62_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xdf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_63_converted_to_utf8, sizeof(gb2312_row_63_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xe0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_64_converted_to_utf8, sizeof(gb2312_row_64_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xe1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_65_converted_to_utf8, sizeof(gb2312_row_65_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xe2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_66_converted_to_utf8, sizeof(gb2312_row_66_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xe3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_67_converted_to_utf8, sizeof(gb2312_row_67_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xe4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_68_converted_to_utf8, sizeof(gb2312_row_68_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xe5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_69_converted_to_utf8, sizeof(gb2312_row_69_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xe6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_70_converted_to_utf8, sizeof(gb2312_row_70_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xe7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_71_converted_to_utf8, sizeof(gb2312_row_71_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xe8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_72_converted_to_utf8, sizeof(gb2312_row_72_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xe9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_73_converted_to_utf8, sizeof(gb2312_row_73_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xea, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_74_converted_to_utf8, sizeof(gb2312_row_74_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xeb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_75_converted_to_utf8, sizeof(gb2312_row_75_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xec, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_76_converted_to_utf8, sizeof(gb2312_row_76_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xed, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_77_converted_to_utf8, sizeof(gb2312_row_77_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xee, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_78_converted_to_utf8, sizeof(gb2312_row_78_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xef, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_79_converted_to_utf8, sizeof(gb2312_row_79_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xf0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_80_converted_to_utf8, sizeof(gb2312_row_80_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xf1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_81_converted_to_utf8, sizeof(gb2312_row_81_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xf2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_82_converted_to_utf8, sizeof(gb2312_row_82_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xf3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_83_converted_to_utf8, sizeof(gb2312_row_83_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xf4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_84_converted_to_utf8, sizeof(gb2312_row_84_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xf5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_85_converted_to_utf8, sizeof(gb2312_row_85_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xf6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_86_converted_to_utf8, sizeof(gb2312_row_86_converted_to_utf8));
  convert_euc_to_utf8("GB2312", 0xf7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_87_converted_to_utf8, sizeof(gb2312_row_87_converted_to_utf8));
}

OFTEST(oficonv_utf8_to_gb2312)
{
  convert_utf8_to_euc("GB2312", 0xa1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_01_converted_to_utf8, sizeof(gb2312_row_01_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xa2, gb2312_row_02, sizeof(gb2312_row_02),     gb2312_row_02_converted_to_utf8, sizeof(gb2312_row_02_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xa3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_03_converted_to_utf8, sizeof(gb2312_row_03_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xa4, gb2312_row_04, sizeof(gb2312_row_04),     gb2312_row_04_converted_to_utf8, sizeof(gb2312_row_04_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xa5, gb2312_row_05, sizeof(gb2312_row_05),     gb2312_row_05_converted_to_utf8, sizeof(gb2312_row_05_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xa6, gb2312_row_06, sizeof(gb2312_row_06),     gb2312_row_06_converted_to_utf8, sizeof(gb2312_row_06_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xa7, gb2312_row_07, sizeof(gb2312_row_07),     gb2312_row_07_converted_to_utf8, sizeof(gb2312_row_07_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xa8, gb2312_row_08, sizeof(gb2312_row_08),     gb2312_row_08_converted_to_utf8, sizeof(gb2312_row_08_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xa9, gb2312_row_09, sizeof(gb2312_row_09),     gb2312_row_09_converted_to_utf8, sizeof(gb2312_row_09_converted_to_utf8));

  // level 1 chinese characters
  convert_utf8_to_euc("GB2312", 0xb0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_16_converted_to_utf8, sizeof(gb2312_row_16_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xb1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_17_converted_to_utf8, sizeof(gb2312_row_17_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xb2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_18_converted_to_utf8, sizeof(gb2312_row_18_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xb3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_19_converted_to_utf8, sizeof(gb2312_row_19_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xb4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_20_converted_to_utf8, sizeof(gb2312_row_20_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xb5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_21_converted_to_utf8, sizeof(gb2312_row_21_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xb6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_22_converted_to_utf8, sizeof(gb2312_row_22_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xb7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_23_converted_to_utf8, sizeof(gb2312_row_23_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xb8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_24_converted_to_utf8, sizeof(gb2312_row_24_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xb9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_25_converted_to_utf8, sizeof(gb2312_row_25_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xba, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_26_converted_to_utf8, sizeof(gb2312_row_26_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xbb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_27_converted_to_utf8, sizeof(gb2312_row_27_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xbc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_28_converted_to_utf8, sizeof(gb2312_row_28_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xbd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_29_converted_to_utf8, sizeof(gb2312_row_29_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xbe, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_30_converted_to_utf8, sizeof(gb2312_row_30_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xbf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_31_converted_to_utf8, sizeof(gb2312_row_31_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xc0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_32_converted_to_utf8, sizeof(gb2312_row_32_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xc1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_33_converted_to_utf8, sizeof(gb2312_row_33_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xc2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_34_converted_to_utf8, sizeof(gb2312_row_34_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xc3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_35_converted_to_utf8, sizeof(gb2312_row_35_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xc4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_36_converted_to_utf8, sizeof(gb2312_row_36_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xc5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_37_converted_to_utf8, sizeof(gb2312_row_37_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xc6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_38_converted_to_utf8, sizeof(gb2312_row_38_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xc7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_39_converted_to_utf8, sizeof(gb2312_row_39_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xc8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_40_converted_to_utf8, sizeof(gb2312_row_40_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xc9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_41_converted_to_utf8, sizeof(gb2312_row_41_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xca, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_42_converted_to_utf8, sizeof(gb2312_row_42_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xcb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_43_converted_to_utf8, sizeof(gb2312_row_43_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xcc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_44_converted_to_utf8, sizeof(gb2312_row_44_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xcd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_45_converted_to_utf8, sizeof(gb2312_row_45_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xce, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_46_converted_to_utf8, sizeof(gb2312_row_46_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xcf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_47_converted_to_utf8, sizeof(gb2312_row_47_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xd0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_48_converted_to_utf8, sizeof(gb2312_row_48_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xd1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_49_converted_to_utf8, sizeof(gb2312_row_49_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xd2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_50_converted_to_utf8, sizeof(gb2312_row_50_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xd3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_51_converted_to_utf8, sizeof(gb2312_row_51_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xd4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_52_converted_to_utf8, sizeof(gb2312_row_52_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xd5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_53_converted_to_utf8, sizeof(gb2312_row_53_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xd6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_54_converted_to_utf8, sizeof(gb2312_row_54_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xd7, gb2312_row_55, sizeof(gb2312_row_55), gb2312_row_55_converted_to_utf8, sizeof(gb2312_row_55_converted_to_utf8));

  // level 2 chinese characters
  convert_utf8_to_euc("GB2312", 0xd8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_56_converted_to_utf8, sizeof(gb2312_row_56_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xd9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_57_converted_to_utf8, sizeof(gb2312_row_57_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xda, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_58_converted_to_utf8, sizeof(gb2312_row_58_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xdb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_59_converted_to_utf8, sizeof(gb2312_row_59_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xdc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_60_converted_to_utf8, sizeof(gb2312_row_60_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xdd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_61_converted_to_utf8, sizeof(gb2312_row_61_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xde, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_62_converted_to_utf8, sizeof(gb2312_row_62_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xdf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_63_converted_to_utf8, sizeof(gb2312_row_63_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xe0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_64_converted_to_utf8, sizeof(gb2312_row_64_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xe1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_65_converted_to_utf8, sizeof(gb2312_row_65_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xe2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_66_converted_to_utf8, sizeof(gb2312_row_66_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xe3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_67_converted_to_utf8, sizeof(gb2312_row_67_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xe4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_68_converted_to_utf8, sizeof(gb2312_row_68_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xe5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_69_converted_to_utf8, sizeof(gb2312_row_69_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xe6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_70_converted_to_utf8, sizeof(gb2312_row_70_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xe7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_71_converted_to_utf8, sizeof(gb2312_row_71_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xe8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_72_converted_to_utf8, sizeof(gb2312_row_72_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xe9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_73_converted_to_utf8, sizeof(gb2312_row_73_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xea, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_74_converted_to_utf8, sizeof(gb2312_row_74_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xeb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_75_converted_to_utf8, sizeof(gb2312_row_75_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xec, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_76_converted_to_utf8, sizeof(gb2312_row_76_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xed, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_77_converted_to_utf8, sizeof(gb2312_row_77_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xee, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_78_converted_to_utf8, sizeof(gb2312_row_78_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xef, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_79_converted_to_utf8, sizeof(gb2312_row_79_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xf0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_80_converted_to_utf8, sizeof(gb2312_row_80_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xf1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_81_converted_to_utf8, sizeof(gb2312_row_81_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xf2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_82_converted_to_utf8, sizeof(gb2312_row_82_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xf3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_83_converted_to_utf8, sizeof(gb2312_row_83_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xf4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_84_converted_to_utf8, sizeof(gb2312_row_84_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xf5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_85_converted_to_utf8, sizeof(gb2312_row_85_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xf6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_86_converted_to_utf8, sizeof(gb2312_row_86_converted_to_utf8));
  convert_utf8_to_euc("GB2312", 0xf7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_87_converted_to_utf8, sizeof(gb2312_row_87_converted_to_utf8));
}

OFTEST(oficonv_gbk_2312_subset_to_utf8)
{
  convert_euc_to_utf8("GBK", 0xa1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_01_converted_to_utf8_alternate, sizeof(gb2312_row_01_converted_to_utf8_alternate));
  convert_euc_to_utf8("GBK", 0xa2, gb2312_row_02, sizeof(gb2312_row_02),     gb2312_row_02_converted_to_utf8, sizeof(gb2312_row_02_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xa3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_03_converted_to_utf8, sizeof(gb2312_row_03_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xa4, gb2312_row_04, sizeof(gb2312_row_04),     gb2312_row_04_converted_to_utf8, sizeof(gb2312_row_04_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xa5, gb2312_row_05, sizeof(gb2312_row_05),     gb2312_row_05_converted_to_utf8, sizeof(gb2312_row_05_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xa6, gb2312_row_06, sizeof(gb2312_row_06),     gb2312_row_06_converted_to_utf8, sizeof(gb2312_row_06_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xa7, gb2312_row_07, sizeof(gb2312_row_07),     gb2312_row_07_converted_to_utf8, sizeof(gb2312_row_07_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xa8, gb2312_row_08, sizeof(gb2312_row_08),     gb2312_row_08_converted_to_utf8, sizeof(gb2312_row_08_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xa9, gb2312_row_09, sizeof(gb2312_row_09),     gb2312_row_09_converted_to_utf8, sizeof(gb2312_row_09_converted_to_utf8));

  // level 1 chinese characters
  convert_euc_to_utf8("GBK", 0xb0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_16_converted_to_utf8, sizeof(gb2312_row_16_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_17_converted_to_utf8, sizeof(gb2312_row_17_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_18_converted_to_utf8, sizeof(gb2312_row_18_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_19_converted_to_utf8, sizeof(gb2312_row_19_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_20_converted_to_utf8, sizeof(gb2312_row_20_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_21_converted_to_utf8, sizeof(gb2312_row_21_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_22_converted_to_utf8, sizeof(gb2312_row_22_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_23_converted_to_utf8, sizeof(gb2312_row_23_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_24_converted_to_utf8, sizeof(gb2312_row_24_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_25_converted_to_utf8, sizeof(gb2312_row_25_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xba, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_26_converted_to_utf8, sizeof(gb2312_row_26_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xbb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_27_converted_to_utf8, sizeof(gb2312_row_27_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xbc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_28_converted_to_utf8, sizeof(gb2312_row_28_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xbd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_29_converted_to_utf8, sizeof(gb2312_row_29_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xbe, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_30_converted_to_utf8, sizeof(gb2312_row_30_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xbf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_31_converted_to_utf8, sizeof(gb2312_row_31_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_32_converted_to_utf8, sizeof(gb2312_row_32_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_33_converted_to_utf8, sizeof(gb2312_row_33_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_34_converted_to_utf8, sizeof(gb2312_row_34_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_35_converted_to_utf8, sizeof(gb2312_row_35_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_36_converted_to_utf8, sizeof(gb2312_row_36_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_37_converted_to_utf8, sizeof(gb2312_row_37_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_38_converted_to_utf8, sizeof(gb2312_row_38_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_39_converted_to_utf8, sizeof(gb2312_row_39_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_40_converted_to_utf8, sizeof(gb2312_row_40_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_41_converted_to_utf8, sizeof(gb2312_row_41_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xca, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_42_converted_to_utf8, sizeof(gb2312_row_42_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xcb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_43_converted_to_utf8, sizeof(gb2312_row_43_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xcc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_44_converted_to_utf8, sizeof(gb2312_row_44_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xcd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_45_converted_to_utf8, sizeof(gb2312_row_45_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xce, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_46_converted_to_utf8, sizeof(gb2312_row_46_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xcf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_47_converted_to_utf8, sizeof(gb2312_row_47_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xd0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_48_converted_to_utf8, sizeof(gb2312_row_48_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xd1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_49_converted_to_utf8, sizeof(gb2312_row_49_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xd2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_50_converted_to_utf8, sizeof(gb2312_row_50_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xd3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_51_converted_to_utf8, sizeof(gb2312_row_51_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xd4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_52_converted_to_utf8, sizeof(gb2312_row_52_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xd5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_53_converted_to_utf8, sizeof(gb2312_row_53_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xd6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_54_converted_to_utf8, sizeof(gb2312_row_54_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xd7, gb2312_row_55, sizeof(gb2312_row_55), gb2312_row_55_converted_to_utf8, sizeof(gb2312_row_55_converted_to_utf8));

  // level 2 chinese characters
  convert_euc_to_utf8("GBK", 0xd8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_56_converted_to_utf8, sizeof(gb2312_row_56_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xd9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_57_converted_to_utf8, sizeof(gb2312_row_57_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xda, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_58_converted_to_utf8, sizeof(gb2312_row_58_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xdb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_59_converted_to_utf8, sizeof(gb2312_row_59_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xdc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_60_converted_to_utf8, sizeof(gb2312_row_60_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xdd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_61_converted_to_utf8, sizeof(gb2312_row_61_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xde, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_62_converted_to_utf8, sizeof(gb2312_row_62_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xdf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_63_converted_to_utf8, sizeof(gb2312_row_63_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_64_converted_to_utf8, sizeof(gb2312_row_64_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_65_converted_to_utf8, sizeof(gb2312_row_65_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_66_converted_to_utf8, sizeof(gb2312_row_66_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_67_converted_to_utf8, sizeof(gb2312_row_67_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_68_converted_to_utf8, sizeof(gb2312_row_68_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_69_converted_to_utf8, sizeof(gb2312_row_69_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_70_converted_to_utf8, sizeof(gb2312_row_70_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_71_converted_to_utf8, sizeof(gb2312_row_71_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_72_converted_to_utf8, sizeof(gb2312_row_72_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_73_converted_to_utf8, sizeof(gb2312_row_73_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xea, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_74_converted_to_utf8, sizeof(gb2312_row_74_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xeb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_75_converted_to_utf8, sizeof(gb2312_row_75_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xec, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_76_converted_to_utf8, sizeof(gb2312_row_76_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xed, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_77_converted_to_utf8, sizeof(gb2312_row_77_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xee, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_78_converted_to_utf8, sizeof(gb2312_row_78_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xef, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_79_converted_to_utf8, sizeof(gb2312_row_79_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xf0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_80_converted_to_utf8, sizeof(gb2312_row_80_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xf1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_81_converted_to_utf8, sizeof(gb2312_row_81_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xf2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_82_converted_to_utf8, sizeof(gb2312_row_82_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xf3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_83_converted_to_utf8, sizeof(gb2312_row_83_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xf4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_84_converted_to_utf8, sizeof(gb2312_row_84_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xf5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_85_converted_to_utf8, sizeof(gb2312_row_85_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xf6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_86_converted_to_utf8, sizeof(gb2312_row_86_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xf7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_87_converted_to_utf8, sizeof(gb2312_row_87_converted_to_utf8));
}

OFTEST(oficonv_utf8_to_gbk_2312_subset)
{
  convert_utf8_to_euc("GBK", 0xa1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_01_converted_to_utf8, sizeof(gb2312_row_01_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xa2, gb2312_row_02, sizeof(gb2312_row_02),     gb2312_row_02_converted_to_utf8, sizeof(gb2312_row_02_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xa3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_03_converted_to_utf8, sizeof(gb2312_row_03_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xa4, gb2312_row_04, sizeof(gb2312_row_04),     gb2312_row_04_converted_to_utf8, sizeof(gb2312_row_04_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xa5, gb2312_row_05, sizeof(gb2312_row_05),     gb2312_row_05_converted_to_utf8, sizeof(gb2312_row_05_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xa6, gb2312_row_06, sizeof(gb2312_row_06),     gb2312_row_06_converted_to_utf8, sizeof(gb2312_row_06_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xa7, gb2312_row_07, sizeof(gb2312_row_07),     gb2312_row_07_converted_to_utf8, sizeof(gb2312_row_07_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xa8, gb2312_row_08, sizeof(gb2312_row_08),     gb2312_row_08_converted_to_utf8, sizeof(gb2312_row_08_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xa9, gb2312_row_09, sizeof(gb2312_row_09),     gb2312_row_09_converted_to_utf8, sizeof(gb2312_row_09_converted_to_utf8));

  // level 1 chinese characters
  convert_utf8_to_euc("GBK", 0xb0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_16_converted_to_utf8, sizeof(gb2312_row_16_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_17_converted_to_utf8, sizeof(gb2312_row_17_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_18_converted_to_utf8, sizeof(gb2312_row_18_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_19_converted_to_utf8, sizeof(gb2312_row_19_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_20_converted_to_utf8, sizeof(gb2312_row_20_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_21_converted_to_utf8, sizeof(gb2312_row_21_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_22_converted_to_utf8, sizeof(gb2312_row_22_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_23_converted_to_utf8, sizeof(gb2312_row_23_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_24_converted_to_utf8, sizeof(gb2312_row_24_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_25_converted_to_utf8, sizeof(gb2312_row_25_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xba, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_26_converted_to_utf8, sizeof(gb2312_row_26_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xbb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_27_converted_to_utf8, sizeof(gb2312_row_27_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xbc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_28_converted_to_utf8, sizeof(gb2312_row_28_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xbd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_29_converted_to_utf8, sizeof(gb2312_row_29_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xbe, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_30_converted_to_utf8, sizeof(gb2312_row_30_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xbf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_31_converted_to_utf8, sizeof(gb2312_row_31_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_32_converted_to_utf8, sizeof(gb2312_row_32_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_33_converted_to_utf8, sizeof(gb2312_row_33_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_34_converted_to_utf8, sizeof(gb2312_row_34_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_35_converted_to_utf8, sizeof(gb2312_row_35_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_36_converted_to_utf8, sizeof(gb2312_row_36_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_37_converted_to_utf8, sizeof(gb2312_row_37_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_38_converted_to_utf8, sizeof(gb2312_row_38_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_39_converted_to_utf8, sizeof(gb2312_row_39_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_40_converted_to_utf8, sizeof(gb2312_row_40_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_41_converted_to_utf8, sizeof(gb2312_row_41_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xca, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_42_converted_to_utf8, sizeof(gb2312_row_42_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xcb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_43_converted_to_utf8, sizeof(gb2312_row_43_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xcc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_44_converted_to_utf8, sizeof(gb2312_row_44_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xcd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_45_converted_to_utf8, sizeof(gb2312_row_45_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xce, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_46_converted_to_utf8, sizeof(gb2312_row_46_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xcf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_47_converted_to_utf8, sizeof(gb2312_row_47_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xd0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_48_converted_to_utf8, sizeof(gb2312_row_48_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xd1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_49_converted_to_utf8, sizeof(gb2312_row_49_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xd2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_50_converted_to_utf8, sizeof(gb2312_row_50_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xd3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_51_converted_to_utf8, sizeof(gb2312_row_51_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xd4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_52_converted_to_utf8, sizeof(gb2312_row_52_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xd5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_53_converted_to_utf8, sizeof(gb2312_row_53_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xd6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_54_converted_to_utf8, sizeof(gb2312_row_54_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xd7, gb2312_row_55, sizeof(gb2312_row_55), gb2312_row_55_converted_to_utf8, sizeof(gb2312_row_55_converted_to_utf8));

  // level 2 chinese characters
  convert_utf8_to_euc("GBK", 0xd8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_56_converted_to_utf8, sizeof(gb2312_row_56_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xd9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_57_converted_to_utf8, sizeof(gb2312_row_57_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xda, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_58_converted_to_utf8, sizeof(gb2312_row_58_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xdb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_59_converted_to_utf8, sizeof(gb2312_row_59_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xdc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_60_converted_to_utf8, sizeof(gb2312_row_60_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xdd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_61_converted_to_utf8, sizeof(gb2312_row_61_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xde, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_62_converted_to_utf8, sizeof(gb2312_row_62_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xdf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_63_converted_to_utf8, sizeof(gb2312_row_63_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_64_converted_to_utf8, sizeof(gb2312_row_64_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_65_converted_to_utf8, sizeof(gb2312_row_65_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_66_converted_to_utf8, sizeof(gb2312_row_66_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_67_converted_to_utf8, sizeof(gb2312_row_67_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_68_converted_to_utf8, sizeof(gb2312_row_68_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_69_converted_to_utf8, sizeof(gb2312_row_69_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_70_converted_to_utf8, sizeof(gb2312_row_70_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_71_converted_to_utf8, sizeof(gb2312_row_71_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_72_converted_to_utf8, sizeof(gb2312_row_72_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_73_converted_to_utf8, sizeof(gb2312_row_73_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xea, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_74_converted_to_utf8, sizeof(gb2312_row_74_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xeb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_75_converted_to_utf8, sizeof(gb2312_row_75_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xec, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_76_converted_to_utf8, sizeof(gb2312_row_76_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xed, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_77_converted_to_utf8, sizeof(gb2312_row_77_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xee, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_78_converted_to_utf8, sizeof(gb2312_row_78_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xef, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_79_converted_to_utf8, sizeof(gb2312_row_79_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xf0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_80_converted_to_utf8, sizeof(gb2312_row_80_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xf1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_81_converted_to_utf8, sizeof(gb2312_row_81_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xf2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_82_converted_to_utf8, sizeof(gb2312_row_82_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xf3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_83_converted_to_utf8, sizeof(gb2312_row_83_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xf4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_84_converted_to_utf8, sizeof(gb2312_row_84_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xf5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_85_converted_to_utf8, sizeof(gb2312_row_85_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xf6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_86_converted_to_utf8, sizeof(gb2312_row_86_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xf7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_87_converted_to_utf8, sizeof(gb2312_row_87_converted_to_utf8));
}

OFTEST(oficonv_gbk_to_utf8)
{
  // The following rows are new in GBK and did not exist in GB2312.
  convert_euc_to_utf8("GBK", 0x81, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x81_converted_to_utf8, sizeof(gbk_row_0x81_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x82, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x82_converted_to_utf8, sizeof(gbk_row_0x82_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x83, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x83_converted_to_utf8, sizeof(gbk_row_0x83_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x84, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x84_converted_to_utf8, sizeof(gbk_row_0x84_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x85, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x85_converted_to_utf8, sizeof(gbk_row_0x85_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x86, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x86_converted_to_utf8, sizeof(gbk_row_0x86_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x87, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x87_converted_to_utf8, sizeof(gbk_row_0x87_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x88, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x88_converted_to_utf8, sizeof(gbk_row_0x88_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x89, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x89_converted_to_utf8, sizeof(gbk_row_0x89_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x8a, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8a_converted_to_utf8, sizeof(gbk_row_0x8a_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x8b, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8b_converted_to_utf8, sizeof(gbk_row_0x8b_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x8c, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8c_converted_to_utf8, sizeof(gbk_row_0x8c_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x8d, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8d_converted_to_utf8, sizeof(gbk_row_0x8d_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x8e, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8e_converted_to_utf8, sizeof(gbk_row_0x8e_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x8f, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8f_converted_to_utf8, sizeof(gbk_row_0x8f_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x90, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x90_converted_to_utf8, sizeof(gbk_row_0x90_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x91, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x91_converted_to_utf8, sizeof(gbk_row_0x91_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x92, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x92_converted_to_utf8, sizeof(gbk_row_0x92_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x93, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x93_converted_to_utf8, sizeof(gbk_row_0x93_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x94, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x94_converted_to_utf8, sizeof(gbk_row_0x94_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x95, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x95_converted_to_utf8, sizeof(gbk_row_0x95_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x96, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x96_converted_to_utf8, sizeof(gbk_row_0x96_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x97, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x97_converted_to_utf8, sizeof(gbk_row_0x97_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x98, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x98_converted_to_utf8, sizeof(gbk_row_0x98_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x99, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x99_converted_to_utf8, sizeof(gbk_row_0x99_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x9a, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9a_converted_to_utf8, sizeof(gbk_row_0x9a_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x9b, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9b_converted_to_utf8, sizeof(gbk_row_0x9b_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x9c, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9c_converted_to_utf8, sizeof(gbk_row_0x9c_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x9d, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9d_converted_to_utf8, sizeof(gbk_row_0x9d_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x9e, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9e_converted_to_utf8, sizeof(gbk_row_0x9e_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0x9f, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9f_converted_to_utf8, sizeof(gbk_row_0x9f_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xa0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xa0_converted_to_utf8, sizeof(gbk_row_0xa0_converted_to_utf8));

  // These rows are identical to GB2312 and do not use the byte range 0x40-0x7f.
  convert_euc_to_utf8("GBK", 0xa1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_01_converted_to_utf8_alternate, sizeof(gb2312_row_01_converted_to_utf8_alternate));
  convert_euc_to_utf8("GBK", 0xa2, gb2312_row_02, sizeof(gb2312_row_02),     gb2312_row_02_converted_to_utf8, sizeof(gb2312_row_02_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xa3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_03_converted_to_utf8, sizeof(gb2312_row_03_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xa4, gb2312_row_04, sizeof(gb2312_row_04),     gb2312_row_04_converted_to_utf8, sizeof(gb2312_row_04_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xa5, gb2312_row_05, sizeof(gb2312_row_05),     gb2312_row_05_converted_to_utf8, sizeof(gb2312_row_05_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xa6, gb2312_row_06, sizeof(gb2312_row_06),     gb2312_row_06_converted_to_utf8, sizeof(gb2312_row_06_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xa7, gb2312_row_07, sizeof(gb2312_row_07),     gb2312_row_07_converted_to_utf8, sizeof(gb2312_row_07_converted_to_utf8));

  // GB2312 row 8 has been extended
  convert_euc_to_utf8("GBK", 0xa8, gbk_row_0xa8, sizeof(gbk_row_0xa8), gbk_row_0xa8_converted_to_utf8, sizeof(gbk_row_0xa8_converted_to_utf8));

  // GB2312 row 9 has been extended
  convert_euc_to_utf8("GBK", 0xa9, gbk_row_0xa9, sizeof(gbk_row_0xa9), gbk_row_0xa9_converted_to_utf8, sizeof(gbk_row_0xa9_converted_to_utf8));

  // The following rows do not use the full set of characters
  convert_euc_to_utf8("GBK", 0xaa, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xaa_converted_to_utf8, sizeof(gbk_row_0xaa_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xab, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xab_converted_to_utf8, sizeof(gbk_row_0xab_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xac, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xac_converted_to_utf8, sizeof(gbk_row_0xac_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xad, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xad_converted_to_utf8, sizeof(gbk_row_0xad_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xae, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xae_converted_to_utf8, sizeof(gbk_row_0xae_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xaf, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xaf_converted_to_utf8, sizeof(gbk_row_0xaf_converted_to_utf8));

  // level 1 chinese characters
  convert_euc_to_utf8("GBK", 0xb0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb0_converted_to_utf8, sizeof(gbk_row_0xb0_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb1_converted_to_utf8, sizeof(gbk_row_0xb1_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb2_converted_to_utf8, sizeof(gbk_row_0xb2_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb3_converted_to_utf8, sizeof(gbk_row_0xb3_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb4_converted_to_utf8, sizeof(gbk_row_0xb4_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb5_converted_to_utf8, sizeof(gbk_row_0xb5_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb6_converted_to_utf8, sizeof(gbk_row_0xb6_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb7, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb7_converted_to_utf8, sizeof(gbk_row_0xb7_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb8, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb8_converted_to_utf8, sizeof(gbk_row_0xb8_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xb9, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb9_converted_to_utf8, sizeof(gbk_row_0xb9_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xba, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xba_converted_to_utf8, sizeof(gbk_row_0xba_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xbb, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbb_converted_to_utf8, sizeof(gbk_row_0xbb_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xbc, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbc_converted_to_utf8, sizeof(gbk_row_0xbc_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xbd, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbd_converted_to_utf8, sizeof(gbk_row_0xbd_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xbe, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbe_converted_to_utf8, sizeof(gbk_row_0xbe_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xbf, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbf_converted_to_utf8, sizeof(gbk_row_0xbf_converted_to_utf8));

  convert_euc_to_utf8("GBK", 0xc0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc0_converted_to_utf8, sizeof(gbk_row_0xc0_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc1_converted_to_utf8, sizeof(gbk_row_0xc1_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc2_converted_to_utf8, sizeof(gbk_row_0xc2_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc3_converted_to_utf8, sizeof(gbk_row_0xc3_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc4_converted_to_utf8, sizeof(gbk_row_0xc4_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc5_converted_to_utf8, sizeof(gbk_row_0xc5_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc6_converted_to_utf8, sizeof(gbk_row_0xc6_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc7, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc7_converted_to_utf8, sizeof(gbk_row_0xc7_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc8, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc8_converted_to_utf8, sizeof(gbk_row_0xc8_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xc9, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc9_converted_to_utf8, sizeof(gbk_row_0xc9_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xca, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xca_converted_to_utf8, sizeof(gbk_row_0xca_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xcb, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xcb_converted_to_utf8, sizeof(gbk_row_0xcb_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xcc, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xcc_converted_to_utf8, sizeof(gbk_row_0xcc_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xcd, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xcd_converted_to_utf8, sizeof(gbk_row_0xcd_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xce, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xce_converted_to_utf8, sizeof(gbk_row_0xce_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xcf, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xcf_converted_to_utf8, sizeof(gbk_row_0xcf_converted_to_utf8));

  convert_euc_to_utf8("GBK", 0xd0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd0_converted_to_utf8, sizeof(gbk_row_0xd0_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xd1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd1_converted_to_utf8, sizeof(gbk_row_0xd1_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xd2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd2_converted_to_utf8, sizeof(gbk_row_0xd2_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xd3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd3_converted_to_utf8, sizeof(gbk_row_0xd3_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xd4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd4_converted_to_utf8, sizeof(gbk_row_0xd4_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xd5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd5_converted_to_utf8, sizeof(gbk_row_0xd5_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xd6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd6_converted_to_utf8, sizeof(gbk_row_0xd6_converted_to_utf8));

  // end of level 1 chinese characters
  convert_euc_to_utf8("GBK", 0xd7, gbk_row_0xd7, sizeof(gbk_row_0xd7), gbk_row_0xd7_converted_to_utf8, sizeof(gbk_row_0xd7_converted_to_utf8));

  // level 2 chinese characters
  convert_euc_to_utf8("GBK", 0xd8, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd8_converted_to_utf8, sizeof(gbk_row_0xd8_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xd9, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd9_converted_to_utf8, sizeof(gbk_row_0xd9_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xda, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xda_converted_to_utf8, sizeof(gbk_row_0xda_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xdb, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xdb_converted_to_utf8, sizeof(gbk_row_0xdb_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xdc, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xdc_converted_to_utf8, sizeof(gbk_row_0xdc_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xdd, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xdd_converted_to_utf8, sizeof(gbk_row_0xdd_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xde, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xde_converted_to_utf8, sizeof(gbk_row_0xde_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xdf, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xdf_converted_to_utf8, sizeof(gbk_row_0xdf_converted_to_utf8));

  convert_euc_to_utf8("GBK", 0xe0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe0_converted_to_utf8, sizeof(gbk_row_0xe0_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe1_converted_to_utf8, sizeof(gbk_row_0xe1_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe2_converted_to_utf8, sizeof(gbk_row_0xe2_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe3_converted_to_utf8, sizeof(gbk_row_0xe3_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe4_converted_to_utf8, sizeof(gbk_row_0xe4_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe5_converted_to_utf8, sizeof(gbk_row_0xe5_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe6_converted_to_utf8, sizeof(gbk_row_0xe6_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe7, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe7_converted_to_utf8, sizeof(gbk_row_0xe7_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe8, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe8_converted_to_utf8, sizeof(gbk_row_0xe8_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xe9, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe9_converted_to_utf8, sizeof(gbk_row_0xe9_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xea, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xea_converted_to_utf8, sizeof(gbk_row_0xea_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xeb, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xeb_converted_to_utf8, sizeof(gbk_row_0xeb_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xec, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xec_converted_to_utf8, sizeof(gbk_row_0xec_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xed, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xed_converted_to_utf8, sizeof(gbk_row_0xed_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xee, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xee_converted_to_utf8, sizeof(gbk_row_0xee_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xef, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xef_converted_to_utf8, sizeof(gbk_row_0xef_converted_to_utf8));

  convert_euc_to_utf8("GBK", 0xf0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf0_converted_to_utf8, sizeof(gbk_row_0xf0_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xf1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf1_converted_to_utf8, sizeof(gbk_row_0xf1_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xf2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf2_converted_to_utf8, sizeof(gbk_row_0xf2_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xf3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf3_converted_to_utf8, sizeof(gbk_row_0xf3_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xf4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf4_converted_to_utf8, sizeof(gbk_row_0xf4_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xf5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf5_converted_to_utf8, sizeof(gbk_row_0xf5_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xf6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf6_converted_to_utf8, sizeof(gbk_row_0xf6_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xf7, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf7_converted_to_utf8, sizeof(gbk_row_0xf7_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xf8, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xf8_converted_to_utf8, sizeof(gbk_row_0xf8_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xf9, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xf9_converted_to_utf8, sizeof(gbk_row_0xf9_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xfa, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xfa_converted_to_utf8, sizeof(gbk_row_0xfa_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xfb, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xfb_converted_to_utf8, sizeof(gbk_row_0xfb_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xfc, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xfc_converted_to_utf8, sizeof(gbk_row_0xfc_converted_to_utf8));
  convert_euc_to_utf8("GBK", 0xfd, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xfd_converted_to_utf8, sizeof(gbk_row_0xfd_converted_to_utf8));

  // end of level 2 chinese characters
  convert_euc_to_utf8("GBK", 0xfe, gbk_row_0xfe, sizeof(gbk_row_0xfe), gbk_row_0xfe_converted_to_utf8, sizeof(gbk_row_0xfe_converted_to_utf8));
}

OFTEST(oficonv_utf8_to_gbk)
{
  // The following rows are new in GBK and did not exist in GB2312.
  convert_utf8_to_euc("GBK", 0x81, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x81_converted_to_utf8, sizeof(gbk_row_0x81_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x82, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x82_converted_to_utf8, sizeof(gbk_row_0x82_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x83, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x83_converted_to_utf8, sizeof(gbk_row_0x83_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x84, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x84_converted_to_utf8, sizeof(gbk_row_0x84_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x85, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x85_converted_to_utf8, sizeof(gbk_row_0x85_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x86, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x86_converted_to_utf8, sizeof(gbk_row_0x86_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x87, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x87_converted_to_utf8, sizeof(gbk_row_0x87_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x88, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x88_converted_to_utf8, sizeof(gbk_row_0x88_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x89, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x89_converted_to_utf8, sizeof(gbk_row_0x89_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x8a, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8a_converted_to_utf8, sizeof(gbk_row_0x8a_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x8b, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8b_converted_to_utf8, sizeof(gbk_row_0x8b_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x8c, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8c_converted_to_utf8, sizeof(gbk_row_0x8c_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x8d, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8d_converted_to_utf8, sizeof(gbk_row_0x8d_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x8e, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8e_converted_to_utf8, sizeof(gbk_row_0x8e_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x8f, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8f_converted_to_utf8, sizeof(gbk_row_0x8f_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x90, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x90_converted_to_utf8, sizeof(gbk_row_0x90_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x91, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x91_converted_to_utf8, sizeof(gbk_row_0x91_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x92, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x92_converted_to_utf8, sizeof(gbk_row_0x92_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x93, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x93_converted_to_utf8, sizeof(gbk_row_0x93_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x94, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x94_converted_to_utf8, sizeof(gbk_row_0x94_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x95, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x95_converted_to_utf8, sizeof(gbk_row_0x95_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x96, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x96_converted_to_utf8, sizeof(gbk_row_0x96_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x97, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x97_converted_to_utf8, sizeof(gbk_row_0x97_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x98, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x98_converted_to_utf8, sizeof(gbk_row_0x98_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x99, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x99_converted_to_utf8, sizeof(gbk_row_0x99_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x9a, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9a_converted_to_utf8, sizeof(gbk_row_0x9a_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x9b, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9b_converted_to_utf8, sizeof(gbk_row_0x9b_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x9c, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9c_converted_to_utf8, sizeof(gbk_row_0x9c_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x9d, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9d_converted_to_utf8, sizeof(gbk_row_0x9d_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x9e, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9e_converted_to_utf8, sizeof(gbk_row_0x9e_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0x9f, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9f_converted_to_utf8, sizeof(gbk_row_0x9f_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xa0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xa0_converted_to_utf8, sizeof(gbk_row_0xa0_converted_to_utf8));

  // These rows are identical to GB2312 and do not use the byte range 0x40-0x7f.
  convert_utf8_to_euc("GBK", 0xa1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_01_converted_to_utf8_alternate, sizeof(gb2312_row_01_converted_to_utf8_alternate));
  convert_utf8_to_euc("GBK", 0xa2, gb2312_row_02, sizeof(gb2312_row_02),     gb2312_row_02_converted_to_utf8, sizeof(gb2312_row_02_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xa3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_03_converted_to_utf8, sizeof(gb2312_row_03_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xa4, gb2312_row_04, sizeof(gb2312_row_04),     gb2312_row_04_converted_to_utf8, sizeof(gb2312_row_04_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xa5, gb2312_row_05, sizeof(gb2312_row_05),     gb2312_row_05_converted_to_utf8, sizeof(gb2312_row_05_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xa6, gb2312_row_06, sizeof(gb2312_row_06),     gb2312_row_06_converted_to_utf8, sizeof(gb2312_row_06_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xa7, gb2312_row_07, sizeof(gb2312_row_07),     gb2312_row_07_converted_to_utf8, sizeof(gb2312_row_07_converted_to_utf8));

  // GB2312 row 8 has been extended
  // the mapping of the dash character is not reversible, it is mapped to 0xa1aa instead of 0xa844, which is, however, permitted.
  convert_from_utf8("GBK", gbk_row_0xa8_alternative, sizeof(gbk_row_0xa8_alternative), gbk_row_0xa8_converted_to_utf8, sizeof(gbk_row_0xa8_converted_to_utf8));

  // GB2312 row 9 has been extended
  // the mapping of two characters is not reversible, 0xa95a and 0xa95c are mapped to 0xa9d6 and 0xa9d7, repectively, which seems a bit fishy
  convert_from_utf8("GBK", gbk_row_0xa9_alternative, sizeof(gbk_row_0xa9_alternative), gbk_row_0xa9_converted_to_utf8, sizeof(gbk_row_0xa9_converted_to_utf8));

  // The following rows do not use the full set of characters
  convert_utf8_to_euc("GBK", 0xaa, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xaa_converted_to_utf8, sizeof(gbk_row_0xaa_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xab, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xab_converted_to_utf8, sizeof(gbk_row_0xab_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xac, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xac_converted_to_utf8, sizeof(gbk_row_0xac_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xad, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xad_converted_to_utf8, sizeof(gbk_row_0xad_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xae, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xae_converted_to_utf8, sizeof(gbk_row_0xae_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xaf, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xaf_converted_to_utf8, sizeof(gbk_row_0xaf_converted_to_utf8));

  // level 1 chinese characters
  convert_utf8_to_euc("GBK", 0xb0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb0_converted_to_utf8, sizeof(gbk_row_0xb0_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb1_converted_to_utf8, sizeof(gbk_row_0xb1_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb2_converted_to_utf8, sizeof(gbk_row_0xb2_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb3_converted_to_utf8, sizeof(gbk_row_0xb3_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb4_converted_to_utf8, sizeof(gbk_row_0xb4_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb5_converted_to_utf8, sizeof(gbk_row_0xb5_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb6_converted_to_utf8, sizeof(gbk_row_0xb6_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb7, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb7_converted_to_utf8, sizeof(gbk_row_0xb7_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb8, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb8_converted_to_utf8, sizeof(gbk_row_0xb8_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xb9, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb9_converted_to_utf8, sizeof(gbk_row_0xb9_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xba, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xba_converted_to_utf8, sizeof(gbk_row_0xba_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xbb, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbb_converted_to_utf8, sizeof(gbk_row_0xbb_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xbc, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbc_converted_to_utf8, sizeof(gbk_row_0xbc_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xbd, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbd_converted_to_utf8, sizeof(gbk_row_0xbd_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xbe, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbe_converted_to_utf8, sizeof(gbk_row_0xbe_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xbf, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbf_converted_to_utf8, sizeof(gbk_row_0xbf_converted_to_utf8));

  convert_utf8_to_euc("GBK", 0xc0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc0_converted_to_utf8, sizeof(gbk_row_0xc0_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc1_converted_to_utf8, sizeof(gbk_row_0xc1_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc2_converted_to_utf8, sizeof(gbk_row_0xc2_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc3_converted_to_utf8, sizeof(gbk_row_0xc3_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc4_converted_to_utf8, sizeof(gbk_row_0xc4_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc5_converted_to_utf8, sizeof(gbk_row_0xc5_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc6_converted_to_utf8, sizeof(gbk_row_0xc6_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc7, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc7_converted_to_utf8, sizeof(gbk_row_0xc7_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc8, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc8_converted_to_utf8, sizeof(gbk_row_0xc8_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xc9, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc9_converted_to_utf8, sizeof(gbk_row_0xc9_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xca, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xca_converted_to_utf8, sizeof(gbk_row_0xca_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xcb, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xcb_converted_to_utf8, sizeof(gbk_row_0xcb_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xcc, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xcc_converted_to_utf8, sizeof(gbk_row_0xcc_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xcd, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xcd_converted_to_utf8, sizeof(gbk_row_0xcd_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xce, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xce_converted_to_utf8, sizeof(gbk_row_0xce_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xcf, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xcf_converted_to_utf8, sizeof(gbk_row_0xcf_converted_to_utf8));

  convert_utf8_to_euc("GBK", 0xd0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd0_converted_to_utf8, sizeof(gbk_row_0xd0_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xd1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd1_converted_to_utf8, sizeof(gbk_row_0xd1_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xd2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd2_converted_to_utf8, sizeof(gbk_row_0xd2_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xd3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd3_converted_to_utf8, sizeof(gbk_row_0xd3_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xd4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd4_converted_to_utf8, sizeof(gbk_row_0xd4_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xd5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd5_converted_to_utf8, sizeof(gbk_row_0xd5_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xd6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd6_converted_to_utf8, sizeof(gbk_row_0xd6_converted_to_utf8));

  // end of level 1 chinese characters
  convert_utf8_to_euc("GBK", 0xd7, gbk_row_0xd7, sizeof(gbk_row_0xd7), gbk_row_0xd7_converted_to_utf8, sizeof(gbk_row_0xd7_converted_to_utf8));

  // level 2 chinese characters
  convert_utf8_to_euc("GBK", 0xd8, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd8_converted_to_utf8, sizeof(gbk_row_0xd8_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xd9, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd9_converted_to_utf8, sizeof(gbk_row_0xd9_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xda, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xda_converted_to_utf8, sizeof(gbk_row_0xda_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xdb, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xdb_converted_to_utf8, sizeof(gbk_row_0xdb_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xdc, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xdc_converted_to_utf8, sizeof(gbk_row_0xdc_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xdd, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xdd_converted_to_utf8, sizeof(gbk_row_0xdd_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xde, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xde_converted_to_utf8, sizeof(gbk_row_0xde_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xdf, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xdf_converted_to_utf8, sizeof(gbk_row_0xdf_converted_to_utf8));

  convert_utf8_to_euc("GBK", 0xe0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe0_converted_to_utf8, sizeof(gbk_row_0xe0_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe1_converted_to_utf8, sizeof(gbk_row_0xe1_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe2_converted_to_utf8, sizeof(gbk_row_0xe2_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe3_converted_to_utf8, sizeof(gbk_row_0xe3_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe4_converted_to_utf8, sizeof(gbk_row_0xe4_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe5_converted_to_utf8, sizeof(gbk_row_0xe5_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe6_converted_to_utf8, sizeof(gbk_row_0xe6_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe7, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe7_converted_to_utf8, sizeof(gbk_row_0xe7_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe8, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe8_converted_to_utf8, sizeof(gbk_row_0xe8_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xe9, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe9_converted_to_utf8, sizeof(gbk_row_0xe9_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xea, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xea_converted_to_utf8, sizeof(gbk_row_0xea_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xeb, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xeb_converted_to_utf8, sizeof(gbk_row_0xeb_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xec, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xec_converted_to_utf8, sizeof(gbk_row_0xec_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xed, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xed_converted_to_utf8, sizeof(gbk_row_0xed_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xee, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xee_converted_to_utf8, sizeof(gbk_row_0xee_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xef, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xef_converted_to_utf8, sizeof(gbk_row_0xef_converted_to_utf8));

  convert_utf8_to_euc("GBK", 0xf0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf0_converted_to_utf8, sizeof(gbk_row_0xf0_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xf1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf1_converted_to_utf8, sizeof(gbk_row_0xf1_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xf2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf2_converted_to_utf8, sizeof(gbk_row_0xf2_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xf3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf3_converted_to_utf8, sizeof(gbk_row_0xf3_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xf4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf4_converted_to_utf8, sizeof(gbk_row_0xf4_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xf5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf5_converted_to_utf8, sizeof(gbk_row_0xf5_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xf6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf6_converted_to_utf8, sizeof(gbk_row_0xf6_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xf7, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf7_converted_to_utf8, sizeof(gbk_row_0xf7_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xf8, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xf8_converted_to_utf8, sizeof(gbk_row_0xf8_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xf9, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xf9_converted_to_utf8, sizeof(gbk_row_0xf9_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xfa, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xfa_converted_to_utf8, sizeof(gbk_row_0xfa_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xfb, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xfb_converted_to_utf8, sizeof(gbk_row_0xfb_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xfc, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xfc_converted_to_utf8, sizeof(gbk_row_0xfc_converted_to_utf8));
  convert_utf8_to_euc("GBK", 0xfd, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xfd_converted_to_utf8, sizeof(gbk_row_0xfd_converted_to_utf8));

  // end of level 2 chinese characters
  // the mapping of several characters is not reversible and seems fishy:
  // characters in the range 0x50-7e are mapped to 0xd0-fe when converting back from UTF-8, i.e. an offset of 0x80 is added
  convert_from_utf8("GBK", gbk_row_0xfe_alternative, sizeof(gbk_row_0xfe_alternative), gbk_row_0xfe_converted_to_utf8, sizeof(gbk_row_0xfe_converted_to_utf8));
}

OFTEST(oficonv_gb18030_2312_subset_to_utf8)
{
  convert_euc_to_utf8("GB18030", 0xa1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_01_converted_to_utf8_alternate, sizeof(gb2312_row_01_converted_to_utf8_alternate));
  convert_euc_to_utf8("GB18030", 0xa2, gb2312_row_02, sizeof(gb2312_row_02),     gb2312_row_02_converted_to_utf8, sizeof(gb2312_row_02_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xa3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_03_converted_to_utf8, sizeof(gb2312_row_03_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xa4, gb2312_row_04, sizeof(gb2312_row_04),     gb2312_row_04_converted_to_utf8, sizeof(gb2312_row_04_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xa5, gb2312_row_05, sizeof(gb2312_row_05),     gb2312_row_05_converted_to_utf8, sizeof(gb2312_row_05_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xa6, gb2312_row_06, sizeof(gb2312_row_06),     gb2312_row_06_converted_to_utf8, sizeof(gb2312_row_06_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xa7, gb2312_row_07, sizeof(gb2312_row_07),     gb2312_row_07_converted_to_utf8, sizeof(gb2312_row_07_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xa8, gb2312_row_08, sizeof(gb2312_row_08),     gb2312_row_08_converted_to_utf8, sizeof(gb2312_row_08_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xa9, gb2312_row_09, sizeof(gb2312_row_09),     gb2312_row_09_converted_to_utf8, sizeof(gb2312_row_09_converted_to_utf8));

  // level 1 chinese characters
  convert_euc_to_utf8("GB18030", 0xb0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_16_converted_to_utf8, sizeof(gb2312_row_16_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_17_converted_to_utf8, sizeof(gb2312_row_17_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_18_converted_to_utf8, sizeof(gb2312_row_18_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_19_converted_to_utf8, sizeof(gb2312_row_19_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_20_converted_to_utf8, sizeof(gb2312_row_20_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_21_converted_to_utf8, sizeof(gb2312_row_21_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_22_converted_to_utf8, sizeof(gb2312_row_22_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_23_converted_to_utf8, sizeof(gb2312_row_23_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_24_converted_to_utf8, sizeof(gb2312_row_24_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_25_converted_to_utf8, sizeof(gb2312_row_25_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xba, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_26_converted_to_utf8, sizeof(gb2312_row_26_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xbb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_27_converted_to_utf8, sizeof(gb2312_row_27_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xbc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_28_converted_to_utf8, sizeof(gb2312_row_28_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xbd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_29_converted_to_utf8, sizeof(gb2312_row_29_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xbe, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_30_converted_to_utf8, sizeof(gb2312_row_30_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xbf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_31_converted_to_utf8, sizeof(gb2312_row_31_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_32_converted_to_utf8, sizeof(gb2312_row_32_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_33_converted_to_utf8, sizeof(gb2312_row_33_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_34_converted_to_utf8, sizeof(gb2312_row_34_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_35_converted_to_utf8, sizeof(gb2312_row_35_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_36_converted_to_utf8, sizeof(gb2312_row_36_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_37_converted_to_utf8, sizeof(gb2312_row_37_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_38_converted_to_utf8, sizeof(gb2312_row_38_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_39_converted_to_utf8, sizeof(gb2312_row_39_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_40_converted_to_utf8, sizeof(gb2312_row_40_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_41_converted_to_utf8, sizeof(gb2312_row_41_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xca, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_42_converted_to_utf8, sizeof(gb2312_row_42_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xcb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_43_converted_to_utf8, sizeof(gb2312_row_43_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xcc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_44_converted_to_utf8, sizeof(gb2312_row_44_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xcd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_45_converted_to_utf8, sizeof(gb2312_row_45_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xce, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_46_converted_to_utf8, sizeof(gb2312_row_46_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xcf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_47_converted_to_utf8, sizeof(gb2312_row_47_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xd0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_48_converted_to_utf8, sizeof(gb2312_row_48_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xd1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_49_converted_to_utf8, sizeof(gb2312_row_49_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xd2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_50_converted_to_utf8, sizeof(gb2312_row_50_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xd3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_51_converted_to_utf8, sizeof(gb2312_row_51_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xd4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_52_converted_to_utf8, sizeof(gb2312_row_52_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xd5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_53_converted_to_utf8, sizeof(gb2312_row_53_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xd6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_54_converted_to_utf8, sizeof(gb2312_row_54_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xd7, gb2312_row_55, sizeof(gb2312_row_55), gb2312_row_55_converted_to_utf8, sizeof(gb2312_row_55_converted_to_utf8));

  // level 2 chinese characters
  convert_euc_to_utf8("GB18030", 0xd8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_56_converted_to_utf8, sizeof(gb2312_row_56_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xd9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_57_converted_to_utf8, sizeof(gb2312_row_57_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xda, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_58_converted_to_utf8, sizeof(gb2312_row_58_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xdb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_59_converted_to_utf8, sizeof(gb2312_row_59_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xdc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_60_converted_to_utf8, sizeof(gb2312_row_60_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xdd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_61_converted_to_utf8, sizeof(gb2312_row_61_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xde, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_62_converted_to_utf8, sizeof(gb2312_row_62_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xdf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_63_converted_to_utf8, sizeof(gb2312_row_63_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_64_converted_to_utf8, sizeof(gb2312_row_64_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_65_converted_to_utf8, sizeof(gb2312_row_65_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_66_converted_to_utf8, sizeof(gb2312_row_66_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_67_converted_to_utf8, sizeof(gb2312_row_67_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_68_converted_to_utf8, sizeof(gb2312_row_68_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_69_converted_to_utf8, sizeof(gb2312_row_69_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_70_converted_to_utf8, sizeof(gb2312_row_70_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_71_converted_to_utf8, sizeof(gb2312_row_71_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_72_converted_to_utf8, sizeof(gb2312_row_72_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_73_converted_to_utf8, sizeof(gb2312_row_73_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xea, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_74_converted_to_utf8, sizeof(gb2312_row_74_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xeb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_75_converted_to_utf8, sizeof(gb2312_row_75_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xec, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_76_converted_to_utf8, sizeof(gb2312_row_76_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xed, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_77_converted_to_utf8, sizeof(gb2312_row_77_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xee, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_78_converted_to_utf8, sizeof(gb2312_row_78_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xef, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_79_converted_to_utf8, sizeof(gb2312_row_79_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xf0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_80_converted_to_utf8, sizeof(gb2312_row_80_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xf1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_81_converted_to_utf8, sizeof(gb2312_row_81_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xf2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_82_converted_to_utf8, sizeof(gb2312_row_82_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xf3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_83_converted_to_utf8, sizeof(gb2312_row_83_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xf4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_84_converted_to_utf8, sizeof(gb2312_row_84_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xf5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_85_converted_to_utf8, sizeof(gb2312_row_85_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xf6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_86_converted_to_utf8, sizeof(gb2312_row_86_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xf7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_87_converted_to_utf8, sizeof(gb2312_row_87_converted_to_utf8));
}

OFTEST(oficonv_utf8_to_gb18030_2312_subset)
{
  // the mapping of character 0x21a4 (interpunct) is not reversible and needs special handling
  convert_from_utf8("GB18030", gb2312_row_01_converted_from_utf8_gb18030_alternate, sizeof(gb2312_row_01_converted_from_utf8_gb18030_alternate), gb2312_row_01_converted_to_utf8, sizeof(gb2312_row_01_converted_to_utf8));

  convert_utf8_to_euc("GB18030", 0xa2, gb2312_row_02, sizeof(gb2312_row_02),     gb2312_row_02_converted_to_utf8, sizeof(gb2312_row_02_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xa3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_03_converted_to_utf8, sizeof(gb2312_row_03_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xa4, gb2312_row_04, sizeof(gb2312_row_04),     gb2312_row_04_converted_to_utf8, sizeof(gb2312_row_04_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xa5, gb2312_row_05, sizeof(gb2312_row_05),     gb2312_row_05_converted_to_utf8, sizeof(gb2312_row_05_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xa6, gb2312_row_06, sizeof(gb2312_row_06),     gb2312_row_06_converted_to_utf8, sizeof(gb2312_row_06_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xa7, gb2312_row_07, sizeof(gb2312_row_07),     gb2312_row_07_converted_to_utf8, sizeof(gb2312_row_07_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xa8, gb2312_row_08, sizeof(gb2312_row_08),     gb2312_row_08_converted_to_utf8, sizeof(gb2312_row_08_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xa9, gb2312_row_09, sizeof(gb2312_row_09),     gb2312_row_09_converted_to_utf8, sizeof(gb2312_row_09_converted_to_utf8));

  // level 1 chinese characters
  convert_utf8_to_euc("GB18030", 0xb0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_16_converted_to_utf8, sizeof(gb2312_row_16_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_17_converted_to_utf8, sizeof(gb2312_row_17_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_18_converted_to_utf8, sizeof(gb2312_row_18_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_19_converted_to_utf8, sizeof(gb2312_row_19_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_20_converted_to_utf8, sizeof(gb2312_row_20_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_21_converted_to_utf8, sizeof(gb2312_row_21_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_22_converted_to_utf8, sizeof(gb2312_row_22_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_23_converted_to_utf8, sizeof(gb2312_row_23_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_24_converted_to_utf8, sizeof(gb2312_row_24_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_25_converted_to_utf8, sizeof(gb2312_row_25_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xba, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_26_converted_to_utf8, sizeof(gb2312_row_26_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xbb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_27_converted_to_utf8, sizeof(gb2312_row_27_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xbc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_28_converted_to_utf8, sizeof(gb2312_row_28_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xbd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_29_converted_to_utf8, sizeof(gb2312_row_29_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xbe, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_30_converted_to_utf8, sizeof(gb2312_row_30_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xbf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_31_converted_to_utf8, sizeof(gb2312_row_31_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_32_converted_to_utf8, sizeof(gb2312_row_32_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_33_converted_to_utf8, sizeof(gb2312_row_33_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_34_converted_to_utf8, sizeof(gb2312_row_34_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_35_converted_to_utf8, sizeof(gb2312_row_35_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_36_converted_to_utf8, sizeof(gb2312_row_36_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_37_converted_to_utf8, sizeof(gb2312_row_37_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_38_converted_to_utf8, sizeof(gb2312_row_38_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_39_converted_to_utf8, sizeof(gb2312_row_39_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_40_converted_to_utf8, sizeof(gb2312_row_40_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_41_converted_to_utf8, sizeof(gb2312_row_41_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xca, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_42_converted_to_utf8, sizeof(gb2312_row_42_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xcb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_43_converted_to_utf8, sizeof(gb2312_row_43_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xcc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_44_converted_to_utf8, sizeof(gb2312_row_44_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xcd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_45_converted_to_utf8, sizeof(gb2312_row_45_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xce, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_46_converted_to_utf8, sizeof(gb2312_row_46_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xcf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_47_converted_to_utf8, sizeof(gb2312_row_47_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xd0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_48_converted_to_utf8, sizeof(gb2312_row_48_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xd1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_49_converted_to_utf8, sizeof(gb2312_row_49_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xd2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_50_converted_to_utf8, sizeof(gb2312_row_50_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xd3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_51_converted_to_utf8, sizeof(gb2312_row_51_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xd4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_52_converted_to_utf8, sizeof(gb2312_row_52_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xd5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_53_converted_to_utf8, sizeof(gb2312_row_53_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xd6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_54_converted_to_utf8, sizeof(gb2312_row_54_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xd7, gb2312_row_55, sizeof(gb2312_row_55), gb2312_row_55_converted_to_utf8, sizeof(gb2312_row_55_converted_to_utf8));

  // level 2 chinese characters
  convert_utf8_to_euc("GB18030", 0xd8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_56_converted_to_utf8, sizeof(gb2312_row_56_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xd9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_57_converted_to_utf8, sizeof(gb2312_row_57_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xda, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_58_converted_to_utf8, sizeof(gb2312_row_58_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xdb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_59_converted_to_utf8, sizeof(gb2312_row_59_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xdc, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_60_converted_to_utf8, sizeof(gb2312_row_60_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xdd, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_61_converted_to_utf8, sizeof(gb2312_row_61_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xde, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_62_converted_to_utf8, sizeof(gb2312_row_62_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xdf, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_63_converted_to_utf8, sizeof(gb2312_row_63_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_64_converted_to_utf8, sizeof(gb2312_row_64_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_65_converted_to_utf8, sizeof(gb2312_row_65_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_66_converted_to_utf8, sizeof(gb2312_row_66_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_67_converted_to_utf8, sizeof(gb2312_row_67_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_68_converted_to_utf8, sizeof(gb2312_row_68_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_69_converted_to_utf8, sizeof(gb2312_row_69_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_70_converted_to_utf8, sizeof(gb2312_row_70_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_71_converted_to_utf8, sizeof(gb2312_row_71_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe8, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_72_converted_to_utf8, sizeof(gb2312_row_72_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe9, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_73_converted_to_utf8, sizeof(gb2312_row_73_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xea, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_74_converted_to_utf8, sizeof(gb2312_row_74_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xeb, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_75_converted_to_utf8, sizeof(gb2312_row_75_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xec, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_76_converted_to_utf8, sizeof(gb2312_row_76_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xed, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_77_converted_to_utf8, sizeof(gb2312_row_77_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xee, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_78_converted_to_utf8, sizeof(gb2312_row_78_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xef, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_79_converted_to_utf8, sizeof(gb2312_row_79_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xf0, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_80_converted_to_utf8, sizeof(gb2312_row_80_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xf1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_81_converted_to_utf8, sizeof(gb2312_row_81_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xf2, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_82_converted_to_utf8, sizeof(gb2312_row_82_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xf3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_83_converted_to_utf8, sizeof(gb2312_row_83_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xf4, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_84_converted_to_utf8, sizeof(gb2312_row_84_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xf5, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_85_converted_to_utf8, sizeof(gb2312_row_85_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xf6, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_86_converted_to_utf8, sizeof(gb2312_row_86_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xf7, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_87_converted_to_utf8, sizeof(gb2312_row_87_converted_to_utf8));
}

OFTEST(oficonv_gb18030_gbk_subset_to_utf8)
{
  // The following rows are new in GBK and did not exist in GB2312.
  convert_euc_to_utf8("GB18030", 0x81, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x81_converted_to_utf8, sizeof(gbk_row_0x81_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x82, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x82_converted_to_utf8, sizeof(gbk_row_0x82_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x83, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x83_converted_to_utf8, sizeof(gbk_row_0x83_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x84, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x84_converted_to_utf8, sizeof(gbk_row_0x84_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x85, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x85_converted_to_utf8, sizeof(gbk_row_0x85_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x86, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x86_converted_to_utf8, sizeof(gbk_row_0x86_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x87, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x87_converted_to_utf8, sizeof(gbk_row_0x87_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x88, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x88_converted_to_utf8, sizeof(gbk_row_0x88_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x89, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x89_converted_to_utf8, sizeof(gbk_row_0x89_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x8a, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8a_converted_to_utf8, sizeof(gbk_row_0x8a_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x8b, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8b_converted_to_utf8, sizeof(gbk_row_0x8b_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x8c, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8c_converted_to_utf8, sizeof(gbk_row_0x8c_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x8d, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8d_converted_to_utf8, sizeof(gbk_row_0x8d_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x8e, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8e_converted_to_utf8, sizeof(gbk_row_0x8e_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x8f, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8f_converted_to_utf8, sizeof(gbk_row_0x8f_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x90, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x90_converted_to_utf8, sizeof(gbk_row_0x90_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x91, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x91_converted_to_utf8, sizeof(gbk_row_0x91_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x92, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x92_converted_to_utf8, sizeof(gbk_row_0x92_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x93, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x93_converted_to_utf8, sizeof(gbk_row_0x93_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x94, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x94_converted_to_utf8, sizeof(gbk_row_0x94_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x95, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x95_converted_to_utf8, sizeof(gbk_row_0x95_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x96, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x96_converted_to_utf8, sizeof(gbk_row_0x96_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x97, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x97_converted_to_utf8, sizeof(gbk_row_0x97_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x98, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x98_converted_to_utf8, sizeof(gbk_row_0x98_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x99, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x99_converted_to_utf8, sizeof(gbk_row_0x99_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x9a, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9a_converted_to_utf8, sizeof(gbk_row_0x9a_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x9b, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9b_converted_to_utf8, sizeof(gbk_row_0x9b_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x9c, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9c_converted_to_utf8, sizeof(gbk_row_0x9c_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x9d, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9d_converted_to_utf8, sizeof(gbk_row_0x9d_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x9e, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9e_converted_to_utf8, sizeof(gbk_row_0x9e_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0x9f, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9f_converted_to_utf8, sizeof(gbk_row_0x9f_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xa0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xa0_converted_to_utf8, sizeof(gbk_row_0xa0_converted_to_utf8));

  // These rows are identical to GB2312 and do not use the byte range 0x40-0x7f.
  convert_euc_to_utf8("GB18030", 0xa1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_01_converted_to_utf8_alternate, sizeof(gb2312_row_01_converted_to_utf8_alternate));
  convert_euc_to_utf8("GB18030", 0xa2, gb2312_row_02, sizeof(gb2312_row_02),     gb2312_row_02_converted_to_utf8, sizeof(gb2312_row_02_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xa3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_03_converted_to_utf8, sizeof(gb2312_row_03_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xa4, gb2312_row_04, sizeof(gb2312_row_04),     gb2312_row_04_converted_to_utf8, sizeof(gb2312_row_04_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xa5, gb2312_row_05, sizeof(gb2312_row_05),     gb2312_row_05_converted_to_utf8, sizeof(gb2312_row_05_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xa6, gb2312_row_06, sizeof(gb2312_row_06),     gb2312_row_06_converted_to_utf8, sizeof(gb2312_row_06_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xa7, gb2312_row_07, sizeof(gb2312_row_07),     gb2312_row_07_converted_to_utf8, sizeof(gb2312_row_07_converted_to_utf8));

  // GB2312 row 8 has been extended
  convert_euc_to_utf8("GB18030", 0xa8, gbk_row_0xa8, sizeof(gbk_row_0xa8), gbk_row_0xa8_converted_to_utf8, sizeof(gbk_row_0xa8_converted_to_utf8));

  // GB2312 row 9 has been extended
  convert_euc_to_utf8("GB18030", 0xa9, gbk_row_0xa9, sizeof(gbk_row_0xa9), gbk_row_0xa9_converted_to_utf8, sizeof(gbk_row_0xa9_converted_to_utf8));

  // The following rows do not use the full set of characters
  convert_euc_to_utf8("GB18030", 0xaa, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xaa_converted_to_utf8, sizeof(gbk_row_0xaa_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xab, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xab_converted_to_utf8, sizeof(gbk_row_0xab_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xac, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xac_converted_to_utf8, sizeof(gbk_row_0xac_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xad, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xad_converted_to_utf8, sizeof(gbk_row_0xad_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xae, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xae_converted_to_utf8, sizeof(gbk_row_0xae_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xaf, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xaf_converted_to_utf8, sizeof(gbk_row_0xaf_converted_to_utf8));

  // level 1 chinese characters
  convert_euc_to_utf8("GB18030", 0xb0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb0_converted_to_utf8, sizeof(gbk_row_0xb0_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb1_converted_to_utf8, sizeof(gbk_row_0xb1_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb2_converted_to_utf8, sizeof(gbk_row_0xb2_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb3_converted_to_utf8, sizeof(gbk_row_0xb3_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb4_converted_to_utf8, sizeof(gbk_row_0xb4_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb5_converted_to_utf8, sizeof(gbk_row_0xb5_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb6_converted_to_utf8, sizeof(gbk_row_0xb6_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb7, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb7_converted_to_utf8, sizeof(gbk_row_0xb7_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb8, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb8_converted_to_utf8, sizeof(gbk_row_0xb8_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xb9, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb9_converted_to_utf8, sizeof(gbk_row_0xb9_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xba, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xba_converted_to_utf8, sizeof(gbk_row_0xba_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xbb, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbb_converted_to_utf8, sizeof(gbk_row_0xbb_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xbc, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbc_converted_to_utf8, sizeof(gbk_row_0xbc_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xbd, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbd_converted_to_utf8, sizeof(gbk_row_0xbd_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xbe, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbe_converted_to_utf8, sizeof(gbk_row_0xbe_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xbf, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbf_converted_to_utf8, sizeof(gbk_row_0xbf_converted_to_utf8));

  convert_euc_to_utf8("GB18030", 0xc0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc0_converted_to_utf8, sizeof(gbk_row_0xc0_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc1_converted_to_utf8, sizeof(gbk_row_0xc1_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc2_converted_to_utf8, sizeof(gbk_row_0xc2_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc3_converted_to_utf8, sizeof(gbk_row_0xc3_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc4_converted_to_utf8, sizeof(gbk_row_0xc4_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc5_converted_to_utf8, sizeof(gbk_row_0xc5_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc6_converted_to_utf8, sizeof(gbk_row_0xc6_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc7, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc7_converted_to_utf8, sizeof(gbk_row_0xc7_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc8, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc8_converted_to_utf8, sizeof(gbk_row_0xc8_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xc9, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc9_converted_to_utf8, sizeof(gbk_row_0xc9_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xca, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xca_converted_to_utf8, sizeof(gbk_row_0xca_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xcb, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xcb_converted_to_utf8, sizeof(gbk_row_0xcb_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xcc, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xcc_converted_to_utf8, sizeof(gbk_row_0xcc_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xcd, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xcd_converted_to_utf8, sizeof(gbk_row_0xcd_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xce, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xce_converted_to_utf8, sizeof(gbk_row_0xce_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xcf, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xcf_converted_to_utf8, sizeof(gbk_row_0xcf_converted_to_utf8));

  convert_euc_to_utf8("GB18030", 0xd0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd0_converted_to_utf8, sizeof(gbk_row_0xd0_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xd1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd1_converted_to_utf8, sizeof(gbk_row_0xd1_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xd2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd2_converted_to_utf8, sizeof(gbk_row_0xd2_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xd3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd3_converted_to_utf8, sizeof(gbk_row_0xd3_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xd4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd4_converted_to_utf8, sizeof(gbk_row_0xd4_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xd5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd5_converted_to_utf8, sizeof(gbk_row_0xd5_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xd6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd6_converted_to_utf8, sizeof(gbk_row_0xd6_converted_to_utf8));

  // end of level 1 chinese characters
  convert_euc_to_utf8("GB18030", 0xd7, gbk_row_0xd7, sizeof(gbk_row_0xd7), gbk_row_0xd7_converted_to_utf8, sizeof(gbk_row_0xd7_converted_to_utf8));

  // level 2 chinese characters
  convert_euc_to_utf8("GB18030", 0xd8, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd8_converted_to_utf8, sizeof(gbk_row_0xd8_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xd9, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd9_converted_to_utf8, sizeof(gbk_row_0xd9_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xda, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xda_converted_to_utf8, sizeof(gbk_row_0xda_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xdb, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xdb_converted_to_utf8, sizeof(gbk_row_0xdb_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xdc, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xdc_converted_to_utf8, sizeof(gbk_row_0xdc_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xdd, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xdd_converted_to_utf8, sizeof(gbk_row_0xdd_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xde, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xde_converted_to_utf8, sizeof(gbk_row_0xde_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xdf, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xdf_converted_to_utf8, sizeof(gbk_row_0xdf_converted_to_utf8));

  convert_euc_to_utf8("GB18030", 0xe0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe0_converted_to_utf8, sizeof(gbk_row_0xe0_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe1_converted_to_utf8, sizeof(gbk_row_0xe1_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe2_converted_to_utf8, sizeof(gbk_row_0xe2_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe3_converted_to_utf8, sizeof(gbk_row_0xe3_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe4_converted_to_utf8, sizeof(gbk_row_0xe4_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe5_converted_to_utf8, sizeof(gbk_row_0xe5_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe6_converted_to_utf8, sizeof(gbk_row_0xe6_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe7, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe7_converted_to_utf8, sizeof(gbk_row_0xe7_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe8, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe8_converted_to_utf8, sizeof(gbk_row_0xe8_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xe9, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe9_converted_to_utf8, sizeof(gbk_row_0xe9_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xea, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xea_converted_to_utf8, sizeof(gbk_row_0xea_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xeb, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xeb_converted_to_utf8, sizeof(gbk_row_0xeb_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xec, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xec_converted_to_utf8, sizeof(gbk_row_0xec_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xed, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xed_converted_to_utf8, sizeof(gbk_row_0xed_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xee, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xee_converted_to_utf8, sizeof(gbk_row_0xee_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xef, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xef_converted_to_utf8, sizeof(gbk_row_0xef_converted_to_utf8));

  convert_euc_to_utf8("GB18030", 0xf0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf0_converted_to_utf8, sizeof(gbk_row_0xf0_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xf1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf1_converted_to_utf8, sizeof(gbk_row_0xf1_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xf2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf2_converted_to_utf8, sizeof(gbk_row_0xf2_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xf3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf3_converted_to_utf8, sizeof(gbk_row_0xf3_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xf4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf4_converted_to_utf8, sizeof(gbk_row_0xf4_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xf5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf5_converted_to_utf8, sizeof(gbk_row_0xf5_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xf6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf6_converted_to_utf8, sizeof(gbk_row_0xf6_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xf7, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf7_converted_to_utf8, sizeof(gbk_row_0xf7_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xf8, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xf8_converted_to_utf8, sizeof(gbk_row_0xf8_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xf9, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xf9_converted_to_utf8, sizeof(gbk_row_0xf9_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xfa, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xfa_converted_to_utf8, sizeof(gbk_row_0xfa_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xfb, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xfb_converted_to_utf8, sizeof(gbk_row_0xfb_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xfc, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xfc_converted_to_utf8, sizeof(gbk_row_0xfc_converted_to_utf8));
  convert_euc_to_utf8("GB18030", 0xfd, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xfd_converted_to_utf8, sizeof(gbk_row_0xfd_converted_to_utf8));

  // end of level 2 chinese characters
  convert_euc_to_utf8("GB18030", 0xfe, gbk_row_0xfe, sizeof(gbk_row_0xfe), gbk_row_0xfe_converted_to_utf8, sizeof(gbk_row_0xfe_converted_to_utf8));
}

OFTEST(oficonv_utf8_to_gb18030_gbk_subset)
{
  // The following rows are new in GBK and did not exist in GB2312.
  convert_utf8_to_euc("GB18030", 0x81, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x81_converted_to_utf8, sizeof(gbk_row_0x81_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x82, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x82_converted_to_utf8, sizeof(gbk_row_0x82_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x83, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x83_converted_to_utf8, sizeof(gbk_row_0x83_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x84, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x84_converted_to_utf8, sizeof(gbk_row_0x84_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x85, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x85_converted_to_utf8, sizeof(gbk_row_0x85_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x86, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x86_converted_to_utf8, sizeof(gbk_row_0x86_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x87, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x87_converted_to_utf8, sizeof(gbk_row_0x87_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x88, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x88_converted_to_utf8, sizeof(gbk_row_0x88_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x89, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x89_converted_to_utf8, sizeof(gbk_row_0x89_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x8a, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8a_converted_to_utf8, sizeof(gbk_row_0x8a_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x8b, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8b_converted_to_utf8, sizeof(gbk_row_0x8b_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x8c, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8c_converted_to_utf8, sizeof(gbk_row_0x8c_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x8d, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8d_converted_to_utf8, sizeof(gbk_row_0x8d_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x8e, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8e_converted_to_utf8, sizeof(gbk_row_0x8e_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x8f, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x8f_converted_to_utf8, sizeof(gbk_row_0x8f_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x90, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x90_converted_to_utf8, sizeof(gbk_row_0x90_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x91, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x91_converted_to_utf8, sizeof(gbk_row_0x91_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x92, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x92_converted_to_utf8, sizeof(gbk_row_0x92_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x93, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x93_converted_to_utf8, sizeof(gbk_row_0x93_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x94, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x94_converted_to_utf8, sizeof(gbk_row_0x94_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x95, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x95_converted_to_utf8, sizeof(gbk_row_0x95_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x96, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x96_converted_to_utf8, sizeof(gbk_row_0x96_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x97, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x97_converted_to_utf8, sizeof(gbk_row_0x97_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x98, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x98_converted_to_utf8, sizeof(gbk_row_0x98_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x99, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x99_converted_to_utf8, sizeof(gbk_row_0x99_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x9a, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9a_converted_to_utf8, sizeof(gbk_row_0x9a_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x9b, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9b_converted_to_utf8, sizeof(gbk_row_0x9b_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x9c, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9c_converted_to_utf8, sizeof(gbk_row_0x9c_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x9d, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9d_converted_to_utf8, sizeof(gbk_row_0x9d_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x9e, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9e_converted_to_utf8, sizeof(gbk_row_0x9e_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0x9f, gbk_full_row, sizeof(gbk_full_row), gbk_row_0x9f_converted_to_utf8, sizeof(gbk_row_0x9f_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xa0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xa0_converted_to_utf8, sizeof(gbk_row_0xa0_converted_to_utf8));

  // These rows are identical to GB2312 and do not use the byte range 0x40-0x7f.
  convert_utf8_to_euc("GB18030", 0xa1, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_01_converted_to_utf8_alternate, sizeof(gb2312_row_01_converted_to_utf8_alternate));
  convert_utf8_to_euc("GB18030", 0xa2, gb2312_row_02, sizeof(gb2312_row_02),     gb2312_row_02_converted_to_utf8, sizeof(gb2312_row_02_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xa3, gb2312_row_16ff, sizeof(gb2312_row_16ff), gb2312_row_03_converted_to_utf8, sizeof(gb2312_row_03_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xa4, gb2312_row_04, sizeof(gb2312_row_04),     gb2312_row_04_converted_to_utf8, sizeof(gb2312_row_04_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xa5, gb2312_row_05, sizeof(gb2312_row_05),     gb2312_row_05_converted_to_utf8, sizeof(gb2312_row_05_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xa6, gb2312_row_06, sizeof(gb2312_row_06),     gb2312_row_06_converted_to_utf8, sizeof(gb2312_row_06_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xa7, gb2312_row_07, sizeof(gb2312_row_07),     gb2312_row_07_converted_to_utf8, sizeof(gb2312_row_07_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xa8, gb2312_row_08, sizeof(gb2312_row_08),     gb2312_row_08_converted_to_utf8, sizeof(gb2312_row_08_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xa9, gb2312_row_09, sizeof(gb2312_row_09),     gb2312_row_09_converted_to_utf8, sizeof(gb2312_row_09_converted_to_utf8));

  // The following rows do not use the full set of characters
  convert_utf8_to_euc("GB18030", 0xaa, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xaa_converted_to_utf8, sizeof(gbk_row_0xaa_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xab, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xab_converted_to_utf8, sizeof(gbk_row_0xab_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xac, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xac_converted_to_utf8, sizeof(gbk_row_0xac_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xad, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xad_converted_to_utf8, sizeof(gbk_row_0xad_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xae, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xae_converted_to_utf8, sizeof(gbk_row_0xae_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xaf, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xaf_converted_to_utf8, sizeof(gbk_row_0xaf_converted_to_utf8));

  // level 1 chinese characters
  convert_utf8_to_euc("GB18030", 0xb0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb0_converted_to_utf8, sizeof(gbk_row_0xb0_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb1_converted_to_utf8, sizeof(gbk_row_0xb1_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb2_converted_to_utf8, sizeof(gbk_row_0xb2_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb3_converted_to_utf8, sizeof(gbk_row_0xb3_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb4_converted_to_utf8, sizeof(gbk_row_0xb4_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb5_converted_to_utf8, sizeof(gbk_row_0xb5_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb6_converted_to_utf8, sizeof(gbk_row_0xb6_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb7, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb7_converted_to_utf8, sizeof(gbk_row_0xb7_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb8, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb8_converted_to_utf8, sizeof(gbk_row_0xb8_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xb9, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xb9_converted_to_utf8, sizeof(gbk_row_0xb9_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xba, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xba_converted_to_utf8, sizeof(gbk_row_0xba_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xbb, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbb_converted_to_utf8, sizeof(gbk_row_0xbb_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xbc, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbc_converted_to_utf8, sizeof(gbk_row_0xbc_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xbd, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbd_converted_to_utf8, sizeof(gbk_row_0xbd_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xbe, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbe_converted_to_utf8, sizeof(gbk_row_0xbe_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xbf, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xbf_converted_to_utf8, sizeof(gbk_row_0xbf_converted_to_utf8));

  convert_utf8_to_euc("GB18030", 0xc0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc0_converted_to_utf8, sizeof(gbk_row_0xc0_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc1_converted_to_utf8, sizeof(gbk_row_0xc1_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc2_converted_to_utf8, sizeof(gbk_row_0xc2_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc3_converted_to_utf8, sizeof(gbk_row_0xc3_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc4_converted_to_utf8, sizeof(gbk_row_0xc4_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc5_converted_to_utf8, sizeof(gbk_row_0xc5_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc6_converted_to_utf8, sizeof(gbk_row_0xc6_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc7, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc7_converted_to_utf8, sizeof(gbk_row_0xc7_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc8, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc8_converted_to_utf8, sizeof(gbk_row_0xc8_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xc9, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xc9_converted_to_utf8, sizeof(gbk_row_0xc9_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xca, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xca_converted_to_utf8, sizeof(gbk_row_0xca_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xcb, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xcb_converted_to_utf8, sizeof(gbk_row_0xcb_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xcc, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xcc_converted_to_utf8, sizeof(gbk_row_0xcc_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xcd, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xcd_converted_to_utf8, sizeof(gbk_row_0xcd_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xce, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xce_converted_to_utf8, sizeof(gbk_row_0xce_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xcf, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xcf_converted_to_utf8, sizeof(gbk_row_0xcf_converted_to_utf8));

  convert_utf8_to_euc("GB18030", 0xd0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd0_converted_to_utf8, sizeof(gbk_row_0xd0_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xd1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd1_converted_to_utf8, sizeof(gbk_row_0xd1_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xd2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd2_converted_to_utf8, sizeof(gbk_row_0xd2_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xd3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd3_converted_to_utf8, sizeof(gbk_row_0xd3_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xd4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd4_converted_to_utf8, sizeof(gbk_row_0xd4_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xd5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd5_converted_to_utf8, sizeof(gbk_row_0xd5_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xd6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd6_converted_to_utf8, sizeof(gbk_row_0xd6_converted_to_utf8));

  // end of level 1 chinese characters
  convert_utf8_to_euc("GB18030", 0xd7, gbk_row_0xd7, sizeof(gbk_row_0xd7), gbk_row_0xd7_converted_to_utf8, sizeof(gbk_row_0xd7_converted_to_utf8));

  // level 2 chinese characters
  convert_utf8_to_euc("GB18030", 0xd8, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd8_converted_to_utf8, sizeof(gbk_row_0xd8_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xd9, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xd9_converted_to_utf8, sizeof(gbk_row_0xd9_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xda, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xda_converted_to_utf8, sizeof(gbk_row_0xda_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xdb, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xdb_converted_to_utf8, sizeof(gbk_row_0xdb_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xdc, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xdc_converted_to_utf8, sizeof(gbk_row_0xdc_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xdd, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xdd_converted_to_utf8, sizeof(gbk_row_0xdd_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xde, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xde_converted_to_utf8, sizeof(gbk_row_0xde_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xdf, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xdf_converted_to_utf8, sizeof(gbk_row_0xdf_converted_to_utf8));

  convert_utf8_to_euc("GB18030", 0xe0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe0_converted_to_utf8, sizeof(gbk_row_0xe0_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe1_converted_to_utf8, sizeof(gbk_row_0xe1_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe2_converted_to_utf8, sizeof(gbk_row_0xe2_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe3_converted_to_utf8, sizeof(gbk_row_0xe3_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe4_converted_to_utf8, sizeof(gbk_row_0xe4_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe5_converted_to_utf8, sizeof(gbk_row_0xe5_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe6_converted_to_utf8, sizeof(gbk_row_0xe6_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe7, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe7_converted_to_utf8, sizeof(gbk_row_0xe7_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe8, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe8_converted_to_utf8, sizeof(gbk_row_0xe8_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xe9, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xe9_converted_to_utf8, sizeof(gbk_row_0xe9_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xea, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xea_converted_to_utf8, sizeof(gbk_row_0xea_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xeb, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xeb_converted_to_utf8, sizeof(gbk_row_0xeb_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xec, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xec_converted_to_utf8, sizeof(gbk_row_0xec_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xed, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xed_converted_to_utf8, sizeof(gbk_row_0xed_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xee, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xee_converted_to_utf8, sizeof(gbk_row_0xee_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xef, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xef_converted_to_utf8, sizeof(gbk_row_0xef_converted_to_utf8));

  convert_utf8_to_euc("GB18030", 0xf0, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf0_converted_to_utf8, sizeof(gbk_row_0xf0_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xf1, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf1_converted_to_utf8, sizeof(gbk_row_0xf1_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xf2, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf2_converted_to_utf8, sizeof(gbk_row_0xf2_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xf3, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf3_converted_to_utf8, sizeof(gbk_row_0xf3_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xf4, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf4_converted_to_utf8, sizeof(gbk_row_0xf4_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xf5, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf5_converted_to_utf8, sizeof(gbk_row_0xf5_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xf6, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf6_converted_to_utf8, sizeof(gbk_row_0xf6_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xf7, gbk_full_row, sizeof(gbk_full_row), gbk_row_0xf7_converted_to_utf8, sizeof(gbk_row_0xf7_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xf8, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xf8_converted_to_utf8, sizeof(gbk_row_0xf8_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xf9, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xf9_converted_to_utf8, sizeof(gbk_row_0xf9_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xfa, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xfa_converted_to_utf8, sizeof(gbk_row_0xfa_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xfb, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xfb_converted_to_utf8, sizeof(gbk_row_0xfb_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xfc, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xfc_converted_to_utf8, sizeof(gbk_row_0xfc_converted_to_utf8));
  convert_utf8_to_euc("GB18030", 0xfd, gbk_row_0xaa_to_0xaf, sizeof(gbk_row_0xaa_to_0xaf), gbk_row_0xfd_converted_to_utf8, sizeof(gbk_row_0xfd_converted_to_utf8));

  // end of level 2 chinese characters
  // the mapping of several characters is not reversible and seems fishy:
  // characters in the range 0x50-7e are mapped to 0xd0-fe when converting back from UTF-8, i.e. an offset of 0x80 is added
  convert_utf8_to_euc("GB18030", 0xfe, gbk_row_0xfe, sizeof(gbk_row_0xfe), gbk_row_0xfe_converted_to_utf8, sizeof(gbk_row_0xfe_converted_to_utf8));

}

OFTEST(oficonv_gb18030_to_utf8)
{
  convert_to_utf8("GB18030", gb18030_latin_with_accents, sizeof(gb18030_latin_with_accents),       gb18030_latin_converted_to_utf8, sizeof(gb18030_latin_converted_to_utf8));
  convert_to_utf8("GB18030", gb18030_cyrillic_with_accents, sizeof(gb18030_cyrillic_with_accents), gb18030_cyrillic_converted_to_utf8, sizeof(gb18030_cyrillic_converted_to_utf8));
  convert_to_utf8("GB18030", gb18030_armenian, sizeof(gb18030_armenian),                           gb18030_armenian_converted_to_utf8, sizeof(gb18030_armenian_converted_to_utf8));
  convert_to_utf8("GB18030", gb18030_devanagari, sizeof(gb18030_devanagari),                       gb18030_devanagari_converted_to_utf8, sizeof(gb18030_devanagari_converted_to_utf8));
  convert_to_utf8("GB18030", gb18030_ethiopic, sizeof(gb18030_ethiopic),                           gb18030_ethiopic_converted_to_utf8, sizeof(gb18030_ethiopic_converted_to_utf8));
  convert_to_utf8("GB18030", gb18030_braille, sizeof(gb18030_braille),                             gb18030_braille_converted_to_utf8, sizeof(gb18030_braille_converted_to_utf8));
  convert_to_utf8("GB18030", gb18030_chinese, sizeof(gb18030_chinese),                             gb18030_chinese_converted_to_utf8, sizeof(gb18030_chinese_converted_to_utf8));
}

OFTEST(oficonv_utf8_to_gb18030)
{
  convert_from_utf8("GB18030", gb18030_latin_with_accents, sizeof(gb18030_latin_with_accents),       gb18030_latin_converted_to_utf8, sizeof(gb18030_latin_converted_to_utf8));
  convert_from_utf8("GB18030", gb18030_cyrillic_with_accents, sizeof(gb18030_cyrillic_with_accents), gb18030_cyrillic_converted_to_utf8, sizeof(gb18030_cyrillic_converted_to_utf8));
  convert_from_utf8("GB18030", gb18030_armenian, sizeof(gb18030_armenian),                           gb18030_armenian_converted_to_utf8, sizeof(gb18030_armenian_converted_to_utf8));
  convert_from_utf8("GB18030", gb18030_devanagari, sizeof(gb18030_devanagari),                       gb18030_devanagari_converted_to_utf8, sizeof(gb18030_devanagari_converted_to_utf8));
  convert_from_utf8("GB18030", gb18030_ethiopic, sizeof(gb18030_ethiopic),                           gb18030_ethiopic_converted_to_utf8, sizeof(gb18030_ethiopic_converted_to_utf8));
  convert_from_utf8("GB18030", gb18030_braille, sizeof(gb18030_braille),                             gb18030_braille_converted_to_utf8, sizeof(gb18030_braille_converted_to_utf8));
  convert_from_utf8("GB18030", gb18030_chinese, sizeof(gb18030_chinese),                             gb18030_chinese_converted_to_utf8, sizeof(gb18030_chinese_converted_to_utf8));
}

