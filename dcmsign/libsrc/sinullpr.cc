/*
 *
 *  Copyright (C) 1998-2000, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module: dcmsign
 *
 *  Author:Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiNullProfile
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-11-07 16:49:06 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"

#ifdef WITH_OPENSSL

#include "sinullpr.h"

OFBool SiNullProfile::isAllowableMACType(E_MACType /* macType */) const
{
  return OFTrue;
}

OFBool SiNullProfile::isAllowableAlgorithmType(E_KeyType keyType) const
{
  if (keyType == EKT_none) return OFFalse;
  return OFTrue;
}

OFBool SiNullProfile::attributeRequired(const DcmTagKey& /* key */) const
{
  return OFFalse;
}

OFBool SiNullProfile::attributeForbidden(const DcmTagKey& /* key */) const
{
  return OFFalse;
}

OFBool SiNullProfile::isAllowableTransferSyntax(E_TransferSyntax xfer) const
{
  if (xfer == EXS_LittleEndianExplicit) return OFTrue;
  return OFFalse;
}

#else /* WITH_OPENSSL */

const int sinullpr_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: sinullpr.cc,v $
 *  Revision 1.1  2000-11-07 16:49:06  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

