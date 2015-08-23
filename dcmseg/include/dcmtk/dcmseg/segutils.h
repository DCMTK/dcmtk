/*
 *
 *  Copyright (C) 2015, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
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
 *  Purpose: Helper functions for the dcmseg module
 *
 */

#ifndef SEGUTILS_H
#define SEGUTILS_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmseg/segdef.h"
#include "dcmtk/dcmseg/segtypes.h"

/** Class that contains helper functionality for the dcmseg module
 */
class DCMTK_DCMSEG_EXPORT DcmSegUtils
{

public:

  /** Pack the given segmentation pixel data, provided "unpacked", into
   *  the packed format expected by DICOM
   *   @param  pixelData Pixel data in unpacked format
   *   @param  rows Number of rows in the pixel data
   *   @param  columns The number of columns in the pixel data
   *   @return The frame data if successfull, NULL if an error occurs
   */
  static DcmIODTypes::Frame* packBinaryFrame(Uint8* pixelData,
                                             const Uint16 rows,
                                             const Uint16 columns);

  /** Compute the number of bytes requried for a binary pixel data frame,
   *  given the number of pixels
   *  @param  numPixels The total number of pixels
   *  @return The number of bytes required to pack the data into a binary
   *          segmentation frame
   */
  static size_t getBytesForBinaryFrame(const size_t& numPixels);

  /** Unpacks a binary segmentation frame into a "sparse" pixel data frame
   *  @param  frame The input frame in packed format
   *  @param  rows  The number of rows in the pixel data
   *  @param  cols  the number of cols in the pixel data
   *  @return The segmentation frame in unpacked format
   */
  static DcmIODTypes::Frame* unpackBinaryFrame(const DcmIODTypes::Frame* frame,
                                               const Uint16 rows,
                                               const Uint16 cols);

};

#endif // SEGUTILS_H
