/*
 *
 *  Copyright (C) 1998-2018, OFFIS e.V.
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
 *  Author:  Andrew Hewett
 *
 *  Purpose: Implementation of class DcmUnlimitedText
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrut.h"


// ********************************


DcmUnlimitedText::DcmUnlimitedText(const DcmTag &tag,
                                   const Uint32 len)
  : DcmCharString(tag, len)
{
    setMaxLength(DCM_UndefinedLength);
}


DcmUnlimitedText::DcmUnlimitedText(const DcmUnlimitedText &old)
  : DcmCharString(old)
{
}


DcmUnlimitedText::~DcmUnlimitedText()
{
}


DcmUnlimitedText &DcmUnlimitedText::operator=(const DcmUnlimitedText &obj)
{
    DcmCharString::operator=(obj);
    return *this;
}


int DcmUnlimitedText::compare(const DcmElement& rhs) const
{
    int result = DcmElement::compare(rhs);
    if (result != 0)
    {
        return result;
    }

    /* cast away constness (dcmdata is not const correct...) */
    DcmUnlimitedText* myThis = NULL;
    DcmUnlimitedText* myRhs = NULL;
    myThis = OFconst_cast(DcmUnlimitedText*, this);
    myRhs = OFstatic_cast(DcmUnlimitedText*, OFconst_cast(DcmElement*, &rhs));

    /* compare length */
    unsigned long thisLength = myThis->getLength();
    unsigned long rhsLength = myRhs->getLength();
    if (thisLength < rhsLength)
    {
        return -1;
    }
    else if (thisLength > rhsLength)
    {
        return 1;
    }

    /* check whether values are equal */
    OFString thisValue, rhsValue;
    myThis->getOFStringArray(thisValue);
    myThis->getOFStringArray(rhsValue);
    return thisValue.compare(rhsValue);
}



OFCondition DcmUnlimitedText::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmUnlimitedText &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmUnlimitedText::ident() const
{
    return EVR_UT;
}


OFCondition DcmUnlimitedText::checkValue(const OFString & /*vm*/,
                                         const OFBool /*oldFormat*/)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
    {
        OFString charset;
        /* try to determine the value of the SpecificCharacterSet element */
        if (getSpecificCharacterSet(charset) == EC_CorruptedData)
            charset = "UNKNOWN";
        l_error = DcmUnlimitedText::checkStringValue(strVal, charset);
    }
    return l_error;
}


unsigned long DcmUnlimitedText::getVM()
{
    /* value multiplicity is 1 for non-empty string, 0 otherwise */
    return (getRealLength() > 0) ? 1 : 0;
}


// ********************************


OFCondition DcmUnlimitedText::getOFString(OFString &strValue,
                                          const unsigned long /*pos*/,
                                          OFBool normalize)
{
    /* treat backslash as a normal character */
    return getOFStringArray(strValue, normalize);
}


OFCondition DcmUnlimitedText::getOFStringArray(OFString &strValue,
                                               OFBool normalize)
{
    /* get string value without handling the "\" as a delimiter */
    OFCondition l_error = getStringValue(strValue);
    if (l_error.good() && normalize)
        normalizeString(strValue, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition DcmUnlimitedText::checkStringValue(const OFString &value,
                                               const OFString &charset)
{
    return DcmByteString::checkStringValue(value, "" /* vm */, "lt", 14, 0 /* maxLen: no check */, charset);
}
