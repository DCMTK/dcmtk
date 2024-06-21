/*
 *
 *  Copyright (C) 2015-2024, Open Connections GmbH
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
 *  Purpose: Class for managing Real World Value Mapping Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmfg/fgrealworldvaluemapping.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrss.h"
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmdata/dcvrsh.h"
#include "dcmtk/dcmiod/iodutil.h"

const OFString FGRealWorldValueMapping::RWVMItem::m_ModuleName = "RealWorldValueMappingItemMacro";

FGRealWorldValueMapping::FGRealWorldValueMapping()
    : FGBase(DcmFGTypes::EFG_REALWORLDVALUEMAPPING)
    , m_Items()
{
}

FGRealWorldValueMapping::~FGRealWorldValueMapping()
{
    clearData();
}

FGBase* FGRealWorldValueMapping::clone() const
{
    FGRealWorldValueMapping* copy = new FGRealWorldValueMapping();
    if (copy)
    {
        for (OFVector<FGRealWorldValueMapping::RWVMItem*>::const_iterator it = m_Items.begin(); it < m_Items.end();
             it++)
        {
            if (*it == NULL)
                return NULL;
            FGRealWorldValueMapping::RWVMItem* item = (*it)->clone();
            if (item == NULL)
                return NULL;
            copy->m_Items.push_back(item);
        }
    }
    return copy;
}

void FGRealWorldValueMapping::clearData()
{
    DcmIODUtil::freeContainer(m_Items);
}

OFCondition FGRealWorldValueMapping::check() const
{
    // More checks checks for 1C conditions could be added
    return EC_Normal;
}

int FGRealWorldValueMapping::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result != 0)
        return result;

    const FGRealWorldValueMapping* myRhs = OFstatic_cast(const FGRealWorldValueMapping*, &rhs);
    if (!myRhs)
        return -1;

    // Compare all items, start with VM
    if (m_Items.size() < myRhs->m_Items.size())
        return 1;
    else if (m_Items.size() > myRhs->m_Items.size())
        return -1;

    for (size_t n = 0; m_Items.size(); n++)
    {
        result = m_Items[n]->compare(*(myRhs->m_Items[n]));
        if (result != 0)
            return result;
    }
    return result;
}

/// Read from Frame Content Sequence
OFCondition FGRealWorldValueMapping::read(DcmItem& item)
{
    clearData();

    DcmIODUtil::readSubSequence(
        item, DCM_RealWorldValueMappingSequence, m_Items, "1-n", "1", "RealWorldValueMappingMacro");

    return EC_Normal;
}

OFCondition FGRealWorldValueMapping::write(DcmItem& item)
{
    // OFCondition result = createNewFGSequence(item, DCM_RealWorldValueMappingSequence, 0, seqItem);
    OFCondition result;
    DcmIODUtil::writeSubSequence(
        result, DCM_RealWorldValueMappingSequence, m_Items, item, "1-n", "1", "RealWorldValueMappingMacro");
    return result;
}

OFVector<FGRealWorldValueMapping::RWVMItem*>& FGRealWorldValueMapping::getRealWorldValueMapping()
{
    return m_Items;
}

// -----------------------------------------------------------

FGRealWorldValueMapping::RWVMItem::RWVMItem(IODComponent* parent)
    : IODComponent(parent)
    , m_MeasurementUnitsCode()
    , m_QuantityDefinitionSequence()
{
    resetRules();
}

FGRealWorldValueMapping::RWVMItem::RWVMItem(OFshared_ptr<DcmItem> item,
                                            OFshared_ptr<IODRules> rules,
                                            IODComponent* parent)
    : IODComponent(item, rules, parent)
    , m_MeasurementUnitsCode()
    , m_QuantityDefinitionSequence()
{
    resetRules();
}

FGRealWorldValueMapping::RWVMItem::RWVMItem(const FGRealWorldValueMapping::RWVMItem& rhs)
    : IODComponent(rhs)
    , m_MeasurementUnitsCode(rhs.m_MeasurementUnitsCode)
    , m_QuantityDefinitionSequence()
{
    OFVector<ContentItemMacro*>::const_iterator it = rhs.m_QuantityDefinitionSequence.begin();
    while (it != rhs.m_QuantityDefinitionSequence.end())
    {
        ContentItemMacro* macro = new ContentItemMacro(**it);
        if (macro == NULL)
        {
            DCMFG_ERROR("Out of memory in copy constructor of FGRealWorldValueMapping::RWVMItem::RWVMItem");
            return;
        }
        m_QuantityDefinitionSequence.push_back(macro);
        it++;
    }
}

FGRealWorldValueMapping::RWVMItem* FGRealWorldValueMapping::RWVMItem::clone()
{
    return new FGRealWorldValueMapping::RWVMItem(*this);
}

FGRealWorldValueMapping::RWVMItem::~RWVMItem()
{
    DcmIODUtil::freeContainer(m_QuantityDefinitionSequence);
}

OFString FGRealWorldValueMapping::RWVMItem::getName() const
{
    return m_ModuleName;
}

int FGRealWorldValueMapping::RWVMItem::compare(const IODComponent& rhs) const
{
    int result                                     = IODComponent::compare(rhs);
    const FGRealWorldValueMapping::RWVMItem* myRhs = OFstatic_cast(const FGRealWorldValueMapping::RWVMItem*, &rhs);
    if (!myRhs)
        return -1;

    if (result == 0)
        result = m_MeasurementUnitsCode.compare(*myRhs);
    if (result == 0)
    {
        size_t rhsSize  = myRhs->m_QuantityDefinitionSequence.size();
        size_t thisSize = m_QuantityDefinitionSequence.size();
        if (thisSize < rhsSize)
            return -1;
        else if (thisSize > rhsSize)
            return 1;

        OFVector<ContentItemMacro*>::const_iterator it    = m_QuantityDefinitionSequence.begin();
        OFVector<ContentItemMacro*>::const_iterator rhsIt = myRhs->m_QuantityDefinitionSequence.begin();
        while (it != m_QuantityDefinitionSequence.end() && (result == 0))
        {
            result = (*it)->compare(*(*rhsIt));
            it++;
        }
    }
    return result;
}

void FGRealWorldValueMapping::RWVMItem::resetRules()
{
    // parameters are tag, VM, type. Overwrite old rules if any.
    m_Rules->addRule(new IODRule(DCM_RealWorldValueFirstValueMapped, "1", "1C", getName(), DcmIODTypes::IE_INSTANCE),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_RealWorldValueLastValueMapped, "1", "1C", getName(), DcmIODTypes::IE_INSTANCE),
                     OFTrue);
    m_Rules->addRule(
        new IODRule(DCM_DoubleFloatRealWorldValueFirstValueMapped, "1", "1C", getName(), DcmIODTypes::IE_INSTANCE),
        OFTrue);
    m_Rules->addRule(
        new IODRule(DCM_DoubleFloatRealWorldValueLastValueMapped, "1", "1C", getName(), DcmIODTypes::IE_INSTANCE),
        OFTrue);
    m_Rules->addRule(new IODRule(DCM_RealWorldValueIntercept, "1", "1C", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_RealWorldValueSlope, "1", "1C", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_RealWorldValueLUTData, "1-n", "1C", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_LUTExplanation, "1", "1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_LUTLabel, "1", "1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_MeasurementUnitsCodeSequence, "1", "1", getName(), DcmIODTypes::IE_INSTANCE),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_QuantityDefinitionSequence, "1-n", "3", getName(), DcmIODTypes::IE_INSTANCE),
                     OFTrue);
}

OFCondition FGRealWorldValueMapping::RWVMItem::read(DcmItem& source, const OFBool clearOldData)
{
    DcmIODUtil::readSingleItem(source, DCM_MeasurementUnitsCodeSequence, m_MeasurementUnitsCode, "1", m_ModuleName);
    DcmIODUtil::readSubSequence(source,
                                DCM_QuantityDefinitionSequence,
                                m_QuantityDefinitionSequence,
                                m_Rules->getByTag(DCM_QuantityDefinitionSequence));
    return IODComponent::read(source, clearOldData);
}

OFCondition FGRealWorldValueMapping::RWVMItem::write(DcmItem& destination)
{
    OFCondition result;
    DcmIODUtil::writeSingleItem(
        result, DCM_MeasurementUnitsCodeSequence, m_MeasurementUnitsCode, *m_Item, "1", m_ModuleName);
    DcmIODUtil::writeSubSequence(result,
                                 DCM_QuantityDefinitionSequence,
                                 m_QuantityDefinitionSequence,
                                 *m_Item,
                                 m_Rules->getByTag(DCM_QuantityDefinitionSequence));
    if (result.good())
        result = IODComponent::write(destination);
    return result;
}

OFCondition FGRealWorldValueMapping::RWVMItem::getRealWorldValueFirstValueMapped(Sint32& value,
                                                                                 const unsigned long pos) const
{
    return getUSorSS(*m_Item, DCM_RealWorldValueFirstValueMapped, pos, value);
}

OFCondition FGRealWorldValueMapping::RWVMItem::getRealWorldValueLastValueMapped(Sint32& value,
                                                                                const unsigned long pos) const
{
    return getUSorSS(*m_Item, DCM_RealWorldValueLastValueMapped, pos, value);
}

OFCondition
FGRealWorldValueMapping::RWVMItem::getDoubleFloatRealWorldValueFirstValueMapped(Float64& value,
                                                                                const unsigned long pos) const
{
    return (*m_Item).findAndGetFloat64(DCM_DoubleFloatRealWorldValueFirstValueMapped, value, pos);
}

OFCondition
FGRealWorldValueMapping::RWVMItem::getDoubleFloatRealWorldValueLastValueMapped(Float64& value,
                                                                               const unsigned long pos) const
{
    return (*m_Item).findAndGetFloat64(DCM_DoubleFloatRealWorldValueFirstValueMapped, value, pos);
}

OFCondition FGRealWorldValueMapping::RWVMItem::getRealWorldValueLUTData(OFVector<Float64>& values) const
{
    DcmElement* elem   = NULL;
    OFCondition result = m_Item->findAndGetElement(DCM_RealWorldValueLUTData, elem);
    if (result.good())
    {
        size_t numValues = elem->getNumberOfValues();
        for (size_t n = 0; n < numValues; n++)
        {
            Float64 value;
            result = elem->getFloat64(value, OFstatic_cast(unsigned long, n));
            if (result.good())
                values.push_back(value);
        }
    }
    return result;
}

OFCondition FGRealWorldValueMapping::RWVMItem::getRealWorldValueLUTData(Float64& value, const unsigned long pos) const
{
    return m_Item->findAndGetFloat64(DCM_RealWorldValueLUTData, value, pos);
}

OFCondition FGRealWorldValueMapping::RWVMItem::getLUTExplanation(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_LUTExplanation, *m_Item, value, pos);
}

OFCondition FGRealWorldValueMapping::RWVMItem::getLUTLabel(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_LUTLabel, *m_Item, value, pos);
}

CodeSequenceMacro& FGRealWorldValueMapping::RWVMItem::getMeasurementUnitsCode()
{
    return m_MeasurementUnitsCode;
}

OFVector<ContentItemMacro*>& FGRealWorldValueMapping::RWVMItem::getEntireQuantityDefinitionSequence()
{
    return m_QuantityDefinitionSequence;
}

OFCondition FGRealWorldValueMapping::RWVMItem::setRealWorldValueFirstValueMappedUnsigned(const Uint16 value,
                                                                                         const OFBool checkValue)
{
    (void)checkValue;
    DcmUnsignedShort* us = new DcmUnsignedShort(DCM_RealWorldValueFirstValueMapped);
    OFCondition result   = us->putUint16(value);
    if (result.good())
        result = m_Item->insert(us, OFTrue /* replace old */);
    if (result.bad())
        delete us;
    return result;
}

OFCondition FGRealWorldValueMapping::RWVMItem::setRealWorldValueFirstValueMappedSigned(const Sint16& value,
                                                                                       const OFBool checkValue)
{
    (void)checkValue;
    DcmSignedShort* ss = new DcmSignedShort(DCM_RealWorldValueFirstValueMapped);
    OFCondition result = ss->putSint16(value);
    if (result.good())
        result = m_Item->insert(ss, OFTrue /* replace old */);
    if (result.bad())
        delete ss;
    return result;
}

OFCondition FGRealWorldValueMapping::RWVMItem::setRealWorldValueLastValueMappedSigned(const Sint16& value,
                                                                                      const OFBool checkValue)
{
    (void)checkValue;
    DcmSignedShort* ss = new DcmSignedShort(DCM_RealWorldValueLastValueMapped);
    OFCondition result = ss->putSint16(value);
    if (result.good())
        result = m_Item->insert(ss, OFTrue /* replace old */);
    if (result.bad())
        delete ss;
    return result;
}

OFCondition FGRealWorldValueMapping::RWVMItem::setRealWorldValueLastValueMappedUnsigned(const Uint16 value,
                                                                                        const OFBool checkValue)
{
    (void)checkValue;
    DcmUnsignedShort* us = new DcmUnsignedShort(DCM_RealWorldValueLastValueMapped);
    OFCondition result   = us->putUint16(value);
    if (result.good())
        result = m_Item->insert(us, OFTrue /* replace old */);
    if (result.bad())
        delete us;
    return result;
}

OFCondition FGRealWorldValueMapping::RWVMItem::setDoubleFloatRealWorldValueFirstValueMapped(const Float64 value,
                                                                                            const OFBool checkValue)
{
    (void)checkValue;
    return (*m_Item).putAndInsertFloat64(DCM_DoubleFloatRealWorldValueFirstValueMapped, value);
}

OFCondition FGRealWorldValueMapping::RWVMItem::setDoubleFloatRealWorldValueLastValueMapped(const Float64 value,
                                                                                           const OFBool checkValue)
{
    (void)checkValue;
    return (*m_Item).putAndInsertFloat64(DCM_DoubleFloatRealWorldValueLastValueMapped, value);
}

OFCondition FGRealWorldValueMapping::RWVMItem::setRealWorldValueIntercept(const Float64 value, const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertFloat64(DCM_RealWorldValueIntercept, value);
}

OFCondition FGRealWorldValueMapping::RWVMItem::setRealWorldValueSlope(const Float64 value, const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertFloat64(DCM_RealWorldValueSlope, value);
}

OFCondition FGRealWorldValueMapping::RWVMItem::setRealWorldValueLUTData(const OFVector<Float64>& value,
                                                                        const OFBool checkValue)
{
    (void)checkValue;
    DcmElement* elem   = NULL;
    OFCondition result = m_Item->findAndGetElement(DCM_RealWorldValueLUTData, elem);
    if (result.good())
    {
        size_t count = value.size();
        for (size_t n = 0; n < count; n++)
        {
            if (result.good())
                result = elem->putFloat64(value[n], OFstatic_cast(unsigned long, n));
        }
    }
    return result;
}

OFCondition FGRealWorldValueMapping::RWVMItem::setLUTExplanation(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_LUTExplanation, value);
    return result;
}

OFCondition FGRealWorldValueMapping::RWVMItem::setLUTLabel(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_LUTLabel, value);
    return result;
}

OFCondition FGRealWorldValueMapping::RWVMItem::getUSorSS(DcmItem& item,
                                                         const DcmTagKey& key,
                                                         const unsigned long pos,
                                                         Sint32& value)
{
    DcmElement* elem   = NULL;
    OFCondition result = item.findAndGetElement(key, elem);
    if (result.good() && elem)
    {
        DcmEVR evr = elem->getVR();
        if (evr == EVR_US)
        {
            Uint16 num;
            result = elem->getUint16(num, pos);
            if (result.good())
                value = num;
        }
        else if (evr == EVR_SS)
        {
            Sint16 num;
            result = elem->getSint16(num, pos);
            if (result.good())
                value = num;
        }
        else
        {
            DCMFG_ERROR("Illegal VR " << DcmVR(evr).getVRName() << " for tag " << DcmTag(key).getTagName());
            result = EC_InvalidVR;
        }
    }
    return result;
}
