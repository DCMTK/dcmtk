/*
 *
 *  Copyright (C) 2026, Open Connections GmbH
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
 *  Module:  dcmfg
 *
 *  Purpose: Tests for Real World Value Mapping FG class
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmfg/fgrealworldvaluemapping.h"
#include "dcmtk/ofstd/oftest.h"

// The getters for First and Last Value Mapped must return the values written
// by the corresponding setters, i.e. read (0040,9216)/(0040,9211) and
// (0040,9214)/(0040,9213), respectively.
OFTEST(dcmfg_real_world_value_mapping_first_last_value_mapped)
{
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, set " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    FGRealWorldValueMapping::RWVMItem item;
    Sint32 intValue = 0;
    Float64 floatValue = 0.0;

    OFCHECK(item.setRealWorldValueFirstValueMappedUnsigned(10).good());
    OFCHECK(item.setRealWorldValueLastValueMappedUnsigned(4095).good());
    OFCHECK(item.getRealWorldValueFirstValueMapped(intValue).good());
    OFCHECK_EQUAL(intValue, 10);
    OFCHECK(item.getRealWorldValueLastValueMapped(intValue).good());
    OFCHECK_EQUAL(intValue, 4095);

    // Signed values must be stored with VR SS, not US
    OFCHECK(item.setRealWorldValueFirstValueMappedSigned(-5).good());
    OFCHECK(item.setRealWorldValueLastValueMappedSigned(100).good());
    OFCHECK(item.getRealWorldValueFirstValueMapped(intValue).good());
    OFCHECK_EQUAL(intValue, -5);
    OFCHECK(item.getRealWorldValueLastValueMapped(intValue).good());
    OFCHECK_EQUAL(intValue, 100);
    DcmElement* elem = NULL;
    OFCHECK(item.getData().findAndGetElement(DCM_RealWorldValueFirstValueMapped, elem).good());
    OFCHECK(elem != NULL && elem->getVR() == EVR_SS);

    // Element with VR "xs" from the dictionary, e.g. read from an implicit VR
    // file without Pixel Representation in the same item
    DcmUnsignedShort* us = new DcmUnsignedShort(DCM_RealWorldValueFirstValueMapped);
    OFCHECK(us->putUint16(7).good());
    OFCHECK(item.getData().insert(us, OFTrue /* replace old */).good());
    OFCHECK(item.getRealWorldValueFirstValueMapped(intValue).good());
    OFCHECK_EQUAL(intValue, 7);

    OFCHECK(item.setDoubleFloatRealWorldValueFirstValueMapped(-1.5).good());
    OFCHECK(item.setDoubleFloatRealWorldValueLastValueMapped(2.5).good());
    OFCHECK(item.getDoubleFloatRealWorldValueFirstValueMapped(floatValue).good());
    OFCHECK_EQUAL(floatValue, -1.5);
    OFCHECK(item.getDoubleFloatRealWorldValueLastValueMapped(floatValue).good());
    OFCHECK_EQUAL(floatValue, 2.5);
}
