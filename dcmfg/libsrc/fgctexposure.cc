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
 *  Purpose: Class for managing the CT Exposure Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmfg/fgctexposure.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/dcmiod/iodutil.h"

// Constructor
FGCTExposure::FGCTExposureItem::FGCTExposureItem()
    : m_ReferencedPathIndex(DCM_ReferencedPathIndex)
    , m_ExposureTimeInMs(DCM_ExposureTimeInms)
    , m_XRayTubeCurrentInMa(DCM_XRayTubeCurrentInmA)
    , m_ExposureInMas(DCM_ExposureInmAs)
    , m_ExposureModulationType(DCM_ExposureModulationType)
    , m_EstimatedDoseSaving(DCM_RETIRED_EstimatedDoseSaving)
    , m_CTDIVol(DCM_CTDIvol)
    , m_CTDIPhantomTypeCodeSequence()
    , m_WaterEquivalentDiameter(DCM_WaterEquivalentDiameter)
    , m_WaterEquivalentDiameterCalculationMethodCodeSequence()
    , m_ImageAndFluoroscopyAreaDoseProduct(DCM_ImageAndFluoroscopyAreaDoseProduct)
{
}

FGCTExposure::FGCTExposureItem::FGCTExposureItem(const FGCTExposureItem& rhs)
    : m_ReferencedPathIndex(rhs.m_ReferencedPathIndex)
    , m_ExposureTimeInMs(rhs.m_ExposureTimeInMs)
    , m_XRayTubeCurrentInMa(rhs.m_XRayTubeCurrentInMa)
    , m_ExposureInMas(rhs.m_ExposureInMas)
    , m_ExposureModulationType(rhs.m_ExposureModulationType)
    , m_EstimatedDoseSaving(rhs.m_EstimatedDoseSaving)
    , m_CTDIVol(rhs.m_CTDIVol)
    , m_CTDIPhantomTypeCodeSequence()
    , m_WaterEquivalentDiameter(rhs.m_WaterEquivalentDiameter)
    , m_WaterEquivalentDiameterCalculationMethodCodeSequence()
    , m_ImageAndFluoroscopyAreaDoseProduct(rhs.m_ImageAndFluoroscopyAreaDoseProduct)
{
    DcmIODUtil::copyContainer(rhs.m_CTDIPhantomTypeCodeSequence, this->m_CTDIPhantomTypeCodeSequence);
    DcmIODUtil::copyContainer(rhs.m_WaterEquivalentDiameterCalculationMethodCodeSequence,
                              this->m_WaterEquivalentDiameterCalculationMethodCodeSequence);
}

FGCTExposure::FGCTExposureItem::~FGCTExposureItem()
{
    DcmIODUtil::freeContainer(m_CTDIPhantomTypeCodeSequence);
    DcmIODUtil::freeContainer(m_WaterEquivalentDiameterCalculationMethodCodeSequence);
}

FGCTExposure::FGCTExposureItem* FGCTExposure::FGCTExposureItem::clone() const
{
    OFCondition result;
    OFunique_ptr<FGCTExposureItem> copy(new FGCTExposureItem);
    if (copy)
    {
        copy->m_ReferencedPathIndex                = this->m_ReferencedPathIndex;
        copy->m_ExposureTimeInMs                   = this->m_ExposureTimeInMs;
        copy->m_XRayTubeCurrentInMa                = this->m_XRayTubeCurrentInMa;
        copy->m_ExposureInMas                      = this->m_ExposureInMas;
        copy->m_ExposureModulationType             = this->m_ExposureModulationType;
        copy->m_EstimatedDoseSaving                = this->m_EstimatedDoseSaving;
        copy->m_CTDIVol                            = this->m_CTDIVol;
        copy->m_WaterEquivalentDiameter            = this->m_WaterEquivalentDiameter;
        copy->m_ImageAndFluoroscopyAreaDoseProduct = this->m_ImageAndFluoroscopyAreaDoseProduct;
        // Copy sequences
        result = DcmIODUtil::copyContainer(this->m_CTDIPhantomTypeCodeSequence, copy->m_CTDIPhantomTypeCodeSequence);
        if (result.good())
            DcmIODUtil::copyContainer(this->m_WaterEquivalentDiameterCalculationMethodCodeSequence,
                                      copy->m_WaterEquivalentDiameterCalculationMethodCodeSequence);
        if (result.good())
            return copy.release();
    }
    return OFnullptr;
}

void FGCTExposure::FGCTExposureItem::clearData()
{
    m_ReferencedPathIndex.clear();
    m_ExposureTimeInMs.clear();
    m_XRayTubeCurrentInMa.clear();
    m_ExposureInMas.clear();
    m_ExposureModulationType.clear();
    m_EstimatedDoseSaving.clear();
    m_CTDIVol.clear();
    m_WaterEquivalentDiameter.clear();
    m_ImageAndFluoroscopyAreaDoseProduct.clear();
    // clear sequences
    DcmIODUtil::freeContainer(m_CTDIPhantomTypeCodeSequence);
    DcmIODUtil::freeContainer(m_WaterEquivalentDiameterCalculationMethodCodeSequence);
}

OFCondition FGCTExposure::FGCTExposureItem::check() const
{
    // Maybe add checks later
    return EC_Normal;
}

/// Read CT Exposure Sequence from given item
OFCondition FGCTExposure::FGCTExposureItem::read(DcmItem& item)
{
    clearData();

    DcmIODUtil::getAndCheckElementFromDataset(item, m_ReferencedPathIndex, "1-n", "1C", "CTExposureMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_ExposureTimeInMs, "1", "1C", "CTExposureMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_XRayTubeCurrentInMa, "1", "1C", "CTExposureMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_ExposureInMas, "1", "1C", "CTExposureMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_ExposureModulationType, "1-n", "1C", "CTExposureMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_EstimatedDoseSaving, "1", "2C", "CTExposureMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_CTDIVol, "1", "2C", "CTExposureMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_WaterEquivalentDiameter, "1", "3", "CTExposureMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_ImageAndFluoroscopyAreaDoseProduct, "1", "3", "CTExposureMacro");

    DcmIODUtil::readSubSequence(
        item, DCM_CTDIPhantomTypeCodeSequence, m_CTDIPhantomTypeCodeSequence, "1", "3", "CTExposureMacro");
    DcmIODUtil::readSubSequence(item,
                                DCM_WaterEquivalentDiameterCalculationMethodCodeSequence,
                                m_WaterEquivalentDiameterCalculationMethodCodeSequence,
                                "1",
                                "3",
                                "CTExposureMacro");

    return EC_Normal;
}

/// Writes single CT Exposure Sequence into given item
OFCondition FGCTExposure::FGCTExposureItem::write(DcmItem& item)
{
    OFCondition result;

    // --- write frame content macro attributes ---
    DcmIODUtil::copyElementToDataset(result, item, m_ReferencedPathIndex, "1-n", "1C", "CTExposureMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_ExposureTimeInMs, "1", "1C", "CTExposureMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_XRayTubeCurrentInMa, "1", "1C", "CTExposureMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_ExposureInMas, "1", "1C", "CTExposureMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_ExposureModulationType, "1-n", "1C", "CTExposureMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_EstimatedDoseSaving, "1", "2C", "CTExposureMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_CTDIVol, "1", "2C", "CTExposureMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_WaterEquivalentDiameter, "1", "3", "CTExposureMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_ImageAndFluoroscopyAreaDoseProduct, "1", "3", "CTExposureMacro");
    DcmIODUtil::writeSubSequence(
        result, DCM_CTDIPhantomTypeCodeSequence, m_CTDIPhantomTypeCodeSequence, item, "1", "3", "CTExposureMacro");
    DcmIODUtil::writeSubSequence(result,
                                 DCM_WaterEquivalentDiameterCalculationMethodCodeSequence,
                                 m_WaterEquivalentDiameterCalculationMethodCodeSequence,
                                 item,
                                 "1",
                                 "3",
                                 "CTExposureMacro");

    return result;
}

int FGCTExposure::FGCTExposureItem::compare(const FGCTExposure::FGCTExposureItem& rhs) const
{
    FGCTExposureItem* myRhs = OFconst_cast(FGCTExposureItem*, OFstatic_cast(const FGCTExposureItem*, &rhs));
    if (!myRhs)
        return -1;

    // Compare all elements
    int result = m_ReferencedPathIndex.compare(myRhs->m_ReferencedPathIndex);
    if (result == 0)
        result = m_ExposureTimeInMs.compare(myRhs->m_ExposureTimeInMs);
    if (result == 0)
        result = m_XRayTubeCurrentInMa.compare(myRhs->m_XRayTubeCurrentInMa);
    if (result == 0)
        result = m_ExposureInMas.compare(myRhs->m_ExposureInMas);
    if (result == 0)
        result = m_ExposureModulationType.compare(myRhs->m_ExposureModulationType);
    if (result == 0)
        result = m_EstimatedDoseSaving.compare(myRhs->m_EstimatedDoseSaving);
    if (result == 0)
        result = m_CTDIVol.compare(myRhs->m_CTDIVol);
    if (result == 0)
        result = m_WaterEquivalentDiameter.compare(myRhs->m_WaterEquivalentDiameter);
    if (result == 0)
        result = m_ImageAndFluoroscopyAreaDoseProduct.compare(myRhs->m_ImageAndFluoroscopyAreaDoseProduct);
    if (result == 0)
        result = DcmIODUtil::compareContainer(m_CTDIPhantomTypeCodeSequence, myRhs->m_CTDIPhantomTypeCodeSequence);
    if (result == 0)
        result = DcmIODUtil::compareContainer(m_WaterEquivalentDiameterCalculationMethodCodeSequence,
                                              myRhs->m_WaterEquivalentDiameterCalculationMethodCodeSequence);

    return result;
}

// --- get() functionality ---

OFCondition FGCTExposure::FGCTExposureItem::getReferencedPathIndex(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_ReferencedPathIndex, value, pos);
}

OFCondition FGCTExposure::FGCTExposureItem::getExposureTimeInMs(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_ExposureTimeInMs, value, pos);
}

OFCondition FGCTExposure::FGCTExposureItem::getExposureTimeInMs(Float64& value, const unsigned long pos)
{
    return m_ExposureTimeInMs.getFloat64(value, pos);
}

OFCondition FGCTExposure::FGCTExposureItem::getXRayTubeCurrentInMa(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_XRayTubeCurrentInMa, value, pos);
}

OFCondition FGCTExposure::FGCTExposureItem::getXRayTubeCurrentInMa(Float64& value, const unsigned long pos)
{
    return m_XRayTubeCurrentInMa.getFloat64(value, pos);
}

OFCondition FGCTExposure::FGCTExposureItem::getExposureInMas(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_ExposureInMas, value, pos);
}

OFCondition FGCTExposure::FGCTExposureItem::getExposureInMas(Float64& value, const unsigned long pos)
{
    return m_ExposureInMas.getFloat64(value, pos);
}

OFCondition FGCTExposure::FGCTExposureItem::getExposureModulationType(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_ExposureModulationType, value, pos);
}

OFCondition FGCTExposure::FGCTExposureItem::getEstimatedDoseSaving(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_EstimatedDoseSaving, value, pos);
}

OFCondition FGCTExposure::FGCTExposureItem::getEstimatedDoseSaving(Float64& value, const unsigned long pos)
{
    return m_EstimatedDoseSaving.getFloat64(value, pos);
}

OFCondition FGCTExposure::FGCTExposureItem::getCTDIVol(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_CTDIVol, value, pos);
}

OFCondition FGCTExposure::FGCTExposureItem::getCTDIVol(Float64& value, const unsigned long pos)
{
    return m_CTDIVol.getFloat64(value, pos);
}

OFVector<CodeSequenceMacro*>& FGCTExposure::FGCTExposureItem::getCTDIPhantomTypeCodeSequence()
{
    return m_CTDIPhantomTypeCodeSequence;
}

OFCondition FGCTExposure::FGCTExposureItem::getWaterEquivalentDiameter(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_WaterEquivalentDiameter, value, pos);
}

OFCondition FGCTExposure::FGCTExposureItem::getWaterEquivalentDiameter(Float64& value, const unsigned long pos)
{
    return m_WaterEquivalentDiameter.getFloat64(value, pos);
}

OFVector<CodeSequenceMacro*>& FGCTExposure::FGCTExposureItem::getWaterEquivalentDiameterCalculationMethodCodeSequence()
{
    return m_WaterEquivalentDiameterCalculationMethodCodeSequence;
}

OFCondition FGCTExposure::FGCTExposureItem::getImageAndFluoroscopyAreaDoseProduct(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_ImageAndFluoroscopyAreaDoseProduct, value, pos);
}

OFCondition FGCTExposure::FGCTExposureItem::getImageAndFluoroscopyAreaDoseProduct(Float64& value,
                                                                                  const unsigned long pos)
{
    return m_ImageAndFluoroscopyAreaDoseProduct.getFloat64(value, pos);
}

// --- set() functionality ---

OFCondition FGCTExposure::FGCTExposureItem::setReferencedPathIndex(const OFVector<Uint16>& values,
                                                                   const bool checkValues)
{
    (void)checkValues;
    return DcmIODUtil::setUint16ValuesOnElement(m_ReferencedPathIndex, values, "1-n", checkValues);
}

OFCondition FGCTExposure::FGCTExposureItem::setExposureTimeInMs(const Float64 value, const bool checkValue)
{
    (void)checkValue;
    return m_ExposureTimeInMs.putFloat64(value);
}

OFCondition FGCTExposure::FGCTExposureItem::setXRayTubeCurrentInMa(const Float64 value, const bool checkValue)
{
    (void)checkValue;
    return m_XRayTubeCurrentInMa.putFloat64(value);
}

OFCondition FGCTExposure::FGCTExposureItem::setExposureInMas(const Float64 value, const bool checkValue)
{
    (void)checkValue;
    return m_ExposureInMas.putFloat64(value);
}

OFCondition FGCTExposure::FGCTExposureItem::setExposureModulationType(const OFString& value, const bool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = m_ExposureModulationType.putString(value.c_str());
    return result;
}

OFCondition FGCTExposure::FGCTExposureItem::setEstimatedDoseSaving(const Float64 value, const bool checkValue)
{
    (void)checkValue;
    return m_EstimatedDoseSaving.putFloat64(value);
}

OFCondition FGCTExposure::FGCTExposureItem::setCTDIVol(const Float64 value, const bool checkValue)
{
    (void)checkValue;
    return m_CTDIVol.putFloat64(value);
}

OFCondition FGCTExposure::FGCTExposureItem::setWaterEquivalentDiameter(const Float64 value, const bool checkValue)
{
    (void)checkValue;
    return m_WaterEquivalentDiameter.putFloat64(value);
}

OFCondition FGCTExposure::FGCTExposureItem::setImageAndFluoroscopyAreaDoseProduct(const Float64 value,
                                                                                  const bool checkValue)
{
    (void)checkValue;
    return m_ImageAndFluoroscopyAreaDoseProduct.putFloat64(value);
}

// ---------------------

// Constructor
FGCTExposure::FGCTExposure()
    : FGBase(DcmFGTypes::EFG_CTEXPOSURE)
    , m_Items()
{
}

FGCTExposure::FGCTExposure(const FGCTExposure& rhs)
    : FGBase(rhs)
    , m_Items()
{
    OFCondition result = DcmIODUtil::copyContainer(rhs.m_Items, this->m_Items);
    if (result.bad())
    {
        DCMFG_ERROR("Cannot copy FGCTExposure class: " << result.text());
    }
}

FGCTExposure::~FGCTExposure()
{
    DcmIODUtil::freeContainer(m_Items);
}

FGBase* FGCTExposure::clone() const
{
    OFunique_ptr<FGCTExposure> copy(new FGCTExposure());
    if (copy)
    {
        if (DcmIODUtil::copyContainer(this->m_Items, copy->m_Items).good())
        {
            return copy.release();
        }
    }
    return OFnullptr;
}

void FGCTExposure::clearData()
{
    DcmIODUtil::freeContainer(m_Items);
    m_Items.clear();
}

OFCondition FGCTExposure::check() const
{
    // Maybe add checks later
    return EC_Normal;
}

/// Read CT Exposure Sequence from given item
OFCondition FGCTExposure::read(DcmItem& item)
{
    clearData();
    DcmItem* seqItem       = NULL;
    unsigned long numItems = 0;
    OFCondition result     = getNumItemsFromFGSequence(item, DCM_CTExposureSequence, numItems);
    if (result.bad())
        return result;

    for (unsigned long num = 0; num < numItems; num++)
    {
        if (getItemFromFGSequence(item, DCM_CTExposureSequence, num, seqItem).good())
        {
            FGCTExposureItem* newItem = new FGCTExposureItem();
            newItem->read(*seqItem);
            m_Items.push_back(newItem);
        }
    }
    return EC_Normal;
}

/// Writes single CT Exposure Sequence into given item
OFCondition FGCTExposure::write(DcmItem& item)
{
    OFCondition result = check();
    if (result.good())
    {
        DcmIODUtil::writeSubSequence(result, DCM_CTExposureSequence, m_Items, item, "1-n", "2", "CTGeometryMacro");
    }
    return result;
}

int FGCTExposure::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result != 0)
        return result;

    const FGCTExposure* myRhs = OFstatic_cast(const FGCTExposure*, &rhs);
    if (!myRhs)
        return -1;

    // Compare all items
    return DcmIODUtil::compareContainer(this->m_Items, myRhs->m_Items);
}

OFVector<FGCTExposure::FGCTExposureItem*>& FGCTExposure::getCTExposureItems()
{
    return m_Items;
}
