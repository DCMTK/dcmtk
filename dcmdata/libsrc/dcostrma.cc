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
 *  Purpose: base classes for output streams
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-08-27 16:55:52 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcostrma.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "dcostrma.h"
#include "dcostrmz.h" /* for DcmZLibOutputFilter */
#include "dcerror.h"  /* for EC_IllegalCall */

DcmOutputStream::DcmOutputStream(DcmConsumer *initial)
: current_(initial)
, compressionFilter_(NULL)
, tell_(0)
{
}

DcmOutputStream::~DcmOutputStream()
{
  // we cannot access the stream anymore at this point because the
  // consumer has most probably already been deleted.
  delete compressionFilter_;
}

OFCondition DcmOutputStream::installCompressionFilter(E_StreamCompression filterType)
{
  OFCondition result = EC_Normal;
  if (compressionFilter_) result = EC_DoubleCompressionFilters;
  else
  {
    switch (filterType)
    {
#ifdef WITH_ZLIB
      case ESC_zlib:
        compressionFilter_ = new DcmZLibOutputFilter();
        if (compressionFilter_) 
        {
          compressionFilter_->append(*current_);
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

OFBool DcmOutputStream::good() const
{
  return current_->good();
}

OFCondition DcmOutputStream::status() const
{
  return current_->status();
}

OFBool DcmOutputStream::isFlushed() const
{
  return current_->isFlushed();
}

Uint32 DcmOutputStream::avail() const
{
  return current_->avail();
}

Uint32 DcmOutputStream::write(const void *buf, Uint32 buflen)
{
  Uint32 result = current_->write(buf, buflen);
  tell_ += result;
  return result;
}

void DcmOutputStream::flush()
{
  current_->flush();
}

Uint32 DcmOutputStream::tell() const
{
  return tell_;
}


/*
 * CVS/RCS Log:
 * $Log: dcostrma.cc,v $
 * Revision 1.1  2002-08-27 16:55:52  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 *
 */
