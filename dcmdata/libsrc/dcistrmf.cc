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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-09-19 08:32:29 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcistrmf.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "dcistrmf.h"
#include "dcerror.h"

BEGIN_EXTERN_C
#include <stdio.h>
#include <errno.h>
END_EXTERN_C

DcmFileProducer::DcmFileProducer(const char *filename, Uint32 offset)
: DcmProducer()
, file_(NULL)
, status_(EC_Normal)
, size_(0)
{
  file_ = fopen(filename, "rb");
  if (file_)
  {
     // Get number of bytes in file
     fseek(file_, 0L, SEEK_END);
     size_ = (Uint32)ftell(file_);
     if (0 != fseek(file_, offset, SEEK_SET))
     {
       const char *text = strerror(errno);
       if (text == NULL) text = "(unknown error code)";
       status_ = makeOFCondition(OFM_dcmdata, 18, OF_error, text);
     }
  }
  else
  {
    const char *text = strerror(errno);
    if (text == NULL) text = "(unknown error code)";
    status_ = makeOFCondition(OFM_dcmdata, 18, OF_error, text);
  }
}

DcmFileProducer::~DcmFileProducer()
{
  if (file_) fclose(file_);
}

OFBool DcmFileProducer::good() const
{
  return status_.good();
}

OFCondition DcmFileProducer::status() const
{
  return status_;
}

OFBool DcmFileProducer::eos() const
{
  if (file_)
  {
    return (feof(file_) || (size_ == (Uint32)ftell(file_)));
  }
  else return OFTrue;
}

Uint32 DcmFileProducer::avail() const
{
  if (file_) return size_ - (Uint32)ftell(file_); else return 0;
}

Uint32 DcmFileProducer::read(void *buf, Uint32 buflen)
{
  Uint32 result = 0;
  if (status_.good() && file_ && buf && buflen)
  {
    result = (Uint32) fread(buf, 1, (size_t)buflen, file_);
  }
  return result;
}

Uint32 DcmFileProducer::skip(Uint32 skiplen)
{
  Uint32 result = 0;
  if (status_.good() && file_ && skiplen)
  {
    Uint32 pos = (Uint32)ftell(file_);
    result = (size_ - pos < skiplen) ? (size_ - pos) : skiplen;
    if (fseek(file_, result, SEEK_CUR))
    {
       const char *text = strerror(errno);
       if (text == NULL) text = "(unknown error code)";
       status_ = makeOFCondition(OFM_dcmdata, 18, OF_error, text);
    }
  }
  return result;
}

void DcmFileProducer::putback(Uint32 num)
{
  if (status_.good() && file_ && num)
  {
    Uint32 pos = (Uint32)ftell(file_);
    if (num <= pos)
    {
      if (fseek(file_, -Sint32(num), SEEK_CUR))
      {
         const char *text = strerror(errno);
         if (text == NULL) text = "(unknown error code)";
         status_ = makeOFCondition(OFM_dcmdata, 18, OF_error, text);
      }
    }
    else status_ = EC_PutbackFailed; // tried to putback before start of file
  }
}


/* ======================================================================= */

DcmInputFileStreamFactory::DcmInputFileStreamFactory(const char *filename, Uint32 offset)
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

DcmInputFileStream::DcmInputFileStream(const char *filename, Uint32 offset)
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


/*
 * CVS/RCS Log:
 * $Log: dcistrmf.cc,v $
 * Revision 1.2  2002-09-19 08:32:29  joergr
 * Added explicit type casts to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.1  2002/08/27 16:55:49  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 *
 */
