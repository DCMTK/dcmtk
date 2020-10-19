/*
 *
 *  Copyright (C) 1994-2020, OFFIS e.V.
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

#ifndef DCISTRMS_H
#define DCISTRMS_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcistrma.h"
#include "dcmtk/dcmdata/dcistrmb.h"

/** input stream that reads from standard input
 */
class DCMTK_DCMDATA_EXPORT DcmStdinStream: public DcmInputStream
{
public:
  /// constructor
  DcmStdinStream();

  /// destructor
  virtual ~DcmStdinStream();

  /** creates a new factory object for the current stream
   *  and stream position.  When activated, the factory will be
   *  able to create new DcmInputStream delivering the same
   *  data as the current stream.  Used to defer loading of
   *  value fields until accessed.
   *  If no factory object can be created (e.g. because the
   *  stream is not seekable), returns NULL.
   *  @return pointer to new factory object if successful, NULL otherwise.
   */
  virtual DcmInputStreamFactory *newFactory() const;

  /** fill the read buffer from stdin. After a call to this method,
   *  the read()-method for the DcmDataset or DcmFileFormat should
   *  be called, and the sequence repeated as long as the read()
   *  method returns EC_StreamNotifyClient.
   */
  virtual void fillBuffer();

private:

  /// private unimplemented copy constructor
  DcmStdinStream(const DcmStdinStream&);

  /// private unimplemented copy assignment operator
  DcmStdinStream& operator=(const DcmStdinStream&);

  /// the final producer of the filter chain
  DcmBufferProducer producer_;

  /// a buffer
  unsigned char *buf_;
};

#endif
