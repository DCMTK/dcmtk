/*
 *
 *  Copyright (c) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTDoseIOD
 *
 *  Generated automatically from DICOM PS 3.3-2007
 *  File created on 2012-02-01 16:51:15 
 *
 */


#include "dcmtk/config/osconfig.h"      // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/drtdose.h"


DRTDoseIOD::DRTDoseIOD()
  : PatientName(DCM_PatientName),
    PatientID(DCM_PatientID),
    IssuerOfPatientID(DCM_IssuerOfPatientID),
    PatientBirthDate(DCM_PatientBirthDate),
    PatientSex(DCM_PatientSex),
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
    ResponsiblePerson(DCM_ResponsiblePerson),
    ResponsiblePersonRole(DCM_ResponsiblePersonRole),
    ResponsibleOrganization(DCM_ResponsibleOrganization),
    PatientIdentityRemoved(DCM_PatientIdentityRemoved),
    DeidentificationMethod(DCM_DeidentificationMethod),
    DeidentificationMethodCodeSequence(),
    ClinicalTrialSponsorName(DCM_ClinicalTrialSponsorName),
    ClinicalTrialProtocolID(DCM_ClinicalTrialProtocolID),
    ClinicalTrialProtocolName(DCM_ClinicalTrialProtocolName),
    ClinicalTrialSiteID(DCM_ClinicalTrialSiteID),
    ClinicalTrialSiteName(DCM_ClinicalTrialSiteName),
    ClinicalTrialSubjectID(DCM_ClinicalTrialSubjectID),
    ClinicalTrialSubjectReadingID(DCM_ClinicalTrialSubjectReadingID),
    StudyInstanceUID(DCM_StudyInstanceUID),
    StudyDate(DCM_StudyDate),
    StudyTime(DCM_StudyTime),
    ReferringPhysicianName(DCM_ReferringPhysicianName),
    ReferringPhysicianIdentificationSequence(),
    StudyID(DCM_StudyID),
    AccessionNumber(DCM_AccessionNumber),
    StudyDescription(DCM_StudyDescription),
    PhysiciansOfRecord(DCM_PhysiciansOfRecord),
    PhysiciansOfRecordIdentificationSequence(),
    NameOfPhysiciansReadingStudy(DCM_NameOfPhysiciansReadingStudy),
    PhysiciansReadingStudyIdentificationSequence(),
    ReferencedStudySequence(),
    ProcedureCodeSequence(),
    AdmittingDiagnosesDescription(DCM_AdmittingDiagnosesDescription),
    AdmittingDiagnosesCodeSequence(),
    PatientAge(DCM_PatientAge),
    PatientSize(DCM_PatientSize),
    PatientWeight(DCM_PatientWeight),
    Occupation(DCM_Occupation),
    AdditionalPatientHistory(DCM_AdditionalPatientHistory),
    AdmissionID(DCM_AdmissionID),
    IssuerOfAdmissionID(DCM_RETIRED_IssuerOfAdmissionID),
    PatientSexNeutered(DCM_PatientSexNeutered),
    ClinicalTrialTimePointID(DCM_ClinicalTrialTimePointID),
    ClinicalTrialTimePointDescription(DCM_ClinicalTrialTimePointDescription),
    Modality(DCM_Modality),
    SeriesInstanceUID(DCM_SeriesInstanceUID),
    SeriesNumber(DCM_SeriesNumber),
    SeriesDescription(DCM_SeriesDescription),
    ReferencedPerformedProcedureStepSequence(),
    RequestAttributesSequence(),
    PerformedProcedureStepID(DCM_PerformedProcedureStepID),
    PerformedProcedureStepStartDate(DCM_PerformedProcedureStepStartDate),
    PerformedProcedureStepStartTime(DCM_PerformedProcedureStepStartTime),
    PerformedProcedureStepDescription(DCM_PerformedProcedureStepDescription),
    PerformedProtocolCodeSequence(),
    ClinicalTrialCoordinatingCenterName(DCM_ClinicalTrialCoordinatingCenterName),
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
    SpatialResolution(DCM_SpatialResolution),
    DateOfLastCalibration(DCM_DateOfLastCalibration),
    TimeOfLastCalibration(DCM_TimeOfLastCalibration),
    PixelPaddingValue(DCM_PixelPaddingValue),
    PatientOrientation(DCM_PatientOrientation),
    ContentDate(DCM_ContentDate),
    ContentTime(DCM_ContentTime),
    ImageType(DCM_ImageType),
    AcquisitionNumber(DCM_AcquisitionNumber),
    AcquisitionDate(DCM_AcquisitionDate),
    AcquisitionTime(DCM_AcquisitionTime),
    AcquisitionDateTime(DCM_AcquisitionDateTime),
    ReferencedImageSequence(),
    DerivationDescription(DCM_DerivationDescription),
    DerivationCodeSequence(),
    SourceImageSequence(),
    ReferencedInstanceSequence(),
    ImagesInAcquisition(DCM_ImagesInAcquisition),
    ImageComments(DCM_ImageComments),
    QualityControlImage(DCM_QualityControlImage),
    BurnedInAnnotation(DCM_BurnedInAnnotation),
    LossyImageCompression(DCM_LossyImageCompression),
    LossyImageCompressionRatio(DCM_LossyImageCompressionRatio),
    LossyImageCompressionMethod(DCM_LossyImageCompressionMethod),
    IconImageSequence(),
    PresentationLUTShape(DCM_PresentationLUTShape),
    IrradiationEventUID(DCM_IrradiationEventUID),
    PixelSpacing(DCM_PixelSpacing),
    ImageOrientationPatient(DCM_ImageOrientationPatient),
    ImagePositionPatient(DCM_ImagePositionPatient),
    SliceThickness(DCM_SliceThickness),
    SliceLocation(DCM_SliceLocation),
    Rows(DCM_Rows),
    Columns(DCM_Columns),
    PixelData(DCM_PixelData),
    PlanarConfiguration(DCM_PlanarConfiguration),
    PixelAspectRatio(DCM_PixelAspectRatio),
    SmallestImagePixelValue(DCM_SmallestImagePixelValue),
    LargestImagePixelValue(DCM_LargestImagePixelValue),
    RedPaletteColorLookupTableDescriptor(DCM_RedPaletteColorLookupTableDescriptor),
    GreenPaletteColorLookupTableDescriptor(DCM_GreenPaletteColorLookupTableDescriptor),
    BluePaletteColorLookupTableDescriptor(DCM_BluePaletteColorLookupTableDescriptor),
    RedPaletteColorLookupTableData(DCM_RedPaletteColorLookupTableData),
    GreenPaletteColorLookupTableData(DCM_GreenPaletteColorLookupTableData),
    BluePaletteColorLookupTableData(DCM_BluePaletteColorLookupTableData),
    ICCProfile(DCM_ICCProfile),
    PixelDataProviderURL(DCM_PixelDataProviderURL),
    NumberOfFrames(DCM_NumberOfFrames),
    FrameIncrementPointer(DCM_FrameIncrementPointer),
    OverlayRows(DCM_OverlayRows),
    OverlayColumns(DCM_OverlayColumns),
    OverlayType(DCM_OverlayType),
    OverlayOrigin(DCM_OverlayOrigin),
    OverlayBitsAllocated(DCM_OverlayBitsAllocated),
    OverlayBitPosition(DCM_OverlayBitPosition),
    OverlayData(DCM_OverlayData),
    OverlayDescription(DCM_OverlayDescription),
    OverlaySubtype(DCM_OverlaySubtype),
    OverlayLabel(DCM_OverlayLabel),
    ROIArea(DCM_ROIArea),
    ROIMean(DCM_ROIMean),
    ROIStandardDeviation(DCM_ROIStandardDeviation),
    NumberOfFramesInOverlay(DCM_NumberOfFramesInOverlay),
    ImageFrameOrigin(DCM_ImageFrameOrigin),
    ModalityLUTSequence(),
    RescaleIntercept(DCM_RescaleIntercept),
    RescaleSlope(DCM_RescaleSlope),
    RescaleType(DCM_RescaleType),
    SamplesPerPixel(DCM_SamplesPerPixel),
    PhotometricInterpretation(DCM_PhotometricInterpretation),
    BitsAllocated(DCM_BitsAllocated),
    BitsStored(DCM_BitsStored),
    HighBit(DCM_HighBit),
    PixelRepresentation(DCM_PixelRepresentation),
    DoseUnits(DCM_DoseUnits),
    DoseType(DCM_DoseType),
    InstanceNumber(DCM_InstanceNumber),
    DoseComment(DCM_DoseComment),
    NormalizationPoint(DCM_NormalizationPoint),
    DoseSummationType(DCM_DoseSummationType),
    ReferencedRTPlanSequence(),
    GridFrameOffsetVector(DCM_GridFrameOffsetVector),
    DoseGridScaling(DCM_DoseGridScaling),
    TissueHeterogeneityCorrection(DCM_TissueHeterogeneityCorrection),
    ReferencedStructureSetSequence(),
    DVHNormalizationPoint(DCM_DVHNormalizationPoint),
    DVHNormalizationDoseValue(DCM_DVHNormalizationDoseValue),
    DVHSequence(),
    StructureSetLabel(DCM_StructureSetLabel),
    StructureSetName(DCM_StructureSetName),
    StructureSetDescription(DCM_StructureSetDescription),
    StructureSetDate(DCM_StructureSetDate),
    StructureSetTime(DCM_StructureSetTime),
    ReferencedFrameOfReferenceSequence(),
    StructureSetROISequence(),
    ROIContourSequence(),
    RTDoseROISequence(),
    SOPClassUID(DCM_SOPClassUID),
    SOPInstanceUID(DCM_SOPInstanceUID),
    SpecificCharacterSet(DCM_SpecificCharacterSet),
    InstanceCreationDate(DCM_InstanceCreationDate),
    InstanceCreationTime(DCM_InstanceCreationTime),
    InstanceCreatorUID(DCM_InstanceCreatorUID),
    RelatedGeneralSOPClassUID(DCM_RelatedGeneralSOPClassUID),
    OriginalSpecializedSOPClassUID(DCM_OriginalSpecializedSOPClassUID),
    CodingSchemeIdentificationSequence(),
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
    HL7StructuredDocumentReferenceSequence()
{
    /* set initial values for a new SOP instance */
    updateAttributes();
}


DRTDoseIOD::DRTDoseIOD(const DRTDoseIOD &copy)
  : PatientName(copy.PatientName),
    PatientID(copy.PatientID),
    IssuerOfPatientID(copy.IssuerOfPatientID),
    PatientBirthDate(copy.PatientBirthDate),
    PatientSex(copy.PatientSex),
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
    ResponsiblePerson(copy.ResponsiblePerson),
    ResponsiblePersonRole(copy.ResponsiblePersonRole),
    ResponsibleOrganization(copy.ResponsibleOrganization),
    PatientIdentityRemoved(copy.PatientIdentityRemoved),
    DeidentificationMethod(copy.DeidentificationMethod),
    DeidentificationMethodCodeSequence(copy.DeidentificationMethodCodeSequence),
    ClinicalTrialSponsorName(copy.ClinicalTrialSponsorName),
    ClinicalTrialProtocolID(copy.ClinicalTrialProtocolID),
    ClinicalTrialProtocolName(copy.ClinicalTrialProtocolName),
    ClinicalTrialSiteID(copy.ClinicalTrialSiteID),
    ClinicalTrialSiteName(copy.ClinicalTrialSiteName),
    ClinicalTrialSubjectID(copy.ClinicalTrialSubjectID),
    ClinicalTrialSubjectReadingID(copy.ClinicalTrialSubjectReadingID),
    StudyInstanceUID(copy.StudyInstanceUID),
    StudyDate(copy.StudyDate),
    StudyTime(copy.StudyTime),
    ReferringPhysicianName(copy.ReferringPhysicianName),
    ReferringPhysicianIdentificationSequence(copy.ReferringPhysicianIdentificationSequence),
    StudyID(copy.StudyID),
    AccessionNumber(copy.AccessionNumber),
    StudyDescription(copy.StudyDescription),
    PhysiciansOfRecord(copy.PhysiciansOfRecord),
    PhysiciansOfRecordIdentificationSequence(copy.PhysiciansOfRecordIdentificationSequence),
    NameOfPhysiciansReadingStudy(copy.NameOfPhysiciansReadingStudy),
    PhysiciansReadingStudyIdentificationSequence(copy.PhysiciansReadingStudyIdentificationSequence),
    ReferencedStudySequence(copy.ReferencedStudySequence),
    ProcedureCodeSequence(copy.ProcedureCodeSequence),
    AdmittingDiagnosesDescription(copy.AdmittingDiagnosesDescription),
    AdmittingDiagnosesCodeSequence(copy.AdmittingDiagnosesCodeSequence),
    PatientAge(copy.PatientAge),
    PatientSize(copy.PatientSize),
    PatientWeight(copy.PatientWeight),
    Occupation(copy.Occupation),
    AdditionalPatientHistory(copy.AdditionalPatientHistory),
    AdmissionID(copy.AdmissionID),
    IssuerOfAdmissionID(copy.IssuerOfAdmissionID),
    PatientSexNeutered(copy.PatientSexNeutered),
    ClinicalTrialTimePointID(copy.ClinicalTrialTimePointID),
    ClinicalTrialTimePointDescription(copy.ClinicalTrialTimePointDescription),
    Modality(copy.Modality),
    SeriesInstanceUID(copy.SeriesInstanceUID),
    SeriesNumber(copy.SeriesNumber),
    SeriesDescription(copy.SeriesDescription),
    ReferencedPerformedProcedureStepSequence(copy.ReferencedPerformedProcedureStepSequence),
    RequestAttributesSequence(copy.RequestAttributesSequence),
    PerformedProcedureStepID(copy.PerformedProcedureStepID),
    PerformedProcedureStepStartDate(copy.PerformedProcedureStepStartDate),
    PerformedProcedureStepStartTime(copy.PerformedProcedureStepStartTime),
    PerformedProcedureStepDescription(copy.PerformedProcedureStepDescription),
    PerformedProtocolCodeSequence(copy.PerformedProtocolCodeSequence),
    ClinicalTrialCoordinatingCenterName(copy.ClinicalTrialCoordinatingCenterName),
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
    SpatialResolution(copy.SpatialResolution),
    DateOfLastCalibration(copy.DateOfLastCalibration),
    TimeOfLastCalibration(copy.TimeOfLastCalibration),
    PixelPaddingValue(copy.PixelPaddingValue),
    PatientOrientation(copy.PatientOrientation),
    ContentDate(copy.ContentDate),
    ContentTime(copy.ContentTime),
    ImageType(copy.ImageType),
    AcquisitionNumber(copy.AcquisitionNumber),
    AcquisitionDate(copy.AcquisitionDate),
    AcquisitionTime(copy.AcquisitionTime),
    AcquisitionDateTime(copy.AcquisitionDateTime),
    ReferencedImageSequence(copy.ReferencedImageSequence),
    DerivationDescription(copy.DerivationDescription),
    DerivationCodeSequence(copy.DerivationCodeSequence),
    SourceImageSequence(copy.SourceImageSequence),
    ReferencedInstanceSequence(copy.ReferencedInstanceSequence),
    ImagesInAcquisition(copy.ImagesInAcquisition),
    ImageComments(copy.ImageComments),
    QualityControlImage(copy.QualityControlImage),
    BurnedInAnnotation(copy.BurnedInAnnotation),
    LossyImageCompression(copy.LossyImageCompression),
    LossyImageCompressionRatio(copy.LossyImageCompressionRatio),
    LossyImageCompressionMethod(copy.LossyImageCompressionMethod),
    IconImageSequence(copy.IconImageSequence),
    PresentationLUTShape(copy.PresentationLUTShape),
    IrradiationEventUID(copy.IrradiationEventUID),
    PixelSpacing(copy.PixelSpacing),
    ImageOrientationPatient(copy.ImageOrientationPatient),
    ImagePositionPatient(copy.ImagePositionPatient),
    SliceThickness(copy.SliceThickness),
    SliceLocation(copy.SliceLocation),
    Rows(copy.Rows),
    Columns(copy.Columns),
    PixelData(copy.PixelData),
    PlanarConfiguration(copy.PlanarConfiguration),
    PixelAspectRatio(copy.PixelAspectRatio),
    SmallestImagePixelValue(copy.SmallestImagePixelValue),
    LargestImagePixelValue(copy.LargestImagePixelValue),
    RedPaletteColorLookupTableDescriptor(copy.RedPaletteColorLookupTableDescriptor),
    GreenPaletteColorLookupTableDescriptor(copy.GreenPaletteColorLookupTableDescriptor),
    BluePaletteColorLookupTableDescriptor(copy.BluePaletteColorLookupTableDescriptor),
    RedPaletteColorLookupTableData(copy.RedPaletteColorLookupTableData),
    GreenPaletteColorLookupTableData(copy.GreenPaletteColorLookupTableData),
    BluePaletteColorLookupTableData(copy.BluePaletteColorLookupTableData),
    ICCProfile(copy.ICCProfile),
    PixelDataProviderURL(copy.PixelDataProviderURL),
    NumberOfFrames(copy.NumberOfFrames),
    FrameIncrementPointer(copy.FrameIncrementPointer),
    OverlayRows(copy.OverlayRows),
    OverlayColumns(copy.OverlayColumns),
    OverlayType(copy.OverlayType),
    OverlayOrigin(copy.OverlayOrigin),
    OverlayBitsAllocated(copy.OverlayBitsAllocated),
    OverlayBitPosition(copy.OverlayBitPosition),
    OverlayData(copy.OverlayData),
    OverlayDescription(copy.OverlayDescription),
    OverlaySubtype(copy.OverlaySubtype),
    OverlayLabel(copy.OverlayLabel),
    ROIArea(copy.ROIArea),
    ROIMean(copy.ROIMean),
    ROIStandardDeviation(copy.ROIStandardDeviation),
    NumberOfFramesInOverlay(copy.NumberOfFramesInOverlay),
    ImageFrameOrigin(copy.ImageFrameOrigin),
    ModalityLUTSequence(copy.ModalityLUTSequence),
    RescaleIntercept(copy.RescaleIntercept),
    RescaleSlope(copy.RescaleSlope),
    RescaleType(copy.RescaleType),
    SamplesPerPixel(copy.SamplesPerPixel),
    PhotometricInterpretation(copy.PhotometricInterpretation),
    BitsAllocated(copy.BitsAllocated),
    BitsStored(copy.BitsStored),
    HighBit(copy.HighBit),
    PixelRepresentation(copy.PixelRepresentation),
    DoseUnits(copy.DoseUnits),
    DoseType(copy.DoseType),
    InstanceNumber(copy.InstanceNumber),
    DoseComment(copy.DoseComment),
    NormalizationPoint(copy.NormalizationPoint),
    DoseSummationType(copy.DoseSummationType),
    ReferencedRTPlanSequence(copy.ReferencedRTPlanSequence),
    GridFrameOffsetVector(copy.GridFrameOffsetVector),
    DoseGridScaling(copy.DoseGridScaling),
    TissueHeterogeneityCorrection(copy.TissueHeterogeneityCorrection),
    ReferencedStructureSetSequence(copy.ReferencedStructureSetSequence),
    DVHNormalizationPoint(copy.DVHNormalizationPoint),
    DVHNormalizationDoseValue(copy.DVHNormalizationDoseValue),
    DVHSequence(copy.DVHSequence),
    StructureSetLabel(copy.StructureSetLabel),
    StructureSetName(copy.StructureSetName),
    StructureSetDescription(copy.StructureSetDescription),
    StructureSetDate(copy.StructureSetDate),
    StructureSetTime(copy.StructureSetTime),
    ReferencedFrameOfReferenceSequence(copy.ReferencedFrameOfReferenceSequence),
    StructureSetROISequence(copy.StructureSetROISequence),
    ROIContourSequence(copy.ROIContourSequence),
    RTDoseROISequence(copy.RTDoseROISequence),
    SOPClassUID(copy.SOPClassUID),
    SOPInstanceUID(copy.SOPInstanceUID),
    SpecificCharacterSet(copy.SpecificCharacterSet),
    InstanceCreationDate(copy.InstanceCreationDate),
    InstanceCreationTime(copy.InstanceCreationTime),
    InstanceCreatorUID(copy.InstanceCreatorUID),
    RelatedGeneralSOPClassUID(copy.RelatedGeneralSOPClassUID),
    OriginalSpecializedSOPClassUID(copy.OriginalSpecializedSOPClassUID),
    CodingSchemeIdentificationSequence(copy.CodingSchemeIdentificationSequence),
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
    HL7StructuredDocumentReferenceSequence(copy.HL7StructuredDocumentReferenceSequence)
{
}


DRTDoseIOD::~DRTDoseIOD()
{
}


DRTDoseIOD &DRTDoseIOD::operator=(const DRTDoseIOD &copy)
{
    if (this != &copy)
    {
        PatientName = copy.PatientName;
        PatientID = copy.PatientID;
        IssuerOfPatientID = copy.IssuerOfPatientID;
        PatientBirthDate = copy.PatientBirthDate;
        PatientSex = copy.PatientSex;
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
        ResponsiblePerson = copy.ResponsiblePerson;
        ResponsiblePersonRole = copy.ResponsiblePersonRole;
        ResponsibleOrganization = copy.ResponsibleOrganization;
        PatientIdentityRemoved = copy.PatientIdentityRemoved;
        DeidentificationMethod = copy.DeidentificationMethod;
        DeidentificationMethodCodeSequence = copy.DeidentificationMethodCodeSequence;
        ClinicalTrialSponsorName = copy.ClinicalTrialSponsorName;
        ClinicalTrialProtocolID = copy.ClinicalTrialProtocolID;
        ClinicalTrialProtocolName = copy.ClinicalTrialProtocolName;
        ClinicalTrialSiteID = copy.ClinicalTrialSiteID;
        ClinicalTrialSiteName = copy.ClinicalTrialSiteName;
        ClinicalTrialSubjectID = copy.ClinicalTrialSubjectID;
        ClinicalTrialSubjectReadingID = copy.ClinicalTrialSubjectReadingID;
        StudyInstanceUID = copy.StudyInstanceUID;
        StudyDate = copy.StudyDate;
        StudyTime = copy.StudyTime;
        ReferringPhysicianName = copy.ReferringPhysicianName;
        ReferringPhysicianIdentificationSequence = copy.ReferringPhysicianIdentificationSequence;
        StudyID = copy.StudyID;
        AccessionNumber = copy.AccessionNumber;
        StudyDescription = copy.StudyDescription;
        PhysiciansOfRecord = copy.PhysiciansOfRecord;
        PhysiciansOfRecordIdentificationSequence = copy.PhysiciansOfRecordIdentificationSequence;
        NameOfPhysiciansReadingStudy = copy.NameOfPhysiciansReadingStudy;
        PhysiciansReadingStudyIdentificationSequence = copy.PhysiciansReadingStudyIdentificationSequence;
        ReferencedStudySequence = copy.ReferencedStudySequence;
        ProcedureCodeSequence = copy.ProcedureCodeSequence;
        AdmittingDiagnosesDescription = copy.AdmittingDiagnosesDescription;
        AdmittingDiagnosesCodeSequence = copy.AdmittingDiagnosesCodeSequence;
        PatientAge = copy.PatientAge;
        PatientSize = copy.PatientSize;
        PatientWeight = copy.PatientWeight;
        Occupation = copy.Occupation;
        AdditionalPatientHistory = copy.AdditionalPatientHistory;
        AdmissionID = copy.AdmissionID;
        IssuerOfAdmissionID = copy.IssuerOfAdmissionID;
        PatientSexNeutered = copy.PatientSexNeutered;
        ClinicalTrialTimePointID = copy.ClinicalTrialTimePointID;
        ClinicalTrialTimePointDescription = copy.ClinicalTrialTimePointDescription;
        Modality = copy.Modality;
        SeriesInstanceUID = copy.SeriesInstanceUID;
        SeriesNumber = copy.SeriesNumber;
        SeriesDescription = copy.SeriesDescription;
        ReferencedPerformedProcedureStepSequence = copy.ReferencedPerformedProcedureStepSequence;
        RequestAttributesSequence = copy.RequestAttributesSequence;
        PerformedProcedureStepID = copy.PerformedProcedureStepID;
        PerformedProcedureStepStartDate = copy.PerformedProcedureStepStartDate;
        PerformedProcedureStepStartTime = copy.PerformedProcedureStepStartTime;
        PerformedProcedureStepDescription = copy.PerformedProcedureStepDescription;
        PerformedProtocolCodeSequence = copy.PerformedProtocolCodeSequence;
        ClinicalTrialCoordinatingCenterName = copy.ClinicalTrialCoordinatingCenterName;
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
        SpatialResolution = copy.SpatialResolution;
        DateOfLastCalibration = copy.DateOfLastCalibration;
        TimeOfLastCalibration = copy.TimeOfLastCalibration;
        PixelPaddingValue = copy.PixelPaddingValue;
        PatientOrientation = copy.PatientOrientation;
        ContentDate = copy.ContentDate;
        ContentTime = copy.ContentTime;
        ImageType = copy.ImageType;
        AcquisitionNumber = copy.AcquisitionNumber;
        AcquisitionDate = copy.AcquisitionDate;
        AcquisitionTime = copy.AcquisitionTime;
        AcquisitionDateTime = copy.AcquisitionDateTime;
        ReferencedImageSequence = copy.ReferencedImageSequence;
        DerivationDescription = copy.DerivationDescription;
        DerivationCodeSequence = copy.DerivationCodeSequence;
        SourceImageSequence = copy.SourceImageSequence;
        ReferencedInstanceSequence = copy.ReferencedInstanceSequence;
        ImagesInAcquisition = copy.ImagesInAcquisition;
        ImageComments = copy.ImageComments;
        QualityControlImage = copy.QualityControlImage;
        BurnedInAnnotation = copy.BurnedInAnnotation;
        LossyImageCompression = copy.LossyImageCompression;
        LossyImageCompressionRatio = copy.LossyImageCompressionRatio;
        LossyImageCompressionMethod = copy.LossyImageCompressionMethod;
        IconImageSequence = copy.IconImageSequence;
        PresentationLUTShape = copy.PresentationLUTShape;
        IrradiationEventUID = copy.IrradiationEventUID;
        PixelSpacing = copy.PixelSpacing;
        ImageOrientationPatient = copy.ImageOrientationPatient;
        ImagePositionPatient = copy.ImagePositionPatient;
        SliceThickness = copy.SliceThickness;
        SliceLocation = copy.SliceLocation;
        Rows = copy.Rows;
        Columns = copy.Columns;
        PixelData = copy.PixelData;
        PlanarConfiguration = copy.PlanarConfiguration;
        PixelAspectRatio = copy.PixelAspectRatio;
        SmallestImagePixelValue = copy.SmallestImagePixelValue;
        LargestImagePixelValue = copy.LargestImagePixelValue;
        RedPaletteColorLookupTableDescriptor = copy.RedPaletteColorLookupTableDescriptor;
        GreenPaletteColorLookupTableDescriptor = copy.GreenPaletteColorLookupTableDescriptor;
        BluePaletteColorLookupTableDescriptor = copy.BluePaletteColorLookupTableDescriptor;
        RedPaletteColorLookupTableData = copy.RedPaletteColorLookupTableData;
        GreenPaletteColorLookupTableData = copy.GreenPaletteColorLookupTableData;
        BluePaletteColorLookupTableData = copy.BluePaletteColorLookupTableData;
        ICCProfile = copy.ICCProfile;
        PixelDataProviderURL = copy.PixelDataProviderURL;
        NumberOfFrames = copy.NumberOfFrames;
        FrameIncrementPointer = copy.FrameIncrementPointer;
        OverlayRows = copy.OverlayRows;
        OverlayColumns = copy.OverlayColumns;
        OverlayType = copy.OverlayType;
        OverlayOrigin = copy.OverlayOrigin;
        OverlayBitsAllocated = copy.OverlayBitsAllocated;
        OverlayBitPosition = copy.OverlayBitPosition;
        OverlayData = copy.OverlayData;
        OverlayDescription = copy.OverlayDescription;
        OverlaySubtype = copy.OverlaySubtype;
        OverlayLabel = copy.OverlayLabel;
        ROIArea = copy.ROIArea;
        ROIMean = copy.ROIMean;
        ROIStandardDeviation = copy.ROIStandardDeviation;
        NumberOfFramesInOverlay = copy.NumberOfFramesInOverlay;
        ImageFrameOrigin = copy.ImageFrameOrigin;
        ModalityLUTSequence = copy.ModalityLUTSequence;
        RescaleIntercept = copy.RescaleIntercept;
        RescaleSlope = copy.RescaleSlope;
        RescaleType = copy.RescaleType;
        SamplesPerPixel = copy.SamplesPerPixel;
        PhotometricInterpretation = copy.PhotometricInterpretation;
        BitsAllocated = copy.BitsAllocated;
        BitsStored = copy.BitsStored;
        HighBit = copy.HighBit;
        PixelRepresentation = copy.PixelRepresentation;
        DoseUnits = copy.DoseUnits;
        DoseType = copy.DoseType;
        InstanceNumber = copy.InstanceNumber;
        DoseComment = copy.DoseComment;
        NormalizationPoint = copy.NormalizationPoint;
        DoseSummationType = copy.DoseSummationType;
        ReferencedRTPlanSequence = copy.ReferencedRTPlanSequence;
        GridFrameOffsetVector = copy.GridFrameOffsetVector;
        DoseGridScaling = copy.DoseGridScaling;
        TissueHeterogeneityCorrection = copy.TissueHeterogeneityCorrection;
        ReferencedStructureSetSequence = copy.ReferencedStructureSetSequence;
        DVHNormalizationPoint = copy.DVHNormalizationPoint;
        DVHNormalizationDoseValue = copy.DVHNormalizationDoseValue;
        DVHSequence = copy.DVHSequence;
        StructureSetLabel = copy.StructureSetLabel;
        StructureSetName = copy.StructureSetName;
        StructureSetDescription = copy.StructureSetDescription;
        StructureSetDate = copy.StructureSetDate;
        StructureSetTime = copy.StructureSetTime;
        ReferencedFrameOfReferenceSequence = copy.ReferencedFrameOfReferenceSequence;
        StructureSetROISequence = copy.StructureSetROISequence;
        ROIContourSequence = copy.ROIContourSequence;
        RTDoseROISequence = copy.RTDoseROISequence;
        SOPClassUID = copy.SOPClassUID;
        SOPInstanceUID = copy.SOPInstanceUID;
        SpecificCharacterSet = copy.SpecificCharacterSet;
        InstanceCreationDate = copy.InstanceCreationDate;
        InstanceCreationTime = copy.InstanceCreationTime;
        InstanceCreatorUID = copy.InstanceCreatorUID;
        RelatedGeneralSOPClassUID = copy.RelatedGeneralSOPClassUID;
        OriginalSpecializedSOPClassUID = copy.OriginalSpecializedSOPClassUID;
        CodingSchemeIdentificationSequence = copy.CodingSchemeIdentificationSequence;
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
    }
    return *this;
}


void DRTDoseIOD::clear()
{
    /* clear all DICOM attributes */
    PatientName.clear();
    PatientID.clear();
    IssuerOfPatientID.clear();
    PatientBirthDate.clear();
    PatientSex.clear();
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
    ResponsiblePerson.clear();
    ResponsiblePersonRole.clear();
    ResponsibleOrganization.clear();
    PatientIdentityRemoved.clear();
    DeidentificationMethod.clear();
    DeidentificationMethodCodeSequence.clear();
    ClinicalTrialSponsorName.clear();
    ClinicalTrialProtocolID.clear();
    ClinicalTrialProtocolName.clear();
    ClinicalTrialSiteID.clear();
    ClinicalTrialSiteName.clear();
    ClinicalTrialSubjectID.clear();
    ClinicalTrialSubjectReadingID.clear();
    StudyInstanceUID.clear();
    StudyDate.clear();
    StudyTime.clear();
    ReferringPhysicianName.clear();
    ReferringPhysicianIdentificationSequence.clear();
    StudyID.clear();
    AccessionNumber.clear();
    StudyDescription.clear();
    PhysiciansOfRecord.clear();
    PhysiciansOfRecordIdentificationSequence.clear();
    NameOfPhysiciansReadingStudy.clear();
    PhysiciansReadingStudyIdentificationSequence.clear();
    ReferencedStudySequence.clear();
    ProcedureCodeSequence.clear();
    AdmittingDiagnosesDescription.clear();
    AdmittingDiagnosesCodeSequence.clear();
    PatientAge.clear();
    PatientSize.clear();
    PatientWeight.clear();
    Occupation.clear();
    AdditionalPatientHistory.clear();
    AdmissionID.clear();
    IssuerOfAdmissionID.clear();
    PatientSexNeutered.clear();
    ClinicalTrialTimePointID.clear();
    ClinicalTrialTimePointDescription.clear();
    Modality.clear();
    SeriesInstanceUID.clear();
    SeriesNumber.clear();
    SeriesDescription.clear();
    ReferencedPerformedProcedureStepSequence.clear();
    RequestAttributesSequence.clear();
    PerformedProcedureStepID.clear();
    PerformedProcedureStepStartDate.clear();
    PerformedProcedureStepStartTime.clear();
    PerformedProcedureStepDescription.clear();
    PerformedProtocolCodeSequence.clear();
    ClinicalTrialCoordinatingCenterName.clear();
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
    SpatialResolution.clear();
    DateOfLastCalibration.clear();
    TimeOfLastCalibration.clear();
    PixelPaddingValue.clear();
    InstanceNumber.clear();
    PatientOrientation.clear();
    ContentDate.clear();
    ContentTime.clear();
    ImageType.clear();
    AcquisitionNumber.clear();
    AcquisitionDate.clear();
    AcquisitionTime.clear();
    AcquisitionDateTime.clear();
    ReferencedImageSequence.clear();
    DerivationDescription.clear();
    DerivationCodeSequence.clear();
    SourceImageSequence.clear();
    ReferencedInstanceSequence.clear();
    ImagesInAcquisition.clear();
    ImageComments.clear();
    QualityControlImage.clear();
    BurnedInAnnotation.clear();
    LossyImageCompression.clear();
    LossyImageCompressionRatio.clear();
    LossyImageCompressionMethod.clear();
    IconImageSequence.clear();
    PresentationLUTShape.clear();
    IrradiationEventUID.clear();
    PixelSpacing.clear();
    ImageOrientationPatient.clear();
    ImagePositionPatient.clear();
    SliceThickness.clear();
    SliceLocation.clear();
    SamplesPerPixel.clear();
    PhotometricInterpretation.clear();
    Rows.clear();
    Columns.clear();
    BitsAllocated.clear();
    BitsStored.clear();
    HighBit.clear();
    PixelRepresentation.clear();
    PixelData.clear();
    PlanarConfiguration.clear();
    PixelAspectRatio.clear();
    SmallestImagePixelValue.clear();
    LargestImagePixelValue.clear();
    RedPaletteColorLookupTableDescriptor.clear();
    GreenPaletteColorLookupTableDescriptor.clear();
    BluePaletteColorLookupTableDescriptor.clear();
    RedPaletteColorLookupTableData.clear();
    GreenPaletteColorLookupTableData.clear();
    BluePaletteColorLookupTableData.clear();
    ICCProfile.clear();
    PixelDataProviderURL.clear();
    NumberOfFrames.clear();
    FrameIncrementPointer.clear();
    OverlayRows.clear();
    OverlayColumns.clear();
    OverlayType.clear();
    OverlayOrigin.clear();
    OverlayBitsAllocated.clear();
    OverlayBitPosition.clear();
    OverlayData.clear();
    OverlayDescription.clear();
    OverlaySubtype.clear();
    OverlayLabel.clear();
    ROIArea.clear();
    ROIMean.clear();
    ROIStandardDeviation.clear();
    NumberOfFramesInOverlay.clear();
    ImageFrameOrigin.clear();
    ModalityLUTSequence.clear();
    RescaleIntercept.clear();
    RescaleSlope.clear();
    RescaleType.clear();
    DoseUnits.clear();
    DoseType.clear();
    DoseComment.clear();
    NormalizationPoint.clear();
    DoseSummationType.clear();
    ReferencedRTPlanSequence.clear();
    GridFrameOffsetVector.clear();
    DoseGridScaling.clear();
    TissueHeterogeneityCorrection.clear();
    ReferencedStructureSetSequence.clear();
    DVHNormalizationPoint.clear();
    DVHNormalizationDoseValue.clear();
    DVHSequence.clear();
    StructureSetLabel.clear();
    StructureSetName.clear();
    StructureSetDescription.clear();
    StructureSetDate.clear();
    StructureSetTime.clear();
    ReferencedFrameOfReferenceSequence.clear();
    StructureSetROISequence.clear();
    ROIContourSequence.clear();
    RTDoseROISequence.clear();
    SOPClassUID.clear();
    SOPInstanceUID.clear();
    SpecificCharacterSet.clear();
    InstanceCreationDate.clear();
    InstanceCreationTime.clear();
    InstanceCreatorUID.clear();
    RelatedGeneralSOPClassUID.clear();
    OriginalSpecializedSOPClassUID.clear();
    CodingSchemeIdentificationSequence.clear();
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
}


OFBool DRTDoseIOD::isValid()
{
    /* tbd: check whether object is valid */
    return OFTrue;
}


OFCondition DRTDoseIOD::checkDatasetForReading(DcmItem &dataset)
{
    OFCondition result = EC_Normal;
    /* check SOP class UID */
    DcmUniqueIdentifier sopClassUID(DCM_SOPClassUID);
    result = getAndCheckElementFromDataset(dataset, sopClassUID, "1", "1", "SOPCommonModule");
    if (result.good())
    {
        OFString tmpString;
        getStringValueFromElement(sopClassUID, tmpString);
        if (tmpString != UID_RTDoseStorage)
        {
            DCMRT_ERROR("Invalid value for attribute SOPClassUID (0008,0016)");
            result = RT_EC_InvalidValue;
        }
    }
    return result;
}


void DRTDoseIOD::updateAttributes()
{
    SOPClassUID.putString(UID_RTDoseStorage);
}


OFCondition DRTDoseIOD::read(DcmItem &dataset)
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
        // --- FrameOfReferenceModule (M) ---
        getAndCheckElementFromDataset(dataset, FrameOfReferenceUID, "1", "1", "FrameOfReferenceModule");
        getAndCheckElementFromDataset(dataset, PositionReferenceIndicator, "1", "2", "FrameOfReferenceModule");

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
        getAndCheckElementFromDataset(dataset, SpatialResolution, "1", "3", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, DateOfLastCalibration, "1-n", "3", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, TimeOfLastCalibration, "1-n", "3", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, PixelPaddingValue, "1", "3", "GeneralEquipmentModule");

        // --- GeneralImageModule (C) ---
        // getAndCheckElementFromDataset(dataset, InstanceNumber, "1", "2", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, PatientOrientation, "2", "2C", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, ContentDate, "1", "2C", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, ContentTime, "1", "2C", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, ImageType, "2-n", "3", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, AcquisitionNumber, "1", "3", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, AcquisitionDate, "1", "3", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, AcquisitionTime, "1", "3", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, AcquisitionDateTime, "1", "3", "GeneralImageModule");
        ReferencedImageSequence.read(dataset, "1-n", "3", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, DerivationDescription, "1", "3", "GeneralImageModule");
        DerivationCodeSequence.read(dataset, "1-n", "3", "GeneralImageModule");
        SourceImageSequence.read(dataset, "1-n", "3", "GeneralImageModule");
        ReferencedInstanceSequence.read(dataset, "1-n", "3", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, ImagesInAcquisition, "1", "3", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, ImageComments, "1", "3", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, QualityControlImage, "1", "3", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, BurnedInAnnotation, "1", "3", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, LossyImageCompression, "1", "3", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, LossyImageCompressionRatio, "1-n", "3", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, LossyImageCompressionMethod, "1-n", "3", "GeneralImageModule");
        IconImageSequence.read(dataset, "1-n", "3", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, PresentationLUTShape, "1", "3", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, IrradiationEventUID, "1", "3", "GeneralImageModule");

        // --- ImagePlaneModule (C) ---
        if (dataset.tagExists(DCM_PixelSpacing) ||
            dataset.tagExists(DCM_ImageOrientationPatient) ||
            dataset.tagExists(DCM_ImagePositionPatient) ||
            dataset.tagExists(DCM_SliceThickness))
        {
            getAndCheckElementFromDataset(dataset, PixelSpacing, "2", "1", "ImagePlaneModule");
            getAndCheckElementFromDataset(dataset, ImageOrientationPatient, "6", "1", "ImagePlaneModule");
            getAndCheckElementFromDataset(dataset, ImagePositionPatient, "3", "1", "ImagePlaneModule");
            getAndCheckElementFromDataset(dataset, SliceThickness, "1", "2", "ImagePlaneModule");
            getAndCheckElementFromDataset(dataset, SliceLocation, "1", "3", "ImagePlaneModule");
        }

        // --- ImagePixelModule (C) ---
        if (dataset.tagExists(DCM_Rows) ||
            dataset.tagExists(DCM_Columns))
        {
            // getAndCheckElementFromDataset(dataset, SamplesPerPixel, "1", "1", "ImagePixelModule");
            // getAndCheckElementFromDataset(dataset, PhotometricInterpretation, "1", "1", "ImagePixelModule");
            getAndCheckElementFromDataset(dataset, Rows, "1", "1", "ImagePixelModule");
            getAndCheckElementFromDataset(dataset, Columns, "1", "1", "ImagePixelModule");
            // getAndCheckElementFromDataset(dataset, BitsAllocated, "1", "1", "ImagePixelModule");
            // getAndCheckElementFromDataset(dataset, BitsStored, "1", "1", "ImagePixelModule");
            // getAndCheckElementFromDataset(dataset, HighBit, "1", "1", "ImagePixelModule");
            // getAndCheckElementFromDataset(dataset, PixelRepresentation, "1", "1", "ImagePixelModule");
            getAndCheckElementFromDataset(dataset, PixelData, "1", "1C", "ImagePixelModule");
            getAndCheckElementFromDataset(dataset, PlanarConfiguration, "1", "1C", "ImagePixelModule");
            getAndCheckElementFromDataset(dataset, PixelAspectRatio, "2", "1C", "ImagePixelModule");
            getAndCheckElementFromDataset(dataset, SmallestImagePixelValue, "1", "3", "ImagePixelModule");
            getAndCheckElementFromDataset(dataset, LargestImagePixelValue, "1", "3", "ImagePixelModule");
            getAndCheckElementFromDataset(dataset, RedPaletteColorLookupTableDescriptor, "3", "1C", "ImagePixelModule");
            getAndCheckElementFromDataset(dataset, GreenPaletteColorLookupTableDescriptor, "3", "1C", "ImagePixelModule");
            getAndCheckElementFromDataset(dataset, BluePaletteColorLookupTableDescriptor, "3", "1C", "ImagePixelModule");
            getAndCheckElementFromDataset(dataset, RedPaletteColorLookupTableData, "1", "1C", "ImagePixelModule");
            getAndCheckElementFromDataset(dataset, GreenPaletteColorLookupTableData, "1", "1C", "ImagePixelModule");
            getAndCheckElementFromDataset(dataset, BluePaletteColorLookupTableData, "1", "1C", "ImagePixelModule");
            getAndCheckElementFromDataset(dataset, ICCProfile, "1", "3", "ImagePixelModule");
            getAndCheckElementFromDataset(dataset, PixelDataProviderURL, "1", "1C", "ImagePixelModule");
        }

        // --- MultiFrameModule (C) ---
        if (dataset.tagExists(DCM_NumberOfFrames) ||
            dataset.tagExists(DCM_FrameIncrementPointer))
        {
            getAndCheckElementFromDataset(dataset, NumberOfFrames, "1", "1", "MultiFrameModule");
            getAndCheckElementFromDataset(dataset, FrameIncrementPointer, "1-n", "1", "MultiFrameModule");
        }

        // --- OverlayPlaneModule (U) ---
        if (dataset.tagExists(DCM_OverlayRows) ||
            dataset.tagExists(DCM_OverlayColumns) ||
            dataset.tagExists(DCM_OverlayType) ||
            dataset.tagExists(DCM_OverlayOrigin) ||
            dataset.tagExists(DCM_OverlayBitsAllocated) ||
            dataset.tagExists(DCM_OverlayBitPosition) ||
            dataset.tagExists(DCM_OverlayData))
        {
            getAndCheckElementFromDataset(dataset, OverlayRows, "1", "1", "OverlayPlaneModule");
            getAndCheckElementFromDataset(dataset, OverlayColumns, "1", "1", "OverlayPlaneModule");
            getAndCheckElementFromDataset(dataset, OverlayType, "1", "1", "OverlayPlaneModule");
            getAndCheckElementFromDataset(dataset, OverlayOrigin, "2", "1", "OverlayPlaneModule");
            getAndCheckElementFromDataset(dataset, OverlayBitsAllocated, "1", "1", "OverlayPlaneModule");
            getAndCheckElementFromDataset(dataset, OverlayBitPosition, "1", "1", "OverlayPlaneModule");
            getAndCheckElementFromDataset(dataset, OverlayData, "1", "1", "OverlayPlaneModule");
            getAndCheckElementFromDataset(dataset, OverlayDescription, "1", "3", "OverlayPlaneModule");
            getAndCheckElementFromDataset(dataset, OverlaySubtype, "1", "3", "OverlayPlaneModule");
            getAndCheckElementFromDataset(dataset, OverlayLabel, "1", "3", "OverlayPlaneModule");
            getAndCheckElementFromDataset(dataset, ROIArea, "1", "3", "OverlayPlaneModule");
            getAndCheckElementFromDataset(dataset, ROIMean, "1", "3", "OverlayPlaneModule");
            getAndCheckElementFromDataset(dataset, ROIStandardDeviation, "1", "3", "OverlayPlaneModule");
        }

        // --- MultiFrameOverlayModule (U) ---
        if (dataset.tagExists(DCM_NumberOfFramesInOverlay))
        {
            getAndCheckElementFromDataset(dataset, NumberOfFramesInOverlay, "1", "1", "MultiFrameOverlayModule");
            getAndCheckElementFromDataset(dataset, ImageFrameOrigin, "1", "3", "MultiFrameOverlayModule");
        }

        // --- ModalityLUTModule (U) ---
        ModalityLUTSequence.read(dataset, "1-n", "1C", "ModalityLUTModule");
        getAndCheckElementFromDataset(dataset, RescaleIntercept, "1", "1C", "ModalityLUTModule");
        getAndCheckElementFromDataset(dataset, RescaleSlope, "1", "1C", "ModalityLUTModule");
        getAndCheckElementFromDataset(dataset, RescaleType, "1", "1C", "ModalityLUTModule");

        // --- RTDoseModule (M) ---
        getAndCheckElementFromDataset(dataset, SamplesPerPixel, "1", "1C", "RTDoseModule");
        getAndCheckElementFromDataset(dataset, PhotometricInterpretation, "1", "1C", "RTDoseModule");
        getAndCheckElementFromDataset(dataset, BitsAllocated, "1", "1C", "RTDoseModule");
        getAndCheckElementFromDataset(dataset, BitsStored, "1", "1C", "RTDoseModule");
        getAndCheckElementFromDataset(dataset, HighBit, "1", "1C", "RTDoseModule");
        getAndCheckElementFromDataset(dataset, PixelRepresentation, "1", "1C", "RTDoseModule");
        getAndCheckElementFromDataset(dataset, DoseUnits, "1", "1", "RTDoseModule");
        getAndCheckElementFromDataset(dataset, DoseType, "1", "1", "RTDoseModule");
        getAndCheckElementFromDataset(dataset, InstanceNumber, "1", "3", "RTDoseModule");
        getAndCheckElementFromDataset(dataset, DoseComment, "1", "3", "RTDoseModule");
        getAndCheckElementFromDataset(dataset, NormalizationPoint, "3", "3", "RTDoseModule");
        getAndCheckElementFromDataset(dataset, DoseSummationType, "1", "1", "RTDoseModule");
        ReferencedRTPlanSequence.read(dataset, "1-n", "1C", "RTDoseModule");
        getAndCheckElementFromDataset(dataset, GridFrameOffsetVector, "2-n", "1C", "RTDoseModule");
        getAndCheckElementFromDataset(dataset, DoseGridScaling, "1", "1", "RTDoseModule");
        getAndCheckElementFromDataset(dataset, TissueHeterogeneityCorrection, "1-3", "3", "RTDoseModule");

        // --- RTDVHModule (U) ---
        if (dataset.tagExists(DCM_ReferencedStructureSetSequence) ||
            dataset.tagExists(DCM_DVHSequence))
        {
            ReferencedStructureSetSequence.read(dataset, "1-n", "1", "RTDVHModule");
            getAndCheckElementFromDataset(dataset, DVHNormalizationPoint, "3", "3", "RTDVHModule");
            getAndCheckElementFromDataset(dataset, DVHNormalizationDoseValue, "1", "3", "RTDVHModule");
            DVHSequence.read(dataset, "1-n", "1", "RTDVHModule");
        }

        // --- StructureSetModule (C) ---
        if (dataset.tagExists(DCM_StructureSetLabel) ||
            dataset.tagExists(DCM_StructureSetDate) ||
            dataset.tagExists(DCM_StructureSetTime))
        {
            getAndCheckElementFromDataset(dataset, StructureSetLabel, "1", "1", "StructureSetModule");
            getAndCheckElementFromDataset(dataset, StructureSetName, "1", "3", "StructureSetModule");
            getAndCheckElementFromDataset(dataset, StructureSetDescription, "1", "3", "StructureSetModule");
            // getAndCheckElementFromDataset(dataset, InstanceNumber, "1", "3", "StructureSetModule");
            getAndCheckElementFromDataset(dataset, StructureSetDate, "1", "2", "StructureSetModule");
            getAndCheckElementFromDataset(dataset, StructureSetTime, "1", "2", "StructureSetModule");
            ReferencedFrameOfReferenceSequence.read(dataset, "1-n", "3", "StructureSetModule");
            StructureSetROISequence.read(dataset, "1-n", "3", "StructureSetModule");
        }

        // --- ROIContourModule (C) ---
        if (dataset.tagExists(DCM_ROIContourSequence))
        {
            ROIContourSequence.read(dataset, "1-n", "1", "ROIContourModule");
        }

        // --- RTDoseROIModule (C) ---
        if (dataset.tagExists(DCM_RTDoseROISequence))
        {
            RTDoseROISequence.read(dataset, "1-n", "1", "RTDoseROIModule");
        }

        // --- SOPCommonModule (M) ---
        getAndCheckElementFromDataset(dataset, SOPClassUID, "1", "1", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, SOPInstanceUID, "1", "1", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, SpecificCharacterSet, "1-n", "1C", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, InstanceCreationDate, "1", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, InstanceCreationTime, "1", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, InstanceCreatorUID, "1", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, RelatedGeneralSOPClassUID, "1-n", "3", "SOPCommonModule");
        getAndCheckElementFromDataset(dataset, OriginalSpecializedSOPClassUID, "1", "3", "SOPCommonModule");
        CodingSchemeIdentificationSequence.read(dataset, "1-n", "3", "SOPCommonModule");
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
    }
    return result;
}


OFCondition DRTDoseIOD::readPatientData(DcmItem &dataset)
{
    OFCondition result = EC_Normal;

    // --- PatientModule (M) ---
    getAndCheckElementFromDataset(dataset, PatientName, "1", "2", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientID, "1", "2", "PatientModule");
    getAndCheckElementFromDataset(dataset, IssuerOfPatientID, "1", "3", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientBirthDate, "1", "2", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientSex, "1", "2", "PatientModule");
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
    getAndCheckElementFromDataset(dataset, ResponsiblePerson, "1", "2C", "PatientModule");
    getAndCheckElementFromDataset(dataset, ResponsiblePersonRole, "1", "1C", "PatientModule");
    getAndCheckElementFromDataset(dataset, ResponsibleOrganization, "1", "2C", "PatientModule");
    getAndCheckElementFromDataset(dataset, PatientIdentityRemoved, "1", "3", "PatientModule");
    getAndCheckElementFromDataset(dataset, DeidentificationMethod, "1-n", "1C", "PatientModule");
    DeidentificationMethodCodeSequence.read(dataset, "1-n", "1C", "PatientModule");

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
    }
    return result;
}


OFCondition DRTDoseIOD::readStudyData(DcmItem &dataset)
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
        getAndCheckElementFromDataset(dataset, StudyID, "1", "2", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, AccessionNumber, "1", "2", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, StudyDescription, "1", "3", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, PhysiciansOfRecord, "1-n", "3", "GeneralStudyModule");
        PhysiciansOfRecordIdentificationSequence.read(dataset, "1-n", "3", "GeneralStudyModule");
        getAndCheckElementFromDataset(dataset, NameOfPhysiciansReadingStudy, "1-n", "3", "GeneralStudyModule");
        PhysiciansReadingStudyIdentificationSequence.read(dataset, "1-n", "3", "GeneralStudyModule");
        ReferencedStudySequence.read(dataset, "1-n", "3", "GeneralStudyModule");
        ProcedureCodeSequence.read(dataset, "1-n", "3", "GeneralStudyModule");

        // --- PatientStudyModule (U) ---
        getAndCheckElementFromDataset(dataset, AdmittingDiagnosesDescription, "1-n", "3", "PatientStudyModule");
        AdmittingDiagnosesCodeSequence.read(dataset, "1-n", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, PatientAge, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, PatientSize, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, PatientWeight, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, Occupation, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, AdditionalPatientHistory, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, AdmissionID, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, IssuerOfAdmissionID, "1", "3", "PatientStudyModule");
        getAndCheckElementFromDataset(dataset, PatientSexNeutered, "1", "2C", "PatientStudyModule");

        // --- ClinicalTrialStudyModule (U) ---
        if (dataset.tagExists(DCM_ClinicalTrialTimePointID))
        {
            getAndCheckElementFromDataset(dataset, ClinicalTrialTimePointID, "1", "2", "ClinicalTrialStudyModule");
            getAndCheckElementFromDataset(dataset, ClinicalTrialTimePointDescription, "1", "3", "ClinicalTrialStudyModule");
        }
    }
    return result;
}


OFCondition DRTDoseIOD::readSeriesData(DcmItem &dataset)
{
    /* read data from PatientIE, StudyIE */
    OFCondition result = readStudyData(dataset);
    if (result.good())
    {
        // --- RTSeriesModule (M) ---
        getAndCheckElementFromDataset(dataset, Modality, "1", "1", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, SeriesInstanceUID, "1", "1", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, SeriesNumber, "1", "2", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, SeriesDescription, "1", "3", "RTSeriesModule");
        ReferencedPerformedProcedureStepSequence.read(dataset, "1-n", "3", "RTSeriesModule");
        RequestAttributesSequence.read(dataset, "1-n", "3", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, PerformedProcedureStepID, "1", "3", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, PerformedProcedureStepStartDate, "1", "3", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, PerformedProcedureStepStartTime, "1", "3", "RTSeriesModule");
        getAndCheckElementFromDataset(dataset, PerformedProcedureStepDescription, "1", "3", "RTSeriesModule");
        PerformedProtocolCodeSequence.read(dataset, "1-n", "3", "RTSeriesModule");

        // --- ClinicalTrialSeriesModule (U) ---
        if (dataset.tagExists(DCM_ClinicalTrialCoordinatingCenterName))
        {
            getAndCheckElementFromDataset(dataset, ClinicalTrialCoordinatingCenterName, "1", "2", "ClinicalTrialSeriesModule");
        }
    }
    return result;
}


OFCondition DRTDoseIOD::write(DcmItem &dataset)
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
        addElementToDataset(result, dataset, new DcmDate(PatientBirthDate), "1", "2", "PatientModule");
        addElementToDataset(result, dataset, new DcmCodeString(PatientSex), "1", "2", "PatientModule");
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
        addElementToDataset(result, dataset, new DcmPersonName(ResponsiblePerson), "1", "2C", "PatientModule");
        addElementToDataset(result, dataset, new DcmCodeString(ResponsiblePersonRole), "1", "1C", "PatientModule");
        addElementToDataset(result, dataset, new DcmLongString(ResponsibleOrganization), "1", "2C", "PatientModule");
        addElementToDataset(result, dataset, new DcmCodeString(PatientIdentityRemoved), "1", "3", "PatientModule");
        addElementToDataset(result, dataset, new DcmLongString(DeidentificationMethod), "1-n", "1C", "PatientModule");
        if (result.good()) result = DeidentificationMethodCodeSequence.write(dataset, "1-n" ,"1C", "PatientModule");

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
        }

        // --- GeneralStudyModule (M) ---
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(StudyInstanceUID), "1", "1", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmDate(StudyDate), "1", "2", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmTime(StudyTime), "1", "2", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmPersonName(ReferringPhysicianName), "1", "2", "GeneralStudyModule");
        if (result.good()) result = ReferringPhysicianIdentificationSequence.write(dataset, "1-n" ,"3", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmShortString(StudyID), "1", "2", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmShortString(AccessionNumber), "1", "2", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmLongString(StudyDescription), "1", "3", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmPersonName(PhysiciansOfRecord), "1-n", "3", "GeneralStudyModule");
        if (result.good()) result = PhysiciansOfRecordIdentificationSequence.write(dataset, "1-n" ,"3", "GeneralStudyModule");
        addElementToDataset(result, dataset, new DcmPersonName(NameOfPhysiciansReadingStudy), "1-n", "3", "GeneralStudyModule");
        if (result.good()) result = PhysiciansReadingStudyIdentificationSequence.write(dataset, "1-n" ,"3", "GeneralStudyModule");
        if (result.good()) result = ReferencedStudySequence.write(dataset, "1-n" ,"3", "GeneralStudyModule");
        if (result.good()) result = ProcedureCodeSequence.write(dataset, "1-n" ,"3", "GeneralStudyModule");

        // --- PatientStudyModule (U) ---
        if (isPatientStudyModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmLongString(AdmittingDiagnosesDescription), "1-n", "3", "PatientStudyModule");
            if (result.good()) result = AdmittingDiagnosesCodeSequence.write(dataset, "1-n" ,"3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmAgeString(PatientAge), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmDecimalString(PatientSize), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmDecimalString(PatientWeight), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmShortString(Occupation), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmLongText(AdditionalPatientHistory), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmLongString(AdmissionID), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmLongString(IssuerOfAdmissionID), "1", "3", "PatientStudyModule");
            addElementToDataset(result, dataset, new DcmCodeString(PatientSexNeutered), "1", "2C", "PatientStudyModule");
        }

        // --- ClinicalTrialStudyModule (U) ---
        if (isClinicalTrialStudyModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmLongString(ClinicalTrialTimePointID), "1", "2", "ClinicalTrialStudyModule");
            addElementToDataset(result, dataset, new DcmShortText(ClinicalTrialTimePointDescription), "1", "3", "ClinicalTrialStudyModule");
        }

        // --- RTSeriesModule (M) ---
        addElementToDataset(result, dataset, new DcmCodeString(Modality), "1", "1", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(SeriesInstanceUID), "1", "1", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmIntegerString(SeriesNumber), "1", "2", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmLongString(SeriesDescription), "1", "3", "RTSeriesModule");
        if (result.good()) result = ReferencedPerformedProcedureStepSequence.write(dataset, "1-n" ,"3", "RTSeriesModule");
        if (result.good()) result = RequestAttributesSequence.write(dataset, "1-n" ,"3", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmShortString(PerformedProcedureStepID), "1", "3", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmDate(PerformedProcedureStepStartDate), "1", "3", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmTime(PerformedProcedureStepStartTime), "1", "3", "RTSeriesModule");
        addElementToDataset(result, dataset, new DcmLongString(PerformedProcedureStepDescription), "1", "3", "RTSeriesModule");
        if (result.good()) result = PerformedProtocolCodeSequence.write(dataset, "1-n" ,"3", "RTSeriesModule");

        // --- ClinicalTrialSeriesModule (U) ---
        if (isClinicalTrialSeriesModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmLongString(ClinicalTrialCoordinatingCenterName), "1", "2", "ClinicalTrialSeriesModule");
        }

        // --- FrameOfReferenceModule (M) ---
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(FrameOfReferenceUID), "1", "1", "FrameOfReferenceModule");
        addElementToDataset(result, dataset, new DcmLongString(PositionReferenceIndicator), "1", "2", "FrameOfReferenceModule");

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
        addElementToDataset(result, dataset, new DcmDecimalString(SpatialResolution), "1", "3", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmDate(DateOfLastCalibration), "1-n", "3", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmTime(TimeOfLastCalibration), "1-n", "3", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmUnsignedShort(PixelPaddingValue), "1", "3", "GeneralEquipmentModule");

        // --- GeneralImageModule (C) ---
        if (isGeneralImageModulePresent(OFFalse /*complete*/))
        {
            // addElementToDataset(result, dataset, new DcmIntegerString(InstanceNumber), "1", "2", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmCodeString(PatientOrientation), "2", "2C", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmDate(ContentDate), "1", "2C", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmTime(ContentTime), "1", "2C", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmCodeString(ImageType), "2-n", "3", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmIntegerString(AcquisitionNumber), "1", "3", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmDate(AcquisitionDate), "1", "3", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmTime(AcquisitionTime), "1", "3", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmDateTime(AcquisitionDateTime), "1", "3", "GeneralImageModule");
            if (result.good()) result = ReferencedImageSequence.write(dataset, "1-n" ,"3", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmShortText(DerivationDescription), "1", "3", "GeneralImageModule");
            if (result.good()) result = DerivationCodeSequence.write(dataset, "1-n" ,"3", "GeneralImageModule");
            if (result.good()) result = SourceImageSequence.write(dataset, "1-n" ,"3", "GeneralImageModule");
            if (result.good()) result = ReferencedInstanceSequence.write(dataset, "1-n" ,"3", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmIntegerString(ImagesInAcquisition), "1", "3", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmLongText(ImageComments), "1", "3", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmCodeString(QualityControlImage), "1", "3", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmCodeString(BurnedInAnnotation), "1", "3", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmCodeString(LossyImageCompression), "1", "3", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmDecimalString(LossyImageCompressionRatio), "1-n", "3", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmCodeString(LossyImageCompressionMethod), "1-n", "3", "GeneralImageModule");
            if (result.good()) result = IconImageSequence.write(dataset, "1-n" ,"3", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmCodeString(PresentationLUTShape), "1", "3", "GeneralImageModule");
            addElementToDataset(result, dataset, new DcmUniqueIdentifier(IrradiationEventUID), "1", "3", "GeneralImageModule");
        }

        // --- ImagePlaneModule (C) ---
        if (isImagePlaneModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmDecimalString(PixelSpacing), "2", "1", "ImagePlaneModule");
            addElementToDataset(result, dataset, new DcmDecimalString(ImageOrientationPatient), "6", "1", "ImagePlaneModule");
            addElementToDataset(result, dataset, new DcmDecimalString(ImagePositionPatient), "3", "1", "ImagePlaneModule");
            addElementToDataset(result, dataset, new DcmDecimalString(SliceThickness), "1", "2", "ImagePlaneModule");
            addElementToDataset(result, dataset, new DcmDecimalString(SliceLocation), "1", "3", "ImagePlaneModule");
        }

        // --- ImagePixelModule (C) ---
        if (isImagePixelModulePresent(OFFalse /*complete*/))
        {
            // addElementToDataset(result, dataset, new DcmUnsignedShort(SamplesPerPixel), "1", "1", "ImagePixelModule");
            // addElementToDataset(result, dataset, new DcmCodeString(PhotometricInterpretation), "1", "1", "ImagePixelModule");
            addElementToDataset(result, dataset, new DcmUnsignedShort(Rows), "1", "1", "ImagePixelModule");
            addElementToDataset(result, dataset, new DcmUnsignedShort(Columns), "1", "1", "ImagePixelModule");
            // addElementToDataset(result, dataset, new DcmUnsignedShort(BitsAllocated), "1", "1", "ImagePixelModule");
            // addElementToDataset(result, dataset, new DcmUnsignedShort(BitsStored), "1", "1", "ImagePixelModule");
            // addElementToDataset(result, dataset, new DcmUnsignedShort(HighBit), "1", "1", "ImagePixelModule");
            // addElementToDataset(result, dataset, new DcmUnsignedShort(PixelRepresentation), "1", "1", "ImagePixelModule");
            addElementToDataset(result, dataset, new DcmPixelData(PixelData), "1", "1C", "ImagePixelModule");
            addElementToDataset(result, dataset, new DcmUnsignedShort(PlanarConfiguration), "1", "1C", "ImagePixelModule");
            addElementToDataset(result, dataset, new DcmIntegerString(PixelAspectRatio), "2", "1C", "ImagePixelModule");
            addElementToDataset(result, dataset, new DcmUnsignedShort(SmallestImagePixelValue), "1", "3", "ImagePixelModule");
            addElementToDataset(result, dataset, new DcmUnsignedShort(LargestImagePixelValue), "1", "3", "ImagePixelModule");
            addElementToDataset(result, dataset, new DcmUnsignedShort(RedPaletteColorLookupTableDescriptor), "3", "1C", "ImagePixelModule");
            addElementToDataset(result, dataset, new DcmUnsignedShort(GreenPaletteColorLookupTableDescriptor), "3", "1C", "ImagePixelModule");
            addElementToDataset(result, dataset, new DcmUnsignedShort(BluePaletteColorLookupTableDescriptor), "3", "1C", "ImagePixelModule");
            addElementToDataset(result, dataset, new DcmOtherByteOtherWord(RedPaletteColorLookupTableData), "1", "1C", "ImagePixelModule");
            addElementToDataset(result, dataset, new DcmOtherByteOtherWord(GreenPaletteColorLookupTableData), "1", "1C", "ImagePixelModule");
            addElementToDataset(result, dataset, new DcmOtherByteOtherWord(BluePaletteColorLookupTableData), "1", "1C", "ImagePixelModule");
            addElementToDataset(result, dataset, new DcmOtherByteOtherWord(ICCProfile), "1", "3", "ImagePixelModule");
            addElementToDataset(result, dataset, new DcmUnlimitedText(PixelDataProviderURL), "1", "1C", "ImagePixelModule");
        }

        // --- MultiFrameModule (C) ---
        if (isMultiFrameModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmIntegerString(NumberOfFrames), "1", "1", "MultiFrameModule");
            addElementToDataset(result, dataset, new DcmAttributeTag(FrameIncrementPointer), "1-n", "1", "MultiFrameModule");
        }

        // --- OverlayPlaneModule (U) ---
        if (isOverlayPlaneModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmUnsignedShort(OverlayRows), "1", "1", "OverlayPlaneModule");
            addElementToDataset(result, dataset, new DcmUnsignedShort(OverlayColumns), "1", "1", "OverlayPlaneModule");
            addElementToDataset(result, dataset, new DcmCodeString(OverlayType), "1", "1", "OverlayPlaneModule");
            addElementToDataset(result, dataset, new DcmSignedShort(OverlayOrigin), "2", "1", "OverlayPlaneModule");
            addElementToDataset(result, dataset, new DcmUnsignedShort(OverlayBitsAllocated), "1", "1", "OverlayPlaneModule");
            addElementToDataset(result, dataset, new DcmUnsignedShort(OverlayBitPosition), "1", "1", "OverlayPlaneModule");
            addElementToDataset(result, dataset, new DcmOtherByteOtherWord(OverlayData), "1", "1", "OverlayPlaneModule");
            addElementToDataset(result, dataset, new DcmLongString(OverlayDescription), "1", "3", "OverlayPlaneModule");
            addElementToDataset(result, dataset, new DcmLongString(OverlaySubtype), "1", "3", "OverlayPlaneModule");
            addElementToDataset(result, dataset, new DcmLongString(OverlayLabel), "1", "3", "OverlayPlaneModule");
            addElementToDataset(result, dataset, new DcmIntegerString(ROIArea), "1", "3", "OverlayPlaneModule");
            addElementToDataset(result, dataset, new DcmDecimalString(ROIMean), "1", "3", "OverlayPlaneModule");
            addElementToDataset(result, dataset, new DcmDecimalString(ROIStandardDeviation), "1", "3", "OverlayPlaneModule");
        }

        // --- MultiFrameOverlayModule (U) ---
        if (isMultiFrameOverlayModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmIntegerString(NumberOfFramesInOverlay), "1", "1", "MultiFrameOverlayModule");
            addElementToDataset(result, dataset, new DcmUnsignedShort(ImageFrameOrigin), "1", "3", "MultiFrameOverlayModule");
        }

        // --- ModalityLUTModule (U) ---
        if (isModalityLUTModulePresent(OFFalse /*complete*/))
        {
            if (result.good()) result = ModalityLUTSequence.write(dataset, "1-n" ,"1C", "ModalityLUTModule");
            addElementToDataset(result, dataset, new DcmDecimalString(RescaleIntercept), "1", "1C", "ModalityLUTModule");
            addElementToDataset(result, dataset, new DcmDecimalString(RescaleSlope), "1", "1C", "ModalityLUTModule");
            addElementToDataset(result, dataset, new DcmLongString(RescaleType), "1", "1C", "ModalityLUTModule");
        }

        // --- RTDoseModule (M) ---
        addElementToDataset(result, dataset, new DcmUnsignedShort(SamplesPerPixel), "1", "1C", "RTDoseModule");
        addElementToDataset(result, dataset, new DcmCodeString(PhotometricInterpretation), "1", "1C", "RTDoseModule");
        addElementToDataset(result, dataset, new DcmUnsignedShort(BitsAllocated), "1", "1C", "RTDoseModule");
        addElementToDataset(result, dataset, new DcmUnsignedShort(BitsStored), "1", "1C", "RTDoseModule");
        addElementToDataset(result, dataset, new DcmUnsignedShort(HighBit), "1", "1C", "RTDoseModule");
        addElementToDataset(result, dataset, new DcmUnsignedShort(PixelRepresentation), "1", "1C", "RTDoseModule");
        addElementToDataset(result, dataset, new DcmCodeString(DoseUnits), "1", "1", "RTDoseModule");
        addElementToDataset(result, dataset, new DcmCodeString(DoseType), "1", "1", "RTDoseModule");
        addElementToDataset(result, dataset, new DcmIntegerString(InstanceNumber), "1", "3", "RTDoseModule");
        addElementToDataset(result, dataset, new DcmLongString(DoseComment), "1", "3", "RTDoseModule");
        addElementToDataset(result, dataset, new DcmDecimalString(NormalizationPoint), "3", "3", "RTDoseModule");
        addElementToDataset(result, dataset, new DcmCodeString(DoseSummationType), "1", "1", "RTDoseModule");
        if (result.good()) result = ReferencedRTPlanSequence.write(dataset, "1-n" ,"1C", "RTDoseModule");
        addElementToDataset(result, dataset, new DcmDecimalString(GridFrameOffsetVector), "2-n", "1C", "RTDoseModule");
        addElementToDataset(result, dataset, new DcmDecimalString(DoseGridScaling), "1", "1", "RTDoseModule");
        addElementToDataset(result, dataset, new DcmCodeString(TissueHeterogeneityCorrection), "1-3", "3", "RTDoseModule");

        // --- RTDVHModule (U) ---
        if (isRTDVHModulePresent(OFFalse /*complete*/))
        {
            if (result.good()) result = ReferencedStructureSetSequence.write(dataset, "1-n" ,"1", "RTDVHModule");
            addElementToDataset(result, dataset, new DcmDecimalString(DVHNormalizationPoint), "3", "3", "RTDVHModule");
            addElementToDataset(result, dataset, new DcmDecimalString(DVHNormalizationDoseValue), "1", "3", "RTDVHModule");
            if (result.good()) result = DVHSequence.write(dataset, "1-n" ,"1", "RTDVHModule");
        }

        // --- StructureSetModule (C) ---
        if (isStructureSetModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmShortString(StructureSetLabel), "1", "1", "StructureSetModule");
            addElementToDataset(result, dataset, new DcmLongString(StructureSetName), "1", "3", "StructureSetModule");
            addElementToDataset(result, dataset, new DcmShortText(StructureSetDescription), "1", "3", "StructureSetModule");
            // addElementToDataset(result, dataset, new DcmIntegerString(InstanceNumber), "1", "3", "StructureSetModule");
            addElementToDataset(result, dataset, new DcmDate(StructureSetDate), "1", "2", "StructureSetModule");
            addElementToDataset(result, dataset, new DcmTime(StructureSetTime), "1", "2", "StructureSetModule");
            if (result.good()) result = ReferencedFrameOfReferenceSequence.write(dataset, "1-n" ,"3", "StructureSetModule");
            if (result.good()) result = StructureSetROISequence.write(dataset, "1-n" ,"3", "StructureSetModule");
        }

        // --- ROIContourModule (C) ---
        if (isROIContourModulePresent(OFFalse /*complete*/))
        {
            if (result.good()) result = ROIContourSequence.write(dataset, "1-n" ,"1", "ROIContourModule");
        }

        // --- RTDoseROIModule (C) ---
        if (isRTDoseROIModulePresent(OFFalse /*complete*/))
        {
            if (result.good()) result = RTDoseROISequence.write(dataset, "1-n" ,"1", "RTDoseROIModule");
        }

        // --- SOPCommonModule (M) ---
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(SOPClassUID), "1", "1", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(SOPInstanceUID), "1", "1", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmCodeString(SpecificCharacterSet), "1-n", "1C", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmDate(InstanceCreationDate), "1", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmTime(InstanceCreationTime), "1", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(InstanceCreatorUID), "1", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(RelatedGeneralSOPClassUID), "1-n", "3", "SOPCommonModule");
        addElementToDataset(result, dataset, new DcmUniqueIdentifier(OriginalSpecializedSOPClassUID), "1", "3", "SOPCommonModule");
        if (result.good()) result = CodingSchemeIdentificationSequence.write(dataset, "1-n" ,"3", "SOPCommonModule");
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
    } else
        result = RT_EC_InvalidObject;
    return result;
}


OFBool DRTDoseIOD::isClinicalTrialSubjectModulePresent(const OFBool complete)
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
               !ClinicalTrialSubjectReadingID.isEmpty();
    }
}


OFBool DRTDoseIOD::isPatientStudyModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !AdmittingDiagnosesDescription.isEmpty() ||
           !AdmittingDiagnosesCodeSequence.isEmpty() ||
           !PatientAge.isEmpty() ||
           !PatientSize.isEmpty() ||
           !PatientWeight.isEmpty() ||
           !Occupation.isEmpty() ||
           !AdditionalPatientHistory.isEmpty() ||
           !AdmissionID.isEmpty() ||
           !IssuerOfAdmissionID.isEmpty() ||
           !PatientSexNeutered.isEmpty();
}


OFBool DRTDoseIOD::isClinicalTrialStudyModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !ClinicalTrialTimePointID.isEmpty() ||
           !ClinicalTrialTimePointDescription.isEmpty();
}


OFBool DRTDoseIOD::isClinicalTrialSeriesModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !ClinicalTrialCoordinatingCenterName.isEmpty();
}


OFBool DRTDoseIOD::isGeneralImageModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !PatientOrientation.isEmpty() ||
           !ContentDate.isEmpty() ||
           !ContentTime.isEmpty() ||
           !ImageType.isEmpty() ||
           !AcquisitionNumber.isEmpty() ||
           !AcquisitionDate.isEmpty() ||
           !AcquisitionTime.isEmpty() ||
           !AcquisitionDateTime.isEmpty() ||
           !ReferencedImageSequence.isEmpty() ||
           !DerivationDescription.isEmpty() ||
           !DerivationCodeSequence.isEmpty() ||
           !SourceImageSequence.isEmpty() ||
           !ReferencedInstanceSequence.isEmpty() ||
           !ImagesInAcquisition.isEmpty() ||
           !ImageComments.isEmpty() ||
           !QualityControlImage.isEmpty() ||
           !BurnedInAnnotation.isEmpty() ||
           !LossyImageCompression.isEmpty() ||
           !LossyImageCompressionRatio.isEmpty() ||
           !LossyImageCompressionMethod.isEmpty() ||
           !IconImageSequence.isEmpty() ||
           !PresentationLUTShape.isEmpty() ||
           !IrradiationEventUID.isEmpty();
}


OFBool DRTDoseIOD::isImagePlaneModulePresent(const OFBool complete)
{
    if (complete)
    {
        /* check whether all mandatory attributes are present */
        return !PixelSpacing.isEmpty() &&
               !ImageOrientationPatient.isEmpty() &&
               !ImagePositionPatient.isEmpty();
    } else {
        /* check whether at least one attribute is present */
        return !PixelSpacing.isEmpty() ||
               !ImageOrientationPatient.isEmpty() ||
               !ImagePositionPatient.isEmpty() ||
               !SliceThickness.isEmpty() ||
               !SliceLocation.isEmpty();
    }
}


OFBool DRTDoseIOD::isImagePixelModulePresent(const OFBool complete)
{
    if (complete)
    {
        /* check whether all mandatory attributes are present */
        return !Rows.isEmpty() &&
               !Columns.isEmpty();
    } else {
        /* check whether at least one attribute is present */
        return !Rows.isEmpty() ||
               !Columns.isEmpty() ||
               !PixelData.isEmpty() ||
               !PlanarConfiguration.isEmpty() ||
               !PixelAspectRatio.isEmpty() ||
               !SmallestImagePixelValue.isEmpty() ||
               !LargestImagePixelValue.isEmpty() ||
               !RedPaletteColorLookupTableDescriptor.isEmpty() ||
               !GreenPaletteColorLookupTableDescriptor.isEmpty() ||
               !BluePaletteColorLookupTableDescriptor.isEmpty() ||
               !RedPaletteColorLookupTableData.isEmpty() ||
               !GreenPaletteColorLookupTableData.isEmpty() ||
               !BluePaletteColorLookupTableData.isEmpty() ||
               !ICCProfile.isEmpty() ||
               !PixelDataProviderURL.isEmpty();
    }
}


OFBool DRTDoseIOD::isMultiFrameModulePresent(const OFBool complete)
{
    if (complete)
    {
        /* check whether all mandatory attributes are present */
        return !NumberOfFrames.isEmpty() &&
               !FrameIncrementPointer.isEmpty();
    } else {
        /* check whether at least one attribute is present */
        return !NumberOfFrames.isEmpty() ||
               !FrameIncrementPointer.isEmpty();
    }
}


OFBool DRTDoseIOD::isOverlayPlaneModulePresent(const OFBool complete)
{
    if (complete)
    {
        /* check whether all mandatory attributes are present */
        return !OverlayRows.isEmpty() &&
               !OverlayColumns.isEmpty() &&
               !OverlayType.isEmpty() &&
               !OverlayOrigin.isEmpty() &&
               !OverlayBitsAllocated.isEmpty() &&
               !OverlayBitPosition.isEmpty() &&
               !OverlayData.isEmpty();
    } else {
        /* check whether at least one attribute is present */
        return !OverlayRows.isEmpty() ||
               !OverlayColumns.isEmpty() ||
               !OverlayType.isEmpty() ||
               !OverlayOrigin.isEmpty() ||
               !OverlayBitsAllocated.isEmpty() ||
               !OverlayBitPosition.isEmpty() ||
               !OverlayData.isEmpty() ||
               !OverlayDescription.isEmpty() ||
               !OverlaySubtype.isEmpty() ||
               !OverlayLabel.isEmpty() ||
               !ROIArea.isEmpty() ||
               !ROIMean.isEmpty() ||
               !ROIStandardDeviation.isEmpty();
    }
}


OFBool DRTDoseIOD::isMultiFrameOverlayModulePresent(const OFBool complete)
{
    if (complete)
    {
        /* check whether all mandatory attributes are present */
        return !NumberOfFramesInOverlay.isEmpty();
    } else {
        /* check whether at least one attribute is present */
        return !NumberOfFramesInOverlay.isEmpty() ||
               !ImageFrameOrigin.isEmpty();
    }
}


OFBool DRTDoseIOD::isModalityLUTModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !ModalityLUTSequence.isEmpty() ||
           !RescaleIntercept.isEmpty() ||
           !RescaleSlope.isEmpty() ||
           !RescaleType.isEmpty();
}


OFBool DRTDoseIOD::isRTDVHModulePresent(const OFBool complete)
{
    if (complete)
    {
        /* check whether all mandatory attributes are present */
        return !ReferencedStructureSetSequence.isEmpty() &&
               !DVHSequence.isEmpty();
    } else {
        /* check whether at least one attribute is present */
        return !ReferencedStructureSetSequence.isEmpty() ||
               !DVHNormalizationPoint.isEmpty() ||
               !DVHNormalizationDoseValue.isEmpty() ||
               !DVHSequence.isEmpty();
    }
}


OFBool DRTDoseIOD::isStructureSetModulePresent(const OFBool complete)
{
    if (complete)
    {
        /* check whether all mandatory attributes are present */
        return !StructureSetLabel.isEmpty();
    } else {
        /* check whether at least one attribute is present */
        return !StructureSetLabel.isEmpty() ||
               !StructureSetName.isEmpty() ||
               !StructureSetDescription.isEmpty() ||
               !StructureSetDate.isEmpty() ||
               !StructureSetTime.isEmpty() ||
               !ReferencedFrameOfReferenceSequence.isEmpty() ||
               !StructureSetROISequence.isEmpty();
    }
}


OFBool DRTDoseIOD::isROIContourModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !ROIContourSequence.isEmpty();
}


OFBool DRTDoseIOD::isRTDoseROIModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !RTDoseROISequence.isEmpty();
}


OFCondition DRTDoseIOD::getAccessionNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AccessionNumber, value, pos);
}


OFCondition DRTDoseIOD::getAcquisitionDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AcquisitionDate, value, pos);
}


OFCondition DRTDoseIOD::getAcquisitionDateTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AcquisitionDateTime, value, pos);
}


OFCondition DRTDoseIOD::getAcquisitionNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AcquisitionNumber, value, pos);
}


OFCondition DRTDoseIOD::getAcquisitionNumber(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, AcquisitionNumber).getSint32(value, pos);
}


OFCondition DRTDoseIOD::getAcquisitionTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AcquisitionTime, value, pos);
}


OFCondition DRTDoseIOD::getAdditionalPatientHistory(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AdditionalPatientHistory, value, pos);
}


OFCondition DRTDoseIOD::getAdmissionID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AdmissionID, value, pos);
}


OFCondition DRTDoseIOD::getAdmittingDiagnosesDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AdmittingDiagnosesDescription, value, pos);
}


OFCondition DRTDoseIOD::getAuthorizationEquipmentCertificationNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AuthorizationEquipmentCertificationNumber, value, pos);
}


OFCondition DRTDoseIOD::getBitsAllocated(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, BitsAllocated).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getBitsStored(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, BitsStored).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getBluePaletteColorLookupTableData(Uint16 *&value, unsigned long *count) const
{
    OFCondition result = OFconst_cast(DcmOtherByteOtherWord &, BluePaletteColorLookupTableData).getUint16Array(value);
    /* set optional count parameter */
    if (count != NULL)
    {
        if (result.good())
            *count = OFconst_cast(DcmOtherByteOtherWord &, BluePaletteColorLookupTableData).getLength() / sizeof(Uint16);
        else
            *count = 0;
    }
    return result;
}


OFCondition DRTDoseIOD::getBluePaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, BluePaletteColorLookupTableDescriptor).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getBurnedInAnnotation(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(BurnedInAnnotation, value, pos);
}


OFCondition DRTDoseIOD::getClinicalTrialCoordinatingCenterName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialCoordinatingCenterName, value, pos);
}


OFCondition DRTDoseIOD::getClinicalTrialProtocolID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialProtocolID, value, pos);
}


OFCondition DRTDoseIOD::getClinicalTrialProtocolName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialProtocolName, value, pos);
}


OFCondition DRTDoseIOD::getClinicalTrialSiteID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSiteID, value, pos);
}


OFCondition DRTDoseIOD::getClinicalTrialSiteName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSiteName, value, pos);
}


OFCondition DRTDoseIOD::getClinicalTrialSponsorName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSponsorName, value, pos);
}


OFCondition DRTDoseIOD::getClinicalTrialSubjectID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSubjectID, value, pos);
}


OFCondition DRTDoseIOD::getClinicalTrialSubjectReadingID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSubjectReadingID, value, pos);
}


OFCondition DRTDoseIOD::getClinicalTrialTimePointDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialTimePointDescription, value, pos);
}


OFCondition DRTDoseIOD::getClinicalTrialTimePointID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialTimePointID, value, pos);
}


OFCondition DRTDoseIOD::getColumns(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, Columns).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getContentDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ContentDate, value, pos);
}


OFCondition DRTDoseIOD::getContentTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ContentTime, value, pos);
}


OFCondition DRTDoseIOD::getDVHNormalizationDoseValue(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DVHNormalizationDoseValue, value, pos);
}


OFCondition DRTDoseIOD::getDVHNormalizationDoseValue(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, DVHNormalizationDoseValue).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getDVHNormalizationPoint(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DVHNormalizationPoint, value, pos);
}


OFCondition DRTDoseIOD::getDVHNormalizationPoint(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, DVHNormalizationPoint).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getDVHNormalizationPoint(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, DVHNormalizationPoint).getFloat64Vector(value);
}


OFCondition DRTDoseIOD::getDateOfLastCalibration(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DateOfLastCalibration, value, pos);
}


OFCondition DRTDoseIOD::getDeidentificationMethod(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DeidentificationMethod, value, pos);
}


OFCondition DRTDoseIOD::getDerivationDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DerivationDescription, value, pos);
}


OFCondition DRTDoseIOD::getDeviceSerialNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DeviceSerialNumber, value, pos);
}


OFCondition DRTDoseIOD::getDoseComment(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DoseComment, value, pos);
}


OFCondition DRTDoseIOD::getDoseGridScaling(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DoseGridScaling, value, pos);
}


OFCondition DRTDoseIOD::getDoseGridScaling(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, DoseGridScaling).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getDoseSummationType(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DoseSummationType, value, pos);
}


OFCondition DRTDoseIOD::getDoseType(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DoseType, value, pos);
}


OFCondition DRTDoseIOD::getDoseUnits(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DoseUnits, value, pos);
}


OFCondition DRTDoseIOD::getEthnicGroup(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(EthnicGroup, value, pos);
}


OFCondition DRTDoseIOD::getFrameIncrementPointer(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(FrameIncrementPointer, value, pos);
}


OFCondition DRTDoseIOD::getFrameOfReferenceUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(FrameOfReferenceUID, value, pos);
}


OFCondition DRTDoseIOD::getGantryID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(GantryID, value, pos);
}


OFCondition DRTDoseIOD::getGreenPaletteColorLookupTableData(Uint16 *&value, unsigned long *count) const
{
    OFCondition result = OFconst_cast(DcmOtherByteOtherWord &, GreenPaletteColorLookupTableData).getUint16Array(value);
    /* set optional count parameter */
    if (count != NULL)
    {
        if (result.good())
            *count = OFconst_cast(DcmOtherByteOtherWord &, GreenPaletteColorLookupTableData).getLength() / sizeof(Uint16);
        else
            *count = 0;
    }
    return result;
}


OFCondition DRTDoseIOD::getGreenPaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, GreenPaletteColorLookupTableDescriptor).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getGridFrameOffsetVector(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(GridFrameOffsetVector, value, pos);
}


OFCondition DRTDoseIOD::getGridFrameOffsetVector(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, GridFrameOffsetVector).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getGridFrameOffsetVector(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, GridFrameOffsetVector).getFloat64Vector(value);
}


OFCondition DRTDoseIOD::getHighBit(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, HighBit).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getICCProfile(Uint8 *&value, unsigned long *count) const
{
    OFCondition result = OFconst_cast(DcmOtherByteOtherWord &, ICCProfile).getUint8Array(value);
    /* set optional count parameter */
    if (count != NULL)
    {
        if (result.good())
            *count = OFconst_cast(DcmOtherByteOtherWord &, ICCProfile).getLength() / sizeof(Uint8);
        else
            *count = 0;
    }
    return result;
}


OFCondition DRTDoseIOD::getImageComments(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ImageComments, value, pos);
}


OFCondition DRTDoseIOD::getImageFrameOrigin(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, ImageFrameOrigin).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getImageOrientationPatient(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ImageOrientationPatient, value, pos);
}


OFCondition DRTDoseIOD::getImageOrientationPatient(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, ImageOrientationPatient).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getImageOrientationPatient(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, ImageOrientationPatient).getFloat64Vector(value);
}


OFCondition DRTDoseIOD::getImagePositionPatient(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ImagePositionPatient, value, pos);
}


OFCondition DRTDoseIOD::getImagePositionPatient(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, ImagePositionPatient).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getImagePositionPatient(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, ImagePositionPatient).getFloat64Vector(value);
}


OFCondition DRTDoseIOD::getImageType(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ImageType, value, pos);
}


OFCondition DRTDoseIOD::getImagesInAcquisition(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ImagesInAcquisition, value, pos);
}


OFCondition DRTDoseIOD::getImagesInAcquisition(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, ImagesInAcquisition).getSint32(value, pos);
}


OFCondition DRTDoseIOD::getInstanceCreationDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceCreationDate, value, pos);
}


OFCondition DRTDoseIOD::getInstanceCreationTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceCreationTime, value, pos);
}


OFCondition DRTDoseIOD::getInstanceCreatorUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceCreatorUID, value, pos);
}


OFCondition DRTDoseIOD::getInstanceNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceNumber, value, pos);
}


OFCondition DRTDoseIOD::getInstanceNumber(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, InstanceNumber).getSint32(value, pos);
}


OFCondition DRTDoseIOD::getInstitutionAddress(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstitutionAddress, value, pos);
}


OFCondition DRTDoseIOD::getInstitutionName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstitutionName, value, pos);
}


OFCondition DRTDoseIOD::getInstitutionalDepartmentName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstitutionalDepartmentName, value, pos);
}


OFCondition DRTDoseIOD::getIrradiationEventUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(IrradiationEventUID, value, pos);
}


OFCondition DRTDoseIOD::getIssuerOfAdmissionID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(IssuerOfAdmissionID, value, pos);
}


OFCondition DRTDoseIOD::getIssuerOfPatientID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(IssuerOfPatientID, value, pos);
}


OFCondition DRTDoseIOD::getLargestImagePixelValue(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, LargestImagePixelValue).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getLossyImageCompression(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(LossyImageCompression, value, pos);
}


OFCondition DRTDoseIOD::getLossyImageCompressionMethod(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(LossyImageCompressionMethod, value, pos);
}


OFCondition DRTDoseIOD::getLossyImageCompressionRatio(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(LossyImageCompressionRatio, value, pos);
}


OFCondition DRTDoseIOD::getLossyImageCompressionRatio(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, LossyImageCompressionRatio).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getLossyImageCompressionRatio(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, LossyImageCompressionRatio).getFloat64Vector(value);
}


OFCondition DRTDoseIOD::getManufacturer(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(Manufacturer, value, pos);
}


OFCondition DRTDoseIOD::getManufacturerModelName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ManufacturerModelName, value, pos);
}


OFCondition DRTDoseIOD::getModality(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(Modality, value, pos);
}


OFCondition DRTDoseIOD::getNameOfPhysiciansReadingStudy(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(NameOfPhysiciansReadingStudy, value, pos);
}


OFCondition DRTDoseIOD::getNormalizationPoint(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(NormalizationPoint, value, pos);
}


OFCondition DRTDoseIOD::getNormalizationPoint(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, NormalizationPoint).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getNormalizationPoint(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, NormalizationPoint).getFloat64Vector(value);
}


OFCondition DRTDoseIOD::getNumberOfFrames(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(NumberOfFrames, value, pos);
}


OFCondition DRTDoseIOD::getNumberOfFrames(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, NumberOfFrames).getSint32(value, pos);
}


OFCondition DRTDoseIOD::getNumberOfFramesInOverlay(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(NumberOfFramesInOverlay, value, pos);
}


OFCondition DRTDoseIOD::getNumberOfFramesInOverlay(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, NumberOfFramesInOverlay).getSint32(value, pos);
}


OFCondition DRTDoseIOD::getOccupation(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(Occupation, value, pos);
}


OFCondition DRTDoseIOD::getOriginalSpecializedSOPClassUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OriginalSpecializedSOPClassUID, value, pos);
}


OFCondition DRTDoseIOD::getOtherPatientIDs(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OtherPatientIDs, value, pos);
}


OFCondition DRTDoseIOD::getOtherPatientNames(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OtherPatientNames, value, pos);
}


OFCondition DRTDoseIOD::getOverlayBitPosition(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, OverlayBitPosition).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getOverlayBitsAllocated(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, OverlayBitsAllocated).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getOverlayColumns(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, OverlayColumns).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getOverlayDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OverlayDescription, value, pos);
}


OFCondition DRTDoseIOD::getOverlayLabel(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OverlayLabel, value, pos);
}


OFCondition DRTDoseIOD::getOverlayOrigin(Sint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmSignedShort &, OverlayOrigin).getSint16(value, pos);
}


OFCondition DRTDoseIOD::getOverlayRows(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, OverlayRows).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getOverlaySubtype(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OverlaySubtype, value, pos);
}


OFCondition DRTDoseIOD::getOverlayType(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OverlayType, value, pos);
}


OFCondition DRTDoseIOD::getPatientAge(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientAge, value, pos);
}


OFCondition DRTDoseIOD::getPatientBirthDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientBirthDate, value, pos);
}


OFCondition DRTDoseIOD::getPatientBirthTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientBirthTime, value, pos);
}


OFCondition DRTDoseIOD::getPatientBreedDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientBreedDescription, value, pos);
}


OFCondition DRTDoseIOD::getPatientComments(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientComments, value, pos);
}


OFCondition DRTDoseIOD::getPatientID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientID, value, pos);
}


OFCondition DRTDoseIOD::getPatientIdentityRemoved(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientIdentityRemoved, value, pos);
}


OFCondition DRTDoseIOD::getPatientName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientName, value, pos);
}


OFCondition DRTDoseIOD::getPatientOrientation(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientOrientation, value, pos);
}


OFCondition DRTDoseIOD::getPatientSex(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientSex, value, pos);
}


OFCondition DRTDoseIOD::getPatientSexNeutered(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientSexNeutered, value, pos);
}


OFCondition DRTDoseIOD::getPatientSize(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientSize, value, pos);
}


OFCondition DRTDoseIOD::getPatientSize(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, PatientSize).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getPatientSpeciesDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientSpeciesDescription, value, pos);
}


OFCondition DRTDoseIOD::getPatientWeight(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientWeight, value, pos);
}


OFCondition DRTDoseIOD::getPatientWeight(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, PatientWeight).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getPerformedProcedureStepDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepDescription, value, pos);
}


OFCondition DRTDoseIOD::getPerformedProcedureStepID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepID, value, pos);
}


OFCondition DRTDoseIOD::getPerformedProcedureStepStartDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepStartDate, value, pos);
}


OFCondition DRTDoseIOD::getPerformedProcedureStepStartTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepStartTime, value, pos);
}


OFCondition DRTDoseIOD::getPhotometricInterpretation(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PhotometricInterpretation, value, pos);
}


OFCondition DRTDoseIOD::getPhysiciansOfRecord(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PhysiciansOfRecord, value, pos);
}


OFCondition DRTDoseIOD::getPixelAspectRatio(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PixelAspectRatio, value, pos);
}


OFCondition DRTDoseIOD::getPixelAspectRatio(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, PixelAspectRatio).getSint32(value, pos);
}


OFCondition DRTDoseIOD::getPixelDataProviderURL(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PixelDataProviderURL, value, pos);
}


OFCondition DRTDoseIOD::getPixelPaddingValue(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, PixelPaddingValue).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getPixelRepresentation(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, PixelRepresentation).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getPixelSpacing(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PixelSpacing, value, pos);
}


OFCondition DRTDoseIOD::getPixelSpacing(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, PixelSpacing).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getPixelSpacing(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, PixelSpacing).getFloat64Vector(value);
}


OFCondition DRTDoseIOD::getPlanarConfiguration(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, PlanarConfiguration).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getPositionReferenceIndicator(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PositionReferenceIndicator, value, pos);
}


OFCondition DRTDoseIOD::getPresentationLUTShape(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PresentationLUTShape, value, pos);
}


OFCondition DRTDoseIOD::getQualityControlImage(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(QualityControlImage, value, pos);
}


OFCondition DRTDoseIOD::getROIArea(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ROIArea, value, pos);
}


OFCondition DRTDoseIOD::getROIArea(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, ROIArea).getSint32(value, pos);
}


OFCondition DRTDoseIOD::getROIMean(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ROIMean, value, pos);
}


OFCondition DRTDoseIOD::getROIMean(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, ROIMean).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getROIStandardDeviation(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ROIStandardDeviation, value, pos);
}


OFCondition DRTDoseIOD::getROIStandardDeviation(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, ROIStandardDeviation).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getRedPaletteColorLookupTableData(Uint16 *&value, unsigned long *count) const
{
    OFCondition result = OFconst_cast(DcmOtherByteOtherWord &, RedPaletteColorLookupTableData).getUint16Array(value);
    /* set optional count parameter */
    if (count != NULL)
    {
        if (result.good())
            *count = OFconst_cast(DcmOtherByteOtherWord &, RedPaletteColorLookupTableData).getLength() / sizeof(Uint16);
        else
            *count = 0;
    }
    return result;
}


OFCondition DRTDoseIOD::getRedPaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, RedPaletteColorLookupTableDescriptor).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getReferringPhysicianName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ReferringPhysicianName, value, pos);
}


OFCondition DRTDoseIOD::getRelatedGeneralSOPClassUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RelatedGeneralSOPClassUID, value, pos);
}


OFCondition DRTDoseIOD::getRescaleIntercept(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RescaleIntercept, value, pos);
}


OFCondition DRTDoseIOD::getRescaleIntercept(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, RescaleIntercept).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getRescaleSlope(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RescaleSlope, value, pos);
}


OFCondition DRTDoseIOD::getRescaleSlope(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, RescaleSlope).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getRescaleType(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RescaleType, value, pos);
}


OFCondition DRTDoseIOD::getResponsibleOrganization(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ResponsibleOrganization, value, pos);
}


OFCondition DRTDoseIOD::getResponsiblePerson(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ResponsiblePerson, value, pos);
}


OFCondition DRTDoseIOD::getResponsiblePersonRole(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ResponsiblePersonRole, value, pos);
}


OFCondition DRTDoseIOD::getRows(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, Rows).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getSOPAuthorizationComment(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPAuthorizationComment, value, pos);
}


OFCondition DRTDoseIOD::getSOPAuthorizationDateTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPAuthorizationDateTime, value, pos);
}


OFCondition DRTDoseIOD::getSOPClassUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPClassUID, value, pos);
}


OFCondition DRTDoseIOD::getSOPInstanceStatus(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPInstanceStatus, value, pos);
}


OFCondition DRTDoseIOD::getSOPInstanceUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPInstanceUID, value, pos);
}


OFCondition DRTDoseIOD::getSamplesPerPixel(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, SamplesPerPixel).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getSeriesDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SeriesDescription, value, pos);
}


OFCondition DRTDoseIOD::getSeriesInstanceUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SeriesInstanceUID, value, pos);
}


OFCondition DRTDoseIOD::getSeriesNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SeriesNumber, value, pos);
}


OFCondition DRTDoseIOD::getSeriesNumber(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, SeriesNumber).getSint32(value, pos);
}


OFCondition DRTDoseIOD::getSliceLocation(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SliceLocation, value, pos);
}


OFCondition DRTDoseIOD::getSliceLocation(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, SliceLocation).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getSliceThickness(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SliceThickness, value, pos);
}


OFCondition DRTDoseIOD::getSliceThickness(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, SliceThickness).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getSmallestImagePixelValue(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, SmallestImagePixelValue).getUint16(value, pos);
}


OFCondition DRTDoseIOD::getSoftwareVersions(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SoftwareVersions, value, pos);
}


OFCondition DRTDoseIOD::getSpatialResolution(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SpatialResolution, value, pos);
}


OFCondition DRTDoseIOD::getSpatialResolution(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, SpatialResolution).getFloat64(value, pos);
}


OFCondition DRTDoseIOD::getSpecificCharacterSet(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SpecificCharacterSet, value, pos);
}


OFCondition DRTDoseIOD::getStationName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StationName, value, pos);
}


OFCondition DRTDoseIOD::getStructureSetDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StructureSetDate, value, pos);
}


OFCondition DRTDoseIOD::getStructureSetDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StructureSetDescription, value, pos);
}


OFCondition DRTDoseIOD::getStructureSetLabel(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StructureSetLabel, value, pos);
}


OFCondition DRTDoseIOD::getStructureSetName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StructureSetName, value, pos);
}


OFCondition DRTDoseIOD::getStructureSetTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StructureSetTime, value, pos);
}


OFCondition DRTDoseIOD::getStudyDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyDate, value, pos);
}


OFCondition DRTDoseIOD::getStudyDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyDescription, value, pos);
}


OFCondition DRTDoseIOD::getStudyID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyID, value, pos);
}


OFCondition DRTDoseIOD::getStudyInstanceUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyInstanceUID, value, pos);
}


OFCondition DRTDoseIOD::getStudyTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyTime, value, pos);
}


OFCondition DRTDoseIOD::getTimeOfLastCalibration(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(TimeOfLastCalibration, value, pos);
}


OFCondition DRTDoseIOD::getTimezoneOffsetFromUTC(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(TimezoneOffsetFromUTC, value, pos);
}


OFCondition DRTDoseIOD::getTissueHeterogeneityCorrection(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(TissueHeterogeneityCorrection, value, pos);
}


OFCondition DRTDoseIOD::setAccessionNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AccessionNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setAcquisitionDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AcquisitionDate.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setAcquisitionDateTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AcquisitionDateTime.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setAcquisitionNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AcquisitionNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setAcquisitionTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AcquisitionTime.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setAdditionalPatientHistory(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = AdditionalPatientHistory.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setAdmissionID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AdmissionID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setAdmittingDiagnosesDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = AdmittingDiagnosesDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setAuthorizationEquipmentCertificationNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AuthorizationEquipmentCertificationNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setBitsAllocated(const Uint16 value, const unsigned long pos)
{
    return BitsAllocated.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setBitsStored(const Uint16 value, const unsigned long pos)
{
    return BitsStored.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setBluePaletteColorLookupTableData(const Uint16 *value, const unsigned long count)
{
    return BluePaletteColorLookupTableData.putUint16Array(value, count);
}


OFCondition DRTDoseIOD::setBluePaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos)
{
    return BluePaletteColorLookupTableDescriptor.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setBurnedInAnnotation(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = BurnedInAnnotation.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setClinicalTrialCoordinatingCenterName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialCoordinatingCenterName.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setClinicalTrialProtocolID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialProtocolID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setClinicalTrialProtocolName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialProtocolName.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setClinicalTrialSiteID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSiteID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setClinicalTrialSiteName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSiteName.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setClinicalTrialSponsorName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSponsorName.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setClinicalTrialSubjectID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSubjectID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setClinicalTrialSubjectReadingID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSubjectReadingID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setClinicalTrialTimePointDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = ClinicalTrialTimePointDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setClinicalTrialTimePointID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialTimePointID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setColumns(const Uint16 value, const unsigned long pos)
{
    return Columns.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setContentDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ContentDate.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setContentTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ContentTime.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setDVHNormalizationDoseValue(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = DVHNormalizationDoseValue.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setDVHNormalizationPoint(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "3") : EC_Normal;
    if (result.good())
        result = DVHNormalizationPoint.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setDateOfLastCalibration(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = DateOfLastCalibration.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setDeidentificationMethod(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = DeidentificationMethod.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setDerivationDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = DerivationDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setDeviceSerialNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = DeviceSerialNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setDoseComment(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = DoseComment.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setDoseGridScaling(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = DoseGridScaling.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setDoseSummationType(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = DoseSummationType.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setDoseType(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = DoseType.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setDoseUnits(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = DoseUnits.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setEthnicGroup(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = EthnicGroup.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setFrameIncrementPointer(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmAttributeTag::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = FrameIncrementPointer.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setFrameOfReferenceUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = FrameOfReferenceUID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setGantryID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = GantryID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setGreenPaletteColorLookupTableData(const Uint16 *value, const unsigned long count)
{
    return GreenPaletteColorLookupTableData.putUint16Array(value, count);
}


OFCondition DRTDoseIOD::setGreenPaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos)
{
    return GreenPaletteColorLookupTableDescriptor.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setGridFrameOffsetVector(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "2-n") : EC_Normal;
    if (result.good())
        result = GridFrameOffsetVector.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setHighBit(const Uint16 value, const unsigned long pos)
{
    return HighBit.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setICCProfile(const Uint8 *value, const unsigned long count)
{
    return ICCProfile.putUint8Array(value, count);
}


OFCondition DRTDoseIOD::setImageComments(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = ImageComments.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setImageFrameOrigin(const Uint16 value, const unsigned long pos)
{
    return ImageFrameOrigin.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setImageOrientationPatient(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "6") : EC_Normal;
    if (result.good())
        result = ImageOrientationPatient.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setImagePositionPatient(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "3") : EC_Normal;
    if (result.good())
        result = ImagePositionPatient.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setImageType(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "2-n") : EC_Normal;
    if (result.good())
        result = ImageType.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setImagesInAcquisition(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ImagesInAcquisition.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setInstanceCreationDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceCreationDate.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setInstanceCreationTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceCreationTime.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setInstanceCreatorUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceCreatorUID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setInstanceNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setInstitutionAddress(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = InstitutionAddress.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setInstitutionName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstitutionName.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setInstitutionalDepartmentName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstitutionalDepartmentName.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setIrradiationEventUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = IrradiationEventUID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setIssuerOfAdmissionID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = IssuerOfAdmissionID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setIssuerOfPatientID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = IssuerOfPatientID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setLargestImagePixelValue(const Uint16 value, const unsigned long pos)
{
    return LargestImagePixelValue.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setLossyImageCompression(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = LossyImageCompression.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setLossyImageCompressionMethod(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = LossyImageCompressionMethod.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setLossyImageCompressionRatio(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = LossyImageCompressionRatio.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setManufacturer(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = Manufacturer.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setManufacturerModelName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ManufacturerModelName.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setModality(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = Modality.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setNameOfPhysiciansReadingStudy(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = NameOfPhysiciansReadingStudy.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setNormalizationPoint(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "3") : EC_Normal;
    if (result.good())
        result = NormalizationPoint.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setNumberOfFrames(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = NumberOfFrames.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setNumberOfFramesInOverlay(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = NumberOfFramesInOverlay.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setOccupation(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = Occupation.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setOriginalSpecializedSOPClassUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = OriginalSpecializedSOPClassUID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setOtherPatientIDs(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = OtherPatientIDs.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setOtherPatientNames(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = OtherPatientNames.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setOverlayBitPosition(const Uint16 value, const unsigned long pos)
{
    return OverlayBitPosition.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setOverlayBitsAllocated(const Uint16 value, const unsigned long pos)
{
    return OverlayBitsAllocated.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setOverlayColumns(const Uint16 value, const unsigned long pos)
{
    return OverlayColumns.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setOverlayDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = OverlayDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setOverlayLabel(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = OverlayLabel.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setOverlayOrigin(const Sint16 value, const unsigned long pos)
{
    return OverlayOrigin.putSint16(value, pos);
}


OFCondition DRTDoseIOD::setOverlayRows(const Uint16 value, const unsigned long pos)
{
    return OverlayRows.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setOverlaySubtype(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = OverlaySubtype.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setOverlayType(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = OverlayType.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPatientAge(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmAgeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientAge.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPatientBirthDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientBirthDate.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPatientBirthTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientBirthTime.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPatientBreedDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientBreedDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPatientComments(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = PatientComments.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPatientID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPatientIdentityRemoved(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientIdentityRemoved.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPatientName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientName.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPatientOrientation(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "2") : EC_Normal;
    if (result.good())
        result = PatientOrientation.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPatientSex(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSex.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPatientSexNeutered(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSexNeutered.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPatientSize(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSize.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPatientSpeciesDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSpeciesDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPatientWeight(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientWeight.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPerformedProcedureStepDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPerformedProcedureStepID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPerformedProcedureStepStartDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepStartDate.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPerformedProcedureStepStartTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepStartTime.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPhotometricInterpretation(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PhotometricInterpretation.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPhysiciansOfRecord(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = PhysiciansOfRecord.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPixelAspectRatio(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "2") : EC_Normal;
    if (result.good())
        result = PixelAspectRatio.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPixelDataProviderURL(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUnlimitedText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = PixelDataProviderURL.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPixelPaddingValue(const Uint16 value, const unsigned long pos)
{
    return PixelPaddingValue.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setPixelRepresentation(const Uint16 value, const unsigned long pos)
{
    return PixelRepresentation.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setPixelSpacing(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "2") : EC_Normal;
    if (result.good())
        result = PixelSpacing.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPlanarConfiguration(const Uint16 value, const unsigned long pos)
{
    return PlanarConfiguration.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setPositionReferenceIndicator(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PositionReferenceIndicator.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setPresentationLUTShape(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PresentationLUTShape.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setQualityControlImage(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = QualityControlImage.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setROIArea(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ROIArea.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setROIMean(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ROIMean.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setROIStandardDeviation(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ROIStandardDeviation.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setRedPaletteColorLookupTableData(const Uint16 *value, const unsigned long count)
{
    return RedPaletteColorLookupTableData.putUint16Array(value, count);
}


OFCondition DRTDoseIOD::setRedPaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos)
{
    return RedPaletteColorLookupTableDescriptor.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setReferringPhysicianName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ReferringPhysicianName.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setRelatedGeneralSOPClassUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = RelatedGeneralSOPClassUID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setRescaleIntercept(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RescaleIntercept.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setRescaleSlope(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RescaleSlope.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setRescaleType(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RescaleType.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setResponsibleOrganization(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ResponsibleOrganization.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setResponsiblePerson(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ResponsiblePerson.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setResponsiblePersonRole(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ResponsiblePersonRole.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setRows(const Uint16 value, const unsigned long pos)
{
    return Rows.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setSOPAuthorizationComment(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = SOPAuthorizationComment.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setSOPAuthorizationDateTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SOPAuthorizationDateTime.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setSOPClassUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SOPClassUID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setSOPInstanceStatus(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SOPInstanceStatus.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setSOPInstanceUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SOPInstanceUID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setSamplesPerPixel(const Uint16 value, const unsigned long pos)
{
    return SamplesPerPixel.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setSeriesDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setSeriesInstanceUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesInstanceUID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setSeriesNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setSliceLocation(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SliceLocation.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setSliceThickness(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SliceThickness.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setSmallestImagePixelValue(const Uint16 value, const unsigned long pos)
{
    return SmallestImagePixelValue.putUint16(value, pos);
}


OFCondition DRTDoseIOD::setSoftwareVersions(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = SoftwareVersions.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setSpatialResolution(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SpatialResolution.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setSpecificCharacterSet(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = SpecificCharacterSet.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setStationName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StationName.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setStructureSetDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StructureSetDate.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setStructureSetDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = StructureSetDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setStructureSetLabel(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StructureSetLabel.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setStructureSetName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StructureSetName.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setStructureSetTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StructureSetTime.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setStudyDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyDate.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setStudyDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setStudyID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setStudyInstanceUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyInstanceUID.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setStudyTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyTime.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setTimeOfLastCalibration(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = TimeOfLastCalibration.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setTimezoneOffsetFromUTC(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = TimezoneOffsetFromUTC.putOFStringArray(value);
    return result;
}


OFCondition DRTDoseIOD::setTissueHeterogeneityCorrection(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1-3") : EC_Normal;
    if (result.good())
        result = TissueHeterogeneityCorrection.putOFStringArray(value);
    return result;
}


// end of source file
