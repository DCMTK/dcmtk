/*
 *
 *  Copyright (C) 1998-2021, OFFIS e.V.
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
 *  Author: Norbert Loxen, Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiPrivateKey
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/siprivat.h"
#include "dcmtk/dcmsign/sipkey.h"
#include "dcmtk/dcmsign/sicert.h"

BEGIN_EXTERN_C
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
END_EXTERN_C

#ifndef HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_ID
#define EVP_PKEY_id(key) key->type
#endif

/* buf     : buffer to write password into
 * size    : length of buffer in bytes
 * rwflag  : nonzero if the password will be used as a new password, i.e. user should be asked to repeat the password
 * userdata: arbitrary pointer that can be set with SSL_CTX_set_default_passwd_cb_userdata()
 * returns : number of bytes written to password buffer, -1 upon error
 */
extern "C" int SiPrivateKey_passwordCallback(char *buf, int size, int rwflag, void *userdata);

int SiPrivateKey_passwordCallback(char *buf, int size, int /* rwflag */, void *userdata)
{
  if (userdata == NULL) return -1;
  OFString *password = (OFString *)userdata;
  int passwordSize = OFstatic_cast(int, password->length());
  if (passwordSize > size) passwordSize = size;
  strncpy(buf, password->c_str(), passwordSize);
  return passwordSize;
}


SiPrivateKey::SiPrivateKey()
: privateKeyPasswd()
, usePrivateKeyPassword(OFFalse)
, pkey(NULL)
{
}


SiPrivateKey::~SiPrivateKey()
{
  if (pkey) EVP_PKEY_free(pkey);
}


void SiPrivateKey::setPrivateKeyPasswd(const char *thePasswd)
{
  if (thePasswd) privateKeyPasswd = thePasswd;
  else privateKeyPasswd.clear();
  usePrivateKeyPassword = OFTrue;
  return;
}


void SiPrivateKey::setPrivateKeyPasswdFromConsole()
{
  privateKeyPasswd.clear();
  usePrivateKeyPassword = OFFalse;
  return;
}


OFCondition SiPrivateKey::loadPrivateKey(const char *filename, int filetype)
{
  OFCondition result = SI_EC_CannotRead;
  if (pkey) EVP_PKEY_free(pkey);
  pkey = NULL;
  if (filename)
  {
    BIO *in = BIO_new(BIO_s_file());
    if (in)
    {
      if (BIO_read_filename(in, filename) > 0)
      {
        if (filetype == X509_FILETYPE_ASN1)
        {
          // ASN.1/DER encoded keys are never encrypted, thus no callback here.
          pkey = d2i_PrivateKey_bio(in, NULL);
          if (pkey) result = EC_Normal;
        } else {
          if (usePrivateKeyPassword)
            pkey = PEM_read_bio_PrivateKey(in, NULL, SiPrivateKey_passwordCallback, &privateKeyPasswd);
            else pkey = PEM_read_bio_PrivateKey(in, NULL, NULL, NULL);  // read password from console
          if (pkey) result = EC_Normal;
        }
      }
      BIO_free(in);
    }
  }
  return result;
}


E_KeyType SiPrivateKey::getKeyType() const
{
  E_KeyType result = EKT_none;
  if (pkey)
  {
    switch(EVP_PKEY_type(EVP_PKEY_id(pkey)))
    {
      case EVP_PKEY_RSA:
        result = EKT_RSA;
        break;
      case EVP_PKEY_DSA:
        result = EKT_DSA;
        break;
      case EVP_PKEY_DH:
        result = EKT_DH;
        break;
      case EVP_PKEY_EC:
        result = EKT_EC;
        break;
      default:
        /* nothing */
        break;
    }
  }
  return result;
}


SiAlgorithm *SiPrivateKey::createAlgorithmForPrivateKey()
{
  if (pkey) return new SiPKEY(pkey, OFFalse);
  return NULL;
}


OFBool SiPrivateKey::matchesCertificate(SiCertificate& cert)
{
  X509 *x509 = cert.getRawCertificate();
  if ((x509 == NULL)||(pkey == NULL)) return OFFalse;
  if (X509_check_private_key(x509, pkey)) return OFTrue;
  return OFFalse;
}


EVP_PKEY *SiPrivateKey::getRawPrivateKey()
{
  return pkey;
}


#else /* WITH_OPENSSL */

int siprivat_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
