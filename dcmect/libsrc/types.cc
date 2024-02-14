/*
 *
 *  Copyright (C) 2024, Open Connections GmbH
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
 *
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for managing common Enhanced CT-specific types
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmect/types.h"
#include "dcmtk/dcmdata/dcerror.h"

OFLogger DCM_dcmectLogger = OFLog::getLogger("dcmtk.dcmect");

/*---------------------------------*
 *  constant definitions
 *---------------------------------*/

// conditions
// makeOFConditionConst(ECT_EC_....,      OFM_dcmect, 1, OF_error, "...");

const OFString EctTypes::DT_ImageType3_Angio         = "ANGIO";
const OFString EctTypes::DT_ImageType3_Cardiac       = "CARDICAC";
const OFString EctTypes::DT_ImageType3_CardiacGated  = "CARDIAC_GATED";
const OFString EctTypes::DT_ImageType3_CardRespGated = "CARDRESP_GATED";
const OFString EctTypes::DT_ImageType3_Dynamic       = "DYNAMIC";
const OFString EctTypes::DT_ImageType3_Fluoroscopy   = "FLOUROSCOPY";
const OFString EctTypes::DT_ImageType3_Localizer     = "LOCALIZER";
const OFString EctTypes::DT_ImageType3_Motion        = "MOTION";
const OFString EctTypes::DT_ImageType3_Perfusion     = "PERFUSION";
const OFString EctTypes::DT_ImageType3_PreContrast   = "PRE_CONTRAST";
const OFString EctTypes::DT_ImageType3_PostContrast  = "POST_CONTRAST";
const OFString EctTypes::DT_ImageType3_RespGated     = "RESP_GATED";
const OFString EctTypes::DT_ImageType3_Rest          = "REST";
const OFString EctTypes::DT_ImageType3_Static        = "STATIC";
const OFString EctTypes::DT_ImageType3_Stress        = "STRESS";
const OFString EctTypes::DT_ImageType3_Volume        = "VOLUME";
const OFString EctTypes::DT_ImageType3_NonParallel   = "NON_PARALLEL";
const OFString EctTypes::DT_ImageType3_WholeBody     = "WHOLE_BODY";

const OFString EctTypes::DT_ImageType4_Addition       = "ADDITION";
const OFString EctTypes::DT_ImageType4_Division       = "DIVISION";
const OFString EctTypes::DT_ImageType4_Masked         = "MASKED";
const OFString EctTypes::DT_ImageType4_Maximum        = "MAXIMUM";
const OFString EctTypes::DT_ImageType4_Mean           = "MEAN";
const OFString EctTypes::DT_ImageType4_Minimum        = "MINIMUM";
const OFString EctTypes::DT_ImageType4_Multiplication = "MULTIPLICATION";
const OFString EctTypes::DT_ImageType4_Resampled      = "RESAMPLED";
const OFString EctTypes::DT_ImageType4_Std_Deviation  = "STD_DEVIATION";
const OFString EctTypes::DT_ImageType4_Subtraction    = "SUBTRACTION";
const OFString EctTypes::DT_ImageType4_None           = "NONE";
const OFString EctTypes::DT_ImageType4_Quantity       = "QUANTITY";
const OFString EctTypes::DT_ImageType4_Mixed          = "MIXED";

const OFString EctTypes::DT_VolBasedCalcTechnique_MaxIp         = "MAX_IP";
const OFString EctTypes::DT_VolBasedCalcTechnique_MinIp         = "MIN_IP";
const OFString EctTypes::DT_VolBasedCalcTechnique_VolumeRender  = "VOLUME_RENDER";
const OFString EctTypes::DT_VolBasedCalcTechnique_SurfaceRender = "SURFACE_RENDER";
const OFString EctTypes::DT_VolBasedCalcTechnique_Mpr           = "MPR";
const OFString EctTypes::DT_VolBasedCalcTechnique_CurvedMpr     = "CURVED_MPR";
const OFString EctTypes::DT_VolBasedCalcTechnique_None          = "NONE";
const OFString EctTypes::DT_VolBasedCalcTechnique_Mixed         = "MIXED";

// conditions
makeOFConditionConst(ECT_InvalidDimensions, OFM_dcmect, 1, OF_error, "Invalid Dimensions");
makeOFConditionConst(ECT_InvalidAttributeValue, OFM_dcmect, 2, OF_error, "Invalid Attribute Value");
makeOFConditionConst(
    ECT_InvalidPixelInfo, OFM_dcmect, 3, OF_error, "Invalid information in pixel data or related attributes");
makeOFConditionConst(ECT_InvalidPixelData, OFM_dcmect, 4, OF_error, "Invalid pixel data");
makeOFConditionConst(ECT_NoPixelData, OFM_dcmect, 5, OF_error, "No pixel data found");
makeOFConditionConst(ECT_InvalidSOPClass, OFM_dcmect, 6, OF_error, "SOP Class not supported");

OFString EctTypes::imageType1ToStr(const E_ImageType1 value)
{
    switch (value)
    {
        case E_ImageType1_Empty:
            return "";
            break;
        case E_ImageType1_Invalid:
            return "";
            break;
        case E_ImageType1_Original:
            return "ORIGINAL";
            break;
        case E_ImageType1_Derived:
            return "DERIVED";
            break;
        case E_ImageType1_Mixed:
            return "MIXED";
            break;
        default:
            DCMECT_ERROR("Internal error, invalid value for E_ImageType1: " << value);
            return "";
    }
}

OFString EctTypes::imageType2ToStr(const E_ImageType2 value)
{
    switch (value)
    {
        case E_ImageType2_Empty:
            return "";
            break;
        case E_ImageType2_Invalid:
            return "";
            break;
        case E_ImageType2_Primary:
            return "PRIMARY";
            break;
        default:
            DCMECT_ERROR("Internal error, invalid value for E_ImageType2: " << value);
            return "";
    }
}

OFString EctTypes::contentQualiToStr(const E_ContentQualification value)
{
    switch (value)
    {
        case E_ContentQuali_Empty:
            return "";
            break;
        case E_ContentQuali_Invalid:
            return "";
            break;
        case E_ContQuali_Product:
            return "PRODUCT";
            break;
        case E_ContQuali_Research:
            return "RESEARCH";
            break;
        case E_ContQuali_Service:
            return "SERVICE";
            break;
        default:
            DCMECT_ERROR("Internal error, invalid value for E_ContentQualification: " << value);
            return "";
    }
}

OFString EctTypes::burnedAnnoToStr(const E_BurnedInAnnotation value)
{
    switch (value)
    {
        case E_BurnedInAnno_Empty:
            return "";
            break;
        case E_BurnedInAnno_Invalid:
            return "";
            break;
        case E_BurnedInAnno_No:
            return "NO";
            break;
        default:
            DCMECT_ERROR("Internal error, invalid value for E_BurnedInAnnotation: " << value);
            return "";
    }
}

OFString EctTypes::pixelPresToStr(const E_PixelPresentation value)
{
    switch (value)
    {
        case E_PixelPres_Empty:
            return "";
            break;
        case E_PixelPres_Invalid:
            return "";
            break;
        case E_PixelPres_Color:
            return "COLOR";
            break;
        case E_PixelPres_Mixed:
            return "MIXED";
            break;
        case E_PixelPres_Monochrome:
            return "MONOCHROME";
        case E_PixelPres_TrueColor:
            return "TRUE_COLOR";
        default:
            DCMECT_ERROR("Internal error, invalid value for E_PixelPresentation: " << value);
            return "";
    }
}

OFString EctTypes::volPropsToStr(const E_VolumetricProperties value)
{
    switch (value)
    {
        case E_VolProps_Empty:
            return "";
            break;
        case E_VolProps_Invalid:
            return "";
            break;
        case E_VolProps_Distorted:
            return "DISTORTED";
            break;
        case E_VolProps_Mixed:
            return "MIXED";
            break;
        case E_VolProps_Sampled:
            return "SAMPLED";
            break;
        case E_VolProps_Volume:
            return "VOLUME";
            break;
        default:
            DCMECT_ERROR("Internal error, invalid value for E_VolumetricProperties: " << value);
            return "";
    }
}

OFString EctTypes::recoVisFeaturesToStr(const E_RecognizableVisualFeatures value)
{
    switch (value)
    {
        case E_RecoVisFeatures_Empty:
            return "";
            break;
        case E_RecoVisFeatures_Invalid:
            return "";
            break;
        case E_RecoVisFeatures_No:
            return "NO";
            break;
        case E_RecoVisFeatures_Yes:
            return "YES";
            break;
        default:
            DCMECT_ERROR("Internal error, invalid value for E_RecognizableVisualFeatures: " << value);
            return "";
    }
}
