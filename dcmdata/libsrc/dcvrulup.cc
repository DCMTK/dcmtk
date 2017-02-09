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
 *  Purpose: Implementation of class DcmUnsignedLongOffset
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcvrulup.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


// ********************************


DcmUnsignedLongOffset::DcmUnsignedLongOffset(const DcmTag &tag)
  : DcmUnsignedLong(tag, 0),
    nextRecord(NULL)
{
}


DcmUnsignedLongOffset::DcmUnsignedLongOffset(const DcmTag &tag,
                                             const Uint32 len)
  : DcmUnsignedLong(tag, len),
    nextRecord(NULL)
{
}


DcmUnsignedLongOffset::DcmUnsignedLongOffset(const DcmUnsignedLongOffset &old)
  : DcmUnsignedLong(old),
    nextRecord(old.nextRecord)
{
}


DcmUnsignedLongOffset& DcmUnsignedLongOffset::operator=(const DcmUnsignedLongOffset& obj)
{
  if (this != &obj)
  {
    // copy parent's member variables
    DcmUnsignedLong::operator=(obj);
    // copy member variables
    nextRecord = obj.nextRecord;
  }
  return *this;
}


DcmUnsignedLongOffset::~DcmUnsignedLongOffset()
{
}


OFCondition DcmUnsignedLongOffset::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmUnsignedLongOffset &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmUnsignedLongOffset::ident() const
{
    /* internal type identifier */
    return EVR_up;
}


OFCondition DcmUnsignedLongOffset::clear()
{
    /* call inherited method */
    errorFlag = DcmUnsignedLong::clear();
    /* remove reference to object */
    nextRecord = NULL;
    return errorFlag;
}


// ********************************


DcmObject* DcmUnsignedLongOffset::getNextRecord()
{
    errorFlag = EC_Normal;
    /* return pointer to currently stored object reference */
    return nextRecord;
}


DcmObject *DcmUnsignedLongOffset::setNextRecord(DcmObject *record)
{
    errorFlag = EC_Normal;
    /* store new object reference */
    nextRecord = record;
    return record;
}


// ********************************


OFCondition DcmUnsignedLongOffset::verify(const OFBool autocorrect)
{
    /* call inherited method */
    errorFlag = DcmUnsignedLong::verify(autocorrect);
    /* perform additional checks on the stored value */
    Uint32 *uintVals;
    errorFlag = getUint32Array(uintVals);
    if (errorFlag.good() && (getLengthField() > 0) && (uintVals != NULL) && (*uintVals != 0) && (nextRecord == NULL))
        errorFlag = EC_CorruptedData;
    return errorFlag;
}
