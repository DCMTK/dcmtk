/*
 *
 *  Copyright (C) 2024-2025, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: test program for DcmElement::getUncompressedFrameSize()
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcpixseq.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcrlerp.h"
#include "dcmtk/dcmdata/dcrledrg.h"
#include "dcmtk/dcmdata/dcrleerg.h"
#include "dcmtk/dcmdata/dcswap.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofvector.h"

OFTEST(dcmdata_uncompressedFrameSize)
{
    // prepare an uncompressed dataset with the attributes we need for this test
    DcmDataset dset;
    OFCHECK(dset.putAndInsertUint16(DCM_BitsAllocated, 8).good());
    OFCHECK(dset.putAndInsertUint16(DCM_BitsStored, 8).good());
    OFCHECK(dset.putAndInsertUint16(DCM_HighBit, 7).good());
    OFCHECK(dset.putAndInsertUint16(DCM_Columns, 256).good());
    OFCHECK(dset.putAndInsertUint16(DCM_Rows, 256).good());
    OFCHECK(dset.putAndInsertUint16(DCM_SamplesPerPixel, 3).good());
    OFCHECK(dset.putAndInsertString(DCM_PhotometricInterpretation, "RGB").good());

    // create and insert an empty pixel data element
    DcmPixelData *px = NULL;
    OFCHECK(NULL != (px = new DcmPixelData(DCM_PixelData)));
    if (px)
    {
        OFCHECK(dset.insert(px).good());

        // Tests for DcmElement::decodedBitsAllocated() with uncompressed pixel data.
        // This should always return the first parameter (BitsAllocated), unless the
        // second parameter (BitsStored) is larger than the first one, in which case
        // the result should be zero.
        OFCHECK(0 == px->decodedBitsAllocated(8, 9));
        OFCHECK(5 == px->decodedBitsAllocated(5, 1));
        OFCHECK(7 == px->decodedBitsAllocated(7, 1));
        OFCHECK(8 == px->decodedBitsAllocated(8, 8));
        OFCHECK(9 == px->decodedBitsAllocated(9, 8));
        OFCHECK(13 == px->decodedBitsAllocated(13, 8));
        OFCHECK(23 == px->decodedBitsAllocated(23, 8));
        OFCHECK(255 == px->decodedBitsAllocated(255, 8));

        // Tests for getUncompressedFrameSize() with an uncompressed dataset
        Uint32 frameSize = 0;
        OFCHECK(px->getUncompressedFrameSize(&dset, frameSize, OFTrue).good());
        OFCHECK(196608 == frameSize);

        OFCHECK(dset.putAndInsertUint16(DCM_BitsAllocated, 12).good());
        OFCHECK(dset.putAndInsertUint16(DCM_BitsStored, 12).good());
        OFCHECK(px->getUncompressedFrameSize(&dset, frameSize, OFTrue).good());
        OFCHECK(294912 == frameSize);

        OFCHECK(dset.putAndInsertUint16(DCM_BitsAllocated, 16).good());
        OFCHECK(dset.putAndInsertUint16(DCM_BitsStored, 12).good());
        OFCHECK(px->getUncompressedFrameSize(&dset, frameSize, OFTrue).good());
        OFCHECK(393216 == frameSize);

        OFCHECK(dset.putAndInsertUint16(DCM_BitsAllocated, 32).good());
        OFCHECK(dset.putAndInsertUint16(DCM_BitsStored, 32).good());
        OFCHECK(px->getUncompressedFrameSize(&dset, frameSize, OFTrue).good());
        OFCHECK(786432 == frameSize);

        // set color model to YBR_FULL_422 and create a frame size indicating that we have in fact RGB
        Uint16 *pxdata = NULL;
        OFCHECK(dset.putAndInsertUint16(DCM_BitsAllocated, 8).good());
        OFCHECK(dset.putAndInsertUint16(DCM_BitsStored, 8).good());
        OFCHECK(dset.putAndInsertString(DCM_PhotometricInterpretation, "YBR_FULL_422").good());
        OFCHECK(px->createUint16Array(98304, pxdata).good());
        OFCHECK(px->getUncompressedFrameSize(&dset, frameSize, OFTrue).good());
        OFCHECK(196608 == frameSize);

        // set color model to YBR_FULL_422 and create a frame size indicating that we have in fact uncompressed YBR_FULL_422
        OFCHECK(px->createUint16Array(65536, pxdata).good());
        OFCHECK(px->getUncompressedFrameSize(&dset, frameSize, OFTrue).good());
        OFCHECK(131072 == frameSize);

        // now convert the dataset to an RLE compressed one and check again
        OFCHECK(dset.putAndInsertString(DCM_PhotometricInterpretation, "RGB").good());
        DcmRLERepresentationParameter rle_rp;
        DcmPixelSequence *pixelSequence = new DcmPixelSequence(DCM_PixelSequenceTag);
        DcmPixelItem *offsetTable = new DcmPixelItem(DCM_PixelItemTag);
        OFCHECK((NULL != pixelSequence) && (NULL != offsetTable));
        if (pixelSequence && offsetTable)
        {
            pixelSequence->insert(offsetTable);
            px->putOriginalRepresentation(EXS_RLELossless, &rle_rp, pixelSequence);

            // this should fail because the RLE decoder is not registered yet,
            // and, therefore, the computation of the uncompressed frame size will fail
            OFCHECK(EC_InvalidValue == px->getUncompressedFrameSize(&dset, frameSize, OFTrue));

            // register RLE decompression codec
            DcmRLEDecoderRegistration::registerCodecs();

            // now we can compute the frame size for the RLE compressed image
            OFCHECK(px->getUncompressedFrameSize(&dset, frameSize, OFTrue).good());
            OFCHECK(196608 == frameSize);

            // the RLE codec should refuse the size calculation if BitsAllocated is not a multiple of 8
            OFCHECK(dset.putAndInsertUint16(DCM_BitsAllocated, 12).good());
            OFCHECK(dset.putAndInsertUint16(DCM_BitsStored, 12).good());
            OFCHECK(EC_InvalidValue == px->getUncompressedFrameSize(&dset, frameSize, OFTrue));

            // 1-bit samples (monochrome only) are accepted and the uncompressed frame is packed, 8 samples per byte
            OFCHECK(dset.putAndInsertUint16(DCM_BitsAllocated, 1).good());
            OFCHECK(dset.putAndInsertUint16(DCM_BitsStored, 1).good());
            OFCHECK(dset.putAndInsertUint16(DCM_SamplesPerPixel, 1).good());
            OFCHECK(dset.putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2").good());
            OFCHECK(px->getUncompressedFrameSize(&dset, frameSize, OFTrue).good());
            OFCHECK(8192 == frameSize);
            OFCHECK(dset.putAndInsertUint16(DCM_BitsAllocated, 8).good());
            OFCHECK(dset.putAndInsertUint16(DCM_BitsStored, 8).good());
            OFCHECK(dset.putAndInsertUint16(DCM_SamplesPerPixel, 3).good());
            OFCHECK(dset.putAndInsertString(DCM_PhotometricInterpretation, "RGB").good());

            // these calls should now invoke the routine in the RLE codec that computes
            // decoded bits allocated.
            OFCHECK(0 == px->decodedBitsAllocated(8, 9));
            OFCHECK(0 == px->decodedBitsAllocated(5, 1));
            OFCHECK(8 == px->decodedBitsAllocated(8, 8));
            OFCHECK(1 == px->decodedBitsAllocated(1, 1));
            OFCHECK(0 == px->decodedBitsAllocated(13, 8));
            OFCHECK(16 == px->decodedBitsAllocated(16, 8));
            OFCHECK(64 == px->decodedBitsAllocated(64, 8));

            // avoid memory leak
            DcmRLEDecoderRegistration::cleanup();
        }
    }
}


// Encodes multi-frame 1-bit pixel data with RLE, decodes it frame by frame and
// as a whole, and checks that all samples are retained.
static void checkRLE1BitRoundTrip(const Uint16 rows, const Uint16 cols, const Uint32 frames)
{
    const size_t frameSamples = OFstatic_cast(size_t, rows) * cols;
    const size_t sampleCount = frameSamples * frames;
    const size_t packedBytes = (sampleCount + 7) / 8;

    DcmDataset dset;
    OFCHECK(dset.putAndInsertUint16(DCM_BitsAllocated, 1).good());
    OFCHECK(dset.putAndInsertUint16(DCM_BitsStored, 1).good());
    OFCHECK(dset.putAndInsertUint16(DCM_HighBit, 0).good());
    OFCHECK(dset.putAndInsertUint16(DCM_SamplesPerPixel, 1).good());
    OFCHECK(dset.putAndInsertUint16(DCM_Rows, rows).good());
    OFCHECK(dset.putAndInsertUint16(DCM_Columns, cols).good());
    char numberOfFrames[12];
    OFStandard::snprintf(numberOfFrames, sizeof(numberOfFrames), "%lu", OFstatic_cast(unsigned long, frames));
    OFCHECK(dset.putAndInsertString(DCM_NumberOfFrames, numberOfFrames).good());
    OFCHECK(dset.putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2").good());

    // runs and isolated samples, so that both RLE repeat and literal runs occur
    OFVector<Uint8> expected(sampleCount);
    for (size_t i = 0; i < sampleCount; ++i)
        expected[i] = OFstatic_cast(Uint8, ((i / 5) % 3 == 0) || (i % 11 == 0));

    // native 1-bit frames are packed without padding, least significant bit first
    OFVector<Uint8> packed(packedBytes + (packedBytes & 1), 0);
    for (size_t i = 0; i < sampleCount; ++i)
        packed[i / 8] |= OFstatic_cast(Uint8, expected[i] << (i % 8));
    OFCHECK(dset.putAndInsertUint8Array(DCM_PixelData, &packed[0], OFstatic_cast(unsigned long, packed.size())).good());

    DcmRLERepresentationParameter rle_rp;
    OFCHECK(dset.chooseRepresentation(EXS_RLELossless, &rle_rp).good());
    dset.removeAllButCurrentRepresentations();

    // decode frame by frame; each 1-bit frame is returned starting at bit 0 of the buffer
    DcmElement *elem = NULL;
    OFCHECK(dset.findAndGetElement(DCM_PixelData, elem).good());
    DcmPixelData *px = OFstatic_cast(DcmPixelData *, elem);
    if (px)
    {
        Uint32 frameSize = 0;
        OFCHECK(px->getUncompressedFrameSize(&dset, frameSize, OFFalse).good());
        OFCHECK(frameSize == (frameSamples + 7) / 8);
        OFVector<Uint8> buffer(frameSize + (frameSize & 1));
        Uint32 startFragment = 0;
        OFBool identical = OFTrue;
        for (Uint32 frame = 0; frame < frames; ++frame)
        {
            OFString colorModel;
            OFCHECK(px->getUncompressedFrame(&dset, frame, startFragment, &buffer[0], OFstatic_cast(Uint32, buffer.size()), colorModel).good());
            // the decoder returns the frame in local byte order, as for 8-bit data
            swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, &buffer[0], OFstatic_cast(Uint32, buffer.size()), sizeof(Uint16));
            for (size_t i = 0; identical && (i < frameSamples); ++i)
                identical = (((buffer[i / 8] >> (i % 8)) & 1) == expected[frame * frameSamples + i]);
        }
        OFCHECK(identical);
    }

    // decode the complete pixel data
    OFCHECK(dset.chooseRepresentation(EXS_LittleEndianExplicit, NULL).good());
    const Uint8 *decoded = NULL;
    unsigned long decodedLength = 0;
    OFCHECK(dset.findAndGetUint8Array(DCM_PixelData, decoded, &decodedLength).good());
    OFBool identical = (decoded != NULL) && (decodedLength >= packedBytes);
    for (size_t i = 0; identical && (i < sampleCount); ++i)
        identical = (((decoded[i / 8] >> (i % 8)) & 1) == expected[i]);
    OFCHECK(identical);
}


OFTEST(dcmdata_rle1BitRoundTrip)
{
    DcmRLEDecoderRegistration::registerCodecs();
    DcmRLEEncoderRegistration::registerCodecs();

    // frames that occupy whole bytes
    checkRLE1BitRoundTrip(8, 8, 2);
    // frames that do not occupy whole bytes, so the second frame starts in the middle of a byte
    checkRLE1BitRoundTrip(13, 7, 2);
    // frames with an odd number of packed bytes (35 samples in 5 bytes)
    checkRLE1BitRoundTrip(7, 5, 2);

    // 1-bit samples are only supported with a single sample per pixel
    DcmDataset dset;
    OFCHECK(dset.putAndInsertUint16(DCM_BitsAllocated, 1).good());
    OFCHECK(dset.putAndInsertUint16(DCM_BitsStored, 1).good());
    OFCHECK(dset.putAndInsertUint16(DCM_HighBit, 0).good());
    OFCHECK(dset.putAndInsertUint16(DCM_SamplesPerPixel, 3).good());
    OFCHECK(dset.putAndInsertUint16(DCM_PlanarConfiguration, 0).good());
    OFCHECK(dset.putAndInsertUint16(DCM_Rows, 8).good());
    OFCHECK(dset.putAndInsertUint16(DCM_Columns, 8).good());
    OFCHECK(dset.putAndInsertString(DCM_PhotometricInterpretation, "RGB").good());
    const Uint8 pixels[24] = { 0 };
    OFCHECK(dset.putAndInsertUint8Array(DCM_PixelData, pixels, sizeof(pixels)).good());
    DcmRLERepresentationParameter rle_rp;
    OFCHECK(dset.chooseRepresentation(EXS_RLELossless, &rle_rp).bad());

    DcmRLEEncoderRegistration::cleanup();
    DcmRLEDecoderRegistration::cleanup();
}
