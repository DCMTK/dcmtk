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
** Module: dimstore
**
** Purpose: 
**	This file contains the routines which help with
**	storage services.
**
**	Module Prefix: DIMSE_
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 2000-11-10 16:25:03 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dimstore.cc,v $
** CVS/RCS Revision:	$Revision: 1.11 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
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
 
typedef struct {
    void *callbackData;
    T_DIMSE_StoreProgress *progress;
    T_DIMSE_C_StoreRQ *request;
    DIMSE_StoreUserCallback callback;
} DIMSE_PrivateUserContext;


static void 
privateUserCallback(void *callbackData, unsigned long bytes)
{
    DIMSE_PrivateUserContext *ctx;
    ctx = (DIMSE_PrivateUserContext*)callbackData;
    ctx->progress->state = DIMSE_StoreProgressing;
    ctx->progress->progressBytes = bytes;
    ctx->progress->callbackCount++;
    if (ctx->callback) {
        ctx->callback(ctx->callbackData, ctx->progress, ctx->request);
    }
}

CONDITION
DIMSE_storeUser(
	/* in */ 
	T_ASC_Association *assoc, T_ASC_PresentationContextID presId,
	T_DIMSE_C_StoreRQ *request,
	const char *imageFileName, DcmDataset *imageDataSet,
	DIMSE_StoreUserCallback callback, void *callbackData,
	/* blocking info for response */
	T_DIMSE_BlockingMode blockMode, int timeout,
	/* out */
	T_DIMSE_C_StoreRSP *response,
	DcmDataset **statusDetail,
        T_DIMSE_DetectedCancelParameters *checkForCancelParams,
        /* in */
        long imageFileTotalBytes)
{
    CONDITION cond;
    T_DIMSE_Message req, rsp;
    DIMSE_PrivateUserContext callbackCtx;
    DIMSE_ProgressCallback privCallback = NULL;
    T_DIMSE_StoreProgress progress;

    if (imageFileName == NULL && imageDataSet == NULL) {
        return COND_PushCondition(DIMSE_NULLKEY,
	    "DIMSE_storeUser: no imageFileName or imageDataSet provided");
    }
    
    bzero((char*)&req, sizeof(req));
    bzero((char*)&rsp, sizeof(rsp));

    req.CommandField = DIMSE_C_STORE_RQ;
    request->DataSetType = DIMSE_DATASET_PRESENT;
    req.msg.CStoreRQ = *request;

    /* set up callback routine */
    if (callback != NULL) {
        /* only if caller requires */
        privCallback = privateUserCallback;	/* function defined above */
	callbackCtx.callbackData = callbackData;
        progress.state = DIMSE_StoreBegin;
	progress.callbackCount = 1;
	progress.progressBytes = 0;
	if (imageFileTotalBytes > 0) progress.totalBytes = imageFileTotalBytes; 
	else
	{
          if (imageFileName != NULL) progress.totalBytes = DU_fileSize(imageFileName);
          else progress.totalBytes = dcmGuessModalityBytes(request->AffectedSOPClassUID);
        }
	callbackCtx.progress = &progress;
	callbackCtx.request = request;
        callbackCtx.callback = callback;
	/* execute initial callback */
	callback(callbackData, &progress, request);
    } else {
        privCallback = NULL;
    }
    
    /* set message with file or data set */
    if (imageFileName != NULL) {
        cond = DIMSE_sendMessageUsingFileData(assoc, presId, &req, 
	    NULL, imageFileName, privCallback, &callbackCtx);
    } else {
        cond = DIMSE_sendMessageUsingMemoryData(assoc, presId, &req, 
	    NULL, imageDataSet, privCallback, &callbackCtx);
    }
    
    if (cond != DIMSE_NORMAL) {
	return cond;
    }

    /* execute final callback */
    if (callback) {
        progress.state = DIMSE_StoreEnd;
	progress.callbackCount++;
	/* execute final callback */
	callback(callbackData, &progress, request);
    }

    /* receive response */
    if (checkForCancelParams != NULL) {
        checkForCancelParams->cancelEncountered = OFTrue;
    }

    do
    {
        T_ASC_PresentationContextID thisPresId = presId;
        cond = DIMSE_receiveCommand(assoc, blockMode, timeout, 
            &thisPresId, &rsp, statusDetail);
        if (cond != DIMSE_NORMAL) return cond;

        if (checkForCancelParams != NULL && rsp.CommandField == DIMSE_C_CANCEL_RQ)
        {
            checkForCancelParams->cancelEncountered = OFTrue;
            checkForCancelParams->req = rsp.msg.CCancelRQ;
            checkForCancelParams->presId = thisPresId;
        } else {
            if (rsp.CommandField != DIMSE_C_STORE_RSP) {
	        return COND_PushCondition(DIMSE_UNEXPECTEDRESPONSE, 
	        	"DIMSE: Unexpected Response Command Field: 0x%x",
	    	    (unsigned)rsp.CommandField);
            }
    
            *response = rsp.msg.CStoreRSP;          // BoundsChecker warning !?
	
            if (response->MessageIDBeingRespondedTo != request->MessageID) {
	        return COND_PushCondition(DIMSE_UNEXPECTEDRESPONSE, 
	    	    "DIMSE: Unexpected Response MsgId: %d (expected: %d)",
	    	    response->MessageIDBeingRespondedTo, request->MessageID);
            }
        }
    } while (checkForCancelParams != NULL && rsp.CommandField == DIMSE_C_CANCEL_RQ);
    
    return DIMSE_NORMAL;
}



CONDITION
DIMSE_sendStoreResponse(T_ASC_Association * assoc, 
	T_ASC_PresentationContextID presID,
	T_DIMSE_C_StoreRQ *request, /* send response to this request */
	T_DIMSE_C_StoreRSP *response, /* response structure */
	DcmDataset *statusDetail)
{
    CONDITION           cond = DIMSE_NORMAL;
    T_DIMSE_Message     rsp;

    bzero((char*)&rsp, sizeof(rsp));
    rsp.CommandField = DIMSE_C_STORE_RSP;
    response->MessageIDBeingRespondedTo = request->MessageID;
    strcpy(response->AffectedSOPClassUID, request->AffectedSOPClassUID);
    strcpy(response->AffectedSOPInstanceUID, request->AffectedSOPInstanceUID);
    response->opts = (O_STORE_AFFECTEDSOPCLASSUID | 
        O_STORE_AFFECTEDSOPINSTANCEUID);
    response->DataSetType = DIMSE_DATASET_NULL;
    rsp.msg.CStoreRSP = *response;

    cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &rsp, 
		statusDetail, NULL, NULL, NULL);

    return cond;
}


typedef struct {
    void *callbackData;
    T_DIMSE_StoreProgress *progress;
    T_DIMSE_C_StoreRQ *request;
    char *imageFileName; 
    DcmDataset **imageDataSet;
    T_DIMSE_C_StoreRSP *response;
    DcmDataset	**statusDetail;
    DIMSE_StoreProviderCallback	callback;
} DIMSE_PrivateProviderContext;

static void 
privateProviderCallback(void *callbackData, unsigned long bytes)
{
    DIMSE_PrivateProviderContext *ctx;
    ctx = (DIMSE_PrivateProviderContext*)callbackData;
    ctx->progress->state = DIMSE_StoreProgressing;
    ctx->progress->progressBytes = bytes;
    ctx->progress->callbackCount++;
    if (ctx->callback) {
        ctx->callback(ctx->callbackData, ctx->progress, ctx->request, 
	    ctx->imageFileName, ctx->imageDataSet, ctx->response,
	    ctx->statusDetail);
    }
}


CONDITION
DIMSE_storeProvider(/* in */ 
	T_ASC_Association *assoc, 
	T_ASC_PresentationContextID presIdCmd,
	T_DIMSE_C_StoreRQ *request,
	const char* imageFileName, int writeMetaheader,
	DcmDataset **imageDataSet,
	DIMSE_StoreProviderCallback callback, void *callbackData,
	/* blocking info for data set */
	T_DIMSE_BlockingMode blockMode, int timeout)
{	
    CONDITION cond = DIMSE_NORMAL;
    DIMSE_PrivateProviderContext callbackCtx;
    DIMSE_ProgressCallback privCallback = NULL;
    T_ASC_PresentationContextID presIdData = 0;
    T_DIMSE_C_StoreRSP response;
    DcmDataset *statusDetail = NULL;
    T_DIMSE_StoreProgress progress;

    bzero((char*)&response, sizeof(response));
    response.DimseStatus = STATUS_Success;	/* assume */
    response.MessageIDBeingRespondedTo = request->MessageID;
    response.DataSetType = DIMSE_DATASET_NULL;	/* always for C-STORE-RSP */
    strcpy(response.AffectedSOPClassUID, request->AffectedSOPClassUID);
    strcpy(response.AffectedSOPInstanceUID, request->AffectedSOPInstanceUID);
    response.opts = (O_STORE_AFFECTEDSOPCLASSUID | 
        O_STORE_AFFECTEDSOPINSTANCEUID);

    /* set up callback routine */
    if (callback != NULL) {
        /* only if caller requires */
        privCallback = privateProviderCallback;	/* function defined above */
	callbackCtx.callbackData = callbackData;
        progress.state = DIMSE_StoreBegin;
	progress.callbackCount = 1;
	progress.progressBytes = 0;
        progress.totalBytes = dcmGuessModalityBytes(request->AffectedSOPClassUID);
	callbackCtx.progress = &progress;
	callbackCtx.request = request;
	callbackCtx.imageFileName = (char*)imageFileName;
	callbackCtx.imageDataSet = imageDataSet;
	callbackCtx.response = &response;
	callbackCtx.statusDetail = &statusDetail;
        callbackCtx.callback = callback;
	/* execute initial callback */
	callback(callbackData, &progress, request, 
	    (char*)imageFileName, imageDataSet,
	    &response, &statusDetail);
    } else {
        privCallback = NULL;
    }
    
    if (imageFileName != NULL) {
        DcmFileStream *filestream = NULL;
        if (DIMSE_NORMAL != (cond = DIMSE_createFilestream(imageFileName, request, assoc, 
          presIdCmd, writeMetaheader, &filestream)))
        {
          return cond;
        } else {
          cond = DIMSE_receiveDataSetInFile(assoc, blockMode, timeout,
          &presIdData, filestream, privCallback, &callbackCtx);
          delete filestream;
          if (cond != DIMSE_NORMAL)
          {
            if (strcpy(imageFileName, NULL_DEVICE_NAME) != 0) unlink(imageFileName);
          }
        }
    } else if (imageDataSet != NULL) {
        cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout,
		&presIdData, imageDataSet, privCallback, &callbackCtx);
    } else {
 	  return COND_PushCondition(DIMSE_BADDATA, 
		"DIMSE_storeProvider: No filename or DcmDataset provided");
    }

    if (presIdData != presIdCmd) {
	cond = COND_PushCondition(DIMSE_INVALIDPRESENTATIONCONTEXTID, 
		"DIMSE: Presentation Contexts of Command and Data Differ");
    }

    if (cond == DIMSE_NORMAL) {
        response.DimseStatus = STATUS_Success;
    } else if (cond == DIMSE_OUTOFRESOURCES) {
        response.DimseStatus = STATUS_STORE_Refused_OutOfResources;
    } else {
        return cond;
    }
   
    /* execute final callback (user does not have to provide callback) */
    if (callback) {
        progress.state = DIMSE_StoreEnd;
	progress.callbackCount++;
	/* execute final callback */
	callback(callbackData, &progress, request, 
	    (char*)imageFileName, imageDataSet,
	    &response, &statusDetail);
    }
    
    cond = DIMSE_sendStoreResponse(assoc, presIdCmd, request, 
        &response, statusDetail);
    
    return cond;
}

/*
** CVS Log
** $Log: dimstore.cc,v $
** Revision 1.11  2000-11-10 16:25:03  meichel
** Fixed problem with DIMSE routines which attempted to delete /dev/null
**   under certain circumstances, which could lead to disastrous results if
**   tools were run with root permissions (what they shouldn't).
**
** Revision 1.10  2000/06/07 08:58:13  meichel
** added optional paramter to DIMSE_storeUser that enables precise file size
**   information inside the store user callback.
**
** Revision 1.9  2000/03/02 12:46:34  joergr
** Rewrote some memory related statements (memcpy, strcpy, etc.) to avoid
** warnings reported by BoundsChecker.
**
** Revision 1.8  2000/02/23 15:12:39  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.7  2000/02/03 11:50:12  meichel
** Moved UID related functions from dcmnet (diutil.h) to dcmdata (dcuid.h)
**   where they belong. Renamed access functions to dcmSOPClassUIDToModality
**   and dcmGuessModalityBytes.
**
** Revision 1.6  2000/02/01 10:24:11  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.5  1999/04/19 08:36:31  meichel
** Added support for C-FIND-CANCEL/C-MOVE-CANCEL in DIMSE_storeUser().
**
** Revision 1.4  1998/08/10 08:53:46  meichel
** renamed member variable in DIMSE structures from "Status" to
**   "DimseStatus". This is required if dcmnet is used together with
**   <X11/Xlib.h> where Status is #define'd as int.
**
** Revision 1.3  1997/05/23 10:45:28  meichel
** Major rewrite of storescp application. See CHANGES for details.
** Changes required to interfaces of some DIMSE functions.
**
** Revision 1.2  1996/04/25 16:11:17  hewett
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
