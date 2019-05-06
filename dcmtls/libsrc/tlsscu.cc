/*
 *
 *  Copyright (C) 2010-2019, OFFIS e.V.
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
 *  Module:  dcmtls
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Base class for TLS-enabled Service Class Users (SCUs)
 *
 */

#include "dcmtk/config/osconfig.h"  /* make sure OS specific configuration is included first */
#include "dcmtk/dcmtls/tlsdefin.h"  /* for DCMTK_DCMTLS_EXPORT */

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlsscu.h"
#include "dcmtk/dcmnet/diutil.h"    /* for dcmnet logger */

BEGIN_EXTERN_C
#include <openssl/ssl.h>
END_EXTERN_C


DcmTLSSCU::DcmTLSSCU() :
  m_tLayer(NULL),
  m_doAuthenticate(OFFalse),
  m_trustedCertDirs(),
  m_trustedCertFiles(),
  m_privateKeyFile(""),
  m_privateKeyFileFormat(DCF_Filetype_PEM),
  m_certificateFile(""),
  m_certKeyFileFormat(DCF_Filetype_PEM),
  m_passwd(NULL),
  m_readSeedFile(""),
  m_writeSeedFile(""),
  m_certVerification(DCV_requireCertificate)
{
}


DcmTLSSCU::DcmTLSSCU(const OFString& peerHost,
                     const OFString& peerAETitle,
                     const Uint16 portNum) :
  m_tLayer(NULL),
  m_doAuthenticate(OFFalse),
  m_trustedCertDirs(),
  m_trustedCertFiles(),
  m_privateKeyFile(""),
  m_privateKeyFileFormat(DCF_Filetype_PEM),
  m_certificateFile(""),
  m_certKeyFileFormat(DCF_Filetype_PEM),
  m_passwd(NULL),
  m_readSeedFile(""),
  m_writeSeedFile(""),
  m_certVerification(DCV_requireCertificate)
{
  setPeerHostName(peerHost);
  setPeerAETitle(peerAETitle);
  setPeerPort(portNum);
}


DcmTLSSCU::~DcmTLSSCU()
{
  if (m_passwd)
  {
    delete[] m_passwd;
    m_passwd = NULL;
  }
}


OFCondition DcmTLSSCU::initNetwork()
{
  OFCondition cond;

  /* First, create TLS layer */
  m_tLayer = new DcmTLSTransportLayer(NET_REQUESTOR, m_readSeedFile.c_str(), OFTrue /* initialize OpenSSL */);
  if (m_tLayer == NULL)
  {
    DCMTLS_ERROR("Unable to create TLS transport layer for SCP, maybe problem with seed file?");
    return EC_IllegalCall; // TODO: need to find better error code
  }

  /* If authentication of both sides (and not only encryption) is desired,
   * handle all associated parameters
   */
  if (m_doAuthenticate)
  {
    // Set password. NULL password will ask user on command line
    if (m_passwd) m_tLayer->setPrivateKeyPasswd(m_passwd);

    // Set file that contains the private key
    if ( cond.good() && (TCS_ok != m_tLayer->setPrivateKeyFile(m_privateKeyFile.c_str(), m_privateKeyFileFormat)) )
    {
      DCMTLS_ERROR("Unable to create TLS transport layer for SCP: Unable to load private TLS key from file " << m_privateKeyFile);
      cond = EC_IllegalCall; // TODO: need to find better error code
    }
    // Set file that contains host certificate
    if ( cond.good() && (TCS_ok != m_tLayer->setCertificateFile(m_certificateFile.c_str(), m_certKeyFileFormat)) )
    {
      DCMTLS_ERROR("Unable to load SCP certificate from file " << m_certificateFile);
      cond = EC_IllegalCall; // TODO: need to find better error code
    }
    // Set whether private key fits with certificate
    if (! m_tLayer->checkPrivateKeyMatchesCertificate() && cond.good())
    {
      DCMTLS_ERROR("Private key from file " << m_privateKeyFile << " and certificate from file " << m_certificateFile << " do not match");
      cond = EC_IllegalCall; // TODO: need to find better error code
    }
  }

  /* Set whether SCU should check the SCP's certificate for validity */
  if (cond.good())
    m_tLayer->setCertificateVerification(m_certVerification);

  /*  Now we are ready to initialize SCU's network structures */
  if (cond.good())
    cond = DcmSCU::initNetwork();

  /* Set the transport layer type (type of network connection) in the params
     structure. The default is an insecure connection in DcmSCU. To make use of
     OPENSSL, it must "inject" the TLS layer here.
  */
  if (cond.good())
    cond = useSecureConnection(m_tLayer);

  /* Finally, look whether everything was successful. If not,
   * delete secure layer created above
   */
  if (cond.bad())
  {
    OFString msg;
    DimseCondition::dump(msg, cond);
    DCMTLS_ERROR(msg);
    delete m_tLayer;
    m_tLayer = NULL;
  }

  return cond;
}


OFCondition DcmTLSSCU::negotiateAssociation()
{
  // Negotiate association
  OFCondition cond = DcmSCU::negotiateAssociation();
  if (cond.bad())
  {
    // if something goes wrong, delete TLS structures
    delete m_tLayer;
    m_tLayer = NULL;
  }
  return cond;
}


void DcmTLSSCU::closeAssociation(const DcmCloseAssociationType closeType)
{
  // Close association
  DcmSCU::closeAssociation(closeType);

  if (m_tLayer && !m_writeSeedFile.empty())
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
  delete m_tLayer;
  m_tLayer = NULL;
}


void DcmTLSSCU::enableAuthentication(const OFString& privateKey,
                                     const OFString& certFile,
                                     const char* passphrase,
                                     const DcmKeyFileFormat privKeyFormat,
                                     const DcmKeyFileFormat certFormat)
{
  m_doAuthenticate = OFTrue;
  m_privateKeyFile = privateKey;
  m_privateKeyFileFormat = privKeyFormat;
  m_certificateFile = certFile;
  m_certKeyFileFormat = certFormat;
  if (m_passwd != NULL)
  {
    delete[] m_passwd;
    m_passwd = NULL;
  }
  if (passphrase == NULL)
  {
    m_passwd = NULL;
    return;
  }

  size_t passLength = strlen(passphrase) + 1;
  m_passwd = new char[passLength];
  strncpy(m_passwd, passphrase, passLength);
  return;
}


void DcmTLSSCU::addTrustedCertFile(const OFString& str)
{
  m_trustedCertFiles.push_back(str);
}


void DcmTLSSCU::addTrustedCertDir(const OFString& certDir)
{
  m_trustedCertDirs.push_back(certDir);
}


void DcmTLSSCU::disableAuthentication()
{
  m_doAuthenticate = OFFalse;
}

DcmTransportLayerStatus DcmTLSSCU::addCipherSuite(const OFString& suite)
{
  if (m_tLayer)
     return m_tLayer->addCipherSuite(suite.c_str());
     else return TCS_illegalCall;
}

DcmTransportLayerStatus DcmTLSSCU::setTLSProfile(DcmTLSSecurityProfile profile)
{
  if (m_tLayer)
  {
    m_tLayer->setTLSProfile(profile);
    return TCS_ok;
  } else return TCS_illegalCall;
}

void DcmTLSSCU::setReadSeedFile(const OFString& seedFile)
{
  m_readSeedFile = seedFile;
}


void DcmTLSSCU::setWriteSeedFile(const OFString& seedFile)
{
  m_writeSeedFile = seedFile;
}


void DcmTLSSCU::setPeerCertVerification(const DcmCertificateVerification cert)
{
  m_certVerification = cert;
}


OFBool DcmTLSSCU::getAuthenticationParams(OFString& privKeyFile,
                                          OFString& certFile,
                                          const char*& passphrase,
                                          int& privKeyFormat,
                                          int& certFormat) const
{
  if (!m_doAuthenticate)
    return OFFalse;

  privKeyFile = m_privateKeyFile;
  certFile = m_certificateFile;
  passphrase = m_passwd;
  privKeyFormat = m_privateKeyFileFormat;
  certFormat = m_certKeyFileFormat;
  return m_doAuthenticate;
}

void DcmTLSSCU::getTrustedCertFiles(OFList<OFString>& files /*out*/) const
{
  files.clear();
  OFListConstIterator(OFString) it = m_trustedCertFiles.begin();
  while (it != m_trustedCertFiles.end())
  {
    files.push_back(*it);
    it++;
  }
}


void DcmTLSSCU::getTrustedCertDirs(OFList<OFString>& trustedDirs /*out*/) const
{
  OFListConstIterator(OFString) it = m_trustedCertDirs.begin();
  while (it != m_trustedCertDirs.end())
  {
    trustedDirs.push_back(*it);
    it++;
  }
}


OFString DcmTLSSCU::getReadSeedFile() const
{
  return m_readSeedFile;
}


OFString DcmTLSSCU::getWriteSeedFile() const
{
  return m_writeSeedFile;
}


void DcmTLSSCU::setDHParam(const OFString& dhParam)
{
  if (!m_tLayer->setTempDHParameters(dhParam.c_str()))
     DCMTLS_WARN("unable to load temporary DH parameter file '" << dhParam << "', ignoring");
}

#else

/* make sure that the object file is not completely empty if compiled
 * without OpenSSL because some linkers might fail otherwise.
 */
DCMTK_DCMTLS_EXPORT void tlsscu_dummy_function()
{
  return;
}

#endif // WITH_OPENSSL
