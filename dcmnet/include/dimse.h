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
**	for DICOM applications.  
**
**	Module Prefix: DIMSE_
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-03-26 18:38:45 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/include/Attic/dimse.h,v $
** CVS/RCS Revision:	$Revision: 1.1 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef DIMSE_H
#define DIMSE_H

/*
 * Required Include Files
 */
 
#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dicom.h"
#include "lst.h"
#include "cond.h"
#include "dul.h"
#include "assoc.h"
#include "dcdatset.h"

/*
 * Constant Definitions
 */



/* 
 * General Status Codes 
 */
#define STATUS_Success	0x0000
#define STATUS_Pending	0xff00

#define DICOM_PENDING_STATUS(status) (((status)&0xff00) == 0xff00)
#define DICOM_WARNING_STATUS(status) (((status)&0xf000) == 0xb000)

/* 
 * Service Class Specific Status Codes 
 * NOTE: some codes are only significant in the high byte 
 * or high nibble (4 bits).
 */
/* Storage Specific Codes*/
#define STATUS_STORE_Refused_OutOfResources		/* high byte */	0xa700	
#define STATUS_STORE_Refused_SOPClassNotSupported 	/* high byte */	0xa800 
#define STATUS_STORE_Error_DataSetDoesNotMatchSOPClass 	/* high byte */	0xa900 
#define STATUS_STORE_Error_CannotUnderstand	/* high nibble */	0xc000	
#define STATUS_STORE_Warning_CoersionOfDataElements 			0xb000
#define STATUS_STORE_Warning_DataSetDoesNotMatchSOPClass 		0xb007
#define STATUS_STORE_Warning_ElementsDiscarded				0xb006

/* Find Specific Codes */
#define STATUS_FIND_Refused_OutOfResources				0xa700
#define STATUS_FIND_Refused_SOPClassNotSupported 			0xa800
#define STATUS_FIND_Failed_IdentifierDoesNotMatchSOPClass 		0xa900
#define STATUS_FIND_Failed_UnableToProcess	/* high nibble */	0xc000	
#define STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest		0xfe00
#define STATUS_FIND_Pending_WarningUnsupportedOptionalKeys		0xff01

/* Move Specific Codes */
#define STATUS_MOVE_Refused_OutOfResourcesNumberOfMatches		0xa701
#define STATUS_MOVE_Refused_OutOfResourcesSubOperations			0xa702
#define STATUS_MOVE_Failed_SOPClassNotSupported 			0xa800
#define STATUS_MOVE_Failed_MoveDestinationUnknown			0xa801
#define STATUS_MOVE_Failed_IdentifierDoesNotMatchSOPClass 		0xa900
#define STATUS_MOVE_Failed_UnableToProcess 	 /* high nibble */      0xc000 
#define STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication	0xfe00
#define STATUS_MOVE_Warning_SubOperationsCompleteOneOrMoreFailures	0xb000


/*
 * Type Definitions
 */


/*
 * DIMSE Command Codes (Request and Response)
 */
typedef enum {			/* DIC_US */
    DIMSE_NOTHING = 0x0000,	/* none of the rest !! */
    DIMSE_C_STORE_RQ = 0x0001,	/* also known as C_SEND_RQ  */
    DIMSE_C_STORE_RSP = 0x8001,	/* also known as C_SEND_RSP */
    DIMSE_C_GET_RQ = 0x0010,
    DIMSE_C_GET_RSP = 0x8010,
    DIMSE_C_FIND_RQ = 0x0020,
    DIMSE_C_FIND_RSP = 0x8020,
    DIMSE_C_MOVE_RQ = 0x0021,
    DIMSE_C_MOVE_RSP = 0x8021,
    DIMSE_C_ECHO_RQ = 0x0030,
    DIMSE_C_ECHO_RSP = 0x8030,
    DIMSE_C_CANCEL_RQ = 0x0fff,
    /* there is no DIMSE_C_CANCEL_RSP */

    DIMSE_N_EVENT_REPORT_RQ = 0x0100,
    DIMSE_N_EVENT_REPORT_RSP = 0x8100,
    DIMSE_N_GET_RQ = 0x0110,
    DIMSE_N_GET_RSP = 0x8110,
    DIMSE_N_SET_RQ = 0x0120,
    DIMSE_N_SET_RSP = 0x8120,
    DIMSE_N_ACTION_RQ = 0x0130,
    DIMSE_N_ACTION_RSP = 0x8130,
    DIMSE_N_CREATE_RQ = 0x0140,
    DIMSE_N_CREATE_RSP = 0x8140,
    DIMSE_N_DELETE_RQ = 0x0150,
    DIMSE_N_DELETE_RSP = 0x8150
} T_DIMSE_Command;

/*
 * DIMSE Data Set Type Codes
 */

typedef enum {			/* DIC_US */
    DIMSE_DATASET_PRESENT = 0x0001,	/* anything other than 0x0101) */
    DIMSE_DATASET_NULL = 0x0101
} T_DIMSE_DataSetType;

/*
 * DIMSE Priority Codes
 */

typedef enum {			/* DIC_US */
    DIMSE_PRIORITY_LOW = 0x0002,
    DIMSE_PRIORITY_MEDIUM = 0x0000,
    DIMSE_PRIORITY_HIGH = 0x0001
} T_DIMSE_Priority;

/*
 * DIMSE Blocking Modes (reading)
 */

typedef enum {
    DIMSE_BLOCKING,
    DIMSE_NONBLOCKING
} T_DIMSE_BlockingMode;


/*
 * DIMSE Messages 
 * 
 */

/* C-STORE */

typedef struct {
	DIC_US          MessageID;				/* M */
	DIC_UI          AffectedSOPClassUID;			/* M */
	T_DIMSE_Priority Priority;				/* M */
	T_DIMSE_DataSetType DataSetType;			/* M */
	DIC_UI          AffectedSOPInstanceUID;			/* M */
	DIC_AE          MoveOriginatorApplicationEntityTitle;	/* U */
	DIC_US          MoveOriginatorID;			/* U */
	/* DataSet provided as argument to DIMSE functions */	/* M */
	unsigned int	opts; /* which optional items are set */
#define O_STORE_MOVEORIGINATORAETITLE			0x0001
#define O_STORE_MOVEORIGINATORID			0x0002
} T_DIMSE_C_StoreRQ;

typedef struct {
	DIC_US          MessageIDBeingRespondedTo;		/* M */
	DIC_UI          AffectedSOPClassUID;			/* U(=) */
	T_DIMSE_DataSetType DataSetType;			/* M */
	DIC_US          Status;					/* M */
	DIC_UI          AffectedSOPInstanceUID;			/* U(=) */
	unsigned int	opts; /* which optional items are set */
#define O_STORE_AFFECTEDSOPCLASSUID		0x0001
#define O_STORE_AFFECTEDSOPINSTANCEUID		0x0002
} T_DIMSE_C_StoreRSP;

/* C-ECHO */

typedef struct {
	DIC_US          MessageID;				/* M */
	DIC_UI          AffectedSOPClassUID;			/* M */
	T_DIMSE_DataSetType DataSetType;			/* M */
} T_DIMSE_C_EchoRQ;

typedef struct {
	DIC_US          MessageIDBeingRespondedTo;		/* M */
	DIC_UI          AffectedSOPClassUID;			/* U(=) */
	T_DIMSE_DataSetType DataSetType;			/* M */
	DIC_US          Status;					/* M */
	unsigned int	opts; /* which optional items are set */
#define O_ECHO_AFFECTEDSOPCLASSUID		0x0001
} T_DIMSE_C_EchoRSP;

/* C-FIND */

typedef struct {
	DIC_US          MessageID;				/* M */
	DIC_UI          AffectedSOPClassUID;			/* M */
	T_DIMSE_Priority Priority;				/* M */
	T_DIMSE_DataSetType DataSetType;			/* M */
	/* Identifier provided as argument to DIMSE functions *//* M */
} T_DIMSE_C_FindRQ;

typedef struct {
	DIC_US          MessageIDBeingRespondedTo;		/* M */
	DIC_UI          AffectedSOPClassUID;			/* U(=) */
	T_DIMSE_DataSetType DataSetType;			/* M */
	DIC_US          Status;					/* M */
	/* Identifier provided as argument to DIMSE functions *//* C */
	unsigned int	opts; /* which optional items are set */
#define O_FIND_AFFECTEDSOPCLASSUID		0x0001
} T_DIMSE_C_FindRSP;

/* C-GET */

typedef struct {
	DIC_US          MessageID;				/* M */
	DIC_UI          AffectedSOPClassUID;			/* M */
	T_DIMSE_Priority Priority;				/* M */
	T_DIMSE_DataSetType DataSetType;			/* M */
	/* Identifier provided as argument to DIMSE functions *//* M */
} T_DIMSE_C_GetRQ;

typedef struct {
	DIC_US          MessageIDBeingRespondedTo;		/* M */
	DIC_UI          AffectedSOPClassUID;			/* U(=) */
	T_DIMSE_DataSetType DataSetType;			/* M */
	DIC_US          Status;					/* M */
	DIC_US          NumberOfRemainingSubOperations;		/* C */
	DIC_US          NumberOfCompletedSubOperations;		/* C */
	DIC_US          NumberOfFailedSubOperations;		/* C */
	DIC_US          NumberOfWarningSubOperations;		/* C */
	unsigned int	opts; /* which optional items are set */
#define O_GET_AFFECTEDSOPCLASSUID		0x0001
#define O_GET_NUMBEROFREMAININGSUBOPERATIONS	0x0002
#define O_GET_NUMBEROFCOMPLETEDSUBOPERATIONS	0x0004
#define O_GET_NUMBEROFFAILEDSUBOPERATIONS	0x0008
#define O_GET_NUMBEROFWARNINGSUBOPERATIONS	0x0010
} T_DIMSE_C_GetRSP;

/* C-MOVE */

typedef struct {
	DIC_US          MessageID;				/* M */
	DIC_UI          AffectedSOPClassUID;			/* M */
	T_DIMSE_Priority Priority;				/* M */
	T_DIMSE_DataSetType DataSetType;			/* M */
	DIC_AE          MoveDestination;			/* M */
	/* Identifier provided as argument to DIMSE functions *//* M */
} T_DIMSE_C_MoveRQ;

typedef struct {
	DIC_US          MessageIDBeingRespondedTo;		/* M */
	DIC_UI          AffectedSOPClassUID;			/* U(=) */
	T_DIMSE_DataSetType DataSetType;			/* M */
	DIC_US          Status;					/* M */
	DIC_US          NumberOfRemainingSubOperations;		/* C */
	DIC_US          NumberOfCompletedSubOperations;		/* C */
	DIC_US          NumberOfFailedSubOperations;		/* C */
	DIC_US          NumberOfWarningSubOperations;		/* C */
	unsigned int	opts; /* which optional items are set */
#define O_MOVE_AFFECTEDSOPCLASSUID		0x0001
#define O_MOVE_NUMBEROFREMAININGSUBOPERATIONS	0x0002
#define O_MOVE_NUMBEROFCOMPLETEDSUBOPERATIONS	0x0004
#define O_MOVE_NUMBEROFFAILEDSUBOPERATIONS	0x0008
#define O_MOVE_NUMBEROFWARNINGSUBOPERATIONS	0x0010
} T_DIMSE_C_MoveRSP;


/* C-CANCEL */

typedef struct {
	DIC_US          MessageIDBeingRespondedTo;		/* M */
	T_DIMSE_DataSetType DataSetType;			/* M */
} T_DIMSE_C_CancelRQ;


/* N-EVENT-REPORT */

typedef struct {
	DIC_US          MessageID;				/* M */
	DIC_UI          AffectedSOPClassUID;			/* M */
	DIC_UI          AffectedSOPInstanceUID;			/* M */
	T_DIMSE_DataSetType DataSetType;			/* M */
	DIC_US		EventTypeID;				/* M */
	/* EventInformation provided as argument to DIMSE functions *//* U */
} T_DIMSE_N_EventReportRQ;

typedef struct {
	DIC_US          MessageIDBeingRespondedTo;		/* M */
	DIC_UI          AffectedSOPClassUID;			/* U(=) */
	DIC_US          Status;					/* M */
	DIC_UI          AffectedSOPInstanceUID;			/* U(=) */
	T_DIMSE_DataSetType DataSetType;			/* M */
	DIC_US		EventTypeID;				/* C(=) */
	/* EventReply provided as argument to DIMSE functions *//* C */
	unsigned int	opts; /* which optional items are set */
#define O_NEVENTREPORT_AFFECTEDSOPCLASSUID		0x0001
#define O_NEVENTREPORT_AFFECTEDSOPINSTANCEUID		0x0002
#define O_NEVENTREPORT_EVENTTYPEID			0x0004
} T_DIMSE_N_EventReportRSP;

/* N-GET */

typedef struct {
	DIC_US          MessageID;				/* M */
	DIC_UI          RequestedSOPClassUID;			/* M */
	DIC_UI          RequestedSOPInstanceUID;		/* M */
	T_DIMSE_DataSetType DataSetType;			/* M */
	/*
	 * In the following array, the attributes to get should be coded
	 * in pairs along the array (e.g. {g,e,g,e,g,e,...}).  The ListCount
	 * variable should contain the number of 'DIC_US' values in the
	 * array (not the number of pairs).
	 */
	int		ListCount;
	DIC_US		*AttributeIdentifierList;		/* U */
} T_DIMSE_N_GetRQ;

typedef struct {
	DIC_US          MessageIDBeingRespondedTo;		/* M */
	DIC_UI          AffectedSOPClassUID;			/* U */
	DIC_US          Status;					/* M */
	DIC_UI          AffectedSOPInstanceUID;			/* U */
	T_DIMSE_DataSetType DataSetType;			/* M */
	/* AttributeList provided as argument to DIMSE functions *//* C */
	unsigned int	opts; /* which optional items are set */
#define O_NGET_AFFECTEDSOPCLASSUID		0x0001
#define O_NGET_AFFECTEDSOPINSTANCEUID		0x0002
} T_DIMSE_N_GetRSP;

/* N-SET */

typedef struct {
	DIC_US          MessageID;				/* M */
	DIC_UI          RequestedSOPClassUID;			/* M */
	DIC_UI          RequestedSOPInstanceUID;		/* M */
	T_DIMSE_DataSetType DataSetType;			/* M */
	/* ModificationList provided as argument to DIMSE functions *//* M */
} T_DIMSE_N_SetRQ;

typedef struct {
	DIC_US          MessageIDBeingRespondedTo;		/* M */
	DIC_UI          AffectedSOPClassUID;			/* U */
	DIC_US          Status;					/* M */
	DIC_UI          AffectedSOPInstanceUID;			/* U */
	T_DIMSE_DataSetType DataSetType;			/* M */
	/* AttributeList provided as argument to DIMSE functions *//* U */
	unsigned int	opts; /* which optional items are set */
#define O_NSET_AFFECTEDSOPCLASSUID		0x0001
#define O_NSET_AFFECTEDSOPINSTANCEUID		0x0002
} T_DIMSE_N_SetRSP;

/* N-ACTION */

typedef struct {
	DIC_US          MessageID;				/* M */
	DIC_UI          RequestedSOPClassUID;			/* M */
	DIC_UI          RequestedSOPInstanceUID;		/* M */
	DIC_US		ActionTypeID;				/* M */
	T_DIMSE_DataSetType DataSetType;			/* M */
	/* ActionInformation provided as argument to DIMSE functions *//* U */
} T_DIMSE_N_ActionRQ;

typedef struct {
	DIC_US          MessageIDBeingRespondedTo;		/* M */
	DIC_UI          AffectedSOPClassUID;			/* U */
	DIC_US          Status;					/* M */
	DIC_UI          AffectedSOPInstanceUID;			/* U */
	DIC_US		ActionTypeID;				/* C(=) */
	T_DIMSE_DataSetType DataSetType;			/* M */
	/* ActionReply provided as argument to DIMSE functions *//* C */
	unsigned int	opts; /* which optional items are set */
#define O_NACTION_AFFECTEDSOPCLASSUID		0x0001
#define O_NACTION_AFFECTEDSOPINSTANCEUID	0x0002
#define O_NACTION_ACTIONTYPEID			0x0004
} T_DIMSE_N_ActionRSP;

/* N-CREATE */

typedef struct {
	DIC_US          MessageID;				/* M */
	DIC_UI          AffectedSOPClassUID;			/* M */
	DIC_UI          AffectedSOPInstanceUID;			/* U */
	T_DIMSE_DataSetType DataSetType;			/* M */
	/* AttributeList provided as argument to DIMSE functions *//* M */
	unsigned int	opts; /* which optional items are set */
#define O_NCREATE_AFFECTEDSOPINSTANCEUID	0x0002
} T_DIMSE_N_CreateRQ;

typedef struct {
	DIC_US          MessageIDBeingRespondedTo;		/* M */
	DIC_UI          AffectedSOPClassUID;			/* U(=) */
	DIC_US          Status;					/* M */
	DIC_UI          AffectedSOPInstanceUID;			/* C */
	T_DIMSE_DataSetType DataSetType;			/* M */
	/* AttributeList provided as argument to DIMSE functions *//* U */
	unsigned int	opts; /* which optional items are set */
#define O_NCREATE_AFFECTEDSOPCLASSUID		0x0001
#define O_NCREATE_AFFECTEDSOPINSTANCEUID	0x0002
} T_DIMSE_N_CreateRSP;

/* N-DELETE */

typedef struct {
	DIC_US          MessageID;				/* M */
	DIC_UI          RequestedSOPClassUID;			/* M */
	DIC_UI          RequestedSOPInstanceUID;		/* M */
	T_DIMSE_DataSetType DataSetType;			/* M */
} T_DIMSE_N_DeleteRQ;

typedef struct {
	DIC_US          MessageIDBeingRespondedTo;		/* M */
	DIC_UI          AffectedSOPClassUID;			/* U */
	DIC_US          Status;					/* M */
	DIC_UI          AffectedSOPInstanceUID;			/* U */
	T_DIMSE_DataSetType DataSetType;			/* M */
	unsigned int	opts; /* which optional items are set */
#define O_NDELETE_AFFECTEDSOPCLASSUID		0x0001
#define O_NDELETE_AFFECTEDSOPINSTANCEUID	0x0002
} T_DIMSE_N_DeleteRSP;



/* 
 * Composite  DIMSE Message 
 */

typedef struct {
	T_DIMSE_Command CommandField;	/* M */

	union {
		/* requests */
		T_DIMSE_C_StoreRQ CStoreRQ;
		T_DIMSE_C_EchoRQ  CEchoRQ;
		T_DIMSE_C_FindRQ  CFindRQ;
		T_DIMSE_C_GetRQ   CGetRQ;
		T_DIMSE_C_MoveRQ  CMoveRQ;
		T_DIMSE_C_CancelRQ  CCancelRQ;
		T_DIMSE_N_EventReportRQ NEventReportRQ;
		T_DIMSE_N_GetRQ NGetRQ;
		T_DIMSE_N_SetRQ NSetRQ;
		T_DIMSE_N_ActionRQ NActionRQ;
		T_DIMSE_N_CreateRQ NCreateRQ;
		T_DIMSE_N_DeleteRQ NDeleteRQ;

		/* responses */
		T_DIMSE_C_StoreRSP CStoreRSP;
		T_DIMSE_C_EchoRSP CEchoRSP;
		T_DIMSE_C_FindRSP CFindRSP;
		T_DIMSE_C_GetRSP  CGetRSP;
		T_DIMSE_C_MoveRSP CMoveRSP;
		T_DIMSE_N_EventReportRSP NEventReportRSP;
		T_DIMSE_N_GetRSP NGetRSP;
		T_DIMSE_N_SetRSP NSetRSP;
		T_DIMSE_N_ActionRSP NActionRSP;
		T_DIMSE_N_CreateRSP NCreateRSP;
		T_DIMSE_N_DeleteRSP NDeleteRSP;
	} msg;

} T_DIMSE_Message;


/*
 * Public Function Prototypes
 */


/*
 * High Level DIMSE Messaging.
 */

/*
 * General Information.
 *
 * Many of the DIMSE routines take parameters for a blocking mode
 * and a timeout.  In all cases, these parameters are only used
 * when receiving commands or data sets (reading).  There are no blocking
 * and timeout options for send operations.
 * When receiving commands or data sets:
 * If the parameter blockMode is DIMSE_BLOCKING, the DIMSE routine
 * will wait until a response arrives before returning.
 * If the parameter blockMode is DIMSE_NONBLOCKING then the DIMSE routine
 * will wait at most timeout seconds for a response and if no response arrives
 * will return DIMSE_NODATAAVAILABLE.  In both cases waiting will be
 * interupted by association release, and abort messages, or network
 * disruption.
 */


/*
 * Verification Service Class
 */
 
CONDITION
DIMSE_echoUser(
	/* in */ 
	T_ASC_Association *assoc, DIC_US msgId, 
	/* blocking info for response */
	T_DIMSE_BlockingMode blockMode, int timeout,
	/* out */
	DIC_US *status, DcmDataset **statusDetail);
 
CONDITION
DIMSE_sendEchoResponse(T_ASC_Association * assoc, 
	T_ASC_PresentationContextID presID,
	T_DIMSE_C_EchoRQ *request, DIC_US status, DcmDataset *statusDetail);


/*
 * Storage Service Class
 */

typedef enum {
    DIMSE_StoreBegin, 	/* before data set */
    DIMSE_StoreProgressing, /* during data set */
    DIMSE_StoreEnd		/* after data set */
} T_DIMSE_StoreProgressState;

typedef struct { /* progress structure for store callback routines */
    T_DIMSE_StoreProgressState state;	/* current state */
    DIC_UL callbackCount;	/* callback execution count */
    DIC_UL progressBytes;	/* sent/received so far */
    DIC_UL totalBytes;		/* total/estimated total to send/receive */
} T_DIMSE_StoreProgress;


typedef void (*DIMSE_StoreUserCallback)(
    void *callbackData, 
    T_DIMSE_StoreProgress *progress,
    T_DIMSE_C_StoreRQ *request	/* original store request */
   );

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
	DcmDataset **statusDetail);

typedef void (*DIMSE_StoreProviderCallback)(
    /* in */
    void *callbackData, 
    T_DIMSE_StoreProgress *progress,	/* progress state */
    T_DIMSE_C_StoreRQ *request,		/* original store request */
    char *imageFileName, DcmDataset **imageDataSet, /* being received into */
    /* in/out */
    T_DIMSE_C_StoreRSP *response, 	/* final store response */
    /* out */
    DcmDataset **statusDetail);

CONDITION
DIMSE_storeProvider(/* in */
	T_ASC_Association *assoc, 
	T_ASC_PresentationContextID presIdCmd,
	T_DIMSE_C_StoreRQ *request,
	const char* imageFileName, DcmDataset **imageDataSet,
	DIMSE_StoreProviderCallback callback, void *callbackData,
	/* blocking info for data set */
	T_DIMSE_BlockingMode blockMode, int timeout);

CONDITION
DIMSE_sendStoreResponse(T_ASC_Association * assoc,
	T_ASC_PresentationContextID presID,
	T_DIMSE_C_StoreRQ *request, /* send response to this request */
	T_DIMSE_C_StoreRSP *response, /* response structure */
	DcmDataset *statusDetail);

/*
 * Query/Retrieve Service Class (FIND)
 */


typedef void (*DIMSE_FindUserCallback)(
	/* in */
	void *callbackData, 
	T_DIMSE_C_FindRQ *request, 	/* original find request */
	int responseCount, 
	T_DIMSE_C_FindRSP *response,	/* pending response received */
	DcmDataset *responseIdentifiers /* pending response identifiers */
	);

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
	T_DIMSE_C_FindRSP *response, DcmDataset **statusDetail);

typedef void (*DIMSE_FindProviderCallback)(
	/* in */ 
	void *callbackData,  
	BOOLEAN cancelled, T_DIMSE_C_FindRQ *request, 
	DcmDataset *requestIdentifiers, int responseCount,
	/* out */
	T_DIMSE_C_FindRSP *response,
	DcmDataset **responseIdentifiers,
	DcmDataset **statusDetail);

CONDITION
DIMSE_findProvider(
	/* in */ 
	T_ASC_Association *assoc, 
	T_ASC_PresentationContextID presIdCmd,
	T_DIMSE_C_FindRQ *request,
	DIMSE_FindProviderCallback callback, void *callbackData,
	/* blocking info for data set */
	T_DIMSE_BlockingMode blockMode, int timeout);
 
CONDITION
DIMSE_sendFindResponse(T_ASC_Association * assoc, 
	T_ASC_PresentationContextID presID,
	T_DIMSE_C_FindRQ *request, 
	T_DIMSE_C_FindRSP *response, DcmDataset *responseIdentifiers,
	DcmDataset *statusDetail);

/*
 * Query/Retrieve Service Class (MOVE)
 */

typedef void (*DIMSE_MoveUserCallback)(
	/* in */ 
	void *callbackData,  
	T_DIMSE_C_MoveRQ *request, 
	int responseCount, T_DIMSE_C_MoveRSP *response);

typedef void (*DIMSE_SubOpProviderCallback)(void *subOpCallbackData,
	T_ASC_Network *net, T_ASC_Association **subOpAssoc);

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
        DcmDataset **responseIdentifers);
	
typedef void (*DIMSE_MoveProviderCallback)(
	/* in */ 
	void *callbackData,  
	BOOLEAN cancelled, T_DIMSE_C_MoveRQ *request, 
	DcmDataset *requestIdentifiers, int responseCount,
	/* out */
	T_DIMSE_C_MoveRSP *response, DcmDataset **statusDetail,	
	DcmDataset **responseIdentifiers);

CONDITION
DIMSE_moveProvider(
	/* in */ 
	T_ASC_Association *assoc, 
	T_ASC_PresentationContextID presIdCmd,
	T_DIMSE_C_MoveRQ *request,
	DIMSE_MoveProviderCallback callback, void *callbackData,
	/* blocking info for data set */
	T_DIMSE_BlockingMode blockMode, int timeout);

CONDITION
DIMSE_sendMoveResponse(T_ASC_Association * assoc, 
	T_ASC_PresentationContextID presID, T_DIMSE_C_MoveRQ *request, 
	T_DIMSE_C_MoveRSP *response, DcmDataset *rspIds, 
	DcmDataset *statusDetail);

/*
 * Query/Retrieve Service Class (CANCEL)
 */
 
CONDITION
DIMSE_sendCancelRequest(T_ASC_Association * assoc, 
	T_ASC_PresentationContextID presId, DIC_US msgId);

CONDITION
DIMSE_checkForCancelRQ(T_ASC_Association * assoc, 
    T_ASC_PresentationContextID presId, DIC_US msgId);


/****
 *
 * Low Level DIMSE Messaging
 * With the exception of DIMSE_receiveCommand, 
 * the following function are only intended for use if you
 * wish to implement an alternative interface to the
 * higher level routines provided above.
 * Service call providers will have to use DIMSE_receiveCommand
 * before calling the appropriate provider routine declared above.
 *
 */


typedef void (*DIMSE_ProgressCallback)(void *callbackContext, 
    unsigned long byteCount);


CONDITION 
DIMSE_sendMessageUsingFileData(T_ASC_Association *association,
		  T_ASC_PresentationContextID presID,
		  T_DIMSE_Message *msg, DcmDataset *statusDetail,
		  const char* dataFileName,
		  DIMSE_ProgressCallback callback,
		  void *callbackContext);

CONDITION 
DIMSE_sendMessageUsingMemoryData(T_ASC_Association *association,
		  T_ASC_PresentationContextID presID,
		  T_DIMSE_Message *msg, DcmDataset *statusDetail,
		  DcmDataset *dataObject,
		  DIMSE_ProgressCallback callback,
		  void *callbackContext);



CONDITION
DIMSE_receiveCommand(T_ASC_Association *association,
		     T_DIMSE_BlockingMode blocking,
		     int timeout,
		     T_ASC_PresentationContextID *presID,
		     T_DIMSE_Message *msg, DcmDataset **statusDetail);

CONDITION
DIMSE_receiveDataSetInMemory(T_ASC_Association *association,
		     T_DIMSE_BlockingMode blocking,
		     int timeout, 
		     T_ASC_PresentationContextID *presID,
		     DcmDataset **dataObject,
		     DIMSE_ProgressCallback callback,
		     void *callbackContext);


CONDITION
DIMSE_receiveDataSetInFile(T_ASC_Association *association,
		     T_DIMSE_BlockingMode blocking,
		     int timeout, 
		     T_ASC_PresentationContextID *presID,
		     const char* dataFileName,
		     DIMSE_ProgressCallback callback,
		     void *callbackContext);



/*
 * Misc functions
 */

void DIMSE_debug(int level);
	/* set debug level */

void DIMSE_warning(T_ASC_Association *assoc, const char *format, ...);
char* DIMSE_Message(CONDITION cond);
	/* return string for DIMSE condition (NULL if invalid) */

/* Debugging functions for printing contents of a command structure */
void DIMSE_printCommand(FILE *f, T_DIMSE_Message *msg);
void DIMSE_printCEchoRQ(FILE * f, T_DIMSE_C_EchoRQ * req);
void DIMSE_printCEchoRSP(FILE * f, T_DIMSE_C_EchoRSP * rsp);
void DIMSE_printCStoreRQ(FILE * f, T_DIMSE_C_StoreRQ * req);
void DIMSE_printCStoreRSP(FILE * f, T_DIMSE_C_StoreRSP * rsp);
void DIMSE_printCGetRQ(FILE * f, T_DIMSE_C_GetRQ * req);
void DIMSE_printCGetRSP(FILE * f, T_DIMSE_C_GetRSP * rsp);
void DIMSE_printCMoveRQ(FILE * f, T_DIMSE_C_MoveRQ * req);
void DIMSE_printCMoveRSP(FILE * f, T_DIMSE_C_MoveRSP * rsp);
void DIMSE_printCFindRQ(FILE * f, T_DIMSE_C_FindRQ * req);
void DIMSE_printCFindRSP(FILE * f, T_DIMSE_C_FindRSP * rsp);
void DIMSE_printCCancelRQ(FILE * f, T_DIMSE_C_CancelRQ * req);
void DIMSE_printNEventReportRQ(FILE * f, T_DIMSE_N_EventReportRQ * req);
void DIMSE_printNEventReportRSP(FILE * f, T_DIMSE_N_EventReportRSP * rsp);
void DIMSE_printNGetRQ(FILE * f, T_DIMSE_N_GetRQ * req);
void DIMSE_printNGetRSP(FILE * f, T_DIMSE_N_GetRSP * rsp);
void DIMSE_printNSetRQ(FILE * f, T_DIMSE_N_SetRQ * req);
void DIMSE_printNSetRSP(FILE * f, T_DIMSE_N_SetRSP * rsp);
void DIMSE_printNActionRQ(FILE * f, T_DIMSE_N_ActionRQ * req);
void DIMSE_printNActionRSP(FILE * f, T_DIMSE_N_ActionRSP * rsp);
void DIMSE_printNCreateRQ(FILE * f, T_DIMSE_N_CreateRQ * req);
void DIMSE_printNCreateRSP(FILE * f, T_DIMSE_N_CreateRSP * rsp);
void DIMSE_printNDeleteRQ(FILE * f, T_DIMSE_N_DeleteRQ * req);
void DIMSE_printNDeleteRSP(FILE * f, T_DIMSE_N_DeleteRSP * rsp);


/*
 * Conditions
 */

/*
 * Now define the fixed values for conditions returned by this package.
 * Note that FAC_DIMSE is used to generate these conditions.
 */

#ifndef FAC_DIMSE
#define FAC_DIMSE		10
#endif

#define	DIMSE_NORMAL				/* Successful return */ \
	FORM_COND(FAC_DIMSE, SEV_SUCC,	1)
#define	DIMSE_NETWORKERROR			/* Network Layer Error */ \
	FORM_COND(FAC_DIMSE, SEV_ERROR,	2)
#define	DIMSE_NULLKEY				/* Caller passed in a NULL key */ \
	FORM_COND(FAC_DIMSE, SEV_ERROR,	3)
#define	DIMSE_ILLEGALASSOCIATION		/* Caller passed in an illegal association */ \
	FORM_COND(FAC_DIMSE, SEV_ERROR,	4)
#define	DIMSE_MALLOCERROR			/* Failed to malloc a buffer */ \
	FORM_COND(FAC_DIMSE, SEV_ERROR,	5)
#define	DIMSE_PEERREQUESTEDRELEASE	/* Peer requested release */ \
	FORM_COND(FAC_DIMSE, SEV_SUCC,	6)
#define	DIMSE_READPDVFAILED			/* Read PDV failed */ \
	FORM_COND(FAC_DIMSE, SEV_ERROR,	7)
#define	DIMSE_BUILDFAILED			/* Failed to build an DICOM object */ \
	FORM_COND(FAC_DIMSE, SEV_ERROR,	8)
#define	DIMSE_UNEXPECTEDPDVTYPE		/* Unexpected PDV type */ \
	FORM_COND(FAC_DIMSE, SEV_ERROR,	9)
#define	DIMSE_PEERABORTEDASSOCIATION	/* An association abort was received*/ \
	FORM_COND(FAC_DIMSE, SEV_ERROR, 10)
#define DIMSE_NOVALIDPRESENTATIONCONTEXTID	/* No valid presentation Context ID (send) */ \
	FORM_COND(FAC_DIMSE, SEV_ERROR, 11)
#define	DIMSE_BADMESSAGE			/* Badly formed message (send) */ \
	FORM_COND(FAC_DIMSE, SEV_ERROR,	12)
#define	DIMSE_BADDATA				/* Inappropriate data for message (send) */ \
	FORM_COND(FAC_DIMSE, SEV_ERROR,	13)
#define	DIMSE_PARSEFAILED			/* Failed to parse received message*/ \
	FORM_COND(FAC_DIMSE, SEV_ERROR,	14)
#define	DIMSE_RECEIVEFAILED			/* Failed to receive message */ \
	FORM_COND(FAC_DIMSE, SEV_ERROR,	15)
#define	DIMSE_OBJECTTOOLARGE			/* Object too large */ \
	FORM_COND(FAC_DIMSE, SEV_ERROR,	15)
#define DIMSE_INVALIDPRESENTATIONCONTEXTID	/* Invalid presentation Context ID */ \
	FORM_COND(FAC_DIMSE, SEV_ERROR, 16)
#define DIMSE_BADCOMMANDTYPE			/* Bad command type */ \
	FORM_COND(FAC_DIMSE, SEV_ERROR, 17)
#define DIMSE_SENDFAILED			/* Failed to send message */ \
	FORM_COND(FAC_DIMSE, SEV_ERROR, 18)
#define DIMSE_UNSUPPORTEDTRANSFERSYNTAX		/* xfer syntax unsupported */\
	FORM_COND(FAC_DIMSE, SEV_ERROR, 19)
#define DIMSE_OUTOFRESOURCES			/* out of resources */\
	FORM_COND(FAC_DIMSE, SEV_ERROR, 20)
#define DIMSE_NODATAAVAILABLE			/* no data (timout in non-blocking mode) */\
	FORM_COND(FAC_DIMSE, SEV_SUCC, 21)
#define DIMSE_UNKNOWNCMDELEMENT			/* Unknown command element encountered */\
	FORM_COND(FAC_DIMSE, SEV_SUCC, 22)

#define DIMSE_UNEXPECTEDRESPONSE		/* Unexpected response received  */\
	FORM_COND(FAC_DIMSE, SEV_SUCC, 23)

#define DIMSE_UNEXPECTEDREQUEST			/* Unexpected request received  */\
	FORM_COND(FAC_DIMSE, SEV_SUCC, 24)

#endif

/*
** CVS Log
** $Log: dimse.h,v $
** Revision 1.1  1996-03-26 18:38:45  hewett
** Initial revision
**
**
*/
