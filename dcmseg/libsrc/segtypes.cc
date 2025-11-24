/*
 *
 *  Copyright (C) 2015-2025, Open Connections GmbH
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
 *  Module:  dcmseg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for managing common segmentation specific types.
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmseg/segtypes.h"

OFLogger DCM_dcmsegLogger = OFLog::getLogger("dcmtk.dcmseg");

/*---------------------------------*
 *  constant definitions
 *---------------------------------*/

// conditions
makeOFConditionConst(SG_EC_MaxSegmentsReached, OFM_dcmseg, 1, OF_error, "Maximum Number of Functional Groups reached");
makeOFConditionConst(SG_EC_NoSuchSegment, OFM_dcmseg, 2, OF_error, "No such segment");
makeOFConditionConst(SG_EC_UnknownSegmentationType, OFM_dcmseg, 3, OF_error, "Unknown Segmentation Type");
makeOFConditionConst(SG_EC_InvalidValue, OFM_dcmseg, 4, OF_error, "Invalid value for Segmentation SOP Class");
makeOFConditionConst(SG_EC_NotEnoughData, OFM_dcmseg, 5, OF_error, "Not enough data");
makeOFConditionConst(SG_EC_MaxFramesReached, OFM_dcmseg, 6, OF_error, "Maximum Number of Frames reached");
makeOFConditionConst(SG_EC_InvalidBitDepth, OFM_dcmseg, 7, OF_error, "Invalid bit depth for given Segmentation Type");
makeOFConditionConst(SG_EC_FramesNotParallel, OFM_dcmseg, 8, OF_error, "Frames are not parallel");
makeOFConditionConst(SG_EC_NoSegmentationBasedSOPClass, OFM_dcmseg, 9, OF_error, "No segmentation SOP class (Segmentation or Label Map Segmentation SOP Class)");
makeOFConditionConst(SG_EC_NoConversionRequired, OFM_dcmseg, 10, OF_ok, "Segmentation-based object does not require requested conversion");
makeOFConditionConst(SG_EC_AlreadyLabelMap, OFM_dcmseg, 11, OF_error, "Segmentation-based object is already a label map");
makeOFConditionConst(SG_EC_CannotConvertFractionalToLabelmap, OFM_dcmseg, 12, OF_error, "Cannot convert fractional to label map segmentations");
makeOFConditionConst(SG_EC_OverlappingSegments, OFM_dcmseg, 13, OF_error, "Binary segmentation contains overlapping segments");
makeOFConditionConst(SG_EC_CannotConvertMissingCIELab, OFM_dcmseg, 14, OF_error, "Cannot convert to PALETTE color model since not all segments contain Recommended Display CIELab Value Macro");
makeOFConditionConst(SG_EC_MissingPlanePositionPatient, OFM_dcmseg, 15, OF_error, "Missing Plane Position (Patient) Functional Group");

DcmSegTypes::E_SegmentationType DcmSegTypes::OFString2Segtype(const OFString& value)
{
    if (value == "BINARY")
        return DcmSegTypes::ST_BINARY;
    if (value == "FRACTIONAL")
        return DcmSegTypes::ST_FRACTIONAL;
    if (value == "LABELMAP")
        return DcmSegTypes::ST_LABELMAP;
    else
        return DcmSegTypes::ST_UNKNOWN;
}

OFString DcmSegTypes::segtype2OFString(const DcmSegTypes::E_SegmentationType& value)
{
    switch (value)
    {
        case DcmSegTypes::ST_BINARY:
            return "BINARY";
        case DcmSegTypes::ST_FRACTIONAL:
            return "FRACTIONAL";
        case DcmSegTypes::ST_LABELMAP:
            return "LABELMAP";
        case DcmSegTypes::ST_UNKNOWN:
            return "UNKNOWN";
        default:
            return "Invalid segmentation type (internal error)";
    }
}

DcmSegTypes::E_SegmentationFractionalType DcmSegTypes::OFString2FractionalType(const OFString& value)
{
    if (value == "PROBABILITY")
        return DcmSegTypes::SFT_PROBABILITY;
    if (value == "OCCUPANCY")
        return DcmSegTypes::SFT_OCCUPANCY;
    else
        return DcmSegTypes::SFT_UNKNOWN;
}

OFString DcmSegTypes::algoType2OFString(DcmSegTypes::E_SegmentAlgoType algo)
{
    switch (algo)
    {
        case SAT_AUTOMATIC:
            return "AUTOMATIC";
            break;
        case SAT_SEMIAUTOMATIC:
            return "SEMIAUTOMATIC";
            break;
        case SAT_MANUAL:
            return "MANUAL";
            break;
        default:
            return "";
    }
}

DcmSegTypes::E_SegmentAlgoType DcmSegTypes::OFString2AlgoType(const OFString& algoType)
{
    if (algoType == "AUTOMATIC")
        return DcmSegTypes::SAT_AUTOMATIC;
    if (algoType == "MANUAL")
        return DcmSegTypes::SAT_MANUAL;
    if (algoType == "SEMIAUTOMATIC")
        return DcmSegTypes::SAT_SEMIAUTOMATIC;
    else
        return DcmSegTypes::SAT_UNKNOWN;
}


OFString DcmSegTypes::labelmapColorModel2OFString(const DcmSegTypes::E_SegmentationLabelmapColorModel value, const OFString& fallbackValue)
{
    OFString result;
    switch (value)
    {
        case DcmSegTypes::SLCM_MONOCHROME2:
            return "MONOCHROME2";
        case DcmSegTypes::SLCM_PALETTE:
            return "PALETTE COLOR";
        case DcmSegTypes::SLCM_UNKNOWN:
            result = "UNKNOWN";
        default:
            result = "Invalid labelmap color model (internal error)";
    }
    if (!fallbackValue.empty())
    {
        DCMSEG_WARN("Invalid value for label map color model: " << result << ". Using fallback value: " << fallbackValue);
        result = fallbackValue;
    }
    return result;
}


DcmSegTypes::E_SegmentationLabelmapColorModel DcmSegTypes::OFString2LabelmapColorModel(const OFString& value)
{
    if (value == "MONOCHROME2")
        return DcmSegTypes::SLCM_MONOCHROME2;
    if (value == "PALETTE COLOR")
        return DcmSegTypes::SLCM_PALETTE;
    else
        return DcmSegTypes::SLCM_UNKNOWN;
}

SegmentDescriptionMacro::SegmentDescriptionMacro()
    : m_SegmentLabel(DCM_SegmentLabel)
    , m_SegmentDescription(DCM_SegmentDescription)
    , m_SegmentAlgorithmType(DcmSegTypes::SAT_UNKNOWN)
    , m_GeneralAnatomyCode("3" /* General Anatomy Optional Macro* */)
    , m_SegmentedPropertyCategoryCode()
    , m_SegmentedPropertyType()
{
}

SegmentDescriptionMacro::~SegmentDescriptionMacro()
{
}

SegmentDescriptionMacro& SegmentDescriptionMacro::operator=(const SegmentDescriptionMacro& rhs)
{
    if (this != &rhs)
    {
        m_SegmentLabel = rhs.m_SegmentLabel;
        m_SegmentDescription = rhs.m_SegmentDescription;
        m_SegmentAlgorithmType = rhs.m_SegmentAlgorithmType;
        m_GeneralAnatomyCode = rhs.m_GeneralAnatomyCode;
        m_SegmentedPropertyCategoryCode = rhs.m_SegmentedPropertyCategoryCode;
        m_SegmentedPropertyType = rhs.m_SegmentedPropertyType;
    }
    return *this;
}


SegmentDescriptionMacro* SegmentDescriptionMacro::clone()
{
    return new SegmentDescriptionMacro(*this);
}


void SegmentDescriptionMacro::clearData()
{
    m_SegmentLabel.clear();
    m_SegmentDescription.clear();
    m_SegmentAlgorithmType = DcmSegTypes::SAT_UNKNOWN;
    m_GeneralAnatomyCode.clearData();
    m_SegmentedPropertyCategoryCode.clearData();
    m_SegmentedPropertyType.clearData();
}

OFCondition SegmentDescriptionMacro::read(DcmItem& item)
{
    /* re-initialize object */
    clearData();

    DcmIODUtil::getAndCheckElementFromDataset(item, m_SegmentLabel, "1", "1", "SegmentDescriptionMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_SegmentDescription, "1", "3", "SegmentDescriptionMacro");
    readSegmentAlgorithmType(item);
    m_GeneralAnatomyCode.read(item);
    DcmIODUtil::readSingleItem<CodeSequenceMacro>(item,
                                                  DCM_SegmentedPropertyCategoryCodeSequence,
                                                  m_SegmentedPropertyCategoryCode,
                                                  "1",
                                                  "SegmentDescriptionMacro");
    DcmIODUtil::readSingleItem<SegmentedPropertyTypeCodeItem>(
        item, DCM_SegmentedPropertyTypeCodeSequence, m_SegmentedPropertyType, "1", "SegmentDescriptionMacro");

    return EC_Normal;
}

OFCondition SegmentDescriptionMacro::write(DcmItem& item)
{
    OFCondition result = EC_Normal;

    /* copy all elements to dataset */
    DcmIODUtil::copyElementToDataset(
        result, item, m_SegmentLabel, "1" /* VM */, "1" /* Type */, "SegmentDescriptionMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_SegmentDescription, "1", "3", "SegmentDescriptionMacro");
    if (result.good())
        result = writeSegmentAlgorithmType(item);
    if (result.good())
    {
        if (m_GeneralAnatomyCode.check(OFTrue /* quiet */).good())
        {
            result = m_GeneralAnatomyCode.write(item);
        }
    }
    DcmIODUtil::writeSingleItem<CodeSequenceMacro>(result,
                                                   DCM_SegmentedPropertyCategoryCodeSequence,
                                                   m_SegmentedPropertyCategoryCode,
                                                   item,
                                                   "1",
                                                   "SegmentDescriptionMacro");
    DcmIODUtil::writeSingleItem<SegmentedPropertyTypeCodeItem>(
        result, DCM_SegmentedPropertyTypeCodeSequence, m_SegmentedPropertyType, item, "1", "SegmentDescriptionMacro");
    return result;
}

OFCondition SegmentDescriptionMacro::getSegmentLabel(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_SegmentLabel, value, pos);
}

OFCondition SegmentDescriptionMacro::getSegmentDescription(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_SegmentDescription, value, pos);
}

DcmSegTypes::E_SegmentAlgoType SegmentDescriptionMacro::getSegmentAlgorithmType()
{
    return m_SegmentAlgorithmType;
}

GeneralAnatomyMacro& SegmentDescriptionMacro::getGeneralAnatomyCode()
{
    return m_GeneralAnatomyCode;
}

CodeSequenceMacro& SegmentDescriptionMacro::getSegmentedPropertyCategoryCode()
{
    return m_SegmentedPropertyCategoryCode;
}

CodeSequenceMacro& SegmentDescriptionMacro::getSegmentedPropertyTypeCode()
{
    return m_SegmentedPropertyType.m_SegmentedPropertyTypeCode;
}

OFVector<CodeSequenceMacro*>& SegmentDescriptionMacro::getSegmentedPropertyTypeModifier()
{
    return m_SegmentedPropertyType.m_SegmentedPropertyTypeModifierCode;
}

OFCondition SegmentDescriptionMacro::setSegmentLabel(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_SegmentLabel.putOFStringArray(value);
    return result;
}

OFCondition SegmentDescriptionMacro::setSegmentDescription(const OFString& value, const OFBool checkValue)
{
    (void)checkValue;
    return m_SegmentDescription.putOFStringArray(value);
}

OFCondition SegmentDescriptionMacro::setSegmentAlgorithmType(const DcmSegTypes::E_SegmentAlgoType value)
{
    m_SegmentAlgorithmType = value;
    return EC_Normal;
}

OFCondition SegmentDescriptionMacro::readSegmentAlgorithmType(DcmItem& item)
{
    DcmCodeString element(DCM_SegmentAlgorithmType);
    OFCondition result = DcmIODUtil::getAndCheckElementFromDataset(item, element, "1", "1", "SegmentDescriptionMacro");
    OFString str;
    if (result.good())
    {
        element.getOFStringArray(str);
        m_SegmentAlgorithmType = DcmSegTypes::OFString2AlgoType(str);
    }

    if (m_SegmentAlgorithmType == DcmSegTypes::SAT_UNKNOWN)
    {
        DCMSEG_ERROR("Invalid value for attribute Segmentation Algorithm Type: " << str);
        return SG_EC_InvalidValue;
    }
    else
        return EC_Normal;
}

OFCondition SegmentDescriptionMacro::writeSegmentAlgorithmType(DcmItem& item)
{
    if (m_SegmentAlgorithmType == DcmSegTypes::SAT_UNKNOWN)
    {
        DCMSEG_ERROR("Segment Algorithm Type not set");
        return SG_EC_InvalidValue;
    }

    OFString str = DcmSegTypes::algoType2OFString(m_SegmentAlgorithmType);
    return item.putAndInsertOFStringArray(DCM_SegmentAlgorithmType, str);
}

SegmentedPropertyTypeCodeItem::SegmentedPropertyTypeCodeItem()
    : m_SegmentedPropertyTypeCode()
    , m_SegmentedPropertyTypeModifierCode()
{
}

SegmentedPropertyTypeCodeItem& SegmentedPropertyTypeCodeItem::operator=(const SegmentedPropertyTypeCodeItem& rhs)
{
    if (this != &rhs)
    {
        m_SegmentedPropertyTypeCode = rhs.m_SegmentedPropertyTypeCode;
        DcmIODUtil::copyContainer(rhs.m_SegmentedPropertyTypeModifierCode, m_SegmentedPropertyTypeModifierCode);
    }
    return *this;
}

SegmentedPropertyTypeCodeItem* SegmentedPropertyTypeCodeItem::clone()
{
    return new SegmentedPropertyTypeCodeItem(*this);
}

SegmentedPropertyTypeCodeItem::SegmentedPropertyTypeCodeItem(const SegmentedPropertyTypeCodeItem& rhs)
    : m_SegmentedPropertyTypeCode(rhs.m_SegmentedPropertyTypeCode)
    , m_SegmentedPropertyTypeModifierCode()
{
    DcmIODUtil::copyContainer(rhs.m_SegmentedPropertyTypeModifierCode, m_SegmentedPropertyTypeModifierCode);
}

SegmentedPropertyTypeCodeItem::~SegmentedPropertyTypeCodeItem()
{
    DcmIODUtil::freeContainer(m_SegmentedPropertyTypeModifierCode);
}

OFCondition SegmentedPropertyTypeCodeItem::check(const OFBool quiet)
{
    OFCondition result = m_SegmentedPropertyTypeCode.check(quiet);
    if (result.good())
    {
        result = checkModifiers(quiet);
    }
    return result;
}

OFCondition SegmentedPropertyTypeCodeItem::checkModifiers(const OFBool quiet)
{
    OFCondition result;
    OFVector<CodeSequenceMacro*>::iterator it = m_SegmentedPropertyTypeModifierCode.begin();
    while (it != m_SegmentedPropertyTypeModifierCode.end())
    {
        result = (*it)->check(quiet);
        if (result.bad())
            return result;
        it++;
    }
    return EC_Normal;
}

void SegmentedPropertyTypeCodeItem::clearData()
{
    DcmIODUtil::freeContainer(m_SegmentedPropertyTypeModifierCode);
    m_SegmentedPropertyTypeCode.clearData();
}

OFCondition SegmentedPropertyTypeCodeItem::read(DcmItem& item, const OFBool clearOldData)
{
    OFCondition result;
    if (clearOldData)
        clearData();

    m_SegmentedPropertyTypeCode.read(item);
    DcmIODUtil::readSubSequence(item,
                                DCM_SegmentedPropertyTypeModifierCodeSequence,
                                m_SegmentedPropertyTypeModifierCode,
                                "1-n",
                                "3",
                                "SegmentDescriptionMacro");
    return result;
}

OFCondition SegmentedPropertyTypeCodeItem::write(DcmItem& item)
{
    OFCondition result = m_SegmentedPropertyTypeCode.write(item);
    if (result.good())
    {
        result = checkModifiers();
        if (result.good())
        {
            DcmIODUtil::writeSubSequence<OFVector<CodeSequenceMacro*> >(result,
                                                                       DCM_SegmentedPropertyTypeModifierCodeSequence,
                                                                       m_SegmentedPropertyTypeModifierCode,
                                                                       item,
                                                                       "1-n",
                                                                       "3",
                                                                       "SegmentDescriptionMacro");
        }
    }
    return result;
}
