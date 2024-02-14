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
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for managing common functional group types
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmfg/fgtypes.h"

OFLogger DCM_dcmfgLogger = OFLog::getLogger("dcmtk.dcmfg");

/*---------------------------------*
 *  constant definitions
 *---------------------------------*/

// Error Conditions
makeOFConditionConst(FG_EC_DoubledFG, OFM_dcmfg, 1, OF_error, "Doubled Functional Group");
makeOFConditionConst(FG_EC_NoSuchGroup, OFM_dcmfg, 2, OF_error, "No such Functional Group");
makeOFConditionConst(FG_EC_NotEnoughItems, OFM_dcmfg, 3, OF_error, "Not enough Items in Functional Group");
makeOFConditionConst(FG_EC_TooManyItems, OFM_dcmfg, 4, OF_error, "Too many Items in Functional Group");
makeOFConditionConst(FG_EC_InvalidData, OFM_dcmfg, 5, OF_error, "Invalid data in Functional Group");
makeOFConditionConst(FG_EC_CouldNotWriteFG, OFM_dcmfg, 6, OF_error, "Could not write Functional Group");
makeOFConditionConst(FG_EC_CouldNotInsertFG, OFM_dcmfg, 7, OF_error, "Could not insert Functional Group");
makeOFConditionConst(FG_EC_NoSharedFG, OFM_dcmfg, 8, OF_error, "No shared Functional Groups found");
makeOFConditionConst(FG_EC_NoPerFrameFG, OFM_dcmfg, 9, OF_error, "No Per-Frame Functional Groups found");
makeOFConditionConst(FG_EC_CouldNotCreateFG, OFM_dcmfg, 10, OF_error, "Could not create Functional Group");
makeOFConditionConst(FG_EC_CouldNotReadSourceImage, OFM_dcmfg, 11, OF_error, "Could not read source image");
makeOFConditionConst(FG_EC_CouldNotAddFG, OFM_dcmfg, 12, OF_error, "Could add Functional Group");
makeOFConditionConst(FG_EC_NotEnoughFrames, OFM_dcmfg, 13, OF_error, "Not enough frames");
makeOFConditionConst(FG_EC_NoStacksFound, OFM_dcmfg, 14, OF_error, "No stacks found");
makeOFConditionConst(FG_EC_SOPClassForbidsConcatenations, OFM_dcmfg, 15, OF_error, "SOP Class forbids Concatenations");
makeOFConditionConst(FG_EC_PixelDataMissing, OFM_dcmfg, 16, OF_error, "Pixel Data is missing");
makeOFConditionConst(FG_EC_PixelDataDimensionsInvalid, OFM_dcmfg, 17, OF_error, "Pixel Data dimensions invalid");
makeOFConditionConst(FG_EC_PixelDataTooLarge, OFM_dcmfg, 18, OF_error, "Pixel Data too large");
makeOFConditionConst(FG_EC_InconsistentConcatenationData, OFM_dcmfg, 19, OF_error, "Inconsistent Concatenation Data");
makeOFConditionConst(FG_EC_ConcatenationComplete, OFM_dcmfg, 20, OF_error, "Concatenation Complete - no more data");
makeOFConditionConst(FG_EC_UnsupportedPixelDataLayout, OFM_dcmfg, 21, OF_error, "Unsupported pixel data layout");

OFString DcmFGTypes::FGType2OFString(const DcmFGTypes::E_FGType fgType)
{
    switch (fgType)
    {
        /// Undefined functional group
        case EFG_UNDEFINED:
            return "Undefined Functional Group Macro";
            break;
        /// Unknown functional group
        case EFG_UNKNOWN:
            return "Unknown Functional Group Macro";
            break;
        /// Pixel Measures
        case EFG_PIXELMEASURES:
            return "Pixel Measures Functional Group Macro";
            break;
        /// Frame Content
        case EFG_FRAMECONTENT:
            return "Frame Content Functional Group Macro";
            break;
        /// CT Acquisition Details
        case EFG_CTACQUISITIONDETAILS:
            return "CT Acquisition Details Functional Group Macro";
            break;
        /// CT Acquisition Type
        case EFG_CTACQUISITIONTYPE:
            return "CT Acquisition Type Functional Group Macro";
            break;
        /// CT Additional X-Ray Source
        case EFG_CTADDITIONALXRAYSOURCE:
            return "CT Additional X-Ray Source";
            break;
        /// CT Exposure
        case EFG_CTEXPOSURE:
            return "CT Exposure Functional Group Macro";
            break;
        case EFG_CTGEOMETRY:
            return "CT Geometry Functional Group Macro";
        /// CT Image Frame Type
        case EFG_CTIMAGEFRAMETYPE:
            return "CT Image Frame Type Functional Group Macro";
            break;
        /// CT Position
        case EFG_CTPOSITION:
            return "CT Position Functional Group Macro";
            break;
        /// CT Reconstruction
        case EFG_CTRECONSTRUCTION:
            return "CT Reconstruction Functional Group Macro";
            break;
        /// CT Table Dynamics
        case EFG_CTTABLEDYNAMICS:
            return "CT Table Dynamics Functional Group Macro";
            break;
        /// CT X-Ray Details
        case EFG_CTXRAYDETAILS:
            return "CT X-Ray Details Functional Group Macro";
            break;
        /// Plane Position (Patient)
        case EFG_PLANEPOSPATIENT:
            return "Plane Position (Patient) Functional Group Macro";
            break;
        /// Plane Orientation (Patient)
        case EFG_PLANEORIENTPATIENT:
            return "Plane Orientation (Patient) Functional Group Macro";
            break;
        /// Derivation Image
        case EFG_DERIVATIONIMAGE:
            return "Derivation Image Functional Group Macro";
            break;
        /// Cardiac Synchronization
        case EFG_CARDIACSYNC:
            return "Cardiac Synchronization Functional Group Macro";
            break;
        /// Frame Anatomy
        case EFG_FRAMEANATOMY:
            return "Frame Anatomy Functional Group Macro";
            break;
        /// Pixel Value Transformation or Identity Pixel Value Transformation
        case EFG_PIXELVALUETRANSMETA:
            return "[CT|Identity] Pixel Value Transformation Functional Group Macro";
            break;
        /// Frame VOI LUT or Frame VOI LUT with LUT
        case EFG_FRAMEVOILUTMETA:
            return "Frame VOI LUT / Frame VOI LUT with LUT Macro";
            break;
        /// Real World Value Mapping
        case EFG_REALWORLDVALUEMAPPING:
            return "Real World Value Mapping Functional Group Macro";
            break;
        /// Contrast/Bolus Usage
        case EFG_CONTRASTBOLUSUSAGE:
            return "Contrast/Bolus Usage Group Macro";
            break;
        /// Pixel Intensity Relationship LUT
        case EFG_PIXELINTENSITYRELLUT:
            return "Pixel Intensity Relation LUT Functional Group Macro";
            break;
        /// Frame Pixel Shift
        case EFG_FRAMEPIXELSHIFT:
            return "Frame Pixel Shift Functional Group Macro";
            break;
        /// Patient Orientation in Frame
        case EFG_PATIENTORIENTINFRAME:
            return "Patient Orientation in Frame Functional Group Macro";
            break;
        /// Frame Display Shutter
        case EFG_FRAMEDISPLAYSHUTTER:
            return "Frame Display Shutter Functional Group Macro";
            break;
        /// Respiratory Synchronization
        case EFG_RESPIRATORYSYNC:
            return "Respiratory Synchronization Functional Group Macro";
            break;
        /// Irradiation Event Identification
        case EFG_IRRADIATIONEVENTIDENT:
            return "Irradiation Event Identification Functional Group Macro";
            break;
        /// Radiopharmaceutical Usage
        case EFG_RADIOPHARAMAUSAGE:
            return "Radiopharmaceutical Usage Functional Group Macro";
            break;
        /// Parametric Map Frame Type
        case EFG_PARAMETRICMAPFRAMETYPE:
            return "Parametric Map Frame Type Functional Group Macro";
            break;
        /// Patient Physiological State
        case EFG_PATIENTPHYSIOSTATE:
            return "Patient Physiological State Functional Group Macro";
            break;
        /// Plane Position (Volume)
        case EFG_PLANEPOSITIONVOLUME:
            return "Plane Position (Volume) Functional Group Macro";
            break;
        /// Plane Orientation (Volume)
        case EFG_PLANEORIENTVOLUME:
            return "Plane Orientation (Volume) Functional Group Macro";
            break;
        /// Temporal Position Macro
        case EFG_TEMPORALPOSITION:
            return "Temporal Position Functional Group Macro";
            break;
        /// Image Data Type
        case EFG_IMAGEDATATYPE:
            return "Image Data Type Functional Group Macro";
            break;
        /// Unassigned Shared Converted Attributes Macro
        case EFG_UNASSIGNEDSHAREDCONVERTEDATTRIBUTES:
            return "Unassigned Shared Converted Attributes Macro";
            break;
        /// Segmentation Macro
        case EFG_SEGMENTATION:
            return "Segmentation Functional Group Macro";
            break;
        /// US Image Description Functional Group Macro
        case EFG_USIMAGEDESCRIPTION:
            return "US Image Description Functional Group Macro";
            break;
    }
    return "Unknown Functional Group Macro (internal error)";
}

DcmFGTypes::E_FGType DcmFGTypes::tagKey2FGType(const DcmTagKey& key)
{
    // Note: Use neat value to enum trick from Alexandrescu in order to have switch statement instead?
    if (key == DCM_PixelMeasuresSequence)
        return EFG_PIXELMEASURES;
    else if (key == DCM_FrameContentSequence)
        return EFG_FRAMECONTENT;
    else if (key == DCM_CTAcquisitionDetailsSequence)
        return EFG_CTACQUISITIONDETAILS;
    else if (key == DCM_CTAdditionalXRaySourceSequence)
        return EFG_CTADDITIONALXRAYSOURCE;
    else if (key == DCM_CTAcquisitionTypeSequence)
        return EFG_CTACQUISITIONTYPE;
    else if (key == DCM_CTExposureSequence)
        return EFG_CTEXPOSURE;
    else if (key == DCM_CTGeometrySequence)
        return EFG_CTGEOMETRY;
    else if (key == DCM_CTImageFrameTypeSequence)
        return EFG_CTIMAGEFRAMETYPE;
    else if (key == DCM_CTPositionSequence)
        return EFG_CTPOSITION;
    else if (key == DCM_CTReconstructionSequence)
        return EFG_CTRECONSTRUCTION;
    else if (key == DCM_CTTableDynamicsSequence)
        return EFG_CTTABLEDYNAMICS;
    else if (key == DCM_CTXRayDetailsSequence)
        return EFG_CTXRAYDETAILS;
    else if (key == DCM_PlanePositionSequence)
        return EFG_PLANEPOSPATIENT;
    else if (key == DCM_PlaneOrientationSequence)
        return EFG_PLANEORIENTPATIENT;
    else if (key == DCM_DerivationImageSequence)
        return EFG_DERIVATIONIMAGE;
    else if (key == DCM_CardiacSynchronizationSequence)
        return EFG_CARDIACSYNC;
    else if (key == DCM_FrameAnatomySequence)
        return EFG_FRAMEANATOMY;
    else if (key == DCM_PixelValueTransformationSequence)
        return EFG_PIXELVALUETRANSMETA;
    else if (key == DCM_FrameVOILUTSequence)
        return EFG_FRAMEVOILUTMETA;
    else if (key == DCM_RealWorldValueMappingSequence)
        return EFG_REALWORLDVALUEMAPPING;
    else if (key == DCM_ContrastBolusUsageSequence)
        return EFG_CONTRASTBOLUSUSAGE;
    else if (key == DCM_PixelIntensityRelationshipLUTSequence)
        return EFG_PIXELINTENSITYRELLUT;
    else if (key == DCM_FramePixelShiftSequence)
        return EFG_FRAMEPIXELSHIFT;
    else if (key == DCM_PatientOrientationInFrameSequence)
        return EFG_PATIENTORIENTINFRAME;
    else if (key == DCM_FrameDisplayShutterSequence)
        return EFG_FRAMEDISPLAYSHUTTER;
    else if (key == DCM_RespiratorySynchronizationSequence)
        return EFG_RESPIRATORYSYNC;
    else if (key == DCM_IrradiationEventIdentificationSequence)
        return EFG_IRRADIATIONEVENTIDENT;
    else if (key == DCM_RadiopharmaceuticalUsageSequence)
        return EFG_RADIOPHARAMAUSAGE;
    else if (key == DCM_PatientPhysiologicalStateSequence)
        return EFG_PATIENTPHYSIOSTATE;
    else if (key == DCM_ParametricMapFrameTypeSequence)
        return EFG_PARAMETRICMAPFRAMETYPE;
    else if (key == DCM_PlanePositionVolumeSequence)
        return EFG_PLANEPOSITIONVOLUME;
    else if (key == DCM_PlaneOrientationVolumeSequence)
        return EFG_PLANEORIENTVOLUME;
    else if (key == DCM_TemporalPositionSequence)
        return EFG_TEMPORALPOSITION;
    else if (key == DCM_ImageDataTypeSequence)
        return EFG_IMAGEDATATYPE;
    else if (key == DCM_UnassignedSharedConvertedAttributesSequence)
        return EFG_UNASSIGNEDSHAREDCONVERTEDATTRIBUTES;
    else if (key == DCM_SegmentIdentificationSequence)
        return EFG_SEGMENTATION;
    else if (key == DCM_USImageDescriptionSequence)
        return EFG_USIMAGEDESCRIPTION;
    else
        return EFG_UNKNOWN;
}

OFString DcmFGTypes::tagKey2FGString(const DcmTagKey& key)
{
    E_FGType fgtype = tagKey2FGType(key);
    return FGType2OFString(fgtype);
}
