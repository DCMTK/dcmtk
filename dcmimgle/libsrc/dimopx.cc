/*
 *
 *  Copyright (C) 1996-2000, OFFIS
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomMonochromePixel (Source)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:24:31 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/dimopx.cc,v $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "dimopx.h"
#include "dimomod.h"
#include "diinpx.h"
#include "dimoopx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMonoPixel::DiMonoPixel(const unsigned long count)
  : DiPixel(count),
    Modality(NULL)
{
}


DiMonoPixel::DiMonoPixel(const DiInputPixel *pixel,
                         DiMonoModality *modality)
  : DiPixel((pixel != NULL) ? pixel->getCount() : 0),
    Modality(modality)
{
}


DiMonoPixel::DiMonoPixel(const DiPixel *pixel,
                         DiMonoModality *modality)
  : DiPixel(pixel->getCount()),
    Modality(modality)
{
}


DiMonoPixel::DiMonoPixel(const DiMonoPixel *pixel,
                         const unsigned long count)
  : DiPixel(count),
    Modality(pixel->Modality)
{
    if (Modality != NULL)
        Modality->addReference();
}


DiMonoPixel::DiMonoPixel(const DiMonoOutputPixel *pixel,
                         DiMonoModality *modality)
  : DiPixel(pixel->getCount()),
    Modality(modality)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMonoPixel::~DiMonoPixel()
{
    if (Modality != NULL)
        Modality->removeReference();
}


/*
 *
 * CVS/RCS Log:
 * $Log: dimopx.cc,v $
 * Revision 1.6  2000-03-08 16:24:31  meichel
 * Updated copyright header.
 *
 * Revision 1.5  1999/10/06 13:45:57  joergr
 * Corrected creation of PrintBitmap pixel data: VOI windows should be applied
 * before clipping to avoid that the region outside the image (border) is also
 * windowed (this requires a new method in dcmimgle to create a DicomImage
 * with the grayscale transformations already applied).
 *
 * Revision 1.4  1999/05/31 12:35:59  joergr
 * Corrected bug concerning the conversion of color images to grayscale.
 *
 * Revision 1.3  1999/01/20 14:54:59  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.2  1998/12/22 13:42:58  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.1  1998/11/27 16:16:45  joergr
 * Added copyright message.
 * Introduced new pixel base class.
 *
 * Revision 1.3  1998/05/11 14:52:34  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
