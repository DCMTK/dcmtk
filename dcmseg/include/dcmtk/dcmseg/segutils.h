/*
 *
 *  Copyright (C) 2015-2018, Open Connections GmbH
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
  static DcmIODTypes::Frame* packBinaryFrame(const Uint8* pixelData,
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

  /** Aligns 1 bit per pixel frame data to make the frame start at a
   *  specific bit position within the first byte. This is used in the context
   *  that dcmseg holds the frames in memory aligned to exact byte positions,
   *  while the DICOM encoding might require a frame to start at an arbitrary
   *  bit position since all (1 bit per pixel) frames are directly concatenated
   *  one after another (i.e. if one frame does not occupy a number of bits
   *  dividable by 8, not all frames will be aligned at exact byte positions).
   *  Note that each byte is filled from the right, i.e. the first pixel will
   *  represented by the bit at the very right of the first byte, and the 9th
   *  pixel will be in the very right position of the following byte.
   *  This is not a regular bit shift operation since the bits from the previous
   *  frame are on the left of the byte, but must be aligned at the right. The
   *  current frame starts from the first bit, occupying the unused bits of
   *  the last frame and then continuing in the next byte at the first bit
   *  from the left.
   *  Example for two bit shift:
   *    Input buffer bytes: hgfedcba 87654321
   *    Result:   fedcba00 654321hg
   *    The 00 in the first byte must be handled by the caller (will
   *    contain the two bits of the previous frame).
   *    See also dcmseg/tests/tutils.cc for more examples.
   *  @param  buf The address of the memory buffer to shift
   *  @param  bufLen The length of the buf memory block in bytes
   *  @param  numBits The number of bits to shift. Must be 0 <= numBits <= 7.
   */
  static void alignFrameOnBitPosition(Uint8* buf,
                                      size_t bufLen,
                                      Uint8 numBits);

  /** Aligns 1 bit per pixel frame data starting at a given bit position in the
   *  provided buffer with the start of that buffer. This is used to create
   *  a frame structure where all the bytes (including the first one) only
   *  contain data from the frame at hand.
   *  Note that each byte is filled from the right, i.e. the first pixel will
   *  represented by the bit at the very right of the first byte, and the 9th
   *  pixel will be in the very right position of the following byte.
   *  Example:
   *    3 bytes input buffer: edcbaZYX mlkjihgf utsrqpon
   *    Result after aligning 3 bits: fghedcba ponmlkji 000utsrq
   *    The 000 are unused bits and therefore zeroed out in the last byte. Bits
   *    ZYX will be shifted out which is ok since it does not belong to the
   *    current frame. See also dcmseg/tests/tutils.cc for more examples.
   *  @param  buf The address of the memory buffer to shift
   *  @param  bufLen The length of the buf memory block in bytes
   *  @param  numBits The number of bits to shift. Must be 0 <= numBits <= 7.
   */
  static void alignFrameOnByteBoundary(Uint8* buf,
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
