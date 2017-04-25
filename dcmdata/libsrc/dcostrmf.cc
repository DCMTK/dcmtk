/*
 *
 *  Copyright (C) 2002-2017, OFFIS e.V.
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
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcostrmf.h"
#include "dcmtk/dcmdata/dcerror.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"


DcmFileConsumer::DcmFileConsumer(const OFFilename &filename)
: DcmConsumer()
, file_()
, status_(EC_Normal)
{
  if (!file_.fopen(filename, "wb"))
  {
    OFString buffer = OFStandard::getLastSystemErrorCode().message();
    status_ = makeOFCondition(OFM_dcmdata, 19, OF_error, buffer.c_str());
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

DcmOutputFileStream::DcmOutputFileStream(const OFFilename &filename)
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
