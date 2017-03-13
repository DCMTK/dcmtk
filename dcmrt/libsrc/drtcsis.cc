/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTCodingSchemeIdentificationSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtcsis.h"


// --- item class ---

DRTCodingSchemeIdentificationSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    CodingSchemeDesignator(DCM_CodingSchemeDesignator),
    CodingSchemeExternalID(DCM_CodingSchemeExternalID),
    CodingSchemeName(DCM_CodingSchemeName),
    CodingSchemeRegistry(DCM_CodingSchemeRegistry),
    CodingSchemeResponsibleOrganization(DCM_CodingSchemeResponsibleOrganization),
    CodingSchemeUID(DCM_CodingSchemeUID),
    CodingSchemeVersion(DCM_CodingSchemeVersion)
{
}


DRTCodingSchemeIdentificationSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    CodingSchemeDesignator(copy.CodingSchemeDesignator),
    CodingSchemeExternalID(copy.CodingSchemeExternalID),
    CodingSchemeName(copy.CodingSchemeName),
    CodingSchemeRegistry(copy.CodingSchemeRegistry),
    CodingSchemeResponsibleOrganization(copy.CodingSchemeResponsibleOrganization),
    CodingSchemeUID(copy.CodingSchemeUID),
    CodingSchemeVersion(copy.CodingSchemeVersion)
{
}


DRTCodingSchemeIdentificationSequence::Item::~Item()
{
}


DRTCodingSchemeIdentificationSequence::Item &DRTCodingSchemeIdentificationSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        CodingSchemeDesignator = copy.CodingSchemeDesignator;
        CodingSchemeExternalID = copy.CodingSchemeExternalID;
        CodingSchemeName = copy.CodingSchemeName;
        CodingSchemeRegistry = copy.CodingSchemeRegistry;
        CodingSchemeResponsibleOrganization = copy.CodingSchemeResponsibleOrganization;
        CodingSchemeUID = copy.CodingSchemeUID;
        CodingSchemeVersion = copy.CodingSchemeVersion;
    }
    return *this;
}


void DRTCodingSchemeIdentificationSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        CodingSchemeDesignator.clear();
        CodingSchemeRegistry.clear();
        CodingSchemeUID.clear();
        CodingSchemeExternalID.clear();
        CodingSchemeName.clear();
        CodingSchemeVersion.clear();
        CodingSchemeResponsibleOrganization.clear();
    }
}


OFBool DRTCodingSchemeIdentificationSequence::Item::isEmpty()
{
    return CodingSchemeDesignator.isEmpty() &&
           CodingSchemeRegistry.isEmpty() &&
           CodingSchemeUID.isEmpty() &&
           CodingSchemeExternalID.isEmpty() &&
           CodingSchemeName.isEmpty() &&
           CodingSchemeVersion.isEmpty() &&
           CodingSchemeResponsibleOrganization.isEmpty();
}


OFBool DRTCodingSchemeIdentificationSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTCodingSchemeIdentificationSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, CodingSchemeDesignator, "1", "1", "CodingSchemeIdentificationSequence");
        getAndCheckElementFromDataset(item, CodingSchemeRegistry, "1", "1C", "CodingSchemeIdentificationSequence");
        getAndCheckElementFromDataset(item, CodingSchemeUID, "1", "1C", "CodingSchemeIdentificationSequence");
        getAndCheckElementFromDataset(item, CodingSchemeExternalID, "1", "2C", "CodingSchemeIdentificationSequence");
        getAndCheckElementFromDataset(item, CodingSchemeName, "1", "3", "CodingSchemeIdentificationSequence");
        getAndCheckElementFromDataset(item, CodingSchemeVersion, "1", "3", "CodingSchemeIdentificationSequence");
        getAndCheckElementFromDataset(item, CodingSchemeResponsibleOrganization, "1", "3", "CodingSchemeIdentificationSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTCodingSchemeIdentificationSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmShortString(CodingSchemeDesignator), "1", "1", "CodingSchemeIdentificationSequence");
        addElementToDataset(result, item, new DcmLongString(CodingSchemeRegistry), "1", "1C", "CodingSchemeIdentificationSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(CodingSchemeUID), "1", "1C", "CodingSchemeIdentificationSequence");
        addElementToDataset(result, item, new DcmShortText(CodingSchemeExternalID), "1", "2C", "CodingSchemeIdentificationSequence");
        addElementToDataset(result, item, new DcmShortText(CodingSchemeName), "1", "3", "CodingSchemeIdentificationSequence");
        addElementToDataset(result, item, new DcmShortString(CodingSchemeVersion), "1", "3", "CodingSchemeIdentificationSequence");
        addElementToDataset(result, item, new DcmShortText(CodingSchemeResponsibleOrganization), "1", "3", "CodingSchemeIdentificationSequence");
    }
    return result;
}


OFCondition DRTCodingSchemeIdentificationSequence::Item::getCodingSchemeDesignator(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CodingSchemeDesignator, value, pos);
}


OFCondition DRTCodingSchemeIdentificationSequence::Item::getCodingSchemeExternalID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CodingSchemeExternalID, value, pos);
}


OFCondition DRTCodingSchemeIdentificationSequence::Item::getCodingSchemeName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CodingSchemeName, value, pos);
}


OFCondition DRTCodingSchemeIdentificationSequence::Item::getCodingSchemeRegistry(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CodingSchemeRegistry, value, pos);
}


OFCondition DRTCodingSchemeIdentificationSequence::Item::getCodingSchemeResponsibleOrganization(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CodingSchemeResponsibleOrganization, value, pos);
}


OFCondition DRTCodingSchemeIdentificationSequence::Item::getCodingSchemeUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CodingSchemeUID, value, pos);
}


OFCondition DRTCodingSchemeIdentificationSequence::Item::getCodingSchemeVersion(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CodingSchemeVersion, value, pos);
}


OFCondition DRTCodingSchemeIdentificationSequence::Item::setCodingSchemeDesignator(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CodingSchemeDesignator.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTCodingSchemeIdentificationSequence::Item::setCodingSchemeExternalID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = CodingSchemeExternalID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTCodingSchemeIdentificationSequence::Item::setCodingSchemeName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = CodingSchemeName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTCodingSchemeIdentificationSequence::Item::setCodingSchemeRegistry(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CodingSchemeRegistry.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTCodingSchemeIdentificationSequence::Item::setCodingSchemeResponsibleOrganization(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = CodingSchemeResponsibleOrganization.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTCodingSchemeIdentificationSequence::Item::setCodingSchemeUID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CodingSchemeUID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTCodingSchemeIdentificationSequence::Item::setCodingSchemeVersion(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CodingSchemeVersion.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTCodingSchemeIdentificationSequence::DRTCodingSchemeIdentificationSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTCodingSchemeIdentificationSequence::DRTCodingSchemeIdentificationSequence(const DRTCodingSchemeIdentificationSequence &copy)
  : EmptyDefaultSequence(copy.EmptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    /* create a copy of the internal sequence of items */
    Item *item = NULL;
    OFListConstIterator(Item *) current = copy.SequenceOfItems.begin();
    const OFListConstIterator(Item *) last = copy.SequenceOfItems.end();
    while (current != last)
    {
        item = new Item(**current);
        if (item != NULL)
        {
            SequenceOfItems.push_back(item);
        } else {
            /* memory exhausted, there is nothing we can do about it */
            break;
        }
        ++current;
    }
    CurrentItem = SequenceOfItems.begin();
}


DRTCodingSchemeIdentificationSequence &DRTCodingSchemeIdentificationSequence::operator=(const DRTCodingSchemeIdentificationSequence &copy)
{
    if (this != &copy)
    {
        clear();
        EmptyDefaultSequence = copy.EmptyDefaultSequence;
        /* create a copy of the internal sequence of items */
        Item *item = NULL;
        OFListConstIterator(Item *) current = copy.SequenceOfItems.begin();
        const OFListConstIterator(Item *) last = copy.SequenceOfItems.end();
        while (current != last)
        {
            item = new Item(**current);
            if (item != NULL)
            {
                SequenceOfItems.push_back(item);
            } else {
                /* memory exhausted, there is nothing we can do about it */
                break;
            }
            ++current;
        }
        CurrentItem = SequenceOfItems.begin();
    }
    return *this;
}


DRTCodingSchemeIdentificationSequence::~DRTCodingSchemeIdentificationSequence()
{
    clear();
}


void DRTCodingSchemeIdentificationSequence::clear()
{
    if (!EmptyDefaultSequence)
    {
        CurrentItem = SequenceOfItems.begin();
        const OFListConstIterator(Item *) last = SequenceOfItems.end();
        /* delete all items and free memory */
        while (CurrentItem != last)
        {
            delete (*CurrentItem);
            CurrentItem = SequenceOfItems.erase(CurrentItem);
        }
        /* make sure that the list is empty */
        SequenceOfItems.clear();
        CurrentItem = SequenceOfItems.end();
    }
}


OFBool DRTCodingSchemeIdentificationSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTCodingSchemeIdentificationSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTCodingSchemeIdentificationSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTCodingSchemeIdentificationSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTCodingSchemeIdentificationSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTCodingSchemeIdentificationSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        size_t idx = num + 1;
        iterator = SequenceOfItems.begin();
        const OFListConstIterator(Item *) last = SequenceOfItems.end();
        while ((--idx > 0) && (iterator != last))
            ++iterator;
        /* specified list item found? */
        if ((idx == 0) && (iterator != last))
            result = EC_Normal;
        else
            result = EC_IllegalParameter;
    }
    return result;
}


OFCondition DRTCodingSchemeIdentificationSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        size_t idx = num + 1;
        iterator = SequenceOfItems.begin();
        const OFListConstIterator(Item *) last = SequenceOfItems.end();
        while ((--idx > 0) && (iterator != last))
            ++iterator;
        /* specified list item found? */
        if ((idx == 0) && (iterator != last))
            result = EC_Normal;
        else
            result = EC_IllegalParameter;
    }
    return result;
}


OFCondition DRTCodingSchemeIdentificationSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTCodingSchemeIdentificationSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTCodingSchemeIdentificationSequence::Item &DRTCodingSchemeIdentificationSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTCodingSchemeIdentificationSequence::Item &DRTCodingSchemeIdentificationSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTCodingSchemeIdentificationSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTCodingSchemeIdentificationSequence::Item &DRTCodingSchemeIdentificationSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTCodingSchemeIdentificationSequence::Item &DRTCodingSchemeIdentificationSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTCodingSchemeIdentificationSequence::Item &DRTCodingSchemeIdentificationSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTCodingSchemeIdentificationSequence::Item &DRTCodingSchemeIdentificationSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTCodingSchemeIdentificationSequence::addItem(Item *&item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        item = new Item();
        if (item != NULL)
        {
            SequenceOfItems.push_back(item);
            result = EC_Normal;
        } else
            result = EC_MemoryExhausted;
    }
    return result;
}


OFCondition DRTCodingSchemeIdentificationSequence::insertItem(const size_t pos, Item *&item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        OFListIterator(Item *) iterator;
        result = gotoItem(pos, iterator);
        if (result.good())
        {
            item = new Item();
            if (item != NULL)
            {
                SequenceOfItems.insert(iterator, 1, item);
                result = EC_Normal;
            } else
                result = EC_MemoryExhausted;
        } else
            result = addItem(item);
    }
    return result;
}


OFCondition DRTCodingSchemeIdentificationSequence::removeItem(const size_t pos)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        OFListIterator(Item *) iterator;
        if (gotoItem(pos, iterator).good())
        {
            delete *iterator;
            iterator = SequenceOfItems.erase(iterator);
            result = EC_Normal;
        } else
            result = EC_IllegalParameter;
    }
    return result;
}


OFCondition DRTCodingSchemeIdentificationSequence::read(DcmItem &dataset,
                                                        const OFString &card,
                                                        const OFString &type,
                                                        const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        /* re-initialize object */
        clear();
        /* retrieve sequence element from dataset */
        DcmSequenceOfItems *sequence;
        result = dataset.findAndGetSequence(DCM_CodingSchemeIdentificationSequence, sequence);
        if (sequence != NULL)
        {
            if (checkElementValue(*sequence, card, type, result, moduleName))
            {
                DcmStack stack;
                OFBool first = OFTrue;
                /* iterate over all sequence items */
                while (result.good() && sequence->nextObject(stack, first /*intoSub*/).good())
                {
                    DcmItem *ditem = OFstatic_cast(DcmItem *, stack.top());
                    if (ditem != NULL)
                    {
                        Item *item = new Item();
                        if (item != NULL)
                        {
                            result = item->read(*ditem);
                            if (result.good())
                            {
                                /* append new item to the end of the list */
                                SequenceOfItems.push_back(item);
                                first = OFFalse;
                            }
                        } else
                            result = EC_MemoryExhausted;
                    } else
                        result = EC_CorruptedData;
                }
            }
        } else {
            DcmSequenceOfItems element(DCM_CodingSchemeIdentificationSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTCodingSchemeIdentificationSequence::write(DcmItem &dataset,
                                                         const OFString &card,
                                                         const OFString &type,
                                                         const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_CodingSchemeIdentificationSequence);
        if (sequence != NULL)
        {
            result = EC_Normal;
            /* an empty optional sequence is not written */
            if ((type == "2") || !SequenceOfItems.empty())
            {
                OFListIterator(Item *) iterator = SequenceOfItems.begin();
                const OFListConstIterator(Item *) last = SequenceOfItems.end();
                /* iterate over all sequence items */
                while (result.good() && (iterator != last))
                {
                    DcmItem *item = new DcmItem();
                    if (item != NULL)
                    {
                        /* append new item to the end of the sequence */
                        result = sequence->append(item);
                        if (result.good())
                        {
                            result = (*iterator)->write(*item);
                            ++iterator;
                        } else
                            delete item;
                    } else
                        result = EC_MemoryExhausted;
                }
                if (result.good())
                {
                    /* insert sequence element into the dataset */
                    result = dataset.insert(sequence, OFTrue /*replaceOld*/);
                }
                if (DCM_dcmrtLogger.isEnabledFor(OFLogger::WARN_LOG_LEVEL))
                    checkElementValue(*sequence, card, type, result, moduleName);
                if (result.good())
                {
                    /* forget reference to sequence object (avoid deletion below) */
                    sequence = NULL;
                }
            }
            else if (type == "1")
            {
                /* empty type 1 sequence not allowed */
                result = RT_EC_InvalidValue;
                if (DCM_dcmrtLogger.isEnabledFor(OFLogger::WARN_LOG_LEVEL))
                    checkElementValue(*sequence, card, type, result, moduleName);
            }
            /* delete sequence (if not inserted into the dataset) */
            delete sequence;
        }
    }
    return result;
}


// end of source file
