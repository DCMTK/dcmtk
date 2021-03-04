/*
 *
 *  Copyright (C) 1994-2021, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were partly developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  For further copyrights, see the following paragraphs.
 *
 */

/*
**  Copyright (C) 1993/1994, OFFIS, Oldenburg University and CERIUM
**
**  This software and supporting documentation were
**  developed by
**
**    Institut OFFIS
**    Bereich Kommunikationssysteme
**    Westerstr. 10-12
**    26121 Oldenburg, Germany
**
**    Fachbereich Informatik
**    Abteilung Prozessinformatik
**    Carl von Ossietzky Universitaet Oldenburg
**    Ammerlaender Heerstr. 114-118
**    26111 Oldenburg, Germany
**
**    CERIUM
**    Laboratoire SIM
**    Faculte de Medecine
**    2 Avenue du Pr. Leon Bernard
**    35043 Rennes Cedex, France
**
**  for CEN/TC251/WG4 as a contribution to the Radiological
**  Society of North America (RSNA) 1993 Digital Imaging and
**  Communications in Medicine (DICOM) Demonstration.
**
**  THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND NEITHER OFFIS,
**  OLDENBURG UNIVERSITY NOR CERIUM MAKE ANY WARRANTY REGARDING
**  THE SOFTWARE, ITS PERFORMANCE, ITS MERCHANTABILITY OR
**  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER
**  DISEASES OR ITS CONFORMITY TO ANY SPECIFICATION.  THE
**  ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF THE SOFTWARE
**  IS WITH THE USER.
**
**  Copyright of the software and supporting documentation
**  is, unless otherwise stated, jointly owned by OFFIS,
**  Oldenburg University and CERIUM and free access is hereby
**  granted as a license to use this software, copy this
**  software and prepare derivative works based upon this
**  software. However, any distribution of this software
**  source code or supporting documentation or derivative
**  works (source code and supporting documentation) must
**  include the three paragraphs of this copyright notice.
**
*/

/*
**
** Author: Andrew Hewett                Created: 03-06-93
**
** Module: dimecho
**
** Purpose:
**      This file contains the routines which help with
**      verification services.
**
** Module Prefix: DIMSE_
**
*/


/*
** Include Files
*/

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/dimse.h"     /* always include the module header */
#include "dcmtk/dcmnet/cond.h"


OFCondition
DIMSE_echoUser(
    /* in */
    T_ASC_Association *assoc, DIC_US msgId,
    /* blocking info for response */
    T_DIMSE_BlockingMode blockMode, int timeout,
    /* out */
    DIC_US *status, DcmDataset **statusDetail)
{
    T_DIMSE_Message req, rsp;
    T_ASC_PresentationContextID presID;
    const char *sopClass;

    /* which SOP class  */
    sopClass = UID_VerificationSOPClass;

    /* which presentation context should be used */
    presID = ASC_findAcceptedPresentationContextID(assoc, sopClass);
    if (presID == 0)
    {
        char buf[1024];
        sprintf(buf, "DIMSE: No Presentation Context for: %s", sopClass);
        return makeDcmnetCondition(DIMSEC_NOVALIDPRESENTATIONCONTEXTID, OF_error, buf);
    }

    memset((char*)&req, 0, sizeof(req));
    memset((char*)&rsp, 0, sizeof(rsp));

    req.CommandField = DIMSE_C_ECHO_RQ;
    req.msg.CEchoRQ.MessageID = msgId;
    OFStandard::strlcpy(req.msg.CEchoRQ.AffectedSOPClassUID,
       sopClass, sizeof(req.msg.CEchoRQ.AffectedSOPClassUID));
    req.msg.CEchoRQ.DataSetType = DIMSE_DATASET_NULL;

    OFCondition cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &req, NULL, NULL, NULL, NULL);
    if (cond.bad()) return cond;

    /* receive response */
    cond = DIMSE_receiveCommand(assoc, blockMode, timeout, &presID, &rsp, statusDetail);
    if (cond.bad()) return cond;

    if (rsp.CommandField != DIMSE_C_ECHO_RSP)
    {
        char buf1[256];
        sprintf(buf1, "DIMSE: Unexpected Response Command Field: 0x%x", (unsigned)rsp.CommandField);
        return makeDcmnetCondition(DIMSEC_UNEXPECTEDRESPONSE, OF_error, buf1);
    }

    if (rsp.msg.CEchoRSP.MessageIDBeingRespondedTo != msgId)
    {
        char buf1[256];
        sprintf(buf1, "DIMSE: Unexpected Response MsgId: %d (expected: %d)", rsp.msg.CEchoRSP.MessageIDBeingRespondedTo, msgId);
        return makeDcmnetCondition(DIMSEC_UNEXPECTEDRESPONSE, OF_error, buf1);
    }

    *status = rsp.msg.CEchoRSP.DimseStatus;

    return EC_Normal;
}

OFCondition
DIMSE_sendEchoResponse(T_ASC_Association * assoc,
    T_ASC_PresentationContextID presID,
    const T_DIMSE_C_EchoRQ *req, DIC_US status, DcmDataset *statusDetail)
{
    T_DIMSE_Message rsp;

    memset((char*)&rsp, 0, sizeof(rsp));

    rsp.CommandField = DIMSE_C_ECHO_RSP;
    rsp.msg.CEchoRSP.MessageIDBeingRespondedTo = req->MessageID;
    OFStandard::strlcpy(rsp.msg.CEchoRSP.AffectedSOPClassUID,
        req->AffectedSOPClassUID, sizeof(rsp.msg.CEchoRSP.AffectedSOPClassUID));
    rsp.msg.CEchoRSP.opts = O_ECHO_AFFECTEDSOPCLASSUID;
    rsp.msg.CEchoRSP.DataSetType = DIMSE_DATASET_NULL;
    rsp.msg.CEchoRSP.DimseStatus = status;

    return DIMSE_sendMessageUsingMemoryData(assoc, presID, &rsp, statusDetail, NULL, NULL, NULL);
}
