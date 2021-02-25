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
 *  Purpose: Utilities (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/ofstd/ofstream.h"

#include "dcmtk/dcmimgle/diutils.h"

// #define INCLUDE_CMATH
// #include "dcmtk/ofstd/ofstdinc.h"

#include <cmath>

/*--------------------*
 *  global variables  *
 *--------------------*/

OFLogger DCM_dcmimgleLogger = OFLog::getLogger("dcmtk.dcmimgle");


/*------------------------*
 *  function definitions  *
 *------------------------*/

unsigned int DicomImageClass::rangeToBits(double minvalue,
                                          double maxvalue)
{
    /* assertion: min < max ! */
    if (minvalue > maxvalue)
    {
        const double temp = minvalue;
        minvalue = maxvalue;
        maxvalue = temp;
    }
    /* signed data? */
    if (minvalue < 0)
    {
        if (fabs(minvalue) > fabs(maxvalue))
            return tobits(OFstatic_cast(unsigned long, fabs(minvalue)), 1) + 1;
        else /* 'minvalue' is negative, 'maxvalue' is positive */
            return tobits(OFstatic_cast(unsigned long, fabs(maxvalue)), 0) + 1;
    }
    return tobits(OFstatic_cast(unsigned long, maxvalue), 0);
}


int DicomImageClass::isRepresentationSigned(EP_Representation repres)
{
    /* determine whether integer representation is signed or unsigned */
    return (repres == EPR_Sint8) || (repres == EPR_Sint16) || (repres == EPR_Sint32);
}


unsigned int DicomImageClass::getRepresentationBits(EP_Representation repres)
{
    unsigned int bits = 0;
    /* determine number of bits for specified representation */
    switch (repres)
    {
        case EPR_Uint8:
        case EPR_Sint8:
            bits = 8;
            break;
        case EPR_Uint16:
        case EPR_Sint16:
            bits = 16;
            break;
        case EPR_Uint32:
        case EPR_Sint32:
            bits = 32;
            break;
    }
    return bits;
}


EP_Representation DicomImageClass::determineRepresentation(double minvalue,
                                                           double maxvalue)
{
    if (minvalue > maxvalue)                        /* assertion: min < max ! */
    {
        const double temp = minvalue;
        minvalue = maxvalue;
        maxvalue = temp;
    }
    if (minvalue < 0)                               /* signed */
    {
        if ((-minvalue <= maxval(7, 0)) && (maxvalue <= maxval(7)))
            return EPR_Sint8;
        if ((-minvalue <= maxval(15, 0)) && (maxvalue <= maxval(15)))
            return EPR_Sint16;
#ifdef DEBUG
        if (-minvalue > maxval(MAX_BITS - 1, 0))
        {
            DCMIMGLE_WARN("minimum pixel value (" << minvalue << ") exceeds signed " << MAX_BITS
                << " bit " << "representation after modality transformation");
        }
        if (maxvalue > maxval(MAX_BITS - 1))
        {
            DCMIMGLE_WARN("maximum pixel value (" << maxvalue << ") exceeds signed " << MAX_BITS
                << " bit " << "representation after modality transformation");
        }
#endif
        return EPR_Sint32;
    }
    if (maxvalue <= maxval(8))
        return EPR_Uint8;
    if (maxvalue <= maxval(16))
        return EPR_Uint16;
#ifdef DEBUG
    if (maxvalue > maxval(MAX_BITS))
    {
        DCMIMGLE_WARN("maximum pixel value (" << maxvalue << ") exceeds unsigned " << MAX_BITS
            << " bit " << "representation after modality transformation");
    }
#endif
    return EPR_Uint32;
}
