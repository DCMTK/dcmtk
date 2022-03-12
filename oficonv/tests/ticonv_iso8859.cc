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
 *  Purpose: oficonv unit tests for ISO-8859 character sets
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/oficonv/iconv.h"
#include "oficonv_tests.h"
#include "tables_iso8859.h"

// Conversion from ISO_IR 100 (Latin 1) to ISO_IR 192 (Unicode)
OFTEST(oficonv_iso8859_1_to_utf8)
{
  convert_iso8859_to_utf8("ISO-8859-1", char_a0_to_ff, sizeof(char_a0_to_ff), iso8859_1_to_utf8_a0_to_ff, sizeof(iso8859_1_to_utf8_a0_to_ff));
}

// Conversion from ISO_IR 101 (Latin 2) to ISO_IR 192 (Unicode)
OFTEST(oficonv_iso8859_2_to_utf8)
{
  convert_iso8859_to_utf8("ISO-8859-2", char_a0_to_ff, sizeof(char_a0_to_ff), iso8859_2_to_utf8_a0_to_ff, sizeof(iso8859_2_to_utf8_a0_to_ff));
}

// Conversion from ISO_IR 109 (Latin 3) to ISO_IR 192 (Unicode)
OFTEST(oficonv_iso8859_3_to_utf8)
{
  convert_iso8859_to_utf8("ISO-8859-3", iso8859_3_char_a0_to_ff, sizeof(iso8859_3_char_a0_to_ff), iso8859_3_to_utf8_a0_to_ff, sizeof(iso8859_3_to_utf8_a0_to_ff));
}

// Conversion from ISO_IR 110 (Latin 4) to ISO_IR 192 (Unicode)
OFTEST(oficonv_iso8859_4_to_utf8)
{
  convert_iso8859_to_utf8("ISO-8859-4", char_a0_to_ff, sizeof(char_a0_to_ff), iso8859_4_to_utf8_a0_to_ff, sizeof(iso8859_4_to_utf8_a0_to_ff));
}

// Conversion from ISO_IR 144 (Cyrillic) to ISO_IR 192 (Unicode)
OFTEST(oficonv_iso8859_5_to_utf8)
{
  convert_iso8859_to_utf8("ISO-8859-5", char_a0_to_ff, sizeof(char_a0_to_ff), iso8859_5_to_utf8_a0_to_ff, sizeof(iso8859_5_to_utf8_a0_to_ff));
}

// Conversion from ISO_IR 127 (Arabic) to ISO_IR 192 (Unicode)
OFTEST(oficonv_iso8859_6_to_utf8)
{
  convert_iso8859_to_utf8("ISO-8859-6", iso8859_6_char_a0_to_ff, sizeof(iso8859_6_char_a0_to_ff), iso8859_6_to_utf8_a0_to_ff, sizeof(iso8859_6_to_utf8_a0_to_ff));
}

// Conversion from ISO_IR 126 (Greek) to ISO_IR 192 (Unicode)
OFTEST(oficonv_iso8859_7_to_utf8)
{
  convert_iso8859_to_utf8("ISO-8859-7", iso8859_7_char_a0_to_ff, sizeof(iso8859_7_char_a0_to_ff), iso8859_7_to_utf8_a0_to_ff, sizeof(iso8859_7_to_utf8_a0_to_ff));
}

// Conversion from ISO_IR 138 (Hebrew) to ISO_IR 192 (Unicode)
OFTEST(oficonv_iso8859_8_to_utf8)
{
  convert_iso8859_to_utf8("ISO-8859-8", iso8859_8_char_a0_to_ff, sizeof(iso8859_8_char_a0_to_ff), iso8859_8_to_utf8_a0_to_ff, sizeof(iso8859_8_to_utf8_a0_to_ff));
}

// Conversion from ISO_IR 148 (Latin 5) to ISO_IR 192 (Unicode)
OFTEST(oficonv_iso8859_9_to_utf8)
{
  convert_iso8859_to_utf8("ISO-8859-9", char_a0_to_ff, sizeof(char_a0_to_ff), iso8859_9_to_utf8_a0_to_ff, sizeof(iso8859_9_to_utf8_a0_to_ff));
}

// Conversion from ISO_IR 166 (Thai) to ISO_IR 192 (Unicode)
OFTEST(oficonv_iso8859_11_to_utf8)
{
  convert_iso8859_to_utf8("ISO-IR-166", iso8859_11_char_a0_to_ff, sizeof(iso8859_11_char_a0_to_ff), iso8859_11_to_utf8_a0_to_ff, sizeof(iso8859_11_to_utf8_a0_to_ff));
}

// Conversion from ISO_IR 203 (Latin-9) to ISO_IR 192 (Unicode)
OFTEST(oficonv_iso8859_15_to_utf8)
{
  convert_iso8859_to_utf8("ISO-8859-15", char_a0_to_ff, sizeof(char_a0_to_ff), iso8859_15_to_utf8_a0_to_ff, sizeof(iso8859_15_to_utf8_a0_to_ff));
}

// Conversion from TIS-620 (Thai) to ISO_IR 192 (Unicode)
// For all pratical purposes, TIS-620 is identical to ISO-IR-166 (ISO 8859-11)
OFTEST(oficonv_tis_620_to_utf8)
{
  convert_iso8859_to_utf8("TIS-620", iso8859_11_char_a0_to_ff, sizeof(iso8859_11_char_a0_to_ff), iso8859_11_to_utf8_a0_to_ff, sizeof(iso8859_11_to_utf8_a0_to_ff));
}

OFTEST(oficonv_utf8_to_iso8859_1)
{
  convert_utf8_to_iso8859("ISO-8859-1", char_a0_to_ff, sizeof(char_a0_to_ff), iso8859_1_to_utf8_a0_to_ff, sizeof(iso8859_1_to_utf8_a0_to_ff));
}

OFTEST(oficonv_utf8_to_iso8859_2)
{
  convert_utf8_to_iso8859("ISO-8859-2", char_a0_to_ff, sizeof(char_a0_to_ff), iso8859_2_to_utf8_a0_to_ff, sizeof(iso8859_2_to_utf8_a0_to_ff));
}

OFTEST(oficonv_utf8_to_iso8859_3)
{
  convert_utf8_to_iso8859("ISO-8859-3", iso8859_3_char_a0_to_ff, sizeof(iso8859_3_char_a0_to_ff), iso8859_3_to_utf8_a0_to_ff, sizeof(iso8859_3_to_utf8_a0_to_ff));
}

OFTEST(oficonv_utf8_to_iso8859_4)
{
  convert_utf8_to_iso8859("ISO-8859-4", char_a0_to_ff, sizeof(char_a0_to_ff), iso8859_4_to_utf8_a0_to_ff, sizeof(iso8859_4_to_utf8_a0_to_ff));
}

OFTEST(oficonv_utf8_to_iso8859_5)
{
  convert_utf8_to_iso8859("ISO-8859-5", char_a0_to_ff, sizeof(char_a0_to_ff), iso8859_5_to_utf8_a0_to_ff, sizeof(iso8859_5_to_utf8_a0_to_ff));
}

OFTEST(oficonv_utf8_to_iso8859_6)
{
  convert_utf8_to_iso8859("ISO-8859-6", iso8859_6_char_a0_to_ff, sizeof(iso8859_6_char_a0_to_ff), iso8859_6_to_utf8_a0_to_ff, sizeof(iso8859_6_to_utf8_a0_to_ff));
}

OFTEST(oficonv_utf8_to_iso8859_7)
{
  convert_utf8_to_iso8859("ISO-8859-7", iso8859_7_char_a0_to_ff, sizeof(iso8859_7_char_a0_to_ff), iso8859_7_to_utf8_a0_to_ff, sizeof(iso8859_7_to_utf8_a0_to_ff));
}

OFTEST(oficonv_utf8_to_iso8859_8)
{
  convert_utf8_to_iso8859("ISO-8859-8", iso8859_8_char_a0_to_ff, sizeof(iso8859_8_char_a0_to_ff), iso8859_8_to_utf8_a0_to_ff, sizeof(iso8859_8_to_utf8_a0_to_ff));
}

OFTEST(oficonv_utf8_to_iso8859_9)
{
  convert_utf8_to_iso8859("ISO-8859-9", char_a0_to_ff, sizeof(char_a0_to_ff), iso8859_9_to_utf8_a0_to_ff, sizeof(iso8859_9_to_utf8_a0_to_ff));
}

OFTEST(oficonv_utf8_to_iso8859_11)
{
  convert_utf8_to_iso8859("ISO-IR-166", iso8859_11_char_a0_to_ff, sizeof(iso8859_11_char_a0_to_ff), iso8859_11_to_utf8_a0_to_ff, sizeof(iso8859_11_to_utf8_a0_to_ff));
}

OFTEST(oficonv_utf8_to_iso8859_15)
{
  convert_utf8_to_iso8859("ISO-8859-15", char_a0_to_ff, sizeof(char_a0_to_ff), iso8859_15_to_utf8_a0_to_ff, sizeof(iso8859_15_to_utf8_a0_to_ff));
}

OFTEST(oficonv_utf8_to_tis_620)
{
  convert_utf8_to_iso8859("TIS-620", iso8859_11_char_a0_to_ff, sizeof(iso8859_11_char_a0_to_ff), iso8859_11_to_utf8_a0_to_ff, sizeof(iso8859_11_to_utf8_a0_to_ff));
}

OFTEST(oficonv_iso8859_to_ascii)
{
  // the transliteration function in oficonv is rather stupid and will simply map all extended
  // characters to '?' when converting to ASCII. We can at least check that this works for all ISO 8859 charsets supported
  convert_iso8859_to_ascii("ISO-8859-1", char_a0_to_ff, sizeof(char_a0_to_ff));
  convert_iso8859_to_ascii("ISO-8859-2", char_a0_to_ff, sizeof(char_a0_to_ff));
  convert_iso8859_to_ascii("ISO-8859-3", iso8859_3_char_a0_to_ff, sizeof(iso8859_3_char_a0_to_ff));
  convert_iso8859_to_ascii("ISO-8859-4", char_a0_to_ff, sizeof(char_a0_to_ff));
  convert_iso8859_to_ascii("ISO-8859-5", char_a0_to_ff, sizeof(char_a0_to_ff));
  convert_iso8859_to_ascii("ISO-8859-6", iso8859_6_char_a0_to_ff, sizeof(iso8859_6_char_a0_to_ff));
  convert_iso8859_to_ascii("ISO-8859-7", iso8859_7_char_a0_to_ff, sizeof(iso8859_7_char_a0_to_ff));
  convert_iso8859_to_ascii("ISO-8859-8", iso8859_8_char_a0_to_ff, sizeof(iso8859_8_char_a0_to_ff));
  convert_iso8859_to_ascii("ISO-8859-9", char_a0_to_ff, sizeof(char_a0_to_ff));
  convert_iso8859_to_ascii("ISO-8859-11", iso8859_11_char_a0_to_ff, sizeof(iso8859_11_char_a0_to_ff));
}

OFTEST(oficonv_iso8859_to_iso8859)
{
  convert_iso8859_to_iso8859("ISO-8859-1", char_a0_to_ff, sizeof(char_a0_to_ff), "ISO-8859-2", iso8859_1_to_iso8859_2, sizeof(iso8859_1_to_iso8859_2));
  convert_iso8859_to_iso8859("ISO-8859-1", char_a0_to_ff, sizeof(char_a0_to_ff), "ISO-8859-3", iso8859_1_to_iso8859_3, sizeof(iso8859_1_to_iso8859_3));
  convert_iso8859_to_iso8859("ISO-8859-1", char_a0_to_ff, sizeof(char_a0_to_ff), "ISO-8859-4", iso8859_1_to_iso8859_4, sizeof(iso8859_1_to_iso8859_4));
  convert_iso8859_to_iso8859("ISO-8859-1", char_a0_to_ff, sizeof(char_a0_to_ff), "ISO-8859-5", iso8859_1_to_iso8859_5, sizeof(iso8859_1_to_iso8859_5));
  convert_iso8859_to_iso8859("ISO-8859-1", char_a0_to_ff, sizeof(char_a0_to_ff), "ISO-8859-9", iso8859_1_to_iso8859_9, sizeof(iso8859_1_to_iso8859_9));
}

// this test uses ISO-8859-3 and thus belongs into this implementation file
OFTEST(oficonv_open_into)
{
  iconv_t id;
  iconv_allocation_t space;

  int result = OFiconv_open_into("ISO-8859-3", "UTF-8", &space);
  OFCHECK(-1 != result);

  if (-1 != result)
  {
    // cast the iconv_allocation_t block to iconv_t, which is a pointer to struct
    id = (iconv_t) (&space);

    // convert the characters below 128 to UTF-8. The output should be identical to the input.
    do_conversion(id, char_20_to_7e, sizeof(char_20_to_7e), char_20_to_7e, sizeof(char_20_to_7e));

    // convert the characters above 128 to UTF-8 and compare against a reference
    do_conversion(id, iso8859_3_to_utf8_a0_to_ff, sizeof(iso8859_3_to_utf8_a0_to_ff), iso8859_3_char_a0_to_ff, sizeof(iso8859_3_char_a0_to_ff));

    // close the conversion descriptor
    result = OFiconv_close_in(&space);
    OFCHECK(-1 != result);
  }
}
