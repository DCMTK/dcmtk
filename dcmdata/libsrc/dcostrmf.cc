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
 *  Purpose: DcmOutputFileStream and related classes,
 *    implements streamed output to files.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:08 $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcostrmf.h"
#include "dcmtk/dcmdata/dcerror.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CERRNO
#include "dcmtk/ofstd/ofstdinc.h"


DcmFileConsumer::DcmFileConsumer(const char *filename)
: DcmConsumer()
, file_()
, status_(EC_Normal)
{

  if (!file_.fopen(filename, "wb"))
  {
    char buf[256];
    const char *text = OFStandard::strerror(errno, buf, sizeof(buf));
    if (text == NULL) text = "(unknown error code)";
    status_ = makeOFCondition(OFM_dcmdata, 19, OF_error, text);
  }
}

DcmFileConsumer::DcmFileConsumer(FILE *file)
: DcmConsumer()
, file_(file)
, status_(EC_Normal)
{
}

DcmFileConsumer::~DcmFileConsumer()
{
  file_.fclose();
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

offile_off_t DcmFileConsumer::avail() const
{
  // since we cannot report "unlimited", let's claim that we can still write 2GB.
  // Note that offile_off_t is a signed type.
  return 2147483647L;
}

offile_off_t DcmFileConsumer::write(const void *buf, offile_off_t buflen)
{
  offile_off_t result = 0;
  if (status_.good() && file_.open() && buf && buflen)
  {
#ifdef WRITE_VERY_LARGE_CHUNKS
    /* This is the old behaviour prior to DCMTK 3.5.5 */
    result = OFstatic_cast(offile_off_t, file_.fwrite(buf, 1, OFstatic_cast(size_t, buflen)));
#else
    /* On Windows (at least for some versions of MSVC), calls to fwrite() for more than
     * 67,076,095 bytes (a bit less than 64 MByte) fail if we're writing to a network
     * share. See MSDN KB899149. As a workaround, we always write in chunks of
     * 32M which should hardly negatively affect performance.
     */
#define DcmFileConsumer_MAX_CHUNK_SIZE 33554432 /* 32 MByte */
    offile_off_t written;
    const char *buf2 = OFstatic_cast(const char *, buf);
    while (buflen > DcmFileConsumer_MAX_CHUNK_SIZE)
    {
      written = OFstatic_cast(offile_off_t, file_.fwrite(buf2, 1, DcmFileConsumer_MAX_CHUNK_SIZE));
      result += written;
      buf2 += written;

      // if we have not written a complete chunk, there is problem; bail out
      if (written == DcmFileConsumer_MAX_CHUNK_SIZE) buflen -= DcmFileConsumer_MAX_CHUNK_SIZE; else buflen = 0;
    }

    // last call to fwrite if the file size is not a multiple of DcmFileConsumer_MAX_CHUNK_SIZE
    if (buflen)
    {
      written = OFstatic_cast(offile_off_t, file_.fwrite(buf2, 1, OFstatic_cast(size_t, buflen)));
      result += written;
    }
#endif
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

DcmOutputFileStream::DcmOutputFileStream(FILE *file)
: DcmOutputStream(&consumer_) // safe because DcmOutputStream only stores pointer
, consumer_(file)
{
}

DcmOutputFileStream::~DcmOutputFileStream()
{
  // last attempt to flush stream before file is closed
  flush();
#ifdef DEBUG
  if (! isFlushed())
  {
    DCMDATA_WARN("closing unflushed DcmOutputFileStream, loss of data!");
  }
#endif
}


/*
 * CVS/RCS Log:
 * $Log: dcostrmf.cc,v $
 * Revision 1.14  2010-10-14 13:14:08  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.13  2010-06-03 10:28:41  joergr
 * Replaced calls to strerror() by new helper function OFStandard::strerror()
 * which results in using the thread safe version of strerror() if available.
 *
 * Revision 1.12  2010-02-22 11:39:54  uli
 * Remove some unneeded includes.
 *
 * Revision 1.11  2009-11-04 09:58:10  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.10  2007-02-23 16:30:12  meichel
 * Fixed bug in DcmFileConsumer::avail introduced when converting
 *   the return type to offile_off_t, which is signed.
 *
 * Revision 1.9  2007/02/19 16:06:10  meichel
 * Class DcmOutputStream and related classes are now safe for use with
 *   large files (2 GBytes or more) if supported by compiler and operating system.
 *
 * Revision 1.8  2007/02/19 15:35:55  meichel
 * When writing DICOM data to file, we now by default split fwrite() calls for
 *   very large attributes into multiple calls, none of which writes more than
 *   32 MBytes. This is a workaround to a bug in most MSVC environments (MSDN
 *   KB899149) and is hardly relevant performance-wise. Previous behaviour can
 *   be enforced by compiling with WRITE_VERY_LARGE_CHUNKS defined.
 *
 * Revision 1.7  2006/08/15 15:49:54  meichel
 * Updated all code in module dcmdata to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.6  2005/12/08 15:41:22  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.5  2004/02/04 16:36:47  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.4  2003/11/07 13:49:09  meichel
 * Added constructor taking an open FILE* instead of a file name string
 *
 * Revision 1.3  2002/11/27 12:06:50  meichel
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
