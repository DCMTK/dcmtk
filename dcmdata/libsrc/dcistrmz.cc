/*
 *
 *  Copyright (C) 1994-2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: zlib compression filter for input streams
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-08-29 15:57:49 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcistrmz.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"

#ifdef WITH_ZLIB

#include "dcistrmz.h"
#include "dcerror.h"
#include "ofconsol.h"

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

OFBool DcmZLibInputFilter::eos() const
{
  if (status_.bad() || (current_ == NULL)) return OFTrue;

  // there may be trailing garbage after the compressed stream.
  // we report eos if the decompressor has reported Z_STREAM_END
  // and the output buffer is empty.
  return (outputBufCount_ == 0) && eos_;
}

Uint32 DcmZLibInputFilter::avail() const
{
  if (status_.good()) return outputBufCount_; else return 0;
}

Uint32 DcmZLibInputFilter::read(void *buf, Uint32 buflen)
{
  if (status_.bad() || (current_ == NULL) || (buf == NULL)) return 0;
  unsigned char *target = (unsigned char *)buf;
  Uint32 offset = 0;
  Uint32 availBytes = 0;
  Uint32 result = 0;
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

      if (availBytes) memcpy(target, outputBuf_ + offset, availBytes);
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

Uint32 DcmZLibInputFilter::skip(Uint32 skiplen)
{
  if (status_.bad() || (current_ == NULL)) return 0;
  Uint32 offset = 0;
  Uint32 availBytes = 0;
  Uint32 result = 0;
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

void DcmZLibInputFilter::putback(Uint32 num)
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

Uint32 DcmZLibInputFilter::fillInputBuffer()
{
  Uint32 result = 0;
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
      Uint32 result2 = current_->read(inputBuf_ + (inputBufStart_ + inputBufCount_ - DCMZLIBINPUTFILTER_BUFSIZE), 
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


Uint32 DcmZLibInputFilter::decompress(const void *buf, Uint32 buflen)
{
  Uint32 result = 0;

  zstream_->next_out = (Bytef *)buf;
  zstream_->avail_out = (uInt)buflen;
  int astatus;

  // decompress from inputBufStart_ to end of data or end of buffer, whatever comes first
  Uint32 numBytes = (inputBufStart_ + inputBufCount_ > DCMZLIBINPUTFILTER_BUFSIZE) ? 
         (DCMZLIBINPUTFILTER_BUFSIZE - inputBufStart_) : inputBufCount_ ;

  if (numBytes || buflen)
  {
    zstream_->next_in = (Bytef *)(inputBuf_ + inputBufStart_);
    zstream_->avail_in = (uInt) numBytes;
    astatus = inflate(zstream_, 0);      

    if (astatus == Z_OK || astatus == Z_BUF_ERROR) { /* everything OK */ }
    else if (astatus == Z_STREAM_END)
    {
#ifdef DEBUG
       if (!eos_)
       {
         Uint32 count = inputBufCount_ - (numBytes - (Uint32)(zstream_->avail_in));
         if (count > 2) 
         {
           /* we silently ignore up to two trailing bytes after the end of the 
            * deflated stream. One byte has been added by ourselves to make sure
            * zlib detects eos, another one might be the padding necessary for
            * odd length zlib streams transmitted through a DICOM network
            * (where PDVs always have even length).
            * Everything else generates a warning.
            */
           ofConsole.lockCerr() << "zlib: " << count-1 << " pending input bytes in buffer." << endl;
           ofConsole.unlockCerr();
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
    inputBufStart_ += numBytes - (Uint32)(zstream_->avail_in);
    inputBufCount_ -= numBytes - (Uint32)(zstream_->avail_in);

    if (inputBufStart_ == DCMZLIBINPUTFILTER_BUFSIZE)
    {
      // wrapped around
      inputBufStart_ = 0;
    
      // now flush to end of data
      if (inputBufCount_ && (zstream_->avail_out > 0))
      {
        zstream_->next_in = (Bytef *)inputBuf_;
        zstream_->avail_in = (uInt) inputBufCount_;
        astatus = inflate(zstream_, 0);      

        if (astatus == Z_OK || astatus == Z_BUF_ERROR) { /* everything OK */ }
        else if (astatus == Z_STREAM_END) 
        {
#ifdef DEBUG
          if (!eos_)
          {
             Uint32 count = (Uint32)(zstream_->avail_in);
             if (count > 2) 
             {
               /* we silently ignore up to two trailing bytes after the end of the 
                * deflated stream. One byte has been added by ourselves to make sure
                * zlib detects eos, another one might be the padding necessary for
                * odd length zlib streams transmitted through a DICOM network
                * (where PDVs always have even length).
                * Everything else generates a warning.
                */
               ofConsole.lockCerr() << "zlib: " << count-1 << " pending input bytes in buffer." << endl;
               ofConsole.unlockCerr();
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
        inputBufStart_ += inputBufCount_ - (Uint32)(zstream_->avail_in);           
        inputBufCount_ = (Uint32)(zstream_->avail_in);

      }
    }     

    // reset buffer start to make things faster
    if (inputBufCount_ == 0) inputBufStart_ = 0;

    // compute result
    result = buflen - (Uint32)(zstream_->avail_out);
  }
  return result;
}

void DcmZLibInputFilter::fillOutputBuffer()
{
  Uint32 inputBytes = 0;
  Uint32 outputBytes = 0;
  Uint32 offset = 0;
  Uint32 availBytes = 0;
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
 * Revision 1.2  2002-08-29 15:57:49  meichel
 * Updated zlib-related classes to correctly compile when WITH_ZLIB is undefined
 *
 * Revision 1.1  2002/08/27 16:55:50  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 *
 */
