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
 *  Purpose: DicomMonoOutputPixel (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/dimoopx.h"
#include "dcmtk/dcmimgle/dimopx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMonoOutputPixel::DiMonoOutputPixel(const DiMonoPixel *pixel,
                                     const unsigned long size,
                                     const unsigned long frame,
                                     const unsigned long max)
  : Count(0),
    FrameSize(size),
    UsedValues(NULL),
    MaxValue(max)
{
    if (pixel != NULL)
    {
        if (pixel->getCount() > frame * size)
            Count = pixel->getCount() - frame * size;       // number of pixels remaining for this 'frame'
    }
    if (Count > FrameSize)
        Count = FrameSize;                                  // cut off at frame 'size'
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMonoOutputPixel::~DiMonoOutputPixel()
{
    delete[] UsedValues;
}


/**********************************/


int DiMonoOutputPixel::isUnused(const unsigned long value)
{
    if (UsedValues == NULL)
        determineUsedValues();                  // create on demand
    if (UsedValues != NULL)
    {
        if (value <= MaxValue)
            return OFstatic_cast(int, UsedValues[value] == 0);
        return 2;                               // out of range
    }
    return 0;
}
