/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Implementation of class DcmAgeString
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvras.h"


// ********************************


DcmAgeString::DcmAgeString(const DcmTag &tag,
                           const Uint32 len)
  : DcmByteString(tag, len)
{
    setMaxLength(4);
}


DcmAgeString::DcmAgeString(const DcmAgeString &old)
  : DcmByteString(old)
{
}


DcmAgeString::~DcmAgeString()
{
}

DcmAgeString &DcmAgeString::operator=(const DcmAgeString &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


OFCondition DcmAgeString::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmAgeString &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmAgeString::ident() const
{
    return EVR_AS;
}


OFCondition DcmAgeString::checkValue(const OFString &vm,
                                     const OFBool /*oldFormat*/)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
        l_error = DcmAgeString::checkStringValue(strVal, vm);
    return l_error;
}


// ********************************


OFCondition DcmAgeString::checkStringValue(const OFString &value,
                                           const OFString &vm)
{
    return DcmByteString::checkStringValue(value, vm, "as", 1);
}
