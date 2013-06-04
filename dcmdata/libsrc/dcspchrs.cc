/*
 *
 *  Copyright (C) 2011-2013, OFFIS e.V.
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
 *  Purpose: Class for supporting the Specfic Character Set attribute
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcspchrs.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcbytstr.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstd.h"


#define MAX_OUTPUT_STRING_LENGTH 60


/*------------------*
 *  implementation  *
 *------------------*/

DcmSpecificCharacterSet::DcmSpecificCharacterSet()
  : SourceCharacterSet(),
    DestinationCharacterSet(),
    DestinationEncoding(),
    EncodingConverter(),
    ConversionDescriptors()
{
}


DcmSpecificCharacterSet::~DcmSpecificCharacterSet()
{
    // this frees all previously allocated ressources
    closeConversionDescriptors();
}


void DcmSpecificCharacterSet::clear()
{
    // this also clears all other member variables
    closeConversionDescriptors();
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


OFBool DcmSpecificCharacterSet::getTransliterationMode() const
{
    return EncodingConverter.getTransliterationMode();
}


OFBool DcmSpecificCharacterSet::getDiscardIllegalSequenceMode() const
{
    return EncodingConverter.getDiscardIllegalSequenceMode();
}


OFCondition DcmSpecificCharacterSet::selectCharacterSet(const OFString &fromCharset,
                                                        const OFString &toCharset,
                                                        const OFBool transliterate,
                                                        const OFBool discardIllegal)
{
    // first, make sure that all conversion descriptors are closed
    closeConversionDescriptors();
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
            status = EncodingConverter.selectEncoding("ASCII", DestinationEncoding);
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
        // enable or disable the transliteration mode
        if (status.good())
        {
            status = EncodingConverter.setTransliterationMode(transliterate);
            if (status.good())
            {
                // output some useful debug information
                if (transliterate)
                {
                    DCMDATA_DEBUG("DcmSpecificCharacterSet: Enabled transliteration mode, "
                        << "i.e. the approximation of similar looking characters will be used");
                } else {
                    DCMDATA_DEBUG("DcmSpecificCharacterSet: Disabled transliteration mode, "
                        << "i.e. the approximation of similar looking characters will not be used");
                }
            }
        }
        // enable or disable the discard illegal sequence mode
        if (status.good())
        {
            status = EncodingConverter.setDiscardIllegalSequenceMode(discardIllegal);
            if (status.good())
            {
                // output some useful debug information
                if (discardIllegal)
                {
                    DCMDATA_DEBUG("DcmSpecificCharacterSet: Enabled 'discard illegal sequence' mode, "
                        << "i.e. non-representable characters will be discarded");
                } else {
                    DCMDATA_DEBUG("DcmSpecificCharacterSet: Disabled 'discard illegal sequence' mode, "
                        << "i.e. non-representable characters will not be discarded");
                }
            }
        }
    }
    return status;
}


OFCondition DcmSpecificCharacterSet::selectCharacterSet(DcmItem &dataset,
                                                        const OFString &toCharset,
                                                        const OFBool transliterate,
                                                        const OFBool discardIllegal)
{
    OFString fromCharset;
    // check whether Specific Character Set (0008,0005) is present in the given item/dataset
    dataset.findAndGetOFStringArray(DCM_SpecificCharacterSet, fromCharset, OFFalse /*searchIntoSub*/);
    // if missing or empty, the default character set (ASCII) will be used
    return selectCharacterSet(fromCharset, toCharset, transliterate, discardIllegal);
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
    else if (DestinationCharacterSet == "ISO_IR 13")    // Japanese
        DestinationEncoding = "JIS_X0201";              // - "ISO-IR-13" is not supported by libiconv
    else if (DestinationCharacterSet == "ISO_IR 166")   // Thai
        DestinationEncoding = "ISO-IR-166";
    else if (DestinationCharacterSet == "ISO_IR 192")   // Unicode in UTF-8 (multi-byte)
        DestinationEncoding = "UTF-8";
    else if (DestinationCharacterSet == "GB18030")      // Chinese (multi-byte)
        DestinationEncoding = "GB18030";
    else if (DestinationCharacterSet == "GBK")          // Chinese (multi-byte, subset of GB18030)
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
    else if (SourceCharacterSet == "ISO_IR 13")     // Japanese
        fromEncoding = "JIS_X0201";                 // - "ISO-IR-13" is not supported by libiconv
    else if (SourceCharacterSet == "ISO_IR 166")    // Thai
        fromEncoding = "ISO-IR-166";
    else if (SourceCharacterSet == "ISO_IR 192")    // Unicode in UTF-8 (multi-byte)
        fromEncoding = "UTF-8";
    else if (SourceCharacterSet == "GB18030")       // Chinese (multi-byte)
        fromEncoding = "GB18030";
    else if (SourceCharacterSet == "GBK")           // Chinese (multi-byte, subset of GB18030)
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
        status = EncodingConverter.selectEncoding(fromEncoding, DestinationEncoding);
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
    OFCharacterEncoding::T_Descriptor descriptor;
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
        else if (definedTerm == "ISO 2022 IR 13")       // Japanese
        {
            encodingName = "JIS_X0201";                 // - "ISO-IR-13" is not supported by libiconv
        }
        else if (definedTerm == "ISO 2022 IR 166")      // Thai
        {
            encodingName = "ISO-IR-166";
            needsASCII = OFTrue;
        }
        else if (definedTerm == "ISO 2022 IR 87")       // Japanese (multi-byte)
        {
            encodingName = "ISO-IR-87";
            notFirstValue = OFTrue;
        }
        else if (definedTerm == "ISO 2022 IR 159")      // Japanese (multi-byte)
        {
            encodingName = "ISO-IR-159";
            notFirstValue = OFTrue;
        }
        else if (definedTerm == "ISO 2022 IR 149")      // Korean (multi-byte)
        {
            encodingName = "EUC-KR";                    // - is this mapping really correct?
            notFirstValue = OFTrue;                     //   "ISO-IR-149" does not work with the sample from DICOM PS 3.5
        }
        else if (definedTerm == "ISO 2022 IR 58")       // Simplified Chinese (multi-byte)
        {
            encodingName = "GB2312";                    // - should work, but not tested yet!
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
            // but first check whether this encoding has already been added before
            if (ConversionDescriptors.find(definedTerm) == ConversionDescriptors.end())
            {
                status = EncodingConverter.openDescriptor(descriptor, encodingName, DestinationEncoding);
                if (status.good())
                {
                    ConversionDescriptors[definedTerm] = descriptor;
                    // output some useful debug information
                    DCMDATA_DEBUG("DcmSpecificCharacterSet: Added character set '" << definedTerm
                        << "' (" << encodingName << ") for the conversion to " << DestinationEncoding);
                    // also remember the default descriptor, which refers to the first character set
                    if (i == 0)
                    {
                        EncodingConverter.ConversionDescriptor = descriptor;
                        DCMDATA_TRACE("DcmSpecificCharacterSet: Also selected this character set "
                            << "(i.e. '" << definedTerm << "') as the default one");
                    }
                }
            } else {
                DCMDATA_WARN("DcmSpecificCharacterSet: '" << definedTerm << "' is defined more than once "
                    << "in SpecificCharacterSet (0008,0005), ignoring the duplicate definition");
            }
        }
        ++i;
    }
    if (status.good())
    {
        // add ASCII to the map if needed but not already there
        if (needsASCII && (ConversionDescriptors.find("ISO 2022 IR 6") == ConversionDescriptors.end()))
        {
            status = EncodingConverter.openDescriptor(descriptor, "ASCII", DestinationEncoding);
            if (status.good())
            {
                ConversionDescriptors["ISO 2022 IR 6"] = descriptor;
                // output some useful debug information
                DCMDATA_DEBUG("DcmSpecificCharacterSet: Added character set 'ISO 2022 IR 6' (ASCII) "
                    << "for the conversion to " << DestinationEncoding
                    << " (because it is needed for one or more of the previously added character sets)");
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
    // check whether there are any code extensions at all
    if ((ConversionDescriptors.size() == 0) || !checkForEscapeCharacter(fromString, fromLength))
    {
        DCMDATA_DEBUG("DcmSpecificCharacterSet: Converting '"
            << convertToLengthLimitedOctalString(fromString, fromLength) << "'");
        // no code extensions according to ISO 2022 used - this is the simple case
        status = EncodingConverter.convertString(fromString, fromLength, toString, OFTrue /*clearMode*/);
    } else {
        if (delimiters.empty())
        {
            DCMDATA_DEBUG("DcmSpecificCharacterSet: Converting '"
                << convertToLengthLimitedOctalString(fromString, fromLength)
                << "' (with code extensions)");
        } else {
            DCMDATA_DEBUG("DcmSpecificCharacterSet: Converting '"
                << convertToLengthLimitedOctalString(fromString, fromLength)
                << "' (with code extensions and delimiters '" << delimiters << "')");
        }
        // code extensions according to ISO 2022 used, so we need to check for
        // particular escape sequences in order to switch between character sets
        toString.clear();
        size_t pos = 0;
        // check whether '=' is a delimiter, as it is used in PN values
        OFBool isFirstGroup = (delimiters.find('=') != OFString_npos);
        // by default, we expect that '^' and '=' (i.e. their ASCII codes) are valid PN delimiters
        // (this implies that the default character set is not "ISO 2022 IR 87" or "ISO 2022 IR 159")
        OFBool checkPNDelimiters = OFTrue;
        const char *firstChar = fromString;
        const char *currentChar = fromString;
        // initially, use the default descriptor
        OFCharacterEncoding::T_Descriptor descriptor = EncodingConverter.ConversionDescriptor;
        DCMDATA_TRACE("  Starting with the default character set");
        // iterate over all characters of the string (as long as there is no error)
        while ((pos < fromLength) && status.good())
        {
            const char c0 = *currentChar++;
            // check for characters ESC, LF, FF, CR or any other specified delimiter
            // (the PN delimiters '^' and '=' require the default character set or ASCII)
            const OFBool isEscape = (c0 == '\033');
            const OFBool isDelimiter = (c0 == '\012') || (c0 == '\014') || (c0 == '\015') ||
                ((delimiters.find(c0) != OFString_npos) && (((c0 != '^') && (c0 != '=')) || checkPNDelimiters));
            if (isEscape || isDelimiter)
            {
                // convert the sub-string (before the delimiter) with the current character set
                const size_t convertLength = currentChar - firstChar - 1;
                if (convertLength > 0)
                {
                    // output some debug information
                    DCMDATA_TRACE("    Converting sub-string '"
                        << convertToLengthLimitedOctalString(firstChar, convertLength) << "'");
                    status = EncodingConverter.convertString(descriptor, firstChar, convertLength, toString, OFFalse /*clearMode*/);
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
                    else if ((c1 == 0x29) && (c2 == 0x49))  // Japanese
                        key = "ISO 2022 IR 13";
                    else if ((c1 == 0x28) && (c2 == 0x4a))  // Japanese - is this really correct?
                        key = "ISO 2022 IR 13";
                    else if ((c1 == 0x2d) && (c2 == 0x54))  // Thai
                        key = "ISO 2022 IR 166";
                    else if ((c1 == 0x24) && (c2 == 0x42))  // Japanese (multi-byte)
                        key = "ISO 2022 IR 87";
                    else if ((c1 == 0x24) && (c2 == 0x28))  // Japanese (multi-byte)
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
                            if (c3 == 0x43)                // Korean (multi-byte)
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
                        T_DescriptorMap::const_iterator iter = ConversionDescriptors.find(key);
                        // check whether the descriptor was found in the map, i.e. properly declared in (0008,0005)
                        if (iter != ConversionDescriptors.end())
                        {
                            descriptor = iter->second;
                            // special case: these Japanese character sets replace the ASCII part (G0 code area),
                            // so according to DICOM PS 3.5 Section 6.2.1 an explicit switch to the default is required
                            checkPNDelimiters = (key != "ISO 2022 IR 87") && (key != "ISO 2022 IR 159");
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
            // the LF, FF, CR character or other delimiters (depending on the VR) also cause a switch
            else if (isDelimiter)
            {
                // output some debug information
                DCMDATA_TRACE("    Appending delimiter '"
                    << convertToLengthLimitedOctalString(currentChar - 1 /* identical to c0 */, 1)
                    << "' to the output");
                // don't forget to append the delimiter
                toString += c0;
                // use the default descriptor again (see DICOM PS 3.5)
                if (descriptor != EncodingConverter.ConversionDescriptor)
                {
                    DCMDATA_TRACE("  Switching back to the default character set (because a delimiter was found)");
                    descriptor = EncodingConverter.ConversionDescriptor;
                    checkPNDelimiters = OFTrue;
                }
                // start new sub-string after delimiter
                firstChar = currentChar;
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
                status = EncodingConverter.convertString(descriptor, firstChar, convertLength, toString, OFFalse /*clearMode*/);
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


OFBool DcmSpecificCharacterSet::isConversionLibraryAvailable()
{
    // just call the appropriate function from the underlying class
    return OFCharacterEncoding::isLibraryAvailable();
}


size_t DcmSpecificCharacterSet::countCharactersInUTF8String(const OFString &utf8String)
{
    // just call the appropriate function from the underlying class
    return OFCharacterEncoding::countCharactersInUTF8String(utf8String);
}


void DcmSpecificCharacterSet::closeConversionDescriptors()
{
    T_DescriptorMap::iterator iter = ConversionDescriptors.begin();
    T_DescriptorMap::const_iterator last = ConversionDescriptors.end();
    // iterate over the map of conversion descriptors
    while (iter != last)
    {
        // make sure that the default descriptor is not closed multiple times
        if (iter->second != EncodingConverter.ConversionDescriptor)
        {
            // and close the descriptor
            if (EncodingConverter.closeDescriptor(iter->second).bad())
            {
                DCMDATA_ERROR("DcmSpecificCharacterSet: Cannot close previously allocated "
                    << "conversion descriptor for '" << iter->first << "'");
            }
        }
        ++iter;
    }
    // clear the map
    ConversionDescriptors.clear();
    // and close the default descriptor
    if (EncodingConverter.closeDescriptor(EncodingConverter.ConversionDescriptor).bad())
        DCMDATA_ERROR("DcmSpecificCharacterSet: Cannot close currently selected conversion descriptor");
    // also clear the various character set and encoding name variables
    SourceCharacterSet.clear();
    DestinationCharacterSet.clear();
    DestinationEncoding.clear();
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
