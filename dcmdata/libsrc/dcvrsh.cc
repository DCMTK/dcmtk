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
 *  Purpose: Implementation of class DcmShortString
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrsh.h"


// ********************************


DcmShortString::DcmShortString(const DcmTag &tag,
                               const Uint32 len)
  : DcmCharString(tag, len)
{
    setMaxLength(16);
    setNonSignificantChars(" \\");
    setDelimiterChars("\\");
}


DcmShortString::DcmShortString(const DcmShortString &old)
  : DcmCharString(old)
{
}


DcmShortString::~DcmShortString()
{
}


DcmShortString &DcmShortString::operator=(const DcmShortString &obj)
{
    DcmCharString::operator=(obj);
    return *this;
}


OFCondition DcmShortString::copyFrom(const DcmObject& rhs)
{
    if (this != &rhs)
    {
        if (rhs.ident() != ident()) return EC_IllegalCall;
        *this = OFstatic_cast(const DcmShortString &, rhs);
    }
    return EC_Normal;
}


// ********************************


DcmEVR DcmShortString::ident() const
{
    return EVR_SH;
}


OFCondition DcmShortString::checkValue(const OFString &vm,
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
        l_error = DcmShortString::checkStringValue(strVal, vm, charset);
    }
    return l_error;
}


// ********************************


OFCondition DcmShortString::getOFString(OFString &stringVal,
                                        const unsigned long pos,
                                        OFBool normalize)
{
    OFCondition l_error = DcmCharString::getOFString(stringVal, pos, normalize);
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition DcmShortString::checkStringValue(const OFString &value,
                                             const OFString &vm,
                                             const OFString &charset)
{
    return DcmByteString::checkStringValue(value, vm, "lo", 12, 0 /* maxLen: no check */, charset);
}
