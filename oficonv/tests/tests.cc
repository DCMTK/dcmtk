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
 *  Module:  oficonv
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: main test program
 *
 */

#include "dcmtk/config/osconfig.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"

OFTEST_REGISTER(oficonv_open);

// conversion of the supported single-byte character sets to UTF-8
OFTEST_REGISTER(oficonv_iso8859_1_to_utf8);
OFTEST_REGISTER(oficonv_iso8859_2_to_utf8);
OFTEST_REGISTER(oficonv_iso8859_3_to_utf8);
OFTEST_REGISTER(oficonv_iso8859_4_to_utf8);
OFTEST_REGISTER(oficonv_iso8859_5_to_utf8);
OFTEST_REGISTER(oficonv_iso8859_6_to_utf8);
OFTEST_REGISTER(oficonv_iso8859_7_to_utf8);
OFTEST_REGISTER(oficonv_iso8859_8_to_utf8);
OFTEST_REGISTER(oficonv_iso8859_9_to_utf8);
OFTEST_REGISTER(oficonv_iso8859_11_to_utf8);
OFTEST_REGISTER(oficonv_iso8859_15_to_utf8);
OFTEST_REGISTER(oficonv_tis_620_to_utf8);
OFTEST_REGISTER(oficonv_shift_jis_to_utf8);
OFTEST_REGISTER(oficonv_jis_x0201_to_ascii);

// conversion of UTF-8 to the supported single-byte character sets, only using characters that can be mapped
OFTEST_REGISTER(oficonv_utf8_to_iso8859_1);
OFTEST_REGISTER(oficonv_utf8_to_iso8859_2);
OFTEST_REGISTER(oficonv_utf8_to_iso8859_3);
OFTEST_REGISTER(oficonv_utf8_to_iso8859_4);
OFTEST_REGISTER(oficonv_utf8_to_iso8859_5);
OFTEST_REGISTER(oficonv_utf8_to_iso8859_6);
OFTEST_REGISTER(oficonv_utf8_to_iso8859_7);
OFTEST_REGISTER(oficonv_utf8_to_iso8859_8);
OFTEST_REGISTER(oficonv_utf8_to_iso8859_9);
OFTEST_REGISTER(oficonv_utf8_to_iso8859_11);
OFTEST_REGISTER(oficonv_utf8_to_iso8859_15);
OFTEST_REGISTER(oficonv_utf8_to_tis_620);
OFTEST_REGISTER(oficonv_utf8_to_shift_jis);

// conversion of the supported single-byte character sets to ASCII
OFTEST_REGISTER(oficonv_iso8859_to_ascii);

// conversion between different ISO 8859 character sets
OFTEST_REGISTER(oficonv_iso8859_to_iso8859);

// conversion of EUR-KR (KS X 1001) to UTF-8 and back
OFTEST_REGISTER(oficonv_euc_kr_to_utf8);
OFTEST_REGISTER(oficonv_utf8_to_euc_kr);
OFTEST_REGISTER(oficonv_ascii_to_euc_kr);

// conversion of EUC-CN (GB2312) to UTF-8 and back
OFTEST_REGISTER(oficonv_gb2312_to_utf8);
OFTEST_REGISTER(oficonv_utf8_to_gb2312);

// conversion of the GB2312 subset of GBK to UTF-8 and back
OFTEST_REGISTER(oficonv_gbk_2312_subset_to_utf8);
OFTEST_REGISTER(oficonv_utf8_to_gbk_2312_subset);

// conversion of the full GBK character set to UTF-8 and back
OFTEST_REGISTER(oficonv_gbk_to_utf8);
OFTEST_REGISTER(oficonv_utf8_to_gbk);

// conversion of the GB2312 subset of GB18030 to UTF-8 and back
OFTEST_REGISTER(oficonv_gb18030_2312_subset_to_utf8);
OFTEST_REGISTER(oficonv_utf8_to_gb18030_2312_subset);

// conversion of the GBK subset of GB18030 to UTF-8 and back
OFTEST_REGISTER(oficonv_gb18030_gbk_subset_to_utf8);
OFTEST_REGISTER(oficonv_utf8_to_gb18030_gbk_subset);

// conversion of 4-byte GB18030 sequences to UTF-8 and back
OFTEST_REGISTER(oficonv_gb18030_to_utf8);
OFTEST_REGISTER(oficonv_utf8_to_gb18030);

// conversion of JIS X0208 to UTF-8 and back
OFTEST_REGISTER(oficonv_jis_x0208_to_utf8);
OFTEST_REGISTER(oficonv_utf8_to_jis_x0208);

// conversion of JIS X0208 to UTF-8 and back
OFTEST_REGISTER(oficonv_jis_x0212_to_utf8);
OFTEST_REGISTER(oficonv_utf8_to_jis_x0212);

// test other functions of dcmtk/oficonv/iconv.h
OFTEST_REGISTER(oficonv_canonicalize);
OFTEST_REGISTER(oficonv_get_list);
OFTEST_REGISTER(oficonvlist);
OFTEST_REGISTER(oficonv_open_into);
OFTEST_REGISTER(oficonv__iconv);
OFTEST_REGISTER(oflocale_charset);
OFTEST_REGISTER(oficonvctl);

OFTEST_MAIN("oficonv")
