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
 *  Purpose: DicomInputPixel (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiInputPixel::DiInputPixel(const unsigned int bits,
                           const unsigned long first,
                           const unsigned long number,
                           const unsigned long fsize)
  : Count(0),
    Bits(bits),
    FirstFrame(first),
    NumberOfFrames(number),
    FrameSize(fsize),
    PixelStart(first * fsize),
    PixelCount(number * fsize),
    ComputedCount(number * fsize),
    AbsMinimum(0),
    AbsMaximum(0)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiInputPixel::~DiInputPixel()
{
}
