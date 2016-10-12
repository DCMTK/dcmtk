/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTIssuerOfServiceEpisodeIDSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtiseis.h"


// --- item class ---

DRTIssuerOfServiceEpisodeIDSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    LocalNamespaceEntityID(DCM_LocalNamespaceEntityID),
    UniversalEntityID(DCM_UniversalEntityID),
    UniversalEntityIDType(DCM_UniversalEntityIDType)
{
}


DRTIssuerOfServiceEpisodeIDSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    LocalNamespaceEntityID(copy.LocalNamespaceEntityID),
    UniversalEntityID(copy.UniversalEntityID),
    UniversalEntityIDType(copy.UniversalEntityIDType)
{
}


DRTIssuerOfServiceEpisodeIDSequence::Item::~Item()
{
}


DRTIssuerOfServiceEpisodeIDSequence::Item &DRTIssuerOfServiceEpisodeIDSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        LocalNamespaceEntityID = copy.LocalNamespaceEntityID;
        UniversalEntityID = copy.UniversalEntityID;
        UniversalEntityIDType = copy.UniversalEntityIDType;
    }
    return *this;
}


void DRTIssuerOfServiceEpisodeIDSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        LocalNamespaceEntityID.clear();
        UniversalEntityID.clear();
        UniversalEntityIDType.clear();
    }
}


OFBool DRTIssuerOfServiceEpisodeIDSequence::Item::isEmpty()
{
    return LocalNamespaceEntityID.isEmpty() &&
           UniversalEntityID.isEmpty() &&
           UniversalEntityIDType.isEmpty();
}


OFBool DRTIssuerOfServiceEpisodeIDSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTIssuerOfServiceEpisodeIDSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, LocalNamespaceEntityID, "1", "1C", "IssuerOfServiceEpisodeIDSequence");
        getAndCheckElementFromDataset(item, UniversalEntityID, "1", "1C", "IssuerOfServiceEpisodeIDSequence");
        getAndCheckElementFromDataset(item, UniversalEntityIDType, "1", "1C", "IssuerOfServiceEpisodeIDSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIssuerOfServiceEpisodeIDSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmUnlimitedText(LocalNamespaceEntityID), "1", "1C", "IssuerOfServiceEpisodeIDSequence");
        addElementToDataset(result, item, new DcmUnlimitedText(UniversalEntityID), "1", "1C", "IssuerOfServiceEpisodeIDSequence");
        addElementToDataset(result, item, new DcmCodeString(UniversalEntityIDType), "1", "1C", "IssuerOfServiceEpisodeIDSequence");
    }
    return result;
}


OFCondition DRTIssuerOfServiceEpisodeIDSequence::Item::getLocalNamespaceEntityID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(LocalNamespaceEntityID, value, pos);
}


OFCondition DRTIssuerOfServiceEpisodeIDSequence::Item::getUniversalEntityID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(UniversalEntityID, value, pos);
}


OFCondition DRTIssuerOfServiceEpisodeIDSequence::Item::getUniversalEntityIDType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(UniversalEntityIDType, value, pos);
}


OFCondition DRTIssuerOfServiceEpisodeIDSequence::Item::setLocalNamespaceEntityID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUnlimitedText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = LocalNamespaceEntityID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIssuerOfServiceEpisodeIDSequence::Item::setUniversalEntityID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUnlimitedText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = UniversalEntityID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIssuerOfServiceEpisodeIDSequence::Item::setUniversalEntityIDType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = UniversalEntityIDType.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTIssuerOfServiceEpisodeIDSequence::DRTIssuerOfServiceEpisodeIDSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTIssuerOfServiceEpisodeIDSequence::DRTIssuerOfServiceEpisodeIDSequence(const DRTIssuerOfServiceEpisodeIDSequence &copy)
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


DRTIssuerOfServiceEpisodeIDSequence &DRTIssuerOfServiceEpisodeIDSequence::operator=(const DRTIssuerOfServiceEpisodeIDSequence &copy)
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


DRTIssuerOfServiceEpisodeIDSequence::~DRTIssuerOfServiceEpisodeIDSequence()
{
    clear();
}


void DRTIssuerOfServiceEpisodeIDSequence::clear()
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


OFBool DRTIssuerOfServiceEpisodeIDSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTIssuerOfServiceEpisodeIDSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTIssuerOfServiceEpisodeIDSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTIssuerOfServiceEpisodeIDSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIssuerOfServiceEpisodeIDSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIssuerOfServiceEpisodeIDSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTIssuerOfServiceEpisodeIDSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTIssuerOfServiceEpisodeIDSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTIssuerOfServiceEpisodeIDSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTIssuerOfServiceEpisodeIDSequence::Item &DRTIssuerOfServiceEpisodeIDSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTIssuerOfServiceEpisodeIDSequence::Item &DRTIssuerOfServiceEpisodeIDSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTIssuerOfServiceEpisodeIDSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTIssuerOfServiceEpisodeIDSequence::Item &DRTIssuerOfServiceEpisodeIDSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTIssuerOfServiceEpisodeIDSequence::Item &DRTIssuerOfServiceEpisodeIDSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTIssuerOfServiceEpisodeIDSequence::Item &DRTIssuerOfServiceEpisodeIDSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTIssuerOfServiceEpisodeIDSequence::Item &DRTIssuerOfServiceEpisodeIDSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTIssuerOfServiceEpisodeIDSequence::addItem(Item *&item)
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


OFCondition DRTIssuerOfServiceEpisodeIDSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTIssuerOfServiceEpisodeIDSequence::removeItem(const unsigned long pos)
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


OFCondition DRTIssuerOfServiceEpisodeIDSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_IssuerOfServiceEpisodeIDSequence, sequence);
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
            DcmSequenceOfItems element(DCM_IssuerOfServiceEpisodeIDSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTIssuerOfServiceEpisodeIDSequence::write(DcmItem &dataset,
                                                       const OFString &card,
                                                       const OFString &type,
                                                       const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_IssuerOfServiceEpisodeIDSequence);
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
