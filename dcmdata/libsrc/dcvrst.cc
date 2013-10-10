/*
 *
 *  Copyright (C) 1994-2013, OFFIS e.V.
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
 *  Author:  Gerd Ehlers
 *
 *  Purpose: Implementation of class DcmShortText
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrst.h"


// ********************************


DcmShortText::DcmShortText(const DcmTag &tag,
                           const Uint32 len)
  : DcmCharString(tag, len)
{
    setMaxLength(1024);
}


DcmShortText::DcmShortText(const DcmShortText &old)
  : DcmCharString(old)
{
}


DcmShortText::~DcmShortText()
{
}


DcmShortText &DcmShortText::operator=(const DcmShortText &obj)
{
    DcmCharString::operator=(obj);
    return *this;
}


OFCondition DcmShortText::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmShortText &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmShortText::ident() const
{
    return EVR_ST;
}


OFCondition DcmShortText::checkValue(const OFString & /*vm*/,
                                     const OFBool /*oldFormat*/)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
    {
        OFString charset;
        /* try to determine the value of the SpecificCharacterSet element */
        getSpecificCharacterSet(charset);
        l_error = DcmShortText::checkStringValue(strVal, charset);
    }
    return l_error;
}


unsigned long DcmShortText::getVM()
{
    /* value multiplicity is 1 for non-empty string, 0 otherwise */
    return (getRealLength() > 0) ? 1 : 0;
}


// ********************************


OFCondition DcmShortText::getOFString(OFString &stringVal,
                                      const unsigned long /*pos*/,
                                      OFBool normalize)
{
    /* treat backslash as a normal character */
    return getOFStringArray(stringVal, normalize);
}


OFCondition DcmShortText::getOFStringArray(OFString &stringVal,
                                           OFBool normalize)
{
    /* get string value without handling the "\" as a delimiter */
    OFCondition l_error = getStringValue(stringVal);
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition DcmShortText::checkStringValue(const OFString &value,
                                           const OFString &charset)
{
    return DcmByteString::checkStringValue(value, "" /* vm */, "lt", 14, 0 /* maxLen: no check */, charset);
}
