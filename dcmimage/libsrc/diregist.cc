/*
 *
 *  Copyright (C) 1998-99, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomRegister (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date $
 *  Source File:      $Source $
 *  CVS/RCS Revision: $Revision $
 *  Status:           $State $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "diregist.h"
#include "dipalimg.h"
#include "dirgbimg.h"
#include "dihsvimg.h"
#include "diargimg.h"
#include "dicmyimg.h"
#include "diybrimg.h"
#include "diyf2img.h"
#include "diyp2img.h"
#include "dicomot.h"
#include "didocu.h"


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
        const DiColorPixel *color = image->getInterData();
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
 * Revision 1.4  1999-05-31 13:01:42  joergr
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
