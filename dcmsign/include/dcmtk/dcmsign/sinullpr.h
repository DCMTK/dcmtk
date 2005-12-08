/*
 *
 *  Copyright (C) 1998-2005, OFFIS
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
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiNullProfile
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-12-08 16:04:40 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef SINULLPR_H
#define SINULLPR_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmsign/sisprof.h"   /* for SiSecurityProfile */

#ifdef WITH_OPENSSL

/** defines a "null" security profile that does not require or forbid any 
 *  MAC algorithm, signature algorithm, or attribute tag. This class can be
 *  used to clean up a proposed attribute list against a dataset.
 */
class SiNullProfile: public SiSecurityProfile
{
public:

  /// default constructor
  SiNullProfile() { }

  /// destructor
  virtual ~SiNullProfile() { }
  
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
 *  $Log: sinullpr.h,v $
 *  Revision 1.4  2005-12-08 16:04:40  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.3  2003/06/04 14:21:03  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.2  2001/06/01 15:50:49  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2000/11/07 16:48:56  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

