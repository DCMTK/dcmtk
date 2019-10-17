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
, x509untrusted(NULL)
, errorCode(0)
{
  x509store = X509_STORE_new();
  x509untrusted = sk_X509_new_null();
}


SiCertificateVerifier::~SiCertificateVerifier()
{
  X509_STORE_free(x509store);
  sk_X509_free(x509untrusted);
}


X509_STORE *SiCertificateVerifier::getTrustedCertStore()
{
  return x509store;
}


stack_st_X509 *SiCertificateVerifier::getUntrustedCerts()
{
  return x509untrusted;
}


OFCondition SiCertificateVerifier::addTrustedCertificateFile(const char *fileName, int fileType)
{
  /* fileType should be X509_FILETYPE_PEM or X509_FILETYPE_ASN1 */
  X509_LOOKUP *x509_lookup = X509_STORE_add_lookup(x509store, X509_LOOKUP_file());
  if (x509_lookup == NULL) return SI_EC_OpenSSLFailure;
  if (! X509_LOOKUP_load_file(x509_lookup, fileName, fileType)) return SI_EC_CannotRead;
  return EC_Normal;
}


OFCondition SiCertificateVerifier::addUntrustedCertificateFile(const char *fileName, int fileType)
{
  OFCondition result = EC_Normal;
  if (x509untrusted)
  {
    // PEM has different loading code because a PEM file can contain several certificates and CRLs
    if (fileType == X509_FILETYPE_PEM)
    {
      BIO *bio = BIO_new_file(fileName, "r");
      if (bio)
      {
        STACK_OF(X509_INFO) *x509infostack = PEM_X509_INFO_read_bio(bio, NULL, NULL, NULL);
        if (x509infostack)
        {
          for (int i = 0; i < sk_X509_INFO_num(x509infostack); i++)
          {
            X509_INFO *xi = sk_X509_INFO_value(x509infostack, i);
            if (xi->x509)
            {
              // move certificate to our list of untrusted certificates
              sk_X509_push(x509untrusted, xi->x509);
              xi->x509 = NULL;
            }
          }
          // delete the remaining x509infostack
          sk_X509_INFO_pop_free(x509infostack, X509_INFO_free);
        } else result = SI_EC_CannotRead;
        BIO_free(bio);
      } else result = SI_EC_CannotRead;
    }
    else if (fileType == X509_FILETYPE_ASN1)
    {
      // load a single certificate in ASN.1 DER format
      BIO *bio = BIO_new_file(fileName, "rb");
      if (bio)
      {
        X509 *xcert = d2i_X509_bio(bio, NULL);
        if (xcert)
        {
          sk_X509_push(x509untrusted, xcert);
        } else result = SI_EC_CannotRead;
        BIO_free(bio);
      } else result = SI_EC_CannotRead;
    } else result = SI_EC_InvalidFiletype;
  } else result = SI_EC_CannotRead;

  return result;
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
  X509_STORE_CTX_init(ctx, x509store, rawcert, x509untrusted);

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
