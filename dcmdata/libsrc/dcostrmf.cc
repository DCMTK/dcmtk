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
 *  Purpose: DcmOutputFileStream and related classes,
 *    implements streamed output to files.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-11-27 12:06:50 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcostrmf.cc,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "dcostrmf.h"
#include "ofconsol.h"
#include "dcerror.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CERRNO
#include "ofstdinc.h"


DcmFileConsumer::DcmFileConsumer(const char *filename)
: DcmConsumer()
, file_(NULL)
, status_(EC_Normal)
{
  file_ = fopen(filename, "wb");
  if (!file_)
  {
    const char *text = strerror(errno);
    if (text == NULL) text = "(unknown error code)";
    status_ = makeOFCondition(OFM_dcmdata, 19, OF_error, text);
  }
}

DcmFileConsumer::~DcmFileConsumer()
{
  if (file_) fclose(file_);
}

OFBool DcmFileConsumer::good() const
{
  return status_.good();
}

OFCondition DcmFileConsumer::status() const
{
  return status_;
}

OFBool DcmFileConsumer::isFlushed() const
{
  return OFTrue;
}

Uint32 DcmFileConsumer::avail() const
{
  return (Uint32) -1; // assume unlimited file size
}

Uint32 DcmFileConsumer::write(const void *buf, Uint32 buflen)
{
  Uint32 result = 0;
  if (status_.good() && file_ && buf && buflen)
  {
    result = (Uint32) fwrite(buf, 1, (size_t)buflen, file_);
  }
  return result;
}

void DcmFileConsumer::flush()
{
  // nothing to flush
}

/* ======================================================================= */

DcmOutputFileStream::DcmOutputFileStream(const char *filename)
: DcmOutputStream(&consumer_) // safe because DcmOutputStream only stores pointer
, consumer_(filename)
{
}

DcmOutputFileStream::~DcmOutputFileStream()
{
  // last attempt to flush stream before file is closed
  flush();
#ifdef DEBUG
  if (! isFlushed())
  {
    ofConsole.lockCerr() << "Warning: closing unflushed DcmOutputFileStream, loss of data!" << endl;
    ofConsole.unlockCerr();
  }
#endif
}


/*
 * CVS/RCS Log:
 * $Log: dcostrmf.cc,v $
 * Revision 1.3  2002-11-27 12:06:50  meichel
 * Adapted module dcmdata to use of new header file ofstdinc.h
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
