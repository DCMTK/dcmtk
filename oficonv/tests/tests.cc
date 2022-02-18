/*
 *
 *  Copyright (C) 2011-2022, OFFIS e.V.
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
OFTEST_REGISTER(oficonv_tis_620_to_utf8);
OFTEST_REGISTER(oficonv_shift_jis_to_utf8);

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
OFTEST_REGISTER(oficonv_utf8_to_tis_620);
OFTEST_REGISTER(oficonv_utf8_to_shift_jis);

// conversion of the supported single-byte character sets to ASCII
OFTEST_REGISTER(oficonv_iso8859_to_ascii);

// conversion of EUR-KR (KS X 1001) to UTF-8 and back
OFTEST_REGISTER(oficonv_eur_kr_to_utf8);
OFTEST_REGISTER(oficonv_utf8_to_eur_kr);


OFTEST_MAIN("oficonv")
