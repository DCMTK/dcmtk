/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
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
 *  Purpose: RLE decompressor
 *
 */

#ifndef DCRLEDEC_H
#define DCRLEDEC_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcerror.h"

/** this class implements an RLE decompressor conforming to the DICOM standard.
 *  The class is loosely based on an implementation by Phil Norman <forrey@eh.org>
 */
class DCMTK_DCMDATA_EXPORT DcmRLEDecoder
{
public:

  /** constructor
   *  @param outputBufferSize size of the output buffer (in bytes)
   *    to which the RLE codec will write decompressed output.
   */
  DcmRLEDecoder(size_t outputBufferSize)
  : fail_(0)
  , outputBufferSize_(outputBufferSize)
  , outputBuffer_(NULL)
  , offset_(0)
  , suspendInfo_(128)
  {
    if (outputBufferSize_ == 0) fail_ = 1;
    else
    {
      outputBuffer_ = new unsigned char[outputBufferSize_];
      if (outputBuffer_ == NULL) fail_ = 1;
    }
  }

  /// destructor
  ~DcmRLEDecoder()
  {
    delete[] outputBuffer_;
  }

  /** resets the decoder object to newly constructed state.
   *  The size and location of the output buffer is not changed.
   */
  inline void clear()
  {
    offset_ = 0;
    suspendInfo_ = 128;
    if (outputBuffer_) fail_ = 0;
  }


  inline OFCondition decompress(void *compressedData, size_t compressedSize)
  {
    // we allow a call for zero bytes
    if (compressedSize == 0) return EC_Normal;

    OFCondition result = EC_IllegalCall;

    // check parameters passed by caller
    if (compressedData == NULL) fail_ = 1;

    if (! fail_)  // if fail_ is true, just ignore input
    {
      result = EC_Normal;
      unsigned char ch;
      unsigned char nbytes;
      unsigned char *cp = OFstatic_cast(unsigned char *, compressedData);

      // check if we suspended last time, clean up
      if (suspendInfo_ > 128)
      {
        // DICOM packbit scheme uses 257 - nbytes to represent replicate runs
        nbytes = OFstatic_cast(unsigned char, 257 - suspendInfo_);

        // suspended replicate run. compressedSize cannot be zero now.
        suspendInfo_ = 128;

        ch = *cp++;
        --compressedSize;
        replicate(ch, nbytes);
      }
      else if (suspendInfo_ < 128)
      {
        // suspended literal run
        nbytes = OFstatic_cast(unsigned char, (suspendInfo_ & 0x7f) + 1);
        suspendInfo_ = 128;
        if (compressedSize < nbytes)
        {
          // we're going to suspend again (oops?), prepare everything
          suspendInfo_ = OFstatic_cast(unsigned char, nbytes - compressedSize - 1);
          nbytes = OFstatic_cast(unsigned char, compressedSize);
          result = EC_StreamNotifyClient;
        }

        literal(cp, nbytes);
        compressedSize -= nbytes;
        cp += nbytes;
      }

      // continue with ordinary RLE decompression
      while (compressedSize && (! fail_))
      {
        ch = *cp++;
        --compressedSize;

        if (ch & 0x80)
        {
          // replicate run
          if (compressedSize)
          {
            // DICOM packbit scheme uses 257 - nbytes to represent replicate runs
            nbytes = OFstatic_cast(unsigned char, 257 - ch);
            ch = *cp++;
            --compressedSize;
            replicate(ch, nbytes);
          }
          else
          {
            // suspension: replicate run but second byte is in next block
            suspendInfo_ = ch;
            result = EC_StreamNotifyClient;
          }
        } 
        else
        {
          // literal run
          nbytes = OFstatic_cast(unsigned char, (ch & 0x7f) + 1);
          if (compressedSize < nbytes)
          {
            // we're going to suspend, prepare everything
            suspendInfo_ = OFstatic_cast(unsigned char, nbytes - compressedSize - 1);
            nbytes = OFstatic_cast(unsigned char, compressedSize);
            result = EC_StreamNotifyClient;
          }

          literal(cp, nbytes);
          compressedSize -= nbytes;
          cp += nbytes;
        }
      }

      // failure status at this point means output buffer overflow
      if (fail_) result = EC_CorruptedData;
    }

    return result;
  }


  /** returns the number of bytes written to the output buffer
   *  @return size of decompressed stream, in bytes
   */
  inline size_t size() const
  {
    return offset_;
  }

  /** returns pointer to the output buffer
   */
  inline void *getOutputBuffer() const
  {
    return outputBuffer_;
  }

  /** returns true if the RLE compressor has failed (out of memory or output buffer too small).
   */
  inline OFBool fail() const
  {
    if (fail_) return OFTrue; else return OFFalse;
  }

private:

  /// private undefined copy constructor
  DcmRLEDecoder(const DcmRLEDecoder&);

  /// private undefined copy assignment operator
  DcmRLEDecoder& operator=(const DcmRLEDecoder&);


  /** this method expands a replicate run
   *  @param ch value to replicate
   *  @param nbytes number of repetitions
   */
  inline void replicate(unsigned char ch, unsigned char nbytes)
  {
     if (offset_ + nbytes > outputBufferSize_)
     {
       // output buffer overflow
       fail_ = 1;
       nbytes = OFstatic_cast(unsigned char, outputBufferSize_ - offset_);
     }

     while (nbytes--) outputBuffer_[offset_++] = ch;
  }


  /** this method expands a literal run
   *  @param cp pointer to buffer
   *  @param nbytes number of bytes in buffer
   */
  inline void literal(unsigned char *cp, unsigned char nbytes)
  {
     if (offset_ + nbytes > outputBufferSize_)
     {
       // output buffer overflow
       fail_ = 1;
       nbytes = OFstatic_cast(unsigned char, outputBufferSize_ - offset_);
     }

     while (nbytes--) outputBuffer_[offset_++] = *cp++;
  }

  /* member variables */

  /** this flag indicates a failure of the RLE codec.  Once a failure is
   *  flagged, the codec will consume all input and not produce any more
   *  output.
   */
  int fail_;

  /** size of output buffer, in bytes
   */
  size_t outputBufferSize_;

  /** this member points to a block of size outputBufferSize_
   *  (unless fail_ is true). This is the block of data to
   *  which the decompressed stream is written
   */
  unsigned char *outputBuffer_;

  /** contains the number of bytes already written to outputBuffer_.
   *  Value is always less or equal to outputBufferSize_.
   */
  size_t offset_;

  /** contains suspension information.  
   *  If not suspended, contains 128.
   *  If suspended during a replicate run, contains control byte of repeat run (> 128).
   *  If suspended during a literal run, contains number of remaining bytes in literal run minus 1 (< 128).
   */
  unsigned char suspendInfo_;
};

#endif
