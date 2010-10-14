/*
**
** Author: Andrew Hewett                Created: 1998.09.03
** 
** Module: dimget
**
** Purpose: 
**      This file contains the routines which help with
**      query/retrieve services using the C-GET operation.
**
**      Module Prefix: DIMSE_
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 2010-10-14 13:14:28 $
** CVS/RCS Revision:    $Revision: 1.8 $
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
#include "dcmtk/dcmnet/dimse.h"              /* always include the module header */
#include "dcmtk/dcmnet/cond.h"

/*
**
*/

static int
selectReadable(T_ASC_Association *assoc, 
    T_ASC_Network *net, T_ASC_Association *subAssoc,
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
        timeout = 1;    /* poll wait until an assoc req or move rsp */
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
DIMSE_getUser(
        /* in */
        T_ASC_Association *assoc, 
        T_ASC_PresentationContextID presID,
        T_DIMSE_C_GetRQ *request,
        DcmDataset *requestIdentifiers,
        DIMSE_GetUserCallback callback, void *callbackData,
        /* blocking info for response */
        T_DIMSE_BlockingMode blockMode, int timeout,
        /* sub-operation provider callback */
        T_ASC_Network *net,
        DIMSE_SubOpProviderCallback subOpCallback, void *subOpCallbackData,
        /* out */
        T_DIMSE_C_GetRSP *response, DcmDataset **statusDetail,
        DcmDataset **rspIds)
{
    T_DIMSE_Message req, rsp;
    DIC_US msgId;
    int responseCount = 0;
    T_ASC_Association *subAssoc = NULL;
    DIC_US status = STATUS_Pending;

    if (requestIdentifiers == NULL) return DIMSE_NULLKEY;

    bzero((char*)&req, sizeof(req));
    bzero((char*)&rsp, sizeof(rsp));
    
    req.CommandField = DIMSE_C_GET_RQ;
    request->DataSetType = DIMSE_DATASET_PRESENT;
    req.msg.CGetRQ = *request;

    msgId = request->MessageID;

    OFCondition cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &req,
                                          NULL, requestIdentifiers, 
                                          NULL, NULL);
    if (cond != EC_Normal) {
        return cond;
    }

    /* receive responses */


    /*
     * DANGER:
     * This code to receive responses is completely untried.
     * It was simply taken from the MOVE version and has not
     * yet been adapted for GET.
     * GET is completely different here.
     */

    while (cond == EC_Normal && status == STATUS_Pending) {

        /* if user wants, multiplex between net/subAssoc 
         * and responses over main assoc.
         */
        switch (selectReadable(assoc, net, subAssoc, blockMode, timeout)) {
        case 0:
            /* none are readble, timeout */
            if (blockMode == DIMSE_BLOCKING) {
                continue;       /* continue with while loop */
            } else {
                return DIMSE_NODATAAVAILABLE;
            }
            /* break; */ // never reached after continue or return.
        case 1:
            /* main association readable */
            break;
        case 2:
            /* net/subAssoc readable */
            if (subOpCallback) {
                subOpCallback(subOpCallbackData, net, &subAssoc);
            }
            continue;   /* continue with main loop */
            /* break; */ // never reached after continue statement
        }

        bzero((char*)&rsp, sizeof(rsp));

        cond = DIMSE_receiveCommand(assoc, blockMode, timeout, &presID, 
                &rsp, statusDetail);
        if (cond != EC_Normal) {
            return cond;
        }
        if (rsp.CommandField != DIMSE_C_GET_RSP)
        {
          char buf1[256];
          sprintf(buf1, "DIMSE: Unexpected Response Command Field: 0x%x", (unsigned)rsp.CommandField);
          return makeDcmnetCondition(DIMSEC_UNEXPECTEDRESPONSE, OF_error, buf1);
        }
    
        *response = rsp.msg.CGetRSP;
        
        if (response->MessageIDBeingRespondedTo != msgId)
        {
          char buf2[256];
          sprintf(buf2, "DIMSE: Unexpected Response MsgId: %d (expected: %d)", response->MessageIDBeingRespondedTo, msgId);
          return makeDcmnetCondition(DIMSEC_UNEXPECTEDRESPONSE, OF_error, buf2);
        }

        status = response->DimseStatus;
        responseCount++;

        switch (status) {
        case STATUS_Pending:
            if (*statusDetail != NULL) {
                DCMNET_WARN(DIMSE_warn_str(assoc) << "getUser: Pending with statusDetail, ignoring detail");
                delete *statusDetail;
                *statusDetail = NULL;
            }
            if (response->DataSetType != DIMSE_DATASET_NULL) {
                DCMNET_WARN(DIMSE_warn_str(assoc) << "getUser: Status Pending, but DataSetType!=NULL");
                DCMNET_WARN(DIMSE_warn_str(assoc) << "Assuming NO response identifiers are present");
            }

            /* execute callback */
            if (callback) {
                callback(callbackData, request, responseCount, response);
            }
            break;
        default:
            if (response->DataSetType != DIMSE_DATASET_NULL) {
                cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout,
                    &presID, rspIds, NULL, NULL);
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
DIMSE_sendGetResponse(T_ASC_Association * assoc, 
        T_ASC_PresentationContextID presID, T_DIMSE_C_GetRQ *request, 
        T_DIMSE_C_GetRSP *response, DcmDataset *rspIds, 
        DcmDataset *statusDetail)
{
    T_DIMSE_Message rsp;
    unsigned int opts;

    bzero((char*)&rsp, sizeof(rsp));
    rsp.CommandField = DIMSE_C_GET_RSP;
    rsp.msg.CGetRSP = *response;
    /* copy over stuff from request */
    rsp.msg.CGetRSP.MessageIDBeingRespondedTo = request->MessageID;
    /* always send afected sop class uid */
    strcpy(rsp.msg.CGetRSP.AffectedSOPClassUID, request->AffectedSOPClassUID);
    rsp.msg.CGetRSP.opts = O_GET_AFFECTEDSOPCLASSUID;
    
    switch (response->DimseStatus) {
    case STATUS_Success:
    case STATUS_Pending:
        /* Success cannot have a Failed SOP Instance UID list (no failures).
         * Pending may not send such a list.
         */ 
        rsp.msg.CGetRSP.DataSetType = DIMSE_DATASET_NULL;
        rspIds = NULL;  /* zero our local pointer */
        break;
    default:
        /* send it if provided */
        if (rspIds == NULL) rsp.msg.CGetRSP.DataSetType = DIMSE_DATASET_NULL;
        else rsp.msg.CGetRSP.DataSetType = DIMSE_DATASET_PRESENT;
        break;
    }

    /* 
     * Make sure the numberOf fields are conformant with
     * the status (see Part 4, C.4.2.1.6-9)
     */
    opts = (O_GET_NUMBEROFREMAININGSUBOPERATIONS |
        O_GET_NUMBEROFCOMPLETEDSUBOPERATIONS |
        O_GET_NUMBEROFFAILEDSUBOPERATIONS |
        O_GET_NUMBEROFWARNINGSUBOPERATIONS);
        
    switch (response->DimseStatus) {
    case STATUS_Pending:
    case STATUS_GET_Cancel_SubOperationsTerminatedDueToCancelIndication:
        break;
    default:
        /* Remaining sub-operations may not be in responses 
         * with a status of Warning, Failed, Refused or Successful
         */
        opts &= (~ O_GET_NUMBEROFREMAININGSUBOPERATIONS);
        break;
    }

    rsp.msg.CGetRSP.opts |= opts;

    return DIMSE_sendMessageUsingMemoryData(assoc, presID, &rsp, statusDetail, rspIds, NULL, NULL);
}

OFCondition
DIMSE_getProvider(
        /* in */ 
        T_ASC_Association *assoc, 
        T_ASC_PresentationContextID presIdCmd,
        T_DIMSE_C_GetRQ *request,
        DIMSE_GetProviderCallback callback, void *callbackData,
        /* blocking info for data set */
        T_DIMSE_BlockingMode blockMode, int timeout)
{       
    T_ASC_PresentationContextID presIdData;
    DcmDataset *statusDetail = NULL;
    DcmDataset *reqIds = NULL;
    DcmDataset *rspIds = NULL;
    OFBool cancelled = OFFalse;
    OFBool normal = OFTrue;
    int responseCount = 0;
    T_DIMSE_C_GetRSP rsp;

    OFCondition cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout, &presIdData, &reqIds, NULL, NULL);

    if (cond.good())
    {    
        if (presIdData != presIdCmd)
        {
            cond = makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error, "DIMSE: Presentation Contexts of Command and Data Differ");
        }
        else
        {
            bzero((char*)&rsp, sizeof(rsp));
            rsp.DimseStatus = STATUS_Pending;   /* assume */
            
            while (cond == EC_Normal && rsp.DimseStatus == STATUS_Pending && normal)
            {
                responseCount++;
            
                cond = DIMSE_checkForCancelRQ(assoc, presIdCmd, request->MessageID);
                if (cond == EC_Normal)
                {
                    /* cancel received */
                    rsp.DimseStatus = STATUS_GET_Cancel_SubOperationsTerminatedDueToCancelIndication;
                    cancelled = OFTrue;     
                } else if (cond == DIMSE_NODATAAVAILABLE) {
                    /* timeout */
                } else {
                    /* some execption condition occured, bail out */
                    normal = OFFalse;
                }
            
                if (normal)            
                {
                    if (callback) {
                        callback(callbackData, cancelled, request, reqIds, 
                            responseCount, &rsp, &statusDetail, &rspIds);
                    } else {
                        return makeDcmnetCondition(DIMSEC_NULLKEY, OF_error, "DIMSE_getProvider: no callback function");
                    }
                    
                    if (cancelled) {
                        /* make sure */
                        rsp.DimseStatus = 
                          STATUS_GET_Cancel_SubOperationsTerminatedDueToCancelIndication;
                        if (rspIds != NULL) {
                            delete reqIds;
                            reqIds = NULL;
                        }
                    }
                    
                    cond = DIMSE_sendGetResponse(assoc, presIdCmd, request, &rsp, rspIds, statusDetail);
                        
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
** $Log: dimget.cc,v $
** Revision 1.8  2010-10-14 13:14:28  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.7  2009-11-18 11:53:59  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.6  2005-12-08 15:44:43  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.5  2002/11/27 13:04:41  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.4  2001/10/12 10:18:34  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.3  2000/02/23 15:12:35  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.2  2000/02/01 10:24:10  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.1  1999/04/19 08:37:27  meichel
** Added experimental support for C-GET.
**
**
*/
