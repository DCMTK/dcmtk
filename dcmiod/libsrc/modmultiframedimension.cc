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
 *  Purpose: Class for managing the Multi-frame Functional Groups Module
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/modmultiframedimension.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcpath.h" // for private tag reservation checking
#include "dcmtk/dcmdata/dcvrat.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmiod/iodutil.h" // for static helpers

const OFString IODMultiframeDimensionModule::m_ModuleName = "MultiframeDimensionModule";

IODMultiframeDimensionModule::IODMultiframeDimensionModule(OFshared_ptr<DcmItem> data, OFshared_ptr<IODRules> rules)
    : IODModule(data, rules)
    , m_DimensionIndexSequence()
    , m_DimensionOrganizationSequence()
    , m_CheckOnWrite(OFTrue)
{
    // reset element rules
    resetRules();
}

IODMultiframeDimensionModule::IODMultiframeDimensionModule()
    : IODModule()
    , m_DimensionIndexSequence()
    , m_DimensionOrganizationSequence()
{
    resetRules();
}

OFCondition IODMultiframeDimensionModule::addDimensionIndex(const DcmTagKey& dimensionIndexPointer,
                                                            const OFString& dimensionOrganizationUID,
                                                            const DcmTagKey& functionalGroupPointer,
                                                            const OFString& dimensionDescriptionLabel,
                                                            const OFString& dimensionIndexPrivateCreator,
                                                            const OFString& functionalGroupPrivateCreator)
{
    if ((dimensionIndexPointer.isPrivate() && dimensionIndexPrivateCreator.empty())
        || (functionalGroupPointer.isPrivate() && functionalGroupPrivateCreator.empty()))
    {
        DCMIOD_ERROR("Cannot add private Dimension Index without private creator");
        return IOD_EC_MissingAttribute;
    }

    if (dimensionOrganizationUID.empty())
    {
        DCMIOD_ERROR("Cannot add Dimension Index without Dimension Organization UID");
        return IOD_EC_MissingAttribute;
    }

    // Add Dimension Organization by its UID if such organization does not exist yet
    OFVector<DimensionOrganizationItem*>::iterator it = m_DimensionOrganizationSequence.begin();
    while (it != m_DimensionOrganizationSequence.end())
    {
        OFString val;
        (*it)->getDimensionOrganizationUID(val);
        if (val == dimensionOrganizationUID)
        {
            break;
        }
        it++;
    }
    OFCondition result;
    if (it == m_DimensionOrganizationSequence.end())
    {
        DimensionOrganizationItem* item = new DimensionOrganizationItem;
        if (item == NULL)
        {
            return EC_MemoryExhausted;
        }
        result = item->setDimensionOrganizationUID(dimensionOrganizationUID);
        if (result.bad())
            return result;
        m_DimensionOrganizationSequence.push_back(item);
    }

    // Create dimension and add it to this object
    DimensionIndexItem* dim = new DimensionIndexItem();
    if (!dim)
        return EC_MemoryExhausted;

    result = dim->setDimensionOrganizationUID(dimensionOrganizationUID);
    if (result.good())
        result = dim->setFunctionalGroupPointer(functionalGroupPointer);
    if (result.good())
        result = dim->setDimensionIndexPointer(dimensionIndexPointer);
    if (result.good() && !dimensionIndexPrivateCreator.empty())
        dim->setDimensionIndexPrivateCreator(dimensionIndexPrivateCreator);
    if (result.good() && !functionalGroupPrivateCreator.empty())
        dim->setFunctionalGroupPrivateCreator(functionalGroupPrivateCreator);
    if (result.good() && !dimensionDescriptionLabel.empty())
        dim->setDimensionDescriptionLabel(dimensionDescriptionLabel);
    if (result.bad())
    {
        DCMIOD_ERROR("Could not add Dimension Index: Invalid data values");
        delete dim;
    }
    m_DimensionIndexSequence.push_back(dim);

    return result;
}

OFString IODMultiframeDimensionModule::getName() const
{
    return m_ModuleName;
}

void IODMultiframeDimensionModule::clearData()
{
    DcmIODUtil::freeContainer(m_DimensionIndexSequence);
    IODComponent::clearData();
}

OFCondition IODMultiframeDimensionModule::read(DcmItem& source, const OFBool clearOldData)
{
    if (clearOldData)
        clearData();

    IODComponent::read(source, OFFalse /* data already cleared above */);

    DcmIODUtil::readSubSequence(
        source, DCM_DimensionIndexSequence, m_DimensionIndexSequence, m_Rules->getByTag(DCM_DimensionIndexSequence));
    DcmIODUtil::readSubSequence(source,
                                DCM_DimensionOrganizationSequence,
                                m_DimensionOrganizationSequence,
                                m_Rules->getByTag(DCM_DimensionOrganizationSequence));

    return EC_Normal;
}

OFCondition IODMultiframeDimensionModule::write(DcmItem& destination)
{
    OFCondition result = EC_Normal;
    if (m_CheckOnWrite)
    {
        result = checkDimensions();
        if (result.bad())
        {
            return result;
        }
    }

    // Re-create dimension organization data
    createDimensionOrganizationData();
    DcmIODUtil::writeSubSequence<OFVector<DimensionOrganizationItem*> >(
        result,
        DCM_DimensionOrganizationSequence,
        m_DimensionOrganizationSequence,
        *m_Item,
        m_Rules->getByTag(DCM_DimensionOrganizationSequence));

    DcmIODUtil::writeSubSequence<OFVector<DimensionIndexItem*> >(result,
                                                                 DCM_DimensionIndexSequence,
                                                                 m_DimensionIndexSequence,
                                                                 *m_Item,
                                                                 m_Rules->getByTag(DCM_DimensionIndexSequence));

    result = IODComponent::write(destination);
    return result;
}

void IODMultiframeDimensionModule::resetRules()
{
    // Parameters for Rule are tag, VM, type (1,1C,2,2C,3), module name and logical IOD level
    m_Rules->addRule(new IODRule(DCM_DimensionOrganizationSequence, "1-n", "1", getName(), DcmIODTypes::IE_INSTANCE),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_DimensionOrganizationType, "1", "3", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_DimensionIndexSequence, "1-n", "1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
}

OFCondition IODMultiframeDimensionModule::checkDimensions(DcmItem* fgItem)
{
    if (fgItem == NULL)
    {
        fgItem = &getData();
    }

    DcmSequenceOfItems* perFrame = NULL;
    if (fgItem->findAndGetSequence(DCM_PerFrameFunctionalGroupsSequence, perFrame).bad())
    {
        DCMIOD_WARN(
            "Will not check dimension consistency with functional groups (no per-frame functional groups found)");
    }

    OFCondition result;
    OFVector<IODMultiframeDimensionModule::DimensionIndexItem*>::iterator dim = m_DimensionIndexSequence.begin();
    size_t errors                                                             = 0;
    size_t dimNo                                                              = 1;
    while (dim != m_DimensionIndexSequence.end())
    {
        DcmTagKey indexPointer;
        result = (*dim)->getDimensionIndexPointer(indexPointer);
        if (result.bad())
        {
            DCMIOD_ERROR("Dimension " << dimNo << " does not provide a dimension index pointer");
            errors++;
        }
        OFString uid;
        result = (*dim)->getDimensionOrganizationUID(uid);
        if (result.bad())
        {
            DCMIOD_ERROR("Dimension " << dimNo << " does not provide a dimension organization UID");
            errors++;
        }
        DcmTagKey fgPointer;
        result = (*dim)->getFunctionalGroupPointer(fgPointer);
        if (result.bad())
        {
            DCMIOD_ERROR("Dimension " << dimNo << " does not provide a functional group pointer");
            errors++;
        }
        OFString privateCreator;
        if (indexPointer.isPrivate() && (indexPointer != DCM_UndefinedTagKey))
        {
            result = (*dim)->getDimensionIndexPrivateCreator(privateCreator);
            if (result.bad())
            {
                DCMIOD_ERROR("Dimension " << dimNo << " has private index pointer " << indexPointer
                                          << " but private creator is not set");
                errors++;
            }
        }
        OFString privateFGCreator;
        if (fgPointer.isPrivate() && (fgPointer != DCM_UndefinedTagKey))
        {
            result = (*dim)->getFunctionalGroupPrivateCreator(privateFGCreator);
            if (result.bad())
            {
                DCMIOD_ERROR("Dimension " << dimNo << " has private fg group pointer " << fgPointer
                                          << " but private creator is not set");
                errors++;
            }
        }

        if (perFrame && (fgPointer != DCM_UndefinedTagKey) && (indexPointer != DCM_UndefinedTagKey))
        {
            if (!getIndexElement(perFrame, fgPointer, indexPointer, privateFGCreator, privateCreator))
            {
                errors++;
            }
        }

        dim++;
    }

    if (errors > 0)
        return IOD_EC_InvalidDimensions;
    else
        return EC_Normal;
}

DcmElement* IODMultiframeDimensionModule::getIndexElement(DcmSequenceOfItems* perFrameFG,
                                                          const DcmTagKey& fgPointer,
                                                          const DcmTagKey& indexPointer,
                                                          const OFString& fgPrivateCreator,
                                                          const OFString& privateCreator)
{
    if (perFrameFG == NULL)
    {
        DCMIOD_ERROR("Cannot get Dimension Index: No per-frame functional groups are provided");
        return NULL; // nothing to do
    }

    size_t numFrames = perFrameFG->card();
    OFCondition result;
    DcmElement* returnValue = NULL;
    for (size_t count = 0; count < numFrames; count++)
    {
        DcmItem* item = perFrameFG->getItem(OFstatic_cast(unsigned long, count));
        if (item != NULL)
        {
            // Check for the functional group mentioned in dimension
            DcmSequenceOfItems* fgSequence = NULL;
            if (item->findAndGetSequence(fgPointer, fgSequence).good())
            {
                // Check private reservation for fg if necessary
                if (fgPointer.isPrivate())
                {
                    result = DcmPathProcessor::checkPrivateTagReservation(item, fgPointer, fgPrivateCreator);
                    if (result.bad())
                    {
                        DCMIOD_ERROR("Cannot get Dimension Index for frame " << count << ": " << result.text());
                    }
                }
                // Check whether actual fg information exists (single item in related fg sequence)
                DcmItem* fgItem = fgSequence->getItem(0);
                if (fgItem != NULL)
                {
                    if (fgItem->findAndGetElement(indexPointer, returnValue).good())
                    {
                        if (returnValue->getTag().isPrivate())
                        {
                            if (!privateCreator.empty())
                            {
                                result = DcmPathProcessor::checkPrivateTagReservation(
                                    fgItem, indexPointer, privateCreator);
                                if (result.bad())
                                {
                                    DCMIOD_ERROR("Cannot get Dimension Index for frame " << count << ": "
                                                                                         << result.text());
                                }
                            }
                            else
                            {
                                DCMIOD_ERROR("Cannot get Dimension Index since fg "
                                             << fgPointer << " misses reservation for frame " << count << ": "
                                             << result.text());
                            }
                        }
                    }
                    else
                    {
                        DCMIOD_ERROR("Cannot get Dimension Index since fg "
                                     << fgPointer << " does not contain index pointer attribute " << indexPointer
                                     << " for frame " << count);
                    }
                }
                else
                {
                    DCMIOD_ERROR("Cannot get Dimension Index since fg " << fgPointer << " does have any data for frame "
                                                                        << count);
                }
            }
            else
            {
                DCMIOD_ERROR("Cannot get Dimension Index since fg " << fgPointer << " does not exist for frame "
                                                                    << count);
            }
        }
        else
        {
            DCMIOD_ERROR("Cannot get Dimension Index: since no per-frame functional group for frame " << count
                                                                                                      << " exists");
        }
    }

    return returnValue;
}

void IODMultiframeDimensionModule::setCheckOnWrite(const OFBool doCheck)
{
    m_CheckOnWrite = doCheck;
}

OFBool IODMultiframeDimensionModule::getCheckOnWrite()
{
    return m_CheckOnWrite;
}

void IODMultiframeDimensionModule::createDimensionOrganizationData()
{
    // Clear old data
    DcmIODUtil::freeContainer(m_DimensionOrganizationSequence);
    // Collect dimensions from all Dimension Index Pointers
    OFVector<DimensionIndexItem*>::iterator item = m_DimensionIndexSequence.begin();
    while (item != m_DimensionIndexSequence.end())
    {
        OFCondition result;
        // Make sure every UID is only added once
        OFBool isNew = OFTrue;
        OFString existinguid, newuid;
        (*item)->getDimensionOrganizationUID(newuid);
        OFVector<DimensionOrganizationItem*>::iterator existing = m_DimensionOrganizationSequence.begin();
        while (existing != m_DimensionOrganizationSequence.end())
        {
            (*existing)->getDimensionOrganizationUID(existinguid);
            if (existinguid == newuid)
            {
                isNew = OFFalse;
                break;
            }
            existing++;
        }
        if (isNew)
        {
            DimensionOrganizationItem* uidItem = new DimensionOrganizationItem();
            if (!uidItem)
            {
                DCMIOD_ERROR("Memory Exhausted while collecting Dimension Organization UIDs");
                return;
            }
            result = uidItem->setDimensionOrganizationUID(newuid);
            if (result.good())
            {
                m_DimensionOrganizationSequence.push_back(uidItem);
            }
            else
            {
                DCMIOD_ERROR("Could not set Dimension Organization UID " << newuid << ": " << result.text());
                delete uidItem;
                return;
            }
        }
        item++;
    }
}

OFCondition IODMultiframeDimensionModule::getDimensionOrganizationType(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_DimensionOrganizationType, *m_Item, value, pos);
}

OFCondition IODMultiframeDimensionModule::setDimensionOrganizationType(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_DimensionOrganizationType, value);
    return result;
}

OFVector<IODMultiframeDimensionModule::DimensionIndexItem*>& IODMultiframeDimensionModule::getDimensionIndexSequence()
{
    return m_DimensionIndexSequence;
}

OFVector<IODMultiframeDimensionModule::DimensionOrganizationItem*>&
IODMultiframeDimensionModule::getDimensionOrganizationSequence()
{
    return m_DimensionOrganizationSequence;
}

IODMultiframeDimensionModule::~IODMultiframeDimensionModule()
{
    DcmIODUtil::freeContainer(m_DimensionIndexSequence);
    DcmIODUtil::freeContainer(m_DimensionOrganizationSequence);
}

IODMultiframeDimensionModule::DimensionIndexItem::DimensionIndexItem(OFshared_ptr<DcmItem> data,
                                                                     OFshared_ptr<IODRules> rules,
                                                                     IODComponent* parent)
    : IODComponent(data, rules, parent)
{
    // reset element rules
    resetRules();
}

IODMultiframeDimensionModule::DimensionIndexItem::DimensionIndexItem(IODComponent* parent)
    : IODComponent(parent)
{
    // reset element rules
    resetRules();
}

IODMultiframeDimensionModule::DimensionIndexItem::~DimensionIndexItem()
{
    // Nothing to do
}

void IODMultiframeDimensionModule::DimensionIndexItem::resetRules()
{
    // Concatenation attributes
    m_Rules->addRule(new IODRule(DCM_DimensionIndexPointer, "1", "1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_DimensionIndexPrivateCreator, "1", "1C", getName(), DcmIODTypes::IE_INSTANCE),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_FunctionalGroupPointer, "1", "1C", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_FunctionalGroupPrivateCreator, "1", "1C", getName(), DcmIODTypes::IE_INSTANCE),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_DimensionOrganizationUID, "1", "1C", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_DimensionDescriptionLabel, "1", "1C", getName(), DcmIODTypes::IE_INSTANCE),
                     OFTrue);
}

OFString IODMultiframeDimensionModule::DimensionIndexItem::getName() const
{
    return "DimensionIndexSequence";
}

OFCondition
IODMultiframeDimensionModule::DimensionIndexItem::getDimensionDescriptionLabel(OFString& value,
                                                                               const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_DimensionDescriptionLabel, *m_Item, value, pos);
}

OFCondition IODMultiframeDimensionModule::DimensionIndexItem::getDimensionIndexPointer(DcmTagKey& value,
                                                                                       const long signed int pos) const
{
    OFString tagstr;
    DcmIODUtil::getStringValueFromItem(DCM_DimensionIndexPointer, *m_Item, tagstr, pos);
    value = DcmIODUtil::parseTagKey(tagstr);
    if (value == DCM_UndefinedTagKey)
        return EC_TagNotFound;
    else
        return EC_Normal;
}

OFCondition
IODMultiframeDimensionModule::DimensionIndexItem::getDimensionIndexPrivateCreator(OFString& value,
                                                                                  const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_DimensionIndexPrivateCreator, *m_Item, value, pos);
}

OFCondition
IODMultiframeDimensionModule::DimensionIndexItem::getDimensionOrganizationUID(OFString& value,
                                                                              const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_DimensionOrganizationUID, *m_Item, value, pos);
}

OFCondition IODMultiframeDimensionModule::DimensionIndexItem::getFunctionalGroupPointer(DcmTagKey& value,
                                                                                        const long signed int pos) const
{
    OFString tagstr;
    DcmIODUtil::getStringValueFromItem(DCM_FunctionalGroupPointer, *m_Item, tagstr, pos);
    value = DcmIODUtil::parseTagKey(tagstr);
    if (value == DCM_UndefinedTagKey)
        return EC_TagNotFound;
    else
        return EC_Normal;
}

OFCondition
IODMultiframeDimensionModule::DimensionIndexItem::getFunctionalGroupPrivateCreator(OFString& value,
                                                                                   const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_FunctionalGroupPrivateCreator, *m_Item, value, pos);
}

OFCondition IODMultiframeDimensionModule::DimensionIndexItem::setDimensionDescriptionLabel(const OFString& value,
                                                                                           const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_DimensionDescriptionLabel, value);
    return result;
}

OFCondition IODMultiframeDimensionModule::DimensionIndexItem::setFunctionalGroupPointer(const DcmTagKey& value,
                                                                                        const OFBool checkValue)
{
    (void)checkValue;
    if (value == DCM_UndefinedTagKey)
        return EC_IllegalParameter;

    DcmAttributeTag* elem = OFstatic_cast(DcmAttributeTag*, DcmItem::newDicomElement(DCM_FunctionalGroupPointer));
    if (elem == NULL)
        return EC_InternalError;

    OFCondition result;
    Uint16* attrTagArray = NULL;
    result = elem->putTagVal(value);
    if (result.good())
        result = elem->getUint16Array(attrTagArray);
    if (result.good())
        result = m_Item->putAndInsertUint16Array(DCM_FunctionalGroupPointer, attrTagArray, 1);
    delete elem;

    return result;
}

OFCondition IODMultiframeDimensionModule::DimensionIndexItem::setFunctionalGroupPrivateCreator(const OFString& value,
                                                                                               const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_FunctionalGroupPrivateCreator, value);
    return result;
}

OFCondition IODMultiframeDimensionModule::DimensionIndexItem::setDimensionIndexPointer(const DcmTagKey& value,
                                                                                       const OFBool checkValue)
{
    (void)checkValue;
    if (value == DCM_UndefinedTagKey)
        return EC_IllegalParameter;

    DcmAttributeTag* elem = OFstatic_cast(DcmAttributeTag*, DcmItem::newDicomElement(DCM_DimensionIndexPointer));
    if (elem == NULL)
        return EC_InternalError;

    OFCondition result;
    Uint16* attrTagArray = NULL;
    result = elem->putTagVal(value);
    if (result.good())
        result = elem->getUint16Array(attrTagArray);
    if (result.good())
        result = m_Item->putAndInsertUint16Array(DCM_DimensionIndexPointer, attrTagArray, 1);
    delete elem;

    return result;
}

OFCondition IODMultiframeDimensionModule::DimensionIndexItem::setDimensionIndexPrivateCreator(const OFString& value,
                                                                                              const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_DimensionIndexPrivateCreator, value);
    return result;
}

OFCondition IODMultiframeDimensionModule::DimensionIndexItem::setDimensionOrganizationUID(const OFString& value,
                                                                                          const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_DimensionOrganizationUID, value);
    return result;
}

// -- Dimension Organization Sequence Item --

IODMultiframeDimensionModule::DimensionOrganizationItem::DimensionOrganizationItem(OFshared_ptr<DcmItem> data,
                                                                                   OFshared_ptr<IODRules> rules,
                                                                                   IODComponent* parent)
    : IODComponent(data, rules, parent)
{
    // reset element rules
    resetRules();
}

IODMultiframeDimensionModule::DimensionOrganizationItem::DimensionOrganizationItem(IODComponent* parent)
    : IODComponent(parent)
{
    // reset element rules
    resetRules();
}

IODMultiframeDimensionModule::DimensionOrganizationItem::~DimensionOrganizationItem()
{
    // Nothing to do
}

void IODMultiframeDimensionModule::DimensionOrganizationItem::resetRules()
{
    // Concatenation attributes
    m_Rules->addRule(new IODRule(DCM_DimensionOrganizationUID, "1", "1C", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
}

OFString IODMultiframeDimensionModule::DimensionOrganizationItem::getName() const
{
    return "DimensionOrganizationSequence";
}

OFCondition
IODMultiframeDimensionModule::DimensionOrganizationItem::getDimensionOrganizationUID(OFString& value,
                                                                                     const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_DimensionOrganizationUID, *m_Item, value, pos);
}

OFCondition
IODMultiframeDimensionModule::DimensionOrganizationItem::setDimensionOrganizationUID(const OFString& value,
                                                                                     const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_DimensionOrganizationUID, value);
    return result;
}
