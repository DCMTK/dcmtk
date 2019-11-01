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

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sisrpr.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmsign/sitypes.h"

OFBool SiStructuredReportingProfile::attributeRequiredIfPresent(const DcmTagKey& key) const
{

  // the SOP Class UID
  if (key == DCM_SOPClassUID) return OFTrue;

  // the Study and Series Instance UIDs
  if (key == DCM_StudyInstanceUID) return OFTrue;
  if (key == DCM_SeriesInstanceUID) return OFTrue;

  // all attributes of the General Equipment Module that are present
  if (key == DCM_Manufacturer) return OFTrue;
  if (key == DCM_InstitutionName) return OFTrue;
  if (key == DCM_InstitutionAddress) return OFTrue;
  if (key == DCM_StationName) return OFTrue;
  if (key == DCM_InstitutionalDepartmentName) return OFTrue;
  if (key == DCM_InstitutionalDepartmentTypeCodeSequence) return OFTrue;
  if (key == DCM_ManufacturerModelName) return OFTrue;
  if (key == DCM_DeviceSerialNumber) return OFTrue;
  if (key == DCM_SoftwareVersions) return OFTrue;
  if (key == DCM_GantryID) return OFTrue;
  if (key == DCM_UDISequence) return OFTrue;
  if (key == DCM_DeviceUID) return OFTrue;
  if (key == DCM_SpatialResolution) return OFTrue;
  if (key == DCM_DateOfLastCalibration) return OFTrue;
  if (key == DCM_TimeOfLastCalibration) return OFTrue;
  if (key == DCM_PixelPaddingValue) return OFTrue;

  // the Current Requested Procedure Evidence Sequence
  if (key == DCM_CurrentRequestedProcedureEvidenceSequence) return OFTrue;

  // the Pertinent Other Evidence Sequence
  if (key == DCM_PertinentOtherEvidenceSequence) return OFTrue;

  // the Predecessor Documents Sequence
  if (key == DCM_PredecessorDocumentsSequence) return OFTrue;

  // the Observation DateTime
  if (key == DCM_ObservationDateTime) return OFTrue;

  // all attributes of the SR Document Content Module that are present
  if (key == DCM_ValueType) return OFTrue;
  if (key == DCM_ConceptNameCodeSequence) return OFTrue;
  if (key == DCM_ContinuityOfContent) return OFTrue;
  if (key == DCM_ContentTemplateSequence) return OFTrue;
  // if (key == DCM_ObservationDateTime) return OFTrue; // see above
  if (key == DCM_ObservationUID) return OFTrue;
  if (key == DCM_ContentSequence) return OFTrue;

  return OFFalse;
}


OFBool SiStructuredReportingProfile::checkRequiredAttributeList(DcmAttributeTag& tagList) const
{
  OFBool result =
    containsTag(tagList, DCM_SOPClassUID) &&
    containsTag(tagList, DCM_StudyInstanceUID) &&
    containsTag(tagList, DCM_SeriesInstanceUID);

  // The wording in DICOM part 15 seems to indicate that the following
  // attributes must also be signed unconditionally. However, since they
  // optional (type 1c or type 3) and thus not present in all
  // valid SR documents, we do not treat them as attributes that cause
  // signature validation according to this profile to fail if absent.
  // We include them in the list of attributes that must be signed if present:
  //
  // - DCM_CurrentRequestedProcedureEvidenceSequence
  // - DCM_PertinentOtherEvidenceSequence
  // - DCM_PredecessorDocumentsSequence
  // - DCM_ObservationDateTime

  return result;
}


static OFBool haveVerificationSignature(DcmItem& item)
{
  signed long itemNo = 0;
  DcmItem *ditem = NULL;
  DcmItem *ditem2 = NULL;
  OFString s;
  while (1)
  {
    if (item.findAndGetSequenceItem(DCM_DigitalSignaturesSequence, ditem, itemNo).good() && ditem)
    {
      if (ditem->findAndGetSequenceItem(DCM_DigitalSignaturePurposeCodeSequence, ditem2).good() && ditem2)
      {
        if (ditem2->findAndGetOFString(DCM_CodeValue, s).good())
        {
          if (s == "5") return OFTrue; // we have digital signature purpose code "5", which means that a verification signature is present
        }
      }
    }
    else return OFFalse; // no (further) item found. There is no verification signature

    // check next item
    ++itemNo;
  }
}


OFCondition SiStructuredReportingProfile::inspectSignatureDataset(DcmItem& item)
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
    if (item.findAndGetOFString(DCM_VerificationFlag, s).good() && (s == "VERIFIED"))
    {
      // check if we already have a verification signature in this dataset
      if (! haveVerificationSignature(item))
      {
        DCMSIGN_WARN("SR document status is 'VERIFIED', but the verification signature is missing");
      }
    }
    return EC_Normal;
  }
  else
  {
    // not an SR document
    return SI_EC_DatasetDoesNotMatchProfile;
  }
}

OFBool SiStructuredReportingProfile::mainDatasetRequired() const
{
  return OFTrue;
}

#else /* WITH_OPENSSL */

int sisrpr_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
