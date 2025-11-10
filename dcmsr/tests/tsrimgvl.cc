/*
 *
 *  Copyright (C) 2025, J. Riesmeier, Oldenburg, Germany
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    test program for class DSRImageReferenceValue
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"

#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dctypes.h"

#include "dcmtk/dcmsr/dsrimgvl.h"
#include "dcmtk/dcmsr/dsrimgtn.h"
#include "dcmtk/dcmsr/codes/dcm.h"


OFTEST(dcmsr_createMonochromeIconImage)
{
    DSRImageReferenceValue imgValue(UID_CTImageStorage, "1.2.3.4.5.6.7.8.9.0.98");
    OFCHECK(!imgValue.hasIconImage());

    /* create a monochrome image dataset */
    DcmDataset dataset;
    Uint8 pixelData[256];
    for (int i = 0; i < 256; i++)
        pixelData[i] = OFstatic_cast(Uint8, i);
    OFCHECK(dataset.putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2").good());
    OFCHECK(dataset.putAndInsertUint16(DCM_SamplesPerPixel, 1).good());
    OFCHECK(dataset.putAndInsertUint16(DCM_PixelRepresentation, 0).good());
    OFCHECK(dataset.putAndInsertUint16(DCM_Rows, 16).good());
    OFCHECK(dataset.putAndInsertUint16(DCM_Columns, 16).good());
    OFCHECK(dataset.putAndInsertUint16(DCM_BitsAllocated, 8).good());
    OFCHECK(dataset.putAndInsertUint16(DCM_BitsStored, 8).good());
    OFCHECK(dataset.putAndInsertUint16(DCM_HighBit, 7).good());
    OFCHECK(dataset.putAndInsertUint8Array(DCM_PixelData, pixelData, sizeof(pixelData)).good());

    /* create an icon image with 64x64 pixels */
    OFCHECK(imgValue.createIconImage(&dataset, EXS_Unknown /*xfer*/, 0 /*frame*/, 64 /*width*/, 64 /*height*/).good());
    OFCHECK(imgValue.hasIconImage());

    /* set value to image tree node and write to item */
    DcmItem item;
    DSRImageTreeNode imgNode(DSRTypes::RT_contains);
    OFCHECK(imgNode.setConceptName(CODE_DCM_BestInSet).good());
    OFCHECK(imgNode.setValue(imgValue).good());
    OFCHECK(imgNode.hasIconImage());
    OFCHECK(imgNode.write(item).good());
    /* output content of the item (in debug mode only) */
    if (DCM_dcmsrLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
        item.print(COUT, DCMTypes::PF_shortenLongTagValues /*flags*/, 2 /*level*/);

    /* delete the icon image */
    imgNode.deleteIconImage();
    OFCHECK(!imgNode.hasIconImage());
}


OFTEST(dcmsr_createColorIconImage)
{
    DSRImageReferenceValue imgValue(UID_CTImageStorage, "1.2.3.4.5.6.7.8.9.0.99");
    OFCHECK(!imgValue.hasIconImage());

    /* create a color image dataset */
    DcmDataset dataset;
    Uint8 pixelData[256][3];
    for (int i = 0; i < 256; i++)
    {
        pixelData[i][0] = OFstatic_cast(Uint8, i);
        pixelData[i][1] = OFstatic_cast(Uint8, i);
        pixelData[i][2] = OFstatic_cast(Uint8, i);
    }
    OFCHECK(dataset.putAndInsertString(DCM_PhotometricInterpretation, "RGB").good());
    OFCHECK(dataset.putAndInsertUint16(DCM_SamplesPerPixel, 3).good());
    OFCHECK(dataset.putAndInsertUint16(DCM_PixelRepresentation, 0).good());
    OFCHECK(dataset.putAndInsertUint16(DCM_PlanarConfiguration, 0).good());
    OFCHECK(dataset.putAndInsertUint16(DCM_Rows, 16).good());
    OFCHECK(dataset.putAndInsertUint16(DCM_Columns, 16).good());
    OFCHECK(dataset.putAndInsertUint16(DCM_BitsAllocated, 8).good());
    OFCHECK(dataset.putAndInsertUint16(DCM_BitsStored, 8).good());
    OFCHECK(dataset.putAndInsertUint16(DCM_HighBit, 7).good());
    OFCHECK(dataset.putAndInsertUint8Array(DCM_PixelData, pixelData[0], sizeof(pixelData)).good());

    /* create an icon image with 64x64 pixels */
    OFCHECK(imgValue.createIconImage(&dataset, EXS_Unknown /*xfer*/, 0 /*frame*/, 64 /*width*/, 64 /*height*/).good());
    OFCHECK(imgValue.hasIconImage());

    /* set value to image tree node and write to item */
    DcmItem item;
    DSRImageTreeNode imgNode(DSRTypes::RT_hasProperties);
    OFCHECK(imgNode.setConceptName(CODE_DCM_BestIllustrationOfFinding).good());
    OFCHECK(imgNode.setValue(imgValue).good());
    OFCHECK(imgNode.hasIconImage());
    OFCHECK(imgNode.write(item).good());
    /* output content of the item (in debug mode only) */
    if (DCM_dcmsrLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
        item.print(COUT, DCMTypes::PF_shortenLongTagValues /*flags*/, 2 /*level*/);

    /* delete the icon image */
    imgNode.deleteIconImage();
    OFCHECK(!imgNode.hasIconImage());
}
