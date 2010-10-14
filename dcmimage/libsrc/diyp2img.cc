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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:15 $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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


/*
 *
 * CVS/RCS Log:
 * $Log: diyp2img.cc,v $
 * Revision 1.10  2010-10-14 13:14:15  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.9  2009-11-25 14:48:46  joergr
 * Adapted code for new approach to access individual frames of a DICOM image.
 *
 * Revision 1.8  2005/12/08 15:42:38  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.7  2003/12/23 10:54:28  joergr
 * Updated copyright header.
 *
 * Revision 1.6  2001/06/01 15:49:37  meichel
 * Updated copyright header
 *
 * Revision 1.5  2000/03/08 16:21:58  meichel
 * Updated copyright header.
 *
 * Revision 1.4  1999/04/28 13:45:30  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.3  1998/11/27 14:35:30  joergr
 * Added copyright message.
 * Renamed variable 'Status' to 'ImageStatus' because of possible conflicts
 * with X windows systems.
 *
 * Revision 1.2  1998/05/11 14:52:40  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
