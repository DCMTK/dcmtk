/*
 *
 *  Copyright (C) 2002-2011, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier / Uli Schlachter
 *
 *  Purpose: test program for base64-code in OFStandard
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2011-07-06 13:04:18 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofstd.h"

static void testBase64(const unsigned char* bin_data, size_t bin_len,
        const char* bin_b64, const char* bin_b64_line_breaks)
{
    OFString tmpString;
    OFOStringStream oss;
    unsigned char *buffer;
    size_t i, length;

    // Without linebreaks
    // encode buffer
    OFCHECK_EQUAL(&OFStandard::encodeBase64(bin_data, bin_len, tmpString), &tmpString);
    OFCHECK_EQUAL(tmpString, bin_b64);
    // encode stream
    OFStandard::encodeBase64(oss, bin_data, bin_len);
    oss << OFStringStream_ends;
    OFSTRINGSTREAM_GETOFSTRING(oss, str1)
    OFCHECK_EQUAL(str1, bin_b64);
    oss.str("");

    // With linebreaks
    // encode buffer
    OFCHECK_EQUAL(&OFStandard::encodeBase64(bin_data, bin_len, tmpString, 72), &tmpString);
    OFCHECK_EQUAL(tmpString, bin_b64_line_breaks);
    // encode stream
    OFStandard::encodeBase64(oss, bin_data, bin_len, 72);
    oss << OFStringStream_ends;
    OFSTRINGSTREAM_GETOFSTRING(oss, str2)
    OFCHECK_EQUAL(str2, bin_b64_line_breaks);

    // decode
    length = OFStandard::decodeBase64(tmpString, buffer);
    OFCHECK_EQUAL(length, bin_len);
    for (i = 0; i < length; i++)
        OFCHECK_EQUAL(buffer[i], bin_data[i]);
    delete[] buffer;
}

OFTEST(ofstd_base64_1)
{
    const size_t bin_len = 10;
    const unsigned char bin_data[bin_len] = {10, 5, 88, 99, 255, 250, 150, 128, 0, 254};
    const char* bin_data_b64 = "CgVYY//6loAA/g==";

    testBase64(bin_data, bin_len, bin_data_b64, bin_data_b64);
}

OFTEST(ofstd_base64_2)
{
    const size_t bin_len = 511;
    size_t i;
    unsigned char *buffer;
    const char* b64 = "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygp"
        "KissLS4vMDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV"
        "5fYGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6PkJGS"
        "k5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/wMHCw8TFxs"
        "fIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v8PHy8/T19vf4+fr7"
        "/P3+//79/Pv6+fj39vX08/Lx8O/u7ezr6uno5+bl5OPi4eDf3t3c29rZ2NfW1dTT0tHQz8"
        "7NzMvKycjHxsXEw8LBwL++vby7urm4t7a1tLOysbCvrq2sq6qpqKempaSjoqGgn56dnJua"
        "mZiXlpWUk5KRkI+OjYyLiomIh4aFhIOCgYB/fn18e3p5eHd2dXRzcnFwb25tbGtqaWhnZm"
        "VkY2JhYF9eXVxbWllYV1ZVVFNSUVBPTk1MS0pJSEdGRURDQkFAPz49PDs6OTg3NjU0MzIx"
        "MC8uLSwrKikoJyYlJCMiISAfHh0cGxoZGBcWFRQTEhEQDw4NDAsKCQgHBgUEAwIBAA==";
    const char *b64_line_breaks =
        "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4vMDEyMzQ1\n"
        "Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5fYGFiY2RlZmdoaWpr\n"
        "bG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6PkJGSk5SVlpeYmZqbnJ2en6Ch\n"
        "oqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX\n"
        "2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v8PHy8/T19vf4+fr7/P3+//79/Pv6+fj39vX08/Lx\n"
        "8O/u7ezr6uno5+bl5OPi4eDf3t3c29rZ2NfW1dTT0tHQz87NzMvKycjHxsXEw8LBwL++vby7\n"
        "urm4t7a1tLOysbCvrq2sq6qpqKempaSjoqGgn56dnJuamZiXlpWUk5KRkI+OjYyLiomIh4aF\n"
        "hIOCgYB/fn18e3p5eHd2dXRzcnFwb25tbGtqaWhnZmVkY2JhYF9eXVxbWllYV1ZVVFNSUVBP\n"
        "Tk1MS0pJSEdGRURDQkFAPz49PDs6OTg3NjU0MzIxMC8uLSwrKikoJyYlJCMiISAfHh0cGxoZ\n"
        "GBcWFRQTEhEQDw4NDAsKCQgHBgUEAwIBAA==";

    buffer = new unsigned char[bin_len];
    for (i = 0; i < 256; i++)
        buffer[i] = OFstatic_cast(unsigned char, i);
    for (i = 256; i < 511; i++)
        buffer[i] = OFstatic_cast(unsigned char, 510 - i);

    testBase64(buffer, bin_len, b64, b64_line_breaks);
}

OFTEST(ofstd_base64_3)
{
    const OFString txt_data1 = ".AB_Cab$$c123-";
    const OFString txt_data2 = "ABC\nabc\n123";
    const unsigned char dec[] = { 0, 16, 154, 109, 205, 118 };
    unsigned char *buffer;
    size_t i, length;

    length = OFStandard::decodeBase64(txt_data1, buffer);
    for (i = 0; i < length; i++)
        OFCHECK_EQUAL(buffer[i], dec[i]);

    length = OFStandard::decodeBase64(txt_data2, buffer);
    for (i = 0; i < length; i++)
        OFCHECK_EQUAL(buffer[i], dec[i]);
}

/*
 *
 * CVS/RCS Log:
 * $Log: tbase64.cc,v $
 * Revision 1.2  2011-07-06 13:04:18  uli
 * Fixed some inconsistencies in test names.
 *
 * Revision 1.1  2011-05-25 10:05:57  uli
 * Imported oftest and converted existing tests to oftest.
 *
 * Revision 1.13  2010-10-14 13:15:16  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.12  2009-04-27 14:21:43  joergr
 * Added further test for UNC syntax of path expressions.
 *
 * Revision 1.11  2008-04-18 09:14:02  joergr
 * Added further tests for combineDirAndFilename().
 *
 * Revision 1.10  2007/06/26 16:19:40  joergr
 * Added new variant of encodeBase64() method that outputs directly to a stream
 * (avoids using a memory buffer for large binary data).
 *
 * Revision 1.9  2006/08/14 16:42:48  meichel
 * Updated all code in module ofstd to correctly compile if the standard
 * namespace has not included into the global one with a "using" directive.
 *
 * Revision 1.8  2005/12/08 15:49:06  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.7  2004/01/16 10:37:23  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.6  2003/09/17 17:01:44  joergr
 * Renamed variable "string" to avoid name clash with STL class.
 *
 * Revision 1.5  2003/08/14 09:01:20  meichel
 * Adapted type casts to new-style typecast operators defined in ofcast.h
 *
 * Revision 1.4  2003/08/12 13:11:46  joergr
 * Improved implementation of normalizeDirName().
 *
 * Revision 1.3  2002/05/14 08:13:55  joergr
 * Added support for Base64 (MIME) encoding and decoding.
 *
 * Revision 1.2  2002/04/16 13:37:01  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 *
 */
