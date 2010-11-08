/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-08 09:49:03 $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcistrmf.h"
#include "dcmtk/dcmdata/dcerror.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CERRNO
#include "dcmtk/ofstd/ofstdinc.h"


DcmFileProducer::DcmFileProducer(const char *filename, offile_off_t offset)
: DcmProducer()
, file_()
, status_(EC_Normal)
, size_(0)
{
  if (file_.fopen(filename, "rb"))
  {
     // Get number of bytes in file
     file_.fseek(0L, SEEK_END);
     size_ =  file_.ftell();
     if (0 != file_.fseek(offset, SEEK_SET))
     {
       OFString s("(unknown error code)");
       file_.getLastErrorString(s);
       status_ = makeOFCondition(OFM_dcmdata, 18, OF_error, s.c_str());
     }
  }
  else
  {
    OFString s("(unknown error code)");
    file_.getLastErrorString(s);
    status_ = makeOFCondition(OFM_dcmdata, 18, OF_error, s.c_str());
  }
}

DcmFileProducer::~DcmFileProducer()
{
}

OFBool DcmFileProducer::good() const
{
  return status_.good();
}

OFCondition DcmFileProducer::status() const
{
  return status_;
}

OFBool DcmFileProducer::eos()
{
  if (file_.open())
  {
    return (file_.eof() || (size_ == file_.ftell()));
  }
  else return OFTrue;
}

offile_off_t DcmFileProducer::avail()
{
  if (file_.open()) return size_ - file_.ftell(); else return 0;
}

offile_off_t DcmFileProducer::read(void *buf, offile_off_t buflen)
{
  offile_off_t result = 0;
  if (status_.good() && file_.open() && buf && buflen)
  {
    result = file_.fread(buf, 1, OFstatic_cast(size_t, buflen));
  }
  return result;
}

offile_off_t DcmFileProducer::skip(offile_off_t skiplen)
{
  offile_off_t result = 0;
  if (status_.good() && file_.open() && skiplen)
  {
    offile_off_t pos = file_.ftell();
    result = (size_ - pos < skiplen) ? (size_ - pos) : skiplen;
    if (file_.fseek(result, SEEK_CUR))
    {
      OFString s("(unknown error code)");
      file_.getLastErrorString(s);
      status_ = makeOFCondition(OFM_dcmdata, 18, OF_error, s.c_str());
    }
  }
  return result;
}

void DcmFileProducer::putback(offile_off_t num)
{
  if (status_.good() && file_.open() && num)
  {
    offile_off_t pos = file_.ftell();
    if (num <= pos)
    {
      if (file_.fseek(-num, SEEK_CUR))
      {
        OFString s("(unknown error code)");
        file_.getLastErrorString(s);
        status_ = makeOFCondition(OFM_dcmdata, 18, OF_error, s.c_str());
      }
    }
    else status_ = EC_PutbackFailed; // tried to putback before start of file
  }
}


/* ======================================================================= */

DcmInputFileStreamFactory::DcmInputFileStreamFactory(const char *filename, offile_off_t offset)
: DcmInputStreamFactory()
, filename_()
, offset_(offset)
{
  if (filename) filename_ = filename;
}

DcmInputFileStreamFactory::DcmInputFileStreamFactory(const DcmInputFileStreamFactory& arg)
: DcmInputStreamFactory(arg)
, filename_(arg.filename_)
, offset_(arg.offset_)
{
}

DcmInputFileStreamFactory::~DcmInputFileStreamFactory()
{
}

DcmInputStream *DcmInputFileStreamFactory::create() const
{
  return new DcmInputFileStream(filename_.c_str(), offset_);
}

/* ======================================================================= */

DcmInputFileStream::DcmInputFileStream(const char *filename, offile_off_t offset)
: DcmInputStream(&producer_) // safe because DcmInputStream only stores pointer
, producer_(filename, offset)
, filename_()
{
  if (filename) filename_ = filename;
}

DcmInputFileStream::~DcmInputFileStream()
{
}

DcmInputStreamFactory *DcmInputFileStream::newFactory() const
{
  DcmInputStreamFactory *result = NULL;
  if (currentProducer() == &producer_)
  {
    // no filter installed, can create factory object
    result = new DcmInputFileStreamFactory(filename_.c_str(), tell());
  }
  return result;
}

/* ======================================================================= */

DcmInputStream *DcmTempFileHandler::create() const
{
    return new DcmInputFileStream(filename_.c_str(), 0);
}

DcmTempFileHandler::DcmTempFileHandler(const char *fname)
#ifdef WITH_THREADS
: refCount_(1), mutex_(), filename_(fname)
#else
: refCount_(1), filename_(fname)
#endif
{
}

DcmTempFileHandler::~DcmTempFileHandler()
{
    unlink(filename_.c_str());
}

DcmTempFileHandler *DcmTempFileHandler::newInstance(const char *fname)
{
    return new DcmTempFileHandler(fname);
}

void DcmTempFileHandler::increaseRefCount()
{
#ifdef WITH_THREADS
    mutex_.lock();
#endif
    ++refCount_;
#ifdef WITH_THREADS
    mutex_.unlock();
#endif
}

void DcmTempFileHandler::decreaseRefCount()
{
#ifdef WITH_THREADS
    mutex_.lock();
#endif
    size_t result = --refCount_;
#ifdef WITH_THREADS
    mutex_.unlock();
#endif
    if (result == 0) delete this;
}

/* ======================================================================= */

DcmInputTempFileStreamFactory::DcmInputTempFileStreamFactory(DcmTempFileHandler *handler)
: DcmInputStreamFactory()
, fileHandler_(handler)
{
    fileHandler_->increaseRefCount();
}

DcmInputTempFileStreamFactory::DcmInputTempFileStreamFactory(const DcmInputTempFileStreamFactory &arg)
: DcmInputStreamFactory(arg)
, fileHandler_(arg.fileHandler_)
{
    fileHandler_->increaseRefCount();
}

DcmInputTempFileStreamFactory::~DcmInputTempFileStreamFactory()
{
    fileHandler_->decreaseRefCount();
}

DcmInputStream *DcmInputTempFileStreamFactory::create() const
{
    return fileHandler_->create();
}

DcmInputStreamFactory *DcmInputTempFileStreamFactory::clone() const
{
    return new DcmInputTempFileStreamFactory(*this);
}

/*
 * CVS/RCS Log:
 * $Log: dcistrmf.cc,v $
 * Revision 1.10  2010-11-08 09:49:03  uli
 * Fixed even more gcc warnings caused by additional compiler flags.
 *
 * Revision 1.9  2010-10-14 13:14:08  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.8  2010-10-04 14:44:42  joergr
 * Replaced "#ifdef _REENTRANT" by "#ifdef WITH_THREADS" where appropriate (i.e.
 * in all cases where OFMutex, OFReadWriteLock, etc. are used).
 *
 * Revision 1.7  2008-05-29 10:39:41  meichel
 * Implemented new classes DcmTempFileHandler and DcmInputTempFileStreamFactory
 *   that perform thread-safe reference counted life cycle management of a
 *   temporary file and are needed for DcmElement temporary file extensions to come.
 *
 * Revision 1.6  2007/02/19 15:45:31  meichel
 * Class DcmInputStream and related classes are now safe for use with
 *   large files (2 GBytes or more) if supported by compiler and operating system.
 *
 * Revision 1.5  2005/12/08 15:41:14  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.4  2004/02/04 16:34:09  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.3  2002/11/27 12:06:48  meichel
 * Adapted module dcmdata to use of new header file ofstdinc.h
 *
 * Revision 1.2  2002/09/19 08:32:29  joergr
 * Added explicit type casts to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.1  2002/08/27 16:55:49  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 *
 */
