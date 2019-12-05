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
 *  Purpose: Class for managing the CT Acquisition Details Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmfg/fgctacquisitiondetails.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/dcmiod/iodutil.h"

// Constructor
FGCTAcquisitionDetails::FGCTAcquisitionDetails()
    : FGBase(DcmFGTypes::EFG_CTACQUISITIONDETAILS)
    , m_Items()
{
}

FGCTAcquisitionDetails::~FGCTAcquisitionDetails()
{
    clearData();
}

FGBase* FGCTAcquisitionDetails::clone() const
{
    OFunique_ptr<FGCTAcquisitionDetails> copy(new FGCTAcquisitionDetails());
    if (copy)
    {
        if (DcmIODUtil::copyContainer(this->m_Items, copy->m_Items).good())
        {
            return copy.release();
        }
    }
    return OFnullptr;
}

void FGCTAcquisitionDetails::clearData()
{
    DcmIODUtil::freeContainer(m_Items);
    m_Items.clear();
}

OFCondition FGCTAcquisitionDetails::check() const
{
    // Maybe add checks later
    return EC_Normal;
}

/// Read CT Acquisition Details Sequence from given item
OFCondition FGCTAcquisitionDetails::read(DcmItem& item)
{
    clearData();
    DcmItem* seqItem       = NULL;
    unsigned long numItems = 0;
    OFCondition result     = getNumItemsFromFGSequence(item, DCM_CTAcquisitionDetailsSequence, numItems);
    if (result.bad())
        return result;

    for (unsigned long num = 0; num < numItems; num++)
    {
        if (getItemFromFGSequence(item, DCM_CTAcquisitionDetailsSequence, num, seqItem).good())
        {
            FGCTAcquisitionDetailsItem* newItem = new FGCTAcquisitionDetailsItem();
            newItem->read(*seqItem);
            m_Items.push_back(newItem);
        }
    }
    return EC_Normal;
}

/// Write CT Acquisition Detail Sequence into given item
OFCondition FGCTAcquisitionDetails::write(DcmItem& item)
{
    OFCondition result = check();
    if (result.good())
    {
        DcmIODUtil::writeSubSequence(
            result, DCM_CTAcquisitionDetailsSequence, m_Items, item, "1-n", "2", "CTAcquisitionDetailsMacro");
    }

    return result;
}

int FGCTAcquisitionDetails::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result != 0)
        return result;

    FGCTAcquisitionDetails* myRhs
        = OFconst_cast(FGCTAcquisitionDetails*, (OFstatic_cast(const FGCTAcquisitionDetails*, &rhs)));
    if (!myRhs)
        return -1;

    // Compare all items
    return DcmIODUtil::compareContainer(this->m_Items, myRhs->m_Items);
}

OFVector<FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem*>& FGCTAcquisitionDetails::getCTAcquisitionDetailsItems()
{
    return m_Items;
}

// ------------ Class FGCTAcquisitionDetailsItem::FGCTAcquisitionDetailsItem -------------

FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::FGCTAcquisitionDetailsItem()
    : m_ReferencedPathIndex(DCM_ReferencedPathIndex)
    , m_RotationDirection(DCM_RotationDirection)
    , m_RevolutionTime(DCM_RevolutionTime)
    , m_SingleCollimationWidth(DCM_SingleCollimationWidth)
    , m_TotalCollimationWidth(DCM_TotalCollimationWidth)
    , m_TableHeight(DCM_TableHeight)
    , m_GantryDetectorTilt(DCM_GantryDetectorTilt)
    , m_DataCollectionDiameter(DCM_DataCollectionDiameter)
{
}

FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::~FGCTAcquisitionDetailsItem()
{
    // Nothing to do
}

// --- get() functionality ---

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::check() const
{
    // Maybe add checks later
    return EC_Normal;
}

void FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::clearData()
{
    m_DataCollectionDiameter.clear();
    m_GantryDetectorTilt.clear();
    m_ReferencedPathIndex.clear();
    m_RevolutionTime.clear();
    m_RotationDirection.clear();
    m_SingleCollimationWidth.clear();
    m_TableHeight.clear();
    m_TotalCollimationWidth.clear();
}

int FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::compare(
    const FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem& rhs) const
{
    FGCTAcquisitionDetailsItem* myRhs = OFconst_cast(FGCTAcquisitionDetailsItem*, &rhs);
    if (!myRhs)
        return -1;

    // Compare all elements
    int result = m_DataCollectionDiameter.compare(myRhs->m_DataCollectionDiameter);
    if (result == 0)
        result = m_GantryDetectorTilt.compare(myRhs->m_GantryDetectorTilt);
    if (result == 0)
        result = m_ReferencedPathIndex.compare(myRhs->m_ReferencedPathIndex);
    if (result == 0)
        result = m_RevolutionTime.compare(myRhs->m_RevolutionTime);
    if (result == 0)
        result = m_RotationDirection.compare(myRhs->m_RotationDirection);
    if (result == 0)
        result = m_SingleCollimationWidth.compare(myRhs->m_SingleCollimationWidth);
    if (result == 0)
        result = m_TableHeight.compare(myRhs->m_TableHeight);
    if (result == 0)
        result = m_TotalCollimationWidth.compare(myRhs->m_TotalCollimationWidth);
    return result;
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::getDataCollectionDiameter(Float64& value,
                                                                                          const unsigned long pos)
{
    return m_DataCollectionDiameter.getFloat64(value, pos);
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::getGantryDetectorTilt(Float64& value,
                                                                                      const unsigned long pos)
{
    return m_GantryDetectorTilt.getFloat64(value, pos);
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::getReferencedPathIndex(OFVector<Uint16>& values)
{
    return DcmIODUtil::getUint16ValuesFromElement(m_ReferencedPathIndex, values);
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::getReferencedPathIndex(Uint16& value,
                                                                                       const unsigned long pos)
{
    return m_ReferencedPathIndex.getUint16(value, pos);
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::getRevolutionTime(Float64& value,
                                                                                  const unsigned long pos)
{
    return m_RevolutionTime.getFloat64(value, pos);
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::getRotationDirection(
    FGCTAcquisitionDetails::E_RotationDirection& value, const long pos)
{
    OFString str;
    DcmIODUtil::getStringValueFromElement(m_RotationDirection, str, pos);
    value = rotaDir2Enum(str);
    if ((value != FGCTAcquisitionDetails::E_RotationDirection_Empty)
        && (value != FGCTAcquisitionDetails::E_RotationDirection_Invalid))
        return EC_Normal;
    else
        return EC_InvalidValue;
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::getRotationDirection(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_RotationDirection, value, pos);
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::getSingleCollimationWidth(Float64& value,
                                                                                          const unsigned long pos)
{
    return m_SingleCollimationWidth.getFloat64(value, pos);
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::getTableHeight(Float64& value, const unsigned long pos)
{
    return m_TableHeight.getFloat64(value, pos);
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::getTotalCollimationWidth(Float64& value,
                                                                                         const unsigned long pos)
{
    return m_TotalCollimationWidth.getFloat64(value, pos);
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::read(DcmItem& item)
{
    clearData();

    DcmIODUtil::getAndCheckElementFromDataset(item, m_ReferencedPathIndex, "1-n", "1C", "CTAcquisitionDetailsMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_RotationDirection, "1", "1C", "CTAcquisitionDetailsMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_RevolutionTime, "1", "1C", "CTAcquisitionDetailsMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_SingleCollimationWidth, "1", "1C", "CTAcquisitionDetailsMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_TotalCollimationWidth, "1", "1C", "CTAcquisitionDetailsMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_TableHeight, "1", "1C", "CTAcquisitionDetailsMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_GantryDetectorTilt, "1", "1C", "CTAcquisitionDetailsMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_DataCollectionDiameter, "1", "1C", "CTAcquisitionDetailsMacro");

    return EC_Normal;
}

FGCTAcquisitionDetails::E_RotationDirection
FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::rotaDir2Enum(const OFString& str)
{
    if (str == "CC")
        return E_RotationDirection_CC;
    else if (str == "CW")
        return E_RotationDirection_CW;
    else if (str.empty())
        return E_RotationDirection_Empty;
    else
        return E_RotationDirection_Invalid;
}

OFBool
FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::rotaDir2Str(const FGCTAcquisitionDetails::E_RotationDirection& rota,
                                                                OFString& result)
{
    switch (rota)
    {
        case FGCTAcquisitionDetails::E_RotationDirection_CC:
            result = "CC";
            break;
        case FGCTAcquisitionDetails::E_RotationDirection_CW:
            result = "CW";
            break;
        case FGCTAcquisitionDetails::E_RotationDirection_Empty:
            result = "";
            break;
        case FGCTAcquisitionDetails::E_RotationDirection_Invalid:
            result = "";
            return OFFalse;
        default:
            result = "";
            DCMFG_WARN("Unknown value for enum FGCTAcquisitionDetails::E_RotationDirection: " << rota);
            return OFFalse;
    }
    return OFTrue;
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::setDataCollectionDiameter(const Float64 value,
                                                                                          const OFBool checkValue)
{
    (void)checkValue;
    return m_DataCollectionDiameter.putFloat64(value);
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::setGantryDetectorTilt(const Float64 value,
                                                                                      const OFBool checkValue)
{
    (void)checkValue;
    return m_GantryDetectorTilt.putFloat64(value);
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::setReferencedPathIndex(const OFVector<Uint16>& values,
                                                                                       const OFBool checkValues)
{
    (void)checkValues;
    return DcmIODUtil::setUint16ValuesOnElement(m_ReferencedPathIndex, values, "1-n", checkValues);
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::setRotationDirection(
    const FGCTAcquisitionDetails::E_RotationDirection& value, const OFBool checkValue)
{
    (void)checkValue;
    OFString val;
    if (rotaDir2Str(value, val))
    {
        return m_RotationDirection.putOFStringArray(val);
    }
    else
        return FG_EC_InvalidData;
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::setRevolutionTime(const Float64 value,
                                                                                  const bool checkValue)
{
    (void)checkValue;
    return m_RevolutionTime.putFloat64(value);
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::setSingleCollimationWidth(const Float64 value,
                                                                                          const OFBool checkValue)
{
    (void)checkValue;
    return m_SingleCollimationWidth.putFloat64(value);
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::setTableHeight(const Float64 value,
                                                                               const OFBool checkValue)
{
    (void)checkValue;
    return m_TableHeight.putFloat64(value);
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::setTotalCollimationWidth(const Float64 value,
                                                                                         const OFBool checkValue)
{
    (void)checkValue;
    return m_TotalCollimationWidth.putFloat64(value);
}

OFCondition FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::write(DcmItem& item)
{
    // --- write frame content macro attributes ---
    OFCondition result;
    DcmIODUtil::copyElementToDataset(result, item, m_ReferencedPathIndex, "1-n", "1C", "CTAcquisitionDetailsMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_RotationDirection, "1", "1C", "CTAcquisitionDetailsMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_RevolutionTime, "1", "1C", "CTAcquisitionDetailsMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_SingleCollimationWidth, "1", "1C", "CTAcquisitionDetailsMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_TotalCollimationWidth, "1", "1C", "CTAcquisitionDetailsMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_TableHeight, "1", "1C", "CTAcquisitionDetailsMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_GantryDetectorTilt, "1", "1C", "CTAcquisitionDetailsMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_DataCollectionDiameter, "1", "1C", "CTAcquisitionDetailsMacro");

    return result;
}

FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem* FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem::clone() const
{
    FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem* copy = new FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem();
    if (copy)
    {
        copy->m_ReferencedPathIndex    = this->m_ReferencedPathIndex;
        copy->m_RotationDirection      = this->m_RotationDirection;
        copy->m_RevolutionTime         = this->m_RevolutionTime;
        copy->m_SingleCollimationWidth = this->m_SingleCollimationWidth;
        copy->m_TotalCollimationWidth  = this->m_TotalCollimationWidth;
        copy->m_TableHeight            = this->m_TableHeight;
        copy->m_GantryDetectorTilt     = this->m_GantryDetectorTilt;
        copy->m_DataCollectionDiameter = this->m_DataCollectionDiameter;
    }
    return copy;
}
