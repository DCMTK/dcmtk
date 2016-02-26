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
   *   @return The frame data if successful, NULL if an error occurs
   */
  static DcmIODTypes::Frame* packBinaryFrame(Uint8* pixelData,
                                             const Uint16 rows,
                                             const Uint16 columns);

  /** Compute the number of bytes required for a binary pixel data frame,
   *  given the number of pixels
   *  @param  numPixels The total number of pixels
   *  @return The number of bytes required to pack the data into a binary
   *          segmentation frame
   */
  static size_t getBytesForBinaryFrame(const size_t& numPixels);

  /** Unpacks a binary segmentation frame into a "sparse" pixel data frame where
   *  every resulting byte represents a single bit of the frame being either
   *  0 (not set) or 1 (set).
   *  @param  frame The input buffer with the frame in packed format
   *  @param  rows The rows of the frame
   *  @param  cols The cols of the frame
   *  @return The segmentation frame in unpacked format. NULL in case of error.
   */
  static DcmIODTypes::Frame* unpackBinaryFrame(const DcmIODTypes::Frame* frame,
                                               Uint16 rows,
                                               Uint16 cols);

  /** Shifts the given memory block some defined number of bits to the right
   *  Bits that fall off the last byte are lost (i.e. no "cycled" shift).
   *  @param  buf The address of the memory buffer to shift
   *  @param  bufLen The length of the buf memory block in bytes
   *  @param  numBits The number of bits to shift. Must be 0 <= numBits <= 7.
   */
  static void shiftRight(Uint8* buf,
                         size_t bufLen,
                         Uint8 numBits);

  /** Shifts the given memory block some defined number of bits to the left.
   *  Bits that fall off the first byte are lost (i.e. no "cycled" shift).
   *  @param  buf The address of the memory buffer to shift
   *  @param  bufLen The length of the buf memory block in bytes
   *  @param  numBits The number of bits to shift. Must be 0 <= numBits <= 7.
   */
  static void shiftLeft(Uint8* buf,
                        size_t bufLen,
                        Uint8 numBits);

  /** Dumps a byte as binary number to a string. Only useful for
   *  debugging purposes.
   *  @param  b The byte to dump
   *  @return A string containing b as a binary number
   */
  static OFString debugByte2Bin(Uint8 b);

  /** Dumps a memory block byte for byte to the debug log stream. Only useful
   * for debugging purposes.
   *  @param  buffer The address of the memory block to dump
   *  @param  length The length of memory to be dumped
   *  @param  what String describing what is dumped.
   */
  static void debugDumpBin(Uint8* buffer,
                           size_t length,
                           const char* what);
};

#endif // SEGUTILS_H
