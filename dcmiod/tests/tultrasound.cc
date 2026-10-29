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
 *  Purpose: Tests for getters of the ultrasound modules
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmiod/modenhusimage.h"
#include "dcmtk/dcmiod/modusfor.h"
#include "dcmtk/ofstd/oftest.h"

// getVolumetoTransducerRelationship() must read Volume to Transducer
// Relationship (0020,9309), not Ultrasound Acquisition Geometry (0020,9307).
OFTEST(dcmiod_usfor_volume_to_transducer_relationship)
{
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, set " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    IODUSFoRModule mod;
    OFString value;

    OFCHECK(mod.setUltrasoundAcquisitionGeometry("APEX").good());
    OFCHECK(mod.setVolumetoTransducerRelationship("FIXED").good());

    OFCHECK(mod.getVolumetoTransducerRelationship(value).good());
    OFCHECK_EQUAL(value, "FIXED");
    OFCHECK(mod.getUltrasoundAcquisitionGeometry(value).good());
    OFCHECK_EQUAL(value, "APEX");
}

// getRescaleIntercept() must read Rescale Intercept (0028,1052),
// not Rescale Slope (0028,1053).
OFTEST(dcmiod_enhus_rescale_intercept)
{
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, set " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    OFshared_ptr<DcmItem> item(new DcmItem());
    OFCHECK(item->putAndInsertOFStringArray(DCM_RescaleSlope, "1").good());
    OFCHECK(item->putAndInsertOFStringArray(DCM_RescaleIntercept, "0").good());

    IODEnhUSImageModule mod(item, OFshared_ptr<IODRules>(new IODRules()));
    OFString value;

    OFCHECK(mod.getRescaleIntercept(value).good());
    OFCHECK_EQUAL(value, "0");
}
