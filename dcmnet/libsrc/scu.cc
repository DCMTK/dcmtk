/*
 *
 *  Copyright (C) 2009, OFFIS
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
 *  Update Date:      $Date: 2009-12-17 09:12:27 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/scu.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/scu.h"
#include "dcmtk/dcmnet/diutil.h"    /* for dcmnet logger */

DcmSCU::DcmSCU() :
  m_net(NULL),
  m_params(NULL),
  m_assoc(NULL),
  m_assocConfigFilename(""),
  m_assocConfigProfile(""),
  m_presContexts(),
  m_currPresID(1),
  m_openDIMSERequest(NULL),
  m_maxReceivePDULength(ASC_DEFAULTMAXPDU),
  m_secureConnection(OFFalse),
  m_blockMode(DIMSE_BLOCKING),
  m_ourAETitle("ANY-SCU"),
  m_peer(""),
  m_peerAETitle("ANY-SCP"),
  m_peerPort(104),
  m_dimse_timeout(0),
  m_acse_timeout(30),
#ifdef WITH_OPENSSL
  m_tLayer(NULL),
  m_keyFileFormat(SSL_FILETYPE_PEM),
  m_doAuthenticate(OFFalse),
  m_trustedCertDirs(),
  m_trustedCertFiles(),
  m_privateKeyFile(""),
  m_certificateFile(""),
  m_passwd(NULL),
  m_readSeedFile(""),
  m_writeSeedFile(""),
  m_certVerification(DCV_requireCertificate),
  m_dhparam("")
#endif
{
#ifdef WITH_OPENSSL
#if OPENSSL_VERSION_NUMBER >= 0x0090700fL
  m_ciphersuites = TLS1_TXT_RSA_WITH_AES_128_SHA;
  m_ciphersuites += ":";
  m_ciphersuites += SSL3_TXT_RSA_DES_192_CBC3_SHA;
#else
  m_ciphersuites(SSL3_TXT_RSA_DES_192_CBC3_SHA);
#endif
#endif

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
#ifdef WITH_OPENSSL
  if (m_passwd)
  {
    delete[] m_passwd;
    m_passwd = NULL;
  }
#endif
}


OFCondition DcmSCU::initNetwork()
{
  OFString msg;
  /* initialize network, i.e. create an instance of T_ASC_Network*. */
  OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, m_acse_timeout, &m_net);
  if (cond.bad())
  {
    DimseCondition::dump(msg, cond);
    DCMNET_ERROR(msg);
    return cond;
  }

#ifdef WITH_OPENSSL

  if (m_secureConnection)
  {
    m_tLayer = new DcmTLSTransportLayer(DICOM_APPLICATION_REQUESTOR, m_readSeedFile.c_str());
    if (m_tLayer == NULL)
    {
      DCMNET_ERROR("Unable to create TLS transport layer for SCP");
      return EC_IllegalCall;
    }

    if (m_doAuthenticate)
    {
      if (m_passwd) m_tLayer->setPrivateKeyPasswd(m_passwd);

      if ( cond.good() && (TCS_ok != m_tLayer->setPrivateKeyFile(m_privateKeyFile.c_str(), m_keyFileFormat)) )
      {
        DCMNET_ERROR("Unable to create TLS transport layer for SCP: Unable to load private TLS key from file " << m_privateKeyFile);
        cond = EC_IllegalCall;
      }
      if ( cond.good() && (TCS_ok != m_tLayer->setCertificateFile(m_certificateFile.c_str(), m_keyFileFormat)) )
      {
        DCMNET_ERROR("Unable to load SCP certificate from file " << m_certificateFile);
        cond = EC_IllegalCall;
      }
      if (! m_tLayer->checkPrivateKeyMatchesCertificate() && cond.good())
      {
        DCMNET_ERROR("Private key from file " << m_privateKeyFile << " and certificate from file " << m_certificateFile << " do not match");
        cond = EC_IllegalCall;
      }
    }

    if ( cond.good() && (TCS_ok != m_tLayer->setCipherSuites(m_ciphersuites.c_str())) )
    {
      DCMNET_ERROR("Unable to set selected cipher suites for SCP");
      cond = EC_IllegalCall;
    }
    if (cond.bad())
    {
      delete m_tLayer; m_tLayer = NULL;
      return cond;
    }
    m_tLayer->setCertificateVerification(m_certVerification);


    cond = ASC_setTransportLayer(m_net, m_tLayer, 0);
    if (cond.bad())
    {
      DimseCondition::dump(msg, cond);
      DCMNET_ERROR(msg);
      delete m_tLayer; m_tLayer = NULL;
      return cond;
    }
  }

#endif

  /* initialize asscociation parameters, i.e. create an instance of T_ASC_Parameters*. */
  cond = ASC_createAssociationParameters(&m_params, m_maxReceivePDULength);
  if (cond.bad())
  {
    DCMNET_ERROR(DimseCondition::dump(msg, cond));
#ifdef WITH_OPENSSL
    delete m_tLayer; m_tLayer = NULL;
#endif
    return cond;
  }

  /* sets this application's title and the called application's title in the params */
  /* structure. The default values are "ANY-SCU" and "ANY-SCP". */
  ASC_setAPTitles(m_params, m_ourAETitle.c_str(), m_peerAETitle.c_str(), NULL);

  /* Set the transport layer type (type of network connection) in the params */
  /* structure. The default is an insecure connection; where OpenSSL is  */
  /* available the user is able to request an encrypted,secure connection. */
  cond = ASC_setTransportLayerType(m_params, m_secureConnection);
  if (cond.bad())
  {
    DCMNET_ERROR(DimseCondition::dump(msg, cond));
#ifdef WITH_OPENSSL
    delete m_tLayer; m_tLayer = NULL;
#endif
    return cond;
  }

  /* Figure out the presentation addresses and copy the */
  /* corresponding values into the association parameters.*/
  DIC_NODENAME localHost;
  DIC_NODENAME peerHost;
  gethostname(localHost, sizeof(localHost) - 1);
  sprintf(peerHost, "%s:%d", m_peer.c_str(), (int)m_peerPort);
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
      delete assocConfig;
      assocConfig = new DcmAssociationConfiguration();
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
        delete assocConfig;
        assocConfig = new DcmAssociationConfiguration();
        return result;
      }
    }
  }

  // Adapt presentation context ID to existing presentation contexts
  // It's important that presentation context ids are numerated 1,3,5,7...!
  int numContexts = ASC_countPresentationContexts(m_params);
  if (numContexts < 127)
  {
    // need Uint16 to avoid overflow in currPresID (unsigned char)
    m_currPresID = OFstatic_cast(Uint8, 2* numContexts +1) /* add 1 to point to the next free ID*/;
  }
  // Print warning if number of overall presenation contexts exceeds 128
  if ( (numContexts + m_presContexts.size()) > 128)
  {
    DCMNET_WARN("Number of presentation contexts exceeds 128 (" << numContexts << "). Some contexts will not be negotiated");
  }
  else
    DCMNET_TRACE("Configured " << numContexts << " presentation contexts from config file");

  // Add presentation contexts not originating from config file
  DCMNET_TRACE("Adding another " << m_presContexts.size() << " presentation contexts configured for SCU");
  OFListIterator(DcmSCUPresContext) contIt = m_presContexts.begin();
  while ( (contIt != m_presContexts.end()) && (m_currPresID <= 253) )
  {
    m_currPresID += 2;
    const unsigned int numTransferSyntaxes = (*contIt).transferSyntaxes.size();
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
    cond = ASC_addPresentationContext( m_params, m_currPresID, (*contIt).abstractSyntaxName.c_str(), transferSyntaxes, numTransferSyntaxes);
    // if adding was successfull, prepare pres. context ID for next addition
    delete[] transferSyntaxes; transferSyntaxes = NULL;
    if (cond.bad())
      return cond;
    contIt++;
  }
  DCMNET_DEBUG("Configured a total of " << ASC_countPresentationContexts(m_params) << " presentation contexts for SCU");

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
#ifdef WITH_OPENSSL
      delete m_tLayer; m_tLayer = NULL;
#endif
      return cond;
    }
    else
    {
      DCMNET_DEBUG("Association Request Failed:\n" << DimseCondition::dump(msg,cond));
#ifdef WITH_OPENSSL
      delete m_tLayer; m_tLayer = NULL;
#endif
      return cond;
    }
  }

  /* dump the presentation contexts which have been accepted/refused */
  DCMNET_DEBUG("Association Parameters Negotiated:\n" << ASC_dumpParameters(msg, m_params, ASC_ASSOC_AC));

  /* count the presentation contexts which have been accepted by the SCP */
  /* If there are none, finish the execution */
  if (ASC_countAcceptedPresentationContexts(m_params) == 0)
  {
#ifdef WITH_OPENSSL
      delete m_tLayer; m_tLayer = NULL;
#endif
    DCMNET_DEBUG("No acceptable presentation contexts found");
    return EC_IllegalCall;
  }

  /* dump general information concerning the establishment of the network connection if required */
  DCMNET_DEBUG("Association Accepted (Max Send PDV: " << (unsigned int)(m_assoc->sendPDVLength));
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


unsigned short DcmSCU::nextMessageID()
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

#ifdef WITH_OPENSSL
  if (m_tLayer && m_writeSeedFile.c_str())
  {
    if (m_tLayer->canWriteRandomSeed())
    {
      if (!m_tLayer->writeRandomSeed(m_writeSeedFile.c_str()))
      {
        DCMNET_WARN("Cannot write random seed file '" << m_writeSeedFile << "', ignoring.");
      }
    } else {
      DCMNET_WARN("Cannot write random seed, ignoring");
    }
  }
  delete m_tLayer; m_tLayer = NULL;
#endif
}


// Sends C-ECHO request to another DICOM application
OFCondition DcmSCU::sendECHORequest( const T_ASC_PresentationContextID& presID )
{
  if (m_assoc == NULL)
    return ASC_NULLKEY;
  OFCondition cond;
  /* Now, assemble dimse message */
  Uint16 status;
  cond = DIMSE_echoUser(m_assoc, nextMessageID(), m_blockMode, m_dimse_timeout, &status, NULL);
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

  /* set message ID */
  msg->msg.CStoreRQ.MessageID = nextMessageID();
  
  T_ASC_PresentationContextID id=presID; //TO
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
  cond = DIMSE_receiveCommand(m_assoc, m_blockMode, m_dimse_timeout, presID,
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
  cond = DIMSE_receiveDataSetInMemory( m_assoc, m_blockMode, m_dimse_timeout, presID,
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


void DcmSCU::setPeerPort(const unsigned long& peerPort)
{
  m_peerPort = peerPort;
}


void DcmSCU::setDIMSETimeout(const int& dimseTimeout)
{
  m_dimse_timeout = dimseTimeout;
}


void DcmSCU::setACSETimeout(const int& acseTimeout)
{
  m_acse_timeout = acseTimeout;
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


OFBool DcmSCU::getSecureConnectionMode() const
{
  return m_secureConnection;
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


unsigned int DcmSCU::getPeerPort() const
{
  return m_peerPort;
}


int DcmSCU::getDIMSETimeout() const
{
  return m_dimse_timeout;
}


int DcmSCU::getACSETimeout() const
{
  return m_acse_timeout;
}

#ifdef WITH_OPENSSL

  void DcmSCU::setSecureConnectionMode(const OFBool& secureMode)
  {
    m_secureConnection = secureMode;
  }


  void DcmSCU::addTrustedCertFile(const OFString& str)
  {
    m_trustedCertFiles.push_back(str);
  }


  void DcmSCU::addTrustedCertDir(const OFString& certDir)
  {
    m_trustedCertDirs.push_back(certDir);
  }


  void DcmSCU::setKeyFileFormat(const int& keyFileFormat)
  {
    m_keyFileFormat = keyFileFormat;
  }


  void DcmSCU::setAuthenticationEnabled(const OFBool& authMode)
  {
    m_doAuthenticate = authMode;
  }


  void DcmSCU::setPrivateKeyFile(const OFString& keyFile)
  {
    m_privateKeyFile = keyFile;
  }


  void DcmSCU::setCertificateFile(const OFString& certFile)
  {
    m_certificateFile = certFile;
  }


  void DcmSCU::setPassword(const char* passwd)
  {
    if (passwd == NULL)
    {
      m_passwd = NULL;
      return;
    }
    delete[] m_passwd; m_passwd = NULL;
    int passLength = strlen(m_passwd) + 1;
    m_passwd = new char[passLength];
    strncpy(m_passwd, passwd, passLength);
  }


  void DcmSCU::setCiphersuites(const OFString& cs)
  {
    m_ciphersuites = cs;
  }


  void DcmSCU::setReadSeedFile(const OFString& seedFile)
  {
    m_readSeedFile = seedFile;
  }


  void DcmSCU::setWriteSeedFile(const OFString& seedFile)
  {
    m_writeSeedFile = seedFile;
  }


  void DcmSCU::setCertVerification(const DcmCertificateVerification& cert)
  {
    m_certVerification = cert;
  }


  void DcmSCU::setDHParam(const OFString& dhParam)
  {
    m_dhparam = dhParam;
  }


  int DcmSCU::getKeyFileFormat() const
  {
    return m_keyFileFormat;
  }


  OFBool DcmSCU::getAuthenticationEnabled() const
  {
    return m_doAuthenticate;
  }


  void DcmSCU::getTrustedCertFiles(OFList<OFString>& files /*out*/) const
  {
    files.clear();
    OFListConstIterator(OFString) it = m_trustedCertFiles.begin();
    while (it != m_trustedCertFiles.end())
    {
      files.push_back(*it);
      it++;
    }
  }


  OFString DcmSCU::getPrivateKeyFile() const
  {
    return m_privateKeyFile;
  }


  OFString DcmSCU::getCertificateFile() const
  {
    return m_certificateFile;
  }


  void DcmSCU::getTrustedCertDirs(OFList<OFString>& trustedDirs /*out*/) const
  {
    OFListConstIterator(OFString) it = m_trustedCertDirs.begin();
    while (it != m_trustedCertDirs.end())
    {
      trustedDirs.push_back(*it);
      it++;
    }
  }


  OFBool DcmSCU::getPassword(OFString& passwd /*out*/) const
  {
    if (m_passwd == NULL)
      return OFFalse;
    passwd = m_passwd;
    return OFTrue;
  }


  OFString DcmSCU::getCiphersuites() const
  {
    return m_ciphersuites;
  }


  OFString DcmSCU::getReadSeedFile() const
  {
    return m_readSeedFile;
  }


  OFString DcmSCU::getWriteSeedFile() const
  {
    return m_writeSeedFile;
  }


  DcmCertificateVerification DcmSCU::getCertVerification() const
  {
    return m_certVerification;
  }


  OFString DcmSCU::getDHParam() const
  {
    return m_dhparam;
  }

#endif


/*
** CVS Log
** $Log: scu.cc,v $
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
