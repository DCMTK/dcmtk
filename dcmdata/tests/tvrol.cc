/*
 *
 *  Copyright (C) 2016-2018, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: test program for class DcmOtherLong
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcvrol.h"
#include "dcmtk/dcmdata/dcdeftag.h"


OFTEST(dcmdata_otherLong)
{
    DcmOtherLong otherLong(DCM_TrackPointIndexList);
    /* append new values */
    OFCHECK(otherLong.putUint32(1, 0).good());
    OFCHECK(otherLong.putUint32(22, 1).good());
    OFCHECK(otherLong.putUint32(333, 2).good());
    OFCHECK(otherLong.putUint32(4444, 3).good());
    OFCHECK(otherLong.putUint32(55555, 4).good());
    OFCHECK(otherLong.putUint32(666666, 5).good());
    OFCHECK(otherLong.putUint32(7777777, 6).good());
    OFCHECK(otherLong.putUint32(88888888, 7).good());
    OFCHECK(otherLong.putUint32(999999999, 8).good());
    /* value multiplicity should always be 1 */
    OFCHECK_EQUAL(otherLong.getVM(), 1);
    /* number of values should be higher */
    OFCHECK_EQUAL(otherLong.getNumberOfValues(), 9);
}
