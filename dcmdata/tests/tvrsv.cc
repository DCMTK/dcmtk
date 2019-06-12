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
 *  Purpose: test program for class DcmSigned64bitVeryLong
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcvrsv.h"
#include "dcmtk/dcmdata/dctagkey.h"


OFTEST(dcmdata_signed64bitVeryLong)
{
    Sint64 value;
    DcmSigned64bitVeryLong veryLong(DcmTagKey(0x1111, 0x1111));
    /* append new values */
    OFCHECK(veryLong.putSint64(-2222, 0).good());
    OFCHECK(veryLong.putSint64(-11, 1).good());
    OFCHECK(veryLong.putSint64(0, 2).good());
    OFCHECK(veryLong.putSint64(11, 3).good());
    OFCHECK(veryLong.putSint64(2222, 4).good());
    /* check number of values */
    OFCHECK_EQUAL(veryLong.getVM(), 5);
    /* check all set values */
    OFCHECK(veryLong.getSint64(value, 0).good());
    OFCHECK_EQUAL(value, -2222);
    OFCHECK(veryLong.getSint64(value, 1).good());
    OFCHECK_EQUAL(value, -11);
    OFCHECK(veryLong.getSint64(value, 2).good());
    OFCHECK_EQUAL(value, 0);
    OFCHECK(veryLong.getSint64(value, 3).good());
    OFCHECK_EQUAL(value, 11);
    OFCHECK(veryLong.getSint64(value, 4).good());
    OFCHECK_EQUAL(value, 2222);
    /* change existing values */
    OFCHECK(veryLong.putSint64(-1111, 1).good());
    OFCHECK(veryLong.putSint64(1111, 3).good());
    /* check again all set values */
    OFCHECK(veryLong.getSint64(value, 0).good());
    OFCHECK_EQUAL(value, -2222);
    OFCHECK(veryLong.getSint64(value, 1).good());
    OFCHECK_EQUAL(value, -1111);
    OFCHECK(veryLong.getSint64(value, 2).good());
    OFCHECK_EQUAL(value, 0);
    OFCHECK(veryLong.getSint64(value, 3).good());
    OFCHECK_EQUAL(value, 1111);
    OFCHECK(veryLong.getSint64(value, 4).good());
    OFCHECK_EQUAL(value, 2222);
}
