/*
 *
 *  Copyright (C) 1998-2016, OFFIS e.V.
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
 *    classes: SiDSA
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sidsa.h"
#include "dcmtk/dcmsign/sicert.h"
#include "dcmtk/dcmsign/siprivat.h"

BEGIN_EXTERN_C
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/dsa.h>
END_EXTERN_C

SiDSA::SiDSA(DSA *key)
: dsa(key)
{
}


SiDSA::~SiDSA()
{
  if (dsa) DSA_free(dsa);
}


OFCondition SiDSA::sign(
    const unsigned char *inputHash, 
    unsigned long inputHashSize,
    E_MACType inputHashAlgorithm,
    unsigned char *outputSignature,
    unsigned long &outputSignatureSize)
{
    if (dsa==NULL) return SI_EC_InitializationFailed;    
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
    int error = DSA_sign(openSSLmac, inputHash, (unsigned int)inputHashSize, outputSignature, &sigLen, dsa);
    outputSignatureSize = sigLen;    
    if (error < 0) return SI_EC_OpenSSLFailure;
    return EC_Normal;    
}


OFCondition SiDSA::verify(
    const unsigned char *inputHash, 
    unsigned long inputHashSize,
    E_MACType inputHashAlgorithm,
    const unsigned char *inputSignature,
    unsigned long inputSignatureSize,
    OFBool &verified)
{
    verified = OFFalse;
    if (dsa==NULL) return SI_EC_InitializationFailed;
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
    int error = DSA_verify(openSSLmac, inputHash, (unsigned int)inputHashSize, (unsigned char *)inputSignature, (unsigned int)inputSignatureSize, dsa);
    if (error < 0) return SI_EC_OpenSSLFailure; else if (error > 0) verified = OFTrue;
    return EC_Normal;
}


unsigned long SiDSA::getSize() const
{   
  if (dsa == NULL) return 0;
  return DSA_size(dsa);
}


E_KeyType SiDSA::keyType() const
{
  return EKT_DSA;
}

#else /* WITH_OPENSSL */

int sidsa_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
