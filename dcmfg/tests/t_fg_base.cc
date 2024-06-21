/*
 *
 *  Copyright (C) 2022-2024, OFFIS e.V.
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
 *  Author:  Michael Onken
 *
 *  Purpose: Tests for FGBase and FGUnknown
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcdeftag.h"


OFTEST(dcmfg_fgbase_fgunknown)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    FGUnknown fg(DCM_ScheduledProcedureStepSequence);
    DcmItem item;
    DcmItem* seqitem = new DcmItem();
    OFCHECK(seqitem->putAndInsertOFStringArray(DCM_PatientName, "Test").good());
    OFCHECK(item.insertSequenceItem(DCM_ScheduledProcedureStepSequence, seqitem).good());
    OFCHECK(fg.read(item).good());

    FGUnknown* fg_copy = OFstatic_cast(FGUnknown*, fg.clone());
    OFCHECK(fg_copy != NULL);
    OFCHECK(fg.compare(*fg_copy) == 0);
}
