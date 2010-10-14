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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomMonochromePixel (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:18 $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/dimopx.h"
#include "dcmtk/dcmimgle/dimomod.h"
#include "dcmtk/dcmimgle/diinpx.h"
#include "dcmtk/dcmimgle/dimoopx.h"


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
  : DiPixel((pixel != NULL) ? pixel->getComputedCount() : 0,
            (pixel != NULL) ? pixel->getPixelCount() : 0),
    Modality(modality)
{
}


DiMonoPixel::DiMonoPixel(const DiPixel *pixel,
                         DiMonoModality *modality)
  : DiPixel(pixel->getCount(), pixel->getInputCount()),
    Modality(modality)
{
}


DiMonoPixel::DiMonoPixel(const DiMonoPixel *pixel,
                         const unsigned long count)
  : DiPixel(count, pixel->getInputCount()),
    Modality(pixel->Modality)
{
    if (Modality != NULL)
        Modality->addReference();
}


DiMonoPixel::DiMonoPixel(DiMonoOutputPixel *pixel,
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
 * Revision 1.13  2010-10-14 13:14:18  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.12  2005/12/08 15:43:02  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.11  2004/02/06 11:10:39  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.10  2003/12/08 17:43:04  joergr
 * Updated copyright header.
 *
 * Revision 1.9  2002/06/26 16:13:25  joergr
 * Enhanced handling of corrupted pixel data and/or length.
 *
 * Revision 1.8  2001/06/01 15:49:58  meichel
 * Updated copyright header
 *
 * Revision 1.7  2000/04/27 13:10:31  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.6  2000/03/08 16:24:31  meichel
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
