/*
 *
 *  Copyright (C) 2015-2022, Open Connections GmbH
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

DcmSegTypes::E_SegmentationType DcmSegTypes::OFString2Segtype(const OFString& value)
{
    if (value == "BINARY")
        return DcmSegTypes::ST_BINARY;
    if (value == "FRACTIONAL")
        return DcmSegTypes::ST_FRACTIONAL;
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
