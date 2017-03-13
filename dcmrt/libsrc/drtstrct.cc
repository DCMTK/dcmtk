/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTStructureSetIOD
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"      // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/drtstrct.h"


DRTStructureSetIOD::DRTStructureSetIOD()
  : PatientName(DCM_PatientName),
    PatientID(DCM_PatientID),
    IssuerOfPatientID(DCM_IssuerOfPatientID),
    IssuerOfPatientIDQualifiersSequence(),
    PatientBirthDate(DCM_PatientBirthDate),
    PatientBirthDateInAlternativeCalendar(DCM_PatientBirthDateInAlternativeCalendar),
    PatientDeathDateInAlternativeCalendar(DCM_PatientDeathDateInAlternativeCalendar),
    PatientAlternativeCalendar(DCM_PatientAlternativeCalendar),
    PatientSex(DCM_PatientSex),
    ReferencedPatientPhotoSequence(),
    QualityControlSubject(DCM_QualityControlSubject),
    ReferencedPatientSequence(),
    PatientBirthTime(DCM_PatientBirthTime),
    OtherPatientIDs(DCM_OtherPatientIDs),
    OtherPatientIDsSequence(),
    OtherPatientNames(DCM_OtherPatientNames),
    EthnicGroup(DCM_EthnicGroup),
    PatientComments(DCM_PatientComments),
    PatientSpeciesDescription(DCM_PatientSpeciesDescription),
    PatientSpeciesCodeSequence(),
    PatientBreedDescription(DCM_PatientBreedDescription),
    PatientBreedCodeSequence(),
    BreedRegistrationSequence(),
    StrainDescription(DCM_StrainDescription),
    StrainNomenclature(DCM_StrainNomenclature),
    StrainCodeSequence(),
    StrainAdditionalInformation(DCM_StrainAdditionalInformation),
    StrainStockSequence(),
    GeneticModificationsSequence(),
    ResponsiblePerson(DCM_ResponsiblePerson),
    ResponsiblePersonRole(DCM_ResponsiblePersonRole),
    ResponsibleOrganization(DCM_ResponsibleOrganization),
    PatientIdentityRemoved(DCM_PatientIdentityRemoved),
    DeidentificationMethod(DCM_DeidentificationMethod),
    DeidentificationMethodCodeSequence(),
    SourcePatientGroupIdentificationSequence(),
    GroupOfPatientsIdentificationSequence(),
    ClinicalTrialSponsorName(DCM_ClinicalTrialSponsorName),
    ClinicalTrialProtocolID(DCM_ClinicalTrialProtocolID),
    ClinicalTrialProtocolName(DCM_ClinicalTrialProtocolName),
    ClinicalTrialSiteID(DCM_ClinicalTrialSiteID),
    ClinicalTrialSiteName(DCM_ClinicalTrialSiteName),
    ClinicalTrialSubjectID(DCM_ClinicalTrialSubjectID),
    ClinicalTrialSubjectReadingID(DCM_ClinicalTrialSubjectReadingID),
    ClinicalTrialProtocolEthicsCommitteeName(DCM_ClinicalTrialProtocolEthicsCommitteeName),
    ClinicalTrialProtocolEthicsCommitteeApprovalNumber(DCM_ClinicalTrialProtocolEthicsCommitteeApprovalNumber),
    StudyInstanceUID(DCM_StudyInstanceUID),
    StudyDate(DCM_StudyDate),
    StudyTime(DCM_StudyTime),
    ReferringPhysicianName(DCM_ReferringPhysicianName),
    ReferringPhysicianIdentificationSequence(),
    ConsultingPhysicianName(DCM_ConsultingPhysicianName),
    ConsultingPhysicianIdentificationSequence(),
    StudyID(DCM_StudyID),
    AccessionNumber(DCM_AccessionNumber),
    IssuerOfAccessionNumberSequence(),
    StudyDescription(DCM_StudyDescription),
    PhysiciansOfRecord(DCM_PhysiciansOfRecord),
    PhysiciansOfRecordIdentificationSequence(),
    NameOfPhysiciansReadingStudy(DCM_NameOfPhysiciansReadingStudy),
    PhysiciansReadingStudyIdentificationSequence(),
    RequestingServiceCodeSequence(),
    ReferencedStudySequence(),
    ProcedureCodeSequence(),
    ReasonForPerformedProcedureCodeSequence(),
    AdmittingDiagnosesDescription(DCM_AdmittingDiagnosesDescription),
    AdmittingDiagnosesCodeSequence(),
    PatientAge(DCM_PatientAge),
    PatientSize(DCM_PatientSize),
    PatientWeight(DCM_PatientWeight),
    PatientBodyMassIndex(DCM_PatientBodyMassIndex),
    MeasuredAPDimension(DCM_MeasuredAPDimension),
    MeasuredLateralDimension(DCM_MeasuredLateralDimension),
    PatientSizeCodeSequence(),
    MedicalAlerts(DCM_MedicalAlerts),
    Allergies(DCM_Allergies),
    SmokingStatus(DCM_SmokingStatus),
    PregnancyStatus(DCM_PregnancyStatus),
    LastMenstrualDate(DCM_LastMenstrualDate),
    PatientState(DCM_PatientState),
    Occupation(DCM_Occupation),
    AdditionalPatientHistory(DCM_AdditionalPatientHistory),
    AdmissionID(DCM_AdmissionID),
    IssuerOfAdmissionIDSequence(),
    ServiceEpisodeID(DCM_ServiceEpisodeID),
    IssuerOfServiceEpisodeIDSequence(),
    ServiceEpisodeDescription(DCM_ServiceEpisodeDescription),
    PatientSexNeutered(DCM_PatientSexNeutered),
    ClinicalTrialTimePointID(DCM_ClinicalTrialTimePointID),
    ClinicalTrialTimePointDescription(DCM_ClinicalTrialTimePointDescription),
    ConsentForClinicalTrialUseSequence(),
    Modality(DCM_Modality),
    SeriesInstanceUID(DCM_SeriesInstanceUID),
    SeriesNumber(DCM_SeriesNumber),
    SeriesDate(DCM_SeriesDate),
    SeriesTime(DCM_SeriesTime),
    SeriesDescription(DCM_SeriesDescription),
    SeriesDescriptionCodeSequence(),
    OperatorsName(DCM_OperatorsName),
    OperatorIdentificationSequence(),
    ReferencedPerformedProcedureStepSequence(),
    RequestAttributesSequence(),
    PerformedProcedureStepID(DCM_PerformedProcedureStepID),
    PerformedProcedureStepStartDate(DCM_PerformedProcedureStepStartDate),
    PerformedProcedureStepStartTime(DCM_PerformedProcedureStepStartTime),
    PerformedProcedureStepEndDate(DCM_PerformedProcedureStepEndDate),
    PerformedProcedureStepEndTime(DCM_PerformedProcedureStepEndTime),
    PerformedProcedureStepDescription(DCM_PerformedProcedureStepDescription),
    PerformedProtocolCodeSequence(),
    CommentsOnThePerformedProcedureStep(DCM_CommentsOnThePerformedProcedureStep),
    ClinicalTrialCoordinatingCenterName(DCM_ClinicalTrialCoordinatingCenterName),
    ClinicalTrialSeriesID(DCM_ClinicalTrialSeriesID),
    ClinicalTrialSeriesDescription(DCM_ClinicalTrialSeriesDescription),
    Manufacturer(DCM_Manufacturer),
    InstitutionName(DCM_InstitutionName),
    InstitutionAddress(DCM_InstitutionAddress),
    StationName(DCM_StationName),
    InstitutionalDepartmentName(DCM_InstitutionalDepartmentName),
    ManufacturerModelName(DCM_ManufacturerModelName),
    DeviceSerialNumber(DCM_DeviceSerialNumber),
    SoftwareVersions(DCM_SoftwareVersions),
    GantryID(DCM_GantryID),
    UDISequence(),
    SpatialResolution(DCM_SpatialResolution),
    DateOfLastCalibration(DCM_DateOfLastCalibration),
    TimeOfLastCalibration(DCM_TimeOfLastCalibration),
    PixelPaddingValue(DCM_PixelPaddingValue),
    FrameOfReferenceUID(DCM_FrameOfReferenceUID),
    PositionReferenceIndicator(DCM_PositionReferenceIndicator),
    StructureSetLabel(DCM_StructureSetLabel),
    StructureSetName(DCM_StructureSetName),
    StructureSetDescription(DCM_StructureSetDescription),
    InstanceNumber(DCM_InstanceNumber),
    StructureSetDate(DCM_StructureSetDate),
    StructureSetTime(DCM_StructureSetTime),
    ReferencedFrameOfReferenceSequence(),
    StructureSetROISequence(),
    PredecessorStructureSetSequence(),
    ROIContourSequence(),
    RTROIObservationsSequence(),
    ApprovalStatus(DCM_ApprovalStatus),
    ReviewDate(DCM_ReviewDate),
    ReviewTime(DCM_ReviewTime),
    ReviewerName(DCM_ReviewerName),
    ReferencedImageSequence(),
    ReferencedInstanceSequence(),
    DerivationDescription(DCM_DerivationDescription),
    DerivationCodeSequence(),
    SourceImageSequence(),
    SourceInstanceSequence(),
    SOPClassUID(DCM_SOPClassUID),
    SOPInstanceUID(DCM_SOPInstanceUID),
    SpecificCharacterSet(DCM_SpecificCharacterSet),
    InstanceCreationDate(DCM_InstanceCreationDate),
    InstanceCreationTime(DCM_InstanceCreationTime),
    InstanceCoercionDateTime(DCM_InstanceCoercionDateTime),
    InstanceCreatorUID(DCM_InstanceCreatorUID),
    RelatedGeneralSOPClassUID(DCM_RelatedGeneralSOPClassUID),
    OriginalSpecializedSOPClassUID(DCM_OriginalSpecializedSOPClassUID),
    CodingSchemeIdentificationSequence(),
    ContextGroupIdentificationSequence(),
    MappingResourceIdentificationSequence(),
    TimezoneOffsetFromUTC(DCM_TimezoneOffsetFromUTC),
    ContributingEquipmentSequence(),
    SOPInstanceStatus(DCM_SOPInstanceStatus),
    SOPAuthorizationDateTime(DCM_SOPAuthorizationDateTime),
    SOPAuthorizationComment(DCM_SOPAuthorizationComment),
    AuthorizationEquipmentCertificationNumber(DCM_AuthorizationEquipmentCertificationNumber),
    MACParametersSequence(),
    DigitalSignaturesSequence(),
    EncryptedAttributesSequence(),
    OriginalAttributesSequence(),
    HL7StructuredDocumentReferenceSequence(),
    LongitudinalTemporalInformationModified(DCM_LongitudinalTemporalInformationModified),
    QueryRetrieveView(DCM_QueryRetrieveView),
    ConversionSourceAttributesSequence(),
    ContentQualification(DCM_ContentQualification),
    PrivateDataElementCharacteristicsSequence(),
    ReferencedSeriesSequence(),
    StudiesContainingOtherReferencedInstancesSequence()
{
    /* set initial values for a new SOP instance */
    updateAttributes();
}


DRTStructureSetIOD::DRTStructureSetIOD(const DRTStructureSetIOD &copy)
  : PatientName(copy.PatientName),
    PatientID(copy.PatientID),
    IssuerOfPatientID(copy.IssuerOfPatientID),
    IssuerOfPatientIDQualifiersSequence(copy.IssuerOfPatientIDQualifiersSequence),
    PatientBirthDate(copy.PatientBirthDate),
    PatientBirthDateInAlternativeCalendar(copy.PatientBirthDateInAlternativeCalendar),
    PatientDeathDateInAlternativeCalendar(copy.PatientDeathDateInAlternativeCalendar),
    PatientAlternativeCalendar(copy.PatientAlternativeCalendar),
    PatientSex(copy.PatientSex),
    ReferencedPatientPhotoSequence(copy.ReferencedPatientPhotoSequence),
    QualityControlSubject(copy.QualityControlSubject),
    ReferencedPatientSequence(copy.ReferencedPatientSequence),
    PatientBirthTime(copy.PatientBirthTime),
    OtherPatientIDs(copy.OtherPatientIDs),
    OtherPatientIDsSequence(copy.OtherPatientIDsSequence),
    OtherPatientNames(copy.OtherPatientNames),
    EthnicGroup(copy.EthnicGroup),
    PatientComments(copy.PatientComments),
    PatientSpeciesDescription(copy.PatientSpeciesDescription),
    PatientSpeciesCodeSequence(copy.PatientSpeciesCodeSequence),
    PatientBreedDescription(copy.PatientBreedDescription),
    PatientBreedCodeSequence(copy.PatientBreedCodeSequence),
    BreedRegistrationSequence(copy.BreedRegistrationSequence),
    StrainDescription(copy.StrainDescription),
    StrainNomenclature(copy.StrainNomenclature),
    StrainCodeSequence(copy.StrainCodeSequence),
    StrainAdditionalInformation(copy.StrainAdditionalInformation),
    StrainStockSequence(copy.StrainStockSequence),
    GeneticModificationsSequence(copy.GeneticModificationsSequence),
    ResponsiblePerson(copy.ResponsiblePerson),
    ResponsiblePersonRole(copy.ResponsiblePersonRole),
    ResponsibleOrganization(copy.ResponsibleOrganization),
    PatientIdentityRemoved(copy.PatientIdentityRemoved),
    DeidentificationMethod(copy.DeidentificationMethod),
    DeidentificationMethodCodeSequence(copy.DeidentificationMethodCodeSequence),
    SourcePatientGroupIdentificationSequence(copy.SourcePatientGroupIdentificationSequence),
    GroupOfPatientsIdentificationSequence(copy.GroupOfPatientsIdentificationSequence),
    ClinicalTrialSponsorName(copy.ClinicalTrialSponsorName),
    ClinicalTrialProtocolID(copy.ClinicalTrialProtocolID),
    ClinicalTrialProtocolName(copy.ClinicalTrialProtocolName),
    ClinicalTrialSiteID(copy.ClinicalTrialSiteID),
    ClinicalTrialSiteName(copy.ClinicalTrialSiteName),
    ClinicalTrialSubjectID(copy.ClinicalTrialSubjectID),
    ClinicalTrialSubjectReadingID(copy.ClinicalTrialSubjectReadingID),
    ClinicalTrialProtocolEthicsCommitteeName(copy.ClinicalTrialProtocolEthicsCommitteeName),
    ClinicalTrialProtocolEthicsCommitteeApprovalNumber(copy.ClinicalTrialProtocolEthicsCommitteeApprovalNumber),
    StudyInstanceUID(copy.StudyInstanceUID),
    StudyDate(copy.StudyDate),
    StudyTime(copy.StudyTime),
    ReferringPhysicianName(copy.ReferringPhysicianName),
    ReferringPhysicianIdentificationSequence(copy.ReferringPhysicianIdentificationSequence),
    ConsultingPhysicianName(copy.ConsultingPhysicianName),
    ConsultingPhysicianIdentificationSequence(copy.ConsultingPhysicianIdentificationSequence),
    StudyID(copy.StudyID),
    AccessionNumber(copy.AccessionNumber),
    IssuerOfAccessionNumberSequence(copy.IssuerOfAccessionNumberSequence),
    StudyDescription(copy.StudyDescription),
    PhysiciansOfRecord(copy.PhysiciansOfRecord),
    PhysiciansOfRecordIdentificationSequence(copy.PhysiciansOfRecordIdentificationSequence),
    NameOfPhysiciansReadingStudy(copy.NameOfPhysiciansReadingStudy),
    PhysiciansReadingStudyIdentificationSequence(copy.PhysiciansReadingStudyIdentificationSequence),
    RequestingServiceCodeSequence(copy.RequestingServiceCodeSequence),
    ReferencedStudySequence(copy.ReferencedStudySequence),
    ProcedureCodeSequence(copy.ProcedureCodeSequence),
    ReasonForPerformedProcedureCodeSequence(copy.ReasonForPerformedProcedureCodeSequence),
    AdmittingDiagnosesDescription(copy.AdmittingDiagnosesDescription),
    AdmittingDiagnosesCodeSequence(copy.AdmittingDiagnosesCodeSequence),
    PatientAge(copy.PatientAge),
    PatientSize(copy.PatientSize),
    PatientWeight(copy.PatientWeight),
    PatientBodyMassIndex(copy.PatientBodyMassIndex),
    MeasuredAPDimension(copy.MeasuredAPDimension),
    MeasuredLateralDimension(copy.MeasuredLateralDimension),
    PatientSizeCodeSequence(copy.PatientSizeCodeSequence),
    MedicalAlerts(copy.MedicalAlerts),
    Allergies(copy.Allergies),
    SmokingStatus(copy.SmokingStatus),
    PregnancyStatus(copy.PregnancyStatus),
    LastMenstrualDate(copy.LastMenstrualDate),
    PatientState(copy.PatientState),
    Occupation(copy.Occupation),
    AdditionalPatientHistory(copy.AdditionalPatientHistory),
    AdmissionID(copy.AdmissionID),
    IssuerOfAdmissionIDSequence(copy.IssuerOfAdmissionIDSequence),
    ServiceEpisodeID(copy.ServiceEpisodeID),
    IssuerOfServiceEpisodeIDSequence(copy.IssuerOfServiceEpisodeIDSequence),
    ServiceEpisodeDescription(copy.ServiceEpisodeDescription),
    PatientSexNeutered(copy.PatientSexNeutered),
    ClinicalTrialTimePointID(copy.ClinicalTrialTimePointID),
    ClinicalTrialTimePointDescription(copy.ClinicalTrialTimePointDescription),
    ConsentForClinicalTrialUseSequence(copy.ConsentForClinicalTrialUseSequence),
    Modality(copy.Modality),
    SeriesInstanceUID(copy.SeriesInstanceUID),
    SeriesNumber(copy.SeriesNumber),
    SeriesDate(copy.SeriesDate),
    SeriesTime(copy.SeriesTime),
    SeriesDescription(copy.SeriesDescription),
    SeriesDescriptionCodeSequence(copy.SeriesDescriptionCodeSequence),
    OperatorsName(copy.OperatorsName),
    OperatorIdentificationSequence(copy.OperatorIdentificationSequence),
    ReferencedPerformedProcedureStepSequence(copy.ReferencedPerformedProcedureStepSequence),
    RequestAttributesSequence(copy.RequestAttributesSequence),
    PerformedProcedureStepID(copy.PerformedProcedureStepID),
    PerformedProcedureStepStartDate(copy.PerformedProcedureStepStartDate),
    PerformedProcedureStepStartTime(copy.PerformedProcedureStepStartTime),
    PerformedProcedureStepEndDate(copy.PerformedProcedureStepEndDate),
    PerformedProcedureStepEndTime(copy.PerformedProcedureStepEndTime),
    PerformedProcedureStepDescription(copy.PerformedProcedureStepDescription),
    PerformedProtocolCodeSequence(copy.PerformedProtocolCodeSequence),
    CommentsOnThePerformedProcedureStep(copy.CommentsOnThePerformedProcedureStep),
    ClinicalTrialCoordinatingCenterName(copy.ClinicalTrialCoordinatingCenterName),
    ClinicalTrialSeriesID(copy.ClinicalTrialSeriesID),
    ClinicalTrialSeriesDescription(copy.ClinicalTrialSeriesDescription),
    Manufacturer(copy.Manufacturer),
    InstitutionName(copy.InstitutionName),
    InstitutionAddress(copy.InstitutionAddress),
    StationName(copy.StationName),
    InstitutionalDepartmentName(copy.InstitutionalDepartmentName),
    ManufacturerModelName(copy.ManufacturerModelName),
    DeviceSerialNumber(copy.DeviceSerialNumber),
    SoftwareVersions(copy.SoftwareVersions),
    GantryID(copy.GantryID),
    UDISequence(copy.UDISequence),
    SpatialResolution(copy.SpatialResolution),
    DateOfLastCalibration(copy.DateOfLastCalibration),
    TimeOfLastCalibration(copy.TimeOfLastCalibration),
    PixelPaddingValue(copy.PixelPaddingValue),
    FrameOfReferenceUID(copy.FrameOfReferenceUID),
    PositionReferenceIndicator(copy.PositionReferenceIndicator),
    StructureSetLabel(copy.StructureSetLabel),
    StructureSetName(copy.StructureSetName),
    StructureSetDescription(copy.StructureSetDescription),
    InstanceNumber(copy.InstanceNumber),
    StructureSetDate(copy.StructureSetDate),
    StructureSetTime(copy.StructureSetTime),
    ReferencedFrameOfReferenceSequence(copy.ReferencedFrameOfReferenceSequence),
    StructureSetROISequence(copy.StructureSetROISequence),
    PredecessorStructureSetSequence(copy.PredecessorStructureSetSequence),
    ROIContourSequence(copy.ROIContourSequence),
    RTROIObservationsSequence(copy.RTROIObservationsSequence),
    ApprovalStatus(copy.ApprovalStatus),
    ReviewDate(copy.ReviewDate),
    ReviewTime(copy.ReviewTime),
    ReviewerName(copy.ReviewerName),
    ReferencedImageSequence(copy.ReferencedImageSequence),
    ReferencedInstanceSequence(copy.ReferencedInstanceSequence),
    DerivationDescription(copy.DerivationDescription),
    DerivationCodeSequence(copy.DerivationCodeSequence),
    SourceImageSequence(copy.SourceImageSequence),
    SourceInstanceSequence(copy.SourceInstanceSequence),
    SOPClassUID(copy.SOPClassUID),
    SOPInstanceUID(copy.SOPInstanceUID),
    SpecificCharacterSet(copy.SpecificCharacterSet),
    InstanceCreationDate(copy.InstanceCreationDate),
    InstanceCreationTime(copy.InstanceCreationTime),
    InstanceCoercionDateTime(copy.InstanceCoercionDateTime),
    InstanceCreatorUID(copy.InstanceCreatorUID),
    RelatedGeneralSOPClassUID(copy.RelatedGeneralSOPClassUID),
    OriginalSpecializedSOPClassUID(copy.OriginalSpecializedSOPClassUID),
    CodingSchemeIdentificationSequence(copy.CodingSchemeIdentificationSequence),
    ContextGroupIdentificationSequence(copy.ContextGroupIdentificationSequence),
    MappingResourceIdentificationSequence(copy.MappingResourceIdentificationSequence),
    TimezoneOffsetFromUTC(copy.TimezoneOffsetFromUTC),
    ContributingEquipmentSequence(copy.ContributingEquipmentSequence),
    SOPInstanceStatus(copy.SOPInstanceStatus),
    SOPAuthorizationDateTime(copy.SOPAuthorizationDateTime),
    SOPAuthorizationComment(copy.SOPAuthorizationComment),
    AuthorizationEquipmentCertificationNumber(copy.AuthorizationEquipmentCertificationNumber),
    MACParametersSequence(copy.MACParametersSequence),
    DigitalSignaturesSequence(copy.DigitalSignaturesSequence),
    EncryptedAttributesSequence(copy.EncryptedAttributesSequence),
    OriginalAttributesSequence(copy.OriginalAttributesSequence),
    HL7StructuredDocumentReferenceSequence(copy.HL7StructuredDocumentReferenceSequence),
    LongitudinalTemporalInformationModified(copy.LongitudinalTemporalInformationModified),
    QueryRetrieveView(copy.QueryRetrieveView),
    ConversionSourceAttributesSequence(copy.ConversionSourceAttributesSequence),
    ContentQualification(copy.ContentQualification),
    PrivateDataElementCharacteristicsSequence(copy.PrivateDataElementCharacteristicsSequence),
    ReferencedSeriesSequence(copy.ReferencedSeriesSequence),
    StudiesContainingOtherReferencedInstancesSequence(copy.StudiesContainingOtherReferencedInstancesSequence)
{
}


DRTStructureSetIOD::~DRTStructureSetIOD()
{
}


DRTStructureSetIOD &DRTStructureSetIOD::operator=(const DRTStructureSetIOD &copy)
{
    if (this != &copy)
    {
        PatientName = copy.PatientName;
        PatientID = copy.PatientID;
        IssuerOfPatientID = copy.IssuerOfPatientID;
        IssuerOfPatientIDQualifiersSequence = copy.IssuerOfPatientIDQualifiersSequence;
        PatientBirthDate = copy.PatientBirthDate;
        PatientBirthDateInAlternativeCalendar = copy.PatientBirthDateInAlternativeCalendar;
        PatientDeathDateInAlternativeCalendar = copy.PatientDeathDateInAlternativeCalendar;
        PatientAlternativeCalendar = copy.PatientAlternativeCalendar;
        PatientSex = copy.PatientSex;
        ReferencedPatientPhotoSequence = copy.ReferencedPatientPhotoSequence;
        QualityControlSubject = copy.QualityControlSubject;
        ReferencedPatientSequence = copy.ReferencedPatientSequence;
        PatientBirthTime = copy.PatientBirthTime;
        OtherPatientIDs = copy.OtherPatientIDs;
        OtherPatientIDsSequence = copy.OtherPatientIDsSequence;
        OtherPatientNames = copy.OtherPatientNames;
        EthnicGroup = copy.EthnicGroup;
        PatientComments = copy.PatientComments;
        PatientSpeciesDescription = copy.PatientSpeciesDescription;
        PatientSpeciesCodeSequence = copy.PatientSpeciesCodeSequence;
        PatientBreedDescription = copy.PatientBreedDescription;
        PatientBreedCodeSequence = copy.PatientBreedCodeSequence;
        BreedRegistrationSequence = copy.BreedRegistrationSequence;
        StrainDescription = copy.StrainDescription;
        StrainNomenclature = copy.StrainNomenclature;
        StrainCodeSequence = copy.StrainCodeSequence;
        StrainAdditionalInformation = copy.StrainAdditionalInformation;
        StrainStockSequence = copy.StrainStockSequence;
        GeneticModificationsSequence = copy.GeneticModificationsSequence;
        ResponsiblePerson = copy.ResponsiblePerson;
        ResponsiblePersonRole = copy.ResponsiblePersonRole;
        ResponsibleOrganization = copy.ResponsibleOrganization;
        PatientIdentityRemoved = copy.PatientIdentityRemoved;
        DeidentificationMethod = copy.DeidentificationMethod;
        DeidentificationMethodCodeSequence = copy.DeidentificationMethodCodeSequence;
        SourcePatientGroupIdentificationSequence = copy.SourcePatientGroupIdentificationSequence;
        GroupOfPatientsIdentificationSequence = copy.GroupOfPatientsIdentificationSequence;
        ClinicalTrialSponsorName = copy.ClinicalTrialSponsorName;
        ClinicalTrialProtocolID = copy.ClinicalTrialProtocolID;
        ClinicalTrialProtocolName = copy.ClinicalTrialProtocolName;
        ClinicalTrialSiteID = copy.ClinicalTrialSiteID;
        ClinicalTrialSiteName = copy.ClinicalTrialSiteName;
        ClinicalTrialSubjectID = copy.ClinicalTrialSubjectID;
        ClinicalTrialSubjectReadingID = copy.ClinicalTrialSubjectReadingID;
        ClinicalTrialProtocolEthicsCommitteeName = copy.ClinicalTrialProtocolEthicsCommitteeName;
        ClinicalTrialProtocolEthicsCommitteeApprovalNumber = copy.ClinicalTrialProtocolEthicsCommitteeApprovalNumber;
        StudyInstanceUID = copy.StudyInstanceUID;
        StudyDate = copy.StudyDate;
        StudyTime = copy.StudyTime;
        ReferringPhysicianName = copy.ReferringPhysicianName;
        ReferringPhysicianIdentificationSequence = copy.ReferringPhysicianIdentificationSequence;
        ConsultingPhysicianName = copy.ConsultingPhysicianName;
        ConsultingPhysicianIdentificationSequence = copy.ConsultingPhysicianIdentificationSequence;
        StudyID = copy.StudyID;
        AccessionNumber = copy.AccessionNumber;
        IssuerOfAccessionNumberSequence = copy.IssuerOfAccessionNumberSequence;
        StudyDescription = copy.StudyDescription;
        PhysiciansOfRecord = copy.PhysiciansOfRecord;
        PhysiciansOfRecordIdentificationSequence = copy.PhysiciansOfRecordIdentificationSequence;
        NameOfPhysiciansReadingStudy = copy.NameOfPhysiciansReadingStudy;
        PhysiciansReadingStudyIdentificationSequence = copy.PhysiciansReadingStudyIdentificationSequence;
        RequestingServiceCodeSequence = copy.RequestingServiceCodeSequence;
        ReferencedStudySequence = copy.ReferencedStudySequence;
        ProcedureCodeSequence = copy.ProcedureCodeSequence;
        ReasonForPerformedProcedureCodeSequence = copy.ReasonForPerformedProcedureCodeSequence;
        AdmittingDiagnosesDescription = copy.AdmittingDiagnosesDescription;
        AdmittingDiagnosesCodeSequence = copy.AdmittingDiagnosesCodeSequence;
        PatientAge = copy.PatientAge;
        PatientSize = copy.PatientSize;
        PatientWeight = copy.PatientWeight;
        PatientBodyMassIndex = copy.PatientBodyMassIndex;
        MeasuredAPDimension = copy.MeasuredAPDimension;
        MeasuredLateralDimension = copy.MeasuredLateralDimension;
        PatientSizeCodeSequence = copy.PatientSizeCodeSequence;
        MedicalAlerts = copy.MedicalAlerts;
        Allergies = copy.Allergies;
        SmokingStatus = copy.SmokingStatus;
        PregnancyStatus = copy.PregnancyStatus;
        LastMenstrualDate = copy.LastMenstrualDate;
        PatientState = copy.PatientState;
        Occupation = copy.Occupation;
        AdditionalPatientHistory = copy.AdditionalPatientHistory;
        AdmissionID = copy.AdmissionID;
        IssuerOfAdmissionIDSequence = copy.IssuerOfAdmissionIDSequence;
        ServiceEpisodeID = copy.ServiceEpisodeID;
        IssuerOfServiceEpisodeIDSequence = copy.IssuerOfServiceEpisodeIDSequence;
        ServiceEpisodeDescription = copy.ServiceEpisodeDescription;
        PatientSexNeutered = copy.PatientSexNeutered;
        ClinicalTrialTimePointID = copy.ClinicalTrialTimePointID;
        ClinicalTrialTimePointDescription = copy.ClinicalTrialTimePointDescription;
        ConsentForClinicalTrialUseSequence = copy.ConsentForClinicalTrialUseSequence;
        Modality = copy.Modality;
        SeriesInstanceUID = copy.SeriesInstanceUID;
        SeriesNumber = copy.SeriesNumber;
        SeriesDate = copy.SeriesDate;
        SeriesTime = copy.SeriesTime;
        SeriesDescription = copy.SeriesDescription;
        SeriesDescriptionCodeSequence = copy.SeriesDescriptionCodeSequence;
        OperatorsName = copy.OperatorsName;
        OperatorIdentificationSequence = copy.OperatorIdentificationSequence;
        ReferencedPerformedProcedureStepSequence = copy.ReferencedPerformedProcedureStepSequence;
        RequestAttributesSequence = copy.RequestAttributesSequence;
        PerformedProcedureStepID = copy.PerformedProcedureStepID;
        PerformedProcedureStepStartDate = copy.PerformedProcedureStepStartDate;
        PerformedProcedureStepStartTime = copy.PerformedProcedureStepStartTime;
        PerformedProcedureStepEndDate = copy.PerformedProcedureStepEndDate;
        PerformedProcedureStepEndTime = copy.PerformedProcedureStepEndTime;
        PerformedProcedureStepDescription = copy.PerformedProcedureStepDescription;
        PerformedProtocolCodeSequence = copy.PerformedProtocolCodeSequence;
        CommentsOnThePerformedProcedureStep = copy.CommentsOnThePerformedProcedureStep;
        ClinicalTrialCoordinatingCenterName = copy.ClinicalTrialCoordinatingCenterName;
        ClinicalTrialSeriesID = copy.ClinicalTrialSeriesID;
        ClinicalTrialSeriesDescription = copy.ClinicalTrialSeriesDescription;
        Manufacturer = copy.Manufacturer;
        InstitutionName = copy.InstitutionName;
        InstitutionAddress = copy.InstitutionAddress;
        StationName = copy.StationName;
        InstitutionalDepartmentName = copy.InstitutionalDepartmentName;
        ManufacturerModelName = copy.ManufacturerModelName;
        DeviceSerialNumber = copy.DeviceSerialNumber;
        SoftwareVersions = copy.SoftwareVersions;
        GantryID = copy.GantryID;
        UDISequence = copy.UDISequence;
        SpatialResolution = copy.SpatialResolution;
        DateOfLastCalibration = copy.DateOfLastCalibration;
        TimeOfLastCalibration = copy.TimeOfLastCalibration;
        PixelPaddingValue = copy.PixelPaddingValue;
        FrameOfReferenceUID = copy.FrameOfReferenceUID;
        PositionReferenceIndicator = copy.PositionReferenceIndicator;
        StructureSetLabel = copy.StructureSetLabel;
        StructureSetName = copy.StructureSetName;
        StructureSetDescription = copy.StructureSetDescription;
        InstanceNumber = copy.InstanceNumber;
        StructureSetDate = copy.StructureSetDate;
        StructureSetTime = copy.StructureSetTime;
        ReferencedFrameOfReferenceSequence = copy.ReferencedFrameOfReferenceSequence;
        StructureSetROISequence = copy.StructureSetROISequence;
        PredecessorStructureSetSequence = copy.PredecessorStructureSetSequence;
        ROIContourSequence = copy.ROIContourSequence;
        RTROIObservationsSequence = copy.RTROIObservationsSequence;
        ApprovalStatus = copy.ApprovalStatus;
        ReviewDate = copy.ReviewDate;
        ReviewTime = copy.ReviewTime;
        ReviewerName = copy.ReviewerName;
        ReferencedImageSequence = copy.ReferencedImageSequence;
        ReferencedInstanceSequence = copy.ReferencedInstanceSequence;
        DerivationDescription = copy.DerivationDescription;
        DerivationCodeSequence = copy.DerivationCodeSequence;
        SourceImageSequence = copy.SourceImageSequence;
        SourceInstanceSequence = copy.SourceInstanceSequence;
        SOPClassUID = copy.SOPClassUID;
        SOPInstanceUID = copy.SOPInstanceUID;
        SpecificCharacterSet = copy.SpecificCharacterSet;
        InstanceCreationDate = copy.InstanceCreationDate;
        InstanceCreationTime = copy.InstanceCreationTime;
        InstanceCoercionDateTime = copy.InstanceCoercionDateTime;
        InstanceCreatorUID = copy.InstanceCreatorUID;
        RelatedGeneralSOPClassUID = copy.RelatedGeneralSOPClassUID;
        OriginalSpecializedSOPClassUID = copy.OriginalSpecializedSOPClassUID;
        CodingSchemeIdentificationSequence = copy.CodingSchemeIdentificationSequence;
        ContextGroupIdentificationSequence = copy.ContextGroupIdentificationSequence;
        MappingResourceIdentificationSequence = copy.MappingResourceIdentificationSequence;
        TimezoneOffsetFromUTC = copy.TimezoneOffsetFromUTC;
        ContributingEquipmentSequence = copy.ContributingEquipmentSequence;
        SOPInstanceStatus = copy.SOPInstanceStatus;
        SOPAuthorizationDateTime = copy.SOPAuthorizationDateTime;
        SOPAuthorizationComment = copy.SOPAuthorizationComment;
        AuthorizationEquipmentCertificationNumber = copy.AuthorizationEquipmentCertificationNumber;
        MACParametersSequence = copy.MACParametersSequence;
        DigitalSignaturesSequence = copy.DigitalSignaturesSequence;
        EncryptedAttributesSequence = copy.EncryptedAttributesSequence;
        OriginalAttributesSequence = copy.OriginalAttributesSequence;
        HL7StructuredDocumentReferenceSequence = copy.HL7StructuredDocumentReferenceSequence;
        LongitudinalTemporalInformationModified = copy.LongitudinalTemporalInformationModified;
        QueryRetrieveView = copy.QueryRetrieveView;
        ConversionSourceAttributesSequence = copy.ConversionSourceAttributesSequence;
        ContentQualification = copy.ContentQualification;
        PrivateDataElementCharacteristicsSequence = copy.PrivateDataElementCharacteristicsSequence;
        ReferencedSeriesSequence = copy.ReferencedSeriesSequence;
        StudiesContainingOtherReferencedInstancesSequence = copy.StudiesContainingOtherReferencedInstancesSequence;
    }
    return *this;
}


void DRTStructureSetIOD::clear()
{
    /* clear all DICOM attributes */
    PatientName.clear();
    PatientID.clear();
    IssuerOfPatientID.clear();
    IssuerOfPatientIDQualifiersSequence.clear();
    PatientBirthDate.clear();
    PatientBirthDateInAlternativeCalendar.clear();
    PatientDeathDateInAlternativeCalendar.clear();
    PatientAlternativeCalendar.clear();
    PatientSex.clear();
    ReferencedPatientPhotoSequence.clear();
    QualityControlSubject.clear();
    ReferencedPatientSequence.clear();
    PatientBirthTime.clear();
    OtherPatientIDs.clear();
    OtherPatientIDsSequence.clear();
    OtherPatientNames.clear();
    EthnicGroup.clear();
    PatientComments.clear();
    PatientSpeciesDescription.clear();
    PatientSpeciesCodeSequence.clear();
    PatientBreedDescription.clear();
    PatientBreedCodeSequence.clear();
    BreedRegistrationSequence.clear();
    StrainDescription.clear();
    StrainNomenclature.clear();
    StrainCodeSequence.clear();
    StrainAdditionalInformation.clear();
    StrainStockSequence.clear();
    GeneticModificationsSequence.clear();
    ResponsiblePerson.clear();
    ResponsiblePersonRole.clear();
    ResponsibleOrganization.clear();
    PatientIdentityRemoved.clear();
    DeidentificationMethod.clear();
    DeidentificationMethodCodeSequence.clear();
    SourcePatientGroupIdentificationSequence.clear();
    GroupOfPatientsIdentificationSequence.clear();
    ClinicalTrialSponsorName.clear();
    ClinicalTrialProtocolID.clear();
    ClinicalTrialProtocolName.clear();
    ClinicalTrialSiteID.clear();
    ClinicalTrialSiteName.clear();
    ClinicalTrialSubjectID.clear();
    ClinicalTrialSubjectReadingID.clear();
    ClinicalTrialProtocolEthicsCommitteeName.clear();
    ClinicalTrialProtocolEthicsCommitteeApprovalNumber.clear();
    StudyInstanceUID.clear();
    StudyDate.clear();
    StudyTime.clear();
    ReferringPhysicianName.clear();
    ReferringPhysicianIdentificationSequence.clear();
    ConsultingPhysicianName.clear();
    ConsultingPhysicianIdentificationSequence.clear();
    StudyID.clear();
    AccessionNumber.clear();
    IssuerOfAccessionNumberSequence.clear();
    StudyDescription.clear();
    PhysiciansOfRecord.clear();
    PhysiciansOfRecordIdentificationSequence.clear();
    NameOfPhysiciansReadingStudy.clear();
    PhysiciansReadingStudyIdentificationSequence.clear();
    RequestingServiceCodeSequence.clear();
    ReferencedStudySequence.clear();
    ProcedureCodeSequence.clear();
    ReasonForPerformedProcedureCodeSequence.clear();
    AdmittingDiagnosesDescription.clear();
    AdmittingDiagnosesCodeSequence.clear();
    PatientAge.clear();
    PatientSize.clear();
    PatientWeight.clear();
    PatientBodyMassIndex.clear();
    MeasuredAPDimension.clear();
    MeasuredLateralDimension.clear();
    PatientSizeCodeSequence.clear();
    MedicalAlerts.clear();
    Allergies.clear();
    SmokingStatus.clear();
    PregnancyStatus.clear();
    LastMenstrualDate.clear();
    PatientState.clear();
    Occupation.clear();
    AdditionalPatientHistory.clear();
    AdmissionID.clear();
    IssuerOfAdmissionIDSequence.clear();
    ServiceEpisodeID.clear();
    IssuerOfServiceEpisodeIDSequence.clear();
    ServiceEpisodeDescription.clear();
    PatientSexNeutered.clear();
    ClinicalTrialTimePointID.clear();
    ClinicalTrialTimePointDescription.clear();
    ConsentForClinicalTrialUseSequence.clear();
    Modality.clear();
    SeriesInstanceUID.clear();
    SeriesNumber.clear();
    SeriesDate.clear();
    SeriesTime.clear();
    SeriesDescription.clear();
    SeriesDescriptionCodeSequence.clear();
    OperatorsName.clear();
    OperatorIdentificationSequence.clear();
    ReferencedPerformedProcedureStepSequence.clear();
    RequestAttributesSequence.clear();
    PerformedProcedureStepID.clear();
    PerformedProcedureStepStartDate.clear();
    PerformedProcedureStepStartTime.clear();
    PerformedProcedureStepEndDate.clear();
    PerformedProcedureStepEndTime.clear();
    PerformedProcedureStepDescription.clear();
    PerformedProtocolCodeSequence.clear();
    CommentsOnThePerformedProcedureStep.clear();
    ClinicalTrialCoordinatingCenterName.clear();
    ClinicalTrialSeriesID.clear();
    ClinicalTrialSeriesDescription.clear();
    Manufacturer.clear();
    InstitutionName.clear();
    InstitutionAddress.clear();
    StationName.clear();
    InstitutionalDepartmentName.clear();
    ManufacturerModelName.clear();
    DeviceSerialNumber.clear();
    SoftwareVersions.clear();
    GantryID.clear();
    UDISequence.clear();
    SpatialResolution.clear();
    DateOfLastCalibration.clear();
    TimeOfLastCalibration.clear();
    PixelPaddingValue.clear();
    FrameOfReferenceUID.clear();
    PositionReferenceIndicator.clear();
    StructureSetLabel.clear();
    StructureSetName.clear();
    StructureSetDescription.clear();
    InstanceNumber.clear();
    StructureSetDate.clear();
    StructureSetTime.clear();
    ReferencedFrameOfReferenceSequence.clear();
    StructureSetROISequence.clear();
    PredecessorStructureSetSequence.clear();
    ROIContourSequence.clear();
    RTROIObservationsSequence.clear();
    ApprovalStatus.clear();
    ReviewDate.clear();
    ReviewTime.clear();
    ReviewerName.clear();
    ReferencedImageSequence.clear();
    ReferencedInstanceSequence.clear();
    DerivationDescription.clear();
    DerivationCodeSequence.clear();
    SourceImageSequence.clear();
    SourceInstanceSequence.clear();
    SOPClassUID.clear();
    SOPInstanceUID.clear();
    SpecificCharacterSet.clear();
    InstanceCreationDate.clear();
    InstanceCreationTime.clear();
    InstanceCoercionDateTime.clear();
    InstanceCreatorUID.clear();
    RelatedGeneralSOPClassUID.clear();
    OriginalSpecializedSOPClassUID.clear();
    CodingSchemeIdentificationSequence.clear();
    ContextGroupIdentificationSequence.clear();
    MappingResourceIdentificationSequence.clear();
    TimezoneOffsetFromUTC.clear();
    ContributingEquipmentSequence.clear();
    SOPInstanceStatus.clear();
    SOPAuthorizationDateTime.clear();
    SOPAuthorizationComment.clear();
    AuthorizationEquipmentCertificationNumber.clear();
    MACParametersSequence.clear();
    DigitalSignaturesSequence.clear();
    EncryptedAttributesSequence.clear();
    OriginalAttributesSequence.clear();
    HL7StructuredDocumentReferenceSequence.clear();
    LongitudinalTemporalInformationModified.clear();
    QueryRetrieveView.clear();
    ConversionSourceAttributesSequence.clear();
    ContentQualification.clear();
    PrivateDataElementCharacteristicsSequence.clear();
    ReferencedSeriesSequence.clear();
    StudiesContainingOtherReferencedInstancesSequence.clear();
}


OFBool DRTStructureSetIOD::isValid()
{
    /* tbd: check whether object is valid */
    return OFTrue;
}


OFCondition DRTStructureSetIOD::checkDatasetForReading(DcmItem &dataset)
{
    OFCondition result = EC_Normal;
    /* check SOP class UID */
    DcmUniqueIdentifier sopClassUID(DCM_SOPClassUID);
    result = getAndCheckElementFromDataset(dataset, sopClassUID, "1", "1", "SOPCommonModule");
    if (result.good())
    {
        OFString tmpString;
        getStringValueFromElement(sopClassUID, tmpString);
        if (tmpString != UID_RTStructureSetStorage)
        {
            DCMRT_ERROR("Invalid value for attribute SOPClassUID (0008,0016)");
            result = RT_EC_InvalidValue;
        }
    }
    return result;
}


void DRTStructureSetIOD::updateAttributes()
{
    SOPClassUID.putString(UID_RTStructureSetStorage);
}


OFCondition DRTStructureSetIOD::read(DcmItem &dataset)
{
    /* re-initialize object */
    clear();
    /* check SOP class UID first */
    OFCondition result = checkDatasetForReading(dataset);
    /* read data from PatientIE, StudyIE, SeriesIE */
    if (result.good())
        result = readSeriesData(dataset);
    if (result.good())
    {
        // --- GeneralEquipmentModule (M) ---
        getAndCheckElementFromDataset(dataset, Manufacturer, "1", "2", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, InstitutionName, "1", "3", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, InstitutionAddress, "1", "3", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, StationName, "1", "3", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, InstitutionalDepartmentName, "1", "3", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, ManufacturerModelName, "1", "3", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, DeviceSerialNumber, "1", "3", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, SoftwareVersions, "1-n", "3", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, GantryID, "1", "3", "GeneralEquipmentModule");
        UDISequence.read(dataset, "1-n", "3", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, SpatialResolution, "1", "3", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, DateOfLastCalibration, "1-n", "3", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, TimeOfLastCalibration, "1-n", "3", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, PixelPaddingValue, "1", "1C", "GeneralEquipmentModule");

        // --- FrameOfReferenceModule (U) ---
        if (dataset.tagExists(DCM_FrameOfReferenceUID) ||
            dataset.tagExists(DCM_PositionReferenceIndicator))
        {
            getAndCheckElementFromDataset(dataset, FrameOfReferenceUID, "1", "1", "FrameOfReferenceModule");
            getAndCheckElementFromDataset(dataset, PositionReferenceIndicator, "1", "2", "FrameOfReferenceModule");
        }

        // --- StructureSetModule (M) ---
        getAndCheckElementFromDataset(dataset, StructureSetLabel, "1", "1", "StructureSetModule");
        getAndCheckElementFromDataset(dataset, StructureSetName, "1", "3", "StructureSetModule");
        getAndCheckElementFromDataset(dataset, StructureSetDescription, "1", "3", "StructureSetModule");
        getAndCheckElementFromDataset(dataset, InstanceNumber, "1", "3", "StructureSetModule");
        getAndCheckElementFromDataset(dataset, StructureSetDate, "1", "2", "StructureSetModule");
        getAndCheckElementFromDataset(dataset, StructureSetTime, "1", "2", "StructureSetModule");
        ReferencedFrameOfReferenceSequence.read(dataset, "1-n", "3", "StructureSetModule");
        StructureSetROISequence.read(dataset, "1-n", "1", "StructureSetModule");
        PredecessorStructureSetSequence.read(dataset, "1-n", "3", "StructureSetModule");

        // --- ROIContourModule (M) ---
        ROIContourSequence.read(dataset, "1-n", "1", "ROIContourModule");

        // --- RTROIObservationsModule (M) ---
        RTROIObservationsSequence.read(dataset, "1-n", "1", "RTROIObservationsModule");

        // --- ApprovalModule (U) ---
        if (dataset.tagExists(DCM_ApprovalStatus))
        {
            getAndCheckElementFromDataset(dataset, ApprovalStatus, "1", "1", "ApprovalModule");
            getAndCheckElementFromDataset(dataset, ReviewDate, "1", "2C", "ApprovalModule");
            getAndCheckElementFromDataset(dataset, ReviewTime, "1", "2C", "ApprovalModule");
            getAndCheckElementFromDataset(dataset, ReviewerName, "1", "2C", "ApprovalModule");
        }

        // --- GeneralReferenceModule (U) ---
        ReferencedImageSequence.read(dataset, "1-n", "3", "GeneralReferenceModule");
        ReferencedInstanceSequence.read(dataset, "1-n", "3", "GeneralReferenceModule");
        getAndCheckElementFromDataset(dataset, DerivationDescription, "1", "3", "GeneralReferenceModule");
        DerivationCodeSequence.read(dataset, "1-n", "3", "GeneralReferenceModule");
        SourceImageSequence.read(dataset, "1-n", "3", "GeneralReferenceModule");
        SourceInstanceSequence.read(dataset, "1-n", "3", "GeneralReferenceModule");

        // --- SOPCommonModule (M) ---
        getAndCheckElementFromDataset(dataset, SOPClassUID, "1", "1", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, SOPInstanceUID, "1", "1", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, SpecificCharacterSet, "1-n", "1C", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, InstanceCreationDate, "1", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, InstanceCreationTime, "1", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, InstanceCoercionDateTime, "1", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, InstanceCreatorUID, "1", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, RelatedGeneralSOPClassUID, "1-n", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, OriginalSpecializedSOPClassUID, "1", "3", "SOPCommonModule");
        CodingSchemeIdentificationSequence.read(dataset, "1-n", "3", "SOPCommonModule");
        ContextGroupIdentificationSequence.read(dataset, "1-n", "3", "SOPCommonModule");
        MappingResourceIdentificationSequence.read(dataset, "1-n", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, TimezoneOffsetFromUTC, "1", "3", "SOPCommonModule");
        ContributingEquipmentSequence.read(dataset, "1-n", "3", "SOPCommonModule");
        // getAndCheckElementFromDataset(dataset, InstanceNumber, "1", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, SOPInstanceStatus, "1", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, SOPAuthorizationDateTime, "1", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, SOPAuthorizationComment, "1", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, AuthorizationEquipmentCertificationNumber, "1", "3", "SOPCommonModule");
        MACParametersSequence.read(dataset, "1-n", "3", "SOPCommonModule");
        DigitalSignaturesSequence.read(dataset, "1-n", "3", "SOPCommonModule");
        EncryptedAttributesSequence.read(dataset, "1-n", "1C", "SOPCommonModule");
        OriginalAttributesSequence.read(dataset, "1-n", "3", "SOPCommonModule");
        HL7StructuredDocumentReferenceSequence.read(dataset, "1-n", "1C", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, LongitudinalTemporalInformationModified, "1", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, QueryRetrieveView, "1", "1C", "SOPCommonModule");
        ConversionSourceAttributesSequence.read(dataset, "1-n", "1C", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, ContentQualification, "1", "3", "SOPCommonModule");
        PrivateDataElementCharacteristicsSequence.read(dataset, "1-n", "3", "SOPCommonModule");

        // --- CommonInstanceReferenceModule (U) ---
        ReferencedSeriesSequence.read(dataset, "1-n", "1C", "CommonInstanceReferenceModule");
        StudiesContainingOtherReferencedInstancesSequence.read(dataset, "1-n", "1C", "CommonInstanceReferenceModule");
    }
    return result;
}


OFCondition DRTStructureSetIOD::readPatientData(DcmItem &dataset)
{
    OFCondition result = EC_Normal;

    // --- PatientModule (M) ---
    getAndCheckElementFromDataset(dataset, PatientName, "1", "2", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientID, "1", "2", "PatientModule");
    getAndCheckElementFromDataset(dataset, IssuerOfPatientID, "1", "3", "PatientModule");
    IssuerOfPatientIDQualifiersSequence.read(dataset, "1-n", "3", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientBirthDate, "1", "2", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientBirthDateInAlternativeCalendar, "1", "3", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientDeathDateInAlternativeCalendar, "1", "3", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientAlternativeCalendar, "1", "1C", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientSex, "1", "2", "PatientModule");
    ReferencedPatientPhotoSequence.read(dataset, "1-n", "3", "PatientModule");
    getAndCheckElementFromDataset(dataset, QualityControlSubject, "1", "3", "PatientModule");
    ReferencedPatientSequence.read(dataset, "1-n", "3", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientBirthTime, "1", "3", "PatientModule");
    getAndCheckElementFromDataset(dataset, OtherPatientIDs, "1-n", "3", "PatientModule");
    OtherPatientIDsSequence.read(dataset, "1-n", "3", "PatientModule");
    getAndCheckElementFromDataset(dataset, OtherPatientNames, "1-n", "3", "PatientModule");
    getAndCheckElementFromDataset(dataset, EthnicGroup, "1", "3", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientComments, "1", "3", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientSpeciesDescription, "1", "1C", "PatientModule");
    PatientSpeciesCodeSequence.read(dataset, "1-n", "1C", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientBreedDescription, "1", "2C", "PatientModule");
    PatientBreedCodeSequence.read(dataset, "1-n", "2C", "PatientModule");
    BreedRegistrationSequence.read(dataset, "1-n", "2C", "PatientModule");
    getAndCheckElementFromDataset(dataset, StrainDescription, "1", "3", "PatientModule");
    getAndCheckElementFromDataset(dataset, StrainNomenclature, "1", "3", "PatientModule");
    StrainCodeSequence.read(dataset, "1-n", "3", "PatientModule");
    getAndCheckElementFromDataset(dataset, StrainAdditionalInformation, "1", "3", "PatientModule");
    StrainStockSequence.read(dataset, "1-n", "3", "PatientModule");
    GeneticModificationsSequence.read(dataset, "1-n", "3", "PatientModule");
    getAndCheckElementFromDataset(dataset, ResponsiblePerson, "1", "2C", "PatientModule");
    getAndCheckElementFromDataset(dataset, ResponsiblePersonRole, "1", "1C", "PatientModule");
    getAndCheckElementFromDataset(dataset, ResponsibleOrganization, "1", "2C", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientIdentityRemoved, "1", "3", "PatientModule");
    getAndCheckElementFromDataset(dataset, DeidentificationMethod, "1-n", "1C", "PatientModule");
    DeidentificationMethodCodeSequence.read(dataset, "1-n", "1C", "PatientModule");
    SourcePatientGroupIdentificationSequence.read(dataset, "1-n", "3", "PatientModule");
    GroupOfPatientsIdentificationSequence.read(dataset, "1-n", "3", "PatientModule");

    // --- ClinicalTrialSubjectModule (U) ---
    if (dataset.tagExists(DCM_ClinicalTrialSponsorName) ||
        dataset.tagExists(DCM_ClinicalTrialProtocolID) ||
        dataset.tagExists(DCM_ClinicalTrialProtocolName) ||
        dataset.tagExists(DCM_ClinicalTrialSiteID) ||
        dataset.tagExists(DCM_ClinicalTrialSiteName))
    {
        getAndCheckElementFromDataset(dataset, ClinicalTrialSponsorName, "1", "1", "ClinicalTrialSubjectModule");
        getAndCheckElementFromDataset(dataset, ClinicalTrialProtocolID, "1", "1", "ClinicalTrialSubjectModule");
        getAndCheckElementFromDataset(dataset, ClinicalTrialProtocolName, "1", "2", "ClinicalTrialSubjectModule");
        getAndCheckElementFromDataset(dataset, ClinicalTrialSiteID, "1", "2", "ClinicalTrialSubjectModule");
        getAndCheckElementFromDataset(dataset, ClinicalTrialSiteName, "1", "2", "ClinicalTrialSubjectModule");
        getAndCheckElementFromDataset(dataset, ClinicalTrialSubjectID, "1", "1C", "ClinicalTrialSubjectModule");
        getAndCheckElementFromDataset(dataset, ClinicalTrialSubjectReadingID, "1", "1C", "ClinicalTrialSubjectModule");
        getAndCheckElementFromDataset(dataset, ClinicalTrialProtocolEthicsCommitteeName, "1", "1C", "ClinicalTrialSubjectModule");
        getAndCheckElementFromDataset(dataset, ClinicalTrialProtocolEthicsCommitteeApprovalNumber, "1", "3", "ClinicalTrialSubjectModule");
    }
    return result;
}


OFCondition DRTStructureSetIOD::readStudyData(DcmItem &dataset)
{
    /* read data from PatientIE */
    OFCondition result = readPatientData(dataset);
    if (result.good())
    {
        // --- GeneralStudyModule (M) ---
        getAndCheckElementFromDataset(dataset, StudyInstanceUID, "1", "1", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, StudyDate, "1", "2", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, StudyTime, "1", "2", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, ReferringPhysicianName, "1", "2", "GeneralStudyModule");
        ReferringPhysicianIdentificationSequence.read(dataset, "1-n", "3", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, ConsultingPhysicianName, "1-n", "3", "GeneralStudyModule");
        ConsultingPhysicianIdentificationSequence.read(dataset, "1-n", "3", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, StudyID, "1", "2", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, AccessionNumber, "1", "2", "GeneralStudyModule");
        IssuerOfAccessionNumberSequence.read(dataset, "1-n", "3", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, StudyDescription, "1", "3", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, PhysiciansOfRecord, "1-n", "3", "GeneralStudyModule");
        PhysiciansOfRecordIdentificationSequence.read(dataset, "1-n", "3", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, NameOfPhysiciansReadingStudy, "1-n", "3", "GeneralStudyModule");
        PhysiciansReadingStudyIdentificationSequence.read(dataset, "1-n", "3", "GeneralStudyModule");
        RequestingServiceCodeSequence.read(dataset, "1-n", "3", "GeneralStudyModule");
        ReferencedStudySequence.read(dataset, "1-n", "3", "GeneralStudyModule");
        ProcedureCodeSequence.read(dataset, "1-n", "3", "GeneralStudyModule");
        ReasonForPerformedProcedureCodeSequence.read(dataset, "1-n", "3", "GeneralStudyModule");

        // --- PatientStudyModule (U) ---
        getAndCheckElementFromDataset(dataset, AdmittingDiagnosesDescription, "1-n", "3", "PatientStudyModule");
        AdmittingDiagnosesCodeSequence.read(dataset, "1-n", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, PatientAge, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, PatientSize, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, PatientWeight, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, PatientBodyMassIndex, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, MeasuredAPDimension, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, MeasuredLateralDimension, "1", "3", "PatientStudyModule");
        PatientSizeCodeSequence.read(dataset, "1-n", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, MedicalAlerts, "1-n", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, Allergies, "1-n", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, SmokingStatus, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, PregnancyStatus, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, LastMenstrualDate, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, PatientState, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, Occupation, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, AdditionalPatientHistory, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, AdmissionID, "1", "3", "PatientStudyModule");
        IssuerOfAdmissionIDSequence.read(dataset, "1-n", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, ServiceEpisodeID, "1", "3", "PatientStudyModule");
        IssuerOfServiceEpisodeIDSequence.read(dataset, "1-n", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, ServiceEpisodeDescription, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, PatientSexNeutered, "1", "2C", "PatientStudyModule");

        // --- ClinicalTrialStudyModule (U) ---
        if (dataset.tagExists(DCM_ClinicalTrialTimePointID))
        {
            getAndCheckElementFromDataset(dataset, ClinicalTrialTimePointID, "1", "2", "ClinicalTrialStudyModule");
            getAndCheckElementFromDataset(dataset, ClinicalTrialTimePointDescription, "1", "3", "ClinicalTrialStudyModule");
            ConsentForClinicalTrialUseSequence.read(dataset, "1-n", "3", "ClinicalTrialStudyModule");
        }
    }
    return result;
}


OFCondition DRTStructureSetIOD::readSeriesData(DcmItem &dataset)
{
    /* read data from PatientIE, StudyIE */
    OFCondition result = readStudyData(dataset);
    if (result.good())
    {
        // --- RTSeriesModule (M) ---
        getAndCheckElementFromDataset(dataset, Modality, "1", "1", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, SeriesInstanceUID, "1", "1", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, SeriesNumber, "1", "2", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, SeriesDate, "1", "3", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, SeriesTime, "1", "3", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, SeriesDescription, "1", "3", "RTSeriesModule");
        SeriesDescriptionCodeSequence.read(dataset, "1-n", "3", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, OperatorsName, "1-n", "2", "RTSeriesModule");
        OperatorIdentificationSequence.read(dataset, "1-n", "3", "RTSeriesModule");
        ReferencedPerformedProcedureStepSequence.read(dataset, "1-n", "3", "RTSeriesModule");
        RequestAttributesSequence.read(dataset, "1-n", "3", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, PerformedProcedureStepID, "1", "3", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, PerformedProcedureStepStartDate, "1", "3", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, PerformedProcedureStepStartTime, "1", "3", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, PerformedProcedureStepEndDate, "1", "3", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, PerformedProcedureStepEndTime, "1", "3", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, PerformedProcedureStepDescription, "1", "3", "RTSeriesModule");
        PerformedProtocolCodeSequence.read(dataset, "1-n", "3", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, CommentsOnThePerformedProcedureStep, "1", "3", "RTSeriesModule");

        // --- ClinicalTrialSeriesModule (U) ---
        if (dataset.tagExists(DCM_ClinicalTrialCoordinatingCenterName))
        {
            getAndCheckElementFromDataset(dataset, ClinicalTrialCoordinatingCenterName, "1", "2", "ClinicalTrialSeriesModule");
            getAndCheckElementFromDataset(dataset, ClinicalTrialSeriesID, "1", "3", "ClinicalTrialSeriesModule");
            getAndCheckElementFromDataset(dataset, ClinicalTrialSeriesDescription, "1", "3", "ClinicalTrialSeriesModule");
        }
    }
    return result;
}


OFCondition DRTStructureSetIOD::write(DcmItem &dataset)
{
    OFCondition result = EC_Normal;
    /* only write valid objects */
    if (isValid())
    {
        /* update various DICOM attributes */
        updateAttributes();

        // --- PatientModule (M) ---
        addElementToDataset(result, dataset, new DcmPersonName(PatientName), "1", "2", "PatientModule");
        addElementToDataset(result, dataset, new DcmLongString(PatientID), "1", "2", "PatientModule");
        addElementToDataset(result, dataset, new DcmLongString(IssuerOfPatientID), "1", "3", "PatientModule");
        if (result.good()) result = IssuerOfPatientIDQualifiersSequence.write(dataset, "1-n" ,"3", "PatientModule");
        addElementToDataset(result, dataset, new DcmDate(PatientBirthDate), "1", "2", "PatientModule");
        addElementToDataset(result, dataset, new DcmLongString(PatientBirthDateInAlternativeCalendar), "1", "3", "PatientModule");
        addElementToDataset(result, dataset, new DcmLongString(PatientDeathDateInAlternativeCalendar), "1", "3", "PatientModule");
        addElementToDataset(result, dataset, new DcmCodeString(PatientAlternativeCalendar), "1", "1C", "PatientModule");
        addElementToDataset(result, dataset, new DcmCodeString(PatientSex), "1", "2", "PatientModule");
        if (result.good()) result = ReferencedPatientPhotoSequence.write(dataset, "1-n" ,"3", "PatientModule");
        addElementToDataset(result, dataset, new DcmCodeString(QualityControlSubject), "1", "3", "PatientModule");
        if (result.good()) result = ReferencedPatientSequence.write(dataset, "1-n" ,"3", "PatientModule");
        addElementToDataset(result, dataset, new DcmTime(PatientBirthTime), "1", "3", "PatientModule");
        addElementToDataset(result, dataset, new DcmLongString(OtherPatientIDs), "1-n", "3", "PatientModule");
        if (result.good()) result = OtherPatientIDsSequence.write(dataset, "1-n" ,"3", "PatientModule");
        addElementToDataset(result, dataset, new DcmPersonName(OtherPatientNames), "1-n", "3", "PatientModule");
        addElementToDataset(result, dataset, new DcmShortString(EthnicGroup), "1", "3", "PatientModule");
        addElementToDataset(result, dataset, new DcmLongText(PatientComments), "1", "3", "PatientModule");
        addElementToDataset(result, dataset, new DcmLongString(PatientSpeciesDescription), "1", "1C", "PatientModule");
        if (result.good()) result = PatientSpeciesCodeSequence.write(dataset, "1-n" ,"1C", "PatientModule");
        addElementToDataset(result, dataset, new DcmLongString(PatientBreedDescription), "1", "2C", "PatientModule");
        if (result.good()) result = PatientBreedCodeSequence.write(dataset, "1-n" ,"2C", "PatientModule");
        if (result.good()) result = BreedRegistrationSequence.write(dataset, "1-n" ,"2C", "PatientModule");
        addElementToDataset(result, dataset, new DcmUnlimitedCharacters(StrainDescription), "1", "3", "PatientModule");
        addElementToDataset(result, dataset, new DcmLongString(StrainNomenclature), "1", "3", "PatientModule");
        if (result.good()) result = StrainCodeSequence.write(dataset, "1-n" ,"3", "PatientModule");
        addElementToDataset(result, dataset, new DcmUnlimitedText(StrainAdditionalInformation), "1", "3", "PatientModule");
        if (result.good()) result = StrainStockSequence.write(dataset, "1-n" ,"3", "PatientModule");
        if (result.good()) result = GeneticModificationsSequence.write(dataset, "1-n" ,"3", "PatientModule");
        addElementToDataset(result, dataset, new DcmPersonName(ResponsiblePerson), "1", "2C", "PatientModule");
        addElementToDataset(result, dataset, new DcmCodeString(ResponsiblePersonRole), "1", "1C", "PatientModule");
        addElementToDataset(result, dataset, new DcmLongString(ResponsibleOrganization), "1", "2C", "PatientModule");
        addElementToDataset(result, dataset, new DcmCodeString(PatientIdentityRemoved), "1", "3", "PatientModule");
        addElementToDataset(result, dataset, new DcmLongString(DeidentificationMethod), "1-n", "1C", "PatientModule");
        if (result.good()) result = DeidentificationMethodCodeSequence.write(dataset, "1-n" ,"1C", "PatientModule");
        if (result.good()) result = SourcePatientGroupIdentificationSequence.write(dataset, "1-n" ,"3", "PatientModule");
        if (result.good()) result = GroupOfPatientsIdentificationSequence.write(dataset, "1-n" ,"3", "PatientModule");

        // --- ClinicalTrialSubjectModule (U) ---
        if (isClinicalTrialSubjectModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmLongString(ClinicalTrialSponsorName), "1", "1", "ClinicalTrialSubjectModule");
            addElementToDataset(result, dataset, new DcmLongString(ClinicalTrialProtocolID), "1", "1", "ClinicalTrialSubjectModule");
            addElementToDataset(result, dataset, new DcmLongString(ClinicalTrialProtocolName), "1", "2", "ClinicalTrialSubjectModule");
            addElementToDataset(result, dataset, new DcmLongString(ClinicalTrialSiteID), "1", "2", "ClinicalTrialSubjectModule");
            addElementToDataset(result, dataset, new DcmLongString(ClinicalTrialSiteName), "1", "2", "ClinicalTrialSubjectModule");
            addElementToDataset(result, dataset, new DcmLongString(ClinicalTrialSubjectID), "1", "1C", "ClinicalTrialSubjectModule");
            addElementToDataset(result, dataset, new DcmLongString(ClinicalTrialSubjectReadingID), "1", "1C", "ClinicalTrialSubjectModule");
            addElementToDataset(result, dataset, new DcmLongString(ClinicalTrialProtocolEthicsCommitteeName), "1", "1C", "ClinicalTrialSubjectModule");
            addElementToDataset(result, dataset, new DcmLongString(ClinicalTrialProtocolEthicsCommitteeApprovalNumber), "1", "3", "ClinicalTrialSubjectModule");
        }

        // --- GeneralStudyModule (M) ---
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(StudyInstanceUID), "1", "1", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmDate(StudyDate), "1", "2", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmTime(StudyTime), "1", "2", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmPersonName(ReferringPhysicianName), "1", "2", "GeneralStudyModule");
        if (result.good()) result = ReferringPhysicianIdentificationSequence.write(dataset, "1-n" ,"3", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmPersonName(ConsultingPhysicianName), "1-n", "3", "GeneralStudyModule");
        if (result.good()) result = ConsultingPhysicianIdentificationSequence.write(dataset, "1-n" ,"3", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmShortString(StudyID), "1", "2", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmShortString(AccessionNumber), "1", "2", "GeneralStudyModule");
        if (result.good()) result = IssuerOfAccessionNumberSequence.write(dataset, "1-n" ,"3", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmLongString(StudyDescription), "1", "3", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmPersonName(PhysiciansOfRecord), "1-n", "3", "GeneralStudyModule");
        if (result.good()) result = PhysiciansOfRecordIdentificationSequence.write(dataset, "1-n" ,"3", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmPersonName(NameOfPhysiciansReadingStudy), "1-n", "3", "GeneralStudyModule");
        if (result.good()) result = PhysiciansReadingStudyIdentificationSequence.write(dataset, "1-n" ,"3", "GeneralStudyModule");
        if (result.good()) result = RequestingServiceCodeSequence.write(dataset, "1-n" ,"3", "GeneralStudyModule");
        if (result.good()) result = ReferencedStudySequence.write(dataset, "1-n" ,"3", "GeneralStudyModule");
        if (result.good()) result = ProcedureCodeSequence.write(dataset, "1-n" ,"3", "GeneralStudyModule");
        if (result.good()) result = ReasonForPerformedProcedureCodeSequence.write(dataset, "1-n" ,"3", "GeneralStudyModule");

        // --- PatientStudyModule (U) ---
        if (isPatientStudyModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmLongString(AdmittingDiagnosesDescription), "1-n", "3", "PatientStudyModule");
            if (result.good()) result = AdmittingDiagnosesCodeSequence.write(dataset, "1-n" ,"3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmAgeString(PatientAge), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmDecimalString(PatientSize), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmDecimalString(PatientWeight), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmDecimalString(PatientBodyMassIndex), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmDecimalString(MeasuredAPDimension), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmDecimalString(MeasuredLateralDimension), "1", "3", "PatientStudyModule");
            if (result.good()) result = PatientSizeCodeSequence.write(dataset, "1-n" ,"3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmLongString(MedicalAlerts), "1-n", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmLongString(Allergies), "1-n", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmCodeString(SmokingStatus), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmUnsignedShort(PregnancyStatus), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmDate(LastMenstrualDate), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmLongString(PatientState), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmShortString(Occupation), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmLongText(AdditionalPatientHistory), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmLongString(AdmissionID), "1", "3", "PatientStudyModule");
            if (result.good()) result = IssuerOfAdmissionIDSequence.write(dataset, "1-n" ,"3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmLongString(ServiceEpisodeID), "1", "3", "PatientStudyModule");
            if (result.good()) result = IssuerOfServiceEpisodeIDSequence.write(dataset, "1-n" ,"3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmLongString(ServiceEpisodeDescription), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmCodeString(PatientSexNeutered), "1", "2C", "PatientStudyModule");
        }

        // --- ClinicalTrialStudyModule (U) ---
        if (isClinicalTrialStudyModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmLongString(ClinicalTrialTimePointID), "1", "2", "ClinicalTrialStudyModule");
            addElementToDataset(result, dataset, new DcmShortText(ClinicalTrialTimePointDescription), "1", "3", "ClinicalTrialStudyModule");
            if (result.good()) result = ConsentForClinicalTrialUseSequence.write(dataset, "1-n" ,"3", "ClinicalTrialStudyModule");
        }

        // --- RTSeriesModule (M) ---
        addElementToDataset(result, dataset, new DcmCodeString(Modality), "1", "1", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(SeriesInstanceUID), "1", "1", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmIntegerString(SeriesNumber), "1", "2", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmDate(SeriesDate), "1", "3", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmTime(SeriesTime), "1", "3", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmLongString(SeriesDescription), "1", "3", "RTSeriesModule");
        if (result.good()) result = SeriesDescriptionCodeSequence.write(dataset, "1-n" ,"3", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmPersonName(OperatorsName), "1-n", "2", "RTSeriesModule");
        if (result.good()) result = OperatorIdentificationSequence.write(dataset, "1-n" ,"3", "RTSeriesModule");
        if (result.good()) result = ReferencedPerformedProcedureStepSequence.write(dataset, "1-n" ,"3", "RTSeriesModule");
        if (result.good()) result = RequestAttributesSequence.write(dataset, "1-n" ,"3", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmShortString(PerformedProcedureStepID), "1", "3", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmDate(PerformedProcedureStepStartDate), "1", "3", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmTime(PerformedProcedureStepStartTime), "1", "3", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmDate(PerformedProcedureStepEndDate), "1", "3", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmTime(PerformedProcedureStepEndTime), "1", "3", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmLongString(PerformedProcedureStepDescription), "1", "3", "RTSeriesModule");
        if (result.good()) result = PerformedProtocolCodeSequence.write(dataset, "1-n" ,"3", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmShortText(CommentsOnThePerformedProcedureStep), "1", "3", "RTSeriesModule");

        // --- ClinicalTrialSeriesModule (U) ---
        if (isClinicalTrialSeriesModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmLongString(ClinicalTrialCoordinatingCenterName), "1", "2", "ClinicalTrialSeriesModule");
            addElementToDataset(result, dataset, new DcmLongString(ClinicalTrialSeriesID), "1", "3", "ClinicalTrialSeriesModule");
            addElementToDataset(result, dataset, new DcmLongString(ClinicalTrialSeriesDescription), "1", "3", "ClinicalTrialSeriesModule");
        }

        // --- GeneralEquipmentModule (M) ---
        addElementToDataset(result, dataset, new DcmLongString(Manufacturer), "1", "2", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmLongString(InstitutionName), "1", "3", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmShortText(InstitutionAddress), "1", "3", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmShortString(StationName), "1", "3", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmLongString(InstitutionalDepartmentName), "1", "3", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmLongString(ManufacturerModelName), "1", "3", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmLongString(DeviceSerialNumber), "1", "3", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmLongString(SoftwareVersions), "1-n", "3", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmLongString(GantryID), "1", "3", "GeneralEquipmentModule");
        if (result.good()) result = UDISequence.write(dataset, "1-n" ,"3", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmDecimalString(SpatialResolution), "1", "3", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmDate(DateOfLastCalibration), "1-n", "3", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmTime(TimeOfLastCalibration), "1-n", "3", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmUnsignedShort(PixelPaddingValue), "1", "1C", "GeneralEquipmentModule");

        // --- FrameOfReferenceModule (U) ---
        if (isFrameOfReferenceModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmUniqueIdentifier(FrameOfReferenceUID), "1", "1", "FrameOfReferenceModule");
            addElementToDataset(result, dataset, new DcmLongString(PositionReferenceIndicator), "1", "2", "FrameOfReferenceModule");
        }

        // --- StructureSetModule (M) ---
        addElementToDataset(result, dataset, new DcmShortString(StructureSetLabel), "1", "1", "StructureSetModule");
        addElementToDataset(result, dataset, new DcmLongString(StructureSetName), "1", "3", "StructureSetModule");
        addElementToDataset(result, dataset, new DcmShortText(StructureSetDescription), "1", "3", "StructureSetModule");
        addElementToDataset(result, dataset, new DcmIntegerString(InstanceNumber), "1", "3", "StructureSetModule");
        addElementToDataset(result, dataset, new DcmDate(StructureSetDate), "1", "2", "StructureSetModule");
        addElementToDataset(result, dataset, new DcmTime(StructureSetTime), "1", "2", "StructureSetModule");
        if (result.good()) result = ReferencedFrameOfReferenceSequence.write(dataset, "1-n" ,"3", "StructureSetModule");
        if (result.good()) result = StructureSetROISequence.write(dataset, "1-n" ,"1", "StructureSetModule");
        if (result.good()) result = PredecessorStructureSetSequence.write(dataset, "1-n" ,"3", "StructureSetModule");

        // --- ROIContourModule (M) ---
        if (result.good()) result = ROIContourSequence.write(dataset, "1-n" ,"1", "ROIContourModule");

        // --- RTROIObservationsModule (M) ---
        if (result.good()) result = RTROIObservationsSequence.write(dataset, "1-n" ,"1", "RTROIObservationsModule");

        // --- ApprovalModule (U) ---
        if (isApprovalModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmCodeString(ApprovalStatus), "1", "1", "ApprovalModule");
            addElementToDataset(result, dataset, new DcmDate(ReviewDate), "1", "2C", "ApprovalModule");
            addElementToDataset(result, dataset, new DcmTime(ReviewTime), "1", "2C", "ApprovalModule");
            addElementToDataset(result, dataset, new DcmPersonName(ReviewerName), "1", "2C", "ApprovalModule");
        }

        // --- GeneralReferenceModule (U) ---
        if (isGeneralReferenceModulePresent(OFFalse /*complete*/))
        {
            if (result.good()) result = ReferencedImageSequence.write(dataset, "1-n" ,"3", "GeneralReferenceModule");
            if (result.good()) result = ReferencedInstanceSequence.write(dataset, "1-n" ,"3", "GeneralReferenceModule");
            addElementToDataset(result, dataset, new DcmShortText(DerivationDescription), "1", "3", "GeneralReferenceModule");
            if (result.good()) result = DerivationCodeSequence.write(dataset, "1-n" ,"3", "GeneralReferenceModule");
            if (result.good()) result = SourceImageSequence.write(dataset, "1-n" ,"3", "GeneralReferenceModule");
            if (result.good()) result = SourceInstanceSequence.write(dataset, "1-n" ,"3", "GeneralReferenceModule");
        }

        // --- SOPCommonModule (M) ---
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(SOPClassUID), "1", "1", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(SOPInstanceUID), "1", "1", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmCodeString(SpecificCharacterSet), "1-n", "1C", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmDate(InstanceCreationDate), "1", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmTime(InstanceCreationTime), "1", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmDateTime(InstanceCoercionDateTime), "1", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(InstanceCreatorUID), "1", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(RelatedGeneralSOPClassUID), "1-n", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(OriginalSpecializedSOPClassUID), "1", "3", "SOPCommonModule");
        if (result.good()) result = CodingSchemeIdentificationSequence.write(dataset, "1-n" ,"3", "SOPCommonModule");
        if (result.good()) result = ContextGroupIdentificationSequence.write(dataset, "1-n" ,"3", "SOPCommonModule");
        if (result.good()) result = MappingResourceIdentificationSequence.write(dataset, "1-n" ,"3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmShortString(TimezoneOffsetFromUTC), "1", "3", "SOPCommonModule");
        if (result.good()) result = ContributingEquipmentSequence.write(dataset, "1-n" ,"3", "SOPCommonModule");
        // addElementToDataset(result, dataset, new DcmIntegerString(InstanceNumber), "1", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmCodeString(SOPInstanceStatus), "1", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmDateTime(SOPAuthorizationDateTime), "1", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmLongText(SOPAuthorizationComment), "1", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmLongString(AuthorizationEquipmentCertificationNumber), "1", "3", "SOPCommonModule");
        if (result.good()) result = MACParametersSequence.write(dataset, "1-n" ,"3", "SOPCommonModule");
        if (result.good()) result = DigitalSignaturesSequence.write(dataset, "1-n" ,"3", "SOPCommonModule");
        if (result.good()) result = EncryptedAttributesSequence.write(dataset, "1-n" ,"1C", "SOPCommonModule");
        if (result.good()) result = OriginalAttributesSequence.write(dataset, "1-n" ,"3", "SOPCommonModule");
        if (result.good()) result = HL7StructuredDocumentReferenceSequence.write(dataset, "1-n" ,"1C", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmCodeString(LongitudinalTemporalInformationModified), "1", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmCodeString(QueryRetrieveView), "1", "1C", "SOPCommonModule");
        if (result.good()) result = ConversionSourceAttributesSequence.write(dataset, "1-n" ,"1C", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmCodeString(ContentQualification), "1", "3", "SOPCommonModule");
        if (result.good()) result = PrivateDataElementCharacteristicsSequence.write(dataset, "1-n" ,"3", "SOPCommonModule");

        // --- CommonInstanceReferenceModule (U) ---
        if (isCommonInstanceReferenceModulePresent(OFFalse /*complete*/))
        {
            if (result.good()) result = ReferencedSeriesSequence.write(dataset, "1-n" ,"1C", "CommonInstanceReferenceModule");
            if (result.good()) result = StudiesContainingOtherReferencedInstancesSequence.write(dataset, "1-n" ,"1C", "CommonInstanceReferenceModule");
        }
    } else
        result = RT_EC_InvalidObject;
    return result;
}


OFBool DRTStructureSetIOD::isClinicalTrialSubjectModulePresent(const OFBool complete)
{
    if (complete)
    {
        /* check whether all mandatory attributes are present */
        return !ClinicalTrialSponsorName.isEmpty() &&
               !ClinicalTrialProtocolID.isEmpty();
    } else {
        /* check whether at least one attribute is present */
        return !ClinicalTrialSponsorName.isEmpty() ||
               !ClinicalTrialProtocolID.isEmpty() ||
               !ClinicalTrialProtocolName.isEmpty() ||
               !ClinicalTrialSiteID.isEmpty() ||
               !ClinicalTrialSiteName.isEmpty() ||
               !ClinicalTrialSubjectID.isEmpty() ||
               !ClinicalTrialSubjectReadingID.isEmpty() ||
               !ClinicalTrialProtocolEthicsCommitteeName.isEmpty() ||
               !ClinicalTrialProtocolEthicsCommitteeApprovalNumber.isEmpty();
    }
}


OFBool DRTStructureSetIOD::isPatientStudyModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !AdmittingDiagnosesDescription.isEmpty() ||
           !AdmittingDiagnosesCodeSequence.isEmpty() ||
           !PatientAge.isEmpty() ||
           !PatientSize.isEmpty() ||
           !PatientWeight.isEmpty() ||
           !PatientBodyMassIndex.isEmpty() ||
           !MeasuredAPDimension.isEmpty() ||
           !MeasuredLateralDimension.isEmpty() ||
           !PatientSizeCodeSequence.isEmpty() ||
           !MedicalAlerts.isEmpty() ||
           !Allergies.isEmpty() ||
           !SmokingStatus.isEmpty() ||
           !PregnancyStatus.isEmpty() ||
           !LastMenstrualDate.isEmpty() ||
           !PatientState.isEmpty() ||
           !Occupation.isEmpty() ||
           !AdditionalPatientHistory.isEmpty() ||
           !AdmissionID.isEmpty() ||
           !IssuerOfAdmissionIDSequence.isEmpty() ||
           !ServiceEpisodeID.isEmpty() ||
           !IssuerOfServiceEpisodeIDSequence.isEmpty() ||
           !ServiceEpisodeDescription.isEmpty() ||
           !PatientSexNeutered.isEmpty();
}


OFBool DRTStructureSetIOD::isClinicalTrialStudyModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !ClinicalTrialTimePointID.isEmpty() ||
           !ClinicalTrialTimePointDescription.isEmpty() ||
           !ConsentForClinicalTrialUseSequence.isEmpty();
}


OFBool DRTStructureSetIOD::isClinicalTrialSeriesModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !ClinicalTrialCoordinatingCenterName.isEmpty() ||
           !ClinicalTrialSeriesID.isEmpty() ||
           !ClinicalTrialSeriesDescription.isEmpty();
}


OFBool DRTStructureSetIOD::isFrameOfReferenceModulePresent(const OFBool complete)
{
    if (complete)
    {
        /* check whether all mandatory attributes are present */
        return !FrameOfReferenceUID.isEmpty();
    } else {
        /* check whether at least one attribute is present */
        return !FrameOfReferenceUID.isEmpty() ||
               !PositionReferenceIndicator.isEmpty();
    }
}


OFBool DRTStructureSetIOD::isApprovalModulePresent(const OFBool complete)
{
    if (complete)
    {
        /* check whether all mandatory attributes are present */
        return !ApprovalStatus.isEmpty();
    } else {
        /* check whether at least one attribute is present */
        return !ApprovalStatus.isEmpty() ||
               !ReviewDate.isEmpty() ||
               !ReviewTime.isEmpty() ||
               !ReviewerName.isEmpty();
    }
}


OFBool DRTStructureSetIOD::isGeneralReferenceModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !ReferencedImageSequence.isEmpty() ||
           !ReferencedInstanceSequence.isEmpty() ||
           !DerivationDescription.isEmpty() ||
           !DerivationCodeSequence.isEmpty() ||
           !SourceImageSequence.isEmpty() ||
           !SourceInstanceSequence.isEmpty();
}


OFBool DRTStructureSetIOD::isCommonInstanceReferenceModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !ReferencedSeriesSequence.isEmpty() ||
           !StudiesContainingOtherReferencedInstancesSequence.isEmpty();
}


OFCondition DRTStructureSetIOD::getAccessionNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AccessionNumber, value, pos);
}


OFCondition DRTStructureSetIOD::getAdditionalPatientHistory(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AdditionalPatientHistory, value, pos);
}


OFCondition DRTStructureSetIOD::getAdmissionID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AdmissionID, value, pos);
}


OFCondition DRTStructureSetIOD::getAdmittingDiagnosesDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AdmittingDiagnosesDescription, value, pos);
}


OFCondition DRTStructureSetIOD::getAllergies(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(Allergies, value, pos);
}


OFCondition DRTStructureSetIOD::getApprovalStatus(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ApprovalStatus, value, pos);
}


OFCondition DRTStructureSetIOD::getAuthorizationEquipmentCertificationNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AuthorizationEquipmentCertificationNumber, value, pos);
}


OFCondition DRTStructureSetIOD::getClinicalTrialCoordinatingCenterName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialCoordinatingCenterName, value, pos);
}


OFCondition DRTStructureSetIOD::getClinicalTrialProtocolEthicsCommitteeApprovalNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialProtocolEthicsCommitteeApprovalNumber, value, pos);
}


OFCondition DRTStructureSetIOD::getClinicalTrialProtocolEthicsCommitteeName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialProtocolEthicsCommitteeName, value, pos);
}


OFCondition DRTStructureSetIOD::getClinicalTrialProtocolID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialProtocolID, value, pos);
}


OFCondition DRTStructureSetIOD::getClinicalTrialProtocolName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialProtocolName, value, pos);
}


OFCondition DRTStructureSetIOD::getClinicalTrialSeriesDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSeriesDescription, value, pos);
}


OFCondition DRTStructureSetIOD::getClinicalTrialSeriesID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSeriesID, value, pos);
}


OFCondition DRTStructureSetIOD::getClinicalTrialSiteID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSiteID, value, pos);
}


OFCondition DRTStructureSetIOD::getClinicalTrialSiteName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSiteName, value, pos);
}


OFCondition DRTStructureSetIOD::getClinicalTrialSponsorName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSponsorName, value, pos);
}


OFCondition DRTStructureSetIOD::getClinicalTrialSubjectID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSubjectID, value, pos);
}


OFCondition DRTStructureSetIOD::getClinicalTrialSubjectReadingID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSubjectReadingID, value, pos);
}


OFCondition DRTStructureSetIOD::getClinicalTrialTimePointDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialTimePointDescription, value, pos);
}


OFCondition DRTStructureSetIOD::getClinicalTrialTimePointID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialTimePointID, value, pos);
}


OFCondition DRTStructureSetIOD::getCommentsOnThePerformedProcedureStep(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(CommentsOnThePerformedProcedureStep, value, pos);
}


OFCondition DRTStructureSetIOD::getConsultingPhysicianName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ConsultingPhysicianName, value, pos);
}


OFCondition DRTStructureSetIOD::getContentQualification(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ContentQualification, value, pos);
}


OFCondition DRTStructureSetIOD::getDateOfLastCalibration(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DateOfLastCalibration, value, pos);
}


OFCondition DRTStructureSetIOD::getDeidentificationMethod(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DeidentificationMethod, value, pos);
}


OFCondition DRTStructureSetIOD::getDerivationDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DerivationDescription, value, pos);
}


OFCondition DRTStructureSetIOD::getDeviceSerialNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DeviceSerialNumber, value, pos);
}


OFCondition DRTStructureSetIOD::getEthnicGroup(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(EthnicGroup, value, pos);
}


OFCondition DRTStructureSetIOD::getFrameOfReferenceUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(FrameOfReferenceUID, value, pos);
}


OFCondition DRTStructureSetIOD::getGantryID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(GantryID, value, pos);
}


OFCondition DRTStructureSetIOD::getInstanceCoercionDateTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceCoercionDateTime, value, pos);
}


OFCondition DRTStructureSetIOD::getInstanceCreationDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceCreationDate, value, pos);
}


OFCondition DRTStructureSetIOD::getInstanceCreationTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceCreationTime, value, pos);
}


OFCondition DRTStructureSetIOD::getInstanceCreatorUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceCreatorUID, value, pos);
}


OFCondition DRTStructureSetIOD::getInstanceNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceNumber, value, pos);
}


OFCondition DRTStructureSetIOD::getInstanceNumber(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, InstanceNumber).getSint32(value, pos);
}


OFCondition DRTStructureSetIOD::getInstitutionAddress(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstitutionAddress, value, pos);
}


OFCondition DRTStructureSetIOD::getInstitutionName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstitutionName, value, pos);
}


OFCondition DRTStructureSetIOD::getInstitutionalDepartmentName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstitutionalDepartmentName, value, pos);
}


OFCondition DRTStructureSetIOD::getIssuerOfPatientID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(IssuerOfPatientID, value, pos);
}


OFCondition DRTStructureSetIOD::getLastMenstrualDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(LastMenstrualDate, value, pos);
}


OFCondition DRTStructureSetIOD::getLongitudinalTemporalInformationModified(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(LongitudinalTemporalInformationModified, value, pos);
}


OFCondition DRTStructureSetIOD::getManufacturer(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(Manufacturer, value, pos);
}


OFCondition DRTStructureSetIOD::getManufacturerModelName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ManufacturerModelName, value, pos);
}


OFCondition DRTStructureSetIOD::getMeasuredAPDimension(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(MeasuredAPDimension, value, pos);
}


OFCondition DRTStructureSetIOD::getMeasuredAPDimension(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, MeasuredAPDimension).getFloat64(value, pos);
}


OFCondition DRTStructureSetIOD::getMeasuredLateralDimension(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(MeasuredLateralDimension, value, pos);
}


OFCondition DRTStructureSetIOD::getMeasuredLateralDimension(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, MeasuredLateralDimension).getFloat64(value, pos);
}


OFCondition DRTStructureSetIOD::getMedicalAlerts(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(MedicalAlerts, value, pos);
}


OFCondition DRTStructureSetIOD::getModality(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(Modality, value, pos);
}


OFCondition DRTStructureSetIOD::getNameOfPhysiciansReadingStudy(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(NameOfPhysiciansReadingStudy, value, pos);
}


OFCondition DRTStructureSetIOD::getOccupation(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(Occupation, value, pos);
}


OFCondition DRTStructureSetIOD::getOperatorsName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OperatorsName, value, pos);
}


OFCondition DRTStructureSetIOD::getOriginalSpecializedSOPClassUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OriginalSpecializedSOPClassUID, value, pos);
}


OFCondition DRTStructureSetIOD::getOtherPatientIDs(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OtherPatientIDs, value, pos);
}


OFCondition DRTStructureSetIOD::getOtherPatientNames(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OtherPatientNames, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientAge(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientAge, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientAlternativeCalendar(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientAlternativeCalendar, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientBirthDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientBirthDate, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientBirthDateInAlternativeCalendar(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientBirthDateInAlternativeCalendar, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientBirthTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientBirthTime, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientBodyMassIndex(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientBodyMassIndex, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientBodyMassIndex(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, PatientBodyMassIndex).getFloat64(value, pos);
}


OFCondition DRTStructureSetIOD::getPatientBreedDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientBreedDescription, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientComments(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientComments, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientDeathDateInAlternativeCalendar(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientDeathDateInAlternativeCalendar, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientID, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientIdentityRemoved(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientIdentityRemoved, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientName, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientSex(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientSex, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientSexNeutered(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientSexNeutered, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientSize(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientSize, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientSize(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, PatientSize).getFloat64(value, pos);
}


OFCondition DRTStructureSetIOD::getPatientSpeciesDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientSpeciesDescription, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientState(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientState, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientWeight(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientWeight, value, pos);
}


OFCondition DRTStructureSetIOD::getPatientWeight(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, PatientWeight).getFloat64(value, pos);
}


OFCondition DRTStructureSetIOD::getPerformedProcedureStepDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepDescription, value, pos);
}


OFCondition DRTStructureSetIOD::getPerformedProcedureStepEndDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepEndDate, value, pos);
}


OFCondition DRTStructureSetIOD::getPerformedProcedureStepEndTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepEndTime, value, pos);
}


OFCondition DRTStructureSetIOD::getPerformedProcedureStepID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepID, value, pos);
}


OFCondition DRTStructureSetIOD::getPerformedProcedureStepStartDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepStartDate, value, pos);
}


OFCondition DRTStructureSetIOD::getPerformedProcedureStepStartTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepStartTime, value, pos);
}


OFCondition DRTStructureSetIOD::getPhysiciansOfRecord(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PhysiciansOfRecord, value, pos);
}


OFCondition DRTStructureSetIOD::getPixelPaddingValue(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, PixelPaddingValue).getUint16(value, pos);
}


OFCondition DRTStructureSetIOD::getPositionReferenceIndicator(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PositionReferenceIndicator, value, pos);
}


OFCondition DRTStructureSetIOD::getPregnancyStatus(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, PregnancyStatus).getUint16(value, pos);
}


OFCondition DRTStructureSetIOD::getQualityControlSubject(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(QualityControlSubject, value, pos);
}


OFCondition DRTStructureSetIOD::getQueryRetrieveView(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(QueryRetrieveView, value, pos);
}


OFCondition DRTStructureSetIOD::getReferringPhysicianName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ReferringPhysicianName, value, pos);
}


OFCondition DRTStructureSetIOD::getRelatedGeneralSOPClassUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RelatedGeneralSOPClassUID, value, pos);
}


OFCondition DRTStructureSetIOD::getResponsibleOrganization(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ResponsibleOrganization, value, pos);
}


OFCondition DRTStructureSetIOD::getResponsiblePerson(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ResponsiblePerson, value, pos);
}


OFCondition DRTStructureSetIOD::getResponsiblePersonRole(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ResponsiblePersonRole, value, pos);
}


OFCondition DRTStructureSetIOD::getReviewDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ReviewDate, value, pos);
}


OFCondition DRTStructureSetIOD::getReviewTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ReviewTime, value, pos);
}


OFCondition DRTStructureSetIOD::getReviewerName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ReviewerName, value, pos);
}


OFCondition DRTStructureSetIOD::getSOPAuthorizationComment(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPAuthorizationComment, value, pos);
}


OFCondition DRTStructureSetIOD::getSOPAuthorizationDateTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPAuthorizationDateTime, value, pos);
}


OFCondition DRTStructureSetIOD::getSOPClassUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPClassUID, value, pos);
}


OFCondition DRTStructureSetIOD::getSOPInstanceStatus(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPInstanceStatus, value, pos);
}


OFCondition DRTStructureSetIOD::getSOPInstanceUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPInstanceUID, value, pos);
}


OFCondition DRTStructureSetIOD::getSeriesDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SeriesDate, value, pos);
}


OFCondition DRTStructureSetIOD::getSeriesDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SeriesDescription, value, pos);
}


OFCondition DRTStructureSetIOD::getSeriesInstanceUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SeriesInstanceUID, value, pos);
}


OFCondition DRTStructureSetIOD::getSeriesNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SeriesNumber, value, pos);
}


OFCondition DRTStructureSetIOD::getSeriesNumber(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, SeriesNumber).getSint32(value, pos);
}


OFCondition DRTStructureSetIOD::getSeriesTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SeriesTime, value, pos);
}


OFCondition DRTStructureSetIOD::getServiceEpisodeDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ServiceEpisodeDescription, value, pos);
}


OFCondition DRTStructureSetIOD::getServiceEpisodeID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ServiceEpisodeID, value, pos);
}


OFCondition DRTStructureSetIOD::getSmokingStatus(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SmokingStatus, value, pos);
}


OFCondition DRTStructureSetIOD::getSoftwareVersions(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SoftwareVersions, value, pos);
}


OFCondition DRTStructureSetIOD::getSpatialResolution(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SpatialResolution, value, pos);
}


OFCondition DRTStructureSetIOD::getSpatialResolution(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, SpatialResolution).getFloat64(value, pos);
}


OFCondition DRTStructureSetIOD::getSpecificCharacterSet(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SpecificCharacterSet, value, pos);
}


OFCondition DRTStructureSetIOD::getStationName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StationName, value, pos);
}


OFCondition DRTStructureSetIOD::getStrainAdditionalInformation(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StrainAdditionalInformation, value, pos);
}


OFCondition DRTStructureSetIOD::getStrainDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StrainDescription, value, pos);
}


OFCondition DRTStructureSetIOD::getStrainNomenclature(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StrainNomenclature, value, pos);
}


OFCondition DRTStructureSetIOD::getStructureSetDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StructureSetDate, value, pos);
}


OFCondition DRTStructureSetIOD::getStructureSetDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StructureSetDescription, value, pos);
}


OFCondition DRTStructureSetIOD::getStructureSetLabel(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StructureSetLabel, value, pos);
}


OFCondition DRTStructureSetIOD::getStructureSetName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StructureSetName, value, pos);
}


OFCondition DRTStructureSetIOD::getStructureSetTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StructureSetTime, value, pos);
}


OFCondition DRTStructureSetIOD::getStudyDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyDate, value, pos);
}


OFCondition DRTStructureSetIOD::getStudyDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyDescription, value, pos);
}


OFCondition DRTStructureSetIOD::getStudyID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyID, value, pos);
}


OFCondition DRTStructureSetIOD::getStudyInstanceUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyInstanceUID, value, pos);
}


OFCondition DRTStructureSetIOD::getStudyTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyTime, value, pos);
}


OFCondition DRTStructureSetIOD::getTimeOfLastCalibration(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(TimeOfLastCalibration, value, pos);
}


OFCondition DRTStructureSetIOD::getTimezoneOffsetFromUTC(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(TimezoneOffsetFromUTC, value, pos);
}


OFCondition DRTStructureSetIOD::setAccessionNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AccessionNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setAdditionalPatientHistory(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = AdditionalPatientHistory.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setAdmissionID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AdmissionID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setAdmittingDiagnosesDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = AdmittingDiagnosesDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setAllergies(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = Allergies.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setApprovalStatus(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ApprovalStatus.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setAuthorizationEquipmentCertificationNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AuthorizationEquipmentCertificationNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setClinicalTrialCoordinatingCenterName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialCoordinatingCenterName.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setClinicalTrialProtocolEthicsCommitteeApprovalNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialProtocolEthicsCommitteeApprovalNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setClinicalTrialProtocolEthicsCommitteeName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialProtocolEthicsCommitteeName.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setClinicalTrialProtocolID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialProtocolID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setClinicalTrialProtocolName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialProtocolName.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setClinicalTrialSeriesDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSeriesDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setClinicalTrialSeriesID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSeriesID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setClinicalTrialSiteID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSiteID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setClinicalTrialSiteName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSiteName.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setClinicalTrialSponsorName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSponsorName.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setClinicalTrialSubjectID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSubjectID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setClinicalTrialSubjectReadingID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSubjectReadingID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setClinicalTrialTimePointDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = ClinicalTrialTimePointDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setClinicalTrialTimePointID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialTimePointID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setCommentsOnThePerformedProcedureStep(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = CommentsOnThePerformedProcedureStep.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setConsultingPhysicianName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = ConsultingPhysicianName.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setContentQualification(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ContentQualification.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setDateOfLastCalibration(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = DateOfLastCalibration.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setDeidentificationMethod(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = DeidentificationMethod.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setDerivationDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = DerivationDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setDeviceSerialNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = DeviceSerialNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setEthnicGroup(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = EthnicGroup.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setFrameOfReferenceUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = FrameOfReferenceUID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setGantryID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = GantryID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setInstanceCoercionDateTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceCoercionDateTime.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setInstanceCreationDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceCreationDate.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setInstanceCreationTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceCreationTime.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setInstanceCreatorUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceCreatorUID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setInstanceNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setInstitutionAddress(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = InstitutionAddress.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setInstitutionName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstitutionName.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setInstitutionalDepartmentName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstitutionalDepartmentName.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setIssuerOfPatientID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = IssuerOfPatientID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setLastMenstrualDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = LastMenstrualDate.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setLongitudinalTemporalInformationModified(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = LongitudinalTemporalInformationModified.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setManufacturer(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = Manufacturer.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setManufacturerModelName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ManufacturerModelName.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setMeasuredAPDimension(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = MeasuredAPDimension.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setMeasuredLateralDimension(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = MeasuredLateralDimension.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setMedicalAlerts(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = MedicalAlerts.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setModality(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = Modality.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setNameOfPhysiciansReadingStudy(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = NameOfPhysiciansReadingStudy.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setOccupation(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = Occupation.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setOperatorsName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = OperatorsName.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setOriginalSpecializedSOPClassUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = OriginalSpecializedSOPClassUID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setOtherPatientIDs(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = OtherPatientIDs.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setOtherPatientNames(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = OtherPatientNames.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientAge(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmAgeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientAge.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientAlternativeCalendar(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientAlternativeCalendar.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientBirthDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientBirthDate.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientBirthDateInAlternativeCalendar(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientBirthDateInAlternativeCalendar.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientBirthTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientBirthTime.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientBodyMassIndex(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientBodyMassIndex.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientBreedDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientBreedDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientComments(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = PatientComments.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientDeathDateInAlternativeCalendar(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientDeathDateInAlternativeCalendar.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientIdentityRemoved(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientIdentityRemoved.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientName.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientSex(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSex.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientSexNeutered(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSexNeutered.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientSize(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSize.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientSpeciesDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSpeciesDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientState(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientState.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPatientWeight(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientWeight.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPerformedProcedureStepDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPerformedProcedureStepEndDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepEndDate.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPerformedProcedureStepEndTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepEndTime.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPerformedProcedureStepID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPerformedProcedureStepStartDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepStartDate.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPerformedProcedureStepStartTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepStartTime.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPhysiciansOfRecord(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = PhysiciansOfRecord.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPixelPaddingValue(const Uint16 value, const unsigned long pos)
{
    return PixelPaddingValue.putUint16(value, pos);
}


OFCondition DRTStructureSetIOD::setPositionReferenceIndicator(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PositionReferenceIndicator.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setPregnancyStatus(const Uint16 value, const unsigned long pos)
{
    return PregnancyStatus.putUint16(value, pos);
}


OFCondition DRTStructureSetIOD::setQualityControlSubject(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = QualityControlSubject.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setQueryRetrieveView(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = QueryRetrieveView.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setReferringPhysicianName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ReferringPhysicianName.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setRelatedGeneralSOPClassUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = RelatedGeneralSOPClassUID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setResponsibleOrganization(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ResponsibleOrganization.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setResponsiblePerson(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ResponsiblePerson.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setResponsiblePersonRole(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ResponsiblePersonRole.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setReviewDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ReviewDate.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setReviewTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ReviewTime.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setReviewerName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ReviewerName.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setSOPAuthorizationComment(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = SOPAuthorizationComment.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setSOPAuthorizationDateTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SOPAuthorizationDateTime.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setSOPClassUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SOPClassUID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setSOPInstanceStatus(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SOPInstanceStatus.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setSOPInstanceUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SOPInstanceUID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setSeriesDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesDate.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setSeriesDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setSeriesInstanceUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesInstanceUID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setSeriesNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setSeriesTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesTime.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setServiceEpisodeDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ServiceEpisodeDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setServiceEpisodeID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ServiceEpisodeID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setSmokingStatus(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SmokingStatus.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setSoftwareVersions(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = SoftwareVersions.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setSpatialResolution(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SpatialResolution.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setSpecificCharacterSet(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = SpecificCharacterSet.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setStationName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StationName.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setStrainAdditionalInformation(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUnlimitedText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = StrainAdditionalInformation.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setStrainDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUnlimitedCharacters::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StrainDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setStrainNomenclature(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StrainNomenclature.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setStructureSetDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StructureSetDate.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setStructureSetDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = StructureSetDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setStructureSetLabel(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StructureSetLabel.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setStructureSetName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StructureSetName.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setStructureSetTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StructureSetTime.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setStudyDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyDate.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setStudyDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setStudyID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setStudyInstanceUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyInstanceUID.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setStudyTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyTime.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setTimeOfLastCalibration(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = TimeOfLastCalibration.putOFStringArray(value);
    return result;
}


OFCondition DRTStructureSetIOD::setTimezoneOffsetFromUTC(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = TimezoneOffsetFromUTC.putOFStringArray(value);
    return result;
}


// end of source file
