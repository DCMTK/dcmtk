/*
 *
 *  Copyright (C) 1998-2019, OFFIS e.V.
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

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sibrsapr.h"   /* for SiBaseRSAProfile */

/** Creator RSA Digital Signature Profile
 *  @remark this class is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
class DCMTK_DCMSIGN_EXPORT SiCreatorProfile: public SiBaseRSAProfile
{
public:

  /// default constructor
  SiCreatorProfile();

  /// destructor
  virtual ~SiCreatorProfile() { }

  /** checks whether an attribute with the given tag is required to be signed
   *  for the current security profile if the attribute is present in the dataset
   *  @param key tag key to be checked
   *  @return true if required, false otherwise.
   */
  virtual OFBool attributeRequiredIfPresent(const DcmTagKey& key) const;

  /** checks whether all attributes that are required unconditionally
   *  to be signed in this profile are included in the given tagList.
   *  @param taglist attribute tag list
   *  @return true if requirements for profile are fulfilled, false otherwise.
   */
  virtual OFBool checkRequiredAttributeList(DcmAttributeTag& tagList) const;

  /** some digital signature profiles specify conditions under which certain
   *  attributes must be included into the signature.
   *  This method allows the signature profile to inspect the dataset in order
   *  to determine whether or not the conditions are met.
   *  This method should be called before DcmSignature::createSignature() is executed.
   *  @param item the dataset or item to which the signature will be added
   *  @return status code
   */
  virtual OFCondition inspectSignatureDataset(DcmItem &item);

  /** returns true if this signature profile only applies to main dataset level
   *  @return OFTrue if this signature profile only applies to main dataset level, OFFalse otherwise
   */
  virtual OFBool mainDatasetRequired() const;

private:

  /// flag indicating whether or not the signature dataset contains the raw data module
  OFBool containsRawData_;
};

#endif
#endif
