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
 *  Purpose: Class for managing the CT Position Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmfg/fgctposition.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/dcmiod/iodutil.h"

// Constructor
FGCTPosition::FGCTPosition()
    : FGBase(DcmFGTypes::EFG_CTPOSITION)
    , m_TablePosition(DCM_TablePosition)
    , m_DataCollectionCenterPatient(DCM_DataCollectionCenterPatient)
    , m_ReconstructionTargetCenterPatient(DCM_ReconstructionTargetCenterPatient)
{
}

FGCTPosition::~FGCTPosition()
{
    // nothing to do
}

FGBase* FGCTPosition::clone() const
{
    FGCTPosition* copy = new FGCTPosition();
    if (copy)
    {
        copy->m_TablePosition                     = this->m_TablePosition;
        copy->m_DataCollectionCenterPatient       = this->m_DataCollectionCenterPatient;
        copy->m_ReconstructionTargetCenterPatient = this->m_ReconstructionTargetCenterPatient;
    }
    return copy;
}

void FGCTPosition::clearData()
{
    m_TablePosition.clear();
    m_DataCollectionCenterPatient.clear();
    m_ReconstructionTargetCenterPatient.clear();
}

OFCondition FGCTPosition::check() const
{
    // Maybe add checks later
    return EC_Normal;
}

/// Read CT Position Sequence from given item
OFCondition FGCTPosition::read(DcmItem& item)
{
    clearData();

    DcmItem* seqItem   = NULL;
    OFCondition result = getItemFromFGSequence(item, DCM_CTPositionSequence, 0, seqItem);
    if (result.bad())
        return result;

    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_TablePosition, "1", "1C", "CTPositionMacro");
    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_DataCollectionCenterPatient, "3", "1C", "CTPositionMacro");
    DcmIODUtil::getAndCheckElementFromDataset(
        *seqItem, m_ReconstructionTargetCenterPatient, "3", "1C", "CTPositionMacro");
    return EC_Normal;
}

/// Writes single CT Position Sequence into given item
OFCondition FGCTPosition::write(DcmItem& item)
{
    DcmItem* seqItem   = NULL;
    OFCondition result = createNewFGSequence(item, DCM_CTPositionSequence, 0, seqItem);
    if (result.bad())
        return result;

    // --- write frame content macro attributes ---
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_TablePosition, "1", "1C", "CTPositionMacro");
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_DataCollectionCenterPatient, "3", "1C", "CTPositionMacro");
    DcmIODUtil::copyElementToDataset(
        result, *seqItem, m_ReconstructionTargetCenterPatient, "3", "1C", "CTPositionMacro");
    return result;
}

int FGCTPosition::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result != 0)
        return result;

    const FGCTPosition* myRhs = OFstatic_cast(const FGCTPosition*, &rhs);
    if (!myRhs)
        return -1;

    // Compare all elements
    result = m_TablePosition.compare(myRhs->m_TablePosition);
    if (result == 0)
        result = m_DataCollectionCenterPatient.compare(myRhs->m_DataCollectionCenterPatient);
    if (result == 0)
        result = m_ReconstructionTargetCenterPatient.compare(myRhs->m_ReconstructionTargetCenterPatient);

    return result;
}

// --- get() functionality ---

OFCondition FGCTPosition::getTablePosition(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_TablePosition, value, pos);
}

OFCondition FGCTPosition::getTablePosition(Float64& value, const unsigned long pos)
{
    return m_TablePosition.getFloat64(value, pos);
}

OFCondition FGCTPosition::getDataCollectionCenterPatient(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_DataCollectionCenterPatient, value, pos);
}

OFCondition FGCTPosition::getDataCollectionCenterPatient(OFVector<Float64>& values)
{
    return DcmIODUtil::getFloat64ValuesFromElement(m_DataCollectionCenterPatient, values);
}

OFCondition FGCTPosition::getReconstructionTargetCenterPatient(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_ReconstructionTargetCenterPatient, value, pos);
}

OFCondition FGCTPosition::getReconstructionTargetCenterPatient(OFVector<Float64>& values)
{
    return DcmIODUtil::getFloat64ValuesFromElement(m_ReconstructionTargetCenterPatient, values);
}

// --- set() functionality ---

OFCondition FGCTPosition::setTablePosition(const Float64 value, const OFBool /* checkValue */)
{
    return m_TablePosition.putFloat64(value);
}

OFCondition FGCTPosition::setDataCollectionCenterPatient(const OFVector<Float64>& values, const OFBool checkValue)
{
    return DcmIODUtil::setFloat64ValuesOnElement(m_DataCollectionCenterPatient, values, "3", checkValue);
}

OFCondition FGCTPosition::setReconstructionTargetCenterPatient(const OFVector<Float64>& values, const OFBool checkValue)
{
    return DcmIODUtil::setFloat64ValuesOnElement(m_ReconstructionTargetCenterPatient, values, "3", checkValue);
}
