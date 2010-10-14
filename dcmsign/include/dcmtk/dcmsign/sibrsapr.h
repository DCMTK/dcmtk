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
 *  Update Date:      $Date: 2010-10-14 13:17:24 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef SIBRSAPR_H
#define SIBRSAPR_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmsign/sisprof.h"   /* for SiSecurityProfile */

#ifdef WITH_OPENSSL

/** Base RSA Digital Signature Profile
 */
class SiBaseRSAProfile: public SiSecurityProfile
{
public:

  /// default constructor
  SiBaseRSAProfile() { }

  /// destructor
  virtual ~SiBaseRSAProfile() { }
  
  /** checks whether the given MAC type can be used with this security profile.
   *  @param macType MAC type to be checked
   *  @return true if MAC type is allowable for this profile, false otherwise.
   */
  virtual OFBool isAllowableMACType(E_MACType macType) const;

  /** checks whether the given public/private key algorithm can be used with this security profile.
   *  @param keyType public key algorithm type to be checked
   *  @return true if public key algorithm is allowable for this profile, false otherwise.
   */
  virtual OFBool isAllowableAlgorithmType(E_KeyType keyType) const;

  /** checks whether the given transfer syntax can be used with this security profile
   *  @param xfer transfer syntax to be checked
   *  @return true if transfer syntax is allowable for this profile, false otherwise.
   */
  virtual OFBool isAllowableTransferSyntax(E_TransferSyntax xfer) const;

  /** checks whether an attribute with the given tag is required to be signed
   *  for the current security profile.
   *  @param key tag key to be checked
   *  @return true if required, false otherwise.
   */
  virtual OFBool attributeRequired(const DcmTagKey& key) const;

  /** checks whether an attribute with the given tag must not be signed
   *  for the current security profile.
   *  @param key tag key to be checked
   *  @return true if attribute must not be signed, false otherwise.
   */
  virtual OFBool attributeForbidden(const DcmTagKey& key) const;

};

#endif
#endif

/*
 *  $Log: sibrsapr.h,v $
 *  Revision 1.4  2010-10-14 13:17:24  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.3  2005-12-08 16:04:32  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.2  2003/06/04 14:21:03  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.1  2001/11/16 15:50:49  meichel
 *  Adapted digital signature code to final text of supplement 41.
 *
 *
 */

