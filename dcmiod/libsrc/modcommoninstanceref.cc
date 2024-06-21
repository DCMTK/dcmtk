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
 *  Module: dcmiod
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class for managing the Common Instance Reference Module
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/modcommoninstanceref.h"
#include "dcmtk/dcmiod/iodutil.h" // for static helpers
#include "dcmtk/dcmdata/dcvrui.h"

const OFString IODCommonInstanceReferenceModule::m_ComponentName = "CommonInstanceReferenceModule";
const OFString IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::m_ComponentName
    = "StudiesContainingOtherReferencedInstancesSequence";

IODCommonInstanceReferenceModule::IODCommonInstanceReferenceModule(OFshared_ptr<DcmItem> item,
                                                                   OFshared_ptr<IODRules> rules)
    : IODModule(item, rules)
    , m_ReferenceSeriesItems()
    , m_StudiesContainingOtherReferencedInstancesSequence()
{
    resetRules();
}

IODCommonInstanceReferenceModule::IODCommonInstanceReferenceModule()
    : IODModule()
    , m_ReferenceSeriesItems()
    , m_StudiesContainingOtherReferencedInstancesSequence()
{
    resetRules();
}

IODCommonInstanceReferenceModule::~IODCommonInstanceReferenceModule()
{
    freeMemory();
}

OFString IODCommonInstanceReferenceModule::getName() const
{
    return m_ComponentName;
}

OFVector<IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem*>&
IODCommonInstanceReferenceModule::getReferencedSeriesItems()
{
    return m_ReferenceSeriesItems;
}

OFVector<IODCommonInstanceReferenceModule::StudiesOtherInstancesItem*>&
IODCommonInstanceReferenceModule::getStudiesContainingOtherReferences()
{
    return m_StudiesContainingOtherReferencedInstancesSequence;
}

void IODCommonInstanceReferenceModule::clearData()
{
    freeMemory();
}

OFCondition IODCommonInstanceReferenceModule::read(DcmItem& source, const OFBool clearOldData)
{
    if (clearOldData)
        clearData();

    DcmIODUtil::readSubSequence(
        source, DCM_ReferencedSeriesSequence, m_ReferenceSeriesItems, m_Rules->getByTag(DCM_ReferencedSeriesSequence));
    DcmIODUtil::readSubSequence(source,
                                DCM_StudiesContainingOtherReferencedInstancesSequence,
                                m_StudiesContainingOtherReferencedInstancesSequence,
                                m_Rules->getByTag(DCM_StudiesContainingOtherReferencedInstancesSequence));

    return EC_Normal;
}

OFCondition IODCommonInstanceReferenceModule::write(DcmItem& destination)
{
    OFCondition result = EC_Normal;

    DcmIODUtil::writeSubSequence<OFVector<IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem*> >(
        result,
        DCM_ReferencedSeriesSequence,
        m_ReferenceSeriesItems,
        *m_Item,
        m_Rules->getByTag(DCM_ReferencedSeriesSequence));
    DcmIODUtil::writeSubSequence<OFVector<StudiesOtherInstancesItem*> >(
        result,
        DCM_StudiesContainingOtherReferencedInstancesSequence,
        m_StudiesContainingOtherReferencedInstancesSequence,
        *m_Item,
        m_Rules->getByTag(DCM_StudiesContainingOtherReferencedInstancesSequence));
    if (result.good())
        result = IODModule::write(destination);
    return result;
}

size_t IODCommonInstanceReferenceModule::addReferences(const IODReferences& references,
                                                       const OFString& studyInstanceUID,
                                                       const OFBool clearOldData)
{
    if (clearOldData)
    {
        clearData();
    }
    OFString ourStudy = studyInstanceUID;
    if (ourStudy.empty())
    {
        m_Item->findAndGetOFString(DCM_StudyInstanceUID, ourStudy);
        if (ourStudy.empty())
        {
            DCMIOD_ERROR("Could not add references: No Study Instance UID specified for \"this\" object");
            return 0;
        }
    }

    const OFVector<IODReference*> refs          = references.get();
    OFVector<IODReference*>::const_iterator ref = refs.begin();
    size_t count                                = 0;
    while (ref != refs.end())
    {
        OFCondition result;
        // If reference belongs into "this" study, add it to Referenced Series Sequence
        OFString refStudy = (*ref)->m_StudyInstanceUID;
        if (refStudy == ourStudy)
        {
            result = addSeriesReference(m_ReferenceSeriesItems, **ref);
        }
        else
        {
            // Reference lies outside of "this" study, put it into Studies Containing
            // Other Referenced Instances Sequence
            OFVector<StudiesOtherInstancesItem*>::iterator it
                = m_StudiesContainingOtherReferencedInstancesSequence.begin();
            while (it != m_StudiesContainingOtherReferencedInstancesSequence.end())
            {
                OFString studyEntry;
                (*it)->getStudyInstanceUID(studyEntry);
                if (studyEntry == refStudy)
                {
                    result = addSeriesReference(
                        (*it)->getReferencedSeriesAndInstanceReferences().getReferencedSeriesItems(), **ref);
                    break;
                }
                else
                {
                    it++;
                }
            }
            // We did not find an entry for this study, add new one
            if (it == m_StudiesContainingOtherReferencedInstancesSequence.end())
            {
                StudiesOtherInstancesItem* newItem = new StudiesOtherInstancesItem();
                if (newItem)
                {
                    result = newItem->setStudyInstanceUID(refStudy);
                    if (result.good())
                    {
                        result = addSeriesReference(
                            newItem->getReferencedSeriesAndInstanceReferences().getReferencedSeriesItems(), **ref);
                    }
                    if (result.good())
                    {
                        m_StudiesContainingOtherReferencedInstancesSequence.push_back(newItem);
                    }
                    else
                    {
                        delete newItem;
                    }
                }
                else
                {
                    DCMIOD_ERROR("Memory exhausted while adding references to Common Instance Reference Module");
                    return count;
                }
            }
        }
        if (result.good())
        {
            count++;
        }
        ref++;
    }
    return count;
}

void IODCommonInstanceReferenceModule::resetRules()
{
    // Parameters for Rule are tag, VM, type (1,1C,2,2C,3), module name and logical IOD level
    m_Rules->addRule(new IODRule(DCM_ReferencedSeriesSequence, "1-n", "1C", getName(), DcmIODTypes::IE_INSTANCE),
                     OFTrue);
    m_Rules->addRule(
        new IODRule(
            DCM_StudiesContainingOtherReferencedInstancesSequence, "1-n", "1C", getName(), DcmIODTypes::IE_INSTANCE),
        OFTrue);
}

OFCondition IODCommonInstanceReferenceModule::addSeriesReference(
    OFVector<IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem*>& container, const IODReference& ref)
{
    OFCondition result;
    OFVector<IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem*>::iterator series = container.begin();
    while (series != container.end())
    {
        OFString s;
        (*series)->getSeriesInstanceUID(s);
        if (s == ref.m_SeriesInstanceUID)
        {
            // There is already an entry for this series
            result = (*series)->addReference(ref.m_SOPClassUID, ref.m_SOPInstanceUID);
            if (result.good())
            {
                return EC_Normal;
            }
            else
            {
                DCMIOD_ERROR("Could not add reference to Common Instance Reference Module: " << ref.toString());
                return IOD_EC_InvalidElementValue;
            }
        }
        series++;
    }
    // If we do not have such a series Referenced Series Sequence, add it
    if (series == container.end())
    {
        IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem* newseries
            = new IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem();
        if (!newseries)
        {
            return EC_MemoryExhausted;
        }
        result = newseries->setSeriesInstanceUID(ref.m_SeriesInstanceUID);
        if (result.good())
            result = newseries->addReference(ref.m_SOPClassUID, ref.m_SOPInstanceUID);
        if (result.good())
        {
            container.push_back(newseries);
        }
        else
        {
            DCMIOD_ERROR("Could not add reference to Common Instance Reference Module: " << ref.toString());
        }
    }
    return result;
}

// -- IODCommonInstanceReferenceModule::StudiesOtherInstancesItem --

IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::StudiesOtherInstancesItem(OFshared_ptr<DcmItem> item,
                                                                                       OFshared_ptr<IODRules> rules,
                                                                                       IODComponent* parent)
    : IODComponent(item, rules, parent)
    , m_ReferencedSeriesAndInstance()
{
    // reset element rules
    resetRules();
}

IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::StudiesOtherInstancesItem(IODComponent* parent)
    : IODComponent(parent)
    , m_ReferencedSeriesAndInstance()
{
    // reset element rules
    resetRules();
}

IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::~StudiesOtherInstancesItem()
{
    clearData();
}

OFString IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::getName() const
{
    return m_ComponentName;
}

void IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::clearData()
{
    m_ReferencedSeriesAndInstance.clearData();
    IODComponent::clearData();
}

OFCondition IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::read(DcmItem& source,
                                                                              const OFBool clearOldData)
{
    if (clearOldData)
        clearData();

    IODComponent::read(source, clearOldData);
    m_ReferencedSeriesAndInstance.read(source, clearOldData);
    return EC_Normal;
}

OFCondition IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::write(DcmItem& destination)
{
    OFCondition result = EC_Normal;
    result             = IODComponent::write(destination);

    if (result.good())
        result = m_ReferencedSeriesAndInstance.write(destination);

    return result;
}

void IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::resetRules()
{
    // Parameters for Rule are tag, VM, type (1,1C,2,2C,3), module name and logical IOD level
    m_Rules->addRule(new IODRule(DCM_StudyInstanceUID, "1", "1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
}

IODSeriesAndInstanceReferenceMacro&
IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::getReferencedSeriesAndInstanceReferences()
{
    return m_ReferencedSeriesAndInstance;
}

OFCondition
IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::getStudyInstanceUID(OFString& value,
                                                                                 const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_StudyInstanceUID, *m_Item, value, pos);
}

OFCondition IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::setStudyInstanceUID(const OFString& value,
                                                                                             const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_StudyInstanceUID, value);
    return result;
}

void IODCommonInstanceReferenceModule::freeMemory()
{
    DcmIODUtil::freeContainer(m_StudiesContainingOtherReferencedInstancesSequence);
    DcmIODUtil::freeContainer(m_ReferenceSeriesItems);
}
