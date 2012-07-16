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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: base classes for output streams
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcostrma.h"
#include "dcmtk/dcmdata/dcostrmz.h" /* for DcmZLibOutputFilter */
#include "dcmtk/dcmdata/dcerror.h"  /* for EC_IllegalCall */

DcmOutputStream::DcmOutputStream(DcmConsumer *initial)
: current_(initial)
, compressionFilter_(NULL)
, tell_(0)
{
}

DcmOutputStream::~DcmOutputStream()
{
  // we cannot access the stream anymore at this point because the
  // consumer has most probably already been deleted.
  delete compressionFilter_;
}

OFCondition DcmOutputStream::installCompressionFilter(E_StreamCompression filterType)
{
  OFCondition result = EC_Normal;
  if (compressionFilter_) result = EC_DoubleCompressionFilters;
  else
  {
    switch (filterType)
    {
#ifdef WITH_ZLIB
      case ESC_zlib:
        compressionFilter_ = new DcmZLibOutputFilter();
        if (compressionFilter_) 
        {
          compressionFilter_->append(*current_);
          current_ = compressionFilter_;
        } else result = EC_MemoryExhausted;        
        break;
#endif
      case ESC_none:
      case ESC_unsupported:
        result = EC_UnsupportedEncoding; 
        break;
    }
  }
  return result;
}

OFBool DcmOutputStream::good() const
{
  return current_->good();
}

OFCondition DcmOutputStream::status() const
{
  return current_->status();
}

OFBool DcmOutputStream::isFlushed() const
{
  return current_->isFlushed();
}

offile_off_t DcmOutputStream::avail() const
{
  return current_->avail();
}

offile_off_t DcmOutputStream::write(const void *buf, offile_off_t buflen)
{
  offile_off_t result = current_->write(buf, buflen);
  tell_ += result;
  return result;
}

void DcmOutputStream::flush()
{
  current_->flush();
}

offile_off_t DcmOutputStream::tell() const
{
  return tell_;
}
