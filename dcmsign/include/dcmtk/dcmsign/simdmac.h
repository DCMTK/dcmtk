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

#ifndef SIMDMAC_H
#define SIMDMAC_H

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/simac.h"
#include "dcmtk/dcmsign/sitypes.h"

BEGIN_EXTERN_C
#include <openssl/opensslv.h>
END_EXTERN_C

#ifndef HAVE_OPENSSL_DECLARATION_NEW_EVP_MD_CTX
struct env_md_ctx_st;
typedef struct env_md_ctx_st EVP_MD_CTX;
#else
struct evp_md_ctx_st;
typedef struct evp_md_ctx_st EVP_MD_CTX;
#endif

/**
 * a class implementing all supported hash functions using the OpenSSL EVP_MD high level API.
 * @remark this class is only available if DCMTK is compiled with
 * OpenSSL support enabled.
 */
class DCMTK_DCMSIGN_EXPORT SiMDMAC : public SiMAC
{
public:
  /// default constructor
  SiMDMAC(E_MACType mactype);

  /// destructor
  virtual ~SiMDMAC();

  /** initializes the MAC algorithm.
   *  @return status code
   */
  virtual OFCondition initialize();

  /** feeds data into the MAC algorithm
   *  @param data pointer to raw data to be fed into the MAC, must not be NULL
   *  @param length number of bytes in raw data array
   *  @return status code
   */
  virtual OFCondition digest(const unsigned char *data, unsigned long length);

  /** finalizes the MAC and writes it to the given output array,
   *  which must be at least getSize() bytes large.
   *  After a call to finalize, the MAC algorithm must be initialized
   *  again, see initialize().
   *  @param result pointer to array of getSize() bytes into which the MAC is written
   *  @return status code
   */
  virtual OFCondition finalize(unsigned char *result);

  /** returns the size of a MAC in bytes.
   *  @return block size for this MAC algorithm
   */
  virtual unsigned long getSize() const;

  /** returns the type of MAC algorithm computed by this object
   *  @return type of MAC algorithm
   */
  virtual E_MACType macType() const;

  /** returns the DICOM identifier for this MAC algorithm
   *  @return DICOM defined term for algorithm
   */
  virtual const char *getDefinedTerm() const;

private:

  /// private undefined copy constructor
  SiMDMAC(SiMDMAC& arg);

  /// private undefined copy assignment operator
  SiMDMAC& operator=(SiMDMAC& arg);

  /// OpenSSL MAC context
  EVP_MD_CTX *ctx;

  /// MAC type
  E_MACType macType_;

};

#endif
#endif
