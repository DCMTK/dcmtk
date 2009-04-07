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
** Module: dimdump
**
** Purpose:
**      This file contains the routines for printing the
**      contents of parsed DIMSE commands.
**
**      Module Prefix: DIMSE_
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 2009-04-07 09:09:22 $
** Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dimdump.cc,v $
** CVS/RCS Revision:    $Revision: 1.10 $
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
#include "dcmtk/dcmnet/dimse.h"         /* always include the module header */
#include "dcmtk/dcmdata/dcuid.h"

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
    } else {
        name = uid;
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

static void DIMSE_printNStatusString(STD_NAMESPACE ostream& dumpStream, int status)
{
  char buf[20];
  switch(status)
  {
    case STATUS_Success:
      dumpStream << "0x0000: Success";
      break;
    case STATUS_N_Cancel:
      dumpStream << "0xFE00: Cancel";
      break;
    case STATUS_N_AttributeListError:
      dumpStream << "0x0107: Attribute list error";
      break;
    case STATUS_N_SOPClassNotSupported:
      dumpStream << "0x0122: SOP class not supported";
      break;
    case STATUS_N_ClassInstanceConflict:
      dumpStream << "0x0119: Class/instance conflict";
      break;
    case STATUS_N_DuplicateSOPInstance:
      dumpStream << "0x0111: Duplicate SOP instance";
      break;
    case STATUS_N_DuplicateInvocation:
      dumpStream << "0x0210: Duplicate invocation";
      break;
    case STATUS_N_InvalidArgumentValue:
      dumpStream << "0x0115: Invalid argument value";
      break;
    case STATUS_N_InvalidAttributeValue:
      dumpStream << "0x0106: Invalid attribute value";
      break;
    case STATUS_N_InvalidObjectInstance:
      dumpStream << "0x0117: Invalid object instance";
      break;
    case STATUS_N_MissingAttribute:
      dumpStream << "0x0120: Missing attribute";
      break;
    case STATUS_N_MissingAttributeValue:
      dumpStream << "0x0121: Missing attribute value";
      break;
    case STATUS_N_MistypedArgument:
      dumpStream << "0x0212: Mistyped argument";
      break;
    case STATUS_N_NoSuchArgument:
      dumpStream << "0x0114: No such argument";
      break;
    case STATUS_N_NoSuchAttribute:
      dumpStream << "0x0105: No such attribute";
      break;
    case STATUS_N_NoSuchEventType:
      dumpStream << "0x0113: No such event type";
      break;
    case STATUS_N_NoSuchObjectInstance:
      dumpStream << "0x0112: No such object instance";
      break;
    case STATUS_N_NoSuchSOPClass:
      dumpStream << "0x0118: No such SOP class";
      break;
    case STATUS_N_ProcessingFailure:
      dumpStream << "0x0110: Processing failure";
      break;
    case STATUS_N_ResourceLimitation:
      dumpStream << "0x0213: Resource limitation";
      break;
    case STATUS_N_UnrecognizedOperation:
      dumpStream << "0x0211: Unrecognized operation";
      break;
    case STATUS_N_PRINT_BFS_Warn_MemoryAllocation:
      dumpStream << "0xB600: Basic film session warning - memory allocation";
      break;
    case STATUS_N_PRINT_BFS_Warn_NoSessionPrinting:
      dumpStream << "0xB601: Basic film session warning - no session printing";
      break;
    case STATUS_N_PRINT_BFS_Warn_EmptyPage:
      dumpStream << "0xB602: Basic film session warning - empty page";
      break;
    case STATUS_N_PRINT_BFB_Warn_EmptyPage:
      dumpStream << "0xB603: Basic film box warning - empty page";
      break;
    case STATUS_N_PRINT_BFS_Fail_NoFilmBox:
      dumpStream << "0xC600: Basic film session failure - no film box";
      break;
    case STATUS_N_PRINT_BFS_Fail_PrintQueueFull:
      dumpStream << "0xC601: Basic film session failure - print queue full";
      break;
    case STATUS_N_PRINT_BSB_Fail_PrintQueueFull:
      dumpStream << "0xC602: Basic film box failure - print queue full";
      break;
    case STATUS_N_PRINT_BFS_BFB_Fail_ImageSize:
      dumpStream << "0xC603: Basic film session/box failure - Image size";
      break;
    case STATUS_N_PRINT_BFS_BFB_Fail_PositionCollision:
      dumpStream << "0xC604: Basic film session/box failure - Position collision";
      break;
    case STATUS_N_PRINT_IB_Fail_InsufficientMemory:
      dumpStream << "0xC605: Image box failure - Insufficient memory";
      break;
    case STATUS_N_PRINT_IB_Fail_MoreThanOneVOILUT:
      dumpStream << "0xC606: Image box failure - More than one VOI LUT";
      break;
    default:
      sprintf(buf, "0x%04x", status);
      dumpStream << buf << ": Unknown Status Code";
      break;
  }
  return;
}

static void DIMSE_printCStoreStatusString(STD_NAMESPACE ostream& dumpStream, int status)
{
  char buf[20];
  sprintf(buf, "0x%04x", status);

  if ((status & 0xff00) == STATUS_STORE_Refused_OutOfResources)
  {
    dumpStream << buf << ": Error: Refused - Out of resources";
  }
  else if ((status & 0xff00) == STATUS_STORE_Refused_SOPClassNotSupported)
  {
    dumpStream << buf << ": Error: Refused - SOP Class not supported";
  }
  else if ((status & 0xff00) == STATUS_STORE_Error_DataSetDoesNotMatchSOPClass)
  {
    dumpStream << buf << ": Error: Refused - Data Set does not match SOP Class";
  }
  else if ((status & 0xf000) == STATUS_STORE_Error_CannotUnderstand)
  {
    dumpStream << buf << ": Error: Cannot understand";
  }
  else if (status == STATUS_STORE_Warning_CoersionOfDataElements)
  {
    dumpStream << buf << ": Warning: Coersion of data elements";
  }
  else if (status == STATUS_STORE_Warning_DataSetDoesNotMatchSOPClass)
  {
    dumpStream << buf << ": Warning: Data Set does not match SOP Class";
  }
  else if (status == STATUS_STORE_Warning_ElementsDiscarded)
  {
    dumpStream << buf << ": Warning: elements discarded";
  }
  else if (DICOM_WARNING_STATUS(status))
  {
    dumpStream << buf << ": Warning";
  }
  else if (DICOM_PENDING_STATUS(status))
  {
    dumpStream << buf << ": Pending";
  }
  else if (status == STATUS_Success)
  {
    dumpStream << "0x0000: Success";
  }
  else
  {
    dumpStream << buf << ": Unknown Status Code";
  }
  return;
}

static void DIMSE_printCFindStatusString(STD_NAMESPACE ostream& dumpStream, int status)
{
  char buf[20];
  sprintf(buf, "0x%04x", status);

  if ((status & 0xff00) == STATUS_FIND_Refused_OutOfResources)
  {
    dumpStream << buf << ": Error: Refused - Out of resources";
  }
  else if ((status & 0xff00) == STATUS_FIND_Refused_SOPClassNotSupported)
  {
    dumpStream << buf << ": Error: Refused - SOP Class not supported";
  }
  else if ((status & 0xff00) == STATUS_FIND_Failed_IdentifierDoesNotMatchSOPClass)
  {
    dumpStream << buf << ": Error: Failed - Identifier does not match SOP Class";
  }
  else if ((status & 0xf000) == STATUS_FIND_Failed_UnableToProcess)
  {
    dumpStream << buf << ": Error: Failed - Unable to process";
  }
  else if (status == STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest)
  {
    dumpStream << buf << ": Cancel: Matching terminated due to Cancel Request";
  }
  else if (status == STATUS_FIND_Pending_WarningUnsupportedOptionalKeys)
  {
    dumpStream << buf << ": Pending: Warning - unsupported optional keys";
  }
  else if (DICOM_WARNING_STATUS(status))
  {
    dumpStream << buf << ": Warning";
  }
  else if (DICOM_PENDING_STATUS(status))
  {
    dumpStream << buf << ": Pending";
  }
  else if (status == STATUS_Success)
  {
    dumpStream << "0x0000: Success";
  }
  else
  {
    dumpStream << buf << ": Unknown Status Code";
  }
  return;
}

static void DIMSE_printCGetMoveStatusString(STD_NAMESPACE ostream& dumpStream, int status)
{
  char buf[20];
  sprintf(buf, "0x%04x", status);

  if ((status & 0xf000) == STATUS_MOVE_Failed_UnableToProcess)
  {
    dumpStream << buf << ": Error: Failed - Unable to process";
  }
  else if (status == STATUS_MOVE_Refused_OutOfResourcesNumberOfMatches)
  {
    dumpStream << buf << ": Error: Refused - out of resources - number of matches";
  }
  else if (status == STATUS_MOVE_Refused_OutOfResourcesSubOperations)
  {
    dumpStream << buf << ": Error: Refused - out of resources - suboperations";
  }
  else if (status == STATUS_MOVE_Failed_SOPClassNotSupported)
  {
    dumpStream << buf << ": Failed: SOP Class not supported";
  }
  else if (status == STATUS_MOVE_Failed_MoveDestinationUnknown)
  {
    dumpStream << buf << ": Failed: Move Destination unknown";
  }
  else if (status == STATUS_MOVE_Failed_IdentifierDoesNotMatchSOPClass)
  {
    dumpStream << buf << ": Failed: Identifier does not match SOP Class";
  }
  else if (status == STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication)
  {
    dumpStream << buf << ": Cancel: Suboperations terminated due to Cancel Indication";
  }
  else if (status == STATUS_MOVE_Warning_SubOperationsCompleteOneOrMoreFailures)
  {
    dumpStream << buf << ": Warning: Suboperations complete, one or more failures";
  }
  else if (DICOM_WARNING_STATUS(status))
  {
    dumpStream << buf << ": Warning";
  }
  else if (DICOM_PENDING_STATUS(status))
  {
    dumpStream << buf << ": Pending";
  }
  else if (status == STATUS_Success)
  {
    dumpStream << "0x0000: Success";
  }
  else
  {
    dumpStream << buf << ": Unknown Status Code";
  }
  return;
}

static void DIMSE_printCEchoStatusString(STD_NAMESPACE ostream& dumpStream, int status)
{
  char buf[20];
  sprintf(buf, "0x%04x", status);
  if (DICOM_WARNING_STATUS(status))
  {
    dumpStream << buf << ": Warning";
  }
  else if (DICOM_PENDING_STATUS(status))
  {
    dumpStream << buf << ": Pending";
  }
  else if (status == STATUS_Success)
  {
    dumpStream << "0x0000: Success";
  }
  else
  {
    dumpStream << buf << ": Unknown Status Code";
  }
  return;
}


static void DIMSE_printNGetRQ(STD_NAMESPACE ostream& outstream, T_DIMSE_N_GetRQ& msg)
{
  char buf[80];
  const char *uid = dcmFindNameOfUID(msg.RequestedSOPClassUID);
  outstream   << "Message Type                  : N-GET RQ" << OFendl
              << "Message ID                    : " << msg.MessageID << OFendl
              << "Requested SOP Class UID       : " << (uid ? uid : msg.RequestedSOPClassUID) << OFendl
              << "Requested SOP Instance UID    : " << msg.RequestedSOPInstanceUID << OFendl
              << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl
              << "Attribute Identifier List     : ";
  if (msg.ListCount==0) outstream << "none";
  else for (int cList=0; cList<msg.ListCount; cList += 2)
  {
    sprintf(buf,"(%04X,%04X) ", msg.AttributeIdentifierList[cList], msg.AttributeIdentifierList[cList+1]);
    outstream << buf;
  }
  outstream << OFendl;
}

static void DIMSE_printNSetRQ(STD_NAMESPACE ostream& outstream, T_DIMSE_N_SetRQ& msg)
{
  const char *uid = dcmFindNameOfUID(msg.RequestedSOPClassUID);
  outstream   << "Message Type                  : N-SET RQ" << OFendl
              << "Message ID                    : " << msg.MessageID << OFendl
              << "Requested SOP Class UID       : " << (uid ? uid : msg.RequestedSOPClassUID) << OFendl
              << "Requested SOP Instance UID    : " << msg.RequestedSOPInstanceUID << OFendl
              << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl;
}

static void DIMSE_printNActionRQ(STD_NAMESPACE ostream& outstream, T_DIMSE_N_ActionRQ& msg)
{
  const char *uid = dcmFindNameOfUID(msg.RequestedSOPClassUID);
  outstream   << "Message Type                  : N-ACTION RQ" << OFendl
              << "Message ID                    : " << msg.MessageID << OFendl
              << "Requested SOP Class UID       : " << (uid ? uid : msg.RequestedSOPClassUID) << OFendl
              << "Requested SOP Instance UID    : " << msg.RequestedSOPInstanceUID << OFendl
              << "Action Type ID                : " << msg.ActionTypeID << OFendl
              << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl;
}

static void DIMSE_printNCreateRQ(STD_NAMESPACE ostream& outstream, T_DIMSE_N_CreateRQ& msg)
{
  const char *uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
  outstream   << "Message Type                  : N-CREATE RQ" << OFendl
              << "Message ID                    : " << msg.MessageID << OFendl
              << "Affected SOP Class UID        : " << (uid ? uid : msg.AffectedSOPClassUID) << OFendl
              << "Affected SOP Instance UID     : ";
  if (msg.opts & O_NCREATE_AFFECTEDSOPINSTANCEUID) outstream << msg.AffectedSOPInstanceUID << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl;
}

static void DIMSE_printNDeleteRQ(STD_NAMESPACE ostream& outstream, T_DIMSE_N_DeleteRQ& msg)
{
  const char *uid = dcmFindNameOfUID(msg.RequestedSOPClassUID);
  outstream   << "Message Type                  : N-DELETE RQ" << OFendl
              << "Message ID                    : " << msg.MessageID << OFendl
              << "Requested SOP Class UID       : " << (uid ? uid : msg.RequestedSOPClassUID) << OFendl
              << "Requested SOP Instance UID    : " << msg.RequestedSOPInstanceUID << OFendl
              << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl;
}

static void DIMSE_printNEventReportRQ(STD_NAMESPACE ostream& outstream, T_DIMSE_N_EventReportRQ& msg)
{
  const char *uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
  outstream   << "Message Type                  : N-EVENT-REPORT RQ" << OFendl
              << "Message ID                    : " << msg.MessageID << OFendl
              << "Affected SOP Class UID        : " << (uid ? uid : msg.AffectedSOPClassUID) << OFendl
              << "Affected SOP Instance UID     : " << msg.AffectedSOPInstanceUID << OFendl
              << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl
              << "Event Type ID                 : " << msg.EventTypeID << OFendl;
}

static void DIMSE_printNGetRSP(STD_NAMESPACE ostream& outstream, T_DIMSE_N_GetRSP& msg)
{
  const char *uid = NULL;
  if (msg.opts & O_NGET_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
  outstream   << "Message Type                  : N-GET RSP" << OFendl
              << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
              << "Affected SOP Class UID        : " ;
  if (msg.opts & O_NGET_AFFECTEDSOPCLASSUID) outstream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Affected SOP Instance UID     : ";
  if (msg.opts & O_NGET_AFFECTEDSOPINSTANCEUID) outstream << msg.AffectedSOPInstanceUID << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl
              << "DIMSE Status                  : ";
  DIMSE_printNStatusString(outstream, msg.DimseStatus);
  outstream   << OFendl;
}

static void DIMSE_printNSetRSP(STD_NAMESPACE ostream& outstream, T_DIMSE_N_SetRSP& msg)
{
  const char *uid = NULL;
  if (msg.opts & O_NSET_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
  outstream   << "Message Type                  : N-SET RSP" << OFendl
              << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
              << "Affected SOP Class UID        : " ;
  if (msg.opts & O_NSET_AFFECTEDSOPCLASSUID) outstream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Affected SOP Instance UID     : ";
  if (msg.opts & O_NSET_AFFECTEDSOPINSTANCEUID) outstream << msg.AffectedSOPInstanceUID << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl
              << "DIMSE Status                  : ";
  DIMSE_printNStatusString(outstream, msg.DimseStatus);
  outstream   << OFendl;
}

static void DIMSE_printNActionRSP(STD_NAMESPACE ostream& outstream, T_DIMSE_N_ActionRSP& msg)
{
  const char *uid = NULL;
  if (msg.opts & O_NACTION_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
  outstream   << "Message Type                  : N-ACTION RSP" << OFendl
              << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
              << "Affected SOP Class UID        : " ;
  if (msg.opts & O_NACTION_AFFECTEDSOPCLASSUID) outstream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Affected SOP Instance UID     : ";
  if (msg.opts & O_NACTION_AFFECTEDSOPINSTANCEUID) outstream << msg.AffectedSOPInstanceUID << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl
              << "Action Type ID                : ";
  if (msg.opts & O_NACTION_ACTIONTYPEID) outstream << msg.ActionTypeID << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "DIMSE Status                  : ";
  DIMSE_printNStatusString(outstream, msg.DimseStatus);
  outstream   << OFendl;
}

static void DIMSE_printNCreateRSP(STD_NAMESPACE ostream& outstream, T_DIMSE_N_CreateRSP& msg)
{
  const char *uid = NULL;
  if (msg.opts & O_NCREATE_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
  outstream   << "Message Type                  : N-CREATE RSP" << OFendl
              << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
              << "Affected SOP Class UID        : " ;
  if (msg.opts & O_NCREATE_AFFECTEDSOPCLASSUID) outstream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Affected SOP Instance UID     : ";
  if (msg.opts & O_NCREATE_AFFECTEDSOPINSTANCEUID) outstream << msg.AffectedSOPInstanceUID << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl
              << "DIMSE Status                  : ";
  DIMSE_printNStatusString(outstream, msg.DimseStatus);
  outstream   << OFendl;
}

static void DIMSE_printNDeleteRSP(STD_NAMESPACE ostream& outstream, T_DIMSE_N_DeleteRSP& msg)
{
  const char *uid = NULL;
  if (msg.opts & O_NDELETE_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
  outstream   << "Message Type                  : N-DELETE RSP" << OFendl
              << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
              << "Affected SOP Class UID        : " ;
  if (msg.opts & O_NDELETE_AFFECTEDSOPCLASSUID) outstream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Affected SOP Instance UID     : ";
  if (msg.opts & O_NDELETE_AFFECTEDSOPINSTANCEUID) outstream << msg.AffectedSOPInstanceUID << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl
              << "DIMSE Status                  : ";
  DIMSE_printNStatusString(outstream, msg.DimseStatus);
  outstream   << OFendl;
}

static void DIMSE_printNEventReportRSP(STD_NAMESPACE ostream& outstream, T_DIMSE_N_EventReportRSP& msg)
{
  const char *uid = NULL;
  if (msg.opts & O_NEVENTREPORT_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
  outstream   << "Message Type                  : N-EVENT-REPORT RSP" << OFendl
              << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
              << "Affected SOP Class UID        : " ;
  if (msg.opts & O_NEVENTREPORT_AFFECTEDSOPCLASSUID) outstream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Affected SOP Instance UID     : ";
  if (msg.opts & O_NEVENTREPORT_AFFECTEDSOPINSTANCEUID) outstream << msg.AffectedSOPInstanceUID << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl
              << "Event Type ID                 : ";
  if (msg.opts & O_NEVENTREPORT_EVENTTYPEID) outstream << msg.EventTypeID << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "DIMSE Status                  : ";
  DIMSE_printNStatusString(outstream, msg.DimseStatus);
  outstream   << OFendl;
}

static void DIMSE_printCStoreRQ(STD_NAMESPACE ostream& outstream, T_DIMSE_C_StoreRQ& msg)
{
  const char *uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
  outstream   << "Message Type                  : C-STORE RQ" << OFendl
              << "Message ID                    : " << msg.MessageID << OFendl
              << "Affected SOP Class UID        : " << (uid ? uid : msg.AffectedSOPClassUID) << OFendl
              << "Affected SOP Instance UID     : " << msg.AffectedSOPInstanceUID << OFendl
              << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl
              << "Priority                      : ";
  switch (msg.Priority)
  {
    case DIMSE_PRIORITY_LOW:
      outstream << "low" << OFendl;
      break;
    case DIMSE_PRIORITY_MEDIUM:
      outstream << "medium" << OFendl;
      break;
    case DIMSE_PRIORITY_HIGH:
      outstream << "high" << OFendl;
      break;
  }
  outstream   << "Move Originator AE Title      : ";
  if (msg.opts & O_STORE_MOVEORIGINATORAETITLE) outstream << msg.MoveOriginatorApplicationEntityTitle << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Move Originator ID            : ";
  if (msg.opts & O_STORE_MOVEORIGINATORID) outstream << msg.MoveOriginatorID << OFendl;
  else outstream << "none" << OFendl;
}

static void DIMSE_printCGetRQ(STD_NAMESPACE ostream& outstream, T_DIMSE_C_GetRQ& msg)
{
  const char *uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
  outstream   << "Message Type                  : C-GET RQ" << OFendl
              << "Message ID                    : " << msg.MessageID << OFendl
              << "Affected SOP Class UID        : " << (uid ? uid : msg.AffectedSOPClassUID) << OFendl
              << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl
              << "Priority                      : ";
  switch (msg.Priority)
  {
    case DIMSE_PRIORITY_LOW:
      outstream << "low" << OFendl;
      break;
    case DIMSE_PRIORITY_MEDIUM:
      outstream << "medium" << OFendl;
      break;
    case DIMSE_PRIORITY_HIGH:
      outstream << "high" << OFendl;
      break;
  }
}

static void DIMSE_printCFindRQ(STD_NAMESPACE ostream& outstream, T_DIMSE_C_FindRQ& msg)
{
  const char *uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
  outstream   << "Message Type                  : C-FIND RQ" << OFendl
              << "Message ID                    : " << msg.MessageID << OFendl
              << "Affected SOP Class UID        : " << (uid ? uid : msg.AffectedSOPClassUID) << OFendl
              << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl
              << "Priority                      : ";
  switch (msg.Priority)
  {
    case DIMSE_PRIORITY_LOW:
      outstream << "low" << OFendl;
      break;
    case DIMSE_PRIORITY_MEDIUM:
      outstream << "medium" << OFendl;
      break;
    case DIMSE_PRIORITY_HIGH:
      outstream << "high" << OFendl;
      break;
  }
}

static void DIMSE_printCMoveRQ(STD_NAMESPACE ostream& outstream, T_DIMSE_C_MoveRQ& msg)
{
  const char *uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
  outstream   << "Message Type                  : C-MOVE RQ" << OFendl
              << "Message ID                    : " << msg.MessageID << OFendl
              << "Affected SOP Class UID        : " << (uid ? uid : msg.AffectedSOPClassUID) << OFendl
              << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl
              << "Priority                      : ";
  switch (msg.Priority)
  {
    case DIMSE_PRIORITY_LOW:
      outstream << "low" << OFendl;
      break;
    case DIMSE_PRIORITY_MEDIUM:
      outstream << "medium" << OFendl;
      break;
    case DIMSE_PRIORITY_HIGH:
      outstream << "high" << OFendl;
      break;
  }
  outstream   << "Move Destination              : " << msg.MoveDestination << OFendl;
}

static void DIMSE_printCEchoRQ(STD_NAMESPACE ostream& outstream, T_DIMSE_C_EchoRQ& msg)
{
  outstream   << "Message Type                  : C-ECHO RQ" << OFendl
              << "Message ID                    : " << msg.MessageID << OFendl
              << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl;
}

static void DIMSE_printCCancelRQ(STD_NAMESPACE ostream& outstream, T_DIMSE_C_CancelRQ& msg)
{
  outstream   << "Message Type                  : C-CANCEL RQ" << OFendl
              << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
              << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl;
}

static void DIMSE_printCStoreRSP(STD_NAMESPACE ostream& outstream, T_DIMSE_C_StoreRSP& msg)
{
  const char *uid = NULL;
  if (msg.opts & O_STORE_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
  outstream   << "Message Type                  : C-STORE RSP" << OFendl
              << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
              << "Affected SOP Class UID        : ";
  if (msg.opts & O_STORE_AFFECTEDSOPCLASSUID) outstream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Affected SOP Instance UID     : ";
  if (msg.opts & O_STORE_AFFECTEDSOPINSTANCEUID) outstream << msg.AffectedSOPInstanceUID << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl
              << "DIMSE Status                  : ";
  DIMSE_printCStoreStatusString(outstream, msg.DimseStatus);
  outstream   << OFendl;
}

static void DIMSE_printCGetRSP(STD_NAMESPACE ostream& outstream, T_DIMSE_C_GetRSP& msg)
{
  const char *uid = NULL;
  if (msg.opts & O_GET_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
  outstream   << "Message Type                  : C-GET RSP" << OFendl
              << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
              << "Affected SOP Class UID        : ";
  if (msg.opts & O_GET_AFFECTEDSOPCLASSUID) outstream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Remaining Suboperations       : ";
  if (msg.opts & O_GET_NUMBEROFREMAININGSUBOPERATIONS) outstream << msg.NumberOfRemainingSubOperations << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Completed Suboperations       : ";
  if (msg.opts & O_GET_NUMBEROFCOMPLETEDSUBOPERATIONS) outstream << msg.NumberOfCompletedSubOperations << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Failed Suboperations          : ";
  if (msg.opts & O_GET_NUMBEROFFAILEDSUBOPERATIONS) outstream << msg.NumberOfFailedSubOperations << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Warning Suboperations         : ";
  if (msg.opts & O_GET_NUMBEROFWARNINGSUBOPERATIONS) outstream << msg.NumberOfWarningSubOperations << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl
              << "DIMSE Status                  : ";
  DIMSE_printCGetMoveStatusString(outstream, msg.DimseStatus);
  outstream   << OFendl;
}

static void DIMSE_printCFindRSP(STD_NAMESPACE ostream& outstream, T_DIMSE_C_FindRSP& msg)
{
  const char *uid = NULL;
  if (msg.opts & O_FIND_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
  outstream   << "Message Type                  : C-FIND RSP" << OFendl
              << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
              << "Affected SOP Class UID        : ";
  if (msg.opts & O_FIND_AFFECTEDSOPCLASSUID) outstream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl
              << "DIMSE Status                  : ";
  DIMSE_printCFindStatusString(outstream, msg.DimseStatus);
  outstream   << OFendl;
}

static void DIMSE_printCMoveRSP(STD_NAMESPACE ostream& outstream, T_DIMSE_C_MoveRSP& msg)
{
  const char *uid = NULL;
  if (msg.opts & O_MOVE_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
  outstream   << "Message Type                  : C-MOVE RSP" << OFendl
              << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
              << "Affected SOP Class UID        : ";
  if (msg.opts & O_MOVE_AFFECTEDSOPCLASSUID) outstream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Remaining Suboperations       : ";
  if (msg.opts & O_MOVE_NUMBEROFREMAININGSUBOPERATIONS) outstream << msg.NumberOfRemainingSubOperations << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Completed Suboperations       : ";
  if (msg.opts & O_MOVE_NUMBEROFCOMPLETEDSUBOPERATIONS) outstream << msg.NumberOfCompletedSubOperations << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Failed Suboperations          : ";
  if (msg.opts & O_MOVE_NUMBEROFFAILEDSUBOPERATIONS) outstream << msg.NumberOfFailedSubOperations << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Warning Suboperations         : ";
  if (msg.opts & O_MOVE_NUMBEROFWARNINGSUBOPERATIONS) outstream << msg.NumberOfWarningSubOperations << OFendl;
  else outstream << "none" << OFendl;
  outstream   << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl
              << "DIMSE Status                  : ";
  DIMSE_printCGetMoveStatusString(outstream, msg.DimseStatus);
  outstream   << OFendl;
}

static void DIMSE_printCEchoRSP(STD_NAMESPACE ostream& outstream, T_DIMSE_C_EchoRSP& msg)
{
  const char *uid = NULL;
  if (msg.opts & O_ECHO_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
  outstream   << "Message Type                  : C-ECHO RSP" << OFendl
              << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
              << "Affected SOP Class UID        : ";
  if (msg.opts & O_ECHO_AFFECTEDSOPCLASSUID) outstream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
  else outstream << "none" << OFendl;

  outstream   << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << OFendl
              << "DIMSE Status                  : ";
  DIMSE_printCEchoStatusString(outstream, msg.DimseStatus);
  outstream   << OFendl;
}

void DIMSE_printMessage(STD_NAMESPACE ostream& outstream, T_DIMSE_Message &msg, DcmItem *dataset)
{
    switch(msg.CommandField)
    {
      case DIMSE_N_GET_RQ:
        DIMSE_printNGetRQ(outstream, msg.msg.NGetRQ);
        break;
      case DIMSE_N_SET_RQ:
        DIMSE_printNSetRQ(outstream, msg.msg.NSetRQ);
        break;
      case DIMSE_N_ACTION_RQ:
        DIMSE_printNActionRQ(outstream, msg.msg.NActionRQ);
        break;
      case DIMSE_N_CREATE_RQ:
        DIMSE_printNCreateRQ(outstream, msg.msg.NCreateRQ);
        break;
      case DIMSE_N_DELETE_RQ:
        DIMSE_printNDeleteRQ(outstream, msg.msg.NDeleteRQ);
        break;
      case DIMSE_N_EVENT_REPORT_RQ:
        DIMSE_printNEventReportRQ(outstream, msg.msg.NEventReportRQ);
        break;
      case DIMSE_N_GET_RSP:
        DIMSE_printNGetRSP(outstream, msg.msg.NGetRSP);
        break;
      case DIMSE_N_SET_RSP:
        DIMSE_printNSetRSP(outstream, msg.msg.NSetRSP);
        break;
      case DIMSE_N_ACTION_RSP:
        DIMSE_printNActionRSP(outstream, msg.msg.NActionRSP);
        break;
      case DIMSE_N_CREATE_RSP:
        DIMSE_printNCreateRSP(outstream, msg.msg.NCreateRSP);
        break;
      case DIMSE_N_DELETE_RSP:
        DIMSE_printNDeleteRSP(outstream, msg.msg.NDeleteRSP);
        break;
      case DIMSE_N_EVENT_REPORT_RSP:
        DIMSE_printNEventReportRSP(outstream, msg.msg.NEventReportRSP);
        break;
      case DIMSE_C_STORE_RQ:
        DIMSE_printCStoreRQ(outstream, msg.msg.CStoreRQ);
        break;
      case DIMSE_C_GET_RQ:
        DIMSE_printCGetRQ(outstream, msg.msg.CGetRQ);
        break;
      case DIMSE_C_FIND_RQ:
        DIMSE_printCFindRQ(outstream, msg.msg.CFindRQ);
        break;
      case DIMSE_C_MOVE_RQ:
        DIMSE_printCMoveRQ(outstream, msg.msg.CMoveRQ);
        break;
      case DIMSE_C_ECHO_RQ:
        DIMSE_printCEchoRQ(outstream, msg.msg.CEchoRQ);
        break;
      case DIMSE_C_CANCEL_RQ:
        DIMSE_printCCancelRQ(outstream, msg.msg.CCancelRQ);
        break;
      case DIMSE_C_STORE_RSP:
        DIMSE_printCStoreRSP(outstream, msg.msg.CStoreRSP);
        break;
      case DIMSE_C_GET_RSP:
        DIMSE_printCGetRSP(outstream, msg.msg.CGetRSP);
        break;
      case DIMSE_C_FIND_RSP:
        DIMSE_printCFindRSP(outstream, msg.msg.CFindRSP);
        break;
      case DIMSE_C_MOVE_RSP:
        DIMSE_printCMoveRSP(outstream, msg.msg.CMoveRSP);
        break;
      case DIMSE_C_ECHO_RSP:
        DIMSE_printCEchoRSP(outstream, msg.msg.CEchoRSP);
        break;
      default:
        outstream   << "Message Type                  : UNKNOWN (DIMSE Protocol Error)" << OFendl;
        break;
    }

    if (dataset) dataset->print(outstream, DCMTypes::PF_shortenLongTagValues);
    return;
}

/*
** CVS Log
** $Log: dimdump.cc,v $
** Revision 1.10  2009-04-07 09:09:22  joergr
** Fixed issue with unknown UID in uid2name().
**
** Revision 1.9  2006/08/15 16:04:29  meichel
** Updated the code in module dcmnet to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.8  2005/12/08 15:44:40  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.7  2003/03/12 17:35:04  meichel
** Updated DcmObject::print() flags
**
** Revision 1.6  2002/11/27 13:04:39  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.5  2000/06/07 08:57:54  meichel
** dcmnet DIMSE routines now allow to retrieve raw command sets as DcmDataset
**   objects, e.g. for logging purposes. Added enhanced message dump functions.
**
** Revision 1.4  2000/02/23 15:12:32  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.3  2000/02/01 10:24:09  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.2  1998/08/10 08:53:42  meichel
** renamed member variable in DIMSE structures from "Status" to
**   "DimseStatus". This is required if dcmnet is used together with
**   <X11/Xlib.h> where Status is #define'd as int.
**
** Revision 1.1.1.1  1996/03/26 18:38:45  hewett
** Initial Release.
**
**
*/
