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
 *  Author: Norbert Loxen, Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiECDSA
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/siecdsa.h"
#include "dcmtk/dcmsign/sicert.h"
#include "dcmtk/dcmsign/siprivat.h"

BEGIN_EXTERN_C
#include <openssl/err.h>
#include <openssl/evp.h>
#ifndef OPENSSL_NO_EC
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#endif
END_EXTERN_C

SiECDSA::SiECDSA(EC_KEY *key)
: ecdsa(key)
{
}

#ifndef OPENSSL_NO_EC

SiECDSA::~SiECDSA()
{
  if (ecdsa) EC_KEY_free(ecdsa);
}


OFCondition SiECDSA::sign(
    const unsigned char *inputHash,
    unsigned long inputHashSize,
    E_MACType inputHashAlgorithm,
    unsigned char *outputSignature,
    unsigned long &outputSignatureSize)
{
    if (ecdsa==NULL) return SI_EC_InitializationFailed;
    int openSSLmac = 0;
    switch (inputHashAlgorithm)
    {
      case EMT_SHA1:
        openSSLmac = NID_sha1;
        break;
      case EMT_RIPEMD160:
        openSSLmac = NID_ripemd160;
        break;
      case EMT_MD5:
        openSSLmac = NID_md5;
        break;
      case EMT_SHA256:
        openSSLmac = NID_sha256;
        break;
      case EMT_SHA384:
        openSSLmac = NID_sha384;
        break;
      case EMT_SHA512:
        openSSLmac = NID_sha512;
        break;
    }
    unsigned int sigLen = 0;
    int error = ECDSA_sign(openSSLmac, inputHash, (unsigned int)inputHashSize, outputSignature, &sigLen, ecdsa);
    outputSignatureSize = sigLen;
    if (error < 0) return SI_EC_OpenSSLFailure;
    return EC_Normal;
}


OFCondition SiECDSA::verify(
    const unsigned char *inputHash,
    unsigned long inputHashSize,
    E_MACType inputHashAlgorithm,
    const unsigned char *inputSignature,
    unsigned long inputSignatureSize,
    OFBool &verified)
{
    verified = OFFalse;
    if (ecdsa==NULL) return SI_EC_InitializationFailed;
    int openSSLmac = 0;
    switch (inputHashAlgorithm)
    {
      case EMT_SHA1:
        openSSLmac = NID_sha1;
        break;
      case EMT_RIPEMD160:
        openSSLmac = NID_ripemd160;
        break;
      case EMT_MD5:
        openSSLmac = NID_md5;
        break;
      case EMT_SHA256:
        openSSLmac = NID_sha256;
        break;
      case EMT_SHA384:
        openSSLmac = NID_sha384;
        break;
      case EMT_SHA512:
        openSSLmac = NID_sha512;
        break;
    }

    // we have to cast away const on inputSignature yet because of OpenSSL limitations
    int error = ECDSA_verify(openSSLmac, inputHash, (unsigned int)inputHashSize, (unsigned char *)inputSignature, (unsigned int)inputSignatureSize, ecdsa);

    if (error < 0)
    {
      const char *err = ERR_reason_error_string(ERR_peek_error());
      if (err) DCMSIGN_ERROR("OpenSSL error: " << err);
      return SI_EC_OpenSSLFailure;
    }
    else if (error > 0) verified = OFTrue;

    return EC_Normal;
}


unsigned long SiECDSA::getSize() const
{
  if (ecdsa == NULL) return 0;
  return ECDSA_size(ecdsa);
}

#else /* OPENSSL_NO_EC */

SiECDSA::~SiECDSA()
{
}

OFCondition SiECDSA::sign(
    const unsigned char * /* inputHash */,
    unsigned long /* inputHashSize */,
    E_MACType /* inputHashAlgorithm */,
    unsigned char * /* outputSignature */,
    unsigned long& /* outputSignatureSize */)
{
    return SI_EC_EllipticCurveNotSupported;
}

OFCondition SiECDSA::verify(
    const unsigned char * /* inputHash */,
    unsigned long /* inputHashSize */,
    E_MACType /* inputHashAlgorithm */,
    const unsigned char * /* inputSignature */,
    unsigned long /* inputSignatureSize */,
    OFBool& /* verified */)
{
    return SI_EC_EllipticCurveNotSupported;
}

unsigned long SiECDSA::getSize() const
{
  return 0;
}

#endif /* OPENSSL_NO_EC */

E_KeyType SiECDSA::keyType() const
{
  return EKT_EC;
}

#else /* WITH_OPENSSL */

int SiECDSA_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
