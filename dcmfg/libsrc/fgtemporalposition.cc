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
 *  Module: dcmfg
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class for managing the Temporal Position
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmfg/fgtemporalposition.h"
#include "dcmtk/dcmiod/iodutil.h"

FGTemporalPosition::FGTemporalPosition()
    : FGBase(DcmFGTypes::EFG_TEMPORALPOSITION)
    , m_TemporalPositionTimeOffset(DCM_TemporalPositionTimeOffset)
{
}

FGTemporalPosition::~FGTemporalPosition()
{
}

void FGTemporalPosition::clearData()
{
    m_TemporalPositionTimeOffset.clear();
}

OFCondition FGTemporalPosition::check() const
{
    return EC_Normal;
}

FGBase* FGTemporalPosition::clone() const
{
    if (FGTemporalPosition* copy = new FGTemporalPosition)
    {
        copy->m_TemporalPositionTimeOffset = m_TemporalPositionTimeOffset;
        return copy;
    }
    return OFnullptr;
}

OFCondition FGTemporalPosition::read(DcmItem& item)
{
    clearData();

    DcmItem* seqItem;
    OFCondition result;

    seqItem = OFnullptr;
    result  = getItemFromFGSequence(item, DCM_TemporalPositionSequence, 0, seqItem);
    if (result.bad())
        return result;
    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_TemporalPositionTimeOffset, "1", "1", "Temporal Position");

    return EC_Normal;
}

OFCondition FGTemporalPosition::write(DcmItem& item)
{
    DcmItem* seqItem;
    OFCondition result = check();
    if (result.bad())
        return result;

    seqItem = OFnullptr;
    result  = createNewFGSequence(item, DCM_TemporalPositionSequence, 0, seqItem);
    if (result.bad())
        return result;
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_TemporalPositionTimeOffset, "1", "1", "Temporal Position");

    return result;
}

int FGTemporalPosition::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result == 0)
    {
        const FGTemporalPosition* myRhs = OFstatic_cast(const FGTemporalPosition*, &rhs);

        // Compare all elements
        result = m_TemporalPositionTimeOffset.compare(myRhs->m_TemporalPositionTimeOffset);
    }

    return result;
}

OFCondition FGTemporalPosition::getTemporalPositionTimeOffset(Float64& value, const unsigned long pos) const
{
    return DcmIODUtil::getFloat64ValueFromElement(m_TemporalPositionTimeOffset, value, pos);
}

OFCondition FGTemporalPosition::setTemporalPositionTimeOffset(const Float64& value, const OFBool checkValue)
{
    // Maybe add checks later.
    (void)checkValue;
    return m_TemporalPositionTimeOffset.putFloat64(value);
}
