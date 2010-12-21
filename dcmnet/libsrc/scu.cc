/*
 *
 *  Copyright (C) 2008-2010, OFFIS e.V.
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
 *  Purpose: Base class for Service Class Users (SCUs)
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2010-12-21 09:37:36 $
 *  CVS/RCS Revision: $Revision: 1.16 $
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
  m_assocConfigFilename(),
  m_assocConfigProfile(),
  m_presContexts(),
  m_openDIMSERequest(NULL),
  m_maxReceivePDULength(ASC_DEFAULTMAXPDU),
  m_blockMode(DIMSE_BLOCKING),
  m_ourAETitle("ANY-SCU"),
  m_peer(),
  m_peerAETitle("ANY-SCP"),
  m_peerPort(104),
  m_dimseTimeout(0),
  m_acseTimeout(30),
  m_verbosePCMode(OFFalse)
{

#ifdef HAVE_GUSI_H
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
  WSAData winSockData;
  /* we need at least version 1.1 */
  WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
  WSAStartup(winSockVersionNeeded, &winSockData); // TODO: check with multiple SCU instances whether this is harmful
#endif
}


DcmSCU::~DcmSCU()
{
  // abort association (if any) and destroy dcmnet data structures
  if (isConnected())
  {
    closeAssociation(DCMSCU_ABORT_ASSOCIATION);
  } else {
    if ((m_assoc != NULL) || (m_net != NULL))
      DCMNET_DEBUG("Cleaning up internal association and network structures");
    ASC_destroyAssociation(&m_assoc);
    ASC_dropNetwork(&m_net);
  }
  // free memory allocated by this class
  delete m_openDIMSERequest;

#ifdef HAVE_WINSOCK_H
  WSACleanup(); // TODO: check with multiple SCU instances whether this is harmful
#endif
}


OFCondition DcmSCU::initNetwork()
{
  // TODO: do we need to check whether the network is already initialized?
  OFString tempStr;
  /* initialize network, i.e. create an instance of T_ASC_Network*. */
  OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, m_acseTimeout, &m_net);
  if (cond.bad())
  {
    DimseCondition::dump(tempStr, cond);
    DCMNET_ERROR(tempStr);
    return cond;
  }

  /* initialize asscociation parameters, i.e. create an instance of T_ASC_Parameters*. */
  cond = ASC_createAssociationParameters(&m_params, m_maxReceivePDULength);
  if (cond.bad())
  {
    DCMNET_ERROR(DimseCondition::dump(tempStr, cond));
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
  if ((m_peer.length() + 5 /* max 65535 */) + 1 /* for ":" */ > 63)
  {
    DCMNET_ERROR("Maximum length of peer host name '" << m_peer << "' is longer than maximum of 57 characters");
    return EC_IllegalCall; // TODO: need to find better error code
  }
  if (strlen(localHost) + 1 > 63)
  {
    DCMNET_ERROR("Maximum length of local host name '" << localHost << " is longer than maximum of 62 characters");
    return EC_IllegalCall; // TODO: need to find better error code
  }
  sprintf(peerHost, "%s:%d", m_peer.c_str(), OFstatic_cast(int, m_peerPort));
  ASC_setPresentationAddresses(m_params, localHost, peerHost);

  /* Add presentation contexts */

  // First, import from config file, if specified
  OFCondition result;
  if (m_assocConfigFilename.length() != 0)
  {
    DcmAssociationConfiguration assocConfig;
    result = DcmAssociationConfigurationFile::initialize(assocConfig, m_assocConfigFilename.c_str());
    if (result.bad())
    {
      DCMNET_WARN("Unable to parse association configuration file " << m_assocConfigFilename
        << " (ignored): " << result.text());
      return result;
    }
    else
    {
      /* perform name mangling for config file key */
      OFString profileName;
      const unsigned char *c = OFreinterpret_cast(const unsigned char *, m_assocConfigProfile.c_str());
      while (*c)
      {
        if (! isspace(*c)) profileName += OFstatic_cast(char, toupper(*c));
        ++c;
      }

      result = assocConfig.setAssociationParameters(profileName.c_str(), *m_params);
      if (result.bad())
      {
        DCMNET_WARN("Unable to apply association configuration file" << m_assocConfigFilename
          <<" (ignored): " << result.text());
        return result;
      }
    }
  }

  // Adapt presentation context ID to existing presentation contexts
  // It's important that presentation context ids are numerated 1,3,5,7...!
  Uint32 nextFreePresID = 257;
  Uint32 numContexts = ASC_countPresentationContexts(m_params);
  if (numContexts <= 127)
  {
    // Need Uint16 to avoid overflow in currPresID (unsigned char)
    nextFreePresID = 2 * numContexts + 1; /* add 1 to point to the next free ID*/
  }
  // Print warning if number of overall presenation contexts exceeds 128
  if ((numContexts + m_presContexts.size()) > 128)
  {
    DCMNET_WARN("Number of presentation contexts exceeds 128 (" << numContexts + m_presContexts.size()
      << "). Some contexts will not be negotiated");
  }
  else
  {
    DCMNET_TRACE("Configured " << numContexts << " presentation contexts from config file");
    if (m_presContexts.size() > 0)
        DCMNET_TRACE("Adding another " << m_presContexts.size() << " presentation contexts configured for SCU");
  }

  // Add presentation contexts not originating from config file
  OFListIterator(DcmSCUPresContext) contIt = m_presContexts.begin();
  OFListConstIterator(DcmSCUPresContext) endOfContList = m_presContexts.end();
  while ((contIt != endOfContList) && (nextFreePresID <= 255))
  {
    const Uint16 numTransferSyntaxes = (*contIt).transferSyntaxes.size();
    const char** transferSyntaxes = new const char*[numTransferSyntaxes];

    // Iterate over transfer syntaxes within one presentation context
    OFListIterator(OFString) syntaxIt = (*contIt).transferSyntaxes.begin();
    OFListIterator(OFString) endOfSyntaxList = (*contIt).transferSyntaxes.end();
    Uint16 sNum = 0;
    // copy all transfersyntaxes to array
    while (syntaxIt != endOfSyntaxList)
    {
      transferSyntaxes[sNum] = (*syntaxIt).c_str();
      ++syntaxIt;
      ++sNum;
    }

    // add the presentation context
    cond = ASC_addPresentationContext(m_params, OFstatic_cast(Uint8, nextFreePresID),
      (*contIt).abstractSyntaxName.c_str(), transferSyntaxes, numTransferSyntaxes);
    // if adding was successfull, prepare pres. context ID for next addition
    delete[] transferSyntaxes;
    transferSyntaxes = NULL;
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
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  }
  DCMNET_DEBUG("Configured a total of " << numContexts << " presentation contexts for SCU");

  return cond;
}


OFCondition DcmSCU::negotiateAssociation()
{
  /* dump presentation contexts if required */
  OFString tempStr;
  if (m_verbosePCMode)
    DCMNET_INFO("Request Parameters:" << OFendl << ASC_dumpParameters(tempStr, m_params, ASC_ASSOC_RQ));
  else
    DCMNET_DEBUG("Request Parameters:" << OFendl << ASC_dumpParameters(tempStr, m_params, ASC_ASSOC_RQ));

  /* create association, i.e. try to establish a network connection to another */
  /* DICOM application. This call creates an instance of T_ASC_Association*. */
  DCMNET_INFO("Requesting Association");
  OFCondition cond = ASC_requestAssociation(m_net, m_params, &m_assoc);
  if (cond.bad())
  {
    if (cond == DUL_ASSOCIATIONREJECTED)
    {
      T_ASC_RejectParameters rej;

      ASC_getRejectParameters(m_params, &rej);
      DCMNET_DEBUG("Association Rejected:" << OFendl << ASC_printRejectParameters(tempStr, &rej));
      return cond;
    }
    else
    {
      DCMNET_DEBUG("Association Request Failed: " << DimseCondition::dump(tempStr, cond));
      return cond;
    }
  }

  /* dump the presentation contexts which have been accepted/refused */
  if (m_verbosePCMode)
    DCMNET_INFO("Association Parameters Negotiated:" << OFendl << ASC_dumpParameters(tempStr, m_params, ASC_ASSOC_AC));
  else
    DCMNET_DEBUG("Association Parameters Negotiated:" << OFendl << ASC_dumpParameters(tempStr, m_params, ASC_ASSOC_AC));

  /* count the presentation contexts which have been accepted by the SCP */
  /* If there are none, finish the execution */
  if (ASC_countAcceptedPresentationContexts(m_params) == 0)
  {
    DCMNET_ERROR("No Acceptable Presentation Contexts");
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  }

  /* dump general information concerning the establishment of the network connection if required */
  DCMNET_INFO("Association Accepted (Max Send PDV: " << OFstatic_cast(unsigned long, m_assoc->sendPDVLength) << ")");
  return EC_Normal;
}


OFCondition DcmSCU::addPresentationContext(const OFString &abstractSyntax,
                                           const OFList<OFString> &xferSyntaxes)

{

  DcmSCUPresContext presContext;
  presContext.abstractSyntaxName = abstractSyntax;
  OFListConstIterator(OFString) it = xferSyntaxes.begin();
  OFListConstIterator(OFString) endOfList = xferSyntaxes.end();
  while (it != endOfList)
  {
    presContext.transferSyntaxes.push_back(*it);
    it++;
  }
  m_presContexts.push_back(presContext);
  return EC_Normal;
}


OFCondition DcmSCU::useSecureConnection(DcmTransportLayer *tlayer)
{
  OFCondition cond = ASC_setTransportLayer(m_net, tlayer, OFFalse /* do not take over ownership */);
  if (cond.good())
    cond = ASC_setTransportLayerType(m_params, OFTrue /* use TLS */);
  return cond;
}


// Reads association configuration from config file
OFCondition readAssocConfigFromFile(const OFString &filename,
                                    const OFString & /* profile */)
{
  DcmAssociationConfiguration assocConfig;
  OFCondition result = DcmAssociationConfigurationFile::initialize(assocConfig, filename.c_str());
  return result;
}


// Returns usable presentation context ID for given abstract syntax and UID
// transfer syntax UID. 0 if none matches.
T_ASC_PresentationContextID DcmSCU::findPresentationContextID(const OFString &abstractSyntax,
                                                              const OFString &transferSyntax)
{
  if (m_assoc == NULL)
    return 0;

  DUL_PRESENTATIONCONTEXT *pc;
  LST_HEAD **l;
  OFBool found = OFFalse;

  if (abstractSyntax.empty()) return 0;

  /* first of all we look for a presentation context
   * matching both abstract and transfer syntax
   */
  l = &m_assoc->params->DULparams.acceptedPresentationContext;
  pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
  (void)LST_Position(l, (LST_NODE*)pc);
  while (pc && !found)
  {
      found = (strcmp(pc->abstractSyntax, abstractSyntax.c_str()) == 0);
      found &= (pc->result == ASC_P_ACCEPTANCE);
      if (!transferSyntax.empty())  // ignore transfer syntax if not specified
        found &= (strcmp(pc->acceptedTransferSyntax, transferSyntax.c_str()) == 0);
      if (!found) pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
  }
  if (found) return pc->presentationContextID;

  return 0;   /* not found */
}


void DcmSCU::findPresentationContext(const T_ASC_PresentationContextID presID,
                                     OFString &abstractSyntax,
                                     OFString &transferSyntax)
{
  transferSyntax.clear();
  abstractSyntax.clear();
  if (m_assoc == NULL)
    return;

  DUL_PRESENTATIONCONTEXT *pc;
  LST_HEAD **l;

  /* first of all we look for a presentation context
   * matching both abstract and transfer syntax
   */
  l = &m_assoc->params->DULparams.acceptedPresentationContext;
  pc = (DUL_PRESENTATIONCONTEXT*) LST_Head(l);
  (void)LST_Position(l, (LST_NODE*)pc);
  while (pc)
  {
     if ((presID == pc->presentationContextID) && (pc->result == ASC_P_ACCEPTANCE))
     {
       // found a match
       transferSyntax = pc->acceptedTransferSyntax;
       abstractSyntax = pc->abstractSyntax;
       return;
     }
     pc = (DUL_PRESENTATIONCONTEXT*) LST_Next(l);
  }
  return;   /* not found */
}


Uint16 DcmSCU::nextMessageID()
{
  if (m_assoc == NULL)
    return 0;
  else
    return m_assoc->nextMsgID++;
}


void DcmSCU::closeAssociation(const DcmCloseAssociationType closeType)
{
  OFCondition cond;
  OFString tempStr;

  /* tear down association, i.e. terminate network connection to SCP */
  switch (closeType)
  {
    case DCMSCU_RELEASE_ASSOCIATION:
      /* release association */
      DCMNET_INFO("Releasing Association");
      cond = ASC_releaseAssociation(m_assoc);
      if (cond.bad())
      {
        DCMNET_ERROR("Association Release Failed: " << DimseCondition::dump(tempStr, cond));
        return; // TODO: do we really need this?
      }
      break;
    case DCMSCU_ABORT_ASSOCIATION:
      /* abort association */
      DCMNET_INFO("Aborting Association");
      cond = ASC_abortAssociation(m_assoc);
      if (cond.bad())
      {
        DCMNET_ERROR("Association Abort Failed: " << DimseCondition::dump(tempStr, cond));
      }
      break;
    case DCMSCU_PEER_REQUESTED_RELEASE:
      /* peer requested release */
      DCMNET_ERROR("Protocol Error: Peer requested release (Aborting)");
      DCMNET_INFO("Aborting Association");
      cond = ASC_abortAssociation(m_assoc);
      if (cond.bad())
      {
        DCMNET_ERROR("Association Abort Failed: " << DimseCondition::dump(tempStr, cond));
      }
      break;
    case DCMSCU_PEER_ABORTED_ASSOCIATION:
      /* peer aborted association */
      DCMNET_INFO("Peer Aborted Association");
      break;
  }

  /* destroy the association, i.e. free memory of T_ASC_Association* structure. This */
  /* call is the counterpart of ASC_requestAssociation(...) which was called above. */
  cond = ASC_destroyAssociation(&m_assoc);
  if (cond.bad())
  {
    DCMNET_ERROR("Unable to clean up internal association structures: " << DimseCondition::dump(tempStr, cond));
  }

  /* drop the network, i.e. free memory of T_ASC_Network* structure. This call */
  /* is the counterpart of ASC_initializeNetwork(...) which was called above. */
  cond = ASC_dropNetwork(&m_net);
  if (cond.bad())
  {
    DCMNET_ERROR("Unable to clean up internal network structures: " << DimseCondition::dump(tempStr, cond));
  }
}


// Sends C-ECHO request to another DICOM application
OFCondition DcmSCU::sendECHORequest(const T_ASC_PresentationContextID presID)
{
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  T_ASC_PresentationContextID pcid = presID;

  /* If necessary, find appropriate presentation context */
  if (pcid == 0)
    pcid = findPresentationContextID(UID_VerificationSOPClass, UID_LittleEndianExplicitTransferSyntax);
  if (pcid == 0)
    pcid = findPresentationContextID(UID_VerificationSOPClass, UID_BigEndianExplicitTransferSyntax);
  if (pcid == 0)
    pcid = findPresentationContextID(UID_VerificationSOPClass, UID_LittleEndianImplicitTransferSyntax);
  if (pcid == 0)
  {
    DCMNET_ERROR("No presentation context found for sending C-ECHO with SOP Class / Transfer Syntax: "
      << dcmFindNameOfUID(UID_VerificationSOPClass, "") << "/"
      << DcmXfer(UID_LittleEndianImplicitTransferSyntax).getXferName());
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  }

  /* Now, assemble dimse message */
  Uint16 status;
  cond = DIMSE_echoUser(m_assoc, nextMessageID(), m_blockMode, m_dimseTimeout, &status, NULL);
  if (cond.bad())
  {
    OFString tempStr;
    DCMNET_ERROR("Failed sending C-ECHO request or receiving response: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }
  else
  {
    if (status == STATUS_Success)
      DCMNET_DEBUG("Successfully sent C-ECHO request");
    else
    {
      DCMNET_ERROR("C-ECHO failed with status code: 0x"
        << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4) << status);
      return makeOFCondition(OFM_dcmnet, 22, OF_error, "SCP returned non-success status in C-ECHO response");
    }
  }
  return EC_Normal;
}


// Sends C-STORE request to another DICOM application
OFCondition DcmSCU::sendSTORERequest(const T_ASC_PresentationContextID presID,
                                     const OFString &dicomFile,
                                     DcmDataset *dset,
                                     DcmDataset *& /* rspCommandSet */,    // TODO
                                     DcmDataset *& /* rspStatusDetail */,  // TODO
                                     Uint16 &rspStatusCode)
{
  // Do some basic validity checks
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID pcid = presID;
  DcmDataset* statusDetail = NULL;
  T_DIMSE_Message msg;
  T_DIMSE_C_StoreRQ* req = &(msg.msg.CStoreRQ);

  // Set type of message
  msg.CommandField = DIMSE_C_STORE_RQ;
  /* Set message ID */
  req->MessageID = nextMessageID();
  /* Load file if necessary */
  OFString sopClass, sopInstance;
  E_TransferSyntax transferSyntax = EXS_Unknown; // Initialized in getDatasetInfo()
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

  cond = getDatasetInfo(dset, sopClass, sopInstance, transferSyntax);
  if ((dset == NULL) || sopClass.empty() || sopInstance.empty() || (transferSyntax == EXS_Unknown))
  {
    DCMNET_ERROR("Cannot send DICOM file, missing information:");
    DCMNET_ERROR("  SOP Class UID: " << sopClass);
    DCMNET_ERROR("  SOP Instance UID: " << sopInstance);
    DCMNET_ERROR("  Transfersyntax: " << DcmXfer(transferSyntax).getXferName());
    delete dcmff;
    dcmff = NULL;
    return EC_IllegalParameter;
  }
  OFStandard::strlcpy(req->AffectedSOPClassUID, sopClass.c_str(), sizeof(req->AffectedSOPClassUID));
  OFStandard::strlcpy(req->AffectedSOPInstanceUID, sopInstance.c_str(), sizeof(req->AffectedSOPInstanceUID));
  req->DataSetType = DIMSE_DATASET_PRESENT;

  /* If necessary, find appropriate presentation context */
  if (pcid == 0)
    pcid = findPresentationContextID(sopClass, DcmXfer(transferSyntax).getXferID());
  if (pcid == 0)
  {
    OFString sopname = dcmFindNameOfUID(sopClass.c_str(), sopClass.c_str());
    OFString tsname = DcmXfer(transferSyntax).getXferName();
    DCMNET_ERROR("No presentation context found for sending C-STORE with SOP Class / Transfer Syntax: "
      << sopname << "/"
      << (tsname.empty() ? DcmXfer(transferSyntax).getXferName() : tsname));
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  }

  /* Send request */

  DCMNET_INFO("Send C-STORE Request");
  DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, msg, DIMSE_OUTGOING, NULL, pcid));
  cond = sendDIMSEMessage(pcid, &msg, dset, NULL /* callback */, NULL /* callbackContext */);
  delete dcmff;
  dcmff = NULL;
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending C-STORE request: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }

  /* Receive response */

  T_DIMSE_Message rsp;
  cond = receiveDIMSECommand(&pcid, &rsp, &statusDetail, NULL /* not interested in the command set */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed receiving DIMSE response: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }

  if (rsp.CommandField == DIMSE_C_STORE_RSP)
  {
    DCMNET_INFO("Received C-STORE Response");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, rsp, DIMSE_INCOMING, NULL, pcid));
  } else {
    DCMNET_ERROR("Expected C-STORE response but received DIMSE command 0x"
        << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
        << OFstatic_cast(unsigned int, rsp.CommandField));
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, rsp, DIMSE_INCOMING, NULL, pcid));
    return DIMSE_BADCOMMANDTYPE;
  }
  T_DIMSE_C_StoreRSP storeRsp = rsp.msg.CStoreRSP;
  rspStatusCode = storeRsp.DimseStatus;
  if (statusDetail != NULL)
  {
    DCMNET_DEBUG("Response has status detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
    delete statusDetail;
  }

  return cond;
}


// Sends a C-FIND Request on given presentation context
OFCondition DcmSCU::sendFINDRequest(const T_ASC_PresentationContextID presID,
                                    DcmDataset *queryKeys,
                                    FINDResponses *responses)
{
  // Do some basic validity checks
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;
  if (queryKeys == NULL)
    return ASC_NULLKEY;

  /* Prepare DIMSE data structures for issuing request */
  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID pcid = presID;
  T_DIMSE_Message msg;
  DcmDataset* statusDetail = NULL;
  T_DIMSE_C_FindRQ* req = &(msg.msg.CFindRQ);
  // Set type of message
  msg.CommandField = DIMSE_C_FIND_RQ;
  // Set message ID
  req->MessageID = nextMessageID();
  // Announce dataset
  req->DataSetType = DIMSE_DATASET_PRESENT;

  // Determine SOP Class from presentation context
  OFString abstractSyntax, transferSyntax;
  findPresentationContext(pcid, abstractSyntax, transferSyntax);
  if (abstractSyntax.empty() || transferSyntax.empty())
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  OFStandard::strlcpy(req->AffectedSOPClassUID, abstractSyntax.c_str(), sizeof(req->AffectedSOPClassUID));

  DCMNET_INFO("Send C-FIND Request");
  DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, msg, DIMSE_OUTGOING, NULL, pcid));
  cond = sendDIMSEMessage(pcid, &msg, queryKeys, NULL /* callback */, NULL /* callbackContext */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending C-FIND request: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }

  /* Receive and handle response */
  OFBool waitForNextResponse = OFTrue;
  while (waitForNextResponse)
  {
    T_DIMSE_Message rsp;
    statusDetail = NULL;

    // Receive command set
    cond = receiveDIMSECommand(&pcid, &rsp, &statusDetail, NULL /* not interested in the command set */);
    if (cond.bad())
    {
      DCMNET_ERROR("Failed receiving DIMSE response: " << DimseCondition::dump(tempStr, cond));
      return cond;
    }

    if (rsp.CommandField == DIMSE_C_FIND_RSP)
    {
      DCMNET_INFO("Received C-FIND Response");
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, rsp, DIMSE_INCOMING, NULL, pcid));
    } else {
      DCMNET_ERROR("Expected C-FIND response but received DIMSE command 0x"
          << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
          << OFstatic_cast(unsigned int, rsp.CommandField));
      DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, rsp, DIMSE_INCOMING, NULL, pcid));
      return DIMSE_BADCOMMANDTYPE;
    }

    // Prepare response package for response handler
    FINDResponse *findrsp = new FINDResponse();
    findrsp->m_affectedSOPClassUID = rsp.msg.CFindRSP.AffectedSOPClassUID;
    findrsp->m_messageIDRespondedTo = rsp.msg.CFindRSP.MessageIDBeingRespondedTo;
    findrsp->m_status = rsp.msg.CFindRSP.DimseStatus;
    DCMNET_DEBUG("Response has status " << findrsp->m_status);

    // Receive dataset if there is one (status PENDING)
    DcmDataset *rspDataset = NULL;
    if (DICOM_PENDING_STATUS(findrsp->m_status))
    {
      // Check if dataset is announced correctly
      if (rsp.msg.CFindRSP.DataSetType == DIMSE_DATASET_NULL)
      {
        DCMNET_ERROR("Received C-FIND response with PENDING status but no dataset announced, aborting");
        delete findrsp;
        return DIMSE_BADMESSAGE;
      }

      // Receive dataset
      cond = receiveDIMSEDataset(&pcid, &rspDataset, NULL /* callback */, NULL /* callbackContext */);
      if (cond.bad())
      {
        DCMNET_ERROR("Unable to receive C-FIND dataset on presentation context "
          << OFstatic_cast(unsigned int, pcid) << ": " << DimseCondition::dump(tempStr, cond));
        delete findrsp;
        return DIMSE_BADDATA;
      }
      DCMNET_DEBUG("Received dataset on presentation context " << OFstatic_cast(unsigned int, pcid));
      findrsp->m_dataset = rspDataset;
    }

    // Handle C-FIND response (has to handle all possible status flags)
    cond = handleFINDResponse(pcid, findrsp, waitForNextResponse);
    if (cond.bad())
    {
      DCMNET_WARN("Unable to handle C-FIND response correctly: " << cond.text() << " (ignored)");
      delete findrsp;
      cond = EC_Normal;
      // don't return here but trust the "waitForNextResponse" variable
    }
    // if response could be handled successfully, add it to response list
    else
    {
      if (responses != NULL) // only add if desired by caller
        responses->add(findrsp);
    }
  }
  /* All responses received or break signal occured */
  return EC_Normal;
}


// Standard handler for C-FIND message responses
OFCondition DcmSCU::handleFINDResponse(Uint16 /* presContextID */,
                                       FINDResponse *response,
                                       OFBool &waitForNextResponse)
{
  DCMNET_DEBUG("Handling C-FIND Response");
  switch (response->m_status) {
    case STATUS_Pending:
    case STATUS_FIND_Pending_WarningUnsupportedOptionalKeys:
      /* in this case the current C-FIND-RSP indicates that */
      /* there will be some more results */
      waitForNextResponse = OFTrue;
      DCMNET_DEBUG("One or more outstanding C-FIND responses");
      break;
    case STATUS_Success:
      /* in this case the current C-FIND-RSP indicates that */
      /* there are no more records that match the search mask */
      waitForNextResponse = OFFalse;
      DCMNET_DEBUG("Received final C-FIND response, no more C-FIND responses expected");
      break;
    default:
      /* in all other cases, don't expect further responses to come */
      waitForNextResponse = OFFalse;
      DCMNET_DEBUG("Status tells not to wait for further C-FIND responses");
      break;
  } //switch
  return EC_Normal;
}


// Send C-FIND-CANCEL and, therefore, ends current C-FIND session
OFCondition DcmSCU::sendCANCELRequest(Uint16 /* presContextID */)
{
  return EC_Normal;
}


// Sends N-ACTION request to another DICOM application
OFCondition DcmSCU::sendACTIONRequest(const T_ASC_PresentationContextID presID,
                                      const OFString &sopInstanceUID,
                                      const Uint16 actionTypeID,
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
  T_DIMSE_N_ActionRQ &actionReq = request.msg.NActionRQ;
  DcmDataset *statusDetail = NULL;

  request.CommandField = DIMSE_N_ACTION_RQ;
  actionReq.MessageID = nextMessageID();
  actionReq.DataSetType = DIMSE_DATASET_PRESENT;
  actionReq.ActionTypeID = actionTypeID;

  // Determine SOP Class from presentation context
  OFString abstractSyntax, transferSyntax;
  findPresentationContext(pcid, abstractSyntax, transferSyntax);
  if (abstractSyntax.empty() || transferSyntax.empty())
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  OFStandard::strlcpy(actionReq.RequestedSOPClassUID, abstractSyntax.c_str(), sizeof(actionReq.RequestedSOPClassUID));
  OFStandard::strlcpy(actionReq.RequestedSOPInstanceUID, sopInstanceUID.c_str(), sizeof(actionReq.RequestedSOPInstanceUID));

  // Send request
  DCMNET_INFO("Sending N-ACTION Request");
  // Output dataset only if trace level is enabled
  if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_OUTGOING, reqDataset, pcid));
  else
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_OUTGOING, NULL, pcid));
  cond = sendDIMSEMessage(pcid, &request, reqDataset, NULL /* callback */, NULL /* callbackContext */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending N-ACTION request: " << DimseCondition::dump(tempStr, cond));
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
  if (response.CommandField == DIMSE_N_ACTION_RSP)
  {
    DCMNET_INFO("Received N-ACTION Response");
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_INCOMING, NULL, pcid));
  } else {
    DCMNET_ERROR("Expected N-ACTION response but received DIMSE command 0x"
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
  T_DIMSE_N_ActionRSP &actionRsp = response.msg.NActionRSP;
  rspStatusCode = actionRsp.DimseStatus;

  // Check whether there is a dataset to be received
  if (actionRsp.DataSetType == DIMSE_DATASET_PRESENT)
  {
    // this should never happen
    DcmDataset *tempDataset = NULL;
    T_ASC_PresentationContextID tempID;
    cond = receiveDIMSEDataset(&tempID, &tempDataset, NULL /* callback */, NULL /* callbackContext */);
    if (cond.good())
    {
      DCMNET_WARN("Received unexpected dataset after N-ACTION response, ignoring");
      delete tempDataset;
    } else {
      DCMNET_ERROR("Failed receiving unexpected dataset after N-ACTION response: "
        << DimseCondition::dump(tempStr, cond));
      return DIMSE_BADDATA;
    }
  }
  if (actionRsp.MessageIDBeingRespondedTo != actionReq.MessageID)
  {
    DCMNET_ERROR("Received response with wrong message ID ("
      << actionRsp.MessageIDBeingRespondedTo << " instead of " << actionReq.MessageID << ")");
    return DIMSE_BADMESSAGE;
  }

  return cond;
}


// Receives N-EVENT-REPORT request
OFCondition DcmSCU::handleEVENTREPORTRequest(DcmDataset *&reqDataset,
                                             Uint16 &eventTypeID,
                                             const int timeout)
{
  // Do some basic validity checks
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  OFString tempStr;
  T_ASC_PresentationContextID presID;
  T_ASC_PresentationContextID presIDdset;
  T_DIMSE_Message request;
  T_DIMSE_N_EventReportRQ &eventReportReq = request.msg.NEventReportRQ;
  DcmDataset *dataset = NULL;
  DcmDataset *statusDetail = NULL;
  Uint16 statusCode = 0;

  if (timeout > 0)
    DCMNET_DEBUG("Handle N-EVENT-REPORT request, waiting up to " << timeout << " seconds (only for N-EVENT-REPORT message)");
  else if ((m_dimseTimeout > 0) && (m_blockMode == DIMSE_NONBLOCKING))
    DCMNET_DEBUG("Handle N-EVENT-REPORT request, waiting up to " << m_dimseTimeout << " seconds (default for all DIMSE messages)");
  else
    DCMNET_DEBUG("Handle N-EVENT-REPORT request, waiting an unlimited period of time");

  // Receive request, use specific timeout (if defined)
  cond = receiveDIMSECommand(&presID, &request, &statusDetail, NULL /* commandSet */, timeout);
  if (cond.bad())
  {
    if (cond != DIMSE_NODATAAVAILABLE)
      DCMNET_ERROR("Failed receiving DIMSE request: " << DimseCondition::dump(tempStr, cond));
    return cond;
  }

  // Check command set
  if (request.CommandField == DIMSE_N_EVENT_REPORT_RQ)
  {
    DCMNET_INFO("Received N-EVENT-REPORT Request");
  } else {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Expected N-EVENT-REPORT request but received DIMSE command 0x"
        << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
        << OFstatic_cast(unsigned int, request.CommandField));
    delete statusDetail;
    return DIMSE_BADCOMMANDTYPE;
  }
  if (statusDetail != NULL)
  {
    DCMNET_DEBUG("Request has status detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
    delete statusDetail;
  }

  // Check if dataset is announced correctly
  if (eventReportReq.DataSetType == DIMSE_DATASET_NULL)
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Received N-EVENT-REPORT request but no dataset announced, aborting");
    return DIMSE_BADMESSAGE;
  }

  // Receive dataset
  cond = receiveDIMSEDataset(&presIDdset, &dataset, NULL /* callback */, NULL /* callbackContext */);
  if (cond.bad())
  {
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_INCOMING, NULL, presID));
    DCMNET_ERROR("Unable to receive N-EVENT-REPORT dataset on presentation context "
      << OFstatic_cast(unsigned int, presID));
    return DIMSE_BADDATA;
  }

  // Output dataset only if trace level is enabled
  if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::TRACE_LOG_LEVEL))
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_INCOMING, dataset, presID));
  else
    DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, request, DIMSE_INCOMING, NULL, presID));

  // Compare presentation context ID of command and data set
  if (presIDdset != presID)
  {
    DCMNET_ERROR("Presentation Context ID of command (" << OFstatic_cast(unsigned int, presID)
      << ") and data set (" << OFstatic_cast(unsigned int, presIDdset) << ") differs");
    delete dataset;
    return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error,
      "DIMSE: Presentation Contexts of Command and Data Set differ");
  }

  // Check the request dataset and return the DIMSE status code to be used
  statusCode = checkEVENTREPORTRequest(eventReportReq, dataset);

  // Send back response
  T_DIMSE_Message response;
  T_DIMSE_N_EventReportRSP &eventReportRsp = response.msg.NEventReportRSP;
  response.CommandField = DIMSE_N_EVENT_REPORT_RSP;
  eventReportRsp.MessageIDBeingRespondedTo = eventReportReq.MessageID;
  eventReportRsp.DimseStatus = statusCode;
  eventReportRsp.DataSetType = DIMSE_DATASET_NULL;
  eventReportRsp.opts = 0;
  eventReportRsp.AffectedSOPClassUID[0] = 0;
  eventReportRsp.AffectedSOPInstanceUID[0] = 0;

  DCMNET_INFO("Sending N-EVENT-REPORT Response");
  DCMNET_DEBUG(DIMSE_dumpMessage(tempStr, response, DIMSE_OUTGOING, NULL, presID));
  cond = sendDIMSEMessage(presID, &response, NULL /* dataObject */, NULL /* callback */, NULL /* callbackContext */);
  if (cond.bad())
  {
    DCMNET_ERROR("Failed sending N-EVENT-REPORT response: " << DimseCondition::dump(tempStr, cond));
    delete dataset;
    return cond;
  }

  // Set return values
  reqDataset = dataset;
  eventTypeID = eventReportReq.EventTypeID;

  return cond;
}


Uint16 DcmSCU::checkEVENTREPORTRequest(T_DIMSE_N_EventReportRQ & /*eventReportReq*/,
                                       DcmDataset * /*reqDataset*/)
{
  // we default to success
  return STATUS_Success;
}


// Sends a DIMSE command and possibly also instance data to the configured peer DICOM application
OFCondition DcmSCU::sendDIMSEMessage(const T_ASC_PresentationContextID presID,
                                     T_DIMSE_Message *msg,
                                     DcmDataset *dataObject,
                                     DIMSE_ProgressCallback callback,
                                     void *callbackContext,
                                     DcmDataset **commandSet)
{
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;
  if (msg == NULL)
    return DIMSE_NULLKEY;

  OFCondition cond;
  /* call the corresponding DIMSE function to sent the message */
  cond = DIMSE_sendMessageUsingMemoryData(m_assoc, presID, msg, NULL /*statusDetail*/, dataObject,
                                          callback, callbackContext, commandSet);

#if 0
  // currently disabled because it is not (yet) needed
  if (cond.good())
  {
    /* create a copy of the current DIMSE command message */
    delete m_openDIMSERequest;
    m_openDIMSERequest = new T_DIMSE_Message;
    memcpy((char*)m_openDIMSERequest, msg, sizeof(*m_openDIMSERequest));
  }
#endif

  return cond;
}


// Receive DIMSE command (excluding dataset!) over the currently open association
OFCondition DcmSCU::receiveDIMSECommand(T_ASC_PresentationContextID *presID,
                                        T_DIMSE_Message *msg,
                                        DcmDataset **statusDetail,
                                        DcmDataset **commandSet,
                                        const Uint32 timeout)
{
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  if (timeout > 0)
  {
    /* call the corresponding DIMSE function to receive the command (use specified timeout)*/
    cond = DIMSE_receiveCommand(m_assoc, DIMSE_NONBLOCKING, timeout, presID,
                                msg, statusDetail, commandSet);
  } else {
    /* call the corresponding DIMSE function to receive the command (use default timeout) */
    cond = DIMSE_receiveCommand(m_assoc, m_blockMode, m_dimseTimeout, presID,
                                msg, statusDetail, commandSet);
  }
  return cond;
}


// Receives one dataset (of instance data) via network from another DICOM application
OFCondition DcmSCU::receiveDIMSEDataset(T_ASC_PresentationContextID *presID,
                                        DcmDataset **dataObject,
                                        DIMSE_ProgressCallback callback,
                                        void *callbackContext)
{
  if (m_assoc == NULL)
    return DIMSE_ILLEGALASSOCIATION;

  OFCondition cond;
  /* call the corresponding DIMSE function to receive the dataset */
  cond = DIMSE_receiveDataSetInMemory(m_assoc, m_blockMode, m_dimseTimeout, presID,
                                      dataObject, callback, callbackContext);
  return cond;
}


void DcmSCU::setMaxReceivePDULength(const unsigned long maxRecPDU)
{
  m_maxReceivePDULength = maxRecPDU;
}


void DcmSCU::setDIMSEBlockingMode(const T_DIMSE_BlockingMode blockingMode)
{
  m_blockMode = blockingMode;
}


void DcmSCU::setAETitle(const OFString &myAETtitle)
{
  m_ourAETitle = myAETtitle;
}


void DcmSCU::setPeerHostName(const OFString &peerHostName)
{
  m_peer = peerHostName;
}


void DcmSCU::setPeerAETitle(const OFString &peerAETitle)
{
  m_peerAETitle = peerAETitle;
}


void DcmSCU::setPeerPort(const Uint16 peerPort)
{
  m_peerPort = peerPort;
}


void DcmSCU::setDIMSETimeout(const Uint32 dimseTimeout)
{
  m_dimseTimeout = dimseTimeout;
}


void DcmSCU::setACSETimeout(const Uint32 acseTimeout)
{
  m_acseTimeout = acseTimeout;
}


void DcmSCU::setAssocConfigFileAndProfile(const OFString &filename,
                                          const OFString &profile)
{
  m_assocConfigFilename = filename;
  m_assocConfigProfile = profile;
}


void DcmSCU::setVerbosePCMode(const OFBool mode)
{
  m_verbosePCMode = mode;
}


/* Get methods */

OFBool DcmSCU::isConnected() const
{
  return (m_assoc != NULL) && (m_assoc->DULassociation != NULL);
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


const OFString &DcmSCU::getAETitle() const
{
  return m_ourAETitle;
}


const OFString &DcmSCU::getPeerHostName() const
{
  return m_peer;
}


const OFString &DcmSCU::getPeerAETitle() const
{
  return m_peerAETitle;
}


Uint16 DcmSCU::getPeerPort() const
{
  return m_peerPort;
}


Uint32 DcmSCU::getDIMSETimeout() const
{
  return m_dimseTimeout;
}


Uint32 DcmSCU::getACSETimeout() const
{
  return m_acseTimeout;
}


OFBool DcmSCU::getVerbosePCMode() const
{
  return m_verbosePCMode;
}


OFCondition DcmSCU::getDatasetInfo(DcmDataset *dataset,
                                   OFString &sopClassUID,
                                   OFString &sopInstanceUID,
                                   E_TransferSyntax &transferSyntax)
{
  if (dataset == NULL)
    return EC_IllegalParameter;
  dataset->findAndGetOFString(DCM_SOPClassUID, sopClassUID);
  dataset->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);
  transferSyntax = dataset->getOriginalXfer();
  return EC_Normal;
}



/* ************************************************************************ */
/*                        C-FIND Response classes                           */
/* ************************************************************************ */

FINDResponses::FINDResponses()
{
  // Nothing to do :-)
}


FINDResponses::~FINDResponses()
{
  Uint32 numelems = m_responses.size();

  FINDResponse *rsp = NULL;
  for (Uint32 i=0; i < numelems; i++)
  {
    rsp = m_responses.front();
    if (rsp != NULL)
    {
      delete rsp; rsp = NULL;
    }
    m_responses.pop_front();
  }
}


Uint32 FINDResponses::numResults() const
{
  return m_responses.size();
}


void FINDResponses::add(FINDResponse *rsp)
{
  if (rsp != NULL)
    m_responses.push_back(rsp);
}


OFListIterator(FINDResponse *) FINDResponses::begin()
{
  return m_responses.begin();
}


OFListIterator(FINDResponse *) FINDResponses::end()
{
  return m_responses.end();
}


/* ************************** FINDResponse class ****************************/

// Standard constructor
FINDResponse::FINDResponse() :
  m_messageIDRespondedTo(0),
  m_affectedSOPClassUID(),
  m_dataset(NULL),
  m_status(0)
{
}

// Destructor, cleans up internal memory (datasets if present)
FINDResponse::~FINDResponse()
{
  if (m_dataset != NULL)
  {
    delete m_dataset;
    m_dataset = NULL;
  }
}


/*
** CVS Log
** $Log: scu.cc,v $
** Revision 1.16  2010-12-21 09:37:36  onken
** Fixed wrong response assignment in DcmSCU's C-STORE code. Thanks to
** forum user "takeos" for the hint and fix.
**
** Revision 1.15  2010-10-20 07:41:36  uli
** Made sure isalpha() & friends are only called with valid arguments.
**
** Revision 1.14  2010-10-14 13:14:29  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.13  2010-10-01 12:25:29  uli
** Fixed most compiler warnings in remaining modules.
**
** Revision 1.12  2010-08-10 11:59:32  uli
** Fixed some cases where dcmFindNameOfUID() returning NULL could cause crashes.
**
** Revision 1.11  2010-06-24 09:26:57  joergr
** Added check on whether the presentation context ID of command and data set are
** identical. Made sure that received dataset is deleted when an error occurs.
** Used more appropriate error conditions / return codes. Further code cleanup.
**
** Revision 1.10  2010-06-22 15:48:53  joergr
** Introduced new enumeration type to be used for closeAssociation().
** Further code cleanup. Renamed some methods, variables, types and so on.
**
** Revision 1.9  2010-06-18 14:58:01  joergr
** Changed some error conditions / return codes to more appropriate values.
** Further revised logging output. Use DimseCondition::dump() where appropriate.
**
** Revision 1.8  2010-06-17 17:13:06  joergr
** Added preliminary support for N-EVENT-REPORT to DcmSCU. Some further code
** cleanups and enhancements. Renamed some methods. Revised documentation.
**
** Revision 1.7  2010-06-09 16:33:34  joergr
** Added preliminary support for N-ACTION to DcmSCU. Some further code cleanups
** and enhancements.
**
** Revision 1.6  2010-06-08 17:54:14  onken
** Added C-FIND functionality to DcmSCU. Some code cleanups. Fixed
** memory leak sometimes occuring during association configuration.
**
** Revision 1.5  2010-06-02 16:01:49  joergr
** Slightly modified some log messages and levels for reasons of consistency.
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
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
*/
