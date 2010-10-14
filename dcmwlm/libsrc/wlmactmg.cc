/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Module:  dcmwlm
 *
 *  Author:  Thomas Wilkens
 *
 *  Purpose: Activity manager class for basic worklist management service
 *           class providers.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:49 $
 *  CVS/RCS Revision: $Revision: 1.34 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

// ----------------------------------------------------------------------------

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmwlm/wltypdef.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcvrat.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmwlm/wlds.h"
#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcdicent.h"  // needed by MSVC5 with STL
#include "dcmtk/dcmwlm/wlmactmg.h"

// ----------------------------------------------------------------------------

// We need two global functions, because we need to pass a function pointer for a callback function
// to a certain function in dcmnet. This function pointer cannot point to an element function of the
// above defined class, because dcmnet expects it to be a pointer to a global function. Hence, function
// FindCallback() needs to be global. Function AddStatusDetail() is used in FindCallback() that's why
// it is also defined as global.

static void FindCallback( void *callbackData, OFBool cancelled, T_DIMSE_C_FindRQ * /*request*/, DcmDataset *requestIdentifiers, int responseCount, T_DIMSE_C_FindRSP *response, DcmDataset **responseIdentifiers, DcmDataset **statusDetail );
// Task         : This function will try to select another record from a database which matches the
//                search mask that was passed. In certain circumstances, the selected information
//                will be dumped to stdout.
// Parameters   : callbackData        - [in] data for this callback function
//                cancelled           - [in] Specifies if we encounteres a C-CANCEL-RQ. In such a case
//                                      the search shall be cancelled.
//                request             - [in] The original C-FIND-RQ message.
//                requestIdentifiers  - [in] Contains the search mask.
//                responseCount       - [in] If we labelled C-FIND-RSP messages consecutively, starting
//                                      at label "1", this number would provide the label's number.
//                response            - [inout] the C-FIND-RSP message (will be sent after the call to
//                                      this function). The status field will be set in this function.
//                responseIdentifiers - [inout] Will in the end contain the next record that matches the
//                                      search mask (captured in requestIdentifiers)
//                statusDetail        - [inout] This variable can be used to capture detailed information
//                                      with regard to the status information which is captured in the
//                                      status element (0000,0900) of the C-FIND-RSP message.
// Return Value : OFCondition value denoting success or error.

static OFString AddStatusDetail( DcmDataset **statusDetail, const DcmElement *elem );
// Task         : This function adds information to the status detail information container.
// Parameters   : statusDetail - [inout] This variable can be used to capture detailed information
//                               with regard to the status information which is captured in the
//                               status element (0000,0900) of the C-FIND-RSP message.
//                elem         - [in] Element that shall be added to the status detail information
//                               container.
// Return Value : none.

// ----------------------------------------------------------------------------

WlmActivityManager::WlmActivityManager(
    WlmDataSource *dataSourcev,
    OFCmdUnsignedInt opt_portv,
    OFBool opt_refuseAssociationv,
    OFBool opt_rejectWithoutImplementationUIDv,
    OFCmdUnsignedInt opt_sleepAfterFindv,
    OFCmdUnsignedInt opt_sleepDuringFindv,
    OFCmdUnsignedInt opt_maxPDUv,
    E_TransferSyntax opt_networkTransferSyntaxv,
    OFBool opt_failInvalidQueryv,
    OFBool opt_singleProcessv,
    int opt_maxAssociationsv,
    T_DIMSE_BlockingMode opt_blockModev,
    int opt_dimse_timeoutv,
    int opt_acse_timeoutv,
    OFBool opt_forkedChildv,
    int argcv,
    char *argvv[] )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : dataSourcev                         - [in] Pointer to the data source which shall be used.
//                opt_portv                           - [in] The port on which the application is supposed to listen.
//                opt_refuseAssociationv              - [in] Specifies if an association shall always be refused by the SCP.
//                opt_rejectWithoutImplementationUIDv - [in] Specifies if the application shall reject an association if no implementation class UID is provided by the calling SCU.
//                opt_sleepAfterFindv                 - [in] Specifies how many seconds the application is supposed to sleep after having handled a C-FIND-Rsp.
//                opt_sleepDuringFindv                - [in] Specifies how many seconds the application is supposed to sleep during the handling of a C-FIND-Rsp.
//                opt_maxPDUv                         - [in] Maximum length of a PDU that can be received in bytes.
//                opt_networkTransferSyntaxv          - [in] Specifies the preferred network transfer syntaxes.
//                opt_failInvalidQueryv               - [in] Specifies if the application shall fail on an invalid query.
//                opt_singleProcessv                  - [in] Specifies if the application shall run in a single process.
//                opt_maxAssociationsv                - [in] Specifies many concurrent associations the application shall be able to handle.
//                opt_forkedChildv                    - [in] Indicates, whether this process was "forked" from a parent process, default: false
//                argcv                               - [in] Number of arguments in command line
//                argvv                               - [in/out] Holds complete commandline
// Return Value : none.
  : dataSource( dataSourcev ), opt_port( opt_portv ), opt_refuseAssociation( opt_refuseAssociationv ),
    opt_rejectWithoutImplementationUID( opt_rejectWithoutImplementationUIDv ),
    opt_sleepAfterFind( opt_sleepAfterFindv ), opt_sleepDuringFind( opt_sleepDuringFindv ),
    opt_maxPDU( opt_maxPDUv ), opt_networkTransferSyntax( opt_networkTransferSyntaxv ),
    opt_failInvalidQuery( opt_failInvalidQueryv ),
    opt_singleProcess( opt_singleProcessv ),  opt_forkedChild( opt_forkedChildv ), cmd_argc( argcv ),
    cmd_argv( argvv ), opt_maxAssociations( opt_maxAssociationsv ),
    opt_blockMode(opt_blockModev), opt_dimse_timeout(opt_dimse_timeoutv), opt_acse_timeout(opt_acse_timeoutv),
    supportedAbstractSyntaxes( NULL ), numberOfSupportedAbstractSyntaxes( 0 ),
    processTable( )
{
  // initialize supported abstract transfer syntaxes.
  supportedAbstractSyntaxes = new char*[2];
  supportedAbstractSyntaxes[0] = new char[ strlen( UID_VerificationSOPClass ) + 1 ];
  strcpy( supportedAbstractSyntaxes[0], UID_VerificationSOPClass );
  supportedAbstractSyntaxes[1] = new char[ strlen( UID_FINDModalityWorklistInformationModel ) + 1 ];
  strcpy( supportedAbstractSyntaxes[1], UID_FINDModalityWorklistInformationModel );
  numberOfSupportedAbstractSyntaxes = 2;

  // make sure not to let dcmdata remove tailing blank padding or perform other
  // manipulations. We want to see the real data.
  dcmEnableAutomaticInputDataCorrection.set( OFFalse );
  if (!opt_forkedChild)
    DCMWLM_WARN("(notice: dcmdata auto correction disabled.)");

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

WlmActivityManager::~WlmActivityManager()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Destructor.
// Parameters   : none.
// Return Value : none.
{
  // free memory
  delete supportedAbstractSyntaxes[0];
  delete supportedAbstractSyntaxes[1];
  delete supportedAbstractSyntaxes;

#ifdef HAVE_WINSOCK_H
  WSACleanup();
#endif
}

// ----------------------------------------------------------------------------

OFCondition WlmActivityManager::StartProvidingService()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Starts providing the implemented service for calling SCUs.
//                After having created an instance of this class, this function
//                shall be called from main.
// Parameters   : none.
// Return Value : Return value that is supposed to be returned from main().
{
  OFCondition cond = EC_Normal;
  T_ASC_Network *net = NULL;

  // Make sure data dictionary is loaded.
  if( !dcmDataDict.isDictionaryLoaded() )
  {
    DCMWLM_WARN("no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE);
  }

#ifdef HAVE_GETEUID
  // If port is privileged we must be as well.
  if( opt_port < 1024 && geteuid() != 0 )
    return( WLM_EC_InsufficientPortPrivileges );
#endif

#ifdef _WIN32
  /* if this process was started by CreateProcess, opt_forkedChild is set */
  if (opt_forkedChild)
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
      DCMWLM_ERROR("cannot read socket handle: " << GetLastError());
      exit(0);
    }
  }
  else
  {
    // parent process
    if (!opt_singleProcess)
      DUL_requestForkOnTransportConnectionReceipt(cmd_argc, cmd_argv);
  }
#endif

  // Initialize network, i.e. create an instance of T_ASC_Network*.
  cond = ASC_initializeNetwork( NET_ACCEPTOR, (int)opt_port, opt_acse_timeout, &net );
  if( cond.bad() ) return( WLM_EC_InitializationOfNetworkConnectionFailed );

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
    cond = WaitForAssociation( net );

    // Clean up any child processes if the execution is not limited to a single process.
    // (On windows platform, childs are not handled via the process table,
    // so there's no need to clean up children)
#ifdef HAVE_FORK
    if( !opt_singleProcess )
      CleanChildren();
#elif _WIN32
    // if running in multi-process mode, always terminate child after one association
    // for unix, this is done in WaitForAssociation() with exit()
    if (DUL_processIsForkedChild()) break;
#endif
  }
  // Drop the network, i.e. free memory of T_ASC_Network* structure. This call
  // is the counterpart of ASC_initializeNetwork(...) which was called above.
  cond = ASC_dropNetwork( &net );
  if( cond.bad() ) return( WLM_EC_TerminationOfNetworkConnectionFailed );

  // return ok
  return( EC_Normal );
}

// ----------------------------------------------------------------------------

void WlmActivityManager::RefuseAssociation( T_ASC_Association **assoc, WlmRefuseReasonType reason )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function takes care of refusing an assocation request.
// Parameters   : assoc  - [in] The association (network connection to another DICOM application).
//                reason - [in] The reason why the association request will be refused.
// Return Value : none.
{
  T_ASC_RejectParameters rej;

  // Dump some information if required.
  switch( reason )
  {
    case WLM_TOO_MANY_ASSOCIATIONS: DCMWLM_INFO("Refusing Association (too many associations)"); break;
    case WLM_CANNOT_FORK:           DCMWLM_INFO("Refusing Association (cannot fork)"); break;
    case WLM_BAD_APP_CONTEXT:       DCMWLM_INFO("Refusing Association (bad application context)"); break;
    case WLM_BAD_AE_SERVICE:        DCMWLM_INFO("Refusing Association (bad application entity service)"); break;
    case WLM_FORCED:                DCMWLM_INFO("Refusing Association (forced via command line)"); break;
    case WLM_NO_IC_UID:             DCMWLM_INFO("Refusing Association (no implementation class UID provided)"); break;
    default:                        DCMWLM_INFO("Refusing Association (unknown reason)"); break;
  }

  // Set some values in the reject message depending on the reason.
  switch( reason )
  {
    case WLM_TOO_MANY_ASSOCIATIONS:
      rej.result = ASC_RESULT_REJECTEDTRANSIENT;
      rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
      rej.reason = ASC_REASON_SP_PRES_LOCALLIMITEXCEEDED;
      break;
    case WLM_CANNOT_FORK:
      rej.result = ASC_RESULT_REJECTEDPERMANENT;
      rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
      rej.reason = ASC_REASON_SP_PRES_TEMPORARYCONGESTION;
      break;
    case WLM_BAD_APP_CONTEXT:
      rej.result = ASC_RESULT_REJECTEDTRANSIENT;
      rej.source = ASC_SOURCE_SERVICEUSER;
      rej.reason = ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED;
      break;
    case WLM_BAD_AE_SERVICE:
      rej.result = ASC_RESULT_REJECTEDPERMANENT;
      rej.source = ASC_SOURCE_SERVICEUSER;
      rej.reason = ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED;
      break;
    case WLM_FORCED:
    case WLM_NO_IC_UID:
    default:
      rej.result = ASC_RESULT_REJECTEDPERMANENT;
      rej.source = ASC_SOURCE_SERVICEUSER;
      rej.reason = ASC_REASON_SU_NOREASON;
      break;
  }

  // Reject the association request.
  ASC_rejectAssociation( *assoc, &rej );

  // Drop the association.
  ASC_dropAssociation( *assoc );

  // Destroy the association.
  ASC_destroyAssociation( assoc );
}

// ----------------------------------------------------------------------------

OFCondition WlmActivityManager::WaitForAssociation( T_ASC_Network * net )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function takes care of receiving, negotiating and accepting/refusing an
//                association request. Additionally, it handles the request the association
//                requesting application transmits after a connection isd established.
// Parameters   : net - [in] Contains network parameters.
// Return Value : Indicator which shows if function was executed successfully.
{
  T_ASC_Association *assoc = NULL;
  char buf[BUFSIZ];
  int timeout;

  // Depending on if the execution is limited to one single process
  // or not we need to set the timeout value correspondingly.
  // for WIN32, child processes cannot be counted (always 0) -> timeout=1000
  if( opt_singleProcess )
    timeout = 1000;
  else
  {
    if( processTable.size() > 0 )
      timeout = 1;
    else
      timeout = 1000;
  }

  // Listen to a socket for timeout seconds and wait for an association request.
  OFCondition cond = ASC_receiveAssociation( net, &assoc, opt_maxPDU, NULL, NULL, OFFalse, DUL_NOBLOCK, timeout );

  // just return, if timeout occured (DUL_NOASSOCIATIONREQUEST)
  // or (WIN32) if dcmnet has started a child for us, to handle this
  // association (signaled by "DULC_FORKEDCHILD") -> return to "event loop"
  if ( ( cond.code() == DULC_FORKEDCHILD ) || ( cond == DUL_NOASSOCIATIONREQUEST ) )
    return EC_Normal;

  // if error occurs and we're not in single process mode, close association and return
  if( cond.bad() && !opt_singleProcess )
  {
    ASC_dropAssociation( assoc );
    ASC_destroyAssociation( &assoc );
    return EC_Normal;
  }
  // Dump some information if required
  DCMWLM_INFO("Association Received ("
    << assoc->params->DULparams.callingPresentationAddress
    << ":" << assoc->params->DULparams.callingAPTitle
    << " -> " << assoc->params->DULparams.calledAPTitle << ")");

  // Dump more information if required
  OFString temp_str;
  DCMWLM_DEBUG("Parameters:" << OFendl << ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_RQ));

  // Now we have to figure out if we might have to refuse the association request.
  // This is the case if at least one of five conditions is met:

  // Condition 1: if option "--refuse" is set we want to refuse the association request.
  if( opt_refuseAssociation )
  {
    RefuseAssociation( &assoc, WLM_FORCED );
    if( !opt_singleProcess )
    {
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    return( EC_Normal );
  }

  // Condition 2: determine the application context name. If an error occurred or if the
  // application context name is not supported we want to refuse the association request.
  cond = ASC_getApplicationContextName( assoc->params, buf );
  if( cond.bad() || strcmp( buf, DICOM_STDAPPLICATIONCONTEXT ) != 0 )
  {
    RefuseAssociation( &assoc, WLM_BAD_APP_CONTEXT );
    if( !opt_singleProcess )
    {
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    return( EC_Normal );
  }

  // Condition 3: if option "--reject" is set and the caller did not provide an
  // implementation class UID we want to refuse the association request
  if( opt_rejectWithoutImplementationUID && strlen( assoc->params->theirImplementationClassUID ) == 0 )
  {
    RefuseAssociation( &assoc, WLM_NO_IC_UID );
    if( !opt_singleProcess )
    {
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    return( EC_Normal );
  }

  // Condition 4: if there are too many concurrent associations
  // we want to refuse the association request
  if( (int)processTable.size() >= opt_maxAssociations )
  {
    RefuseAssociation( &assoc, WLM_TOO_MANY_ASSOCIATIONS );
    if( !opt_singleProcess )
    {
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    return( EC_Normal );
  }

  // Condition 5: if the called application entity title is not supported
  // whithin the data source we want to refuse the association request
  dataSource->SetCalledApplicationEntityTitle( assoc->params->DULparams.calledAPTitle );
  if( !dataSource->IsCalledApplicationEntityTitleSupported() )
  {
    RefuseAssociation( &assoc, WLM_BAD_AE_SERVICE );
    if( !opt_singleProcess )
    {
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    return( EC_Normal );
  }

  // If we get to this point the association shall be negotiated.
  cond = NegotiateAssociation( assoc );
  if( cond.bad() )
  {
    if( !opt_singleProcess )
    {
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    return( EC_Normal );
  }

  // Reject association if no presentation context was negotiated
  if( ASC_countAcceptedPresentationContexts( assoc->params ) == 0 )
  {
    DCMWLM_INFO("No Acceptable Presentation Contexts");
    RefuseAssociation( &assoc, WLM_FORCED );
    if( !opt_singleProcess )
    {
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    return( EC_Normal );
  }

  // If the negotiation was successful, accept the association request.
  cond = ASC_acknowledgeAssociation( assoc );
  if( cond.bad() )
  {
    if( !opt_singleProcess )
    {
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    return( EC_Normal );
  }

  // Dump some information if required.
  DCMWLM_INFO("Association Acknowledged (Max Send PDV: " << assoc->sendPDVLength << ")");

  // Dump some more information if required.
  DCMWLM_DEBUG(ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_AC));

  // Depending on if this execution shall be limited to one process or not, spawn a sub-
  // process to handle the association or don't. (Note: For windows dcmnet is handling
  // the creation for a new subprocess, so we can call HandleAssociation directly, too)
  if( opt_singleProcess || opt_forkedChild )
  {
    // Go ahead and handle the association (i.e. handle the callers requests) in this process.
    HandleAssociation( assoc );
  }
#ifdef HAVE_FORK
  else
  {
    // Spawn a sub-process to handle the association (i.e. handle the callers requests)
    int pid = (int)(fork());
    if( pid < 0 )
    {
      RefuseAssociation( &assoc, WLM_CANNOT_FORK );
      if( !opt_singleProcess )
      {
        ASC_dropAssociation( assoc );
        ASC_destroyAssociation( &assoc );
      }
      return( EC_Normal );
    }
    else if( pid > 0 )
    {
      // Fork returns a positive process id if this is the parent process.
      // If this is the case, remeber the process in a table and go ahead.
      AddProcessToTable( pid, assoc );

      // the child will handle the association, we can drop it
      ASC_dropAssociation( assoc );
      ASC_destroyAssociation( &assoc );
    }
    else
    {
      // If the process id is not positive, this must be the child process.
      // We want to handle the association, i.e. the callers requests.
      HandleAssociation( assoc );

      // When everything is finished, terminate the child process.
      exit(0);
    }
  }
#endif // HAVE_FORK

  return( EC_Normal );
}

// ----------------------------------------------------------------------------

OFCondition WlmActivityManager::NegotiateAssociation( T_ASC_Association *assoc )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function negotiates a presentation context which will be used by this application
//                and the other DICOM appliation that requests an association.
// Parameters   : assoc - [in] The association (network connection to another DICOM application).
// Return Value : OFCondition value denoting success or error.
{
  const char* transferSyntaxes[] = { NULL, NULL, NULL };
  int numTransferSyntaxes = 0;

  switch( opt_networkTransferSyntax )
  {
    case EXS_LittleEndianImplicit:
      // we only support Little Endian Implicit
      transferSyntaxes[0]  = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 1;
      break;
    case EXS_LittleEndianExplicit:
      // we prefer Little Endian Explicit
      transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 2;
      break;
    case EXS_BigEndianExplicit:
      // we prefer Big Endian Explicit
      transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 2;
      break;
    default:
      // We prefer explicit transfer syntaxes.
      // If we are running on a Little Endian machine we prefer
      // LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
      if (gLocalByteOrder == EBO_LittleEndian)  //defined in dcxfer.h
      {
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
      }
      else
      {
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      }
      transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
      break;
  }

  // accept any of the supported abstract syntaxes
  OFCondition cond = ASC_acceptContextsWithPreferredTransferSyntaxes( assoc->params, (const char**)supportedAbstractSyntaxes, numberOfSupportedAbstractSyntaxes, (const char**)transferSyntaxes, numTransferSyntaxes);

  return cond;
}

// ----------------------------------------------------------------------------

void WlmActivityManager::HandleAssociation( T_ASC_Association *assoc )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function takes care of handling the other DICOM application's request. After
//                having accomplished all necessary steps, the association will be dropped and destroyed.
// Parameters   : assoc - [in] The association (network connection to another DICOM application).
// Return Value : none.
{
  // Receive a DIMSE command and perform all the necessary actions. (Note that ReceiveAndHandleCommands()
  // will always return a value 'cond' for which 'cond.bad()' will be true. This value indicates that either
  // some kind of error occurred, or that the peer aborted the association (DUL_PEERABORTEDASSOCIATION),
  // or that the peer requested the release of the association (DUL_PEERREQUESTEDRELEASE).) (Also note
  // that ReceiveAndHandleCommands() will never return EC_Normal.)
  OFCondition cond = ReceiveAndHandleCommands( assoc );

  // Clean up on association termination.
  if( cond == DUL_PEERREQUESTEDRELEASE )
  {
    DCMWLM_INFO("Association Release");
    ASC_acknowledgeRelease( assoc );
    ASC_dropSCPAssociation( assoc );
  }
  else if( cond == DUL_PEERABORTEDASSOCIATION )
  {
    DCMWLM_INFO("Association Aborted");
  }
  else
  {
    OFString temp_str;
    DCMWLM_ERROR("DIMSE failure (aborting association): " << DimseCondition::dump(temp_str, cond));
    ASC_abortAssociation( assoc );
  }

  // Drop and destroy the association.
  ASC_dropAssociation( assoc );
  ASC_destroyAssociation( &assoc );

  // Dump some information if required.
  DCMWLM_INFO("+++++++++++++++++++++++++++++");
}

// ----------------------------------------------------------------------------

OFCondition WlmActivityManager::ReceiveAndHandleCommands( T_ASC_Association *assoc )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function takes care of handling the other DICOM application's request.
// Parameters   : assoc - [in] The association (network connection to another DICOM application).
// Return Value : An OFCondition value 'cond' for which 'cond.bad()' will always be set
//                indicating that either some kind of error occurred, or that the peer aborted
//                the association (DUL_PEERABORTEDASSOCIATION), or that the peer requested the
//                release of the association (DUL_PEERREQUESTEDRELEASE).
{
  OFCondition cond = EC_Normal;
  T_DIMSE_Message msg;
  T_ASC_PresentationContextID presID;

  // Tell object that manages the data source if it should fail on an invalid query or not.
  dataSource->SetFailOnInvalidQuery( opt_failInvalidQuery );

  // start a loop to be able to receive more than one DIMSE command
  while( cond.good() )
  {
    // receive a DIMSE command over the network
    cond = DIMSE_receiveCommand( assoc, DIMSE_BLOCKING, 0, &presID, &msg, NULL );

    // check if peer did release or abort, or if we have a valid message
    if( cond.good() )
    {
      // in case we received a valid message, process this command
      // note that we can only process a C-ECHO-RQ, a C-FIND-RQ and a C-CANCEL-RQ
      switch( msg.CommandField )
      {
        case DIMSE_C_ECHO_RQ:
          // Process C-ECHO-Request
          cond = HandleEchoSCP( assoc, &msg.msg.CEchoRQ, presID );
          break;
        case DIMSE_C_FIND_RQ:
          // Process C-FIND-Request
          cond = HandleFindSCP( assoc, &msg.msg.CFindRQ, presID );
          break;
        case DIMSE_C_CANCEL_RQ:
          // Process C-CANCEL-Request
          // This is a late cancel request, just ignore it
          DCMWLM_WARN("Received late Cancel Request, ignoring");
          break;
        default:
          // We cannot handle this kind of message.
          // (Note that the condition will be returned and that the caller will abort the association.)
          cond = DIMSE_BADCOMMANDTYPE;
          break;
      }
    }
  }

  // return result
  return( cond );
}

// ----------------------------------------------------------------------------

OFCondition WlmActivityManager::HandleEchoSCP( T_ASC_Association *assoc, T_DIMSE_C_EchoRQ *req, T_ASC_PresentationContextID presId )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Having received a DIMSE C-ECHO-RQ message, this function takes care of sending a
//                DIMSE C-ECHO-RSP message over the network connection.
// Parameters   : assoc   - [in] The association (network connection to another DICOM application).
//                request - [in] The DIMSE C-ECHO-RQ message that was received.
//                presID  - [in] The ID of the presentation context which was specified in the PDV
//                               which contained the DIMSE command.
// Return Value : OFCondition value denoting success or error.
{
  OFString temp_str;

  // Dump information if required
  DCMWLM_INFO("Received Echo Request");
  DCMWLM_DEBUG(DIMSE_dumpMessage(temp_str, *req, DIMSE_INCOMING, NULL, presId));

  // Send an echo response
  OFCondition cond = DIMSE_sendEchoResponse( assoc, presId, req, STATUS_Success, NULL );
  if( cond.bad() )
    DCMWLM_ERROR("Echo SCP Failed: " << DimseCondition::dump(temp_str, cond));

  // return return value
  return cond;
}

// ----------------------------------------------------------------------------

struct WlmFindContextType
// Date   : December 10, 2001
// Author : Thomas Wilkens
// Task   : This struct will be used in the following method to pass information
//          from the class to the callback method. (Note that it cannot be defined
//          in wltypdef.h because it makes use of class WlmDataSource which is
//          unknown in wltypdef.h.)
{
  WlmDataSource *dataSource;
  WlmDataSourceStatusType priorStatus;
  DIC_AE ourAETitle;
  OFCmdUnsignedInt opt_sleepDuringFind;
};

// ----------------------------------------------------------------------------

OFCondition WlmActivityManager::HandleFindSCP( T_ASC_Association *assoc, T_DIMSE_C_FindRQ *request, T_ASC_PresentationContextID presID )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function processes a DIMSE C-FIND-RQ commmand that was
//                received over the network connection.
// Parameters   : assoc    - [in] The association (network connection to another DICOM application).
//                request  - [in] The DIMSE C-FIND-RQ message that was received.
//                presID   - [in] The ID of the presentation context which was specified in the PDV
//                                which contained the DIMSE command.
// Return Value : OFCondition value denoting success or error.
{
  // Create callback data which needs to be passed to DIMSE_findProvider later.
  OFString temp_str;
  WlmFindContextType context;
  context.dataSource = dataSource;
  context.priorStatus = WLM_PENDING;
  ASC_getAPTitles( assoc->params, NULL, context.ourAETitle, NULL );
  context.opt_sleepDuringFind = opt_sleepDuringFind;

  // Dump some information if required.
  DCMWLM_INFO(DIMSE_dumpMessage(temp_str, *request, DIMSE_INCOMING, NULL, presID));

  // Handle a C-FIND-Request on the provider side: receive the data set that represents the search mask
  // over the network, try to select corresponding records that match the search mask from some data source
  // (this is done whithin the callback function FindCallback() that will be passed) and send corresponding
  // C-FIND-RSP messages to the other DICOM application this application is connected with. In the end,
  // also send the C-FIND-RSP message that indicates that there are no more search results.
  OFCondition cond = DIMSE_findProvider( assoc, presID, request, FindCallback, &context, opt_blockMode, opt_dimse_timeout );
  if( cond.bad() )
    DCMWLM_ERROR("Find SCP Failed: " << DimseCondition::dump(temp_str, cond));

  // If option "--sleep-after" is set we need to sleep opt_sleepAfterFind
  // seconds after having processed one C-FIND-Request message.
  if( opt_sleepAfterFind > 0 )
  {
    DCMWLM_INFO("Sleeping (after find): " << opt_sleepAfterFind << " secs");
    OFStandard::sleep( (unsigned int)opt_sleepAfterFind );
  }

  // return result
  return cond;
}

// ----------------------------------------------------------------------------

void WlmActivityManager::AddProcessToTable( int pid, T_ASC_Association *assoc )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function adds a process to the table that stores process information.
// Parameters   : pid   - [in] the process id of the sub-process which was just started.
//                assoc - [in] The association (network connection to another DICOM application).
// Return Value : none.
{
  WlmProcessSlotType *ps;

  // Allocate some memory for a new item in the list of processes.
  ps = new WlmProcessSlotType ();

  // Remember process information in the new item.
  ASC_getPresentationAddresses( assoc->params, ps->peerName, NULL );
  ASC_getAPTitles( assoc->params, ps->callingAETitle, ps->calledAETitle, NULL );
  ps->processId = pid;
  ps->startTime = time(NULL);
  ps->hasStorageAbility = OFFalse;

  // Add new item to the beginning of the list.
  processTable.push_back(ps);
}

// ----------------------------------------------------------------------------

void WlmActivityManager::RemoveProcessFromTable( int pid )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function removes one particular item from the table which stores all subprocess
//                information. The item which shall be deleted will be identified by its process id.
// Parameters   : pid - [in] process id.
// Return Value : none.
{
  WlmProcessSlotType *ps = NULL;

  // try to find item that corresponds to the given process id
  OFListIterator(WlmProcessSlotType*) it = processTable.begin();
  while ( it != processTable.end() )
  {
    ps = *it;
    // if process can be found, delete it from list and free memory
    if ( ps->processId == pid )
    {
      processTable.remove(*it);
      delete ps;
      return;
    }
    it++;
  }

  // dump a warning if process could not be found in process table
  DCMWLM_WARN("WlmActivityManager::RemoveProcessFromTable: Could not find process " << pid);
}

// ----------------------------------------------------------------------------

void WlmActivityManager::CleanChildren()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function takes care of removing items referring to (terminated) subprocess
//                from the table which stores all subprocess information. Three different versions
//                for three different platforms are implemented.
// Parameters   : none.
// Return Value : none.
{

#ifdef HAVE_WAITPID                                           // PLATFORMS THAT HAVE waitpid()
  int options = WNOHANG;
  int stat_loc;
  int child=1;

  while( child > 0 )
  {
    // determine status for child processes
    child = (int)(waitpid(-1, &stat_loc, options));
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
        DCMWLM_WARN("WlmActivityManager::CleanChildren: Wait for child failed");
      }
    }
    else if( child > 0 )
    {
      // dump some information if required
      DCMWLM_INFO("Cleaned up after child (" << child << ")");

      // remove item from process table
      RemoveProcessFromTable( child );
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
        DCMWLM_WARN("WlmActivityManager::CleanChildren: Wait for child failed");
      }
    }
    else if( child > 0 )
    {
      // dump some information if required
      DCMWLM_INFO("Cleaned up after child (" << child << ")");

      // remove item from process table
      RemoveProcessFromTable( child );
    }
  }
#else                                                         // OTHER PLATFORMS
// for other platforms without waitpid() and without wait3() we
// don't know how to cleanup after children. Dump an error message.
  DCMWLM_WARN("WlmActivityManager::CleanChildren: Cannot wait for child processes");
#endif
}

// ----------------------------------------------------------------------------

static OFString AddStatusDetail( DcmDataset **statusDetail, const DcmElement *elem )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function adds information to the status detail information container.
// Parameters   : statusDetail - [inout] This variable can be used to capture detailed information
//                               with regard to the status information which is captured in the
//                               status element (0000,0900) of the C-FIND-RSP message.
//                elem         - [in] Element that shall be added to the status detail information
//                               container.
// Return Value : none.
{
  // If no element was passed, return to the caller.
  if( elem == NULL )
    return "";

  OFOStringStream log;
  DcmAttributeTag *at;
  DcmLongString *lo;

  // Create the container object if necessary
  if( *statusDetail == NULL )
    *statusDetail = new DcmDataset();

  // Determine the element's data type
  DcmVR vr( elem->ident() );

  // Dump some information
  log << "Status Detail:" << OFendl;

  // Depending on the element's identification, insert different
  // types of objects into the container.
  switch( elem->ident() )
  {
    case EVR_LO:
      lo = new DcmLongString( *((DcmLongString*)elem) );
      if( lo->getLength() > vr.getMaxValueLength() )
      {
        log << "AddStatusDetail: INTERNAL ERROR: value too large (max " << (unsigned long)(vr.getMaxValueLength())
            << ") for " << vr.getVRName() << ": " << OFendl;
      }
      (*statusDetail)->insert( lo, OFTrue /*replaceOld*/ );
      lo->print(log);
      break;
    case EVR_AT:
      at = new DcmAttributeTag( *((DcmAttributeTag*)elem) );
      if( at->getLength() > vr.getMaxValueLength() )
      {
        log << "AddStatusDetail: INTERNAL ERROR: value too large (max " << (unsigned long)(vr.getMaxValueLength())
            << ") for " << vr.getVRName() << ": " << OFendl;
      }
      (*statusDetail)->insert( at, OFTrue /*replaceOld*/ );
      at->print(log);
      break;
    default:
      // other status detail is not supported
      log << "AddStatusDetail: unsupported status detail type: " << vr.getVRName() << OFendl;
      break;
  }
  log << OFStringStream_ends;
  OFSTRINGSTREAM_GETOFSTRING(log, ret)
  return ret;
}

// ----------------------------------------------------------------------------

static void FindCallback( void *callbackData, OFBool cancelled, T_DIMSE_C_FindRQ * /*request*/, DcmDataset *requestIdentifiers, int responseCount, T_DIMSE_C_FindRSP *response, DcmDataset **responseIdentifiers, DcmDataset **statusDetail )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function will try to select another record from a database which matches the
//                search mask that was passed. In certain circumstances, the selected information
//                will be dumped to stdout.
// Parameters   : callbackData        - [in] data for this callback function
//                cancelled           - [in] Specifies if we encounteres a C-CANCEL-RQ. In such a case
//                                      the search shall be cancelled.
//                request             - [in] The original C-FIND-RQ message.
//                requestIdentifiers  - [in] Contains the search mask.
//                responseCount       - [in] If we labelled C-FIND-RSP messages consecutively, starting
//                                      at label "1", this number would provide the label's number.
//                response            - [inout] the C-FIND-RSP message (will be sent after the call to
//                                      this function). The status field will be set in this function.
//                responseIdentifiers - [inout] Will in the end contain the next record that matches the
//                                      search mask (captured in requestIdentifiers)
//                statusDetail        - [inout] This variable can be used to capture detailed information
//                                      with regard to the status information which is captured in the
//                                      status element (0000,0900) of the C-FIND-RSP message.
// Return Value : OFCondition value denoting success or error.
{
  WlmDataSourceStatusType dbstatus;
  WlmFindContextType *context = NULL;
  WlmDataSource *dataSource = NULL;
  OFCmdUnsignedInt opt_sleepDuringFind = 0;

  // Recover contents of context.
  context = (WlmFindContextType*)callbackData;
  dataSource = context->dataSource;
  opt_sleepDuringFind = context->opt_sleepDuringFind;

  // Determine the data source's current status.
  dbstatus = context->priorStatus;

  // If this is the first time this callback function is called, we need to do something special
  if( responseCount == 1 )
  {
    // Dump some information if required
    DCMWLM_INFO("Find SCP Request Identifiers:" << OFendl
      << DcmObject::PrintHelper(*requestIdentifiers) << OFendl
      << "=============================");

    // Determine the records that match the search mask. After this call, the
    // matching records will be available through dataSource->nextFindResponse(...).)
    dbstatus = dataSource->StartFindRequest( *requestIdentifiers );
    if( !( dbstatus == WLM_PENDING || dbstatus == WLM_PENDING_WARNING || dbstatus == WLM_SUCCESS) )
      DCMWLM_DEBUG("Worklist Database: StartFindRequest() Failed (" << DU_cfindStatusString((Uint16)dbstatus) << ")");

    DCMWLM_INFO("=============================");
  }

  // If opt_sleepDuringFind is set the application is supposed
  // to sleep n seconds during the find process.
  if( opt_sleepDuringFind > 0 )
  {
    DCMWLM_INFO("SLEEPING (during find): " << opt_sleepDuringFind << " secs");
    OFStandard::sleep((unsigned int)opt_sleepDuringFind);
  }

  // If we encountered a C-CANCEL-RQ and if we have pending
  // responses, the search shall be cancelled
  if( cancelled && ( dbstatus == WLM_PENDING || dbstatus == WLM_PENDING_WARNING ) )
    dbstatus = dataSource->CancelFindRequest();

  // If the dbstatus is "pending" try to select another matching record.
  if( dbstatus == WLM_PENDING || dbstatus == WLM_PENDING_WARNING )
  {
    // Get the next matching record/data set
    *responseIdentifiers = dataSource->NextFindResponse( dbstatus );
  }

  // Dump some information if required
  if (DCM_dcmwlmGetLogger().isEnabledFor(OFLogger::INFO_LOG_LEVEL))
  {
    DCMWLM_INFO("Worklist Find SCP Response " << responseCount << " (" << DU_cfindStatusString((Uint16)dbstatus) << ")");
    if( *responseIdentifiers != NULL && (*responseIdentifiers)->card() > 0 )
    {
      DCMWLM_INFO(DcmObject::PrintHelper(**responseIdentifiers) << OFendl << "-----------------------------");
    }
  }

  // Set response status
  response->DimseStatus = dbstatus;

  // Delete status detail information if there is some
  if( *statusDetail != NULL )
  {
    delete *statusDetail;
    *statusDetail = NULL;
  }

  // Depending on the data source's current status, we may have to
  // return status detail information.
  switch( dbstatus )
  {
    case WLM_FAILED_IDENTIFIER_DOES_NOT_MATCH_SOP_CLASS:
    case WLM_FAILED_UNABLE_TO_PROCESS:
      DCMWLM_WARN(AddStatusDetail( statusDetail, dataSource->GetOffendingElements()));
      DCMWLM_WARN(AddStatusDetail( statusDetail, dataSource->GetErrorComments()));
      break;
    case WLM_REFUSED_OUT_OF_RESOURCES:
      // out of resources may only have error comment detail
      DCMWLM_WARN(AddStatusDetail( statusDetail, dataSource->GetErrorComments()));
      break;
    default:
      // other status codes may not have any status detail
      break;
  }
}

// ----------------------------------------------------------------------------

/*
** CVS Log
** $Log: wlmactmg.cc,v $
** Revision 1.34  2010-10-14 13:14:49  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.33  2010-06-17 15:49:09  joergr
** Fixed wrong logger macro (was DCMNET_INFO instead of DCMWLM_INFO).
**
** Revision 1.32  2010-06-17 15:22:39  joergr
** Moved output of "No Acceptable Presentation Contexts" message to another code
** line in order to be visible at all (if appropriate).
**
** Revision 1.31  2010-06-02 12:33:33  joergr
** Appended missing OFStringStream_ends to the end of output streams because
** this is required when OFOStringStream is mapped to ostrstream.
**
** Revision 1.30  2010-05-18 16:43:01  joergr
** Slightly modified log messages and log levels in order to be more consistent.
** Replaced '\n' by OFendl in log messages.
**
** Revision 1.29  2010-02-15 13:23:30  joergr
** Fixed wrong output in debug mode (error message when status is "success").
** Replaced remaining CERR output by DCMWLM_ERROR() macro.
**
** Revision 1.28  2010-01-26 13:39:13  uli
** Use DIMSE_dumpMessage() for some more log messages.
**
** Revision 1.27  2009-12-02 13:16:16  uli
** Corrected build failures on windows.
**
** Revision 1.26  2009-11-24 10:40:01  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.25  2009-01-07 17:21:34  joergr
** Avoid double output of "auto correction" notice for forked children (Win32).
**
** Revision 1.24  2006-12-15 14:49:28  onken
** Removed excessive use char* and C-array in favour of OFString and
** OFList. Simplified some implementation details.
**
** Revision 1.23  2006/08/15 16:15:48  meichel
** Updated the code in module dcmwlm to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.22  2006/08/15 14:22:27  onken
** Removed	superfluous output message
**
** Revision 1.21  2006/08/14 15:31:01  onken
** Added WIN32 multiprocess mode to wlmscpfs.
**
** Revision 1.20  2005/12/08 15:48:35  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.19  2005/11/17 13:45:41  meichel
** Added command line options for DIMSE and ACSE timeouts
**
** Revision 1.18  2005/11/16 14:59:09  meichel
** Set association timeout in ASC_initializeNetwork to 30 seconds. This improves
**   the responsiveness of the tools if the peer blocks during assoc negotiation.
**
** Revision 1.17  2005/08/30 08:39:20  meichel
** The worklist SCP now rejects an association when no presentation context
**   was accepted while processing the association request. Needed for some SCUs
**   which become confused otherwise.
**
** Revision 1.16  2004/02/24 14:45:34  meichel
** Fixed resource leak due to sockets remaining in CLOSE_WAIT state.
**
** Revision 1.15  2003/08/21 09:33:52  wilkens
** Got rid of memory leak in function FindCallback().
** Got rid of some unnecessary if-statements in function FindCallback().
**
** Revision 1.14  2003/06/06 09:45:42  meichel
** Added static sleep function in class OFStandard. This replaces the various
**   calls to sleep(), Sleep() and usleep() throughout the toolkit.
**
** Revision 1.13  2003/06/04 14:28:37  meichel
** Added various includes needed by MSVC5 with STL
**
** Revision 1.12  2002/12/12 16:49:13  wilkens
** Added some code to avoid compiler warning (unreachable code) on Sun CC 2.0.1.
**
** Revision 1.11  2002/12/11 16:55:13  meichel
** Added typecasts to avoid warnings on OSF/1
**
** Revision 1.10  2002/08/12 10:56:18  wilkens
** Made some modifications in in order to be able to create a new application
** which contains both wlmscpdb and ppsscpdb and another application which
** contains both wlmscpfs and ppsscpfs.
**
** Revision 1.9  2002/08/05 09:10:13  wilkens
** Modfified the project's structure in order to be able to create a new
** application which contains both wlmscpdb and ppsscpdb.
**
** Revision 1.8  2002/07/17 13:10:43  wilkens
** Corrected some minor logical errors in the wlmscpdb sources and completely
** updated the wlmscpfs so that it does not use the original wlistctn sources
** any more but standard wlm sources which are now used by all three variants
** of wlmscps.
**
** Revision 1.7  2002/06/10 11:25:12  wilkens
** Made some corrections to keep gcc 2.95.3 quiet.
**
** Revision 1.6  2002/04/18 14:20:25  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
** Revision 1.5  2002/01/08 19:14:54  joergr
** Minor adaptations to keep the gcc compiler on Linux and Solaris happy.
** Currently only the "file version" of the worklist SCP is supported on
** Unix systems.
**
** Revision 1.4  2002/01/08 17:46:04  joergr
** Reformatted source files (replaced Windows newlines by Unix ones, replaced
** tabulator characters by spaces, etc.)
**
** Revision 1.3  2002/01/08 17:30:03  joergr
** Reworked database support after trials at the hospital (modfied by MC/JR on
** 2002-01-08).
**
**
*/
