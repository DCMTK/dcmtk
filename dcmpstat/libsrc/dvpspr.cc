/*
 *
 *  Copyright (C) 1998-99, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSPrintMessageHandler
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-10-28 08:18:57 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dvpspr.h"

static void printStatusString(ostream& dumpStream, int status)
{
  char buf[20];
  switch(status)
  {
    case 0x0000:
      dumpStream << "0x0000: Success";
      break;
    case 0xFE00:
      dumpStream << "0xFE00: Cancel";
      break;
    case 0x0107:
      dumpStream << "0x0107: Attribute list error";
      break;
    case 0x0122:
      dumpStream << "0x0122: SOP class not supported";
      break;
    case 0x0119:
      dumpStream << "0x0119: Class/instance conflict";
      break;
    case 0x0111:
      dumpStream << "0x0111: Duplicate SOP instance";
      break;
    case 0x0210:
      dumpStream << "0x0210: Duplicate invocation";
      break;
    case 0x0115:
      dumpStream << "0x0115: Invalid argument value";
      break;
    case 0x0106:
      dumpStream << "0x0106: Invalid attribute value";
      break;
    case 0x0117:
      dumpStream << "0x0117: Invalid object instance";
      break;
    case 0x0120:
      dumpStream << "0x0120: Missing attribute";
      break;
    case 0x0121:
      dumpStream << "0x0121: Missing attribute value";
      break;
    case 0x0212:
      dumpStream << "0x0212: Mistyped argument";
      break;
    case 0x0114:
      dumpStream << "0x0114: No such argument";
      break;
    case 0x0105:
      dumpStream << "0x0105: No such attribute";
      break;
    case 0x0113:
      dumpStream << "0x0113: No such event type";
      break;
    case 0x0112:
      dumpStream << "0x0112: No such object instance";
      break;
    case 0x0118:
      dumpStream << "0x0118: No such SOP class";
      break;
    case 0x0110:
      dumpStream << "0x0110: Processing failure";
      break;
    case 0x0213:
      dumpStream << "0x0213: Resource limitation";
      break;
    case 0x0211:
      dumpStream << "0x0211: Unrecognized operation";
      break;
    case 0xB600:
      dumpStream << "0xB600: Basic film session warning - memory allocation";
      break;
    case 0xB601:
      dumpStream << "0xB601: Basic film session warning - no session printing";
      break;
    case 0xB602:
      dumpStream << "0xB602: Basic film session warning - empty page";
      break;
    case 0xB603:
      dumpStream << "0xB603: Basic film box warning - empty page";
      break;
    case 0xC600:
      dumpStream << "0xC600: Basic film session failure - no film box";
      break;
    case 0xC601:
      dumpStream << "0xC601: Basic film session failure - print queue full";
      break;
    case 0xC602:
      dumpStream << "0xC602: Basic film box failure - print queue full";
      break;
    case 0xC603:
      dumpStream << "0xC603: Basic film session/box failure - Image size";
      break;
    case 0xC604:
      dumpStream << "0xC604: Basic film session/box failure - Position collision";
      break;
    case 0xC605:
      dumpStream << "0xC605: Image box failure - Insufficient memory";
      break;
    case 0xC606:
      dumpStream << "0xC606: Image box failure - More than one VOI LUT";
      break;
    default:
      sprintf(buf, "0x%04x", status);
      dumpStream << buf << ": Unknown Status Code";
      break;
  }
  return;
}

/* --------------- class DVPSPrintMessageHandler --------------- */

DVPSPrintMessageHandler::DVPSPrintMessageHandler()
: assoc(NULL)
, net(NULL)
, eventHandler(NULL)
, blockMode(DIMSE_BLOCKING)
, timeout(0)
, dumpStream(NULL)
, logstream(&cerr)
{
}

DVPSPrintMessageHandler::~DVPSPrintMessageHandler()
{
  abortAssociation(); // won't do any harm if there is no association in place
}

void DVPSPrintMessageHandler::dumpNMessage(T_DIMSE_Message &msg, DcmItem *dataset, OFBool outgoing)
{
    if (dumpStream == NULL) return;
    const char *uid = NULL;
    char buf[80];
    
    if (outgoing)
    {
      *dumpStream << "===================== OUTGOING DIMSE MESSAGE ====================" << endl;
    } else {
      *dumpStream << "===================== INCOMING DIMSE MESSAGE ====================" << endl;
    }
    switch(msg.CommandField)
    {
      case DIMSE_N_GET_RQ:
        uid = dcmFindNameOfUID(msg.msg.NGetRQ.RequestedSOPClassUID);
        *dumpStream << "Message Type                  : N-GET RQ" << endl
                    << "Message ID                    : " << msg.msg.NGetRQ.MessageID << endl
                    << "Requested SOP Class UID       : " << (uid ? uid : msg.msg.NGetRQ.RequestedSOPClassUID) << endl
                    << "Requested SOP Instance UID    : " << msg.msg.NGetRQ.RequestedSOPInstanceUID << endl
                    << "Data Set                      : " << ((msg.msg.NGetRQ.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << endl
                    << "Attribute Identifier List     : ";
        if (msg.msg.NGetRQ.ListCount==0) *dumpStream << "none";
        else for (int cList=0; cList<msg.msg.NGetRQ.ListCount; cList += 2)
        {
          sprintf(buf,"(%04X,%04X) ", msg.msg.NGetRQ.AttributeIdentifierList[cList], msg.msg.NGetRQ.AttributeIdentifierList[cList+1]);
          *dumpStream << buf;
        }
        *dumpStream << endl;
        break;
      case DIMSE_N_SET_RQ:
        uid = dcmFindNameOfUID(msg.msg.NSetRQ.RequestedSOPClassUID);
        *dumpStream << "Message Type                  : N-SET RQ" << endl
                    << "Message ID                    : " << msg.msg.NSetRQ.MessageID << endl
                    << "Requested SOP Class UID       : " << (uid ? uid : msg.msg.NSetRQ.RequestedSOPClassUID) << endl
                    << "Requested SOP Instance UID    : " << msg.msg.NSetRQ.RequestedSOPInstanceUID << endl
                    << "Data Set                      : " << ((msg.msg.NSetRQ.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << endl;
        break;
      case DIMSE_N_ACTION_RQ:
        uid = dcmFindNameOfUID(msg.msg.NSetRQ.RequestedSOPClassUID);
        *dumpStream << "Message Type                  : N-ACTION RQ" << endl
                    << "Message ID                    : " << msg.msg.NActionRQ.MessageID << endl
                    << "Requested SOP Class UID       : " << (uid ? uid : msg.msg.NActionRQ.RequestedSOPClassUID) << endl
                    << "Requested SOP Instance UID    : " << msg.msg.NActionRQ.RequestedSOPInstanceUID << endl
                    << "Action Type ID                : " << msg.msg.NActionRQ.ActionTypeID << endl
                    << "Data Set                      : " << ((msg.msg.NActionRQ.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << endl;
        break;
      case DIMSE_N_CREATE_RQ:
        uid = dcmFindNameOfUID(msg.msg.NCreateRQ.AffectedSOPClassUID);
        *dumpStream << "Message Type                  : N-CREATE RQ" << endl
                    << "Message ID                    : " << msg.msg.NCreateRQ.MessageID << endl
                    << "Affected SOP Class UID        : " << (uid ? uid : msg.msg.NCreateRQ.AffectedSOPClassUID) << endl
                    << "Affected SOP Instance UID     : ";
        if (msg.msg.NCreateRQ.opts & O_NCREATE_AFFECTEDSOPINSTANCEUID) *dumpStream << msg.msg.NCreateRQ.AffectedSOPInstanceUID << endl;
        else *dumpStream << "none" << endl;
        *dumpStream << "Data Set                      : " << ((msg.msg.NCreateRQ.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << endl;
        break;
      case DIMSE_N_DELETE_RQ:
        uid = dcmFindNameOfUID(msg.msg.NDeleteRQ.RequestedSOPClassUID);
        *dumpStream << "Message Type                  : N-DELETE RQ" << endl
                    << "Message ID                    : " << msg.msg.NDeleteRQ.MessageID << endl
                    << "Requested SOP Class UID       : " << (uid ? uid : msg.msg.NDeleteRQ.RequestedSOPClassUID) << endl
                    << "Requested SOP Instance UID    : " << msg.msg.NDeleteRQ.RequestedSOPInstanceUID << endl
                    << "Data Set                      : " << ((msg.msg.NDeleteRQ.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << endl;
        break;
      case DIMSE_N_EVENT_REPORT_RQ:
        uid = dcmFindNameOfUID(msg.msg.NEventReportRQ.AffectedSOPClassUID);
        *dumpStream << "Message Type                  : N-EVENT-REPORT RQ" << endl
                    << "Message ID                    : " << msg.msg.NEventReportRQ.MessageID << endl
                    << "Affected SOP Class UID        : " << (uid ? uid : msg.msg.NEventReportRQ.AffectedSOPClassUID) << endl
                    << "Affected SOP Instance UID     : " << msg.msg.NEventReportRQ.AffectedSOPInstanceUID << endl
                    << "Data Set                      : " << ((msg.msg.NEventReportRQ.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << endl
                    << "Event Type ID                 : " << msg.msg.NEventReportRQ.EventTypeID << endl;
        break;
      case DIMSE_N_GET_RSP:
        uid = NULL;
        if (msg.msg.NGetRSP.opts & O_NGET_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.msg.NGetRSP.AffectedSOPClassUID);
        *dumpStream << "Message Type                  : N-GET RSP" << endl
                    << "Message ID Being Responded To : " << msg.msg.NGetRSP.MessageIDBeingRespondedTo << endl                    
                    << "Affected SOP Class UID        : " ;
        if (msg.msg.NGetRSP.opts & O_NGET_AFFECTEDSOPCLASSUID) *dumpStream << (uid ? uid : msg.msg.NGetRSP.AffectedSOPClassUID) << endl;
        else *dumpStream << "none" << endl;
        *dumpStream << "Affected SOP Instance UID     : ";
        if (msg.msg.NGetRSP.opts & O_NGET_AFFECTEDSOPINSTANCEUID) *dumpStream << msg.msg.NGetRSP.AffectedSOPInstanceUID << endl;
        else *dumpStream << "none" << endl;
        *dumpStream << "Data Set                      : " << ((msg.msg.NGetRSP.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << endl
                    << "DIMSE Status                  : ";
        printStatusString(*dumpStream, msg.msg.NGetRSP.DimseStatus);
        *dumpStream << endl;
        break;
      case DIMSE_N_SET_RSP:
        uid = NULL;
        if (msg.msg.NSetRSP.opts & O_NSET_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.msg.NSetRSP.AffectedSOPClassUID);
        *dumpStream << "Message Type                  : N-SET RSP" << endl
                    << "Message ID Being Responded To : " << msg.msg.NSetRSP.MessageIDBeingRespondedTo << endl                    
                    << "Affected SOP Class UID        : " ;
        if (msg.msg.NSetRSP.opts & O_NSET_AFFECTEDSOPCLASSUID) *dumpStream << (uid ? uid : msg.msg.NSetRSP.AffectedSOPClassUID) << endl;
        else *dumpStream << "none" << endl;
        *dumpStream << "Affected SOP Instance UID     : ";
        if (msg.msg.NSetRSP.opts & O_NSET_AFFECTEDSOPINSTANCEUID) *dumpStream << msg.msg.NSetRSP.AffectedSOPInstanceUID << endl;
        else *dumpStream << "none" << endl;
        *dumpStream << "Data Set                      : " << ((msg.msg.NSetRSP.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << endl
                    << "DIMSE Status                  : ";
        printStatusString(*dumpStream, msg.msg.NSetRSP.DimseStatus);
        *dumpStream << endl;
        break;
      case DIMSE_N_ACTION_RSP:
        uid = NULL;
        if (msg.msg.NActionRSP.opts & O_NACTION_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.msg.NActionRSP.AffectedSOPClassUID);
        *dumpStream << "Message Type                  : N-ACTION RSP" << endl
                    << "Message ID Being Responded To : " << msg.msg.NActionRSP.MessageIDBeingRespondedTo << endl                    
                    << "Affected SOP Class UID        : " ;
        if (msg.msg.NActionRSP.opts & O_NACTION_AFFECTEDSOPCLASSUID) *dumpStream << (uid ? uid : msg.msg.NActionRSP.AffectedSOPClassUID) << endl;
        else *dumpStream << "none" << endl;
        *dumpStream << "Affected SOP Instance UID     : ";
        if (msg.msg.NActionRSP.opts & O_NACTION_AFFECTEDSOPINSTANCEUID) *dumpStream << msg.msg.NActionRSP.AffectedSOPInstanceUID << endl;
        else *dumpStream << "none" << endl;
        *dumpStream << "Data Set                      : " << ((msg.msg.NActionRSP.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << endl
                    << "Action Type ID                : ";
        if (msg.msg.NActionRSP.opts & O_NACTION_ACTIONTYPEID) *dumpStream << msg.msg.NActionRSP.ActionTypeID << endl;
        else *dumpStream << "none" << endl;
        *dumpStream << "DIMSE Status                  : ";
        printStatusString(*dumpStream, msg.msg.NActionRSP.DimseStatus);
        *dumpStream << endl;
        break;
      case DIMSE_N_CREATE_RSP:
        uid = NULL;
        if (msg.msg.NCreateRSP.opts & O_NCREATE_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.msg.NCreateRSP.AffectedSOPClassUID);
        *dumpStream << "Message Type                  : N-CREATE RSP" << endl
                    << "Message ID Being Responded To : " << msg.msg.NCreateRSP.MessageIDBeingRespondedTo << endl                    
                    << "Affected SOP Class UID        : " ;
        if (msg.msg.NCreateRSP.opts & O_NCREATE_AFFECTEDSOPCLASSUID) *dumpStream << (uid ? uid : msg.msg.NCreateRSP.AffectedSOPClassUID) << endl;
        else *dumpStream << "none" << endl;
        *dumpStream << "Affected SOP Instance UID     : ";
        if (msg.msg.NCreateRSP.opts & O_NCREATE_AFFECTEDSOPINSTANCEUID) *dumpStream << msg.msg.NCreateRSP.AffectedSOPInstanceUID << endl;
        else *dumpStream << "none" << endl;
        *dumpStream << "Data Set                      : " << ((msg.msg.NCreateRSP.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << endl
                    << "DIMSE Status                  : ";
        printStatusString(*dumpStream, msg.msg.NCreateRSP.DimseStatus);
        *dumpStream << endl;
        break;
      case DIMSE_N_DELETE_RSP:
        uid = NULL;
        if (msg.msg.NDeleteRSP.opts & O_NDELETE_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.msg.NDeleteRSP.AffectedSOPClassUID);
        *dumpStream << "Message Type                  : N-DELETE RSP" << endl
                    << "Message ID Being Responded To : " << msg.msg.NDeleteRSP.MessageIDBeingRespondedTo << endl                    
                    << "Affected SOP Class UID        : " ;
        if (msg.msg.NDeleteRSP.opts & O_NDELETE_AFFECTEDSOPCLASSUID) *dumpStream << (uid ? uid : msg.msg.NDeleteRSP.AffectedSOPClassUID) << endl;
        else *dumpStream << "none" << endl;
        *dumpStream << "Affected SOP Instance UID     : ";
        if (msg.msg.NDeleteRSP.opts & O_NDELETE_AFFECTEDSOPINSTANCEUID) *dumpStream << msg.msg.NDeleteRSP.AffectedSOPInstanceUID << endl;
        else *dumpStream << "none" << endl;
        *dumpStream << "Data Set                      : " << ((msg.msg.NDeleteRSP.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << endl
                    << "DIMSE Status                  : ";
        printStatusString(*dumpStream, msg.msg.NDeleteRSP.DimseStatus);
        *dumpStream << endl;
        break;
      case DIMSE_N_EVENT_REPORT_RSP:
        uid = NULL;
        if (msg.msg.NEventReportRSP.opts & O_NEVENTREPORT_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.msg.NEventReportRSP.AffectedSOPClassUID);
        *dumpStream << "Message Type                  : N-EVENT-REPORT RSP" << endl
                    << "Message ID Being Responded To : " << msg.msg.NEventReportRSP.MessageIDBeingRespondedTo << endl                    
                    << "Affected SOP Class UID        : " ;
        if (msg.msg.NEventReportRSP.opts & O_NEVENTREPORT_AFFECTEDSOPCLASSUID) *dumpStream << (uid ? uid : msg.msg.NEventReportRSP.AffectedSOPClassUID) << endl;
        else *dumpStream << "none" << endl;
        *dumpStream << "Affected SOP Instance UID     : ";
        if (msg.msg.NEventReportRSP.opts & O_NEVENTREPORT_AFFECTEDSOPINSTANCEUID) *dumpStream << msg.msg.NEventReportRSP.AffectedSOPInstanceUID << endl;
        else *dumpStream << "none" << endl;
        *dumpStream << "Data Set                      : " << ((msg.msg.NEventReportRSP.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present" ) << endl
                    << "Event Type ID                 : ";
        if (msg.msg.NEventReportRSP.opts & O_NEVENTREPORT_EVENTTYPEID) *dumpStream << msg.msg.NEventReportRSP.EventTypeID << endl;
        else *dumpStream << "none" << endl;
        *dumpStream << "DIMSE Status                  : ";
        printStatusString(*dumpStream, msg.msg.NEventReportRSP.DimseStatus);
        *dumpStream << endl;
        break;
      default:
        *dumpStream << "Message Type                  : UNKNOWN (NOT DIMSE-N)" << endl;
        break;
    }

    if (dataset) dataset->print(*dumpStream, OFFalse);
    *dumpStream << "======================= END DIMSE MESSAGE =======================" << endl << endl;  
    return;
}

CONDITION DVPSPrintMessageHandler::sendNRequest(
    T_ASC_PresentationContextID presId,
    T_DIMSE_Message &request,
    DcmDataset *rqDataSet,
    T_DIMSE_Message &response,
    DcmDataset* &statusDetail,
    DcmDataset* &rspDataset)
{
    CONDITION cond;
    T_DIMSE_Command expectedResponse;
    DIC_US expectedMessageID=0;
    if (assoc == NULL)
    {
      return COND_PushCondition(DIMSE_ILLEGALASSOCIATION, "DIMSE: Cannot send message with NULL association");
    }

    T_DIMSE_DataSetType datasetType = DIMSE_DATASET_NULL;
    if (rqDataSet && (rqDataSet->card() > 0)) datasetType = DIMSE_DATASET_PRESENT;
    
    switch(request.CommandField)
    {
      case DIMSE_N_GET_RQ:
        request.msg.NGetRQ.DataSetType = datasetType;
        expectedResponse = DIMSE_N_GET_RSP;
        expectedMessageID = request.msg.NGetRQ.MessageID;
        break;
      case DIMSE_N_SET_RQ:
        request.msg.NSetRQ.DataSetType = datasetType;
        expectedResponse = DIMSE_N_SET_RSP;
        expectedMessageID = request.msg.NSetRQ.MessageID;
        break;
      case DIMSE_N_ACTION_RQ:
        request.msg.NActionRQ.DataSetType = datasetType;
        expectedResponse = DIMSE_N_ACTION_RSP;
        expectedMessageID = request.msg.NActionRQ.MessageID;
        break;
      case DIMSE_N_CREATE_RQ:
        request.msg.NCreateRQ.DataSetType = datasetType;
        expectedResponse = DIMSE_N_CREATE_RSP;
        expectedMessageID = request.msg.NCreateRQ.MessageID;
        break;
      case DIMSE_N_DELETE_RQ:
        request.msg.NDeleteRQ.DataSetType = datasetType;
        expectedResponse = DIMSE_N_DELETE_RSP;
        expectedMessageID = request.msg.NDeleteRQ.MessageID;
        break;
      default:
        return COND_PushCondition(DIMSE_BADCOMMANDTYPE, 
          "DVPSPrintMessageHandler::sendNRequest: Unexpected Request Command Field: 0x%x",
          (unsigned)request.CommandField);
        /* break; */
    }

    dumpNMessage(request, rqDataSet, OFTrue);
    cond = DIMSE_sendMessageUsingMemoryData(assoc, presId, &request, NULL, rqDataSet, NULL, NULL);
    if (cond != DIMSE_NORMAL) return cond;

    T_ASC_PresentationContextID thisPresId;
    T_DIMSE_Message eventReportRsp;
    DIC_US eventReportStatus;
    do
    {
        thisPresId = presId;
        statusDetail = NULL;
        cond = DIMSE_receiveCommand(assoc, blockMode, timeout, &thisPresId, &response, &statusDetail);
        if (cond != DIMSE_NORMAL) return cond;

        if (response.CommandField == DIMSE_N_EVENT_REPORT_RQ)
        {
          /* handle N-EVENT-REPORT-RQ */
          rspDataset = NULL;
          if (response.msg.NEventReportRQ.DataSetType == DIMSE_DATASET_PRESENT)
          {
            cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout, &thisPresId, &rspDataset, NULL, NULL);
            if (cond != DIMSE_NORMAL) return cond;
          }  
          dumpNMessage(response, rspDataset, OFFalse);
          // call event handler if registered
          eventReportStatus = STATUS_Success;
          if (eventHandler) eventReportStatus = eventHandler->handleEvent(response.msg.NEventReportRQ, rspDataset, statusDetail);
          if (rspDataset) delete rspDataset;
          rspDataset = NULL;
          if (statusDetail) delete statusDetail;
          statusDetail = NULL;
          
          // send back N-EVENT-REPORT-RSP */
          eventReportRsp.CommandField = DIMSE_N_EVENT_REPORT_RSP;
          eventReportRsp.msg.NEventReportRSP.MessageIDBeingRespondedTo = response.msg.NEventReportRQ.MessageID;
          eventReportRsp.msg.NEventReportRSP.EventTypeID = response.msg.NEventReportRQ.EventTypeID;
          eventReportRsp.msg.NEventReportRSP.DimseStatus = eventReportStatus;
          eventReportRsp.msg.NEventReportRSP.DataSetType = DIMSE_DATASET_NULL;
          eventReportRsp.msg.NEventReportRSP.opts = O_NEVENTREPORT_EVENTTYPEID;
          eventReportRsp.msg.NEventReportRSP.AffectedSOPClassUID[0] = 0;
          eventReportRsp.msg.NEventReportRSP.AffectedSOPInstanceUID[0] = 0;
          dumpNMessage(eventReportRsp, NULL, OFTrue);
          cond = DIMSE_sendMessageUsingMemoryData(assoc, thisPresId, &eventReportRsp, NULL, NULL, NULL, NULL);
          if (cond != DIMSE_NORMAL) return cond;
        } else {
          /* No N-EVENT-REPORT-RQ. Check if this message is what we expected */
          if (response.CommandField != expectedResponse)
          {
            return COND_PushCondition(DIMSE_UNEXPECTEDRESPONSE, 
              "DIMSE: Unexpected Response Command Field: 0x%x",
              (unsigned)response.CommandField);
          }
          T_DIMSE_DataSetType responseDataset = DIMSE_DATASET_NULL;
          DIC_US responseMessageID = 0;
	  /** change request to response */     
          switch(expectedResponse)
          {
            case DIMSE_N_GET_RSP:
              responseDataset = response.msg.NGetRSP.DataSetType;
              responseMessageID = response.msg.NGetRSP.MessageIDBeingRespondedTo;
              break;
            case DIMSE_N_SET_RSP:
              responseDataset = response.msg.NSetRSP.DataSetType;
              responseMessageID = response.msg.NSetRSP.MessageIDBeingRespondedTo;
              break;
            case DIMSE_N_ACTION_RSP:
              responseDataset = response.msg.NActionRSP.DataSetType;
              responseMessageID = response.msg.NActionRSP.MessageIDBeingRespondedTo;
              break;
            case DIMSE_N_CREATE_RSP:
              responseDataset = response.msg.NCreateRSP.DataSetType;
              responseMessageID = response.msg.NCreateRSP.MessageIDBeingRespondedTo;
              break;
            case DIMSE_N_DELETE_RSP:
              responseDataset = response.msg.NDeleteRSP.DataSetType;
              responseMessageID = response.msg.NDeleteRSP.MessageIDBeingRespondedTo;
              break;
            default:
              return COND_PushCondition(DIMSE_UNEXPECTEDRESPONSE, 
                "DIMSE: Unexpected Response Command Field: 0x%x",
                (unsigned)response.CommandField);
              /* break; */
          }
          if (responseMessageID != expectedMessageID)
          {
            return COND_PushCondition(DIMSE_UNEXPECTEDRESPONSE, 
              "DIMSE: Unexpected Message ID Being Responded To: 0x%x",
              (unsigned)responseMessageID);
          }
          rspDataset = NULL;
          if (responseDataset == DIMSE_DATASET_PRESENT)
          {
            cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout, &thisPresId, &rspDataset, NULL, NULL);
            if (cond != DIMSE_NORMAL) return cond;
          }
          dumpNMessage(response, rspDataset, OFFalse);
        }
    } while (response.CommandField == DIMSE_N_EVENT_REPORT_RQ);
    return DIMSE_NORMAL;
}    

CONDITION DVPSPrintMessageHandler::createRQ(
    const char *sopclassUID, 
    OFString& sopinstanceUID, 
    DcmDataset *attributeListIn, 
    Uint16& status, 
    DcmDataset* &attributeListOut)
{
  if (assoc == NULL)
  {
    return COND_PushCondition(DIMSE_ILLEGALASSOCIATION, "DVPSPrintMessageHandler::createRQ: Cannot send message with NULL association");
  }
  if (sopclassUID==NULL)
  {
    return COND_PushCondition(DIMSE_NULLKEY, "DVPSPrintMessageHandler::createRQ: NULL key");
  }

  T_ASC_PresentationContextID presCtx = findAcceptedPC(sopclassUID);
  if (presCtx == 0)
  {
    return COND_PushCondition(DIMSE_NOVALIDPRESENTATIONCONTEXTID, "DVPSPrintMessageHandler::createRQ: no valid presentation context");
  }  

  T_DIMSE_Message request;
  T_DIMSE_Message response;
  DcmDataset *statusDetail = NULL;

  // construct N-CREATE-RQ
  request.CommandField = DIMSE_N_CREATE_RQ;
  request.msg.NCreateRQ.MessageID = assoc->nextMsgID++;
  strcpy(request.msg.NCreateRQ.AffectedSOPClassUID, sopclassUID);
  if (sopinstanceUID.size() > 0)
  {
    strcpy(request.msg.NCreateRQ.AffectedSOPInstanceUID, sopinstanceUID.c_str());
    request.msg.NCreateRQ.opts = O_NCREATE_AFFECTEDSOPINSTANCEUID;
  } else {
    request.msg.NCreateRQ.AffectedSOPInstanceUID[0] = 0;
    request.msg.NCreateRQ.opts = 0;
  }
  
  CONDITION cond = sendNRequest(presCtx, request, attributeListIn, response, statusDetail, attributeListOut);
  if (DIMSE_NORMAL == cond) 
  {
    status = response.msg.NCreateRSP.DimseStatus;
    // if response contains SOP Instance UID, copy it.
    if (response.msg.NCreateRSP.opts & O_NCREATE_AFFECTEDSOPINSTANCEUID)
    {
      sopinstanceUID = response.msg.NCreateRSP.AffectedSOPInstanceUID;
    }
  }
  if (statusDetail) delete statusDetail;
  return cond;
}

CONDITION DVPSPrintMessageHandler::setRQ(
    const char *sopclassUID, 
    const char *sopinstanceUID, 
    DcmDataset *modificationList, 
    Uint16& status, 
    DcmDataset* &attributeListOut)
{
  if (assoc == NULL)
  {
    return COND_PushCondition(DIMSE_ILLEGALASSOCIATION, "DVPSPrintMessageHandler::setRQ: Cannot send message with NULL association");
  }
  if ((sopclassUID==NULL)||(sopinstanceUID==NULL)||(modificationList==NULL))
  {
    return COND_PushCondition(DIMSE_NULLKEY, "DVPSPrintMessageHandler::setRQ: NULL key");
  }

  T_ASC_PresentationContextID presCtx = findAcceptedPC(sopclassUID);
  if (presCtx == 0)
  {
    return COND_PushCondition(DIMSE_NOVALIDPRESENTATIONCONTEXTID, "DVPSPrintMessageHandler::setRQ: no valid presentation context");
  }  

  T_DIMSE_Message request;
  T_DIMSE_Message response;
  DcmDataset *statusDetail = NULL;

  // construct N-SET-RQ
  request.CommandField = DIMSE_N_SET_RQ;
  request.msg.NSetRQ.MessageID = assoc->nextMsgID++;
  strcpy(request.msg.NSetRQ.RequestedSOPClassUID, sopclassUID);
  strcpy(request.msg.NSetRQ.RequestedSOPInstanceUID, sopinstanceUID);
   
  CONDITION cond = sendNRequest(presCtx, request, modificationList, response, statusDetail, attributeListOut);
  if (DIMSE_NORMAL == cond) status = response.msg.NSetRSP.DimseStatus;
  if (statusDetail) delete statusDetail;
  return cond;   
}

CONDITION DVPSPrintMessageHandler::getRQ(
    const char *sopclassUID, 
    const char *sopinstanceUID, 
    const Uint16 *attributeIdentifierList,
    size_t numShorts,
    Uint16& status, 
    DcmDataset* &attributeListOut)
{
  if (assoc == NULL)
  {
    return COND_PushCondition(DIMSE_ILLEGALASSOCIATION, "DVPSPrintMessageHandler::getRQ: Cannot send message with NULL association");
  }
  if ((sopclassUID==NULL)||(sopinstanceUID==NULL))
  {
    return COND_PushCondition(DIMSE_NULLKEY, "DVPSPrintMessageHandler::getRQ: NULL key");
  }

  T_ASC_PresentationContextID presCtx = findAcceptedPC(sopclassUID);
  if (presCtx == 0)
  {
    return COND_PushCondition(DIMSE_NOVALIDPRESENTATIONCONTEXTID, "DVPSPrintMessageHandler::getRQ: no valid presentation context");
  }  

  T_DIMSE_Message request;
  T_DIMSE_Message response;
  DcmDataset *statusDetail = NULL;

  // construct N-GET-RQ
  request.CommandField = DIMSE_N_GET_RQ;
  request.msg.NGetRQ.MessageID = assoc->nextMsgID++;
  strcpy(request.msg.NGetRQ.RequestedSOPClassUID, sopclassUID);
  strcpy(request.msg.NGetRQ.RequestedSOPInstanceUID, sopinstanceUID);
  request.msg.NGetRQ.ListCount = 0;
  if (attributeIdentifierList) request.msg.NGetRQ.ListCount = (int)numShorts;
  request.msg.NGetRQ.AttributeIdentifierList = (DIC_US *)attributeIdentifierList;
   
  CONDITION cond = sendNRequest(presCtx, request, NULL, response, statusDetail, attributeListOut);
  if (DIMSE_NORMAL == cond) status = response.msg.NGetRSP.DimseStatus;
  if (statusDetail) delete statusDetail;
  return cond;   
}


CONDITION DVPSPrintMessageHandler::actionRQ(
    const char *sopclassUID, 
    const char *sopinstanceUID, 
    Uint16 actionTypeID, 
    DcmDataset *actionInformation,
    Uint16& status, 
    DcmDataset* &actionReply)
{
  if (assoc == NULL)
  {
    return COND_PushCondition(DIMSE_ILLEGALASSOCIATION, "DVPSPrintMessageHandler::actionRQ: Cannot send message with NULL association");
  }
  if ((sopclassUID==NULL)||(sopinstanceUID==NULL))
  {
    return COND_PushCondition(DIMSE_NULLKEY, "DVPSPrintMessageHandler::actionRQ: NULL key");
  }

  T_ASC_PresentationContextID presCtx = findAcceptedPC(sopclassUID);
  if (presCtx == 0)
  {
    return COND_PushCondition(DIMSE_NOVALIDPRESENTATIONCONTEXTID, "DVPSPrintMessageHandler::actionRQ: no valid presentation context");
  }  

  T_DIMSE_Message request;
  T_DIMSE_Message response;
  DcmDataset *statusDetail = NULL;

  // construct N-ACTION-RQ
  request.CommandField = DIMSE_N_ACTION_RQ;
  request.msg.NActionRQ.MessageID = assoc->nextMsgID++;
  strcpy(request.msg.NActionRQ.RequestedSOPClassUID, sopclassUID);
  strcpy(request.msg.NActionRQ.RequestedSOPInstanceUID, sopinstanceUID);
  request.msg.NActionRQ.ActionTypeID = (DIC_US)actionTypeID;
   
  CONDITION cond = sendNRequest(presCtx, request, actionInformation, response, statusDetail, actionReply);
  if (DIMSE_NORMAL == cond) status = response.msg.NActionRSP.DimseStatus;
  if (statusDetail) delete statusDetail;
  return cond;   
}

CONDITION DVPSPrintMessageHandler::deleteRQ(
    const char *sopclassUID, 
    const char *sopinstanceUID, 
    Uint16& status)
{
  if (assoc == NULL)
  {
    return COND_PushCondition(DIMSE_ILLEGALASSOCIATION, "DVPSPrintMessageHandler::deleteRQ: Cannot send message with NULL association");
  }
  if ((sopclassUID==NULL)||(sopinstanceUID==NULL))
  {
    return COND_PushCondition(DIMSE_NULLKEY, "DVPSPrintMessageHandler::deleteRQ: NULL key");
  }

  T_ASC_PresentationContextID presCtx = findAcceptedPC(sopclassUID);
  if (presCtx == 0)
  {
    return COND_PushCondition(DIMSE_NOVALIDPRESENTATIONCONTEXTID, "DVPSPrintMessageHandler::deleteRQ: no valid presentation context");
  }  

  T_DIMSE_Message request;
  T_DIMSE_Message response;
  DcmDataset *statusDetail = NULL;
  DcmDataset *attributeListOut = NULL;

  // construct N-DELETE-RQ
  request.CommandField = DIMSE_N_DELETE_RQ;
  request.msg.NDeleteRQ.MessageID = assoc->nextMsgID++;
  strcpy(request.msg.NDeleteRQ.RequestedSOPClassUID, sopclassUID);
  strcpy(request.msg.NDeleteRQ.RequestedSOPInstanceUID, sopinstanceUID);
   
  CONDITION cond = sendNRequest(presCtx, request, NULL, response, statusDetail, attributeListOut);
  if (DIMSE_NORMAL == cond) status = response.msg.NDeleteRSP.DimseStatus;
  if (statusDetail) delete statusDetail;
  if (attributeListOut) delete attributeListOut;  // should never happen
  return cond;   
}

CONDITION DVPSPrintMessageHandler::releaseAssociation()
{
  CONDITION result = ASC_NORMAL;
  if (assoc)
  {
    result = ASC_releaseAssociation(assoc);
    ASC_destroyAssociation(&assoc);
    ASC_dropNetwork(&net);
    assoc = NULL;
    net = NULL;
  }
  return result;
}

CONDITION DVPSPrintMessageHandler::abortAssociation()
{
  CONDITION result = ASC_NORMAL;
  if (assoc)
  {
    result = ASC_abortAssociation(assoc);
    ASC_destroyAssociation(&assoc);
    ASC_dropNetwork(&net);
    assoc = NULL;
    net = NULL;
  }
  return result;
}

T_ASC_PresentationContextID DVPSPrintMessageHandler::findAcceptedPC(const char *sopclassuid)
{
  if ((assoc==NULL)||(sopclassuid==NULL)) return 0;

  // if the SOP class is one of the Basic Grayscale Print Management Meta SOP Classes,
  // look for a presentation context for Basic Grayscale Print.
  OFString sopclass(sopclassuid);
  if ((sopclass == UID_BasicFilmSessionSOPClass) ||
      (sopclass == UID_BasicFilmBoxSOPClass) ||
      (sopclass == UID_BasicGrayscaleImageBoxSOPClass) ||
      (sopclass == UID_PrinterSOPClass)) sopclassuid = UID_BasicGrayscalePrintManagementMetaSOPClass;
  return ASC_findAcceptedPresentationContextID(assoc, sopclassuid);
}


CONDITION DVPSPrintMessageHandler::negotiateAssociation(
  const char *myAEtitle,
  const char *peerAEtitle,
  const char *peerHost,
  int peerPort,
  long peerMaxPDU,
  OFBool negotiatePresentationLUT,
  OFBool negotiateAnnotationBox,
  OFBool implicitOnly,
  OFBool verbose)
{
  if (assoc)
  {
    return COND_PushCondition(DIMSE_NETWORKERROR, "DVPSPrintMessageHandler::negotiateAssociation: association alread in place");
  }
  if ((myAEtitle==NULL)||(peerAEtitle==NULL)||(peerHost==NULL))
  {
    return COND_PushCondition(DIMSE_NULLKEY, "DVPSPrintMessageHandler::negotiateAssociation: NULL key");
  }
  
  T_ASC_Parameters *params=NULL;
  DIC_NODENAME dnlocalHost;
  DIC_NODENAME dnpeerHost;

  CONDITION cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 1000, &net);
  if (!SUCCESS(cond)) 
  {
		return cond;
		}
		
	cond = ASC_createAssociationParameters(&params, peerMaxPDU);

  ASC_setAPTitles(params, myAEtitle, peerAEtitle, NULL);
  gethostname(dnlocalHost, sizeof(dnlocalHost) - 1);
  sprintf(dnpeerHost, "%s:%d", peerHost, peerPort);
  ASC_setPresentationAddresses(params, dnlocalHost, dnpeerHost);

  /* presentation contexts */
  const char* transferSyntaxes[3];
  int transferSyntaxCount = 0;

  if (implicitOnly)
  {
      transferSyntaxes[0] = UID_LittleEndianImplicitTransferSyntax;
      transferSyntaxCount = 1;
  } else {
      /* gLocalByteOrder is defined in dcxfer.h */
      if (gLocalByteOrder == EBO_LittleEndian) {
          /* we are on a little endian machine */
          transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
          transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
      } else {
          /* we are on a big endian machine */
          transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
          transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      }
      transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
      transferSyntaxCount = 3;
  }

  /* we always propose basic grayscale, presentation LUT and annotation box*/
  if (SUCCESS(cond)) cond = ASC_addPresentationContext(params, 1, UID_BasicGrayscalePrintManagementMetaSOPClass, transferSyntaxes, transferSyntaxCount);
  if (negotiatePresentationLUT)
  {
    if (SUCCESS(cond)) cond = ASC_addPresentationContext(params, 3, UID_PresentationLUTSOPClass, transferSyntaxes, transferSyntaxCount);
  }

  if (negotiateAnnotationBox)
  {
    if (SUCCESS(cond)) cond = ASC_addPresentationContext(params, 5, UID_BasicAnnotationBoxSOPClass, transferSyntaxes, transferSyntaxCount);
  }
  
  /* create association */
  if (verbose) *logstream << "Requesting Association" << endl;
    
  if (SUCCESS(cond)) 
  {
    cond = ASC_requestAssociation(net, params, &assoc);

    if (cond == ASC_ASSOCIATIONREJECTED)
    {
      T_ASC_RejectParameters rej;
      ASC_getRejectParameters(params, &rej);
      if (verbose)
      {
        *logstream << "Association Rejected" << endl;
        ASC_printRejectParameters(stderr, &rej);
      }
    }else{
			if (!SUCCESS(cond)) 
			{
				if (params) ASC_destroyAssociationParameters(&params);
				if (net) ASC_dropNetwork(&net);
				assoc = NULL;
				net = NULL;
				return cond;
			}
		}
  }

  if ((SUCCESS(cond)) && (0 == ASC_findAcceptedPresentationContextID(assoc, UID_BasicGrayscalePrintManagementMetaSOPClass)))
  {
    cond = COND_PushCondition(DIMSE_NOVALIDPRESENTATIONCONTEXTID, "DVPSPrintMessageHandler::negotiateAssociation: Peer does not support Basic Grayscale Print Management");
    if (verbose) *logstream << "Peer does not support Basic Grayscale Print Management, aborting association." << endl;
    abortAssociation();
  }
  
  if (SUCCESS(cond))
  {
    if (verbose) *logstream << "Association accepted (Max Send PDV: " << assoc->sendPDVLength << ")" << endl;
  } else {
    if (params) ASC_destroyAssociationParameters(&params);
    if (assoc) ASC_destroyAssociation(&assoc);
    if (net) ASC_dropNetwork(&net);
    assoc = NULL;
    net = NULL;
  }
  return cond;
}

OFBool DVPSPrintMessageHandler::printerSupportsPresentationLUT()
{
  if ((assoc)&&(0 != ASC_findAcceptedPresentationContextID(assoc, UID_PresentationLUTSOPClass))) return OFTrue;
  return OFFalse;
}

OFBool DVPSPrintMessageHandler::printerSupportsAnnotationBox()
{
  if ((assoc)&&(0 != ASC_findAcceptedPresentationContextID(assoc, UID_BasicAnnotationBoxSOPClass))) return OFTrue;
  return OFFalse;
}

/*
 *  $Log: dvpspr.cc,v $
 *  Revision 1.7  1999-10-28 08:18:57  meichel
 *  Print client does not attempt any more to negotiate Presentation LUT or
 *    Annotation Box if config file says that the printer does not support them.
 *
 *  Revision 1.6  1999/10/19 14:48:24  meichel
 *  added support for the Basic Annotation Box SOP Class
 *    as well as access methods for Max Density and Min Density.
 *
 *  Revision 1.5  1999/10/13 14:10:49  meichel
 *  Now negotiation Basic Annotation Box SOP Class
 *
 *  Revision 1.4  1999/09/24 15:24:08  meichel
 *  Print spooler (dcmprtsv) now logs diagnostic messages in log files
 *    when operating in spool mode.
 *
 *  Revision 1.3  1999/09/17 14:33:53  meichel
 *  Completed print spool functionality including Supplement 22 support
 *
 *  Revision 1.2  1999/08/26 09:29:49  thiel
 *  Extensions for the usage of the StoredPrint
 *
 *  Revision 1.1  1999/07/30 13:34:58  meichel
 *  Added new classes managing Stored Print objects
 *
 *
 */

