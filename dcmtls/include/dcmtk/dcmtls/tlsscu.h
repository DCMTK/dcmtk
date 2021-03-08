/*
 *
 *  Copyright (C) 2010-2021, OFFIS e.V.
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

#ifndef TLSSCU_H
#define TLSSCU_H

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#ifdef WITH_OPENSSL
#include "dcmtk/dcmnet/scu.h"
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#include "dcmtk/dcmtls/tlsdefin.h"

/** Base class for implementing DICOM TLS-enabled Service Class User functionality. This
 *  class is derived from the general DcmSCU class and just adds the corresponding TLS
 *  capabilities.
 *  @remark This class is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 *  @warning This class is EXPERIMENTAL. Be careful to use it in production environment.
 *  API changes in the future are possible and likely to happen.
 */
class DCMTK_DCMTLS_EXPORT DcmTLSSCU : public DcmSCU
{

public:

  /** Constructor, just initializes internal class members
   */
  DcmTLSSCU();

  /** Initialize SCU with connection peer information
   *  @param peerHost    The hostname or IP of the host to talk to
   *  @param peerAETitle AETitle of peer host to be used
   *  @param portNum     TCP/IP port number to talk to
   */
  DcmTLSSCU(const OFString& peerHost,
            const OFString& peerAETitle,
            const Uint16 portNum);

  /** Virtual destructor
    */
  virtual ~DcmTLSSCU();

  /** Initialize network, i.e.\ prepare for association negotiation
   *  @return EC_Normal if initialization was successful, otherwise error code
   */
  virtual OFCondition initNetwork();

  /** Negotiates association by using presentation contexts and parameters
   *  as defined by earlier function calls
   *  @return EC_Normal if negotiation was successful, otherwise error code.
   */
  virtual OFCondition negotiateAssociation();

  /** Closes the association of this SCU
   *  @deprecated The use of this method is deprecated. Please use
   *    DcmSCU::releaseAssociation() or DcmSCU::abortAssociation() instead.
   *  @param closeType Define whether to release or abort the association
   */
  virtual void closeAssociation(const DcmCloseAssociationType closeType);

  /** Add file with trusted certificate (used if authentication is enabled)
   *  @param str Filename
   */
  virtual void addTrustedCertFile(const OFString& str);

  /** Add directory to list of directories containing trusted certificates (used
   *  if authentication is enabled)
   *  @param certDir Not documented yet
   */
  virtual void addTrustedCertDir(const OFString& certDir);

  /** Turn on authentication for TLS
   *  @param privateKey    File with private key to authenticate with
   *  @param certFile      File with certificate to authenticate with
   *  @param passphrase    Password to access key. NULL asks user on console.
   *         "" will send empty password. Default is asking the
   *         user to enter password.
   *  @param privKeyFormat Format of private key parameter. Default is
   *                       SSL_FILETYPE_PEM.
   *  @param certFormat    Format of certificate file parameter. Default is
   *                       SSL_FILETYPE_PEM.
   */
  virtual void enableAuthentication(const OFString& privateKey,
                                    const OFString& certFile,
                                    const char* passphrase = NULL,
                                    const DcmKeyFileFormat privKeyFormat = DCF_Filetype_PEM,
                                    const DcmKeyFileFormat certFormat = DCF_Filetype_PEM);

  /** Disables authentication. However, DcmTLSSCU will try to establish secured
   *  connection in terms of encrypting data. Default is that authentication is disabled.
   */
  virtual void disableAuthentication();

  /** replace the current list of ciphersuites by the list of ciphersuites
   *  for the given profile. Caller must ensure that initNetwork() is executed before this call.
   *  @param profile TLS Security Profile
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setTLSProfile(DcmTLSSecurityProfile profile);

  /** adds a ciphersuite to the list of ciphersuites for TLS negotiation.
   *  Caller must ensure that initNetwork() is executed before this call.
   *  It is the responsibility of the user to ensure that the added ciphersuite
   *  does not break the rules of the selected profile. Use with care!
   *  @param suite TLS ciphersuite name, in the official TLS name form.
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition addCipherSuite(const OFString& suite);

  /** Set file to be used as random seed for initializing the Pseudo Random
   *  Number Generator (PRNG)
   *  @param seedFile The seed file to be used.
   */
  virtual void setReadSeedFile(const OFString& seedFile);

  /** Set file to be use to store the updated random seed to store the updated
   *  random seed from the Pseudo Random Number Generator (PRNG).
   *  @param seedFile The seed file to be used for writing back seed info
   */
  virtual void setWriteSeedFile(const OFString& seedFile);

  /** Set whether peer's certificate must be there, only is checked if there or is ignored
   *  @param cert Peer certificate verification mode. The following values are permitted:
   *         DCV_requireCertificate, DCV_checkCertificate, DCV_ignoreCertificate
   */
  virtual void setPeerCertVerification(const DcmCertificateVerification cert);

  /** Set Diffie-Hellman parameters from file. This method should be
   *  called after calls to initNetwork() and setTLSProfile().
   *  @param filename of dhParam Diffie-Hellman parameter file to be used.
   */
  virtual void setDHParam(const OFString& dhParam);

  /** Returns OFTrue if authentication is enabled
   *  @param privKeyFile   The file containing the private key used
   *  @param certFile      The file containing the certificate used
   *  @param passphrase    The passphrase used for unlocking the private key
   *         file. If NULL, password is asked from STDIN. If empty string (""),
   *         password is sent empty. All others values are sent as given.
   *  @param privKeyFormat Format of the private key in privKeyFile, see
   *         documentation of m_privateKeyFileFormat.
   *  @param certFormat    The certificate format of certificate in certFile.
   *         See documentation of m_certKeyFileFormat.
   *  @return Returns OFTrue if authentication is enabled, OFFalse otherwise
   */
  virtual OFBool getAuthenticationParams(OFString& privKeyFile,
                                         OFString& certFile,
                                         const char*& passphrase,
                                         int& privKeyFormat,
                                         int& certFormat) const;

  /** Get files considered for trusted certificates.
   *  @param files The file names that are considered as trusted
   *         certificates
   */
  virtual void getTrustedCertFiles(OFList<OFString>& files /*out*/) const;

  /** Get directories containing considered to contain trusted certificates.
   *  @param trustedDirs directories considered to contain trusted certificates.
   */
  virtual void getTrustedCertDirs(OFList<OFString>& trustedDirs /*out*/) const;

  /** Get random seed file used for initializing Pseudo Random Number
   *  Generator (PRNG)
   *  @return Random seed file used for reading
   */
  virtual OFString getReadSeedFile() const;

  /** Get random seed file the PRNG should use to store back updated random
   *  seed information
   *  @return Get random seed file used for writing back updated seed
   */
  virtual OFString getWriteSeedFile() const;

private:

  /** Private undefined copy-constructor. Shall never be called.
   *  @param src Source object
   */
  DcmTLSSCU(const DcmTLSSCU &src);

  /** Private undefined operator=. Shall never be called.
   *  @param src Source object
   *  @return Reference to this object
   */
  DcmTLSSCU &operator=(const DcmTLSSCU &src);

  /// The TLS layer responsible for all encryption/authentication stuff
  DcmTLSTransportLayer *m_tLayer;

  /// If enabled, authentication of client/server is enabled
  OFBool m_doAuthenticate;

  /// A list of directories containing trusted certificates (if authentication is enabled)
  OFList<OFString> m_trustedCertDirs;

  /// A list of files containing trusted certificates (if authentication is enabled)
  OFList<OFString> m_trustedCertFiles;

  /// The file containing the private key (if authentication is enabled)
  OFString m_privateKeyFile;

  /// private key file format
  DcmKeyFileFormat m_privateKeyFileFormat;

  /// File containing the certificate the SCU should use for authentication
  OFString m_certificateFile;

  /// certificate (public key) file format
  DcmKeyFileFormat m_certKeyFileFormat;

  /// password required to open the private key file
  char* m_passwd;

  /// Random seed file used for initializing Pseudo Random Number
  /// Generator (PRNG)
  OFString m_readSeedFile;

  /// Random seed file used for writing updated seed from Pseudo Random Number
  /// Generator back to a file
  OFString m_writeSeedFile;

  /// Denotes how certificates are handled, i.e.\ whether they are required, validated or
  /// only validated if present
  DcmCertificateVerification m_certVerification;

};

#endif // WITH_OPENSSL

#endif // TLSSCU_H
