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
 *  Purpose: Class for managing the CT Geometry Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmfg/fgctgeometry.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/dcmiod/iodutil.h"

// Constructor
FGCTGeometry::FGCTGeometryItem::FGCTGeometryItem()
    : m_ReferencedPathIndex(DCM_ReferencedPathIndex)
    , m_DistanceSourceToDetector(DCM_DistanceSourceToDetector)
    , m_DistanceSourceToDataCollectionCenter(DCM_DistanceSourceToDataCollectionCenter)
{
}

FGCTGeometry::FGCTGeometryItem::~FGCTGeometryItem()
{
    // nothing to do
}

FGCTGeometry::FGCTGeometryItem* FGCTGeometry::FGCTGeometryItem::clone() const
{
    FGCTGeometryItem* copy = new FGCTGeometryItem();
    if (copy)
    {
        copy->m_ReferencedPathIndex                  = this->m_ReferencedPathIndex;
        copy->m_DistanceSourceToDetector             = this->m_DistanceSourceToDetector;
        copy->m_DistanceSourceToDataCollectionCenter = this->m_DistanceSourceToDataCollectionCenter;
    }
    return copy;
}

void FGCTGeometry::FGCTGeometryItem::clearData()
{
    m_ReferencedPathIndex.clear();
    m_DistanceSourceToDetector.clear();
    m_DistanceSourceToDataCollectionCenter.clear();
}

OFCondition FGCTGeometry::FGCTGeometryItem::check() const
{
    // Maybe add checks later
    return EC_Normal;
}

/// Read CT Geometry Sequence from given item
OFCondition FGCTGeometry::FGCTGeometryItem::read(DcmItem& item)
{
    clearData();

    DcmIODUtil::getAndCheckElementFromDataset(item, m_ReferencedPathIndex, "1-n", "1C", "CTGeometryMacro");
    DcmIODUtil::getAndCheckElementFromDataset(item, m_DistanceSourceToDetector, "1", "1C", "CTGeometryMacro");
    DcmIODUtil::getAndCheckElementFromDataset(
        item, m_DistanceSourceToDataCollectionCenter, "1", "1C", "CTGeometryMacro");
    return EC_Normal;
}

/// Writes single CT Geometry Sequence into given item
OFCondition FGCTGeometry::FGCTGeometryItem::write(DcmItem& item)
{
    OFCondition result;

    // --- write frame content macro attributes ---
    DcmIODUtil::copyElementToDataset(result, item, m_ReferencedPathIndex, "1-n", "1C", "CTGeometryMacro");
    DcmIODUtil::copyElementToDataset(result, item, m_DistanceSourceToDetector, "1", "1C", "CTGeometryMacro");
    DcmIODUtil::copyElementToDataset(
        result, item, m_DistanceSourceToDataCollectionCenter, "1", "1C", "CTGeometryMacro");
    return result;
}

int FGCTGeometry::FGCTGeometryItem::compare(const FGCTGeometry::FGCTGeometryItem& rhs) const
{
    const FGCTGeometry::FGCTGeometryItem* myRhs = OFstatic_cast(const FGCTGeometry::FGCTGeometryItem*, &rhs);
    if (!myRhs)
        return -1;

    // Compare all elements
    int result = m_ReferencedPathIndex.compare(myRhs->m_ReferencedPathIndex);
    if (result == 0)
        result = m_DistanceSourceToDetector.compare(myRhs->m_DistanceSourceToDetector);
    if (result == 0)
        result = m_DistanceSourceToDataCollectionCenter.compare(myRhs->m_DistanceSourceToDataCollectionCenter);

    return result;
}

// --- get() functionality ---

OFCondition FGCTGeometry::FGCTGeometryItem::getReferencedPathIndex(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_ReferencedPathIndex, value, pos);
}

OFCondition FGCTGeometry::FGCTGeometryItem::getReferencedPathIndex(OFVector<Uint16>& values)
{
    return DcmIODUtil::getUint16ValuesFromElement(m_ReferencedPathIndex, values);
}

OFCondition FGCTGeometry::FGCTGeometryItem::getDistanceSourceToDataCollectionCenter(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_DistanceSourceToDataCollectionCenter, value, pos);
}

OFCondition FGCTGeometry::FGCTGeometryItem::getDistanceSourceToDataCollectionCenter(OFVector<Float64>& values)
{
    return DcmIODUtil::getFloat64ValuesFromElement(m_DistanceSourceToDataCollectionCenter, values);
}

OFCondition FGCTGeometry::FGCTGeometryItem::getDistanceSourceToDetector(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_DistanceSourceToDetector, value, pos);
}

OFCondition FGCTGeometry::FGCTGeometryItem::getDistanceSourceToDetector(Float64& value, const unsigned long pos)
{
    return m_DistanceSourceToDetector.getFloat64(value, pos);
}

// --- set() functionality ---

OFCondition FGCTGeometry::FGCTGeometryItem::setReferencedPathIndex(const OFVector<Uint16>& values,
                                                                   const bool checkValue)
{
    return DcmIODUtil::setUint16ValuesOnElement(m_ReferencedPathIndex, values, "1-n", checkValue);
}

OFCondition FGCTGeometry::FGCTGeometryItem::setDistanceSourceToDetector(const Float64 value, const bool checkValue)
{
    (void)checkValue;
    return m_DistanceSourceToDetector.putFloat64(value, 0);
}

OFCondition FGCTGeometry::FGCTGeometryItem::setDistanceSourceToDataCollectionCenter(const Float64 value,
                                                                                    const bool checkValue)
{
    (void)checkValue;
    return m_DistanceSourceToDataCollectionCenter.putFloat64(value, 0);
}

// ------------------------

// Constructor
FGCTGeometry::FGCTGeometry()
    : FGBase(DcmFGTypes::EFG_CTGEOMETRY)
    , m_Items()
{
}

FGCTGeometry::~FGCTGeometry()
{
    clearData();
}

FGBase* FGCTGeometry::clone() const
{
    OFunique_ptr<FGCTGeometry> copy(new FGCTGeometry);
    if (copy)
    {
        if (DcmIODUtil::copyContainer(this->m_Items, copy->m_Items).good())
        {
            return copy.release();
        }
    }
    return OFnullptr;
}

void FGCTGeometry::clearData()
{
    DcmIODUtil::freeContainer(m_Items);
    m_Items.clear();
}

OFCondition FGCTGeometry::check() const
{
    // Maybe add checks later
    return EC_Normal;
}

/// Read CT Geometry Sequence from given item
OFCondition FGCTGeometry::read(DcmItem& item)
{
    clearData();
    DcmItem* seqItem       = NULL;
    unsigned long numItems = 0;
    OFCondition result     = getNumItemsFromFGSequence(item, DCM_CTGeometrySequence, numItems);
    if (result.bad())
        return result;

    for (unsigned long num = 0; num < numItems; num++)
    {
        if (getItemFromFGSequence(item, DCM_CTGeometrySequence, num, seqItem).good())
        {
            FGCTGeometryItem* newItem = new FGCTGeometryItem();
            newItem->read(*seqItem);
            m_Items.push_back(newItem);
        }
    }
    return EC_Normal;
}

/// Writes single CT Geometry Sequence into given item
OFCondition FGCTGeometry::write(DcmItem& item)
{
    OFCondition result = check();
    if (result.good())
    {
        DcmIODUtil::writeSubSequence(result, DCM_CTGeometrySequence, m_Items, item, "1-n", "2", "CTGeometryMacro");
    }
    return result;
}

int FGCTGeometry::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result != 0)
        return result;

    const FGCTGeometry* myRhs = OFstatic_cast(const FGCTGeometry*, &rhs);
    if (!myRhs)
        return -1;

    // Compare all items
    return DcmIODUtil::compareContainer(this->m_Items, myRhs->m_Items);
}

OFVector<FGCTGeometry::FGCTGeometryItem*>& FGCTGeometry::getCTGeometryItems()
{
    return m_Items;
}
