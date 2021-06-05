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
 *  Purpose: Class for managing the CT Table Dynamics Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmfg/fgcttabledynamics.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/dcmiod/iodutil.h"

// Constructor
FGCTTableDynamics::FGCTTableDynamics()
    : FGBase(DcmFGTypes::EFG_CTTABLEDYNAMICS)
    , m_Items()
{
}

FGCTTableDynamics::~FGCTTableDynamics()
{
    clearData();
}

FGBase* FGCTTableDynamics::clone() const
{
    OFunique_ptr<FGCTTableDynamics> copy(new FGCTTableDynamics);
    if (copy)
    {
        if (DcmIODUtil::copyContainer(this->m_Items, copy->m_Items).good())
        {
            return copy.release();
        }
    }
    return OFnullptr;
}

void FGCTTableDynamics::clearData()
{
    DcmIODUtil::freeContainer(m_Items);
    m_Items.clear();
}

OFCondition FGCTTableDynamics::check() const
{
    // Maybe add checks later
    return EC_Normal;
}

/// Read CT Table Dynamics Sequence from given item
OFCondition FGCTTableDynamics::read(DcmItem& item)
{
    clearData();
    DcmItem* seqItem       = NULL;
    unsigned long numItems = 0;
    OFCondition result     = getNumItemsFromFGSequence(item, DCM_CTTableDynamicsSequence, numItems);
    if (result.bad())
        return result;

    for (unsigned long num = 0; num < numItems; num++)
    {
        if (getItemFromFGSequence(item, DCM_CTTableDynamicsSequence, num, seqItem).good())
        {
            FGCTTableDynamicsItem* newItem = new FGCTTableDynamicsItem();
            newItem->read(*seqItem);
            m_Items.push_back(newItem);
        }
    }
    return EC_Normal;
}

/// Writes single CT Table Dynamics Sequence into given item
OFCondition FGCTTableDynamics::write(DcmItem& item)
{
    OFCondition result = check();
    if (result.good())
    {
        DcmIODUtil::writeSubSequence(
            result, DCM_CTTableDynamicsSequence, m_Items, item, "1-n", "2", "CTTableDynamicsMacro");
    }
    return result;
}

int FGCTTableDynamics::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result != 0)
        return result;

    const FGCTTableDynamics* myRhs = OFstatic_cast(const FGCTTableDynamics*, &rhs);
    if (!myRhs)
        return -1;

    // Compare all items
    return DcmIODUtil::compareContainer(this->m_Items, myRhs->m_Items);
}

OFVector<FGCTTableDynamics::FGCTTableDynamicsItem*>& FGCTTableDynamics::getCTTableDynamicsItems()
{
    return m_Items;
}

// --------------------------------------------------------------------------

// Constructor
FGCTTableDynamics::FGCTTableDynamicsItem::FGCTTableDynamicsItem()
    : m_TableSpeed(DCM_TableSpeed)
    , m_TableFeedPerRotation(DCM_TableFeedPerRotation)
    , m_SpiralPitchFactor(DCM_SpiralPitchFactor)
{
}

FGCTTableDynamics::FGCTTableDynamicsItem::~FGCTTableDynamicsItem()
{
    // nothing to do
}

FGCTTableDynamics::FGCTTableDynamicsItem* FGCTTableDynamics::FGCTTableDynamicsItem::clone() const
{
    FGCTTableDynamicsItem* copy = new FGCTTableDynamicsItem();
    if (copy)
    {
        copy->m_TableSpeed           = this->m_TableSpeed;
        copy->m_TableFeedPerRotation = this->m_TableFeedPerRotation;
        copy->m_SpiralPitchFactor    = this->m_SpiralPitchFactor;
    }
    return copy;
}

void FGCTTableDynamics::FGCTTableDynamicsItem::clearData()
{
    m_TableSpeed.clear();
    m_TableFeedPerRotation.clear();
    m_SpiralPitchFactor.clear();
}

OFCondition FGCTTableDynamics::FGCTTableDynamicsItem::check() const
{
    // Maybe add checks later
    return EC_Normal;
}

/// Read CT Table Dynamics Sequence from given item
OFCondition FGCTTableDynamics::FGCTTableDynamicsItem::read(DcmItem& item)
{
    clearData();

    DcmIODUtil::getAndCheckElementFromDataset(item, m_TableSpeed, "1", "1C", "CTTableDynamicsMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_TableFeedPerRotation, "1", "1C", "CTTableDynamicsMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_SpiralPitchFactor, "1", "1C", "CTTableDynamicsMacro");
    return EC_Normal;
}

/// Writes single CT Table Dynamics Sequence into given item
OFCondition FGCTTableDynamics::FGCTTableDynamicsItem::write(DcmItem& item)
{
    // --- write frame content macro attributes ---
    OFCondition result;
    DcmIODUtil::copyElementToDataset(result, item, m_TableSpeed, "1", "1C", "CTTableDynamicsMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_TableFeedPerRotation, "1", "1C", "CTTableDynamicsMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_SpiralPitchFactor, "1", "1C", "CTTableDynamicsMacro");
    return result;
}

int FGCTTableDynamics::FGCTTableDynamicsItem::compare(const FGCTTableDynamics::FGCTTableDynamicsItem& rhs) const
{
    int result                         = 0;
    const FGCTTableDynamicsItem* myRhs = OFstatic_cast(const FGCTTableDynamicsItem*, &rhs);
    if (!myRhs)
        return -1;

    // Compare all elements
    result = m_TableSpeed.compare(myRhs->m_TableSpeed);
    if (result == 0)
        result = m_TableFeedPerRotation.compare(myRhs->m_TableFeedPerRotation);
    if (result == 0)
        result = m_SpiralPitchFactor.compare(myRhs->m_SpiralPitchFactor);

    return result;
}

// --- get() functionality ---

OFCondition FGCTTableDynamics::FGCTTableDynamicsItem::getTableSpeed(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_TableSpeed, value, pos);
}

OFCondition FGCTTableDynamics::FGCTTableDynamicsItem::getTableSpeed(Float64& value, const unsigned long pos)
{
    return m_TableSpeed.getFloat64(value, pos);
}

OFCondition FGCTTableDynamics::FGCTTableDynamicsItem::getSpiralPitchFactor(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_SpiralPitchFactor, value, pos);
}

OFCondition FGCTTableDynamics::FGCTTableDynamicsItem::getSpiralPitchFactor(Float64& value, const unsigned long pos)
{
    return m_SpiralPitchFactor.getFloat64(value, pos);
}

OFCondition FGCTTableDynamics::FGCTTableDynamicsItem::getTableFeedPerRotation(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_TableFeedPerRotation, value, pos);
}

OFCondition FGCTTableDynamics::FGCTTableDynamicsItem::getTableFeedPerRotation(Float64& value, const unsigned long pos)
{
    return m_TableFeedPerRotation.getFloat64(value, pos);
}

// --- set() functionality ---

OFCondition FGCTTableDynamics::FGCTTableDynamicsItem::setTableSpeed(const Float64& value, const OFBool /* checkValue */)
{
    return m_TableSpeed.putFloat64(value);
}

OFCondition FGCTTableDynamics::FGCTTableDynamicsItem::setTableFeedPerRotation(const Float64& value,
                                                                              const OFBool /* checkValue */)
{
    return m_TableFeedPerRotation.putFloat64(value);
}

OFCondition FGCTTableDynamics::FGCTTableDynamicsItem::setSpiralPitchFactor(const Float64& value,
                                                                           const OFBool /* checkValue */)
{
    return m_SpiralPitchFactor.putFloat64(value);
}
