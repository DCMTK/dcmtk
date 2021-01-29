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
 *    classes: SiPKEY
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sipkey.h"
#include "dcmtk/dcmsign/sicert.h"
#include "dcmtk/dcmsign/siprivat.h"

BEGIN_EXTERN_C
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
END_EXTERN_C

SiPKEY::SiPKEY(EVP_PKEY *key, OFBool owned)
: signing_key(key)
, owned_(owned)
{
}

SiPKEY::~SiPKEY()
{
  if (owned_ && signing_key) EVP_PKEY_free(signing_key);
}


OFCondition SiPKEY::sign(
    const unsigned char *inputHash,
    unsigned long inputHashSize,
    E_MACType inputHashAlgorithm,
    unsigned char *outputSignature,
    unsigned long &outputSignatureSize)
{
    if (signing_key==NULL) return SI_EC_InitializationFailed;
    const EVP_MD *openSSLmac = NULL;
    switch (inputHashAlgorithm)
    {
      case EMT_SHA1:
        openSSLmac = EVP_sha1();
        break;
      case EMT_RIPEMD160:
        openSSLmac = EVP_ripemd160();
        break;
      case EMT_MD5:
        openSSLmac = EVP_md5();
        break;
      case EMT_SHA256:
        openSSLmac = EVP_sha256();
        break;
      case EMT_SHA384:
        openSSLmac = EVP_sha384();
        break;
      case EMT_SHA512:
        openSSLmac = EVP_sha512();
        break;
    }

    E_KeyType keytype = keyType();
    size_t sigLen = outputSignatureSize;
    OFCondition result = EC_Normal;

    // create context structure
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(signing_key, NULL /* no engine */);
    if (ctx == NULL)
    {
      DCMSIGN_DEBUG("SiPKEY::sign(): call to EVP_PKEY_CTX_new() failed");
      result = SI_EC_OpenSSLFailure;
    }

    if (result.good())
    {
      // initialize signature operation
      if (EVP_PKEY_sign_init(ctx) <= 0)
      {
        DCMSIGN_DEBUG("SiPKEY::sign(): call to EVP_PKEY_sign_init() failed");
        result = SI_EC_OpenSSLFailure;
      }
    }

    if (result.good() && (keytype == EKT_RSA))
    {
      // set padding mode (only for RSA keys)
      if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0)
      {
        DCMSIGN_DEBUG("SiPKEY::sign(): call to EVP_PKEY_CTX_set_rsa_padding() failed");
        result = SI_EC_OpenSSLFailure;
      }
    }

    if (result.good() && (keytype == EKT_RSA))
    {
      // set MAC used for hash key generation (only for RSA keys)
      if (EVP_PKEY_CTX_set_signature_md(ctx, openSSLmac) <= 0)
      {
        DCMSIGN_DEBUG("SiPKEY::sign(): call to EVP_PKEY_CTX_set_signature_md() failed");
        result = SI_EC_OpenSSLFailure;
      }
    }

    if (result.good())
    {
      // finally, create the signature
      if (EVP_PKEY_sign(ctx, outputSignature, &sigLen, inputHash, inputHashSize) <= 0)
      {
        DCMSIGN_DEBUG("SiPKEY::sign(): call to EVP_PKEY_sign() failed");
        result = SI_EC_OpenSSLFailure;
      }
    }

    // clean up context. Call is safe if ctx==NULL.
    EVP_PKEY_CTX_free(ctx);

    outputSignatureSize = OFstatic_cast(unsigned long, sigLen);
    return result;
}


OFCondition SiPKEY::verify(
    const unsigned char *inputHash,
    unsigned long inputHashSize,
    E_MACType inputHashAlgorithm,
    const unsigned char *inputSignature,
    unsigned long inputSignatureSize,
    OFBool &verified)
{
    verified = OFFalse;
    if (signing_key==NULL) return SI_EC_InitializationFailed;

    const EVP_MD *openSSLmac = NULL;
    switch (inputHashAlgorithm)
    {
      case EMT_SHA1:
        openSSLmac = EVP_sha1();
        break;
      case EMT_RIPEMD160:
        openSSLmac = EVP_ripemd160();
        break;
      case EMT_MD5:
        openSSLmac = EVP_md5();
        break;
      case EMT_SHA256:
        openSSLmac = EVP_sha256();
        break;
      case EMT_SHA384:
        openSSLmac = EVP_sha384();
        break;
      case EMT_SHA512:
        openSSLmac = EVP_sha512();
        break;
    }

    E_KeyType keytype = keyType();
    OFCondition result = EC_Normal;

    // create context structure
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(signing_key, NULL /* no engine */);
    if (ctx == NULL)
    {
      DCMSIGN_DEBUG("SiPKEY::verify(): call to EVP_PKEY_CTX_new() failed");
      result = SI_EC_OpenSSLFailure;
    }

    if (result.good())
    {
      // initialize signature operation
      if (EVP_PKEY_verify_init(ctx) <= 0)
      {
        DCMSIGN_DEBUG("SiPKEY::verify(): call to EVP_PKEY_verify_init() failed");
        result = SI_EC_OpenSSLFailure;
      }
    }

    if (result.good() && (keytype == EKT_RSA))
    {
      // set padding mode (only for RSA keys)
      if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0)
      {
        DCMSIGN_DEBUG("SiPKEY::verify(): call to EVP_PKEY_CTX_set_rsa_padding() failed");
        result = SI_EC_OpenSSLFailure;
      }
    }

    if (result.good() && (keytype == EKT_RSA))
    {
      // set MAC used for hash key generation (only for RSA keys)
      if (EVP_PKEY_CTX_set_signature_md(ctx, openSSLmac) <= 0)
      {
        DCMSIGN_DEBUG("SiPKEY::verify(): call to EVP_PKEY_CTX_set_signature_md() failed");
        result = SI_EC_OpenSSLFailure;
      }
    }

    if (result.good())
    {
      // finally, create the signature
      if (EVP_PKEY_verify(ctx, inputSignature, inputSignatureSize, inputHash, inputHashSize) > 0) verified = OFTrue;
    }

    // clean up structures. Both calls are safe if parameter is NULL.
    EVP_PKEY_CTX_free(ctx);

    return result;
}

unsigned long SiPKEY::getSize() const
{
  if (signing_key == NULL) return 0;
  return EVP_PKEY_size(signing_key);
}


E_KeyType SiPKEY::keyType() const
{
  E_KeyType result = EKT_none;
  if (signing_key) switch(EVP_PKEY_type(EVP_PKEY_id(signing_key)))
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

  return result;
}

#else /* WITH_OPENSSL */

int sirsa_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
