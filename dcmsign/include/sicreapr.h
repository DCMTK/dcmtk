/*
 *
 *  Copyright (C) 1998-2001, OFFIS
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
 *  Author: Norbert Loxen, Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiCreatorProfile
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-11-16 15:50:50 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef SICREAPR_H
#define SICREAPR_H

#include "osconfig.h"

#ifdef WITH_OPENSSL

#include "sibrsapr.h"   /* for SiBaseRSAProfile */

/** Creator RSA Digital Signature Profile
 */
class SiCreatorProfile: public SiBaseRSAProfile
{ 
public:

  /// default constructor
  SiCreatorProfile() { }

  /// destructor
  virtual ~SiCreatorProfile() { }
  
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
 *  $Log: sicreapr.h,v $
 *  Revision 1.3  2001-11-16 15:50:50  meichel
 *  Adapted digital signature code to final text of supplement 41.
 *
 *  Revision 1.2  2001/06/01 15:50:48  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2000/11/07 16:48:54  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

