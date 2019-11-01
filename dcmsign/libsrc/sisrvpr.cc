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

OFBool SiStructuredReportingVerificationProfile::attributeRequiredIfPresent(const DcmTagKey& key) const
{

  // call base class implementation
  if (SiStructuredReportingProfile::attributeRequiredIfPresent(key)) return OFTrue;

  // check additional keys required for a verification signature
  if (key == DCM_SOPInstanceUID) return OFTrue;
  if (key == DCM_VerificationFlag) return OFTrue; // Type 1 in SR Document General Module
  if (key == DCM_VerifyingObserverSequence) return OFTrue; // Type 1C, must be present if VerificationFlag == VERIFIED
  if (key == DCM_VerificationDateTime) return OFTrue;

  return OFFalse;
}


OFBool SiStructuredReportingVerificationProfile::checkRequiredAttributeList(DcmAttributeTag& tagList) const
{
  OFBool result = SiStructuredReportingProfile::checkRequiredAttributeList(tagList);

  result = result &&
    containsTag(tagList, DCM_SOPInstanceUID) &&
    containsTag(tagList, DCM_VerificationFlag) &&
    containsTag(tagList, DCM_VerifyingObserverSequence);

  // The wording in DICOM part 15 seems to indicate that ObservationDateTime
  // must also be signed unconditionally. However, ObservationDateTime is actually
  // an attribute within the VerifyingObserverSequence and will thus not be listed
  // explicitly in tagList.

  return result;
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
      // SR document status is not 'VERIFIED', cannot apply SR RSA Digital Signature Profile verification signature
      return SI_EC_DatasetDoesNotMatchProfile;
    }
    return EC_Normal;
  }
  else
  {
    // not an SR document
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
