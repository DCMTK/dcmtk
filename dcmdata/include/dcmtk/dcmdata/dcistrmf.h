/*
 *
 *  Copyright (C) 1994-2012, OFFIS e.V.
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
 *  Purpose: DcmInputFileStream and related classes,
 *    implements streamed input from files.
 *
 */

#ifndef DCISTRMF_H
#define DCISTRMF_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcistrma.h"


/** producer class that reads data from a plain file.
 */
class DCMTK_DCMDATA_EXPORT DcmFileProducer: public DcmProducer
{
public:
  /** constructor
   *  @param filename name of file to be opened (may contain wide chars
   *    if support enabled)
   *  @param offset byte offset to skip from the start of file
   */
  DcmFileProducer(const OFFilename &filename, offile_off_t offset = 0);

  /// destructor
  virtual ~DcmFileProducer();

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

private:

  /// private unimplemented copy constructor
  DcmFileProducer(const DcmFileProducer&);

  /// private unimplemented copy assignment operator
  DcmFileProducer& operator=(const DcmFileProducer&);

  /// the file we're actually reading from
  OFFile file_;

  /// status
  OFCondition status_;

  /// number of bytes in file
  offile_off_t size_;
};


/** input stream factory for plain files
 */
class DCMTK_DCMDATA_EXPORT DcmInputFileStreamFactory: public DcmInputStreamFactory
{
public:

  /** constructor
   *  @param filename name of file to be opened (may contain wide chars
   *    if support enabled)
   *  @param offset byte offset to skip from the start of file
   */
  DcmInputFileStreamFactory(const OFFilename &filename, offile_off_t offset);

  /// copy constructor
  DcmInputFileStreamFactory(const DcmInputFileStreamFactory &arg);

  /// destructor
  virtual ~DcmInputFileStreamFactory();

  /** create a new input stream object
   *  @return pointer to new input stream object
   */
  virtual DcmInputStream *create() const;

  /** returns a pointer to a copy of this object
   */
  virtual DcmInputStreamFactory *clone() const
  {
    return new DcmInputFileStreamFactory(*this);
  }

private:


  /// private unimplemented copy assignment operator
  DcmInputFileStreamFactory& operator=(const DcmInputFileStreamFactory&);

  /// filename
  OFFilename filename_;

  /// offset in file
  offile_off_t offset_;

};


/** input stream that reads from a plain file
 */
class DCMTK_DCMDATA_EXPORT DcmInputFileStream: public DcmInputStream
{
public:
  /** constructor
   *  @param filename name of file to be opened (may contain wide chars
   *    if support enabled)
   *  @param offset byte offset to skip from the start of file
   */
  DcmInputFileStream(const OFFilename &filename, offile_off_t offset = 0);

  /// destructor
  virtual ~DcmInputFileStream();

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

private:

  /// private unimplemented copy constructor
  DcmInputFileStream(const DcmInputFileStream&);

  /// private unimplemented copy assignment operator
  DcmInputFileStream& operator=(const DcmInputFileStream&);

  /// the final producer of the filter chain
  DcmFileProducer producer_;

  /// filename
  OFFilename filename_;
};

/** class that manages the life cycle of a temporary file.
 *  It maintains a thread-safe reference counter, and when this counter
 *  is decreased to zero, unlinks (deletes) the file and then the handler
 *  object itself.
 */
class DCMTK_DCMDATA_EXPORT DcmTempFileHandler
{
public:

  /** static method that permits creation of instances of
   *  this class (only) on the heap, never on the stack.
   *  A newly created instance always has a reference counter of 1.
   *  @param filename path to temporary file (may contain wide chars
   *    if support enabled)
   */
  static DcmTempFileHandler *newInstance(const OFFilename &filename);

  /** create an input stream that permits reading from the temporary file
   *  @return pointer to input stream. Note that there is no guarantee
   *    that the input stream actually permits reading, i.e. the presence of the
   *    temporary file is not checked.
   */
  DcmInputStream *create() const;

  /// increase reference counter for this object
  void increaseRefCount();

  /** decreases reference counter for this object and deletes
   *  the temporary file and this object if the reference counter becomes zero.
   */
  void decreaseRefCount();

private:

  /** private constructor.
   *  Instances of this class are always created through newInstance().
   *  @param filename path to temporary file (may contain wide chars
   *    if support enabled)
   */
  DcmTempFileHandler(const OFFilename &filename);

  /** private destructor. Instances of this class
   *  are always deleted through the reference counting methods
   */
  virtual ~DcmTempFileHandler();

  /// private undefined copy constructor
  DcmTempFileHandler(const DcmTempFileHandler& arg);

  /// private undefined copy assignment operator
  DcmTempFileHandler& operator=(const DcmTempFileHandler& arg);

  /** number of references to temporary file.
   *  Default initialized to 1 upon construction of this object
   */
  size_t refCount_;

#ifdef WITH_THREADS
  /// mutex for MT-safe reference counting
  OFMutex mutex_;
#endif

  /// path to temporary file
  OFFilename filename_;
};

/** input stream factory for temporary file handlers
 */
class DCMTK_DCMDATA_EXPORT DcmInputTempFileStreamFactory: public DcmInputStreamFactory
{
public:

  /** constructor
   *  @param handler pointer to temporary file handler.
   *    Reference counter of temporary file handler is increased by this operation.
   */
  DcmInputTempFileStreamFactory(DcmTempFileHandler *handler);

  /// copy constructor
  DcmInputTempFileStreamFactory(const DcmInputTempFileStreamFactory &arg);

  /// destructor, decreases reference counter of temporary file handler
  virtual ~DcmInputTempFileStreamFactory();

  /** create a new input stream object
   *  @return pointer to new input stream object
   */
  virtual DcmInputStream *create() const;

  /** returns a pointer to a copy of this object
   */
  virtual DcmInputStreamFactory *clone() const;

private:

  /// private unimplemented copy assignment operator
  DcmInputTempFileStreamFactory& operator=(const DcmInputTempFileStreamFactory&);

  /// handler for temporary file
  DcmTempFileHandler *fileHandler_;
};


#endif
