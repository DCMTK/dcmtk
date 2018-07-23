/*
 *
 *  Copyright (C) 2016-2018, OFFIS e.V.
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
 *    classes: SiSHA384
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sisha384.h"
#include "dcmtk/dcmdata/dcerror.h"

#define INCLUDE_CSTDLIB
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#include <openssl/sha.h>
END_EXTERN_C


SiSHA384::SiSHA384()
: ctx(new SHA512_CTX())
{
  initialize();
}

SiSHA384::~SiSHA384()
{
  delete ctx;
}

unsigned long SiSHA384::getSize() const
{
  return SHA384_DIGEST_LENGTH;
}

OFCondition SiSHA384::initialize()
{
  SHA384_Init(ctx);
  return EC_Normal;
}

OFCondition SiSHA384::digest(const unsigned char *data, unsigned long length)
{
  if (length == 0) return EC_Normal;
  if ((data == NULL)||(ctx == NULL)) return EC_IllegalCall;
  SHA384_Update(ctx, data, length);
  return EC_Normal;
}

OFCondition SiSHA384::finalize(unsigned char *result)
{
  if ((result == NULL)||(ctx == NULL)) return EC_IllegalCall;
  SHA384_Final(result, ctx);
  return EC_Normal;
}

E_MACType SiSHA384::macType() const
{
  return EMT_SHA384;
}

const char *SiSHA384::getDefinedTerm() const
{
  return SI_DEFTERMS_SHA384;
}

#else /* WITH_OPENSSL */

int sisha384_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
