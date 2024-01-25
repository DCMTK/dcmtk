/*
 *
 *  Copyright (C) 2011-2023, OFFIS e.V.
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
 *  Purpose: Class for supporting the Specific Character Set attribute
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcspchrs.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcbytstr.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstd.h"

#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_OFICONV
#include "dcmtk/oficonv/iconv.h"

BEGIN_EXTERN_C

/* logger function that forwards oficonv log output to the dcmdata logger */
static void oficonv_logger_callback(int level, const char *text1, const char *text2, const char *text3)
{
    switch(level)
    {
        case 0:
            DCMDATA_TRACE(text1 << text2 << text3);
            break;
        case 1:
            DCMDATA_DEBUG(text1 << text2 << text3);
            break;
        case 2:
            DCMDATA_INFO(text1 << text2 << text3);
            break;
        case 3:
            DCMDATA_WARN(text1 << text2 << text3);
            break;
        case 4:
            DCMDATA_ERROR(text1 << text2 << text3);
            break;
        default:
            DCMDATA_FATAL(text1 << text2 << text3);
            break;
    }
}
END_EXTERN_C

#endif


#define MAX_OUTPUT_STRING_LENGTH 60



/*------------------*
 *  implementation  *
 *------------------*/

DcmSpecificCharacterSet::DcmSpecificCharacterSet()
  : SourceCharacterSet(),
    DestinationCharacterSet(),
    DestinationEncoding(),
    DefaultEncodingConverter(),
    EncodingConverters()
{
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_OFICONV
    // set the callback function for oficonv so that logger output goes to the dcmdata logger
    if (get_oficonv_logger_callback() == NULL)
    {
        set_oficonv_logger_callback(oficonv_logger_callback);
    }
#endif
}


DcmSpecificCharacterSet::~DcmSpecificCharacterSet()
{
    clear();
}


void DcmSpecificCharacterSet::clear()
{
    DefaultEncodingConverter.clear();
    EncodingConverters.clear();
    SourceCharacterSet.clear();
    DestinationCharacterSet.clear();
    DestinationEncoding.clear();
}


DcmSpecificCharacterSet::operator OFBool() const
{
    return OFstatic_cast(OFBool, DefaultEncodingConverter);
}


OFBool DcmSpecificCharacterSet::operator!() const
{
    return !DefaultEncodingConverter;
}


const OFString &DcmSpecificCharacterSet::getSourceCharacterSet() const
{
    return SourceCharacterSet;
}


const OFString &DcmSpecificCharacterSet::getDestinationCharacterSet() const
{
    return DestinationCharacterSet;
}


const OFString &DcmSpecificCharacterSet::getDestinationEncoding() const
{
    return DestinationEncoding;
}


unsigned DcmSpecificCharacterSet::getConversionFlags() const
{
    return DefaultEncodingConverter.getConversionFlags();
}


OFCondition DcmSpecificCharacterSet::setConversionFlags(const unsigned flags)
{
    if (!EncodingConverters.empty())
    {
        /* pass conversion flags to all "encoding converters" */
        for (T_EncodingConvertersMap::iterator it = EncodingConverters.begin();
            it != EncodingConverters.end(); ++it)
        {
            OFCondition status = it->second.setConversionFlags(flags);
            if (status.bad())
                return status;
        }
        return EC_Normal;
    } else return DefaultEncodingConverter.setConversionFlags(flags);
}


OFCondition DcmSpecificCharacterSet::selectCharacterSet(const OFString &fromCharset,
                                                        const OFString &toCharset)
{
    // first, make sure that all converters are cleared
    clear();
    // determine the destination encoding (and check whether it is supported at all)
    OFCondition status = determineDestinationEncoding(toCharset);
    if (status.good())
    {
        // normalize the given string (original VR is "CS" with VM "1-n")
        SourceCharacterSet = fromCharset;
        normalizeString(SourceCharacterSet, MULTIPART, DELETE_LEADING, DELETE_TRAILING);
        // check whether it is multi-valued
        const unsigned long sourceVM = DcmElement::determineVM(SourceCharacterSet.c_str(), SourceCharacterSet.length());
        if (sourceVM == 0)
        {
            // no character set specified, use ASCII
            status = DefaultEncodingConverter.selectEncoding("ASCII", DestinationEncoding);
            // output some useful debug information
            if (status.good())
            {
                DCMDATA_DEBUG("DcmSpecificCharacterSet: Selected character set '' (ASCII) "
                    << "for the conversion to " << DestinationEncoding);
            }
        }
        else if (sourceVM == 1)
        {
            // a single character set specified (no code extensions)
            status = selectCharacterSetWithoutCodeExtensions();
        } else {
            // multiple character sets specified (code extensions used)
            status = selectCharacterSetWithCodeExtensions(sourceVM);
        }
    }
    return status;
}


OFCondition DcmSpecificCharacterSet::selectCharacterSet(DcmItem &dataset,
                                                        const OFString &toCharset)
{
    OFString fromCharset;
    // check whether Specific Character Set (0008,0005) is present in the given item/dataset
    dataset.findAndGetOFStringArray(DCM_SpecificCharacterSet, fromCharset, OFFalse /*searchIntoSub*/);
    // if missing or empty, the default character set (ASCII) will be used
    return selectCharacterSet(fromCharset, toCharset);
}


OFCondition DcmSpecificCharacterSet::determineDestinationEncoding(const OFString &toCharset)
{
    OFCondition status = EC_Normal;
    // normalize the given string (original VR is "CS" with VM "1-n", but we only support VM "1")
    DestinationCharacterSet = toCharset;
    normalizeString(DestinationCharacterSet, !MULTIPART, DELETE_LEADING, DELETE_TRAILING);
    // there should only be a single character set specified (no code extensions)
    if (DestinationCharacterSet.empty())                // ASCII (no value)
        DestinationEncoding = "ASCII";
    else if (DestinationCharacterSet == "ISO_IR 6")     // ASCII
    {
        DCMDATA_WARN("DcmSpecificCharacterSet: 'ISO_IR 6' is not a defined term in DICOM, "
            << "will be treated as an empty value (ASCII)");
        DestinationCharacterSet.clear();
        DestinationEncoding = "ASCII";
    }
    else if (DestinationCharacterSet == "ISO_IR 100")   // Latin alphabet No. 1
        DestinationEncoding = "ISO-8859-1";
    else if (DestinationCharacterSet == "ISO_IR 101")   // Latin alphabet No. 2
        DestinationEncoding = "ISO-8859-2";
    else if (DestinationCharacterSet == "ISO_IR 109")   // Latin alphabet No. 3
        DestinationEncoding = "ISO-8859-3";
    else if (DestinationCharacterSet == "ISO_IR 110")   // Latin alphabet No. 4
        DestinationEncoding = "ISO-8859-4";
    else if (DestinationCharacterSet == "ISO_IR 144")   // Cyrillic
        DestinationEncoding = "ISO-8859-5";
    else if (DestinationCharacterSet == "ISO_IR 127")   // Arabic
        DestinationEncoding = "ISO-8859-6";
    else if (DestinationCharacterSet == "ISO_IR 126")   // Greek
        DestinationEncoding = "ISO-8859-7";
    else if (DestinationCharacterSet == "ISO_IR 138")   // Hebrew
        DestinationEncoding = "ISO-8859-8";
    else if (DestinationCharacterSet == "ISO_IR 148")   // Latin alphabet No. 5
        DestinationEncoding = "ISO-8859-9";
    else if (DestinationCharacterSet == "ISO_IR 203")   // Latin alphabet No. 9
        DestinationEncoding = "ISO-8859-15";
    else if (DestinationCharacterSet == "ISO_IR 13")    // Japanese
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_ICONV
        DestinationEncoding = "JIS_X0201";              // - the name "ISO-IR-13" is not supported by libiconv
#else
        DestinationEncoding = "Shift_JIS";              // - ICU and stdlibc iconv only know "Shift_JIS", which is a superset of JIS X0201
#endif
    else if (DestinationCharacterSet == "ISO_IR 166")   // Thai
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_ICU
        DestinationEncoding = "TIS-620";                // - the name "ISO-IR-166" is not supported by ICU
#else
        DestinationEncoding = "ISO-IR-166";
#endif
    else if (DestinationCharacterSet == "ISO_IR 192")   // Unicode in UTF-8 (multi-byte)
        DestinationEncoding = "UTF-8";
    else if (DestinationCharacterSet == "GB18030")      // Chinese (multi-byte)
        DestinationEncoding = "GB18030";
    else if (DestinationCharacterSet == "GBK")          // Chinese (multi-byte, subset of "GB 18030")
        DestinationEncoding = "GBK";
    else {
        DestinationEncoding.clear();
        // create an appropriate error code
        OFOStringStream stream;
        stream << "Cannot select destination character set: SpecificCharacterSet (0008,0005) value '"
            << DestinationCharacterSet << "' not supported" << OFStringStream_ends;
        OFSTRINGSTREAM_GETOFSTRING(stream, message)
        status = makeOFCondition(OFM_dcmdata, EC_CODE_CannotSelectCharacterSet, OF_error, message.c_str());
    }
    return status;
}


OFCondition DcmSpecificCharacterSet::selectCharacterSetWithoutCodeExtensions()
{
    OFCondition status = EC_Normal;
    // a single character set specified (no code extensions)
    OFString fromEncoding;
    if (SourceCharacterSet == "ISO_IR 6")           // ASCII
    {
        DCMDATA_WARN("DcmSpecificCharacterSet: 'ISO_IR 6' is not a defined term in DICOM, "
            << "will be treated as an empty value (ASCII)");
        SourceCharacterSet.clear();
        fromEncoding = "ASCII";
    }
    else if (SourceCharacterSet == "ISO_IR 100")    // Latin alphabet No. 1
        fromEncoding = "ISO-8859-1";
    else if (SourceCharacterSet == "ISO_IR 101")    // Latin alphabet No. 2
        fromEncoding = "ISO-8859-2";
    else if (SourceCharacterSet == "ISO_IR 109")    // Latin alphabet No. 3
        fromEncoding = "ISO-8859-3";
    else if (SourceCharacterSet == "ISO_IR 110")    // Latin alphabet No. 4
        fromEncoding = "ISO-8859-4";
    else if (SourceCharacterSet == "ISO_IR 144")    // Cyrillic
        fromEncoding = "ISO-8859-5";
    else if (SourceCharacterSet == "ISO_IR 127")    // Arabic
        fromEncoding = "ISO-8859-6";
    else if (SourceCharacterSet == "ISO_IR 126")    // Greek
        fromEncoding = "ISO-8859-7";
    else if (SourceCharacterSet == "ISO_IR 138")    // Hebrew
        fromEncoding = "ISO-8859-8";
    else if (SourceCharacterSet == "ISO_IR 148")    // Latin alphabet No. 5
        fromEncoding = "ISO-8859-9";
    else if (SourceCharacterSet == "ISO_IR 203")    // Latin alphabet No. 9
        fromEncoding = "ISO-8859-15";
    else if (SourceCharacterSet == "ISO_IR 13")     // Japanese
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_ICONV
        fromEncoding = "JIS_X0201";                 // - the name "ISO-IR-13" is not supported by libiconv
#else
        fromEncoding = "Shift_JIS";                 // - ICU and stdlibc iconv only know "Shift_JIS", which is a superset of JIS X0201
#endif
    else if (SourceCharacterSet == "ISO_IR 166")    // Thai
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_ICU
        fromEncoding = "TIS-620";                   // - the name "ISO-IR-166" is not supported by ICU
#else
        fromEncoding = "ISO-IR-166";
#endif
    else if (SourceCharacterSet == "ISO_IR 192")    // Unicode in UTF-8 (multi-byte)
        fromEncoding = "UTF-8";
    else if (SourceCharacterSet == "GB18030")       // Chinese (multi-byte)
        fromEncoding = "GB18030";
    else if (SourceCharacterSet == "GBK")           // Chinese (multi-byte, subset of "GB 18030")
        fromEncoding = "GBK";
    else {
        // create an appropriate error code
        OFOStringStream stream;
        stream << "Cannot select source character set: SpecificCharacterSet (0008,0005) value '"
            << SourceCharacterSet << "' not supported" << OFStringStream_ends;
        OFSTRINGSTREAM_GETOFSTRING(stream, message)
        status = makeOFCondition(OFM_dcmdata, EC_CODE_CannotSelectCharacterSet, OF_error, message.c_str());
    }
    // check whether an appropriate character encoding has been found
    if (!fromEncoding.empty())
    {
        status = DefaultEncodingConverter.selectEncoding(fromEncoding, DestinationEncoding);
        // output some useful debug information
        if (status.good())
        {
            DCMDATA_DEBUG("DcmSpecificCharacterSet: Selected character set '" << SourceCharacterSet
                << "' (" << fromEncoding << ") for the conversion to " << DestinationEncoding);
        }
    }
    return status;
}


OFCondition DcmSpecificCharacterSet::selectCharacterSetWithCodeExtensions(const unsigned long sourceVM)
{
    // first, check whether multiple character sets are specified (i.e. code extensions used)
    if (sourceVM <= 1)
        return EC_IllegalCall;
    // then proceed with the real work
    OFCondition status = EC_Normal;
    size_t pos = 0;
    OFBool needsASCII = OFFalse;
    OFBool notFirstValue = OFFalse;
    OFString definedTerm;
    unsigned long i = 0;
    while ((i < sourceVM) && status.good())
    {
        // extract single value from string (separated by a backslash)
        pos = DcmElement::getValueFromString(SourceCharacterSet.c_str(), pos, SourceCharacterSet.length(), definedTerm);
        if (definedTerm.empty() && (i == 0))            // assuming ASCII (according to DICOM PS 3.5)
            definedTerm = "ISO 2022 IR 6";
        // determine character encoding from DICOM defined term
        OFString encodingName;
        if (definedTerm == "ISO 2022 IR 6")             // ASCII
            encodingName = "ASCII";
        else if (definedTerm == "ISO 2022 IR 100")      // Latin alphabet No. 1
        {
            encodingName = "ISO-8859-1";
            needsASCII = OFTrue;
        }
        else if (definedTerm == "ISO 2022 IR 101")      // Latin alphabet No. 2
        {
            encodingName = "ISO-8859-2";
            needsASCII = OFTrue;
        }
        else if (definedTerm == "ISO 2022 IR 109")      // Latin alphabet No. 3
        {
            encodingName = "ISO-8859-3";
            needsASCII = OFTrue;
        }
        else if (definedTerm == "ISO 2022 IR 110")      // Latin alphabet No. 4
        {
            encodingName = "ISO-8859-4";
            needsASCII = OFTrue;
        }
        else if (definedTerm == "ISO 2022 IR 144")      // Cyrillic
        {
            encodingName = "ISO-8859-5";
            needsASCII = OFTrue;
        }
        else if (definedTerm == "ISO 2022 IR 127")      // Arabic
        {
            encodingName = "ISO-8859-6";
            needsASCII = OFTrue;
        }
        else if (definedTerm == "ISO 2022 IR 126")      // Greek
        {
            encodingName = "ISO-8859-7";
            needsASCII = OFTrue;
        }
        else if (definedTerm == "ISO 2022 IR 138")      // Hebrew
        {
            encodingName = "ISO-8859-8";
            needsASCII = OFTrue;
        }
        else if (definedTerm == "ISO 2022 IR 148")      // Latin alphabet No. 5
        {
            encodingName = "ISO-8859-9";
            needsASCII = OFTrue;
        }
        else if (definedTerm == "ISO 2022 IR 203")      // Latin alphabet No. 9
        {
            encodingName = "ISO-8859-15";
            needsASCII = OFTrue;
        }
        else if (definedTerm == "ISO 2022 IR 13")       // Japanese
        {
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_ICONV
            encodingName = "JIS_X0201";                 // - the name "ISO-IR-13" is not supported by libiconv
#else
            encodingName = "Shift_JIS";                 // - ICU and stdlibc iconv only know "Shift_JIS", which is a superset of JIS X0201
#endif
        }
        else if (definedTerm == "ISO 2022 IR 166")      // Thai
        {
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_ICU
            encodingName = "TIS-620";                   // - "ISO-IR-166" is not supported by ICU
#else
            encodingName = "ISO-IR-166";
#endif
            needsASCII = OFTrue;
        }
        else if (definedTerm == "ISO 2022 IR 87")       // Japanese (multi-byte), JIS X0208
        {
            encodingName = "ISO-IR-87";                 // - this might generate an error since "ISO-IR-87" is not supported by ICU and stdlibc iconv
            notFirstValue = OFTrue;
        }
        else if (definedTerm == "ISO 2022 IR 159")      // Japanese (multi-byte), JIS X0212
        {
            encodingName = "ISO-IR-159";                // - this might generate an error since "ISO-IR-159" is not supported by ICU and stdlibc iconv
            notFirstValue = OFTrue;
        }
        else if (definedTerm == "ISO 2022 IR 149")      // Korean (multi-byte)
        {
            encodingName = "EUC-KR";
            notFirstValue = OFTrue;
        }
        else if (definedTerm == "ISO 2022 IR 58")       // Simplified Chinese (multi-byte)
        {
            encodingName = "GB2312";
            notFirstValue = OFTrue;
        }
        else {
            // create an appropriate error code
            OFOStringStream stream;
            stream << "Cannot select source character set: SpecificCharacterSet (0008,0005) value " << (i + 1)
                << " of " << sourceVM << " '" << definedTerm << "' not supported" << OFStringStream_ends;
            OFSTRINGSTREAM_GETOFSTRING(stream, message)
            status = makeOFCondition(OFM_dcmdata, EC_CODE_CannotSelectCharacterSet, OF_error, message.c_str());
        }
        // check whether character set is allowed as the default (first value)
        if ((i == 0) && notFirstValue)
        {
            OFOStringStream stream;
            stream << "Cannot select source character set: '" << definedTerm << "' is not a allowed "
                << "as the first value in SpecificCharacterSet (0008,0005)" << OFStringStream_ends;
            OFSTRINGSTREAM_GETOFSTRING(stream, message)
            status = makeOFCondition(OFM_dcmdata, EC_CODE_CannotSelectCharacterSet, OF_error, message.c_str());
        }
        // add descriptor to the map using the defined term as a key
        if (status.good() && !encodingName.empty())
        {
            OFPair<T_EncodingConvertersMap::iterator,OFBool> conv = EncodingConverters.insert(
                OFMake_pair(definedTerm, OFCharacterEncoding()));
            // but first check whether this encoding has already been added before
            if (conv.second)
            {
                status = conv.first->second.selectEncoding(encodingName, DestinationEncoding);
                if (status.good())
                {
                    // output some useful debug information
                    DCMDATA_DEBUG("DcmSpecificCharacterSet: Added character set '" << definedTerm
                        << "' (" << encodingName << ") for the conversion to " << DestinationEncoding);
                    // also remember the default descriptor, which refers to the first character set
                    if (i == 0)
                    {
                        DefaultEncodingConverter = conv.first->second;
                        DCMDATA_TRACE("DcmSpecificCharacterSet: Also selected this character set "
                            << "(i.e. '" << definedTerm << "') as the default one");
                    }
                } else {
                    DCMDATA_ERROR("DcmSpecificCharacterSet: '" << definedTerm <<
                        "' is not supported by the utilized character set conversion library '"
                        << OFCharacterEncoding::getLibraryVersionString() << '\'');
                    EncodingConverters.erase(conv.first);
                }
            } else {
                DCMDATA_WARN("DcmSpecificCharacterSet: '" << definedTerm << "' is defined more than once "
                    << "in SpecificCharacterSet (0008,0005), ignoring the duplicate definition");
            }
        }
        ++i;
    }
    // add ASCII to the map if needed but not already there
    if (status.good() && needsASCII)
    {
        OFPair<T_EncodingConvertersMap::iterator,OFBool> conv = EncodingConverters.insert(
            OFMake_pair(OFString("ISO 2022 IR 6"), OFCharacterEncoding()));
        if (conv.second)
        {
            status = conv.first->second.selectEncoding("ASCII", DestinationEncoding);
            if (status.good())
            {
                // output some useful debug information
                DCMDATA_DEBUG("DcmSpecificCharacterSet: Added character set 'ISO 2022 IR 6' (ASCII) "
                    << "for the conversion to " << DestinationEncoding
                    << " (because it is needed for one or more of the previously added character sets)");
            } else {
                DCMDATA_ERROR("DcmSpecificCharacterSet: 'ISO 2022 IR 6' is not supported by"
                    << " the utilized character set conversion library '"
                    << OFCharacterEncoding::getLibraryVersionString() << '\'');
                EncodingConverters.erase(conv.first);
            }
        }
    }
    return status;
}


OFCondition DcmSpecificCharacterSet::convertString(const OFString &fromString,
                                                   OFString &toString,
                                                   const OFString &delimiters)
{
    // call the real method converting the given string
    return convertString(fromString.c_str(), fromString.length(), toString, delimiters);
}


OFCondition DcmSpecificCharacterSet::convertString(const char *fromString,
                                                   const size_t fromLength,
                                                   OFString &toString,
                                                   const OFString &delimiters)
{
    OFCondition status = EC_Normal;
    // check whether there are or could be any code extensions
    const OFBool hasEscapeChar = checkForEscapeCharacter(fromString, fromLength);
    if (EncodingConverters.empty() || (!hasEscapeChar && delimiters.empty()))
    {
        DCMDATA_DEBUG("DcmSpecificCharacterSet: Converting '"
            << convertToLengthLimitedOctalString(fromString, fromLength) << "'");
        // no code extensions according to ISO 2022 used - this is the simple case
        status = DefaultEncodingConverter.convertString(fromString, fromLength, toString, OFTrue /*clearMode*/);
    } else {
        if (delimiters.empty())
        {
            DCMDATA_DEBUG("DcmSpecificCharacterSet: Converting '"
                << convertToLengthLimitedOctalString(fromString, fromLength)
                << "' (with code extensions)");
        } else {
            DCMDATA_DEBUG("DcmSpecificCharacterSet: Converting '"
                << convertToLengthLimitedOctalString(fromString, fromLength)
                << "' (with " << (hasEscapeChar ? "code extensions and " : "")
                << "delimiters '" << delimiters << "')");
        }
        // code extensions according to ISO 2022 (possibly) used, so we need to check
        // for particular escape sequences in order to switch between character sets
        toString.clear();
        size_t pos = 0;
        // some (extended) character sets use more than 1 byte per character
        // (however, the default character set always uses a single byte)
        unsigned char bytesPerChar = 1;
        // check whether '=' is a delimiter, as it is used in PN values
        OFBool isFirstGroup = (delimiters.find('=') != OFString_npos);
        // by default, we expect that delimiters can be checked by their corresponding ASCII codes
        // (this implies that the default character set is not "ISO 2022 IR 87" or "ISO 2022 IR 159")
        OFBool checkDelimiters = OFTrue;
        const char *firstChar = fromString;
        const char *currentChar = fromString;
        // initially, use the default descriptor
        OFCharacterEncoding converter = DefaultEncodingConverter;
        DCMDATA_TRACE("  Starting with the default character set");
        // iterate over all characters of the string (as long as there is no error)
        while ((pos < fromLength) && status.good())
        {
            const char c0 = *currentChar++;
            // check for characters ESC, HT, LF, FF, CR or any other specified delimiter
            const OFBool isEscape = (c0 == '\033');
            const OFBool isDelimiter = checkDelimiters &&
                ((c0 == '\011') || (c0 == '\012') || (c0 == '\014') || (c0 == '\015') || (delimiters.find(c0) != OFString_npos));
            if (isEscape || isDelimiter)
            {
                // convert the sub-string (before the delimiter) with the current character set
                const size_t convertLength = currentChar - firstChar - 1;
                if (convertLength > 0)
                {
                    // output some debug information
                    DCMDATA_TRACE("    Converting sub-string '"
                        << convertToLengthLimitedOctalString(firstChar, convertLength) << "'");
                    status = converter.convertString(firstChar, convertLength, toString, OFFalse /*clearMode*/);
                    if (status.bad())
                        DCMDATA_TRACE("    -> ERROR: " << status.text());
                }
                // check whether this was the first component group of a PN value
                if (isDelimiter && (c0 == '='))
                    isFirstGroup = OFFalse;
            }
            // the ESC character is used to explicitly switch between character sets
            if (isEscape)
            {
                // report a warning as this is a violation of DICOM PS 3.5 Section 6.2.1
                if (isFirstGroup)
                {
                    DCMDATA_WARN("DcmSpecificCharacterSet: Escape sequences shall not be used "
                        << "in the first component group of a Person Name (PN), using them anyway");
                }
                // we need at least two more characters to determine the new character set
                size_t escLength = 2;
                if (pos + escLength < fromLength)
                {
                    OFString key;
                    const char c1 = *currentChar++;
                    const char c2 = *currentChar++;
                    char c3 = '\0';
                    if ((c1 == 0x28) && (c2 == 0x42))       // ASCII
                        key = "ISO 2022 IR 6";
                    else if ((c1 == 0x2d) && (c2 == 0x41))  // Latin alphabet No. 1
                        key = "ISO 2022 IR 100";
                    else if ((c1 == 0x2d) && (c2 == 0x42))  // Latin alphabet No. 2
                        key = "ISO 2022 IR 101";
                    else if ((c1 == 0x2d) && (c2 == 0x43))  // Latin alphabet No. 3
                        key = "ISO 2022 IR 109";
                    else if ((c1 == 0x2d) && (c2 == 0x44))  // Latin alphabet No. 4
                        key = "ISO 2022 IR 110";
                    else if ((c1 == 0x2d) && (c2 == 0x4c))  // Cyrillic
                        key = "ISO 2022 IR 144";
                    else if ((c1 == 0x2d) && (c2 == 0x47))  // Arabic
                        key = "ISO 2022 IR 127";
                    else if ((c1 == 0x2d) && (c2 == 0x46))  // Greek
                        key = "ISO 2022 IR 126";
                    else if ((c1 == 0x2d) && (c2 == 0x48))  // Hebrew
                        key = "ISO 2022 IR 138";
                    else if ((c1 == 0x2d) && (c2 == 0x4d))  // Latin alphabet No. 5
                        key = "ISO 2022 IR 148";
                    else if ((c1 == 0x2d) && (c2 == 0x62))  // Latin alphabet No. 9
                        key = "ISO 2022 IR 203";
                    else if ((c1 == 0x29) && (c2 == 0x49))  // Japanese, JIS X0201, G1 set (Katakana)
                        key = "ISO 2022 IR 13";
                    else if ((c1 == 0x28) && (c2 == 0x4a))  // Japanese, JIS X0201, G0 set (Romaji, i.e. ASCII)
                        key = "ISO 2022 IR 13";
                    else if ((c1 == 0x2d) && (c2 == 0x54))  // Thai
                        key = "ISO 2022 IR 166";
                    else if ((c1 == 0x24) && (c2 == 0x42))  // Japanese (multi-byte), JIS X0208 (Kanji)
                        key = "ISO 2022 IR 87";
                    else if ((c1 == 0x24) && (c2 == 0x28))  // Japanese (multi-byte), JIS X0212 (Supplementary Kanji set)
                    {
                        escLength = 3;
                        // do we still have another character in the string?
                        if (pos + escLength < fromLength)
                        {
                            c3 = *currentChar++;
                            if (c3 == 0x44)
                                key = "ISO 2022 IR 159";
                        }
                    }
                    else if ((c1 == 0x24) && (c2 == 0x29)) // might be Korean or Chinese
                    {
                        escLength = 3;
                        // do we still have another character in the string?
                        if (pos + escLength < fromLength)
                        {
                            c3 = *currentChar++;
                            if (c3 == 0x43)                // Korean (single- and multi-byte)
                                key = "ISO 2022 IR 149";
                            else if (c3 == 0x41)           // Simplified Chinese (multi-byte)
                                key = "ISO 2022 IR 58";
                        }
                    }
                    // check whether a valid escape sequence has been found
                    if (key.empty())
                    {
                        OFOStringStream stream;
                        stream << "Cannot convert character set: Illegal escape sequence 'ESC "
                            << STD_NAMESPACE dec << STD_NAMESPACE setfill('0')
                            << STD_NAMESPACE setw(2) << OFstatic_cast(int, c1 >> 4) << "/"
                            << STD_NAMESPACE setw(2) << OFstatic_cast(int, c1 & 0x0f) << " "
                            << STD_NAMESPACE setw(2) << OFstatic_cast(int, c2 >> 4) << "/"
                            << STD_NAMESPACE setw(2) << OFstatic_cast(int, c2 & 0x0f);
                        if (escLength == 3)
                        {
                            stream << " " << STD_NAMESPACE setw(2) << OFstatic_cast(int, c3 >> 4) << "/"
                                << STD_NAMESPACE setw(2) << OFstatic_cast(int, c3 & 0x0f);
                        }
                        stream  << "' found" << OFStringStream_ends;
                        OFSTRINGSTREAM_GETOFSTRING(stream, message)
                        status = makeOFCondition(OFM_dcmdata, EC_CODE_CannotConvertCharacterSet, OF_error, message.c_str());
                    }
                    if (status.good())
                    {
                        DCMDATA_TRACE("  Switching to character set '" << key << "'");
                        T_EncodingConvertersMap::const_iterator it = EncodingConverters.find(key);
                        // check whether the descriptor was found in the map, i.e. properly declared in (0008,0005)
                        if (it != EncodingConverters.end())
                        {
                            converter = it->second;
                            // special case: these Japanese character sets replace the ASCII part (G0 code area),
                            // so according to DICOM PS 3.5 Section 6.2.1.2 an explicit switch to the default is required
                            checkDelimiters = (key != "ISO 2022 IR 87") && (key != "ISO 2022 IR 159");
                            // determine number of bytes per character (used by the selected character set)
                            if ((key == "ISO 2022 IR 87") || (key == "ISO 2022 IR 159") || (key == "ISO 2022 IR 58"))
                            {
                                DCMDATA_TRACE("    Now using 2 bytes per character");
                                bytesPerChar = 2;
                            }
                            else if (key == "ISO 2022 IR 149")
                            {
                                DCMDATA_TRACE("    Now using 1 or 2 bytes per character");
                                bytesPerChar = 0;      // special handling for single- and multi-byte
                            } else {
                                DCMDATA_TRACE("    Now using 1 byte per character");
                                bytesPerChar = 1;
                            }
                        } else {
                            OFOStringStream stream;
                            stream << "Cannot convert character set: Escape sequence refers to character set '" << key << "' that "
                                "was not declared in SpecificCharacterSet (0008,0005)" << OFStringStream_ends;
                            OFSTRINGSTREAM_GETOFSTRING(stream, message)
                            status = makeOFCondition(OFM_dcmdata, EC_CODE_CannotConvertCharacterSet, OF_error, message.c_str());
                        }
                    }
                    pos += escLength;
                }
                // check whether the escape sequence was complete
                if (status.good() && (pos >= fromLength))
                {
                    OFOStringStream stream;
                    stream << "Cannot convert character set: Incomplete escape sequence (" << (escLength + 1)
                        << " bytes expected) at the end of the string to be converted" << OFStringStream_ends;
                    OFSTRINGSTREAM_GETOFSTRING(stream, message)
                    status = makeOFCondition(OFM_dcmdata, EC_CODE_CannotConvertCharacterSet, OF_error, message.c_str());
                }
                // do not copy the escape sequence to the output
                firstChar = currentChar;
            }
            // the HT, LF, FF, CR character or other delimiters (depending on the VR) also cause a switch
            else if (isDelimiter)
            {
                // output some debug information
                DCMDATA_TRACE("    Appending delimiter '"
                    << convertToLengthLimitedOctalString(currentChar - 1 /* identical to c0 */, 1)
                    << "' to the output");
                // don't forget to append the delimiter
                toString += c0;
                // use the default descriptor again (see DICOM PS 3.5)
                if (converter != DefaultEncodingConverter)
                {
                    DCMDATA_TRACE("  Switching back to the default character set (because a delimiter was found)");
                    converter = DefaultEncodingConverter;
                    checkDelimiters = OFTrue;
                }
                // start new sub-string after delimiter
                firstChar = currentChar;
            }
            // skip remaining bytes of current character (if any)
            else if (bytesPerChar != 1)
            {
                const size_t skipBytes = (bytesPerChar > 0) ? (bytesPerChar - 1) : ((c0 & 0x80) ? 1 : 0);
                if (pos + skipBytes < fromLength)
                    currentChar += skipBytes;
                pos += skipBytes;
            }
            ++pos;
        }
        if (status.good())
        {
            // convert any remaining characters from the input string
            const size_t convertLength = currentChar - firstChar;
            if (convertLength > 0)
            {
                // output some debug information
                DCMDATA_TRACE("    Converting remaining sub-string '"
                    << convertToLengthLimitedOctalString(firstChar, convertLength) << "'");
                status = converter.convertString(firstChar, convertLength, toString, OFFalse /*clearMode*/);
                if (status.bad())
                    DCMDATA_TRACE("    -> ERROR: " << status.text());
            }
        }
    }
    if (status.good())
    {
        // finally, output some debug information
        if (DestinationEncoding == "UTF-8")
        {
            // output code points only in case of UTF-8 output
            DCMDATA_TRACE("Converted result in " << DestinationEncoding << " is '"
                << convertToLengthLimitedOctalString(toString.c_str(), toString.length()) << "' ("
                << countCharactersInUTF8String(toString) << " code points)");
        } else {
            DCMDATA_TRACE("Converted result in " << DestinationEncoding << " is '"
                << convertToLengthLimitedOctalString(toString.c_str(), toString.length()) << "'");
        }
    }
    return status;
}


OFBool DcmSpecificCharacterSet::isConversionAvailable()
{
    // just call the appropriate function from the underlying class
    return OFCharacterEncoding::isLibraryAvailable();
}


size_t DcmSpecificCharacterSet::countCharactersInUTF8String(const OFString &utf8String)
{
    // just call the appropriate function from the underlying class
    return OFCharacterEncoding::countCharactersInUTF8String(utf8String);
}


OFBool DcmSpecificCharacterSet::checkForEscapeCharacter(const char *strValue,
                                                        const size_t strLength) const
{
    OFBool result = OFFalse;
    // iterate over the string of characters
    for (size_t pos = 0; pos < strLength; ++pos)
    {
        // and search for the first ESC character
        if (*strValue++ == '\033')
        {
            // then return with "true"
            result = OFTrue;
            break;
        }
    }
    return result;
}


OFString DcmSpecificCharacterSet::convertToLengthLimitedOctalString(const char *strValue,
                                                                    const size_t strLength) const
{
    OFString octalString;
    // convert given string to octal representation, allow one character more than the maximum ...
    OFStandard::convertToOctalString(OFString(strValue, strLength), octalString, MAX_OUTPUT_STRING_LENGTH + 1);
    // ... in order to determine whether trailing dots should be added, i.e. the string was cropped
    if (octalString.length() > MAX_OUTPUT_STRING_LENGTH)
    {
        octalString.erase(MAX_OUTPUT_STRING_LENGTH);
        octalString.append("...");
    }
    // return string by-value (in order to avoid another parameter)
    return octalString;
}
