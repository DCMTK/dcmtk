/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Purpose: DcmInputBufferStream and related classes,
 *    implements input to blocks of memory as needed in the dcmnet module.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:08 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcistrmb.h"
#include "dcmtk/dcmdata/dcerror.h"

#define DCMBUFFERPRODUCER_BUFSIZE 1024

DcmBufferProducer::DcmBufferProducer()
: DcmProducer()
, buffer_(NULL)
, backup_(new unsigned char[DCMBUFFERPRODUCER_BUFSIZE])
, bufSize_(0)
, bufIndex_(0)
, backupIndex_(DCMBUFFERPRODUCER_BUFSIZE)
, backupStart_(DCMBUFFERPRODUCER_BUFSIZE)
, status_(EC_Normal)
, eosflag_(OFFalse)
{
  if (!backup_) status_ = EC_MemoryExhausted;
}


DcmBufferProducer::~DcmBufferProducer()
{
  delete[] backup_;
}


OFBool DcmBufferProducer::good() const
{
  return status_.good();
}

OFCondition DcmBufferProducer::status() const
{
  return status_;
}

OFBool DcmBufferProducer::eos() 
{
  // end of stream is true if the user has called setEos() before
  // and there is no more data available in the current buffer.
  // We also flag end of stream if the status is bad.
  return (eosflag_ && (avail() == 0)) || (!status_.good());
}


offile_off_t DcmBufferProducer::avail()
{
  if (status_.good())
  {
    // in the backup buffer, we have (DCMBUFFERPRODUCER_BUFSIZE - backupIndex_)
    // bytes available. In the user buffer, we have (bufSize_ - bufIndex_).
    return DCMBUFFERPRODUCER_BUFSIZE + bufSize_ - bufIndex_ - backupIndex_;
  }
  else return 0;
}


offile_off_t DcmBufferProducer::read(void *buf, offile_off_t buflen)
{
  offile_off_t result = 0;
  if (status_.good() && buflen && buf)
  {
    unsigned char *target = OFstatic_cast(unsigned char *, buf);
    if (backupIndex_ < DCMBUFFERPRODUCER_BUFSIZE)
    {
      // we have data in the backup buffer, read first
      result = DCMBUFFERPRODUCER_BUFSIZE - backupIndex_;
      if (result > buflen) result = buflen;
      memcpy(target, backup_ + backupIndex_, OFstatic_cast(size_t, result));
      backupIndex_ += result;
      target += result;
      buflen -= result;
    }

    if (buflen && bufSize_)
    {
      // read data from user buffer
      offile_off_t numbytes = bufSize_ - bufIndex_;
      if (numbytes > buflen) numbytes = buflen;
      memcpy(target, buffer_ + bufIndex_, OFstatic_cast(size_t, numbytes));
      bufIndex_ += numbytes;
      result += numbytes;
    }
  }
  return result;
}


offile_off_t DcmBufferProducer::skip(offile_off_t skiplen)
{
  offile_off_t result = 0;
  if (status_.good() && skiplen)
  {
    if (backupIndex_ < DCMBUFFERPRODUCER_BUFSIZE)
    {
      // we have data in the backup buffer, skip first
      result = DCMBUFFERPRODUCER_BUFSIZE - backupIndex_;
      if (result > skiplen) result = skiplen;
      backupIndex_ += result;
      skiplen -= result;
    }

    if (skiplen && bufSize_)
    {
      // skip data from user buffer
      offile_off_t skipbytes = bufSize_ - bufIndex_;
      if (skipbytes > skiplen) skipbytes = skiplen;
      bufIndex_ += skipbytes;
      result += skipbytes;
    }
  }
  return result;
}

void DcmBufferProducer::putback(offile_off_t num)
{
  if (status_.good() && num)
  {
    if (bufSize_ && bufIndex_)
    {
      // since bufIndex_ > 0, data has already been read from the user buffer.
      // This means we should putback in the user buffer first, and only
      // if this is not sufficient we also touch the backup buffer.
      if (num > bufIndex_)
      {
        num -= bufIndex_;
        bufIndex_ = 0;
      }
      else
      {
        bufIndex_ -= num;
        num = 0;
      }
    }

    if (num && (backupIndex_ > backupStart_))
    {
      // there is still a number of bytes to putback, and we have data in the
      // backup buffer, so we can actually putback something there.
      // This will cause the next read operation to read from the backup
      // buffer first and only then access the user buffer.
      if (num > (backupIndex_ - backupStart_))
      {
        num -= backupIndex_ - backupStart_;
        backupIndex_ = backupStart_;
      }
      else
      {
        backupIndex_ -= num;
        num = 0;
      }
    }

    if (num)
    {
      // we didn't manage to execute the putback request because there was
      // not enough data available in both buffers. Producer failure.
      status_ = EC_PutbackFailed;
    }
  }
}

void DcmBufferProducer::setBuffer(const void *buf, offile_off_t buflen)
{
  if (status_.good())
  {
    if (buffer_ || eosflag_)
    {
      // error: attempt to set new buffer without calling releaseBuffer before
      // or after calling setEos.
      status_ = EC_IllegalCall;
    }
    else if (buf && buflen)
    {
      buffer_   = OFstatic_cast(unsigned char *, OFconst_cast(void *, buf));
      bufSize_  = buflen;
      bufIndex_ = 0;
    }
  }
}

void DcmBufferProducer::releaseBuffer()
{
  // releaseBuffer() might be called multiple times, so buffer_ could already be NULL.
  if (status_.good() && buffer_)
  {
    // compute the least number of bytes that we have to store in the backup buffer
    offile_off_t numBytes = bufSize_ - bufIndex_;

    if (numBytes > backupIndex_)
    {
      // number of bytes is larger than free space in backup buffer; fail.
      status_ = EC_IllegalCall;
    }
    else
    {
      // if number of bytes is smaller than free space in backup buffer, make as large as possible
      if (numBytes < backupIndex_)
      {
        numBytes = (backupIndex_ < bufSize_) ? backupIndex_ : bufSize_;
      }

      // if number of bytes is smaller than backup buffer, move old
      // data in backup buffer to keep older data available for putback operations
      if (numBytes < DCMBUFFERPRODUCER_BUFSIZE)
      {
        // move (DCMBUFFERPRODUCER_BUFSIZE - numBytes) bytes from end of backup buffer
        // to start of backup buffer. Everything else will be overwritten from the
        // user buffer.
        memmove(backup_, backup_ + numBytes, OFstatic_cast(size_t, DCMBUFFERPRODUCER_BUFSIZE - numBytes));

        // adjust backupStart_
        if (backupStart_ < numBytes) backupStart_ = 0; else backupStart_ -= numBytes;
      }
      else
      {
        // the backup buffer will be filled completely from the user buffer
        backupStart_ = 0;
      }

      // copy (numBytes) bytes from the end of the user buffer to the end of the backup buffer
      memcpy(backup_ + DCMBUFFERPRODUCER_BUFSIZE - numBytes, buffer_ + bufSize_ - numBytes, OFstatic_cast(size_t, numBytes));

      // adjust backupIndex_
      if (backupIndex_ == DCMBUFFERPRODUCER_BUFSIZE)
      {
        // there was no unread data in the backup buffer before.
        // backupIndex_ only depends on the number of unread bytes in the user buffer.
        // we know that (bufSize_ - bufIndex_ < DCMBUFFERPRODUCER_BUFSIZE) because this was tested before.
        backupIndex_ = DCMBUFFERPRODUCER_BUFSIZE + bufIndex_ - bufSize_;
      }
      else
      {
        // there was unread data in the backup buffer before.
        // This implies that all of the user buffer is unread and the complete user
        // buffer fits into the free space in the backup buffer, because otherwise
        // we would not have got this far.
        // Adjust backupIndex_ by the number of bytes we have moved the content of the backup buffer.
        backupIndex_ -= numBytes;
      }

      // release user buffer
      buffer_ = NULL;
      bufSize_ = 0;
      bufIndex_ = 0;
    }
  }

  // the number of bytes that can be putback after this operation depends
  // on the size of the backup buffer and on the number of unread bytes
  // in both buffers at the time of the releaseBuffer() operation.
  // If the user only calls releaseBuffer() when most data has been read
  // from the buffer, we should be able to putback almost 1K.
}

void DcmBufferProducer::setEos()
{
  eosflag_ = OFTrue;
}

/* ======================================================================= */

DcmInputBufferStream::DcmInputBufferStream()
: DcmInputStream(&producer_) // safe because DcmInputStream only stores pointer
, producer_()
{
}

DcmInputBufferStream::~DcmInputBufferStream()
{
#ifdef DEBUG
  if ((!eos()) && (avail() > 0))
  {
      DCMDATA_WARN("closing unflushed DcmInputBufferStream, loss of data!");
  }
#endif
}

DcmInputStreamFactory *DcmInputBufferStream::newFactory() const
{
  // we don't support delayed loading from buffer streams
  return NULL;
}

void DcmInputBufferStream::setBuffer(const void *buf, offile_off_t buflen)
{
  producer_.setBuffer(buf, buflen);

  // if there is a compression filter, the following call will
  // cause it to feed the compression engine with data from the
  // new buffer.
  skip(0);
}

void DcmInputBufferStream::releaseBuffer()
{
  producer_.releaseBuffer();
}

void DcmInputBufferStream::setEos()
{
  producer_.setEos();
}


/*
 * CVS/RCS Log:
 * $Log: dcistrmb.cc,v $
 * Revision 1.9  2010-10-14 13:14:08  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.8  2010-02-22 11:39:54  uli
 * Remove some unneeded includes.
 *
 * Revision 1.7  2009-11-04 09:58:09  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.6  2007-02-19 15:45:31  meichel
 * Class DcmInputStream and related classes are now safe for use with
 *   large files (2 GBytes or more) if supported by compiler and operating system.
 *
 * Revision 1.5  2006/08/15 15:49:54  meichel
 * Updated all code in module dcmdata to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.4  2005/12/08 15:41:13  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.3  2004/02/04 16:33:40  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.2  2002/09/19 08:32:29  joergr
 * Added explicit type casts to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.1  2002/08/27 16:55:48  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 *
 */
