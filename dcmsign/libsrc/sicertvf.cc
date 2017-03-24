/*
 *
 *  Copyright (C) 1998-2017, OFFIS e.V.
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

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sicert.h"
#include "dcmtk/dcmsign/sicertvf.h"

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


OFCondition SiCertificateVerifier::addTrustedCertificateFile(const char *fileName, int fileType)
{
  /* fileType should be X509_FILETYPE_PEM or X509_FILETYPE_ASN1 */
  X509_LOOKUP *x509_lookup = X509_STORE_add_lookup(x509store, X509_LOOKUP_file());
  if (x509_lookup == NULL) return SI_EC_OpenSSLFailure;
  if (! X509_LOOKUP_load_file(x509_lookup, fileName, fileType)) return SI_EC_CannotRead;
  return EC_Normal;
}


OFCondition SiCertificateVerifier::addTrustedCertificateDir(const char *pathName, int fileType)
{
  /* fileType should be X509_FILETYPE_PEM or X509_FILETYPE_ASN1 */
  X509_LOOKUP *x509_lookup = X509_STORE_add_lookup(x509store, X509_LOOKUP_hash_dir());
  if (x509_lookup == NULL) return SI_EC_OpenSSLFailure;
  if (! X509_LOOKUP_add_dir(x509_lookup, pathName, fileType)) return SI_EC_CannotRead;
  return EC_Normal;
}


OFCondition SiCertificateVerifier::addCertificateRevocationList(const char *fileName, int fileType)
{
  OFCondition result = SI_EC_CannotRead;
  X509_CRL *x509crl = NULL;
  if (fileName)
  {
    BIO *in = BIO_new(BIO_s_file());
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
            result = EC_Normal;
          }
        } else {
          x509crl = PEM_read_bio_X509_CRL(in, NULL, NULL, NULL);
          if (x509crl)
          {
            X509_STORE_add_crl(x509store, x509crl);
            result = EC_Normal;
          }
        }
      }
      BIO_free(in);
    }
  }
  return result;
}


OFCondition SiCertificateVerifier::verifyCertificate(SiCertificate& certificate)
{
  errorCode = 0;
  X509 *rawcert = certificate.getRawCertificate();
  if (rawcert == NULL) return SI_EC_VerificationFailed_NoCertificate;

  X509_STORE_CTX *ctx = X509_STORE_CTX_new();
  X509_STORE_CTX_init(ctx, x509store, rawcert, NULL);

  // If a complete chain can be built and validated X509_verify_cert() returns 1,
  // otherwise it returns zero, in exceptional circumstances it can also return a negative code.
  int ok = X509_verify_cert(ctx);

  errorCode = X509_STORE_CTX_get_error(ctx);
  X509_STORE_CTX_cleanup(ctx);
  X509_STORE_CTX_free(ctx);

  if (ok == 1) return EC_Normal; else return SI_EC_VerificationFailed_NoTrust;
}


const char *SiCertificateVerifier::lastError() const
{
  return X509_verify_cert_error_string(errorCode);
}


#else /* WITH_OPENSSL */

int sicertvf_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
