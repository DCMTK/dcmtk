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
 *    classes: SiMDMAC
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/simdmac.h"
#include "dcmtk/dcmdata/dcerror.h"

BEGIN_EXTERN_C
#include <openssl/evp.h>
END_EXTERN_C


SiMDMAC::SiMDMAC(E_MACType mactype)
: ctx(NULL)
, macType_(mactype)
{
  initialize();
}

SiMDMAC::~SiMDMAC()
{
  EVP_MD_CTX_destroy(ctx); // This is the same as EVP_MD_CTX_free() in OpenSSL 3.0
}

unsigned long SiMDMAC::getSize() const
{
  if (ctx) return EVP_MD_CTX_size(ctx);
  return 0;
}

OFCondition SiMDMAC::initialize()
{
  EVP_MD_CTX_destroy(ctx); // This is the same as EVP_MD_CTX_free() in OpenSSL 3.0

  ctx = EVP_MD_CTX_create(); // This is the same as EVP_MD_CTX_new() in OpenSSL 3.0
  if (ctx==NULL) return SI_EC_InitializationFailed;

  const EVP_MD *md_type = NULL;
  switch (macType_)
  {
    case EMT_SHA1:
      md_type = EVP_sha1();
      break;
    case EMT_RIPEMD160:
      md_type = EVP_ripemd160();
      break;
    case EMT_MD5:
      md_type = EVP_md5();
      break;
    case EMT_SHA256:
      md_type = EVP_sha256();
      break;
    case EMT_SHA384:
      md_type = EVP_sha384();
      break;
    case EMT_SHA512:
      md_type = EVP_sha512();
      break;
  }
  if (md_type==NULL) return SI_EC_InitializationFailed;

  if (EVP_DigestInit_ex(ctx, md_type, NULL) <= 0)
  {
     DCMSIGN_DEBUG("SiMDMAC::initialize(): call to EVP_DigestInit_ex() failed");
     EVP_MD_CTX_destroy(ctx); // This is the same as EVP_MD_CTX_free() in OpenSSL 3.0
     ctx = NULL;
     return SI_EC_OpenSSLFailure;
  }

  return EC_Normal;
}

OFCondition SiMDMAC::digest(const unsigned char *data, unsigned long length)
{
  if (length == 0) return EC_Normal;
  if (ctx==NULL) return SI_EC_InitializationFailed;
  if (data == NULL) return EC_IllegalCall;

  if (EVP_DigestUpdate(ctx, data, length) <= 0)
  {
     DCMSIGN_DEBUG("SiMDMAC::digest(): call to EVP_DigestUpdate() failed");
     return SI_EC_OpenSSLFailure;
  }
  return EC_Normal;
}

OFCondition SiMDMAC::finalize(unsigned char *result)
{
  if (ctx==NULL) return SI_EC_InitializationFailed;
  if (result==NULL) return EC_IllegalCall;
  if (EVP_DigestFinal_ex(ctx, result, NULL) <= 0)
  {
     DCMSIGN_DEBUG("SiMDMAC::finalize(): call to EVP_DigestFinal_ex() failed");
     return SI_EC_OpenSSLFailure;
  }
  return EC_Normal;
}

E_MACType SiMDMAC::macType() const
{
  return macType_;
}

const char *SiMDMAC::getDefinedTerm() const
{
  switch (macType_)
  {
    case EMT_SHA1:
      return SI_DEFTERMS_SHA1;
      break;
    case EMT_RIPEMD160:
      return SI_DEFTERMS_RIPEMD160;
      break;
    case EMT_MD5:
      return SI_DEFTERMS_MD5;
      break;
    case EMT_SHA256:
      return SI_DEFTERMS_SHA256;
      break;
    case EMT_SHA384:
      return SI_DEFTERMS_SHA384;
      break;
    case EMT_SHA512:
      return SI_DEFTERMS_SHA512;
      break;
  }
  return "UNKNOWN_MAC";
}

#else /* WITH_OPENSSL */

int simdmac_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
