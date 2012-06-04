/*
 *
 *  Copyright (c) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTImageIOD
 *
 *  Generated automatically from DICOM PS 3.3-2007
 *  File created on 2012-02-01 16:51:15 
 *
 */


#include "dcmtk/config/osconfig.h"      // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/drtimage.h"


DRTImageIOD::DRTImageIOD()
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
    InstanceNumber(DCM_InstanceNumber),
    PatientOrientation(DCM_PatientOrientation),
    ContentDate(DCM_ContentDate),
    ContentTime(DCM_ContentTime),
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
    ContrastBolusAgent(DCM_ContrastBolusAgent),
    ContrastBolusAgentSequence(),
    ContrastBolusRoute(DCM_ContrastBolusRoute),
    ContrastBolusAdministrationRouteSequence(),
    ContrastBolusVolume(DCM_ContrastBolusVolume),
    ContrastBolusStartTime(DCM_ContrastBolusStartTime),
    ContrastBolusStopTime(DCM_ContrastBolusStopTime),
    ContrastBolusTotalDose(DCM_ContrastBolusTotalDose),
    ContrastFlowRate(DCM_ContrastFlowRate),
    ContrastFlowDuration(DCM_ContrastFlowDuration),
    ContrastBolusIngredient(DCM_ContrastBolusIngredient),
    ContrastBolusIngredientConcentration(DCM_ContrastBolusIngredientConcentration),
    PreferredPlaybackSequencing(DCM_PreferredPlaybackSequencing),
    FrameTime(DCM_FrameTime),
    FrameTimeVector(DCM_FrameTimeVector),
    StartTrim(DCM_StartTrim),
    StopTrim(DCM_StopTrim),
    RecommendedDisplayFrameRate(DCM_RecommendedDisplayFrameRate),
    CineRate(DCM_CineRate),
    FrameDelay(DCM_FrameDelay),
    ImageTriggerDelay(DCM_ImageTriggerDelay),
    EffectiveDuration(DCM_EffectiveDuration),
    ActualFrameDuration(DCM_ActualFrameDuration),
    MultiplexedAudioChannelsDescriptionCodeSequence(),
    NumberOfFrames(DCM_NumberOfFrames),
    FrameIncrementPointer(DCM_FrameIncrementPointer),
    DeviceSequence(),
    SamplesPerPixel(DCM_SamplesPerPixel),
    PhotometricInterpretation(DCM_PhotometricInterpretation),
    BitsAllocated(DCM_BitsAllocated),
    BitsStored(DCM_BitsStored),
    HighBit(DCM_HighBit),
    PixelRepresentation(DCM_PixelRepresentation),
    PixelIntensityRelationship(DCM_PixelIntensityRelationship),
    PixelIntensityRelationshipSign(DCM_PixelIntensityRelationshipSign),
    RTImageLabel(DCM_RTImageLabel),
    RTImageName(DCM_RTImageName),
    RTImageDescription(DCM_RTImageDescription),
    OperatorsName(DCM_OperatorsName),
    ImageType(DCM_ImageType),
    ConversionType(DCM_ConversionType),
    ReportedValuesOrigin(DCM_ReportedValuesOrigin),
    RTImagePlane(DCM_RTImagePlane),
    XRayImageReceptorTranslation(DCM_XRayImageReceptorTranslation),
    XRayImageReceptorAngle(DCM_XRayImageReceptorAngle),
    RTImageOrientation(DCM_RTImageOrientation),
    ImagePlanePixelSpacing(DCM_ImagePlanePixelSpacing),
    RTImagePosition(DCM_RTImagePosition),
    RadiationMachineName(DCM_RadiationMachineName),
    PrimaryDosimeterUnit(DCM_PrimaryDosimeterUnit),
    RadiationMachineSAD(DCM_RadiationMachineSAD),
    RadiationMachineSSD(DCM_RadiationMachineSSD),
    RTImageSID(DCM_RTImageSID),
    SourceToReferenceObjectDistance(DCM_SourceToReferenceObjectDistance),
    ReferencedRTPlanSequence(),
    ReferencedBeamNumber(DCM_ReferencedBeamNumber),
    ReferencedFractionGroupNumber(DCM_ReferencedFractionGroupNumber),
    FractionNumber(DCM_FractionNumber),
    StartCumulativeMetersetWeight(DCM_StartCumulativeMetersetWeight),
    EndCumulativeMetersetWeight(DCM_EndCumulativeMetersetWeight),
    ExposureSequence(),
    FluenceMapSequence(),
    GantryAngle(DCM_GantryAngle),
    GantryPitchAngle(DCM_GantryPitchAngle),
    BeamLimitingDeviceAngle(DCM_BeamLimitingDeviceAngle),
    PatientSupportAngle(DCM_PatientSupportAngle),
    TableTopEccentricAxisDistance(DCM_TableTopEccentricAxisDistance),
    TableTopEccentricAngle(DCM_TableTopEccentricAngle),
    TableTopVerticalPosition(DCM_TableTopVerticalPosition),
    TableTopLongitudinalPosition(DCM_TableTopLongitudinalPosition),
    TableTopLateralPosition(DCM_TableTopLateralPosition),
    IsocenterPosition(DCM_IsocenterPosition),
    PatientPosition(DCM_PatientPosition),
    ModalityLUTSequence(),
    RescaleIntercept(DCM_RescaleIntercept),
    RescaleSlope(DCM_RescaleSlope),
    RescaleType(DCM_RescaleType),
    VOILUTSequence(),
    WindowCenter(DCM_WindowCenter),
    WindowWidth(DCM_WindowWidth),
    WindowCenterWidthExplanation(DCM_WindowCenterWidthExplanation),
    VOILUTFunction(DCM_VOILUTFunction),
    ApprovalStatus(DCM_ApprovalStatus),
    ReviewDate(DCM_ReviewDate),
    ReviewTime(DCM_ReviewTime),
    ReviewerName(DCM_ReviewerName),
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


DRTImageIOD::DRTImageIOD(const DRTImageIOD &copy)
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
    InstanceNumber(copy.InstanceNumber),
    PatientOrientation(copy.PatientOrientation),
    ContentDate(copy.ContentDate),
    ContentTime(copy.ContentTime),
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
    ContrastBolusAgent(copy.ContrastBolusAgent),
    ContrastBolusAgentSequence(copy.ContrastBolusAgentSequence),
    ContrastBolusRoute(copy.ContrastBolusRoute),
    ContrastBolusAdministrationRouteSequence(copy.ContrastBolusAdministrationRouteSequence),
    ContrastBolusVolume(copy.ContrastBolusVolume),
    ContrastBolusStartTime(copy.ContrastBolusStartTime),
    ContrastBolusStopTime(copy.ContrastBolusStopTime),
    ContrastBolusTotalDose(copy.ContrastBolusTotalDose),
    ContrastFlowRate(copy.ContrastFlowRate),
    ContrastFlowDuration(copy.ContrastFlowDuration),
    ContrastBolusIngredient(copy.ContrastBolusIngredient),
    ContrastBolusIngredientConcentration(copy.ContrastBolusIngredientConcentration),
    PreferredPlaybackSequencing(copy.PreferredPlaybackSequencing),
    FrameTime(copy.FrameTime),
    FrameTimeVector(copy.FrameTimeVector),
    StartTrim(copy.StartTrim),
    StopTrim(copy.StopTrim),
    RecommendedDisplayFrameRate(copy.RecommendedDisplayFrameRate),
    CineRate(copy.CineRate),
    FrameDelay(copy.FrameDelay),
    ImageTriggerDelay(copy.ImageTriggerDelay),
    EffectiveDuration(copy.EffectiveDuration),
    ActualFrameDuration(copy.ActualFrameDuration),
    MultiplexedAudioChannelsDescriptionCodeSequence(copy.MultiplexedAudioChannelsDescriptionCodeSequence),
    NumberOfFrames(copy.NumberOfFrames),
    FrameIncrementPointer(copy.FrameIncrementPointer),
    DeviceSequence(copy.DeviceSequence),
    SamplesPerPixel(copy.SamplesPerPixel),
    PhotometricInterpretation(copy.PhotometricInterpretation),
    BitsAllocated(copy.BitsAllocated),
    BitsStored(copy.BitsStored),
    HighBit(copy.HighBit),
    PixelRepresentation(copy.PixelRepresentation),
    PixelIntensityRelationship(copy.PixelIntensityRelationship),
    PixelIntensityRelationshipSign(copy.PixelIntensityRelationshipSign),
    RTImageLabel(copy.RTImageLabel),
    RTImageName(copy.RTImageName),
    RTImageDescription(copy.RTImageDescription),
    OperatorsName(copy.OperatorsName),
    ImageType(copy.ImageType),
    ConversionType(copy.ConversionType),
    ReportedValuesOrigin(copy.ReportedValuesOrigin),
    RTImagePlane(copy.RTImagePlane),
    XRayImageReceptorTranslation(copy.XRayImageReceptorTranslation),
    XRayImageReceptorAngle(copy.XRayImageReceptorAngle),
    RTImageOrientation(copy.RTImageOrientation),
    ImagePlanePixelSpacing(copy.ImagePlanePixelSpacing),
    RTImagePosition(copy.RTImagePosition),
    RadiationMachineName(copy.RadiationMachineName),
    PrimaryDosimeterUnit(copy.PrimaryDosimeterUnit),
    RadiationMachineSAD(copy.RadiationMachineSAD),
    RadiationMachineSSD(copy.RadiationMachineSSD),
    RTImageSID(copy.RTImageSID),
    SourceToReferenceObjectDistance(copy.SourceToReferenceObjectDistance),
    ReferencedRTPlanSequence(copy.ReferencedRTPlanSequence),
    ReferencedBeamNumber(copy.ReferencedBeamNumber),
    ReferencedFractionGroupNumber(copy.ReferencedFractionGroupNumber),
    FractionNumber(copy.FractionNumber),
    StartCumulativeMetersetWeight(copy.StartCumulativeMetersetWeight),
    EndCumulativeMetersetWeight(copy.EndCumulativeMetersetWeight),
    ExposureSequence(copy.ExposureSequence),
    FluenceMapSequence(copy.FluenceMapSequence),
    GantryAngle(copy.GantryAngle),
    GantryPitchAngle(copy.GantryPitchAngle),
    BeamLimitingDeviceAngle(copy.BeamLimitingDeviceAngle),
    PatientSupportAngle(copy.PatientSupportAngle),
    TableTopEccentricAxisDistance(copy.TableTopEccentricAxisDistance),
    TableTopEccentricAngle(copy.TableTopEccentricAngle),
    TableTopVerticalPosition(copy.TableTopVerticalPosition),
    TableTopLongitudinalPosition(copy.TableTopLongitudinalPosition),
    TableTopLateralPosition(copy.TableTopLateralPosition),
    IsocenterPosition(copy.IsocenterPosition),
    PatientPosition(copy.PatientPosition),
    ModalityLUTSequence(copy.ModalityLUTSequence),
    RescaleIntercept(copy.RescaleIntercept),
    RescaleSlope(copy.RescaleSlope),
    RescaleType(copy.RescaleType),
    VOILUTSequence(copy.VOILUTSequence),
    WindowCenter(copy.WindowCenter),
    WindowWidth(copy.WindowWidth),
    WindowCenterWidthExplanation(copy.WindowCenterWidthExplanation),
    VOILUTFunction(copy.VOILUTFunction),
    ApprovalStatus(copy.ApprovalStatus),
    ReviewDate(copy.ReviewDate),
    ReviewTime(copy.ReviewTime),
    ReviewerName(copy.ReviewerName),
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


DRTImageIOD::~DRTImageIOD()
{
}


DRTImageIOD &DRTImageIOD::operator=(const DRTImageIOD &copy)
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
        InstanceNumber = copy.InstanceNumber;
        PatientOrientation = copy.PatientOrientation;
        ContentDate = copy.ContentDate;
        ContentTime = copy.ContentTime;
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
        ContrastBolusAgent = copy.ContrastBolusAgent;
        ContrastBolusAgentSequence = copy.ContrastBolusAgentSequence;
        ContrastBolusRoute = copy.ContrastBolusRoute;
        ContrastBolusAdministrationRouteSequence = copy.ContrastBolusAdministrationRouteSequence;
        ContrastBolusVolume = copy.ContrastBolusVolume;
        ContrastBolusStartTime = copy.ContrastBolusStartTime;
        ContrastBolusStopTime = copy.ContrastBolusStopTime;
        ContrastBolusTotalDose = copy.ContrastBolusTotalDose;
        ContrastFlowRate = copy.ContrastFlowRate;
        ContrastFlowDuration = copy.ContrastFlowDuration;
        ContrastBolusIngredient = copy.ContrastBolusIngredient;
        ContrastBolusIngredientConcentration = copy.ContrastBolusIngredientConcentration;
        PreferredPlaybackSequencing = copy.PreferredPlaybackSequencing;
        FrameTime = copy.FrameTime;
        FrameTimeVector = copy.FrameTimeVector;
        StartTrim = copy.StartTrim;
        StopTrim = copy.StopTrim;
        RecommendedDisplayFrameRate = copy.RecommendedDisplayFrameRate;
        CineRate = copy.CineRate;
        FrameDelay = copy.FrameDelay;
        ImageTriggerDelay = copy.ImageTriggerDelay;
        EffectiveDuration = copy.EffectiveDuration;
        ActualFrameDuration = copy.ActualFrameDuration;
        MultiplexedAudioChannelsDescriptionCodeSequence = copy.MultiplexedAudioChannelsDescriptionCodeSequence;
        NumberOfFrames = copy.NumberOfFrames;
        FrameIncrementPointer = copy.FrameIncrementPointer;
        DeviceSequence = copy.DeviceSequence;
        SamplesPerPixel = copy.SamplesPerPixel;
        PhotometricInterpretation = copy.PhotometricInterpretation;
        BitsAllocated = copy.BitsAllocated;
        BitsStored = copy.BitsStored;
        HighBit = copy.HighBit;
        PixelRepresentation = copy.PixelRepresentation;
        PixelIntensityRelationship = copy.PixelIntensityRelationship;
        PixelIntensityRelationshipSign = copy.PixelIntensityRelationshipSign;
        RTImageLabel = copy.RTImageLabel;
        RTImageName = copy.RTImageName;
        RTImageDescription = copy.RTImageDescription;
        OperatorsName = copy.OperatorsName;
        ImageType = copy.ImageType;
        ConversionType = copy.ConversionType;
        ReportedValuesOrigin = copy.ReportedValuesOrigin;
        RTImagePlane = copy.RTImagePlane;
        XRayImageReceptorTranslation = copy.XRayImageReceptorTranslation;
        XRayImageReceptorAngle = copy.XRayImageReceptorAngle;
        RTImageOrientation = copy.RTImageOrientation;
        ImagePlanePixelSpacing = copy.ImagePlanePixelSpacing;
        RTImagePosition = copy.RTImagePosition;
        RadiationMachineName = copy.RadiationMachineName;
        PrimaryDosimeterUnit = copy.PrimaryDosimeterUnit;
        RadiationMachineSAD = copy.RadiationMachineSAD;
        RadiationMachineSSD = copy.RadiationMachineSSD;
        RTImageSID = copy.RTImageSID;
        SourceToReferenceObjectDistance = copy.SourceToReferenceObjectDistance;
        ReferencedRTPlanSequence = copy.ReferencedRTPlanSequence;
        ReferencedBeamNumber = copy.ReferencedBeamNumber;
        ReferencedFractionGroupNumber = copy.ReferencedFractionGroupNumber;
        FractionNumber = copy.FractionNumber;
        StartCumulativeMetersetWeight = copy.StartCumulativeMetersetWeight;
        EndCumulativeMetersetWeight = copy.EndCumulativeMetersetWeight;
        ExposureSequence = copy.ExposureSequence;
        FluenceMapSequence = copy.FluenceMapSequence;
        GantryAngle = copy.GantryAngle;
        GantryPitchAngle = copy.GantryPitchAngle;
        BeamLimitingDeviceAngle = copy.BeamLimitingDeviceAngle;
        PatientSupportAngle = copy.PatientSupportAngle;
        TableTopEccentricAxisDistance = copy.TableTopEccentricAxisDistance;
        TableTopEccentricAngle = copy.TableTopEccentricAngle;
        TableTopVerticalPosition = copy.TableTopVerticalPosition;
        TableTopLongitudinalPosition = copy.TableTopLongitudinalPosition;
        TableTopLateralPosition = copy.TableTopLateralPosition;
        IsocenterPosition = copy.IsocenterPosition;
        PatientPosition = copy.PatientPosition;
        ModalityLUTSequence = copy.ModalityLUTSequence;
        RescaleIntercept = copy.RescaleIntercept;
        RescaleSlope = copy.RescaleSlope;
        RescaleType = copy.RescaleType;
        VOILUTSequence = copy.VOILUTSequence;
        WindowCenter = copy.WindowCenter;
        WindowWidth = copy.WindowWidth;
        WindowCenterWidthExplanation = copy.WindowCenterWidthExplanation;
        VOILUTFunction = copy.VOILUTFunction;
        ApprovalStatus = copy.ApprovalStatus;
        ReviewDate = copy.ReviewDate;
        ReviewTime = copy.ReviewTime;
        ReviewerName = copy.ReviewerName;
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


void DRTImageIOD::clear()
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
    ContrastBolusAgent.clear();
    ContrastBolusAgentSequence.clear();
    ContrastBolusRoute.clear();
    ContrastBolusAdministrationRouteSequence.clear();
    ContrastBolusVolume.clear();
    ContrastBolusStartTime.clear();
    ContrastBolusStopTime.clear();
    ContrastBolusTotalDose.clear();
    ContrastFlowRate.clear();
    ContrastFlowDuration.clear();
    ContrastBolusIngredient.clear();
    ContrastBolusIngredientConcentration.clear();
    PreferredPlaybackSequencing.clear();
    FrameTime.clear();
    FrameTimeVector.clear();
    StartTrim.clear();
    StopTrim.clear();
    RecommendedDisplayFrameRate.clear();
    CineRate.clear();
    FrameDelay.clear();
    ImageTriggerDelay.clear();
    EffectiveDuration.clear();
    ActualFrameDuration.clear();
    MultiplexedAudioChannelsDescriptionCodeSequence.clear();
    NumberOfFrames.clear();
    FrameIncrementPointer.clear();
    DeviceSequence.clear();
    PixelIntensityRelationship.clear();
    PixelIntensityRelationshipSign.clear();
    RTImageLabel.clear();
    RTImageName.clear();
    RTImageDescription.clear();
    OperatorsName.clear();
    ConversionType.clear();
    ReportedValuesOrigin.clear();
    RTImagePlane.clear();
    XRayImageReceptorTranslation.clear();
    XRayImageReceptorAngle.clear();
    RTImageOrientation.clear();
    ImagePlanePixelSpacing.clear();
    RTImagePosition.clear();
    RadiationMachineName.clear();
    PrimaryDosimeterUnit.clear();
    RadiationMachineSAD.clear();
    RadiationMachineSSD.clear();
    RTImageSID.clear();
    SourceToReferenceObjectDistance.clear();
    ReferencedRTPlanSequence.clear();
    ReferencedBeamNumber.clear();
    ReferencedFractionGroupNumber.clear();
    FractionNumber.clear();
    StartCumulativeMetersetWeight.clear();
    EndCumulativeMetersetWeight.clear();
    ExposureSequence.clear();
    FluenceMapSequence.clear();
    GantryAngle.clear();
    GantryPitchAngle.clear();
    BeamLimitingDeviceAngle.clear();
    PatientSupportAngle.clear();
    TableTopEccentricAxisDistance.clear();
    TableTopEccentricAngle.clear();
    TableTopVerticalPosition.clear();
    TableTopLongitudinalPosition.clear();
    TableTopLateralPosition.clear();
    IsocenterPosition.clear();
    PatientPosition.clear();
    ModalityLUTSequence.clear();
    RescaleIntercept.clear();
    RescaleSlope.clear();
    RescaleType.clear();
    VOILUTSequence.clear();
    WindowCenter.clear();
    WindowWidth.clear();
    WindowCenterWidthExplanation.clear();
    VOILUTFunction.clear();
    ApprovalStatus.clear();
    ReviewDate.clear();
    ReviewTime.clear();
    ReviewerName.clear();
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


OFBool DRTImageIOD::isValid()
{
    /* tbd: check whether object is valid */
    return OFTrue;
}


OFCondition DRTImageIOD::checkDatasetForReading(DcmItem &dataset)
{
    OFCondition result = EC_Normal;
    /* check SOP class UID */
    DcmUniqueIdentifier sopClassUID(DCM_SOPClassUID);
    result = getAndCheckElementFromDataset(dataset, sopClassUID, "1", "1", "SOPCommonModule");
    if (result.good())
    {
        OFString tmpString;
        getStringValueFromElement(sopClassUID, tmpString);
        if (tmpString != UID_RTImageStorage)
        {
            DCMRT_ERROR("Invalid value for attribute SOPClassUID (0008,0016)");
            result = RT_EC_InvalidValue;
        }
    }
    return result;
}


void DRTImageIOD::updateAttributes()
{
    SOPClassUID.putString(UID_RTImageStorage);
}


OFCondition DRTImageIOD::read(DcmItem &dataset)
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
        getAndCheckElementFromDataset(dataset, SpatialResolution, "1", "3", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, DateOfLastCalibration, "1-n", "3", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, TimeOfLastCalibration, "1-n", "3", "GeneralEquipmentModule");
        getAndCheckElementFromDataset(dataset, PixelPaddingValue, "1", "3", "GeneralEquipmentModule");

        // --- GeneralImageModule (M) ---
        getAndCheckElementFromDataset(dataset, InstanceNumber, "1", "2", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, PatientOrientation, "2", "2C", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, ContentDate, "1", "2C", "GeneralImageModule");
        getAndCheckElementFromDataset(dataset, ContentTime, "1", "2C", "GeneralImageModule");
        // getAndCheckElementFromDataset(dataset, ImageType, "2-n", "3", "GeneralImageModule");
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

        // --- ImagePixelModule (M) ---
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

        // --- ContrastBolusModule (C) ---
        if (dataset.tagExists(DCM_ContrastBolusAgent))
        {
            getAndCheckElementFromDataset(dataset, ContrastBolusAgent, "1", "2", "ContrastBolusModule");
            ContrastBolusAgentSequence.read(dataset, "1-n", "3", "ContrastBolusModule");
            getAndCheckElementFromDataset(dataset, ContrastBolusRoute, "1", "3", "ContrastBolusModule");
            ContrastBolusAdministrationRouteSequence.read(dataset, "1-n", "3", "ContrastBolusModule");
            getAndCheckElementFromDataset(dataset, ContrastBolusVolume, "1", "3", "ContrastBolusModule");
            getAndCheckElementFromDataset(dataset, ContrastBolusStartTime, "1", "3", "ContrastBolusModule");
            getAndCheckElementFromDataset(dataset, ContrastBolusStopTime, "1", "3", "ContrastBolusModule");
            getAndCheckElementFromDataset(dataset, ContrastBolusTotalDose, "1", "3", "ContrastBolusModule");
            getAndCheckElementFromDataset(dataset, ContrastFlowRate, "1-n", "3", "ContrastBolusModule");
            getAndCheckElementFromDataset(dataset, ContrastFlowDuration, "1-n", "3", "ContrastBolusModule");
            getAndCheckElementFromDataset(dataset, ContrastBolusIngredient, "1", "3", "ContrastBolusModule");
            getAndCheckElementFromDataset(dataset, ContrastBolusIngredientConcentration, "1", "3", "ContrastBolusModule");
        }

        // --- CineModule (C) ---
        getAndCheckElementFromDataset(dataset, PreferredPlaybackSequencing, "1", "3", "CineModule");
        getAndCheckElementFromDataset(dataset, FrameTime, "1", "1C", "CineModule");
        getAndCheckElementFromDataset(dataset, FrameTimeVector, "1-n", "1C", "CineModule");
        getAndCheckElementFromDataset(dataset, StartTrim, "1", "3", "CineModule");
        getAndCheckElementFromDataset(dataset, StopTrim, "1", "3", "CineModule");
        getAndCheckElementFromDataset(dataset, RecommendedDisplayFrameRate, "1", "3", "CineModule");
        getAndCheckElementFromDataset(dataset, CineRate, "1", "3", "CineModule");
        getAndCheckElementFromDataset(dataset, FrameDelay, "1", "3", "CineModule");
        getAndCheckElementFromDataset(dataset, ImageTriggerDelay, "1", "3", "CineModule");
        getAndCheckElementFromDataset(dataset, EffectiveDuration, "1", "3", "CineModule");
        getAndCheckElementFromDataset(dataset, ActualFrameDuration, "1", "3", "CineModule");
        MultiplexedAudioChannelsDescriptionCodeSequence.read(dataset, "1-n", "2C", "CineModule");

        // --- MultiFrameModule (C) ---
        if (dataset.tagExists(DCM_NumberOfFrames) ||
            dataset.tagExists(DCM_FrameIncrementPointer))
        {
            getAndCheckElementFromDataset(dataset, NumberOfFrames, "1", "1", "MultiFrameModule");
            getAndCheckElementFromDataset(dataset, FrameIncrementPointer, "1-n", "1", "MultiFrameModule");
        }

        // --- DeviceModule (U) ---
        DeviceSequence.read(dataset, "1-n", "3", "DeviceModule");

        // --- RTImageModule (M) ---
        getAndCheckElementFromDataset(dataset, SamplesPerPixel, "1", "1", "RTImageModule");
        getAndCheckElementFromDataset(dataset, PhotometricInterpretation, "1", "1", "RTImageModule");
        getAndCheckElementFromDataset(dataset, BitsAllocated, "1", "1", "RTImageModule");
        getAndCheckElementFromDataset(dataset, BitsStored, "1", "1", "RTImageModule");
        getAndCheckElementFromDataset(dataset, HighBit, "1", "1", "RTImageModule");
        getAndCheckElementFromDataset(dataset, PixelRepresentation, "1", "1", "RTImageModule");
        getAndCheckElementFromDataset(dataset, PixelIntensityRelationship, "1", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, PixelIntensityRelationshipSign, "1", "1C", "RTImageModule");
        getAndCheckElementFromDataset(dataset, RTImageLabel, "1", "1", "RTImageModule");
        getAndCheckElementFromDataset(dataset, RTImageName, "1", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, RTImageDescription, "1", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, OperatorsName, "1-n", "2", "RTImageModule");
        getAndCheckElementFromDataset(dataset, ImageType, "2-n", "1", "RTImageModule");
        getAndCheckElementFromDataset(dataset, ConversionType, "1", "2", "RTImageModule");
        getAndCheckElementFromDataset(dataset, ReportedValuesOrigin, "1", "2C", "RTImageModule");
        getAndCheckElementFromDataset(dataset, RTImagePlane, "1", "1", "RTImageModule");
        getAndCheckElementFromDataset(dataset, XRayImageReceptorTranslation, "3", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, XRayImageReceptorAngle, "1", "2", "RTImageModule");
        getAndCheckElementFromDataset(dataset, RTImageOrientation, "6", "2C", "RTImageModule");
        getAndCheckElementFromDataset(dataset, ImagePlanePixelSpacing, "2", "2", "RTImageModule");
        getAndCheckElementFromDataset(dataset, RTImagePosition, "2", "2", "RTImageModule");
        getAndCheckElementFromDataset(dataset, RadiationMachineName, "1", "2", "RTImageModule");
        getAndCheckElementFromDataset(dataset, PrimaryDosimeterUnit, "1", "2", "RTImageModule");
        getAndCheckElementFromDataset(dataset, RadiationMachineSAD, "1", "2", "RTImageModule");
        getAndCheckElementFromDataset(dataset, RadiationMachineSSD, "1", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, RTImageSID, "1", "2", "RTImageModule");
        getAndCheckElementFromDataset(dataset, SourceToReferenceObjectDistance, "1", "3", "RTImageModule");
        ReferencedRTPlanSequence.read(dataset, "1-n", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, ReferencedBeamNumber, "1", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, ReferencedFractionGroupNumber, "1", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, FractionNumber, "1", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, StartCumulativeMetersetWeight, "1", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, EndCumulativeMetersetWeight, "1", "3", "RTImageModule");
        ExposureSequence.read(dataset, "1-n", "3", "RTImageModule");
        FluenceMapSequence.read(dataset, "1-n", "1C", "RTImageModule");
        getAndCheckElementFromDataset(dataset, GantryAngle, "1", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, GantryPitchAngle, "1", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, BeamLimitingDeviceAngle, "1", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, PatientSupportAngle, "1", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, TableTopEccentricAxisDistance, "1", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, TableTopEccentricAngle, "1", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, TableTopVerticalPosition, "1", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, TableTopLongitudinalPosition, "1", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, TableTopLateralPosition, "1", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, IsocenterPosition, "3", "3", "RTImageModule");
        getAndCheckElementFromDataset(dataset, PatientPosition, "1", "1C", "RTImageModule");

        // --- ModalityLUTModule (U) ---
        ModalityLUTSequence.read(dataset, "1-n", "1C", "ModalityLUTModule");
        getAndCheckElementFromDataset(dataset, RescaleIntercept, "1", "1C", "ModalityLUTModule");
        getAndCheckElementFromDataset(dataset, RescaleSlope, "1", "1C", "ModalityLUTModule");
        getAndCheckElementFromDataset(dataset, RescaleType, "1", "1C", "ModalityLUTModule");

        // --- VOILUTModule (U) ---
        VOILUTSequence.read(dataset, "1-n", "1C", "VOILUTModule");
        getAndCheckElementFromDataset(dataset, WindowCenter, "1-n", "1C", "VOILUTModule");
        getAndCheckElementFromDataset(dataset, WindowWidth, "1-n", "1C", "VOILUTModule");
        getAndCheckElementFromDataset(dataset, WindowCenterWidthExplanation, "1-n", "3", "VOILUTModule");
        getAndCheckElementFromDataset(dataset, VOILUTFunction, "1", "3", "VOILUTModule");

        // --- ApprovalModule (U) ---
        if (dataset.tagExists(DCM_ApprovalStatus))
        {
            getAndCheckElementFromDataset(dataset, ApprovalStatus, "1", "1", "ApprovalModule");
            getAndCheckElementFromDataset(dataset, ReviewDate, "1", "2C", "ApprovalModule");
            getAndCheckElementFromDataset(dataset, ReviewTime, "1", "2C", "ApprovalModule");
            getAndCheckElementFromDataset(dataset, ReviewerName, "1", "2C", "ApprovalModule");
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


OFCondition DRTImageIOD::readPatientData(DcmItem &dataset)
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


OFCondition DRTImageIOD::readStudyData(DcmItem &dataset)
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


OFCondition DRTImageIOD::readSeriesData(DcmItem &dataset)
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


OFCondition DRTImageIOD::write(DcmItem &dataset)
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
        addElementToDataset(result, dataset, new DcmDecimalString(SpatialResolution), "1", "3", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmDate(DateOfLastCalibration), "1-n", "3", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmTime(TimeOfLastCalibration), "1-n", "3", "GeneralEquipmentModule");
        addElementToDataset(result, dataset, new DcmUnsignedShort(PixelPaddingValue), "1", "3", "GeneralEquipmentModule");

        // --- GeneralImageModule (M) ---
        addElementToDataset(result, dataset, new DcmIntegerString(InstanceNumber), "1", "2", "GeneralImageModule");
        addElementToDataset(result, dataset, new DcmCodeString(PatientOrientation), "2", "2C", "GeneralImageModule");
        addElementToDataset(result, dataset, new DcmDate(ContentDate), "1", "2C", "GeneralImageModule");
        addElementToDataset(result, dataset, new DcmTime(ContentTime), "1", "2C", "GeneralImageModule");
        // addElementToDataset(result, dataset, new DcmCodeString(ImageType), "2-n", "3", "GeneralImageModule");
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

        // --- ImagePixelModule (M) ---
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

        // --- ContrastBolusModule (C) ---
        if (isContrastBolusModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmLongString(ContrastBolusAgent), "1", "2", "ContrastBolusModule");
            if (result.good()) result = ContrastBolusAgentSequence.write(dataset, "1-n" ,"3", "ContrastBolusModule");
            addElementToDataset(result, dataset, new DcmLongString(ContrastBolusRoute), "1", "3", "ContrastBolusModule");
            if (result.good()) result = ContrastBolusAdministrationRouteSequence.write(dataset, "1-n" ,"3", "ContrastBolusModule");
            addElementToDataset(result, dataset, new DcmDecimalString(ContrastBolusVolume), "1", "3", "ContrastBolusModule");
            addElementToDataset(result, dataset, new DcmTime(ContrastBolusStartTime), "1", "3", "ContrastBolusModule");
            addElementToDataset(result, dataset, new DcmTime(ContrastBolusStopTime), "1", "3", "ContrastBolusModule");
            addElementToDataset(result, dataset, new DcmDecimalString(ContrastBolusTotalDose), "1", "3", "ContrastBolusModule");
            addElementToDataset(result, dataset, new DcmDecimalString(ContrastFlowRate), "1-n", "3", "ContrastBolusModule");
            addElementToDataset(result, dataset, new DcmDecimalString(ContrastFlowDuration), "1-n", "3", "ContrastBolusModule");
            addElementToDataset(result, dataset, new DcmCodeString(ContrastBolusIngredient), "1", "3", "ContrastBolusModule");
            addElementToDataset(result, dataset, new DcmDecimalString(ContrastBolusIngredientConcentration), "1", "3", "ContrastBolusModule");
        }

        // --- CineModule (C) ---
        if (isCineModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmUnsignedShort(PreferredPlaybackSequencing), "1", "3", "CineModule");
            addElementToDataset(result, dataset, new DcmDecimalString(FrameTime), "1", "1C", "CineModule");
            addElementToDataset(result, dataset, new DcmDecimalString(FrameTimeVector), "1-n", "1C", "CineModule");
            addElementToDataset(result, dataset, new DcmIntegerString(StartTrim), "1", "3", "CineModule");
            addElementToDataset(result, dataset, new DcmIntegerString(StopTrim), "1", "3", "CineModule");
            addElementToDataset(result, dataset, new DcmIntegerString(RecommendedDisplayFrameRate), "1", "3", "CineModule");
            addElementToDataset(result, dataset, new DcmIntegerString(CineRate), "1", "3", "CineModule");
            addElementToDataset(result, dataset, new DcmDecimalString(FrameDelay), "1", "3", "CineModule");
            addElementToDataset(result, dataset, new DcmDecimalString(ImageTriggerDelay), "1", "3", "CineModule");
            addElementToDataset(result, dataset, new DcmDecimalString(EffectiveDuration), "1", "3", "CineModule");
            addElementToDataset(result, dataset, new DcmIntegerString(ActualFrameDuration), "1", "3", "CineModule");
            if (result.good()) result = MultiplexedAudioChannelsDescriptionCodeSequence.write(dataset, "1-n" ,"2C", "CineModule");
        }

        // --- MultiFrameModule (C) ---
        if (isMultiFrameModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmIntegerString(NumberOfFrames), "1", "1", "MultiFrameModule");
            addElementToDataset(result, dataset, new DcmAttributeTag(FrameIncrementPointer), "1-n", "1", "MultiFrameModule");
        }

        // --- DeviceModule (U) ---
        if (isDeviceModulePresent(OFFalse /*complete*/))
        {
            if (result.good()) result = DeviceSequence.write(dataset, "1-n" ,"3", "DeviceModule");
        }

        // --- RTImageModule (M) ---
        addElementToDataset(result, dataset, new DcmUnsignedShort(SamplesPerPixel), "1", "1", "RTImageModule");
        addElementToDataset(result, dataset, new DcmCodeString(PhotometricInterpretation), "1", "1", "RTImageModule");
        addElementToDataset(result, dataset, new DcmUnsignedShort(BitsAllocated), "1", "1", "RTImageModule");
        addElementToDataset(result, dataset, new DcmUnsignedShort(BitsStored), "1", "1", "RTImageModule");
        addElementToDataset(result, dataset, new DcmUnsignedShort(HighBit), "1", "1", "RTImageModule");
        addElementToDataset(result, dataset, new DcmUnsignedShort(PixelRepresentation), "1", "1", "RTImageModule");
        addElementToDataset(result, dataset, new DcmCodeString(PixelIntensityRelationship), "1", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmSignedShort(PixelIntensityRelationshipSign), "1", "1C", "RTImageModule");
        addElementToDataset(result, dataset, new DcmShortString(RTImageLabel), "1", "1", "RTImageModule");
        addElementToDataset(result, dataset, new DcmLongString(RTImageName), "1", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmShortText(RTImageDescription), "1", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmPersonName(OperatorsName), "1-n", "2", "RTImageModule");
        addElementToDataset(result, dataset, new DcmCodeString(ImageType), "2-n", "1", "RTImageModule");
        addElementToDataset(result, dataset, new DcmCodeString(ConversionType), "1", "2", "RTImageModule");
        addElementToDataset(result, dataset, new DcmCodeString(ReportedValuesOrigin), "1", "2C", "RTImageModule");
        addElementToDataset(result, dataset, new DcmCodeString(RTImagePlane), "1", "1", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(XRayImageReceptorTranslation), "3", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(XRayImageReceptorAngle), "1", "2", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(RTImageOrientation), "6", "2C", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(ImagePlanePixelSpacing), "2", "2", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(RTImagePosition), "2", "2", "RTImageModule");
        addElementToDataset(result, dataset, new DcmShortString(RadiationMachineName), "1", "2", "RTImageModule");
        addElementToDataset(result, dataset, new DcmCodeString(PrimaryDosimeterUnit), "1", "2", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(RadiationMachineSAD), "1", "2", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(RadiationMachineSSD), "1", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(RTImageSID), "1", "2", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(SourceToReferenceObjectDistance), "1", "3", "RTImageModule");
        if (result.good()) result = ReferencedRTPlanSequence.write(dataset, "1-n" ,"3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmIntegerString(ReferencedBeamNumber), "1", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmIntegerString(ReferencedFractionGroupNumber), "1", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmIntegerString(FractionNumber), "1", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(StartCumulativeMetersetWeight), "1", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(EndCumulativeMetersetWeight), "1", "3", "RTImageModule");
        if (result.good()) result = ExposureSequence.write(dataset, "1-n" ,"3", "RTImageModule");
        if (result.good()) result = FluenceMapSequence.write(dataset, "1-n" ,"1C", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(GantryAngle), "1", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmFloatingPointSingle(GantryPitchAngle), "1", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(BeamLimitingDeviceAngle), "1", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(PatientSupportAngle), "1", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(TableTopEccentricAxisDistance), "1", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(TableTopEccentricAngle), "1", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(TableTopVerticalPosition), "1", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(TableTopLongitudinalPosition), "1", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(TableTopLateralPosition), "1", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmDecimalString(IsocenterPosition), "3", "3", "RTImageModule");
        addElementToDataset(result, dataset, new DcmCodeString(PatientPosition), "1", "1C", "RTImageModule");

        // --- ModalityLUTModule (U) ---
        if (isModalityLUTModulePresent(OFFalse /*complete*/))
        {
            if (result.good()) result = ModalityLUTSequence.write(dataset, "1-n" ,"1C", "ModalityLUTModule");
            addElementToDataset(result, dataset, new DcmDecimalString(RescaleIntercept), "1", "1C", "ModalityLUTModule");
            addElementToDataset(result, dataset, new DcmDecimalString(RescaleSlope), "1", "1C", "ModalityLUTModule");
            addElementToDataset(result, dataset, new DcmLongString(RescaleType), "1", "1C", "ModalityLUTModule");
        }

        // --- VOILUTModule (U) ---
        if (isVOILUTModulePresent(OFFalse /*complete*/))
        {
            if (result.good()) result = VOILUTSequence.write(dataset, "1-n" ,"1C", "VOILUTModule");
            addElementToDataset(result, dataset, new DcmDecimalString(WindowCenter), "1-n", "1C", "VOILUTModule");
            addElementToDataset(result, dataset, new DcmDecimalString(WindowWidth), "1-n", "1C", "VOILUTModule");
            addElementToDataset(result, dataset, new DcmLongString(WindowCenterWidthExplanation), "1-n", "3", "VOILUTModule");
            addElementToDataset(result, dataset, new DcmCodeString(VOILUTFunction), "1", "3", "VOILUTModule");
        }

        // --- ApprovalModule (U) ---
        if (isApprovalModulePresent(OFFalse /*complete*/))
        {
            addElementToDataset(result, dataset, new DcmCodeString(ApprovalStatus), "1", "1", "ApprovalModule");
            addElementToDataset(result, dataset, new DcmDate(ReviewDate), "1", "2C", "ApprovalModule");
            addElementToDataset(result, dataset, new DcmTime(ReviewTime), "1", "2C", "ApprovalModule");
            addElementToDataset(result, dataset, new DcmPersonName(ReviewerName), "1", "2C", "ApprovalModule");
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


OFBool DRTImageIOD::isClinicalTrialSubjectModulePresent(const OFBool complete)
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


OFBool DRTImageIOD::isPatientStudyModulePresent(const OFBool /*complete*/)
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


OFBool DRTImageIOD::isClinicalTrialStudyModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !ClinicalTrialTimePointID.isEmpty() ||
           !ClinicalTrialTimePointDescription.isEmpty();
}


OFBool DRTImageIOD::isClinicalTrialSeriesModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !ClinicalTrialCoordinatingCenterName.isEmpty();
}


OFBool DRTImageIOD::isFrameOfReferenceModulePresent(const OFBool complete)
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


OFBool DRTImageIOD::isContrastBolusModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !ContrastBolusAgent.isEmpty() ||
           !ContrastBolusAgentSequence.isEmpty() ||
           !ContrastBolusRoute.isEmpty() ||
           !ContrastBolusAdministrationRouteSequence.isEmpty() ||
           !ContrastBolusVolume.isEmpty() ||
           !ContrastBolusStartTime.isEmpty() ||
           !ContrastBolusStopTime.isEmpty() ||
           !ContrastBolusTotalDose.isEmpty() ||
           !ContrastFlowRate.isEmpty() ||
           !ContrastFlowDuration.isEmpty() ||
           !ContrastBolusIngredient.isEmpty() ||
           !ContrastBolusIngredientConcentration.isEmpty();
}


OFBool DRTImageIOD::isCineModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !PreferredPlaybackSequencing.isEmpty() ||
           !FrameTime.isEmpty() ||
           !FrameTimeVector.isEmpty() ||
           !StartTrim.isEmpty() ||
           !StopTrim.isEmpty() ||
           !RecommendedDisplayFrameRate.isEmpty() ||
           !CineRate.isEmpty() ||
           !FrameDelay.isEmpty() ||
           !ImageTriggerDelay.isEmpty() ||
           !EffectiveDuration.isEmpty() ||
           !ActualFrameDuration.isEmpty() ||
           !MultiplexedAudioChannelsDescriptionCodeSequence.isEmpty();
}


OFBool DRTImageIOD::isMultiFrameModulePresent(const OFBool complete)
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


OFBool DRTImageIOD::isDeviceModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !DeviceSequence.isEmpty();
}


OFBool DRTImageIOD::isModalityLUTModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !ModalityLUTSequence.isEmpty() ||
           !RescaleIntercept.isEmpty() ||
           !RescaleSlope.isEmpty() ||
           !RescaleType.isEmpty();
}


OFBool DRTImageIOD::isVOILUTModulePresent(const OFBool /*complete*/)
{
    /* check whether at least one attribute is present */
    return !VOILUTSequence.isEmpty() ||
           !WindowCenter.isEmpty() ||
           !WindowWidth.isEmpty() ||
           !WindowCenterWidthExplanation.isEmpty() ||
           !VOILUTFunction.isEmpty();
}


OFBool DRTImageIOD::isApprovalModulePresent(const OFBool complete)
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


OFCondition DRTImageIOD::getAccessionNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AccessionNumber, value, pos);
}


OFCondition DRTImageIOD::getAcquisitionDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AcquisitionDate, value, pos);
}


OFCondition DRTImageIOD::getAcquisitionDateTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AcquisitionDateTime, value, pos);
}


OFCondition DRTImageIOD::getAcquisitionNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AcquisitionNumber, value, pos);
}


OFCondition DRTImageIOD::getAcquisitionNumber(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, AcquisitionNumber).getSint32(value, pos);
}


OFCondition DRTImageIOD::getAcquisitionTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AcquisitionTime, value, pos);
}


OFCondition DRTImageIOD::getActualFrameDuration(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ActualFrameDuration, value, pos);
}


OFCondition DRTImageIOD::getActualFrameDuration(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, ActualFrameDuration).getSint32(value, pos);
}


OFCondition DRTImageIOD::getAdditionalPatientHistory(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AdditionalPatientHistory, value, pos);
}


OFCondition DRTImageIOD::getAdmissionID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AdmissionID, value, pos);
}


OFCondition DRTImageIOD::getAdmittingDiagnosesDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AdmittingDiagnosesDescription, value, pos);
}


OFCondition DRTImageIOD::getApprovalStatus(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ApprovalStatus, value, pos);
}


OFCondition DRTImageIOD::getAuthorizationEquipmentCertificationNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(AuthorizationEquipmentCertificationNumber, value, pos);
}


OFCondition DRTImageIOD::getBeamLimitingDeviceAngle(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(BeamLimitingDeviceAngle, value, pos);
}


OFCondition DRTImageIOD::getBeamLimitingDeviceAngle(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, BeamLimitingDeviceAngle).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getBitsAllocated(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, BitsAllocated).getUint16(value, pos);
}


OFCondition DRTImageIOD::getBitsStored(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, BitsStored).getUint16(value, pos);
}


OFCondition DRTImageIOD::getBluePaletteColorLookupTableData(Uint16 *&value, unsigned long *count) const
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


OFCondition DRTImageIOD::getBluePaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, BluePaletteColorLookupTableDescriptor).getUint16(value, pos);
}


OFCondition DRTImageIOD::getBurnedInAnnotation(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(BurnedInAnnotation, value, pos);
}


OFCondition DRTImageIOD::getCineRate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(CineRate, value, pos);
}


OFCondition DRTImageIOD::getCineRate(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, CineRate).getSint32(value, pos);
}


OFCondition DRTImageIOD::getClinicalTrialCoordinatingCenterName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialCoordinatingCenterName, value, pos);
}


OFCondition DRTImageIOD::getClinicalTrialProtocolID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialProtocolID, value, pos);
}


OFCondition DRTImageIOD::getClinicalTrialProtocolName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialProtocolName, value, pos);
}


OFCondition DRTImageIOD::getClinicalTrialSiteID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSiteID, value, pos);
}


OFCondition DRTImageIOD::getClinicalTrialSiteName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSiteName, value, pos);
}


OFCondition DRTImageIOD::getClinicalTrialSponsorName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSponsorName, value, pos);
}


OFCondition DRTImageIOD::getClinicalTrialSubjectID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSubjectID, value, pos);
}


OFCondition DRTImageIOD::getClinicalTrialSubjectReadingID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialSubjectReadingID, value, pos);
}


OFCondition DRTImageIOD::getClinicalTrialTimePointDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialTimePointDescription, value, pos);
}


OFCondition DRTImageIOD::getClinicalTrialTimePointID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ClinicalTrialTimePointID, value, pos);
}


OFCondition DRTImageIOD::getColumns(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, Columns).getUint16(value, pos);
}


OFCondition DRTImageIOD::getContentDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ContentDate, value, pos);
}


OFCondition DRTImageIOD::getContentTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ContentTime, value, pos);
}


OFCondition DRTImageIOD::getContrastBolusAgent(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ContrastBolusAgent, value, pos);
}


OFCondition DRTImageIOD::getContrastBolusIngredient(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ContrastBolusIngredient, value, pos);
}


OFCondition DRTImageIOD::getContrastBolusIngredientConcentration(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ContrastBolusIngredientConcentration, value, pos);
}


OFCondition DRTImageIOD::getContrastBolusIngredientConcentration(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, ContrastBolusIngredientConcentration).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getContrastBolusRoute(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ContrastBolusRoute, value, pos);
}


OFCondition DRTImageIOD::getContrastBolusStartTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ContrastBolusStartTime, value, pos);
}


OFCondition DRTImageIOD::getContrastBolusStopTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ContrastBolusStopTime, value, pos);
}


OFCondition DRTImageIOD::getContrastBolusTotalDose(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ContrastBolusTotalDose, value, pos);
}


OFCondition DRTImageIOD::getContrastBolusTotalDose(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, ContrastBolusTotalDose).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getContrastBolusVolume(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ContrastBolusVolume, value, pos);
}


OFCondition DRTImageIOD::getContrastBolusVolume(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, ContrastBolusVolume).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getContrastFlowDuration(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ContrastFlowDuration, value, pos);
}


OFCondition DRTImageIOD::getContrastFlowDuration(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, ContrastFlowDuration).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getContrastFlowDuration(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, ContrastFlowDuration).getFloat64Vector(value);
}


OFCondition DRTImageIOD::getContrastFlowRate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ContrastFlowRate, value, pos);
}


OFCondition DRTImageIOD::getContrastFlowRate(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, ContrastFlowRate).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getContrastFlowRate(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, ContrastFlowRate).getFloat64Vector(value);
}


OFCondition DRTImageIOD::getConversionType(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ConversionType, value, pos);
}


OFCondition DRTImageIOD::getDateOfLastCalibration(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DateOfLastCalibration, value, pos);
}


OFCondition DRTImageIOD::getDeidentificationMethod(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DeidentificationMethod, value, pos);
}


OFCondition DRTImageIOD::getDerivationDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DerivationDescription, value, pos);
}


OFCondition DRTImageIOD::getDeviceSerialNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(DeviceSerialNumber, value, pos);
}


OFCondition DRTImageIOD::getEffectiveDuration(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(EffectiveDuration, value, pos);
}


OFCondition DRTImageIOD::getEffectiveDuration(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, EffectiveDuration).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getEndCumulativeMetersetWeight(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(EndCumulativeMetersetWeight, value, pos);
}


OFCondition DRTImageIOD::getEndCumulativeMetersetWeight(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, EndCumulativeMetersetWeight).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getEthnicGroup(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(EthnicGroup, value, pos);
}


OFCondition DRTImageIOD::getFractionNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(FractionNumber, value, pos);
}


OFCondition DRTImageIOD::getFractionNumber(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, FractionNumber).getSint32(value, pos);
}


OFCondition DRTImageIOD::getFrameDelay(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(FrameDelay, value, pos);
}


OFCondition DRTImageIOD::getFrameDelay(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, FrameDelay).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getFrameIncrementPointer(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(FrameIncrementPointer, value, pos);
}


OFCondition DRTImageIOD::getFrameOfReferenceUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(FrameOfReferenceUID, value, pos);
}


OFCondition DRTImageIOD::getFrameTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(FrameTime, value, pos);
}


OFCondition DRTImageIOD::getFrameTime(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, FrameTime).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getFrameTimeVector(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(FrameTimeVector, value, pos);
}


OFCondition DRTImageIOD::getFrameTimeVector(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, FrameTimeVector).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getFrameTimeVector(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, FrameTimeVector).getFloat64Vector(value);
}


OFCondition DRTImageIOD::getGantryAngle(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(GantryAngle, value, pos);
}


OFCondition DRTImageIOD::getGantryAngle(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, GantryAngle).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getGantryID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(GantryID, value, pos);
}


OFCondition DRTImageIOD::getGantryPitchAngle(Float32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmFloatingPointSingle &, GantryPitchAngle).getFloat32(value, pos);
}


OFCondition DRTImageIOD::getGreenPaletteColorLookupTableData(Uint16 *&value, unsigned long *count) const
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


OFCondition DRTImageIOD::getGreenPaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, GreenPaletteColorLookupTableDescriptor).getUint16(value, pos);
}


OFCondition DRTImageIOD::getHighBit(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, HighBit).getUint16(value, pos);
}


OFCondition DRTImageIOD::getICCProfile(Uint8 *&value, unsigned long *count) const
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


OFCondition DRTImageIOD::getImageComments(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ImageComments, value, pos);
}


OFCondition DRTImageIOD::getImagePlanePixelSpacing(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ImagePlanePixelSpacing, value, pos);
}


OFCondition DRTImageIOD::getImagePlanePixelSpacing(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, ImagePlanePixelSpacing).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getImagePlanePixelSpacing(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, ImagePlanePixelSpacing).getFloat64Vector(value);
}


OFCondition DRTImageIOD::getImageTriggerDelay(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ImageTriggerDelay, value, pos);
}


OFCondition DRTImageIOD::getImageTriggerDelay(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, ImageTriggerDelay).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getImageType(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ImageType, value, pos);
}


OFCondition DRTImageIOD::getImagesInAcquisition(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ImagesInAcquisition, value, pos);
}


OFCondition DRTImageIOD::getImagesInAcquisition(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, ImagesInAcquisition).getSint32(value, pos);
}


OFCondition DRTImageIOD::getInstanceCreationDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceCreationDate, value, pos);
}


OFCondition DRTImageIOD::getInstanceCreationTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceCreationTime, value, pos);
}


OFCondition DRTImageIOD::getInstanceCreatorUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceCreatorUID, value, pos);
}


OFCondition DRTImageIOD::getInstanceNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstanceNumber, value, pos);
}


OFCondition DRTImageIOD::getInstanceNumber(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, InstanceNumber).getSint32(value, pos);
}


OFCondition DRTImageIOD::getInstitutionAddress(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstitutionAddress, value, pos);
}


OFCondition DRTImageIOD::getInstitutionName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstitutionName, value, pos);
}


OFCondition DRTImageIOD::getInstitutionalDepartmentName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(InstitutionalDepartmentName, value, pos);
}


OFCondition DRTImageIOD::getIrradiationEventUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(IrradiationEventUID, value, pos);
}


OFCondition DRTImageIOD::getIsocenterPosition(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(IsocenterPosition, value, pos);
}


OFCondition DRTImageIOD::getIsocenterPosition(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, IsocenterPosition).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getIsocenterPosition(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, IsocenterPosition).getFloat64Vector(value);
}


OFCondition DRTImageIOD::getIssuerOfAdmissionID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(IssuerOfAdmissionID, value, pos);
}


OFCondition DRTImageIOD::getIssuerOfPatientID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(IssuerOfPatientID, value, pos);
}


OFCondition DRTImageIOD::getLargestImagePixelValue(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, LargestImagePixelValue).getUint16(value, pos);
}


OFCondition DRTImageIOD::getLossyImageCompression(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(LossyImageCompression, value, pos);
}


OFCondition DRTImageIOD::getLossyImageCompressionMethod(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(LossyImageCompressionMethod, value, pos);
}


OFCondition DRTImageIOD::getLossyImageCompressionRatio(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(LossyImageCompressionRatio, value, pos);
}


OFCondition DRTImageIOD::getLossyImageCompressionRatio(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, LossyImageCompressionRatio).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getLossyImageCompressionRatio(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, LossyImageCompressionRatio).getFloat64Vector(value);
}


OFCondition DRTImageIOD::getManufacturer(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(Manufacturer, value, pos);
}


OFCondition DRTImageIOD::getManufacturerModelName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ManufacturerModelName, value, pos);
}


OFCondition DRTImageIOD::getModality(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(Modality, value, pos);
}


OFCondition DRTImageIOD::getNameOfPhysiciansReadingStudy(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(NameOfPhysiciansReadingStudy, value, pos);
}


OFCondition DRTImageIOD::getNumberOfFrames(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(NumberOfFrames, value, pos);
}


OFCondition DRTImageIOD::getNumberOfFrames(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, NumberOfFrames).getSint32(value, pos);
}


OFCondition DRTImageIOD::getOccupation(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(Occupation, value, pos);
}


OFCondition DRTImageIOD::getOperatorsName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OperatorsName, value, pos);
}


OFCondition DRTImageIOD::getOriginalSpecializedSOPClassUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OriginalSpecializedSOPClassUID, value, pos);
}


OFCondition DRTImageIOD::getOtherPatientIDs(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OtherPatientIDs, value, pos);
}


OFCondition DRTImageIOD::getOtherPatientNames(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(OtherPatientNames, value, pos);
}


OFCondition DRTImageIOD::getPatientAge(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientAge, value, pos);
}


OFCondition DRTImageIOD::getPatientBirthDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientBirthDate, value, pos);
}


OFCondition DRTImageIOD::getPatientBirthTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientBirthTime, value, pos);
}


OFCondition DRTImageIOD::getPatientBreedDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientBreedDescription, value, pos);
}


OFCondition DRTImageIOD::getPatientComments(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientComments, value, pos);
}


OFCondition DRTImageIOD::getPatientID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientID, value, pos);
}


OFCondition DRTImageIOD::getPatientIdentityRemoved(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientIdentityRemoved, value, pos);
}


OFCondition DRTImageIOD::getPatientName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientName, value, pos);
}


OFCondition DRTImageIOD::getPatientOrientation(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientOrientation, value, pos);
}


OFCondition DRTImageIOD::getPatientPosition(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientPosition, value, pos);
}


OFCondition DRTImageIOD::getPatientSex(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientSex, value, pos);
}


OFCondition DRTImageIOD::getPatientSexNeutered(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientSexNeutered, value, pos);
}


OFCondition DRTImageIOD::getPatientSize(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientSize, value, pos);
}


OFCondition DRTImageIOD::getPatientSize(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, PatientSize).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getPatientSpeciesDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientSpeciesDescription, value, pos);
}


OFCondition DRTImageIOD::getPatientSupportAngle(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientSupportAngle, value, pos);
}


OFCondition DRTImageIOD::getPatientSupportAngle(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, PatientSupportAngle).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getPatientWeight(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PatientWeight, value, pos);
}


OFCondition DRTImageIOD::getPatientWeight(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, PatientWeight).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getPerformedProcedureStepDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepDescription, value, pos);
}


OFCondition DRTImageIOD::getPerformedProcedureStepID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepID, value, pos);
}


OFCondition DRTImageIOD::getPerformedProcedureStepStartDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepStartDate, value, pos);
}


OFCondition DRTImageIOD::getPerformedProcedureStepStartTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PerformedProcedureStepStartTime, value, pos);
}


OFCondition DRTImageIOD::getPhotometricInterpretation(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PhotometricInterpretation, value, pos);
}


OFCondition DRTImageIOD::getPhysiciansOfRecord(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PhysiciansOfRecord, value, pos);
}


OFCondition DRTImageIOD::getPixelAspectRatio(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PixelAspectRatio, value, pos);
}


OFCondition DRTImageIOD::getPixelAspectRatio(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, PixelAspectRatio).getSint32(value, pos);
}


OFCondition DRTImageIOD::getPixelDataProviderURL(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PixelDataProviderURL, value, pos);
}


OFCondition DRTImageIOD::getPixelIntensityRelationship(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PixelIntensityRelationship, value, pos);
}


OFCondition DRTImageIOD::getPixelIntensityRelationshipSign(Sint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmSignedShort &, PixelIntensityRelationshipSign).getSint16(value, pos);
}


OFCondition DRTImageIOD::getPixelPaddingValue(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, PixelPaddingValue).getUint16(value, pos);
}


OFCondition DRTImageIOD::getPixelRepresentation(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, PixelRepresentation).getUint16(value, pos);
}


OFCondition DRTImageIOD::getPlanarConfiguration(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, PlanarConfiguration).getUint16(value, pos);
}


OFCondition DRTImageIOD::getPositionReferenceIndicator(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PositionReferenceIndicator, value, pos);
}


OFCondition DRTImageIOD::getPreferredPlaybackSequencing(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, PreferredPlaybackSequencing).getUint16(value, pos);
}


OFCondition DRTImageIOD::getPresentationLUTShape(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PresentationLUTShape, value, pos);
}


OFCondition DRTImageIOD::getPrimaryDosimeterUnit(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(PrimaryDosimeterUnit, value, pos);
}


OFCondition DRTImageIOD::getQualityControlImage(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(QualityControlImage, value, pos);
}


OFCondition DRTImageIOD::getRTImageDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RTImageDescription, value, pos);
}


OFCondition DRTImageIOD::getRTImageLabel(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RTImageLabel, value, pos);
}


OFCondition DRTImageIOD::getRTImageName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RTImageName, value, pos);
}


OFCondition DRTImageIOD::getRTImageOrientation(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RTImageOrientation, value, pos);
}


OFCondition DRTImageIOD::getRTImageOrientation(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, RTImageOrientation).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getRTImageOrientation(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, RTImageOrientation).getFloat64Vector(value);
}


OFCondition DRTImageIOD::getRTImagePlane(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RTImagePlane, value, pos);
}


OFCondition DRTImageIOD::getRTImagePosition(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RTImagePosition, value, pos);
}


OFCondition DRTImageIOD::getRTImagePosition(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, RTImagePosition).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getRTImagePosition(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, RTImagePosition).getFloat64Vector(value);
}


OFCondition DRTImageIOD::getRTImageSID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RTImageSID, value, pos);
}


OFCondition DRTImageIOD::getRTImageSID(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, RTImageSID).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getRadiationMachineName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RadiationMachineName, value, pos);
}


OFCondition DRTImageIOD::getRadiationMachineSAD(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RadiationMachineSAD, value, pos);
}


OFCondition DRTImageIOD::getRadiationMachineSAD(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, RadiationMachineSAD).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getRadiationMachineSSD(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RadiationMachineSSD, value, pos);
}


OFCondition DRTImageIOD::getRadiationMachineSSD(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, RadiationMachineSSD).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getRecommendedDisplayFrameRate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RecommendedDisplayFrameRate, value, pos);
}


OFCondition DRTImageIOD::getRecommendedDisplayFrameRate(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, RecommendedDisplayFrameRate).getSint32(value, pos);
}


OFCondition DRTImageIOD::getRedPaletteColorLookupTableData(Uint16 *&value, unsigned long *count) const
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


OFCondition DRTImageIOD::getRedPaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, RedPaletteColorLookupTableDescriptor).getUint16(value, pos);
}


OFCondition DRTImageIOD::getReferencedBeamNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ReferencedBeamNumber, value, pos);
}


OFCondition DRTImageIOD::getReferencedBeamNumber(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, ReferencedBeamNumber).getSint32(value, pos);
}


OFCondition DRTImageIOD::getReferencedFractionGroupNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ReferencedFractionGroupNumber, value, pos);
}


OFCondition DRTImageIOD::getReferencedFractionGroupNumber(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, ReferencedFractionGroupNumber).getSint32(value, pos);
}


OFCondition DRTImageIOD::getReferringPhysicianName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ReferringPhysicianName, value, pos);
}


OFCondition DRTImageIOD::getRelatedGeneralSOPClassUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RelatedGeneralSOPClassUID, value, pos);
}


OFCondition DRTImageIOD::getReportedValuesOrigin(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ReportedValuesOrigin, value, pos);
}


OFCondition DRTImageIOD::getRescaleIntercept(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RescaleIntercept, value, pos);
}


OFCondition DRTImageIOD::getRescaleIntercept(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, RescaleIntercept).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getRescaleSlope(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RescaleSlope, value, pos);
}


OFCondition DRTImageIOD::getRescaleSlope(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, RescaleSlope).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getRescaleType(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(RescaleType, value, pos);
}


OFCondition DRTImageIOD::getResponsibleOrganization(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ResponsibleOrganization, value, pos);
}


OFCondition DRTImageIOD::getResponsiblePerson(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ResponsiblePerson, value, pos);
}


OFCondition DRTImageIOD::getResponsiblePersonRole(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ResponsiblePersonRole, value, pos);
}


OFCondition DRTImageIOD::getReviewDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ReviewDate, value, pos);
}


OFCondition DRTImageIOD::getReviewTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ReviewTime, value, pos);
}


OFCondition DRTImageIOD::getReviewerName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(ReviewerName, value, pos);
}


OFCondition DRTImageIOD::getRows(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, Rows).getUint16(value, pos);
}


OFCondition DRTImageIOD::getSOPAuthorizationComment(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPAuthorizationComment, value, pos);
}


OFCondition DRTImageIOD::getSOPAuthorizationDateTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPAuthorizationDateTime, value, pos);
}


OFCondition DRTImageIOD::getSOPClassUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPClassUID, value, pos);
}


OFCondition DRTImageIOD::getSOPInstanceStatus(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPInstanceStatus, value, pos);
}


OFCondition DRTImageIOD::getSOPInstanceUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SOPInstanceUID, value, pos);
}


OFCondition DRTImageIOD::getSamplesPerPixel(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, SamplesPerPixel).getUint16(value, pos);
}


OFCondition DRTImageIOD::getSeriesDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SeriesDescription, value, pos);
}


OFCondition DRTImageIOD::getSeriesInstanceUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SeriesInstanceUID, value, pos);
}


OFCondition DRTImageIOD::getSeriesNumber(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SeriesNumber, value, pos);
}


OFCondition DRTImageIOD::getSeriesNumber(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, SeriesNumber).getSint32(value, pos);
}


OFCondition DRTImageIOD::getSmallestImagePixelValue(Uint16 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmUnsignedShort &, SmallestImagePixelValue).getUint16(value, pos);
}


OFCondition DRTImageIOD::getSoftwareVersions(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SoftwareVersions, value, pos);
}


OFCondition DRTImageIOD::getSourceToReferenceObjectDistance(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SourceToReferenceObjectDistance, value, pos);
}


OFCondition DRTImageIOD::getSourceToReferenceObjectDistance(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, SourceToReferenceObjectDistance).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getSpatialResolution(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SpatialResolution, value, pos);
}


OFCondition DRTImageIOD::getSpatialResolution(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, SpatialResolution).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getSpecificCharacterSet(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(SpecificCharacterSet, value, pos);
}


OFCondition DRTImageIOD::getStartCumulativeMetersetWeight(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StartCumulativeMetersetWeight, value, pos);
}


OFCondition DRTImageIOD::getStartCumulativeMetersetWeight(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, StartCumulativeMetersetWeight).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getStartTrim(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StartTrim, value, pos);
}


OFCondition DRTImageIOD::getStartTrim(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, StartTrim).getSint32(value, pos);
}


OFCondition DRTImageIOD::getStationName(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StationName, value, pos);
}


OFCondition DRTImageIOD::getStopTrim(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StopTrim, value, pos);
}


OFCondition DRTImageIOD::getStopTrim(Sint32 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmIntegerString &, StopTrim).getSint32(value, pos);
}


OFCondition DRTImageIOD::getStudyDate(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyDate, value, pos);
}


OFCondition DRTImageIOD::getStudyDescription(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyDescription, value, pos);
}


OFCondition DRTImageIOD::getStudyID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyID, value, pos);
}


OFCondition DRTImageIOD::getStudyInstanceUID(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyInstanceUID, value, pos);
}


OFCondition DRTImageIOD::getStudyTime(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(StudyTime, value, pos);
}


OFCondition DRTImageIOD::getTableTopEccentricAngle(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(TableTopEccentricAngle, value, pos);
}


OFCondition DRTImageIOD::getTableTopEccentricAngle(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, TableTopEccentricAngle).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getTableTopEccentricAxisDistance(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(TableTopEccentricAxisDistance, value, pos);
}


OFCondition DRTImageIOD::getTableTopEccentricAxisDistance(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, TableTopEccentricAxisDistance).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getTableTopLateralPosition(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(TableTopLateralPosition, value, pos);
}


OFCondition DRTImageIOD::getTableTopLateralPosition(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, TableTopLateralPosition).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getTableTopLongitudinalPosition(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(TableTopLongitudinalPosition, value, pos);
}


OFCondition DRTImageIOD::getTableTopLongitudinalPosition(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, TableTopLongitudinalPosition).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getTableTopVerticalPosition(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(TableTopVerticalPosition, value, pos);
}


OFCondition DRTImageIOD::getTableTopVerticalPosition(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, TableTopVerticalPosition).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getTimeOfLastCalibration(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(TimeOfLastCalibration, value, pos);
}


OFCondition DRTImageIOD::getTimezoneOffsetFromUTC(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(TimezoneOffsetFromUTC, value, pos);
}


OFCondition DRTImageIOD::getVOILUTFunction(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(VOILUTFunction, value, pos);
}


OFCondition DRTImageIOD::getWindowCenter(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(WindowCenter, value, pos);
}


OFCondition DRTImageIOD::getWindowCenter(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, WindowCenter).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getWindowCenter(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, WindowCenter).getFloat64Vector(value);
}


OFCondition DRTImageIOD::getWindowCenterWidthExplanation(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(WindowCenterWidthExplanation, value, pos);
}


OFCondition DRTImageIOD::getWindowWidth(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(WindowWidth, value, pos);
}


OFCondition DRTImageIOD::getWindowWidth(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, WindowWidth).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getWindowWidth(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, WindowWidth).getFloat64Vector(value);
}


OFCondition DRTImageIOD::getXRayImageReceptorAngle(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(XRayImageReceptorAngle, value, pos);
}


OFCondition DRTImageIOD::getXRayImageReceptorAngle(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, XRayImageReceptorAngle).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getXRayImageReceptorTranslation(OFString &value, const signed long pos) const
{
    return getStringValueFromElement(XRayImageReceptorTranslation, value, pos);
}


OFCondition DRTImageIOD::getXRayImageReceptorTranslation(Float64 &value, const unsigned long pos) const
{
    return OFconst_cast(DcmDecimalString &, XRayImageReceptorTranslation).getFloat64(value, pos);
}


OFCondition DRTImageIOD::getXRayImageReceptorTranslation(OFVector<Float64> &value) const
{
    return OFconst_cast(DcmDecimalString &, XRayImageReceptorTranslation).getFloat64Vector(value);
}


OFCondition DRTImageIOD::setAccessionNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AccessionNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setAcquisitionDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AcquisitionDate.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setAcquisitionDateTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AcquisitionDateTime.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setAcquisitionNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AcquisitionNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setAcquisitionTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AcquisitionTime.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setActualFrameDuration(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ActualFrameDuration.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setAdditionalPatientHistory(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = AdditionalPatientHistory.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setAdmissionID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AdmissionID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setAdmittingDiagnosesDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = AdmittingDiagnosesDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setApprovalStatus(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ApprovalStatus.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setAuthorizationEquipmentCertificationNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = AuthorizationEquipmentCertificationNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setBeamLimitingDeviceAngle(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = BeamLimitingDeviceAngle.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setBitsAllocated(const Uint16 value, const unsigned long pos)
{
    return BitsAllocated.putUint16(value, pos);
}


OFCondition DRTImageIOD::setBitsStored(const Uint16 value, const unsigned long pos)
{
    return BitsStored.putUint16(value, pos);
}


OFCondition DRTImageIOD::setBluePaletteColorLookupTableData(const Uint16 *value, const unsigned long count)
{
    return BluePaletteColorLookupTableData.putUint16Array(value, count);
}


OFCondition DRTImageIOD::setBluePaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos)
{
    return BluePaletteColorLookupTableDescriptor.putUint16(value, pos);
}


OFCondition DRTImageIOD::setBurnedInAnnotation(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = BurnedInAnnotation.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setCineRate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = CineRate.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setClinicalTrialCoordinatingCenterName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialCoordinatingCenterName.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setClinicalTrialProtocolID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialProtocolID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setClinicalTrialProtocolName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialProtocolName.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setClinicalTrialSiteID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSiteID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setClinicalTrialSiteName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSiteName.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setClinicalTrialSponsorName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSponsorName.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setClinicalTrialSubjectID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSubjectID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setClinicalTrialSubjectReadingID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialSubjectReadingID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setClinicalTrialTimePointDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = ClinicalTrialTimePointDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setClinicalTrialTimePointID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ClinicalTrialTimePointID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setColumns(const Uint16 value, const unsigned long pos)
{
    return Columns.putUint16(value, pos);
}


OFCondition DRTImageIOD::setContentDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ContentDate.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setContentTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ContentTime.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setContrastBolusAgent(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ContrastBolusAgent.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setContrastBolusIngredient(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ContrastBolusIngredient.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setContrastBolusIngredientConcentration(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ContrastBolusIngredientConcentration.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setContrastBolusRoute(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ContrastBolusRoute.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setContrastBolusStartTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ContrastBolusStartTime.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setContrastBolusStopTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ContrastBolusStopTime.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setContrastBolusTotalDose(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ContrastBolusTotalDose.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setContrastBolusVolume(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ContrastBolusVolume.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setContrastFlowDuration(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = ContrastFlowDuration.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setContrastFlowRate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = ContrastFlowRate.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setConversionType(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ConversionType.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setDateOfLastCalibration(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = DateOfLastCalibration.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setDeidentificationMethod(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = DeidentificationMethod.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setDerivationDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = DerivationDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setDeviceSerialNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = DeviceSerialNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setEffectiveDuration(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = EffectiveDuration.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setEndCumulativeMetersetWeight(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = EndCumulativeMetersetWeight.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setEthnicGroup(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = EthnicGroup.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setFractionNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = FractionNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setFrameDelay(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = FrameDelay.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setFrameIncrementPointer(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmAttributeTag::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = FrameIncrementPointer.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setFrameOfReferenceUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = FrameOfReferenceUID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setFrameTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = FrameTime.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setFrameTimeVector(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = FrameTimeVector.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setGantryAngle(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = GantryAngle.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setGantryID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = GantryID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setGantryPitchAngle(const Float32 value, const unsigned long pos)
{
    return GantryPitchAngle.putFloat32(value, pos);
}


OFCondition DRTImageIOD::setGreenPaletteColorLookupTableData(const Uint16 *value, const unsigned long count)
{
    return GreenPaletteColorLookupTableData.putUint16Array(value, count);
}


OFCondition DRTImageIOD::setGreenPaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos)
{
    return GreenPaletteColorLookupTableDescriptor.putUint16(value, pos);
}


OFCondition DRTImageIOD::setHighBit(const Uint16 value, const unsigned long pos)
{
    return HighBit.putUint16(value, pos);
}


OFCondition DRTImageIOD::setICCProfile(const Uint8 *value, const unsigned long count)
{
    return ICCProfile.putUint8Array(value, count);
}


OFCondition DRTImageIOD::setImageComments(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = ImageComments.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setImagePlanePixelSpacing(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "2") : EC_Normal;
    if (result.good())
        result = ImagePlanePixelSpacing.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setImageTriggerDelay(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ImageTriggerDelay.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setImageType(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "2-n") : EC_Normal;
    if (result.good())
        result = ImageType.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setImagesInAcquisition(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ImagesInAcquisition.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setInstanceCreationDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceCreationDate.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setInstanceCreationTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceCreationTime.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setInstanceCreatorUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceCreatorUID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setInstanceNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstanceNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setInstitutionAddress(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = InstitutionAddress.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setInstitutionName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstitutionName.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setInstitutionalDepartmentName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = InstitutionalDepartmentName.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setIrradiationEventUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = IrradiationEventUID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setIsocenterPosition(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "3") : EC_Normal;
    if (result.good())
        result = IsocenterPosition.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setIssuerOfAdmissionID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = IssuerOfAdmissionID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setIssuerOfPatientID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = IssuerOfPatientID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setLargestImagePixelValue(const Uint16 value, const unsigned long pos)
{
    return LargestImagePixelValue.putUint16(value, pos);
}


OFCondition DRTImageIOD::setLossyImageCompression(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = LossyImageCompression.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setLossyImageCompressionMethod(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = LossyImageCompressionMethod.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setLossyImageCompressionRatio(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = LossyImageCompressionRatio.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setManufacturer(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = Manufacturer.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setManufacturerModelName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ManufacturerModelName.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setModality(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = Modality.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setNameOfPhysiciansReadingStudy(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = NameOfPhysiciansReadingStudy.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setNumberOfFrames(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = NumberOfFrames.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setOccupation(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = Occupation.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setOperatorsName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = OperatorsName.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setOriginalSpecializedSOPClassUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = OriginalSpecializedSOPClassUID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setOtherPatientIDs(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = OtherPatientIDs.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setOtherPatientNames(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = OtherPatientNames.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPatientAge(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmAgeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientAge.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPatientBirthDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientBirthDate.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPatientBirthTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientBirthTime.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPatientBreedDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientBreedDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPatientComments(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = PatientComments.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPatientID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPatientIdentityRemoved(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientIdentityRemoved.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPatientName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientName.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPatientOrientation(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "2") : EC_Normal;
    if (result.good())
        result = PatientOrientation.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPatientPosition(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientPosition.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPatientSex(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSex.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPatientSexNeutered(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSexNeutered.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPatientSize(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSize.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPatientSpeciesDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSpeciesDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPatientSupportAngle(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientSupportAngle.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPatientWeight(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PatientWeight.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPerformedProcedureStepDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPerformedProcedureStepID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPerformedProcedureStepStartDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepStartDate.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPerformedProcedureStepStartTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PerformedProcedureStepStartTime.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPhotometricInterpretation(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PhotometricInterpretation.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPhysiciansOfRecord(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = PhysiciansOfRecord.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPixelAspectRatio(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "2") : EC_Normal;
    if (result.good())
        result = PixelAspectRatio.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPixelDataProviderURL(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUnlimitedText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = PixelDataProviderURL.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPixelIntensityRelationship(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PixelIntensityRelationship.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPixelIntensityRelationshipSign(const Sint16 value, const unsigned long pos)
{
    return PixelIntensityRelationshipSign.putSint16(value, pos);
}


OFCondition DRTImageIOD::setPixelPaddingValue(const Uint16 value, const unsigned long pos)
{
    return PixelPaddingValue.putUint16(value, pos);
}


OFCondition DRTImageIOD::setPixelRepresentation(const Uint16 value, const unsigned long pos)
{
    return PixelRepresentation.putUint16(value, pos);
}


OFCondition DRTImageIOD::setPlanarConfiguration(const Uint16 value, const unsigned long pos)
{
    return PlanarConfiguration.putUint16(value, pos);
}


OFCondition DRTImageIOD::setPositionReferenceIndicator(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PositionReferenceIndicator.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPreferredPlaybackSequencing(const Uint16 value, const unsigned long pos)
{
    return PreferredPlaybackSequencing.putUint16(value, pos);
}


OFCondition DRTImageIOD::setPresentationLUTShape(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PresentationLUTShape.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setPrimaryDosimeterUnit(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = PrimaryDosimeterUnit.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setQualityControlImage(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = QualityControlImage.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setRTImageDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = RTImageDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setRTImageLabel(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RTImageLabel.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setRTImageName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RTImageName.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setRTImageOrientation(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "6") : EC_Normal;
    if (result.good())
        result = RTImageOrientation.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setRTImagePlane(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RTImagePlane.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setRTImagePosition(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "2") : EC_Normal;
    if (result.good())
        result = RTImagePosition.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setRTImageSID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RTImageSID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setRadiationMachineName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RadiationMachineName.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setRadiationMachineSAD(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RadiationMachineSAD.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setRadiationMachineSSD(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RadiationMachineSSD.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setRecommendedDisplayFrameRate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RecommendedDisplayFrameRate.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setRedPaletteColorLookupTableData(const Uint16 *value, const unsigned long count)
{
    return RedPaletteColorLookupTableData.putUint16Array(value, count);
}


OFCondition DRTImageIOD::setRedPaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos)
{
    return RedPaletteColorLookupTableDescriptor.putUint16(value, pos);
}


OFCondition DRTImageIOD::setReferencedBeamNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ReferencedBeamNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setReferencedFractionGroupNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ReferencedFractionGroupNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setReferringPhysicianName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ReferringPhysicianName.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setRelatedGeneralSOPClassUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = RelatedGeneralSOPClassUID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setReportedValuesOrigin(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ReportedValuesOrigin.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setRescaleIntercept(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RescaleIntercept.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setRescaleSlope(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RescaleSlope.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setRescaleType(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = RescaleType.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setResponsibleOrganization(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ResponsibleOrganization.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setResponsiblePerson(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ResponsiblePerson.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setResponsiblePersonRole(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ResponsiblePersonRole.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setReviewDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ReviewDate.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setReviewTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ReviewTime.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setReviewerName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = ReviewerName.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setRows(const Uint16 value, const unsigned long pos)
{
    return Rows.putUint16(value, pos);
}


OFCondition DRTImageIOD::setSOPAuthorizationComment(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = SOPAuthorizationComment.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setSOPAuthorizationDateTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SOPAuthorizationDateTime.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setSOPClassUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SOPClassUID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setSOPInstanceStatus(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SOPInstanceStatus.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setSOPInstanceUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SOPInstanceUID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setSamplesPerPixel(const Uint16 value, const unsigned long pos)
{
    return SamplesPerPixel.putUint16(value, pos);
}


OFCondition DRTImageIOD::setSeriesDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setSeriesInstanceUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesInstanceUID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setSeriesNumber(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SeriesNumber.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setSmallestImagePixelValue(const Uint16 value, const unsigned long pos)
{
    return SmallestImagePixelValue.putUint16(value, pos);
}


OFCondition DRTImageIOD::setSoftwareVersions(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = SoftwareVersions.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setSourceToReferenceObjectDistance(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SourceToReferenceObjectDistance.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setSpatialResolution(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = SpatialResolution.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setSpecificCharacterSet(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = SpecificCharacterSet.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setStartCumulativeMetersetWeight(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StartCumulativeMetersetWeight.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setStartTrim(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StartTrim.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setStationName(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StationName.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setStopTrim(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StopTrim.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setStudyDate(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyDate.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setStudyDescription(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyDescription.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setStudyID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setStudyInstanceUID(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyInstanceUID.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setStudyTime(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = StudyTime.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setTableTopEccentricAngle(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = TableTopEccentricAngle.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setTableTopEccentricAxisDistance(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = TableTopEccentricAxisDistance.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setTableTopLateralPosition(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = TableTopLateralPosition.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setTableTopLongitudinalPosition(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = TableTopLongitudinalPosition.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setTableTopVerticalPosition(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = TableTopVerticalPosition.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setTimeOfLastCalibration(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmTime::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = TimeOfLastCalibration.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setTimezoneOffsetFromUTC(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = TimezoneOffsetFromUTC.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setVOILUTFunction(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = VOILUTFunction.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setWindowCenter(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = WindowCenter.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setWindowCenterWidthExplanation(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = WindowCenterWidthExplanation.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setWindowWidth(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = WindowWidth.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setXRayImageReceptorAngle(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = XRayImageReceptorAngle.putOFStringArray(value);
    return result;
}


OFCondition DRTImageIOD::setXRayImageReceptorTranslation(const OFString &value, const OFBool check)
{
    OFCondition result = (check) ? DcmDecimalString::checkStringValue(value, "3") : EC_Normal;
    if (result.good())
        result = XRayImageReceptorTranslation.putOFStringArray(value);
    return result;
}


// end of source file
