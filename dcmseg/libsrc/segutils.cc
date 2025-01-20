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

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmiod/iodtypes.h"
#include "dcmtk/dcmseg/segtypes.h"
#include "dcmtk/dcmseg/segutils.h"


DcmIODTypes::Frame* DcmSegUtils::packBinaryFrame(const Uint8* pixelData, const Uint16 rows, const Uint16 columns)
{
    // Sanity checking
    const size_t totalBits = OFstatic_cast(size_t, rows) * columns;
    if (totalBits == 0)
    {
        DCMSEG_ERROR("Unable to pack binary segmentation frame: Rows or Columns is 0");
        return NULL;
    }
    if (!pixelData)
    {
        DCMSEG_ERROR("Unable to pack binary segmentation frame: No pixel data provided");
        return NULL;
    }

    // Calculate total number of bytes required
    size_t totalBytes = (totalBits + 7) / 8; // +7 to round up to the nearest byte

    // Allocate memory for the packed bit array
    Uint8* packedData = new Uint8[totalBytes];
    if (packedData == NULL)
    {
        DCMSEG_ERROR("Cannot allocate memory for packed binary frame");
        return NULL;
    }
    memset(packedData, 0, totalBytes); // Initialize to 0

    // Pack the bits
    for (Uint32 i = 0; i < totalBits; ++i) {
        if (pixelData[i] != 0) {
            Uint32 byteIndex = i / 8;
            Uint32 bitIndex = i % 8;
            DCMSEG_TRACE("bitIndex: " << bitIndex << ", byteIndex: " << byteIndex << ", packedData[byteIndex]: " << DcmSegUtils::debugByte2Bin(packedData[byteIndex]));
            packedData[byteIndex] |= (1 << bitIndex); // Fill from right to left
        }
    }

    // Create and return the frame
    DcmIODTypes::Frame* frame = new DcmIODTypes::Frame();
    if (frame == NULL)
    {
        DCMSEG_ERROR("Cannot allocate memory for packed binary frame");
        delete[] packedData;
        return NULL;
    }
    frame->pixData = packedData;
    frame->length = totalBytes;
    return frame;
}

OFCondition DcmSegUtils::concatBinaryFrames(const OFVector<DcmIODTypes::Frame*>& frames,
                                            const Uint16 rows,
                                            const Uint16 cols,
                                            Uint8* pixData,
                                            const size_t pixDataLength)
{
    // Calculate total number of bits required (one per pixel)
    size_t totalBits =  0;
    if (!OFStandard::safeMult(OFstatic_cast(size_t,rows)*cols, frames.size(), totalBits))
    {
        DCMSEG_ERROR("Too many frames to concatenate");
        return EC_IllegalParameter;
    }

    // Calculate total number of bytes required (add one byte if totalBits is not a multiple of 8)
    size_t totalBytes = (totalBits + 7) / 8; // +7 to round up to the nearest byte

    // Ensure the provided pixDataLength is sufficient
    if (pixDataLength < totalBytes) {
        return EC_IllegalParameter; // or another appropriate error code
    }

    // Initialize the target pixData array to 0
    std::memset(pixData, 0, pixDataLength);

    // Concatenate the bits
    Uint32 bitIndex = 0;
    for (size_t frameIndex = 0; frameIndex < frames.size(); ++frameIndex)
    {
        const DcmIODTypes::Frame* frame = frames[frameIndex];
        Uint32 frameBits = rows * cols;
        for (Uint32 i = 0; i < frameBits; ++i)
        {
            Uint32 byteIndex = i / 8;
            Uint32 bitPos = i % 8;
            if (frame->pixData[byteIndex] & (1 << bitPos % 8)) {
                Uint32 targetByteIndex = bitIndex / 8;
                Uint32 targetBitPos = bitIndex % 8;
                pixData[targetByteIndex] |= (1 << targetBitPos);
                DCMSEG_TRACE("Setting bit at targetByteIndex: " << targetByteIndex << ", targetBitPos: " << targetBitPos << ", frame->pixData[" << byteIndex << "]: " << DcmSegUtils::debugByte2Bin(frame->pixData[byteIndex]) << ", value: " << DcmSegUtils::debugByte2Bin(pixData[targetByteIndex]));
            }
            bitIndex++;
        }
    }

    return EC_Normal; // or another appropriate success code
}


DcmIODTypes::Frame* DcmSegUtils::unpackBinaryFrame(const DcmIODTypes::Frame* frame, Uint16 rows, Uint16 cols)
{
    // Sanity checking
    if ((frame == NULL) || (rows == 0) || (cols == 0))
    {
        DCMSEG_ERROR("Cannot unpack binary frame, invalid input data");
        return NULL;
    }

    // Calculate total number of pixels
    Uint32 totalPixels = rows * cols;

    // Allocate memory for the unpacked pixel data
    DcmIODTypes::Frame* result = new DcmIODTypes::Frame();
    if (result == NULL)
    {
        DCMSEG_ERROR("Cannot allocate memory for unpacked binary frame");
        return NULL;
    }
    result->pixData = new Uint8[totalPixels];
    if (result->pixData == NULL)
    {
        DCMSEG_ERROR("Cannot allocate memory for unpacked binary frame");
        delete result;
        return NULL;
    }
    result->length = totalPixels;
    memset(result->pixData, 0, totalPixels); // Initialize to 0

    // Unpack the bits
    for (Uint32 i = 0; i < totalPixels; ++i) {
        Uint32 byteIndex = i / 8;
        Uint32 bitIndex = i % 8;
        if (frame->pixData[byteIndex] & (1 << (bitIndex))) {
            result->pixData[i] = 1;
        } else
        {
            result->pixData[i] = 0;
        }
    }

    return result;
}

void DcmSegUtils::debugDumpBin(Uint8* buffer, size_t length, const OFString& what, const OFBool raw)
{
    OFStringStream ss;
    for (size_t n = 0; n < length; n++)
    {
        if (raw)
        {
            ss << debugByte2Bin(buffer[n]);
        }
        else
        {
            ss << what << " #" << n << ": " << debugByte2Bin((buffer[n]));
        }
    }
    DCMSEG_DEBUG(ss.str());
}

OFString DcmSegUtils::debugByte2Bin(Uint8 b)
{
    OFString result("00000000");
    for (int i = 7; i >= 0; i--)
    {
        result[i] = (b & 1) + '0';

        b = OFstatic_cast(unsigned char, b) >> 1;
    }
    return result;
}

