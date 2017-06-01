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
 *  Purpose: class DcmCodeString
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcmatch.h"


#define MAX_CS_LENGTH 16


// ********************************


DcmCodeString::DcmCodeString(const DcmTag &tag,
                             const Uint32 len)
  : DcmByteString(tag, len)
{
    setMaxLength(MAX_CS_LENGTH);
    setNonSignificantChars(" \\");
}


DcmCodeString::DcmCodeString(const DcmCodeString &old)
  : DcmByteString(old)
{
}


DcmCodeString::~DcmCodeString()
{
}


DcmCodeString &DcmCodeString::operator=(const DcmCodeString &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


OFCondition DcmCodeString::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmCodeString &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmCodeString::ident() const
{
    return EVR_CS;
}


OFCondition DcmCodeString::checkValue(const OFString &vm,
                                      const OFBool /*oldFormat*/)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
        l_error = DcmCodeString::checkStringValue(strVal, vm);
    return l_error;
}


// ********************************


OFCondition DcmCodeString::getOFString(OFString &stringVal,
                                       const unsigned long pos,
                                       OFBool normalize)
{
    OFCondition l_error = DcmByteString::getOFString(stringVal, pos, normalize);
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFBool DcmCodeString::checkVR(const OFString &value,
                              size_t *pos,
                              const OFBool checkLength)
{
    unsigned char c;
    size_t i;
    const size_t length = value.length();
    const size_t maxlen = (length < MAX_CS_LENGTH) || (!checkLength) ? length : MAX_CS_LENGTH;
    /* iterate over all characters (up to the maximum) */
    for (i = 0; i < maxlen; i++)
    {
        c = value.at(i);
        /* check for valid CS character: A-Z, 0-9, _ and ' ' (space) */
        if ((c != ' ') && (c != '_') && !isdigit(c) && !(isalpha(c) && isupper(c)))
            break;
    }
    /* return position of first invalid character (eos if all valid) */
    if (pos != NULL)
        *pos = i;
    /* OFFalse in case of any invalid character */
    return (i == length);
}


// ********************************


OFCondition DcmCodeString::checkStringValue(const OFString &value,
                                            const OFString &vm)
{
    return DcmByteString::checkStringValue(value, vm, "cs", 10, MAX_CS_LENGTH);
}


OFBool DcmCodeString::matches(const OFString& key,
                              const OFString& candidate,
                              const OFBool enableWildCardMatching) const
{
  if (enableWildCardMatching)
    return DcmAttributeMatching::wildCardMatching(key.c_str(), key.length(), candidate.c_str(), candidate.length());
  else
    return DcmByteString::matches(key, candidate, OFFalse);
}
