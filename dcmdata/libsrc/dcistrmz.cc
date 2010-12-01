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
 *  Purpose: zlib compression filter for input streams
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-12-01 13:21:18 $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_ZLIB

#include "dcmtk/dcmdata/dcistrmz.h"
#include "dcmtk/dcmdata/dcerror.h"

#define DCMZLIBINPUTFILTER_BUFSIZE 4096
#define DCMZLIBINPUTFILTER_PUTBACKSIZE 1024

OFGlobal<OFBool> dcmZlibExpectRFC1950Encoding(OFFalse);


DcmZLibInputFilter::DcmZLibInputFilter()
: DcmInputFilter()
, current_(NULL)
, zstream_(new z_stream)
, status_(EC_MemoryExhausted)
, eos_(OFFalse)
, inputBuf_(new unsigned char[DCMZLIBINPUTFILTER_BUFSIZE])
, inputBufStart_(0)
, inputBufCount_(0)
, outputBuf_(new unsigned char[DCMZLIBINPUTFILTER_BUFSIZE])
, outputBufStart_(0)
, outputBufCount_(0)
, outputBufPutback_(0)
, padded_(OFFalse)
{
  if (zstream_ && inputBuf_ && outputBuf_)
  {
    zstream_->zalloc = Z_NULL;
    zstream_->zfree = Z_NULL;
    zstream_->opaque = Z_NULL;
    zstream_->next_in = Z_NULL;
    zstream_->avail_in = 0;

    if (dcmZlibExpectRFC1950Encoding.get())
    {
      /* expect non-standard bitstream *with* zlib header
       * This is easy because it is the normal zlib format anyway.
       */
      if (Z_OK == inflateInit(zstream_)) status_ = EC_Normal;
      else
      {
        OFString etext = "ZLib Error: ";
        if (zstream_->msg) etext += zstream_->msg;
        status_ = makeOFCondition(OFM_dcmdata, 16, OF_error, etext.c_str());
      }
    }
    else
    {
      /* windowBits is passed < 0 to tell that there is no zlib header.
       * Note that in this case inflate *requires* an extra "dummy" byte
       * after the compressed stream in order to complete decompression and
       * return Z_STREAM_END.
       */
      if (Z_OK == inflateInit2(zstream_, -MAX_WBITS)) status_ = EC_Normal;
      else
      {
        OFString etext = "ZLib Error: ";
        if (zstream_->msg) etext += zstream_->msg;
        status_ = makeOFCondition(OFM_dcmdata, 16, OF_error, etext.c_str());
      }
    }
  }
}

DcmZLibInputFilter::~DcmZLibInputFilter()
{
  if (zstream_)
  {
    inflateEnd(zstream_); // discards any unprocessed input and does not flush any pending output
    delete zstream_;
  }
  delete[] inputBuf_;
  delete[] outputBuf_;
}

OFBool DcmZLibInputFilter::good() const
{
  return status_.good();
}

OFCondition DcmZLibInputFilter::status() const
{
  return status_;
}

OFBool DcmZLibInputFilter::eos()
{
  if (status_.bad() || (current_ == NULL)) return OFTrue;

  // there may be trailing garbage after the compressed stream.
  // we report eos if the decompressor has reported Z_STREAM_END
  // and the output buffer is empty.
  return (outputBufCount_ == 0) && eos_;
}

offile_off_t DcmZLibInputFilter::avail()
{
  if (status_.good()) return outputBufCount_; else return 0;
}

offile_off_t DcmZLibInputFilter::read(void *buf, offile_off_t buflen)
{
  if (status_.bad() || (current_ == NULL) || (buf == NULL)) return 0;
  unsigned char *target = OFstatic_cast(unsigned char *, buf);
  offile_off_t offset = 0;
  offile_off_t availBytes = 0;
  offile_off_t result = 0;
  do
  {
    // copy bytes from output buffer to user provided block of data
    if (outputBufCount_)
    {
      // determine next block of data in output buffer
      offset = outputBufStart_ + outputBufPutback_;
      if (offset >= DCMZLIBINPUTFILTER_BUFSIZE) offset -= DCMZLIBINPUTFILTER_BUFSIZE;

      availBytes = outputBufCount_;
      if (offset + availBytes > DCMZLIBINPUTFILTER_BUFSIZE) availBytes = DCMZLIBINPUTFILTER_BUFSIZE - offset;
      if (availBytes > buflen) availBytes = buflen;

      if (availBytes) memcpy(target, outputBuf_ + offset, OFstatic_cast(size_t, availBytes));
      target += availBytes;
      result += availBytes;
      buflen -= availBytes;

      // adjust pointers
      outputBufPutback_ += availBytes;
      outputBufCount_ -= availBytes;
      if (outputBufPutback_ > DCMZLIBINPUTFILTER_PUTBACKSIZE)
      {
        outputBufStart_ += outputBufPutback_ - DCMZLIBINPUTFILTER_PUTBACKSIZE;
        if (outputBufStart_ >= DCMZLIBINPUTFILTER_BUFSIZE) outputBufStart_ -= DCMZLIBINPUTFILTER_BUFSIZE;
        outputBufPutback_ = DCMZLIBINPUTFILTER_PUTBACKSIZE;
      }
    }

    // refill output buffer
    fillOutputBuffer();
  } while (buflen && outputBufCount_);

  // we're either done or the output buffer is empty because of producer suspension
  return result;
}

offile_off_t DcmZLibInputFilter::skip(offile_off_t skiplen)
{
  if (status_.bad() || (current_ == NULL)) return 0;
  offile_off_t offset = 0;
  offile_off_t availBytes = 0;
  offile_off_t result = 0;
  do
  {
    // copy bytes from output buffer to user provided block of data
    if (outputBufCount_)
    {
      // determine next block of data in output buffer
      offset = outputBufStart_ + outputBufPutback_;
      if (offset >= DCMZLIBINPUTFILTER_BUFSIZE) offset -= DCMZLIBINPUTFILTER_BUFSIZE;

      availBytes = outputBufCount_;
      if (offset + availBytes > DCMZLIBINPUTFILTER_BUFSIZE) availBytes = DCMZLIBINPUTFILTER_BUFSIZE - offset;
      if (availBytes > skiplen) availBytes = skiplen;
      result += availBytes;
      skiplen -= availBytes;

      // adjust pointers
      outputBufPutback_ += availBytes;
      outputBufCount_ -= availBytes;
      if (outputBufPutback_ > DCMZLIBINPUTFILTER_PUTBACKSIZE)
      {
        outputBufStart_ += outputBufPutback_ - DCMZLIBINPUTFILTER_PUTBACKSIZE;
        outputBufPutback_ = DCMZLIBINPUTFILTER_PUTBACKSIZE;
        if (outputBufStart_ >= DCMZLIBINPUTFILTER_BUFSIZE) outputBufStart_ -= DCMZLIBINPUTFILTER_BUFSIZE;
      }
    }

    // refill output buffer
    fillOutputBuffer();
  } while (skiplen && outputBufCount_);

  // we're either done or the output buffer is empty because of producer suspension
  return result;
}

void DcmZLibInputFilter::putback(offile_off_t num)
{
  if (num > outputBufPutback_) status_ = EC_PutbackFailed;
  else
  {
    outputBufPutback_ -= num;
    outputBufCount_ += num;
  }
}

void DcmZLibInputFilter::append(DcmProducer& producer)
{
  current_ = &producer;
}

offile_off_t DcmZLibInputFilter::fillInputBuffer()
{
  offile_off_t result = 0;
  if (status_.good() && current_ && (inputBufCount_ < DCMZLIBINPUTFILTER_BUFSIZE))
  {

    // use first part of input buffer
    if (inputBufStart_ + inputBufCount_ < DCMZLIBINPUTFILTER_BUFSIZE)
    {
      result = current_->read(inputBuf_ + inputBufStart_ + inputBufCount_,
        DCMZLIBINPUTFILTER_BUFSIZE - (inputBufStart_ + inputBufCount_));

      inputBufCount_ += result;

      if (result == 0)
      {
        if (current_->eos() && !padded_)
        {
           // producer has signalled eos, now append zero pad byte that makes
           // zlib recognize the end of stream when no zlib header is present
           *(inputBuf_ + inputBufStart_ + inputBufCount_) = 0;
           inputBufCount_++;
           padded_ = OFTrue;
        }
        return result; // producer suspension
      }
    }

    // use second part of input buffer
    if (inputBufCount_ < DCMZLIBINPUTFILTER_BUFSIZE &&
        inputBufStart_ + inputBufCount_ >= DCMZLIBINPUTFILTER_BUFSIZE)
    {
      offile_off_t result2 = current_->read(inputBuf_ + (inputBufStart_ + inputBufCount_ - DCMZLIBINPUTFILTER_BUFSIZE),
        DCMZLIBINPUTFILTER_BUFSIZE - inputBufCount_);

      inputBufCount_ += result2;
      result += result2;

      if (result2 == 0 && current_->eos() && !padded_)
      {
         // producer has signalled eos, now append zero pad byte that makes
         // zlib recognize the end of stream when no zlib header is present
         *(inputBuf_ + inputBufStart_ + inputBufCount_ - DCMZLIBINPUTFILTER_BUFSIZE) = 0;
         inputBufCount_++;
         padded_ = OFTrue;
      }
    }
  }
  return result;
}


offile_off_t DcmZLibInputFilter::decompress(const void *buf, offile_off_t buflen)
{
  offile_off_t result = 0;

  zstream_->next_out = OFstatic_cast(Bytef *, OFconst_cast(void *, buf));
  zstream_->avail_out = OFstatic_cast(uInt, buflen);
  int astatus;

  // decompress from inputBufStart_ to end of data or end of buffer, whatever comes first
  offile_off_t numBytes = (inputBufStart_ + inputBufCount_ > DCMZLIBINPUTFILTER_BUFSIZE) ?
         (DCMZLIBINPUTFILTER_BUFSIZE - inputBufStart_) : inputBufCount_ ;

  if (numBytes || buflen)
  {
    zstream_->next_in = OFstatic_cast(Bytef *, inputBuf_ + inputBufStart_);
    zstream_->avail_in = OFstatic_cast(uInt, numBytes);
    astatus = inflate(zstream_, 0);

    if (astatus == Z_OK || astatus == Z_BUF_ERROR) { /* everything OK */ }
    else if (astatus == Z_STREAM_END)
    {
#ifdef DEBUG
       if (!eos_)
       {
         offile_off_t count = inputBufCount_ - (numBytes - OFstatic_cast(offile_off_t, zstream_->avail_in));
         if (count > 2)
         {
           /* we silently ignore up to two trailing bytes after the end of the
            * deflated stream. One byte has been added by ourselves to make sure
            * zlib detects eos, another one might be the padding necessary for
            * odd length zlib streams transmitted through a DICOM network
            * (where PDVs always have even length).
            * Everything else generates a warning.
            */
           DCMDATA_WARN("zlib: " << OFstatic_cast(ulong, count-1) << " pending input bytes in buffer.");
         }
       }
#endif
       eos_ = OFTrue;
    }
    else
    {
      OFString etext = "ZLib Error: ";
      if (zstream_->msg) etext += zstream_->msg;
      status_ = makeOFCondition(OFM_dcmdata, 16, OF_error, etext.c_str());
    }

    // adjust counters
    inputBufStart_ += numBytes - OFstatic_cast(offile_off_t, zstream_->avail_in);
    inputBufCount_ -= numBytes - OFstatic_cast(offile_off_t, zstream_->avail_in);

    if (inputBufStart_ == DCMZLIBINPUTFILTER_BUFSIZE)
    {
      // wrapped around
      inputBufStart_ = 0;

      // now flush to end of data
      if (inputBufCount_ && (zstream_->avail_out > 0))
      {
        zstream_->next_in = OFstatic_cast(Bytef *, inputBuf_);
        zstream_->avail_in = OFstatic_cast(uInt, inputBufCount_);
        astatus = inflate(zstream_, 0);

        if (astatus == Z_OK || astatus == Z_BUF_ERROR) { /* everything OK */ }
        else if (astatus == Z_STREAM_END)
        {
#ifdef DEBUG
          if (!eos_)
          {
             offile_off_t count = OFstatic_cast(offile_off_t, zstream_->avail_in);
             if (count > 2)
             {
               /* we silently ignore up to two trailing bytes after the end of the
                * deflated stream. One byte has been added by ourselves to make sure
                * zlib detects eos, another one might be the padding necessary for
                * odd length zlib streams transmitted through a DICOM network
                * (where PDVs always have even length).
                * Everything else generates a warning.
                */
               DCMDATA_WARN("zlib: " << OFstatic_cast(ulong, count-1) << " pending input bytes in buffer.");
             }
          }
#endif
          eos_ = OFTrue;
        }
        else
        {
          OFString etext = "ZLib Error: ";
          if (zstream_->msg) etext += zstream_->msg;
          status_ = makeOFCondition(OFM_dcmdata, 16, OF_error, etext.c_str());
        }

        // adjust counters
        inputBufStart_ += inputBufCount_ - OFstatic_cast(offile_off_t, zstream_->avail_in);
        inputBufCount_ = OFstatic_cast(offile_off_t, zstream_->avail_in);

      }
    }

    // reset buffer start to make things faster
    if (inputBufCount_ == 0) inputBufStart_ = 0;

    // compute result
    result = buflen - OFstatic_cast(offile_off_t, zstream_->avail_out);
  }
  return result;
}

void DcmZLibInputFilter::fillOutputBuffer()
{
  offile_off_t inputBytes = 0;
  offile_off_t outputBytes = 0;
  offile_off_t offset = 0;
  offile_off_t availBytes = 0;
  do
  {
    inputBytes = fillInputBuffer();

    // determine next block of free space in output buffer
    offset = outputBufStart_ + outputBufPutback_ + outputBufCount_;
    if (offset >= DCMZLIBINPUTFILTER_BUFSIZE) offset -= DCMZLIBINPUTFILTER_BUFSIZE;

    availBytes = DCMZLIBINPUTFILTER_BUFSIZE - (outputBufPutback_ + outputBufCount_);
    if (offset + availBytes > DCMZLIBINPUTFILTER_BUFSIZE) availBytes = DCMZLIBINPUTFILTER_BUFSIZE - offset;

    // decompress to output buffer
    outputBytes = decompress(outputBuf_ + offset, availBytes);
    outputBufCount_ += outputBytes;
  }
  while (inputBytes || outputBytes);
}

#else  /* WITH_ZLIB */

/* make sure that the object file is not completely empty if compiled
 * without zlib because some linkers might fail otherwise.
 */
void dcistrmz_dummy_function()
{
  return;
}

#endif /* WITH_ZLIB */


/*
 * CVS/RCS Log:
 * $Log: dcistrmz.cc,v $
 * Revision 1.13  2010-12-01 13:21:18  uli
 * Fixed build problem with MSC6 when zlib support is enabled.
 *
 * Revision 1.12  2010-10-14 13:14:08  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.11  2010-08-05 08:38:10  uli
 * Fixed some warnings from -Wold-style-cast.
 *
 * Revision 1.10  2010-02-22 11:39:54  uli
 * Remove some unneeded includes.
 *
 * Revision 1.9  2009-11-04 09:58:09  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.8  2007-02-19 15:45:31  meichel
 * Class DcmInputStream and related classes are now safe for use with
 *   large files (2 GBytes or more) if supported by compiler and operating system.
 *
 * Revision 1.7  2006/08/15 15:49:54  meichel
 * Updated all code in module dcmdata to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.6  2005/12/08 15:41:15  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.5  2004/04/07 12:19:14  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.4  2002/12/20 14:55:34  wilkens
 * Inserted three casts in order to get rid of compiler warning on Solaris 2.5.1
 * using compiler SC 2.0.1.
 *
 * Revision 1.3  2002/09/19 08:32:28  joergr
 * Added explicit type casts to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.2  2002/08/29 15:57:49  meichel
 * Updated zlib-related classes to correctly compile when WITH_ZLIB is undefined
 *
 * Revision 1.1  2002/08/27 16:55:50  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 *
 */
