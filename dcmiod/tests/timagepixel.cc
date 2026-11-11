/*
 *
 *  Copyright (C) 2016-2026, OFFIS e.V.
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

#include "dcmtk/dcmdata/dcdict.h"
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

OFTEST(dcmiod_double_float_pixel_padding)
{
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, set " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    IODDoubleFloatingPointImagePixelModule module;
    Float64 value = 0.0;

    // Double Float Pixel Padding Value (0028,0123) is not set yet
    OFCHECK(module.getDoubleFloatPixelPaddingValue(value).bad());

    // Value written by the setter must be returned by the getter
    OFCHECK(module.setDoubleFloatPixelPaddingValue(-1024.5).good());
    OFCHECK(module.getDoubleFloatPixelPaddingValue(value).good());
    OFCHECK_EQUAL(value, -1024.5);

    // Same for Double Float Pixel Padding Range Limit (0028,0124)
    value = 0.0;
    OFCHECK(module.setDoubleFloatPixelPaddingRangeLimit(-2000.25).good());
    OFCHECK(module.getDoubleFloatPixelPaddingRangeLimit(value).good());
    OFCHECK_EQUAL(value, -2000.25);
}
