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
 *  Purpose: common test functions for the oficonv unit tests
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "oficonv_tests.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/oficonv/iconv.h"

// printable ASCII characters (ISO 646). These are the same in all ISO 8859 alphabets.
static unsigned char char_20_to_7e[] = {
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e
};

#define MAXLEN 4096

void do_conversion(iconv_t id, unsigned char *src, size_t srcsize, unsigned char *reference, size_t refsize)
{
  char output[MAXLEN];
  char *src_ptr = OFreinterpret_cast(char *, src);
  size_t src_len = srcsize;
  char *dst_ptr = output;
  size_t dst_len = MAXLEN;

  memset(output, 0, MAXLEN);
  (void) OFiconv(id, &src_ptr, &src_len, &dst_ptr, &dst_len);

  if (src_len > 0)
  {
    OFOStringStream str;
    str << "OFiconv did not convert all characters, " << src_len << " characters left in buffer." << OFStringStream_ends;
    OFSTRINGSTREAM_GETSTR(str, c_str)
    OFCHECK_FAIL(c_str);
    OFSTRINGSTREAM_FREESTR(c_str)
  }

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
    if (0 != memcmp(reference, output, refsize))
    {
      OFCHECK_FAIL("OFiconv output does not match expected string.");
    }
  }

#if 0
  // This prints the converted output to stdout, which may be useful for debugging purposes
  COUT << output << OFendl;
#endif
}


void convert_iso8859_to_utf8(const char *charset, unsigned char *src, size_t srcsize, unsigned char *reference, size_t refsize)
{
  iconv_t invalid_id = OFreinterpret_cast(iconv_t, -1);
  iconv_t id;

  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", charset)));
  if (id != invalid_id)
  {
    // convert the characters below 128 to UTF-8. The output should be identical to the input.
    do_conversion(id, char_20_to_7e, sizeof(char_20_to_7e), char_20_to_7e, sizeof(char_20_to_7e));

    // convert the characters above 128 to UTF-8 and compare against a reference
    do_conversion(id, src, srcsize, reference, refsize);

    OFiconv_close(id);
  }
}


void convert_iso8859_to_iso8859(const char *src_charset, unsigned char *src, size_t srcsize, const char *dst_charset, unsigned char *reference, size_t refsize)
{
  iconv_t invalid_id = OFreinterpret_cast(iconv_t, -1);
  iconv_t id;

  OFCHECK(invalid_id != (id = OFiconv_open(dst_charset, src_charset)));
  if (id != invalid_id)
  {
    // convert the characters below 128. The output should be identical to the input.
    do_conversion(id, char_20_to_7e, sizeof(char_20_to_7e), char_20_to_7e, sizeof(char_20_to_7e));

    // convert the characters above 128 to the destination charset and compare against a reference
    do_conversion(id, src, srcsize, reference, refsize);

    OFiconv_close(id);
  }
}


void convert_utf8_to_iso8859(const char *charset, unsigned char *reference, size_t refsize, unsigned char *src, size_t srcsize)
{
  iconv_t invalid_id = OFreinterpret_cast(iconv_t, -1);
  iconv_t id;

  OFCHECK(invalid_id != (id = OFiconv_open(charset, "UTF-8")));
  if (id != invalid_id)
  {
    // convert the characters below 128 to UTF-8. The output should be identical to the input.
    do_conversion(id, char_20_to_7e, sizeof(char_20_to_7e), char_20_to_7e, sizeof(char_20_to_7e));

    // convert the characters above 128 to UTF-8 and compare against a reference
    do_conversion(id, src, srcsize, reference, refsize);

    OFiconv_close(id);
  }
}


void convert_iso8859_to_ascii(const char *charset, unsigned char *src, size_t srcsize)
{
  iconv_t invalid_id = OFreinterpret_cast(iconv_t, -1);
  iconv_t id;
  unsigned char question_marks[256];

  OFCHECK(invalid_id != (id = OFiconv_open("ASCII", charset)));
  if (id != invalid_id)
  {
    // convert the characters below 128 to ASCII. The output should be identical to the input.
    do_conversion(id, char_20_to_7e, sizeof(char_20_to_7e), char_20_to_7e, sizeof(char_20_to_7e));

    memset(question_marks, '?', sizeof(question_marks));

    // convert the characters above 128 to ASCII and compare against a reference
    do_conversion(id, src, srcsize, question_marks, srcsize);

    OFiconv_close(id);
  }
}


void convert_to_utf8(const char *charset, unsigned char *src, size_t srcsize, unsigned char *reference, size_t refsize)
{
  iconv_t invalid_id = OFreinterpret_cast(iconv_t, -1);
  iconv_t id;

  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", charset)));
  if (id != invalid_id)
  {
    do_conversion(id, src, srcsize, reference, refsize);

    OFiconv_close(id);
  }
}


void convert_from_utf8(const char *charset, unsigned char *reference, size_t refsize, unsigned char *src, size_t srcsize)
{
  iconv_t invalid_id = OFreinterpret_cast(iconv_t, -1);
  iconv_t id;

  OFCHECK(invalid_id != (id = OFiconv_open(charset, "UTF-8")));
  if (id != invalid_id)
  {
    do_conversion(id, src, srcsize, reference, refsize);

    OFiconv_close(id);
  }
}


void convert_jis_x0201_to_ascii(const char *charset, unsigned char *src, size_t srcsize, unsigned char *reference, size_t refsize)
{
  iconv_t invalid_id = OFreinterpret_cast(iconv_t, -1);
  iconv_t id;

  OFCHECK(invalid_id != (id = OFiconv_open("ASCII", charset)));
  if (id != invalid_id)
  {
    do_conversion(id, src, srcsize, reference, refsize);

    OFiconv_close(id);
  }
}


void convert_euc_to_utf8(const char *charset, unsigned char prefix, unsigned char *src, size_t srcsize, unsigned char *reference, size_t refsize)
{
  iconv_t invalid_id = OFreinterpret_cast(iconv_t, -1);
  iconv_t id;

  // Expand input buffer by prefixing each byte with the given prefix byte, converting single bytes to double bytes
  // This assumes that all characters in the input buffer use the same prefix byte, i.e. belong to the same row.
  unsigned char *buf = new unsigned char[srcsize * 2];
  unsigned char *srcptr = src;
  unsigned char *bufptr = buf;
  for (size_t i = 0; i < srcsize; ++i)
  {
    *bufptr++ = prefix;
    *bufptr++ = *srcptr++;
  }

  OFCHECK(invalid_id != (id = OFiconv_open("UTF-8", charset)));
  if (id != invalid_id)
  {
    do_conversion(id, buf, srcsize * 2, reference, refsize);

    OFiconv_close(id);
  }
  delete[] buf;
}


void convert_utf8_to_euc(const char *charset, unsigned char prefix, unsigned char *reference, size_t refsize, unsigned char *src, size_t srcsize)
{
  iconv_t invalid_id = OFreinterpret_cast(iconv_t, -1);
  iconv_t id;

  // Expand input buffer by prefixing each byte with the given prefix byte, converting single bytes to double bytes
  // This assumes that all characters in the input buffer use the same prefix byte, i.e. belong to the same row.
  unsigned char *refbuf = new unsigned char[refsize * 2];
  unsigned char *refptr = reference;
  unsigned char *bufptr = refbuf;
  for (size_t i = 0; i < refsize; ++i)
  {
    *bufptr++ = prefix;
    *bufptr++ = *refptr++;
  }

  OFCHECK(invalid_id != (id = OFiconv_open(charset, "UTF-8")));
  if (id != invalid_id)
  {
    do_conversion(id, src, srcsize, refbuf, refsize * 2);

    OFiconv_close(id);
  }
  delete[] refbuf;
}


void check_euc_ascii_characters(const char *charset)
{
  iconv_t invalid_id = OFreinterpret_cast(iconv_t, -1);
  iconv_t id;

  // make sure that a conversion from EUC to ASCII is an identity operation for characters < 128
  OFCHECK(invalid_id != (id = OFiconv_open("ASCII", charset)));
  if (id != invalid_id)
  {
    do_conversion(id, char_20_to_7e, sizeof(char_20_to_7e), char_20_to_7e, sizeof(char_20_to_7e));
    OFiconv_close(id);
  }

  // make sure that a conversion from ASCII to EUC is an identity operation
  OFCHECK(invalid_id != (id = OFiconv_open(charset, "ASCII")));
  if (id != invalid_id)
  {
    do_conversion(id, char_20_to_7e, sizeof(char_20_to_7e), char_20_to_7e, sizeof(char_20_to_7e));
    OFiconv_close(id);
  }
}
