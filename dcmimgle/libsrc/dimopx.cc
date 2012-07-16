/*
 *
 *  Copyright (C) 1996-2011, OFFIS e.V.
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


/*********************************************************************/


DiMonoModality *DiMonoPixel::addReferenceToModality()
{
    if (Modality != NULL)
        Modality->addReference();
    return Modality;
}
