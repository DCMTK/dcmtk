/*
 *
 *  Copyright (C) 2007-2010, OFFIS e.V.
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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Implementation of class DcmWriteCache
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:11 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcwcache.h"   /* for class DcmWriteCache */
#include "dcmtk/dcmdata/dcelem.h"     /* for class DcmElement */
#include "dcmtk/dcmdata/dcostrma.h"   /* for class DcmOutputStream */


void DcmWriteCache::init(void *owner,
                         Uint32 fieldLength,
                         Uint32 bytesTransferred,
                         E_ByteOrder byteOrder)
{
  if (! buf_)
  {
    capacity_ = DcmWriteCacheBufsize;
    buf_ = new Uint8[capacity_];
  }

  if (owner != owner_)
  {
    owner_ = owner;
    fieldLength_ = fieldLength;
    fieldOffset_ = bytesTransferred;
    byteOrder_ = byteOrder;
    offset_ = 0;
    numBytes_ = 0;
  }
}

OFCondition DcmWriteCache::fillBuffer(DcmElement& elem)
{
  OFCondition result = EC_Normal;
  if (buf_)
  {
      // re-fill the buffer only if completely empty
      if (! numBytes_)
      {
        offset_ = 0;

        // compute the number of bytes to read - either buffer size or
        // the remaining number of bytes in the element, whatever is smaller
        Uint32 bytesToRead = fieldLength_ - fieldOffset_;
        if (bytesToRead > capacity_) bytesToRead = capacity_;

        result = elem.getPartialValue(buf_, fieldOffset_, bytesToRead, &fcache_, byteOrder_);

        if (result.good())
        {
          numBytes_ = bytesToRead;
          fieldOffset_ += numBytes_;
        }

      }
  } else result = EC_IllegalCall;
  return result;
}

Uint32 DcmWriteCache::writeBuffer(DcmOutputStream &outStream)
{
  Uint32 result = 0;
  if (buf_ && numBytes_)
  {
    result = OFstatic_cast(Uint32, outStream.write(buf_ + offset_, numBytes_));

    numBytes_ -= result;
    offset_ += result;
  }
  return result;
}


/*
 * CVS/RCS Log:
 * $Log: dcwcache.cc,v $
 * Revision 1.3  2010-10-14 13:14:11  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.2  2009-02-04 17:55:30  joergr
 * Fixes various type mismatches reported by MSVC introduced with OFFile class.
 *
 * Revision 1.1  2007-11-29 14:30:20  meichel
 * Write methods now handle large raw data elements (such as pixel data)
 *   without loading everything into memory. This allows very large images to
 *   be sent over a network connection, or to be copied without ever being
 *   fully in memory.
 *
 *
 */
