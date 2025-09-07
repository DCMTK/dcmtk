/*
 *
 *  Copyright (C) 1994-2025, OFFIS e.V.
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
 *  Author:  Andreas Barth
 *
 *  Purpose: Implementation of class DcmCharString
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcspchrs.h"   /* for class DcmSpecificCharacterSet */
#include "dcmtk/dcmdata/dcitem.h"     /* for class DcmItem */
#include "dcmtk/dcmdata/dcdeftag.h"   /* for tag definitions */
#include "dcmtk/dcmdata/dcjson.h"     /* JSON helper classes */
#include "dcmtk/dcmdata/dcmatch.h"

//
// This implementation does not support 16 bit character sets. Since 8 bit
// character sets are supported by the class DcmByteString the class
// DcmCharString is derived from DcmByteString without any extensions.
// No special implementation is necessary.
//
// If the extension for > 8 bit character sets will be implemented this class
// must be derived directly from DcmElement. This class is designed to support
// the value representations (LO, LT, PN, SH, ST, UC and UT). They are a problem
// because their value width (1, 2, ... bytes) is specified by the element
// SpecificCharacterSet (0008, 0005) and an implementation must support
// different value widths that cannot be derived from the value representation.
//


#include "dcmtk/dcmdata/dcchrstr.h"

static unsigned long getMaximumNumberOfComponents(const OFString& s, Uint32 len)
{
    // a byte representing a backslash may also be part of a multi-byte character,
    // so the found value may be higher than the real VM
    unsigned long vm = 1;
    const char *p = s.c_str();
    for (size_t i = 0; i < len; i++)
        if (*p++ == '\\')
            ++vm;
    return vm;
}

static OFCondition getOFStringAtIndex(
    OFString& stringVal, const unsigned long pos, const char *str, Uint32 len)
{
    // works for single-byte encodings
    const char *p = str;
    const char *start = str;
    unsigned long vm = 1;
    for (size_t i = 0; i < len; i++)
    {
        if (*p++ == '\\')
        {
            if (pos == vm)
                start = p;
            else if (pos + 1 == vm)
            {
                stringVal.assign(start, p - 1);
                return EC_Normal;
            }
            ++vm;
        }
    }
    if (pos + 1 == vm)
    {
        stringVal.assign(start, str + len);
        return EC_Normal;
    }
    if (pos > 0)
        return EC_IllegalParameter;
    stringVal.clear();
    return EC_Normal;
}


// ********************************

// helper functions dealing with specific character sets

static OFBool isNonAsciiConformMultiByteSingleValueCharacterSet(const OFString& charset)
{
    // Returns the multi-byte character set names that are only allowed as a single value
    // in SpecificCharacterSet and that may contain bytes that look like ASCII (e.g.
    // with the highest bit cleared), but are part of a multi-byte non-ASCII character.
    // This includes only Chinese encodings, as in UTF-8 (ISO_IR 192) any byte belonging
    // to a non-ASCII character has the highest bit set.
    return charset == "GBK" || charset == "GB18030";
}

static OFBool isMultiValuedCharacterSet(const OFString& charset)
{
    return charset.find('\\') != OFString_npos;
}

static void skipMultiByteEscapeSequence(const char *&p, size_t &i, const Uint32 len)
{
    if ((*p != 0x1b) || (i >= len - 2))
        return;

    // found an escape sequence, check if it is for a multi-byte encoding
    ++i;
    // The escape sequence for the following encodings starts with "$":
    // ISO 2022 IR 87, ISO 2022 IR 159, ISO 2022 IR 149, ISO 2022 IR 58
    bool isMultiByte = *++p == '$';
    if (!isMultiByte && *p == '-')
    {
        ++i;
        isMultiByte = *++p == 'T'; // ISO 2022 IR 166
    }
    if (!isMultiByte)
        return;

    // we are inside a part encoded using a multi-byte extension,
    // skip until the next escape sequence or the end of the value
    while (++i < len - 2 && *p++ != 0x1b) {}
}

// ********************************


DcmCharString::DcmCharString(const DcmTag &tag, const Uint32 len)
  : DcmByteString(tag, len)
{
}

DcmCharString::DcmCharString(const DcmCharString &old)
  : DcmByteString(old)
{
}

DcmCharString::~DcmCharString(void)
{
}


DcmCharString &DcmCharString::operator=(const DcmCharString &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


OFCondition DcmCharString::copyFrom(const DcmObject& rhs)
{
    if (this != &rhs)
    {
        if (rhs.ident() != ident()) return EC_IllegalCall;
        *this = OFstatic_cast(const DcmCharString &, rhs);
    }
    return EC_Normal;
}


// ********************************


OFCondition DcmCharString::verify(const OFBool autocorrect)
{
    const Uint32 maxLen = getMaxLength();
    char *str = NULL;
    Uint32 len = 0;
    /* get string data */
    errorFlag = getString(str, len);
    /* check for non-empty string */
    if ((str != NULL) && (len > 0))
    {
        const unsigned long vm = getVM();
        /* check whether there is anything to verify at all */
        if (maxLen != DCM_UndefinedLength)
        {
            /* TODO: is it really a good idea to create a copy of the string? */
            OFString value(str, len);
            size_t posStart = 0;
            unsigned long vmNum = 0;
            /* check all string components */
            while (posStart != OFString_npos)
            {
                ++vmNum;
                /* search for next component separator */
                const size_t posEnd = (vm > 1) ? value.find('\\', posStart) : OFString_npos;
                const size_t fieldLen = (posEnd == OFString_npos) ? value.length() - posStart : posEnd - posStart;
                /* check size limit for each string component */
                if (fieldLen > maxLen)
                {
                    DCMDATA_DEBUG("DcmCharString::verify() maximum length violated in element "
                        << getTagName() << " " << getTag() << " value " << vmNum << ": "
                        << fieldLen << " bytes found but only " << maxLen << " characters allowed");
                    errorFlag = EC_MaximumLengthViolated;
                    if (autocorrect)
                    {
                        /*  TODO: We are currently not removing any characters since we do not
                         *        know whether a character consists of one or more bytes.
                         *        This will be fixed in a future version.
                         */
                        DCMDATA_DEBUG("DcmCharString::verify() not correcting value length since "
                            << "multi-byte character sets are not yet supported, so cannot decide");
                    }
                }
                posStart = (posEnd == OFString_npos) ? posEnd : posEnd + 1;
            }
        }
    }
    /* report a debug message if an error occurred */
    if (errorFlag.bad())
    {
        DCMDATA_WARN("DcmCharString: One or more illegal values in element "
            << getTagName() << " " << getTag() << " with VM=" << getVM());
        /* do not return with an error since we do not know whether there really is a violation */
        errorFlag = EC_Normal;
    }
    return errorFlag;
}


// ********************************


unsigned long DcmCharString::getVM()
{
    // the vast majority of values have VM 0 or 1, so optimize for these
    char *str = NULL;
    Uint32 len = 0;
    OFCondition result = getString(str, len);
    if (!result.good() || (str == NULL) || (len == 0))
        return 0;

    if (!supportsMultiValue())
        return 1;

    unsigned long vm = getMaximumNumberOfComponents(str, len);
    if (vm == 1 || !containsExtendedCharacters())
        return vm;

    // We have a string containing extended characters and possibly backslashes -
    // now we have to get the Specific Character Set to filter out bytes with the
    // value for backslash (0x5C) that are part of a multi-byte character.
    OFString charset;
    result = getSpecificCharacterSet(charset);
    if (!result.good() || charset.empty())
        return vm;

    if (isMultiValuedCharacterSet(charset) || isNonAsciiConformMultiByteSingleValueCharacterSet(charset))
    {
        const char *p = str;
        vm = 1;
        while ((p = findNextComponentPosition(p, len - OFstatic_cast(Uint32, p - str), charset)) != NULL)
            ++vm;
        return vm;
    }

    return vm;
}


// ********************************


OFCondition DcmCharString::getOFString(OFString& stringVal, const unsigned long pos, OFBool /*normalize*/)
{
    char *str = NULL;
    Uint32 len = 0;
    OFCondition result = getString(str, len);
    if (result.bad())
        return result;

    if ((str == NULL) || (len == 0))
    {
        if (pos > 0)
            return EC_IllegalParameter;
        stringVal.clear();
        return EC_Normal;
    }

    if (!supportsMultiValue() || getMaximumNumberOfComponents(str, len) == 0)
    {
        if (pos > 0)
            return EC_IllegalParameter;
        stringVal.assign(str, str + len);
        return EC_Normal;
    }

    // only check for multi-byte character sets if the value contains any non-ASCII characters
    // oe Escape sequences
    if (containsExtendedCharacters())
    {
        // We have a string containing extended characters and possibly backslashes -
        // now we have to get the Specific Character Set to filter out bytes with the
        // value for backslash (0x5C) that are part of a multi-byte character.
        OFString charset;
        result = getSpecificCharacterSet(charset);
        if (result.good() && !charset.empty() &&
            (isMultiValuedCharacterSet(charset) || isNonAsciiConformMultiByteSingleValueCharacterSet(charset)))
        {
            const char *p = str;
            unsigned long index = 0;
            while (index < pos &&
                (p = findNextComponentPosition(p, len - OFstatic_cast(Uint32, p - str), charset)) != NULL)
                ++index;
            if (p == NULL)
                return EC_IllegalParameter;
            if (p - str == len)
                stringVal.clear();
            else
            {
                const char *end = findNextComponentPosition(p, len - OFstatic_cast(Uint32, p - str), charset);
                if (end == NULL)
                    end = str + len + 1;
                // account for the backslash before the end pointer
                stringVal.assign(p, end - 1);
            }
            return EC_Normal;
        }
    }
    // single-byte, single-value encoding, or value without extended characters
    return getOFStringAtIndex(stringVal, pos, str, len);
}


// ********************************


OFBool DcmCharString::isAffectedBySpecificCharacterSet() const
{
    return OFTrue;
}


// ********************************


OFCondition DcmCharString::convertCharacterSet(DcmSpecificCharacterSet &converter)
{
    char *str = NULL;
    Uint32 len = 0;
    OFCondition status = getString(str, len);
    // do nothing if string value is empty
    if (status.good() && (str != NULL) && (len > 0))
    {
        OFString resultStr;
        status = converter.convertString(str, len, resultStr, getDelimiterChars());
        if (status.good())
        {
            // check whether the value has changed during the conversion (slows down the process?)
            if (OFString(str, len) != resultStr)
            {
                DCMDATA_TRACE("DcmCharString::convertCharacterSet() updating value of element "
                    << getTagName() << " " << getTag() << " after the conversion to "
                    << converter.getDestinationEncoding() << " encoding");
                // update the element value
                status = putOFStringArray(resultStr);
            } else {
                DCMDATA_TRACE("DcmCharString::convertCharacterSet() not updating value of element "
                    << getTagName() << " " << getTag() << " because the value has not changed");
            }
        }
    }
    return status;
}


// ********************************


OFCondition DcmCharString::getSpecificCharacterSet(OFString &charset)
{
    OFCondition status = EC_CorruptedData;
    // start with current dataset-level
    DcmItem *item = getParentItem();
    while ((item != NULL) && status.bad())
    {
        // check whether the attribute SpecificCharacterSet should be present at all
        if (item->checkForSpecificCharacterSet())
        {
            // by default, the string components are normalized (i.e. padding is removed)
            status = item->findAndGetOFStringArray(DCM_SpecificCharacterSet, charset);
        }
        // if element could not be found, go one level up
        if (status.bad())
            item = item->getParentItem();
    }
    // output some debug information
    if (status.good())
    {
        DCMDATA_TRACE("DcmCharString::getSpecificCharacterSet() element " << getTagName()
            << " " << getTag() << " uses character set \"" << charset << "\"");
    }
    return status;
}


// ********************************


OFCondition DcmCharString::writeJson(STD_NAMESPACE ostream &out,
    DcmJsonFormat &format)
{
    OFCondition result = EC_Normal;

    /* always write JSON Opener */
    DcmElement::writeJsonOpener(out, format);

    /* write element value (if non-empty) */
    if (!isEmpty())
    {
        if (format.asBulkDataURI(getTag(), getLength()))
        {
            /* adjust byte order to little endian */
            Uint8 *byteValues = OFstatic_cast(Uint8 *, getValue(EBO_LittleEndian));
            result = format.writeBulkData(out, getTag(), getLengthField(), byteValues);
        }
        else
        {
            OFString value;
            OFCondition status = getOFString(value, 0L);
            if (status.bad())
                return status;
            format.printValuePrefix(out);
            DcmJsonFormat::printValueString(out, value);
            const unsigned long vm = getVM();
            for (unsigned long valNo = 1; valNo < vm; ++valNo)
            {
                status = getOFString(value, valNo);
                if (status.bad())
                    return status;
                format.printNextArrayElementPrefix(out);
                DcmJsonFormat::printValueString(out, value);
            }
            format.printValueSuffix(out);
        }
    }

    /* write JSON Closer  */
    DcmElement::writeJsonCloser(out, format);
    return result;
}


// ********************************


const OFString& DcmCharString::getDelimiterChars() const
{
    /* use actual VR of this class (including derived ones) */
    return DcmVR(ident()).getDelimiterChars();
}

const char* DcmCharString::findNextComponentPosition(const char* str, Uint32 len, const OFString& charSet) const
{
    if (charSet.empty())
        return DcmByteString::findNextComponentPosition(str, len, charSet);

    const char *p = str;
    if (isNonAsciiConformMultiByteSingleValueCharacterSet(charSet))
    {
        // special handling to find real backslashes in chinese multi-bytes encodings;
        // the first byte for 2-byte characters, and the first and third bytes of 4-byte
        // characters are always > 0x80, so we can exclude these characters
        for (size_t i = 0; i < len; ++i, ++p)
        {
            if (*p == '\\')
                return p + 1;
            if ((*p & 0x80) != 0)
            {
                // this is a 2-byte character or the first or second part
                // of a 4-byte character - skip the next byte
                ++p;
            }
        }
        return NULL;
    }

    for (size_t i = 0; i < len; ++i, ++p)
    {
        if (*p == '\\')
            return p + 1;
        skipMultiByteEscapeSequence(p, i, len);
        ++p;
    }

    return NULL;
}

OFBool DcmCharString::isUniversalMatch(const OFBool normalize,
                                       const OFBool enableWildCardMatching)
{
  if(!isEmpty(normalize))
  {
    if(enableWildCardMatching)
    {
      OFString value;
      for(unsigned long valNo = 0; valNo < getVM(); ++valNo)
      {
        getOFString(value, valNo, normalize);
        if(value.find_first_not_of( '*' ) != OFString_npos)
          return OFFalse;
      }
    }
    else
      return OFFalse;
  }
  return OFTrue;
}


OFBool DcmCharString::matches(const OFString& key,
                              const OFString& candidate,
                              const OFBool enableWildCardMatching) const
{
  if (enableWildCardMatching)
    return DcmAttributeMatching::wildCardMatching(key.c_str(), key.length(), candidate.c_str(), candidate.length());
  else
    return DcmByteString::matches(key, candidate, OFFalse);
}
