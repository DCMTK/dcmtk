/*
 *
 *  Copyright (C) 2015-2024, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmiod
 *
 *  Author: Michael Onken
 *
 *  Purpose: Helper functions for copying common modules between datasets
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/modhelp.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcitem.h"

// List of tags within the Patient Module
const DcmTagKey DcmModuleHelpers::patientModuleTags[] = { DCM_PatientName,
                                                          DCM_PatientID,
                                                          // Macro IssuerOfPatientIDMacro
                                                          DCM_IssuerOfPatientID,
                                                          DCM_IssuerOfPatientIDQualifiersSequence,
                                                          // EndMacro IssuerOfPatientIDMacro
                                                          DCM_PatientBirthDate,
                                                          DCM_PatientSex,
                                                          DCM_QualityControlSubject,
                                                          DCM_ReferencedPatientSequence,
                                                          DCM_PatientBirthTime,
                                                          DCM_RETIRED_OtherPatientIDs,
                                                          DCM_OtherPatientIDsSequence,
                                                          DCM_OtherPatientNames,
                                                          DCM_EthnicGroup,
                                                          DCM_PatientComments,
                                                          DCM_PatientSpeciesDescription,
                                                          DCM_PatientSpeciesCodeSequence,
                                                          DCM_PatientBreedDescription,
                                                          DCM_PatientBreedCodeSequence,
                                                          DCM_BreedRegistrationSequence,
                                                          DCM_ResponsiblePerson,
                                                          DCM_ResponsiblePersonRole,
                                                          DCM_ResponsibleOrganization,
                                                          DCM_PatientIdentityRemoved,
                                                          DCM_DeidentificationMethod,
                                                          DCM_DeidentificationMethodCodeSequence };

// List of tags within the Clinical Trial Subject Module
const DcmTagKey DcmModuleHelpers::clinicalTrialSubjectModuleTags[]
    = { DCM_ClinicalTrialSponsorName,
        DCM_ClinicalTrialProtocolID,
        DCM_ClinicalTrialProtocolName,
        DCM_ClinicalTrialSiteID,
        DCM_ClinicalTrialSiteName,
        DCM_ClinicalTrialSubjectID,
        DCM_ClinicalTrialSubjectReadingID,
        DCM_ClinicalTrialProtocolEthicsCommitteeName,
        DCM_ClinicalTrialProtocolEthicsCommitteeApprovalNumber };

// List of tags within the General Study Module
const DcmTagKey DcmModuleHelpers::generalStudyModuleTags[] = { DCM_StudyInstanceUID,
                                                               DCM_StudyDate,
                                                               DCM_StudyTime,
                                                               DCM_ReferringPhysicianName,
                                                               DCM_ReferringPhysicianIdentificationSequence,
                                                               DCM_StudyID,
                                                               DCM_AccessionNumber,
                                                               DCM_IssuerOfAccessionNumberSequence,
                                                               DCM_StudyDescription,
                                                               DCM_PhysiciansOfRecord,
                                                               DCM_PhysiciansOfRecordIdentificationSequence,
                                                               DCM_NameOfPhysiciansReadingStudy,
                                                               DCM_PhysiciansReadingStudyIdentificationSequence,
                                                               DCM_RequestingServiceCodeSequence,
                                                               DCM_ReferencedStudySequence,
                                                               DCM_ProcedureCodeSequence,
                                                               DCM_ReasonForPerformedProcedureCodeSequence };

// List of tags within the Patient Study Module
const DcmTagKey DcmModuleHelpers::patientStudyModuleTags[] = { DCM_AdmittingDiagnosesDescription,
                                                               DCM_AdmittingDiagnosesCodeSequence,
                                                               DCM_PatientAge,
                                                               DCM_PatientSize,
                                                               DCM_PatientWeight,
                                                               DCM_PatientSizeCodeSequence,
                                                               DCM_Occupation,
                                                               DCM_AdditionalPatientHistory,
                                                               DCM_AdmissionID,
                                                               DCM_IssuerOfAdmissionIDSequence,
                                                               DCM_ServiceEpisodeID,
                                                               DCM_IssuerOfServiceEpisodeIDSequence,
                                                               DCM_ServiceEpisodeDescription,
                                                               DCM_PatientSexNeutered };

// List of tags within the Clinical Trial Study Module
const DcmTagKey DcmModuleHelpers::clinicalTrialStudyModuleTags[]
    = { DCM_ClinicalTrialTimePointID, DCM_ClinicalTrialTimePointDescription, DCM_ConsentForClinicalTrialUseSequence };

// List of tags within the General Series Module
const DcmTagKey DcmModuleHelpers::generalSeriesModuleTags[] = { DCM_Modality,
                                                                DCM_SeriesInstanceUID,
                                                                DCM_SeriesNumber,
                                                                DCM_Laterality,
                                                                DCM_SeriesDate,
                                                                DCM_SeriesTime,
                                                                DCM_PerformingPhysicianName,
                                                                DCM_PerformingPhysicianIdentificationSequence,
                                                                DCM_ProtocolName,
                                                                DCM_SeriesDescription,
                                                                DCM_SeriesDescriptionCodeSequence,
                                                                DCM_OperatorsName,
                                                                DCM_OperatorIdentificationSequence,
                                                                DCM_ReferencedPerformedProcedureStepSequence,
                                                                DCM_RelatedSeriesSequence,
                                                                DCM_BodyPartExamined,
                                                                DCM_PatientPosition,
                                                                DCM_SmallestPixelValueInSeries,
                                                                DCM_LargestPixelValueInSeries,
                                                                DCM_RequestAttributesSequence,
                                                                // Macro PerformedProcedureStepSummaryMacro
                                                                DCM_PerformedProcedureStepID,
                                                                DCM_PerformedProcedureStepStartDate,
                                                                DCM_PerformedProcedureStepStartTime,
                                                                DCM_PerformedProcedureStepEndDate,
                                                                DCM_PerformedProcedureStepEndTime,
                                                                DCM_PerformedProcedureStepDescription,
                                                                DCM_PerformedProtocolCodeSequence,
                                                                DCM_CommentsOnThePerformedProcedureStep,
                                                                // EndMacro PerformedProcedureStepSummaryMacro
                                                                DCM_AnatomicalOrientationType };

// List of tags within the Clinical Trial Series Module
const DcmTagKey DcmModuleHelpers::clinicalTrialSeriesModuleTags[]
    = { DCM_ClinicalTrialCoordinatingCenterName, DCM_ClinicalTrialSeriesID, DCM_ClinicalTrialSeriesDescription };

// List of tags within the General Equipment Module
const DcmTagKey DcmModuleHelpers::generalEquipmentModuleTags[] = { DCM_Manufacturer,
                                                                   DCM_InstitutionName,
                                                                   DCM_InstitutionAddress,
                                                                   DCM_StationName,
                                                                   DCM_InstitutionalDepartmentName,
                                                                   DCM_ManufacturerModelName,
                                                                   DCM_DeviceSerialNumber,
                                                                   DCM_SoftwareVersions,
                                                                   DCM_GantryID,
                                                                   DCM_SpatialResolution,
                                                                   DCM_DateOfLastCalibration,
                                                                   DCM_TimeOfLastCalibration,
                                                                   DCM_PixelPaddingValue };

// List of tags within the Frame of Reference Module
const DcmTagKey DcmModuleHelpers::frameOfReferenceModuleTags[]
    = { DCM_FrameOfReferenceUID, DCM_PositionReferenceIndicator };

// List of tags within the SOP Common Module, excluding Digital Signatures Macro
const DcmTagKey DcmModuleHelpers::sopCommonModuleTags[] = { DCM_SOPClassUID,
                                                            DCM_SOPInstanceUID,
                                                            DCM_SpecificCharacterSet,
                                                            DCM_InstanceCreationDate,
                                                            DCM_InstanceCreationTime,
                                                            DCM_InstanceCoercionDateTime,
                                                            DCM_InstanceCreatorUID,
                                                            DCM_RelatedGeneralSOPClassUID,
                                                            DCM_OriginalSpecializedSOPClassUID,
                                                            DCM_CodingSchemeIdentificationSequence,
                                                            DCM_TimezoneOffsetFromUTC,
                                                            DCM_ContributingEquipmentSequence,
                                                            DCM_InstanceNumber,
                                                            DCM_SOPInstanceStatus,
                                                            DCM_SOPAuthorizationDateTime,
                                                            DCM_SOPAuthorizationComment,
                                                            DCM_AuthorizationEquipmentCertificationNumber,
                                                            // Macro DigitalSignaturesMacro
                                                            // DCM_MACParametersSequence,
                                                            // DCM_DigitalSignaturesSequence,
                                                            // EndMacro DigitalSignaturesMacro
                                                            DCM_EncryptedAttributesSequence,
                                                            DCM_OriginalAttributesSequence,
                                                            DCM_HL7StructuredDocumentReferenceSequence,
                                                            DCM_LongitudinalTemporalInformationModified,
                                                            DCM_QueryRetrieveView,
                                                            DCM_ConversionSourceAttributesSequence };

// List of tags within the General Image Module
const DcmTagKey DcmModuleHelpers::generalImageModuleTags[] = { DCM_InstanceNumber,
                                                               DCM_PatientOrientation,
                                                               DCM_ContentDate,
                                                               DCM_ContentTime,
                                                               DCM_ImageType,
                                                               DCM_AcquisitionNumber,
                                                               DCM_AcquisitionDate,
                                                               DCM_AcquisitionTime,
                                                               DCM_AcquisitionDateTime,
                                                               DCM_ReferencedImageSequence,
                                                               DCM_DerivationDescription,
                                                               DCM_DerivationCodeSequence,
                                                               DCM_SourceImageSequence,
                                                               DCM_ReferencedInstanceSequence,
                                                               DCM_ImagesInAcquisition,
                                                               DCM_ImageComments,
                                                               DCM_QualityControlImage,
                                                               DCM_BurnedInAnnotation,
                                                               DCM_RecognizableVisualFeatures,
                                                               DCM_LossyImageCompression,
                                                               DCM_LossyImageCompressionRatio,
                                                               DCM_LossyImageCompressionMethod,
                                                               DCM_IconImageSequence,
                                                               DCM_PresentationLUTShape,
                                                               DCM_IrradiationEventUID,
                                                               DCM_RealWorldValueMappingSequence };

void DcmModuleHelpers::copyElement(const DcmTagKey& tag, DcmItem& src, DcmItem& dest)
{
    DcmElement* delem;
    OFCondition cond;
    // get (deep) copy of element
    cond = src.findAndGetElement(tag, delem, OFFalse /*searchIntoSub*/, OFTrue /*createCopy*/);
    if (cond.good())
    {
        // and insert into destination item
        cond = dest.insert(delem, OFTrue);
        if (cond.bad())
        {
            // we do not expect any errors here, so report it
            DCMDATA_ERROR("Could not insert element with tag " << tag << " into item: " << cond.text());
        }
    }
}

template <size_t N>
inline void DcmModuleHelpers::copyModule(const DcmTagKey (&tags)[N], DcmItem& src, DcmItem& dest)
{
    for (const DcmTagKey *it = tags, *const end = tags + N; it != end; ++it)
        DcmModuleHelpers::copyElement(*it, src, dest);
}

void DcmModuleHelpers::copyPatientModule(DcmItem& src, DcmItem& dest)
{
    copyModule(patientModuleTags, src, dest);
}

void DcmModuleHelpers::copyClinicalTrialSubjectModule(DcmItem& src, DcmItem& dest)
{
    copyModule(clinicalTrialSubjectModuleTags, src, dest);
}

void DcmModuleHelpers::copyGeneralStudyModule(DcmItem& src, DcmItem& dest)
{
    copyModule(generalStudyModuleTags, src, dest);
}

void DcmModuleHelpers::copyPatientStudyModule(DcmItem& src, DcmItem& dest)
{
    copyModule(patientStudyModuleTags, src, dest);
}

void DcmModuleHelpers::copyClinicalTrialStudyModule(DcmItem& src, DcmItem& dest)
{
    copyModule(clinicalTrialStudyModuleTags, src, dest);
}

void DcmModuleHelpers::copyGeneralSeriesModule(DcmItem& src, DcmItem& dest)
{
    copyModule(generalSeriesModuleTags, src, dest);
}

void DcmModuleHelpers::copyClinicalTrialSeriesModule(DcmItem& src, DcmItem& dest)
{
    copyModule(clinicalTrialSeriesModuleTags, src, dest);
}

void DcmModuleHelpers::copyGeneralEquipmentModule(DcmItem& src, DcmItem& dest)
{
    copyModule(generalEquipmentModuleTags, src, dest);
}

void DcmModuleHelpers::copyFrameOfReferenceModule(DcmItem& src, DcmItem& dest)
{
    copyModule(frameOfReferenceModuleTags, src, dest);
}

void DcmModuleHelpers::copySOPCommonModule(DcmItem& src, DcmItem& dest)
{
    copyModule(sopCommonModuleTags, src, dest);
}

void DcmModuleHelpers::copyGeneralImageModule(DcmItem& src, DcmItem& dest)
{
    copyModule(generalImageModuleTags, src, dest);
}
