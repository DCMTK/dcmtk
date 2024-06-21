/*
 *
 *  Copyright (C) 2002-2023, OFFIS e.V.
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
 *  Purpose: DcmOutputBufferStream and related classes,
 *    implements output to blocks of memory as needed in the dcmnet module.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcostrmb.h"
#include "dcmtk/dcmdata/dcerror.h"


DcmBufferConsumer::DcmBufferConsumer(void *buf, offile_off_t bufLen)
: DcmConsumer()
, buffer_(OFstatic_cast(unsigned char *, buf))
, bufSize_(bufLen)
, filled_(0)
, status_(EC_Normal)
{
  if (buffer_ == NULL) status_ = EC_IllegalCall;
}

DcmBufferConsumer::~DcmBufferConsumer()
{
}

OFBool DcmBufferConsumer::good() const
{
  return status_.good();
}

OFCondition DcmBufferConsumer::status() const
{
  return status_;
}

OFBool DcmBufferConsumer::isFlushed() const
{
  return (filled_ == 0);
}

offile_off_t DcmBufferConsumer::avail() const
{
  return bufSize_ - filled_;
}

offile_off_t DcmBufferConsumer::write(const void *buf, offile_off_t buflen)
{
  offile_off_t result = 0;
  if (buflen && buf && status_.good())
  {
    result = bufSize_ - filled_;
    if (result > buflen) result = buflen;
    memcpy(buffer_+ filled_, buf, OFstatic_cast(size_t, result));
    filled_ += result;
  }
  return result;
}

void DcmBufferConsumer::flush()
{
  // nothing to flush
}

void DcmBufferConsumer::flushBuffer(void *& buffer, offile_off_t& length)
{
  buffer = buffer_;
  length = filled_;
  filled_ = 0;
}

offile_off_t DcmBufferConsumer::filled()
{
  return filled_;
}

/* ======================================================================= */

DcmOutputBufferStream::DcmOutputBufferStream(void *buf, offile_off_t bufLen)
: DcmOutputStream(&consumer_) // safe because DcmOutputStream only stores pointer
, consumer_(buf, bufLen)
{
}

DcmOutputBufferStream::~DcmOutputBufferStream()
{
#ifdef DEBUG
  if (! isFlushed())
  {
    DCMDATA_WARN("DcmOutputBufferStream: Closing unflushed DcmOutputBufferStream, loss of data!");
  }
#endif
}

void DcmOutputBufferStream::flushBuffer(void *& buffer, offile_off_t& length)
{
  consumer_.flushBuffer(buffer, length);
}

offile_off_t DcmOutputBufferStream::filled()
{
  return consumer_.filled();
}
