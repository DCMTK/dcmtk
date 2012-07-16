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
 *  Purpose: DcmInputBufferStream and related classes,
 *    implements input to blocks of memory as needed in the dcmnet module.
 *
 */

#ifndef DCISTRMB_H
#define DCISTRMB_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcistrma.h"


/** producer class that reads data from a buffer provided by the caller.
 *  Used for DICOM network communication.
 */
class DCMTK_DCMDATA_EXPORT DcmBufferProducer: public DcmProducer
{
public:
  /** constructor
   */
  DcmBufferProducer();

  /// destructor
  virtual ~DcmBufferProducer();

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

  /** adds the content of the given buffer to the input stream.
   *  @param buf buffer from which data is read. Must be allocated
   *    by caller and remain valid until releaseBuffer() is called
   *  @param buflen buffer length, must be even number > 0.
   */
  virtual void setBuffer(const void *buf, offile_off_t buflen);

  /** releases the current buffer. Should only be called when
   *  the content of the buffer has been read as far as possible.
   *  Pending input from the buffer is copied into an internal
   *  backup buffer if necessary. If an overflow condition
   *  occurs, the producer status becomes bad.
   */
  virtual void releaseBuffer();

  /** marks the end of stream, i.e. the data provided with the last call
   *  to setBuffer is the last data available in the stream.
   */
  virtual void setEos();

private:

  /// private unimplemented copy constructor
  DcmBufferProducer(const DcmBufferProducer&);

  /// private unimplemented copy assignment operator
  DcmBufferProducer& operator=(const DcmBufferProducer&);

  /// the user buffer we're actually reading from
  unsigned char *buffer_;

  /// the backup buffer
  unsigned char *backup_;

  /// size of the user buffer, in bytes
  offile_off_t bufSize_;

  /// number of bytes read from the user buffer
  offile_off_t bufIndex_;

  /// number of bytes read from the backup buffer
  offile_off_t backupIndex_;

  /// index of first valid byte in backup buffer, for putback
  offile_off_t backupStart_;

  /// status
  OFCondition status_;

  /// true if setEos has been called before
  OFBool eosflag_;

};


/** input stream that reads from a buffer of fixed length
 *  which must be provided by the caller.
 */
class DCMTK_DCMDATA_EXPORT DcmInputBufferStream: public DcmInputStream
{
public:
  /** constructor
   */
  DcmInputBufferStream();

  /// destructor
  virtual ~DcmInputBufferStream();

  /** creates a new factory object for the current stream
   *  and stream position.  When activated, the factory will be
   *  able to create new DcmInputStream delivering the same
   *  data as the current stream.  Used to defer loading of
   *  value fields until accessed.
   *  If no factory object can be created (e.g. because the
   *  stream is not seekable), returns NULL.
   *  @return pointer to new factory object if successful, NULL otherwise.
   */
  virtual DcmInputStreamFactory *newFactory() const;

  /** adds the content of the given buffer to the input stream.
   *  @param buf buffer from which data is read. Must be allocated
   *    by caller and remain valid until releaseBuffer() is called
   *  @param buflen buffer length, must be even number > 0.
   */
  virtual void setBuffer(const void *buf, offile_off_t buflen);

  /** releases the current buffer. Should only be called when
   *  the content of the buffer has been read as far as possible.
   *  Pending input from the buffer is copied into an internal
   *  backup buffer if necessary. If an overflow condition
   *  occurs, the producer status becomes bad.
   */
  virtual void releaseBuffer();

  /** marks the end of stream, i.e. the data provided with the last call
   *  to setBuffer is the last data available in the stream.
   */
  virtual void setEos();

private:

  /// private unimplemented copy constructor
  DcmInputBufferStream(const DcmInputBufferStream&);

  /// private unimplemented copy assignment operator
  DcmInputBufferStream& operator=(const DcmInputBufferStream&);

  /// the final producer of the filter chain
  DcmBufferProducer producer_;

};


#endif
