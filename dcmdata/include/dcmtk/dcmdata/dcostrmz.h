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
 *  Purpose: zlib compression filter for output streams
 *
 */

#ifndef DCOSTRMZ_H
#define DCOSTRMZ_H

#include "dcmtk/config/osconfig.h"

#ifdef WITH_ZLIB

#include "dcmtk/dcmdata/dcostrma.h" /* for DcmOutputFilter */

BEGIN_EXTERN_C
#include <zlib.h>
END_EXTERN_C

/** global flag defining the compression level for zlib (deflate) compression.
 *  Legal values are 0..9. Default is Z_DEFAULT_COMPRESSION which is defined
 *  to be 6 for the current zlib implementation.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<int> dcmZlibCompressionLevel;

/** zlib compression filter for output streams
 */
class DCMTK_DCMDATA_EXPORT DcmZLibOutputFilter: public DcmOutputFilter
{
public:

  /// default constructor
  DcmZLibOutputFilter();

  /// destructor
  virtual ~DcmZLibOutputFilter();

  /** returns the status of the consumer. Unless the status is good,
   *  the consumer will not permit any operation.
   *  @return status, true if good
   */
  virtual OFBool good() const;

  /** returns the status of the consumer as an OFCondition object.
   *  Unless the status is good, the consumer will not permit any operation.
   *  @return status, EC_Normal if good
   */
  virtual OFCondition status() const;

  /** returns true if the consumer is flushed, i.e. has no more data
   *  pending in it's internal state that needs to be flushed before
   *  the stream is closed.
   *  @return true if consumer is flushed, false otherwise
   */
  virtual OFBool isFlushed() const;

  /** returns the minimum number of bytes that can be written with the
   *  next call to write(). The DcmObject write methods rely on avail
   *  to return a value > 0 if there is no I/O suspension since certain
   *  data such as tag and length are only written "en bloc", i.e. all
   *  or nothing.
   *  @return minimum of space available in consumer
   */
  virtual offile_off_t avail() const;

  /** processes as many bytes as possible from the given input block.
   *  @param buf pointer to memory block, must not be NULL
   *  @param buflen length of memory block
   *  @return number of bytes actually processed.
   */
  virtual offile_off_t write(const void *buf, offile_off_t buflen);

  /** instructs the consumer to flush its internal content until
   *  either the consumer becomes "flushed" or I/O suspension occurs.
   *  After a call to flush(), a call to write() will produce undefined
   *  behaviour.
   */
  virtual void flush();

  /** determines the consumer to which the filter is supposed
   *  to write it's output.  Once a consumer for the output filter has
   *  been defined, it cannot be changed anymore during the lifetime
   *  of the object.
   *  @param consumer reference to consumer, must not be circular chain
   */
  virtual void append(DcmConsumer& consumer);

private:

  /// private unimplemented copy constructor
  DcmZLibOutputFilter(const DcmZLibOutputFilter&);

  /// private unimplemented copy assignment operator
  DcmZLibOutputFilter& operator=(const DcmZLibOutputFilter&);

  /** writes the content of the output ring buffer
   *  to the next filter stage until the output ring buffer
   *  becomes empty or the next filter stage becomes full
   */
  void flushOutputBuffer();

  /** feed data from the given block of data to the compression
   *  codec until complete or the output ring buffer becomes full.
   *  Does not flush the output ring buffer.
   *  @param buf pointer to input data
   *  @param buflen number of bytes in buf
   *  @param finalize true if the current block of data constitutes
   *    the end of the input stream, i.e. the compression codec
   *    should be forced to flush its internal state.
   *  @return number of bytes processed
   */
  offile_off_t compress(const void *buf, offile_off_t buflen, OFBool finalize);

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

  /// pointer to consumer to which compressed output is written
  DcmConsumer *current_;

  /// pointer to struct z_stream object containing the zlib status
  z_streamp zstream_;

  /// status
  OFCondition status_;

  /// true if the zlib object has reported Z_STREAM_END
  OFBool flushed_;

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

  /// number of bytes in output ring buffer
  offile_off_t outputBufCount_;

};

#endif
#endif
