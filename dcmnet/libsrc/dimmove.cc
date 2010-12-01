/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
** Module: dimmove
**
** Purpose:
**      This file contains the routines which help with
**      query/retrieve services using the C-MOVE operation.
**
** Module Prefix: DIMSE_
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 2010-12-01 08:26:36 $
** CVS/RCS Revision:    $Revision: 1.15 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
*/

/*
** Include Files
*/

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/dimse.h"       /* always include the module header */
#include "dcmtk/dcmnet/cond.h"

/*
**
*/

static int
selectReadable(
    T_ASC_Association *assoc,
    T_ASC_Network *net,
    T_ASC_Association *subAssoc,
    T_DIMSE_BlockingMode blockMode, int timeout)
{
    T_ASC_Association *assocList[2];
    int assocCount = 0;

    if (net != NULL && subAssoc == NULL) {
        if (ASC_associationWaiting(net, 0)) {
            /* association request waiting on network */
            return 2;
        }
    }
    assocList[0] = assoc;
    assocCount = 1;
    assocList[1] = subAssoc;
    if (subAssoc != NULL) assocCount++;
    if (subAssoc == NULL) {
        timeout = 1;            /* poll wait until an assoc req or move rsp */
    } else {
        if (blockMode == DIMSE_BLOCKING) {
            timeout = 10000;    /* a long time */
        }
    }
    if (!ASC_selectReadableAssociation(assocList, assocCount, timeout)) {
        /* none readable */
        return 0;
    }
    if (assocList[0] != NULL) {
        /* main association readable */
        return 1;
    }
    if (assocList[1] != NULL) {
        /* sub association readable */
        return 2;
    }
    /* should not be reached */
    return 0;
}

OFCondition
DIMSE_moveUser(
    /* in */
    T_ASC_Association *assoc,
    T_ASC_PresentationContextID presID,
    T_DIMSE_C_MoveRQ *request,
    DcmDataset *requestIdentifiers,
    DIMSE_MoveUserCallback callback, void *callbackData,
    /* blocking info for response */
    T_DIMSE_BlockingMode blockMode, int timeout,
    /* sub-operation provider callback */
    T_ASC_Network *net,
    DIMSE_SubOpProviderCallback subOpCallback, void *subOpCallbackData,
    /* out */
    T_DIMSE_C_MoveRSP *response, DcmDataset **statusDetail,
    DcmDataset **rspIds,
    OFBool ignorePendingDatasets)
{
    T_DIMSE_Message req, rsp;
    DIC_US msgId;
    int responseCount = 0;
    T_ASC_Association *subAssoc = NULL;
    DIC_US status = STATUS_Pending;
    OFBool firstLoop = OFTrue;

    if (requestIdentifiers == NULL) return DIMSE_NULLKEY;

    bzero((char*)&req, sizeof(req));
    bzero((char*)&rsp, sizeof(rsp));

    req.CommandField = DIMSE_C_MOVE_RQ;
    request->DataSetType = DIMSE_DATASET_PRESENT;
    req.msg.CMoveRQ = *request;

    msgId = request->MessageID;

    OFCondition cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &req, NULL, requestIdentifiers, NULL, NULL);
    if (cond != EC_Normal) {
        return cond;
    }

    /* receive responses */

    while (cond == EC_Normal && status == STATUS_Pending) {

        /* if user wants, multiplex between net/subAssoc
         * and move responses over main assoc.
         */
        switch (selectReadable(assoc, net, subAssoc, blockMode, timeout)) {
        case 0:
            /* none are readable, timeout */
            if ((blockMode == DIMSE_BLOCKING) || firstLoop) {
                firstLoop = OFFalse;
                continue;  /* continue with while loop */
            } else {
                return DIMSE_NODATAAVAILABLE;
            }
            /* break; */   // never reached after continue or return statement
        case 1:
            /* main association readable */
            firstLoop = OFFalse;
            break;
        case 2:
            /* net/subAssoc readable */
            if (subOpCallback) {
                subOpCallback(subOpCallbackData, net, &subAssoc);
            }
            firstLoop = OFFalse;
            continue;    /* continue with main loop */
            /* break; */ // never reached after continue statement
        }

        bzero((char*)&rsp, sizeof(rsp));

        cond = DIMSE_receiveCommand(assoc, blockMode, timeout, &presID, &rsp, statusDetail);
        if (cond != EC_Normal) {
            return cond;
        }
        if (rsp.CommandField != DIMSE_C_MOVE_RSP) {
            char buf1[256];
            sprintf(buf1, "DIMSE: Unexpected Response Command Field: 0x%x", (unsigned)rsp.CommandField);
            return makeDcmnetCondition(DIMSEC_UNEXPECTEDRESPONSE, OF_error, buf1);
        }

        *response = rsp.msg.CMoveRSP;

        if (response->MessageIDBeingRespondedTo != msgId) {
            char buf2[256];
            sprintf(buf2, "DIMSE: Unexpected Response MsgId: %d (expected: %d)", response->MessageIDBeingRespondedTo, msgId);
            return makeDcmnetCondition(DIMSEC_UNEXPECTEDRESPONSE, OF_error, buf2);
        }

        status = response->DimseStatus;
        responseCount++;

        switch (status) {
        case STATUS_Pending:
            if (*statusDetail != NULL) {
                DCMNET_WARN(DIMSE_warn_str(assoc) << "moveUser: Pending with statusDetail, ignoring detail");
                delete *statusDetail;
                *statusDetail = NULL;
            }
            if (response->DataSetType != DIMSE_DATASET_NULL) {
                DCMNET_WARN(DIMSE_warn_str(assoc) << "moveUser: Status Pending, but DataSetType!=NULL");
                if (! ignorePendingDatasets) {
                    // Some systems send an (illegal) dataset following C-MOVE-RSP messages
                    // with pending status, which is a protocol violation, but we need to
                    // handle this nevertheless. The MV300 has been reported to exhibit
                    // this behavior.
                    DCMNET_WARN(DIMSE_warn_str(assoc) << "Reading but ignoring response identifier set");
                    DcmDataset *tempset = NULL;
                    cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout, &presID, &tempset, NULL, NULL);
                    delete tempset;
                    if (cond != EC_Normal) {
                        return cond;
                    }
                } else {
                    // The alternative is to assume that the command set is wrong
                    // and not to read a dataset from the network association.
                    DCMNET_WARN(DIMSE_warn_str(assoc) << "Assuming NO response identifiers are present");
                }
            }

            /* execute callback */
            if (callback) {
                callback(callbackData, request, responseCount, response);
            }
            break;
        default:
            if (response->DataSetType != DIMSE_DATASET_NULL) {
                cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout, &presID, rspIds, NULL, NULL);
                if (cond != EC_Normal) {
                    return cond;
                }
            }
            break;
        }
    }

    /* do remaining sub-association work, we may receive a non-pending
     * status before the sub-association has cleaned up.
     */
    while (subAssoc != NULL) {
        if (subOpCallback) {
            subOpCallback(subOpCallbackData, net, &subAssoc);
        }
    }

    return cond;
}

OFCondition
DIMSE_sendMoveResponse(
    T_ASC_Association *assoc,
    T_ASC_PresentationContextID presID,
    T_DIMSE_C_MoveRQ *request,
    T_DIMSE_C_MoveRSP *response, DcmDataset *rspIds,
    DcmDataset *statusDetail)
{
    OFCondition cond = EC_Normal;
    T_DIMSE_Message rsp;
    unsigned int opts;

    bzero((char*)&rsp, sizeof(rsp));
    rsp.CommandField = DIMSE_C_MOVE_RSP;
    rsp.msg.CMoveRSP = *response;
    /* copy over stuff from request */
    rsp.msg.CMoveRSP.MessageIDBeingRespondedTo = request->MessageID;
    /* always send afected sop class uid */
    strcpy(rsp.msg.CMoveRSP.AffectedSOPClassUID, request->AffectedSOPClassUID);
    rsp.msg.CMoveRSP.opts = O_MOVE_AFFECTEDSOPCLASSUID;

    switch (response->DimseStatus) {
    case STATUS_Success:
    case STATUS_Pending:
        /* Success cannot have a Failed SOP Instance UID list (no failures).
         * Pending may not send such a list.
         */
        rsp.msg.CMoveRSP.DataSetType = DIMSE_DATASET_NULL;
        rspIds = NULL;  /* zero our local pointer */
        break;
    default:
        /* send it if provided */
        if (rspIds == NULL) rsp.msg.CMoveRSP.DataSetType = DIMSE_DATASET_NULL;
        else rsp.msg.CMoveRSP.DataSetType = DIMSE_DATASET_PRESENT;
        break;
    }

    /*
     * Make sure the numberOf fields are conformant with
     * the status (see Part 4, C.4.2.1.6-9)
     */
    opts = (O_MOVE_NUMBEROFREMAININGSUBOPERATIONS |
            O_MOVE_NUMBEROFCOMPLETEDSUBOPERATIONS |
            O_MOVE_NUMBEROFFAILEDSUBOPERATIONS |
            O_MOVE_NUMBEROFWARNINGSUBOPERATIONS);

    switch (response->DimseStatus) {
    case STATUS_Pending:
    case STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication:
        break;
    default:
        /* Remaining sub-operations may not be in responses
         * with a status of Warning, Failed, Refused or Successful
         */
        opts &= (~ O_MOVE_NUMBEROFREMAININGSUBOPERATIONS);
        break;
    }

    rsp.msg.CMoveRSP.opts |= opts;

    cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &rsp, statusDetail, rspIds, NULL, NULL);

    return cond;

}

OFCondition
DIMSE_moveProvider(
    /* in */
    T_ASC_Association *assoc,
    T_ASC_PresentationContextID presIdCmd,
    T_DIMSE_C_MoveRQ *request,
    DIMSE_MoveProviderCallback callback, void *callbackData,
    /* blocking info for data set */
    T_DIMSE_BlockingMode blockMode, int timeout)
{
    OFCondition cond = EC_Normal;
    T_ASC_PresentationContextID presIdData;
    DcmDataset *statusDetail = NULL;
    DcmDataset *reqIds = NULL;
    DcmDataset *rspIds = NULL;
    OFBool cancelled = OFFalse;
    OFBool normal = OFTrue;
    int responseCount = 0;
    T_DIMSE_C_MoveRSP rsp;

    cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout, &presIdData, &reqIds, NULL, NULL);

    if (cond.good()) {
        if (presIdData != presIdCmd) {
          cond = makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error, "DIMSE: Presentation Contexts of Command and Data Differ");
        } else {
            bzero((char*)&rsp, sizeof(rsp));
            rsp.DimseStatus = STATUS_Pending;   /* assume */

            while (cond == EC_Normal && rsp.DimseStatus == STATUS_Pending && normal) {
                responseCount++;

                cond = DIMSE_checkForCancelRQ(assoc, presIdCmd, request->MessageID);
                if (cond == EC_Normal) {
                    /* cancel received */
                    rsp.DimseStatus = STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication;
                    cancelled = OFTrue;
                } else if (cond == DIMSE_NODATAAVAILABLE) {
                    /* timeout */
                } else {
                    /* some execption condition occured, bail out */
                    normal = OFFalse;
                }

                if (normal) {
                     if (callback) {
                         callback(callbackData, cancelled, request, reqIds, responseCount, &rsp, &statusDetail, &rspIds);
                     } else {
                         return makeDcmnetCondition(DIMSEC_NULLKEY, OF_error, "DIMSE_moveProvider: no callback function");
                     }

                     if (cancelled) {
                         /* make sure */
                         rsp.DimseStatus =
                           STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication;
                         if (rspIds != NULL) {
                             delete reqIds;
                             reqIds = NULL;
                         }
                     }

                     cond = DIMSE_sendMoveResponse(assoc, presIdCmd, request, &rsp, rspIds, statusDetail);

                     if (rspIds != NULL) {
                         delete rspIds;
                         rspIds = NULL;
                     }
                     if (statusDetail != NULL) {
                         delete statusDetail;
                         statusDetail = NULL;
                     }
                }
            }
        }
    }

    delete reqIds;
    delete rspIds;
    return cond;
}

/*
** CVS Log
** $Log: dimmove.cc,v $
** Revision 1.15  2010-12-01 08:26:36  joergr
** Added OFFIS copyright header (beginning with the year 1994).
**
** Revision 1.14  2010-09-13 10:40:17  joergr
** Fixed issue with non-blocking mode in Move SCU (given timeout was ignored).
**
** Revision 1.13  2009-11-18 11:53:59  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.12  2005-12-08 15:44:44  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.11  2005/11/22 16:44:47  meichel
** Added option to movescu that allows graceful handling of Move SCPs
**   that send illegal datasets following pending C-MOVE-RSP messages.
**
** Revision 1.10  2002/11/27 13:04:41  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.9  2001/10/12 10:18:35  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.8  2000/02/23 15:12:36  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.7  2000/02/01 10:24:10  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.6  1998/08/10 08:53:45  meichel
** renamed member variable in DIMSE structures from "Status" to
**   "DimseStatus". This is required if dcmnet is used together with
**   <X11/Xlib.h> where Status is #define'd as int.
**
** Revision 1.5  1998/01/27 10:51:45  meichel
** Removed some unused variables, meaningless const modifiers
**   and unreached statements.
**
** Revision 1.4  1997/09/18 08:10:58  meichel
** Many minor type conflicts (e.g. long passed as int) solved.
**
** Revision 1.3  1997/07/21 08:47:18  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.2  1996/04/25 16:11:16  hewett
** Added parameter casts to char* for bzero calls.  Replaced some declarations
** of DIC_UL with unsigned long (reduces mismatch problems with 32 & 64 bit
** architectures).  Added some protection to inclusion of sys/socket.h (due
** to MIPS/Ultrix).
**
** Revision 1.1.1.1  1996/03/26 18:38:46  hewett
** Initial Release.
**
**
*/
