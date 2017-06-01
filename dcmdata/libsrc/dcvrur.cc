/*
 *
 *  Copyright (C) 2014, OFFIS e.V.
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
 *  Purpose: Implementation of class DcmUniversalResourceIdentifierOrLocator
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrur.h"
#include "dcmtk/dcmdata/dcmatch.h"


// ********************************


DcmUniversalResourceIdentifierOrLocator::DcmUniversalResourceIdentifierOrLocator(const DcmTag &tag,
                                                                                 const Uint32 len)
  : DcmByteString(tag, len)
{
    setMaxLength(DCM_UndefinedLength);
}


DcmUniversalResourceIdentifierOrLocator::DcmUniversalResourceIdentifierOrLocator(const DcmUniversalResourceIdentifierOrLocator &old)
  : DcmByteString(old)
{
}


DcmUniversalResourceIdentifierOrLocator::~DcmUniversalResourceIdentifierOrLocator()
{
}


DcmUniversalResourceIdentifierOrLocator &DcmUniversalResourceIdentifierOrLocator::operator=(const DcmUniversalResourceIdentifierOrLocator &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


OFCondition DcmUniversalResourceIdentifierOrLocator::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmUniversalResourceIdentifierOrLocator &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmUniversalResourceIdentifierOrLocator::ident() const
{
    return EVR_UR;
}


OFCondition DcmUniversalResourceIdentifierOrLocator::checkValue(const OFString & /*vm*/,
                                                                const OFBool /*oldFormat*/)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
        l_error = DcmUniversalResourceIdentifierOrLocator::checkStringValue(strVal);
    return l_error;
}


unsigned long DcmUniversalResourceIdentifierOrLocator::getVM()
{
    /* value multiplicity is 1 for non-empty string, 0 otherwise */
    return (getRealLength() > 0) ? 1 : 0;
}


// ********************************


OFCondition DcmUniversalResourceIdentifierOrLocator::getOFString(OFString &strValue,
                                                                 const unsigned long /*pos*/,
                                                                 OFBool normalize)
{
    /* treat backslash as a normal character */
    return getOFStringArray(strValue, normalize);
}


OFCondition DcmUniversalResourceIdentifierOrLocator::getOFStringArray(OFString &strValue,
                                                                      OFBool normalize)
{
    /* get string value without handling the "\" as a delimiter */
    OFCondition l_error = getStringValue(strValue);
    if (l_error.good() && normalize)
        normalizeString(strValue, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition DcmUniversalResourceIdentifierOrLocator::checkStringValue(const OFString &value)
{
    return DcmByteString::checkStringValue(value, "" /* vm */, "ur", 19, 0 /* maxLen: no check */);
}


OFBool DcmUniversalResourceIdentifierOrLocator::matches(const OFString& key,
                                                        const OFString& candidate,
                                                        const OFBool enableWildCardMatching) const
{
  if (enableWildCardMatching)
    return DcmAttributeMatching::wildCardMatching(key.c_str(), key.length(), candidate.c_str(), candidate.length());
  else
    return DcmByteString::matches(key, candidate, OFFalse);
}
