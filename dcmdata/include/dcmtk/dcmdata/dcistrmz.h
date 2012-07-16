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
 *  Purpose: zlib compression filter for input streams
 *
 */

#ifndef DCISTRMZ_H
#define DCISTRMZ_H

#include "dcmtk/config/osconfig.h"

#ifdef WITH_ZLIB

#include "dcmtk/dcmdata/dcistrma.h" /* for DcmInputFilter */

BEGIN_EXTERN_C
#include <zlib.h>
END_EXTERN_C

/** global flag defining the behaviour of the zlib decompressor.
 *  When true, the input stream is expected to be in deflated ZLIB format 
 *  instead of deflated bitstream format (i.e. RFC 1950 instead of RFC 1951).
 *  Default is false, i.e. DICOM compliant behavior.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmZlibExpectRFC1950Encoding;

/** zlib compression filter for input streams
 */
class DCMTK_DCMDATA_EXPORT DcmZLibInputFilter: public DcmInputFilter
{
public:

  /// default constructor
  DcmZLibInputFilter();

  /// destructor
  virtual ~DcmZLibInputFilter();

  /** returns the status of the producer. Unless the status is good,
   *  the producer will not permit any operation.
   *  @return status, true if good
   */
  virtual OFBool good() const;

  /** returns the status of the producer as an OFCondition object.
   *  Unless the status is good, the producer will not permit any operation.
   *  @return status, EC_Normal if good
   */
  virtual OFCondition status() const;

  /** returns true if the producer is at the end of stream.
   *  @return true if end of stream, false otherwise
   */
  virtual OFBool eos();

  /** returns the minimum number of bytes that can be read with the
   *  next call to read(). The DcmObject read methods rely on avail
   *  to return a value > 0 if there is no I/O suspension since certain
   *  data such as tag and length are only read "en bloc", i.e. all
   *  or nothing.
   *  @return minimum of data available in producer
   */
  virtual offile_off_t avail();

  /** reads as many bytes as possible into the given block.
   *  @param buf pointer to memory block, must not be NULL
   *  @param buflen length of memory block
   *  @return number of bytes actually read. 
   */
  virtual offile_off_t read(void *buf, offile_off_t buflen);

  /** skips over the given number of bytes (or less)
   *  @param skiplen number of bytes to skip
   *  @return number of bytes actually skipped. 
   */
  virtual offile_off_t skip(offile_off_t skiplen);

  /** resets the stream to the position by the given number of bytes.
   *  @param num number of bytes to putback. If the putback operation
   *    fails, the producer status becomes bad. 
   */
  virtual void putback(offile_off_t num);

  /** determines the producer from which the filter is supposed
   *  to read it's input. Once a producer for the input filter has 
   *  been defined, it cannot be changed anymore during the lifetime
   *  of the object.
   *  @param producer reference to producer, must not be circular chain
   */
  virtual void append(DcmProducer& producer);

private:

  /// private unimplemented copy constructor
  DcmZLibInputFilter(const DcmZLibInputFilter&);

  /// private unimplemented copy assignment operator
  DcmZLibInputFilter& operator=(const DcmZLibInputFilter&);

  /** reads data from the producer into the input ring buffer
   *  until the input ring buffer becomes full or the
   *  producer suspends
   *  @return number of bytes added to input buffer
   */
  offile_off_t fillInputBuffer();

  /** decompress data into the given output block until
   *  complete or no more input is available in the input ring buffer.
   *  does not re-fill the input ring buffer from the producer.
   *  @param buf pointer to input data
   *  @param buflen number of bytes in buf
   *  @return number of bytes processed
   */   
  offile_off_t decompress(const void *buf, offile_off_t buflen);

  /** reads and decompresses data from the producer
   *  until the producer suspends or the output ring buffer becomes full.
   */
  void fillOutputBuffer();
  
  /** writes the content of the output ring buffer
   *  to the next filter stage until the output ring buffer
   *  becomes empty or the next filter stage becomes full
   */
  void flushOutputBuffer();

  /** copies as much of the given block of data as possible
   *  in the input ring buffer
   *  @param buf pointer to input data
   *  @param buflen number of bytes in buf
   *  @return number of bytes copied to input ring buffer
   */
  offile_off_t fillInputBuffer(const void *buf, offile_off_t buflen);

  /** feed data from the input ring buffer to the compression
   *  codec until complete or the output ring buffer becomes full.
   *  Does not flush the output ring buffer.
   *  @param finalize true if the content of the input ring buffer 
   *    constitutes the end of the input stream, i.e. the compression codec
   *    should be forced to flush its internal state.
   */   
  void compressInputBuffer(OFBool finalize);


  /// pointer to producer from which compressed input is read
  DcmProducer *current_;

  /// pointer to struct z_stream object containing the zlib status
  z_streamp zstream_;

  /// status
  OFCondition status_;

  /// true if the zlib object has reported Z_STREAM_END
  OFBool eos_;

  /// input ring buffer
  unsigned char *inputBuf_;

  /// offset of first byte in input ring buffer
  offile_off_t inputBufStart_;

  /// number of bytes in input ring buffer
  offile_off_t inputBufCount_;

  /// output ring buffer
  unsigned char *outputBuf_;

  /// offset of first byte in output ring buffer
  offile_off_t outputBufStart_;

  /// number of bytes available for read in output ring buffer
  offile_off_t outputBufCount_;

  /// number of putback bytes in output ring buffer
  offile_off_t outputBufPutback_;

  /// flag indicating whether or not a pad byte has been appended to the input stream
  OFBool padded_;
};

#endif
#endif
