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
 *  Purpose: DcmInputFileStream and related classes,
 *    implements streamed input from files.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-08-27 16:55:33 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcistrmf.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCISTRMF_H
#define DCISTRMF_H

#include "osconfig.h"
#include "dcistrma.h"

BEGIN_EXTERN_C
#include <stdio.h> /* for FILE */
END_EXTERN_C

/** producer class that reads data from a plain file.
 */
class DcmFileProducer: public DcmProducer
{
public:
  /** constructor
   *  @param filename name of file to be opened, must not be NULL or empty
   *  @param offset byte offset to skip from the start of file
   */
  DcmFileProducer(const char *filename, Uint32 offset = 0);

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
  virtual OFBool eos() const;

  /** returns the minimum number of bytes that can be read with the
   *  next call to read(). The DcmObject read methods rely on avail
   *  to return a value > 0 if there is no I/O suspension since certain
   *  data such as tag and length are only read "en bloc", i.e. all
   *  or nothing.
   *  @return minimum of data available in producer
   */
  virtual Uint32 avail() const;

  /** reads as many bytes as possible into the given block.
   *  @param buf pointer to memory block, must not be NULL
   *  @param buflen length of memory block
   *  @return number of bytes actually read. 
   */
  virtual Uint32 read(void *buf, Uint32 buflen);

  /** skips over the given number of bytes (or less)
   *  @param skiplen number of bytes to skip
   *  @return number of bytes actually skipped. 
   */
  virtual Uint32 skip(Uint32 skiplen);

  /** resets the stream to the position by the given number of bytes.
   *  @param num number of bytes to putback. If the putback operation
   *    fails, the producer status becomes bad. 
   */
  virtual void putback(Uint32 num);

private:

  /// private unimplemented copy constructor
  DcmFileProducer(const DcmFileProducer&);

  /// private unimplemented copy assignment operator
  DcmFileProducer& operator=(const DcmFileProducer&);

  /// the file we're actually reading from
  FILE *file_;

  /// status
  OFCondition status_;

  /// number of bytes in file
  Uint32 size_;
};


/** input stream factory for plain files
 */
class DcmInputFileStreamFactory: public DcmInputStreamFactory
{
public:

  /** constructor
   *  @param filename name of file to be opened, must not be NULL or empty
   *  @param offset byte offset to skip from the start of file
   */
  DcmInputFileStreamFactory(const char *filename, Uint32 offset);

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
  OFString filename_;

  /// offset in file
  Uint32 offset_;
  
};


/** input stream that reads from a plain file
 */
class DcmInputFileStream: public DcmInputStream
{
public:
  /** constructor
   *  @param filename name of file to be opened, must not be NULL or empty
   *  @param offset byte offset to skip from the start of file
   */
  DcmInputFileStream(const char *filename, Uint32 offset = 0);

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
  OFString filename_;
};


#endif

/*
 * CVS/RCS Log:
 * $Log: dcistrmf.h,v $
 * Revision 1.1  2002-08-27 16:55:33  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 *
 */
