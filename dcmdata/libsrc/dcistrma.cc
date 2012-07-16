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
 *  Purpose: base classes for input streams
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcistrma.h"
#include "dcmtk/dcmdata/dcistrmz.h" /* for DcmZLibInputFilter */
#include "dcmtk/dcmdata/dcerror.h"  /* for EC_IllegalCall */

DcmInputStream::DcmInputStream(DcmProducer *initial)
: current_(initial)
, compressionFilter_(NULL)
, tell_(0)
, mark_(0)
{
}

DcmInputStream::~DcmInputStream()
{
  // we cannot access the stream anymore at this point because the
  // producer has most probably already been deleted.
  delete compressionFilter_;
}

OFBool DcmInputStream::good() const
{
  return current_->good();
}

OFCondition DcmInputStream::status() const
{
  return current_->status();
}

OFBool DcmInputStream::eos() 
{
  return current_->eos();
}

offile_off_t DcmInputStream::avail() 
{
  return current_->avail();
}

offile_off_t DcmInputStream::read(void *buf, offile_off_t buflen)
{
  offile_off_t result = current_->read(buf, buflen);
  tell_ += result;
  return result;
}

offile_off_t DcmInputStream::skip(offile_off_t skiplen)
{
  offile_off_t result = current_->skip(skiplen);
  tell_ += result;
  return result;
}

offile_off_t DcmInputStream::tell() const
{
  return tell_;
}

void DcmInputStream::mark()
{
  mark_ = tell_;
}

void DcmInputStream::putback()
{
  current_->putback(tell_ - mark_);
  tell_ = mark_;
}

const DcmProducer *DcmInputStream::currentProducer() const
{
  return current_;
}

OFCondition DcmInputStream::installCompressionFilter(E_StreamCompression filterType)
{
  OFCondition result = EC_Normal;
  if (compressionFilter_) result = EC_DoubleCompressionFilters;
  else
  {
    switch (filterType)
    {
#ifdef WITH_ZLIB
      case ESC_zlib:
        compressionFilter_ = new DcmZLibInputFilter();
        if (compressionFilter_) 
        {
          compressionFilter_->append(*current_);
          // feed the compression engine with data from the producer
          compressionFilter_->skip(0); 
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
