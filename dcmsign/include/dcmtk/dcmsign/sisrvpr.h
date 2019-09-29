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
 *    classes: SiStructuredReportingVerificationProfile
 *
 */

#ifndef SISRVPR_H
#define SISRVPR_H

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sisrpr.h"   /* for SiStructuredReportingProfile */

/** Structured Reporting RSA Digital Signature Profile - Verification Signature
 *  @remark this class is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
class DCMTK_DCMSIGN_EXPORT SiStructuredReportingVerificationProfile: public SiStructuredReportingProfile
{
public:

  /// default constructor
  SiStructuredReportingVerificationProfile() { }

  /// destructor
  virtual ~SiStructuredReportingVerificationProfile() { }

  /** checks whether an attribute with the given tag is required to be signed
   *  for the current security profile.
   *  @param key tag key to be checked
   *  @return true if required, false otherwise.
   */
  virtual OFBool attributeRequired(const DcmTagKey& key) const;

  /** some digital signature profiles specify conditions under which certain
   *  attributes must be included into the signature.
   *  This method allows the signature profile to inspect the dataset in order
   *  to determine whether or not the conditions are met.
   *  This method should be called before DcmSignature::createSignature() is executed.
   *  @param item the dataset or item to which the signature will be added
   *  @return status code
   */
  virtual OFCondition inspectSignatureDataset(DcmItem &item);

  /** return the required digital signature purpose for this signature profile
   *  @return for this class, this method always returns ESP_VerificationSignature.
   */
  virtual SiSignaturePurpose::E_SignaturePurposeType getOverrideSignaturePurpose() const;

};

#endif
#endif
