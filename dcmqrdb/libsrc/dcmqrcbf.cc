/*
 *
 *  Copyright (C) 1993-2017, OFFIS e.V.
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
 *  Module:  dcmqrdb
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: class DcmQueryRetrieveFindContext
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmqrdb/dcmqrcbf.h"

#include "dcmtk/dcmqrdb/dcmqrcnf.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmqrdb/dcmqropt.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmqrdb/dcmqrdbs.h"
#include "dcmtk/dcmqrdb/dcmqrdbi.h"


void DcmQueryRetrieveFindContext::callbackHandler(
    /* in */
    OFBool cancelled, T_DIMSE_C_FindRQ *request,
    DcmDataset *requestIdentifiers, int responseCount,
    /* out */
    T_DIMSE_C_FindRSP *response,
    DcmDataset **responseIdentifiers,
    DcmDataset **stDetail)
{
    OFCondition dbcond = EC_Normal;
    DcmQueryRetrieveDatabaseStatus dbStatus(priorStatus);

    if (responseCount == 1) {
        /* start the database search */
        DCMQRDB_INFO("Find SCP Request Identifiers:" << OFendl << DcmObject::PrintHelper(*requestIdentifiers));
        dbcond = dbHandle.startFindRequest(
            request->AffectedSOPClassUID, requestIdentifiers, &dbStatus);
        if (dbcond.bad()) {
            DCMQRDB_ERROR("findSCP: Database: startFindRequest Failed ("
                    << DU_cfindStatusString(dbStatus.status()) << "):");
        }
    }

    /* only cancel if we have pending responses */
    if (cancelled && DICOM_PENDING_STATUS(dbStatus.status())) {
        dbHandle.cancelFindRequest(&dbStatus);
    }

    if (DICOM_PENDING_STATUS(dbStatus.status())) {
        dbcond = dbHandle.nextFindResponse(responseIdentifiers, &dbStatus, characterSetOptions);
        if (dbcond.bad()) {
             DCMQRDB_ERROR("findSCP: Database: nextFindResponse Failed ("
                     << DU_cfindStatusString(dbStatus.status()) << "):");
        }
    }

    if (*responseIdentifiers != NULL)
    {

        if (! DU_putStringDOElement(*responseIdentifiers, DCM_RetrieveAETitle, ourAETitle.c_str())) {
            DCMQRDB_ERROR("DO: adding Retrieve AE Title");
        }
    }

    /* set response status */
    response->DimseStatus = dbStatus.status();
    *stDetail = dbStatus.extractStatusDetail();

    OFString str;
    DCMQRDB_INFO("Find SCP Response " << responseCount << " [status: "
            << DU_cfindStatusString(dbStatus.status()) << "]");
    DCMQRDB_DEBUG(DIMSE_dumpMessage(str, *response, DIMSE_OUTGOING));
    if (DICOM_PENDING_STATUS(dbStatus.status()) && (*responseIdentifiers != NULL))
        DCMQRDB_DEBUG("Find SCP Response Identifiers:" << OFendl << DcmObject::PrintHelper(**responseIdentifiers));
    if (*stDetail)
        DCMQRDB_DEBUG("  Status detail:" << OFendl << DcmObject::PrintHelper(**stDetail));
}
