/*
 *
 *  Copyright (C) 2019-2021, Open Connections GmbH
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
 *  Purpose: Class for managing the CT Acquisition Type Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmfg/fgctacquisitiontype.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/dcmiod/iodutil.h"

const OFString FGCTAcquisitionType::DT_AcquisitionType_Sequenced     = "SEQUENCED";
const OFString FGCTAcquisitionType::DT_AcquisitionType_Spiral        = "SPIRAL";
const OFString FGCTAcquisitionType::DT_AcquisitionType_ConstantAngle = "CONSTANT_ANGLE";
const OFString FGCTAcquisitionType::DT_AcquisitionType_Stationary    = "STATIONARY";
const OFString FGCTAcquisitionType::DT_AcquisitionType_Free          = "FREE";

// Constructor
FGCTAcquisitionType::FGCTAcquisitionType()
    : FGBase(DcmFGTypes::EFG_CTACQUISITIONTYPE)
    , m_AcquisitionType(DCM_AcquisitionType)
    , m_TubeAngle(DCM_TubeAngle)
    , m_ConstantVolumeFlag(DCM_ConstantVolumeFlag)
    , m_FluoroscopyFlag(DCM_FluoroscopyFlag)
{
}

FGCTAcquisitionType::~FGCTAcquisitionType()
{
    // nothing to do
}

FGBase* FGCTAcquisitionType::clone() const
{
    FGCTAcquisitionType* copy = new FGCTAcquisitionType();
    if (copy)
    {
        copy->m_AcquisitionType    = this->m_AcquisitionType;
        copy->m_TubeAngle          = this->m_TubeAngle;
        copy->m_ConstantVolumeFlag = this->m_ConstantVolumeFlag;
        copy->m_FluoroscopyFlag    = this->m_FluoroscopyFlag;
    }
    return copy;
}

void FGCTAcquisitionType::clearData()
{
    m_AcquisitionType.clear();
    m_TubeAngle.clear();
    m_ConstantVolumeFlag.clear();
    m_FluoroscopyFlag.clear();
}

OFCondition FGCTAcquisitionType::check() const
{
    // Maybe add checks later
    return EC_Normal;
}

/// Read CT Acquisition Type Sequence from given item
OFCondition FGCTAcquisitionType::read(DcmItem& item)
{
    clearData();

    DcmItem* seqItem   = NULL;
    OFCondition result = getItemFromFGSequence(item, DCM_CTAcquisitionTypeSequence, 0, seqItem);
    if (result.bad())
        return result;

    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_AcquisitionType, "1", "1", "CTAcquisitionTypeMacro");
    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_TubeAngle, "1", "1", "CTAcquisitionTypeMacro");
    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_ConstantVolumeFlag, "1", "1", "CTAcquisitionTypeMacro");
    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_FluoroscopyFlag, "1", "1", "CTAcquisitionTypeMacro");
    return EC_Normal;
}

/// Writes single CT Acquisition Type Sequence into given item
OFCondition FGCTAcquisitionType::write(DcmItem& item)
{
    DcmItem* seqItem   = NULL;
    OFCondition result = createNewFGSequence(item, DCM_CTAcquisitionTypeSequence, 0, seqItem);
    if (result.bad())
        return result;

    // --- write frame content macro attributes ---
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_AcquisitionType, "1", "1", "CTAcquisitionTypeMacro");
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_TubeAngle, "1", "1", "CTAcquisitionTypeMacro");
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_ConstantVolumeFlag, "1", "1", "CTAcquisitionTypeMacro");
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_FluoroscopyFlag, "1", "1", "CTAcquisitionTypeMacro");
    return result;
}

int FGCTAcquisitionType::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result != 0)
        return result;

    const FGCTAcquisitionType* myRhs = OFstatic_cast(const FGCTAcquisitionType*, &rhs);
    if (!myRhs)
        return -1;

    // Compare all elements
    result = m_AcquisitionType.compare(myRhs->m_AcquisitionType);
    if (result == 0)
        result = m_TubeAngle.compare(myRhs->m_TubeAngle);
    if (result == 0)
        result = m_ConstantVolumeFlag.compare(myRhs->m_ConstantVolumeFlag);
    if (result == 0)
        result = m_FluoroscopyFlag.compare(myRhs->m_FluoroscopyFlag);

    return result;
}

// --- get() functionality ---

OFCondition FGCTAcquisitionType::getAcquisitionType(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_AcquisitionType, value, pos);
}

OFCondition FGCTAcquisitionType::getTubeAngle(Float64& value, const unsigned long pos)
{
    return m_TubeAngle.getFloat64(value, pos);
}

OFCondition FGCTAcquisitionType::getConstantVolumeFlag(FGCTAcquisitionType::E_ConstantVolumeFlag& value, const long pos)
{
    OFString str;
    DcmIODUtil::getStringValueFromElement(m_ConstantVolumeFlag, str, pos);
    value = constVolFlag2Enum(str);
    if ((value != FGCTAcquisitionType::E_ConstVol_Empty) && (value != FGCTAcquisitionType::E_ConstVol_Invalid))
        return EC_Normal;
    else
        return EC_InvalidValue;
}

OFCondition FGCTAcquisitionType::getConstantVolumeFlag(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_ConstantVolumeFlag, value, pos);
}

OFCondition FGCTAcquisitionType::getFluoroscopyFlag(FGCTAcquisitionType::E_FluoroscopyFlag& value, const long pos)
{
    OFString str;
    DcmIODUtil::getStringValueFromElement(m_FluoroscopyFlag, str, pos);
    value = fluoroscopyFlag2Enum(str);
    if ((value != FGCTAcquisitionType::E_Fluoroscopy_Empty) && (value != FGCTAcquisitionType::E_Fluoroscopy_Invalid))
        return EC_Normal;
    else
        return EC_InvalidValue;
}

OFCondition FGCTAcquisitionType::getFluoroscopyFlag(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_FluoroscopyFlag, value, pos);
}

// --- set() functionality ---

OFCondition FGCTAcquisitionType::setAcquisitionType(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_AcquisitionType.putOFStringArray(value);
    return result;
}

OFCondition FGCTAcquisitionType::setTubeAngle(const Float64& value, const OFBool /* checkValue */)
{
    return m_TubeAngle.putFloat64(value);
}

OFCondition FGCTAcquisitionType::setConstantVolumeFlag(const FGCTAcquisitionType::E_ConstantVolumeFlag& value,
                                                       const OFBool checkValue)
{
    (void)checkValue;
    OFString val;
    if (constVolFlag2Str(value, val))
    {
        return m_ConstantVolumeFlag.putOFStringArray(val);
    }
    else
        return FG_EC_InvalidData;
}

OFCondition FGCTAcquisitionType::setConstantVolumeFlag(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_ConstantVolumeFlag.putOFStringArray(value);
    return result;
}

OFCondition FGCTAcquisitionType::setFluoroscopyFlag(const FGCTAcquisitionType::E_FluoroscopyFlag& value,
                                                    const OFBool checkValue)
{
    (void)checkValue;
    OFString val;
    if (fluoroscopyFlag2Str(value, val))
    {
        return m_FluoroscopyFlag.putOFStringArray(val);
    }
    else
        return FG_EC_InvalidData;
}

OFCondition FGCTAcquisitionType::setFluoroscopyFlag(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_FluoroscopyFlag.putOFStringArray(value);
    return result;
}

FGCTAcquisitionType::E_ConstantVolumeFlag FGCTAcquisitionType::constVolFlag2Enum(const OFString& str)
{
    if (str == "NO")
        return E_ConstVol_No;
    else if (str == "YES")
        return E_ConstVol_Yes;
    else if (str.empty())
        return E_ConstVol_Empty;
    else
        return E_ConstVol_Invalid;
}

OFBool FGCTAcquisitionType::constVolFlag2Str(const FGCTAcquisitionType::E_ConstantVolumeFlag& eval, OFString& result)
{
    switch (eval)
    {
        case FGCTAcquisitionType::E_ConstVol_No:
            result = "NO";
            break;
        case FGCTAcquisitionType::E_ConstVol_Yes:
            result = "YES";
            break;
        case FGCTAcquisitionType::E_ConstVol_Empty:
            result = "";
            break;
        case FGCTAcquisitionType::E_ConstVol_Invalid:
            result = "";
            return OFFalse;
        default:
            result = "";
            DCMFG_WARN("Unknown value for enum FGCTAcquisitionType::E_ConstantVolumeFlag: " << eval);
            return OFFalse;
    }
    return OFTrue;
}

FGCTAcquisitionType::E_FluoroscopyFlag FGCTAcquisitionType::fluoroscopyFlag2Enum(const OFString& str)
{
    if (str == "NO")
        return E_Fluoroscopy_No;
    else if (str == "YES")
        return E_Fluoroscopy_Yes;
    else if (str.empty())
        return E_Fluoroscopy_Empty;
    else
        return E_Fluoroscopy_Invalid;
}

OFBool FGCTAcquisitionType::fluoroscopyFlag2Str(const FGCTAcquisitionType::E_FluoroscopyFlag& eval, OFString& result)
{
    switch (eval)
    {
        case FGCTAcquisitionType::E_Fluoroscopy_No:
            result = "NO";
            break;
        case FGCTAcquisitionType::E_Fluoroscopy_Yes:
            result = "YES";
            break;
        case FGCTAcquisitionType::E_Fluoroscopy_Empty:
            result = "";
            break;
        case FGCTAcquisitionType::E_Fluoroscopy_Invalid:
            result = "";
            return OFFalse;
        default:
            result = "";
            DCMFG_WARN("Unknown value for enum FGCTAcquisitionType::E_FluoroscopyFlag: " << eval);
            return OFFalse;
    }
    return OFTrue;
}
