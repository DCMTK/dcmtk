/*
 *
 *  Copyright (C) 2016, OFFIS e.V.
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
 *    classes: SiSHA256
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sisha256.h"
#include "dcmtk/dcmdata/dcerror.h"

#define INCLUDE_CSTDLIB
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#include <openssl/sha.h>
END_EXTERN_C

 
SiSHA256::SiSHA256()
: ctx(new SHA256_CTX())
{
  initialize();
}

SiSHA256::~SiSHA256()
{
  delete ctx;
}

unsigned long SiSHA256::getSize() const
{
  return SHA256_DIGEST_LENGTH;
}

OFCondition SiSHA256::initialize()
{
  SHA256_Init(ctx);
  return EC_Normal;
}

OFCondition SiSHA256::digest(const unsigned char *data, unsigned long length)
{
  if (length == 0) return EC_Normal;
  if ((data == NULL)||(ctx == NULL)) return EC_IllegalCall;
  SHA256_Update(ctx, data, length);
  return EC_Normal;
}

OFCondition SiSHA256::finalize(unsigned char *result)
{
  if ((result == NULL)||(ctx == NULL)) return EC_IllegalCall;
  SHA256_Final(result, ctx);
  return EC_Normal;  
}

E_MACType SiSHA256::macType() const
{
  return EMT_SHA256;
}

const char *SiSHA256::getDefinedTerm() const
{
  return SI_DEFTERMS_SHA256;
}

#else /* WITH_OPENSSL */

int sisha256_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
