/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:24 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
class SiCertificateVerifier
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

/*
 *  $Log: sicertvf.h,v $
 *  Revision 1.6  2010-10-14 13:17:24  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.5  2005-12-08 16:04:34  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.4  2003/06/04 14:21:03  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.3  2001/09/26 14:30:19  meichel
 *  Adapted dcmsign to class OFCondition
 *
 *  Revision 1.2  2001/06/01 15:50:47  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2001/01/25 15:11:43  meichel
 *  Added class SiCertificateVerifier in dcmsign which allows to check
 *    whether a certificate from a digital signature is trusted, i.e. issued
 *    by a known CA and not contained in a CRL.
 *
 *  Revision 1.1  2000/11/07 16:48:53  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

