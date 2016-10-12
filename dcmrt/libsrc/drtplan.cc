/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTPlanIOD
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"      // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/drtplan.h"


DRTPlanIOD::DRTPlanIOD()
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
    FrameOfReferenceUID(DCM_FrameOfReferenceUID),
    PositionReferenceIndicator(DCM_PositionReferenceIndicator),
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
    RTPlanLabel(DCM_RTPlanLabel),
    RTPlanName(DCM_RTPlanName),
    RTPlanDescription(DCM_RTPlanDescription),
    InstanceNumber(DCM_InstanceNumber),
    RTPlanDate(DCM_RTPlanDate),
    RTPlanTime(DCM_RTPlanTime),
    TreatmentProtocols(DCM_TreatmentProtocols),
    PlanIntent(DCM_PlanIntent),
    TreatmentSites(DCM_TreatmentSites),
    RTPlanGeometry(DCM_RTPlanGeometry),
    ReferencedStructureSetSequence(),
    ReferencedDoseSequence(),
    ReferencedRTPlanSequence(),
    FrameOfReferenceToDisplayedCoordinateSystemTransformationMatrix(DCM_FrameOfReferenceToDisplayedCoordinateSystemTransformationMatrix),
    PrescriptionDescription(DCM_PrescriptionDescription),
    DoseReferenceSequence(),
    ToleranceTableSequence(),
    PatientSetupSequence(),
    FractionGroupSequence(),
    BeamSequence(),
    BrachyTreatmentTechnique(DCM_BrachyTreatmentTechnique),
    BrachyTreatmentType(DCM_BrachyTreatmentType),
    TreatmentMachineSequence(),
    SourceSequence(),
    ApplicationSetupSequence(),
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


DRTPlanIOD::DRTPlanIOD(const DRTPlanIOD &copy)
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
    FrameOfReferenceUID(copy.FrameOfReferenceUID),
    PositionReferenceIndicator(copy.PositionReferenceIndicator),
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
    RTPlanLabel(copy.RTPlanLabel),
    RTPlanName(copy.RTPlanName),
    RTPlanDescription(copy.RTPlanDescription),
    InstanceNumber(copy.InstanceNumber),
    RTPlanDate(copy.RTPlanDate),
    RTPlanTime(copy.RTPlanTime),
    TreatmentProtocols(copy.TreatmentProtocols),
    PlanIntent(copy.PlanIntent),
    TreatmentSites(copy.TreatmentSites),
    RTPlanGeometry(copy.RTPlanGeometry),
    ReferencedStructureSetSequence(copy.ReferencedStructureSetSequence),
    ReferencedDoseSequence(copy.ReferencedDoseSequence),
    ReferencedRTPlanSequence(copy.ReferencedRTPlanSequence),
    FrameOfReferenceToDisplayedCoordinateSystemTransformationMatrix(copy.FrameOfReferenceToDisplayedCoordinateSystemTransformationMatrix),
    PrescriptionDescription(copy.PrescriptionDescription),
    DoseReferenceSequence(copy.DoseReferenceSequence),
    ToleranceTableSequence(copy.ToleranceTableSequence),
    PatientSetupSequence(copy.PatientSetupSequence),
    FractionGroupSequence(copy.FractionGroupSequence),
    BeamSequence(copy.BeamSequence),
    BrachyTreatmentTechnique(copy.BrachyTreatmentTechnique),
    BrachyTreatmentType(copy.BrachyTreatmentType),
    TreatmentMachineSequence(copy.TreatmentMachineSequence),
    SourceSequence(copy.SourceSequence),
    ApplicationSetupSequence(copy.ApplicationSetupSequence),
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


DRTPlanIOD::~DRTPlanIOD()
{
}


DRTPlanIOD &DRTPlanIOD::operator=(const DRTPlanIOD &copy)
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
        FrameOfReferenceUID = copy.FrameOfReferenceUID;
        PositionReferenceIndicator = copy.PositionReferenceIndicator;
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
        RTPlanLabel = copy.RTPlanLabel;
        RTPlanName = copy.RTPlanName;
        RTPlanDescription = copy.RTPlanDescription;
        InstanceNumber = copy.InstanceNumber;
        RTPlanDate = copy.RTPlanDate;
        RTPlanTime = copy.RTPlanTime;
        TreatmentProtocols = copy.TreatmentProtocols;
        PlanIntent = copy.PlanIntent;
        TreatmentSites = copy.TreatmentSites;
        RTPlanGeometry = copy.RTPlanGeometry;
        ReferencedStructureSetSequence = copy.ReferencedStructureSetSequence;
        ReferencedDoseSequence = copy.ReferencedDoseSequence;
        ReferencedRTPlanSequence = copy.ReferencedRTPlanSequence;
        FrameOfReferenceToDisplayedCoordinateSystemTransformationMatrix = copy.FrameOfReferenceToDisplayedCoordinateSystemTransformationMatrix;
        PrescriptionDescription = copy.PrescriptionDescription;
        DoseReferenceSequence = copy.DoseReferenceSequence;
        ToleranceTableSequence = copy.ToleranceTableSequence;
        PatientSetupSequence = copy.PatientSetupSequence;
        FractionGroupSequence = copy.FractionGroupSequence;
        BeamSequence = copy.BeamSequence;
        BrachyTreatmentTechnique = copy.BrachyTreatmentTechnique;
        BrachyTreatmentType = copy.BrachyTreatmentType;
        TreatmentMachineSequence = copy.TreatmentMachineSequence;
        SourceSequence = copy.SourceSequence;
        ApplicationSetupSequence = copy.ApplicationSetupSequence;
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


void DRTPlanIOD::clear()
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
    FrameOfReferenceUID.clear();
    PositionReferenceIndicator.clear();
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
    RTPlanLabel.clear();
    RTPlanName.clear();
    RTPlanDescription.clear();
    InstanceNumber.clear();
    RTPlanDate.clear();
    RTPlanTime.clear();
    TreatmentProtocols.clear();
    PlanIntent.clear();
    TreatmentSites.clear();
    RTPlanGeometry.clear();
    ReferencedStructureSetSequence.clear();
    ReferencedDoseSequence.clear();
    ReferencedRTPlanSequence.clear();
    FrameOfReferenceToDisplayedCoordinateSystemTransformationMatrix.clear();
    PrescriptionDescription.clear();
    DoseReferenceSequence.clear();
    ToleranceTableSequence.clear();
    PatientSetupSequence.clear();
    FractionGroupSequence.clear();
    BeamSequence.clear();
    BrachyTreatmentTechnique.clear();
    BrachyTreatmentType.clear();
    TreatmentMachineSequence.clear();
    SourceSequence.clear();
    ApplicationSetupSequence.clear();
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


OFBool DRTPlanIOD::isValid()
{
    /* tbd: check whether object is valid */
    return OFTrue;
}


OFCondition DRTPlanIOD::checkDatasetForReading(DcmItem &dataset)
{
    OFCondition result = EC_Normal;
    /* check SOP class UID */
    DcmUniqueIdentifier sopClassUID(DCM_SOPClassUID);
    result = getAndCheckElementFromDataset(dataset, sopClassUID, "1", "1", "SOPCommonModule");
    if (result.good())
    {
        OFString tmpString;
        getStringValueFromElement(sopClassUID, tmpString);
        if (tmpString != UID_RTPlanStorage)
        {
            DCMRT_ERROR("Invalid value for attribute SOPClassUID (0008,0016)");
            result = RT_EC_InvalidValue;
        }
    }
    return result;
}


void DRTPlanIOD::updateAttributes()
{
    SOPClassUID.putString(UID_RTPlanStorage);
}


OFCondition DRTPlanIOD::read(DcmItem &dataset)
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
        // --- FrameOfReferenceModule (U) ---
        if (dataset.tagExists(DCM_FrameOfReferenceUID) ||
            dataset.tagExists(DCM_PositionReferenceIndicator))
        {
            getAndCheckElementFromDataset(dataset, FrameOfReferenceUID, "1", "1", "FrameOfReferenceModule");
            getAndCheckElementFromDataset(dataset, PositionReferenceIndicator, "1", "2", "FrameOfReferenceModule");
        }

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

        // --- RTGeneralPlanModule (M) ---
        getAndCheckElementFromDataset(dataset, RTPlanLabel, "1", "1", "RTGeneralPlanModule");
        getAndCheckElementFromDataset(dataset, RTPlanName, "1", "3", "RTGeneralPlanModule");
        getAndCheckElementFromDataset(dataset, RTPlanDescription, "1", "3", "RTGeneralPlanModule");
        getAndCheckElementFromDataset(dataset, InstanceNumber, "1", "3", "RTGeneralPlanModule");
        getAndCheckElementFromDataset(dataset, RTPlanDate, "1", "2", "RTGeneralPlanModule");
        getAndCheckElementFromDataset(dataset, RTPlanTime, "1", "2", "RTGeneralPlanModule");
        getAndCheckElementFromDataset(dataset, TreatmentProtocols, "1-n", "3", "RTGeneralPlanModule");
        getAndCheckElementFromDataset(dataset, PlanIntent, "1", "3", "RTGeneralPlanModule");
        getAndCheckElementFromDataset(dataset, TreatmentSites, "1-n", "3", "RTGeneralPlanModule");
        getAndCheckElementFromDataset(dataset, RTPlanGeometry, "1", "1", "RTGeneralPlanModule");
        ReferencedStructureSetSequence.read(dataset, "1-n", "1C", "RTGeneralPlanModule");
        ReferencedDoseSequence.read(dataset, "1-n", "3", "RTGeneralPlanModule");
        ReferencedRTPlanSequence.read(dataset, "1-n", "3", "RTGeneralPlanModule");
        getAndCheckElementFromDataset(dataset, FrameOfReferenceToDisplayedCoordinateSystemTransformationMatrix, "16", "3", "RTGeneralPlanModule");

        // --- RTPrescriptionModule (U) ---
        getAndCheckElementFromDataset(dataset, PrescriptionDescription, "1", "3", "RTPrescriptionModule");
        DoseReferenceSequence.read(dataset, "1-n", "3", "RTPrescriptionModule");

        // --- RTToleranceTablesModule (U) ---
        ToleranceTableSequence.read(dataset, "1-n", "3", "RTToleranceTablesModule");

        // --- RTPatientSetupModule (U) ---
        if (dataset.tagExists(DCM_PatientSetupSequence))
        {
            PatientSetupSequence.read(dataset, "1-n", "1", "RTPatientSetupModule");
        }

        // --- RTFractionSchemeModule (U) ---
        if (dataset.tagExists(DCM_FractionGroupSequence))
        {
            FractionGroupSequence.read(dataset, "1-n", "1", "RTFractionSchemeModule");
        }

        // --- RTBeamsModule (C) ---
        if (dataset.tagExists(DCM_BeamSequence))
        {
            BeamSequence.read(dataset, "1-n", "1", "RTBeamsModule");
        }

        // --- RTBrachyApplicationSetupsModule (C) ---
        if (dataset.tagExists(DCM_BrachyTreatmentTechnique) ||
            dataset.tagExists(DCM_BrachyTreatmentType) ||
            dataset.tagExists(DCM_TreatmentMachineSequence) ||
            dataset.tagExists(DCM_SourceSequence) ||
            dataset.tagExists(DCM_ApplicationSetupSequence))
        {
            getAndCheckElementFromDataset(dataset, BrachyTreatmentTechnique, "1", "1", "RTBrachyApplicationSetupsModule");
            getAndCheckElementFromDataset(dataset, BrachyTreatmentType, "1", "1", "RTBrachyApplicationSetupsModule");
            TreatmentMachineSequence.read(dataset, "1-n", "1", "RTBrachyApplicationSetupsModule");
            SourceSequence.read(dataset, "1-n", "1", "RTBrachyApplicationSetupsModule");
            ApplicationSetupSequence.read(dataset, "1-n", "1", "RTBrachyApplicationSetupsModule");
        }

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


OFCondition DRTPlanIOD::readPatientData(DcmItem &dataset)
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


OFCondition DRTPlanIOD::readStudyData(DcmItem &dataset)
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


OFCondition DRTPlanIOD::readSeriesData(DcmItem &dataset)
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


OFCondition DRTPlanIOD::write(DcmItem &dataset)
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

        // --- FrameOfReferenceModule (U) ---
        if (isFrameOfReferenceModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmUniqueIdentifier(FrameOfReferenceUID), "1", "1", "FrameOfReferenceModule");
            addElementToDataset(result, dataset, new DcmLongString(PositionReferenceIndicator), "1", "2", "FrameOfReferenceModule");
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

        // --- RTGeneralPlanModule (M) ---
        addElementToDataset(result, dataset, new DcmShortString(RTPlanLabel), "1", "1", "RTGeneralPlanModule");
        addElementToDataset(result, dataset, new DcmLongString(RTPlanName), "1", "3", "RTGeneralPlanModule");
        addElementToDataset(result, dataset, new DcmShortText(RTPlanDescription), "1", "3", "RTGeneralPlanModule");
        addElementToDataset(result, dataset, new DcmIntegerString(InstanceNumber), "1", "3", "RTGeneralPlanModule");
        addElementToDataset(result, dataset, new DcmDate(RTPlanDate), "1", "2", "RTGeneralPlanModule");
        addElementToDataset(result, dataset, new DcmTime(RTPlanTime), "1", "2", "RTGeneralPlanModule");
        addElementToDataset(result, dataset, new DcmLongString(TreatmentProtocols), "1-n", "3", "RTGeneralPlanModule");
        addElementToDataset(result, dataset, new DcmCodeString(PlanIntent), "1", "3", "RTGeneralPlanModule");
        addElementToDataset(result, dataset, new DcmLongString(TreatmentSites), "1-n", "3", "RTGeneralPlanModule");
        addElementToDataset(result, dataset, new DcmCodeString(RTPlanGeometry), "1", "1", "RTGeneralPlanModule");
        if (result.good()) result = ReferencedStructureSetSequence.write(dataset, "1-n" ,"1C", "RTGeneralPlanModule");
        if (result.good()) result = ReferencedDoseSequence.write(dataset, "1-n" ,"3", "RTGeneralPlanModule");
        if (result.good()) result = ReferencedRTPlanSequence.write(dataset, "1-n" ,"3", "RTGeneralPlanModule");
        addElementToDataset(result, dataset, new DcmFloatingPointDouble(FrameOfReferenceToDisplayedCoordinateSystemTransformationMatrix), "16", "3", "RTGeneralPlanModule");

        // --- RTPrescriptionModule (U) ---
        if (isRTPrescriptionModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmShortText(PrescriptionDescription), "1", "3", "RTPrescriptionModule");
            if (result.good()) result = DoseReferenceSequence.write(dataset, "1-n" ,"3", "RTPrescriptionModule");
        }

        // --- RTToleranceTablesModule (U) ---
        if (isRTToleranceTablesModulePresent(OFFalse /*complete*/))
        {
            if (result.good()) result = ToleranceTableSequence.write(dataset, "1-n" ,"3", "RTToleranceTablesModule");
        }

        // --- RTPatientSetupModule (U) ---
        if (isRTPatientSetupModulePresent(OFFalse /*complete*/))
        {
            if (result.good()) result = PatientSetupSequence.write(dataset, "1-n" ,"1", "RTPatientSetupModule");
        }

        // --- RTFractionSchemeModule (U) ---
        if (isRTFractionSchemeModulePresent(OFFalse /*complete*/))
        {
            if (result.good()) result = FractionGroupSequence.write(dataset, "1-n" ,"1", "RTFractionSchemeModule");
        }

        // --- RTBeamsModule (C) ---
        if (isRTBeamsModulePresent(OFFalse /*complete*/))
        {
            if (result.good()) result = BeamSequence.write(dataset, "1-n" ,"1", "RTBeamsModule");
        }

        // --- RTBrachyApplicationSetupsModule (C) ---
        if (isRTBrachyApplicationSetupsModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmCodeString(BrachyTreatmentTechnique), "1", "1", "RTBrachyApplicationSetupsModule");
            addElementToDataset(result, dataset, new DcmCodeString(BrachyTreatmentType), "1", "1", "RTBrachyApplicationSetupsModule");
            if (result.good()) result = TreatmentMachineSequence.write(dataset, "1-n" ,"1", "RTBrachyApplicationSetupsModule");
            if (result.good()) result = SourceSequence.write(dataset, "1-n" ,"1", "RTBrachyApplicationSetupsModule");
            if (result.good()) result = ApplicationSetupSequence.write(dataset, "1-n" ,"1", "RTBrachyApplicationSetupsModule");
        }

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


OFBool DRTPlanIOD::isClinicalTrialSubjectModulePresent(const OFBool complete)
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


OFBool DRTPlanIOD::isPatientStudyModulePresent(const OFBool /*complete*/)
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


OFBool DRTPlanIOD::isClinicalTrialStudyModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !ClinicalTrialTimePointID.isEmpty() ||
           !ClinicalTrialTimePointDescription.isEmpty() ||
           !ConsentForClinicalTrialUseSequence.isEmpty();
}


OFBool DRTPlanIOD::isClinicalTrialSeriesModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !ClinicalTrialCoordinatingCenterName.isEmpty() ||
           !ClinicalTrialSeriesID.isEmpty() ||
           !ClinicalTrialSeriesDescription.isEmpty();
}


OFBool DRTPlanIOD::isFrameOfReferenceModulePresent(const OFBool complete)
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


OFBool DRTPlanIOD::isRTPrescriptionModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !PrescriptionDescription.isEmpty() ||
           !DoseReferenceSequence.isEmpty();
}


OFBool DRTPlanIOD::isRTToleranceTablesModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !ToleranceTableSequence.isEmpty();
}


OFBool DRTPlanIOD::isRTPatientSetupModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !PatientSetupSequence.isEmpty();
}


OFBool DRTPlanIOD::isRTFractionSchemeModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !FractionGroupSequence.isEmpty();
}


OFBool DRTPlanIOD::isRTBeamsModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !BeamSequence.isEmpty();
}


OFBool DRTPlanIOD::isRTBrachyApplicationSetupsModulePresent(const OFBool complete)
{
    if (complete)
    {
        /* check whether all mandatory attributes are present */
        return !BrachyTreatmentTechnique.isEmpty() &&
               !BrachyTreatmentType.isEmpty() &&
               !TreatmentMachineSequence.isEmpty() &&
               !SourceSequence.isEmpty() &&
               !ApplicationSetupSequence.isEmpty();
    } else {
        /* check whether at least one attribute is present */
        return !BrachyTreatmentTechnique.isEmpty() ||
               !BrachyTreatmentType.isEmpty() ||
               !TreatmentMachineSequence.isEmpty() ||
               !SourceSequence.isEmpty() ||
               !ApplicationSetupSequence.isEmpty();
    }
}


OFBool DRTPlanIOD::isApprovalModulePresent(const OFBool complete)
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


OFBool DRTPlanIOD::isGeneralReferenceModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !ReferencedImageSequence.isEmpty() ||
           !ReferencedInstanceSequence.isEmpty() ||
           !DerivationDescription.isEmpty() ||
           !DerivationCodeSequence.isEmpty() ||
           !SourceImageSequence.isEmpty() ||
           !SourceInstanceSequence.isEmpty();
}


OFBool DRTPlanIOD::isCommonInstanceReferenceModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !ReferencedSeriesSequence.isEmpty() ||
           !StudiesContainingOtherReferencedInstancesSequence.isEmpty();
}


OFCondition DRTPlanIOD::getAccessionNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AccessionNumber, value, pos);
}


OFCondition DRTPlanIOD::getAdditionalPatientHistory(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AdditionalPatientHistory, value, pos);
}


OFCondition DRTPlanIOD::getAdmissionID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AdmissionID, value, pos);
}


OFCondition DRTPlanIOD::getAdmittingDiagnosesDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AdmittingDiagnosesDescription, value, pos);
}


OFCondition DRTPlanIOD::getAllergies(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(Allergies, value, pos);
}


OFCondition DRTPlanIOD::getApprovalStatus(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ApprovalStatus, value, pos);
}


OFCondition DRTPlanIOD::getAuthorizationEquipmentCertificationNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AuthorizationEquipmentCertificationNumber, value, pos);
}


OFCondition DRTPlanIOD::getBrachyTreatmentTechnique(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(BrachyTreatmentTechnique, value, pos);
}


OFCondition DRTPlanIOD::getBrachyTreatmentType(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(BrachyTreatmentType, value, pos);
}


OFCondition DRTPlanIOD::getClinicalTrialCoordinatingCenterName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialCoordinatingCenterName, value, pos);
}


OFCondition DRTPlanIOD::getClinicalTrialProtocolEthicsCommitteeApprovalNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialProtocolEthicsCommitteeApprovalNumber, value, pos);
}


OFCondition DRTPlanIOD::getClinicalTrialProtocolEthicsCommitteeName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialProtocolEthicsCommitteeName, value, pos);
}


OFCondition DRTPlanIOD::getClinicalTrialProtocolID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialProtocolID, value, pos);
}


OFCondition DRTPlanIOD::getClinicalTrialProtocolName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialProtocolName, value, pos);
}


OFCondition DRTPlanIOD::getClinicalTrialSeriesDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSeriesDescription, value, pos);
}


OFCondition DRTPlanIOD::getClinicalTrialSeriesID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSeriesID, value, pos);
}


OFCondition DRTPlanIOD::getClinicalTrialSiteID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSiteID, value, pos);
}


OFCondition DRTPlanIOD::getClinicalTrialSiteName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSiteName, value, pos);
}


OFCondition DRTPlanIOD::getClinicalTrialSponsorName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSponsorName, value, pos);
}


OFCondition DRTPlanIOD::getClinicalTrialSubjectID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSubjectID, value, pos);
}


OFCondition DRTPlanIOD::getClinicalTrialSubjectReadingID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSubjectReadingID, value, pos);
}


OFCondition DRTPlanIOD::getClinicalTrialTimePointDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialTimePointDescription, value, pos);
}


OFCondition DRTPlanIOD::getClinicalTrialTimePointID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialTimePointID, value, pos);
}


OFCondition DRTPlanIOD::getCommentsOnThePerformedProcedureStep(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(CommentsOnThePerformedProcedureStep, value, pos);
}


OFCondition DRTPlanIOD::getConsultingPhysicianName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ConsultingPhysicianName, value, pos);
}


OFCondition DRTPlanIOD::getContentQualification(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ContentQualification, value, pos);
}


OFCondition DRTPlanIOD::getDateOfLastCalibration(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DateOfLastCalibration, value, pos);
}


OFCondition DRTPlanIOD::getDeidentificationMethod(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DeidentificationMethod, value, pos);
}


OFCondition DRTPlanIOD::getDerivationDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DerivationDescription, value, pos);
}


OFCondition DRTPlanIOD::getDeviceSerialNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DeviceSerialNumber, value, pos);
}


OFCondition DRTPlanIOD::getEthnicGroup(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(EthnicGroup, value, pos);
}


OFCondition DRTPlanIOD::getFrameOfReferenceToDisplayedCoordinateSystemTransformationMatrix(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmFloatingPointDouble &, FrameOfReferenceToDisplayedCoordinateSystemTransformationMatrix).getFloat64(value, pos);
}


OFCondition DRTPlanIOD::getFrameOfReferenceUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(FrameOfReferenceUID, value, pos);
}


OFCondition DRTPlanIOD::getGantryID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(GantryID, value, pos);
}


OFCondition DRTPlanIOD::getInstanceCoercionDateTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceCoercionDateTime, value, pos);
}


OFCondition DRTPlanIOD::getInstanceCreationDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceCreationDate, value, pos);
}


OFCondition DRTPlanIOD::getInstanceCreationTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceCreationTime, value, pos);
}


OFCondition DRTPlanIOD::getInstanceCreatorUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceCreatorUID, value, pos);
}


OFCondition DRTPlanIOD::getInstanceNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceNumber, value, pos);
}


OFCondition DRTPlanIOD::getInstanceNumber(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, InstanceNumber).getSint32(value, pos);
}


OFCondition DRTPlanIOD::getInstitutionAddress(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstitutionAddress, value, pos);
}


OFCondition DRTPlanIOD::getInstitutionName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstitutionName, value, pos);
}


OFCondition DRTPlanIOD::getInstitutionalDepartmentName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstitutionalDepartmentName, value, pos);
}


OFCondition DRTPlanIOD::getIssuerOfPatientID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(IssuerOfPatientID, value, pos);
}


OFCondition DRTPlanIOD::getLastMenstrualDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(LastMenstrualDate, value, pos);
}


OFCondition DRTPlanIOD::getLongitudinalTemporalInformationModified(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(LongitudinalTemporalInformationModified, value, pos);
}


OFCondition DRTPlanIOD::getManufacturer(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(Manufacturer, value, pos);
}


OFCondition DRTPlanIOD::getManufacturerModelName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ManufacturerModelName, value, pos);
}


OFCondition DRTPlanIOD::getMeasuredAPDimension(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(MeasuredAPDimension, value, pos);
}


OFCondition DRTPlanIOD::getMeasuredAPDimension(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, MeasuredAPDimension).getFloat64(value, pos);
}


OFCondition DRTPlanIOD::getMeasuredLateralDimension(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(MeasuredLateralDimension, value, pos);
}


OFCondition DRTPlanIOD::getMeasuredLateralDimension(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, MeasuredLateralDimension).getFloat64(value, pos);
}


OFCondition DRTPlanIOD::getMedicalAlerts(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(MedicalAlerts, value, pos);
}


OFCondition DRTPlanIOD::getModality(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(Modality, value, pos);
}


OFCondition DRTPlanIOD::getNameOfPhysiciansReadingStudy(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(NameOfPhysiciansReadingStudy, value, pos);
}


OFCondition DRTPlanIOD::getOccupation(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(Occupation, value, pos);
}


OFCondition DRTPlanIOD::getOperatorsName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OperatorsName, value, pos);
}


OFCondition DRTPlanIOD::getOriginalSpecializedSOPClassUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OriginalSpecializedSOPClassUID, value, pos);
}


OFCondition DRTPlanIOD::getOtherPatientIDs(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OtherPatientIDs, value, pos);
}


OFCondition DRTPlanIOD::getOtherPatientNames(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OtherPatientNames, value, pos);
}


OFCondition DRTPlanIOD::getPatientAge(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientAge, value, pos);
}


OFCondition DRTPlanIOD::getPatientAlternativeCalendar(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientAlternativeCalendar, value, pos);
}


OFCondition DRTPlanIOD::getPatientBirthDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientBirthDate, value, pos);
}


OFCondition DRTPlanIOD::getPatientBirthDateInAlternativeCalendar(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientBirthDateInAlternativeCalendar, value, pos);
}


OFCondition DRTPlanIOD::getPatientBirthTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientBirthTime, value, pos);
}


OFCondition DRTPlanIOD::getPatientBodyMassIndex(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientBodyMassIndex, value, pos);
}


OFCondition DRTPlanIOD::getPatientBodyMassIndex(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, PatientBodyMassIndex).getFloat64(value, pos);
}


OFCondition DRTPlanIOD::getPatientBreedDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientBreedDescription, value, pos);
}


OFCondition DRTPlanIOD::getPatientComments(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientComments, value, pos);
}


OFCondition DRTPlanIOD::getPatientDeathDateInAlternativeCalendar(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientDeathDateInAlternativeCalendar, value, pos);
}


OFCondition DRTPlanIOD::getPatientID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientID, value, pos);
}


OFCondition DRTPlanIOD::getPatientIdentityRemoved(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientIdentityRemoved, value, pos);
}


OFCondition DRTPlanIOD::getPatientName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientName, value, pos);
}


OFCondition DRTPlanIOD::getPatientSex(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientSex, value, pos);
}


OFCondition DRTPlanIOD::getPatientSexNeutered(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientSexNeutered, value, pos);
}


OFCondition DRTPlanIOD::getPatientSize(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientSize, value, pos);
}


OFCondition DRTPlanIOD::getPatientSize(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, PatientSize).getFloat64(value, pos);
}


OFCondition DRTPlanIOD::getPatientSpeciesDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientSpeciesDescription, value, pos);
}


OFCondition DRTPlanIOD::getPatientState(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientState, value, pos);
}


OFCondition DRTPlanIOD::getPatientWeight(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientWeight, value, pos);
}


OFCondition DRTPlanIOD::getPatientWeight(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, PatientWeight).getFloat64(value, pos);
}


OFCondition DRTPlanIOD::getPerformedProcedureStepDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepDescription, value, pos);
}


OFCondition DRTPlanIOD::getPerformedProcedureStepEndDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepEndDate, value, pos);
}


OFCondition DRTPlanIOD::getPerformedProcedureStepEndTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepEndTime, value, pos);
}


OFCondition DRTPlanIOD::getPerformedProcedureStepID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepID, value, pos);
}


OFCondition DRTPlanIOD::getPerformedProcedureStepStartDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepStartDate, value, pos);
}


OFCondition DRTPlanIOD::getPerformedProcedureStepStartTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepStartTime, value, pos);
}


OFCondition DRTPlanIOD::getPhysiciansOfRecord(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PhysiciansOfRecord, value, pos);
}


OFCondition DRTPlanIOD::getPixelPaddingValue(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, PixelPaddingValue).getUint16(value, pos);
}


OFCondition DRTPlanIOD::getPlanIntent(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PlanIntent, value, pos);
}


OFCondition DRTPlanIOD::getPositionReferenceIndicator(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PositionReferenceIndicator, value, pos);
}


OFCondition DRTPlanIOD::getPregnancyStatus(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, PregnancyStatus).getUint16(value, pos);
}


OFCondition DRTPlanIOD::getPrescriptionDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PrescriptionDescription, value, pos);
}


OFCondition DRTPlanIOD::getQualityControlSubject(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(QualityControlSubject, value, pos);
}


OFCondition DRTPlanIOD::getQueryRetrieveView(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(QueryRetrieveView, value, pos);
}


OFCondition DRTPlanIOD::getRTPlanDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RTPlanDate, value, pos);
}


OFCondition DRTPlanIOD::getRTPlanDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RTPlanDescription, value, pos);
}


OFCondition DRTPlanIOD::getRTPlanGeometry(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RTPlanGeometry, value, pos);
}


OFCondition DRTPlanIOD::getRTPlanLabel(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RTPlanLabel, value, pos);
}


OFCondition DRTPlanIOD::getRTPlanName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RTPlanName, value, pos);
}


OFCondition DRTPlanIOD::getRTPlanTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RTPlanTime, value, pos);
}


OFCondition DRTPlanIOD::getReferringPhysicianName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ReferringPhysicianName, value, pos);
}


OFCondition DRTPlanIOD::getRelatedGeneralSOPClassUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RelatedGeneralSOPClassUID, value, pos);
}


OFCondition DRTPlanIOD::getResponsibleOrganization(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ResponsibleOrganization, value, pos);
}


OFCondition DRTPlanIOD::getResponsiblePerson(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ResponsiblePerson, value, pos);
}


OFCondition DRTPlanIOD::getResponsiblePersonRole(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ResponsiblePersonRole, value, pos);
}


OFCondition DRTPlanIOD::getReviewDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ReviewDate, value, pos);
}


OFCondition DRTPlanIOD::getReviewTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ReviewTime, value, pos);
}


OFCondition DRTPlanIOD::getReviewerName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ReviewerName, value, pos);
}


OFCondition DRTPlanIOD::getSOPAuthorizationComment(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPAuthorizationComment, value, pos);
}


OFCondition DRTPlanIOD::getSOPAuthorizationDateTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPAuthorizationDateTime, value, pos);
}


OFCondition DRTPlanIOD::getSOPClassUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPClassUID, value, pos);
}


OFCondition DRTPlanIOD::getSOPInstanceStatus(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPInstanceStatus, value, pos);
}


OFCondition DRTPlanIOD::getSOPInstanceUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPInstanceUID, value, pos);
}


OFCondition DRTPlanIOD::getSeriesDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SeriesDate, value, pos);
}


OFCondition DRTPlanIOD::getSeriesDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SeriesDescription, value, pos);
}


OFCondition DRTPlanIOD::getSeriesInstanceUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SeriesInstanceUID, value, pos);
}


OFCondition DRTPlanIOD::getSeriesNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SeriesNumber, value, pos);
}


OFCondition DRTPlanIOD::getSeriesNumber(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, SeriesNumber).getSint32(value, pos);
}


OFCondition DRTPlanIOD::getSeriesTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SeriesTime, value, pos);
}


OFCondition DRTPlanIOD::getServiceEpisodeDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ServiceEpisodeDescription, value, pos);
}


OFCondition DRTPlanIOD::getServiceEpisodeID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ServiceEpisodeID, value, pos);
}


OFCondition DRTPlanIOD::getSmokingStatus(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SmokingStatus, value, pos);
}


OFCondition DRTPlanIOD::getSoftwareVersions(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SoftwareVersions, value, pos);
}


OFCondition DRTPlanIOD::getSpatialResolution(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SpatialResolution, value, pos);
}


OFCondition DRTPlanIOD::getSpatialResolution(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, SpatialResolution).getFloat64(value, pos);
}


OFCondition DRTPlanIOD::getSpecificCharacterSet(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SpecificCharacterSet, value, pos);
}


OFCondition DRTPlanIOD::getStationName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StationName, value, pos);
}


OFCondition DRTPlanIOD::getStrainAdditionalInformation(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StrainAdditionalInformation, value, pos);
}


OFCondition DRTPlanIOD::getStrainDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StrainDescription, value, pos);
}


OFCondition DRTPlanIOD::getStrainNomenclature(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StrainNomenclature, value, pos);
}


OFCondition DRTPlanIOD::getStudyDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyDate, value, pos);
}


OFCondition DRTPlanIOD::getStudyDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyDescription, value, pos);
}


OFCondition DRTPlanIOD::getStudyID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyID, value, pos);
}


OFCondition DRTPlanIOD::getStudyInstanceUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyInstanceUID, value, pos);
}


OFCondition DRTPlanIOD::getStudyTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyTime, value, pos);
}


OFCondition DRTPlanIOD::getTimeOfLastCalibration(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(TimeOfLastCalibration, value, pos);
}


OFCondition DRTPlanIOD::getTimezoneOffsetFromUTC(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(TimezoneOffsetFromUTC, value, pos);
}


OFCondition DRTPlanIOD::getTreatmentProtocols(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(TreatmentProtocols, value, pos);
}


OFCondition DRTPlanIOD::getTreatmentSites(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(TreatmentSites, value, pos);
}


OFCondition DRTPlanIOD::setAccessionNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AccessionNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setAdditionalPatientHistory(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = AdditionalPatientHistory.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setAdmissionID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AdmissionID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setAdmittingDiagnosesDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = AdmittingDiagnosesDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setAllergies(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = Allergies.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setApprovalStatus(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ApprovalStatus.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setAuthorizationEquipmentCertificationNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AuthorizationEquipmentCertificationNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setBrachyTreatmentTechnique(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = BrachyTreatmentTechnique.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setBrachyTreatmentType(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = BrachyTreatmentType.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setClinicalTrialCoordinatingCenterName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialCoordinatingCenterName.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setClinicalTrialProtocolEthicsCommitteeApprovalNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialProtocolEthicsCommitteeApprovalNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setClinicalTrialProtocolEthicsCommitteeName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialProtocolEthicsCommitteeName.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setClinicalTrialProtocolID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialProtocolID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setClinicalTrialProtocolName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialProtocolName.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setClinicalTrialSeriesDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSeriesDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setClinicalTrialSeriesID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSeriesID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setClinicalTrialSiteID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSiteID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setClinicalTrialSiteName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSiteName.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setClinicalTrialSponsorName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSponsorName.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setClinicalTrialSubjectID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSubjectID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setClinicalTrialSubjectReadingID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSubjectReadingID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setClinicalTrialTimePointDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = ClinicalTrialTimePointDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setClinicalTrialTimePointID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialTimePointID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setCommentsOnThePerformedProcedureStep(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = CommentsOnThePerformedProcedureStep.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setConsultingPhysicianName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = ConsultingPhysicianName.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setContentQualification(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ContentQualification.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setDateOfLastCalibration(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = DateOfLastCalibration.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setDeidentificationMethod(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = DeidentificationMethod.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setDerivationDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = DerivationDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setDeviceSerialNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = DeviceSerialNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setEthnicGroup(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = EthnicGroup.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setFrameOfReferenceToDisplayedCoordinateSystemTransformationMatrix(const Float64 value, const unsigned long pos)
{
    return FrameOfReferenceToDisplayedCoordinateSystemTransformationMatrix.putFloat64(value, pos);
}


OFCondition DRTPlanIOD::setFrameOfReferenceUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = FrameOfReferenceUID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setGantryID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = GantryID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setInstanceCoercionDateTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceCoercionDateTime.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setInstanceCreationDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceCreationDate.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setInstanceCreationTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceCreationTime.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setInstanceCreatorUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceCreatorUID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setInstanceNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setInstitutionAddress(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = InstitutionAddress.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setInstitutionName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstitutionName.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setInstitutionalDepartmentName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstitutionalDepartmentName.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setIssuerOfPatientID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = IssuerOfPatientID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setLastMenstrualDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = LastMenstrualDate.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setLongitudinalTemporalInformationModified(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = LongitudinalTemporalInformationModified.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setManufacturer(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = Manufacturer.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setManufacturerModelName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ManufacturerModelName.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setMeasuredAPDimension(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = MeasuredAPDimension.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setMeasuredLateralDimension(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = MeasuredLateralDimension.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setMedicalAlerts(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = MedicalAlerts.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setModality(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = Modality.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setNameOfPhysiciansReadingStudy(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = NameOfPhysiciansReadingStudy.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setOccupation(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = Occupation.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setOperatorsName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = OperatorsName.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setOriginalSpecializedSOPClassUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = OriginalSpecializedSOPClassUID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setOtherPatientIDs(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = OtherPatientIDs.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setOtherPatientNames(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = OtherPatientNames.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientAge(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmAgeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientAge.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientAlternativeCalendar(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientAlternativeCalendar.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientBirthDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientBirthDate.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientBirthDateInAlternativeCalendar(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientBirthDateInAlternativeCalendar.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientBirthTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientBirthTime.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientBodyMassIndex(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientBodyMassIndex.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientBreedDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientBreedDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientComments(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = PatientComments.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientDeathDateInAlternativeCalendar(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientDeathDateInAlternativeCalendar.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientIdentityRemoved(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientIdentityRemoved.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientName.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientSex(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSex.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientSexNeutered(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSexNeutered.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientSize(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSize.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientSpeciesDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSpeciesDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientState(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientState.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPatientWeight(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientWeight.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPerformedProcedureStepDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPerformedProcedureStepEndDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepEndDate.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPerformedProcedureStepEndTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepEndTime.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPerformedProcedureStepID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPerformedProcedureStepStartDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepStartDate.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPerformedProcedureStepStartTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepStartTime.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPhysiciansOfRecord(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = PhysiciansOfRecord.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPixelPaddingValue(const Uint16 value, const unsigned long pos)
{
    return PixelPaddingValue.putUint16(value, pos);
}


OFCondition DRTPlanIOD::setPlanIntent(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PlanIntent.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPositionReferenceIndicator(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PositionReferenceIndicator.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setPregnancyStatus(const Uint16 value, const unsigned long pos)
{
    return PregnancyStatus.putUint16(value, pos);
}


OFCondition DRTPlanIOD::setPrescriptionDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = PrescriptionDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setQualityControlSubject(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = QualityControlSubject.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setQueryRetrieveView(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = QueryRetrieveView.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setRTPlanDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RTPlanDate.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setRTPlanDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = RTPlanDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setRTPlanGeometry(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RTPlanGeometry.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setRTPlanLabel(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RTPlanLabel.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setRTPlanName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RTPlanName.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setRTPlanTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RTPlanTime.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setReferringPhysicianName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ReferringPhysicianName.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setRelatedGeneralSOPClassUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = RelatedGeneralSOPClassUID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setResponsibleOrganization(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ResponsibleOrganization.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setResponsiblePerson(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ResponsiblePerson.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setResponsiblePersonRole(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ResponsiblePersonRole.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setReviewDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ReviewDate.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setReviewTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ReviewTime.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setReviewerName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ReviewerName.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setSOPAuthorizationComment(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = SOPAuthorizationComment.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setSOPAuthorizationDateTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SOPAuthorizationDateTime.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setSOPClassUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SOPClassUID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setSOPInstanceStatus(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SOPInstanceStatus.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setSOPInstanceUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SOPInstanceUID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setSeriesDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesDate.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setSeriesDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setSeriesInstanceUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesInstanceUID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setSeriesNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setSeriesTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesTime.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setServiceEpisodeDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ServiceEpisodeDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setServiceEpisodeID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ServiceEpisodeID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setSmokingStatus(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SmokingStatus.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setSoftwareVersions(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = SoftwareVersions.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setSpatialResolution(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SpatialResolution.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setSpecificCharacterSet(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = SpecificCharacterSet.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setStationName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StationName.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setStrainAdditionalInformation(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUnlimitedText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = StrainAdditionalInformation.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setStrainDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUnlimitedCharacters::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StrainDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setStrainNomenclature(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StrainNomenclature.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setStudyDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyDate.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setStudyDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setStudyID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setStudyInstanceUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyInstanceUID.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setStudyTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyTime.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setTimeOfLastCalibration(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = TimeOfLastCalibration.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setTimezoneOffsetFromUTC(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = TimezoneOffsetFromUTC.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setTreatmentProtocols(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = TreatmentProtocols.putOFStringArray(value);
    return result;
}


OFCondition DRTPlanIOD::setTreatmentSites(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = TreatmentSites.putOFStringArray(value);
    return result;
}


// end of source file
