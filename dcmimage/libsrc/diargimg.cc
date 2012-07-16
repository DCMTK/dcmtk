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
 *  Purpose: DiARGBImage (Source) - UNTESTED !!!
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcdeftag.h"

#include "dcmtk/dcmimage/diargimg.h"
#include "dcmtk/dcmimage/diargpxt.h"
#include "dcmtk/dcmimage/diqttype.h"
#include "dcmtk/dcmimage/dilogger.h"
#include "dcmtk/dcmimgle/diluptab.h"
#include "dcmtk/dcmimgle/diinpx.h"
#include "dcmtk/dcmimgle/didocu.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiARGBImage::DiARGBImage(const DiDocument *docu,
                         const EI_Status status)
  : DiColorImage(docu, status, 4),
    Palette()  // initializes the three color palettes to NULL
{
    if ((Document != NULL) && (InputData != NULL) && (ImageStatus == EIS_Normal))
    {
        if (BitsStored <= MAX_TABLE_ENTRY_SIZE)                         // color depth <= 16
        {
            const EL_BitsPerTableEntry descMode = (Document->getFlags() & CIF_CheckLutBitDepth) ? ELM_CheckValue : ELM_UseValue;
            Palette[0] = new DiLookupTable(Document, DCM_RedPaletteColorLookupTableDescriptor,
                DCM_RedPaletteColorLookupTableData, DCM_UndefinedTagKey, descMode, &ImageStatus);
            Palette[1] = new DiLookupTable(Document, DCM_GreenPaletteColorLookupTableDescriptor,
                DCM_GreenPaletteColorLookupTableData, DCM_UndefinedTagKey, descMode, &ImageStatus);
            Palette[2] = new DiLookupTable(Document, DCM_BluePaletteColorLookupTableDescriptor,
                DCM_BluePaletteColorLookupTableData, DCM_UndefinedTagKey, descMode, &ImageStatus);
            if ((ImageStatus == EIS_Normal) && (Palette[0] != NULL) && (Palette[1] != NULL) && (Palette[2] != NULL))
            {
                BitsPerSample = BitsStored;
                for (int jj = 0; jj < 3; jj++)                          // determine maximum bit count
                {
                    if (Palette[jj]->getBits() > OFstatic_cast(Uint16, BitsPerSample))
                        BitsPerSample = Palette[jj]->getBits();
                }
                Init();                                                 // create intermediate representation
            }
        }
        else                                                            // color depth > 16
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

DiARGBImage::~DiARGBImage()
{
    delete Palette[0];
    delete Palette[1];
    delete Palette[2];
}


/*********************************************************************/


void DiARGBImage::Init()
{
    /* number of pixels per plane */
    const unsigned long planeSize = OFstatic_cast(unsigned long, Columns) * OFstatic_cast(unsigned long, Rows);
    switch (InputData->getRepresentation())
    {
        case EPR_Uint8:
            if (BitsPerSample <= 8)
                InterData = new DiARGBPixelTemplate<Uint8, Uint32, Uint8>(Document, InputData, Palette, ImageStatus,
                    planeSize, BitsStored);
            else
                InterData = new DiARGBPixelTemplate<Uint8, Uint32, Uint16>(Document, InputData, Palette, ImageStatus,
                    planeSize, BitsStored);
            break;
        case EPR_Sint8:
            if (BitsPerSample <= 8)
                InterData = new DiARGBPixelTemplate<Sint8, Sint32, Uint8>(Document, InputData, Palette, ImageStatus,
                    planeSize, BitsStored);
            else
                InterData = new DiARGBPixelTemplate<Sint8, Sint32, Uint16>(Document, InputData, Palette, ImageStatus,
                    planeSize, BitsStored);
            break;
        case EPR_Uint16:
            if (BitsPerSample <= 8)
                InterData = new DiARGBPixelTemplate<Uint16, Uint32, Uint8>(Document, InputData, Palette, ImageStatus,
                    planeSize, BitsStored);
            else
                InterData = new DiARGBPixelTemplate<Uint16, Uint32, Uint16>(Document, InputData, Palette, ImageStatus,
                    planeSize, BitsStored);
            break;
        case EPR_Sint16:
            if (BitsPerSample <= 8)
                InterData = new DiARGBPixelTemplate<Sint16, Sint32, Uint8>(Document, InputData, Palette, ImageStatus,
                    planeSize, BitsStored);
            else
                InterData = new DiARGBPixelTemplate<Sint16, Sint32, Uint16>(Document, InputData, Palette, ImageStatus,
                    planeSize, BitsStored);
            break;
        default:
            DCMIMAGE_WARN("invalid value for inter-representation");
    }
    deleteInputData();                          // input data is no longer needed
    checkInterData();
}


/*********************************************************************/


int DiARGBImage::processNextFrames(const unsigned long fcount)
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
