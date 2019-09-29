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

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sisrvpr.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmsign/sitypes.h"

OFBool SiStructuredReportingVerificationProfile::attributeRequired(const DcmTagKey& key) const
{

  // call base class implementation
  if (SiStructuredReportingProfile::attributeRequired(key)) return OFTrue;

  // check additional keys required for a verification signature
  if (key == DCM_SOPInstanceUID) return OFTrue;
  if (key == DCM_VerificationFlag) return OFTrue;
  if (key == DCM_VerifyingObserverSequence) return OFTrue;
  if (key == DCM_VerificationDateTime) return OFTrue;

  return OFFalse;
}

OFCondition SiStructuredReportingVerificationProfile::inspectSignatureDataset(DcmItem & item)
{
  DcmElement *delem;
  OFString s;

  // check if this is an SR document by looking up the most important
  // attributes we expect in any SR document
  if (item.findAndGetElement(DCM_ValueType, delem).good() &&
      item.findAndGetElement(DCM_ConceptNameCodeSequence, delem).good() &&
      item.findAndGetElement(DCM_ContentSequence, delem).good())
  {
    // fine, this is an SR document
    if (item.findAndGetOFString(DCM_VerificationFlag, s).bad() || (s != "VERIFIED"))
    {
      DCMSIGN_ERROR("SR document status is not 'VERIFIED', cannot apply SR RSA Digital Signature Profile verification signature");
      return SI_EC_DatasetDoesNotMatchProfile;
    }
    return EC_Normal;
  }
  else
  {
    // not an SR document
    DCMSIGN_ERROR("Dataset is not a Structured Report, cannot apply Structured Reporting RSA Digital Signature Profile");
    return SI_EC_DatasetDoesNotMatchProfile;
  }
}

SiSignaturePurpose::E_SignaturePurposeType SiStructuredReportingVerificationProfile::getOverrideSignaturePurpose() const
{
  return SiSignaturePurpose::ESP_VerificationSignature;
}

#else /* WITH_OPENSSL */

int sisrvpr_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
