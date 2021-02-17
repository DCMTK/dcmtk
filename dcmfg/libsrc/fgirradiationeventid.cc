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
 *  Purpose: Class for managing the Irradiation Event Identification Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmfg/fgirradiationeventid.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/dcmiod/iodutil.h"

// Constructor
FGIrradiationEventIdentification::FGIrradiationEventIdentification()
    : FGBase(DcmFGTypes::EFG_IRRADIATIONEVENTIDENT)
    , m_IrraditionEventUID(DCM_IrradiationEventUID)
{
}

FGIrradiationEventIdentification::~FGIrradiationEventIdentification()
{
    // nothing to do
}

FGBase* FGIrradiationEventIdentification::clone() const
{
    FGIrradiationEventIdentification* copy = new FGIrradiationEventIdentification();
    if (copy)
    {
        copy->m_IrraditionEventUID = this->m_IrraditionEventUID;
    }
    return copy;
}

void FGIrradiationEventIdentification::clearData()
{
    m_IrraditionEventUID.clear();
}

OFCondition FGIrradiationEventIdentification::check() const
{
    // Maybe add checks later
    return EC_Normal;
}

// Read Irradiation Event Identification Sequence from given item
OFCondition FGIrradiationEventIdentification::read(DcmItem& item)
{
    clearData();

    DcmItem* seqItem   = NULL;
    OFCondition result = getItemFromFGSequence(item, DCM_IrradiationEventIdentificationSequence, 0, seqItem);
    if (result.bad())
        return result;

    DcmIODUtil::getAndCheckElementFromDataset(
        *seqItem, m_IrraditionEventUID, "1", "", "IrradiationEventIdentificationMacro");

    return EC_Normal;
}

// Writes single Irradiation Event Identification Sequence into given item
OFCondition FGIrradiationEventIdentification::write(DcmItem& item)
{
    DcmItem* seqItem   = NULL;
    OFCondition result = createNewFGSequence(item, DCM_IrradiationEventIdentificationSequence, 0, seqItem);
    if (result.bad())
        return result;

    // --- write Irradiation Event Identification Sequence attributes ---
    DcmIODUtil::copyElementToDataset(
        result, *seqItem, m_IrraditionEventUID, "1", "1", "IrradiationEventIdentificationMacro");
    return result;
}

int FGIrradiationEventIdentification::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result != 0)
        return result;

    const FGIrradiationEventIdentification* myRhs = OFstatic_cast(const FGIrradiationEventIdentification*, &rhs);
    if (!myRhs)
        return -1;

    // Compare all elements
    result = m_IrraditionEventUID.compare(myRhs->m_IrraditionEventUID);
    return result;
}

// --- get() functionality ---

OFCondition FGIrradiationEventIdentification::getIrradiationEventUID(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_IrraditionEventUID, value, pos);
}

// --- set() functionality ---

OFCondition FGIrradiationEventIdentification::setIrradiationEventUID(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_IrraditionEventUID.putOFStringArray(value);
    return result;
}
