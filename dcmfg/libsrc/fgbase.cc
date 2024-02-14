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
 *  Purpose: Base class(es) for functional groups
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/dcmdata/dcsequen.h"

FGBase::FGBase(const DcmFGTypes::E_FGType fgType)
    : m_fgType(fgType)
{
}

FGBase::~FGBase()
{
}

void FGBase::clearData()
{
}

DcmFGTypes::E_FGType FGBase::getType() const
{
    return m_fgType;
}

DcmFGTypes::E_FGSharedType FGBase::getSharedType() const
{
    return DcmFGTypes::EFGS_UNKNOWN;
}

int FGBase::compare(const FGBase& rhs) const
{
    // Check for equality from FGBase' point of view

    // Same objects?
    if (this == &rhs)
        return 0;

    // Type identical?
    if (this->getType() != rhs.getType())
        return -1;

    // This is all we can do
    return 0;
}

OFCondition FGBase::getItemFromFGSequence(DcmItem& source,
                                          const DcmTagKey& seqKey,
                                          const long unsigned int itemNum,
                                          DcmItem*& result)
{
    result                  = NULL;
    DcmSequenceOfItems* seq = NULL;
    if (source.findAndGetSequence(seqKey, seq).bad())
    {
        DCMFG_ERROR("Functional Group Sequence does not exist: " << seqKey << " (" << DcmFGTypes::tagKey2FGType(seqKey)
                                                                 << ")");
        return EC_TagNotFound;
    }
    result = seq->getItem(itemNum);
    if (result == NULL)
    {
        DCMFG_DEBUG("Functional Group Sequence " << seqKey << " (" << DcmFGTypes::tagKey2FGType(seqKey)
                                                 << ") does not have " << itemNum - 1 << " items");
        return FG_EC_NotEnoughItems;
    }
    return EC_Normal;
}

OFCondition FGBase::getNumItemsFromFGSequence(DcmItem& source, const DcmTagKey& seqKey, unsigned long& result)
{
    result                  = 0;
    DcmSequenceOfItems* seq = NULL;
    if (source.findAndGetSequence(seqKey, seq).bad())
    {
        DCMFG_ERROR("Functional Group Sequence does not exist: " << seqKey << " (" << DcmFGTypes::tagKey2FGType(seqKey)
                                                                 << ")");
        return EC_TagNotFound;
    }
    result = seq->card();
    return EC_Normal;
}

OFCondition FGBase::createNewFGSequence(DcmItem& destination,
                                        const DcmTagKey& seqKey,
                                        const unsigned long numItems,
                                        DcmItem*& firstItem)
{
    if (numItems > OFstatic_cast(unsigned long, OFnumeric_limits<signed long>::max()))
        return EC_IllegalParameter;

    firstItem          = NULL;
    OFCondition result = destination.insertEmptyElement(seqKey, OFTrue /* replace old */);
    if (result.bad())
    {
        DCMFG_ERROR("Could not create Functional Group with sequence " << seqKey << " ("
                                                                       << DcmFGTypes::tagKey2FGType(seqKey) << ")");
        return FG_EC_CouldNotInsertFG;
    }
    DcmItem* dummy = NULL;
    result         = destination.findOrCreateSequenceItem(seqKey, dummy, OFstatic_cast(signed long, numItems));
    if (result.bad())
    {
        // clean up
        destination.findAndDeleteElement(seqKey);
        DCMFG_ERROR("Could not create " << numItems << " items in Functional Group with sequence " << seqKey << " ("
                                        << DcmFGTypes::tagKey2FGType(seqKey) << ")");
        return FG_EC_CouldNotInsertFG;
    }
    destination.findOrCreateSequenceItem(seqKey, firstItem, 0);
    return EC_Normal;
}

// ------------------ class FGUnknown ------------------------------------

FGUnknown::FGUnknown(const DcmTagKey& seqStartTag, const DcmFGTypes::E_FGSharedType sharedType)
    : FGBase(DcmFGTypes::EFG_UNKNOWN)
    , m_seqStartTag(seqStartTag)
    , m_fgSequence(NULL)
    , m_sharedType(sharedType)
{
}

FGUnknown::FGUnknown(const FGUnknown& rhs)
    : FGBase(DcmFGTypes::EFG_UNKNOWN)
    , m_seqStartTag(rhs.m_seqStartTag)
    , m_fgSequence(NULL)
    , m_sharedType(rhs.m_sharedType)
{
    if (rhs.m_fgSequence)
    {
        this->m_fgSequence = OFstatic_cast(DcmSequenceOfItems*, rhs.m_fgSequence->clone());
    }
}

FGUnknown& FGUnknown::operator=(const FGUnknown& rhs)
{
    m_seqStartTag = rhs.m_seqStartTag;
    m_fgSequence  = OFstatic_cast(DcmSequenceOfItems*, rhs.m_fgSequence->clone());
    m_sharedType  = rhs.m_sharedType;
    return *this;
}

void FGUnknown::clearData()
{
    if (m_fgSequence != NULL)
    {
        delete m_fgSequence;
        m_fgSequence = NULL;
    }
}

OFCondition FGUnknown::read(DcmItem& item)
{
    // clear old data
    clearData();
    OFCondition result = item.findAndGetSequence(
        m_seqStartTag, m_fgSequence, OFFalse /* no depth search */, OFTrue /* create copy! */);
    if (result.bad())
    {
        if (m_fgSequence != NULL)
            delete m_fgSequence;
        m_fgSequence = NULL;
    }
    return result;
}

OFCondition FGUnknown::write(DcmItem& item)
{
    return item.insert(new DcmSequenceOfItems(*m_fgSequence), OFTrue /* replace old! */);
}

OFCondition FGUnknown::check() const
{
    return EC_Normal;
}

int FGUnknown::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result == 0)
    {
        const FGUnknown* myRhs = OFstatic_cast(const FGUnknown*, &rhs);

        // Compare all elements
        if (m_seqStartTag == myRhs->m_seqStartTag)
        {
            if (m_fgSequence && myRhs->m_fgSequence)
            {
                result = m_fgSequence->compare(*(myRhs->m_fgSequence));
            }
            else if (!m_fgSequence && !myRhs->m_fgSequence) // if both are not set, objects are still equal
            {
                result = 0;
            }
            else if (m_fgSequence)
            {
                result = 1; // this object is bigger (more information)
            }
            else if (myRhs->m_fgSequence)
            {
                result = -1; // rhs object is bigger (more information)
            }
        }
        else
        {
            result = (m_seqStartTag < myRhs->m_seqStartTag) ? -1 : 1;
        }
    }

    return result;
}

FGUnknown::~FGUnknown()
{
    clearData();
}

FGBase* FGUnknown::clone() const
{
    //     FGUnknown* copy = new FGUnknown(this->m_seqStartTag);
    //     if (copy)
    //     {
    //         if (this->m_fgSequence != NULL)
    //             *(copy->m_fgSequence) = *(this->m_fgSequence);
    //     }
    //     return copy;
    return new FGUnknown(*this);
}
