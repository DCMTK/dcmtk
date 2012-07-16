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
 *  Purpose: DicomColorPixel (Source)
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcdeftag.h"

#include "dcmtk/dcmimage/dicopx.h"
#include "dcmtk/dcmimage/diqttype.h"
#include "dcmtk/dcmimgle/dimopx.h"
#include "dcmtk/dcmimgle/diinpx.h"
#include "dcmtk/dcmimgle/didocu.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiColorPixel::DiColorPixel(const DiDocument *docu,
                           const DiInputPixel *pixel,
                           const Uint16 samples,
                           EI_Status &status,
                           const Uint16 sample_rate)
  : DiPixel(0),
    PlanarConfiguration(0)
{
    if (docu != NULL)
    {
        Uint16 us = 0;
        if (docu->getValue(DCM_SamplesPerPixel, us))
        {
            if (us != samples)
            {
                DCMIMAGE_WARN("invalid value for 'SamplesPerPixel' (" << us
                    << ") ... assuming " << samples);
            }
            if (docu->getValue(DCM_PlanarConfiguration, us))
            {
                /* only use Planar Configuration attribute if there are multiple planes */
                if (samples > 1)
                {
                    PlanarConfiguration = (us == 1);
                    if ((us != 0) && (us != 1))
                    {
                        DCMIMAGE_WARN("invalid value for 'PlanarConfiguration' (" << us
                            << ") ... assuming 'color-by-pixel' (0)");
                    }
                } else {
                    DCMIMAGE_WARN("unexpected attribute 'PlanarConfiguration' (" << us
                        << ") ... ignoring");
                }
            }
            else if (samples > 1)
            {
                status = EIS_MissingAttribute;
                DCMIMAGE_ERROR("mandatory attribute 'PlanarConfiguration' is missing");
                return;
            }
            if (pixel != NULL)
            {
                // number of pixels (per plane) computed from the length of the PixelData attribute
                InputCount = pixel->getPixelCount() / ((sample_rate == 0) ? samples : sample_rate);
                // number of pixels allocated for the intermediate buffer
                Count = pixel->getComputedCount() / ((sample_rate == 0) ? samples : sample_rate);
            }
        } else {
            status = EIS_MissingAttribute;
            DCMIMAGE_ERROR("mandatory attribute 'SamplesPerPixel' is missing");
        }
    }
}

DiColorPixel::DiColorPixel(const DiColorPixel *pixel,
                           const unsigned long count)
  : DiPixel(count, pixel->InputCount),
    PlanarConfiguration(pixel->PlanarConfiguration)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiColorPixel::~DiColorPixel()
{
}
