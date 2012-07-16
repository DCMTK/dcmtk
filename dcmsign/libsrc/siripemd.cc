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
 *    classes: SiRIPEMD160
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/siripemd.h"
#include "dcmtk/dcmdata/dcerror.h"

#define INCLUDE_CSTDLIB
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#include <openssl/ripemd.h>
END_EXTERN_C

SiRIPEMD160::SiRIPEMD160()
: ctx(new RIPEMD160_CTX())
{
  initialize();
}

SiRIPEMD160::~SiRIPEMD160()
{
  delete ctx;
}

unsigned long SiRIPEMD160::getSize() const
{
  return RIPEMD160_DIGEST_LENGTH;
}

OFCondition SiRIPEMD160::initialize()
{
  RIPEMD160_Init(ctx);
  return EC_Normal;
}

OFCondition SiRIPEMD160::digest(const unsigned char *data, unsigned long length)
{
  if (length == 0) return EC_Normal;
  if ((data == NULL)||(ctx == NULL)) return EC_IllegalCall;
  RIPEMD160_Update(ctx, data, length);
  return EC_Normal;
}

OFCondition SiRIPEMD160::finalize(unsigned char *result)
{
  if ((result == NULL)||(ctx == NULL)) return EC_IllegalCall;
  RIPEMD160_Final(result, ctx);
  return EC_Normal;  
}

E_MACType SiRIPEMD160::macType() const
{
  return EMT_RIPEMD160;
}

const char *SiRIPEMD160::getDefinedTerm() const
{
  return SI_DEFTERMS_RIPEMD160;
}

#else /* WITH_OPENSSL */

int siripemd_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
