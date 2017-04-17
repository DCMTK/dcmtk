/*
 *
 *  Copyright (C) 2015-2017, OFFIS e.V.
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
 *  Purpose: Implementation of class DcmUnlimitedCharacters
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvruc.h"


// ********************************


DcmUnlimitedCharacters::DcmUnlimitedCharacters(const DcmTag &tag,
                                               const Uint32 len)
  : DcmCharString(tag, len)
{
    setMaxLength(DCM_UndefinedLength);
}


DcmUnlimitedCharacters::DcmUnlimitedCharacters(const DcmUnlimitedCharacters &old)
  : DcmCharString(old)
{
}


DcmUnlimitedCharacters::~DcmUnlimitedCharacters()
{
}


DcmUnlimitedCharacters &DcmUnlimitedCharacters::operator=(const DcmUnlimitedCharacters &obj)
{
    DcmCharString::operator=(obj);
    return *this;
}


OFCondition DcmUnlimitedCharacters::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmUnlimitedCharacters &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmUnlimitedCharacters::ident() const
{
    return EVR_UC;
}


OFCondition DcmUnlimitedCharacters::checkValue(const OFString &vm,
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
        l_error = DcmUnlimitedCharacters::checkStringValue(strVal, vm, charset);
    }
    return l_error;
}


// ********************************


OFCondition DcmUnlimitedCharacters::getOFString(OFString &stringVal,
                                                const unsigned long pos,
                                                OFBool normalize)
{
    OFCondition l_error = DcmCharString::getOFString(stringVal, pos, normalize);
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition DcmUnlimitedCharacters::checkStringValue(const OFString &value,
                                                     const OFString &vm,
                                                     const OFString &charset)
{
    return DcmByteString::checkStringValue(value, vm, "uc", 20, 0 /* maxLen: no check */, charset);
}
