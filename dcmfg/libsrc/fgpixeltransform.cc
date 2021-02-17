/*
 *
 *  Copyright (C) 2016-2019, Open Connections GmbH
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
 *  Module: dcmfg
 *
 *  Author: Jan Schlamelcher
 *
 *  Purpose: Class for managing the (Identity) Pixel Value Transformation FG
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmfg/fgpixeltransform.h"
#include "dcmtk/dcmiod/iodutil.h"

FGPixelValueTransformation::FGPixelValueTransformation()
    : FGBase(DcmFGTypes::EFG_PIXELVALUETRANSMETA)
    , m_RescaleIntercept(DCM_RescaleIntercept)
    , m_RescaleSlope(DCM_RescaleSlope)
    , m_RescaleType(DCM_RescaleType)
    , m_fgSubType(E_PixelValTrans_Normal)
{
    m_RescaleIntercept.putOFStringArray("0");
    m_RescaleSlope.putOFStringArray("1");
    m_RescaleType.putOFStringArray("US");
}

void FGPixelValueTransformation::setUseAsIdentityPixelValueTransformation()
{
    m_fgSubType = E_PixelValTrans_Identity;
}

void FGPixelValueTransformation::setFGType(const FGPixelValueTransformation::E_PixelValueTransformationSubType fgType)
{
    m_fgSubType = fgType;
}

FGPixelValueTransformation::E_PixelValueTransformationSubType FGPixelValueTransformation::getFGType() const
{
    return m_fgSubType;
}

FGPixelValueTransformation::~FGPixelValueTransformation()
{
}

OFCondition FGPixelValueTransformation::check() const
{
    OFCondition result = EC_Normal;
    Float64 rs, ri;
    rs = ri = 0;
    OFString rt;
    switch (m_fgSubType)
    {
        case E_PixelValTrans_Identity:
            if (OFconst_cast(DcmDecimalString*, &m_RescaleIntercept)->getFloat64(ri).good())
            {
                if (ri != 0)
                {
                    DCMFG_ERROR("Rescale Intercept in Identity Pixel Value Transformation FG must be 0 but is set to "
                                << ri);
                    result = FG_EC_InvalidData;
                }
            }
            else
            {
                DCMFG_ERROR(
                    "Invalid or no value for Rescale Intercept in Identity Pixel Value Transformation FG (0 is the "
                    "only valid value");
                result = FG_EC_InvalidData;
            }

            if (OFconst_cast(DcmDecimalString*, &m_RescaleSlope)->getFloat64(rs).good())
            {
                if (rs != 1)
                {
                    DCMFG_ERROR("Rescale Slope in Identity Pixel Value Transformation FG must be 1 but is set to "
                                << rs);
                    result = FG_EC_InvalidData;
                    ;
                }
            }
            else
            {
                DCMFG_ERROR("Invalid or no value for Rescale Slope in Identity Pixel Value Transformation FG (1 is the "
                            "only valid value");
                result = FG_EC_InvalidData;
                ;
            }

            if (OFconst_cast(DcmLongString*, &m_RescaleType)->getOFStringArray(rt).good())
            {
                if (rt != "US")
                {
                    DCMFG_ERROR("Rescale Type in Identity Pixel Value Transformation FG must be \"US\" but is set to "
                                << rt);
                    result = FG_EC_InvalidData;
                    ;
                }
            }
            else
            {
                DCMFG_ERROR(
                    "Invalid or no value for Rescale Type in Identity Pixel Value Transformation FG (\"US\" is the "
                    "only valid value");
                result = FG_EC_InvalidData;
                ;
            }
            break;
        case E_PixelValTrans_CT:
        case E_PixelValTrans_Normal:
        default:
            // No checks possible
            break;
    }
    return result;
}

void FGPixelValueTransformation::clearData()
{
    m_RescaleIntercept.clear();
    m_RescaleSlope.clear();
    m_RescaleType.clear();
}

FGBase* FGPixelValueTransformation::clone() const
{
    if (FGPixelValueTransformation* copy = new FGPixelValueTransformation)
    {
        copy->m_RescaleIntercept = m_RescaleIntercept;
        copy->m_RescaleSlope     = m_RescaleSlope;
        copy->m_RescaleType      = m_RescaleType;
        return copy;
    }
    return OFnullptr;
}

OFCondition FGPixelValueTransformation::read(DcmItem& item)
{
    clearData();

    DcmItem* seqItem;
    OFCondition result;

    seqItem = OFnullptr;
    result  = getItemFromFGSequence(item, DCM_PixelValueTransformationSequence, 0, seqItem);
    if (result.bad())
        return result;
    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_RescaleIntercept, "1", "1", fgType2Str().c_str());
    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_RescaleSlope, "1", "1", fgType2Str().c_str());
    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_RescaleType, "1", "1", fgType2Str().c_str());

    // Try to guess type of Pixel Value Transformation FG by checking for SOP Class
    OFString sopClass;
    if (item.getRootItem()->findAndGetOFString(DCM_SOPClassUID, sopClass).good() && !sopClass.empty())
    {
        // Identity Pixel Value Transformation FG
        if ((sopClass == UID_BreastTomosynthesisImageStorage)
            || (sopClass == UID_BreastProjectionXRayImageStorageForPresentation)
            || (sopClass == UID_BreastProjectionXRayImageStorageForProcessing)
            || (sopClass == UID_ParametricMapStorage))

        {
            DCMFG_DEBUG("FGPixelValueTransformation: Using SOP Class to set Pixel Value Transformation FG to type: "
                        << fgType2Str());
            m_fgSubType = E_PixelValTrans_Identity;
        }

        // CT Pixel Value Transformation FG
        else if ((sopClass == UID_EnhancedCTImageStorage) || (sopClass == UID_LegacyConvertedEnhancedCTImageStorage))
        {
            m_fgSubType = E_PixelValTrans_CT;
            DCMFG_DEBUG("FGPixelValueTransformation: Using SOP Class to set Pixel Value Transformation FG to type: "
                        << fgType2Str());
        }
        else
        {
            m_fgSubType = E_PixelValTrans_Normal;
            DCMFG_DEBUG(
                "FGPixelValueTransformation: Setting Pixel Value Transformation FG is of type: " << fgType2Str());
        }
    }

    return EC_Normal;
}

OFCondition FGPixelValueTransformation::write(DcmItem& item)
{
    DcmItem* seqItem;
    if (m_fgSubType == E_PixelValTrans_Identity)
    {
        DCMFG_DEBUG(fgType2Str() << ": Fixing values for Rescale Slope, Intercept and Type to "
                                    "enumerated values '1', '0' and 'US'");
        m_RescaleSlope.putOFStringArray("1");
        m_RescaleIntercept.putOFStringArray("0");
        m_RescaleType.putOFStringArray("US");
    }

    seqItem            = OFnullptr;
    OFCondition result = createNewFGSequence(item, DCM_PixelValueTransformationSequence, 0, seqItem);
    if (result.bad())
        return result;
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_RescaleIntercept, "1", "1", fgType2Str().c_str());
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_RescaleSlope, "1", "1", fgType2Str().c_str());
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_RescaleType, "1", "1", fgType2Str().c_str());

    return result;
}

int FGPixelValueTransformation::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result == 0)
    {
        const FGPixelValueTransformation* myRhs = OFstatic_cast(const FGPixelValueTransformation*, &rhs);
        if (!myRhs)
            return -1;

        // Compare all elements
        result = m_RescaleIntercept.compare(myRhs->m_RescaleIntercept);
        if (result == 0)
            result = m_RescaleSlope.compare(myRhs->m_RescaleSlope);
        if (result == 0)
            result = m_RescaleType.compare(myRhs->m_RescaleType);
    }

    return result;
}

OFCondition FGPixelValueTransformation::getRescaleIntercept(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromElement(m_RescaleIntercept, value, pos);
}

OFCondition FGPixelValueTransformation::getRescaleSlope(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromElement(m_RescaleSlope, value, pos);
}

OFCondition FGPixelValueTransformation::getRescaleType(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromElement(m_RescaleType, value, pos);
}

OFCondition FGPixelValueTransformation::setRescaleIntercept(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_RescaleIntercept.putString(value.c_str());
    return result;
}

OFCondition FGPixelValueTransformation::setRescaleSlope(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_RescaleSlope.putString(value.c_str());
    return result;
}

OFCondition FGPixelValueTransformation::setRescaleType(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_RescaleType.putString(value.c_str());
    return result;
}

OFString FGPixelValueTransformation::fgType2Str()
{
    OFString result;
    switch (m_fgSubType)
    {
        case E_PixelValTrans_Normal:
            result = "Pixel Value Transformation";
            break;
        case E_PixelValTrans_Identity:
            result = "Identity Pixel Value Transformation";
            break;
        case E_PixelValTrans_CT:
            result = "CT Pixel Value Transformation";
            break;
        default:
            DCMFG_WARN("Internal error: Unknown value for enum E_PixelValueTransformationType in fgType2Str(): "
                       << m_fgSubType);
            result = "";
    }
    return result;
}
