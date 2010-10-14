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
 *    classes: SiAuthorizationProfile
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:24 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef SIAUTOPR_H
#define SIAUTOPR_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmsign/sibrsapr.h"   /* for SiBaseRSAProfile */

#ifdef WITH_OPENSSL

/** Authorization Digital Signature Profile
 */
class SiAuthorizationProfile: public SiBaseRSAProfile
{
public:

  /// default constructor
  SiAuthorizationProfile() { }

  /// destructor
  virtual ~SiAuthorizationProfile() { }

  /** checks whether an attribute with the given tag is required to be signed
   *  for the current security profile.
   *  @param key tag key to be checked
   *  @return true if required, false otherwise.
   */
  virtual OFBool attributeRequired(const DcmTagKey& key) const;

};

#endif
#endif

/*
 *  $Log: siautopr.h,v $
 *  Revision 1.6  2010-10-14 13:17:24  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.5  2005-12-08 16:04:31  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.4  2003/06/04 14:21:03  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.3  2001/11/16 15:50:49  meichel
 *  Adapted digital signature code to final text of supplement 41.
 *
 *  Revision 1.2  2001/06/01 15:50:47  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2000/11/07 16:48:52  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

