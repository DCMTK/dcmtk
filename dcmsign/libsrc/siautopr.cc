/*
 *
 *  Copyright (C) 1998-2019, OFFIS e.V.
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
 *  Author: Norbert Loxen, Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiAuthorizationProfile
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/siautopr.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmsign/sitypes.h"

SiAuthorizationProfile::SiAuthorizationProfile()
: containsRawData_(OFFalse)
{
}

OFBool SiAuthorizationProfile::attributeRequiredIfPresent(const DcmTagKey& key) const
{
  // The Authorization RSA Digital Signature Profile requires that any
  // attributes whose Values are verifiable by the technician or physician
  // (e.g., their values are displayed to the technician or physician) must
  // be included in the signature. This must be handled manually.

  // the SOP Class and Instance UIDs
  if (key == DCM_SOPClassUID) return OFTrue;
  if (key == DCM_SOPInstanceUID) return OFTrue;

  // the Study and Series Instance UIDs
  if (key == DCM_StudyInstanceUID) return OFTrue;
  if (key == DCM_SeriesInstanceUID) return OFTrue;

  // any attributes of the Overlay Plane Module that are present
  if (key == DCM_OverlayRows) return OFTrue;
  if (key == DCM_OverlayColumns) return OFTrue;
  if (key == DCM_OverlayType) return OFTrue;
  if (key == DCM_OverlayOrigin) return OFTrue;
  if (key == DCM_OverlayBitsAllocated) return OFTrue;
  if (key == DCM_OverlayBitPosition) return OFTrue;
  if (key == DCM_OverlayData) return OFTrue;
  if (key == DCM_OverlayDescription) return OFTrue;
  if (key == DCM_OverlaySubtype) return OFTrue;
  if (key == DCM_OverlayLabel) return OFTrue;
  if (key == DCM_ROIArea) return OFTrue;
  if (key == DCM_ROIMean) return OFTrue;
  if (key == DCM_ROIStandardDeviation) return OFTrue;

  // any attributes of the Curve Module that are present
  if ((key.getGroup() >= 0x5000) && (key.getGroup() < 0x5020) && ((key.getGroup() & 1) == 0))
  {
    switch (key.getElement())
    {
      case 0x0005: // CurveDimensions (retired)
      case 0x0010: // NumberOfPoints (retired)
      case 0x0103: // DataValueRepresentation (retired)
      case 0x3000: // CurveData (retired)
      case 0x0022: // CurveDescription (retired)
      case 0x0030: // AxisUnits (retired)
      case 0x0040: // AxisLabels (retired)
      case 0x0104: // MinimumCoordinateValue (retired)
      case 0x0105: // MaximumCoordinateValue (retired)
      case 0x0106: // CurveRange (retired)
      case 0x0110: // CurveDataDescriptor (retired)
      case 0x0112: // CoordinateStartValue (retired)
      case 0x0114: // CoordinateStepValue (retired)
      case 0x2500: // CurveLabel (retired)
      case 0x2600: // ReferencedOverlaySequence (retired)
        return OFTrue;
        break;
      default:
        /* nothing */
        break;
    }
  }

  // any attributes of the Graphic Annotation Module that are present
  if (key == DCM_GraphicAnnotationSequence) return OFTrue;

  // any attributes of the General Image Module that are present
  if (key == DCM_InstanceNumber) return OFTrue;
  if (key == DCM_PatientOrientation) return OFTrue;
  if (key == DCM_ContentDate) return OFTrue;
  if (key == DCM_ContentTime) return OFTrue;
  if (key == DCM_ImageType) return OFTrue;
  if (key == DCM_AcquisitionNumber) return OFTrue;
  if (key == DCM_AcquisitionDate) return OFTrue;
  if (key == DCM_AcquisitionTime) return OFTrue;
  if (key == DCM_AcquisitionDateTime) return OFTrue;
  if (key == DCM_ImagesInAcquisition) return OFTrue;
  if (key == DCM_ImageComments) return OFTrue;
  if (key == DCM_QualityControlImage) return OFTrue;
  if (key == DCM_BurnedInAnnotation) return OFTrue;
  if (key == DCM_RecognizableVisualFeatures) return OFTrue;
  if (key == DCM_LossyImageCompression) return OFTrue;
  if (key == DCM_LossyImageCompressionRatio) return OFTrue;
  if (key == DCM_LossyImageCompressionMethod) return OFTrue;
  if (key == DCM_IconImageSequence) return OFTrue;
  if (key == DCM_PresentationLUTShape) return OFTrue;
  if (key == DCM_IrradiationEventUID) return OFTrue;
  if (key == DCM_RealWorldValueMappingSequence) return OFTrue;
  if (key == DCM_ImageLaterality) return OFTrue;
  if (key == DCM_AnatomicRegionSequence) return OFTrue;
  if (key == DCM_PrimaryAnatomicStructureSequence) return OFTrue;

  // any attributes of the Image Pixel Module that are present
  if (key == DCM_SamplesPerPixel) return OFTrue;
  if (key == DCM_PhotometricInterpretation) return OFTrue;
  if (key == DCM_Rows) return OFTrue;
  if (key == DCM_Columns) return OFTrue;
  if (key == DCM_BitsAllocated) return OFTrue;
  if (key == DCM_BitsStored) return OFTrue;
  if (key == DCM_HighBit) return OFTrue;
  if (key == DCM_PixelRepresentation) return OFTrue;
  if (key == DCM_PlanarConfiguration) return OFTrue;
  if (key == DCM_PixelAspectRatio) return OFTrue;
  if (key == DCM_SmallestImagePixelValue) return OFTrue;
  if (key == DCM_LargestImagePixelValue) return OFTrue;
  if (key == DCM_RedPaletteColorLookupTableDescriptor) return OFTrue;
  if (key == DCM_GreenPaletteColorLookupTableDescriptor) return OFTrue;
  if (key == DCM_BluePaletteColorLookupTableDescriptor) return OFTrue;
  if (key == DCM_RedPaletteColorLookupTableData) return OFTrue;
  if (key == DCM_GreenPaletteColorLookupTableData) return OFTrue;
  if (key == DCM_BluePaletteColorLookupTableData) return OFTrue;
  if (key == DCM_ICCProfile) return OFTrue;
  if (key == DCM_ColorSpace) return OFTrue;
  if (key == DCM_PixelData) return OFTrue;
  if (key == DCM_PixelDataProviderURL) return OFTrue;
  if (key == DCM_PixelPaddingRangeLimit) return OFTrue;
  if (key == DCM_ExtendedOffsetTable) return OFTrue;
  if (key == DCM_ExtendedOffsetTableLengths) return OFTrue;

  // any attributes of the SR Document General Module that are present
  // if (key == DCM_InstanceNumber) return OFTrue; // also in General Image Module
  if (key == DCM_PreliminaryFlag) return OFTrue;
  if (key == DCM_CompletionFlag) return OFTrue;
  if (key == DCM_CompletionFlagDescription) return OFTrue;
  if (key == DCM_VerificationFlag) return OFTrue;
  // if (key == DCM_ContentDate) return OFTrue; // also in General Image Module
  // if (key == DCM_ContentTime) return OFTrue; // also in General Image Module
  if (key == DCM_VerifyingObserverSequence) return OFTrue;
  if (key == DCM_AuthorObserverSequence) return OFTrue;
  if (key == DCM_ParticipantSequence) return OFTrue;
  if (key == DCM_CustodialOrganizationSequence) return OFTrue;
  if (key == DCM_PredecessorDocumentsSequence) return OFTrue;
  if (key == DCM_IdenticalDocumentsSequence) return OFTrue;
  if (key == DCM_ReferencedRequestSequence) return OFTrue;
  if (key == DCM_PerformedProcedureCodeSequence) return OFTrue;
  if (key == DCM_CurrentRequestedProcedureEvidenceSequence) return OFTrue;
  if (key == DCM_PertinentOtherEvidenceSequence) return OFTrue;
  if (key == DCM_ReferencedInstanceSequence) return OFTrue;

  // any attributes of the SR Document Content Module that are present
  if (key == DCM_ValueType) return OFTrue;
  if (key == DCM_ConceptNameCodeSequence) return OFTrue;
  if (key == DCM_ContinuityOfContent) return OFTrue;
  if (key == DCM_ContentTemplateSequence) return OFTrue;
  if (key == DCM_ObservationDateTime) return OFTrue;
  if (key == DCM_ObservationUID) return OFTrue;
  if (key == DCM_ContentSequence) return OFTrue;

  // any attributes of the Waveform Module that are present
  if (key == DCM_WaveformSequence) return OFTrue;
  if (key == DCM_WaveformDataDisplayScale) return OFTrue;
  if (key == DCM_WaveformDisplayBackgroundCIELabValue) return OFTrue;
  if (key == DCM_WaveformPresentationGroupSequence) return OFTrue;

  // any attributes of the Waveform Annotation Module that are present
  if (key == DCM_WaveformAnnotationSequence) return OFTrue;

  // any attributes of the Multi-frame Functional Groups module that are present
  if (key == DCM_SharedFunctionalGroupsSequence) return OFTrue;
  if (key == DCM_PerFrameFunctionalGroupsSequence) return OFTrue;
  // if (key == DCM_InstanceNumber) return OFTrue; // also in General Image Module
  // if (key == DCM_ContentDate) return OFTrue; // also in General Image Module
  // if (key == DCM_ContentTime) return OFTrue; // also in General Image Module
  if (key == DCM_NumberOfFrames) return OFTrue;
  if (key == DCM_StereoPairsPresent) return OFTrue;
  if (key == DCM_ConcatenationFrameOffsetNumber) return OFTrue;
  if (key == DCM_RepresentativeFrameNumber) return OFTrue;
  if (key == DCM_ConcatenationUID) return OFTrue;
  if (key == DCM_SOPInstanceUIDOfConcatenationSource) return OFTrue;
  if (key == DCM_InConcatenationNumber) return OFTrue;
  if (key == DCM_InConcatenationTotalNumber) return OFTrue;

  // any attributes of the Enhanced MR Image Module that are present
  // if (key == DCM_AcquisitionNumber) return OFTrue; // also in General Image Module
  // if (key == DCM_AcquisitionDateTime) return OFTrue; // also in General Image Module
  if (key == DCM_AcquisitionDuration) return OFTrue;
  if (key == DCM_ReferencedRawDataSequence) return OFTrue;
  if (key == DCM_ReferencedWaveformSequence) return OFTrue;
  if (key == DCM_ReferencedImageEvidenceSequence) return OFTrue;
  if (key == DCM_SourceImageEvidenceSequence) return OFTrue;
  if (key == DCM_ReferencedPresentationStateSequence) return OFTrue;
  if (key == DCM_ContentQualification) return OFTrue;
  if (key == DCM_ResonantNucleus) return OFTrue;
  if (key == DCM_KSpaceFiltering) return OFTrue;
  if (key == DCM_MagneticFieldStrength) return OFTrue;
  if (key == DCM_ApplicableSafetyStandardAgency) return OFTrue;
  if (key == DCM_ApplicableSafetyStandardDescription) return OFTrue;
  // if (key == DCM_ImageComments) return OFTrue; // also in General Image Module
  if (key == DCM_IsocenterPosition) return OFTrue;
  if (key == DCM_B1rms) return OFTrue;
  // if (key == DCM_ImageType) return OFTrue; // also in General Image Module
  if (key == DCM_PixelPresentation) return OFTrue;
  if (key == DCM_VolumetricProperties) return OFTrue;
  if (key == DCM_VolumeBasedCalculationTechnique) return OFTrue;
  if (key == DCM_ComplexImageComponent) return OFTrue;
  if (key == DCM_AcquisitionContrast) return OFTrue;
  if (key == DCM_FunctionalSettlingPhaseFramesPresent) return OFTrue;
  // if (key == DCM_SamplesPerPixel) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_PhotometricInterpretation) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_BitsAllocated) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_BitsStored) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_HighBit) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_PixelRepresentation) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_PlanarConfiguration) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_BurnedInAnnotation) return OFTrue; // also in General Image Module
  // if (key == DCM_RecognizableVisualFeatures) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompression) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompressionRatio) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompressionMethod) return OFTrue; // also in General Image Module
  // if (key == DCM_PresentationLUTShape) return OFTrue; // also in General Image Module
  // if (key == DCM_IconImageSequence) return OFTrue; // also in General Image Module
  if (key == DCM_ViewCodeSequence) return OFTrue;
  if (key == DCM_SliceProgressionDirection) return OFTrue;

  // any attributes of the MR Spectroscopy modules that are present
  // if (key == DCM_AcquisitionNumber) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_AcquisitionDateTime) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_AcquisitionDuration) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ReferencedRawDataSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ReferencedWaveformSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ReferencedImageEvidenceSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_SourceImageEvidenceSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ReferencedPresentationStateSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ContentQualification) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ResonantNucleus) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_k-SpaceFiltering) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_MagneticFieldStrength) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ApplicableSafetyStandardAgency) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ApplicableSafetyStandardDescription) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ImageComments) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_IsocenterPosition) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_B1rms) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ImageType) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_VolumetricProperties) return OFTrue; // also in Enhanced MR Image Module
  // if (key == DCM_VolumeBasedCalculationTechnique) return OFTrue; // also in Enhanced MR Image Module
  // if (key == DCM_ComplexImageComponent) return OFTrue; // also in Enhanced MR Image Module
  // if (key == DCM_AcquisitionContrast) return OFTrue; // also in Enhanced MR Image Module
  if (key == DCM_TransmitterFrequency) return OFTrue;
  if (key == DCM_SpectralWidth) return OFTrue;
  if (key == DCM_ChemicalShiftReference) return OFTrue;
  if (key == DCM_VolumeLocalizationTechnique) return OFTrue;
  if (key == DCM_VolumeLocalizationSequence) return OFTrue;
  if (key == DCM_Decoupling) return OFTrue;
  if (key == DCM_DecoupledNucleus) return OFTrue;
  if (key == DCM_DecouplingFrequency) return OFTrue;
  if (key == DCM_DecouplingMethod) return OFTrue;
  if (key == DCM_DecouplingChemicalShiftReference) return OFTrue;
  if (key == DCM_TimeDomainFiltering) return OFTrue;
  if (key == DCM_NumberOfZeroFills) return OFTrue;
  if (key == DCM_BaselineCorrection) return OFTrue;
  if (key == DCM_FrequencyCorrection) return OFTrue;
  if (key == DCM_FirstOrderPhaseCorrection) return OFTrue;
  if (key == DCM_WaterReferencedPhaseCorrection) return OFTrue;
  if (key == DCM_WaterReferenceAcquisition) return OFTrue;
  // if (key == DCM_ReferencedInstanceSequence) return OFTrue; // also in SR Document General Module

  // any attributes of the Raw Data module that are present
  // if (key == DCM_InstanceNumber) return OFTrue; // also in General Image Module
  // if (key == DCM_ContentDate) return OFTrue; // also in General Image Module
  // if (key == DCM_ContentTime) return OFTrue; // also in General Image Module
  // if (key == DCM_AcquisitionDateTime) return OFTrue; // also in General Image Module
  if (key == DCM_ContentLabel) return OFTrue;
  if (key == DCM_ContentDescription) return OFTrue;
  // if (key == DCM_ConceptNameCodeSequence) return OFTrue; // also in SR Document Content Module
  // if (key == DCM_ImageLaterality) return OFTrue; // also in General Image Module
  if (key == DCM_CreatorVersionUID) return OFTrue;
  // if (key == DCM_ReferencedInstanceSequence) return OFTrue; // also in SR Document General Module

  // if the Raw Data Module is present, we need to include all private tags into
  // signature because the raw data as such is stored in private attributes.
  if (((key.getGroup() & 1) == 1) && containsRawData_) return OFTrue;

  // any attributes of the Enhanced CT Image module that are present
  // if (key == DCM_ImageType) return OFTrue; // also in General Image Module
  if (key == DCM_MultienergyCTAcquisition) return OFTrue;
  // if (key == DCM_AcquisitionNumber) return OFTrue; // also in General Image Module
  // if (key == DCM_AcquisitionDateTime) return OFTrue; // also in General Image Module
  // if (key == DCM_AcquisitionDuration) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ReferencedRawDataSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ReferencedWaveformSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ReferencedImageEvidenceSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_SourceImageEvidenceSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ReferencedPresentationStateSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_SamplesPerPixel) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_PhotometricInterpretation) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_BitsAllocated) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_BitsStored) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_HighBit) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_ContentQualification) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ImageComments) return OFTrue; // also in General Image Module
  // if (key == DCM_BurnedInAnnotation) return OFTrue; // also in General Image Module
  // if (key == DCM_RecognizableVisualFeatures) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompression) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompressionRatio) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompressionMethod) return OFTrue; // also in General Image Module
  // if (key == DCM_PresentationLUTShape) return OFTrue; // also in General Image Module
  // if (key == DCM_IconImageSequence) return OFTrue; // also in General Image Module
  // if (key == DCM_IsocenterPosition) return OFTrue;  // also in Enhanced MR Image Module

  // any attributes of the Enhanced XA/XRF Image module that are present
  // if (key == DCM_ImageType) return OFTrue; // also in General Image Module
  if (key == DCM_PlanesInAcquisition) return OFTrue;
  if (key == DCM_PlaneIdentification) return OFTrue;
  // if (key == DCM_AcquisitionNumber) return OFTrue; // also in General Image Module
  // if (key == DCM_AcquisitionDateTime) return OFTrue; // also in General Image Module
  // if (key == DCM_BitsAllocated) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_BitsStored) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_HighBit) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_SamplesPerPixel) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_PixelRepresentation) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_PhotometricInterpretation) return OFTrue; // also in Image Pixel Module
  if (key == DCM_AcquisitionProtocolName) return OFTrue;
  if (key == DCM_AcquisitionProtocolDescription) return OFTrue;
  if (key == DCM_ScanOptions) return OFTrue;
  // if (key == DCM_ContentQualification) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_PatientOrientationCodeSequence) return OFTrue; // also in General Image Module
  if (key == DCM_PatientGantryRelationshipCodeSequence) return OFTrue;
  if (key == DCM_ExaminedBodyThickness) return OFTrue;
  // if (key == DCM_BurnedInAnnotation) return OFTrue; // also in General Image Module
  // if (key == DCM_RecognizableVisualFeatures) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompression) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompressionRatio) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompressionMethod) return OFTrue; // also in General Image Module
  if (key == DCM_ReferencedOtherPlaneSequence) return OFTrue;
  // if (key == DCM_ReferencedImageEvidenceSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_SourceImageEvidenceSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ReferencedInstanceSequence) return OFTrue; // also in SR Document General Module
  // if (key == DCM_ImageComments) return OFTrue; // also in General Image Module
  // if (key == DCM_QualityControlImage) return OFTrue; // also in General Image Module
  // if (key == DCM_IconImageSequence) return OFTrue; // also in General Image Module
  // if (key == DCM_PresentationLUTShape) return OFTrue; // also in General Image Module

  // any attributes of the Segmentation Image module that are present
  // if (key == DCM_ImageType) return OFTrue; // also in General Image Module
  if (key == DCM_ImageOrientationSlide) return OFTrue;
  // if (key == DCM_SamplesPerPixel) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_PhotometricInterpretation) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_PixelRepresentation) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_BitsAllocated) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_BitsStored) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_HighBit) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_LossyImageCompression) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompressionRatio) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompressionMethod) return OFTrue; // also in General Image Module
  if (key == DCM_SegmentationType) return OFTrue;
  if (key == DCM_SegmentationFractionalType) return OFTrue;
  if (key == DCM_MaximumFractionalValue) return OFTrue;
  if (key == DCM_SegmentsOverlap) return OFTrue;
  if (key == DCM_SegmentSequence) return OFTrue;

  // any attributes of the Encapsulated Document module that are present
  // if (key == DCM_InstanceNumber) return OFTrue; // also in General Image Module
  // if (key == DCM_ContentDate) return OFTrue; // also in General Image Module
  // if (key == DCM_ContentTime) return OFTrue; // also in General Image Module
  // if (key == DCM_AcquisitionDateTime) return OFTrue; // also in General Image Module
  // if (key == DCM_ImageLaterality) return OFTrue; // also in General Image Module
  // if (key == DCM_BurnedInAnnotation) return OFTrue; // also in General Image Module
  // if (key == DCM_RecognizableVisualFeatures) return OFTrue; // also in General Image Module
  if (key == DCM_SourceInstanceSequence) return OFTrue;
  if (key == DCM_DocumentTitle) return OFTrue;
  // if (key == DCM_ConceptNameCodeSequence) return OFTrue; // also in SR Document Content Module
  if (key == DCM_DocumentClassCodeSequence) return OFTrue;
  // if (key == DCM_VerificationFlag) return OFTrue; // also in SR Document General Module
  if (key == DCM_HL7InstanceIdentifier) return OFTrue;
  // if (key == DCM_PredecessorDocumentsSequence) return OFTrue; // also in SR Document General Module
  // if (key == DCM_IdenticalDocumentsSequence) return OFTrue; // also in SR Document General Module
  if (key == DCM_MIMETypeOfEncapsulatedDocument) return OFTrue;
  if (key == DCM_ListOfMIMETypes) return OFTrue;
  if (key == DCM_EncapsulatedDocument) return OFTrue;
  if (key == DCM_EncapsulatedDocumentLength) return OFTrue;
  // if (key == DCM_ValueType) return OFTrue; // also in SR Document Content Module
  // if (key == DCM_ContentSequence) return OFTrue; // also in SR Document Content Module
  // if (key == DCM_ContinuityOfContent) return OFTrue; // also in SR Document Content Module
  // if (key == DCM_ContentTemplateSequence) return OFTrue; // also in SR Document Content Module

  // any attributes of the X-Ray 3D Image module that are present
  // if (key == DCM_ImageType) return OFTrue; // also in General Image Module
  // if (key == DCM_BitsAllocated) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_BitsStored) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_HighBit) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_SamplesPerPixel) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_PhotometricInterpretation) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_ContentQualification) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_BurnedInAnnotation) return OFTrue; // also in General Image Module
  // if (key == DCM_RecognizableVisualFeatures) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompression) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompressionRatio) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompressionMethod) return OFTrue; // also in General Image Module
  // if (key == DCM_ReferencedImageEvidenceSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ImageComments) return OFTrue; // also in General Image Module
  // if (key == DCM_QualityControlImage) return OFTrue; // also in General Image Module
  // if (key == DCM_IconImageSequence) return OFTrue; // also in General Image Module
  // if (key == DCM_PresentationLUTShape) return OFTrue; // also in General Image Module
  if (key == DCM_SourceIrradiationEventSequence) return OFTrue;

  // any attributes of the Enhanced PET Image module that are present
  // if (key == DCM_ImageType) return OFTrue; // also in General Image Module
  // if (key == DCM_AcquisitionNumber) return OFTrue; // also in General Image Module
  // if (key == DCM_AcquisitionDateTime) return OFTrue; // also in General Image Module
  // if (key == DCM_AcquisitionDuration) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ReferencedRawDataSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ReferencedWaveformSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ReferencedImageEvidenceSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_SourceImageEvidenceSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_SamplesPerPixel) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_PhotometricInterpretation) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_BitsAllocated) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_BitsStored) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_HighBit) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_ContentQualification) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ImageComments) return OFTrue; // also in General Image Module
  // if (key == DCM_BurnedInAnnotation) return OFTrue; // also in General Image Module
  // if (key == DCM_RecognizableVisualFeatures) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompression) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompressionRatio) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompressionMethod) return OFTrue; // also in General Image Module
  // if (key == DCM_PresentationLUTShape) return OFTrue; // also in General Image Module
  // if (key == DCM_IconImageSequence) return OFTrue; // also in General Image Module

  // any attributes of the Enhanced US Image module that are present
  // if (key == DCM_ImageType) return OFTrue; // also in General Image Module
  // if (key == DCM_SamplesPerPixel) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_PhotometricInterpretation) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_BitsAllocated) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_BitsStored) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_HighBit) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_PixelRepresentation) return OFTrue; // also in Image Pixel Module
  if (key == DCM_DimensionOrganizationType) return OFTrue;
  // if (key == DCM_AcquisitionDateTime) return OFTrue; // also in General Image Module
  // if (key == DCM_AcquisitionDuration) return OFTrue;  // also in Enhanced MR Image Module
  if (key == DCM_PositionMeasuringDeviceUsed) return OFTrue;
  // if (key == DCM_LossyImageCompression) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompressionRatio) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompressionMethod) return OFTrue; // also in General Image Module
  // if (key == DCM_PresentationLUTShape) return OFTrue; // also in General Image Module
  if (key == DCM_RescaleIntercept) return OFTrue;
  if (key == DCM_RescaleSlope) return OFTrue;
  if (key == DCM_SourceImageSequence) return OFTrue;
  if (key == DCM_ReferencedImageSequence) return OFTrue;
  // if (key == DCM_ReferencedRawDataSequence) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_ReferencedInstanceSequence) return OFTrue; // also in SR Document General Module
  if (key == DCM_NumberOfStages) return OFTrue;
  if (key == DCM_StageNumber) return OFTrue;
  if (key == DCM_StageCodeSequence) return OFTrue;
  if (key == DCM_EventTimerSequence) return OFTrue;
  // if (key == DCM_BurnedInAnnotation) return OFTrue; // also in General Image Module
  // if (key == DCM_RecognizableVisualFeatures) return OFTrue; // also in General Image Module
  // if (key == DCM_IconImageSequence) return OFTrue; // also in General Image Module
  if (key == DCM_TransducerData) return OFTrue;
  if (key == DCM_TransducerGeometryCodeSequence) return OFTrue;
  if (key == DCM_TransducerBeamSteeringCodeSequence) return OFTrue;
  if (key == DCM_TransducerApplicationCodeSequence) return OFTrue;
  if (key == DCM_ProcessingFunction) return OFTrue;
  if (key == DCM_MechanicalIndex) return OFTrue;
  if (key == DCM_BoneThermalIndex) return OFTrue;
  if (key == DCM_CranialThermalIndex) return OFTrue;
  if (key == DCM_SoftTissueThermalIndex) return OFTrue;
  if (key == DCM_DepthsOfFocus) return OFTrue;
  if (key == DCM_DepthOfScanField) return OFTrue;

  // any attributes of the Surface Segmentation module that are present
  // if (key == DCM_InstanceNumber) return OFTrue; // also in General Image Module
  // if (key == DCM_ContentLabel) return OFTrue; // also in Raw Data Module
  // if (key == DCM_ContentDescription) return OFTrue; // also in Raw Data Module
  // if (key == DCM_ConceptNameCodeSequence) return OFTrue; // also in SR Document Content Module
  if (key == DCM_AlternateContentDescriptionSequence) return OFTrue;
  if (key == DCM_ContentCreatorName) return OFTrue;
  if (key == DCM_ContentCreatorIdentificationCodeSequence) return OFTrue;
  // if (key == DCM_ContentDate) return OFTrue; // also in General Image Module
  // if (key == DCM_ContentTime) return OFTrue; // also in General Image Module
  // if (key == DCM_SegmentSequence) return OFTrue; // also in Segmentation Image Module

  // any attributes of the Surface Mesh Module that are present
  if (key == DCM_NumberOfSurfaces) return OFTrue;
  if (key == DCM_SurfaceSequence) return OFTrue;

  // any attributes of the Structured Display module that are present
  // if (key == DCM_InstanceNumber) return OFTrue; // also in General Image Module
  // if (key == DCM_ContentLabel) return OFTrue; // also in Raw Data Module
  // if (key == DCM_ContentDescription) return OFTrue; // also in Raw Data Module
  // if (key == DCM_ConceptNameCodeSequence) return OFTrue; // also in SR Document Content Module
  // if (key == DCM_AlternateContentDescriptionSequence) return OFTrue; // also in Surface Segmentation Module
  // if (key == DCM_ContentCreatorName) return OFTrue; // also in Surface Segmentation Module
  // if (key == DCM_ContentCreatorIdentificationCodeSequence) return OFTrue; // also in Surface Segmentation Module
  if (key == DCM_PresentationCreationDate) return OFTrue;
  if (key == DCM_PresentationCreationTime) return OFTrue;
  if (key == DCM_NumberOfScreens) return OFTrue;
  if (key == DCM_NominalScreenDefinitionSequence) return OFTrue;
  // if (key == DCM_IconImageSequence) return OFTrue; // also in General Image Module
  if (key == DCM_StructuredDisplayBackgroundCIELabValue) return OFTrue;
  if (key == DCM_EmptyImageBoxCIELabValue) return OFTrue;
  if (key == DCM_HangingProtocolName) return OFTrue;
  if (key == DCM_HangingProtocolCreator) return OFTrue;

  // any attributes of the Structured Display Annotation module that are present
  if (key == DCM_StructuredDisplayTextBoxSequence) return OFTrue;

  // any attributes of the Structured Display Image Box module that are present
  if (key == DCM_StructuredDisplayImageBoxSequence) return OFTrue;
  if (key == DCM_ImageBoxSynchronizationSequence) return OFTrue;

  // any Attributes of the Implant Template module that are present
  if (key == DCM_Manufacturer) return OFTrue;
  if (key == DCM_FrameOfReferenceUID) return OFTrue;
  if (key == DCM_ImplantName) return OFTrue;
  if (key == DCM_ImplantSize) return OFTrue;
  if (key == DCM_ImplantPartNumber) return OFTrue;
  if (key == DCM_ImplantTemplateVersion) return OFTrue;
  if (key == DCM_ReplacedImplantTemplateSequence) return OFTrue;
  if (key == DCM_ImplantType) return OFTrue;
  if (key == DCM_OriginalImplantTemplateSequence) return OFTrue;
  if (key == DCM_DerivationImplantTemplateSequence) return OFTrue;
  if (key == DCM_EffectiveDateTime) return OFTrue;
  if (key == DCM_ImplantTargetAnatomySequence) return OFTrue;
  if (key == DCM_NotificationFromManufacturerSequence) return OFTrue;
  if (key == DCM_InformationFromManufacturerSequence) return OFTrue;
  if (key == DCM_ImplantRegulatoryDisapprovalCodeSequence) return OFTrue;
  if (key == DCM_OverallTemplateSpatialTolerance) return OFTrue;
  if (key == DCM_MaterialsCodeSequence) return OFTrue;
  if (key == DCM_CoatingMaterialsCodeSequence) return OFTrue;
  if (key == DCM_ImplantTypeCodeSequence) return OFTrue;
  if (key == DCM_FixationMethodCodeSequence) return OFTrue;

  // any Attributes of the Implant Assembly Template module that are present
  // if (key == DCM_EffectiveDateTime) return OFTrue; // also in Implant Template Module
  if (key == DCM_ImplantAssemblyTemplateName) return OFTrue;
  if (key == DCM_ImplantAssemblyTemplateIssuer) return OFTrue;
  if (key == DCM_ImplantAssemblyTemplateVersion) return OFTrue;
  if (key == DCM_ReplacedImplantAssemblyTemplateSequence) return OFTrue;
  if (key == DCM_ImplantAssemblyTemplateType) return OFTrue;
  if (key == DCM_OriginalImplantAssemblyTemplateSequence) return OFTrue;
  if (key == DCM_DerivationImplantAssemblyTemplateSequence) return OFTrue;
  if (key == DCM_ImplantAssemblyTemplateTargetAnatomySequence) return OFTrue;
  if (key == DCM_ProcedureTypeCodeSequence) return OFTrue;
  if (key == DCM_SurgicalTechnique) return OFTrue;
  // if (key == DCM_MIMETypeOfEncapsulatedDocument) return OFTrue; // also in Encapsulated Document Module
  // if (key == DCM_EncapsulatedDocument) return OFTrue; // also in Encapsulated Document Module
  if (key == DCM_ComponentTypesSequence) return OFTrue;
  if (key == DCM_ComponentAssemblySequence) return OFTrue;

  // any Attributes of the Implant Template Group module that are present
  // if (key == DCM_EffectiveDateTime) return OFTrue; // also in Implant Template Module
  if (key == DCM_ImplantTemplateGroupName) return OFTrue;
  if (key == DCM_ImplantTemplateGroupDescription) return OFTrue;
  if (key == DCM_ImplantTemplateGroupIssuer) return OFTrue;
  if (key == DCM_ImplantTemplateGroupVersion) return OFTrue;
  if (key == DCM_ReplacedImplantTemplateGroupSequence) return OFTrue;
  if (key == DCM_ImplantTemplateGroupTargetAnatomySequence) return OFTrue;
  if (key == DCM_ImplantTemplateGroupMembersSequence) return OFTrue;
  if (key == DCM_ImplantTemplateGroupVariationDimensionSequence) return OFTrue;

  // any attributes of the Point Cloud Module that are present
  if (key == DCM_SurfacePointsSequence) return OFTrue;
  if (key == DCM_SurfacePointPresentationValueData) return OFTrue;
  if (key == DCM_SurfacePointColorCIELabValueData) return OFTrue;

  // any attributes of the Enhanced Mammography Image module that are present
  if (key == DCM_PositionerMotion) return OFTrue;
  if (key == DCM_PositionerType) return OFTrue;
  // if (key == DCM_ContentQualification) return OFTrue;  // also in Enhanced MR Image Module
  // if (key == DCM_AcquisitionDateTime) return OFTrue; // also in General Image Module
  // if (key == DCM_AcquisitionDuration) return OFTrue;  // also in Enhanced MR Image Module
  if (key == DCM_KVP) return OFTrue;
  if (key == DCM_XRayTubeCurrentInmA) return OFTrue;
  if (key == DCM_ExposureTimeInms) return OFTrue;
  if (key == DCM_ExposureInmAs) return OFTrue;
  if (key == DCM_FocalSpots) return OFTrue;
  if (key == DCM_AnodeTargetMaterial) return OFTrue;
  if (key == DCM_BodyPartThickness) return OFTrue;
  if (key == DCM_CompressionForce) return OFTrue;
  if (key == DCM_CompressionPressure) return OFTrue;
  if (key == DCM_CompressionContactArea) return OFTrue;
  if (key == DCM_PaddleDescription) return OFTrue;
  if (key == DCM_ExposureControlMode) return OFTrue;
  if (key == DCM_ExposureControlModeDescription) return OFTrue;
  if (key == DCM_PatientOrientation) return OFTrue;
  // if (key == DCM_ImageComments) return OFTrue; // also in General Image Module
  // if (key == DCM_SamplesPerPixel) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_PhotometricInterpretation) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_BitsAllocated) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_BitsStored) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_HighBit) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_PixelRepresentation) return OFTrue; // also in Image Pixel Module
  // if (key == DCM_QualityControlImage) return OFTrue; // also in General Image Module
  // if (key == DCM_BurnedInAnnotation) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompression) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompressionRatio) return OFTrue; // also in General Image Module
  // if (key == DCM_LossyImageCompressionMethod) return OFTrue; // also in General Image Module
  if (key == DCM_OrganDose) return OFTrue;
  if (key == DCM_EntranceDoseInmGy) return OFTrue;
  if (key == DCM_EntranceDoseDerivation) return OFTrue;
  if (key == DCM_TypeOfDetectorMotion) return OFTrue;
  // if (key == DCM_IconImageSequence) return OFTrue; // also in General Image Module
  // if (key == DCM_PresentationLUTShape) return OFTrue; // also in General Image Module

  // any attributes of the Volumetric Graphic Annotation Module that are present
  if (key == DCM_VolumetricAnnotationSequence) return OFTrue;
  if (key == DCM_VolumetricPresentationInputAnnotationSequence) return OFTrue;

  /* Any attributes whose values are verifiable by the technician or physician
   * (e.g., their values are displayed to the technician or physician)
   * This is obviously application dependent. We don't handle that here.
   */
  return OFFalse;
}


OFBool SiAuthorizationProfile::checkRequiredAttributeList(DcmAttributeTag& tagList) const
{
  OFBool result =
    containsTag(tagList, DCM_SOPClassUID) &&
    containsTag(tagList, DCM_StudyInstanceUID) &&
    containsTag(tagList, DCM_SeriesInstanceUID) &&
    containsTag(tagList, DCM_SOPInstanceUID);

  return result;
}


OFCondition SiAuthorizationProfile::inspectSignatureDataset(DcmItem &item)
{
  DcmElement *delem = NULL;
  if (item.findAndGetElement(DCM_CreatorVersionUID, delem).good())
  {
    // The CreatorVersionUID attribute is present in the dataset or item.
    // Since this attribute only occurs in the Raw Data Module, this means that
    // the Raw Data Module is present.
    containsRawData_ = OFTrue;
  }
  else
  {
    containsRawData_ = OFFalse;
  }
  return EC_Normal;
}

OFBool SiAuthorizationProfile::mainDatasetRequired() const
{
  return OFTrue;
}

#else /* WITH_OPENSSL */

int siautopr_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
