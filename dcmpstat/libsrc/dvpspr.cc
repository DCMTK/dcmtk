/*
 *
 *  Copyright (C) 1998-2024, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSPrintMessageHandler
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpspr.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmpstat/dvpsdef.h"

/* --------------- class DVPSPrintMessageHandler --------------- */

DVPSPrintMessageHandler::DVPSPrintMessageHandler()
: assoc(NULL)
, net(NULL)
, eventHandler(NULL)
, blockMode(DIMSE_BLOCKING)
, timeout(0)
{
}

DVPSPrintMessageHandler::~DVPSPrintMessageHandler()
{
  abortAssociation(); // won't do any harm if there is no association in place
}

void DVPSPrintMessageHandler::dumpNMessage(T_DIMSE_Message &msg, DcmItem *dataset, OFBool outgoing)
{
    OFString str;
    if (outgoing) {
        DIMSE_dumpMessage(str, msg, DIMSE_OUTGOING, dataset);
    } else {
        DIMSE_dumpMessage(str, msg, DIMSE_INCOMING, dataset);
    }
    DCMPSTAT_DUMP(str);
}

OFCondition DVPSPrintMessageHandler::sendNRequest(
    T_ASC_PresentationContextID presId,
    T_DIMSE_Message &request,
    DcmDataset *rqDataSet,
    T_DIMSE_Message &response,
    DcmDataset* &statusDetail,
    DcmDataset* &rspDataset)
{
    OFCondition cond = EC_Normal;
    T_DIMSE_Command expectedResponse;
    DIC_US expectedMessageID=0;
    if (assoc == NULL)
    {
      return DIMSE_ILLEGALASSOCIATION;
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
        return DIMSE_BADCOMMANDTYPE;
        /* break; */
    }

    dumpNMessage(request, rqDataSet, OFTrue);
    cond = DIMSE_sendMessageUsingMemoryData(assoc, presId, &request, NULL, rqDataSet, NULL, NULL);
    if (cond.bad()) return cond;

    T_ASC_PresentationContextID thisPresId;
    T_DIMSE_Message eventReportRsp;
    DIC_US eventReportStatus;
    do
    {
        thisPresId = presId;
        statusDetail = NULL;
        cond = DIMSE_receiveCommand(assoc, blockMode, timeout, &thisPresId, &response, &statusDetail);
        if (cond.bad()) return cond;

        if (response.CommandField == DIMSE_N_EVENT_REPORT_RQ)
        {
          /* handle N-EVENT-REPORT-RQ */
          rspDataset = NULL;
          if (response.msg.NEventReportRQ.DataSetType != DIMSE_DATASET_NULL)
          {
            cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout, &thisPresId, &rspDataset, NULL, NULL);
            if (cond.bad()) return cond;
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
          if (cond.bad()) return cond;
        } else {
          /* No N-EVENT-REPORT-RQ. Check if this message is what we expected */
          if (response.CommandField != expectedResponse)
          {
            char buf1[256];
            OFStandard::snprintf(buf1, sizeof(buf1), "DIMSE: Unexpected Response Command Field: 0x%x", (unsigned)response.CommandField);
            return makeDcmnetCondition(DIMSEC_UNEXPECTEDRESPONSE, OF_error, buf1);
          }
          T_DIMSE_DataSetType responseDataset = DIMSE_DATASET_NULL;
          DIC_US responseMessageID = 0;
          /* change request to response */
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
              {
                char buf1[256];
                OFStandard::snprintf(buf1, sizeof(buf1), "DIMSE: Unexpected Response Command Field: 0x%x", (unsigned)response.CommandField);
                return makeDcmnetCondition(DIMSEC_UNEXPECTEDRESPONSE, OF_error, buf1);
              }
              /* break; */
          }
          if (responseMessageID != expectedMessageID)
          {
            char buf1[256];
            OFStandard::snprintf(buf1, sizeof(buf1), "DIMSE: Unexpected Response Command Field: 0x%x", (unsigned)response.CommandField);
            return makeDcmnetCondition(DIMSEC_UNEXPECTEDRESPONSE, OF_error, buf1);
          }
          rspDataset = NULL;
          if (responseDataset != DIMSE_DATASET_NULL)
          {
            cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout, &thisPresId, &rspDataset, NULL, NULL);
            if (cond.bad()) return cond;
          }
          dumpNMessage(response, rspDataset, OFFalse);
        }
    } while (response.CommandField == DIMSE_N_EVENT_REPORT_RQ);
    return EC_Normal;
}

OFCondition DVPSPrintMessageHandler::createRQ(
    const char *sopclassUID,
    OFString& sopinstanceUID,
    DcmDataset *attributeListIn,
    Uint16& status,
    DcmDataset* &attributeListOut)
{
  if (assoc == NULL)
  {
    return DIMSE_ILLEGALASSOCIATION;
  }
  if (sopclassUID==NULL)
  {
    return DIMSE_NULLKEY;
  }

  T_ASC_PresentationContextID presCtx = findAcceptedPC(sopclassUID);
  if (presCtx == 0)
  {
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  }

  T_DIMSE_Message request;
  T_DIMSE_Message response;
  DcmDataset *statusDetail = NULL;

  // construct N-CREATE-RQ
  request.CommandField = DIMSE_N_CREATE_RQ;
  request.msg.NCreateRQ.MessageID = assoc->nextMsgID++;
  OFStandard::strlcpy(request.msg.NCreateRQ.AffectedSOPClassUID, sopclassUID, sizeof(request.msg.NCreateRQ.AffectedSOPClassUID));
  if (sopinstanceUID.size() > 0)
  {
    OFStandard::strlcpy(request.msg.NCreateRQ.AffectedSOPInstanceUID, sopinstanceUID.c_str(), sizeof(request.msg.NCreateRQ.AffectedSOPInstanceUID));
    request.msg.NCreateRQ.opts = O_NCREATE_AFFECTEDSOPINSTANCEUID;
  } else {
    request.msg.NCreateRQ.AffectedSOPInstanceUID[0] = 0;
    request.msg.NCreateRQ.opts = 0;
  }

  OFCondition cond = sendNRequest(presCtx, request, attributeListIn, response, statusDetail, attributeListOut);
  if (cond.good())
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

OFCondition DVPSPrintMessageHandler::setRQ(
    const char *sopclassUID,
    const char *sopinstanceUID,
    DcmDataset *modificationList,
    Uint16& status,
    DcmDataset* &attributeListOut)
{
  if (assoc == NULL)
  {
    return DIMSE_ILLEGALASSOCIATION;
  }
  if ((sopclassUID==NULL)||(sopinstanceUID==NULL)||(modificationList==NULL))
  {
    return DIMSE_NULLKEY;
  }

  T_ASC_PresentationContextID presCtx = findAcceptedPC(sopclassUID);
  if (presCtx == 0)
  {
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  }

  T_DIMSE_Message request;
  T_DIMSE_Message response;
  DcmDataset *statusDetail = NULL;

  // construct N-SET-RQ
  request.CommandField = DIMSE_N_SET_RQ;
  request.msg.NSetRQ.MessageID = assoc->nextMsgID++;
  OFStandard::strlcpy(request.msg.NSetRQ.RequestedSOPClassUID, sopclassUID, sizeof(request.msg.NSetRQ.RequestedSOPClassUID));
  OFStandard::strlcpy(request.msg.NSetRQ.RequestedSOPInstanceUID, sopinstanceUID, sizeof(request.msg.NSetRQ.RequestedSOPInstanceUID));

  OFCondition cond = sendNRequest(presCtx, request, modificationList, response, statusDetail, attributeListOut);
  if (cond.good()) status = response.msg.NSetRSP.DimseStatus;
  if (statusDetail) delete statusDetail;
  return cond;
}

OFCondition DVPSPrintMessageHandler::getRQ(
    const char *sopclassUID,
    const char *sopinstanceUID,
    const Uint16 *attributeIdentifierList,
    size_t numShorts,
    Uint16& status,
    DcmDataset* &attributeListOut)
{
  if (assoc == NULL)
  {
    return DIMSE_ILLEGALASSOCIATION;
  }
  if ((sopclassUID==NULL)||(sopinstanceUID==NULL))
  {
    return DIMSE_NULLKEY;
  }

  T_ASC_PresentationContextID presCtx = findAcceptedPC(sopclassUID);
  if (presCtx == 0)
  {
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  }

  T_DIMSE_Message request;
  T_DIMSE_Message response;
  DcmDataset *statusDetail = NULL;

  // construct N-GET-RQ
  request.CommandField = DIMSE_N_GET_RQ;
  request.msg.NGetRQ.MessageID = assoc->nextMsgID++;
  OFStandard::strlcpy(request.msg.NGetRQ.RequestedSOPClassUID, sopclassUID, sizeof(request.msg.NGetRQ.RequestedSOPClassUID));
  OFStandard::strlcpy(request.msg.NGetRQ.RequestedSOPInstanceUID, sopinstanceUID, sizeof(request.msg.NGetRQ.RequestedSOPInstanceUID));
  request.msg.NGetRQ.ListCount = 0;
  if (attributeIdentifierList) request.msg.NGetRQ.ListCount = (int)numShorts;
  request.msg.NGetRQ.AttributeIdentifierList = (DIC_US *)attributeIdentifierList;

  OFCondition cond = sendNRequest(presCtx, request, NULL, response, statusDetail, attributeListOut);
  if (cond.good()) status = response.msg.NGetRSP.DimseStatus;
  if (statusDetail) delete statusDetail;
  return cond;
}


OFCondition DVPSPrintMessageHandler::actionRQ(
    const char *sopclassUID,
    const char *sopinstanceUID,
    Uint16 actionTypeID,
    DcmDataset *actionInformation,
    Uint16& status,
    DcmDataset* &actionReply)
{
  if (assoc == NULL)
  {
    return DIMSE_ILLEGALASSOCIATION;
  }
  if ((sopclassUID==NULL)||(sopinstanceUID==NULL))
  {
    return DIMSE_NULLKEY;
  }

  T_ASC_PresentationContextID presCtx = findAcceptedPC(sopclassUID);
  if (presCtx == 0)
  {
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  }

  T_DIMSE_Message request;
  T_DIMSE_Message response;
  DcmDataset *statusDetail = NULL;

  // construct N-ACTION-RQ
  request.CommandField = DIMSE_N_ACTION_RQ;
  request.msg.NActionRQ.MessageID = assoc->nextMsgID++;
  OFStandard::strlcpy(request.msg.NActionRQ.RequestedSOPClassUID, sopclassUID, sizeof(request.msg.NActionRQ.RequestedSOPClassUID));
  OFStandard::strlcpy(request.msg.NActionRQ.RequestedSOPInstanceUID, sopinstanceUID, sizeof(request.msg.NActionRQ.RequestedSOPInstanceUID));
  request.msg.NActionRQ.ActionTypeID = (DIC_US)actionTypeID;

  OFCondition cond = sendNRequest(presCtx, request, actionInformation, response, statusDetail, actionReply);
  if (cond.good()) status = response.msg.NActionRSP.DimseStatus;
  if (statusDetail) delete statusDetail;
  return cond;
}

OFCondition DVPSPrintMessageHandler::deleteRQ(
    const char *sopclassUID,
    const char *sopinstanceUID,
    Uint16& status)
{
  if (assoc == NULL)
  {
    return DIMSE_ILLEGALASSOCIATION;
  }
  if ((sopclassUID==NULL)||(sopinstanceUID==NULL))
  {
    return DIMSE_NULLKEY;
  }

  T_ASC_PresentationContextID presCtx = findAcceptedPC(sopclassUID);
  if (presCtx == 0)
  {
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  }

  T_DIMSE_Message request;
  T_DIMSE_Message response;
  DcmDataset *statusDetail = NULL;
  DcmDataset *attributeListOut = NULL;

  // construct N-DELETE-RQ
  request.CommandField = DIMSE_N_DELETE_RQ;
  request.msg.NDeleteRQ.MessageID = assoc->nextMsgID++;
  OFStandard::strlcpy(request.msg.NDeleteRQ.RequestedSOPClassUID, sopclassUID, sizeof(request.msg.NDeleteRQ.RequestedSOPClassUID));
  OFStandard::strlcpy(request.msg.NDeleteRQ.RequestedSOPInstanceUID, sopinstanceUID, sizeof(request.msg.NDeleteRQ.RequestedSOPInstanceUID));

  OFCondition cond = sendNRequest(presCtx, request, NULL, response, statusDetail, attributeListOut);
  if (cond.good()) status = response.msg.NDeleteRSP.DimseStatus;
  if (statusDetail) delete statusDetail;
  if (attributeListOut) delete attributeListOut;  // should never happen
  return cond;
}

OFCondition DVPSPrintMessageHandler::releaseAssociation()
{
  OFCondition result = EC_Normal;
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

OFCondition DVPSPrintMessageHandler::abortAssociation()
{
  OFCondition result = EC_Normal;
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


OFCondition DVPSPrintMessageHandler::negotiateAssociation(
  DcmTransportLayer *tlayer,
  const char *myAEtitle,
  const char *peerAEtitle,
  const char *peerHost,
  int peerPort,
  long peerMaxPDU,
  OFBool negotiatePresentationLUT,
  OFBool negotiateAnnotationBox,
  OFBool implicitOnly)
{
  if (assoc)
  {
    return makeDcmnetCondition(DIMSEC_ILLEGALASSOCIATION, OF_error, "association already in place");
  }
  if ((myAEtitle==NULL)||(peerAEtitle==NULL)||(peerHost==NULL))
  {
    return DIMSE_NULLKEY;
  }

  T_ASC_Parameters *params=NULL;
  DIC_NODENAME dnpeerHost;

  OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 30, &net);
  if (cond.good()) cond = ASC_createAssociationParameters(&params, peerMaxPDU, dcmConnectionTimeout.get());

  if (tlayer && cond.good())
  {
    cond = ASC_setTransportLayer(net, tlayer, 0);
    if (cond.good()) cond = ASC_setTransportLayerType(params, OFTrue /* use TLS */);
  }

  if (cond.bad()) return cond;

  ASC_setAPTitles(params, myAEtitle, peerAEtitle, NULL);
  OFStandard::snprintf(dnpeerHost, sizeof(dnpeerHost), "%s:%d", peerHost, peerPort);
  ASC_setPresentationAddresses(params, OFStandard::getHostName().c_str(), dnpeerHost);

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
  if (cond.good()) cond = ASC_addPresentationContext(params, 1, UID_BasicGrayscalePrintManagementMetaSOPClass, transferSyntaxes, transferSyntaxCount);
  if (negotiatePresentationLUT)
  {
    if (cond.good()) cond = ASC_addPresentationContext(params, 3, UID_PresentationLUTSOPClass, transferSyntaxes, transferSyntaxCount);
  }

  if (negotiateAnnotationBox)
  {
    if (cond.good()) cond = ASC_addPresentationContext(params, 5, UID_BasicAnnotationBoxSOPClass, transferSyntaxes, transferSyntaxCount);
  }

  /* create association */
  DCMPSTAT_INFO("Requesting Association");

  if (cond.good())
  {
    cond = ASC_requestAssociation(net, params, &assoc);

    if (cond == DUL_ASSOCIATIONREJECTED)
    {
      OFString temp_str;
      T_ASC_RejectParameters rej;
      ASC_getRejectParameters(params, &rej);
      DCMPSTAT_WARN("Association Rejected" << OFendl << ASC_printRejectParameters(temp_str, &rej));
    } else {
      if (cond.bad())
      {
        // if assoc is non-NULL, then params has already been moved into the
        // assoc structure. Make sure we only delete once!
        if (assoc) ASC_destroyAssociation(&assoc);
        else if (params) ASC_destroyAssociationParameters(&params);

        if (net) ASC_dropNetwork(&net);
        assoc = NULL;
        net = NULL;
        return cond;
      }
    }
  }

  if ((cond.good()) && (0 == ASC_findAcceptedPresentationContextID(assoc, UID_BasicGrayscalePrintManagementMetaSOPClass)))
  {
    DCMPSTAT_WARN("Peer does not support Basic Grayscale Print Management, aborting association.");
    abortAssociation();
    cond = DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  }

  if (cond.good())
  {
    DCMPSTAT_INFO("Association accepted (Max Send PDV: " << assoc->sendPDVLength << ")");
  } else {
    // params is now an alias to assoc->params. Don't call ASC_destroyAssociationParameters.
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
