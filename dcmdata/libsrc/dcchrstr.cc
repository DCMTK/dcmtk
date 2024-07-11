/*
 *
 *  Copyright (C) 1994-2022, OFFIS e.V.
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
#include "dcmtk/dcmdata/dcjson.h"     /* json helper classes */
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


OFBool DcmCharString::containsExtendedCharacters(const OFBool /*checkAllStrings*/)
{
    OFBool result = OFFalse;
    char *str = NULL;
    Uint32 len = 0;
    /* determine length in order to support possibly embedded NULL bytes */
    if (getString(str, len).good())
        result = DcmByteString::containsExtendedCharacters(str, len);
    return result;
}


OFBool DcmCharString::isAffectedBySpecificCharacterSet() const
{
    return OFTrue;
}


OFCondition DcmCharString::convertCharacterSet(DcmSpecificCharacterSet &converter)
{
    char *str = NULL;
    Uint32 len = 0;
    OFCondition status = getString(str, len);
    // do nothing if string value is empty
    if (status.good() && (str != NULL) && (len > 0))
    {
        OFString resultStr;
        // convert string to selected character string and replace the element value
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
    /* always write JSON Opener */
    DcmElement::writeJsonOpener(out, format);
    /* write element value (if non-empty) */
    if (!isEmpty())
    {
        OFString value;
        if (format.asBulkDataURI(getTag(), value))
        {
            format.printBulkDataURIPrefix(out);
            DcmJsonFormat::printString(out, value);
        }
        else
        {
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
    /* always report success */
    return EC_Normal;
}


// ********************************


const OFString& DcmCharString::getDelimiterChars() const
{
    /* use actual VR of this class (including derived ones) */
    return DcmVR(ident()).getDelimiterChars();
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
