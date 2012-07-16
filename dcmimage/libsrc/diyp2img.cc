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
 *  Purpose: DicomYBRPart422Image (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctypes.h"

#include "dcmtk/dcmimage/diyp2img.h"
#include "dcmtk/dcmimage/diyp2pxt.h"
#include "dcmtk/dcmimage/dilogger.h"
#include "dcmtk/dcmimgle/diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiYBRPart422Image::DiYBRPart422Image(const DiDocument *docu,
                                     const EI_Status status)
  : DiColorImage(docu, status, 2)
{
    if ((Document != NULL) && (InputData != NULL) && (ImageStatus == EIS_Normal))
    {
        Init();                                                 // create intermediate representation
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiYBRPart422Image::~DiYBRPart422Image()
{
}


/*********************************************************************/


void DiYBRPart422Image::Init()
{
    switch (InputData->getRepresentation())
    {
        case EPR_Uint8:
            InterData = new DiYBRPart422PixelTemplate<Uint8, Uint8>(Document, InputData, ImageStatus, BitsPerSample);
            break;
        case EPR_Sint8:
            InterData = new DiYBRPart422PixelTemplate<Sint8, Uint8>(Document, InputData, ImageStatus, BitsPerSample);
            break;
        case EPR_Uint16:
            InterData = new DiYBRPart422PixelTemplate<Uint16, Uint16>(Document, InputData, ImageStatus, BitsPerSample);
            break;
        case EPR_Sint16:
            InterData = new DiYBRPart422PixelTemplate<Sint16, Uint16>(Document, InputData, ImageStatus, BitsPerSample);
            break;
        case EPR_Uint32:
            InterData = new DiYBRPart422PixelTemplate<Uint32, Uint32>(Document, InputData, ImageStatus, BitsPerSample);
            break;
        case EPR_Sint32:
            InterData = new DiYBRPart422PixelTemplate<Sint32, Uint32>(Document, InputData, ImageStatus, BitsPerSample);
            break;
    }
    deleteInputData();
    checkInterData();
}


/*********************************************************************/


int DiYBRPart422Image::processNextFrames(const unsigned long fcount)
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
