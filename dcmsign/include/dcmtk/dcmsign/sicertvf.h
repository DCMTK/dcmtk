/*
 *
 *  Copyright (C) 1998-2011, OFFIS e.V.
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
#include "dcmtk/dcmsign/sitypes.h"

#ifdef WITH_OPENSSL

class SiCertificate;
struct x509_store_st;
typedef struct x509_store_st X509_STORE;

/** a class representing X.509 public key certificates.
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
   *  @param filetype file format: X509_FILETYPE_PEM or X509_FILETYPE_ASN1
   *  @return SI_EC_Normal if successful, an error code otherwise
   */  
  OFCondition addTrustedCertificateFile(const char *fileName, int fileType);

  /** loads all files as certificates from the specified directory and adds them
   *  to the pool of trusted certificates.
   *  @param fileName path to the directory containing certificate files
   *  @param filetype file format: X509_FILETYPE_PEM or X509_FILETYPE_ASN1
   *  @return SI_EC_Normal if successful, an error code otherwise
   */
  OFCondition addTrustedCertificateDir(const char *pathName, int fileType);

  /** loads a certificate revocation list (CRL) in X.509 format from a file and 
   *  adds it to the pool of trusted certificates and CRLs.
   *  @param fileName path to the CRL file
   *  @param filetype file format: X509_FILETYPE_PEM or X509_FILETYPE_ASN1
   *  @return SI_EC_Normal if successful, an error code otherwise
   */  
  OFCondition addCertificateRevocationList(const char *fileName, int fileType);

  /** verifies a certificate against the known trusted CA certificates
   *  and certificate revocation lists. Returns a status flag and stores
   *  a detailed error description that can be retrieved with lastError().
   *  @param certificate the certificate to verify
   *  @return SI_EC_Normal if successful, an error code otherwise.
   *     If the certificate could not be verified, returns SI_EC_VerificationFailed_NoTrust.
   */
  OFCondition verifyCertificate(SiCertificate& certificate);

  /** returns an error string containing a textual description of the result
   *  of the last call to verifyCertificate() if that call returned 
   *  SI_EC_VerificationFailed_NoTrust.
   *  @return text string
   */
  const char *lastError() const;

private:

  /// private undefined copy constructor
  SiCertificateVerifier(SiCertificateVerifier& arg);

  /// private undefined copy assignment operator
  SiCertificateVerifier& operator=(SiCertificateVerifier& arg);

  /// OpenSSL X.509 certificate store
  X509_STORE* x509store;

  /// OpenSSL X.509 certificate verification error code for the last operation
  long errorCode;

};

#endif
#endif
