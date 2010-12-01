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
** Module: dimstore
**
** Purpose:
**      This file contains the routines which help with
**      storage services.
**
** Module Prefix: DIMSE_
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 2010-12-01 08:26:36 $
** CVS/RCS Revision:    $Revision: 1.23 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
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
#include "dcmtk/dcmnet/dimse.h"		/* always include the module header */
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/dcmdata/dcostrmf.h"    /* for class DcmOutputFileStream */
#include "dcmtk/ofstd/ofstd.h"         /* for OFStandard::getFileSize() */


/* Global flag to enable/disable workaround code for some buggy Store SCUs
 * in DIMSE_storeProvider().  If enabled, an illegal space-padding in the
 * Affected SOP Instance UID field of the C-STORE-RQ message is retained
 * in the corresponding C-STORE-RSP message.
 * To enable the workaround, this flag must be set to OFTrue and
 * dcmEnableAutomaticInputDataCorrection must be set to OFFalse.
 * (see declaration in dcmdata/include/dcobject.h)
 */
OFGlobal<OFBool> dcmPeerRequiresExactUIDCopy(OFFalse);


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

OFCondition
DIMSE_storeUser(
	T_ASC_Association *assoc, T_ASC_PresentationContextID presId,
	T_DIMSE_C_StoreRQ *request,
	const char *imageFileName, DcmDataset *imageDataSet,
	DIMSE_StoreUserCallback callback, void *callbackData,
	T_DIMSE_BlockingMode blockMode, int timeout,
	T_DIMSE_C_StoreRSP *response,
	DcmDataset **statusDetail,
        T_DIMSE_DetectedCancelParameters *checkForCancelParams,
        long imageFileTotalBytes)
    /*
     * This function transmits data from a file or a dataset to an SCP. The transmission is
     * conducted via network and using DIMSE C-STORE messages. Additionally, this function
     * evaluates C-STORE-Response messages which were received from the SCP.
     *
     * Parameters:
     *   assoc                - [in] The association (network connection to SCP).
     *   presId               - [in] The ID of the presentation context which shall be used
     *   request              - [in] Represents a DIMSE C-Store Request Message. Contains corresponding
     *                               information, e.g. message ID, affected SOP class UID, etc.
     *   imageFileName        - [in] The name of the file which is currently processed.
     *   imageDataSet         - [in] The data set which is currently processed.
     *   callback             - [in] Pointer to a function which shall be called to indicate progress.
     *   callbackData         - [in] Pointer to data which shall be passed to the progress indicating function
     *   blockMode            - [in] The blocking mode for receiving data (either DIMSE_BLOCKING or DIMSE_NONBLOCKING)
     *   timeout              - [in] Timeout interval for receiving data. If the blocking mode is DIMSE_NONBLOCKING
     *   response             - [out] Represents a DIMSE C-Store Response Message. Contains corresponding
     *                                information, e.g. message ID being responded to, affected SOP class UID, etc.
     *                                This variable contains in the end the C-STORE-RSP command which was received
     *                                as a response to the C-STORE-RQ which was sent.
     *   statusDetail         - [out] If a non-NULL value is passed this variable will in the end contain detailed
     *                                information with regard to the status information which is captured in the status
     *                                element (0000,0900) of the response message. Note that the value for element (0000,0900)
     *                                is not contained in this return value but in response.
     *   checkForCancelParams - [out] Indicates, if a C-Cancel (Request) Message was encountered. Contains corresponding
     *                                information, e.g. a boolean value if a corresponding message was encountered and the
     *                                C-Cancel (Request) Message itself (in case it actually was encountered).
     *   imageFileTotalBytes  - [in] The size of the file which is currently processed in bytes.
     */
{
    OFCondition cond = EC_Normal;
    T_DIMSE_Message req, rsp;
    DIMSE_PrivateUserContext callbackCtx;
    DIMSE_ProgressCallback privCallback = NULL;
    T_DIMSE_StoreProgress progress;

    /* if there is no image file or no data set, no data can be sent */
    if (imageFileName == NULL && imageDataSet == NULL) return DIMSE_NULLKEY;

    /* initialize the variables which represent DIMSE C-STORE request and DIMSE C-STORE response messages */
    bzero((char*)&req, sizeof(req));
    bzero((char*)&rsp, sizeof(rsp));

    /* set corresponding values in the request message variable */
    req.CommandField = DIMSE_C_STORE_RQ;
    request->DataSetType = DIMSE_DATASET_PRESENT;
    req.msg.CStoreRQ = *request;

    /* set up callback routine which is used to indicate progress */
    if (callback != NULL) {
        /* in case the caller indicated that he has his own progress indicating */
        /* function set some variables correspondingly so that this particular */
        /* function will be called whenever progress shall be indicated. */
        privCallback = privateUserCallback;	/* function defined above */
	callbackCtx.callbackData = callbackData;
        progress.state = DIMSE_StoreBegin;
	progress.callbackCount = 1;
	progress.progressBytes = 0;
	if (imageFileTotalBytes > 0) progress.totalBytes = imageFileTotalBytes;
	else
	{
          if (imageFileName != NULL) progress.totalBytes = OFStandard::getFileSize(imageFileName);
          else progress.totalBytes = dcmGuessModalityBytes(request->AffectedSOPClassUID);
        }
	callbackCtx.progress = &progress;
	callbackCtx.request = request;
        callbackCtx.callback = callback;
	/* execute initial callback */
	callback(callbackData, &progress, request);
    } else {
        /* in case the caller does not have his own progress indicating function no */
        /* corresponding function will be called when progress shall be indicated. */
        privCallback = NULL;
    }

    /* send C-STORE-RQ message and instance data using file data or data set */
    if (imageFileName != NULL) {
        cond = DIMSE_sendMessageUsingFileData(assoc, presId, &req,
	    NULL, imageFileName, privCallback, &callbackCtx);
    } else {
        cond = DIMSE_sendMessageUsingMemoryData(assoc, presId, &req,
	    NULL, imageDataSet, privCallback, &callbackCtx);
    }

    if (cond != EC_Normal) {
	return cond;
    }

    /* execute final callback */
    if (callback) {
        progress.state = DIMSE_StoreEnd;
	progress.callbackCount++;
	/* execute final callback */
	callback(callbackData, &progress, request);
    }

    /* check if a C-CANCEL-RQ message was encountered earlier */
    if (checkForCancelParams != NULL) {
        checkForCancelParams->cancelEncountered = OFFalse;
    }

    /* try to receive C-STORE-RSP */
    do
    {
        /* remember the ID of the presentation context in a local variable */
        T_ASC_PresentationContextID thisPresId = presId;

        /* try to receive a C-STORE-RSP over the network. */
        cond = DIMSE_receiveCommand(assoc, blockMode, timeout,
            &thisPresId, &rsp, statusDetail);
        if (cond != EC_Normal) return cond;

        /* if everything was successful so far, the rsp variable contains the command which */
        /* was received check if we encountered a C-CANCEL-RQ; if so, set some variables */
        if (checkForCancelParams != NULL && rsp.CommandField == DIMSE_C_CANCEL_RQ)
        {
            checkForCancelParams->cancelEncountered = OFTrue;
            checkForCancelParams->req = rsp.msg.CCancelRQ;
            checkForCancelParams->presId = thisPresId;
        } else {
        /* if we did not receive a C-CANCEL-RQ */

            /* if we did also not encounter a C-STORE-RSP, something is wrong */
            if (rsp.CommandField != DIMSE_C_STORE_RSP)
            {
              char buf[256];
              sprintf(buf, "DIMSE: Unexpected Response Command Field: 0x%x", (unsigned)rsp.CommandField);
              return makeDcmnetCondition(DIMSEC_UNEXPECTEDRESPONSE, OF_error, buf);
            }

            /* if we get to here, we received a C-STORE-RSP; store this message in the reference parameter */
            *response = rsp.msg.CStoreRSP;          // BoundsChecker warning !?

            /* check if the response relates to the request which was sent earlier; if not, return an error */
            if (response->MessageIDBeingRespondedTo != request->MessageID)
            {
              char buf2[256];
              sprintf(buf2, "DIMSE: Unexpected Response MsgId: %d (expected: %d)", response->MessageIDBeingRespondedTo, request->MessageID);
              return makeDcmnetCondition(DIMSEC_UNEXPECTEDRESPONSE, OF_error, buf2);
            }
        }
    } while (checkForCancelParams != NULL && rsp.CommandField == DIMSE_C_CANCEL_RQ);

    /* return result value */
    return EC_Normal;
}



OFCondition
DIMSE_sendStoreResponse(T_ASC_Association * assoc,
	T_ASC_PresentationContextID presID,
	T_DIMSE_C_StoreRQ *request,
	T_DIMSE_C_StoreRSP *response,
	DcmDataset *statusDetail)
    /*
     * This function takes care of sending a C-STORE-RSP message over the network to the DICOM
     * application this application is connected with.
     *
     * Parameters:
     *   assoc        - [in] The association (network connection to another DICOM application).
     *   presID       - [in] The ID of the presentation context which was specified in the PDV
     *                       which contained the DIMSE C-STORE-RQ command.
     *   request      - [in] The DIMSE C-STORE-RQ command which was received earlier.
     *   response     - [inout] The C-STORE-RSP command which shall be sent. Might be modified.
     *   statusDetail - [in] Detailed information with regard to the status information which is captured
     *                       in the status element (0000,0900). Note that the value for element (0000,0900)
     *                       is contained in this variable.
     */
{
    OFCondition           cond = EC_Normal;
    T_DIMSE_Message     rsp;

    /* create response message */
    bzero((char*)&rsp, sizeof(rsp));
    rsp.CommandField = DIMSE_C_STORE_RSP;
    response->MessageIDBeingRespondedTo = request->MessageID;
    strcpy(response->AffectedSOPClassUID, request->AffectedSOPClassUID);
    strcpy(response->AffectedSOPInstanceUID, request->AffectedSOPInstanceUID);
    response->opts = (O_STORE_AFFECTEDSOPCLASSUID |
        O_STORE_AFFECTEDSOPINSTANCEUID);
    response->DataSetType = DIMSE_DATASET_NULL;
    rsp.msg.CStoreRSP = *response;

    /* send response message over the network */
    cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &rsp,
		statusDetail, NULL, NULL, NULL);

    /* return reult value */
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


OFCondition
DIMSE_storeProvider( T_ASC_Association *assoc,
	T_ASC_PresentationContextID presIdCmd,
	T_DIMSE_C_StoreRQ *request,
	const char* imageFileName, int writeMetaheader,
	DcmDataset **imageDataSet,
	DIMSE_StoreProviderCallback callback, void *callbackData,
	T_DIMSE_BlockingMode blockMode, int timeout)
    /*
     * This function receives a data set over the network and either stores this data in a file (exactly as it was
     * received) or it stores this data in memory. Before, during and after the process of receiving data, the callback
     * function which was provided by the caller (if it was provided) will be called to indicate progress.
     *
     * Parameters:
     *   assoc           - [in] The association (network connection to another DICOM application).
     *   presIDCmd       - [in] The ID of the presentation context which was specified in the PDV which contained
     *                          the DIMSE command.
     *   request         - [in] The DIMSE C-STORE-RQ message that was received.
     *   imageFileName   - [in] If this variable does not equal NULL, the information (which was received over the network)
     *                          will be written to a file exactly as it was received over the network. In such a case, this
     *                          this variable contains the name of the file the information shall be written to.
     *   writeMetaheader - [in] Specifies if the resulting file shall only contain the dataset which was received
     *                          (OFFalse) or if it shall contain both metaheader and dataset information (OFTrue)
     *                          (i.e if the file will be written according to the DICOM file format).
     *   imageDataSet    - [inout] If this variable does not equal NULL, and at the same time imageFileName equals NULL,
     *                          this variable will in the end contain the information which was received over the network.
     *                          Note that this function assumes that either imageFileName or imageDataSet does not equal NULL.
     *   callback        - [in] Pointer to a function which shall be called to indicate progress.
     *   callbackData    - [in] Pointer to data which shall be passed to the progress indicating function
     *   blockMode       - [in] The blocking mode for receiving data (either DIMSE_BLOCKING or DIMSE_NONBLOCKING)
     *   timeout         - [in] Timeout interval for receiving data (if the blocking mode is DIMSE_NONBLOCKING).
     */
{
    OFCondition cond = EC_Normal;
    DIMSE_PrivateProviderContext callbackCtx;
    DIMSE_ProgressCallback privCallback = NULL;
    T_ASC_PresentationContextID presIdData = 0;
    T_DIMSE_C_StoreRSP response;
    DcmDataset *statusDetail = NULL;
    T_DIMSE_StoreProgress progress;

    /* initialize the C-STORE-RSP message variable */
    bzero((char*)&response, sizeof(response));
    response.DimseStatus = STATUS_Success;	/* assume */
    response.MessageIDBeingRespondedTo = request->MessageID;
    response.DataSetType = DIMSE_DATASET_NULL;	/* always for C-STORE-RSP */
    strcpy(response.AffectedSOPClassUID, request->AffectedSOPClassUID);
    strcpy(response.AffectedSOPInstanceUID, request->AffectedSOPInstanceUID);
    response.opts = (O_STORE_AFFECTEDSOPCLASSUID | O_STORE_AFFECTEDSOPINSTANCEUID);
    if (request->opts & O_STORE_RQ_BLANK_PADDING) response.opts |= O_STORE_RSP_BLANK_PADDING;
    if (dcmPeerRequiresExactUIDCopy.get()) response.opts |= O_STORE_PEER_REQUIRES_EXACT_UID_COPY;

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

    /* in the following, we want to receive data over the network and do something with this data. If the */
    /* imageFileName does not equal NULL, the caller required that the data shall be written to a file */
    /* exactly the way it was received over the network. Hence, a filestream will be created and the data */
    /* set will be received and written to the file through the call to DIMSE_receiveDataSetInFile(...).*/
    /* If the imageFileName does equal NULL but at the same time imageDataSet does not equal NULL, the */
    /* data shall be received and stored in memory. This will be handled through the call to function */
    /* DIMSE_receiveDataSetInMemory(...). The case in which both variables are NULL is considered to */
    /* be an error and will be handled correspondingly. */
    if (imageFileName != NULL) {
        /* create filestream */
        DcmOutputFileStream *filestream = NULL;
        cond = DIMSE_createFilestream(imageFileName, request, assoc, presIdCmd, writeMetaheader, &filestream);
        if (cond.bad())
        {
          /* We cannot create the filestream, so ignore the incoming dataset and return an out-of-resources error to the SCU */
          DIC_UL bytesRead = 0;
          DIC_UL pdvCount=0;
          cond = DIMSE_ignoreDataSet(assoc, blockMode, timeout, &bytesRead, &pdvCount);
          if (cond.good())
          {
            OFString s = "DIMSE_storeProvider: Cannot create file: ";
            s += imageFileName;
            cond = makeDcmnetCondition(DIMSEC_OUTOFRESOURCES, OF_error, s.c_str());
          }
        } else {
          /* if no error occured, receive data and write it to the file */
          cond = DIMSE_receiveDataSetInFile(assoc, blockMode, timeout, &presIdData, filestream, privCallback, &callbackCtx);
          delete filestream;
          if (cond != EC_Normal)
          {
            if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0) unlink(imageFileName);
          }
        }
    }
    else if (imageDataSet != NULL)
    {
        /* receive data and store it in memory */
        cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout,
		&presIdData, imageDataSet, privCallback, &callbackCtx);
    } else {
        /* if both variables are set to NULL, report an error */
 	return DIMSE_BADDATA;
    }

    /* check if presentation context IDs of the command (which was received earlier) and of the data */
    /* set (which was received just now) differ from each other. If this is the case, return an error. */
    if (cond.good() && (presIdData != presIdCmd))
    {
    	cond = makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error, "DIMSE: Presentation Contexts of Command and Data Differ");
    }

    /* depending on the error status, set the success indicating flag in the response message */
    if (cond == EC_Normal) {
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

    /* send a C-STORE-RSP message over the network to the other DICOM application */
    OFCondition cond2 = DIMSE_sendStoreResponse(assoc, presIdCmd, request, &response, statusDetail);

    /* if we already had an error condition, don't overwrite */
    if (cond.good()) cond = cond2;

    /* return result value */
    return cond;
}

/*
** CVS Log
** $Log: dimstore.cc,v $
** Revision 1.23  2010-12-01 08:26:36  joergr
** Added OFFIS copyright header (beginning with the year 1994).
**
** Revision 1.22  2010-10-14 13:14:28  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.21  2010-05-21 11:47:52  uli
** Replaced DU_fileSize() with OFStandard::getFileSize().
**
** Revision 1.20  2006-06-23 10:09:17  meichel
** Fixed incorrect default initialization of C-GET cancel struct.
**
** Revision 1.19  2005/12/08 15:44:46  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.18  2005/02/22 09:40:58  meichel
** Fixed two bugs in "bit-preserving" Store SCP code. Errors while creating or
**   writing the DICOM file (e.g. file system full) now result in a DIMSE error
**   response (out of resources) being sent back to the SCU.
**
** Revision 1.17  2002/11/27 13:04:42  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.16  2002/08/27 17:00:52  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.15  2001/11/01 13:49:04  wilkens
** Added lots of comments.
**
** Revision 1.14  2001/10/12 10:18:36  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.13  2000/12/15 13:28:18  meichel
** Global flag to enable/disable workaround code for some buggy Store SCUs
**   in DIMSE_storeProvider().  If enabled, an illegal space-padding in the
**   Affected SOP Instance UID field of the C-STORE-RQ message is retained
**   in the corresponding C-STORE-RSP message.
**
** Revision 1.12  2000/11/10 18:07:45  meichel
** Mixed up strcmp and strcpy - oops.
**
** Revision 1.11  2000/11/10 16:25:03  meichel
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
