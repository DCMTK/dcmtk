/*
 *
 *  Copyright (C) 2019, Open Connections GmbH
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
 *  Purpose: Class for managing the CT Image Frame Type Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmfg/fgctimageframetype.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/dcmiod/iodutil.h"

const OFString FGCTImageFrameType::DT_VolBasedCalcTechnique_MaxIp         = "MAX_IP";
const OFString FGCTImageFrameType::DT_VolBasedCalcTechnique_MinIp         = "MIN_IP";
const OFString FGCTImageFrameType::DT_VolBasedCalcTechnique_VolumeRender  = "VOLUME_RENDER";
const OFString FGCTImageFrameType::DT_VolBasedCalcTechnique_SurfaceRender = "SURFACE_RENDER";
const OFString FGCTImageFrameType::DT_VolBasedCalcTechnique_Mpr           = "MPR";
const OFString FGCTImageFrameType::DT_VolBasedCalcTechnique_CurvedMpr     = "CURVED_MPR";
const OFString FGCTImageFrameType::DT_VolBasedCalcTechnique_None          = "NONE";
const OFString FGCTImageFrameType::DT_VolBasedCalcTechnique_Mixed         = "MIXED";

// Constructor
FGCTImageFrameType::FGCTImageFrameType()
    : FGBase(DcmFGTypes::EFG_CTIMAGEFRAMETYPE)
    , m_FrameType(DCM_FrameType)
    , m_PixelPresentation(DCM_PixelPresentation)
    , m_VolumetricProperties(DCM_VolumetricProperties)
    , m_VolumeBasedCalculationTechnique(DCM_VolumeBasedCalculationTechnique)
{
}

FGCTImageFrameType::~FGCTImageFrameType()
{
    // nothing to do
}

FGBase* FGCTImageFrameType::clone() const
{
    FGCTImageFrameType* copy = new FGCTImageFrameType();
    if (copy)
    {
        copy->m_FrameType                       = this->m_FrameType;
        copy->m_PixelPresentation               = this->m_PixelPresentation;
        copy->m_VolumetricProperties            = this->m_VolumetricProperties;
        copy->m_VolumeBasedCalculationTechnique = this->m_VolumeBasedCalculationTechnique;
    }
    return copy;
}

void FGCTImageFrameType::clearData()
{
    m_FrameType.clear();
    m_PixelPresentation.clear();
    m_VolumetricProperties.clear();
    m_VolumeBasedCalculationTechnique.clear();
}

OFCondition FGCTImageFrameType::check() const
{
    // Maybe add checks later
    return EC_Normal;
}

/// Read CT Image Frame Type Sequence from given item
OFCondition FGCTImageFrameType::read(DcmItem& item)
{
    clearData();

    DcmItem* seqItem   = NULL;
    OFCondition result = getItemFromFGSequence(item, DCM_CTImageFrameTypeSequence, 0, seqItem);
    if (result.bad())
        return result;

    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_FrameType, "4", "1", "CTImageFrameTypeMacro");
    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_PixelPresentation, "1", "1", "CTImageFrameTypeMacro");
    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_VolumetricProperties, "1", "1", "CTImageFrameTypeMacro");
    DcmIODUtil::getAndCheckElementFromDataset(
        *seqItem, m_VolumeBasedCalculationTechnique, "1", "1", "CTImageFrameTypeMacro");
    return EC_Normal;
}

/// Writes single CT Image Frame Type Sequence into given item
OFCondition FGCTImageFrameType::write(DcmItem& item)
{
    DcmItem* seqItem   = NULL;
    OFCondition result = createNewFGSequence(item, DCM_CTImageFrameTypeSequence, 0, seqItem);
    if (result.bad())
        return result;

    // --- write frame content macro attributes ---
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_FrameType, "4", "1", "CTImageFrameTypeMacro");
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_PixelPresentation, "1", "1", "CTImageFrameTypeMacro");
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_VolumetricProperties, "1", "1", "CTImageFrameTypeMacro");
    DcmIODUtil::copyElementToDataset(
        result, *seqItem, m_VolumeBasedCalculationTechnique, "1", "1", "CTImageFrameTypeMacro");
    return result;
}

int FGCTImageFrameType::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result != 0)
        return result;

    const FGCTImageFrameType* myRhs = OFstatic_cast(const FGCTImageFrameType*, &rhs);
    if (!myRhs)
        return -1;

    // Compare all elements
    result = m_FrameType.compare(myRhs->m_FrameType);
    if (result == 0)
        result = m_PixelPresentation.compare(myRhs->m_PixelPresentation);
    if (result == 0)
        result = m_VolumetricProperties.compare(myRhs->m_VolumetricProperties);
    if (result == 0)
        result = m_VolumeBasedCalculationTechnique.compare(myRhs->m_VolumeBasedCalculationTechnique);

    return result;
}

// --- get() functionality ---

OFCondition FGCTImageFrameType::getFrameType(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_FrameType, value, pos);
}

OFCondition FGCTImageFrameType::getPixelPresentation(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_PixelPresentation, value, pos);
}

OFCondition FGCTImageFrameType::getPixelPresentation(FGCTImageFrameType::E_PixelPresentation& value, const long pos)
{
    OFString str;
    DcmIODUtil::getStringValueFromElement(m_PixelPresentation, str, pos);
    value = pixelPres2Enum(str);
    if ((value != FGCTImageFrameType::E_PixelPres_Empty) && (value != FGCTImageFrameType::E_PixelPres_Invalid))
        return EC_Normal;
    else
        return EC_InvalidValue;
}

OFCondition FGCTImageFrameType::getVolumetricProperties(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_VolumetricProperties, value, pos);
}

OFCondition FGCTImageFrameType::getVolumetricProperties(FGCTImageFrameType::E_VolumetricProperties& value,
                                                        const long pos)
{
    OFString str;
    DcmIODUtil::getStringValueFromElement(m_VolumetricProperties, str, pos);
    value = volProps2Enum(str);
    if ((value != FGCTImageFrameType::E_VolProp_Empty) && (value != FGCTImageFrameType::E_VolProp_Invalid))
        return EC_Normal;
    else
        return EC_InvalidValue;
}

OFCondition FGCTImageFrameType::getVolumeBasedCalculationTechnique(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_VolumeBasedCalculationTechnique, value, pos);
}

// --- set() functionality ---

OFCondition FGCTImageFrameType::setFrameType(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "4") : EC_Normal;
    if (result.good())
        result = m_FrameType.putOFStringArray(value);
    return result;
}

OFCondition FGCTImageFrameType::setPixelPresentation(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_PixelPresentation.putOFStringArray(value);
    return result;
}

OFCondition FGCTImageFrameType::setPixelPresentation(const FGCTImageFrameType::E_PixelPresentation& value,
                                                     const OFBool checkValue)
{
    (void)checkValue;
    OFString val;
    if (pixelPres2Str(value, val))
    {
        return m_PixelPresentation.putOFStringArray(val);
    }
    else
        return FG_EC_InvalidData;
}

OFCondition FGCTImageFrameType::setVolumetricProperties(const OFString& value, const OFBool checkValue)
{

    if (checkValue)
    {
        if (!volProps2Enum(value))
            return FG_EC_InvalidData;
    }
    return m_VolumetricProperties.putOFStringArray(value);
}

OFCondition FGCTImageFrameType::setVolumetricProperties(const FGCTImageFrameType::E_VolumetricProperties& value,
                                                        const OFBool checkValue)
{
    (void)checkValue;
    OFString val;
    if (volProps2Str(value, val))
    {
        return m_VolumetricProperties.putOFStringArray(val);
    }
    else
        return FG_EC_InvalidData;
}

OFCondition FGCTImageFrameType::setVolumeBasedCalculationTechnique(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_VolumeBasedCalculationTechnique.putOFStringArray(value);
    return result;
}

FGCTImageFrameType::E_PixelPresentation FGCTImageFrameType::pixelPres2Enum(const OFString& str)
{
    if (str == "COLOR")
        return E_PixelPres_Color;
    else if (str == "MONOCHROME")
        return E_PixelPres_Monochrome;
    else if (str == "MIXED")
        return E_PixelPres_Mixed;
    else if (str == "TRUE_COLOR")
        return E_PixelPres_TrueColor;
    else if (str.empty())
        return E_PixelPres_Empty;
    else
        return E_PixelPres_Invalid;
}

OFBool FGCTImageFrameType::pixelPres2Str(const FGCTImageFrameType::E_PixelPresentation& eval, OFString& result)
{
    switch (eval)
    {
        case E_PixelPres_Color:
            result = "COLOR";
            break;
        case E_PixelPres_Monochrome:
            result = "MONOCHROME";
            break;
        case E_PixelPres_Mixed:
            result = "MIXED";
            break;
        case E_PixelPres_TrueColor:
            result = "TRUE COLOR";
            break;
        case E_PixelPres_Empty:
            result = "";
            break;
        case E_PixelPres_Invalid:
            result = "";
            return OFFalse;
        default:
            result = "";
            DCMFG_WARN("Unknown value for enum FGCTImageFrameType::E_PixelPresentation: " << eval);
            return OFFalse;
    }
    return OFTrue;
}

FGCTImageFrameType::E_VolumetricProperties FGCTImageFrameType::volProps2Enum(const OFString& str)
{
    if (str == "DISTORTED")
        return E_VolProp_Distorted;
    else if (str == "MIXED")
        return E_VolProp_Mixed;
    else if (str == "SAMPLED")
        return E_VolProp_Sampled;
    else if (str == "VOLUME")
        return E_VolProp_Volume;
    else if (str.empty())
        return E_VolProp_Empty;
    else
        return E_VolProp_Invalid;
}

OFBool FGCTImageFrameType::volProps2Str(const FGCTImageFrameType::E_VolumetricProperties& eval, OFString& result)
{
    switch (eval)
    {
        case E_VolProp_Distorted:
            result = "DISTORTED";
            break;
        case E_VolProp_Mixed:
            result = "MIXED";
            break;
        case E_VolProp_Sampled:
            result = "SAMPLED";
            break;
        case E_VolProp_Volume:
            result = "VOLUME";
            break;
        case E_VolProp_Empty:
            result = "";
            break;
        case E_VolProp_Invalid:
            result = "";
            return OFFalse;
        default:
            result = "";
            DCMFG_WARN("Unknown value for enum FGCTImageFrameType::E_VolumetricProperties: " << eval);
            return OFFalse;
    }
    return OFTrue;
}
