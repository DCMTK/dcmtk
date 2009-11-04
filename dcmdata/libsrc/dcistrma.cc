/*
 *
 *  Copyright (C) 1994-2007, OFFIS
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
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2009-11-04 09:58:09 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcistrma.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcistrma.h"
#include "dcmtk/dcmdata/dcistrmz.h" /* for DcmZLibInputFilter */
#include "dcmtk/dcmdata/dcerror.h"  /* for EC_IllegalCall */

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

OFBool DcmInputStream::eos() 
{
  return current_->eos();
}

offile_off_t DcmInputStream::avail() 
{
  return current_->avail();
}

offile_off_t DcmInputStream::read(void *buf, offile_off_t buflen)
{
  offile_off_t result = current_->read(buf, buflen);
  tell_ += result;
  return result;
}

offile_off_t DcmInputStream::skip(offile_off_t skiplen)
{
  offile_off_t result = current_->skip(skiplen);
  tell_ += result;
  return result;
}

offile_off_t DcmInputStream::tell() const
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
 * Revision 1.4  2009-11-04 09:58:09  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.3  2007-02-19 15:45:30  meichel
 * Class DcmInputStream and related classes are now safe for use with
 *   large files (2 GBytes or more) if supported by compiler and operating system.
 *
 * Revision 1.2  2005/12/08 15:41:12  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2002/08/27 16:55:48  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 *
 */
