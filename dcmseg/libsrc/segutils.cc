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

#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmseg/segutils.h"

DcmIODTypes::Frame* DcmSegUtils::packBinaryFrame(const Uint8* pixelData, const Uint16 rows, const Uint16 columns)
{
    // Sanity checking
    const size_t numPixels = OFstatic_cast(size_t, rows) * columns;
    if (numPixels == 0)
    {
        DCMSEG_ERROR("Unable to pack binary segmentation frame: Rows or Columns is 0");
        return NULL;
    }
    if (!pixelData)
    {
        DCMSEG_ERROR("Unable to pack binary segmentation frame: No pixel data provided");
        return NULL;
    }
    DcmIODTypes::Frame* frame = new DcmIODTypes::Frame();
    if (frame == NULL)
    {
        DCMSEG_ERROR("Could not pack binary segmentation frame: Memory exhausted");
        return NULL;
    }
    frame->length  = getBytesForBinaryFrame(numPixels);
    frame->pixData = new Uint8[frame->length];
    if (frame->pixData == 0)
    {
        delete frame;
        return NULL;
    }
    memset(frame->pixData, 0, frame->length);

    size_t bytePos = 0;
    for (size_t count = 0; count < numPixels; count++)
    {
        // Compute byte position
        bytePos = count / 8;
        frame->pixData[bytePos]
            |= (pixelData[count] != 0) /* value to set */ << (count % 8 /* bit position (0-7) within byte */);
    }
    return frame;
}

DcmIODTypes::Frame* DcmSegUtils::unpackBinaryFrame(const DcmIODTypes::Frame* frame, Uint16 rows, Uint16 cols)
{
    // Sanity checking
    if ((frame == NULL) || (rows == 0) || (cols == 0))
    {
        DCMSEG_ERROR("Cannot unpack binary frame, invalid input data");
        return NULL;
    }

    // Create result frame in memory
    size_t numBits             = OFstatic_cast(size_t, rows) * cols;
    DcmIODTypes::Frame* result = new DcmIODTypes::Frame();
    if (result)
    {
        result->pixData = new Uint8[numBits];
        if (!result->pixData)
        {
            delete result;
            return NULL;
        }
        result->length = numBits;
    }
    if (!result || !(result->pixData))
    {
        DCMSEG_ERROR("Cannot unpack binary frame, memory exhausted");
        return NULL;
    }
    memset(result->pixData, 0, result->length);

    // Transform and copy from packed frame to unpacked result frame
    size_t bytePos = 0;
    for (size_t count = 0; count < numBits; count++)
    {
        // Compute byte position
        bytePos = count / 8;
        // Bit position (0-7) within byte
        Uint8 bitpos = (count % 8);
        if ((frame->pixData[bytePos] & (1 << bitpos) /* check whether bit at bitpos is set*/))
        {
            result->pixData[count] = 1;
        }
        else
        {
            result->pixData[count] = 0;
        }
    }
    return result;
}

size_t DcmSegUtils::getBytesForBinaryFrame(const size_t& numPixels)
{
    // check whether the 1-bit pixels exactly fit into bytes
    size_t remainder = numPixels % 8;
    // number of bytes that work on an exact fit
    size_t bytesRequired = numPixels / 8;
    // add one byte if we have a remainder
    if (remainder > 0)
        bytesRequired++;
    return bytesRequired;
}

void DcmSegUtils::alignFrameOnBitPosition(Uint8* buf, const size_t bufLen, const Uint8 numBits)
{
    if (numBits > 7)
    {
        DCMSEG_ERROR("Invalid input data: alignFrameOnBitPosition() can only shift 0-7 bits");
        return;
    }
    Uint8 carryOver = 0;
    for (size_t x = 0; x < bufLen; x++)
    {
        // Store current byte since we need to restore its first bits later
        Uint8 current = buf[x];
        // Shift pixels given num bits to the left, creating (8 - num bits)
        // empty bits at the right
        buf[x] <<= numBits;
        // If there is a carry over from the previous byte, add it in again (will always be at the end,
        // see carry over calculation in next step)
        buf[x] |= carryOver;
        // Compute carry over bits for next byte to be handled, i.e. those bits at the left that will be
        // overwritten in the next byte
        carryOver = OFstatic_cast(unsigned char, current) >> (8 - numBits);
    }
}

void DcmSegUtils::alignFrameOnByteBoundary(Uint8* buf, const size_t bufLen, const Uint8 numBits)
{
    return DcmIODUtil::alignFrameOnByteBoundary(buf, bufLen, numBits);
}

void DcmSegUtils::debugDumpBin(Uint8* buffer, size_t length, const char* what)
{
    for (size_t n = 0; n < length; n++)
    {
        DCMSEG_DEBUG(what << " #" << n << ": " << OFstatic_cast(size_t, (buffer[n])) << ", bytepos " << &(buffer[n])
                          << " (" << debugByte2Bin((buffer[n])) << ")");
    }
    DCMSEG_DEBUG("");
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
