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
 *  Purpose: General oficonv unit tests
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/oficonv/iconv.h"
#include "oficonv_tests.h"
#include "tables_invalids.h"

// This test only exercises OFiconv_open() for various pairs of
// character sets and checks whether valid conversion contexts are returned.
OFTEST(oficonv_open)
{
#ifdef DEBUG
  // set log level to 1 in order to print debug information about opened iconv data files
  set_oficonv_log_level(1);
#endif

  iconv_t invalid_id = OFreinterpret_cast(iconv_t, -1);
  iconv_t id;

  // ASCII to ASCII
  OFCHECK(invalid_id != (id = OFiconv_open("ASCII", "ASCII")));
  if (id != invalid_id) OFiconv_close(id);

  // To some charset from UTF-8
  OFCHECK(invalid_id != (id = OFiconv_open("ASCII", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-1", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-2", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-3", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-4", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-5", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-6", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-7", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-8", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-9", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("JIS_X0201", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("Shift_JIS", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("TIS-620", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-IR-166", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("GB18030", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("GB2312", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-IR-87", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("JIS_X0212", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("EUC-KR", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("GBK", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  // the following call should fail because "DCMTK" is not a known character set name
  OFCHECK(invalid_id == (id = OFiconv_open("DCMTK", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);

  // To UTF-8 from some charset
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "ASCII")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "ISO-8859-2")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "ISO-8859-3")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "ISO-8859-4")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "ISO-8859-5")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "ISO-8859-6")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "ISO-8859-7")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "ISO-8859-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "ISO-8859-9")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "JIS_X0201")));
  if (id != invalid_id) OFiconv_close(id);
  // Shift_JIS is an extension of JIS_X0201; DICOM uses only the JIS_X0201 characters
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "Shift_JIS")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "TIS-620")));
  if (id != invalid_id) OFiconv_close(id);
  // ISO 8859-11 (ISO_IR 166) is identical to TIS-620 except for the A0 character,
  // which is a non-breaking space in ISO 8859-11 and undefined in TIS-620
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "ISO-IR-166")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "UTF-8")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "GB18030")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "GBK")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "ISO-IR-87")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "JIS_X0212")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", "EUC-KR")));
  if (id != invalid_id) OFiconv_close(id);
  // the following call should fail because "DCMTK" is not a known character set name
  OFCHECK(invalid_id == (id = OFiconv_open("UTF-8", "DCMTK")));
  if (id != invalid_id) OFiconv_close(id);

  // To some charset from Latin-1
  OFCHECK(invalid_id != (id = OFiconv_open("ASCII", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-2", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-3", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-4", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-5", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-6", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-7", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-8", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-9", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("JIS_X0201", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("Shift_JIS", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("TIS-620", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-IR-166", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("GB18030", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("GBK", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("ISO-IR-87", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("JIS_X0212", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("EUC-KR", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  OFCHECK(invalid_id != (id = OFiconv_open("GBK", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
  // the following call should fail because "DCMTK" is not a known character set name
  OFCHECK(invalid_id == (id = OFiconv_open("DCMTK", "ISO-8859-1")));
  if (id != invalid_id) OFiconv_close(id);
}


OFTEST(oficonv_canonicalize)
{
  const char *c;

  // these are the canonical names we expect as result
  OFString latin1("ISO-8859-1");
  OFString euc_cn("EUC-CN");
  OFString euc_kr("EUC-KR");
  OFString latin2("ISO-8859-2");
  OFString latin3("ISO-8859-3");
  OFString latin4("ISO-8859-4");
  OFString cyrillic("ISO-8859-5");
  OFString arabic("ISO-8859-6");
  OFString greek("ISO-8859-7");
  OFString hebrew("ISO-8859-8");
  OFString latin5("ISO-8859-9");
  OFString thai("ISO-8859-11");
  OFString latin9("ISO-8859-15");
  OFString ascii("ISO646-US");
  OFString jisx0201("JISX0201-KANA");
  OFString jisx0208("JISX0208:1990");
  OFString shift_jis("Shift_JIS");
  OFString jisx0212("JISX0212");
  OFString utf8("UTF-8");
  OFString utf16("UTF-16");

  // exercise all synonyms for ISO-8859-1 as defined in oficonv/datasrc/esdb/esdb.alias,
  // with mixed uppercase/lowercase spellings
  c = OFiconv_canonicalize("cp819");            OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("csisolatin1");      OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("ibm819");           OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("iso_8859-1");       OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("iso_8859-1:1987");  OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("iso-ir-100");       OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("iso8859-1");        OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("l1");               OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("latin1");           OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("CP819");            OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("CSISOLATIN1");      OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("IBM819");           OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("ISO_8859-1");       OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("ISO_8859-1:1987");  OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("ISO-IR-100");       OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("ISO8859-1");        OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("L1");               OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("LATIN1");           OFCHECK(c && latin1 == c);
  c = OFiconv_canonicalize("cSiSoLaTiN1");      OFCHECK(c && latin1 == c);

  // exercise at least one synonym for each supported character set
  c = OFiconv_canonicalize("gb2312");           OFCHECK(c && euc_cn == c);
  c = OFiconv_canonicalize("iso-ir-149");       OFCHECK(c && euc_kr == c);
  c = OFiconv_canonicalize("iso-ir-101");       OFCHECK(c && latin2 == c);
  c = OFiconv_canonicalize("latin3");           OFCHECK(c && latin3 == c);
  c = OFiconv_canonicalize("iso_8859-4");       OFCHECK(c && latin4 == c);
  c = OFiconv_canonicalize("cyrillic");         OFCHECK(c && cyrillic == c);
  c = OFiconv_canonicalize("arabic");           OFCHECK(c && arabic == c);
  c = OFiconv_canonicalize("greek");            OFCHECK(c && greek == c);
  c = OFiconv_canonicalize("hebrew");           OFCHECK(c && hebrew == c);
  c = OFiconv_canonicalize("iso-ir-148");       OFCHECK(c && latin5 == c);
  c = OFiconv_canonicalize("tis-620");          OFCHECK(c && thai == c);
  c = OFiconv_canonicalize("iso-ir-203");       OFCHECK(c && latin9 == c);
  c = OFiconv_canonicalize("ascii");            OFCHECK(c && ascii == c);
  c = OFiconv_canonicalize("jis_x0201");        OFCHECK(c && jisx0201 == c);
  c = OFiconv_canonicalize("jis0208");          OFCHECK(c && jisx0208 == c);
  c = OFiconv_canonicalize("ms_kanji");         OFCHECK(c && shift_jis == c);
  c = OFiconv_canonicalize("jis_x0212");        OFCHECK(c && jisx0212 == c);
  c = OFiconv_canonicalize("utf8");             OFCHECK(c && utf8 == c);
  c = OFiconv_canonicalize("unicode");          OFCHECK(c && utf16 == c);
}


OFTEST(oficonv_get_list)
{
  char **names = NULL;
  size_t count = 0;

  // call OF__iconv_get_list with false as third parameter
  int result = OF__iconv_get_list(&names, &count, false);
  OFCHECK(result == 0 && count > 0);
  for (size_t i=0; i < count; i++)
  {
    OFCHECK(names[i] != NULL);
  }
  OF__iconv_free_list(names, count);

  names = NULL;
  count = 0;

  // call OF__iconv_get_list with true as third parameter
  result = OF__iconv_get_list(&names, &count, true);
  OFCHECK(result == 0 && count > 0);
  for (size_t i=0; i < count; i++)
  {
    OFCHECK(names[i] != NULL);
  }
  OF__iconv_free_list(names, count);
}


BEGIN_EXTERN_C
int OFiconvlist_callback(unsigned int count, const char * const *names, void *arg);
END_EXTERN_C

static int OFiconvlist_callback_var = 0;
int OFiconvlist_callback(unsigned int count, const char * const *names, void *arg)
{
    OFCHECK(count > 0 && names != NULL && arg == &OFiconvlist_callback_var);
    for (size_t i=0; i < count; i++)
    {
      OFCHECK(names[i] != NULL);
    }
    return 0;
}


OFTEST(oficonvlist)
{
  OFiconvlist(OFiconvlist_callback, &OFiconvlist_callback_var);
}


OFTEST(oflocale_charset)
{
  iconv_locale_allocation_t buf;
  const char *lc = OFlocale_charset(&buf);
  OFCHECK(lc != NULL);
}

#define MAXLEN 4096

OFTEST(oficonv__iconv)
{
  iconv_t invalid_id = OFreinterpret_cast(iconv_t, -1);
  iconv_t id;

  OFCHECK(invalid_id != (id = OFiconv_open("ISO-8859-1", "UTF-8")));
  if (id != invalid_id)
  {
    // first part of the test case: test how OFiconv handles an input string containing an illegal byte sequence
    char output[MAXLEN];
    char *src_ptr = OFreinterpret_cast(char *, OF__iconv_test_sequence);
    size_t src_len = sizeof(OF__iconv_test_sequence);
    char *dst_ptr = output;
    size_t dst_len = MAXLEN;
    memset(output, 0, MAXLEN);

    // perform the conversion
    size_t result = OFiconv(id, &src_ptr, &src_len, &dst_ptr, &dst_len);

    // since the source sequence contains an illegal byte sequence, this call should have failed
    OFCHECK(result == OFstatic_cast(size_t, -1));

    // including the illegal byte sequence, there should be 20 bytes left in the source buffer
    OFCHECK(src_len == 20);

    // compare the output against a reference
    size_t refsize = sizeof(test_sequence_converted_to_latin_1);
    if ((MAXLEN - dst_len) != refsize)
    {
      OFOStringStream str;
      str << "OFiconv output does not have expected size, expected " << refsize << " bytes and found " << (MAXLEN - dst_len) << "." << OFStringStream_ends;
      OFSTRINGSTREAM_GETSTR(str, c_str)
      OFCHECK_FAIL(c_str);
      OFSTRINGSTREAM_FREESTR(c_str)
    }
    else
    {
      if (0 != memcmp(test_sequence_converted_to_latin_1, output, refsize))
      {
        OFCHECK_FAIL("OFiconv output does not match expected string.");
      }
    }

    // second part of the test case: test how OF__iconv handles an input string containing an illegal byte sequence
    src_ptr = OFreinterpret_cast(char *, OF__iconv_test_sequence);
    src_len = sizeof(OF__iconv_test_sequence);
    dst_ptr = output;
    dst_len = MAXLEN;
    memset(output, 0, MAXLEN);
    size_t invalids = 0;

    // perform the conversion
    size_t OF__iconv_result = OF__iconv(id, &src_ptr, &src_len, &dst_ptr, &dst_len, 0, &invalids);

    // since the source sequence contains an illegal byte sequence, this call should have failed
    OFCHECK(OF__iconv_result == (size_t)-1);

    // including the illegal byte sequence, there should be 20 bytes left in the source buffer
    OFCHECK(src_len == 20);

    // four characters should have been converted to the invalids character symbol because no mapping to Latin-1 exists
    OFCHECK(invalids == 4);

    // compare the output against a reference
    if ((MAXLEN - dst_len) != refsize)
    {
      OFOStringStream str;
      str << "OFiconv output does not have expected size, expected " << refsize << " bytes and found " << (MAXLEN - dst_len) << "." << OFStringStream_ends;
      OFSTRINGSTREAM_GETSTR(str, c_str)
      OFCHECK_FAIL(c_str);
      OFSTRINGSTREAM_FREESTR(c_str)
    }
    else
    {
      if (0 != memcmp(test_sequence_converted_to_latin_1, output, refsize))
      {
        OFCHECK_FAIL("OFiconv output does not match expected string.");
      }
    }

    // third part of the test case: test how OF__iconv handles an input string containing an illegal byte sequence when __ICONV_F_HIDE_INVALID is passed
    src_ptr = OFreinterpret_cast(char *, OF__iconv_test_sequence);
    src_len = sizeof(OF__iconv_test_sequence);
    dst_ptr = output;
    dst_len = MAXLEN;
    memset(output, 0, MAXLEN);
    invalids = 0;

    // perform the conversion
    OF__iconv_result = OF__iconv(id, &src_ptr, &src_len, &dst_ptr, &dst_len, __ICONV_F_HIDE_INVALID, &invalids);

    // since the source sequence contains an illegal byte sequence, this call should have failed
    OFCHECK(OF__iconv_result == (size_t)-1);

    // including the illegal byte sequence, there should be 20 bytes left in the source buffer
    OFCHECK(src_len == 20);

    // four characters should have been converted to the invalids character symbol because no mapping to Latin-1 exists
    OFCHECK(invalids == 4);

    // compare the output against a reference
    refsize = sizeof(test_sequence_converted_to_latin_1_invalids_hidden);
    if ((MAXLEN - dst_len) != refsize)
    {
      OFOStringStream str;
      str << "OFiconv output does not have expected size, expected " << refsize << " bytes and found " << (MAXLEN - dst_len) << "." << OFStringStream_ends;
      OFSTRINGSTREAM_GETSTR(str, c_str)
      OFCHECK_FAIL(c_str);
      OFSTRINGSTREAM_FREESTR(c_str)
    }
    else
    {
      if (0 != memcmp(test_sequence_converted_to_latin_1_invalids_hidden, output, refsize))
      {
        OFCHECK_FAIL("OFiconv output does not match expected string.");
      }
    }

    // all done.
    OFiconv_close(id);
  }
}

BEGIN_EXTERN_C
void test_unicode_char_hook(unsigned int mbr, void *data);
void test_wide_char_hook(_citrus_wc_t wc, void *data);
END_EXTERN_C

size_t unicode_char_counter = 0;
size_t wide_char_counter = 0;

void test_unicode_char_hook(unsigned int /* mbr */ , void *data)
{
  OFCHECK(data == &unicode_char_counter);
  unicode_char_counter++;
}

void test_wide_char_hook(_citrus_wc_t /* wc */, void *data)
{
  OFCHECK(data == &unicode_char_counter);
  wide_char_counter++;
}

OFTEST(oficonvctl)
{
  iconv_t invalid_id = OFreinterpret_cast(iconv_t, -1);
  iconv_t id1, id2, id3;
  OFCHECK(invalid_id != (id1 = OFiconv_open("ISO-8859-1", "UTF-8")));
  OFCHECK(invalid_id != (id2 = OFiconv_open("UTF-8", "UTF-8")));
  OFCHECK(invalid_id != (id3 = OFiconv_open("UTF-16", "UTF-8")));

  char output[MAXLEN];
  char *src_ptr = OFreinterpret_cast(char *, OFiconvctl_test_sequence);
  size_t src_len = sizeof(OFiconvctl_test_sequence);
  char *dst_ptr = output;
  size_t dst_len = MAXLEN;
  memset(output, 0, MAXLEN);

  if ((id1 != invalid_id) && (id1 != invalid_id) && (id1 != invalid_id))
  {
    // Test 1: ICONV_TRIVIALP
    int intvar = -1;
    // a conversion from UTF-8 to ISO-8859-1 is not trivial
    OFCHECK(0 == OFiconvctl(id1, ICONV_TRIVIALP, &intvar) && 0 == intvar);
    intvar = -1;
    // a conversion from UTF-8 to UTF-8 is trivial
    OFCHECK(0 == OFiconvctl(id2, ICONV_TRIVIALP, &intvar) && 1 == intvar);
    intvar = -1;
    // a conversion from UTF-8 to UTF-16 is not trivial, although both are Unicode
    OFCHECK(0 == OFiconvctl(id3, ICONV_TRIVIALP, &intvar) && 0 == intvar);

    // Test 2: ICONV_GET_TRANSLITERATE
    // In this implementation, transliteration is always "on" and cannot be disabled
    intvar = -1;
    OFCHECK(0 == OFiconvctl(id1, ICONV_GET_TRANSLITERATE, &intvar) && 1 == intvar);

    // Test 3: ICONV_SET_TRANSLITERATE
    // In this implementation, transliteration is always "on" and cannot be disabled

    // enable transliteration, this should work
    intvar = 1;
    OFCHECK(0 == OFiconvctl(id1, ICONV_SET_TRANSLITERATE, &intvar));

    // disable transliteration, this should fail
    intvar = 0;
    OFCHECK(-1 == OFiconvctl(id1, ICONV_SET_TRANSLITERATE, &intvar));

    // after the previous operation, transliteration, should still be "on"
    intvar = -1;
    OFCHECK(0 == OFiconvctl(id1, ICONV_GET_TRANSLITERATE, &intvar) && 1 == intvar);

    // Test 4: ICONV_GET_DISCARD_ILSEQ
    // By default, this feature is off
    intvar = -1;
    OFCHECK(0 == OFiconvctl(id1, ICONV_GET_DISCARD_ILSEQ, &intvar) && 0 == intvar);

    // Test 5: ICONV_SET_DISCARD_ILSEQ
    // first part of the test case: do not discard illegal sequences
    intvar = 0;
    OFCHECK(0 == OFiconvctl(id1, ICONV_SET_DISCARD_ILSEQ, &intvar));

    // perform the conversion. Four characters should be converted to 'invalid'
    OFCHECK(4 == OFiconv(id1, &src_ptr, &src_len, &dst_ptr, &dst_len));
    OFCHECK(src_len == 0);

    // compare the output against a reference
    size_t refsize = sizeof(test_sequence_converted_to_latin_1);
    if ((MAXLEN - dst_len) != refsize)
    {
      OFOStringStream str;
      str << "OFiconv output does not have expected size, expected " << refsize << " bytes and found " << (MAXLEN - dst_len) << "." << OFStringStream_ends;
      OFSTRINGSTREAM_GETSTR(str, c_str)
      OFCHECK_FAIL(c_str);
      OFSTRINGSTREAM_FREESTR(c_str)
    }
    else
    {
      if (0 != memcmp(test_sequence_converted_to_latin_1, output, refsize))
      {
        OFCHECK_FAIL("OFiconv output does not match expected string.");
      }
    }

    // second part of the test case: do discard illegal sequences
    intvar = 1;
    OFCHECK(0 == OFiconvctl(id1, ICONV_SET_DISCARD_ILSEQ, &intvar));

    src_ptr = OFreinterpret_cast(char *, OFiconvctl_test_sequence);
    src_len = sizeof(OFiconvctl_test_sequence);
    dst_ptr = output;
    dst_len = MAXLEN;
    memset(output, 0, MAXLEN);

    // perform the conversion. Four characters should be converted to 'invalid'
    OFCHECK(4 == OFiconv(id1, &src_ptr, &src_len, &dst_ptr, &dst_len));
    OFCHECK(src_len == 0);

    // compare the output against a reference
    refsize = sizeof(test_sequence_converted_to_latin_1_invalids_hidden);
    if ((MAXLEN - dst_len) != refsize)
    {
      OFOStringStream str;
      str << "OFiconv output does not have expected size, expected " << refsize << " bytes and found " << (MAXLEN - dst_len) << "." << OFStringStream_ends;
      OFSTRINGSTREAM_GETSTR(str, c_str)
      OFCHECK_FAIL(c_str);
      OFSTRINGSTREAM_FREESTR(c_str)
    }
    else
    {
      if (0 != memcmp(test_sequence_converted_to_latin_1_invalids_hidden, output, refsize))
      {
        OFCHECK_FAIL("OFiconv output does not match expected string.");
      }
    }

    // return the conversion descriptor to its original state: do not discard illegal sequences
    intvar = 0;
    OFCHECK(0 == OFiconvctl(id1, ICONV_SET_DISCARD_ILSEQ, &intvar));

    // Test 6: ICONV_SET_HOOKS
    struct iconv_hooks hooks;
    hooks.uc_hook = test_unicode_char_hook;
    hooks.wc_hook = test_wide_char_hook;
    hooks.data = &unicode_char_counter;
    OFCHECK(0 == OFiconvctl(id1, ICONV_SET_HOOKS, &hooks));

    src_ptr = OFreinterpret_cast(char *, OFiconvctl_test_sequence);
    src_len = sizeof(OFiconvctl_test_sequence);
    dst_ptr = output;
    dst_len = MAXLEN;
    memset(output, 0, MAXLEN);

    // perform a conversion from UTF-8 to Latin-1.
    // The Unicode callback should be called for each of the 36 Unicode characters in question
    // Note: We don't have a test case for the wide char hook - there seems to be
    // no character set used by DICOM that would trigger this callback.
    OFCHECK(4 == OFiconv(id1, &src_ptr, &src_len, &dst_ptr, &dst_len));
    OFCHECK(src_len == 0);
    OFCHECK(unicode_char_counter == 36);
    OFCHECK(wide_char_counter == 0);

    // Test 7: ICONV_SET_FALLBACKS
    // in GNU libiconv, this call expects a pointer to struct iconv_fallbacks as parameter.
    // This implementation, however, does not support this function and always returns an error.
    // Just in case someone tries this with GNU libiconv, pass a buffer that is large enough
    memset(output, 0, MAXLEN);
    OFCHECK(-1 == OFiconvctl(id1, ICONV_SET_FALLBACKS, &output));

    // Test 8: ICONV_GET_ILSEQ_INVALID
    // By default, this feature is off
    intvar = -1;
    OFCHECK(0 == OFiconvctl(id1, ICONV_GET_DISCARD_ILSEQ, &intvar) && 0 == intvar);

    // Test 9: ICONV_SET_ILSEQ_INVALID
    intvar = 1;
    OFCHECK(0 == OFiconvctl(id1, ICONV_SET_ILSEQ_INVALID, &intvar));

    src_ptr = OFreinterpret_cast(char *, OFiconvctl_test_sequence);
    src_len = sizeof(OFiconvctl_test_sequence);
    dst_ptr = output;
    dst_len = MAXLEN;
    memset(output, 0, MAXLEN);

    // perform the conversion. This should fail at the first Japanese character
    OFCHECK((size_t)-1 == OFiconv(id1, &src_ptr, &src_len, &dst_ptr, &dst_len));

    // there should be 28 bytes left in the source buffer
    OFCHECK(src_len == 28);

    // compare the output against a reference
    refsize = sizeof(test_sequence_converted_to_latin_1_invalids_stopped);
    if ((MAXLEN - dst_len) != refsize)
    {
      OFOStringStream str;
      str << "OFiconv output does not have expected size, expected " << refsize << " bytes and found " << (MAXLEN - dst_len) << "." << OFStringStream_ends;
      OFSTRINGSTREAM_GETSTR(str, c_str)
      OFCHECK_FAIL(c_str);
      OFSTRINGSTREAM_FREESTR(c_str)
    }
    else
    {
      if (0 != memcmp(test_sequence_converted_to_latin_1_invalids_stopped, output, refsize))
      {
        OFCHECK_FAIL("OFiconv output does not match expected string.");
      }
    }

    // finally, release the conversion descriptors
    OFiconv_close(id1);
    OFiconv_close(id2);
    OFiconv_close(id3);
  }
}
