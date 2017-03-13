/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTMotionSynchronizationSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtmss.h"


// --- item class ---

DRTMotionSynchronizationSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    RespiratoryMotionCompensationTechnique(DCM_RespiratoryMotionCompensationTechnique),
    RespiratoryMotionCompensationTechniqueDescription(DCM_RespiratoryMotionCompensationTechniqueDescription),
    RespiratorySignalSource(DCM_RespiratorySignalSource),
    RespiratorySignalSourceID(DCM_RespiratorySignalSourceID)
{
}


DRTMotionSynchronizationSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    RespiratoryMotionCompensationTechnique(copy.RespiratoryMotionCompensationTechnique),
    RespiratoryMotionCompensationTechniqueDescription(copy.RespiratoryMotionCompensationTechniqueDescription),
    RespiratorySignalSource(copy.RespiratorySignalSource),
    RespiratorySignalSourceID(copy.RespiratorySignalSourceID)
{
}


DRTMotionSynchronizationSequence::Item::~Item()
{
}


DRTMotionSynchronizationSequence::Item &DRTMotionSynchronizationSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        RespiratoryMotionCompensationTechnique = copy.RespiratoryMotionCompensationTechnique;
        RespiratoryMotionCompensationTechniqueDescription = copy.RespiratoryMotionCompensationTechniqueDescription;
        RespiratorySignalSource = copy.RespiratorySignalSource;
        RespiratorySignalSourceID = copy.RespiratorySignalSourceID;
    }
    return *this;
}


void DRTMotionSynchronizationSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        RespiratoryMotionCompensationTechnique.clear();
        RespiratorySignalSource.clear();
        RespiratoryMotionCompensationTechniqueDescription.clear();
        RespiratorySignalSourceID.clear();
    }
}


OFBool DRTMotionSynchronizationSequence::Item::isEmpty()
{
    return RespiratoryMotionCompensationTechnique.isEmpty() &&
           RespiratorySignalSource.isEmpty() &&
           RespiratoryMotionCompensationTechniqueDescription.isEmpty() &&
           RespiratorySignalSourceID.isEmpty();
}


OFBool DRTMotionSynchronizationSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTMotionSynchronizationSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, RespiratoryMotionCompensationTechnique, "1", "1", "MotionSynchronizationSequence");
        getAndCheckElementFromDataset(item, RespiratorySignalSource, "1", "1", "MotionSynchronizationSequence");
        getAndCheckElementFromDataset(item, RespiratoryMotionCompensationTechniqueDescription, "1", "3", "MotionSynchronizationSequence");
        getAndCheckElementFromDataset(item, RespiratorySignalSourceID, "1", "3", "MotionSynchronizationSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTMotionSynchronizationSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmCodeString(RespiratoryMotionCompensationTechnique), "1", "1", "MotionSynchronizationSequence");
        addElementToDataset(result, item, new DcmCodeString(RespiratorySignalSource), "1", "1", "MotionSynchronizationSequence");
        addElementToDataset(result, item, new DcmShortText(RespiratoryMotionCompensationTechniqueDescription), "1", "3", "MotionSynchronizationSequence");
        addElementToDataset(result, item, new DcmShortString(RespiratorySignalSourceID), "1", "3", "MotionSynchronizationSequence");
    }
    return result;
}


OFCondition DRTMotionSynchronizationSequence::Item::getRespiratoryMotionCompensationTechnique(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RespiratoryMotionCompensationTechnique, value, pos);
}


OFCondition DRTMotionSynchronizationSequence::Item::getRespiratoryMotionCompensationTechniqueDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RespiratoryMotionCompensationTechniqueDescription, value, pos);
}


OFCondition DRTMotionSynchronizationSequence::Item::getRespiratorySignalSource(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RespiratorySignalSource, value, pos);
}


OFCondition DRTMotionSynchronizationSequence::Item::getRespiratorySignalSourceID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RespiratorySignalSourceID, value, pos);
}


OFCondition DRTMotionSynchronizationSequence::Item::setRespiratoryMotionCompensationTechnique(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RespiratoryMotionCompensationTechnique.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTMotionSynchronizationSequence::Item::setRespiratoryMotionCompensationTechniqueDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = RespiratoryMotionCompensationTechniqueDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTMotionSynchronizationSequence::Item::setRespiratorySignalSource(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RespiratorySignalSource.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTMotionSynchronizationSequence::Item::setRespiratorySignalSourceID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RespiratorySignalSourceID.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTMotionSynchronizationSequence::DRTMotionSynchronizationSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTMotionSynchronizationSequence::DRTMotionSynchronizationSequence(const DRTMotionSynchronizationSequence &copy)
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


DRTMotionSynchronizationSequence &DRTMotionSynchronizationSequence::operator=(const DRTMotionSynchronizationSequence &copy)
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


DRTMotionSynchronizationSequence::~DRTMotionSynchronizationSequence()
{
    clear();
}


void DRTMotionSynchronizationSequence::clear()
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


OFBool DRTMotionSynchronizationSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTMotionSynchronizationSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTMotionSynchronizationSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTMotionSynchronizationSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTMotionSynchronizationSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTMotionSynchronizationSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTMotionSynchronizationSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTMotionSynchronizationSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTMotionSynchronizationSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTMotionSynchronizationSequence::Item &DRTMotionSynchronizationSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTMotionSynchronizationSequence::Item &DRTMotionSynchronizationSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTMotionSynchronizationSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTMotionSynchronizationSequence::Item &DRTMotionSynchronizationSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTMotionSynchronizationSequence::Item &DRTMotionSynchronizationSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTMotionSynchronizationSequence::Item &DRTMotionSynchronizationSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTMotionSynchronizationSequence::Item &DRTMotionSynchronizationSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTMotionSynchronizationSequence::addItem(Item *&item)
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


OFCondition DRTMotionSynchronizationSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTMotionSynchronizationSequence::removeItem(const size_t pos)
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


OFCondition DRTMotionSynchronizationSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_MotionSynchronizationSequence, sequence);
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
            DcmSequenceOfItems element(DCM_MotionSynchronizationSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTMotionSynchronizationSequence::write(DcmItem &dataset,
                                                    const OFString &card,
                                                    const OFString &type,
                                                    const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_MotionSynchronizationSequence);
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
