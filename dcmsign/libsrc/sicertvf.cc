/*
 *
 *  Copyright (C) 1998-2000, OFFIS
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
 *  Module: dcmsign
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiCertificateVerifier
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-01-25 15:11:47 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"

#ifdef WITH_OPENSSL

#include "sicert.h"
#include "sicertvf.h"

BEGIN_EXTERN_C
#include <openssl/pem.h>
#include <openssl/x509.h>
END_EXTERN_C


SiCertificateVerifier::SiCertificateVerifier()
: x509store(NULL)
, errorCode(0)
{
  x509store = X509_STORE_new();
}

SiCertificateVerifier::~SiCertificateVerifier()
{
  if (x509store) X509_STORE_free(x509store);	
}


SI_E_Condition SiCertificateVerifier::addTrustedCertificateFile(const char *fileName, int fileType)
{
  /* fileType should be X509_FILETYPE_PEM or X509_FILETYPE_ASN1 */
  X509_LOOKUP *x509_lookup = X509_STORE_add_lookup(x509store, X509_LOOKUP_file());
  if (x509_lookup == NULL) return SI_EC_OpenSSLFailure;
  if (! X509_LOOKUP_load_file(x509_lookup, fileName, fileType)) return SI_EC_CannotRead;
  return SI_EC_Normal;
}


SI_E_Condition SiCertificateVerifier::addTrustedCertificateDir(const char *pathName, int fileType)
{
  /* fileType should be X509_FILETYPE_PEM or X509_FILETYPE_ASN1 */
  X509_LOOKUP *x509_lookup = X509_STORE_add_lookup(x509store, X509_LOOKUP_hash_dir());
  if (x509_lookup == NULL) return SI_EC_OpenSSLFailure;
  if (! X509_LOOKUP_add_dir(x509_lookup, pathName, fileType)) return SI_EC_CannotRead;
  return SI_EC_Normal;
}


SI_E_Condition SiCertificateVerifier::addCertificateRevocationList(const char *fileName, int fileType)
{
  SI_E_Condition result = SI_EC_CannotRead;  
  X509_CRL *x509crl = NULL;
  if (fileName)
  {
    BIO *in = BIO_new(BIO_s_file_internal());
    if (in)
    {
      if (BIO_read_filename(in, fileName) > 0)
      {
        if (fileType == X509_FILETYPE_ASN1)
        {
          x509crl = d2i_X509_CRL_bio(in, NULL);
          if (x509crl) 
          {
            X509_STORE_add_crl(x509store, x509crl);
            result = SI_EC_Normal;
          }
        } else {
          x509crl = PEM_read_bio_X509_CRL(in, NULL, NULL, NULL);
          if (x509crl) 
          {
            X509_STORE_add_crl(x509store, x509crl);
            result = SI_EC_Normal;
          }
        }
      }
      BIO_free(in);
    }
  }
  return result;
}


SI_E_Condition SiCertificateVerifier::verifyCertificate(SiCertificate& certificate)
{
  errorCode = 0;
  X509 *rawcert = certificate.getRawCertificate();
  if (rawcert == NULL) return SI_EC_VerificationFailed_NoCertificate;

  X509_STORE_CTX ctx;
  X509_STORE_CTX_init(&ctx, x509store, rawcert, NULL);
  int ok = X509_verify_cert(&ctx); /* returns nonzero if successful */
  errorCode = X509_STORE_CTX_get_error(&ctx);
  X509_STORE_CTX_cleanup(&ctx);
  if (ok) return SI_EC_Normal; else return SI_EC_VerificationFailed_NoTrust;
}


const char *SiCertificateVerifier::lastError() const
{
  return X509_verify_cert_error_string(errorCode);
}


#else /* WITH_OPENSSL */

const int sicertvf_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: sicertvf.cc,v $
 *  Revision 1.1  2001-01-25 15:11:47  meichel
 *  Added class SiCertificateVerifier in dcmsign which allows to check
 *    whether a certificate from a digital signature is trusted, i.e. issued
 *    by a known CA and not contained in a CRL.
 *
 *
 */

