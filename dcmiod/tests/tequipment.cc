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
 *  Module:  dcmiod
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Tests for the General Equipment Module
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcvr.h"
#include "dcmtk/dcmiod/modequipment.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/oftypes.h"

// Pixel Padding Value (0028,0120) has VR "US or SS"; getPixelPaddingValueVR()
// reports which representation is actually stored, and isPixelPaddingValueSigned()
// is the convenience predicate on top of it.
OFTEST(dcmiod_equipment_pixel_padding_value_vr)
{
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, set " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    IODGeneralEquipmentModule eq;
    DcmEVR vr = EVR_UN;

    // Absent: the VR query fails and "is signed" is false.
    OFCHECK(eq.getPixelPaddingValueVR(vr).bad());
    OFCHECK(!eq.isPixelPaddingValueSigned());

    // Unsigned (US) value.
    OFCHECK(eq.setPixelPaddingValue(OFstatic_cast(Uint16, 7)).good());
    vr = EVR_UN;
    OFCHECK(eq.getPixelPaddingValueVR(vr).good());
    OFCHECK(vr == EVR_US);
    OFCHECK(!eq.isPixelPaddingValueSigned());

    // Signed (SS) value replaces the element; the stored VR is now SS.
    OFCHECK(eq.setPixelPaddingValue(OFstatic_cast(Sint16, -3)).good());
    vr = EVR_UN;
    OFCHECK(eq.getPixelPaddingValueVR(vr).good());
    OFCHECK(vr == EVR_SS);
    OFCHECK(eq.isPixelPaddingValueSigned());
}

// getInstitutionalDepartmentName() must read Institutional Department Name
// (0008,1040), not Station Name (0008,1010).
OFTEST(dcmiod_equipment_institutional_department_name)
{
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, set " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    IODGeneralEquipmentModule eq;
    OFString value;

    OFCHECK(eq.setStationName("STATION").good());
    OFCHECK(eq.setInstutionalDepartmentName("RADIOLOGY").good());

    OFCHECK(eq.getInstitutionalDepartmentName(value).good());
    OFCHECK_EQUAL(value, "RADIOLOGY");
    OFCHECK(eq.getStationName(value).good());
    OFCHECK_EQUAL(value, "STATION");
}
