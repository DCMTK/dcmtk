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

#include "dctmacro.h"

#define ENDIAN_UINT16(w) LITTLE_ENDIAN_UINT16(w)
#define ENDIAN_UINT32(w) LITTLE_ENDIAN_UINT32(w)


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


OFTEST(dcmdata_handleAmbiguousVR_fromDictionary)
{
    /* enable setting VR from data dictionary */
    dcmPreferVRFromDataDictionary.set(true);
    /* create wave data for OW */
    const Uint8 data[] = {
        TAG_AND_LENGTH_SHORT(DCM_WaveformBitsAllocated, 'U', 'S', 2),
        '\x00', '\x10',
        TAG_AND_LENGTH(DCM_WaveformData, 'O', 'W', 8),
        '\x00', '\x01', '\x00', '\x02', '\x00', '\x03', '\x00', '\x04',
    };
    DcmDataset dset;
    OFCHECK(readDataset(dset, data, sizeof data, EXS_LittleEndianExplicit).good());
    const Uint16 *uintVals = nullptr;
    unsigned long numUints = 0;
    /* check that the data can be accessed as a 16-bit array (regression test, used to fail with EC_IllegalCall) */
    OFCHECK(dset.getRootItem()->findAndGetUint16Array(DCM_WaveformData, uintVals, &numUints).good());
    OFCHECK_EQUAL(numUints, 4);
}
