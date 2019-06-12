/*
 *
 *  Copyright (C) 2019, OFFIS e.V.
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
 *  Purpose: test program for class DcmOther64bitVeryLong
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcvrov.h"
#include "dcmtk/dcmdata/dcdeftag.h"


OFTEST(dcmdata_other64bitVeryLong)
{
    DcmOther64bitVeryLong veryLong(DCM_ExtendedOffsetTable);
    /* append new values */
    OFCHECK(veryLong.putUint64(1, 0).good());
    OFCHECK(veryLong.putUint64(22, 1).good());
    OFCHECK(veryLong.putUint64(333, 2).good());
    OFCHECK(veryLong.putUint64(4444, 3).good());
    OFCHECK(veryLong.putUint64(55555, 4).good());
    OFCHECK(veryLong.putUint64(666666, 5).good());
    OFCHECK(veryLong.putUint64(7777777, 6).good());
    OFCHECK(veryLong.putUint64(88888888, 7).good());
    OFCHECK(veryLong.putUint64(999999999, 8).good());
    OFCHECK(veryLong.putUint64(1111111111, 9).good());
    OFCHECK(veryLong.putUint64(22222222222UL, 10).good());
    /* value multiplicity should always be 1 */
    OFCHECK_EQUAL(veryLong.getVM(), 1);
    /* number of values should be higher */
    OFCHECK_EQUAL(veryLong.getNumberOfValues(), 11);
}
