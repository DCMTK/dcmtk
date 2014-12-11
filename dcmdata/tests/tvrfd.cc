/*
 *
 *  Copyright (C) 2014, OFFIS e.V.
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
 *  Purpose: test program for class DcmFloatingPointDouble
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcvrfd.h"
#include "dcmtk/dcmdata/dcdeftag.h"


OFTEST(dcmdata_floatingPointDouble)
{
    Float64 value;
    DcmFloatingPointDouble floatDouble(DCM_InversionTimes);
    /* append new values */
    OFCHECK(floatDouble.putFloat64(1.1, 0).good());
    OFCHECK(floatDouble.putFloat64(2.2, 1).good());
    OFCHECK(floatDouble.putFloat64(3.3, 2).good());
    OFCHECK(floatDouble.putFloat64(4.4, 3).good());
    OFCHECK(floatDouble.putFloat64(5.5, 5).bad());
    /* check number of values */
    OFCHECK_EQUAL(floatDouble.getVM(), 4);
    /* check all set values */
    OFCHECK(floatDouble.getFloat64(value, 0).good());
    OFCHECK_EQUAL(value, 1.1);
    OFCHECK(floatDouble.getFloat64(value, 1).good());
    OFCHECK_EQUAL(value, 2.2);
    OFCHECK(floatDouble.getFloat64(value, 2).good());
    OFCHECK_EQUAL(value, 3.3);
    OFCHECK(floatDouble.getFloat64(value, 3).good());
    OFCHECK_EQUAL(value, 4.4);
    /* change existing values */
    OFCHECK(floatDouble.putFloat64(22.22, 1).good());
    OFCHECK(floatDouble.putFloat64(33.33, 2).good());
    /* check again all set values */
    OFCHECK(floatDouble.getFloat64(value, 0).good());
    OFCHECK_EQUAL(value, 1.1);
    OFCHECK(floatDouble.getFloat64(value, 1).good());
    OFCHECK_EQUAL(value, 22.22);
    OFCHECK(floatDouble.getFloat64(value, 2).good());
    OFCHECK_EQUAL(value, 33.33);
    OFCHECK(floatDouble.getFloat64(value, 3).good());
    OFCHECK_EQUAL(value, 4.4);
}
