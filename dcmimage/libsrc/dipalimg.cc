/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomPaletteImage (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dctag.h"

#include "dcmtk/dcmimgle/didocu.h"
#include "dcmtk/dcmimage/dipalimg.h"
#include "dcmtk/dcmimage/dipalpxt.h"
#include "dcmtk/dcmimage/diqttype.h"
#include "dcmtk/dcmimage/dilogger.h"
#include "dcmtk/dcmimgle/diluptab.h"
#include "dcmtk/dcmimgle/diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiPaletteImage::DiPaletteImage(const DiDocument *docu,
                               const EI_Status status)
  : DiColorImage(docu, status, 1),
    Palette()  // initializes the three color palettes to NULL
{
    if ((Document != NULL) && (InputData != NULL) && (ImageStatus == EIS_Normal))
    {
        if (BitsStored <= MAX_TABLE_ENTRY_SIZE)
        {
            const EL_BitsPerTableEntry descMode = (docu->getFlags() & CIF_CheckLutBitDepth) ? ELM_CheckValue : ELM_UseValue;
            /* wrong palette attribute tags used */
            if (Document->getFlags() & CIF_WrongPaletteAttributeTags)
            {
                Palette[0] = new DiLookupTable(Document, DCM_RETIRED_LargeRedPaletteColorLookupTableDescriptor,
                    DCM_RETIRED_LargeRedPaletteColorLookupTableData, DCM_UndefinedTagKey /*explanation*/, descMode, &ImageStatus);
                Palette[1] = new DiLookupTable(Document, DCM_RETIRED_LargeGreenPaletteColorLookupTableDescriptor,
                    DCM_RETIRED_LargeGreenPaletteColorLookupTableData, DCM_UndefinedTagKey /*explanation*/, descMode, &ImageStatus);
                Palette[2] = new DiLookupTable(Document, DCM_RETIRED_LargeBluePaletteColorLookupTableDescriptor,
                    DCM_RETIRED_LargeBluePaletteColorLookupTableData, DCM_UndefinedTagKey /*explanation*/, descMode, &ImageStatus);
            } else {
                const Uint16 *dummy = NULL;
                /* check for (non-empty) segmented palette */
                if ((Document->getValue(DCM_SegmentedRedPaletteColorLookupTableData, dummy) > 0) ||
                    (Document->getValue(DCM_SegmentedGreenPaletteColorLookupTableData, dummy) > 0) ||
                    (Document->getValue(DCM_SegmentedBluePaletteColorLookupTableData, dummy) > 0))
                {
                    DCMIMAGE_WARN("segmented palettes not yet supported ... ignoring");
                }
                /* read data from non-segmented palettes (if present) */
                Palette[0] = new DiLookupTable(Document, DCM_RedPaletteColorLookupTableDescriptor,
                    DCM_RedPaletteColorLookupTableData, DCM_UndefinedTagKey /*explanation*/, descMode, &ImageStatus);
                Palette[1] = new DiLookupTable(Document, DCM_GreenPaletteColorLookupTableDescriptor,
                    DCM_GreenPaletteColorLookupTableData, DCM_UndefinedTagKey /*explanation*/, descMode, &ImageStatus);
                Palette[2] = new DiLookupTable(Document, DCM_BluePaletteColorLookupTableDescriptor,
                    DCM_BluePaletteColorLookupTableData, DCM_UndefinedTagKey /*explanation*/, descMode, &ImageStatus);
            }
            if ((ImageStatus == EIS_Normal) && (Palette[0] != NULL) && (Palette[1] != NULL) && (Palette[2] != NULL))
            {
                BitsPerSample = 0;
                /* determine the maximum value for bits stored of the three lookup tables */
                for (int jj = 0; jj < 3; jj++)
                {
                    if (Palette[jj]->getBits() > OFstatic_cast(Uint16, BitsPerSample))
                        BitsPerSample = Palette[jj]->getBits();
                }
                if ((BitsPerSample < 1) || (BitsPerSample > MAX_TABLE_ENTRY_SIZE))
                {
                    DCMIMAGE_WARN("invalid value for 'BitsPerSample' (" << BitsPerSample
                        << ") computed from color palettes");
                }
                Init();                                                 // create intermediate representation
            }
        }
        else
        {
            ImageStatus = EIS_InvalidValue;
            DCMIMAGE_ERROR("invalid value for 'BitsStored' (" << BitsStored << ") "
                << "... exceeds maximum palette entry size of " << MAX_TABLE_ENTRY_SIZE << " bits");
        }
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiPaletteImage::~DiPaletteImage()
{
    delete Palette[0];
    delete Palette[1];
    delete Palette[2];
}


/*********************************************************************/


void DiPaletteImage::Init()
{
    switch (InputData->getRepresentation())
    {
        case EPR_Uint8:
            if (BitsPerSample <= 8)
                InterData = new DiPalettePixelTemplate<Uint8, Uint32, Uint8>(Document, InputData, Palette, ImageStatus);
            else
                InterData = new DiPalettePixelTemplate<Uint8, Uint32, Uint16>(Document, InputData, Palette, ImageStatus);
            break;
        case EPR_Sint8:
            if (BitsPerSample <= 8)
                InterData = new DiPalettePixelTemplate<Sint8, Sint32, Uint8>(Document, InputData, Palette, ImageStatus);
            else
                InterData = new DiPalettePixelTemplate<Sint8, Sint32, Uint16>(Document, InputData, Palette, ImageStatus);
            break;
        case EPR_Uint16:
            if (BitsPerSample <= 8)
                InterData = new DiPalettePixelTemplate<Uint16, Uint32, Uint8>(Document, InputData, Palette, ImageStatus);
            else
                InterData = new DiPalettePixelTemplate<Uint16, Uint32, Uint16>(Document, InputData, Palette, ImageStatus);
            break;
        case EPR_Sint16:
            if (BitsPerSample <= 8)
                InterData = new DiPalettePixelTemplate<Sint16, Sint32, Uint8>(Document, InputData, Palette, ImageStatus);
            else
                InterData = new DiPalettePixelTemplate<Sint16, Sint32, Uint16>(Document, InputData, Palette, ImageStatus);
            break;
        default:
            DCMIMAGE_WARN("invalid value for inter-representation");
    }
    deleteInputData();
    checkInterData();
}


/*********************************************************************/


int DiPaletteImage::processNextFrames(const unsigned long fcount)
{
    if (DiImage::processNextFrames(fcount))
    {
        delete InterData;
        InterData = NULL;
        Init();
        return (ImageStatus == EIS_Normal);
    }
    return 0;
}
