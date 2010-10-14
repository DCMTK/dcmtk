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
 *  Author: Norbert Loxen, Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiPrivateKey
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:38 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/siprivat.h"
#include "dcmtk/dcmsign/sirsa.h"
#include "dcmtk/dcmsign/sidsa.h"
#include "dcmtk/dcmsign/sicert.h"

#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
END_EXTERN_C


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
  int passwordSize = password->length();
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
    BIO *in = BIO_new(BIO_s_file_internal());
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
    switch(pkey->type)
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
      default:
        /* nothing */
        break;
    }
  }
  return result;
}


SiAlgorithm *SiPrivateKey::createAlgorithmForPrivateKey()
{
  if (pkey)
  {
    switch(pkey->type)
    {
      case EVP_PKEY_RSA:
        return new SiRSA(EVP_PKEY_get1_RSA(pkey));
        /* break; */
      case EVP_PKEY_DSA:
        return new SiDSA(EVP_PKEY_get1_DSA(pkey));
        /* break; */
      case EVP_PKEY_DH:
      default:
        /* nothing */
        break;
    }
  }    
  return NULL;
}


OFBool SiPrivateKey::matchesCertificate(SiCertificate& cert)
{
  X509 *x509 = cert.getRawCertificate();
  if ((x509 == NULL)||(pkey == NULL)) return OFFalse;
  if (X509_check_private_key(x509, pkey)) return OFTrue;
  return OFFalse;
}

#else /* WITH_OPENSSL */

int siprivat_cc_dummy_to_keep_linker_from_moaning = 0;

#endif


/*
 *  $Log: siprivat.cc,v $
 *  Revision 1.8  2010-10-14 13:14:38  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.7  2010-06-25 09:15:19  uli
 *  Fixed issues with compiling with HAVE_STD_STRING.
 *
 *  Revision 1.6  2005-12-08 15:47:27  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.5  2002/12/16 12:57:52  meichel
 *  Minor modification to shut up linker on MacOS X when compiling
 *    without OpenSSL support
 *
 *  Revision 1.4  2001/09/26 14:30:25  meichel
 *  Adapted dcmsign to class OFCondition
 *
 *  Revision 1.3  2001/06/01 15:50:54  meichel
 *  Updated copyright header
 *
 *  Revision 1.2  2000/11/14 13:54:20  meichel
 *  Renamed callback functions to avoid linker name clashes
 *
 *  Revision 1.1  2000/11/07 16:49:06  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

