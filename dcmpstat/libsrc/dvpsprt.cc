/*
 *
 *  Copyright (C) 1998-2000, OFFIS
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
 *    classes: DVPSPrintSCP
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-05-31 12:58:12 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsprt.h"
#include "dvpsdef.h"     /* for constants */
#include "dviface.h"
#include "dvpsfs.h"
#include "dvpssp.h"
#include "dvpshlp.h"

DVPSPrintSCP::DVPSPrintSCP(DVInterface &iface, const char *cfname)
: dviface(iface)
, cfgname(cfname)
, blockMode(DIMSE_BLOCKING)
, timeout(0)
, filmSession(NULL)
, presentationLUTList()
, storedPrintList()
, assoc(NULL)
, studyInstanceUID(DCM_StudyInstanceUID)
, psSeriesInstanceUID(DCM_SeriesInstanceUID)
, imageSeriesInstanceUID(DCM_SeriesInstanceUID)
{
}

DVPSPrintSCP::~DVPSPrintSCP()
{
  dropAssociation();
}

int DVPSPrintSCP::errorCond(CONDITION cond, const char *message)
{
  int result = (!SUCCESS(cond));
  if (result) 
  {  
    CERR << message << endl;
    COND_DumpConditions(); 
  }
  return result;
}


DVPSAssociationNegotiationResult DVPSPrintSCP::negotiateAssociation(
  T_ASC_Network &net,
  OFBool opt_verbose)
{  
  DVPSAssociationNegotiationResult result = DVPSJ_success;
  char buf[BUFSIZ];    
  OFBool dropAssoc = OFFalse;

  // get AETITLE from config file
  const char *aetitle = dviface.getTargetAETitle(cfgname);
  if (aetitle==NULL) aetitle = dviface.getNetworkAETitle(); // default if AETITLE is missing
  
  // get MaxPDU from config file
  unsigned long maxPDU = dviface.getTargetMaxPDU(cfgname);  
  if (maxPDU == 0) maxPDU = DEFAULT_MAXPDU;
  else if (maxPDU > ASC_MAXIMUMPDUSIZE)
  {
      CERR << "warning: max PDU size " << maxPDU << " too big, using default: " << DEFAULT_MAXPDU << endl;
      maxPDU = DEFAULT_MAXPDU;
  }
  else if (maxPDU < ASC_MINIMUMPDUSIZE)
  {
      CERR << "warning: max PDU size " << maxPDU << " too small, using default: " << DEFAULT_MAXPDU << endl;
      maxPDU = DEFAULT_MAXPDU;
  }
  
  // check whether we want to support Presentation LUT
  OFBool supportPresentationLUT = dviface.getTargetPrinterSupportsPresentationLUT(cfgname);  

  // check whether we want to support Implicit VR only 
  OFBool implicitOnly = dviface.getTargetImplicitOnly(cfgname);  
  
  CONDITION cond = ASC_receiveAssociation(&net, &assoc, maxPDU);
  if (errorCond(cond, "Failed to receive association:")) 
  {
    dropAssoc = OFTrue;
    result = DVPSJ_error;
  }
  else 
  {
    if (opt_verbose)
    {
      time_t t = time(NULL);
      CERR << "Association Received (" << assoc->params->DULparams.callingPresentationAddress
           << ":" << assoc->params->DULparams.callingAPTitle << " -> " 
           << assoc->params->DULparams.calledAPTitle
           << ") " << ctime(&t);
    }

    ASC_setAPTitles(assoc->params, NULL, NULL, aetitle);
    
    /* Application Context Name */
    if ((cond = ASC_getApplicationContextName(assoc->params, buf) != ASC_NORMAL) || strcmp(buf, DICOM_STDAPPLICATIONCONTEXT) != 0)
    {
        /* reject: the application context name is not supported */
        if (opt_verbose) CERR << "Bad AppContextName: " << buf << endl;
        cond = refuseAssociation(OFTrue);
        dropAssoc = OFTrue;
        result = DVPSJ_error;
    } else {   
      const char *abstractSyntaxes[] =
      {
        UID_BasicGrayscalePrintManagementMetaSOPClass,
        UID_PresentationLUTSOPClass,
        UID_PrivateShutdownSOPClass
      };

      int numAbstractSyntaxes = 2;
      if (supportPresentationLUT) numAbstractSyntaxes = 3;

      const char* transferSyntaxes[] = { NULL, NULL, NULL };
      int numTransferSyntaxes = 0;
      if (implicitOnly)
      {
        transferSyntaxes[0] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 1;
      } else {
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;

        if (gLocalByteOrder == EBO_LittleEndian) {
          /* we are on a little endian machine */
          transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
          transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        } else {
          /* we are on a big endian machine */
          transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
          transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        }
      }

      /* accept presentation contexts */
      cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
        assoc->params, 
        (const char**)abstractSyntaxes, numAbstractSyntaxes,
        (const char**)transferSyntaxes, numTransferSyntaxes);
      errorCond(cond, "Cannot accept presentation contexts:");

    }

    /* check if we have negotiated the private "shutdown" SOP Class */      
    if (0 != ASC_findAcceptedPresentationContextID(assoc, UID_PrivateShutdownSOPClass)) 
    {
      cond = refuseAssociation(OFFalse);
      dropAssoc = OFTrue;
      result = DVPSJ_terminate;
    }
  } /* receiveAssociation successful */
  if (dropAssoc) dropAssociation();
  return result;
}


CONDITION DVPSPrintSCP::refuseAssociation(OFBool isBadContext)
{
    T_ASC_RejectParameters rej;

    if (isBadContext)
    {
        rej.result = ASC_RESULT_REJECTEDTRANSIENT;
        rej.source = ASC_SOURCE_SERVICEUSER;
        rej.reason = ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED;
    } else {
        rej.result = ASC_RESULT_REJECTEDPERMANENT;
        rej.source = ASC_SOURCE_SERVICEUSER;
        rej.reason = ASC_REASON_SU_NOREASON;
    }

    CONDITION cond = ASC_rejectAssociation(assoc, &rej);
    errorCond(cond, "Association Reject Failed:");
    return cond;
}


void DVPSPrintSCP::dropAssociation()
{
  if (assoc == NULL) return;
  CONDITION cond = ASC_dropSCPAssociation(assoc);
  errorCond(cond, "Cannot Drop Association:");
  cond = ASC_destroyAssociation(&assoc);
  errorCond(cond, "Cannot Destroy Association:");
  assoc = NULL;
  COND_PopCondition(OFTrue);
  return;
}


void DVPSPrintSCP::handleClient(OFBool opt_verbose)
{
  CONDITION cond = ASC_acknowledgeAssociation(assoc);
  if (! errorCond(cond, "Cannot acknowledge association:"))
  {
    if (opt_verbose)
    {
      CERR << "Association Acknowledged (Max Send PDV: " << assoc->sendPDVLength << ")" << endl;
      if (ASC_countAcceptedPresentationContexts(assoc->params) == 0) CERR << "    (but no valid presentation contexts)" << endl;
    }

    T_DIMSE_Message msg;
    T_ASC_PresentationContextID presID;
    cond = DIMSE_NORMAL;
    
    /* do real work */
    while (cond == DIMSE_NORMAL)
    {
      cond = DIMSE_receiveCommand(assoc, DIMSE_BLOCKING, 0, &presID, &msg, NULL);
      /* did peer release, abort, or do we have a valid message ? */      
      switch (cond)
      {
        case DIMSE_NORMAL:
          /* process command */
          switch (msg.CommandField)
          {
            case DIMSE_N_GET_RQ:
              cond = handleNGet(msg, presID);
              break;
            case DIMSE_N_SET_RQ:
              cond = handleNSet(msg, presID);
              break;
            case DIMSE_N_ACTION_RQ:
              cond = handleNAction(msg, presID);
              break;
            case DIMSE_N_CREATE_RQ:
              cond = handleNCreate(msg, presID);
              break;
            case DIMSE_N_DELETE_RQ:
              cond = handleNDelete(msg, presID);
              break;
            default:
              cond = DIMSE_BADCOMMANDTYPE; /* unsupported command */
              CERR << "Cannot handle command: 0x" << hex << (unsigned)msg.CommandField << dec << endl;
              break;
          }
          break;
        case DIMSE_PEERREQUESTEDRELEASE:
        case DIMSE_PEERABORTEDASSOCIATION:
        default:
          /* finish processing loop */
          break;
      }
    } /* while */

    /* close association */
    if (cond == DIMSE_PEERREQUESTEDRELEASE)
    {
      COND_PopCondition(OFFalse);
      if (opt_verbose) CERR << "Association Release" << endl;
      cond = ASC_acknowledgeRelease(assoc);
      errorCond(cond, "Cannot release association:");
    } 
    else if (cond == DIMSE_PEERABORTEDASSOCIATION)
    {
      COND_PopCondition(OFFalse);       /* pop DIMSE abort */
      COND_PopCondition(OFFalse);       /* pop DUL abort */
      if (opt_verbose) CERR << "Association Aborted" << endl;
    } 
    else
    {
      errorCond(cond, "DIMSE Failure (aborting association):");
      cond = ASC_abortAssociation(assoc);
      errorCond(cond, "Cannot abort association:");
    }
  }
  dropAssociation();
}


CONDITION DVPSPrintSCP::handleNGet(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID)
{
  // initialize response message
  T_DIMSE_Message rsp;
  rsp.CommandField = DIMSE_N_GET_RSP;
  rsp.msg.NGetRSP.MessageIDBeingRespondedTo = rq.msg.NGetRQ.MessageID;
  rsp.msg.NGetRSP.AffectedSOPClassUID[0] = 0;
  rsp.msg.NGetRSP.DimseStatus = STATUS_Success;
  rsp.msg.NGetRSP.AffectedSOPInstanceUID[0] = 0;
  rsp.msg.NGetRSP.DataSetType = DIMSE_DATASET_NULL;
  rsp.msg.NGetRSP.opts = 0;

  CONDITION cond = DIMSE_NORMAL;
  DcmDataset *rspDataset = NULL;
  
  if (rq.msg.NGetRQ.DataSetType == DIMSE_DATASET_PRESENT)
  {
     // should not happen
     DcmDataset *dataset = NULL;
     cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout, &presID, &dataset, NULL, NULL);
     delete dataset;
     if (cond != DIMSE_NORMAL) return cond;
  }  

  OFString sopClass(rq.msg.NGetRQ.RequestedSOPClassUID);
  if (sopClass == UID_PrinterSOPClass)
  {
    // Print N-GET
    printerNGet(rq, rsp, rspDataset);
  } else {
    CERR << "error: N-GET unsupported for SOP class '" << sopClass.c_str() << "'" << endl;
    rsp.msg.NGetRSP.DimseStatus = 0x0118; // no such SOP Class    
  }
  cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &rsp, NULL, rspDataset, NULL, NULL);
  delete rspDataset;
  return cond;
}


CONDITION DVPSPrintSCP::handleNSet(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID)
{
  // initialize response message
  T_DIMSE_Message rsp;
  rsp.CommandField = DIMSE_N_SET_RSP;
  rsp.msg.NSetRSP.MessageIDBeingRespondedTo = rq.msg.NSetRQ.MessageID;
  rsp.msg.NSetRSP.AffectedSOPClassUID[0] = 0;
  rsp.msg.NSetRSP.DimseStatus = STATUS_Success;
  rsp.msg.NSetRSP.AffectedSOPInstanceUID[0] = 0;
  rsp.msg.NSetRSP.DataSetType = DIMSE_DATASET_NULL;
  rsp.msg.NSetRSP.opts = 0;

  CONDITION cond = DIMSE_NORMAL;
  DcmDataset *rqDataset = NULL;
  DcmDataset *rspDataset = NULL;
  
  if (rq.msg.NSetRQ.DataSetType == DIMSE_DATASET_PRESENT)
  {
     cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout, &presID, &rqDataset, NULL, NULL);
     if (cond != DIMSE_NORMAL) return cond;
  }  

  OFString sopClass(rq.msg.NSetRQ.RequestedSOPClassUID);
  if (sopClass == UID_BasicFilmSessionSOPClass)
  {
    // BFS N-SET
    filmSessionNSet(rq, rqDataset, rsp, rspDataset);
  } else if (sopClass == UID_BasicFilmBoxSOPClass)
  {
    // BFB N-SET
    filmBoxNSet(rq, rqDataset, rsp, rspDataset);
  } else if (sopClass == UID_BasicGrayscaleImageBoxSOPClass)
  {
    // BGIB N-SET
    imageBoxNSet(rq, rqDataset, rsp, rspDataset);
  } else {
    CERR << "error: N-SET unsupported for SOP class '" << sopClass.c_str() << "'" << endl;
    rsp.msg.NSetRSP.DimseStatus = 0x0118; // no such SOP Class    
  }
  cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &rsp, NULL, rspDataset, NULL, NULL);
  delete rqDataset;
  delete rspDataset;
  return cond;
}


CONDITION DVPSPrintSCP::handleNAction(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID)
{
  // initialize response message
  T_DIMSE_Message rsp;
  rsp.CommandField = DIMSE_N_ACTION_RSP;
  rsp.msg.NActionRSP.MessageIDBeingRespondedTo = rq.msg.NActionRQ.MessageID;
  rsp.msg.NActionRSP.AffectedSOPClassUID[0] = 0;
  rsp.msg.NActionRSP.DimseStatus = STATUS_Success;
  rsp.msg.NActionRSP.AffectedSOPInstanceUID[0] = 0;
  rsp.msg.NActionRSP.ActionTypeID = rq.msg.NActionRQ.ActionTypeID;
  rsp.msg.NActionRSP.DataSetType = DIMSE_DATASET_NULL;
  rsp.msg.NActionRSP.opts = O_NACTION_ACTIONTYPEID;

  CONDITION cond = DIMSE_NORMAL;
  DcmDataset *rqDataset = NULL;
  
  if (rq.msg.NActionRQ.DataSetType == DIMSE_DATASET_PRESENT)
  {
     cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout, &presID, &rqDataset, NULL, NULL);
     if (cond != DIMSE_NORMAL) return cond;
  }  

  OFString sopClass(rq.msg.NActionRQ.RequestedSOPClassUID);
  if (sopClass == UID_BasicFilmSessionSOPClass)
  {
    // BFS N-ACTION
    filmSessionNAction(rq, rsp);
  } else if (sopClass == UID_BasicFilmBoxSOPClass)
  {
    // BFB N-ACTION
    filmBoxNAction(rq, rsp);
  } else {
    CERR << "error: N-ACTION unsupported for SOP class '" << sopClass.c_str() << "'" << endl;
    rsp.msg.NActionRSP.DimseStatus = 0x0118; // no such SOP Class    
  }
  cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &rsp, NULL, NULL, NULL, NULL);
  delete rqDataset;
  return cond;
}


CONDITION DVPSPrintSCP::handleNCreate(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID)
{
  // initialize response message
  T_DIMSE_Message rsp;
  rsp.CommandField = DIMSE_N_CREATE_RSP;
  rsp.msg.NCreateRSP.MessageIDBeingRespondedTo = rq.msg.NCreateRQ.MessageID;
  rsp.msg.NCreateRSP.AffectedSOPClassUID[0] = 0;
  rsp.msg.NCreateRSP.DimseStatus = STATUS_Success;
  if (rq.msg.NCreateRQ.opts & O_NCREATE_AFFECTEDSOPINSTANCEUID)
  {
    // instance UID is provided by SCU
    strncpy(rsp.msg.NCreateRSP.AffectedSOPInstanceUID, rsp.msg.NCreateRQ.AffectedSOPInstanceUID, sizeof(DIC_UI));
  } else {
    // we generate our own instance UID
    dcmGenerateUniqueIdentifer(rsp.msg.NCreateRSP.AffectedSOPInstanceUID);
  }	
  rsp.msg.NCreateRSP.DataSetType = DIMSE_DATASET_NULL;
  rsp.msg.NCreateRSP.opts = O_NCREATE_AFFECTEDSOPINSTANCEUID;

  CONDITION cond = DIMSE_NORMAL;
  DcmDataset *rqDataset = NULL;
  DcmDataset *rspDataset = NULL;
  
  if (rq.msg.NCreateRQ.DataSetType == DIMSE_DATASET_PRESENT)
  {
     cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout, &presID, &rqDataset, NULL, NULL);
     if (cond != DIMSE_NORMAL) return cond;
  }  

  OFString sopClass(rq.msg.NCreateRQ.AffectedSOPClassUID);
  if (sopClass == UID_BasicFilmSessionSOPClass)
  {
    // BFS N-CREATE
    filmSessionNCreate(rqDataset, rsp, rspDataset);
  } else if (sopClass == UID_BasicFilmBoxSOPClass)
  {
    // BFB N-CREATE
    filmBoxNCreate(rqDataset, rsp, rspDataset);
  } else if (sopClass == UID_PresentationLUTSOPClass)
  {
    // P-LUT N-CREATE
    presentationLUTNCreate(rqDataset, rsp, rspDataset);
  } else {
    CERR << "error: N-CREATE unsupported for SOP class '" << sopClass.c_str() << "'" << endl;
    rsp.msg.NCreateRSP.DimseStatus = 0x0118; // no such SOP Class    
    rsp.msg.NCreateRSP.opts = 0;  // don't include affected SOP instance UID
  }
  cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &rsp, NULL, rspDataset, NULL, NULL);
  delete rqDataset;
  delete rspDataset;
  return cond;
}

CONDITION DVPSPrintSCP::handleNDelete(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID)
{
  // initialize response message
  T_DIMSE_Message rsp;
  rsp.CommandField = DIMSE_N_DELETE_RSP;
  rsp.msg.NDeleteRSP.MessageIDBeingRespondedTo = rq.msg.NDeleteRQ.MessageID;
  rsp.msg.NDeleteRSP.AffectedSOPClassUID[0] = 0;
  rsp.msg.NDeleteRSP.DimseStatus = STATUS_Success;
  rsp.msg.NDeleteRSP.AffectedSOPInstanceUID[0] = 0;
  rsp.msg.NDeleteRSP.DataSetType = DIMSE_DATASET_NULL;
  rsp.msg.NDeleteRSP.opts = 0;

  CONDITION cond = DIMSE_NORMAL;
  if (rq.msg.NDeleteRQ.DataSetType == DIMSE_DATASET_PRESENT)
  {
     // should not happen
     DcmDataset *dataset = NULL;
     cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout, &presID, &dataset, NULL, NULL);
     delete dataset;
     if (cond != DIMSE_NORMAL) return cond;
  }  

  OFString sopClass(rq.msg.NDeleteRQ.RequestedSOPClassUID);
  if (sopClass == UID_BasicFilmSessionSOPClass)
  {
    // BFS N-DELETE
    filmSessionNDelete(rq, rsp);
  } else if (sopClass == UID_BasicFilmBoxSOPClass)
  {
    // BFB N-DELETE
    filmBoxNDelete(rq, rsp);
  } else if (sopClass == UID_PresentationLUTSOPClass)
  {
    // P-LUT N-DELETE
    presentationLUTNDelete(rq, rsp);
  } else {
    CERR << "error: N-DELETE unsupported for SOP class '" << sopClass.c_str() << "'" << endl;
    rsp.msg.NDeleteRSP.DimseStatus = 0x0118; // no such SOP Class    
  }
  return DIMSE_sendMessageUsingMemoryData(assoc, presID, &rsp, NULL, NULL, NULL, NULL);
}


void DVPSPrintSCP::printerNGet(T_DIMSE_Message& rq, T_DIMSE_Message& rsp, DcmDataset *& rspDataset)
{  
  OFString printerInstance(UID_PrinterSOPInstance);
  if (printerInstance == rq.msg.NGetRQ.RequestedSOPInstanceUID)
  {
    OFBool result = OFTrue;
    rspDataset = new DcmDataset;    
    if (rspDataset == NULL)
    {
      rsp.msg.NGetRSP.DimseStatus = 0x0110; // processing failure
      result = OFFalse;
    }
    
    int i=0;
    DIC_US group=0;
    DIC_US element=0;
    while (i+1 < rq.msg.NGetRQ.ListCount)
    {
      group = rq.msg.NGetRQ.AttributeIdentifierList[i++];
      element = rq.msg.NGetRQ.AttributeIdentifierList[i++];
      if ((group == 0x2110)&&(element == 0x0010))
      {
       if (EC_Normal != DVPSHelper::putStringValue(rspDataset, DCM_PrinterStatus, DEFAULT_printerStatus)) result = OFFalse;
      } 
      else if  ((group == 0x2110)&&(element == 0x0020))
      {
       if (EC_Normal != DVPSHelper::putStringValue(rspDataset, DCM_PrinterStatusInfo, DEFAULT_printerStatusInfo)) result = OFFalse;
      }
      else
      {
      	char buf[20];
      	sprintf(buf, "(%04x,%04x)", (int)group, (int)element);
        CERR << "error: cannot retrieve printer information: unsupported attribute " << buf << " in attribute list." << endl;
        rsp.msg.NGetRSP.DimseStatus = 0x0107; // attribute list error
        result = OFFalse;
      }
    }

    if (rq.msg.NGetRQ.ListCount == 0)
    {
     if (EC_Normal != DVPSHelper::putStringValue(rspDataset, DCM_PrinterStatus, DEFAULT_printerStatus)) result = OFFalse;
     if (EC_Normal != DVPSHelper::putStringValue(rspDataset, DCM_PrinterStatusInfo, DEFAULT_printerStatusInfo)) result = OFFalse;
    }

    if (result)
    {
      rsp.msg.NSetRSP.DataSetType = DIMSE_DATASET_PRESENT;
    } else {
      delete rspDataset;
      rspDataset = NULL;
      if (rsp.msg.NGetRSP.DimseStatus == 0) rsp.msg.NGetRSP.DimseStatus = 0x0110; // processing failure
      result = OFFalse;
    }
  } else {
    CERR << "error: cannot retrieve printer information, instance UID is not well-known printer SOP instance UID." << endl;
    rsp.msg.NGetRSP.DimseStatus = 0x0112; // no such object instance
  }	  
}

void DVPSPrintSCP::filmSessionNSet(T_DIMSE_Message& rq, DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset)
{
  if (filmSession && (filmSession->isInstance(rq.msg.NSetRQ.RequestedSOPInstanceUID)))
  {
    DVPSFilmSession *newSession = new DVPSFilmSession(*filmSession);
    if (newSession)
    {
      if (newSession->printSCPSet(dviface, cfgname, rqDataset, rsp, rspDataset))
      {
        delete filmSession;
        filmSession = newSession;
      } else delete newSession;
    } else {
      CERR << "error: cannot update film session, out of memory." << endl;
      rsp.msg.NSetRSP.DimseStatus = 0x0110; // processing failure 
    }    
  } else {
    // film session does not exist or wrong instance UID
    CERR << "error: cannot update film session, object not found." << endl;
    rsp.msg.NSetRSP.DimseStatus = 0x0112; // no such object instance
  }
}

void DVPSPrintSCP::filmBoxNSet(T_DIMSE_Message& rq, DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset)
{
  OFBool plut = OFFalse;
  if (assoc && (0 != ASC_findAcceptedPresentationContextID(assoc, UID_PresentationLUTSOPClass))) plut=OFTrue;
  storedPrintList.printSCPBasicFilmBoxSet(dviface, cfgname, rq, rqDataset, rsp, rspDataset, plut, presentationLUTList);
}

void DVPSPrintSCP::filmSessionNAction(T_DIMSE_Message& rq, T_DIMSE_Message& rsp)
{
  if (filmSession && (filmSession->isInstance(rq.msg.NActionRQ.RequestedSOPInstanceUID)))
  {
    storedPrintList.printSCPBasicFilmSessionAction(dviface, cfgname, rsp, presentationLUTList);
  } else {
    // film session does not exist or wrong instance UID
    CERR << "error: cannot print film session, object not found." << endl;
    rsp.msg.NActionRSP.DimseStatus = 0x0112; // no such object instance
  }
}

void DVPSPrintSCP::filmBoxNAction(T_DIMSE_Message& rq, T_DIMSE_Message& rsp)
{
  storedPrintList.printSCPBasicFilmBoxAction(dviface, cfgname, rq, rsp, presentationLUTList);
}

void DVPSPrintSCP::filmSessionNCreate(DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset)
{
  if (filmSession)
  {
    // film session exists already, refuse n-create
    CERR << "error: cannot create two film sessions concurrently." << endl;
    rsp.msg.NCreateRSP.DimseStatus = 0x0111; // duplicate SOP instance  
    rsp.msg.NCreateRSP.opts = 0;  // don't include affected SOP instance UID   
  } else {
    DVPSFilmSession *newSession = new DVPSFilmSession();
    if (newSession)
    {
      DIC_AE peerTitle;
      peerTitle[0]=0;
      ASC_getAPTitles(assoc->params, peerTitle, NULL, NULL);     
      if (newSession->printSCPCreate(dviface, cfgname, rqDataset, rsp, rspDataset, peerTitle)) filmSession = newSession;
      char uid[100];
      studyInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
      psSeriesInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
      imageSeriesInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));      
    } else {
      CERR << "error: cannot create film session, out of memory." << endl;
      rsp.msg.NCreateRSP.DimseStatus = 0x0110; // processing failure 
      rsp.msg.NCreateRSP.opts = 0;  // don't include affected SOP instance UID   
    }
  }
}

void DVPSPrintSCP::filmBoxNCreate(DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset)
{
  if (filmSession)
  {
    if (storedPrintList.haveFilmBoxInstance(rsp.msg.NCreateRSP.AffectedSOPInstanceUID))
    {
      CERR << "error: cannot create film box, requested SOP instance UID already in use." << endl;
      rsp.msg.NCreateRSP.DimseStatus = 0x0111; // duplicate SOP instance  
      rsp.msg.NCreateRSP.opts = 0;  // don't include affected SOP instance UID   
    } else {
      DVPSStoredPrint *newSPrint = new DVPSStoredPrint(DEFAULT_illumination, DEFAULT_reflectedAmbientLight);
      if (newSPrint)
      {
        OFBool plut = OFFalse;
        if (assoc && (0 != ASC_findAcceptedPresentationContextID(assoc, UID_PresentationLUTSOPClass))) plut=OFTrue;
        if (newSPrint->printSCPCreate(dviface, cfgname, rqDataset, rsp, rspDataset, plut, 
          presentationLUTList, filmSession->getUID(), studyInstanceUID, psSeriesInstanceUID, imageSeriesInstanceUID)) 
        {
          storedPrintList.insert(newSPrint);
        } else delete newSPrint;
      } else {
        CERR << "error: cannot create film box, out of memory." << endl;
        rsp.msg.NCreateRSP.DimseStatus = 0x0110; // processing failure 
        rsp.msg.NCreateRSP.opts = 0;  // don't include affected SOP instance UID   
      }
    }
  } else {
    // no film session, refuse n-create
    CERR << "error: cannot create film box without film session." << endl;
    rsp.msg.NCreateRSP.DimseStatus = 0x0117; // invalid object instance  
    rsp.msg.NCreateRSP.opts = 0;  // don't include affected SOP instance UID   
  }
}

void DVPSPrintSCP::presentationLUTNCreate(DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset)
{
  if (presentationLUTList.findPresentationLUT(rsp.msg.NCreateRSP.AffectedSOPInstanceUID))
  {
    CERR << "error: cannot create presentation LUT, requested SOP instance UID already in use." << endl;
    rsp.msg.NCreateRSP.DimseStatus = 0x0111; // duplicate SOP instance  
    rsp.msg.NCreateRSP.opts = 0;  // don't include affected SOP instance UID   
  } else {
    DVPSPresentationLUT *newPLut = new DVPSPresentationLUT();
    if (newPLut)
    {
      OFBool matchRequired = dviface.getTargetPrinterPresentationLUTMatchRequired(cfgname);
      OFBool supports12Bit = dviface.getTargetPrinterSupports12BitTransmission(cfgname);
      if (newPLut->printSCPCreate(rqDataset, rsp, rspDataset, matchRequired, supports12Bit)) 
      {
        presentationLUTList.insert(newPLut);
      } else delete newPLut;
    } else {
      CERR << "error: cannot create presentation LUT, out of memory." << endl;
      rsp.msg.NCreateRSP.DimseStatus = 0x0110; // processing failure 
      rsp.msg.NCreateRSP.opts = 0;  // don't include affected SOP instance UID   
    }
  }
}

void DVPSPrintSCP::filmSessionNDelete(T_DIMSE_Message& rq, T_DIMSE_Message& rsp)
{
  if (filmSession && (filmSession->isInstance(rq.msg.NDeleteRQ.RequestedSOPInstanceUID)))
  {
    // delete film box hierarchy and film session object
    storedPrintList.clear();
    delete filmSession;
    filmSession = NULL;    
  } else {
    // film session does not exist or wrong instance UID
    CERR << "error: cannot delete film session with instance UID '" << rq.msg.NDeleteRQ.RequestedSOPInstanceUID << "': object does not exist." << endl;
    rsp.msg.NDeleteRSP.DimseStatus = 0x0112; // no such object instance
  }
}

void DVPSPrintSCP::filmBoxNDelete(T_DIMSE_Message& rq, T_DIMSE_Message& rsp)
{
  storedPrintList.printSCPBasicFilmBoxDelete(rq, rsp);
}

void DVPSPrintSCP::presentationLUTNDelete(T_DIMSE_Message& rq, T_DIMSE_Message& rsp)
{ 
  // check whether references to this object exist. In this case, don't delete
  if (storedPrintList.usesPresentationLUT(rq.msg.NDeleteRQ.RequestedSOPInstanceUID))
  {
    CERR << "error: cannot delete presentation LUT '" << rq.msg.NDeleteRQ.RequestedSOPInstanceUID << "': object still in use." << endl;
    rsp.msg.NDeleteRSP.DimseStatus = 0x0110; // processing failure
  } else {
    presentationLUTList.printSCPDelete(rq, rsp);
  }
}

void DVPSPrintSCP::imageBoxNSet(T_DIMSE_Message& rq, DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset)
{
  storedPrintList.printSCPBasicGrayscaleImageBoxSet(dviface, cfgname, rq, rqDataset, rsp, rspDataset);
}



/*
 *  $Log: dvpsprt.cc,v $
 *  Revision 1.1  2000-05-31 12:58:12  meichel
 *  Added initial Print SCP support
 *
 *
 */

