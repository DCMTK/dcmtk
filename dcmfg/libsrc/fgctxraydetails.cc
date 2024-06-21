/*
 *
 *  Copyright (C) 2019-2024, Open Connections GmbH
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
 *  Purpose: Class for managing the CT X-Ray Details Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmfg/fgctxraydetails.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/dcmiod/iodutil.h"

// Constructor
FGCTXRayDetails::FGCTXRayDetails()
    : FGBase(DcmFGTypes::EFG_CTXRAYDETAILS)
    , m_Items()
{
}

FGCTXRayDetails::~FGCTXRayDetails()
{
    DcmIODUtil::freeContainer(m_Items);
}

FGBase* FGCTXRayDetails::clone() const
{
    OFunique_ptr<FGCTXRayDetails> copy(new FGCTXRayDetails);
    if (copy)
    {
        if (DcmIODUtil::copyContainer(this->m_Items, copy->m_Items).good())
        {
            return copy.release();
        }
    }
    return OFnullptr;
}

void FGCTXRayDetails::clearData()
{
    DcmIODUtil::freeContainer(m_Items);
    m_Items.clear();
}

OFCondition FGCTXRayDetails::check() const
{
    // Maybe add checks later
    return EC_Normal;
}

/// Read CT X-Ray Details Sequence from given item
OFCondition FGCTXRayDetails::read(DcmItem& item)
{
    clearData();
    DcmItem* seqItem       = NULL;
    unsigned long numItems = 0;
    OFCondition result     = getNumItemsFromFGSequence(item, DCM_CTXRayDetailsSequence, numItems);
    if (result.bad())
        return result;

    for (unsigned long num = 0; num < numItems; num++)
    {
        if (getItemFromFGSequence(item, DCM_CTXRayDetailsSequence, num, seqItem).good())
        {
            FGCTXRayDetailsItem* newItem = new FGCTXRayDetailsItem();
            newItem->read(*seqItem);
            m_Items.push_back(newItem);
        }
    }
    return EC_Normal;
}

/// Writes single CT X-Ray Details Sequence into given item
OFCondition FGCTXRayDetails::write(DcmItem& item)
{
    OFCondition result = check();
    if (result.good())
    {
        DcmIODUtil::writeSubSequence(
            result, DCM_CTXRayDetailsSequence, m_Items, item, "1-n", "2", "CTXRayDetailsMacro");
    }
    return result;
}

int FGCTXRayDetails::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result != 0)
        return result;

    const FGCTXRayDetails* myRhs = OFstatic_cast(const FGCTXRayDetails*, &rhs);
    if (!myRhs)
        return -1;

    // Compare all items
    return DcmIODUtil::compareContainer(this->m_Items, myRhs->m_Items);
}

OFVector<FGCTXRayDetails::FGCTXRayDetailsItem*>& FGCTXRayDetails::getCTXRayDetailsItems()
{
    return m_Items;
}

// ---------------------------------------------

// Constructor
FGCTXRayDetails::FGCTXRayDetailsItem::FGCTXRayDetailsItem()
    : m_ReferencedPathIndex(DCM_ReferencedPathIndex)
    , m_KVP(DCM_KVP)
    , m_FocalSpots(DCM_FocalSpots)
    , m_FilterType(DCM_FilterType)
    , m_FilterMaterial(DCM_FilterMaterial)
    , m_CalciumScoringMassFactorPatient(DCM_CalciumScoringMassFactorPatient)
    , m_CalciumScoringMassFactorDevice(DCM_CalciumScoringMassFactorDevice)
    , m_EnergyWeightingFactor(DCM_EnergyWeightingFactor)
{
}

FGCTXRayDetails::FGCTXRayDetailsItem::~FGCTXRayDetailsItem()
{
    // nothing to do
}

FGCTXRayDetails::FGCTXRayDetailsItem* FGCTXRayDetails::FGCTXRayDetailsItem::clone() const
{
    FGCTXRayDetailsItem* copy = new FGCTXRayDetailsItem();
    if (copy)
    {
        copy->m_ReferencedPathIndex             = this->m_ReferencedPathIndex;
        copy->m_KVP                             = this->m_KVP;
        copy->m_FocalSpots                      = this->m_FocalSpots;
        copy->m_FilterType                      = this->m_FilterType;
        copy->m_FilterMaterial                  = this->m_FilterMaterial;
        copy->m_CalciumScoringMassFactorPatient = this->m_CalciumScoringMassFactorPatient;
        copy->m_CalciumScoringMassFactorDevice  = this->m_CalciumScoringMassFactorDevice;
        copy->m_EnergyWeightingFactor           = this->m_EnergyWeightingFactor;
    }
    return copy;
}

void FGCTXRayDetails::FGCTXRayDetailsItem::clearData()
{
    m_ReferencedPathIndex.clear();
    m_KVP.clear();
    m_FocalSpots.clear();
    m_FilterType.clear();
    m_FilterMaterial.clear();
    m_CalciumScoringMassFactorPatient.clear();
    m_CalciumScoringMassFactorDevice.clear();
    m_EnergyWeightingFactor.clear();
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::check() const
{
    // Maybe add checks later
    return EC_Normal;
}

/// Read CT X-Ray Details Sequence from given item
OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::read(DcmItem& item)
{
    clearData();

    DcmIODUtil::getAndCheckElementFromDataset(item, m_ReferencedPathIndex, "1-n", "1C", "CTXRayDetailsMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_KVP, "1", "1C", "CTXRayDetailsMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_FocalSpots, "1-n", "1C", "CTXRayDetailsMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_FilterType, "1", "1C", "CTXRayDetailsMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_FilterMaterial, "1-n", "1C", "CTXRayDetailsMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_CalciumScoringMassFactorPatient, "1", "3", "CTXRayDetailsMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_CalciumScoringMassFactorDevice, "3", "3", "CTXRayDetailsMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_EnergyWeightingFactor, "1", "3", "CTXRayDetailsMacro");
    return EC_Normal;
}

/// Writes single CT X-Ray Details Sequence into given item
OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::write(DcmItem& item)
{
    // --- write frame content macro attributes ---
    OFCondition result;
    DcmIODUtil::copyElementToDataset(result, item, m_ReferencedPathIndex, "1-n", "1C", "CTXRayDetailsMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_KVP, "1", "1C", "CTXRayDetailsMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_FocalSpots, "1-n", "1C", "CTXRayDetailsMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_FilterType, "1", "1C", "CTXRayDetailsMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_FilterMaterial, "1-n", "1C", "CTXRayDetailsMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_CalciumScoringMassFactorPatient, "1", "3", "CTXRayDetailsMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_CalciumScoringMassFactorDevice, "3", "3", "CTXRayDetailsMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_EnergyWeightingFactor, "1", "3", "CTXRayDetailsMacro");

    return result;
}

int FGCTXRayDetails::FGCTXRayDetailsItem::compare(const FGCTXRayDetails::FGCTXRayDetailsItem& rhs) const
{
    const FGCTXRayDetails::FGCTXRayDetailsItem* myRhs
        = OFstatic_cast(const FGCTXRayDetails::FGCTXRayDetailsItem*, &rhs);
    if (!myRhs)
        return -1;

    // Compare all elements
    int result = m_ReferencedPathIndex.compare(myRhs->m_ReferencedPathIndex);
    if (result == 0)
        result = m_KVP.compare(myRhs->m_KVP);
    if (result == 0)
        result = m_FocalSpots.compare(myRhs->m_FocalSpots);
    if (result == 0)
        result = m_FilterType.compare(myRhs->m_FilterType);
    if (result == 0)
        result = m_FilterMaterial.compare(myRhs->m_FilterMaterial);
    if (result == 0)
        result = m_CalciumScoringMassFactorPatient.compare(myRhs->m_CalciumScoringMassFactorPatient);
    if (result == 0)
        result = m_CalciumScoringMassFactorDevice.compare(myRhs->m_CalciumScoringMassFactorDevice);
    if (result == 0)
        result = m_EnergyWeightingFactor.compare(myRhs->m_EnergyWeightingFactor);

    return result;
}

// --- get() functionality ---

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::getReferencedPathIndex(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_ReferencedPathIndex, value, pos);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::getReferencedPathIndex(OFVector<Uint16>& values)
{
    return DcmIODUtil::getUint16ValuesFromElement(m_ReferencedPathIndex, values);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::getKVP(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_KVP, value, pos);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::getKVP(Float64& value, const unsigned long pos)
{
    return m_KVP.getFloat64(value, pos);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::getFocalSpots(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_FocalSpots, value, pos);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::getFocalSpots(OFVector<Float64>& values)
{
    return DcmIODUtil::getFloat64ValuesFromElement(m_FocalSpots, values);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::getFilterType(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_FilterType, value, pos);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::getFilterMaterial(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_FilterType, value, pos);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::getCalciumScoringMassFactorPatient(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_CalciumScoringMassFactorPatient, value, pos);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::getCalciumScoringMassFactorPatient(Float32& value,
                                                                                     const unsigned long pos)
{
    return m_CalciumScoringMassFactorPatient.getFloat32(value, pos);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::getCalciumScoringMassFactorDevice(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_CalciumScoringMassFactorDevice, value, pos);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::getCalciumScoringMassFactorDevice(OFVector<Float64>& values)
{
    return DcmIODUtil::getFloat64ValuesFromElement(m_CalciumScoringMassFactorDevice, values);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::getEnergyWeightingFactor(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_EnergyWeightingFactor, value, pos);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::getEnergyWeightingFactor(Float32& value, const unsigned long pos)
{
    return m_EnergyWeightingFactor.getFloat32(value, pos);
}

// --- set() functionality ---

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::setReferencedPathIndex(const OFVector<Uint16>& values,
                                                                         const bool checkValues)
{
    (void)checkValues;
    return DcmIODUtil::setUint16ValuesOnElement(m_ReferencedPathIndex, values, "1-n", checkValues);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::setKVP(const Float64 value, const bool checkValue)
{
    (void)checkValue;
    return m_KVP.putFloat64(value);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::setFocalSpots(const OFVector<Float64>& values, const bool checkValues)
{
    return DcmIODUtil::setFloat64ValuesOnElement(m_FocalSpots, values, "1-n", checkValues);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::setFilterType(const OFString& value, const bool checkValue)
{
    OFCondition result = (checkValue) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_FilterType.putString(value.c_str());
    return result;
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::setFilterMaterial(const OFString& value, const bool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = m_FilterMaterial.putString(value.c_str());
    return result;
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::setCalciumScoringMassFactorPatient(const Float32 value,
                                                                                     const bool /* checkValue */)
{
    return m_CalciumScoringMassFactorPatient.putFloat32(value);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::setCalciumScoringMassFactorDevice(const OFVector<Float32>& values,
                                                                                    const bool checkValues)
{
    return DcmIODUtil::setFloat32ValuesOnElement(m_CalciumScoringMassFactorDevice, values, "1-n", checkValues);
}

OFCondition FGCTXRayDetails::FGCTXRayDetailsItem::setEnergyWeightingFactor(const Float32 value, const bool /* checkValue */)
{
    return m_EnergyWeightingFactor.putFloat32(value);
}
