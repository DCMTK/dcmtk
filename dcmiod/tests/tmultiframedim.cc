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
 *  Purpose: Tests for the Multi-frame Dimension Module
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmiod/modmultiframedimension.h"


OFTEST(dcmiod_multiframe_dimension_add_index_errors)
{
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    IODMultiframeDimensionModule dims;

    // Invalid Dimension Organization UID: Nothing must be added
    OFCHECK(dims.addDimensionIndex(DCM_ImagePositionPatient, "abc", DCM_PlanePositionSequence).bad());
    OFCHECK(dims.getDimensionOrganizationSequence().empty());
    OFCHECK(dims.getDimensionIndexSequence().empty());

    // Invalid Dimension Index Pointer: The (already deleted) Dimension Index
    // item must not be added to the Dimension Index Sequence
    char uid[100];
    dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT);
    OFCHECK(dims.addDimensionIndex(DCM_UndefinedTagKey, uid, DCM_PlanePositionSequence).bad());
    OFCHECK(dims.getDimensionIndexSequence().empty());

    // Valid parameters
    OFCHECK(dims.addDimensionIndex(DCM_ImagePositionPatient, uid, DCM_PlanePositionSequence).good());
    OFCHECK(dims.getDimensionOrganizationSequence().size() == 1);
    OFCHECK(dims.getDimensionIndexSequence().size() == 1);
}
