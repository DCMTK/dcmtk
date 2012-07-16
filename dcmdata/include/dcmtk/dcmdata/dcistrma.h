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
 *  Purpose: base classes for input streams
 *
 */

#ifndef DCISTRMA_H
#define DCISTRMA_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"  /* for OFBool */
#include "dcmtk/ofstd/ofcond.h"   /* for OFCondition */
#include "dcmtk/ofstd/offile.h"   /* for offile_off_t */
#include "dcmtk/dcmdata/dcxfer.h"   /* for E_StreamCompression */

class DcmInputStream;

/** pure virtual abstract base class for producers, i.e. the initial node 
 *  of a filter chain in an input stream.
 */
class DCMTK_DCMDATA_EXPORT DcmProducer
{
public:

  /// destructor
  virtual ~DcmProducer()
  {
  }

  /** returns the status of the producer. Unless the status is good,
   *  the producer will not permit any operation.
   *  @return status, true if good
   */
  virtual OFBool good() const = 0;

  /** returns the status of the producer as an OFCondition object.
   *  Unless the status is good, the producer will not permit any operation.
   *  @return status, EC_Normal if good
   */
  virtual OFCondition status() const = 0;

  /** returns true if the producer is at the end of stream.
   *  @return true if end of stream, false otherwise
   */
  virtual OFBool eos() = 0;

  /** returns the minimum number of bytes that can be read with the
   *  next call to read(). The DcmObject read methods rely on avail
   *  to return a value > 0 if there is no I/O suspension since certain
   *  data such as tag and length are only read "en bloc", i.e. all
   *  or nothing.
   *  @return minimum of data available in producer
   */
  virtual offile_off_t avail() = 0;

  /** reads as many bytes as possible into the given block.
   *  @param buf pointer to memory block, must not be NULL
   *  @param buflen length of memory block
   *  @return number of bytes actually read. 
   */
  virtual offile_off_t read(void *buf, offile_off_t buflen) = 0;

  /** skips over the given number of bytes (or less)
   *  @param skiplen number of bytes to skip
   *  @return number of bytes actually skipped. 
   */
  virtual offile_off_t skip(offile_off_t skiplen) = 0;

  /** resets the stream to the position by the given number of bytes.
   *  @param num number of bytes to putback. If the putback operation
   *    fails, the producer status becomes bad. 
   */
  virtual void putback(offile_off_t num) = 0;

};


/** pure virtual abstract base class for input filters, i.e. 
 *  intermediate nodes of a filter chain in an input stream.
 */
class DCMTK_DCMDATA_EXPORT DcmInputFilter: public DcmProducer
{
public:

  /// destructor
  virtual ~DcmInputFilter()
  {
  }

  /** determines the producer from which the filter is supposed
   *  to read it's input. Once a producer for the input filter has 
   *  been defined, it cannot be changed anymore during the lifetime
   *  of the object.
   *  @param producer reference to producer, must not be circular chain
   */
  virtual void append(DcmProducer& producer) = 0;
};


/** pure virtual abstract base class for input stream factories,
 *  i.e. objects that can create a new input stream
 */
class DCMTK_DCMDATA_EXPORT DcmInputStreamFactory
{
public:

  /// destructor
  virtual ~DcmInputStreamFactory()
  {
  }

  /** create a new input stream object
   *  @return pointer to new input stream object
   */
  virtual DcmInputStream *create() const = 0;

  /** returns a pointer to a copy of this object
   */
  virtual DcmInputStreamFactory *clone() const = 0;
};


/** abstract base class for input streams.
 */
class DCMTK_DCMDATA_EXPORT DcmInputStream
{
public:

  /// destructor
  virtual ~DcmInputStream();

  /** returns the status of the stream. Unless the status is good,
   *  the stream will not permit any operation.
   *  @return status, true if good
   */
  virtual OFBool good() const;

  /** returns the status of the stream as an OFCondition object.
   *  Unless the status is good, the stream will not permit any operation.
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

  /** returns the total number of bytes read from the stream so far
   *  @return total number of bytes read from the stream
   */
  virtual offile_off_t tell() const;

  /** installs a compression filter for the given stream compression type,
   *  which should be neither ESC_none nor ESC_unsupported. Once a compression
   *  filter is active, it cannot be deactivated or replaced during the
   *  lifetime of the stream.
   *  @param filterType type of compression filter
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition installCompressionFilter(E_StreamCompression filterType);

  /** creates a new factory object for the current stream
   *  and stream position.  When activated, the factory will be
   *  able to create new DcmInputStream delivering the same
   *  data as the current stream.  Used to defer loading of
   *  value fields until accessed.
   *  If no factory object can be created (e.g. because the
   *  stream is not seekable), returns NULL.
   *  @return pointer to new factory object if successful, NULL otherwise.
   */
  virtual DcmInputStreamFactory *newFactory() const = 0;

  /** marks the current stream position for a later putback operation,
   *  overwriting a possibly existing prior putback mark.
   *  The DcmObject read methods rely on the possibility to putback
   *  up to 132 bytes for transfer syntax detection, parse error recovery etc.
   *  Implementations of this class should guarantee a putback capability
   *  of at least 1 kbyte.
   */
  virtual void mark();

  /** resets the stream to the position previously marked with
   *  setPutbackMark(). If the putback operation fails (no putback mark 
   *  set or putback buffer exceeded), status of the producer switches to bad.
   */
  virtual void putback();

protected:

  /** protected constructor, to be called from derived class constructor
   *  @param initial initial pointer to first node in filter chain
   *     The pointer is not dereferenced in the constructor, so the
   *     object pointed to may be initialized later in the subclass
   *     constructor.
   */
  DcmInputStream(DcmProducer *initial);

  /** returns pointer to current producer object
   */
  const DcmProducer *currentProducer() const;

private:

  /// private unimplemented copy constructor
  DcmInputStream(const DcmInputStream&);

  /// private unimplemented copy assignment operator
  DcmInputStream& operator=(const DcmInputStream&);

  /// pointer to first node in filter chain
  DcmProducer *current_;

  /// pointer to compression filter, NULL if no compression
  DcmInputFilter *compressionFilter_;

  /// counter for number of bytes read so far
  offile_off_t tell_;

  /// putback marker
  offile_off_t mark_;
};



#endif
