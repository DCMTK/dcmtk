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
** Author: Andrew Hewett		Created: 03-06-93
** 
** Module: dimmove
**
** Purpose: 
**	This file contains the routines which help with
**	query/retrieve services using the C-MOVE operation.
**
**	Module Prefix: DIMSE_
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-04-25 16:11:16 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dimmove.cc,v $
** CVS/RCS Revision:	$Revision: 1.2 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
*/

/* 
** Include Files
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
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
        DcmDataset **rspIds)
{
    CONDITION cond;
    T_DIMSE_Message req, rsp;
    char *sopClass;
    DIC_US msgId;
    int responseCount = 0;
    T_ASC_Association *subAssoc = NULL;
    DIC_US status = STATUS_Pending;

    if (requestIdentifiers == NULL) {
        return COND_PushCondition(DIMSE_NULLKEY,
	    "DIMSE_moveUser: No Request Identifiers Supplied");
    }

    bzero((char*)&req, sizeof(req));
    bzero((char*)&rsp, sizeof(rsp));
    
    req.CommandField = DIMSE_C_MOVE_RQ;
    request->DataSetType = DIMSE_DATASET_PRESENT;
    req.msg.CMoveRQ = *request;

    msgId = request->MessageID;
    sopClass = request->AffectedSOPClassUID;

    cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &req,
					  NULL, requestIdentifiers, 
					  NULL, NULL);
    if (cond != DIMSE_NORMAL) {
	return cond;
    }

    /* receive responses */

    while (cond == DIMSE_NORMAL && status == STATUS_Pending) {

	/* if user wants, multiplex between net/subAssoc 
	 * and move responses over main assoc.
	 */
	switch (selectReadable(assoc, net, subAssoc, blockMode, timeout)) {
	case 0:
	    /* none are readble, timeout */
	    if (blockMode == DIMSE_BLOCKING) {
	        continue;	/* continue with while loop */
	    } else {
	        return COND_PushCondition(DIMSE_NODATAAVAILABLE,
		    "DIMSE_moveUser: receive timeout");
	    }
	    break;
	case 1:
	    /* main association readable */
	    break;
	case 2:
	    /* net/subAssoc readable */
	    if (subOpCallback) {
	        subOpCallback(subOpCallbackData, net, &subAssoc);
	    }
	    continue;	/* continue with main loop */
	    break;
	}

        bzero((char*)&rsp, sizeof(rsp));

	cond = DIMSE_receiveCommand(assoc, blockMode, timeout, &presID, 
		&rsp, statusDetail);
	if (cond != DIMSE_NORMAL) {
	    return cond;
	}
        if (rsp.CommandField != DIMSE_C_MOVE_RSP) {
	    return COND_PushCondition(DIMSE_UNEXPECTEDRESPONSE, 
		"DIMSE: Unexpected Response Command Field: 0x%x",
		(unsigned)rsp.CommandField);
        }
    
        *response = rsp.msg.CMoveRSP;
	
        if (response->MessageIDBeingRespondedTo != msgId) {
	    return COND_PushCondition(DIMSE_UNEXPECTEDRESPONSE, 
		"DIMSE: Unexpected Response MsgId: %d (expected: %d)",
		response->MessageIDBeingRespondedTo, msgId);
        }

        status = response->Status;
	responseCount++;

	switch (status) {
	case STATUS_Pending:
	    if (*statusDetail != NULL) {
	        DIMSE_warning(assoc, 
		    "moveUser: Pending with statusDetail, ignoring detail");
		delete *statusDetail;
		*statusDetail = NULL;
	    }
	    if (response->DataSetType != DIMSE_DATASET_NULL) {
		DIMSE_warning(assoc, 
		    "moveUser: Status Pending, but DataSetType!=NULL");
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
DIMSE_sendMoveResponse(T_ASC_Association * assoc, 
	T_ASC_PresentationContextID presID, T_DIMSE_C_MoveRQ *request, 
	T_DIMSE_C_MoveRSP *response, DcmDataset *rspIds, 
	DcmDataset *statusDetail)
{
    CONDITION cond = DIMSE_NORMAL;
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
    
    switch (response->Status) {
    case STATUS_Success:
    case STATUS_Pending:
        /* Success cannot have a Failed SOP Instance UID list (no failures).
	 * Pending may not send such a list.
	 */ 
        rsp.msg.CMoveRSP.DataSetType = DIMSE_DATASET_NULL;
	rspIds = NULL;	/* zero our local pointer */
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
	
    switch (response->Status) {
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

    cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &rsp, 
	statusDetail, rspIds, NULL, NULL);

    return cond;

}

CONDITION
DIMSE_moveProvider(
	/* in */ 
	T_ASC_Association *assoc, 
	T_ASC_PresentationContextID presIdCmd,
	T_DIMSE_C_MoveRQ *request,
	DIMSE_MoveProviderCallback callback, void *callbackData,
	/* blocking info for data set */
	T_DIMSE_BlockingMode blockMode, int timeout)
{	
    CONDITION cond = DIMSE_NORMAL;
    T_ASC_PresentationContextID presIdData;
    DcmDataset *statusDetail = NULL;
    DcmDataset *reqIds = NULL;
    DcmDataset *rspIds = NULL;
    BOOLEAN cancelled = FALSE;
    int responseCount = 0;
    T_DIMSE_C_MoveRSP rsp;

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
    rsp.Status = STATUS_Pending;	/* assume */
    
    while (cond == DIMSE_NORMAL && rsp.Status == STATUS_Pending) {
	responseCount++;

	cond = DIMSE_checkForCancelRQ(assoc, presIdCmd, request->MessageID);
	if (cond == DIMSE_NORMAL) {
	    /* cancel received */
	    rsp.Status = 
	      STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication;
	    cancelled = TRUE;	    
	} else if (cond == DIMSE_NODATAAVAILABLE) {
	    /* get rid of timeout on condition stack */
	    COND_PopCondition(FALSE);	
	} else {
	    /* some execption condition occured */
	    goto providerCleanup;
	}

        if (callback) {
	    callback(callbackData, cancelled, request, reqIds, 
	        responseCount, &rsp, &statusDetail, &rspIds);
	} else {
	    return COND_PushCondition(DIMSE_NULLKEY,
	        "DIMSE_moveProvider: no callback function"); 
        }
	
	if (cancelled) {
	    /* make sure */
	    rsp.Status = 
	      STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication;
	    if (rspIds != NULL) {
	        delete reqIds;
	        reqIds = NULL;
	    }
	}
	
	cond = DIMSE_sendMoveResponse(assoc, presIdCmd, request,
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
** $Log: dimmove.cc,v $
** Revision 1.2  1996-04-25 16:11:16  hewett
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
