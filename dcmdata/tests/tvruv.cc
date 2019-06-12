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
 *  Purpose: test program for class DcmUnsigned64bitVeryLong
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcvruv.h"
#include "dcmtk/dcmdata/dctagkey.h"


OFTEST(dcmdata_unsigned64bitVeryLong)
{
    Uint64 value;
    DcmUnsigned64bitVeryLong veryLong(DcmTagKey(0x1111, 0x1111));
    /* append new values */
    OFCHECK(veryLong.putUint64(1, 0).good());
    OFCHECK(veryLong.putUint64(22, 1).good());
    OFCHECK(veryLong.putUint64(333, 2).good());
    OFCHECK(veryLong.putUint64(4444, 3).good());
    OFCHECK(veryLong.putUint64(55555, 5).bad());
    /* check number of values */
    OFCHECK_EQUAL(veryLong.getVM(), 4);
    /* check all set values */
    OFCHECK(veryLong.getUint64(value, 0).good());
    OFCHECK_EQUAL(value, 1);
    OFCHECK(veryLong.getUint64(value, 1).good());
    OFCHECK_EQUAL(value, 22);
    OFCHECK(veryLong.getUint64(value, 2).good());
    OFCHECK_EQUAL(value, 333);
    OFCHECK(veryLong.getUint64(value, 3).good());
    OFCHECK_EQUAL(value, 4444);
    /* change existing values */
    OFCHECK(veryLong.putUint64(2222, 1).good());
    OFCHECK(veryLong.putUint64(3333, 2).good());
    /* check again all set values */
    OFCHECK(veryLong.getUint64(value, 0).good());
    OFCHECK_EQUAL(value, 1);
    OFCHECK(veryLong.getUint64(value, 1).good());
    OFCHECK_EQUAL(value, 2222);
    OFCHECK(veryLong.getUint64(value, 2).good());
    OFCHECK_EQUAL(value, 3333);
    OFCHECK(veryLong.getUint64(value, 3).good());
    OFCHECK_EQUAL(value, 4444);
}
