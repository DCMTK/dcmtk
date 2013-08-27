/*
 *
 *  Copyright (C) 2010-2013, OFFIS e.V.
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
 *  @warning This class is EXPERIMENTAL. Be careful to use it in production environment.
 */
class DCMTK_DCMTLS_EXPORT DcmTLSSCU : public DcmSCU
{

public:

  /** Constructor, just initializes internal class members
   */
  DcmTLSSCU();

  /** Initialize SCU with connection peer information
   *  @param peerHost [in] The hostname or IP of the host to talk to
   *  @param peerAETtitle [in] AETitle of peer host to be used
   *  @param portNum [in] TCP/IP port number to talk to
   */
  DcmTLSSCU(const OFString& peerHost,
            const OFString& peerAETitle,
            const Uint16 portNum);

  /** Virtual destructor
    */
  virtual ~DcmTLSSCU();

  /** Initialize network, i.e. prepare for association negotiation.
   *  @return EC_Normal if initialization was successful, otherwise error code
   */
  virtual OFCondition initNetwork();

  /** Negotiate association by using presentation contexts and parameters
   *  as defined by earlier function calls.
   *  @return EC_Normal if negotiation was successful, otherwise error code
   */
  virtual OFCondition negotiateAssociation();

  /** Closes the association of this SCU
   *  @deprecated The use of this method is deprecated. Please use
   *    DcmSCU::releaseAssociation() or DcmSCU::abortAssociation() instead.
   *  @param closeType [in] Define whether to release or abort the association
   */
  virtual void closeAssociation(const DcmCloseAssociationType closeType);

  /** TODO: Not documented yet
   *  @param str [in] Not documented yet
   */
  virtual void addTrustedCertFile(const OFString& str);

  /** TODO: Not documented yet
   *  @param certDir [in] Not documented yet
   */
  virtual void addTrustedCertDir(const OFString& certDir);

  /** TODO: Not documented yet
   *  @param privateKey    [in] File with private key to authenticate with
   *  @param certFile      [in] File with certificate to authenticate with
   *  @param passphrase    [in] Password to access key. NULL asks user on console.
   *                            "" will send empty password. Default is asking the
   *                            user to enter password.
   *  @param privKeyFormat [in] Format of private key parameter. Default is
   *                            SSL_FILETYPE_PEM.
   *  @param certFormat    [in] Format of certificate file parameter. Default is
   *                            SSL_FILETYPE_PEM.
   */
  virtual void enableAuthentication(const OFString& privateKey,
                                    const OFString& certFile,
                                    const char* passphrase = NULL,
                                    const int privKeyFormat = SSL_FILETYPE_PEM,
                                    const int certFormat = SSL_FILETYPE_PEM);

  /** Disables authentication. However, DcmTLSSCU will try to establish secured connection
   *  in terms of encrypting data. Default is that authentication is disabled.
   */
  virtual void disableAuthentication();

  /** TODO: Not documented yet
   *  @param cs [in] TODO: Not documented yet
   */
  virtual void addCiphersuite(const OFString& cs);

  /** TODO: Not documented yet
   *  @param seedFile [in] TODO: Not documented yet
   */
  virtual void setReadSeedFile(const OFString& seedFile);

  /** TODO: Not documented yet
   *  @param seedFile [in] TODO: Not documented yet
   */
  virtual void setWriteSeedFile(const OFString& seedFile);

  /** Set whether peer's certificate must be there, only is checked if there or is ignored
   *  @param cert [in] peer certificate verification mode. The following values are permitted:
   *                   DCV_requireCertificate, DCV_checkCertificate, DCV_ignoreCertificate
   */
  virtual void setPeerCertVerification(const DcmCertificateVerification cert);

  /** Not documented yet
   *  @param dhParam [in] TODO: Not documented yet
   */
  virtual void setDHParam(const OFString& dhParam);

  /** Returns OFTrue if authentication is enabled
   *  @param ... TODO: Not documented yet
   *  @return Return value OFTrue
   */
  virtual OFBool getAuthenticationParams(OFString& privKeyFile,
                                         OFString& certFile,
                                         const char*& passphrase,
                                         int& privKeyFormat,
                                         int& certFormat) const;

  /** TODO: Not documented yet
   *  @param files [out] TODO: Not documented yet
   */
  virtual void getTrustedCertFiles(OFList<OFString>& files /*out*/) const;

  /** TODO: Not documented yet
   *  @param trustedDirs [out] TODO: Not documented yet
   */
  virtual void getTrustedCertDirs(OFList<OFString>& trustedDirs /*out*/) const;

  /** TODO: Not documented yet
   *  @return TODO: Not documented yet
   */
  virtual OFString getCiphersuites() const;

  /** TODO: Not documented yet
   *  @return TODO: Not documented yet
   */
  virtual OFString getReadSeedFile() const;

  /** TODO: Not documented yet
   *  @return TODO: Not documented yet
   */
  virtual OFString getWriteSeedFile() const;

  /** TODO: Not documented yet
   *  @return TODO: Not documented yet
   */
  virtual OFString getDHParam() const;

private:

  /** Private undefined copy-constructor. Shall never be called.
   *  @param src Source object
   */
  DcmTLSSCU(const DcmTLSSCU &src);

  /** Private undefined operator=. Shall never be called.
   *  @param src Source object
   *  @return Reference to this
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

  /// Might be either "SSL_FILETYPE_PEM" (default) or "SSL_FILETYPE_ASN1"
  int m_privateKeyFileFormat;

  /// File containing the certificate the SCU should use for authentication
  OFString m_certificateFile;

  /// Might be either "SSL_FILETYPE_PEM" (default) or "SSL_FILETYPE_ASN1"
  int m_certKeyFileFormat;

  /// Password used for unlocking the private key file. If NULL, password is asked from
  /// STDIN. If empty string (""), password is sent empty. All others values are sent as
  /// given.
  char* m_passwd;

  /// String containing all cipher suites being (additionally? TODO) configured
  OFString m_ciphersuites;

  /// TODO: Not documented yet
  OFString m_readSeedFile;

  /// TODO: Not documented yet
  OFString m_writeSeedFile;

  /// Denotes how certificates are handled, i.e. whether they are required, validated or
  /// only validated if present
  DcmCertificateVerification m_certVerification;

  /// File containing Diffie-Hellman parameters to be used
  OFString m_dhparam;

};

#endif // WITH_OPENSSL

#endif // TLSSCU_H
