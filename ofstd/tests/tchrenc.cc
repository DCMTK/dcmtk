/*
 *
 *  Copyright (C) 2011-2016, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: test program for OFCharacterEncoding
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofchrenc.h"


#define checkConversionFlags(flags)\
    if (OFCharacterEncoding::supportsConversionFlags(flags))\
    {\
        OFCHECK(charEnc.setConversionFlags(flags).good());\
        OFCHECK_EQUAL(charEnc.getConversionFlags(), flags);\
    }


OFTEST(ofstd_OFCharacterEncoding_1)
{
    // the returned string should never be empty
    OFCHECK(!OFCharacterEncoding::getLibraryVersionString().empty());
    // check whether character set conversion support is available
    if (OFCharacterEncoding::isLibraryAvailable())
    {
        OFCharacterEncoding charEnc;
        OFString resultStr;
        // first, do some basic validity checks
        OFCHECK(charEnc.selectEncoding("ASCII", "UTF-8").good());
        OFCHECK(charEnc.convertString("Simple Text", resultStr).good());
        OFCHECK_EQUAL(resultStr, "Simple Text");
        OFCHECK(charEnc.convertString("", resultStr, OFFalse /*clear*/).good());
        OFCHECK_EQUAL(resultStr, "Simple Text");
        OFCHECK(charEnc.convertString(NULL, 1, resultStr).good());
        OFCHECK(resultStr.empty());
        OFCHECK(charEnc.convertString("", 0, resultStr).good());
        OFCHECK(resultStr.empty());
        OFCHECK(charEnc.convertString(" ", 0, resultStr).good());
        OFCHECK(resultStr.empty());
        // check string with embedded NULL byte
        OFCHECK(charEnc.convertString(" \0 ", 3, resultStr).good());
        OFCHECK_EQUAL(resultStr, OFString(" \0 ", 3));
        OFCHECK(charEnc.selectEncoding("ASCII", "DCMTK").bad());
        OFCHECK(charEnc.selectEncoding("DCMTK", "ASCII").bad());
        // some implementations of iconv_open() in the C standard library do
        // not understand the "" argument
        if (OFCharacterEncoding::hasDefaultEncoding())
        {
            OFCHECK(charEnc.selectEncoding("", "ASCII").good());
            OFCHECK(charEnc.selectEncoding("ASCII", "").good());
            OFCHECK(charEnc.selectEncoding("ASCII", charEnc.getLocaleEncoding()).good());
        }
        else
        {
            OFCHECK(charEnc.selectEncoding("ASCII", "UTF-8").good());
        }
        checkConversionFlags(OFCharacterEncoding::AbortTranscodingOnIllegalSequence);
        checkConversionFlags(OFCharacterEncoding::DiscardIllegalSequences);
        checkConversionFlags(OFCharacterEncoding::TransliterateIllegalSequences);
        checkConversionFlags(OFCharacterEncoding::TransliterateIllegalSequences
                             | OFCharacterEncoding::DiscardIllegalSequences);
        charEnc.clear();
        OFCHECK(charEnc.convertString("Simple Text", resultStr).bad());
    }
}


OFTEST(ofstd_OFCharacterEncoding_2)
{
    if (OFCharacterEncoding::isLibraryAvailable())
    {
        OFCharacterEncoding charEnc;
        OFString resultStr1, resultStr2;
        // then do some real conversions
        OFCHECK(charEnc.selectEncoding("ISO-8859-1", "UTF-8").good());
        OFCHECK(charEnc.convertString("J\366rg", resultStr1).good());
        OFCHECK_EQUAL(resultStr1, "J\303\266rg");
        OFCHECK_EQUAL(resultStr1.length(), 5);
        OFCHECK(charEnc.convertString("J\351r\364me", resultStr2).good());
        OFCHECK_EQUAL(resultStr2, "J\303\251r\303\264me");
        OFCHECK_EQUAL(resultStr2.length(), 8);
        // and convert back to the original encoding
        OFString resultStr3, resultStr4;
        OFCHECK(charEnc.selectEncoding("UTF-8", "ISO-8859-1").good());
        OFCHECK(charEnc.convertString(resultStr1, resultStr3).good());
        OFCHECK_EQUAL(resultStr3, "J\366rg");
        OFCHECK(charEnc.convertString(resultStr2, resultStr4).good());
        OFCHECK_EQUAL(resultStr4, "J\351r\364me");
    }
}


OFTEST(ofstd_OFCharacterEncoding_3)
{
    if (OFCharacterEncoding::isLibraryAvailable())
    {
        // create a huge string with valid ISO 8859-1 characters (code #32 to #255)
        char hugeStr[4096];
        for (size_t i = 0; i < sizeof(hugeStr); i++)
            hugeStr[i] = OFstatic_cast(char, 32 + (i % 224));
        // then convert it to UTF-8 (and check whether the internal buffering works)
        OFCharacterEncoding charEnc;
        OFString resultStr;
        OFCHECK(charEnc.selectEncoding("ISO-8859-1", "UTF-8").good());
        OFCHECK(charEnc.convertString(OFString(hugeStr, sizeof(hugeStr)), resultStr).good());
        OFCHECK_EQUAL(resultStr.length(), 6400);
    }
}


OFTEST(ofstd_OFCharacterEncoding_4)
{
    if (OFCharacterEncoding::isLibraryAvailable())
    {
        OFCharacterEncoding charEnc;
        OFCHECK(charEnc.selectEncoding("ISO-8859-1", "ASCII").good());
        OFString resultStr;
        if (OFCharacterEncoding::supportsConversionFlags(OFCharacterEncoding::AbortTranscodingOnIllegalSequence))
        {
            OFCHECK(charEnc.setConversionFlags(OFCharacterEncoding::AbortTranscodingOnIllegalSequence).good());
            OFCHECK(charEnc.convertString("J\366rg", resultStr).bad());
        }
        if (OFCharacterEncoding::supportsConversionFlags(OFCharacterEncoding::DiscardIllegalSequences))
        {
            OFCHECK(charEnc.setConversionFlags(OFCharacterEncoding::DiscardIllegalSequences).good());
            OFCHECK(charEnc.convertString("J\366rg", resultStr).good());
            OFCHECK_EQUAL(resultStr, "Jrg");
        }
        if (OFCharacterEncoding::supportsConversionFlags(OFCharacterEncoding::TransliterateIllegalSequences))
        {
            OFCHECK(charEnc.setConversionFlags(OFCharacterEncoding::TransliterateIllegalSequences).good());
            OFCHECK(charEnc.convertString("J\366rg", resultStr).good());
            OFCHECK_EQUAL(resultStr, "J\"org");
        }
    }
}


OFTEST(ofstd_OFCharacterEncoding_5)
{
    if (OFCharacterEncoding::isLibraryAvailable())
    {
        OFCharacterEncoding charEnc;
        OFCHECK(charEnc.selectEncoding("ISO-8859-1", "ASCII").good());
        OFString resultStr;
        if (OFCharacterEncoding::supportsConversionFlags(OFCharacterEncoding::TransliterateIllegalSequences))
        {
            OFCHECK(charEnc.setConversionFlags(OFCharacterEncoding::TransliterateIllegalSequences).good());
            OFCHECK(charEnc.convertString("J\366rg", resultStr).good());
            OFCHECK_EQUAL(resultStr, "J\"org");
        }
        if (OFCharacterEncoding::supportsConversionFlags(OFCharacterEncoding::DiscardIllegalSequences))
        {
            OFCHECK(charEnc.setConversionFlags(OFCharacterEncoding::DiscardIllegalSequences).good());
            OFCHECK(charEnc.convertString("J\366rg", resultStr).good());
            OFCHECK_EQUAL(resultStr, "Jrg");
        }
        if (OFCharacterEncoding::supportsConversionFlags(OFCharacterEncoding::AbortTranscodingOnIllegalSequence))
        {
            OFCHECK(charEnc.setConversionFlags(OFCharacterEncoding::AbortTranscodingOnIllegalSequence).good());
            OFCHECK(charEnc.convertString("J\366rg", resultStr).bad());
        }
    }
}


OFTEST(ofstd_OFCharacterEncoding_6)
{
    if (OFCharacterEncoding::isLibraryAvailable())
    {
        OFCharacterEncoding charEnc;
        OFString resultStr1, resultStr2;
        OFCHECK(charEnc.selectEncoding("ISO-8859-1", "UTF-8").good());
        OFCHECK(charEnc.convertString("J\366rg", resultStr1).good());
        // count number of bytes and number of characters
        OFCHECK_EQUAL(resultStr1.length(), 5);
        OFCHECK_EQUAL(charEnc.countCharactersInUTF8String(resultStr1), 4);
        OFCHECK_EQUAL(charEnc.countCharactersInUTF8String("Joerg"), 5);
        OFCHECK(charEnc.convertString("J\351r\364me", resultStr2).good());
        // count number of bytes and number of characters
        OFCHECK_EQUAL(resultStr2.length(), 8);
        OFCHECK_EQUAL(charEnc.countCharactersInUTF8String(resultStr2), 6);
        OFCHECK_EQUAL(charEnc.countCharactersInUTF8String("Jerome"), 6);
        // final test: empty string
        OFCHECK_EQUAL(charEnc.countCharactersInUTF8String(""), 0);
    }
}


OFTEST(ofstd_OFCharacterEncoding_7)
{
#ifdef _WIN32
    OFString resultStr;
    wchar_t *resultPtr;
    size_t resultLen;
    /* check Windows-specific conversion from UTF-8 */
    OFCHECK(OFCharacterEncoding::convertToWideCharString(NULL, 1, resultPtr, resultLen).good());
    OFCHECK_EQUAL(resultLen, 0);
    delete[] resultPtr;
    OFCHECK(OFCharacterEncoding::convertToWideCharString("", 0, resultPtr, resultLen).good());
    OFCHECK_EQUAL(resultLen, 0);
    delete[] resultPtr;
    OFCHECK(OFCharacterEncoding::convertToWideCharString(OFString(""), resultPtr, resultLen).good());
    OFCHECK_EQUAL(resultLen, 0);
    delete[] resultPtr;
    OFCHECK(OFCharacterEncoding::convertToWideCharString(OFString("J\303\266rg"), resultPtr, resultLen).good());
    OFCHECK_EQUAL(resultLen, 4);
    if (resultPtr != NULL)
        OFCHECK(wcscmp(resultPtr, L"J\x00f6rg") == 0);
    delete[] resultPtr;
    // check string with embedded NULL byte
    OFCHECK(OFCharacterEncoding::convertToWideCharString(OFString("Te\0st", 5), resultPtr, resultLen).good());
    OFCHECK_EQUAL(resultLen, 5);
    /* check Windows-specific conversion to UTF-8 */
    OFCHECK(OFCharacterEncoding::convertFromWideCharString(NULL, 1, resultStr).good());
    OFCHECK(OFCharacterEncoding::convertFromWideCharString(L"", 0, resultStr).good());
    OFCHECK(OFCharacterEncoding::convertFromWideCharString(L"J\x00f6rg", 4, resultStr).good());
    OFCHECK_EQUAL(resultStr, "J\303\266rg");
    // check string with embedded NULL character
    OFCHECK(OFCharacterEncoding::convertFromWideCharString(L"Te\0st", 5, resultStr).good());
    OFCHECK_EQUAL(resultStr, OFString("Te\0st", 5));
#endif
}


OFTEST(ofstd_OFCharacterEncoding_8)
{
#ifdef _WIN32
    OFString resultStr;
    wchar_t *resultPtr;
    size_t resultLen;
    /* check Windows-specific conversion from Latin-1 */
    OFCHECK(OFCharacterEncoding::convertToWideCharString(OFString("J\366rg"), resultPtr, resultLen,
        OFCharacterEncoding::CPC_Latin1).good());
    OFCHECK_EQUAL(resultLen, 4);
    if (resultPtr != NULL)
        OFCHECK(wcscmp(resultPtr, L"J\x00f6rg") == 0);
    delete[] resultPtr;
    /* check Windows-specific conversion to Latin-1 */
    OFCHECK(OFCharacterEncoding::convertFromWideCharString(L"J\x00f6rg", 4, resultStr,
        OFCharacterEncoding::CPC_Latin1).good());
    OFCHECK_EQUAL(resultStr, "J\366rg");
#endif
}
