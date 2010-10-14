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
 *  Update Date:      $Date: 2010-10-14 13:14:38 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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

  X509_STORE_CTX ctx;
  X509_STORE_CTX_init(&ctx, x509store, rawcert, NULL);
  int ok = X509_verify_cert(&ctx); /* returns nonzero if successful */
  errorCode = X509_STORE_CTX_get_error(&ctx);
  X509_STORE_CTX_cleanup(&ctx);
  if (ok) return EC_Normal; else return SI_EC_VerificationFailed_NoTrust;
}


const char *SiCertificateVerifier::lastError() const
{
  return X509_verify_cert_error_string(errorCode);
}


#else /* WITH_OPENSSL */

int sicertvf_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: sicertvf.cc,v $
 *  Revision 1.6  2010-10-14 13:14:38  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.5  2005-12-08 15:47:21  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.4  2002/12/16 12:57:50  meichel
 *  Minor modification to shut up linker on MacOS X when compiling
 *    without OpenSSL support
 *
 *  Revision 1.3  2001/09/26 14:30:24  meichel
 *  Adapted dcmsign to class OFCondition
 *
 *  Revision 1.2  2001/06/01 15:50:53  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2001/01/25 15:11:47  meichel
 *  Added class SiCertificateVerifier in dcmsign which allows to check
 *    whether a certificate from a digital signature is trusted, i.e. issued
 *    by a known CA and not contained in a CRL.
 *
 *
 */

