/*
 *
 *  Copyright (C) 2009-2010, OFFIS
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
 *  Module:  dcmnet
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Base class for Service Class Providers (SCPs)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-06-17 17:08:05 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/scp.h"
#include "dcmtk/dcmnet/diutil.h"


static DUL_PRESENTATIONCONTEXT* findPresentationContextID(LST_HEAD * head,
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

static void getPresentationContextInfo(const T_ASC_Association *assoc,
                                       const Uint8 presID,
                                       DcmPresentationContextInfo& info)
{
  if (assoc == NULL)
    return;

  DUL_PRESENTATIONCONTEXT *pc = findPresentationContextID(assoc->params->DULparams.acceptedPresentationContext, presID);
  if (pc != NULL)
  {
    info.abstractSyntax = pc->abstractSyntax;
    info.acceptedTransferSyntax = pc->acceptedTransferSyntax;
    info.presentationContextID = pc->presentationContextID;
    info.proposedSCRole = pc->proposedSCRole;
    info.acceptedSCRole = pc->acceptedSCRole;
  }
  return;
}

// ----------------------------------------------------------------------------

DcmSCP::DcmSCP() :
  m_assoc(NULL),
  m_assocConfig(NULL),
  m_assocCfgProfileName("DEFAULT"),
  m_port(104),
  m_aetitle("DCMTK_SCP"),
  m_refuseAssociation(OFFalse),
  m_maxReceivePDULength(ASC_DEFAULTMAXPDU),
  m_singleProcess(OFTrue),
  m_forkedChild(OFFalse),
  m_maxAssociations(1),
  m_blockMode(DIMSE_BLOCKING),
  m_DIMSETimeout(0),
  m_ACSETimeout(30),
  m_verbosePCMode(OFFalse),
  m_processTable(),
  m_respondWithCalledAETitle(OFTrue),
  m_pcInfo()
{
  // make sure not to let dcmdata remove tailing blank padding or perform other
  // manipulations. We want to see the real data.
  dcmEnableAutomaticInputDataCorrection.set( OFFalse );
  if (!m_forkedChild)
    DCMNET_WARN("(notice: dcmdata auto correction disabled.)");

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
#ifdef HAVE_WINSOCK_H
  WSACleanup();
#endif

  // Clean up memory of association configuration
  if (m_assocConfig)
  {
    delete m_assocConfig;
    m_assocConfig = NULL;
  }
  // If there is an open association, drop it and free memory (just to be sure...)
  if (m_assoc)
  {
    dropAndDestroyAssociation();
  }
}

// ----------------------------------------------------------------------------

OFCondition DcmSCP::enableMultiProcessMode(int argc,
                                           char *argv[])
{
  if ( m_assoc != NULL )
    return EC_IllegalCall;
#ifdef _WIN32
  if ( (argc == 0) || (argv == NULL) )
    return EC_IllegalCall;
  m_cmd_argc = argc;
  m_cmd_argv = argv;
#endif
  m_singleProcess = OFFalse;
  return EC_Normal;
}

// ----------------------------------------------------------------------------

#ifdef _WIN32
OFCondition DcmSCP::markAsForkedChild()
{
  /* Marking as a forked child must be done before actually starting
     any network initializiation -> Be sure there is no association so far.
   */
  if ( m_singleProcess || (m_assoc != NULL) )
    return EC_IllegalCall;
  m_forkedChild = OFTrue;
  return EC_Normal;
}
#endif

// ----------------------------------------------------------------------------

OFCondition DcmSCP::listen()
{
  OFCondition cond = EC_Normal;
  // Make sure data dictionary is loaded.
  if( !dcmDataDict.isDictionaryLoaded() )
    DCMDATA_WARN("no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE);

#ifdef HAVE_GETEUID
  // If port is privileged we must be as well.
  if( m_port < 1024 && geteuid() != 0 )
  {
    DCMNET_ERROR("No privileges to open this network port (choose port below 1024?)");
    return EC_IllegalCall;
  }
#endif

#ifdef _WIN32
  /* if this process was started by CreateProcess, m_forkedChild is set */
  if (m_forkedChild)
  {
    /* tell dcmnet DUL about child process status, too */
    DUL_markProcessAsForkedChild();
    char buf[256];
    DWORD bytesRead = 0;
    HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);

    // read socket handle number from stdin, i.e. the anonymous pipe
    // to which our parent process has written the handle number.
    if (ReadFile(hStdIn, buf, sizeof(buf), &bytesRead, NULL))
    {
      // make sure buffer is zero terminated
      buf[bytesRead] = '\0';
        dcmExternalSocketHandle.set(atoi(buf));
    }
    else
    {
      exit(1);
    }
  }
  else
  {
    // parent process
    if (!m_singleProcess)
    {
      DUL_requestForkOnTransportConnectionReceipt(m_cmd_argc, m_cmd_argv);
    }
  }
#endif
    // Initialize network, i.e. create an instance of T_ASC_Network*.
  T_ASC_Network *m_net = NULL;
  cond = ASC_initializeNetwork( NET_ACCEPTOR, OFstatic_cast(int, m_port), m_ACSETimeout, &m_net );
  if( cond.bad() ) return( cond );

#if defined(HAVE_SETUID) && defined(HAVE_GETUID)
  // Return to normal uid so that we can't do too much damage in case
  // things go very wrong. Only works if the program is setuid root,
  // and run by another user. Running as root user may be
  // potentially disasterous if this program screws up badly.
  setuid( getuid() );
#endif

  // If we get to this point, the entire initialization process has been completed
  // successfully. Now, we want to start handling all incoming requests. Since
  // this activity is supposed to represent a server process, we do not want to
  // terminate this activity. Hence, create an endless while-loop.
  while( cond.good() )
  {
    // Wait for an association and handle the requests of
    // the calling applications correspondingly.
    cond = waitForAssociation(m_net);

    // Clean up any child processes if the execution is not limited to a single process.
    // (On Windows platform, childs are not handled via the process table,
    // so there's no need to clean up children)
#ifdef HAVE_FORK
    if( !m_singleProcess )
      cleanChildren();
#elif _WIN32
    // if running in multi-process mode, always terminate child after one association
    // for unix, this is done in WaitForAssociation() with exit()
    if (DUL_processIsForkedChild()) return cond;
#endif
  }
  // Drop the network, i.e. free memory of T_ASC_Network* structure. This call
  // is the counterpart of ASC_initializeNetwork(...) which was called above.
  cond = ASC_dropNetwork( &m_net );
  m_net = NULL;
  if( cond.bad() ) return( cond );

  // return ok
  return( EC_Normal );
}

// ----------------------------------------------------------------------------

void DcmSCP::refuseAssociation(DcmRefuseReasonType reason)
{
  if (m_assoc == NULL)
  {
    DCMNET_WARN("DcmSCP::refuseAssociation() called but actually no association running, ignoring");
    return;
  }

  T_ASC_RejectParameters rej;

  // Dump some information if required.
  if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    switch( reason )
    {
      case DCMSCP_TOO_MANY_ASSOCIATIONS: DCMNET_ERROR("Refusing Association (too many associations)"); break;
      case DCMSCP_CANNOT_FORK:           DCMNET_ERROR("Refusing Association (cannot fork)"); break;
      case DCMSCP_BAD_APP_CONTEXT:       DCMNET_ERROR("Refusing Association (bad application context)"); break;
      case DCMSCP_BAD_AE_SERVICE:        DCMNET_ERROR("Refusing Association (bad application entity service)"); break;
      case DCMSCP_FORCED:                DCMNET_WARN ("Refusing Association (forced via command line)"); break;
      case DCMSCP_NO_IC_UID:             DCMNET_ERROR("Refusing Association (no implementation class UID provided)"); break;
      case DCMSCP_NO_PRES_CONTEXTS:      DCMNET_ERROR("Refusing Association (no acceptable presentation contexts)"); break;
      case DCMSCP_INTERNAL_ERROR:        DCMNET_ERROR("Refusing Association (internal error)"); break;
      default:                           DCMNET_ERROR("Refusing Association (unknown reason)"); break;
    }
  }

  // Set some values in the reject message depending on the reason.
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
    case DCMSCP_BAD_APP_CONTEXT:
      rej.result = ASC_RESULT_REJECTEDTRANSIENT;
      rej.source = ASC_SOURCE_SERVICEUSER;
      rej.reason = ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED;
      break;
    case DCMSCP_BAD_AE_SERVICE:
      rej.result = ASC_RESULT_REJECTEDPERMANENT;
      rej.source = ASC_SOURCE_SERVICEUSER;
      rej.reason = ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED;
      break;
    case DCMSCP_FORCED:
    case DCMSCP_NO_IC_UID:
    default:
      rej.result = ASC_RESULT_REJECTEDPERMANENT;
      rej.source = ASC_SOURCE_SERVICEUSER;
      rej.reason = ASC_REASON_SU_NOREASON;
      break;
  }

  // Reject the association request.
  ASC_rejectAssociation( m_assoc, &rej );

  // Drop and destroy the association.
  dropAndDestroyAssociation();}

// ----------------------------------------------------------------------------

OFCondition DcmSCP::waitForAssociation(T_ASC_Network* network)
{
  if (network == NULL) return ASC_NULLKEY; // TODO specific error
  if (m_assoc != NULL) return EC_IllegalCall;
  char buf[BUFSIZ];
  Uint16 timeout;

  // Depending on if the execution is limited to one single process
  // or not we need to set the timeout value correspondingly.
  // for WIN32, child processes cannot be counted (always 0) -> timeout=1000
  if( m_singleProcess )
    timeout = 1000;
  else
  {
    if( m_processTable.size() > 0 )
      timeout = 1;
    else
      timeout = 1000;
  }

  // Listen to a socket for timeout seconds and wait for an association request
  OFCondition cond = ASC_receiveAssociation( network, &m_assoc, m_maxReceivePDULength, NULL, NULL, OFFalse, DUL_NOBLOCK, OFstatic_cast(int, timeout) );

  // just return, if timeout occured (DUL_NOASSOCIATIONREQUEST)
  // or (WIN32) if dcmnet has started a child for us, to handle this
  // association (signaled by "DULC_FORKEDCHILD") -> return to "event loop"
  if ( ( cond.code() == DULC_FORKEDCHILD ) || ( cond == DUL_NOASSOCIATIONREQUEST ) )
    return EC_Normal;

  // if error occurs and we're not in single process mode, close association and return
  if( cond.bad() && !m_singleProcess )
  {
    dropAndDestroyAssociation();
    return EC_Normal;
  }
  // call notifier function
  DcmSCPActionType desiredAction = DCMSCP_ACTION_UNDEFINED;
  notifyAssociationRequest(*m_assoc->params, desiredAction);
  if (desiredAction != DCMSCP_ACTION_UNDEFINED)
  {
    if (desiredAction == DCMSCP_ACTION_REFUSE_ASSOCIATION)
    {
        refuseAssociation( DCMSCP_INTERNAL_ERROR );
        if( !m_singleProcess )
        {
          dropAndDestroyAssociation();
        }
        return( EC_Normal );
    }
    else desiredAction = DCMSCP_ACTION_UNDEFINED; // reset for later use
  }

  // Now we have to figure out if we might have to refuse the association request.
  // This is the case if at least one of five conditions is met:

  // Condition 1: if option "--refuse" is set we want to refuse the association request.
  if( m_refuseAssociation )
  {
    refuseAssociation( DCMSCP_FORCED );
    if( !m_singleProcess )
    {
      dropAndDestroyAssociation();
    }
    return( EC_Normal );
  }

  // Condition 2: determine the application context name. If an error occurred or if the
  // application context name is not supported we want to refuse the association request.
  cond = ASC_getApplicationContextName( m_assoc->params, buf );
  if( cond.bad() || strcmp( buf, DICOM_STDAPPLICATIONCONTEXT ) != 0 )
  {
    refuseAssociation( DCMSCP_BAD_APP_CONTEXT );
    if( !m_singleProcess )
    {
      dropAndDestroyAssociation();
    }
    return( EC_Normal );
  }

  // Condition 3: if there are too many concurrent associations
  // we want to refuse the association request
  if( m_processTable.size() >= m_maxAssociations )
  {
    refuseAssociation( DCMSCP_TOO_MANY_ASSOCIATIONS );
    if( !m_singleProcess )
    {
      dropAndDestroyAssociation();
    }
    return( EC_Normal );
  }

  // Condition 4: if the called application entity title is not supported
  // whithin the data source we want to refuse the association request

  if(!calledAETitleAccepted(m_assoc->params->DULparams.callingAPTitle,
                            m_assoc->params->DULparams.calledAPTitle) )
  {
    refuseAssociation( DCMSCP_BAD_AE_SERVICE );
    if( !m_singleProcess )
    {
      dropAndDestroyAssociation();
    }
    return( EC_Normal );
  }

  /* If we get to this point the association shall be negotiated.
     Thus, for every presentation context it is checked whether
     it can be accepdted. However, this is only a "dry" run, i.e. there
     is not yet sent a response message to the SCU
   */
  cond = negotiateAssociation();
  if( cond.bad() )
  {
    if( !m_singleProcess )
    {
      dropAndDestroyAssociation();
    }
    return( EC_Normal );
  }

  // Reject association if no presentation context was negotiated
  if( ASC_countAcceptedPresentationContexts( m_assoc->params ) == 0 )
  {
    DCMNET_INFO("No Acceptable Presentation Contexts");
    refuseAssociation( DCMSCP_NO_PRES_CONTEXTS );
    if( !m_singleProcess )
    {
      dropAndDestroyAssociation();
    }
    return( EC_Normal );
  }

  // If the negotiation was successful, accept the association request.
  cond = ASC_acknowledgeAssociation( m_assoc );
  if( cond.bad() )
  {
    if( !m_singleProcess )
    {
      dropAndDestroyAssociation();
    }
    return( EC_Normal );
  }
  notifyAssociationAcknowledge();

  // Dump some debug information
  OFString tmpstr;
  DCMNET_INFO("Association Acknowledged (Max Send PDV: " << OFstatic_cast(Uint32, m_assoc->sendPDVLength) << ")");
  if (m_verbosePCMode)
    DCMNET_INFO(ASC_dumpParameters(tmpstr, m_assoc->params, ASC_ASSOC_AC));
  else
    DCMNET_DEBUG(ASC_dumpParameters(tmpstr, m_assoc->params, ASC_ASSOC_AC));

  // Depending on if this execution shall be limited to one process or not, spawn a sub-
  // process to handle the association or don't. (Note: For Windows dcmnet is handling
  // the creation for a new subprocess, so we can call HandleAssociation directly, too)
  if( m_singleProcess || m_forkedChild )
  {
    // Go ahead and handle the association (i.e. handle the callers requests) in this process.
    handleAssociation();
  }
#ifdef HAVE_FORK
  else
  {
    DCMNET_DEBUG("DcmSCP: Forking new child to handle connection");
    // Spawn a sub-process to handle the association (i.e. handle the callers requests)
    int pid = OFstatic_cast(int, fork());
    if( pid < 0 )
    {
      refuseAssociation(DCMSCP_CANNOT_FORK );
      if( !m_singleProcess )
      {
        dropAndDestroyAssociation();
      }
      return( EC_Normal );
    }
    else if( pid > 0 )
    {
      // Fork returns a positive process id if this is the parent process.
      // If this is the case, remeber the process in a table and go ahead.
      addProcessToTable( pid );

      // the child will handle the association, we can drop it
      dropAndDestroyAssociation();
    }
    else
    {
      // If the process id is not positive, this must be the child process.
      // We want to handle the association, i.e. the callers requests.
      handleAssociation( );

      // When everything is finished, terminate the child process.
      exit(0);
    }
  }
#endif // HAVE_FORK

  return( EC_Normal );
}

// ----------------------------------------------------------------------------

OFCondition DcmSCP::negotiateAssociation()
{
  // check whether there is something to negotiate...
  if (m_assoc == NULL)
    return ASC_NULLKEY;
  if (m_assocConfig == NULL)
  {
    DCMNET_ERROR("Cannot negotiate association: Missing association configuration");
    return EC_IllegalCall;
  }

  /* set presentation contexts as defined in config file */
  OFCondition result;
  if (m_assocConfig)
    result = m_assocConfig->evaluateAssociationParameters(m_assocCfgProfileName.c_str(), *m_assoc);
  else
    return EC_IllegalCall;
  if (result.bad())
  {
    OFString tmpstr;
    DCMNET_ERROR(DimseCondition::dump(tmpstr, result));
  }
  return result;
}

// ----------------------------------------------------------------------------

void DcmSCP::handleAssociation( )
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
  T_DIMSE_Message msg;
  T_ASC_PresentationContextID presID;

  // start a loop to be able to receive more than one DIMSE command
  while( cond.good() )
  {
    // receive a DIMSE command over the network
    cond = DIMSE_receiveCommand( m_assoc, DIMSE_BLOCKING, 0, &presID, &msg, NULL );

    // check if peer did release or abort, or if we have a valid message
    if( cond.good() )
    {
      DcmPresentationContextInfo pcInfo;
      getPresentationContextInfo(m_assoc, presID, pcInfo);
      cond = handleIncomingCommand(&msg, pcInfo);
    }
  }
  // Clean up on association termination.
  if( cond == DUL_PEERREQUESTEDRELEASE )
  {
    notifyReleaseRequest();
    ASC_acknowledgeRelease(m_assoc);
    ASC_dropSCPAssociation(m_assoc);
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

  // Dump some information if required.
  DCMNET_DEBUG( "+++++++++++++++++++++++++++++" );
}

// ----------------------------------------------------------------------------

OFCondition DcmSCP::handleIncomingCommand(T_DIMSE_Message* msg,
                                          const DcmPresentationContextInfo& info)
{
  OFCondition cond;
  switch( msg->CommandField )
  {
    case DIMSE_C_ECHO_RQ:
      // Process C-ECHO-Request
      cond = handleEchoRequest( &msg->msg.CEchoRQ, info.presentationContextID );
      break;
    default:
      /* We cannot handle this kind of message.
         Note that the condition will be returned and
         that the caller is responsible to end the association if desired
       */
      DCMNET_ERROR("Cannot handle this kind of DIMSE message: ");
      OFString tmpstr;
      DCMNET_ERROR(DIMSE_dumpMessage(tmpstr, *msg, DIMSE_INCOMING));
      cond = EC_IllegalCall; // TODO specific error
      break;
  }

  // return result
  return cond;
}

// ----------------------------------------------------------------------------

OFCondition DcmSCP::handleEchoRequest(T_DIMSE_C_EchoRQ *req,
                                      T_ASC_PresentationContextID presId)
{
  // Dump debug information
  DCMNET_INFO("Received C-ECHO Request: MsgID " << OFstatic_cast(Uint16, req->MessageID));
  DCMNET_DEBUG("Sending C-ECHO Response");

  // Send an echo response
  OFCondition cond = DIMSE_sendEchoResponse( m_assoc, presId, req, STATUS_Success, NULL );
  if( cond.bad() )
    DCMNET_ERROR("Cannot send C-ECHO Response: " << cond.text());
  else
    DCMNET_DEBUG("C-ECHO successfully sent");

  // return return value
  return cond;
}

// ----------------------------------------------------------------------------

// Receive dataset over existing association.
OFCondition DcmSCP::receiveDataset(DcmDataset **dataObject,
                                   DIMSE_ProgressCallback callback,
                                   void *callbackContext)
{
  if ( (m_assoc == NULL) ) return ASC_NULLKEY;
  T_ASC_PresentationContextID presID;
  return DIMSE_receiveDataSetInMemory(m_assoc,
    m_blockMode,
    m_DIMSETimeout,
    &presID,
    dataObject,
    callback,
    callbackContext);
}

// ----------------------------------------------------------------------------

// Send response to former storage request
OFCondition DcmSCP::sendCStoreResponse(T_ASC_PresentationContextID presID,
                                       T_DIMSE_C_StoreRQ *request,
                                       T_DIMSE_C_StoreRSP *response,
                                       DcmDataset *statusDetail)
{
  return DIMSE_sendStoreResponse(m_assoc, presID, request, response, statusDetail);
}

// ----------------------------------------------------------------------------

void DcmSCP::addProcessToTable(int pid)
{
  DcmProcessSlotType *ps;

  // Allocate some memory for a new item in the list of processes.
  ps = new DcmProcessSlotType ();

  // Remember process information in the new item.
  ASC_getPresentationAddresses( m_assoc->params, ps->peerName, NULL );
  ASC_getAPTitles( m_assoc->params, ps->callingAETitle, ps->calledAETitle, NULL );
  ps->processId = pid;
  ps->startTime = time(NULL);
  ps->hasStorageAbility = OFFalse;

  // Add new item to the beginning of the list.
  m_processTable.push_back(ps);
}

// ----------------------------------------------------------------------------

void DcmSCP::removeProcessFromTable( int pid )
{
  DcmProcessSlotType *ps = NULL;

  // try to find item that corresponds to the given process id
  OFListIterator(DcmProcessSlotType*) it = m_processTable.begin();
  while ( it != m_processTable.end() )
  {
    ps = *it;
    // if process can be found, delete it from list and free memory
    if ( ps->processId == pid )
    {
      m_processTable.remove(*it);
      delete ps;
      return;
    }
    it++;
  }

  // dump a warning if process could not be found in process table
  DCMNET_WARN("Could not find and delete process with ID " << pid << " from internal process table");
}

// ----------------------------------------------------------------------------

void DcmSCP::cleanChildren()
{

#ifdef HAVE_WAITPID                                           // PLATFORMS THAT HAVE waitpid()
  int options = WNOHANG;
  int stat_loc;
  int child = 1;

  while( child > 0 )
  {
    // determine status for child processes
    child = OFstatic_cast(int, waitpid(-1, &stat_loc, options));
    if( child == 0 )
    {
      // child not yet finished
    }
    else if( child < 0 )
    {
      if( errno == ECHILD )
      {
        // no children
      }
      else
      {
        DCMNET_WARN("Wait for child failed while cleaning child process (ignored)");
      }
    }
    else if( child > 0 )
    {
      // dump some debug information
      DCMNET_DEBUG("Cleaned up after child " << child);
      // remove item from process table
      removeProcessFromTable( child );
    }
  }

#elif HAVE_WAIT3                                              // PLATFORMS THAT HAVE wait3()
#if defined(__NeXT__)
  // some systems need a union wait as argument to wait3
  union wait status;
#else
  int status;
#endif
  int options = WNOHANG;
  struct rusage rusage;
  int child = 1;
  char msg[200];

  while( child > 0 )
  {
    // determine status for child processes
    child = wait3( &status, options, &rusage );
    if( child < 0 )
    {
      if( errno == ECHILD )
      {
        // no children
      }
      else
      {
        DCMNET_WARN("Wait for child failed while cleaning child process (ignored)");
      }
    }
    else if( child > 0 )
    {
      DCMNET_DEBUG("Cleaned up after child " << child);
      // remove item from process table
      RemoveProcessFromTable( child );
    }
  }
#else                                                         // OTHER PLATFORMS
  // for other platforms without waitpid() and without wait3() we
  // don't know how to cleanup after children. Dump an error message.
  DCMNET_WARN("Don't know how to wait for child processes on this platform (ignoring)");
#endif
}


// ----------------------------------------------------------------------------

// static void addStatusDetail(DcmDataset **statusDetail,
//                             const DcmElement *elem)
//{
//  DCMNET_TRACE("Add element to status detail");
//  // If no element was passed, return to the caller.
//  if( elem == NULL )
//    return;
//
//  DcmAttributeTag *at;
//  DcmLongString *lo;
//
//  // Create the container object if necessary
//  if( *statusDetail == NULL )
//    *statusDetail = new DcmDataset();
//
//  // Determine the element's data type
//  DcmVR vr( elem->ident() );
//
//  // Depending on the element's identification, insert different
//  // types of objects into the container.
//  switch( elem->ident() )
//  {
//    case EVR_LO:
//      lo = new DcmLongString( *((DcmLongString*)elem) );
//      if( lo->getLength() > vr.getMaxValueLength() )
//      {
//        DCMNET_WARN("Value inside given LO attribute too large for status detail (max " << OFstatic_cast(Uint32, vr.getMaxValueLength()) << ") for " << vr.getVRName());
//      }
//      (*statusDetail)->insert( lo, OFTrue /*replaceOld*/ );
//      // Print debug information
//      {
//        OFString strtemp;
//        OFOStringStream oss;
//        lo->print(oss);
//        OFSTRINGSTREAM_GETSTR(oss, strtemp);
//        DCMNET_DEBUG("Adding AT status detail information: " << strtemp);
//        OFSTRINGSTREAM_FREESTR(tmpString)
//      }
//      break;
//    case EVR_AT:
//      at = new DcmAttributeTag( *((DcmAttributeTag*)elem) );
//      if( at->getLength() > vr.getMaxValueLength() )
//      {
//        DCMNET_WARN("Value inside given AT attribute too large for status detail (max " << OFstatic_cast(Uint32, vr.getMaxValueLength()) << ") for " << vr.getVRName());
//      }
//      (*statusDetail)->insert( at, OFTrue /*replaceOld*/ );
//      // Print debug information
//      {
//        OFString strtemp;
//        OFOStringStream oss;
//        at->print(oss);
//        OFSTRINGSTREAM_GETSTR(oss, strtemp);
//        DCMNET_DEBUG("Adding AT status detail information: " << strtemp);
//        OFSTRINGSTREAM_FREESTR(tmpString)
//      }
//      break;
//    default:  // other status detail is not supported
//      DCMNET_ERROR("Cannot add status detail, unsupported detail attribute type: " << vr.getVRName());
//      break;
//  }
//}


// ----------------------------------------------------------------------------

void DcmSCP::forceAssociationRefuse(const OFBool doRefuse)
{
  m_refuseAssociation = doRefuse;
}

// ----------------------------------------------------------------------------

void DcmSCP::setMaxReceivePDULength(const Uint32 maxRecPDU)
{
  m_maxReceivePDULength = maxRecPDU;
}

void DcmSCP::setPort(const Uint16& port)
{
  m_port = port;
}

// ----------------------------------------------------------------------------

void DcmSCP::setAETitle(const OFString& aetitle)
{
  m_aetitle = aetitle;
}

// ----------------------------------------------------------------------------

void DcmSCP::setRespondWithCalledAETitle(const OFBool useCalled)
{
  m_respondWithCalledAETitle = useCalled;
}

// ----------------------------------------------------------------------------


void DcmSCP::setMaxAssociations(const Uint16 maxAssocs)
{
  m_maxAssociations = maxAssocs;
}

// ----------------------------------------------------------------------------

void DcmSCP::setDIMSEBlockingMode(const T_DIMSE_BlockingMode blockingMode)
{
  m_blockMode = blockingMode;
}

// ----------------------------------------------------------------------------

void DcmSCP::setDIMSETimeout(const Uint32 dimseTimeout)
{
  m_DIMSETimeout = dimseTimeout;
}

// ----------------------------------------------------------------------------

void DcmSCP::setACSETimeout(const Uint32 acseTimeout)
{
  m_ACSETimeout = acseTimeout;
}

// ----------------------------------------------------------------------------

void DcmSCP::setVerbosePCMode(const OFBool mode)
{
  m_verbosePCMode = mode;
}

// ----------------------------------------------------------------------------

/* Get methods for SCP settings and current association information */

OFBool DcmSCP::getRefuseAssociation() const
{
  return m_refuseAssociation;
}

// ----------------------------------------------------------------------------

Uint32 DcmSCP::getMaxReceivePDULength() const
{
  return m_maxReceivePDULength;
}

// ----------------------------------------------------------------------------

Uint16 DcmSCP::getPort() const
{
  return m_port;
}

// ----------------------------------------------------------------------------

const OFString &DcmSCP::getAETitle() const
{
  return m_aetitle;
}

// ----------------------------------------------------------------------------

OFBool DcmSCP::getRespondWithCalledAETitle() const
{
  return m_respondWithCalledAETitle;
}

// ----------------------------------------------------------------------------

OFBool DcmSCP::getSingleProcess() const
{
  return m_singleProcess;
}

// ----------------------------------------------------------------------------

Uint16 DcmSCP::getMaxAssociations() const
{
  return m_maxAssociations;
}

// ----------------------------------------------------------------------------

Uint16 DcmSCP::numAssociations() const
{
  return m_processTable.size(); // TODO: +1 for "this" process?
}

// ----------------------------------------------------------------------------

T_DIMSE_BlockingMode DcmSCP::getDIMSEBlockingMode() const
{
  return m_blockMode;
}

// ----------------------------------------------------------------------------

Uint32 DcmSCP::getDIMSETimeout () const
{
  return m_DIMSETimeout;
}

// ----------------------------------------------------------------------------

Uint32 DcmSCP::getACSETimeout () const
{
  return m_ACSETimeout;
}

// ----------------------------------------------------------------------------

OFBool DcmSCP::getVerbosePCMode() const
{
  return m_verbosePCMode;
}

// ----------------------------------------------------------------------------

OFBool DcmSCP::isConnected() const
{
  return (m_assoc != NULL);
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

OFBool DcmSCP::calledAETitleAccepted(const OFString& callingAE,
                                     const OFString& calledAE)
{
  if (m_respondWithCalledAETitle)  // default is to use the called AE title
  {
    return OFTrue;
  }
  if (calledAE != m_aetitle)
    return OFFalse;
  else
    return OFTrue;
}

// ----------------------------------------------------------------------------

// Reads association configuration from config file
OFCondition DcmSCP::loadAssociationCfgFile(const OFString& assocFile)
{
  // delete any previous association configuration
  if (m_assocConfig)
    delete m_assocConfig;

  OFString profileName;
  m_assocConfig = new DcmAssociationConfiguration();
  DCMNET_DEBUG("Loading SCP configuration file...");
  OFCondition result = DcmAssociationConfigurationFile::initialize(*m_assocConfig, assocFile.c_str());
  if (result.bad())
  {
    DCMNET_ERROR("DcmSCP: Unable to parse association configuration file " << assocFile << ": " << result.text());
    delete m_assocConfig;
    m_assocConfig = NULL;
  }
  return result;
}

// ----------------------------------------------------------------------------

OFCondition DcmSCP::setAndCheckAssociationProfile(const OFString& profileName)
{
  if (m_assocConfig == NULL)
    return EC_IllegalCall;
  if (profileName.empty())
    return EC_IllegalParameter;

  DCMNET_TRACE("Setting and checking SCP association profile");
  OFString mangledName;
  OFCondition result;

  /* perform name mangling for config file key */
  const char *c = profileName.c_str();
  while (*c)
  {
    if (! isspace(*c)) mangledName += OFstatic_cast(char, toupper(*c));
    ++c;
  }
  /* check profile */
  if (result.good() && !m_assocConfig->isKnownProfile(mangledName.c_str()))
  {
    DCMNET_ERROR("No association profile named \"" << profileName << "\" in association configuration");
    result = EC_IllegalParameter;
  }
  if (result.good() && !m_assocConfig->isValidSCPProfile(mangledName.c_str()))
  {
    DCMNET_ERROR("The association profile named \"" << profileName << "\" is not a valid SCP association profile");
    result = EC_IllegalParameter;
  }
  if (result.good())
    m_assocCfgProfileName = mangledName;

  return result;
}

// ----------------------------------------------------------------------------

OFCondition DcmSCP::addPresentationContext(const OFString& abstractSyntax,
                                           const OFList<OFString> xferSyntaxes,
                                           const OFString& profile)
{
  if (profile.empty())
    return EC_IllegalParameter;

  const OFString DCMSCP_TS_KEY("DCMSCP_GEN_TS_KEY");
  const OFString DCMSCP_PC_KEY("DCMSCP_GEN_PC_KEY");
  // create new association configuration if not already existing
  OFBool newlyCreated = OFFalse;
  if (!m_assocConfig)
  {
    m_assocConfig = new DcmAssociationConfiguration();
    newlyCreated = OFTrue;
  }
  OFListConstIterator(OFString) it = xferSyntaxes.begin();
  OFListConstIterator(OFString) endOfList = xferSyntaxes.end();
  OFCondition result;
  // the association configuration needs key names for transfer syntaxes and
  // presentation contexts. Use predefined key names.
  while ((it != endOfList) && result.good())
  {
    result = m_assocConfig->addTransferSyntax(DCMSCP_TS_KEY.c_str(), (*it).c_str());
    it++;
  }
  if (result.good())
  {
    result = m_assocConfig->addPresentationContext(DCMSCP_PC_KEY.c_str(), abstractSyntax.c_str(), DCMSCP_TS_KEY.c_str());
  }
  /* perform name mangling for config file key */
  const char *c = profile.c_str();
  OFString mangledName;
  while (*c)
  {
    if (! isspace(*c)) mangledName += OFstatic_cast(char, toupper(*c));
    ++c;
  }
  if (result.good() && !m_assocConfig->isKnownProfile(mangledName.c_str()))
  {
    result = m_assocConfig->addProfile(mangledName.c_str(), DCMSCP_PC_KEY.c_str());
  }
  if (result.bad() && newlyCreated)
  {
    // in case of error clear association configuration if it was newly created
    delete m_assocConfig;
    m_assocConfig = NULL;
  }

  return result;
}

// ----------------------------------------------------------------------------

void DcmSCP::dropAndDestroyAssociation()
{
  if (m_assoc)
  {
    notifyAssociatonTermination();
    ASC_dropAssociation( m_assoc );
    ASC_destroyAssociation( &m_assoc );
  }
}


/* ************************************************************************** */
/*                            Notify functions                                */
/* ************************************************************************** */


void DcmSCP::notifyAssociationRequest(const T_ASC_Parameters& params,
                                      DcmSCPActionType& desiredAction)
{
  // Dump some information if required
  DCMNET_INFO("Association Received " << params.DULparams.callingPresentationAddress << ": "
                                      << params.DULparams.callingAPTitle << " -> "
                                      << params.DULparams.calledAPTitle);

    // Dump more information if required
  OFString msg;
  if (m_verbosePCMode)
    DCMNET_INFO("Incoming Association Request:" << OFendl << ASC_dumpParameters(msg, m_assoc->params, ASC_ASSOC_RQ));
  else
    DCMNET_DEBUG("Incoming Association Request:" << OFendl << ASC_dumpParameters(msg, m_assoc->params, ASC_ASSOC_RQ));
}

// ----------------------------------------------------------------------------

void DcmSCP::notifyAssociationAcknowledge()
{
  DCMNET_INFO("Association Acknowledged");
}

// ----------------------------------------------------------------------------

void DcmSCP::notifyReleaseRequest()
{
  DCMNET_INFO("Received Association Release Request");
}

// ----------------------------------------------------------------------------

void DcmSCP::notifyAbortRequest()
{
  DCMNET_INFO("DcmSCP: Received Association Abort Request");
}

// ----------------------------------------------------------------------------

void DcmSCP::notifyAssociatonTermination()
{
  DCMNET_INFO("DcmSCP: Association Terminated");
}

// ----------------------------------------------------------------------------

void DcmSCP::notifyDIMSEError(const OFCondition& cond)
{
  DCMNET_DEBUG("DIMSE Error, detail (if available): " << cond.text());
}

/*
** CVS Log
** $Log: scp.cc,v $
** Revision 1.7  2010-06-17 17:08:05  joergr
** Aligned SCP class with existing SCU class. Some further code cleanups.
** Changed default profile from "Default" to "DEFAULT". Revised documentation.
**
** Revision 1.6  2010-06-14 15:56:21  joergr
** Minor fixes to log messages and log levels.
**
** Revision 1.5  2010-06-02 16:03:38  joergr
** Slightly modified some log messages and levels for reasons of consistency.
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.4  2010-04-29 16:14:56  onken
** Added function for responding to storage requests to SCP class.
**
** Revision 1.3  2010-03-08 12:59:53  joergr
** Changed OFListIterator to OFListConstIterator in order to compile when
** HAVE_STL is defined.
**
** Revision 1.2  2009-12-21 15:33:58  onken
** Added documentation and refactored / enhanced some code.
**
** Revision 1.1  2009-12-16 17:05:35  onken
** Added base classes for SCU and SCP implementation.
**
*/
