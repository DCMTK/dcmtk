/*
 *
 *  Copyright (C) 2009-2013, OFFIS e.V.
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
 *  Module:  dcmnet
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Base class for Service Class Providers (SCPs)
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmnet/scp.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmdata/dcostrmf.h" /* for class DcmOutputFileStream */

// ----------------------------------------------------------------------------

DcmSCP::DcmSCP() :
  m_assoc(NULL),
  m_cfg()
{
#ifdef HAVE_GUSI_H
  // needed for Macintosh.
  GUSISetup( GUSIwithSIOUXSockets );
  GUSISetup( GUSIwithInternetSockets );
#endif

#ifdef HAVE_WINSOCK_H
  WSAData winSockData;
  // we need at least version 1.1.
  WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
  WSAStartup(winSockVersionNeeded, &winSockData);
#endif
}

// ----------------------------------------------------------------------------

DcmSCP::~DcmSCP()
{
  // If there is an open association, drop it and free memory (just to be sure...)
  if (m_assoc)
  {
    dropAndDestroyAssociation();
  }

#ifdef HAVE_WINSOCK_H
  WSACleanup();
#endif
}

// ----------------------------------------------------------------------------

DcmSCPConfig& DcmSCP::getConfig()
{
  return *m_cfg;
}

// ----------------------------------------------------------------------------

OFCondition DcmSCP::setConfig(const DcmSCPConfig& config)
{
  if (isConnected())
  {
    return EC_IllegalCall; // TODO: need to find better error code
  }
  m_cfg = DcmSharedSCPConfig( config );
  return EC_Normal;
}


// ----------------------------------------------------------------------------

OFCondition DcmSCP::listen()
{

  // make sure not to let dcmdata remove trailing blank padding or perform other
  // manipulations. We want to see the real data.
  dcmEnableAutomaticInputDataCorrection.set( OFFalse );

  OFCondition cond = EC_Normal;
  // Make sure data dictionary is loaded.
  if( !dcmDataDict.isDictionaryLoaded() )
    DCMNET_WARN("No data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE);

#ifndef DISABLE_PORT_PERMISSION_CHECK
#ifdef HAVE_GETEUID
  // If port is privileged we must be as well.
  if( m_cfg->getPort() < 1024 && geteuid() != 0 )
  {
    DCMNET_ERROR("No privileges to open this network port (" << m_cfg->getPort() << ")");
    return NET_EC_InsufficientPortPrivileges;
  }
#endif
#endif

  // Initialize network, i.e. create an instance of T_ASC_Network*.
  T_ASC_Network *network = NULL;
  cond = ASC_initializeNetwork( NET_ACCEPTOR, OFstatic_cast(int, m_cfg->getPort()), m_cfg->getACSETimeout(), &network );
  if( cond.bad() )
    return cond;

#if defined(HAVE_SETUID) && defined(HAVE_GETUID)
  // Return to normal uid so that we can't do too much damage in case
  // things go very wrong. Only works if the program is setuid root,
  // and run by another user. Running as root user may be
  // potentially disastrous if this program screws up badly.
  setuid( getuid() );
#endif

  // If we get to this point, the entire initialization process has been completed
  // successfully. Now, we want to start handling all incoming requests. Since
  // this activity is supposed to represent a server process, we do not want to
  // terminate this activity (unless indicated by the stopAfterCurrentAssociation()
  // method). Hence, create an infinite while-loop.
  while( cond.good() && !stopAfterCurrentAssociation() )
  {
    // Wait for an association and handle the requests of
    // the calling applications correspondingly.
    cond = waitForAssociationRQ(network);
  }
  // Drop the network, i.e. free memory of T_ASC_Network* structure. This call
  // is the counterpart of ASC_initializeNetwork(...) which was called above.
  cond = ASC_dropNetwork( &network );
  network = NULL;

  // return ok
  return cond;
}

// ----------------------------------------------------------------------------

void DcmSCP::findPresentationContext(const T_ASC_PresentationContextID presID,
                                     OFString &abstractSyntax,
                                     OFString &transferSyntax)
{
  transferSyntax.clear();
  abstractSyntax.clear();
  if (m_assoc == NULL)
    return;

  DUL_PRESENTATIONCONTEXT *pc;
  LST_HEAD **l;

  /* we look for a presentation context matching
   * both abstract and transfer syntax
   */
  l = &m_assoc->params->DULparams.acceptedPresentationContext;
  pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
  (void)LST_Position(l, (LST_NODE*)pc);
  while (pc)
  {
     if (presID == pc->presentationContextID)
     {
       if (pc->result == ASC_P_ACCEPTANCE)
       {
         // found a match
         transferSyntax = pc->acceptedTransferSyntax;
         abstractSyntax = pc->abstractSyntax;
       }
       break;
     }
     pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
  }
}


DUL_PRESENTATIONCONTEXT* DcmSCP::findPresentationContextID(LST_HEAD *head,
                                                           T_ASC_PresentationContextID presentationContextID)
{
  DUL_PRESENTATIONCONTEXT *pc;
  LST_HEAD **l;
  OFBool found = OFFalse;

  if (head == NULL)
    return NULL;

  l = &head;
  if (*l == NULL)
    return NULL;

  pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
  (void)LST_Position(l, (LST_NODE*)pc);

  while (pc && !found)
  {
    if (pc->presentationContextID == presentationContextID)
    {
      found = OFTrue;
    } else
    {
      pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
    }
  }
  return pc;
}

// ----------------------------------------------------------------------------

void DcmSCP::refuseAssociation(const DcmRefuseReasonType reason)
{
  if (m_assoc == NULL)
  {
    DCMNET_WARN("DcmSCP::refuseAssociation() called but actually no association running, ignoring");
    return;
  }

  T_ASC_RejectParameters rej;

  // dump some information if required
  switch( reason )
  {
    case DCMSCP_TOO_MANY_ASSOCIATIONS:
      DCMNET_INFO("Refusing Association (too many associations)");
      break;
    case DCMSCP_CANNOT_FORK:
      DCMNET_INFO("Refusing Association (cannot fork)");
      break;
    case DCMSCP_BAD_APPLICATION_CONTEXT_NAME:
      DCMNET_INFO("Refusing Association (bad application context)");
      break;
    case DCMSCP_CALLED_AE_TITLE_NOT_RECOGNIZED:
      DCMNET_INFO("Refusing Association (called AE title not recognized)");
      break;
    case DCMSCP_CALLING_AE_TITLE_NOT_RECOGNIZED:
      DCMNET_INFO("Refusing Association (calling AE title not recognized)");
      break;
    case DCMSCP_FORCED:
      DCMNET_INFO("Refusing Association (forced via command line)");
      break;
    case DCMSCP_NO_IMPLEMENTATION_CLASS_UID:
      DCMNET_INFO("Refusing Association (no implementation class UID provided)");
      break;
    case DCMSCP_NO_PRESENTATION_CONTEXTS:
      DCMNET_INFO("Refusing Association (no acceptable presentation contexts)");
      break;
    case DCMSCP_INTERNAL_ERROR:
      DCMNET_INFO("Refusing Association (internal error)");
      break;
    default:
      DCMNET_INFO("Refusing Association (unknown reason)");
      break;
  }

  // Set some values in the reject message depending on the reason
  switch( reason )
  {
    case DCMSCP_TOO_MANY_ASSOCIATIONS:
      rej.result = ASC_RESULT_REJECTEDTRANSIENT;
      rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
      rej.reason = ASC_REASON_SP_PRES_LOCALLIMITEXCEEDED;
      break;
    case DCMSCP_CANNOT_FORK:
      rej.result = ASC_RESULT_REJECTEDPERMANENT;
      rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
      rej.reason = ASC_REASON_SP_PRES_TEMPORARYCONGESTION;
      break;
    case DCMSCP_BAD_APPLICATION_CONTEXT_NAME:
      rej.result = ASC_RESULT_REJECTEDTRANSIENT;
      rej.source = ASC_SOURCE_SERVICEUSER;
      rej.reason = ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED;
      break;
    case DCMSCP_CALLED_AE_TITLE_NOT_RECOGNIZED:
      rej.result = ASC_RESULT_REJECTEDPERMANENT;
      rej.source = ASC_SOURCE_SERVICEUSER;
      rej.reason = ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED;
      break;
    case DCMSCP_CALLING_AE_TITLE_NOT_RECOGNIZED:
      rej.result = ASC_RESULT_REJECTEDPERMANENT;
      rej.source = ASC_SOURCE_SERVICEUSER;
      rej.reason = ASC_REASON_SU_CALLINGAETITLENOTRECOGNIZED;
      break;
    case DCMSCP_FORCED:
    case DCMSCP_NO_IMPLEMENTATION_CLASS_UID:
    case DCMSCP_NO_PRESENTATION_CONTEXTS:
    case DCMSCP_INTERNAL_ERROR:
    default:
      rej.result = ASC_RESULT_REJECTEDPERMANENT;
      rej.source = ASC_SOURCE_SERVICEUSER;
      rej.reason = ASC_REASON_SU_NOREASON;
      break;
  }

  // Reject the association request.
  ASC_rejectAssociation( m_assoc, &rej );

  // Drop and destroy the association.
  dropAndDestroyAssociation();
}

// ----------------------------------------------------------------------------

OFCondition DcmSCP::waitForAssociationRQ(T_ASC_Network *network)
{
  if (network == NULL)
    return ASC_NULLKEY;
  if (m_assoc != NULL)
    return DIMSE_ILLEGALASSOCIATION;

  Uint32 timeout = m_cfg->getConnnectionTimeout();

  // Listen to a socket for timeout seconds and wait for an association request
  OFCondition cond = ASC_receiveAssociation( network, &m_assoc, m_cfg->getMaxReceivePDULength(), NULL, NULL, OFFalse,
                                             m_cfg->getConnectionBlockingMode(), OFstatic_cast(int, timeout) );

  // just return, if timeout occurred (DUL_NOASSOCIATIONREQUEST)
  if ( cond == DUL_NOASSOCIATIONREQUEST )
  {
    return EC_Normal;
  }

  // if error occurs close association and return
  if( cond.bad() )
  {
    dropAndDestroyAssociation();
    return EC_Normal;
  }

  return processAssociationRQ();
}


OFCondition DcmSCP::processAssociationRQ()
{
  DcmSCPActionType desiredAction = DCMSCP_ACTION_UNDEFINED;
  if ( (m_assoc == NULL) || (m_assoc->params == NULL) )
    return ASC_NULLKEY;

  // call notifier function
  notifyAssociationRequest(*m_assoc->params, desiredAction);
  if (desiredAction != DCMSCP_ACTION_UNDEFINED)
  {
    if (desiredAction == DCMSCP_ACTION_REFUSE_ASSOCIATION)
    {
      refuseAssociation( DCMSCP_INTERNAL_ERROR );
      dropAndDestroyAssociation();
      return EC_Normal;
    }
    else desiredAction = DCMSCP_ACTION_UNDEFINED; // reset for later use
  }

  // Now we have to figure out if we might have to refuse the association request.
  // This is the case if at least one of five conditions is met:

  // Condition 1: if option "--refuse" is set we want to refuse the association request.
  if( m_cfg->getRefuseAssociation() )
  {
    refuseAssociation( DCMSCP_FORCED );
    dropAndDestroyAssociation();
    return EC_Normal;
  }

  // Condition 2: determine the application context name. If an error occurred or if the
  // application context name is not supported we want to refuse the association request.
  char buf[BUFSIZ];
  OFCondition cond = ASC_getApplicationContextName( m_assoc->params, buf );
  if( cond.bad() || strcmp( buf, DICOM_STDAPPLICATIONCONTEXT ) != 0 )
  {
    refuseAssociation( DCMSCP_BAD_APPLICATION_CONTEXT_NAME );
    dropAndDestroyAssociation();
    return EC_Normal;
  }

  // Condition 3: if the calling or called application entity title is not supported
  // we want to refuse the association request
  if (!checkCalledAETitleAccepted(m_assoc->params->DULparams.calledAPTitle))
  {
    refuseAssociation( DCMSCP_CALLED_AE_TITLE_NOT_RECOGNIZED );
    dropAndDestroyAssociation();
    return EC_Normal;
  }

  if (!checkCallingAETitleAccepted(m_assoc->params->DULparams.callingAPTitle))
  {
    refuseAssociation( DCMSCP_CALLING_AE_TITLE_NOT_RECOGNIZED );
    dropAndDestroyAssociation();
    return EC_Normal;
  }

  /* set our application entity title */
  if (m_cfg->getRespondWithCalledAETitle())
    ASC_setAPTitles(m_assoc->params, NULL, NULL, m_assoc->params->DULparams.calledAPTitle);
  else
    ASC_setAPTitles(m_assoc->params, NULL, NULL, m_cfg->getAETitle().c_str());

  /* If we get to this point the association shall be negotiated.
     Thus, for every presentation context it is checked whether
     it can be accepted. However, this is only a "dry" run, i.e.
     there is not yet sent a response message to the SCU
   */
  cond = negotiateAssociation();
  if( cond.bad() )
  {
    dropAndDestroyAssociation();
    return EC_Normal;
  }

  // Reject association if no presentation context was negotiated
  if( ASC_countAcceptedPresentationContexts( m_assoc->params ) == 0 )
  {
    // Dump some debug information
    OFString tempStr;
    DCMNET_INFO("No Acceptable Presentation Contexts");
    if (m_cfg->getVerbosePCMode())
      DCMNET_INFO(ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_RJ));
    else
      DCMNET_DEBUG(ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_RJ));
    refuseAssociation( DCMSCP_NO_PRESENTATION_CONTEXTS );
    dropAndDestroyAssociation();
    return EC_Normal;
  }

  // If the negotiation was successful, accept the association request
  cond = ASC_acknowledgeAssociation( m_assoc );
  if( cond.bad() )
  {
    dropAndDestroyAssociation();
    return EC_Normal;
  }
  notifyAssociationAcknowledge();

  // Dump some debug information
  OFString tempStr;
  DCMNET_INFO("Association Acknowledged (Max Send PDV: " << OFstatic_cast(Uint32, m_assoc->sendPDVLength) << ")");
  if (m_cfg->getVerbosePCMode())
    DCMNET_INFO(ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_AC));
  else
    DCMNET_DEBUG(ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_AC));

   // Go ahead and handle the association (i.e. handle the callers requests) in this process
   handleAssociation();
   return EC_Normal;
}

// ----------------------------------------------------------------------------

OFCondition DcmSCP::negotiateAssociation()
{
  // check whether there is something to negotiate...
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  /* set presentation contexts as defined in association configuration */
  OFCondition result = m_cfg->evaluateIncomingAssociation(*m_assoc);
  if (result.bad())
  {
    OFString tempStr;
    DCMNET_ERROR(DimseCondition::dump(tempStr, result));
  }
  return result;
}

// ----------------------------------------------------------------------------

void DcmSCP::handleAssociation()
{
  if (m_assoc == NULL)
  {
    DCMNET_WARN("DcmSCP::handleAssociation() called but SCP actually has no association running, ignoring");
    return;
  }

  // Receive a DIMSE command and perform all the necessary actions. (Note that ReceiveAndHandleCommands()
  // will always return a value 'cond' for which 'cond.bad()' will be true. This value indicates that either
  // some kind of error occurred, or that the peer aborted the association (DUL_PEERABORTEDASSOCIATION),
  // or that the peer requested the release of the association (DUL_PEERREQUESTEDRELEASE).) (Also note
  // that ReceiveAndHandleCommands() will never return EC_Normal.)
  OFCondition cond = EC_Normal;
  T_DIMSE_Message message;
  T_ASC_PresentationContextID presID;

  // start a loop to be able to receive more than one DIMSE command
  while( cond.good() )
  {
    // receive a DIMSE command over the network
    cond = DIMSE_receiveCommand( m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
                                 &presID, &message, NULL );

    // check if peer did release or abort, or if we have a valid message
    if( cond.good() )
    {
      DcmPresentationContextInfo pcInfo;
      getPresentationContextInfo(m_assoc, presID, pcInfo);
      cond = handleIncomingCommand(&message, pcInfo);
    }
  }
  // Clean up on association termination.
  if( cond == DUL_PEERREQUESTEDRELEASE )
  {
    notifyReleaseRequest();
    ASC_acknowledgeRelease(m_assoc);
  }
  else if( cond == DUL_PEERABORTEDASSOCIATION )
  {
    notifyAbortRequest();
  }
  else
  {
    notifyDIMSEError(cond);
    ASC_abortAssociation( m_assoc );
  }

  // Drop and destroy the association.
  dropAndDestroyAssociation();

  // Output separator line.
  DCMNET_DEBUG( "+++++++++++++++++++++++++++++" );
}

// ----------------------------------------------------------------------------

OFCondition DcmSCP::handleIncomingCommand(T_DIMSE_Message *incomingMsg,
                                          const DcmPresentationContextInfo &info)
{
  OFCondition cond;
  if( incomingMsg->CommandField == DIMSE_C_ECHO_RQ )
  {
    // Process C-ECHO request
    cond = handleECHORequest( incomingMsg->msg.CEchoRQ, info.presentationContextID );
  } else {
    // We cannot handle this kind of message. Note that the condition will be returned
    // and that the caller is responsible to end the association if desired.
    OFString tempStr;
    DCMNET_ERROR("Cannot handle this kind of DIMSE command (0x"
      << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
      << OFstatic_cast(unsigned int, incomingMsg->CommandField) << ")");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, *incomingMsg, DIMSE_INCOMING));
    cond = DIMSE_BADCOMMANDTYPE;
  }

  // return result
  return cond;
}

// ----------------------------------------------------------------------------

// -- C-ECHO --

OFCondition DcmSCP::handleECHORequest(T_DIMSE_C_EchoRQ &reqMessage,
                                      const T_ASC_PresentationContextID presID)
{
  OFCondition cond;
  OFString tempStr;

  // Dump debug information
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Received C-ECHO Request");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_INFO("Sending C-ECHO Response");
  } else {
    DCMNET_INFO("Received C-ECHO Request (MsgID " << reqMessage.MessageID << ")");
    DCMNET_INFO("Sending C-ECHO Response (" << DU_cechoStatusString(STATUS_Success) << ")");
  }

  // Send response message
  cond = DIMSE_sendEchoResponse( m_assoc, presID, &reqMessage, STATUS_Success, NULL );
  if( cond.bad() )
    DCMNET_ERROR("Cannot send C-ECHO Response: " << DimseCondition::dump(tempStr, cond));
  else
    DCMNET_DEBUG("C-ECHO Response successfully sent");

  return cond;
}

// ----------------------------------------------------------------------------

// -- C-STORE --

OFCondition DcmSCP::handleSTORERequest(T_DIMSE_C_StoreRQ &reqMessage,
                                       const T_ASC_PresentationContextID presID,
                                       DcmDataset *&reqDataset)
{
  // First, receive the C-STORE request
  OFCondition cond = receiveSTORERequest(reqMessage, presID, reqDataset);

  if (cond.good())
  {
    // Then, check the request message and dataset and return an DIMSE status code
    const Uint16 rspStatusCode = checkSTORERequest(reqMessage, reqDataset);
    // ... that is sent back with the C-STORE response message
    cond = sendSTOREResponse(presID, reqMessage, rspStatusCode);
  }

  return cond;
}


OFCondition DcmSCP::receiveSTORERequest(T_DIMSE_C_StoreRQ &reqMessage,
                                        const T_ASC_PresentationContextID presID,
                                        DcmDataset *&reqDataset)
{
  // Do some basic validity checks
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID presIDdset;
  // Remember the passed dataset pointer
  DcmDataset *dataset = reqDataset;

  // Dump debug information
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    DCMNET_INFO("Received C-STORE Request");
  else
    DCMNET_INFO("Received C-STORE Request (MsgID " << reqMessage.MessageID << ")");

  // Check if dataset is announced correctly
  if (reqMessage.DataSetType == DIMSE_DATASET_NULL)
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Received C-STORE request but no dataset announced, aborting");
    return DIMSE_BADMESSAGE;
  }

  // Receive dataset (in memory)
  cond = receiveDIMSEDataset(&presIDdset, &dataset);
  if (cond.bad())
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Unable to receive C-STORE dataset on presentation context " << OFstatic_cast(unsigned int, presID));
    return cond;
  }

  // Output request message only if trace level is enabled
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, dataset, presID));
  else
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));

  // Compare presentation context ID of command and data set
  if (presIDdset != presID)
  {
    DCMNET_ERROR("Presentation Context ID of command (" << OFstatic_cast(unsigned int, presID)
      << ") and data set (" << OFstatic_cast(unsigned int, presIDdset) << ") differs");
    if (dataset != reqDataset)
    {
      // Free memory allocated by receiveDIMSEDataset()
      delete dataset;
    }
    return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error,
      "DIMSE: Presentation Contexts of Command and Data Set differ");
  }

  // Set return value
  reqDataset = dataset;

  return cond;
}


OFCondition DcmSCP::receiveSTORERequest(T_DIMSE_C_StoreRQ &reqMessage,
                                        const T_ASC_PresentationContextID presID,
                                        const OFString &filename)
{
  // Do some basic validity checks
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  OFString tempStr;
  // Use presentation context ID of the command set as a default
  T_ASC_PresentationContextID presIDdset = presID;

  // Dump debug information
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    DCMNET_INFO("Received C-STORE Request");
  else
    DCMNET_INFO("Received C-STORE Request (MsgID " << reqMessage.MessageID << ")");

  // Check if dataset is announced correctly
  if (reqMessage.DataSetType == DIMSE_DATASET_NULL)
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Received C-STORE request but no dataset announced, aborting");
    return DIMSE_BADMESSAGE;
  }

  // Receive dataset (directly to file)
  cond = receiveSTORERequestDataset(&presIDdset, reqMessage, filename);
  if (cond.bad())
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Unable to receive C-STORE dataset on presentation context " << OFstatic_cast(unsigned int, presID));
    return cond;
  }

  // Output request message only if trace level is enabled
  DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));

  // Compare presentation context ID of command and data set
  if (presIDdset != presID)
  {
    DCMNET_ERROR("Presentation Context ID of command (" << OFstatic_cast(unsigned int, presID)
      << ") and data set (" << OFstatic_cast(unsigned int, presIDdset) << ") differs");
    return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error,
      "DIMSE: Presentation Contexts of Command and Data Set differ");
  }

  return cond;
}


OFCondition DcmSCP::sendSTOREResponse(T_ASC_PresentationContextID presID,
                                      const T_DIMSE_C_StoreRQ &reqMessage,
                                      const Uint16 rspStatusCode)
{
  // Call the method doing the real work
  return sendSTOREResponse(presID, reqMessage.MessageID, reqMessage.AffectedSOPClassUID, reqMessage.AffectedSOPInstanceUID,
    rspStatusCode, NULL /* statusDetail */);
}


OFCondition DcmSCP::sendSTOREResponse(const T_ASC_PresentationContextID presID,
                                      const Uint16 messageID,
                                      const OFString &sopClassUID,
                                      const OFString &sopInstanceUID,
                                      const Uint16 rspStatusCode,
                                      DcmDataset *statusDetail)
{
  OFCondition cond;
  OFString tempStr;

  // Send back response
  T_DIMSE_Message response;
  // Make sure everything is zeroed (especially options)
  bzero((char*)&response, sizeof(response));
  T_DIMSE_C_StoreRSP &storeRsp = response.msg.CStoreRSP;
  response.CommandField = DIMSE_C_STORE_RSP;
  storeRsp.MessageIDBeingRespondedTo = messageID;
  storeRsp.DimseStatus = rspStatusCode;
  storeRsp.DataSetType = DIMSE_DATASET_NULL;
  // Always send the optional fields "Affected SOP Class UID" and "Affected SOP Instance UID"
  storeRsp.opts = O_STORE_AFFECTEDSOPCLASSUID | O_STORE_AFFECTEDSOPINSTANCEUID;
  OFStandard::strlcpy(storeRsp.AffectedSOPClassUID, sopClassUID.c_str(), sizeof(storeRsp.AffectedSOPClassUID));
  OFStandard::strlcpy(storeRsp.AffectedSOPInstanceUID, sopInstanceUID.c_str(), sizeof(storeRsp.AffectedSOPInstanceUID));

  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-STORE Response");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, NULL, presID));
  } else {
    DCMNET_INFO("Sending C-STORE Response (" << DU_cstoreStatusString(rspStatusCode) << ")");
  }

  // Send response message
  cond = sendDIMSEMessage(presID, &response, NULL /* dataObject */, statusDetail);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending C-STORE response: " << DimseCondition::dump(tempStr, cond));
  }

  return cond;
}


Uint16 DcmSCP::checkSTORERequest(T_DIMSE_C_StoreRQ & /*reqMessage*/,
                                 DcmDataset * /*reqDataset*/)
{
  // we default to success
  return STATUS_Success;
}

// ----------------------------------------------------------------------------

// -- C-FIND --

OFCondition DcmSCP::handleFINDRequest(T_DIMSE_C_FindRQ &reqMessage,
                                      const T_ASC_PresentationContextID presID,
                                      DcmDataset *&reqDataset)
{
  // Do some basic validity checks
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID presIDdset;
  DcmDataset *dataset = NULL;

  // Dump debug information
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    DCMNET_INFO("Received C-FIND Request");
  else
    DCMNET_INFO("Received C-FIND Request (MsgID " << reqMessage.MessageID << ")");

  // Check if dataset is announced correctly
  if (reqMessage.DataSetType == DIMSE_DATASET_NULL)
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Received C-FIND request but no dataset announced, aborting");
    return DIMSE_BADMESSAGE;
  }

  // Receive dataset
  cond = receiveDIMSEDataset(&presIDdset, &dataset);
  if (cond.bad())
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Unable to receive C-FIND dataset on presentation context " << OFstatic_cast(unsigned int, presID));
    return DIMSE_BADDATA;
  }

  // Output request message only if trace level is enabled
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, dataset, presID));
  else
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));

  // Compare presentation context ID of command and data set
  if (presIDdset != presID)
  {
    DCMNET_ERROR("Presentation Context ID of command (" << OFstatic_cast(unsigned int, presID)
      << ") and data set (" << OFstatic_cast(unsigned int, presIDdset) << ") differs");
    delete dataset;
    return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error,
      "DIMSE: Presentation Contexts of Command and Data Set differ");
  }

  // Set return value
  reqDataset = dataset;

  return cond;
}


OFCondition DcmSCP::sendFINDResponse(const T_ASC_PresentationContextID presID,
                                     const Uint16 messageID,
                                     const OFString &sopClassUID,
                                     DcmDataset *rspDataset,
                                     const Uint16 rspStatusCode,
                                     DcmDataset* statusDetail)
{
  OFCondition cond;
  OFString tempStr;

  // Send back response
  T_DIMSE_Message response;
  // Make sure everything is zeroed (especially options)
  bzero((char*)&response, sizeof(response));
  T_DIMSE_C_FindRSP &findRsp = response.msg.CFindRSP;
  response.CommandField = DIMSE_C_FIND_RSP;
  findRsp.MessageIDBeingRespondedTo = messageID;
  findRsp.DimseStatus = rspStatusCode;
  // Always send (the optional) field "Affected SOP Class UID"
  findRsp.opts = O_FIND_AFFECTEDSOPCLASSUID;
  OFStandard::strlcpy(findRsp.AffectedSOPClassUID, sopClassUID.c_str(), sizeof(findRsp.AffectedSOPClassUID));

  if (rspDataset)
    findRsp.DataSetType = DIMSE_DATASET_PRESENT;
  else
    findRsp.DataSetType = DIMSE_DATASET_NULL;

  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-FIND Response");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, rspDataset, presID));
  } else {
    DCMNET_INFO("Sending C-FIND Response (" << DU_cfindStatusString(rspStatusCode) << ")");
  }

  // Send response message with dataset
  cond = sendDIMSEMessage(presID, &response, rspDataset /* dataObject */, statusDetail);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending C-FIND response: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }
  return cond;
}


OFCondition DcmSCP::checkForCANCEL(T_ASC_PresentationContextID presID,
                                   const Uint16 messageID)
{
  return DIMSE_checkForCancelRQ(m_assoc, presID, messageID);
}

// ----------------------------------------------------------------------------

// -- C-MOVE --

OFCondition DcmSCP::handleMOVERequest(T_DIMSE_C_MoveRQ &reqMessage,
                                      const T_ASC_PresentationContextID presID,
                                      DcmDataset *&reqDataset,
                                      OFString &moveDest)
{
  // Do some basic validity checks
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID presIDdset;
  DcmDataset *dataset = NULL;

  // Dump debug information
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    DCMNET_INFO("Received C-MOVE Request");
  else
    DCMNET_INFO("Received C-MOVE Request (MsgID " << reqMessage.MessageID << ")");

  // Check if dataset is announced correctly
  if (reqMessage.DataSetType == DIMSE_DATASET_NULL)
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Received C-MOVE request but no dataset announced, aborting");
    return DIMSE_BADMESSAGE;
  }

  // Receive dataset
  cond = receiveDIMSEDataset(&presIDdset, &dataset);
  if (cond.bad())
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Unable to receive C-MOVE dataset on presentation context " << OFstatic_cast(unsigned int, presID));
    return DIMSE_BADDATA;
  }

  // Output request message only if trace level is enabled
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, dataset, presID));
  else
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));

  // Compare presentation context ID of command and data set
  if (presIDdset != presID)
  {
    DCMNET_ERROR("Presentation Context ID of command (" << OFstatic_cast(unsigned int, presID)
      << ") and data set (" << OFstatic_cast(unsigned int, presIDdset) << ") differs");
    delete dataset;
    return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error,
      "DIMSE: Presentation Contexts of Command and Data Set differ");
  }

  // Set return values
  reqDataset = dataset;
  moveDest = reqMessage.MoveDestination;

  return cond;
}


OFCondition DcmSCP::sendMOVEResponse(const T_ASC_PresentationContextID presID,
                                     const Uint16 messageID,
                                     const OFString &sopClassUID,
                                     DcmDataset *rspDataset,
                                     const Uint16 rspStatusCode,
                                     DcmDataset *statusDetail,
                                     const Uint16 numRemain,
                                     const Uint16 numComplete,
                                     const Uint16 numFail,
                                     const Uint16 numWarn)
{
  OFCondition cond;
  OFString tempStr;

  // Send back response
  T_DIMSE_Message response;
  // Make sure everything is zeroed (especially options)
  bzero((char*)&response, sizeof(response));
  T_DIMSE_C_MoveRSP &moveRsp = response.msg.CMoveRSP;
  response.CommandField = DIMSE_C_MOVE_RSP;
  moveRsp.MessageIDBeingRespondedTo = messageID;
  moveRsp.DimseStatus = rspStatusCode;
  // Always send the optional field "Affected SOP Class UID"
  moveRsp.opts = O_MOVE_AFFECTEDSOPCLASSUID;
  OFStandard::strlcpy(moveRsp.AffectedSOPClassUID, sopClassUID.c_str(), sizeof(moveRsp.AffectedSOPClassUID));
  // Only send the other optional fields if needed
  if ( (numRemain != 0) || (numComplete != 0) || (numFail != 0) || (numWarn != 0) )
  {
    moveRsp.NumberOfRemainingSubOperations = numRemain;
    moveRsp.NumberOfCompletedSubOperations = numComplete;
    moveRsp.NumberOfFailedSubOperations = numFail;
    moveRsp.NumberOfWarningSubOperations = numWarn;
    moveRsp.opts |= O_MOVE_NUMBEROFREMAININGSUBOPERATIONS | O_MOVE_NUMBEROFCOMPLETEDSUBOPERATIONS |
                    O_MOVE_NUMBEROFFAILEDSUBOPERATIONS | O_MOVE_NUMBEROFWARNINGSUBOPERATIONS;
  }

  if (rspDataset)
    moveRsp.DataSetType = DIMSE_DATASET_PRESENT;
  else
    moveRsp.DataSetType = DIMSE_DATASET_NULL;

  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending C-MOVE Response");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, rspDataset, presID));
  } else {
    DCMNET_INFO("Sending C-MOVE Response (" << DU_cmoveStatusString(rspStatusCode) << ")");
  }

  // Send response message with dataset
  cond = sendDIMSEMessage(presID, &response, rspDataset /* dataObject */, statusDetail);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending C-MOVE response: " << DimseCondition::dump(tempStr, cond));
  }

  return cond;
}

// ----------------------------------------------------------------------------

// -- N-ACTION --

OFCondition DcmSCP::handleACTIONRequest(T_DIMSE_N_ActionRQ &reqMessage,
                                        const T_ASC_PresentationContextID presID,
                                        DcmDataset *&reqDataset,
                                        Uint16 &actionTypeID)
{
  // Do some basic validity checks
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID presIDdset;
  DcmDataset *dataset = NULL;

  // Dump debug information
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    DCMNET_INFO("Received N-ACTION Request");
  else
    DCMNET_INFO("Received N-ACTION Request (MsgID " << reqMessage.MessageID << ")");

  // Check if dataset is announced correctly
  if (reqMessage.DataSetType == DIMSE_DATASET_NULL)
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Received N-ACTION request but no dataset announced, aborting");
    return DIMSE_BADMESSAGE;
  }

  // Receive dataset
  cond = receiveDIMSEDataset(&presIDdset, &dataset);
  if (cond.bad())
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Unable to receive N-ACTION dataset on presentation context " << OFstatic_cast(unsigned int, presID));
    return DIMSE_BADDATA;
  }

  // Output request message only if trace level is enabled
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, dataset, presID));
  else
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));

  // Compare presentation context ID of command and data set
  if (presIDdset != presID)
  {
    DCMNET_ERROR("Presentation Context ID of command (" << OFstatic_cast(unsigned int, presID)
      << ") and data set (" << OFstatic_cast(unsigned int, presIDdset) << ") differs");
    delete dataset;
    return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error,
      "DIMSE: Presentation Contexts of Command and Data Set differ");
  }

  // Set return values
  reqDataset = dataset;
  actionTypeID = reqMessage.ActionTypeID;

  return cond;
}


OFCondition DcmSCP::sendACTIONResponse(const T_ASC_PresentationContextID presID,
                                       const Uint16 messageID,
                                       const OFString &sopClassUID,
                                       const OFString &sopInstanceUID,
                                       const Uint16 rspStatusCode)
{
  OFCondition cond;
  OFString tempStr;

  // Send back response
  T_DIMSE_Message response;
  // Make sure everything is zeroed (especially options)
  bzero((char*)&response, sizeof(response));
  T_DIMSE_N_ActionRSP &actionRsp = response.msg.NActionRSP;
  response.CommandField = DIMSE_N_ACTION_RSP;
  actionRsp.MessageIDBeingRespondedTo = messageID;
  actionRsp.DimseStatus = rspStatusCode;
  actionRsp.DataSetType = DIMSE_DATASET_NULL;
  // Always send the optional fields "Affected SOP Class UID" and "Affected SOP Instance UID"
  actionRsp.opts = O_NACTION_AFFECTEDSOPCLASSUID | O_NACTION_AFFECTEDSOPINSTANCEUID;
  OFStandard::strlcpy(actionRsp.AffectedSOPClassUID, sopClassUID.c_str(), sizeof(actionRsp.AffectedSOPClassUID));
  OFStandard::strlcpy(actionRsp.AffectedSOPInstanceUID, sopInstanceUID.c_str(), sizeof(actionRsp.AffectedSOPInstanceUID));
  // Do not send any other optional fields, e.g. "Action Type ID"

  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending N-ACTION Response");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, NULL, presID));
  } else {
    DCMNET_INFO("Sending N-ACTION Response (" << DU_nactionStatusString(rspStatusCode) << ")");
  }

  // Send response message
  cond = sendDIMSEMessage(presID, &response, NULL /* dataObject */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending N-ACTION response: " << DimseCondition::dump(tempStr, cond));
  }

  return cond;
}

// ----------------------------------------------------------------------------

// -- N-EVENT-REPORT --

OFCondition DcmSCP::handleEVENTREPORTRequest(T_DIMSE_N_EventReportRQ &reqMessage,
                                             const T_ASC_PresentationContextID presID,
                                             DcmDataset *&reqDataset,
                                             Uint16 &eventTypeID)
{
  // Do some basic validity checks
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID presIDdset;
  DcmDataset *dataset = NULL;
  // DcmDataset *statusDetail = NULL; // TODO: do we need this and if so, how do we get it?
  Uint16 rspStatusCode = 0;

  // Dump debug information
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
    DCMNET_INFO("Received N-EVENT-REPORT Request");
  else
    DCMNET_INFO("Received N-EVENT-REPORT Request (MsgID " << reqMessage.MessageID << ")");

  // Check if dataset is announced correctly
  if (reqMessage.DataSetType == DIMSE_DATASET_NULL)
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Received N-EVENT-REPORT request but no dataset announced, aborting");
    return DIMSE_BADMESSAGE;
  }

  // Receive dataset
  cond = receiveDIMSEDataset(&presIDdset, &dataset);
  if (cond.bad())
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Unable to receive N-EVENT-REPORT dataset on presentation context " << OFstatic_cast(unsigned int, presID));
    return DIMSE_BADDATA;
  }

  // Output dataset only if trace level is enabled
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, dataset, presID));
  else
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, reqMessage, DIMSE_INCOMING, NULL, presID));

  // Compare presentation context ID of command and data set
  if (presIDdset != presID)
  {
    DCMNET_ERROR("Presentation Context ID of command (" << OFstatic_cast(unsigned int, presID)
      << ") and data set (" << OFstatic_cast(unsigned int, presIDdset) << ") differs");
    delete dataset;
    return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error,
      "DIMSE: Presentation Contexts of Command and Data Set differ");
  }

  // Check the request message and dataset and return the DIMSE status code to be used
  rspStatusCode = checkEVENTREPORTRequest(reqMessage, dataset);

  // Send back response
  T_DIMSE_Message response;
  // Make sure everything is zeroed (especially options)
  bzero((char*)&response, sizeof(response));
  T_DIMSE_N_EventReportRSP &eventReportRsp = response.msg.NEventReportRSP;
  response.CommandField = DIMSE_N_EVENT_REPORT_RSP;
  eventReportRsp.MessageIDBeingRespondedTo = reqMessage.MessageID;
  eventReportRsp.DimseStatus = rspStatusCode;
  eventReportRsp.DataSetType = DIMSE_DATASET_NULL;
  // Do not send any optional fields
  eventReportRsp.opts = 0;
  eventReportRsp.AffectedSOPClassUID[0] = 0;
  eventReportRsp.AffectedSOPInstanceUID[0] = 0;

  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending N-EVENT-REPORT Response");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, NULL, presID));
  } else {
    DCMNET_INFO("Sending N-EVENT-REPORT Response (" << DU_neventReportStatusString(rspStatusCode) << ")");
  }
  // Send response message
  cond = sendDIMSEMessage(presID, &response, NULL /* dataObject */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending N-EVENT-REPORT response: " << DimseCondition::dump(tempStr, cond));
    delete dataset;
    return cond;
  }

  // Set return values
  reqDataset = dataset;
  eventTypeID = reqMessage.EventTypeID;

  return cond;
}


OFCondition DcmSCP::sendEVENTREPORTRequest(const T_ASC_PresentationContextID presID,
                                           const OFString &sopInstanceUID,
                                           const Uint16 messageID,
                                           const Uint16 eventTypeID,
                                           DcmDataset *reqDataset,
                                           Uint16 &rspStatusCode)
{
  // Do some basic validity checks
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;
  if (sopInstanceUID.empty() || (reqDataset == NULL))
    return DIMSE_NULLKEY;

  // Prepare DIMSE data structures for issuing request
  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID pcid = presID;
  T_DIMSE_Message request;
  // Make sure everything is zeroed (especially options)
  bzero((char*)&request, sizeof(request));
  T_DIMSE_N_EventReportRQ &eventReportReq = request.msg.NEventReportRQ;
  DcmDataset *statusDetail = NULL;

  request.CommandField = DIMSE_N_EVENT_REPORT_RQ;

  // Generate a new message ID (?)
  eventReportReq.MessageID = messageID;

  eventReportReq.DataSetType = DIMSE_DATASET_PRESENT;
  eventReportReq.EventTypeID = eventTypeID;

  // Determine SOP Class from presentation context
  OFString abstractSyntax, transferSyntax;

  findPresentationContext(pcid, abstractSyntax, transferSyntax);
  if (abstractSyntax.empty() || transferSyntax.empty())
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  OFStandard::strlcpy(eventReportReq.AffectedSOPClassUID, abstractSyntax.c_str(), sizeof(eventReportReq.AffectedSOPClassUID));
  OFStandard::strlcpy(eventReportReq.AffectedSOPInstanceUID, sopInstanceUID.c_str(), sizeof(eventReportReq.AffectedSOPInstanceUID));

  // Send request
  if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    DCMNET_INFO("Sending N-EVENT-REPORT Request");
    // Output dataset only if trace level is enabled
    if (DCM_dcmnetLogger.isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_OUTGOING, reqDataset, pcid));
    else
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_OUTGOING, NULL, pcid));
  } else {
    DCMNET_INFO("Sending N-EVENT-REPORT Request (MsgID " << eventReportReq.MessageID << ")");
  }
  cond = sendDIMSEMessage(pcid, &request, reqDataset);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending N-EVENT-REPORT request: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }
  // Receive response
  T_DIMSE_Message response;
  cond = receiveDIMSECommand(&pcid, &response, &statusDetail, NULL /* commandSet */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed receiving DIMSE response: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }

  // Check command set
  if (response.CommandField == DIMSE_N_EVENT_REPORT_RSP)
  {
    if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
      DCMNET_INFO("Received N-EVENT-REPORT Response");
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_INCOMING, NULL, pcid));
    } else {
      DCMNET_INFO("Received N-EVENT-REPORT Response (" << DU_neventReportStatusString(response.msg.NEventReportRSP.DimseStatus) << ")");
    }
  } else {
    DCMNET_ERROR("Expected N-EVENT-REPORT response but received DIMSE command 0x"
        << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
        << OFstatic_cast(unsigned int, response.CommandField));
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_INCOMING, NULL, pcid));
    delete statusDetail;
    return DIMSE_BADCOMMANDTYPE;
  }
  if (statusDetail != NULL)
  {
    DCMNET_DEBUG("Response has status detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
    delete statusDetail;
  }

  // Set return value
  T_DIMSE_N_EventReportRSP &eventReportRsp = response.msg.NEventReportRSP;
  rspStatusCode = eventReportRsp.DimseStatus;

  // Check whether there is a dataset to be received
  if (eventReportRsp.DataSetType == DIMSE_DATASET_PRESENT)
  {
    // this should never happen
    DcmDataset *tempDataset = NULL;
    T_ASC_PresentationContextID tempID;
    // Receive dataset
    cond = receiveDIMSEDataset(&tempID, &tempDataset);
    if (cond.good())
    {
      DCMNET_WARN("Received unexpected dataset after N-EVENT-REPORT response, ignoring");
      delete tempDataset;
    } else {
      DCMNET_ERROR("Failed receiving unexpected dataset after N-EVENT-REPORT response: "
        << DimseCondition::dump(tempStr, cond));
      return DIMSE_BADDATA;
    }
  }

  // Check whether the message ID being responded to is equal to the message ID of the request
  if (eventReportRsp.MessageIDBeingRespondedTo != eventReportReq.MessageID)
  {
    DCMNET_ERROR("Received response with wrong message ID (" << eventReportRsp.MessageIDBeingRespondedTo
      << " instead of " << eventReportReq.MessageID << ")");
    return DIMSE_BADMESSAGE;
  }

  return cond;
}


Uint16 DcmSCP::checkEVENTREPORTRequest(T_DIMSE_N_EventReportRQ & /*reqMessage*/,
                                       DcmDataset * /*reqDataset*/)
{
  // we default to success
  return STATUS_Success;
}


/* ************************************************************************* */
/*                         General message handling                          */
/* ************************************************************************* */

void DcmSCP::notifySENDProgress(const unsigned long byteCount)
{
  DCMNET_TRACE("Bytes sent: " << byteCount);
}


void DcmSCP::notifyRECEIVEProgress(const unsigned long byteCount)
{
  DCMNET_TRACE("Bytes received: " << byteCount);
}


/* ************************************************************************* */
/*                            Various helpers                                */
/* ************************************************************************* */

// Sends a DIMSE command and possibly also instance data to the configured peer DICOM application
OFCondition DcmSCP::sendDIMSEMessage(const T_ASC_PresentationContextID presID,
                                     T_DIMSE_Message *message,
                                     DcmDataset *dataObject,
                                     DcmDataset *statusDetail,
                                     DcmDataset **commandSet)
{
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;
  if (message == NULL)
    return DIMSE_NULLKEY;

  OFCondition cond;
  /* call the corresponding DIMSE function to sent the message */
  if (m_cfg->getProgressNotificationMode())
  {
    cond = DIMSE_sendMessageUsingMemoryData(m_assoc, presID, message, statusDetail, dataObject,
                                            callbackSENDProgress, this /*callbackData*/, commandSet);
  } else {
    cond = DIMSE_sendMessageUsingMemoryData(m_assoc, presID, message, statusDetail, dataObject,
                                            NULL /*callback*/, NULL /*callbackData*/, commandSet);
  }
  return cond;
}

// ----------------------------------------------------------------------------

// Receive DIMSE command (excluding dataset!) over the currently open association
OFCondition DcmSCP::receiveDIMSECommand(T_ASC_PresentationContextID *presID,
                                        T_DIMSE_Message *message,
                                        DcmDataset **statusDetail,
                                        DcmDataset **commandSet,
                                        const Uint32 timeout)
{
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  if (timeout > 0)
  {
    /* call the corresponding DIMSE function to receive the command (use specified timeout) */
    cond = DIMSE_receiveCommand(m_assoc, DIMSE_NONBLOCKING, timeout, presID,
                                message, statusDetail, commandSet);
  } else {
    /* call the corresponding DIMSE function to receive the command (use default timeout) */
    cond = DIMSE_receiveCommand(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(), presID,
                                message, statusDetail, commandSet);
  }
  return cond;
}

// ----------------------------------------------------------------------------

// Receives one dataset (of instance data) via network from another DICOM application in memory
OFCondition DcmSCP::receiveDIMSEDataset(T_ASC_PresentationContextID *presID,
                                        DcmDataset **dataObject)
{
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  /* call the corresponding DIMSE function to receive the dataset */
  if (m_cfg->getProgressNotificationMode())
  {
    cond = DIMSE_receiveDataSetInMemory(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
                                        presID, dataObject, callbackRECEIVEProgress, this /*callbackData*/);
  } else {
    cond = DIMSE_receiveDataSetInMemory(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
                                        presID, dataObject, NULL /*callback*/, NULL /*callbackData*/);
  }

  if (cond.good())
  {
    DCMNET_DEBUG("Received dataset on presentation context " << OFstatic_cast(unsigned int, *presID));
  } else {
    OFString tempStr;
    DCMNET_ERROR("Unable to receive dataset on presentation context "
      << OFstatic_cast(unsigned int, *presID) << ": " << DimseCondition::dump(tempStr, cond));
  }
  return cond;
}

// ----------------------------------------------------------------------------

// Receives one C-STORE request dataset via network from another DICOM application
// (and store it directly to file)
OFCondition DcmSCP::receiveSTORERequestDataset(T_ASC_PresentationContextID *presID,
                                               T_DIMSE_C_StoreRQ &reqMessage,
                                               const OFString &filename)
{
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;
  if (filename.empty())
    return EC_InvalidFilename;

  OFString tempStr;
  DcmOutputFileStream *filestream = NULL;
  // Receive dataset over the network and write it directly to a file
  OFCondition cond = DIMSE_createFilestream(filename.c_str(), &reqMessage, m_assoc, *presID,
                                            OFTrue /*writeMetaheader*/, &filestream);
  if (cond.good())
  {
    if (m_cfg->getProgressNotificationMode())
    {
      cond = DIMSE_receiveDataSetInFile(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
                                        presID, filestream, callbackRECEIVEProgress, this /*callbackData*/);
    } else {
      cond = DIMSE_receiveDataSetInFile(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
                                        presID, filestream, NULL /*callback*/, NULL /*callbackData*/);
    }
    delete filestream;
    if (cond.good())
    {
      DCMNET_DEBUG("Received dataset on presentation context " << OFstatic_cast(unsigned int, *presID)
        << " and stored it directly to file");
    } else {
      DCMNET_ERROR("Unable to receive dataset on presentation context "
        << OFstatic_cast(unsigned int, *presID) << ": " << DimseCondition::dump(tempStr, cond));
      // Delete created file in case of error
      OFStandard::deleteFile(filename);
    }

  } else {

    DCMNET_ERROR("Unable to receive dataset on presentation context "
      << OFstatic_cast(unsigned int, *presID) << ": " << DimseCondition::dump(tempStr, cond));
    // Could not create the filestream, so ignore the dataset
    DIC_UL bytesRead = 0;
    DIC_UL pdvCount = 0;
    DCMNET_DEBUG("Ignoring incoming dataset and returning an error status to the SCU");
    cond = DIMSE_ignoreDataSet(m_assoc, m_cfg->getDIMSEBlockingMode(), m_cfg->getDIMSETimeout(),
                               &bytesRead, &pdvCount);
    if (cond.good())
    {
      tempStr = "Cannot create file: " + filename;
      cond = makeDcmnetCondition(DIMSEC_OUTOFRESOURCES, OF_error, tempStr.c_str());
    }
  }
  return cond;
}

// ----------------------------------------------------------------------------

OFBool DcmSCP::addStatusDetail(DcmDataset **statusDetail,
                               const DcmElement *elem)
{
  DCMNET_TRACE("Add element to status detail");
  // If no element was passed, return to the caller.
  if( elem == NULL )
    return OFFalse;

  DcmAttributeTag *at;
  DcmLongString *lo;

  // Create the container object if necessary
  if( *statusDetail == NULL )
    *statusDetail = new DcmDataset();

  if (statusDetail == NULL)
  {
    DCMNET_ERROR("Cannot create status detail object, memory exhausted!");
    return OFFalse;
  }

  // Determine the element's data type
  DcmVR vr( elem->ident() );

  // Depending on the element's identification, insert different
  // types of objects into the container.
  switch( elem->ident() )
  {
    case EVR_LO:
      lo = new DcmLongString( *((DcmLongString*)elem) );
      if( lo->getLength() > vr.getMaxValueLength() )
      {
        DCMNET_WARN("Value inside given LO attribute too large for status detail (max " << OFstatic_cast(Uint32, vr.getMaxValueLength()) << ") for " << vr.getVRName());
      }
      (*statusDetail)->insert( lo, OFTrue /*replaceOld*/ );
      // Print debug information
      {
        OFOStringStream oss;
        lo->print(oss);
        OFSTRINGSTREAM_GETSTR(oss, strtemp);
        DCMNET_DEBUG("Adding LO status detail information: " << strtemp);
        OFSTRINGSTREAM_FREESTR(tmpString)
      }
      break;
    case EVR_AT:
      at = new DcmAttributeTag( *((DcmAttributeTag*)elem) );
      if( at->getLength() > vr.getMaxValueLength() )
      {
        DCMNET_WARN("Value inside given AT attribute too large for status detail (max " << OFstatic_cast(Uint32, vr.getMaxValueLength()) << ") for " << vr.getVRName());
      }
      (*statusDetail)->insert( at, OFTrue /*replaceOld*/ );
      // Print debug information
      {
        OFOStringStream oss;
        at->print(oss);
        OFSTRINGSTREAM_GETSTR(oss, strtemp);
        DCMNET_DEBUG("Adding AT status detail information: " << strtemp);
        OFSTRINGSTREAM_FREESTR(tmpString)
      }
      break;
    default:  // other status detail is not supported
      DCMNET_ERROR("Cannot add status detail, unsupported detail attribute type: " << vr.getVRName());
      return OFFalse;
      break;
  }
  return OFTrue;
}

// ----------------------------------------------------------------------------

void DcmSCP::forceAssociationRefuse(const OFBool doRefuse)
{
  m_cfg->forceAssociationRefuse(doRefuse);
}

// ----------------------------------------------------------------------------

void DcmSCP::setMaxReceivePDULength(const Uint32 maxRecPDU)
{
  m_cfg->setMaxReceivePDULength(maxRecPDU);
}

// ----------------------------------------------------------------------------

OFCondition DcmSCP::addPresentationContext(const OFString &abstractSyntax,
                                           const OFList<OFString> xferSyntaxes,
                                           const T_ASC_SC_ROLE role,
                                           const OFString &profile)
{
  return m_cfg->addPresentationContext(abstractSyntax,xferSyntaxes,role,profile);
}

// ----------------------------------------------------------------------------

void DcmSCP::setPort(const Uint16 port)
{
  m_cfg->setPort(port);
}

// ----------------------------------------------------------------------------

void DcmSCP::setAETitle(const OFString &aetitle)
{
  m_cfg->setAETitle(aetitle);
}

// ----------------------------------------------------------------------------

void DcmSCP::setRespondWithCalledAETitle(const OFBool useCalled)
{
  m_cfg->setRespondWithCalledAETitle(useCalled);
}

// ----------------------------------------------------------------------------

OFCondition DcmSCP::loadAssociationCfgFile(const OFString &assocFile)
{
  return m_cfg->loadAssociationCfgFile(assocFile);
}

// ----------------------------------------------------------------------------

OFCondition DcmSCP::setAndCheckAssociationProfile(const OFString &profileName)
{
  return m_cfg->setAndCheckAssociationProfile(profileName);
}

// ----------------------------------------------------------------------------

void DcmSCP::setConnectionBlockingMode(const DUL_BLOCKOPTIONS blockingMode)
{
  m_cfg->setConnectionBlockingMode(blockingMode);
}

// ----------------------------------------------------------------------------

void DcmSCP::setDIMSEBlockingMode(const T_DIMSE_BlockingMode blockingMode)
{
  m_cfg->setDIMSEBlockingMode(blockingMode);
}

// ----------------------------------------------------------------------------

void DcmSCP::setDIMSETimeout(const Uint32 dimseTimeout)
{
  m_cfg->setDIMSETimeout(dimseTimeout);
}

// ----------------------------------------------------------------------------

void DcmSCP::setACSETimeout(const Uint32 acseTimeout)
{
  m_cfg->setACSETimeout(acseTimeout);
}

// ----------------------------------------------------------------------------

void DcmSCP::setConnectionTimeout(const Uint32 timeout)
{
  m_cfg->setConnectionTimeout(timeout);
}

// ----------------------------------------------------------------------------

void DcmSCP::setVerbosePCMode(const OFBool mode)
{
  m_cfg->setVerbosePCMode(mode);
}

// ----------------------------------------------------------------------------

void DcmSCP::setHostLookupEnabled(const OFBool mode)
{
  m_cfg->setHostLookupEnabled(mode);
}

// ----------------------------------------------------------------------------

void DcmSCP::setProgressNotificationMode(const OFBool mode)
{
  m_cfg->setProgressNotificationMode(mode);
}

// ----------------------------------------------------------------------------

/* Get methods for SCP settings and current association information */

OFBool DcmSCP::getRefuseAssociation() const
{
  return m_cfg->getRefuseAssociation();
}

// ----------------------------------------------------------------------------

Uint32 DcmSCP::getMaxReceivePDULength() const
{
  return m_cfg->getMaxReceivePDULength();
}

// ----------------------------------------------------------------------------

Uint16 DcmSCP::getPort() const
{
  return m_cfg->getPort();
}

// ----------------------------------------------------------------------------

const OFString &DcmSCP::getAETitle() const
{
  return m_cfg->getAETitle();
}

// ----------------------------------------------------------------------------

OFBool DcmSCP::getRespondWithCalledAETitle() const
{
  return m_cfg->getRespondWithCalledAETitle();
}

// ----------------------------------------------------------------------------

DUL_BLOCKOPTIONS DcmSCP::getConnectionBlockingMode() const
{
  return m_cfg->getConnectionBlockingMode();
}

// ----------------------------------------------------------------------------

T_DIMSE_BlockingMode DcmSCP::getDIMSEBlockingMode() const
{
  return m_cfg->getDIMSEBlockingMode();
}

// ----------------------------------------------------------------------------

Uint32 DcmSCP::getDIMSETimeout() const
{
  return m_cfg->getDIMSETimeout();
}

// ----------------------------------------------------------------------------

Uint32 DcmSCP::getConnnectionTimeout() const
{
  return m_cfg->getConnnectionTimeout();
}

// ----------------------------------------------------------------------------

Uint32 DcmSCP::getACSETimeout() const
{
  return m_cfg->getACSETimeout();
}

// ----------------------------------------------------------------------------

OFBool DcmSCP::getVerbosePCMode() const
{
  return m_cfg->getVerbosePCMode();
}

// ----------------------------------------------------------------------------

OFBool DcmSCP::getHostLookupEnabled() const
{
  return m_cfg->getHostLookupEnabled();
}

// ----------------------------------------------------------------------------

OFBool DcmSCP::getProgressNotificationMode() const
{
  return m_cfg->getProgressNotificationMode();
}

// ----------------------------------------------------------------------------

OFBool DcmSCP::isConnected() const
{
  return (m_assoc != NULL) && (m_assoc->DULassociation != NULL);
}

// ----------------------------------------------------------------------------

OFString DcmSCP::getPeerAETitle() const
{
  if (m_assoc == NULL)
    return "";
  return m_assoc->params->DULparams.callingAPTitle;
}

// ----------------------------------------------------------------------------

OFString DcmSCP::getCalledAETitle() const
{
  if (m_assoc == NULL)
    return "";
  return m_assoc->params->DULparams.calledAPTitle;
}

// ----------------------------------------------------------------------------

Uint32 DcmSCP::getPeerMaxPDULength() const
{
  if (m_assoc == NULL)
    return 0;
  return m_assoc->params->theirMaxPDUReceiveSize;
}

// ----------------------------------------------------------------------------

OFString DcmSCP::getPeerIP() const
{
  if (m_assoc == NULL)
    return "";
  return m_assoc->params->DULparams.callingPresentationAddress;
}

// ----------------------------------------------------------------------------

void DcmSCP::dropAndDestroyAssociation()
{
  if (m_assoc)
  {
    notifyAssociationTermination();
    ASC_dropSCPAssociation( m_assoc );
    ASC_destroyAssociation( &m_assoc );
  }
}


/* ************************************************************************** */
/*                            Notify functions                                */
/* ************************************************************************** */


void DcmSCP::notifyAssociationRequest(const T_ASC_Parameters &params,
                                      DcmSCPActionType & /* desiredAction */)
{
  // Dump some information if required
  DCMNET_INFO("Association Received " << params.DULparams.callingPresentationAddress << ": "
                                      << params.DULparams.callingAPTitle << " -> "
                                      << params.DULparams.calledAPTitle);

    // Dump more information if required
  OFString tempStr;
  if (m_cfg->getVerbosePCMode())
    DCMNET_INFO("Incoming Association Request:" << OFendl << ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_RQ));
  else
    DCMNET_DEBUG("Incoming Association Request:" << OFendl << ASC_dumpParameters(tempStr, m_assoc->params, ASC_ASSOC_RQ));
}

// ----------------------------------------------------------------------------

OFBool DcmSCP::checkCalledAETitleAccepted(const OFString& calledAETitle)
{
  return OFTrue;
}

// ----------------------------------------------------------------------------

OFBool DcmSCP::checkCallingAETitleAccepted(const OFString& callingAETitle)
{
  return OFTrue;
}

// ----------------------------------------------------------------------------

OFBool DcmSCP::checkCallingHostAccepted(const OFString& hostOrIP)
{
  return OFTrue;
}

// ----------------------------------------------------------------------------

void DcmSCP::notifyAssociationAcknowledge()
{
  DCMNET_DEBUG("DcmSCP: Association Acknowledged");
}

// ----------------------------------------------------------------------------

void DcmSCP::notifyReleaseRequest()
{
  DCMNET_INFO("Received Association Release Request");
}

// ----------------------------------------------------------------------------

void DcmSCP::notifyAbortRequest()
{
  DCMNET_INFO("Received Association Abort Request");
}

// ----------------------------------------------------------------------------

void DcmSCP::notifyAssociationTermination()
{
  DCMNET_DEBUG("DcmSCP: Association Terminated");
}

// ----------------------------------------------------------------------------

void DcmSCP::notifyDIMSEError(const OFCondition &cond)
{
  OFString tempStr;
  DCMNET_DEBUG("DIMSE Error, detail (if available): " << DimseCondition::dump(tempStr, cond));
}

// ----------------------------------------------------------------------------

OFBool DcmSCP::stopAfterCurrentAssociation()
{
  return OFFalse;
}


/* ************************************************************************* */
/*                            Callback functions                             */
/* ************************************************************************* */

void DcmSCP::callbackSENDProgress(void *callbackContext,
                                  const unsigned long byteCount)
{
  if (callbackContext != NULL)
    OFreinterpret_cast(DcmSCP *, callbackContext)->notifySENDProgress(byteCount);
}


void DcmSCP::callbackRECEIVEProgress(void *callbackContext,
                                     const unsigned long byteCount)
{
  if (callbackContext != NULL)
    OFreinterpret_cast(DcmSCP *, callbackContext)->notifyRECEIVEProgress(byteCount);
}


/* ************************************************************************* */
/*                         Static helper functions                           */
/* ************************************************************************* */

OFBool DcmSCP::getPresentationContextInfo(const T_ASC_Association *assoc,
                                          const Uint8 presID,
                                          DcmPresentationContextInfo &info)
{
  if (assoc != NULL)
  {
    DUL_PRESENTATIONCONTEXT *pc = findPresentationContextID(assoc->params->DULparams.acceptedPresentationContext, presID);
    if (pc != NULL)
    {
      info.abstractSyntax = pc->abstractSyntax;
      info.acceptedTransferSyntax = pc->acceptedTransferSyntax;
      info.presentationContextID = pc->presentationContextID;
      info.proposedSCRole = pc->proposedSCRole;
      info.acceptedSCRole = pc->acceptedSCRole;
      return OFTrue;
    }
  }
  return OFFalse;
}
