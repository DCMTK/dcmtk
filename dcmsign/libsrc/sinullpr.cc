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
 *  Author:Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiNullProfile
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:38 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sinullpr.h"

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
  if ((xfer == EXS_LittleEndianImplicit)||(xfer == EXS_BigEndianExplicit)) return OFFalse;
  return OFTrue;
}

#else /* WITH_OPENSSL */

int sinullpr_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: sinullpr.cc,v $
 *  Revision 1.6  2010-10-14 13:14:38  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.5  2005-12-08 15:47:26  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.4  2002/12/16 12:57:52  meichel
 *  Minor modification to shut up linker on MacOS X when compiling
 *    without OpenSSL support
 *
 *  Revision 1.3  2001/11/16 15:50:54  meichel
 *  Adapted digital signature code to final text of supplement 41.
 *
 *  Revision 1.2  2001/06/01 15:50:54  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2000/11/07 16:49:06  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

