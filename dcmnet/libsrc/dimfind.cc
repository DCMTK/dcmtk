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
** Module: dimfind
**
** Purpose: 
**	This file contains the routines which help with
**	query services using the C-FIND operation.
**
**	Module Prefix: DIMSE_
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 2000-02-23 15:12:34 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dimfind.cc,v $
** CVS/RCS Revision:	$Revision: 1.8 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
*/

/* 
** Include Files
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
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
 

CONDITION
DIMSE_findUser(
	/* in */
	T_ASC_Association *assoc, 
        T_ASC_PresentationContextID presID,
	T_DIMSE_C_FindRQ *request, DcmDataset *requestIdentifiers,
	DIMSE_FindUserCallback callback, void *callbackData,
	/* blocking info for response */
	T_DIMSE_BlockingMode blockMode, int timeout,
	/* out */
	T_DIMSE_C_FindRSP *response, DcmDataset **statusDetail)
{
    CONDITION cond;
    T_DIMSE_Message req, rsp;
    DIC_US msgId;
    int responseCount = 0;
    DcmDataset *rspIds = NULL;
    DIC_US status = STATUS_Pending;

    if (requestIdentifiers == NULL) {
        return COND_PushCondition(DIMSE_NULLKEY,
	    "DIMSE_findUser: No Request Identifiers Supplied");
    }

    bzero((char*)&req, sizeof(req));
    bzero((char*)&rsp, sizeof(rsp));

    req.CommandField = DIMSE_C_FIND_RQ;
    request->DataSetType = DIMSE_DATASET_PRESENT;
    req.msg.CFindRQ = *request;

    msgId = request->MessageID;

    cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &req,
					  NULL, requestIdentifiers, 
					  NULL, NULL);
    if (cond != DIMSE_NORMAL) {
	return cond;
    }

    /* receive responses */

    while (cond == DIMSE_NORMAL && DICOM_PENDING_STATUS(status)) {

	/* initialise the response to collect */
        bzero((char*)&rsp, sizeof(rsp));
	if (rspIds != NULL) {
	    delete rspIds;
	    rspIds = NULL;
	}

	cond = DIMSE_receiveCommand(assoc, blockMode, timeout, &presID, 
		&rsp, statusDetail);
	if (cond != DIMSE_NORMAL) {
	    return cond;
	}
        if (rsp.CommandField != DIMSE_C_FIND_RSP) {
	    return COND_PushCondition(DIMSE_UNEXPECTEDRESPONSE, 
		"DIMSE: Unexpected Response Command Field: 0x%x",
		(unsigned)rsp.CommandField);
        }
    
        *response = rsp.msg.CFindRSP;
	
        if (response->MessageIDBeingRespondedTo != msgId) {
	    return COND_PushCondition(DIMSE_UNEXPECTEDRESPONSE, 
		"DIMSE: Unexpected Response MsgId: %d (expected: %d)",
		response->MessageIDBeingRespondedTo, msgId);
        }

        status = response->DimseStatus;
	responseCount++;

	switch (status) {
	case STATUS_Pending:
	case STATUS_FIND_Pending_WarningUnsupportedOptionalKeys:
	    if (*statusDetail != NULL) {
	        DIMSE_warning(assoc, 
		    "findUser: Pending with statusDetail, ignoring detail");
                delete *statusDetail;
                *statusDetail = NULL;
	    }
	    if (response->DataSetType == DIMSE_DATASET_NULL) {
		DIMSE_warning(assoc, 
		    "findUser: Status Pending, but DataSetType==NULL");
		DIMSE_warning(assoc, 
		    "  Assuming response identifiers are present");
	    }
            cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout,
	    	&presID, &rspIds, NULL, NULL);
    	    if (cond != DIMSE_NORMAL) {
		return cond;
	    }

	    /* execute callback */
	    if (callback) {
	        callback(callbackData, request, responseCount, 
		    response, rspIds);
	    }
	    break;
	case STATUS_Success:
	    if (response->DataSetType != DIMSE_DATASET_NULL) {
	        DIMSE_warning(assoc, 
		    "findUser: Status Success, but DataSetType!=NULL");
		DIMSE_warning(assoc, 
		    "  Assuming no response identifiers are present");
	    }
	    break;
	default:
	    if (response->DataSetType != DIMSE_DATASET_NULL) {
		DIMSE_warning(assoc, 
		    "findUser: Status %s, but DataSetType!=NULL",
		    DU_cfindStatusString(status));
		DIMSE_warning(assoc, 
		    "  Assuming no response identifiers are present");
	    }
	    break;
	}
    }
    return cond;
}

CONDITION
DIMSE_sendFindResponse(T_ASC_Association * assoc, 
	T_ASC_PresentationContextID presID,
	T_DIMSE_C_FindRQ *request, 
	T_DIMSE_C_FindRSP *response, DcmDataset *rspIds,
	DcmDataset *statusDetail)
{
    CONDITION 	cond = DIMSE_NORMAL;
    DIC_US	dtype;
    T_DIMSE_Message rsp;

    bzero((char*)&rsp, sizeof(rsp));
    rsp.CommandField = DIMSE_C_FIND_RSP;
    rsp.msg.CFindRSP = *response;
    rsp.msg.CFindRSP.MessageIDBeingRespondedTo = request->MessageID;
    strcpy(rsp.msg.CFindRSP.AffectedSOPClassUID,
	    request->AffectedSOPClassUID);
    rsp.msg.CFindRSP.opts = O_FIND_AFFECTEDSOPCLASSUID;

    if (rspIds != NULL) {
	dtype = DIMSE_DATASET_PRESENT;
    } else {
	dtype = DIMSE_DATASET_NULL;
    }
    rsp.msg.CFindRSP.DataSetType = (T_DIMSE_DataSetType)dtype;

    cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &rsp, 
	statusDetail, rspIds, NULL, NULL);

    return cond;
}


CONDITION
DIMSE_findProvider(
	/* in */ 
	T_ASC_Association *assoc, 
	T_ASC_PresentationContextID presIdCmd,
	T_DIMSE_C_FindRQ *request,
	DIMSE_FindProviderCallback callback, void *callbackData,
	/* blocking info for data set */
	T_DIMSE_BlockingMode blockMode, int timeout)
{	
    CONDITION cond = DIMSE_NORMAL;
    T_ASC_PresentationContextID presIdData;
    T_DIMSE_C_FindRSP rsp;
    DcmDataset *statusDetail = NULL;
    DcmDataset *reqIds = NULL;
    DcmDataset *rspIds = NULL;
    OFBool cancelled = OFFalse;
    int responseCount = 0;

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
    rsp.DimseStatus = STATUS_Pending;
    
    while (cond == DIMSE_NORMAL && DICOM_PENDING_STATUS(rsp.DimseStatus)) {
	responseCount++;

	cond = DIMSE_checkForCancelRQ(assoc, presIdCmd, request->MessageID);
	if (cond == DIMSE_NORMAL) {
	    /* cancel received */
	    rsp.DimseStatus = 
	        STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest;
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
	        responseCount, &rsp, &rspIds, &statusDetail);
	} else {
	    return COND_PushCondition(DIMSE_NULLKEY,
	        "DIMSE_findProvider: no callback function"); 
        }
	
	if (cancelled) {
	    /* make sure */
	    rsp.DimseStatus = 
	        STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest;
	    if (rspIds != NULL) {
	        delete reqIds;
	        reqIds = NULL;
	    }
	}
	
	cond = DIMSE_sendFindResponse(assoc, presIdCmd, request,
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
    if (reqIds != NULL) delete reqIds;
    if (rspIds != NULL) delete rspIds;
       
    return cond;
}

/*
** CVS Log
** $Log: dimfind.cc,v $
** Revision 1.8  2000-02-23 15:12:34  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.7  2000/02/01 10:24:09  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.6  1998/08/10 08:53:44  meichel
** renamed member variable in DIMSE structures from "Status" to
**   "DimseStatus". This is required if dcmnet is used together with
**   <X11/Xlib.h> where Status is #define'd as int.
**
** Revision 1.5  1998/01/27 10:51:44  meichel
** Removed some unused variables, meaningless const modifiers
**   and unreached statements.
**
** Revision 1.4  1997/07/21 08:47:18  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.3  1996/04/25 16:11:15  hewett
** Added parameter casts to char* for bzero calls.  Replaced some declarations
** of DIC_UL with unsigned long (reduces mismatch problems with 32 & 64 bit
** architectures).  Added some protection to inclusion of sys/socket.h (due
** to MIPS/Ultrix).
**
** Revision 1.2  1996/04/22 10:02:59  hewett
** Corrected memory leak whereby response ids where not being deleted.
**
** Revision 1.1.1.1  1996/03/26 18:38:46  hewett
** Initial Release.
**
**
*/
