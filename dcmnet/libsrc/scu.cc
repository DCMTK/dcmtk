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
 *  Purpose: Base class for Service Class Users (SCUs)
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2010-04-29 16:13:25 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/scu.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"  /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcuid.h"    /* for dcmFindUIDName() */
#include "dcmtk/dcmnet/scu.h"
#include "dcmtk/dcmnet/diutil.h"    /* for dcmnet logger */


DcmSCU::DcmSCU() :
  m_assoc(NULL),
  m_net(NULL),
  m_params(NULL),
  m_assocConfigFilename(""),
  m_assocConfigProfile(""),
  m_presContexts(),
  m_openDIMSERequest(NULL),
  m_maxReceivePDULength(ASC_DEFAULTMAXPDU),
  m_blockMode(DIMSE_BLOCKING),
  m_ourAETitle("ANY-SCU"),
  m_peer(""),
  m_peerAETitle("ANY-SCP"),
  m_peerPort(104),
  m_dimseTimeout(0),
  m_acseTimeout(30)
{

#ifdef HAVE_GUSI_H
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

#if defined HAVE_WINSOCK_H || defined HAVE_WINSOCK2_H
  WSAData winSockData;
  /* we need at least version 1.1 */
  WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
  WSAStartup(winSockVersionNeeded, &winSockData);
#endif

}

DcmSCU::~DcmSCU()
{
}


OFCondition DcmSCU::initNetwork()
{
  OFString msg;
  /* initialize network, i.e. create an instance of T_ASC_Network*. */
  OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, m_acseTimeout, &m_net);
  if (cond.bad())
  {
    DimseCondition::dump(msg, cond);
    DCMNET_ERROR(msg);
    return cond;
  }

  /* initialize asscociation parameters, i.e. create an instance of T_ASC_Parameters*. */
  cond = ASC_createAssociationParameters(&m_params, m_maxReceivePDULength);
  if (cond.bad())
  {
    DCMNET_ERROR(DimseCondition::dump(msg, cond));
    return cond;
  }

  /* sets this application's title and the called application's title in the params */
  /* structure. The default values are "ANY-SCU" and "ANY-SCP". */
  ASC_setAPTitles(m_params, m_ourAETitle.c_str(), m_peerAETitle.c_str(), NULL);

  /* Figure out the presentation addresses and copy the */
  /* corresponding values into the association parameters.*/
  DIC_NODENAME localHost;
  DIC_NODENAME peerHost;
  gethostname(localHost, sizeof(localHost) - 1);
  /* Since the underlying dcmnet structures reserve only 64 bytes for peer
     as well as local host name, we check here for buffer overflow.
   */
  if ( (m_peer.length() + 5 /* max 65535 */) +1 /* for ":" */ > 63)
  {
    DCMNET_ERROR("Maximum length of peer host name '" << m_peer << "' is longer than maximum of 57 characters");
    return EC_IllegalCall;
  }
  if ( strlen(localHost) + 1 > 63 )
  {
    DCMNET_ERROR("Maximum length of local host name '" << localHost << " is longer than maximum of 62 characters");
    return EC_IllegalCall;
  }
  sprintf(peerHost, "%s:%d", m_peer.c_str(), OFstatic_cast(int, m_peerPort) );
  ASC_setPresentationAddresses(m_params, localHost, peerHost);

  /* Add presentation contexts */

  // First, import from config file, if specified
  DcmAssociationConfiguration *assocConfig = new DcmAssociationConfiguration();
  OFCondition result;
  if (m_assocConfigFilename.length() != 0)
  {
    result = DcmAssociationConfigurationFile::initialize(*assocConfig, m_assocConfigFilename.c_str());
    if (result.bad())
    {
      DCMNET_WARN("Unable to parse association configuration file " << m_assocConfigFilename << " (ignored): " << result.text());
      delete assocConfig; assocConfig = NULL;
      return result;
    }
    else
    {
      /* perform name mangling for config file key */
      OFString profileName;
      const char *c = m_assocConfigProfile.c_str();
      while (*c)
      {
        if (! isspace(*c)) profileName += (char) (toupper(*c));
        ++c;
      }

      result = assocConfig->setAssociationParameters(profileName.c_str(), *m_params);
      if (result.bad())
      {
        DCMNET_WARN("Unable to apply association configuration file" << m_assocConfigFilename <<" (ignored): " << result.text());
        delete assocConfig; assocConfig = NULL;
        return result;
      }
    }
    delete assocConfig; assocConfig = NULL;
  }

  // Adapt presentation context ID to existing presentation contexts
  // It's important that presentation context ids are numerated 1,3,5,7...!
  Uint32 nextFreePresID = 257;
  Uint32 numContexts = ASC_countPresentationContexts(m_params);
  if (numContexts <= 127)
  {
    // Need Uint16 to avoid overflow in currPresID (unsigned char)
    nextFreePresID = 2* numContexts +1; /* add 1 to point to the next free ID*/
  }
  // Print warning if number of overall presenation contexts exceeds 128
  if ( (numContexts + m_presContexts.size()) > 128)
  {
    DCMNET_WARN("Number of presentation contexts exceeds 128 (" << numContexts + m_presContexts.size() << "). Some contexts will not be negotiated");
  }
  else
  {
    DCMNET_TRACE("Configured " << numContexts << " presentation contexts from config file");
    if (m_presContexts.size() > 0)
        DCMNET_TRACE("Adding another " << m_presContexts.size() << " presentation contexts configured for SCU");
  }

  // Add presentation contexts not originating from config file
  OFListIterator(DcmSCUPresContext) contIt = m_presContexts.begin();
  while ( (contIt != m_presContexts.end()) && (nextFreePresID <= 255) )
  {
    const Uint16 numTransferSyntaxes = (*contIt).transferSyntaxes.size();
    const char** transferSyntaxes = new const char*[numTransferSyntaxes];

    // Iterate over transfer syntaxes within one presentation context
    OFListIterator(OFString) syntaxIt = (*contIt).transferSyntaxes.begin();
    Uint16 sNum = 0;
    // copy all transfersyntaxes to array
    while (syntaxIt != (*contIt).transferSyntaxes.end())
    {
      transferSyntaxes[sNum] = (*syntaxIt).c_str();
      ++syntaxIt;
      ++sNum;
    }

    // add the presentation context
    cond = ASC_addPresentationContext( m_params, OFstatic_cast(Uint8, nextFreePresID), (*contIt).abstractSyntaxName.c_str(), transferSyntaxes, numTransferSyntaxes);
    // if adding was successfull, prepare pres. context ID for next addition
    delete[] transferSyntaxes; transferSyntaxes = NULL;
    if (cond.bad())
      return cond;
    contIt++;
    // goto next free nr, only odd presentation context numbers permitted
    nextFreePresID += 2;
  }
  
  numContexts = ASC_countPresentationContexts(m_params);
  if (numContexts == 0)
  {
    DCMNET_ERROR("Cannot initialize network: No presentation contexts defined");
    return EC_IllegalCall;
  }
  DCMNET_DEBUG("Configured a total of " << numContexts << " presentation contexts for SCU");

  return cond;
}



OFCondition DcmSCU::negotiateAssociation()
{
  /* dump presentation contexts if required */
  OFString msg;
  DCMNET_DEBUG(ASC_dumpParameters(msg, m_params, ASC_ASSOC_RQ));

  /* create association, i.e. try to establish a network connection to another */
  /* DICOM application. This call creates an instance of T_ASC_Association*. */
  DCMNET_DEBUG("Requesting Association");
  OFCondition cond = ASC_requestAssociation(m_net, m_params, &m_assoc);
  if (cond.bad()) {
    if (cond == DUL_ASSOCIATIONREJECTED)
    {
      T_ASC_RejectParameters rej;

      ASC_getRejectParameters(m_params, &rej);
      DCMNET_DEBUG("Association rejected");
      ASC_printRejectParameters(msg, &rej);
      return cond;
    }
    else
    {
      DCMNET_DEBUG("Association Request Failed:\n" << DimseCondition::dump(msg,cond));
      return cond;
    }
  }

  /* dump the presentation contexts which have been accepted/refused */
  DCMNET_DEBUG("Association Parameters Negotiated:\n" << ASC_dumpParameters(msg, m_params, ASC_ASSOC_AC));

  /* count the presentation contexts which have been accepted by the SCP */
  /* If there are none, finish the execution */
  if (ASC_countAcceptedPresentationContexts(m_params) == 0)
  {
    DCMNET_DEBUG("No acceptable presentation contexts found");
    return EC_IllegalCall;
  }

  /* dump general information concerning the establishment of the network connection if required */
  DCMNET_DEBUG("Association Accepted (Max Send PDV: " << (unsigned long)(m_assoc->sendPDVLength));
  return EC_Normal;
}


OFCondition DcmSCU::addPresentationContext(const OFString& abstractSyntax,
                                           const OFList<OFString>& tSyntaxes)

{

  DcmSCUPresContext presContext;
  presContext.abstractSyntaxName = abstractSyntax;
  OFListConstIterator(OFString) it = tSyntaxes.begin();
  while (it != tSyntaxes.end())
  {
    presContext.transferSyntaxes.push_back(*it);
    it++;
  }
  m_presContexts.push_back(presContext);
  return EC_Normal;
}


OFCondition DcmSCU::useSecureConnection(DcmTransportLayer* tlayer)
{
  OFCondition cond = ASC_setTransportLayer(m_net, tlayer, OFFalse /* do not take over ownership */);
  if (cond.good())
    cond = ASC_setTransportLayerType(m_params, OFTrue /* use TLS */);
  return cond;
}


// Reads association configuration from config file
OFCondition readAssocConfigFromFile(const OFString& filename,
                                    const OFString& profile)
{
  DcmAssociationConfiguration assocConfig;
  OFCondition result = DcmAssociationConfigurationFile::initialize(assocConfig, filename.c_str());
  return result;
}


// Returns usable presentation context ID for given abstract syntax and UID
// transfer syntax UID. 0 if none matches.
T_ASC_PresentationContextID DcmSCU::findPresContID(const OFString& abstractSyntax,
                                                   const OFString& transferSyntax)
{
  if (m_assoc == NULL)
    return 0;

  DUL_PRESENTATIONCONTEXT *pc;
  LST_HEAD **l;
  OFBool found = OFFalse;

  if ( transferSyntax.empty() || abstractSyntax.empty() ) return 0;

  /* first of all we look for a presentation context
   * matching both abstract and transfer syntax
   */
  l = &m_assoc->params->DULparams.acceptedPresentationContext;
  pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
  (void)LST_Position(l, (LST_NODE*)pc);
  while (pc && !found)
  {
      found =  (strcmp(pc->abstractSyntax, abstractSyntax.c_str()) == 0);
      found &= (pc->result == ASC_P_ACCEPTANCE);
      found &= (strcmp(pc->acceptedTransferSyntax, transferSyntax.c_str()) == 0);
      if (!found) pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
  }
  if (found) return pc->presentationContextID;

  return 0;   /* not found */
}


Uint16 DcmSCU::nextMessageID()
{
  if (m_assoc == NULL)
    return 0;
  else return m_assoc->nextMsgID++;
}



void DcmSCU::closeAssociation(const OFCondition& abortOrReleaseRequested)
{
  OFCondition cond;
  OFString msg;

  /* tear down association, i.e. terminate network connection to SCP */
  if (abortOrReleaseRequested == EC_Normal)
  {
    /* release association */
    DCMNET_DEBUG("Releasing association");
    cond = ASC_releaseAssociation(m_assoc);
    if (cond.bad())
    {
      DCMNET_ERROR("Association release failed :\n" << DimseCondition::dump(msg, cond));
      return;
    }
  }
  else if (abortOrReleaseRequested == DUL_PEERREQUESTEDRELEASE)
  {
    DCMNET_ERROR("Protocol Error: peer requested release (Aborting)");
    cond = ASC_abortAssociation(m_assoc);
    if (cond.bad()) 
    {
      DCMNET_ERROR("Association abort failed: " << cond.text() << ": " << DimseCondition::dump(msg, cond));
    }
  }
  else if (abortOrReleaseRequested == DUL_PEERABORTEDASSOCIATION)
  {
    DCMNET_DEBUG("Peer Aborted Association");
  }
  else
  {
    DCMNET_ERROR("DcmSCU failed");
    DCMNET_ERROR(DimseCondition::dump(msg, abortOrReleaseRequested));
    DCMNET_DEBUG("Aborting Association");
    cond = ASC_abortAssociation(m_assoc);
    if (cond.bad()) 
    {
      DCMNET_ERROR("Association Abort Failed:" << DimseCondition::dump(msg, cond));
    }
  }

  /* destroy the association, i.e. free memory of T_ASC_Association* structure. This */
  /* call is the counterpart of ASC_requestAssociation(...) which was called above. */
  cond = ASC_destroyAssociation(&m_assoc);
  if (cond.bad()) 
  {
    DCMNET_ERROR("Unable to clean up internal association structures" << DimseCondition::dump(msg, cond));
  }

  /* drop the network, i.e. free memory of T_ASC_Network* structure. This call */
  /* is the counterpart of ASC_initializeNetwork(...) which was called above. */
  cond = ASC_dropNetwork(&m_net);
  if (cond.bad()) 
  {
    DCMNET_ERROR("Unable to clean up internal network structures" << DimseCondition::dump(msg, cond));  
  }

#ifdef HAVE_WINSOCK_H
  WSACleanup();
#endif

}


// Sends C-ECHO request to another DICOM application
OFCondition DcmSCU::sendECHORequest( const T_ASC_PresentationContextID& presID )
{
  if (m_assoc == NULL)
    return ASC_NULLKEY;
  OFCondition cond;
  Uint16 pcid = presID;

  /* If necessary, find appropriate presentation context */
  if (pcid == 0)
    pcid = findPresContID(UID_VerificationSOPClass, UID_LittleEndianImplicitTransferSyntax);
  if (pcid == 0)
  {
    DCMNET_ERROR("No presentation context found for sending C-ECHO with SOP Class/TS: " 
      << dcmFindNameOfUID(UID_VerificationSOPClass) << "/"
      << DcmXfer(UID_LittleEndianImplicitTransferSyntax).getXferName());
    return EC_IllegalCall;
  }

  /* Now, assemble dimse message */
  Uint16 status;
  cond = DIMSE_echoUser(m_assoc, nextMessageID(), m_blockMode, m_dimseTimeout, &status, NULL);
  if (cond.bad())
  {  
    DCMNET_ERROR("Failed sending C-ECHO request or receiving response: " << cond.text());
    return cond;
  }
  else
  {
    if (status == STATUS_Success) 
      DCMNET_DEBUG("Successfully sent C-ECHO request");
    else
    {
      DCMNET_ERROR("C-ECHO failed with status code: " << status);
      return makeOFCondition(OFM_dcmnet, 22, OF_error, "SCP returned non-success status in C-ECHO response");
    }
  }
  return EC_Normal;
}


// Sends C-ECHO request to another DICOM application
OFCondition DcmSCU::sendSTORERequest(const T_ASC_PresentationContextID& presID,
                                     const OFString& dicomFile,
                                     DcmDataset *dset,
                                     DcmDataset*& rspCommandSet,
                                     DcmDataset*& rspStatusDetail,
                                     Uint16& rspStatusCode)
{
  Uint8 pcid = presID;
  if (m_assoc == NULL)
    return ASC_NULLKEY;
  OFCondition cond;
  DcmDataset* statusDetail = NULL;
  T_DIMSE_Message msg;
  T_DIMSE_C_StoreRQ* req = &(msg.msg.CStoreRQ);
  
  // Set type of message
  msg.CommandField = DIMSE_C_STORE_RQ;
  /* Set message ID */
  req->MessageID = nextMessageID();
  /* Load file if necessary */
  OFString sopClass, sopInstance;
  E_TransferSyntax ts;
  DcmFileFormat *dcmff = NULL;
  if (!dicomFile.empty())
  {
    dcmff = new DcmFileFormat();
    if (dcmff == NULL) return EC_MemoryExhausted;
    cond = dcmff->loadFile(dicomFile.c_str());
    if (cond.bad())
      return cond;
    dset = dcmff->getDataset();
  }

  /* Fill message according to dataset to be sent */

  cond = getDatasetInfo(dset, sopClass, sopInstance, ts);
  if ( (dset == NULL) || sopClass.empty() || sopInstance.empty() || (ts == EXS_Unknown))
  {
    DCMNET_ERROR("Cannot send DICOM file, missing information:");
    DCMNET_ERROR("  SOP Class UID: " << sopClass);
    DCMNET_ERROR("  SOP Instance UID: " << sopInstance);
    DCMNET_ERROR("  Transfersyntax: " << DcmXfer(ts).getXferName());
    delete dcmff; dcmff = NULL;
    return EC_IllegalParameter;
  }
  strncpy(req->AffectedSOPClassUID, sopClass.c_str(), sopClass.length()+1);
  strncpy(req->AffectedSOPInstanceUID, sopInstance.c_str(), sopInstance.length()+1);
  req->DataSetType = DIMSE_DATASET_PRESENT;

  /* If necessary, find appropriate presentation context */
  if (pcid == 0)
    pcid = findPresContID(sopClass, DcmXfer(ts).getXferID());
  if (pcid == 0)
  {
    OFString sopname = dcmFindNameOfUID(sopClass.c_str());
    OFString tsname = DcmXfer(ts).getXferName();
    DCMNET_ERROR("No presentation context found for sending C-STORE with SOP Class/TS: " 
      << (sopname.empty() ? sopClass : sopname) << "/"
      << (tsname.empty() ? DcmXfer(ts).getXferName() : tsname));
    return EC_IllegalCall;
  }

  /* Send request */

  cond = DIMSE_sendMessageUsingMemoryData(m_assoc, pcid, &msg, statusDetail, dset, NULL, NULL, NULL);
  delete dcmff; dcmff = NULL;
  if (cond == EC_Normal) 
  {
    DCMNET_DEBUG("Successfully sent C-STORE request");
  }
  else
  {  
    DCMNET_ERROR("Failed sending C-STORE request" << cond.text());
     return cond;
  }
  
  /* Receive response */
  
  T_DIMSE_Message rsp;
  cond = receiveDIMSEResponse(&pcid, &rsp, &statusDetail, NULL /* not interested in the command set */);
  if (cond.bad())
    return cond;
  
  DCMNET_DEBUG("Received response (probably C-STORE-RSP) on presentation context " << pcid);
  if (rsp.CommandField != DIMSE_C_STORE_RSP)
  {
    DCMNET_ERROR("Expected C-STORE-RSP but received DIMSE command " << rsp.CommandField);
    return cond;
  }
  T_DIMSE_C_StoreRSP storeRsp = msg.msg.CStoreRSP;
  DCMNET_DEBUG("Response has status " << storeRsp.DimseStatus);
  if (statusDetail != NULL)
  {
    DCMNET_DEBUG("Response has status detail:" << DcmObject::PrintHelper(*statusDetail));
    delete statusDetail;
  }
  else
  {
    DCMNET_DEBUG("Response has status detail: none");
  }

  return cond;
}

/*
 * This function sends a DIMSE command and possibly also instance data from a data object via network
 * to another DICOM application.
 *
 * Parameters:
 *   presID          - [in] Presentation context ID identifying the context to be used
 *   msg             - [in] Structure that represents a certain DIMSE command which shall be sent.
 *   statusDetail    - [in] Detailed information with regard to the status information which is captured
 *                          in the status element (0000,0900). Note that the value for element (0000,0900)
 *                          is contained in this variable.
 *   dataObject      - [in] The instance data which shall be sent to the other DICOM application,
 *                          NULL, if there is none
 *   callback        - [in] Pointer to a function which shall be called to indicate progress.
 *   callbackContext - [in] Pointer to data which shall be passed to the progress indicating function
 *   commandSet      - [out] [optional parameter, default = NULL] If this parameter is not NULL
 *                           it will return a copy of the DIMSE command which is sent to the other
 *                           DICOM application.
 */
OFCondition DcmSCU::sendDIMSERequest( const T_ASC_PresentationContextID& presID,
                                      T_DIMSE_Message *msg,
                                      /*DcmDataset *statusDetail,*/ // Brauche ich nur bei RSP nachrichten oder?
                                      DcmDataset *dataObject,
                                      DIMSE_ProgressCallback callback,
                                      void *callbackContext,
                                      DcmDataset **commandSet)
{
  OFCondition cond;
  if (msg == NULL)
    return DIMSE_NULLKEY;
  
  T_ASC_PresentationContextID id=presID; //TODO
  /* call the according DIMSE function to sent the message */
  cond = DIMSE_sendMessageUsingMemoryData(m_assoc, id, msg, NULL/*statusDetail*/, dataObject,
                                          callback, callbackContext, commandSet);
  if (cond.good())
  {
    if (m_openDIMSERequest == NULL)
      delete m_openDIMSERequest;
    m_openDIMSERequest = new T_DIMSE_Message;
    bzero((char*)m_openDIMSERequest, sizeof(*m_openDIMSERequest)); // TODO: notwendig?
    memcpy((char*)m_openDIMSERequest, msg, sizeof(*m_openDIMSERequest));
  }

  return cond;
}

/*   presId       - [out] Contains in the end the ID of the presentation context which was specified in the DIMSE command.
 *   msg          - [out] Contains in the end information which represents a certain DIMSE command which was received.
 *   statusDetail - [out] If a non-NULL value is passed this variable will in the end contain detailed
 *                        information with regard to the status information which is captured in the status
 *                        element (0000,0900). Note that the value for element (0000,0900) is not contained
 *                        in this return value but in msg. For details on the structure of this object, see
 *                        DICOM standard (year 2000) part 7, annex C) (or the corresponding section in a later
 *                        version of the standard.)
 *   commandSet   - [out] [optional parameter, default = NULL] If this parameter is not NULL
 *                        it will return a copy of the DIMSE command which was received from the other
 *                        DICOM application.
 */
OFCondition DcmSCU::receiveDIMSEResponse( T_ASC_PresentationContextID *presID,
                                          T_DIMSE_Message *msg,
                                          DcmDataset **statusDetail,
                                          DcmDataset **commandSet)
{
  if (m_assoc == NULL)
    return DIMSE_NULLKEY;
  OFCondition cond;
  cond = DIMSE_receiveCommand(m_assoc, m_blockMode, m_dimseTimeout, presID,
                              msg, statusDetail, commandSet);
  return cond;

}

/*
 * This function revceives one data set (of instance data) via network from another DICOM application.
 *
 * Parameters:
 *   assoc           - [in] The association (network connection to another DICOM application).
 *   blocking        - [in] The blocking mode for receiving data (either DIMSE_BLOCKING or DIMSE_NONBLOCKING)
 *   timeout         - [in] Timeout interval for receiving data (if the blocking mode is DIMSE_NONBLOCKING).
 *   presID          - [out] Contains in the end the ID of the presentation context which was used in the PDVs
 *                          that were received on the network. If the PDVs show different presentation context
 *                          IDs, this function will return an error.
 *   dataObject      - [out] Contains in the end the information which was received over the network.
 *                          Note that this function assumes that either imageFileName or imageDataSet does not equal NULL.
 *   callback        - [in] Pointer to a function which shall be called to indicate progress.
 *   callbackData    - [in] Pointer to data which shall be passed to the progress indicating function
 */
OFCondition DcmSCU::receiveDIMSEDataset( T_ASC_PresentationContextID *presID,
                                         DcmDataset **dataObject,
                                         DIMSE_ProgressCallback callback,
                                         void *callbackContext)
{
  if (m_assoc == NULL)
    return DIMSE_NULLKEY;
  OFCondition cond;
  cond = DIMSE_receiveDataSetInMemory( m_assoc, m_blockMode, m_dimseTimeout, presID,
                                       dataObject, callback, callbackContext);
  return cond;

}


void DcmSCU::setMaxReceivePDULength(const unsigned long& maxRecPDU)
{
  m_maxReceivePDULength = maxRecPDU;
}


void DcmSCU::setDIMSEBlockingMode(const T_DIMSE_BlockingMode& blockingMode)
{
  m_blockMode = blockingMode;
}


void DcmSCU::setAETitle(const OFString& myAETtitle)
{
  m_ourAETitle = myAETtitle;
}


void DcmSCU::setPeerHostName(const OFString& peerHostName)
{
  m_peer = peerHostName;
}


void DcmSCU::setPeerAETitle(const OFString& peerAETitle)
{
  m_peerAETitle = peerAETitle;
}


void DcmSCU::setPeerPort(const Uint16 peerPort)
{
  m_peerPort = peerPort;
}


void DcmSCU::setDIMSETimeout(const Uint16 dimseTimeout)
{
  m_dimseTimeout = dimseTimeout;
}


void DcmSCU::setACSETimeout(const Uint16 acseTimeout)
{
  m_acseTimeout = acseTimeout;
}


void DcmSCU::setAssocConfigFileAndProfile(const OFString& filename,
                                          const OFString& profile )
{
  m_assocConfigFilename = filename;
  m_assocConfigProfile = profile;
}


/* Get methods */

OFBool DcmSCU::isConnected() const
{
  return (m_assoc != NULL);
}

Uint32 DcmSCU::getMaxReceivePDULength() const
{
  return m_maxReceivePDULength;
}


OFBool DcmSCU::getTLSEnabled() const
{
  return OFFalse;
}


T_DIMSE_BlockingMode DcmSCU::getDIMSEBlockingMode() const
{
  return m_blockMode;
}


OFString DcmSCU::getAETitle() const
{
  return m_ourAETitle;
}


OFString DcmSCU::getPeerHostName() const
{
  return m_peer;
}


OFString DcmSCU::getPeerAETitle() const
{
  return m_peerAETitle;
}


Uint16 DcmSCU::getPeerPort() const
{
  return m_peerPort;
}


Uint16 DcmSCU::getDIMSETimeout() const
{
  return m_dimseTimeout;
}


Uint16 DcmSCU::getACSETimeout() const
{
  return m_acseTimeout;
}


OFCondition DcmSCU::getDatasetInfo(DcmDataset* dataset,
                                   OFString& sopClassUID,
                                   OFString& sopInstanceUID,
                                   E_TransferSyntax& transferSyntax)
{
  if (dataset == NULL)
    return EC_IllegalCall;
  dataset->findAndGetOFString(DCM_SOPClassUID, sopClassUID);
  dataset->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);
  transferSyntax = dataset->getOriginalXfer();
  return EC_Normal;
}


/*
** CVS Log
** $Log: scu.cc,v $
** Revision 1.4  2010-04-29 16:13:25  onken
** Made SCU class independent from dcmtls, i.e. outsourced TLS API. Added
** direct API support for sending C-STORE requests. Further API changes and
** some bugs fixed.
**
** Revision 1.3  2009-12-21 15:33:58  onken
** Added documentation and refactored / enhanced some code.
**
** Revision 1.2  2009-12-17 09:12:27  onken
** Fixed other scu and scp base class compile issues.
**
** Revision 1.1  2009-12-16 17:05:35  onken
** Added base classes for SCU and SCP implementation.
**
** Revision 1.5  2009-12-02 14:26:05  uli
** Stop including dcdebug.h which was removed.
**
** Revision 1.4  2009-11-18 12:37:28  uli
** Fix compiler errors due to removal of DUL_Debug() and DIMSE_Debug().
**
** Revision 1.3  2009-01-08 18:25:34  joergr
** Replaced further OFListIterator() by OFListConstIterator() in order to
** compile when STL list classes are used.
**
** Revision 1.2  2009-01-08 13:33:31  joergr
** Replaced OFListIterator() by OFListConstIterator() in order to compile when
** STL list classes are used.
**
** Revision 1.1  2008-09-29 13:51:55  onken
** Initial checkin of module dcmppscu implementing an MPPS commandline client.
**
**
**
*/
