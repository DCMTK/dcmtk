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
** Module: dimse
**
** Purpose: 
**	This file contains the routines which provide dimse layer services
**	for DICOM V.3 applications.  
**
**	Module Prefix: DIMSE_
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1998-01-28 17:38:13 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dimse.cc,v $
** CVS/RCS Revision:	$Revision: 1.14 $
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
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_UNIX_H
#if defined(macintosh) && defined (HAVE_WINSOCK_H)
/* unix.h defines timeval incompatible with winsock.h */
#define timeval _UNWANTED_timeval
#endif
#include <unix.h>	/* for unlink() under Metrowerks C++ (Macintosh) */
#undef timeval
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
#include <errno.h>

#include "diutil.h"
#include "dimse.h"		/* always include the module header */
#include "dimcond.h"
#include "dimcmd.h"

#include "dctk.h"

/*
 * Type definitions
 */

/*
 * Useful Macros
 */


/*
 * Global variables (should be used very, very rarely).
 */

static int debug = 0;

/*
** Private Functions Prototypes
*/

/*
** Private Functions Bodies
*/


static OFBool
isDataDictPresent()
{
    /* is a data dictionary present */
    return dcmDataDict.isDictionaryLoaded();
}

/*
 * PDV Reading
 */

static CONDITION
DIMSE_readNextPDV(T_ASC_Association * assoc,
    T_DIMSE_BlockingMode blocking,
    int timeout,
    DUL_PDV * pdv)
{
    CONDITION cond;
    DUL_BLOCKOPTIONS blk;

    /*
     * NOTE: DUL currently ignores blocking and timeout so do it here! 
     */

    if (blocking == DIMSE_NONBLOCKING) {
	if (!ASC_dataWaiting(assoc, timeout))
	    return COND_PushCondition(DIMSE_NODATAAVAILABLE,
		DIMSE_Message(DIMSE_NODATAAVAILABLE));
    }
    blk = (blocking == DIMSE_BLOCKING) ? (DUL_BLOCK) : (DUL_NOBLOCK);

    cond = DUL_NextPDV(&assoc->DULassociation, pdv);

    if (cond != DUL_NORMAL) {
	/* there is no pdv waiting to be picked up */

#ifdef PUT_DUL_NOPDVS_ON_CONDITION_STACK
        /* see DUL_NextPDV in dul.cc for a description */
	COND_PopCondition(OFFalse);
#endif

	/* try to read new pdv's */
	cond = DUL_ReadPDVs(&assoc->DULassociation, NULL, blk, timeout);

	if (cond != DUL_PDATAPDUARRIVED) {
	    if (cond == DUL_NULLKEY || cond == DUL_ILLEGALKEY)
		return COND_PushCondition(DIMSE_ILLEGALASSOCIATION,
		    DIMSE_Message(DIMSE_ILLEGALASSOCIATION));
	    else if (cond == DUL_PEERREQUESTEDRELEASE)
		return COND_PushCondition(DIMSE_PEERREQUESTEDRELEASE,
		    DIMSE_Message(DIMSE_PEERREQUESTEDRELEASE));
	    else if (cond == DUL_PEERABORTEDASSOCIATION ||
		cond == DUL_PEERDROPPEDASSOCIATION)
		return COND_PushCondition(DIMSE_PEERABORTEDASSOCIATION,
		    DIMSE_Message(DIMSE_PEERABORTEDASSOCIATION));
	    else
		return COND_PushCondition(DIMSE_READPDVFAILED,
		    DIMSE_Message(DIMSE_READPDVFAILED));
	}
	cond = DUL_NextPDV(&assoc->DULassociation, pdv);
	if (cond != DUL_NORMAL) {
	    return COND_PushCondition(DIMSE_READPDVFAILED,
		DIMSE_Message(DIMSE_READPDVFAILED));
	}
    }
    return DIMSE_NORMAL;
}

/*
 * Checking and Validation
 */


static CONDITION
getTransferSyntax(T_ASC_Association * assoc, 
	T_ASC_PresentationContextID pid,
	E_TransferSyntax *xferSyntax)
{
    CONDITION cond = DIMSE_NORMAL;
    CONDITION acond = ASC_NORMAL;
    T_ASC_PresentationContext pc;
    char *ts = NULL;

    /* is this a valid presentation context ? */
    acond = ASC_findAcceptedPresentationContext(assoc->params, pid, &pc);
    if (acond != ASC_NORMAL) {
	cond = DIMSE_INVALIDPRESENTATIONCONTEXTID;
	COND_PushCondition(cond, DIMSE_Message(cond), pid);
	return DIMSE_RECEIVEFAILED;
    }
    ts = pc.acceptedTransferSyntax;
    
    DcmXfer xfer(ts);
    *xferSyntax = xfer.getXfer();

    switch (*xferSyntax) {
    case EXS_LittleEndianImplicit:
    case EXS_LittleEndianExplicit:
    case EXS_BigEndianExplicit:
    	/* OK, these can be supported */
    	break;
    default:
	cond = DIMSE_UNSUPPORTEDTRANSFERSYNTAX;
	COND_PushCondition(cond, DIMSE_Message(cond), ts);
	cond = DIMSE_RECEIVEFAILED;
	break;
    }
    return cond;
}


static CONDITION
checkPresentationContextForMessage(T_ASC_Association * assoc,
	T_DIMSE_Message * msg, T_ASC_PresentationContextID presID,
	E_TransferSyntax * xferSyntax)
{
    CONDITION cond;
    /* char *as; */	/* abstract syntax */
    /* char *ts; */	/* transfer syntax */
    T_ASC_PresentationContext pc;


    cond = ASC_findAcceptedPresentationContext(assoc->params, presID, &pc);
    if (cond != ASC_NORMAL) {
	cond = DIMSE_INVALIDPRESENTATIONCONTEXTID;
    } else {
	cond = DIMSE_NORMAL;
    }

    /*
     * as = pc.abstractSyntax;
     * ts = pc.acceptedTransferSyntax;
     */
     
    if (cond == DIMSE_NORMAL) {

    switch (msg->CommandField) {
	case DIMSE_C_ECHO_RQ:
	case DIMSE_C_ECHO_RSP:
	case DIMSE_C_STORE_RQ:
	case DIMSE_C_STORE_RSP:
	case DIMSE_C_GET_RQ:
	case DIMSE_C_GET_RSP:
	case DIMSE_C_FIND_RQ:
	case DIMSE_C_FIND_RSP:
	case DIMSE_C_MOVE_RQ:
	case DIMSE_C_MOVE_RSP:
	case DIMSE_C_CANCEL_RQ:
	case DIMSE_N_EVENT_REPORT_RQ:
	case DIMSE_N_EVENT_REPORT_RSP:
	case DIMSE_N_GET_RQ:
	case DIMSE_N_GET_RSP:
	case DIMSE_N_SET_RQ:
	case DIMSE_N_SET_RSP:
	case DIMSE_N_ACTION_RQ:
	case DIMSE_N_ACTION_RSP:
	case DIMSE_N_CREATE_RQ:
	case DIMSE_N_CREATE_RSP:
	case DIMSE_N_DELETE_RQ:
	case DIMSE_N_DELETE_RSP:
	    break;
    
	default:
	    cond = DIMSE_BADCOMMANDTYPE;
	    break;
	}
    }

    if (cond == DIMSE_NORMAL) {
	cond = getTransferSyntax(assoc, presID, xferSyntax);
    }
    if (cond != DIMSE_NORMAL)
	return COND_PushCondition(cond, DIMSE_Message(cond));
    return DIMSE_NORMAL;
}

static CONDITION
validateMessage(T_ASC_Association *assoc, T_DIMSE_Message *msg)
{
    CONDITION cond = DIMSE_NORMAL;

    switch (msg->CommandField) {
    case DIMSE_C_ECHO_RQ:
	if (msg->msg.CEchoRQ.DataSetType != DIMSE_DATASET_NULL) {
	    DIMSE_warning(assoc, "CEchoRQ: DataSetType != NULL");
	    cond = DIMSE_BADMESSAGE;
	}
	break;
    case DIMSE_C_ECHO_RSP:
	if (msg->msg.CEchoRSP.DataSetType != DIMSE_DATASET_NULL) {
	    DIMSE_warning(assoc, "CEchoRSP: DataSetType != NULL");
	    cond = DIMSE_BADMESSAGE;
	}
	break;
    case DIMSE_C_STORE_RQ:
	if (msg->msg.CStoreRQ.DataSetType == DIMSE_DATASET_NULL) {
	    DIMSE_warning(assoc, "CStoreRQ: DataSetType == NULL");
	    cond = DIMSE_BADMESSAGE;
	}
	if (! IN_RANGE(strlen(msg->msg.CStoreRQ.AffectedSOPInstanceUID), 
		1, DIC_UI_LEN)) {
	    DIMSE_warning(assoc, "CStoreRQ: AffectedSOPInstanceUID: bad size");
	    cond = DIMSE_BADMESSAGE;
	}
	break;
    case DIMSE_C_STORE_RSP:
	if (msg->msg.CStoreRSP.DataSetType != DIMSE_DATASET_NULL) {
	    DIMSE_warning(assoc, "CStoreRSP: DataSetType != NULL");
	    cond = DIMSE_BADMESSAGE;
	}
	if ((msg->msg.CStoreRSP.opts & O_STORE_AFFECTEDSOPINSTANCEUID) &&
	    (! IN_RANGE(strlen(msg->msg.CStoreRSP.AffectedSOPInstanceUID), 
		1, DIC_UI_LEN))) {
	    DIMSE_warning(assoc, "CStoreRSP: AffectedSOPInstanceUID: bad size");
	    cond = DIMSE_BADMESSAGE;
	}
	break;
    case DIMSE_C_GET_RQ:
	if (msg->msg.CGetRQ.DataSetType == DIMSE_DATASET_NULL) {
	    DIMSE_warning(assoc, "CGetRQ: DataSetType == NULL");
	    cond = DIMSE_BADMESSAGE;
	}
	break;
    case DIMSE_C_GET_RSP:
	/* data set can be empty or present */
	break;
    case DIMSE_C_FIND_RQ:
	if (msg->msg.CFindRQ.DataSetType == DIMSE_DATASET_NULL) {
	    DIMSE_warning(assoc, "CFindRQ: DataSetType == NULL");
	    cond = DIMSE_BADMESSAGE;
	}
	break;
    case DIMSE_C_FIND_RSP:
	/* data set can be empty or present */
	break;
    case DIMSE_C_MOVE_RQ:
	if (msg->msg.CMoveRQ.DataSetType == DIMSE_DATASET_NULL) {
	    DIMSE_warning(assoc, "CMoveRQ: DataSetType == NULL");
	    cond = DIMSE_BADMESSAGE;
	}
	break;
    case DIMSE_C_MOVE_RSP:
	/* data set can be empty or present */
	break;
    case DIMSE_C_CANCEL_RQ:
	if (msg->msg.CCancelRQ.DataSetType != DIMSE_DATASET_NULL) {
	    DIMSE_warning(assoc, "CCancelRQ: DataSetType != NULL");
	    cond = DIMSE_BADMESSAGE;
	}
	break;

    case DIMSE_N_EVENT_REPORT_RQ:
    case DIMSE_N_EVENT_REPORT_RSP:
    case DIMSE_N_GET_RQ:
    case DIMSE_N_GET_RSP:
    case DIMSE_N_SET_RQ:
    case DIMSE_N_SET_RSP:
    case DIMSE_N_ACTION_RQ:
    case DIMSE_N_ACTION_RSP:
    case DIMSE_N_CREATE_RQ:
    case DIMSE_N_CREATE_RSP:
    case DIMSE_N_DELETE_RQ:
    case DIMSE_N_DELETE_RSP:
	/* No checking on the normalized messages yet, assume ok */
	break;

    default:
	cond = DIMSE_BADCOMMANDTYPE;
	break;
    }

    if (cond != DIMSE_NORMAL) 
	return COND_PushCondition(cond, 
	    "Invalid Message: %s", DIMSE_Message(cond));

    return DIMSE_NORMAL;
}


/*
 * Message sending support routines
 */

#if 0
/*** Currently Unused */
/*
** SendStraightFileData does not work for files with a Metaheader.
** It would need to be rewritten to read data after the Metaheader.
** However, finding out where the Metaheader ends means parsing the
** whole file in the current implementation.  It is thus not worth
** the effort!
*/
static CONDITION
sendStraightFileData(T_ASC_Association * assoc, const char *dataFileName,
	T_ASC_PresentationContextID presID,
	E_TransferSyntax /* xferSyntax */,
	DIMSE_ProgressCallback callback,
	void *callbackContext)

{
    /* we assume that the file contains transfer syntax compatible data */
    CONDITION cond;
    unsigned char *buf;
    unsigned long bufLen;
    FILE *f;
    long nbytes;
    int last;
    unsigned long bytesTransmitted = 0;
    CONDITION dulCond;
    DUL_PDVLIST pdvList;
    DUL_PDV pdv;
    unsigned long pdvCount = 0;

    buf = assoc->sendPDVBuffer;
    bufLen = assoc->sendPDVLength;

    cond = DIMSE_NORMAL;

    f = fopen(dataFileName, "rb");
    if (f == NULL) {
	DIMSE_warning(assoc, 
	    "sendStraightFileData: cannot open dicom file (%s): %s\n", 
	    dataFileName, strerror(errno));
	cond = DIMSE_SENDFAILED;
    }
    
    while (SUCCESS(cond) && ((nbytes = fread(buf, 1, bufLen, f)) > 0)) {
	last = ((unsigned long)nbytes != bufLen);
	pdv.fragmentLength = nbytes;
	pdv.presentationContextID = presID;
	pdv.pdvType = DUL_DATASETPDV;
	pdv.lastPDV = last;
	pdv.data = buf;

	pdvList.count = 1;
	pdvList.pdv = &pdv;

	if (debug) {
	    printf("DIMSE sendStraightFileData: sending %lu bytes (last: %s)\n",
		   pdv.fragmentLength, ((last)?("YES"):("NO")));
	}

	dulCond = DUL_WritePDVs(&assoc->DULassociation, &pdvList);
	if (dulCond != DUL_NORMAL) {
	    cond = DIMSE_SENDFAILED;
	}

	bytesTransmitted += nbytes;
	pdvCount += pdvList.count;

	if (callback) { /* execute callback function */
	    callback(callbackContext, bytesTransmitted);
	}
    }

    fclose(f);

    return cond;
}
#endif

static CONDITION
sendDcmDataset(T_ASC_Association * assoc, DcmDataset * obj,
		T_ASC_PresentationContextID presID,
		E_TransferSyntax xferSyntax, DUL_DATAPDV pdvType,
		DIMSE_ProgressCallback callback,
		void *callbackContext)

{
    CONDITION dulCond;
    E_Condition econd;
    unsigned char *buf;
    unsigned long bufLen;
    int last = OFFalse;
    Uint32 rtnLength;
    Uint32 bytesTransmitted = 0;
    DUL_PDVLIST pdvList;
    DUL_PDV pdv;
    unsigned long pdvCount = 0;

    buf = assoc->sendPDVBuffer;
    bufLen = assoc->sendPDVLength;

    econd = EC_Normal;
    bytesTransmitted = 0;

    DcmBufferStream outBuf(buf, bufLen, DCM_WriteMode);

    obj->transferInit();
    
    if (econd != EC_Normal) {
	    DIMSE_warning(assoc, "writeBlockInit Failed (%s)", 
	        dcmErrorConditionToString(econd));
	    return DIMSE_SENDFAILED;        
    }

    while (!last) {
    	econd = obj->write(outBuf, xferSyntax, EET_ExplicitLength, 
			   EGL_recalcGL, EPD_withoutPadding);
	if (econd == EC_Normal) {
	    last = OFTrue;	/* all contents have been written */
	} else if (econd == EC_StreamNotifyClient) {
	    /* ok,just no more space in streambuf */
	    last = OFFalse;
	} else {
	    /* some error has occurred */
	    DIMSE_warning(assoc, "writeBlock Failed (%s)", 
	        dcmErrorConditionToString(econd));
	    return DIMSE_SENDFAILED;	
	}

	void *fullBuf = NULL;
	outBuf.GetBuffer(fullBuf, rtnLength);

	if (rtnLength > 0) {
	    pdv.fragmentLength = rtnLength;
	    pdv.presentationContextID = presID;
	    pdv.pdvType = pdvType;
	    pdv.lastPDV = last;
	    pdv.data = fullBuf;

	    pdvList.count = 1;
	    pdvList.pdv = &pdv;

	    if (debug) {
	        printf("DIMSE sendDcmDataset: sending %lu bytes\n",
		   pdv.fragmentLength);
	    }

	    dulCond = DUL_WritePDVs(&assoc->DULassociation, &pdvList);
	    if (dulCond != DUL_NORMAL)
	        return DIMSE_SENDFAILED;

	    bytesTransmitted += rtnLength;
	    pdvCount += pdvList.count;

	    if (callback) { /* execute callback function */
	        callback(callbackContext, bytesTransmitted);
	    }
	}
    }

    obj->transferEnd();
    
    return DIMSE_NORMAL;
}

/*
** Public Functions Bodies
*/

/*
 * DIMSE Messaging
 */

/*
 * Message Send
 */

static CONDITION 
DIMSE_sendMessage(T_ASC_Association *assoc,
		  T_ASC_PresentationContextID presID,
		  T_DIMSE_Message *msg, DcmDataset *statusDetail,
		  DcmDataset *dataObject,
		  const char *dataFileName,
		  DIMSE_ProgressCallback callback,
		  void *callbackContext)

{
    CONDITION cond;
    E_TransferSyntax xferSyntax;
    DcmDataset *cmdObj = NULL;
    DcmFileFormat dcmff;
    int fromFile = 0;
    
    if (!isDataDictPresent())
    {
	  /* we must have a data dictionary */
	  return COND_PushCondition(
	    DIMSE_BUILDFAILED,
        "DIMSE_sendMessage: missing data dictionary");
    }

    if (DIMSE_NORMAL != (cond = validateMessage(assoc, msg))) return cond;
    if (DIMSE_NORMAL != (checkPresentationContextForMessage(assoc, msg, presID, &xferSyntax))) return cond;

    cond = DIMSE_buildCmdObject(msg, &cmdObj);

    if (SUCCESS(cond) && statusDetail != NULL)
    {
      /* move the status detail to the command */
      DcmElement* e;
      while ((e = statusDetail->remove((unsigned long)0)) != NULL) cmdObj->insert(e, OFTrue);
    }
    
    if (SUCCESS(cond) && DIMSE_isDataSetPresent(msg))
    {
      /* read the data from file if necessary */
      if ((dataObject != NULL)&&(dataFileName != NULL))
      {
	    DIMSE_warning(assoc, "sendData: both object and file specified (sending object only)");
      }
      else if ((dataObject == NULL)&&(dataFileName != NULL))
      {
        DcmFileStream inf(dataFileName, DCM_ReadMode);
        if (inf.Fail())
        {
	       DIMSE_warning(assoc, 
	       "sendMessage: cannot open dicom file (%s): %s\n", 
	       dataFileName, strerror(errno));
	       cond = DIMSE_SENDFAILED;
        } else {
          dcmff.transferInit();
          dcmff.read(inf, EXS_Unknown);
          dcmff.transferEnd();
          dataObject = dcmff.getDataset();
          fromFile = 1;
	    }      
      }
      
      /* check if we can convert the dataset to the required transfer syntax */
      if (dataObject)
      {
        if (! dataObject->canWriteXfer(xferSyntax))
        {
          DcmXfer writeXferSyntax(xferSyntax);
          DcmXfer originalXferSyntax(dataObject->getOriginalXfer());
          if (fromFile && dataFileName)
          {
	        DIMSE_warning(assoc, 
	         "sendMessage: unable to convert DICOM file '%s'\nfrom '%s' transfer syntax to '%s'.\n",
	         dataFileName, originalXferSyntax.getXferName(), writeXferSyntax.getXferName());
          } else {
	        DIMSE_warning(assoc, 
	         "sendMessage: unable to convert dataset\nfrom '%s' transfer syntax to '%s'.\n",
	         originalXferSyntax.getXferName(), writeXferSyntax.getXferName());
          }
	      cond = DIMSE_SENDFAILED;
        }
      } else {
	    DIMSE_warning(assoc, 
	    "sendMessage: no dataset to send\n");
	    cond = DIMSE_SENDFAILED;
      }
    }

    if (SUCCESS(cond))
    {
      if (debug)
      {
	    printf("DIMSE Command To Send:\n");
	    cmdObj->print();
      }
	  /* Send the command.
	   * Commands are always little endian implicit.
	   */
	  cond = sendDcmDataset(assoc, cmdObj, presID, 
	    EXS_LittleEndianImplicit, DUL_COMMANDPDV,
	    NULL, NULL);	
    }

    if (SUCCESS(cond) && DIMSE_isDataSetPresent(msg) &&(dataObject))
    {
        cond = sendDcmDataset(assoc, dataObject, presID, xferSyntax,
          DUL_DATASETPDV, callback, callbackContext);
    }

    if (cond != DIMSE_NORMAL) return COND_PushCondition(cond, DIMSE_Message(cond));
    return DIMSE_NORMAL;
}

CONDITION
DIMSE_sendMessageUsingFileData(T_ASC_Association * assoc,
		  T_ASC_PresentationContextID presID,
		  T_DIMSE_Message * msg,
		  DcmDataset *statusDetail,
		  const char *dataFileName,
		  DIMSE_ProgressCallback callback,
		  void *callbackContext)

{
    CONDITION cond;

    cond = DIMSE_sendMessage(assoc, presID, msg, statusDetail, 
	NULL, dataFileName,
	callback, callbackContext);

    return cond;
}

CONDITION
DIMSE_sendMessageUsingMemoryData(T_ASC_Association * assoc,
		  T_ASC_PresentationContextID presID,
		  T_DIMSE_Message * msg,
		  DcmDataset *statusDetail,
		  DcmDataset *dataObject,
		  DIMSE_ProgressCallback callback,
		  void *callbackContext)

{
    CONDITION cond;

    cond = DIMSE_sendMessage(assoc, presID, msg, statusDetail, 
	dataObject, NULL,
	callback, callbackContext);

    return cond;
}

/*
 * Message Receive
 */

static CONDITION
ignoreDataSet(T_ASC_Association * assoc,
	      T_DIMSE_BlockingMode blocking, int timeout,
	      DIC_UL * bytesRead, DIC_UL * pdvCount)
{
    CONDITION           cond = DIMSE_NORMAL;
    DUL_PDV             pdv;
    OFBool             last = OFFalse;

    while (!last) {
	cond = DIMSE_readNextPDV(assoc, blocking, timeout, &pdv);
	if (cond != DIMSE_NORMAL) {
	    break;
	}
	if (pdv.pdvType != DUL_DATASETPDV) {
	    cond = DIMSE_UNEXPECTEDPDVTYPE;
	    break;
	}
	*bytesRead += pdv.fragmentLength;
	(*pdvCount)++;
	last = pdv.lastPDV;
    }
    return cond;
}


CONDITION
DIMSE_receiveCommand(T_ASC_Association * assoc,
		     T_DIMSE_BlockingMode blocking,
		     int timeout,
		     T_ASC_PresentationContextID *presID,
		     T_DIMSE_Message * msg, DcmDataset ** statusDetail)
{
    CONDITION cond;
    unsigned long bytesRead;
    unsigned long pdvCount;

    DUL_DATAPDV type;
    OFBool last;
    DUL_PDV pdv;

    DIC_UL elemsLeft;
    T_ASC_PresentationContextID pid = 0;
    E_TransferSyntax xferSyntax;
    DcmDataset *cmdSet;
    E_Condition econd = EC_Normal;

    if (debug) {
	printf("DIMSE receiveCommand\n");
    }

    if (!isDataDictPresent()) {
	/* we must have a data dictionary */
	return COND_PushCondition(
	    DIMSE_PARSEFAILED,
	        "DIMSE_receiveCommand: missing data dictionary");
    }

    pdvCount = 0;

    cmdSet = new DcmDataset();
    if (cmdSet == NULL) {
	return COND_PushCondition(DIMSE_MALLOCERROR, 
	    "DIMSE: receiveCommand: Cannot allocate DcmDataset");
    }

    cmdSet->transferInit();

    DcmBufferStream cmdBuf(DCM_ReadMode);
    econd = cmdBuf.GetError();
    if (econd != EC_Normal) {
	return COND_PushCondition(DIMSE_PARSEFAILED, 
	    "DIMSE: receiveCommand: Failed to initialize cmdBuf(DCM_ReadMode)");
    }

    for (last = OFFalse, bytesRead = 0, type = DUL_COMMANDPDV;
	 type == DUL_COMMANDPDV && !last;) {
	 
	cmdBuf.ReleaseBuffer();	/* make the stream remember any unread bytes */
	
	cond = DIMSE_readNextPDV(assoc, blocking, timeout, &pdv);
	if (cond != DIMSE_NORMAL) {
	    if (cond == DIMSE_READPDVFAILED)
		return COND_PushCondition(DIMSE_RECEIVEFAILED,
					DIMSE_Message(DIMSE_RECEIVEFAILED));
	    else {
	        /* it was an abort or release request */
		return cond;
	    }
	}

	if (pdvCount == 0) {
	    pid = pdv.presentationContextID;
	} else if (pdv.presentationContextID != pid) {
	    cond = DIMSE_INVALIDPRESENTATIONCONTEXTID;
	    COND_PushCondition(cond, 
	        "DIMSE: Different PIDs inside Command Set: %d != %d", 
	        pid, pdv.presentationContextID);
	    return DIMSE_RECEIVEFAILED;
	}
	if ((pdv.fragmentLength % 2) != 0) {
	    /* This should NEVER happen.  See Part 7, Annex F. */
	    cond = COND_PushCondition(DIMSE_RECEIVEFAILED,
	        "DIMSE: Odd Fragment Length: %lu", pdv.fragmentLength);
	    break;
	}
	
	if (pdv.fragmentLength > 0) {
	    cmdBuf.SetBuffer(pdv.data, pdv.fragmentLength);
	}

	if (pdv.lastPDV) {
	    cmdBuf.SetEndOfStream();
	}
	
        /* Commands are always little endian implicit */

	econd = cmdSet->read(cmdBuf, EXS_LittleEndianImplicit, EGL_withoutGL);
	if (econd != EC_Normal && econd != EC_StreamNotifyClient) {
	    delete cmdSet;
	    return COND_PushCondition(DIMSE_RECEIVEFAILED, 
	        "DIMSE: receiveCommand: cmdSet->read() Failed (%s)",	
		dcmErrorConditionToString(econd));
	}
	
	bytesRead += pdv.fragmentLength;
	last = pdv.lastPDV;
	type = pdv.pdvType;
	pdvCount++;

    }

    cmdSet->transferEnd();
    
    if (debug) {
	printf("DIMSE receiveCommand: %lu pdv's (%lu bytes), presID=%d\n",
	       pdvCount, bytesRead, pid);
    }

    /* is this a valid presentation context ? */
    cond = getTransferSyntax(assoc, pid, &xferSyntax);
    if (!SUCCESS(cond)) 
        return cond;

    if (type != DUL_COMMANDPDV) {
        cond = DIMSE_UNEXPECTEDPDVTYPE;
	COND_PushCondition(cond, "DIMSE: Command PDV Expected");
	return DIMSE_RECEIVEFAILED;
    }

    if (debug) {
	printf("DIMSE Command Received:\n");
        cmdSet->print();
    }

    cond = DIMSE_parseCmdObject(msg, cmdSet);
    if (cond != DIMSE_NORMAL) {
	delete cmdSet;    
    }
    
    if (cond == DIMSE_NORMAL) {
	cond = validateMessage(assoc, msg);
    }
    
    /*
     * Whatever is left in the Cmd object should be status detail
     * information. 
     */

    if (cond == DIMSE_NORMAL) {
        elemsLeft = DIMSE_countElements(cmdSet);
        if ((statusDetail != NULL) && (elemsLeft > 0)) {
	    /* this must be any status detail */
	    *statusDetail = cmdSet;
	} else {
	    /* nothing left or the caller does not want status detail */
	    delete cmdSet;
	}
    }

    /* set the Presentation Context ID we received */
    *presID = pid;

    if (cond != DIMSE_NORMAL)
	return COND_PushCondition(cond, DIMSE_Message(cond));
    return DIMSE_NORMAL;

}

CONDITION DIMSE_createFilestream(
  const char *filename,
  const T_DIMSE_C_StoreRQ *request,
  const T_ASC_Association *assoc, 
  T_ASC_PresentationContextID presIdCmd,
  int writeMetaheader,
  DcmFileStream **filestream)
{
  CONDITION cond = DIMSE_NORMAL;
  DcmElement *elem=NULL;
  DcmMetaInfo *metainfo=NULL;
  DcmTag metaElementGroupLength(DCM_MetaElementGroupLength);
  DcmTag fileMetaInformationVersion(DCM_FileMetaInformationVersion);
  DcmTag mediaStorageSOPClassUID(DCM_MediaStorageSOPClassUID);
  DcmTag mediaStorageSOPInstanceUID(DCM_MediaStorageSOPInstanceUID);
  DcmTag transferSyntaxUID(DCM_TransferSyntaxUID);
  DcmTag implementationClassUID(DCM_ImplementationClassUID);
  DcmTag implementationVersionName(DCM_ImplementationVersionName);
  T_ASC_PresentationContext presentationContext;
  

  if ((filename == NULL) || (request==NULL) || (assoc==NULL) ||
      (assoc->params==NULL) || (filestream==NULL))
  {
    cond = COND_PushCondition(DIMSE_NULLKEY, 
      "DIMSE_createFilestream: NULL parameters");
    return cond;
  }
  if (ASC_NORMAL != ASC_findAcceptedPresentationContext(assoc->params,
      presIdCmd, &presentationContext))
  {
    cond = COND_PushCondition(DIMSE_INVALIDPRESENTATIONCONTEXTID, 
      "DIMSE_createFilestream: Invalid presentation context ID");
    return cond;
  }    

  if (writeMetaheader)
  {
    if (NULL == (metainfo = new DcmMetaInfo()))
    {
      cond = COND_PushCondition(DIMSE_OUTOFRESOURCES, 
       "DIMSE_createFilestream: out of memory");
      return cond;
    }
    if (NULL != (elem = new DcmUnsignedLong(metaElementGroupLength)))
    {
      metainfo->insert(elem, OFTrue);
      Uint32 temp = 0;
      ((DcmUnsignedLong*)elem)->putUint32Array(&temp, 1);
    } else cond = DIMSE_OUTOFRESOURCES;
    if (NULL != (elem = new DcmOtherByteOtherWord(fileMetaInformationVersion)))
    {
      metainfo->insert(elem, OFTrue);
      Uint8 version[2] = {0,1};
      ((DcmOtherByteOtherWord*)elem)->putUint8Array( version, 2 );
    } else cond = DIMSE_OUTOFRESOURCES;
    if (NULL != (elem = new DcmUniqueIdentifier(mediaStorageSOPClassUID)))
    {
      metainfo->insert(elem, OFTrue);
      ((DcmUniqueIdentifier*)elem)->putString(request->AffectedSOPClassUID);
    } else cond = DIMSE_OUTOFRESOURCES;
    if (NULL != (elem = new DcmUniqueIdentifier(mediaStorageSOPInstanceUID)))
    {
      metainfo->insert(elem, OFTrue);
      ((DcmUniqueIdentifier*)elem)->putString(request->AffectedSOPInstanceUID);
    } else cond = DIMSE_OUTOFRESOURCES;
    if (NULL != (elem = new DcmUniqueIdentifier(transferSyntaxUID)))
    {
      metainfo->insert(elem, OFTrue);
      elem->putString(presentationContext.acceptedTransferSyntax);
    } else cond = DIMSE_OUTOFRESOURCES;
    if (NULL != (elem = new DcmUniqueIdentifier(implementationClassUID)))
    {
      metainfo->insert(elem, OFTrue);
      const char *uid = OFFIS_IMPLEMENTATION_CLASS_UID;
      ((DcmUniqueIdentifier*)elem)->putString(uid);
    } else cond = DIMSE_OUTOFRESOURCES;
    if (NULL != (elem = new DcmShortString(implementationVersionName)))
    {
      metainfo->insert(elem, OFTrue);
      char *version = OFFIS_DTK_IMPLEMENTATION_VERSION_NAME2;
      ((DcmShortString*)elem)->putString(version);
    } else cond = DIMSE_OUTOFRESOURCES;
    if (cond == DIMSE_OUTOFRESOURCES)
    {
      delete metainfo;
      cond = COND_PushCondition(DIMSE_OUTOFRESOURCES, 
       "DIMSE_createFilestream: out of memory");
      return cond;
    }
    if (EC_Normal != metainfo->computeGroupLengthAndPadding
	  (EGL_withGL, EPD_noChange, META_HEADER_DEFAULT_TRANSFERSYNTAX, 
	  EET_UndefinedLength))
	{
      delete metainfo;
      cond = COND_PushCondition(DIMSE_OUTOFRESOURCES, 
       "DIMSE_createFilestream: cannot compute metaheader length");
      return cond;
	}
  }
  
  *filestream = new DcmFileStream(filename, DCM_WriteMode);
  if ((*filestream == NULL)||((*filestream)->Fail()))
  {
     if (metainfo) delete metainfo;
     if (*filestream)
     {
       delete *filestream;
       *filestream = NULL;
     }
     cond = COND_PushCondition(DIMSE_OUTOFRESOURCES, 
       "DIMSE_createFilestream: cannot create file '%s'", filename);
     return cond;
  }
  
  if (metainfo)
  {
    metainfo->transferInit();
    if (EC_Normal != metainfo->write(**filestream, META_HEADER_DEFAULT_TRANSFERSYNTAX, EET_ExplicitLength))
    {
      cond = COND_PushCondition(DIMSE_OUTOFRESOURCES, 
       "DIMSE_createFilestream: cannot write metaheader to file '%s'", filename);
    }
    metainfo->transferEnd();
    delete metainfo;
  }

  return cond;
  
}

CONDITION
DIMSE_receiveDataSetInFile(T_ASC_Association *assoc,
	T_DIMSE_BlockingMode blocking, int timeout, 
	T_ASC_PresentationContextID *presID,
	DcmStream *filestream,
	DIMSE_ProgressCallback callback, void *callbackData)
{
    CONDITION cond = DIMSE_NORMAL;
    DUL_PDV pdv;
    T_ASC_PresentationContextID pid = 0;
    E_TransferSyntax xferSyntax; 
    OFBool last = OFFalse;
    DIC_UL pdvCount = 0;
    DIC_UL bytesRead = 0;

    if ((assoc == NULL) || (presID==NULL) || (filestream==NULL))
    {
      cond = COND_PushCondition(DIMSE_NULLKEY, 
        "DIMSE_receiveDataSetInFile: NULL parameters");
      return cond;
    }

    *presID = 0;	/* invalid value */

    while (!last)
    {
	  cond = DIMSE_readNextPDV(assoc, blocking, timeout, &pdv);
	  if (cond != DIMSE_NORMAL) break;
      if (pdv.pdvType != DUL_DATASETPDV)
      {
	    cond = DIMSE_UNEXPECTEDPDVTYPE;
	    break;
	  }
	  if (pdvCount == 0)
	  {
	    pid = pdv.presentationContextID;
	    /* is this a valid presentation context ? */
	    cond = getTransferSyntax(assoc, pid, &xferSyntax);
	    if (!SUCCESS(cond)) break;
	  }
	  else if (pdv.presentationContextID != pid)
	  {
	    cond = DIMSE_INVALIDPRESENTATIONCONTEXTID;
	    COND_PushCondition(cond, 
	        "DIMSE: Different PIDs inside Data Set: %d != %d", 
	        pid, pdv.presentationContextID);
	    cond = DIMSE_RECEIVEFAILED;
	    break;
	  }
	  if ((pdv.fragmentLength % 2) != 0)
	  {
	    /* This should NEVER happen.  See Part 7, Annex F. */
	    cond = COND_PushCondition(DIMSE_RECEIVEFAILED,
	        "DIMSE: Odd Fragment Length: %lu", pdv.fragmentLength);
	    break;
	  }
	  filestream->WriteBytes((void *)(pdv.data), (Uint32)(pdv.fragmentLength));
	  if (filestream->Fail())
	  {
	      cond = ignoreDataSet(assoc, blocking, timeout, &bytesRead, &pdvCount);
	      if (cond == DIMSE_NORMAL)
	      {
	          cond = COND_PushCondition(DIMSE_OUTOFRESOURCES, 
	            "DIMSE_receiveDataSetInFile: Cannot write to file");
	      }
	     break;
	  }
	  bytesRead += pdv.fragmentLength;
	  pdvCount++;
	  last = pdv.lastPDV;
	  if (debug)
	  {
	     printf("DIMSE receiveFileData: %lu bytes read (last: %s)\n",
		   pdv.fragmentLength, ((last)?("YES"):("NO")));
	  }
	  if (callback)
	  { /* execute callback function */
	    callback(callbackData, bytesRead);
	  }
    }

    if (cond != DIMSE_NORMAL)
    {
	  return COND_PushCondition(cond, DIMSE_Message(cond));
    }

    /* set the Presentation Context ID we received */
    *presID = pid;

    return cond;
}


CONDITION
DIMSE_receiveDataSetInMemory(T_ASC_Association * assoc,
			     T_DIMSE_BlockingMode blocking,
			     int timeout,
			     T_ASC_PresentationContextID * presID,
			     DcmDataset ** dataObject,
		  	     DIMSE_ProgressCallback callback,
		             void *callbackData)
{
    CONDITION cond = DIMSE_NORMAL;
    E_Condition econd = EC_Normal;
    DcmDataset *dset = NULL;
    DUL_PDV pdv;
    T_ASC_PresentationContextID pid = 0;
    E_TransferSyntax xferSyntax;
    OFBool last = OFFalse;
    DIC_UL pdvCount = 0;
    DIC_UL bytesRead = 0;

    if (dataObject == NULL) {
	return COND_PushCondition(DIMSE_NULLKEY,
	        "DIMSE_receiveDataSetInMemory: dataObject==NULL");
    }

    if (!isDataDictPresent()) {
	/* we must have a data dictionary */
	return COND_PushCondition(
	    DIMSE_PARSEFAILED,
	        "DIMSE_receiveDataSetInMemory: missing data dictionary");
    }

    if (*dataObject == NULL) {
	dset = new DcmDataset();
    } else {
	dset = *dataObject; /* use the passed in Dataset */
    }
    if (dset == NULL) {
	cond = ignoreDataSet(assoc, blocking, timeout, 
	    &bytesRead, &pdvCount);
	if (cond == DIMSE_NORMAL) {
            return COND_PushCondition(DIMSE_OUTOFRESOURCES,
	        "DIMSE_receiveDataSetInMemory: Cannot create DcmDataset");
	}
	/* there was a network problem */
	return cond;
    }
    
    DcmBufferStream dataBuf(DCM_ReadMode);
    
    dset->transferInit();

    while (!last && cond == DIMSE_NORMAL) {
    
	dataBuf.ReleaseBuffer(); /* make the stream remember any unread bytes */
        
	cond = DIMSE_readNextPDV(assoc, blocking, timeout, &pdv);
	if (cond != DIMSE_NORMAL) {
	    break;
	}
	if (pdv.pdvType != DUL_DATASETPDV) {
	    cond = DIMSE_UNEXPECTEDPDVTYPE;
	    break;
	}
	if (pdvCount == 0) {
	    pid = pdv.presentationContextID;

	    /* is this a valid presentation context ? */
	    cond = getTransferSyntax(assoc, pid, &xferSyntax);
	    if (!SUCCESS(cond)) 
	        break;
		
	} else if (pdv.presentationContextID != pid) {
	    cond = DIMSE_INVALIDPRESENTATIONCONTEXTID;
	    COND_PushCondition(cond, 
	        "DIMSE: Different PIDs inside Data Set: %d != %d", 
	        pid, pdv.presentationContextID);
	    cond = DIMSE_RECEIVEFAILED;
	    break;
	}
	if ((pdv.fragmentLength % 2) != 0) {
	    /* This should NEVER happen.  See Part 7, Annex F. */
	    cond = COND_PushCondition(DIMSE_RECEIVEFAILED,
	        "DIMSE: Odd Fragment Length: %lu", pdv.fragmentLength);
	    break;
	}

	
	if (pdv.fragmentLength > 0) {
	    dataBuf.SetBuffer(pdv.data, pdv.fragmentLength);
	}

	if (pdv.lastPDV) {
	    dataBuf.SetEndOfStream();
	}
	
	econd = dset->read(dataBuf, xferSyntax);
	if (econd != EC_Normal && econd != EC_StreamNotifyClient) {
	    DIMSE_warning(assoc, 
		"DIMSE_receiveDataSetInMemory: dset->read() Failed (%s)", 
		dcmErrorConditionToString(econd));
	    cond = DIMSE_RECEIVEFAILED;
	    break;
	}

	bytesRead += pdv.fragmentLength;
	pdvCount++;
	last = pdv.lastPDV;
	if (debug) {
	    printf("DIMSE receiveFileData: %lu bytes read (last: %s)\n",
		   pdv.fragmentLength, ((last) ? ("YES") : ("NO")));
	}
	if (callback) { /* execute callback function */
	    callback(callbackData, bytesRead);
	}
    }

    dset->transferEnd();
    
    if (cond != DIMSE_NORMAL) {
	return COND_PushCondition(cond, DIMSE_Message(cond));
    }

    /* set the Presentation Context ID we received */
    *presID = pid;

    /* set the object we received */
    *dataObject = dset;

    return cond;
}


/*
 * Misc functions
 */

void DIMSE_debug(int level)
{
	debug = level;
}

void DIMSE_warning(T_ASC_Association *assoc,
	const char *format, ...)
{
    va_list args;

    fprintf(stderr, "DIMSE Warning: (%s,%s): ", 
	assoc->params->DULparams.callingAPTitle,
	assoc->params->DULparams.calledAPTitle);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}

/*
** CVS Log
** $Log: dimse.cc,v $
** Revision 1.14  1998-01-28 17:38:13  meichel
** Removed minor bug from DICOM Upper Layer / DIMSE modules.
**   For each PDV received, an error condition was pushed on the error stack
**   and then again pulled from it. If a callback function was registered
**   with the condition stack, it was flooded with error messages.
**
** Revision 1.13  1998/01/27 10:51:46  meichel
** Removed some unused variables, meaningless const modifiers
**   and unreached statements.
**
** Revision 1.12  1997/09/18 08:10:59  meichel
** Many minor type conflicts (e.g. long passed as int) solved.
**
** Revision 1.11  1997/08/06 12:20:13  andreas
** - Using Windows NT with Visual C++ 4.x the standard open mode for files
**   is TEXT with conversions. For binary files (image files, imagectn database
**   index) this must be changed (e.g. fopen(filename, "...b"); or
**   open(filename, ..... |O_BINARY);)
**
** Revision 1.10  1997/07/21 08:47:19  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.9  1997/05/28 12:04:46  meichel
** DIMSE_sendMessage() now checks whether the dataset to be sent
** can be converted to the requested transfer syntax prior to
** transmitting the message and dataset. If the test fails,
** DIMSE_SENDFAILED is returned.
**
** Revision 1.8  1997/05/23 10:45:28  meichel
** Major rewrite of storescp application. See CHANGES for details.
** Changes required to interfaces of some DIMSE functions.
**
** Revision 1.7  1997/05/22 13:30:30  hewett
** Modified the test for presence of a data dictionary to use the
** method DcmDataDictionary::isDictionaryLoaded().
**
** Revision 1.6  1997/05/16 08:31:37  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.5  1997/02/06 12:21:14  hewett
** Updated for Macintosh CodeWarrior 11.  Corrected for incompatibilities
** in the timeval structure between unix.h and winsock.h
**
** Revision 1.4  1996/09/03 11:40:25  hewett
** Added automatic tests in the DIMSE level network code to check
** that a data dictionary is loaded.  Calls to DIMSE routines will
** now fail if no data dictionary is loaded.  Previously, the lack of
** a loaded data dictionary would cause obscure errors.
**
** Revision 1.3  1996/04/27 12:57:58  hewett
** Corrected cause of warnings when compiling under "c++ -O -g -Wall"
** under Solaris 2.4.  Mostly due to unintialized variables.
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

