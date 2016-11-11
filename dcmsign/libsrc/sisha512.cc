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
 *    classes: SiSHA512
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sisha512.h"
#include "dcmtk/dcmdata/dcerror.h"

#define INCLUDE_CSTDLIB
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#include <openssl/sha.h>
END_EXTERN_C

 
SiSHA512::SiSHA512()
: ctx(new SHA512_CTX())
{
  initialize();
}

SiSHA512::~SiSHA512()
{
  delete ctx;
}

unsigned long SiSHA512::getSize() const
{
  return SHA512_DIGEST_LENGTH;
}

OFCondition SiSHA512::initialize()
{
  SHA512_Init(ctx);
  return EC_Normal;
}

OFCondition SiSHA512::digest(const unsigned char *data, unsigned long length)
{
  if (length == 0) return EC_Normal;
  if ((data == NULL)||(ctx == NULL)) return EC_IllegalCall;
  SHA512_Update(ctx, data, length);
  return EC_Normal;
}

OFCondition SiSHA512::finalize(unsigned char *result)
{
  if ((result == NULL)||(ctx == NULL)) return EC_IllegalCall;
  SHA512_Final(result, ctx);
  return EC_Normal;  
}

E_MACType SiSHA512::macType() const
{
  return EMT_SHA512;
}

const char *SiSHA512::getDefinedTerm() const
{
  return SI_DEFTERMS_SHA512;
}

#else /* WITH_OPENSSL */

int sisha512_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
