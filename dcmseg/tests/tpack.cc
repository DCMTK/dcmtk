/*
 *
 *  Copyright (C) 2015, Open Connections GmbH
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
 *  Module:  dcmseg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Test code of binary segmentation byte packing/unpacking
 *
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmseg/segutils.h"

#include "dcmtk/ofstd/oftest.h"

#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstd.h"
#include <bitset>


OFTEST(dcmseg_BinarySegmentationPackaging)
{
  Uint8 bulk[500];
  unsigned int now = OFstatic_cast(unsigned int, time(NULL));
  // Create some random bulk data with ones and zeroes
  for (size_t n = 0; n < 500; n++)
  {
    bulk[n] = OFrand_r(now) % 3;
  }

  // Pack the random data as a binary segmentation frame
  DcmIODTypes::Frame* packed = DcmSegUtils::packBinaryFrame(bulk, 10, 50);

  // Unpack the binary segmentation frame to unpacked format
  DcmIODTypes::Frame *unpacked = DcmSegUtils::unpackBinaryFrame(packed, 10, 50);

  // Re-pack the binary segmentation frame to packed format
  DcmIODTypes::Frame* repacked = DcmSegUtils::packBinaryFrame(unpacked->pixData, 10, 50);

  // Compare packed and re-packed format (round-trip)
  if ( memcmp(packed->pixData, repacked->pixData, DcmSegUtils::getBytesForBinaryFrame(500)) != 0)
  {
    OFCHECK_FAIL("Error: Unpacking and repacking binary segmentation data corrupts values");
  }

  // Delete data
  delete packed;
  delete unpacked;
  delete repacked;
}
