/*
 *
 *  Copyright (C) 2002-2021, OFFIS e.V.
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
 *  Purpose: DcmStdinStream and related classes,
 *    implements streamed input from stdin.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcistrms.h"
#include "dcmtk/dcmdata/dcerror.h"

BEGIN_EXTERN_C
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
END_EXTERN_C

// size of the buffer we use to read from stdin
#define DCMSTDINSTREAMBUFSIZE 32768

DcmStdinStream::DcmStdinStream()
: DcmInputStream(&producer_) // safe because DcmInputStream only stores pointer
, producer_()
, buf_(new unsigned char[DCMSTDINSTREAMBUFSIZE])
{
#ifdef _WIN32
    // Set "stdin" to binary mode
    int result = setmode(fileno(stdin), O_BINARY);
    if (result == -1) DCMDATA_ERROR("Failed to switch stdin to binary mode");
#endif
}

DcmStdinStream::~DcmStdinStream()
{
  delete[] buf_;
}

DcmInputStreamFactory *DcmStdinStream::newFactory() const
{
  // we don't support delayed loading from stdin streams
  return NULL;
}

void DcmStdinStream::fillBuffer()
{
  // if there are a few bytes left in the previous buffer, make the
  // buffer producer make a backup copy
  producer_.releaseBuffer();

  // read the next block from stdin
  size_t numBytes = fread(buf_, 1, DCMSTDINSTREAMBUFSIZE, stdin);

  // make the buffer available to the buffer producer
  producer_.setBuffer(buf_, OFstatic_cast(offile_off_t, numBytes));

  // check if we are at the end of stream, and if so, notify the buffer producer
  if (feof(stdin)) producer_.setEos();

  // if there is a compression filter, the following call will
  // cause it to feed the compression engine with data from the
  // new buffer.
  skip(0);
}

