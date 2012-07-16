/*
 *
 *  Copyright (C) 2007-2011, OFFIS e.V.
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
 *  Purpose: file cache facility for DcmElement::getPartialValue
 *
 */


#ifndef DCWCACHE_H
#define DCWCACHE_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftypes.h"      /* for Uint8 */
#include "dcmtk/dcmdata/dcfcache.h"   /* for class DcmFileCache */

class DcmElement;
class DcmOutputStream;

#define DcmWriteCacheBufsize 65536    /* buffer size, in bytes */

/** This class implements a buffering mechanism that is used when writing large
 *  elements that reside in file into an output stream. DcmElement::getPartialValue
 *  is used to fill the buffer maintained by this class, and the buffer content
 *  is then copied to the output stream. The intermediate buffer is necessary
 *  because both DcmElement::getPartialValue and DcmOutputStream::write expect
 *  a buffer to write to and read from, respectively.
 */
class DCMTK_DCMDATA_EXPORT DcmWriteCache
{
public:

  /// default constructor. Construction is cheap (no allocation of memory block).
  DcmWriteCache()
  : fcache_()
  , buf_(NULL)
  , owner_(NULL)
  , offset_(0)
  , numBytes_(0)
  , capacity_(0)
  , fieldLength_(0)
  , fieldOffset_(0)
  , byteOrder_(EBO_unknown)
  {
  }

  /// destructor
  ~DcmWriteCache()
  {
    delete[] buf_;
  }

  /** initialize the buffer maintained by this class. Can safely be called multiple times.
   *  @param owner current "owner" (DcmElement instance using this buffer)
   *  @param fieldLength number of bytes of user data in DICOM element
   *  @param bytesTransferred number of bytes of user data in DICOM element that have already been transferred
   *  @param byteOrder byteOrder desired byte order in buffer
   */
  void init(void *owner, Uint32 fieldLength, Uint32 bytesTransferred, E_ByteOrder byteOrder);

  /** check whether the buffer is currently empty
   *  @return true if buffer is empty, false otherwise
   */
  OFBool bufferIsEmpty() const { return (numBytes_ == 0); }

  /** return the number of bytes of user data currently in buffer
   *  @return number of bytes of user data currently in buffer
   */
  Uint32 contentLength() const { return numBytes_; }

  /** fill buffer from given DICOM element if buffer is currently empty.
   *  This method uses DcmElement::getPartialValue to fill the buffer from the given
   *  DICOM element at the given offset (which is updated to reflect the number of bytes
   *  read into the buffer).
   *  @param elem DICOM element to read from
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition fillBuffer(DcmElement& elem);

  /** write buffer content to output stream
   *  @param outStream output stream to write to
   *  @return number of bytes written
   */
  Uint32 writeBuffer(DcmOutputStream &outStream);

private:

  /// private undefined copy constructor
  DcmWriteCache(const DcmWriteCache& arg);

  /// private undefined copy assignment operator
  DcmWriteCache& operator=(const DcmWriteCache& arg);

  /// file cache object
  DcmFileCache fcache_;

  /// write buffer
  Uint8 *buf_;

  /// current "owner" (DcmElement instance using this buffer)
  void *owner_;

  /// offset within buffer to first byte
  Uint32 offset_;

  /// number of user data bytes currently in buffer
  Uint32 numBytes_;

  /// buffer size in bytes
  Uint32 capacity_;

  /// length of the current DICOM element, in bytes
  Uint32 fieldLength_;

  /// offset within the current DICOM element, in bytes
  Uint32 fieldOffset_;

  /// current output byte order
  E_ByteOrder byteOrder_;
};

#endif
