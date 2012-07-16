/*
 *
 *  Copyright (C) 1998-2011, OFFIS e.V.
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
 *    classes: SiCreatorProfile
 *
 */

#ifndef SICREAPR_H
#define SICREAPR_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmsign/sibrsapr.h"   /* for SiBaseRSAProfile */

#ifdef WITH_OPENSSL

/** Creator RSA Digital Signature Profile
 */
class DCMTK_DCMSIGN_EXPORT SiCreatorProfile: public SiBaseRSAProfile
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
