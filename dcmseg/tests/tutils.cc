/*
 *
 *  Copyright (C) 2015-2026, Open Connections GmbH
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
 *  Purpose: tests for dcmseg's DcmSegUtils functionalities
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmseg/segutils.h"
#include "dcmtk/dcmseg/segtypes.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/oftime.h" // For debugByte2Bin
#include <iostream>

#define bufLen 4


// Convert an 8-character "01010101" string into a Uint8.
// Used in place of C++14 binary literals (0b...) to keep the test code C++98-compliant
// while remaining as readable as the original literal form.
static Uint8 bin2byte(const char* bits)
{
    Uint8 value = 0;
    for (int i = 0; i < 8 && bits[i] != '\0'; ++i)
    {
        value = OFstatic_cast(Uint8, (value << 1) | (bits[i] == '1' ? 1 : 0));
    }
    return value;
}


// Check whether packing of sparse frames into binary packed frames works correctly
OFTEST(dcmseg_packBinaryFrame)
{
    // Check whether the following statically defined frames are packed correctly
    Uint8 sparseFrame1[8] = {1, 1, 1, 1, 0, 0, 0, 0};
    DcmIODTypes::Frame<Uint8>* packed = DcmSegUtils::packBinaryFrame(sparseFrame1, 4, 2);
    OFCHECK(packed != NULL);
    OFCHECK(packed->getLengthInBytes() == 1);
    OFCHECK_MSG(OFstatic_cast(Uint8*, packed->getPixelData())[0] == bin2byte("00001111"), OFString("Expected 00001111, got ") + DcmSegUtils::debugByte2Bin(  OFstatic_cast(Uint8*, packed->getPixelData())[0]));
    delete packed;

    Uint8 sparseFrame2[8] = {1, 0, 1, 0, 1, 0, 1, 0};
    packed = DcmSegUtils::packBinaryFrame(sparseFrame2, 4, 2);
    OFCHECK(packed != NULL);
    OFCHECK(packed->getLengthInBytes() == 1);
    OFCHECK_MSG(OFstatic_cast(Uint8*, packed->getPixelData())[0] == bin2byte("01010101"), OFString("Expected 01010101, got ") + DcmSegUtils::debugByte2Bin(  OFstatic_cast(Uint8*, packed->getPixelData())[0]));
    delete packed;

    // Now try the that is larger than a byte and not a multiple of 8, with every third pixel set to 1
    Uint8 sparseFrame3[15] = {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1};
    packed = DcmSegUtils::packBinaryFrame(sparseFrame3, 5, 3);
    OFCHECK(packed != NULL);
    OFCHECK(packed->getLengthInBytes() == 2);
    OFCHECK_MSG(OFstatic_cast(Uint8*, packed->getPixelData())[0] == bin2byte("00100100"), OFString("Expected 00100100, got ") + DcmSegUtils::debugByte2Bin(  OFstatic_cast(Uint8*, packed->getPixelData())[0]));
    OFCHECK_MSG(OFstatic_cast(Uint8*, packed->getPixelData())[1] == bin2byte("01001001"), OFString("Expected 01001001, got ") + DcmSegUtils::debugByte2Bin(  OFstatic_cast(Uint8*, packed->getPixelData())[1]));
    delete packed;

    // Now the same but with every 5th pixel set to 1, and rows=7 and cols=5
    Uint8 sparseFrame4[35];
    memset(sparseFrame4, 0, 35);
    for (int i = 0; i < 35; i += 5)
    {
        sparseFrame4[i] = 1;
    }
    packed = DcmSegUtils::packBinaryFrame(sparseFrame4, 7, 5);
    OFCHECK(packed != NULL);
    OFCHECK(packed->getLengthInBytes() == 5);
    OFCHECK_MSG(OFstatic_cast(Uint8*, packed->getPixelData())[0] == bin2byte("00100001"), OFString("Expected 00100001, got ") + DcmSegUtils::debugByte2Bin(  OFstatic_cast(Uint8*, packed->getPixelData())[0]));
    OFCHECK_MSG(OFstatic_cast(Uint8*, packed->getPixelData())[1] == bin2byte("10000100"), OFString("Expected 10000100, got ") + DcmSegUtils::debugByte2Bin(  OFstatic_cast(Uint8*, packed->getPixelData())[1]));
    OFCHECK_MSG(OFstatic_cast(Uint8*, packed->getPixelData())[2] == bin2byte("00010000"), OFString("Expected 00010000, got ") + DcmSegUtils::debugByte2Bin(  OFstatic_cast(Uint8*, packed->getPixelData())[2]));
    OFCHECK_MSG(OFstatic_cast(Uint8*, packed->getPixelData())[3] == bin2byte("01000010"), OFString("Expected 01000010, got ") + DcmSegUtils::debugByte2Bin(  OFstatic_cast(Uint8*, packed->getPixelData())[3]));
    OFCHECK_MSG(OFstatic_cast(Uint8*, packed->getPixelData())[4] == bin2byte("00000000"), OFString("Expected 00000000, got ") + DcmSegUtils::debugByte2Bin(  OFstatic_cast(Uint8*, packed->getPixelData())[4]));
    delete packed;

    // In 1000 iterations create sparse frames and pack them. Check whether the
    // packed frame is correct. This is not bullet proof but we use the same addressing
    // as in the packing/unpacking code to make sure we address the correct bit.
    // Use a random number of cols and rows (each between 1 and 100).
    // If the packed frame is not correct, the test fails.
    OFTime tm;
    for (unsigned int i = 0; i < 1000; i++)
    {
        unsigned int now = OFstatic_cast(unsigned int, time(NULL));
        Uint16 cols = OFstatic_cast(Uint16, OFrand_r(now) % 100 + 1);
        Uint16 rows = OFstatic_cast(Uint16, OFrand_r(now) % 100 + 1);
        Uint16 pixelCount = OFstatic_cast(Uint16, cols * rows);

        Uint8* sparseFrame = new Uint8[pixelCount];
        OFCHECK(sparseFrame != NULL);

        // Create a random sparse frame
        for (unsigned int j = 0; j < pixelCount; j++)
        {
            tm.setCurrentTime();
            unsigned int micro = tm.getMicroSecond();
            sparseFrame[j] = OFstatic_cast(Uint8, OFrand_r(micro) % 2);
        }
        // Pack the frame
        DcmIODTypes::Frame<Uint8>* packedFrame = DcmSegUtils::packBinaryFrame(sparseFrame, rows, cols);
        OFCHECK(packedFrame != NULL);

        // Check the result
        for (unsigned int j = 0; j < pixelCount; j++)
        {
            Uint32 byteIndex = j / 8;
            Uint32 bitIndex = j % 8;
            Uint8 mask = 1 << bitIndex;
            Uint8 currentByte = OFstatic_cast(Uint8*, packedFrame->getPixelData())[byteIndex];
            // Check whether the bit at position j is set correctly
            if ((sparseFrame[j] == 0) != ((currentByte & mask) == 0))
            {
                OFCHECK_FAIL("Failed for row " << j / cols << " and column " << j % cols);
            }
        }

        // Clean up
        delete[] sparseFrame;
        delete packedFrame;
    }
}

OFTEST(dcmseg_packAndUnpackBinaryFrame)
{

    // Make 1000 iterations, using a random number of cols and rows (each between 1 and 100).
    // In each iteration, create a random "sparse" frame, pack it, unpack it and compare the result.
    // If the result is not the same, the test fails.
    unsigned int now = OFstatic_cast(unsigned int, time(NULL));
    for (unsigned int i = 0; i < 1000; i++)
    {
        Uint16 cols = OFstatic_cast(Uint16, OFrand_r(now) % 100 + 1);
        Uint16 rows = OFstatic_cast(Uint16, OFrand_r(now) % 100 + 1);
        Uint16 pixelCount = OFstatic_cast(Uint16, cols * rows);

        Uint8* sparseFrame = new Uint8[pixelCount];
        OFCHECK(sparseFrame != NULL);
        DcmIODTypes::FrameBase* packed;
        DcmIODTypes::FrameBase* unpacked;

        // Create a random sparse frame
        for (unsigned int j = 0; j < pixelCount; j++)
        {
            sparseFrame[j] = OFstatic_cast(Uint8, OFrand_r(now) % 2);
        }

        // Pack and unpack the frame
        packed = DcmSegUtils::packBinaryFrame(sparseFrame, rows, cols);
        OFCHECK(packed != NULL);
        unpacked = DcmSegUtils::unpackBinaryFrame(OFstatic_cast(DcmIODTypes::Frame<Uint8>*, packed), rows, cols);
        OFCHECK(unpacked != NULL);

        // Compare the result
        OFCHECK(memcmp(sparseFrame, unpacked->getPixelData(), pixelCount) == 0);

        delete[] sparseFrame;
        delete packed;
        delete unpacked;
    }

    // Do the same with frame completely 0, and 1 (i.e. all bits set), respectively
    for (int i = 0; i < 10; ++i) {
        Uint16 rows = OFstatic_cast(Uint16, OFrand_r(now) % 100 + 1);
        Uint16 cols = OFstatic_cast(Uint16, OFrand_r(now) % 100 + 1);
        Uint16 pixelCount = OFstatic_cast(Uint16, rows * cols);

        Uint8* sparseFrame = new Uint8[pixelCount];
        OFCHECK(sparseFrame != NULL);
        memset(sparseFrame, 0, pixelCount);
        DcmIODTypes::FrameBase* packed = DcmSegUtils::packBinaryFrame(sparseFrame, rows, cols);
        OFCHECK(packed != NULL);
        DcmIODTypes::FrameBase* unpacked = DcmSegUtils::unpackBinaryFrame(OFstatic_cast(DcmIODTypes::Frame<Uint8>*, packed), rows, cols);
        OFCHECK(unpacked != NULL);
        OFCHECK(memcmp(sparseFrame, unpacked->getPixelData(), pixelCount) == 0);
        delete packed;
        delete unpacked;

        memset(sparseFrame, 1, pixelCount);
        packed = DcmSegUtils::packBinaryFrame(sparseFrame, rows, cols);
        OFCHECK(packed != NULL);
        unpacked = DcmSegUtils::unpackBinaryFrame(OFstatic_cast(DcmIODTypes::Frame<Uint8>*, packed), rows, cols);
        OFCHECK(unpacked != NULL);
        OFCHECK(memcmp(sparseFrame, unpacked->getPixelData(), pixelCount) == 0);
        delete packed;
        delete unpacked;

        delete[] sparseFrame;
    }
}

OFTEST(dcmseg_concatBinaryFrames)
{
    // Test with two frames that will overlap in the packed frame within one byte
    const int rows = 3;
    const int cols = 4;
    Uint8 sparseFrame1[rows * cols] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    Uint8 sparseFrame2[rows * cols] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    DcmIODTypes::FrameBase* packed1 = DcmSegUtils::packBinaryFrame(sparseFrame1, rows, cols);
    DcmIODTypes::FrameBase* packed2 = DcmSegUtils::packBinaryFrame(sparseFrame2, rows, cols);
    OFCHECK(packed1 != NULL);
    OFCHECK(packed2 != NULL);
    OFVector<DcmIODTypes::FrameBase*> inputFrames;
    inputFrames.push_back(packed1);
    inputFrames.push_back(packed2);
    // Now concatenate the two frames into a single bit array
    Uint8* pixData = new Uint8[2 * rows * cols];
    OFCHECK(pixData != NULL);
    OFCondition result = DcmSegUtils::concatBinaryFrames(inputFrames, rows, cols, pixData, 2 * rows * cols);
    OFCHECK(result.good());
    // Check the result
    OFCHECK_MSG(pixData[0] == bin2byte("11111111"), OFString("Expected 11111111, got ") + DcmSegUtils::debugByte2Bin(pixData[0]));
    OFCHECK_MSG(pixData[1] == bin2byte("11101111"), OFString("Expected 11101111, got ") + DcmSegUtils::debugByte2Bin(pixData[1]));
    OFCHECK_MSG(pixData[2] == bin2byte("11111111"), OFString("Expected 11111111, got ") + DcmSegUtils::debugByte2Bin(pixData[2]));
    // avoid memory leaks
    delete packed1;
    delete packed2;
    delete[] pixData;
}

// Test DcmSegUtils::debugByte2Bin()
OFTEST(dcmseg_debugByte2Bin)
{
    // Test with all possible bit positions, at least
    OFCHECK(DcmSegUtils::debugByte2Bin(0) == "00000000");
    OFCHECK(DcmSegUtils::debugByte2Bin(1) == "00000001");
    OFCHECK(DcmSegUtils::debugByte2Bin(2) == "00000010");
    OFCHECK(DcmSegUtils::debugByte2Bin(4) == "00000100");
    OFCHECK(DcmSegUtils::debugByte2Bin(8) == "00001000");
    OFCHECK(DcmSegUtils::debugByte2Bin(16) == "00010000");
    OFCHECK(DcmSegUtils::debugByte2Bin(32) == "00100000");
    OFCHECK(DcmSegUtils::debugByte2Bin(64) == "01000000");
    OFCHECK(DcmSegUtils::debugByte2Bin(128) == "10000000");
    OFCHECK(DcmSegUtils::debugByte2Bin(255) == "11111111");
}


OFTEST(dcmseg_extractBinaryFrames)
{
    // Exhaustively test extractBinaryFrames for bitsPerFrame 1..200 and numFrames 1..20.
    // For each combination:
    //   1. Create random sparse frames (one byte per pixel, value 0 or 1)
    //   2. Pack each frame individually with packBinaryFrame
    //   3. Concatenate all packed frames into a single pixel data buffer with concatBinaryFrames
    //   4. Extract individual frames from that buffer with extractBinaryFrames
    //   5. Compare each extracted frame byte-by-byte against the individually packed original
    //
    // This covers all bit-alignment cases (bitsPerFrame % 8 == 0..7), the memcpy fast path
    // (multiples of 8), the bit-by-bit slow path, and multi-frame boundary crossings.

    unsigned int seed = OFstatic_cast(unsigned int, time(NULL));
    size_t numTests = 0;

    for (size_t bitsPerFrame = 1; bitsPerFrame <= 200; bitsPerFrame++)
    {
        for (size_t numFrames = 1; numFrames <= 20; numFrames++)
        {
            size_t bytesPerFrame = (bitsPerFrame + 7) / 8;

            // Step 1: Create random sparse frames and pack each individually
            OFVector<Uint8*> sparseFrames;
            OFVector<DcmIODTypes::FrameBase*> packedFrames;
            sparseFrames.reserve(numFrames);
            packedFrames.reserve(numFrames);

            for (size_t f = 0; f < numFrames; f++)
            {
                Uint8* sparse = new Uint8[bitsPerFrame];
                for (size_t b = 0; b < bitsPerFrame; b++)
                {
                    sparse[b] = OFstatic_cast(Uint8, OFrand_r(seed) % 2);
                }
                sparseFrames.push_back(sparse);

                // Use rows=1, cols=bitsPerFrame to pack (rows*cols = bitsPerFrame)
                DcmIODTypes::FrameBase* packed = DcmSegUtils::packBinaryFrame(
                    sparse, 1, OFstatic_cast(Uint16, bitsPerFrame));
                OFCHECK(packed != NULL);
                if (!packed)
                    goto cleanup;
                packedFrames.push_back(packed);
            }

            {
                // Step 2: Concatenate all packed frames into a single buffer
                // Total bits = numFrames * bitsPerFrame, total bytes = ceil(total_bits / 8)
                size_t totalBits = numFrames * bitsPerFrame;
                size_t totalBytes = (totalBits + 7) / 8;
                Uint8* pixData = new Uint8[totalBytes];
                memset(pixData, 0, totalBytes);

                OFCondition result = DcmSegUtils::concatBinaryFrames(
                    packedFrames, 1, OFstatic_cast(Uint16, bitsPerFrame),
                    pixData, totalBytes);
                OFCHECK(result.good());

                if (result.good())
                {
                    // Step 3: Extract frames from concatenated buffer
                    OFVector<DcmIODTypes::FrameBase*> extracted;
                    result = DcmIODUtil::extractBinaryFrames(
                        pixData, totalBytes, numFrames, bitsPerFrame, extracted);
                    OFCHECK(result.good());

                    if (result.good())
                    {
                        OFCHECK(extracted.size() == numFrames);

                        // Step 4: Compare each extracted frame against individually packed original
                        for (size_t f = 0; f < numFrames && f < extracted.size(); f++)
                        {
                            OFCHECK(extracted[f] != NULL);
                            OFCHECK(packedFrames[f] != NULL);
                            if (extracted[f] && packedFrames[f])
                            {
                                int cmp = memcmp(extracted[f]->getPixelData(),
                                                 packedFrames[f]->getPixelData(),
                                                 bytesPerFrame);
                                if (cmp != 0)
                                {
                                    OFCHECK_FAIL("extractBinaryFrames mismatch: bitsPerFrame="
                                        << bitsPerFrame << " numFrames=" << numFrames
                                        << " frame=" << f);
                                }
                            }
                        }
                    }

                    // Clean up extracted frames
                    for (size_t f = 0; f < extracted.size(); f++)
                        delete extracted[f];
                }

                delete[] pixData;
            }

        cleanup:
            // Clean up packed and sparse frames
            for (size_t f = 0; f < packedFrames.size(); f++)
                delete packedFrames[f];
            for (size_t f = 0; f < sparseFrames.size(); f++)
                delete[] sparseFrames[f];

            numTests++;
        }
    }
    // Sanity check: we ran 200 * 20 = 4000 combinations
    OFCHECK(numTests == 4000);

    // Undersized buffers must be rejected instead of over-read: request more
    // frames (or larger frames) than the buffer can hold and expect an error.
    {
        Uint8 buffer[10];
        memset(buffer, 0, sizeof(buffer));
        OFVector<DcmIODTypes::FrameBase*> extracted;
        // 10 bytes hold 5 frames of 16 bits; asking for 6 must fail.
        OFCHECK(DcmIODUtil::extractBinaryFrames(buffer, sizeof(buffer), 6, 16, extracted).bad());
        OFCHECK(extracted.empty());
        // Exactly filling the buffer (5 frames of 16 bits = 10 bytes) must work.
        OFCHECK(DcmIODUtil::extractBinaryFrames(buffer, sizeof(buffer), 5, 16, extracted).good());
        for (size_t f = 0; f < extracted.size(); f++)
            delete extracted[f];
    }
}

