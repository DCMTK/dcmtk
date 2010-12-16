/*
 *
 *  Copyright (C) 2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-12-16 08:34:06 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"  /* make sure OS specific configuration is included first */

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlsscu.h"
#include "dcmtk/dcmnet/diutil.h"    /* for dcmnet logger */


DcmTLSSCU::DcmTLSSCU() :
  m_tLayer(NULL),
  m_doAuthenticate(OFFalse),
  m_trustedCertDirs(),
  m_trustedCertFiles(),
  m_privateKeyFile(""),
  m_privateKeyFileFormat(SSL_FILETYPE_PEM),
  m_certificateFile(""),
  m_certKeyFileFormat(SSL_FILETYPE_PEM),
  m_passwd(NULL),
  m_readSeedFile(""),
  m_writeSeedFile(""),
  m_certVerification(DCV_requireCertificate),
  m_dhparam("")
{
#if OPENSSL_VERSION_NUMBER >= 0x0090700fL
  m_ciphersuites = TLS1_TXT_RSA_WITH_AES_128_SHA;
  m_ciphersuites += ":";
  m_ciphersuites += SSL3_TXT_RSA_DES_192_CBC3_SHA;
#else
  m_ciphersuites = SSL3_TXT_RSA_DES_192_CBC3_SHA;
#endif
}


DcmTLSSCU::DcmTLSSCU(const OFString& peerHost,
                     const OFString& peerAETitle,
                     const Uint16& portNum) :
  m_tLayer(NULL),
  m_doAuthenticate(OFFalse),
  m_trustedCertDirs(),
  m_trustedCertFiles(),
  m_privateKeyFile(""),
  m_privateKeyFileFormat(SSL_FILETYPE_PEM),
  m_certificateFile(""),
  m_certKeyFileFormat(SSL_FILETYPE_PEM),
  m_passwd(NULL),
  m_readSeedFile(""),
  m_writeSeedFile(""),
  m_certVerification(DCV_requireCertificate),
  m_dhparam("")
{
#if OPENSSL_VERSION_NUMBER >= 0x0090700fL
  m_ciphersuites = TLS1_TXT_RSA_WITH_AES_128_SHA;
  m_ciphersuites += ":";
  m_ciphersuites += SSL3_TXT_RSA_DES_192_CBC3_SHA;
#else
  m_ciphersuites = SSL3_TXT_RSA_DES_192_CBC3_SHA;
#endif
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
  m_tLayer = new DcmTLSTransportLayer(DICOM_APPLICATION_REQUESTOR, m_readSeedFile.c_str());
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
    // Set password. NULL password will ask user on commandline
    if (m_passwd) m_tLayer->setPrivateKeyPasswd(m_passwd);

    // Set file that contains the privatey key
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

  /* Set cipher suites to be supported */
  if ( cond.good() && (TCS_ok != m_tLayer->setCipherSuites(m_ciphersuites.c_str())) )
  {
    DCMTLS_ERROR("Unable to set selected cipher suites for SCP");
    cond = EC_IllegalCall; // TODO: need to find better error code
  }

  /* Initiliaze Diffie-Hellman parameters from file if given */
  if (!m_dhparam.empty() && cond.good())
  {
    if (!m_tLayer->setTempDHParameters(m_dhparam.c_str()))
      cond = EC_IllegalCall; // TODO: need to find better error code
  }

  /* Set whether SCU should check the SCP's certificate for validity */
  if (cond.good())
    m_tLayer->setCertificateVerification(m_certVerification);

  /*  Now we are ready to initialize scu's network structures */
  if (cond.good())
    cond = DcmSCU::initNetwork();

  /* Set the transport layer type (type of network connection) in the params
     structure. The default is an insecure connection in DcmSCU. To make use of
     OPENSSL, it must "inject" the TLS layer here.
  */
  if (cond.good())
    cond = useSecureConnection(m_tLayer);

  /* Finally, look whether everyhting was successful. If not,
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
                                     const int& privKeyFormat,
                                     const int& certFormat)
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

  int passLength = strlen(passphrase) + 1;
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


void DcmTLSSCU::addCiphersuite(const OFString& cs)
{
  if (m_ciphersuites.empty())
    m_ciphersuites = cs;
  else
  {
    m_ciphersuites+= ":";
    m_ciphersuites+= cs;
  }
}


void DcmTLSSCU::setReadSeedFile(const OFString& seedFile)
{
  m_readSeedFile = seedFile;
}


void DcmTLSSCU::setWriteSeedFile(const OFString& seedFile)
{
  m_writeSeedFile = seedFile;
}


void DcmTLSSCU::setPeerCertVerification(const DcmCertificateVerification& cert)
{
  m_certVerification = cert;
}


void DcmTLSSCU::setDHParam(const OFString& dhParam)
{
  m_dhparam = dhParam;
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


OFString DcmTLSSCU::getCiphersuites() const
{
  return m_ciphersuites;
}


OFString DcmTLSSCU::getReadSeedFile() const
{
  return m_readSeedFile;
}


OFString DcmTLSSCU::getWriteSeedFile() const
{
  return m_writeSeedFile;
}


OFString DcmTLSSCU::getDHParam() const
{
  return m_dhparam;
}


#endif // WITH_OPENSSL


/*
** CVS Log
** $Log: tlsscu.cc,v $
** Revision 1.4  2010-12-16 08:34:06  joergr
** Fixed initialization of OFString variable (reported by gcc 2.95.3).
**
** Revision 1.3  2010-10-14 13:14:46  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.2  2010-06-22 15:50:09  joergr
** Introduced new enumeration type to be used for closeAssociation().
**
** Revision 1.1  2010-04-29 16:17:56  onken
** Added new class DcmTLSSCU as base class for developing TLS-enabled SCUs.
**
*/
