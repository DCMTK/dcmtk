/*
 *
 *  Copyright (C) 2011-2020, OFFIS e.V.
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
#include "dcmtk/dcmdata/dcitem.h"


OFTEST(dcmdata_decimalString_1)
{
    DcmDecimalString decStr(DCM_ContourData, EVR_DS);
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
    DcmDecimalString decStr(DCM_ContourData, EVR_DS);
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
    DcmDecimalString decStr(DCM_ContourData, EVR_DS);
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
    DcmDecimalString decStr(DCM_ContourData, EVR_DS);
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


OFTEST(dcmdata_decimalString_putFloat64)
{
    // Test insertion in the beginning
    OFString testStr;
    DcmDecimalString decStr(DCM_ContourData, EVR_DS);
    OFCHECK(decStr.putFloat64(0, 0).good());
    decStr.getOFStringArray(testStr);
    OFCHECK(testStr == "0");

    // Test insertion at the end (append)
    OFCHECK(decStr.putFloat64(0.1, 1).good());
    decStr.getOFStringArray(testStr);
    OFCHECK(testStr == "0\\0.1");

    // Test insertion at the end (sparse values in the middle)
    OFCHECK(decStr.putFloat64(0.5, 5).good());
    decStr.getOFStringArray(testStr);
    OFCHECK(testStr == "0\\0.1\\\\\\\\0.5");

    // Test insertion in the middle
    OFCHECK(decStr.putFloat64(0.3, 3).good());
    decStr.getOFStringArray(testStr);
    OFCHECK(testStr == "0\\0.1\\\\0.3\\\\0.5");

    // Test insertion of a long float value (maximum precision)
    OFCHECK(decStr.putFloat64(0.12345678901234,0).good());
    decStr.getOFStringArray(testStr);
    OFCHECK(testStr == "0.123457\\0.1\\\\0.3\\\\0.5");

    // Test insertion of long integer value
    decStr.clear();
    OFCHECK(decStr.putFloat64(1234567890123456.0,0).good());
    decStr.getOFStringArray(testStr);
    OFCHECK(testStr == "1234567890123456");

    // Test insertion of (max) long negative integer value
    decStr.clear();
    OFCHECK(decStr.putFloat64(-123456789012345.0,0).good());
    decStr.getOFStringArray(testStr);
    OFCHECK(testStr == "-123456789012345");

    // Error: Try to insert integers longer than 16 bytes
    // Test insertion of long integer value
    decStr.clear();
    OFCHECK(decStr.putFloat64(12345678901234567.0,0).bad());

    // Test insertion of (max) long negative integer value
    decStr.clear();
    OFCHECK(decStr.putFloat64(-1234567890123456.0,0).bad());

    // Test insertion of a long float value (maximum precision)
    decStr.clear();
    OFCHECK(decStr.putFloat64Prec(0.12345678901234, 0, 14).good());
    decStr.getOFStringArray(testStr);
    OFCHECK(testStr == "0.12345678901234");

    // Error: Test insertion of a long float value (> maximum precision which would
    // result in a DS value > 16 chars.
    decStr.clear();
    OFCHECK(decStr.putFloat64Prec(0.123456789012345, 0, 15).bad());
    decStr.getOFStringArray(testStr);

    // Test whether trailing zeroes are kept if desired
    decStr.clear();
    OFCHECK(decStr.putFloat64Prec(0.123400, 0, 6, OFFalse).good());
    decStr.getOFStringArray(testStr);
    OFCHECK(testStr == "0.123400");

    // Check DcmItem::putFloat64() for Decimal Strings
    DcmItem item;
    OFCHECK(item.putAndInsertFloat64(DcmTag(DCM_ContourData, EVR_DS), 0.1).good());
    OFCHECK(item.findAndGetOFStringArray(DCM_ContourData, testStr).good());
    OFCHECK(testStr == "0.1");
}
