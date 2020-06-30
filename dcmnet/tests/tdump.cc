/*
 *
 *  Copyright (C) 2009-2020, OFFIS e.V.
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
 *  Module:  dcmnet
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: test program for DIMSE_dumpMessage()
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmnet/dimse.h"


OFTEST(dcmnet_dimseDump_nullByte)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    const char expected[] = "===================== INCOMING DIMSE MESSAGE ====================\nMessage Type                  : C-ECHO RQ\nMessage ID                    : 42\nData Set                      : present\n-----------------------------------------------------------------(fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n(0000,1000) UI [1.2\0]                                   #   4, 1 AffectedSOPInstanceUID\n(fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n======================= END DIMSE MESSAGE =======================";
    OFString result;
    T_DIMSE_C_EchoRQ rq = { 42, "1.2.3\0", DIMSE_DATASET_PRESENT };

    dcmEnableAutomaticInputDataCorrection.set(OFFalse);
    DcmUniqueIdentifier* id = new DcmUniqueIdentifier(DCM_AffectedSOPInstanceUID);
    id->putString("1.2\0", 4);
    dcmEnableAutomaticInputDataCorrection.set(OFTrue);

    DcmItem item;
    OFCondition cond = item.insert(id);
    OFCHECK(cond.good());

    // There was a bug in DIMSE_dumpMessage() where the \0-byte caused the
    // message to be cut off. This is what we are testing for.
    DIMSE_dumpMessage(result, rq, DIMSE_INCOMING, &item, 0);
    OFCHECK_EQUAL(result, OFString(expected, sizeof(expected) - 1));
}
