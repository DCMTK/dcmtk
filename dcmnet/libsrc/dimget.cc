/*
**
** Author: Andrew Hewett		Created: 1998.09.03
** 
** Module: dimget
**
** Purpose: 
**	This file contains the routines which help with
**	query/retrieve services using the C-GET operation.
**
**	Module Prefix: DIMSE_
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 2000-02-01 10:24:10 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dimget.cc,v $
** CVS/RCS Revision:	$Revision: 1.2 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
*/

/* 
** Include Files
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB_4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB_4
END_EXTERN_C
#endif
#endif

#include <stdio.h>
#include <string.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#include "diutil.h"
#include "dimse.h"		/* always include the module header */
#include "dimcond.h"

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
        timeout = 1;	/* poll wait until an assoc req or move rsp */
    } else {
        if (blockMode == DIMSE_BLOCKING) {
            timeout = 10000;	/* a long time */
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

CONDITION
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
    CONDITION cond;
    T_DIMSE_Message req, rsp;
    DIC_US msgId;
    int responseCount = 0;
    T_ASC_Association *subAssoc = NULL;
    DIC_US status = STATUS_Pending;

    if (requestIdentifiers == NULL) {
        return COND_PushCondition(DIMSE_NULLKEY,
	    "DIMSE_getUser: No Request Identifiers Supplied");
    }

    bzero((char*)&req, sizeof(req));
    bzero((char*)&rsp, sizeof(rsp));
    
    req.CommandField = DIMSE_C_GET_RQ;
    request->DataSetType = DIMSE_DATASET_PRESENT;
    req.msg.CGetRQ = *request;

    msgId = request->MessageID;

    cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &req,
					  NULL, requestIdentifiers, 
					  NULL, NULL);
    if (cond != DIMSE_NORMAL) {
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

    while (cond == DIMSE_NORMAL && status == STATUS_Pending) {

	/* if user wants, multiplex between net/subAssoc 
	 * and responses over main assoc.
	 */
	switch (selectReadable(assoc, net, subAssoc, blockMode, timeout)) {
	case 0:
	    /* none are readble, timeout */
	    if (blockMode == DIMSE_BLOCKING) {
	        continue;	/* continue with while loop */
	    } else {
	        return COND_PushCondition(DIMSE_NODATAAVAILABLE,
		    "DIMSE_getUser: receive timeout");
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
	    continue;	/* continue with main loop */
	    /* break; */ // never reached after continue statement
	}

        bzero((char*)&rsp, sizeof(rsp));

	cond = DIMSE_receiveCommand(assoc, blockMode, timeout, &presID, 
		&rsp, statusDetail);
	if (cond != DIMSE_NORMAL) {
	    return cond;
	}
        if (rsp.CommandField != DIMSE_C_GET_RSP) {
	    return COND_PushCondition(DIMSE_UNEXPECTEDRESPONSE, 
		"DIMSE: Unexpected Response Command Field: 0x%x",
		(unsigned)rsp.CommandField);
        }
    
        *response = rsp.msg.CGetRSP;
	
        if (response->MessageIDBeingRespondedTo != msgId) {
	    return COND_PushCondition(DIMSE_UNEXPECTEDRESPONSE, 
		"DIMSE: Unexpected Response MsgId: %d (expected: %d)",
		response->MessageIDBeingRespondedTo, msgId);
        }

        status = response->DimseStatus;
	responseCount++;

	switch (status) {
	case STATUS_Pending:
	    if (*statusDetail != NULL) {
	        DIMSE_warning(assoc, 
		    "getUser: Pending with statusDetail, ignoring detail");
		delete *statusDetail;
		*statusDetail = NULL;
	    }
	    if (response->DataSetType != DIMSE_DATASET_NULL) {
		DIMSE_warning(assoc, 
		    "getUser: Status Pending, but DataSetType!=NULL");
		DIMSE_warning(assoc, 
		    "  Assuming NO response identifiers are present");
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
    	        if (cond != DIMSE_NORMAL) {
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

CONDITION
DIMSE_sendGetResponse(T_ASC_Association * assoc, 
	T_ASC_PresentationContextID presID, T_DIMSE_C_GetRQ *request, 
	T_DIMSE_C_GetRSP *response, DcmDataset *rspIds, 
	DcmDataset *statusDetail)
{
    CONDITION cond = DIMSE_NORMAL;
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
	rspIds = NULL;	/* zero our local pointer */
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

    cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &rsp, 
	statusDetail, rspIds, NULL, NULL);

    return cond;

}

CONDITION
DIMSE_getProvider(
	/* in */ 
	T_ASC_Association *assoc, 
	T_ASC_PresentationContextID presIdCmd,
	T_DIMSE_C_GetRQ *request,
	DIMSE_GetProviderCallback callback, void *callbackData,
	/* blocking info for data set */
	T_DIMSE_BlockingMode blockMode, int timeout)
{	
    CONDITION cond = DIMSE_NORMAL;
    T_ASC_PresentationContextID presIdData;
    DcmDataset *statusDetail = NULL;
    DcmDataset *reqIds = NULL;
    DcmDataset *rspIds = NULL;
    OFBool cancelled = OFFalse;
    int responseCount = 0;
    T_DIMSE_C_GetRSP rsp;

    cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout,
		&presIdData, &reqIds, NULL, NULL);

    if (cond != DIMSE_NORMAL) {
	goto providerCleanup;
    }

    if (presIdData != presIdCmd) {
	cond = COND_PushCondition(DIMSE_INVALIDPRESENTATIONCONTEXTID, 
		"DIMSE: Presentation Contexts of Command and Data Differ");
	goto providerCleanup;
    }

    bzero((char*)&rsp, sizeof(rsp));
    rsp.DimseStatus = STATUS_Pending;	/* assume */
    
    while (cond == DIMSE_NORMAL && rsp.DimseStatus == STATUS_Pending) {
	responseCount++;

	cond = DIMSE_checkForCancelRQ(assoc, presIdCmd, request->MessageID);
	if (cond == DIMSE_NORMAL) {
	    /* cancel received */
	    rsp.DimseStatus = 
	      STATUS_GET_Cancel_SubOperationsTerminatedDueToCancelIndication;
	    cancelled = OFTrue;	    
	} else if (cond == DIMSE_NODATAAVAILABLE) {
	    /* get rid of timeout on condition stack */
	    COND_PopCondition(OFFalse);	
	} else {
	    /* some execption condition occured */
	    goto providerCleanup;
	}

        if (callback) {
	    callback(callbackData, cancelled, request, reqIds, 
	        responseCount, &rsp, &statusDetail, &rspIds);
	} else {
	    return COND_PushCondition(DIMSE_NULLKEY,
	        "DIMSE_getProvider: no callback function"); 
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
	
	cond = DIMSE_sendGetResponse(assoc, presIdCmd, request,
	    &rsp, rspIds, statusDetail);
	    
	if (rspIds != NULL) {
	    delete rspIds;
	    rspIds = NULL;
	}
	if (statusDetail != NULL) {
	    delete statusDetail;
	    statusDetail = NULL;
	}
    }

providerCleanup:
    if (reqIds != NULL) {
        delete reqIds;
        reqIds = NULL;
    }
    if (rspIds != NULL) {
        delete rspIds;
        rspIds = NULL;
    }
       
    return cond;
}

/*
** CVS Log
** $Log: dimget.cc,v $
** Revision 1.2  2000-02-01 10:24:10  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.1  1999/04/19 08:37:27  meichel
** Added experimental support for C-GET.
**
**
*/
