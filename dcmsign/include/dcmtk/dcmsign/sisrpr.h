/*
 *
 *  Copyright (C) 2019, OFFIS e.V.
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
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiStructuredReportingProfile
 *
 */

#ifndef SISRPR_H
#define SISRPR_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmsign/sibrsapr.h"   /* for SiBaseRSAProfile */

#ifdef WITH_OPENSSL

/** Structured Reporting RSA Digital Signature Profile
 *  @remark this class is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
class DCMTK_DCMSIGN_EXPORT SiStructuredReportingProfile: public SiBaseRSAProfile
{
public:

  /// default constructor
  SiStructuredReportingProfile() { }

  /// destructor
  virtual ~SiStructuredReportingProfile() { }

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

};

#endif
#endif
