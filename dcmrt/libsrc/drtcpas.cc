/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTCorrectedParameterSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtcpas.h"


// --- item class ---

DRTCorrectedParameterSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    CorrectionValue(DCM_CorrectionValue),
    ParameterItemIndex(DCM_ParameterItemIndex),
    ParameterPointer(DCM_ParameterPointer),
    ParameterSequencePointer(DCM_ParameterSequencePointer)
{
}


DRTCorrectedParameterSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    CorrectionValue(copy.CorrectionValue),
    ParameterItemIndex(copy.ParameterItemIndex),
    ParameterPointer(copy.ParameterPointer),
    ParameterSequencePointer(copy.ParameterSequencePointer)
{
}


DRTCorrectedParameterSequence::Item::~Item()
{
}


DRTCorrectedParameterSequence::Item &DRTCorrectedParameterSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        CorrectionValue = copy.CorrectionValue;
        ParameterItemIndex = copy.ParameterItemIndex;
        ParameterPointer = copy.ParameterPointer;
        ParameterSequencePointer = copy.ParameterSequencePointer;
    }
    return *this;
}


void DRTCorrectedParameterSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ParameterSequencePointer.clear();
        ParameterItemIndex.clear();
        ParameterPointer.clear();
        CorrectionValue.clear();
    }
}


OFBool DRTCorrectedParameterSequence::Item::isEmpty()
{
    return ParameterSequencePointer.isEmpty() &&
           ParameterItemIndex.isEmpty() &&
           ParameterPointer.isEmpty() &&
           CorrectionValue.isEmpty();
}


OFBool DRTCorrectedParameterSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTCorrectedParameterSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ParameterSequencePointer, "1", "1", "CorrectedParameterSequence");
        getAndCheckElementFromDataset(item, ParameterItemIndex, "1", "1", "CorrectedParameterSequence");
        getAndCheckElementFromDataset(item, ParameterPointer, "1", "1", "CorrectedParameterSequence");
        getAndCheckElementFromDataset(item, CorrectionValue, "1", "1", "CorrectedParameterSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTCorrectedParameterSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmAttributeTag(ParameterSequencePointer), "1", "1", "CorrectedParameterSequence");
        addElementToDataset(result, item, new DcmIntegerString(ParameterItemIndex), "1", "1", "CorrectedParameterSequence");
        addElementToDataset(result, item, new DcmAttributeTag(ParameterPointer), "1", "1", "CorrectedParameterSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(CorrectionValue), "1", "1", "CorrectedParameterSequence");
    }
    return result;
}


OFCondition DRTCorrectedParameterSequence::Item::getCorrectionValue(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, CorrectionValue).getFloat32(value, pos);
}


OFCondition DRTCorrectedParameterSequence::Item::getParameterItemIndex(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ParameterItemIndex, value, pos);
}


OFCondition DRTCorrectedParameterSequence::Item::getParameterItemIndex(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ParameterItemIndex).getSint32(value, pos);
}


OFCondition DRTCorrectedParameterSequence::Item::getParameterPointer(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ParameterPointer, value, pos);
}


OFCondition DRTCorrectedParameterSequence::Item::getParameterSequencePointer(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ParameterSequencePointer, value, pos);
}


OFCondition DRTCorrectedParameterSequence::Item::setCorrectionValue(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return CorrectionValue.putFloat32(value, pos);
}


OFCondition DRTCorrectedParameterSequence::Item::setParameterItemIndex(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ParameterItemIndex.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTCorrectedParameterSequence::Item::setParameterPointer(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmAttributeTag::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ParameterPointer.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTCorrectedParameterSequence::Item::setParameterSequencePointer(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmAttributeTag::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ParameterSequencePointer.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTCorrectedParameterSequence::DRTCorrectedParameterSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTCorrectedParameterSequence::DRTCorrectedParameterSequence(const DRTCorrectedParameterSequence &copy)
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


DRTCorrectedParameterSequence &DRTCorrectedParameterSequence::operator=(const DRTCorrectedParameterSequence &copy)
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


DRTCorrectedParameterSequence::~DRTCorrectedParameterSequence()
{
    clear();
}


void DRTCorrectedParameterSequence::clear()
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


OFBool DRTCorrectedParameterSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTCorrectedParameterSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTCorrectedParameterSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTCorrectedParameterSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTCorrectedParameterSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTCorrectedParameterSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTCorrectedParameterSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTCorrectedParameterSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTCorrectedParameterSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTCorrectedParameterSequence::Item &DRTCorrectedParameterSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTCorrectedParameterSequence::Item &DRTCorrectedParameterSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTCorrectedParameterSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTCorrectedParameterSequence::Item &DRTCorrectedParameterSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTCorrectedParameterSequence::Item &DRTCorrectedParameterSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTCorrectedParameterSequence::Item &DRTCorrectedParameterSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTCorrectedParameterSequence::Item &DRTCorrectedParameterSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTCorrectedParameterSequence::addItem(Item *&item)
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


OFCondition DRTCorrectedParameterSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTCorrectedParameterSequence::removeItem(const unsigned long pos)
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


OFCondition DRTCorrectedParameterSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_CorrectedParameterSequence, sequence);
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
            DcmSequenceOfItems element(DCM_CorrectedParameterSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTCorrectedParameterSequence::write(DcmItem &dataset,
                                                 const OFString &card,
                                                 const OFString &type,
                                                 const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_CorrectedParameterSequence);
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
