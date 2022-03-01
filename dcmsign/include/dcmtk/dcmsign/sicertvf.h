/*
 *
 *  Copyright (C) 1998-2019, OFFIS e.V.
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
 *  Module: dcmsign
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiCertificateVerifier
 *
 */

#ifndef SICERTVF_H
#define SICERTVF_H

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sitypes.h"

class SiCertificate;
struct x509_store_st;
struct x509_store_ctx_st;
struct stack_st_X509; // this is STACK_OF(X509) in OpenSSL
typedef struct x509_store_st X509_STORE;
typedef struct x509_store_ctx_st X509_STORE_CTX;

/** a class representing X.509 public key certificates.
 *  @remark this class is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
class DCMTK_DCMSIGN_EXPORT SiCertificateVerifier
{
public:

  /// default constructor
  SiCertificateVerifier();

  ///destructor
  virtual ~SiCertificateVerifier();

  /** loads a certificate from a file and adds it to the pool of trusted certificates.
   *  @param fileName path to the certificate file
   *  @param fileType file format: X509_FILETYPE_PEM or X509_FILETYPE_ASN1
   *  @return SI_EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition addTrustedCertificateFile(const char *fileName, int fileType);

  /** loads an untrusted certificate from a file and adds it to the pool
   *  of untrusted certificates. During certificate verification these will
   *  only be accepted as intermediate CAs (not as root CA) and will undergo
   *  additional scrutiny (e.g. check of the purpose extension, if present).
   *  @param fileName path to the certificate file
   *  @param filetype file format: X509_FILETYPE_PEM or X509_FILETYPE_ASN1
   *  @return SI_EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition addUntrustedCertificateFile(const char *fileName, int fileType);

  /** loads all files as certificates from the specified directory and adds them
   *  to the pool of trusted certificates.
   *  @param pathName path to the directory containing certificate files
   *  @param fileType file format: X509_FILETYPE_PEM or X509_FILETYPE_ASN1
   *  @return SI_EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition addTrustedCertificateDir(const char *pathName, int fileType);

  /** loads a certificate revocation list (CRL) in X.509 format from a file and
   *  adds it to the pool of trusted certificates and CRLs.
   *  @param fileName path to the CRL file
   *  @param fileType file format: X509_FILETYPE_PEM or X509_FILETYPE_ASN1
   *  @return SI_EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition addCertificateRevocationList(const char *fileName, int fileType);

  /** verifies a certificate against the known trusted CA certificates
   *  and certificate revocation lists. Returns a status flag and stores
   *  a detailed error description that can be retrieved with lastError().
   *  @param certificate the certificate to verify
   *  @return SI_EC_Normal if successful, an error code otherwise.
   *     If the certificate could not be verified, returns SI_EC_VerificationFailed_NoTrust.
   */
  virtual OFCondition verifyCertificate(SiCertificate& certificate);

  /** returns an error string containing a textual description of the result
   *  of the last call to verifyCertificate() if that call returned
   *  SI_EC_VerificationFailed_NoTrust.
   *  @return text string
   */
  virtual const char *lastError() const;

  /** returns true if the result of the last call to verifyCertificate()
   *  was the status code indicating that the certificate has expired,
   *  false otherwise
   *  @return true if verifyCertificate() reported certificate expiry.
   */
  virtual OFBool lastErrorIsCertExpiry() const;

  /** returns a pointer to the trusted certificate store managed by this object.
   *  @return pointer to trusted certificate store
   */
  virtual X509_STORE *getTrustedCertStore();

  /** returns a pointer to the stack of untrusted certificates managed by this object.
   *  Note that the return type is equivalent to OpenSSL's STACK_OF(X509).
   *  @return pointer to stack of untrusted certificates
   */
  virtual stack_st_X509 *getUntrustedCerts();

  /** enable or disable the verification of certificate revocation lists.
   *  When enabled, a CRL is expected to be present for every CA certificate,
   *  and certificate verification will fail if no CRL is found.
   *  @param enabled OFTrue to enable verification, OFFalse to disable
   */
  virtual void setCRLverification(OFBool enabled);

  /** Callback function for certificate verification operations.
   *  This method can be used by derived classes to examine and modify the
   * result of a certificate verification.
   *  @param deflt default return code that should be returned if the callback does
   *    not modify the result of the verification
   *  @param ctx certificate verification context object
   *  @return result of the verification, 0 for error, 1 for no error, 2 for "policy checking complete".
   */
  virtual int verifyCallback(int deflt, X509_STORE_CTX *ctx);

private:

  /// private undefined copy constructor
  SiCertificateVerifier(SiCertificateVerifier& arg);

  /// private undefined copy assignment operator
  SiCertificateVerifier& operator=(SiCertificateVerifier& arg);

  /// OpenSSL X.509 certificate store
  X509_STORE *x509store;

  /// OpenSSL X.509 stack of untrusted intermediate certificates
  stack_st_X509 *x509untrusted;

  /// flag indicating whether CRL verification should be enabled
  OFBool enableCRLverification;

  /// OpenSSL X.509 certificate verification error code for the last operation
  long errorCode;

};

#endif
#endif

