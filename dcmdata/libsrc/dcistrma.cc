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
 *  Purpose: base classes for input streams
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-08-27 16:55:48 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcistrma.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "dcistrma.h"
#include "dcistrmz.h" /* for DcmZLibInputFilter */
#include "dcerror.h"  /* for EC_IllegalCall */

DcmInputStream::DcmInputStream(DcmProducer *initial)
: current_(initial)
, compressionFilter_(NULL)
, tell_(0)
, mark_(0)
{
}

DcmInputStream::~DcmInputStream()
{
  // we cannot access the stream anymore at this point because the
  // producer has most probably already been deleted.
  delete compressionFilter_;
}

OFBool DcmInputStream::good() const
{
  return current_->good();
}

OFCondition DcmInputStream::status() const
{
  return current_->status();
}

OFBool DcmInputStream::eos() const
{
  return current_->eos();
}

Uint32 DcmInputStream::avail() const
{
  return current_->avail();
}

Uint32 DcmInputStream::read(void *buf, Uint32 buflen)
{
  Uint32 result = current_->read(buf, buflen);
  tell_ += result;
  return result;
}

Uint32 DcmInputStream::skip(Uint32 skiplen)
{
  Uint32 result = current_->skip(skiplen);
  tell_ += result;
  return result;
}

Uint32 DcmInputStream::tell() const
{
  return tell_;
}

void DcmInputStream::mark()
{
  mark_ = tell_;
}

void DcmInputStream::putback()
{
  current_->putback(tell_ - mark_);
  tell_ = mark_;
}

const DcmProducer *DcmInputStream::currentProducer() const
{
  return current_;
}

OFCondition DcmInputStream::installCompressionFilter(E_StreamCompression filterType)
{
  OFCondition result = EC_Normal;
  if (compressionFilter_) result = EC_DoubleCompressionFilters;
  else
  {
    switch (filterType)
    {
#ifdef WITH_ZLIB
      case ESC_zlib:
        compressionFilter_ = new DcmZLibInputFilter();
        if (compressionFilter_) 
        {
          compressionFilter_->append(*current_);
          // feed the compression engine with data from the producer
          compressionFilter_->skip(0); 
          current_ = compressionFilter_;
        } else result = EC_MemoryExhausted;        
        break;
#endif
      case ESC_none:
      case ESC_unsupported:
        result = EC_UnsupportedEncoding; 
        break;
    }
  }

  return result;
}



/*
 * CVS/RCS Log:
 * $Log: dcistrma.cc,v $
 * Revision 1.1  2002-08-27 16:55:48  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 *
 */
