/*
 *
 *  Copyright (C) 2015-2024, Open Connections GmbH
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
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofvector.h"

/** Class that contains helper functionality for the dcmseg module
 */
class DCMTK_DCMSEG_EXPORT DcmSegUtils
{

public:
    /** Pack the given segmentation pixel data, provided "unpacked", into
     *  the packed format expected by DICOM
     *  @param  pixelData Pixel data in unpacked format, i.e on byte per pixel, either 0 (not set) or non-0 (set)
     *  @param  rows Number of rows in the pixel data
     *  @param  columns The number of columns in the pixel data
     *  @return The frame data if successful, NULL if an error occurs
     */
    static DcmIODTypes::Frame* packBinaryFrame(const Uint8* pixelData, const Uint16 rows, const Uint16 columns);

    /** Concatenate given frames into a single bit array
     *  @param  frames The frames to concatenate. Each frame is expected to be in packed format (1 bit per pixel),
     *          with the last byte padded with 0s if necessary
     *  @param  rows The number of rows in the frames
     *  @param  cols The number of columns in the frames
     *  @param  pixData The buffer to store the concatenated frames. Must be pre-allocated and have a size of at least
     *          pixDataLength bytes. Bytes are set to 0 before concatenation.
     *  @param  pixDataLength The length of the pixData buffer in bytes
     *  @return EC_Normal if successful, an error code otherwise
     */
    static OFCondition concatBinaryFrames(const OFVector<DcmIODTypes::Frame*>& frames, const Uint16 rows, const Uint16 cols, Uint8* pixData, const size_t pixDataLength);

    /** Unpacks a binary segmentation frame into a "sparse" pixel data frame where
     *  every resulting byte represents a single bit of the frame being either
     *  0 (not set) or 1 (set).
     *  @param  frame The input buffer with the frame in packed format
     *  @param  rows The rows of the frame
     *  @param  cols The cols of the frame
     *  @return The segmentation frame in unpacked format. NULL in case of error.
     */
    static DcmIODTypes::Frame* unpackBinaryFrame(const DcmIODTypes::Frame* frame, Uint16 rows, Uint16 cols);


    /** Dumps a byte as binary number to a string. Only useful for
     *  debugging purposes.
     *  @param  b The byte to dump
     *  @return A string containing b as a binary number
     */
    static OFString debugByte2Bin(Uint8 b);

    /** Dumps a memory block byte for byte to the debug log stream. Only useful
     *  for debugging purposes.
     *  @param  buffer The address of the memory block to dump
     *  @param  length The length of memory to be dumped
     *  @param  what String describing what is dumped
     *  @param  raw If OFTrue, no additional information is added to the output,
     *  otherwise bytes are enumerated
     */
    static void debugDumpBin(Uint8* buffer, size_t length, const OFString& what, const OFBool raw);
};

#endif // SEGUTILS_H
