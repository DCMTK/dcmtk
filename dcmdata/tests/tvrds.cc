/*
 *
 *  Copyright (C) 2011, OFFIS e.V.
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
 *  Purpose: test program for class DcmDecimalString
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcvrds.h"
#include "dcmtk/dcmdata/dcdeftag.h"


OFTEST(dcmdata_decimalString_1)
{
    DcmDecimalString decStr(DCM_ContourData);
    OFVector<Float64> doubleVals;
    OFCHECK(decStr.putString("1\\2.0\\3.5\\-4.99\\+500.005\\6.66E-01").good());
    OFCHECK(decStr.getFloat64Vector(doubleVals).good());
    OFCHECK_EQUAL(doubleVals.size(), 6);
    OFCHECK_EQUAL(doubleVals[0], 1);
    OFCHECK_EQUAL(doubleVals[1], 2);
    OFCHECK_EQUAL(doubleVals[2], 3.5);
    OFCHECK_EQUAL(doubleVals[3], -4.99);
    OFCHECK_EQUAL(doubleVals[4], 500.005);
    OFCHECK_EQUAL(doubleVals[5], 0.666);
}

OFTEST(dcmdata_decimalString_2)
{
    DcmDecimalString decStr(DCM_ContourData);
    OFVector<Float64> doubleVals;
    /* insert a NULL byte into the string */
    OFCHECK(decStr.putString("1\\2.0\\3.5\\-4.99\0\\+500.005\\6.66E-01", 34).good());
    OFCHECK(decStr.getFloat64Vector(doubleVals).good());
    OFCHECK_EQUAL(doubleVals.size(), 6);
    OFCHECK_EQUAL(doubleVals[0], 1);
    OFCHECK_EQUAL(doubleVals[1], 2);
    OFCHECK_EQUAL(doubleVals[2], 3.5);
    OFCHECK_EQUAL(doubleVals[3], -4.99);
    OFCHECK_EQUAL(doubleVals[4], 500.005);
    OFCHECK_EQUAL(doubleVals[5], 0.666);
}

OFTEST(dcmdata_decimalString_3)
{
    DcmDecimalString decStr(DCM_ContourData);
    OFVector<Float64> doubleVals;
    /* insert a NULL byte into the string */
    OFCHECK(decStr.putOFStringArray(OFString("1\\2.0\\3.5\\-4.99\0\\+500.005\\6.66E-01", 34)).good());
    OFCHECK(decStr.getFloat64Vector(doubleVals).good());
    OFCHECK_EQUAL(doubleVals.size(), 6);
    OFCHECK_EQUAL(doubleVals[0], 1);
    OFCHECK_EQUAL(doubleVals[1], 2);
    OFCHECK_EQUAL(doubleVals[2], 3.5);
    OFCHECK_EQUAL(doubleVals[3], -4.99);
    OFCHECK_EQUAL(doubleVals[4], 500.005);
    OFCHECK_EQUAL(doubleVals[5], 0.666);
}

OFTEST(dcmdata_decimalString_4)
{
    DcmDecimalString decStr(DCM_ContourData);
    OFVector<Float64> doubleVals;
    OFCHECK(decStr.putString("1\\2.0\\3.5\\-4.99\\+500.005\\6.66E-01\\").good());
    OFCHECK_EQUAL(decStr.getVM(), 7);
    OFCHECK(decStr.putString("\\1\\2.0\\3.5\\-4.99\\+500.005\\6.66E-01").good());
    OFCHECK_EQUAL(decStr.getVM(), 7);
    OFCHECK(decStr.putString("1\\2.0\\3.5\\-4.99\\ \\+500.005\\6.66E-01").good());
    OFCHECK_EQUAL(decStr.getVM(), 7);
    /* insert an invalid value into the string */
    OFCHECK(decStr.putString("1\\2.0\\3.5\\-4.99\\ - \\+500.005\\6.66E-01").good());
    OFCHECK(decStr.getFloat64Vector(doubleVals).bad());
    OFCHECK_EQUAL(doubleVals.size(), 4);
}
