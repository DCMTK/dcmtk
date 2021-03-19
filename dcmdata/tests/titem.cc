/*
 *
 *  Copyright (C) 2021, OFFIS e.V.
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
 *  Purpose: test program for class DcmItem
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcvrat.h"
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmdata/dcdeftag.h"


OFTEST(dcmdata_findAndGetUint16Array)
{
    DcmItem item;
    /* create data elements with values */
    DcmUnsignedShort usValue(DCM_SynchronizationChannel);
    DcmAttributeTag atValue(DCM_FrameIncrementPointer);
    usValue.putString("1\\2");
    atValue.putTagVal(DCM_FrameTime);
    /* add elements to item */
    OFCHECK(item.insert(new DcmUnsignedShort(usValue)).good());
    OFCHECK(item.insert(new DcmAttributeTag(atValue)).good());
    /* and check the number of 16-bit values stored */
    const Uint16 *uintVals = NULL;
    unsigned long numUints = 0;
    OFCHECK(item.findAndGetUint16Array(DCM_SynchronizationChannel, uintVals, &numUints).good());
    OFCHECK_EQUAL(numUints, 2);
    OFCHECK(item.findAndGetUint16Array(DCM_FrameIncrementPointer, uintVals, &numUints).good());
    OFCHECK_EQUAL(numUints, 2);
}
