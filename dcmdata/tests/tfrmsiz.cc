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

            // these calls should now invoke the routine in the RLE codec that computes
            // decoded bits allocated.
            OFCHECK(0 == px->decodedBitsAllocated(8, 9));
            OFCHECK(0 == px->decodedBitsAllocated(5, 1));
            OFCHECK(8 == px->decodedBitsAllocated(8, 8));
            OFCHECK(0 == px->decodedBitsAllocated(13, 8));
            OFCHECK(16 == px->decodedBitsAllocated(16, 8));
            OFCHECK(64 == px->decodedBitsAllocated(64, 8));

            // avoid memory leak
            DcmRLEDecoderRegistration::cleanup();
        }
    }
}
