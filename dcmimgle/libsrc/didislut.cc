/*
 *
 *  Copyright (C) 1996-2021, OFFIS e.V.
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
 *  Purpose: DicomDisplayLUT (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/didislut.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiDisplayLUT::DiDisplayLUT(const unsigned long count,
                           const Uint16 max,
                           const double amb,
                           const double illum)
  : DiBaseLUT(count, OFstatic_cast(Uint16, DicomImageClass::tobits(max, 0))),
    AmbientLight((amb > 0) ? amb : 0),
    Illumination((illum > 0) ? illum : 0)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiDisplayLUT::~DiDisplayLUT()
{
}
