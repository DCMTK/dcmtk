/*
 *
 *  Copyright (C) 2002-2009, OFFIS
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
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-02-22 11:39:54 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcostrmb.h"
#include "dcmtk/dcmdata/dcerror.h"


DcmBufferConsumer::DcmBufferConsumer(void *buf, offile_off_t bufLen)
: DcmConsumer()
, buffer_(OFstatic_cast(unsigned char *, buf))
, bufSize_(bufLen)
, filled_(0)
, status_(EC_Normal)
{
  if ((buffer_ == NULL) || (bufSize_ == 0)) status_ = EC_IllegalCall;
}

DcmBufferConsumer::~DcmBufferConsumer()
{
}

OFBool DcmBufferConsumer::good() const
{
  return status_.good();
}

OFCondition DcmBufferConsumer::status() const
{
  return status_;
}

OFBool DcmBufferConsumer::isFlushed() const
{
  return (filled_ == 0);
}

offile_off_t DcmBufferConsumer::avail() const
{
  return bufSize_ - filled_;
}

offile_off_t DcmBufferConsumer::write(const void *buf, offile_off_t buflen)
{
  offile_off_t result = 0;
  if (status_.good() && buf && buflen)
  {
    result = bufSize_ - filled_;
    if (result > buflen) result = buflen;
    memcpy(buffer_+ filled_, buf, OFstatic_cast(size_t, result));
    filled_ += result;
  }
  return result;
}

void DcmBufferConsumer::flush()
{
  // nothing to flush
}

void DcmBufferConsumer::flushBuffer(void *& buffer, offile_off_t& length)
{
  buffer = buffer_;
  length = filled_;
  filled_ = 0;
}

/* ======================================================================= */

DcmOutputBufferStream::DcmOutputBufferStream(void *buf, offile_off_t bufLen)
: DcmOutputStream(&consumer_) // safe because DcmOutputStream only stores pointer
, consumer_(buf, bufLen)
{
}

DcmOutputBufferStream::~DcmOutputBufferStream()
{
#ifdef DEBUG
  if (! isFlushed())
  {
    DCMDATA_WARN("DcmOutputBufferStream: Closing unflushed DcmOutputBufferStream, loss of data!");
  }
#endif
}

void DcmOutputBufferStream::flushBuffer(void *& buffer, offile_off_t& length)
{
  consumer_.flushBuffer(buffer, length);
}


/*
 * CVS/RCS Log:
 * $Log: dcostrmb.cc,v $
 * Revision 1.9  2010-02-22 11:39:54  uli
 * Remove some unneeded includes.
 *
 * Revision 1.8  2009-12-04 17:10:00  joergr
 * Slightly modified some log messages.
 *
 * Revision 1.7  2009-11-04 09:58:10  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.6  2007-02-19 16:06:10  meichel
 * Class DcmOutputStream and related classes are now safe for use with
 *   large files (2 GBytes or more) if supported by compiler and operating system.
 *
 * Revision 1.5  2006/08/15 15:49:54  meichel
 * Updated all code in module dcmdata to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.4  2005/12/08 15:41:21  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.3  2004/02/04 16:36:47  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.2  2002/09/19 08:32:28  joergr
 * Added explicit type casts to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.1  2002/08/27 16:55:53  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 *
 */
