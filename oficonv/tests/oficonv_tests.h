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

#ifndef OFICONV_TESTS_H
#define OFICONV_TESTS_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/oficonv/iconv.h"

/** perform a character set conversion using a given conversion descriptor and compare the result against a reference.
 *  This function calls OFTEST_CHECK and triggers a unit test failure if the result of the conversion differs from the reference.
 *  @param id conversion descriptor
 *  @param src source string
 *  @param srcsize number of bytes in the source string
 *  @param reference expected output string
 *  @param refsize number of bytes in the expected output string
 */
void do_conversion(iconv_t id, unsigned char *src, size_t srcsize, unsigned char *reference, size_t refsize);

/** convert all printable characters of an ISO 8859 character set to UTF-8 and compare against a reference.
 *  The routine automatically converts all printable ASCII characters (< 128), so the source string should
 *  only contain characters >= 128.
 *  @param charset name of the ISO 8859 character set for the source string
 *  @param src source string
 *  @param srcsize number of bytes in the source string
 *  @param reference expected UTF-8 output string
 *  @param refsize number of bytes in the expected UTF-8 output string
 */
void convert_iso8859_to_utf8(const char *charset, unsigned char *src, size_t srcsize, unsigned char *reference, size_t refsize);

/** convert all printable characters of an ISO 8859 character set to another ISO 8859 character set and compare against a reference.
 *  The routine automatically converts all printable ASCII characters (< 128), so the source string should
 *  only contain characters >= 128.
 *  @param src_charset name of the ISO 8859 character set for the source string
 *  @param src source string
 *  @param srcsize number of bytes in the source string
 *  @param dst_charset name of the ISO 8859 character set for the output/reference string
 *  @param reference expected output string
 *  @param refsize number of bytes in the expected output string
 */
void convert_iso8859_to_iso8859(const char *src_charset, unsigned char *src, size_t srcsize, const char *dst_charset, unsigned char *reference, size_t refsize);

/** convert UTF-8 characters to an ISO 8859 character set  and compare against a reference.
 *  The routine automatically converts all printable ASCII characters (< 128), so the source string should
 *  only contain UTF-8 code for the characters corresponding to values >= 128 in ISO 8859.
 *  @param charset name of the ISO 8859 character set for the source string
 *  @param reference expected UTF-8 output string
 *  @param refsize number of bytes in the expected UTF-8 output string
 *  @param src source string
 *  @param srcsize number of bytes in the source string
 */
void convert_utf8_to_iso8859(const char *charset, unsigned char *reference, size_t refsize, unsigned char *src, size_t srcsize);

/** convert all printable characters of an ISO 8859 character set to ASCII and compare against a reference.
 *  The routine automatically converts all printable ASCII characters (< 128), so the source string should
 *  only contain characters >= 128, which will all be converted to '?' by the rather stupid
 *  Citrus iconv transliteration rules.
 *  @param charset name of the ISO 8859 character set for the source string
 *  @param src source string
 *  @param srcsize number of bytes in the source string
 */
void convert_iso8859_to_ascii(const char *charset, unsigned char *src, size_t srcsize);

/** convert an arbitrary source sequence of bytes to UTF-8 and compare against a reference.
 *  @param charset name of the character set for the source string
 *  @param src source string
 *  @param srcsize number of bytes in the source string
 *  @param reference expected UTF-8 output
 *  @param refsize number of bytes in the expected UTF-8 output
 */
void convert_to_utf8(const char *charset, unsigned char *src, size_t srcsize, unsigned char *reference, size_t refsize);

/** convert a UTF-8 string to an arbitrary encoding and compare against a reference.
 *  @param charset name of the character set for the source string
 *  @param reference expected output
 *  @param refsize number of bytes in the expected output
 *  @param src UTF-8 source string
 *  @param srcsize number of bytes in the UTF-8 source string
 */
void convert_from_utf8(const char *charset, unsigned char *reference, size_t refsize, unsigned char *src, size_t srcsize);

/** convert a single byte string to ASCII and compare against a reference.
 *  @param charset name of the character set for the source string
 *  @param src source string
 *  @param srcsize number of bytes in the source string
 *  @param reference expected ASCII output
 *  @param refsize number of bytes in the expected ASCII output
 */
void convert_jis_x0201_to_ascii(const char *charset, unsigned char *src, size_t srcsize, unsigned char *reference, size_t refsize);

/** convert an array containing only the second bytes of a two-byte EUC sequence
 *  to UTF-8 and compare against a reference. This function first prepares an input
 *  buffer twice the size of the source string by placing the prefix byte before
 *  every byte from the source string (i.e. all bytes in the source string must
 *  belong to the same EUC row), and then performs conversion to UTF-8.
 *  @param charset name of the character set for the source string
 *  @param prefix prefix byte (first byte of each character)
 *  @param src source string, only containing the second bytes of each character
 *  @param srcsize number of bytes in the source string
 *  @param reference expected UTF-8 output
 *  @param refsize number of bytes in the expected UTF-8 output
 */
void convert_euc_to_utf8(const char *charset, unsigned char prefix, unsigned char *src, size_t srcsize, unsigned char *reference, size_t refsize);


/** convert a UTF-8 string into a two-byte EUC sequence and compare against a reference
 *  This function first prepares the reference by placing the prefix byte before
 *  every byte from the reference string (i.e. all bytes in the reference string must
 *  belong to the same EUC row), and then compares against the conversion from UTF-8.
 *  @param charset name of the character set for the source string
 *  @param prefix prefix byte (first byte of each EUC character)
 *  @param reference reference string, only containing the second bytes of each character
 *  @param refsize number of bytes in the reference string
 *  @param src source string in UTF-8
 *  @param srcsize number of bytes in the source string
 */
void convert_utf8_to_euc(const char *charset, unsigned char prefix, unsigned char *reference, size_t refsize, unsigned char *src, size_t srcsize);

/** Check that the conversion from ASCII to an EUC encoding and back is an identity operation.
 *  @param charset name of the EUC encoding
 */
void check_euc_ascii_characters(const char *charset);

#endif
