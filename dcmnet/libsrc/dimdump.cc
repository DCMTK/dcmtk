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
** Module: dimdump
**
** Purpose: 
**	This file contains the routines for printing the 
**	contents of parsed DIMSE commands.
**
**	Module Prefix: DIMSE_
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1998-08-10 08:53:42 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dimdump.cc,v $
** CVS/RCS Revision:	$Revision: 1.2 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
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

#include "dcuid.h"

/*
 * Public Functions
 */

void
DIMSE_printCommand(FILE * f, T_DIMSE_Message * msg)
{
    switch (msg->CommandField) {
	case DIMSE_NOTHING:
	fprintf(f, "Undefined Request/Response\n");
	break;
    case DIMSE_C_STORE_RQ:
	DIMSE_printCStoreRQ(f, &msg->msg.CStoreRQ);
	break;
    case DIMSE_C_STORE_RSP:
	DIMSE_printCStoreRSP(f, &msg->msg.CStoreRSP);
	break;
    case DIMSE_C_GET_RQ:
	DIMSE_printCGetRQ(f, &msg->msg.CGetRQ);
	break;
    case DIMSE_C_GET_RSP:
	DIMSE_printCGetRSP(f, &msg->msg.CGetRSP);
	break;
    case DIMSE_C_FIND_RQ:
	DIMSE_printCFindRQ(f, &msg->msg.CFindRQ);
	break;
    case DIMSE_C_FIND_RSP:
	DIMSE_printCFindRSP(f, &msg->msg.CFindRSP);
	break;
    case DIMSE_C_MOVE_RQ:
	DIMSE_printCMoveRQ(f, &msg->msg.CMoveRQ);
	break;
    case DIMSE_C_MOVE_RSP:
	DIMSE_printCMoveRSP(f, &msg->msg.CMoveRSP);
	break;
    case DIMSE_C_ECHO_RQ:
	DIMSE_printCEchoRQ(f, &msg->msg.CEchoRQ);
	break;
    case DIMSE_C_ECHO_RSP:
	DIMSE_printCEchoRSP(f, &msg->msg.CEchoRSP);
	break;
    case DIMSE_C_CANCEL_RQ:
	DIMSE_printCCancelRQ(f, &msg->msg.CCancelRQ);
	break;
    case DIMSE_N_EVENT_REPORT_RQ:
	DIMSE_printNEventReportRQ(f, &msg->msg.NEventReportRQ);
	break;
    case DIMSE_N_EVENT_REPORT_RSP:
	DIMSE_printNEventReportRSP(f, &msg->msg.NEventReportRSP);
	break;
    case DIMSE_N_GET_RQ:
	DIMSE_printNGetRQ(f, &msg->msg.NGetRQ);
	break;
    case DIMSE_N_GET_RSP:
	DIMSE_printNGetRSP(f, &msg->msg.NGetRSP);
	break;
    case DIMSE_N_SET_RQ:
	DIMSE_printNSetRQ(f, &msg->msg.NSetRQ);
	break;
    case DIMSE_N_SET_RSP:
	DIMSE_printNSetRSP(f, &msg->msg.NSetRSP);
	break;
    case DIMSE_N_ACTION_RQ:
	DIMSE_printNActionRQ(f, &msg->msg.NActionRQ);
	break;
    case DIMSE_N_ACTION_RSP:
	DIMSE_printNActionRSP(f, &msg->msg.NActionRSP);
	break;
    case DIMSE_N_CREATE_RQ:
	DIMSE_printNCreateRQ(f, &msg->msg.NCreateRQ);
	break;
    case DIMSE_N_CREATE_RSP:
	DIMSE_printNCreateRSP(f, &msg->msg.NCreateRSP);
	break;
    case DIMSE_N_DELETE_RQ:
	DIMSE_printNDeleteRQ(f, &msg->msg.NDeleteRQ);
	break;
    case DIMSE_N_DELETE_RSP:
	DIMSE_printNDeleteRSP(f, &msg->msg.NDeleteRSP);
	break;
    default:
	fprintf(f, "DIMSE_printCommand: Bad msg->CommandField\n");
	break;
    }
}

static const char*
uid2name(const char* uid) 
{
    static char staticname[DIC_UI_LEN+2];
    const char* name = dcmFindNameOfUID(uid);
    /* return the name of the UID or the UID if unknown */
    if (name) {
	sprintf(staticname, "=%s", name);
	name = staticname;
    }
    return (name);
}

void
DIMSE_printCEchoRQ(FILE * f, T_DIMSE_C_EchoRQ * req)
{
    fprintf(f, "C-Echo RQ: MsgID %d\n", req->MessageID);
    fprintf(f, "  AffectedSOPClassUID: %s\n",
	    uid2name(req->AffectedSOPClassUID));
    fprintf(f, "  Data Set: %s\n",
	    (req->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}

void
DIMSE_printCEchoRSP(FILE * f, T_DIMSE_C_EchoRSP * rsp)
{
    fprintf(f, "C-Echo RSP: MsgID: %d [Status=%s]\n",
	    rsp->MessageIDBeingRespondedTo,
	    DU_cstoreStatusString(rsp->DimseStatus));
    if (rsp->opts & O_ECHO_AFFECTEDSOPCLASSUID)
	fprintf(f, "  AffectedSOPClassUID: %s\n",
		uid2name(rsp->AffectedSOPClassUID));
    fprintf(f, "  Data Set: %s\n",
	    (rsp->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}

void
DIMSE_printCStoreRQ(FILE * f, T_DIMSE_C_StoreRQ * req)
{
    fprintf(f, "C-Store RQ: MsgID: %d\n",
	    req->MessageID);
    fprintf(f, "  AffectedSOPClassUID: %s\n",
	    uid2name(req->AffectedSOPClassUID));
    fprintf(f, "  AffectedSOPInstanceUID: %s\n",
	    req->AffectedSOPInstanceUID);
    fprintf(f, "  Priority: %d\n", req->Priority);
    fprintf(f, "  Data Set: %s\n",
	    (req->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
    if (req->opts & O_STORE_MOVEORIGINATORAETITLE)
	fprintf(f, "  Move Originator AE Title: %s\n",
		req->MoveOriginatorApplicationEntityTitle);
    if (req->opts & O_STORE_MOVEORIGINATORID)
	fprintf(f, "  Move Originator ID: %d\n",
		req->MoveOriginatorID);
}

void
DIMSE_printCStoreRSP(FILE * f, T_DIMSE_C_StoreRSP * rsp)
{
    fprintf(f, "C-Store RSP: MsgID: %d [Status=%s]\n",
	    rsp->MessageIDBeingRespondedTo,
	    DU_cstoreStatusString(rsp->DimseStatus));
    if (rsp->opts & O_STORE_AFFECTEDSOPCLASSUID)
	fprintf(f, "  AffectedSOPClassUID: %s\n",
		uid2name(rsp->AffectedSOPClassUID));
    if (rsp->opts & O_STORE_AFFECTEDSOPINSTANCEUID)
	fprintf(f, "  AffectedSOPInstanceUID: %s\n",
		rsp->AffectedSOPInstanceUID);
    fprintf(f, "  Data Set: %s\n",
	    (rsp->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}

void
DIMSE_printCGetRQ(FILE * f, T_DIMSE_C_GetRQ * req)
{
    fprintf(f, "C-Get RQ: MsgID: %d\n",
	    req->MessageID);
    fprintf(f, "  AffectedSOPClassUID: %s\n",
	    uid2name(req->AffectedSOPClassUID));
    fprintf(f, "  Priority: %d\n", req->Priority);
    fprintf(f, "  Data Set: %s\n",
	    (req->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}

void
DIMSE_printCGetRSP(FILE * f, T_DIMSE_C_GetRSP * rsp)
{
    fprintf(f, "C-Get RSP: MsgID: %d [Status=%s]\n",
	    rsp->MessageIDBeingRespondedTo,
	    DU_cgetStatusString(rsp->DimseStatus));
    if (rsp->opts & O_GET_AFFECTEDSOPCLASSUID)
	fprintf(f, "  AffectedSOPClassUID: %s\n",
		uid2name(rsp->AffectedSOPClassUID));
    fprintf(f, "  Data Set: %s\n",
	    (rsp->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
    if (rsp->opts & O_GET_NUMBEROFREMAININGSUBOPERATIONS)
	fprintf(f, "  NumberOfRemainingSubOperations: %d\n",
		rsp->NumberOfRemainingSubOperations);
    if (rsp->opts & O_GET_NUMBEROFCOMPLETEDSUBOPERATIONS)
	fprintf(f, "  NumberOfCompletedSubOperations: %d\n",
		rsp->NumberOfCompletedSubOperations);
    if (rsp->opts & O_GET_NUMBEROFFAILEDSUBOPERATIONS)
	fprintf(f, "  NumberOfFailedSubOperations: %d\n",
		rsp->NumberOfFailedSubOperations);
    if (rsp->opts & O_GET_NUMBEROFWARNINGSUBOPERATIONS)
	fprintf(f, "  NumberOfWarningSubOperations: %d\n",
		rsp->NumberOfWarningSubOperations);
}

void
DIMSE_printCMoveRQ(FILE * f, T_DIMSE_C_MoveRQ * req)
{
    fprintf(f, "C-Move RQ: MsgID: %d\n",
	    req->MessageID);
    fprintf(f, "  AffectedSOPClassUID: %s\n",
	    uid2name(req->AffectedSOPClassUID));
    fprintf(f, "  Priority: %d\n", req->Priority);
    fprintf(f, "  Data Set: %s\n",
	    (req->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
    fprintf(f, "  Move Destination: %s\n", req->MoveDestination);    
}

void
DIMSE_printCMoveRSP(FILE * f, T_DIMSE_C_MoveRSP * rsp)
{
    fprintf(f, "C-Move RSP: MsgID: %d [Status=%s]\n",
	    rsp->MessageIDBeingRespondedTo,
	    DU_cmoveStatusString(rsp->DimseStatus));
    if (rsp->opts & O_MOVE_AFFECTEDSOPCLASSUID)
	fprintf(f, "  AffectedSOPClassUID: %s\n",
		uid2name(rsp->AffectedSOPClassUID));
    fprintf(f, "  Data Set: %s\n",
	    (rsp->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
    if (rsp->opts & O_MOVE_NUMBEROFREMAININGSUBOPERATIONS)
	fprintf(f, "  NumberOfRemainingSubOperations: %d\n",
		rsp->NumberOfRemainingSubOperations);
    if (rsp->opts & O_MOVE_NUMBEROFCOMPLETEDSUBOPERATIONS)
	fprintf(f, "  NumberOfCompletedSubOperations: %d\n",
		rsp->NumberOfCompletedSubOperations);
    if (rsp->opts & O_MOVE_NUMBEROFFAILEDSUBOPERATIONS)
	fprintf(f, "  NumberOfFailedSubOperations: %d\n",
		rsp->NumberOfFailedSubOperations);
    if (rsp->opts & O_MOVE_NUMBEROFWARNINGSUBOPERATIONS)
	fprintf(f, "  NumberOfWarningSubOperations: %d\n",
		rsp->NumberOfWarningSubOperations);
}

void
DIMSE_printCFindRQ(FILE * f, T_DIMSE_C_FindRQ * req)
{
    fprintf(f, "C-Find RQ: MsgID: %d\n",
	    req->MessageID);
    fprintf(f, "  AffectedSOPClassUID: %s\n",
	    uid2name(req->AffectedSOPClassUID));
    fprintf(f, "  Priority: %d\n", req->Priority);
    fprintf(f, "  Data Set: %s\n",
	    (req->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}

void
DIMSE_printCFindRSP(FILE * f, T_DIMSE_C_FindRSP * rsp)
{
    fprintf(f, "C-Find RSP: MsgID: %d [Status=%s]\n",
	    rsp->MessageIDBeingRespondedTo,
	    DU_cfindStatusString(rsp->DimseStatus));
    if (rsp->opts & O_FIND_AFFECTEDSOPCLASSUID)
	fprintf(f, "  AffectedSOPClassUID: %s\n",
		uid2name(rsp->AffectedSOPClassUID));
    fprintf(f, "  Data Set: %s\n",
	    (rsp->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}

void
DIMSE_printCCancelRQ(FILE * f, T_DIMSE_C_CancelRQ * req)
{
    fprintf(f, "C-Cancel RQ: MsgID: %d\n",
	    req->MessageIDBeingRespondedTo);
    fprintf(f, "  Data Set: %s\n",
	    (req->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}

void DIMSE_printNEventReportRQ(FILE * f, T_DIMSE_N_EventReportRQ * req)
{
    fprintf(f, "N-Event-Report RQ: MsgID: %d\n",
	    req->MessageID);
    fprintf(f, "  AffectedSOPClassUID: %s\n",
	    uid2name(req->AffectedSOPClassUID));
    fprintf(f, "  AffectedSOPInstanceUID: %s\n",
	    req->AffectedSOPInstanceUID);
    fprintf(f, "  EventTypeID: 0x%hx\n", req->EventTypeID);
    fprintf(f, "  Data Set: %s\n",
	    (req->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}

void DIMSE_printNEventReportRSP(FILE * f, T_DIMSE_N_EventReportRSP * rsp)
{
    fprintf(f, "N-Event-Report RSP: MsgID: %d [Status=0x%hx]\n",
	    rsp->MessageIDBeingRespondedTo, rsp->DimseStatus);
    if (rsp->opts & O_NEVENTREPORT_AFFECTEDSOPCLASSUID)
	fprintf(f, "  AffectedSOPClassUID: %s\n",
		uid2name(rsp->AffectedSOPClassUID));
    if (rsp->opts & O_NEVENTREPORT_AFFECTEDSOPINSTANCEUID)
	fprintf(f, "  AffectedSOPInstanceUID: %s\n",
		rsp->AffectedSOPInstanceUID);
    if (rsp->opts & O_NEVENTREPORT_EVENTTYPEID)
	fprintf(f, "  EventTypeID: 0x%hx\n",
		rsp->EventTypeID);
    fprintf(f, "  Data Set: %s\n",
	    (rsp->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}

void DIMSE_printNGetRQ(FILE * f, T_DIMSE_N_GetRQ * req)
{
    int i;
    fprintf(f, "N-Get RQ: MsgID: %d\n",
	    req->MessageID);
    fprintf(f, "  RequestedSOPClassUID: %s\n",
	    uid2name(req->RequestedSOPClassUID));
    fprintf(f, "  RequestedSOPInstanceUID: %s\n",
	    req->RequestedSOPInstanceUID);
    fprintf(f, "  Data Set: %s\n",
	    (req->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
    for (i=0; i<req->ListCount; i += 2) {
        fprintf(f, "  AttributeIdentiferList[%d-%d]: (0x%hx,0x%hx)\n",
		i, i+1, req->AttributeIdentifierList[i],
		req->AttributeIdentifierList[i+1]);
    }
}

void DIMSE_printNGetRSP(FILE * f, T_DIMSE_N_GetRSP * rsp)
{
    fprintf(f, "N-Get RSP: MsgID: %d [Status=0x%hx]\n",
	    rsp->MessageIDBeingRespondedTo, rsp->DimseStatus);
    if (rsp->opts & O_NGET_AFFECTEDSOPCLASSUID)
	fprintf(f, "  AffectedSOPClassUID: %s\n",
		uid2name(rsp->AffectedSOPClassUID));
    if (rsp->opts & O_NGET_AFFECTEDSOPINSTANCEUID)
	fprintf(f, "  AffectedSOPInstanceUID: %s\n",
		rsp->AffectedSOPInstanceUID);
    fprintf(f, "  Data Set: %s\n",
	    (rsp->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}

void DIMSE_printNSetRQ(FILE * f, T_DIMSE_N_SetRQ * req)
{
    fprintf(f, "N-Set RQ: MsgID: %d\n",
	    req->MessageID);
    fprintf(f, "  RequestedSOPClassUID: %s\n",
	    uid2name(req->RequestedSOPClassUID));
    fprintf(f, "  RequestedSOPInstanceUID: %s\n",
	    req->RequestedSOPInstanceUID);
    fprintf(f, "  Data Set: %s\n",
	    (req->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}

void DIMSE_printNSetRSP(FILE * f, T_DIMSE_N_SetRSP * rsp)
{
    fprintf(f, "N-Set RSP: MsgID: %d [Status=0x%hx]\n",
	    rsp->MessageIDBeingRespondedTo, rsp->DimseStatus);
    if (rsp->opts & O_NSET_AFFECTEDSOPCLASSUID)
	fprintf(f, "  AffectedSOPClassUID: %s\n",
		uid2name(rsp->AffectedSOPClassUID));
    if (rsp->opts & O_NSET_AFFECTEDSOPINSTANCEUID)
	fprintf(f, "  AffectedSOPInstanceUID: %s\n",
		rsp->AffectedSOPInstanceUID);
    fprintf(f, "  Data Set: %s\n",
	    (rsp->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}

void DIMSE_printNActionRQ(FILE * f, T_DIMSE_N_ActionRQ * req)
{
    fprintf(f, "N-Action RQ: MsgID: %d\n",
	    req->MessageID);
    fprintf(f, "  RequestedSOPClassUID: %s\n",
	    uid2name(req->RequestedSOPClassUID));
    fprintf(f, "  RequestedSOPInstanceUID: %s\n",
	    req->RequestedSOPInstanceUID);
    fprintf(f, "  ActionTypeID: 0x%hx\n", req->ActionTypeID);
    fprintf(f, "  Data Set: %s\n",
	    (req->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}

void DIMSE_printNActionRSP(FILE * f, T_DIMSE_N_ActionRSP * rsp)
{
    fprintf(f, "N-Action RSP: MsgID: %d [Status=0x%hx]\n",
	    rsp->MessageIDBeingRespondedTo, rsp->DimseStatus);
    if (rsp->opts & O_NACTION_AFFECTEDSOPCLASSUID)
	fprintf(f, "  AffectedSOPClassUID: %s\n",
		uid2name(rsp->AffectedSOPClassUID));
    if (rsp->opts & O_NACTION_AFFECTEDSOPINSTANCEUID)
	fprintf(f, "  AffectedSOPInstanceUID: %s\n",
		rsp->AffectedSOPInstanceUID);
    if (rsp->opts & O_NACTION_ACTIONTYPEID)
	fprintf(f, "  ActionTypeID: 0x%hx\n",
		rsp->ActionTypeID);
    fprintf(f, "  Data Set: %s\n",
	    (rsp->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}

void DIMSE_printNCreateRQ(FILE * f, T_DIMSE_N_CreateRQ * req)
{
    fprintf(f, "N-Create RQ: MsgID: %d\n",
	    req->MessageID);
    fprintf(f, "  AffectedSOPClassUID: %s\n",
	    uid2name(req->AffectedSOPClassUID));
    if (req->opts & O_NCREATE_AFFECTEDSOPINSTANCEUID)
        fprintf(f, "  AffectedSOPInstanceUID: %s\n",
	        req->AffectedSOPInstanceUID);
    fprintf(f, "  Data Set: %s\n",
	    (req->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}

void DIMSE_printNCreateRSP(FILE * f, T_DIMSE_N_CreateRSP * rsp)
{
    fprintf(f, "N-Create RSP: MsgID: %d [Status=0x%hx]\n",
	    rsp->MessageIDBeingRespondedTo, rsp->DimseStatus);
    if (rsp->opts & O_NCREATE_AFFECTEDSOPCLASSUID)
	fprintf(f, "  AffectedSOPClassUID: %s\n",
		uid2name(rsp->AffectedSOPClassUID));
    if (rsp->opts & O_NCREATE_AFFECTEDSOPINSTANCEUID)
	fprintf(f, "  AffectedSOPInstanceUID: %s\n",
		rsp->AffectedSOPInstanceUID);
    fprintf(f, "  Data Set: %s\n",
	    (rsp->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}

void DIMSE_printNDeleteRQ(FILE * f, T_DIMSE_N_DeleteRQ * req)
{
    fprintf(f, "N-Delete RQ: MsgID: %d\n",
	    req->MessageID);
    fprintf(f, "  RequestedSOPClassUID: %s\n",
	    uid2name(req->RequestedSOPClassUID));
    fprintf(f, "  RequestedSOPInstanceUID: %s\n",
	    req->RequestedSOPInstanceUID);
    fprintf(f, "  Data Set: %s\n",
	    (req->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}

void DIMSE_printNDeleteRSP(FILE * f, T_DIMSE_N_DeleteRSP * rsp)
{
    fprintf(f, "N-Delete RSP: MsgID: %d [Status=0x%hx]\n",
	    rsp->MessageIDBeingRespondedTo, rsp->DimseStatus);
    if (rsp->opts & O_NDELETE_AFFECTEDSOPCLASSUID)
	fprintf(f, "  AffectedSOPClassUID: %s\n",
		uid2name(rsp->AffectedSOPClassUID));
    if (rsp->opts & O_NDELETE_AFFECTEDSOPINSTANCEUID)
	fprintf(f, "  AffectedSOPInstanceUID: %s\n",
		rsp->AffectedSOPInstanceUID);
    fprintf(f, "  Data Set: %s\n",
	    (rsp->DataSetType == DIMSE_DATASET_NULL) ?
	    "Not Present" : "Present");
}


/*
** CVS Log
** $Log: dimdump.cc,v $
** Revision 1.2  1998-08-10 08:53:42  meichel
** renamed member variable in DIMSE structures from "Status" to
**   "DimseStatus". This is required if dcmnet is used together with
**   <X11/Xlib.h> where Status is #define'd as int.
**
** Revision 1.1.1.1  1996/03/26 18:38:45  hewett
** Initial Release.
**
**
*/
