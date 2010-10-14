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
 *  Purpose: DicomRegister (Source)
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

#include "dcmtk/dcmimage/diregist.h"
#include "dcmtk/dcmimage/dipalimg.h"
#include "dcmtk/dcmimage/dirgbimg.h"
#include "dcmtk/dcmimage/dihsvimg.h"
#include "dcmtk/dcmimage/diargimg.h"
#include "dcmtk/dcmimage/dicmyimg.h"
#include "dcmtk/dcmimage/diybrimg.h"
#include "dcmtk/dcmimage/diyf2img.h"
#include "dcmtk/dcmimage/diyp2img.h"
#include "dcmtk/dcmimage/dicomot.h"
#include "dcmtk/dcmimgle/didocu.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiRegister::DiRegister()
{
    DiRegisterBase::Pointer = this;
}


/*--------------*
 *  destructor  *
 *--------------*/

DiRegister::~DiRegister()
{
}


/*--------------*/

DiImage *DiRegister::createImage(const DiDocument *docu,
                                 const EI_Status status,
                                 const EP_Interpretation photo)
{
    DiImage *image = NULL;
    switch (photo)
    {
        case EPI_PaletteColor:
            image = new DiPaletteImage(docu, status);
            break;
        case EPI_RGB:
            image = new DiRGBImage(docu, status);
            break;
        case EPI_HSV:
            image = new DiHSVImage(docu, status);
            break;
        case EPI_ARGB:
            image = new DiARGBImage(docu, status);
            break;
        case EPI_CMYK:
            image = new DiCMYKImage(docu, status);
            break;
        case EPI_YBR_Full:
            image = new DiYBRImage(docu, status);
            break;
        case EPI_YBR_Full_422:
            image = new DiYBR422Image(docu, status);
            break;
        case EPI_YBR_Partial_422:
            image = new DiYBRPart422Image(docu, status);
            break;
        default:
            ;
    }
    return image;
}


DiMonoPixel *DiRegister::createMonoImageData(const DiColorImage *image,
                                             const double red,
                                             const double green,
                                             const double blue)
{
    DiMonoPixel *inter = NULL;
    if (image != NULL)
    {
        const DiColorPixel *color = image->getColorInterData();
        if (color != NULL)
        {
            DiMonoModality *modality = new DiMonoModality(image->getBits());
            if (modality != NULL)
            {
                switch (color->getRepresentation())
                {
                    case EPR_Uint8:
                        inter = new DiColorMonoTemplate<Uint8>(color, modality, red, green, blue);
                        break;
                    case EPR_Sint8:
                        inter = new DiColorMonoTemplate<Sint8>(color, modality, red, green, blue);
                        break;
                    case EPR_Uint16:
                        inter = new DiColorMonoTemplate<Uint16>(color, modality, red, green, blue);
                        break;
                    case EPR_Sint16:
                        inter = new DiColorMonoTemplate<Sint16>(color, modality, red, green, blue);
                        break;
                    case EPR_Uint32:
                        inter = new DiColorMonoTemplate<Uint32>(color, modality, red, green, blue);
                        break;
                    case EPR_Sint32:
                        inter = new DiColorMonoTemplate<Sint32>(color, modality, red, green, blue);
                        break;
                }
            }
        }
    }
    return inter;
}


/*
 *
 * CVS/RCS Log:
 * $Log: diregist.cc,v $
 * Revision 1.10  2010-10-14 13:14:15  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.9  2005-12-08 15:42:34  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.8  2004/07/20 18:13:50  joergr
 * Added API method to "officially" access the internal intermediate pixel data
 * representation (e.g. to get Hounsfield Units for CT images).
 *
 * Revision 1.7  2003/12/23 10:54:28  joergr
 * Updated copyright header.
 *
 * Revision 1.6  2001/06/01 15:49:36  meichel
 * Updated copyright header
 *
 * Revision 1.5  2000/03/08 16:21:57  meichel
 * Updated copyright header.
 *
 * Revision 1.4  1999/05/31 13:01:42  joergr
 * Corrected bug concerning the conversion of color images to grayscale.
 *
 * Revision 1.3  1999/04/28 13:43:47  joergr
 * Added some checks to avoid dereferencing null pointers.
 *
 * Revision 1.2  1999/03/24 17:12:38  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.1  1998/11/27 13:05:59  joergr
 * Added registration class to allow easy combination of both modules.
 * Added copyright message to all source files.
 *
 *
 */
