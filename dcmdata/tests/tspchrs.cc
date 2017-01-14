/*
 *
 *  Copyright (C) 2011-2017, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: test program for class DcmSpecificCharacterSet
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcspchrs.h"
#include "dcmtk/dcmdata/dctypes.h"


OFTEST(dcmdata_specificCharacterSet_1)
{
    DcmSpecificCharacterSet converter;
    if (converter.isConversionAvailable())
    {
        // try to select all DICOM character sets without code extensions
        OFCHECK(converter.selectCharacterSet("").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 6  ").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 100").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 101").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 109").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 110").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 144").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 127").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 126").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 138").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 148").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 13 ").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 166").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 192").good());
        OFCHECK(converter.selectCharacterSet("  GB18030 ").good());
        OFCHECK(converter.selectCharacterSet("    GBK   ").good());
        // try to select all DICOM character sets with code extensions
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 6  ").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 100").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 101").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 109").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 110").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 144").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 127").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 126").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 138").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 148").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 13 ").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 166").good());
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_ICONV
        // (only the ICONV library supports these character sets)
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 87 ").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 159").good());
#endif
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 149").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 58 ").good());
        // the following should fail
        OFCHECK(converter.selectCharacterSet("DCMTK").bad());
        OFCHECK(converter.selectCharacterSet(" \\ ").bad());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 999").bad());
        OFCHECK(converter.selectCharacterSet("\\ISO_IR 100").bad());
        OFCHECK(converter.selectCharacterSet("ISO 2022 IR 87\\ISO 2022 IR 100").bad());
        OFCHECK(converter.selectCharacterSet("ISO 2022 IR 159\\ISO 2022 IR 100").bad());
        OFCHECK(converter.selectCharacterSet("ISO 2022 IR 149\\ISO 2022 IR 100").bad());
        // the following should work
        OFCHECK(converter.selectCharacterSet("ISO 2022 IR 6\\ISO 2022 IR 100").good());
        OFCHECK(converter.selectCharacterSet("ISO 2022 IR 100\\ISO 2022 IR 126").good());
        OFCHECK(converter.selectCharacterSet("ISO 2022 IR 144\\ISO 2022 IR 138").good());
        // use a different destination character set than the default (UTF-8)
        OFCHECK(converter.selectCharacterSet("", "ISO_IR 100").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 6", "ISO_IR 100").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 192", " ISO_IR 100 ").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 192", "ISO 2022 IR 100\\ISO 2022 IR 126").bad());
    } else {
        // in case there is no libiconv, report a warning but do not fail
        DCMDATA_WARN("Cannot test DcmSpecificCharacterSet since the underlying character set conversion library is not available");
    }
}

OFTEST(dcmdata_specificCharacterSet_2)
{
    DcmSpecificCharacterSet converter;
    if (converter.isConversionAvailable())
    {
        OFString resultStr;
        // check whether string conversion from Latin-1 to UTF-8 works
        OFCHECK(converter.selectCharacterSet("ISO_IR 100").good());
        OFCHECK(converter.convertString("J\366rg", resultStr).good());
        OFCHECK_EQUAL(resultStr, "J\303\266rg");
        OFCHECK(converter.convertString("J\351r\364me", resultStr).good());
        OFCHECK_EQUAL(resultStr, "J\303\251r\303\264me");
        // check whether string conversion from UTF-8 to UTF-8 works :-)
        OFCHECK(converter.selectCharacterSet("ISO_IR 192").good());
        OFCHECK(converter.convertString("J\303\266rg", resultStr).good());
        OFCHECK_EQUAL(resultStr, "J\303\266rg");
        OFCHECK(converter.convertString("J\303\251r\303\264me", resultStr).good());
        OFCHECK_EQUAL(resultStr, "J\303\251r\303\264me");
        // the following should fail
        converter.clear();
        OFCHECK(converter.convertString("Some Text", resultStr).bad());
    } else {
        // in case there is no libiconv, report a warning but do not fail
        DCMDATA_WARN("Cannot test DcmSpecificCharacterSet since the underlying character set conversion library is not available");
    }
}


OFTEST(dcmdata_specificCharacterSet_3)
{
    DcmSpecificCharacterSet converter;
    if (converter.isConversionAvailable())
    {
        OFString resultStr;
        // we need the PN delimiters for a standard comformant conversion
        const OFString delimiters("\\^=");
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_ICONV
        // check whether string conversion from Japanese language to UTF-8 works
        // (only the ICONV library supports these character sets)
        // example taken from DICOM PS 3.5 Annex H.3.1
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 87").good());
        OFCHECK(converter.convertString("Yamada^Tarou=\033$B;3ED\033(B^\033$BB@O:\033(B=\033$B$d$^$@\033(B^\033$B$?$m$&\033(B", resultStr, delimiters).good());
        OFCHECK_EQUAL(resultStr, "Yamada^Tarou=\345\261\261\347\224\260^\345\244\252\351\203\216=\343\202\204\343\201\276\343\201\240^\343\201\237\343\202\215\343\201\206");
        // example taken from DICOM PS 3.5 Annex H.3.2
        OFCHECK(converter.selectCharacterSet("ISO 2022 IR 13\\ISO 2022 IR 87").good());
        OFCHECK(converter.convertString("\324\317\300\336^\300\333\263=\033$B;3ED\033(J^\033$BB@O:\033(J=\033$B$d$^$@\033(J^\033$B$?$m$&\033(J", resultStr, delimiters).good());
        OFCHECK_EQUAL(resultStr, "\357\276\224\357\276\217\357\276\200\357\276\236^\357\276\200\357\276\233\357\275\263=\345\261\261\347\224\260^\345\244\252\351\203\216=\343\202\204\343\201\276\343\201\240^\343\201\237\343\202\215\343\201\206");
#endif
        // check whether string conversion from Korean language to UTF-8 works
        // example taken from DICOM PS 3.5 Annex I.2
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 149").good());
        OFCHECK(converter.convertString("Hong^Gildong=\033$)C\373\363^\033$)C\321\316\324\327=\033$)C\310\253^\033$)C\261\346\265\277", resultStr, delimiters).good());
        OFCHECK_EQUAL(resultStr, "Hong^Gildong=\346\264\252^\345\220\211\346\264\236=\355\231\215^\352\270\270\353\217\231");
        // modified example: check for 8-bit mode (using ASCII characters)
        OFCHECK(converter.convertString("Hong^Gildong=\033$)C\373\363^\033$)C\321\316ASCII\324\327=\033$)C\310\253^\033$)C\261\346\265\277", resultStr, delimiters).good());
        OFCHECK_EQUAL(resultStr, "Hong^Gildong=\346\264\252^\345\220\211ASCII\346\264\236=\355\231\215^\352\270\270\353\217\231");
        // check whether string conversion from Chinese language to UTF-8 works
        // example taken from DICOM PS 3.5 Annex J.3
        OFCHECK(converter.selectCharacterSet("GB18030").good());
        OFCHECK(converter.convertString("Wang^XiaoDong=\315\365^\320\241\266\253=", resultStr, delimiters).good());
        OFCHECK_EQUAL(resultStr, "Wang^XiaoDong=\347\216\213^\345\260\217\344\270\234=");
        // check whether string conversion from Chinese language to UTF-8 works
        // example taken from DICOM PS 3.5 Annex K.2
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 58").good());
        OFCHECK(converter.convertString("Zhang^XiaoDong=\033\044\051\101\325\305\136\033\044\051\101\320\241\266\253=", resultStr, delimiters).good());
        OFCHECK_EQUAL(resultStr, "Zhang^XiaoDong=\345\274\240\136\345\260\217\344\270\234=");
        // check whether CR and LF are detected correctly
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 13").good());
        OFCHECK(converter.convertString("Japanese\r\033(J\324\317\300\336\nText", resultStr).good());
        OFCHECK_EQUAL(resultStr, "Japanese\015\357\276\224\357\276\217\357\276\200\357\276\236\012Text");
        // same with '\' (backslash), which is used for separating multiple values
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 13").good());
        OFCHECK(converter.convertString("Japanese\\\033(J\324\317\300\336\\Text", resultStr, "\\").good());
        OFCHECK_EQUAL(resultStr, "Japanese\\\357\276\224\357\276\217\357\276\200\357\276\236\\Text");
        // the following should fail (wrong character set)
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 166").good());
        OFCHECK(converter.convertString("Yamada^Tarou=\033$B;3ED\033(B^\033$BB@O:\033(B=\033$B$d$^$@\033(B^\033$B$?$m$&\033(B", resultStr, delimiters).bad());
    } else {
        // in case there is no libiconv, report a warning but do not fail
        DCMDATA_WARN("Cannot test DcmSpecificCharacterSet since the underlying character set conversion library is not available");
    }
}


OFTEST(dcmdata_specificCharacterSet_4)
{
    DcmSpecificCharacterSet converter;
    if (converter.isConversionAvailable())
    {
        OFString resultStr;
        // check whether string conversion from UTF-8 to Latin-1 works
        OFCHECK(converter.selectCharacterSet("ISO_IR 192", "ISO_IR 100").good());
        OFCHECK(converter.convertString("J\303\266rg", resultStr).good());
        OFCHECK_EQUAL(resultStr, "J\366rg");
        OFCHECK(converter.convertString("J\303\251r\303\264me", resultStr).good());
        OFCHECK_EQUAL(resultStr, "J\351r\364me");
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_ICONV
        // (only the ICONV library supports this character set)
        // the following should fail
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 87", "ISO_IR 100").good());
        OFCHECK(converter.convertString("Yamada^Tarou=\033$B;3ED\033(B^\033$BB@O:\033(B=\033$B$d$^$@\033(B^\033$B$?$m$&\033(B", resultStr).bad());
#endif
        OFCHECK(converter.selectCharacterSet("ISO_IR 100", "").good());
        if (OFCharacterEncoding::supportsConversionFlags(OFCharacterEncoding::AbortTranscodingOnIllegalSequence))
        {
            OFCHECK(converter.setConversionFlags(OFCharacterEncoding::AbortTranscodingOnIllegalSequence).good());
            OFCHECK(converter.convertString("J\366rg", resultStr).bad());
        }
        if (OFCharacterEncoding::supportsConversionFlags(OFCharacterEncoding::DiscardIllegalSequences))
        {
            OFCHECK(converter.setConversionFlags(OFCharacterEncoding::DiscardIllegalSequences).good());
            OFCHECK(converter.convertString("J\366rg", resultStr).good());
            OFCHECK_EQUAL(resultStr, "Jrg"); // discard mode removes characters that cannot be "translated"
        }
        if (OFCharacterEncoding::supportsConversionFlags(OFCharacterEncoding::TransliterateIllegalSequences))
        {
            OFCHECK(converter.setConversionFlags(OFCharacterEncoding::TransliterateIllegalSequences).good());
            OFCHECK(converter.convertString("J\366rg", resultStr).good());
            OFCHECK_EQUAL(resultStr, "J\"org");
        }
    } else {
        // in case there is no libiconv, report a warning but do not fail
        DCMDATA_WARN("Cannot test DcmSpecificCharacterSet since the underlying character set conversion library is not available");
    }
}
