/*
 *
 *  Copyright (C) 2015-2025, Open Connections GmbH
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
#include <iostream>
#include "dcmtk/dcmseg/segutils.h"


OFCondition DcmSegUtils::concatBinaryFrames(const OFVector<DcmIODTypes::FrameBase*>& frames,
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
    memset(pixData, 0, pixDataLength);

    // Concatenate the bits
    size_t bitIndex = 0;

    for (size_t frameIndex = 0; frameIndex < frames.size(); ++frameIndex)
    {
        // Make sure frame has correct bit depth
        if (frames[frameIndex]->bytesPerPixel() != 1)
        {
            DCMSEG_ERROR("Cannot concatenate frames with bits allocated != 1");
            return EC_IllegalParameter;
        }
        // Cast the frame to the appropriate type to make access easier
        DcmIODTypes::Frame<Uint8>* frame = OFstatic_cast(DcmIODTypes::Frame<Uint8>*,frames[frameIndex]);
        size_t frameBits = rows * cols;
        // If a frame always has bytes fully packed, i.e. number of bits i a multiple of 8,
        // we can copy the bytes directly and don't have to fiddle with the bits
        if (frameBits % 8 == 0)
        {
            size_t bytesToCopy = frameBits / 8;
            memcpy(pixData + (bitIndex / 8), frame->getPixelData(), bytesToCopy);
            DCMSEG_TRACE("Copying " << bytesToCopy << " bytes from frame " << frameIndex << " to pixData at index " << (bitIndex / 8));
            bitIndex += frameBits;
        }
        else
        {
            // we need to copy bitwise, so we iterate over the bits
            DCMSEG_TRACE("Copying " << frameBits << " bits from frame " << frameIndex << " to pixData at index " << (bitIndex / 8));
            for (size_t i = 0; i < frameBits; ++i)
            {
                size_t byteIndex = i / 8;
                size_t bitPos = i % 8;
                if (frame->m_pixData[byteIndex] & (1 << bitPos % 8)) {
                    size_t targetByteIndex = bitIndex / 8;
                    size_t targetBitPos = bitIndex % 8;
                    pixData[targetByteIndex] |= (1 << targetBitPos);
                    DCMSEG_TRACE("Setting bit at targetByteIndex: " << targetByteIndex << ", targetBitPos: " << targetBitPos << ", frame->pixData[" << byteIndex << "]: " << DcmSegUtils::debugByte2Bin(OFstatic_cast(Uint8*, frame->getPixelData())[byteIndex]) << ", value: " << DcmSegUtils::debugByte2Bin(pixData[targetByteIndex]));
                }
                bitIndex++;
            }
        }
    }

    return EC_Normal; // or another appropriate success code
}


DcmIODTypes::Frame<Uint8>* DcmSegUtils::unpackBinaryFrame(const DcmIODTypes::Frame<Uint8>* frame, Uint16 rows, Uint16 cols)
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
    DcmIODTypes::Frame<Uint8>* result = new DcmIODTypes::Frame<Uint8>();
    if (result == NULL)
    {
        DCMSEG_ERROR("Cannot allocate memory for unpacked binary frame");
        return NULL;
    }
    result->m_pixData = new Uint8[totalPixels];
    if (result->m_pixData == NULL)
    {
        DCMSEG_ERROR("Cannot allocate memory for unpacked binary frame");
        delete result;
        return NULL;
    }
    result->m_numPixels = totalPixels;
    memset(result->m_pixData, 0, totalPixels); // Initialize to 0

    // Unpack the bits
    for (Uint32 i = 0; i < totalPixels; ++i) {
        Uint32 byteIndex = i / 8;
        Uint32 bitIndex = i % 8;
        if (frame->m_pixData[byteIndex] & (1 << (bitIndex))) {
            result->m_pixData[i] = 1;
        } else
        {
            result->m_pixData[i] = 0;
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
