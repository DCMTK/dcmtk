/*
 *
 *  Copyright (C) 1993-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:35 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
        dbcond = dbHandle.nextFindResponse(responseIdentifiers, &dbStatus);
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


/*
 * CVS Log
 * $Log: dcmqrcbf.cc,v $
 * Revision 1.6  2010-10-14 13:14:35  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.5  2010-09-09 15:00:02  joergr
 * Made log messages more consistent. Replaced '\n' by OFendl where appropriate.
 *
 * Revision 1.4  2009-11-24 10:10:42  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.3  2009-08-21 09:54:11  joergr
 * Replaced tabs by spaces and updated copyright date.
 *
 * Revision 1.2  2005/12/08 15:47:04  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2005/03/30 13:34:53  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */
