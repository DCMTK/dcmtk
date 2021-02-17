/*
 *
 *  Copyright (C) 2016-2019, OFFIS e.V.
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
 *  Module:  dcmiod
 *
 *  Author:  Jan Schlamelcher
 *
 *  Purpose: Tests for Image Pixel Module functionalities
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmiod/iodimage.h"
#include "dcmtk/dcmiod/modfloatingpointimagepixel.h"
#include "dcmtk/dcmiod/modimagepixel.h"
#include "dcmtk/ofstd/oftest.h"

OFTEST(dcmiod_imagepixel)
{
    DcmIODImage<IODImagePixelModule<Uint8>, IODFloatingPointImagePixelModule> image1;
    DcmIODImage<IODImagePixelModule<Uint8>, IODImagePixelModule<Uint8>, IODDoubleFloatingPointImagePixelModule> image2(
        OFin_place<IODImagePixelModule<Uint8> >);
}
