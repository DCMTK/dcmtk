/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTContextGroupIdentificationSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtcgis.h"


// --- item class ---

DRTContextGroupIdentificationSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ContextGroupVersion(DCM_ContextGroupVersion),
    ContextIdentifier(DCM_ContextIdentifier),
    ContextUID(DCM_ContextUID),
    MappingResource(DCM_MappingResource)
{
}


DRTContextGroupIdentificationSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ContextGroupVersion(copy.ContextGroupVersion),
    ContextIdentifier(copy.ContextIdentifier),
    ContextUID(copy.ContextUID),
    MappingResource(copy.MappingResource)
{
}


DRTContextGroupIdentificationSequence::Item::~Item()
{
}


DRTContextGroupIdentificationSequence::Item &DRTContextGroupIdentificationSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ContextGroupVersion = copy.ContextGroupVersion;
        ContextIdentifier = copy.ContextIdentifier;
        ContextUID = copy.ContextUID;
        MappingResource = copy.MappingResource;
    }
    return *this;
}


void DRTContextGroupIdentificationSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ContextIdentifier.clear();
        ContextUID.clear();
        MappingResource.clear();
        ContextGroupVersion.clear();
    }
}


OFBool DRTContextGroupIdentificationSequence::Item::isEmpty()
{
    return ContextIdentifier.isEmpty() &&
           ContextUID.isEmpty() &&
           MappingResource.isEmpty() &&
           ContextGroupVersion.isEmpty();
}


OFBool DRTContextGroupIdentificationSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTContextGroupIdentificationSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ContextIdentifier, "1", "1", "ContextGroupIdentificationSequence");
        getAndCheckElementFromDataset(item, ContextUID, "1", "3", "ContextGroupIdentificationSequence");
        getAndCheckElementFromDataset(item, MappingResource, "1", "1", "ContextGroupIdentificationSequence");
        getAndCheckElementFromDataset(item, ContextGroupVersion, "1", "1", "ContextGroupIdentificationSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTContextGroupIdentificationSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmCodeString(ContextIdentifier), "1", "1", "ContextGroupIdentificationSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(ContextUID), "1", "3", "ContextGroupIdentificationSequence");
        addElementToDataset(result, item, new DcmCodeString(MappingResource), "1", "1", "ContextGroupIdentificationSequence");
        addElementToDataset(result, item, new DcmDateTime(ContextGroupVersion), "1", "1", "ContextGroupIdentificationSequence");
    }
    return result;
}


OFCondition DRTContextGroupIdentificationSequence::Item::getContextGroupVersion(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContextGroupVersion, value, pos);
}


OFCondition DRTContextGroupIdentificationSequence::Item::getContextIdentifier(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContextIdentifier, value, pos);
}


OFCondition DRTContextGroupIdentificationSequence::Item::getContextUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContextUID, value, pos);
}


OFCondition DRTContextGroupIdentificationSequence::Item::getMappingResource(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MappingResource, value, pos);
}


OFCondition DRTContextGroupIdentificationSequence::Item::setContextGroupVersion(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ContextGroupVersion.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContextGroupIdentificationSequence::Item::setContextIdentifier(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ContextIdentifier.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContextGroupIdentificationSequence::Item::setContextUID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ContextUID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContextGroupIdentificationSequence::Item::setMappingResource(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = MappingResource.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTContextGroupIdentificationSequence::DRTContextGroupIdentificationSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTContextGroupIdentificationSequence::DRTContextGroupIdentificationSequence(const DRTContextGroupIdentificationSequence &copy)
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


DRTContextGroupIdentificationSequence &DRTContextGroupIdentificationSequence::operator=(const DRTContextGroupIdentificationSequence &copy)
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


DRTContextGroupIdentificationSequence::~DRTContextGroupIdentificationSequence()
{
    clear();
}


void DRTContextGroupIdentificationSequence::clear()
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


OFBool DRTContextGroupIdentificationSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTContextGroupIdentificationSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTContextGroupIdentificationSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTContextGroupIdentificationSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTContextGroupIdentificationSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTContextGroupIdentificationSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        unsigned long idx = num + 1;
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


OFCondition DRTContextGroupIdentificationSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        unsigned long idx = num + 1;
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


OFCondition DRTContextGroupIdentificationSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTContextGroupIdentificationSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTContextGroupIdentificationSequence::Item &DRTContextGroupIdentificationSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTContextGroupIdentificationSequence::Item &DRTContextGroupIdentificationSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTContextGroupIdentificationSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTContextGroupIdentificationSequence::Item &DRTContextGroupIdentificationSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTContextGroupIdentificationSequence::Item &DRTContextGroupIdentificationSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTContextGroupIdentificationSequence::Item &DRTContextGroupIdentificationSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTContextGroupIdentificationSequence::Item &DRTContextGroupIdentificationSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTContextGroupIdentificationSequence::addItem(Item *&item)
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


OFCondition DRTContextGroupIdentificationSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTContextGroupIdentificationSequence::removeItem(const unsigned long pos)
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


OFCondition DRTContextGroupIdentificationSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ContextGroupIdentificationSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ContextGroupIdentificationSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTContextGroupIdentificationSequence::write(DcmItem &dataset,
                                                         const OFString &card,
                                                         const OFString &type,
                                                         const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ContextGroupIdentificationSequence);
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
