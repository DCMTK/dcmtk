/*
 *
 *  Copyright (C) 1994-2005, OFFIS
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
 *  Purpose: DcmOutputBufferStream and related classes,
 *    implements output to blocks of memory as needed in the dcmnet module.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-12-08 16:28:25 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/dcmtk/dcmdata/dcostrmb.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCOSTRMB_H
#define DCOSTRMB_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcostrma.h"

/** consumer class that stores data in a buffer provided by the caller.
 *  Used for DICOM network communication.
 */
class DcmBufferConsumer: public DcmConsumer
{
public:

  /** constructor
   *  @param buf buffer in which data is stored. Must be allocated
   *    by caller and remain valid during the lifetime of this object.
   *  @param bufLen buffer length, must be even number > 0.
   */
  DcmBufferConsumer(void *buf, Uint32 bufLen);

  /// destructor
  virtual ~DcmBufferConsumer();

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
  virtual Uint32 avail() const;

  /** processes as many bytes as possible from the given input block.
   *  @param buf pointer to memory block, must not be NULL
   *  @param buflen length of memory block
   *  @return number of bytes actually processed. 
   */
  virtual Uint32 write(const void *buf, Uint32 buflen);

  /** instructs the consumer to flush its internal content until
   *  either the consumer becomes "flushed" or I/O suspension occurs.
   *  After a call to flush(), a call to write() will produce undefined
   *  behaviour.
   */
  virtual void flush();

  /** retrieves and flushes the underlying buffer.
   *  After return of this method, the buffer is considered to have
   *  been flushed (copied, stored) by the caller and is reused
   *  by the next write operation.
   *  @param buffer pointer to user provided buffer returned in this parameter
   *  @param length number of bytes in buffer returned in this parameter
   */
  virtual void flushBuffer(void *& buffer, Uint32& length);

private:

  /// private unimplemented copy constructor
  DcmBufferConsumer(const DcmBufferConsumer&);

  /// private unimplemented copy assignment operator
  DcmBufferConsumer& operator=(const DcmBufferConsumer&);

  /// the buffer we're actually writing to
  unsigned char *buffer_;

  /// size of the buffer, in bytes
  Uint32 bufSize_;

  /// number of bytes filled in buffer
  Uint32 filled_;

  /// status
  OFCondition status_;
};


/** output stream that writes into a buffer of fixed length
 *  which must be provided by the caller.
 */
class DcmOutputBufferStream: public DcmOutputStream
{
public:
  /** constructor
   *  @param buf buffer in which data is stored. Must be allocated
   *    by caller and remain valid during the lifetime of this object.
   *  @param bufLen buffer length, must be even number > 0.
   */
  DcmOutputBufferStream(void *buf, Uint32 bufLen);

  /// destructor
  virtual ~DcmOutputBufferStream();

  /** retrieves and flushes the underlying buffer.
   *  After return of this method, the buffer is considered to have
   *  been flushed (copied, stored) by the caller and is reused
   *  by the next write operation.
   *  @param buffer pointer to user provided buffer returned in this parameter
   *  @param length number of bytes in buffer returned in this parameter
   */
  virtual void flushBuffer(void *& buffer, Uint32& length);

private:

  /// private unimplemented copy constructor
  DcmOutputBufferStream(const DcmOutputBufferStream&);

  /// private unimplemented copy assignment operator
  DcmOutputBufferStream& operator=(const DcmOutputBufferStream&);

  /// the final consumer of the filter chain
  DcmBufferConsumer consumer_;

};


#endif

/*
 * CVS/RCS Log:
 * $Log: dcostrmb.h,v $
 * Revision 1.2  2005-12-08 16:28:25  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2002/08/27 16:55:36  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 *
 */
