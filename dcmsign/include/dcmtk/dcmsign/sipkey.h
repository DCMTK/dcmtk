/*
 *
 *  Copyright (C) 1998-2020, OFFIS e.V.
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

#ifndef SIPKEY_H
#define SIPKEY_H

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sialgo.h"
#include "dcmtk/ofstd/oftypes.h"

class SiPrivateKey;
struct evp_pkey_st;
typedef struct evp_pkey_st EVP_PKEY;

/**
 *  This class implements all supported public key crypto algorithms
 *  using the OpenSSL EVP_PKEY high level API.
 *  @remark this class is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */

class DCMTK_DCMSIGN_EXPORT SiPKEY : public SiAlgorithm
{
public:

  /** constructor
   *  @param pointer to public or private key
   *  @param owned if true, ownership of the key is transferred to the SiPKEY instance
   */
  SiPKEY(EVP_PKEY *key, OFBool owned);

  /// destructor
  virtual ~SiPKEY();

  /** creates a signature.
   *  @param inputHash array of hash key bytes that are to be signed
   *  @param inputHashSize length of hash key array in bytes
   *  @param inputHashAlgorithm MAC algorithm used for creation of hash key.
   *    Required for creation of PKCS#1 RSA signature padding.
   *  @param outputSignature pointer to array of at least getSize() which must be allocated by caller.
   *  @param outputSignatureSize returns the number of bytes written to outputSignature.
   *  @return SI_EC_Normal if successful, errorcode otherwise.
   */
  virtual OFCondition sign(
    const unsigned char *inputHash,
    unsigned long inputHashSize,
    E_MACType inputHashAlgorithm,
    unsigned char *outputSignature,
    unsigned long &outputSignatureSize);

  /** verifies a signature.
   *  @param inputHash array of bytes containing hash key to be verified against signature
   *  @param inputHashSize length of hash key array in bytes
   *  @param inputHashAlgorithm MAC algorithm used for creation of hash key.
   *    Required for creation of PKCS#1 RSA signature padding.
   *  @param inputSignature array of bytes containing signature to be verified
   *  @param inputSignatureSize length of signature array in bytes
   *  @param verified returns whether the signature was successfully verified
   *  @return SI_EC_Normal if successful, errorcode otherwise.
   */
  virtual OFCondition verify(
    const unsigned char *inputHash,
    unsigned long inputHashSize,
    E_MACType inputHashAlgorithm,
    const unsigned char *inputSignature,
    unsigned long inputSignatureSize,
    OFBool &verified);

  /** returns the size of a block of encrypted/decrypted ciphertext in bytes.
   *  The result depends on the public key algorithm, key size and padding scheme.
   *  In general the input to decrypt() or encrypt() must be less than or equal
   *  to this block size.  The output of decrypt() or encrypt() is always equal
   *  to this block size.
   *  @return block size for this public key cryptosystem and key
   */
  virtual unsigned long getSize() const;

  /** returns the type of public key algorithm computed by this object
   *  @return type of public key algorithm
   */
  virtual E_KeyType keyType() const;

private:

  /// private undefined copy constructor
  SiPKEY(SiPKEY& arg);

  /// private undefined copy assignment operator
  SiPKEY& operator=(SiPKEY& arg);

  /// key used for signature/verification
  EVP_PKEY *signing_key;

  /// indicates if the signing key object is owned by this instance.
  OFBool owned_;
};

#endif
#endif
