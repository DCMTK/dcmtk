/*
 *
 *  Copyright (C) 1994-2017, OFFIS e.V.
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
 *  Purpose: Implementation of class DcmApplicationEntity
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrae.h"
#include "dcmtk/dcmdata/dcmatch.h"


#define MAX_AE_LENGTH 16


// ********************************


DcmApplicationEntity::DcmApplicationEntity(const DcmTag &tag,
                                           const Uint32 len)
  : DcmByteString(tag, len)
{
    setMaxLength(MAX_AE_LENGTH);
    setNonSignificantChars(" \\");
}


DcmApplicationEntity::DcmApplicationEntity(const DcmApplicationEntity &old)
  : DcmByteString(old)
{
}


DcmApplicationEntity::~DcmApplicationEntity()
{
}


DcmApplicationEntity &DcmApplicationEntity::operator=(const DcmApplicationEntity &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


OFCondition DcmApplicationEntity::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmApplicationEntity &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmApplicationEntity::ident() const
{
    return EVR_AE;
}


OFCondition DcmApplicationEntity::checkValue(const OFString &vm,
                                             const OFBool /*oldFormat*/)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
        l_error = DcmApplicationEntity::checkStringValue(strVal, vm);
    return l_error;
}


// ********************************


OFCondition DcmApplicationEntity::getOFString(OFString &stringVal,
                                              const unsigned long pos,
                                              OFBool normalize)
{
    /* call inherited method */
    OFCondition l_error = DcmByteString::getOFString(stringVal, pos, normalize);
    /* normalize string if required */
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition DcmApplicationEntity::checkStringValue(const OFString &value,
                                                   const OFString &vm)
{
    return DcmByteString::checkStringValue(value, vm, "ae", 13, MAX_AE_LENGTH);
}


OFBool DcmApplicationEntity::matches(const OFString& key,
                                     const OFString& candidate,
                                     const OFBool enableWildCardMatching) const
{
  if (enableWildCardMatching)
    return DcmAttributeMatching::wildCardMatching(key.c_str(), key.length(), candidate.c_str(), candidate.length());
  else
    return DcmByteString::matches(key, candidate, OFFalse);
}
