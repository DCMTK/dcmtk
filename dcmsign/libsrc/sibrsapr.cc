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
 *    classes: SiBaseRSAProfile
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sibrsapr.h"
#include "dcmtk/dcmdata/dcdeftag.h"

OFBool SiBaseRSAProfile::isAllowableMACType(E_MACType macType) const
{
  OFBool result = OFFalse;
  switch (macType)
  {
    case EMT_RIPEMD160:
    case EMT_SHA1:
    case EMT_MD5:
      result = OFTrue;
      break;
    default:
      /* nothing */
      break;
  }
  return result;
}

OFBool SiBaseRSAProfile::isAllowableAlgorithmType(E_KeyType keyType) const
{
  OFBool result = OFFalse;
  switch (keyType)
  {
    case EKT_RSA:
      result = OFTrue;
      break;
    default:
      /* nothing */
      break;
  }
  return result;
}

OFBool SiBaseRSAProfile::attributeRequired(const DcmTagKey& /* key */) const
{
  return OFFalse;
}

OFBool SiBaseRSAProfile::attributeForbidden(const DcmTagKey& /* key */) const
{
  return OFFalse;
}

OFBool SiBaseRSAProfile::isAllowableTransferSyntax(E_TransferSyntax xfer) const
{
  if ((xfer == EXS_LittleEndianImplicit)||(xfer == EXS_BigEndianExplicit)) return OFFalse;
  return OFTrue;
}

#else /* WITH_OPENSSL */

int sibrsapr_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
