/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTReferencedCalculatedDoseReferenceSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtrcdrs.h"


// --- item class ---

DRTReferencedCalculatedDoseReferenceSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    CalculatedDoseReferenceDoseValue(DCM_CalculatedDoseReferenceDoseValue),
    ReferencedCalculatedDoseReferenceNumber(DCM_ReferencedCalculatedDoseReferenceNumber),
    ReferencedDoseReferenceNumber(DCM_ReferencedDoseReferenceNumber)
{
}


DRTReferencedCalculatedDoseReferenceSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    CalculatedDoseReferenceDoseValue(copy.CalculatedDoseReferenceDoseValue),
    ReferencedCalculatedDoseReferenceNumber(copy.ReferencedCalculatedDoseReferenceNumber),
    ReferencedDoseReferenceNumber(copy.ReferencedDoseReferenceNumber)
{
}


DRTReferencedCalculatedDoseReferenceSequence::Item::~Item()
{
}


DRTReferencedCalculatedDoseReferenceSequence::Item &DRTReferencedCalculatedDoseReferenceSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        CalculatedDoseReferenceDoseValue = copy.CalculatedDoseReferenceDoseValue;
        ReferencedCalculatedDoseReferenceNumber = copy.ReferencedCalculatedDoseReferenceNumber;
        ReferencedDoseReferenceNumber = copy.ReferencedDoseReferenceNumber;
    }
    return *this;
}


void DRTReferencedCalculatedDoseReferenceSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ReferencedDoseReferenceNumber.clear();
        ReferencedCalculatedDoseReferenceNumber.clear();
        CalculatedDoseReferenceDoseValue.clear();
    }
}


OFBool DRTReferencedCalculatedDoseReferenceSequence::Item::isEmpty()
{
    return ReferencedDoseReferenceNumber.isEmpty() &&
           ReferencedCalculatedDoseReferenceNumber.isEmpty() &&
           CalculatedDoseReferenceDoseValue.isEmpty();
}


OFBool DRTReferencedCalculatedDoseReferenceSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ReferencedDoseReferenceNumber, "1", "1C", "ReferencedCalculatedDoseReferenceSequence");
        getAndCheckElementFromDataset(item, ReferencedCalculatedDoseReferenceNumber, "1", "1C", "ReferencedCalculatedDoseReferenceSequence");
        getAndCheckElementFromDataset(item, CalculatedDoseReferenceDoseValue, "1", "1", "ReferencedCalculatedDoseReferenceSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ReferencedDoseReferenceNumber), "1", "1C", "ReferencedCalculatedDoseReferenceSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferencedCalculatedDoseReferenceNumber), "1", "1C", "ReferencedCalculatedDoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(CalculatedDoseReferenceDoseValue), "1", "1", "ReferencedCalculatedDoseReferenceSequence");
    }
    return result;
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::Item::getCalculatedDoseReferenceDoseValue(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CalculatedDoseReferenceDoseValue, value, pos);
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::Item::getCalculatedDoseReferenceDoseValue(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CalculatedDoseReferenceDoseValue).getFloat64(value, pos);
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::Item::getReferencedCalculatedDoseReferenceNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedCalculatedDoseReferenceNumber, value, pos);
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::Item::getReferencedCalculatedDoseReferenceNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedCalculatedDoseReferenceNumber).getSint32(value, pos);
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::Item::getReferencedDoseReferenceNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedDoseReferenceNumber, value, pos);
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::Item::getReferencedDoseReferenceNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedDoseReferenceNumber).getSint32(value, pos);
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::Item::setCalculatedDoseReferenceDoseValue(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CalculatedDoseReferenceDoseValue.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::Item::setReferencedCalculatedDoseReferenceNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedCalculatedDoseReferenceNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::Item::setReferencedDoseReferenceNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedDoseReferenceNumber.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTReferencedCalculatedDoseReferenceSequence::DRTReferencedCalculatedDoseReferenceSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTReferencedCalculatedDoseReferenceSequence::DRTReferencedCalculatedDoseReferenceSequence(const DRTReferencedCalculatedDoseReferenceSequence &copy)
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


DRTReferencedCalculatedDoseReferenceSequence &DRTReferencedCalculatedDoseReferenceSequence::operator=(const DRTReferencedCalculatedDoseReferenceSequence &copy)
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


DRTReferencedCalculatedDoseReferenceSequence::~DRTReferencedCalculatedDoseReferenceSequence()
{
    clear();
}


void DRTReferencedCalculatedDoseReferenceSequence::clear()
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


OFBool DRTReferencedCalculatedDoseReferenceSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTReferencedCalculatedDoseReferenceSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTReferencedCalculatedDoseReferenceSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTReferencedCalculatedDoseReferenceSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTReferencedCalculatedDoseReferenceSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTReferencedCalculatedDoseReferenceSequence::Item &DRTReferencedCalculatedDoseReferenceSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTReferencedCalculatedDoseReferenceSequence::Item &DRTReferencedCalculatedDoseReferenceSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTReferencedCalculatedDoseReferenceSequence::Item &DRTReferencedCalculatedDoseReferenceSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTReferencedCalculatedDoseReferenceSequence::Item &DRTReferencedCalculatedDoseReferenceSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTReferencedCalculatedDoseReferenceSequence::Item &DRTReferencedCalculatedDoseReferenceSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTReferencedCalculatedDoseReferenceSequence::Item &DRTReferencedCalculatedDoseReferenceSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::addItem(Item *&item)
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


OFCondition DRTReferencedCalculatedDoseReferenceSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTReferencedCalculatedDoseReferenceSequence::removeItem(const size_t pos)
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


OFCondition DRTReferencedCalculatedDoseReferenceSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ReferencedCalculatedDoseReferenceSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ReferencedCalculatedDoseReferenceSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTReferencedCalculatedDoseReferenceSequence::write(DcmItem &dataset,
                                                                const OFString &card,
                                                                const OFString &type,
                                                                const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ReferencedCalculatedDoseReferenceSequence);
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
