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
 *  Module: dcmfg
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class for managing the CT Additional X-Ray Source
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmfg/fgctadditionalxraysource.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/dcmiod/iodutil.h"

FGCTAdditionalXRaySource::FGCTAdditionalXRaySource()
    : FGBase(DcmFGTypes::EFG_CTADDITIONALXRAYSOURCE)
    , m_Items()
{
}

OFCondition FGCTAdditionalXRaySource::check() const
{
    // TODO Maybe add checks later
    return EC_Normal;
}

void FGCTAdditionalXRaySource::clearData()
{
    DcmIODUtil::freeContainer(m_Items);
    m_Items.clear();
}

int FGCTAdditionalXRaySource::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result != 0)
        return result;

    FGCTAdditionalXRaySource* myRhs
        = OFconst_cast(FGCTAdditionalXRaySource*, (OFstatic_cast(const FGCTAdditionalXRaySource*, &rhs)));
    if (!myRhs)
        return -1;

    // Compare all items
    return DcmIODUtil::compareContainer(m_Items, myRhs->getCTAdditionalXRaySourceItems());
}

OFCondition FGCTAdditionalXRaySource::read(DcmItem& item)
{
    clearData();
    DcmItem* seqItem       = NULL;
    unsigned long numItems = 0;
    OFCondition result     = getNumItemsFromFGSequence(item, DCM_CTAdditionalXRaySourceSequence, numItems);
    if (result.bad())
        return result;

    for (unsigned long num = 0; num < numItems; num++)
    {
        if (getItemFromFGSequence(item, DCM_CTAdditionalXRaySourceSequence, num, seqItem).good())
        {
            FGCTAdditionalXRaySourceItem* newItem = new FGCTAdditionalXRaySourceItem();
            newItem->read(*seqItem);
            m_Items.push_back(newItem);
        }
    }
    return EC_Normal;
}

OFCondition FGCTAdditionalXRaySource::write(DcmItem& item)
{
    OFCondition result = check();
    if (result.good())
    {
        DcmIODUtil::writeSubSequence(
            result, DCM_CTAdditionalXRaySourceSequence, m_Items, item, "1-n", "2", "CT Additional X-Ray Source");
    }

    return result;
}

FGBase* FGCTAdditionalXRaySource::clone() const
{
    OFunique_ptr<FGCTAdditionalXRaySource> copy(new FGCTAdditionalXRaySource());
    if (copy)
    {
        if (DcmIODUtil::copyContainer(this->m_Items, copy->m_Items).good())
        {
            return copy.release();
        }
    }
    return OFnullptr;
}

OFVector<FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem*>&
FGCTAdditionalXRaySource::getCTAdditionalXRaySourceItems()
{
    return m_Items;
}

FGCTAdditionalXRaySource::~FGCTAdditionalXRaySource()
{
    clearData();
}

FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::FGCTAdditionalXRaySourceItem()
    : m_KVP(DCM_KVP)
    , m_XRayTubeCurrentInmA(DCM_XRayTubeCurrentInmA)
    , m_DataCollectionDiameter(DCM_DataCollectionDiameter)
    , m_FocalSpots(DCM_FocalSpots)
    , m_FilterType(DCM_FilterType)
    , m_FilterMaterial(DCM_FilterMaterial)
    , m_ExposureInmAs(DCM_ExposureInmAs)
    , m_EnergyWeightingFactor(DCM_EnergyWeightingFactor)
{
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::check() const
{
    // TODO: Maybe add checks lager
    return EC_Normal;
}

FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::~FGCTAdditionalXRaySourceItem()
{
    // nothing to do
}

void FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::clearData()
{
    m_KVP.clear();
    m_XRayTubeCurrentInmA.clear();
    m_DataCollectionDiameter.clear();
    m_FocalSpots.clear();
    m_FilterType.clear();
    m_FilterMaterial.clear();
    m_ExposureInmAs.clear();
    m_EnergyWeightingFactor.clear();
}

FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem*
FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::clone() const
{
    if (FGCTAdditionalXRaySourceItem* copy = new FGCTAdditionalXRaySourceItem)
    {
        copy->m_KVP                    = m_KVP;
        copy->m_XRayTubeCurrentInmA    = m_XRayTubeCurrentInmA;
        copy->m_DataCollectionDiameter = m_DataCollectionDiameter;
        copy->m_FocalSpots             = m_FocalSpots;
        copy->m_FilterType             = m_FilterType;
        copy->m_FilterMaterial         = m_FilterMaterial;
        copy->m_ExposureInmAs          = m_ExposureInmAs;
        copy->m_EnergyWeightingFactor  = m_EnergyWeightingFactor;
        return copy;
    }
    return OFnullptr;
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::read(DcmItem& item)
{
    clearData();

    OFCondition result;

    DcmIODUtil::getAndCheckElementFromDataset(item, m_KVP, "1", "1", "CT Additional X-Ray Source");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_XRayTubeCurrentInmA, "1", "1", "CT Additional X-Ray Source");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_DataCollectionDiameter, "1", "1", "CT Additional X-Ray Source");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_FocalSpots, "1-n", "1", "CT Additional X-Ray Source");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_FilterType, "1", "1", "CT Additional X-Ray Source");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_FilterMaterial, "1-n", "1", "CT Additional X-Ray Source");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_ExposureInmAs, "1", "1", "CT Additional X-Ray Source");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_EnergyWeightingFactor, "1", "1C", "CT Additional X-Ray Source");

    return EC_Normal;
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::write(DcmItem& item)
{
    OFCondition result = check();
    if (result.bad())
        return result;

    DcmIODUtil::copyElementToDataset(result, item, m_KVP, "1", "1", "CT Additional X-Ray Source");
    DcmIODUtil::copyElementToDataset(result, item, m_XRayTubeCurrentInmA, "1", "1", "CT Additional X-Ray Source");
    DcmIODUtil::copyElementToDataset(result, item, m_DataCollectionDiameter, "1", "1", "CT Additional X-Ray Source");
    DcmIODUtil::copyElementToDataset(result, item, m_FocalSpots, "1-n", "1", "CT Additional X-Ray Source");
    DcmIODUtil::copyElementToDataset(result, item, m_FilterType, "1", "1", "CT Additional X-Ray Source");
    DcmIODUtil::copyElementToDataset(result, item, m_FilterMaterial, "1-n", "1", "CT Additional X-Ray Source");
    DcmIODUtil::copyElementToDataset(result, item, m_ExposureInmAs, "1", "1", "CT Additional X-Ray Source");
    DcmIODUtil::copyElementToDataset(result, item, m_EnergyWeightingFactor, "1", "1C", "CT Additional X-Ray Source");

    return result;
}

int FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::compare(
    const FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem& rhs) const
{
    int result                                = 0;
    const FGCTAdditionalXRaySourceItem* myRhs = OFstatic_cast(const FGCTAdditionalXRaySourceItem*, &rhs);

    // Compare all elements
    result = m_KVP.compare(myRhs->m_KVP);
    if (result == 0)
        result = m_XRayTubeCurrentInmA.compare(myRhs->m_XRayTubeCurrentInmA);
    if (result == 0)
        result = m_DataCollectionDiameter.compare(myRhs->m_DataCollectionDiameter);
    if (result == 0)
        result = m_FocalSpots.compare(myRhs->m_FocalSpots);
    if (result == 0)
        result = m_FilterType.compare(myRhs->m_FilterType);
    if (result == 0)
        result = m_FilterMaterial.compare(myRhs->m_FilterMaterial);
    if (result == 0)
        result = m_ExposureInmAs.compare(myRhs->m_ExposureInmAs);
    if (result == 0)
        result = m_EnergyWeightingFactor.compare(myRhs->m_EnergyWeightingFactor);

    return result;
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::getKVP(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromElement(m_KVP, value, pos);
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::getXRayTubeCurrentInmA(Float64& value,
                                                                                           const unsigned long pos) const
{
    return DcmIODUtil::getFloat64ValueFromElement(m_XRayTubeCurrentInmA, value, pos);
}

OFCondition
FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::getDataCollectionDiameter(OFString& value,
                                                                                  const signed long pos) const
{
    return DcmIODUtil::getStringValueFromElement(m_DataCollectionDiameter, value, pos);
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::getFocalSpots(OFString& value,
                                                                                  const signed long pos) const
{
    return DcmIODUtil::getStringValueFromElement(m_FocalSpots, value, pos);
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::getFilterType(OFString& value,
                                                                                  const signed long pos) const
{
    return DcmIODUtil::getStringValueFromElement(m_FilterType, value, pos);
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::getFilterMaterial(OFString& value,
                                                                                      const signed long pos) const
{
    return DcmIODUtil::getStringValueFromElement(m_FilterMaterial, value, pos);
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::getExposureInmAs(Float64& value,
                                                                                     const unsigned long pos) const
{
    return DcmIODUtil::getFloat64ValueFromElement(m_ExposureInmAs, value, pos);
}

OFCondition
FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::getEnergyWeightingFactor(Float32& value,
                                                                                 const unsigned long pos) const
{
    return DcmIODUtil::getFloat32ValueFromElement(m_EnergyWeightingFactor, value, pos);
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::setKVP(const OFString& value,
                                                                           const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_KVP.putString(value.c_str());
    return result;
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::setKVP(const Float64 value, const OFBool /* checkValue */)
{
    return m_KVP.putFloat64(value);
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::setXRayTubeCurrentInmA(const Float64 value,
                                                                                           const OFBool /* checkValue */)
{
    return m_XRayTubeCurrentInmA.putFloat64(value);
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::setDataCollectionDiameter(const OFString& value,
                                                                                              const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_DataCollectionDiameter.putString(value.c_str());
    return result;
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::setDataCollectionDiameter(const Float64 value,
                                                                                              const OFBool /* checkValue */)
{
    return m_DataCollectionDiameter.putFloat64(value);
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::setFocalSpots(const OFString& value,
                                                                                  const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_FocalSpots.putString(value.c_str());
    return result;
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::setFocalSpots(const OFVector<Float64>& values,
                                                                                  const OFBool checkValue)
{
    return DcmIODUtil::setFloat64ValuesOnElement(m_FocalSpots, values, "1-n", checkValue);
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::setFilterType(const OFString& value,
                                                                                  const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_FilterType.putString(value.c_str());
    return result;
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::setFilterMaterial(const OFString& value,
                                                                                      const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_FilterMaterial.putString(value.c_str());
    return result;
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::setExposureInmAs(const Float64 value,
                                                                                     const OFBool /* checkValue */)
{
    return m_ExposureInmAs.putFloat64(value);
}

OFCondition FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem::setEnergyWeightingFactor(const Float32 value,
                                                                                             const OFBool /* checkValue */)
{
    return m_EnergyWeightingFactor.putFloat32(value);
}
