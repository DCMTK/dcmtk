/*
 *
 *  Copyright (C) 2002-2023, OFFIS e.V.
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
 *  Author:  Jasper den Otter, Marco Eichelberg
 *
 *  Purpose: DcmStdoutStream and related classes,
 *    implements streamed output to stdout.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcostrms.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/ofstd/ofconsol.h"

BEGIN_EXTERN_C
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
END_EXTERN_C


DcmStdoutConsumer::DcmStdoutConsumer(const OFFilename & /* filename */)
: DcmConsumer()
, file_()
, status_(EC_Normal)
{
#ifdef _WIN32
    // Set "stdout" to binary mode
    int result = setmode(fileno(stdout), O_BINARY);
    if (result == -1) DCMDATA_ERROR("Failed to switch stdout to binary mode");
#endif
}

DcmStdoutConsumer::DcmStdoutConsumer(FILE *file)
: DcmConsumer()
, file_(file)
, status_(EC_Normal)
{
}

DcmStdoutConsumer::~DcmStdoutConsumer()
{
}

OFBool DcmStdoutConsumer::good() const
{
  return status_.good();
}

OFCondition DcmStdoutConsumer::status() const
{
  return status_;
}

OFBool DcmStdoutConsumer::isFlushed() const
{
  return OFTrue;
}

offile_off_t DcmStdoutConsumer::avail() const
{
  // since we cannot report "unlimited", let's claim that we can still write 2GB.
  // Note that offile_off_t is a signed type.
  return 2147483647L;
}

offile_off_t DcmStdoutConsumer::write(const void *buf, offile_off_t buflen)
{
  if (buflen && buf)
    return OFstatic_cast(offile_off_t, fwrite(buf, 1, OFstatic_cast(size_t, buflen), stdout));
    else return 0;
}

void DcmStdoutConsumer::flush()
{
  fflush(stdout);
}

/* ======================================================================= */

DcmStdoutStream::DcmStdoutStream(const OFFilename &filename)
: DcmOutputStream(&consumer_) // safe because DcmStdoutStream only stores pointer
, consumer_(filename)
{
}

DcmStdoutStream::DcmStdoutStream(FILE *file)
: DcmOutputStream(&consumer_) // safe because DcmStdoutStream only stores pointer
, consumer_(file)
{
}

DcmStdoutStream::~DcmStdoutStream()
{
  // last attempt to flush stream before file is closed
  flush();
#ifdef DEBUG
  if (! isFlushed())
  {
    DCMDATA_WARN("closing unflushed DcmStdoutStream, loss of data!");
  }
#endif
}
