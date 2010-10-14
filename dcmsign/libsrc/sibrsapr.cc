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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:38 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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

/*
 *  $Log: sibrsapr.cc,v $
 *  Revision 1.4  2010-10-14 13:14:38  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.3  2005-12-08 15:47:19  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.2  2002/12/16 12:57:49  meichel
 *  Minor modification to shut up linker on MacOS X when compiling
 *    without OpenSSL support
 *
 *  Revision 1.1  2001/11/16 15:50:53  meichel
 *  Adapted digital signature code to final text of supplement 41.
 *
 *  Revision 1.3  2001/06/01 15:50:52  meichel
 *  Updated copyright header
 *
 *  Revision 1.2  2000/11/07 18:07:08  joergr
 *  Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 *  Revision 1.1  2000/11/07 16:49:03  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

