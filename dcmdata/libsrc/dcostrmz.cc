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
 *  Purpose: zlib compression filter for output streams
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:08 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_ZLIB

#include "dcmtk/dcmdata/dcostrmz.h"
#include "dcmtk/dcmdata/dcerror.h"

#define DCMZLIBOUTPUTFILTER_BUFSIZE 4096

/* taken from zutil.h */
#if MAX_MEM_LEVEL >= 8
#define DEF_MEM_LEVEL 8
#else
#define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif

OFGlobal<int> dcmZlibCompressionLevel(Z_DEFAULT_COMPRESSION);


DcmZLibOutputFilter::DcmZLibOutputFilter()
: DcmOutputFilter()
, current_(NULL)
, zstream_(new z_stream)
, status_(EC_MemoryExhausted)
, flushed_(OFFalse)
, inputBuf_(new unsigned char[DCMZLIBOUTPUTFILTER_BUFSIZE])
, inputBufStart_(0)
, inputBufCount_(0)
, outputBuf_(new unsigned char[DCMZLIBOUTPUTFILTER_BUFSIZE])
, outputBufStart_(0)
, outputBufCount_(0)
{
  if (zstream_ && inputBuf_ && outputBuf_)
  {
    zstream_->zalloc = Z_NULL;
    zstream_->zfree = Z_NULL;
    zstream_->opaque = Z_NULL;
#ifdef ZLIB_ENCODE_RFC1950_HEADER
    /* create deflated ZLIB format instead of deflated bitstream format
     * (i.e. RFC 1950 instead of RFC 1951).
     * THE RESULTING BITSTREAM IS NOT DICOM COMPLIANT!
     * Use only for testing, and use with care.
     */
    if (Z_OK == deflateInit(zstream_, dcmZlibCompressionLevel.get()))
#else
    /* windowBits is passed < 0 to suppress zlib header */
    if (Z_OK == deflateInit2(zstream_, dcmZlibCompressionLevel.get(),
        Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY))
#endif
    {
        status_ = EC_Normal;
    }
    else
    {
      OFString etext = "ZLib Error: ";
      if (zstream_->msg) etext += zstream_->msg;
      status_ = makeOFCondition(OFM_dcmdata, 16, OF_error, etext.c_str());
    }
  }
}

DcmZLibOutputFilter::~DcmZLibOutputFilter()
{
  if (zstream_)
  {
    deflateEnd(zstream_); // discards any unprocessed input and does not flush any pending output
    delete zstream_;
  }
  delete[] inputBuf_;
  delete[] outputBuf_;
}


OFBool DcmZLibOutputFilter::good() const
{
  return status_.good();
}

OFCondition DcmZLibOutputFilter::status() const
{
  return status_;
}

OFBool DcmZLibOutputFilter::isFlushed() const
{
  if (status_.bad() || (current_ == NULL)) return OFTrue;
  return (inputBufCount_ == 0) && (outputBufCount_ == 0) && flushed_ && current_->isFlushed();
}


offile_off_t DcmZLibOutputFilter::avail() const
{
  // compute number of bytes available in input buffer
  if (status_.good() ) return DCMZLIBOUTPUTFILTER_BUFSIZE - inputBufCount_;
    else return 0;
}

void DcmZLibOutputFilter::flushOutputBuffer()
{
  if (outputBufCount_)
  {
    // flush from outputBufStart_ to end of data or end of buffer, whatever comes first
    offile_off_t numBytes = (outputBufStart_ + outputBufCount_ > DCMZLIBOUTPUTFILTER_BUFSIZE) ?
      (DCMZLIBOUTPUTFILTER_BUFSIZE - outputBufStart_) : outputBufCount_ ;

    offile_off_t written = current_->write(outputBuf_ + outputBufStart_, numBytes);

    // adjust counters
    outputBufCount_ -= written;
    outputBufStart_ += written;

    if (outputBufStart_ == DCMZLIBOUTPUTFILTER_BUFSIZE)
    {
      // wrapped around
      outputBufStart_ = 0;

      // now flush to end of data
      if (outputBufCount_ && written)
      {
        written = current_->write(outputBuf_, outputBufCount_);

        // adjust counters
        outputBufCount_ -= written;
        outputBufStart_ += written;
      }
    }

    // reset buffer start to make things faster
    if (outputBufCount_ == 0) outputBufStart_ = 0;
  }
}

offile_off_t DcmZLibOutputFilter::fillInputBuffer(const void *buf, offile_off_t buflen)
{
  offile_off_t result = 0;
  if (buf && buflen && inputBufCount_ < DCMZLIBOUTPUTFILTER_BUFSIZE)
  {

    const unsigned char *data = OFstatic_cast(const unsigned char *, buf);

    // use first part of input buffer
    if (inputBufStart_ + inputBufCount_ < DCMZLIBOUTPUTFILTER_BUFSIZE)
    {
      result = DCMZLIBOUTPUTFILTER_BUFSIZE - (inputBufStart_ + inputBufCount_);
      if (result > buflen) result = buflen;

      memcpy(inputBuf_ + inputBufStart_ + inputBufCount_, data, OFstatic_cast(size_t, result));
      inputBufCount_ += result;
      data += result;
      buflen -= result;
    }

    // use second part of input buffer
    if (buflen && (inputBufCount_ < DCMZLIBOUTPUTFILTER_BUFSIZE) &&
        inputBufStart_ + inputBufCount_ >= DCMZLIBOUTPUTFILTER_BUFSIZE)
    {
      offile_off_t len = DCMZLIBOUTPUTFILTER_BUFSIZE - inputBufCount_;
      if (len > buflen) len = buflen;

      memcpy(inputBuf_ + (inputBufStart_ + inputBufCount_ - DCMZLIBOUTPUTFILTER_BUFSIZE), data, OFstatic_cast(size_t, len));

      inputBufCount_ += len;
      result += len;
    }
  }
  return result;
}

void DcmZLibOutputFilter::compressInputBuffer(OFBool finalize)
{
  if (inputBufCount_ || finalize)
  {
    // flush from inputBufStart_ to end of data or end of buffer, whatever comes first
    offile_off_t numBytes = (inputBufStart_ + inputBufCount_ > DCMZLIBOUTPUTFILTER_BUFSIZE) ?
      (DCMZLIBOUTPUTFILTER_BUFSIZE - inputBufStart_) : inputBufCount_ ;

    offile_off_t written = compress(inputBuf_ + inputBufStart_, numBytes, finalize);

    // adjust counters
    inputBufCount_ -= written;
    inputBufStart_ += written;

    if (inputBufStart_ == DCMZLIBOUTPUTFILTER_BUFSIZE)
    {
      // wrapped around
      inputBufStart_ = 0;

      // now flush to end of data
      if (inputBufCount_ && written)
      {
        written = compress(inputBuf_, inputBufCount_, finalize);

        // adjust counters
        inputBufCount_ -= written;
        inputBufStart_ += written;
      }
    }

    // reset buffer start to make things faster
    if (inputBufCount_ == 0) inputBufStart_ = 0;
  }
}

offile_off_t DcmZLibOutputFilter::compress(const void *buf, offile_off_t buflen, OFBool finalize)
{
  offile_off_t result = 0;
  if (outputBufCount_ < DCMZLIBOUTPUTFILTER_BUFSIZE)
  {
    zstream_->next_in = OFstatic_cast(Bytef *, OFconst_cast(void *, buf));
    zstream_->avail_in = OFstatic_cast(uInt, buflen);
    int zstatus;

    // use first part of output buffer
    if (outputBufStart_ + outputBufCount_ < DCMZLIBOUTPUTFILTER_BUFSIZE)
    {
      zstream_->next_out = OFstatic_cast(Bytef *, outputBuf_ + outputBufStart_ + outputBufCount_);
      zstream_->avail_out = OFstatic_cast(uInt, DCMZLIBOUTPUTFILTER_BUFSIZE - (outputBufStart_ + outputBufCount_));
      zstatus = deflate(zstream_, (finalize ? Z_FINISH : 0));

      if (zstatus == Z_OK || zstatus == Z_BUF_ERROR) { /* everything OK */ }
      else if (zstatus == Z_STREAM_END) flushed_ = OFTrue;
      else
      {
        OFString etext = "ZLib Error: ";
        if (zstream_->msg) etext += zstream_->msg;
        status_ = makeOFCondition(OFM_dcmdata, 16, OF_error, etext.c_str());
      }

      outputBufCount_ = DCMZLIBOUTPUTFILTER_BUFSIZE - outputBufStart_ - OFstatic_cast(offile_off_t, zstream_->avail_out);
    }

    // use second part of output buffer
    if ((outputBufCount_ < DCMZLIBOUTPUTFILTER_BUFSIZE) &&
        outputBufStart_ + outputBufCount_ >= DCMZLIBOUTPUTFILTER_BUFSIZE)
    {
      zstream_->next_out = OFstatic_cast(Bytef *, outputBuf_ + (outputBufStart_ + outputBufCount_ - DCMZLIBOUTPUTFILTER_BUFSIZE));
      zstream_->avail_out = OFstatic_cast(uInt, DCMZLIBOUTPUTFILTER_BUFSIZE - outputBufCount_);
      zstatus = deflate(zstream_, (finalize ? Z_FINISH : 0));

      if (zstatus == Z_OK || zstatus == Z_BUF_ERROR) { /* everything OK */ }
      else if (zstatus == Z_STREAM_END) flushed_ = OFTrue;
      else
      {
        OFString etext = "ZLib Error: ";
        if (zstream_->msg) etext += zstream_->msg;
        status_ = makeOFCondition(OFM_dcmdata, 16, OF_error, etext.c_str());
      }

      outputBufCount_ =  DCMZLIBOUTPUTFILTER_BUFSIZE - OFstatic_cast(offile_off_t, zstream_->avail_out);
    }

    result = (buflen - OFstatic_cast(offile_off_t, zstream_->avail_in));
  }
  return result;
}

offile_off_t DcmZLibOutputFilter::write(const void *buf, offile_off_t buflen)
{
  if (status_.bad() || (current_ == NULL)) return 0;

  // flush output buffer if necessary
  if (outputBufCount_ == DCMZLIBOUTPUTFILTER_BUFSIZE) flushOutputBuffer();

  // compress pending input from input buffer
  while (status_.good() && inputBufCount_ > 0 && outputBufCount_ < DCMZLIBOUTPUTFILTER_BUFSIZE)
  {
    compressInputBuffer(OFFalse);
    if (outputBufCount_ == DCMZLIBOUTPUTFILTER_BUFSIZE) flushOutputBuffer();
  }

  const unsigned char *data = OFstatic_cast(const unsigned char *, buf);
  offile_off_t result = 0;

  // compress user data only if input buffer is empty
  if (inputBufCount_ == 0)
  {
    while (status_.good() && (buflen > result) && outputBufCount_ < DCMZLIBOUTPUTFILTER_BUFSIZE)
    {
      result += compress(data+result, buflen-result, OFFalse);
      if (outputBufCount_ == DCMZLIBOUTPUTFILTER_BUFSIZE) flushOutputBuffer();
    }
  }

  // finally stuff as much into the input buffer as possible
  result += fillInputBuffer(data+result, buflen-result);

  // total number of bytes consumed from input
  return result;
}


void DcmZLibOutputFilter::flush()
{
  if (status_.good() && current_)
  {
    // flush output buffer first
    if (outputBufCount_ == DCMZLIBOUTPUTFILTER_BUFSIZE) flushOutputBuffer();

    // compress pending input from input buffer
    while (status_.good() && inputBufCount_ > 0 && outputBufCount_ < DCMZLIBOUTPUTFILTER_BUFSIZE)
    {
      compressInputBuffer(OFTrue);
      if (outputBufCount_ == DCMZLIBOUTPUTFILTER_BUFSIZE) flushOutputBuffer();
    }

    while (status_.good() && (! flushed_) && outputBufCount_ < DCMZLIBOUTPUTFILTER_BUFSIZE)
    {
      // create output from compression engine until end of compressed stream
      compress(NULL, 0, OFTrue);
      if (outputBufCount_ == DCMZLIBOUTPUTFILTER_BUFSIZE) flushOutputBuffer();
    }

    // final attempt to flush output buffer
    if (outputBufCount_ > 0) flushOutputBuffer();
  }
}


void DcmZLibOutputFilter::append(DcmConsumer& consumer)
{
  current_ = &consumer;
}

#else  /* WITH_ZLIB */

/* make sure that the object file is not completely empty if compiled
 * without zlib because some linkers might fail otherwise.
 */
void dcostrmz_dummy_function()
{
  return;
}

#endif /* WITH_ZLIB */

/*
 * CVS/RCS Log:
 * $Log: dcostrmz.cc,v $
 * Revision 1.8  2010-10-14 13:14:08  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.7  2009-11-04 09:58:10  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.6  2007-02-19 16:06:10  meichel
 * Class DcmOutputStream and related classes are now safe for use with
 *   large files (2 GBytes or more) if supported by compiler and operating system.
 *
 * Revision 1.5  2005/12/08 15:41:23  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.4  2004/04/07 12:19:14  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.3  2002/12/20 14:55:34  wilkens
 * Inserted three casts in order to get rid of compiler warning on Solaris 2.5.1
 * using compiler SC 2.0.1.
 *
 * Revision 1.2  2002/08/29 15:57:49  meichel
 * Updated zlib-related classes to correctly compile when WITH_ZLIB is undefined
 *
 * Revision 1.1  2002/08/27 16:55:54  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 *
 */
