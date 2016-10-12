/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTMeasuredDoseReferenceSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtmdrs.h"


// --- item class ---

DRTMeasuredDoseReferenceSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    DoseUnits(DCM_DoseUnits),
    MeasuredDoseDescription(DCM_MeasuredDoseDescription),
    MeasuredDoseReferenceNumber(DCM_MeasuredDoseReferenceNumber),
    MeasuredDoseType(DCM_MeasuredDoseType),
    MeasuredDoseValue(DCM_MeasuredDoseValue),
    ReferencedDoseReferenceNumber(DCM_ReferencedDoseReferenceNumber)
{
}


DRTMeasuredDoseReferenceSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    DoseUnits(copy.DoseUnits),
    MeasuredDoseDescription(copy.MeasuredDoseDescription),
    MeasuredDoseReferenceNumber(copy.MeasuredDoseReferenceNumber),
    MeasuredDoseType(copy.MeasuredDoseType),
    MeasuredDoseValue(copy.MeasuredDoseValue),
    ReferencedDoseReferenceNumber(copy.ReferencedDoseReferenceNumber)
{
}


DRTMeasuredDoseReferenceSequence::Item::~Item()
{
}


DRTMeasuredDoseReferenceSequence::Item &DRTMeasuredDoseReferenceSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        DoseUnits = copy.DoseUnits;
        MeasuredDoseDescription = copy.MeasuredDoseDescription;
        MeasuredDoseReferenceNumber = copy.MeasuredDoseReferenceNumber;
        MeasuredDoseType = copy.MeasuredDoseType;
        MeasuredDoseValue = copy.MeasuredDoseValue;
        ReferencedDoseReferenceNumber = copy.ReferencedDoseReferenceNumber;
    }
    return *this;
}


void DRTMeasuredDoseReferenceSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ReferencedDoseReferenceNumber.clear();
        MeasuredDoseReferenceNumber.clear();
        DoseUnits.clear();
        MeasuredDoseValue.clear();
        MeasuredDoseType.clear();
        MeasuredDoseDescription.clear();
    }
}


OFBool DRTMeasuredDoseReferenceSequence::Item::isEmpty()
{
    return ReferencedDoseReferenceNumber.isEmpty() &&
           MeasuredDoseReferenceNumber.isEmpty() &&
           DoseUnits.isEmpty() &&
           MeasuredDoseValue.isEmpty() &&
           MeasuredDoseType.isEmpty() &&
           MeasuredDoseDescription.isEmpty();
}


OFBool DRTMeasuredDoseReferenceSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTMeasuredDoseReferenceSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ReferencedDoseReferenceNumber, "1", "1C", "MeasuredDoseReferenceSequence");
        getAndCheckElementFromDataset(item, MeasuredDoseReferenceNumber, "1", "1C", "MeasuredDoseReferenceSequence");
        getAndCheckElementFromDataset(item, DoseUnits, "1", "1", "MeasuredDoseReferenceSequence");
        getAndCheckElementFromDataset(item, MeasuredDoseValue, "1", "2", "MeasuredDoseReferenceSequence");
        getAndCheckElementFromDataset(item, MeasuredDoseType, "1", "2", "MeasuredDoseReferenceSequence");
        getAndCheckElementFromDataset(item, MeasuredDoseDescription, "1", "3", "MeasuredDoseReferenceSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTMeasuredDoseReferenceSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ReferencedDoseReferenceNumber), "1", "1C", "MeasuredDoseReferenceSequence");
        addElementToDataset(result, item, new DcmIntegerString(MeasuredDoseReferenceNumber), "1", "1C", "MeasuredDoseReferenceSequence");
        addElementToDataset(result, item, new DcmCodeString(DoseUnits), "1", "1", "MeasuredDoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(MeasuredDoseValue), "1", "2", "MeasuredDoseReferenceSequence");
        addElementToDataset(result, item, new DcmCodeString(MeasuredDoseType), "1", "2", "MeasuredDoseReferenceSequence");
        addElementToDataset(result, item, new DcmShortText(MeasuredDoseDescription), "1", "3", "MeasuredDoseReferenceSequence");
    }
    return result;
}


OFCondition DRTMeasuredDoseReferenceSequence::Item::getDoseUnits(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DoseUnits, value, pos);
}


OFCondition DRTMeasuredDoseReferenceSequence::Item::getMeasuredDoseDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MeasuredDoseDescription, value, pos);
}


OFCondition DRTMeasuredDoseReferenceSequence::Item::getMeasuredDoseReferenceNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MeasuredDoseReferenceNumber, value, pos);
}


OFCondition DRTMeasuredDoseReferenceSequence::Item::getMeasuredDoseReferenceNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, MeasuredDoseReferenceNumber).getSint32(value, pos);
}


OFCondition DRTMeasuredDoseReferenceSequence::Item::getMeasuredDoseType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MeasuredDoseType, value, pos);
}


OFCondition DRTMeasuredDoseReferenceSequence::Item::getMeasuredDoseValue(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MeasuredDoseValue, value, pos);
}


OFCondition DRTMeasuredDoseReferenceSequence::Item::getMeasuredDoseValue(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, MeasuredDoseValue).getFloat64(value, pos);
}


OFCondition DRTMeasuredDoseReferenceSequence::Item::getReferencedDoseReferenceNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedDoseReferenceNumber, value, pos);
}


OFCondition DRTMeasuredDoseReferenceSequence::Item::getReferencedDoseReferenceNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedDoseReferenceNumber).getSint32(value, pos);
}


OFCondition DRTMeasuredDoseReferenceSequence::Item::setDoseUnits(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DoseUnits.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTMeasuredDoseReferenceSequence::Item::setMeasuredDoseDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = MeasuredDoseDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTMeasuredDoseReferenceSequence::Item::setMeasuredDoseReferenceNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = MeasuredDoseReferenceNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTMeasuredDoseReferenceSequence::Item::setMeasuredDoseType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = MeasuredDoseType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTMeasuredDoseReferenceSequence::Item::setMeasuredDoseValue(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = MeasuredDoseValue.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTMeasuredDoseReferenceSequence::Item::setReferencedDoseReferenceNumber(const OFString &value, const OFBool check)
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

DRTMeasuredDoseReferenceSequence::DRTMeasuredDoseReferenceSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTMeasuredDoseReferenceSequence::DRTMeasuredDoseReferenceSequence(const DRTMeasuredDoseReferenceSequence &copy)
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


DRTMeasuredDoseReferenceSequence &DRTMeasuredDoseReferenceSequence::operator=(const DRTMeasuredDoseReferenceSequence &copy)
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


DRTMeasuredDoseReferenceSequence::~DRTMeasuredDoseReferenceSequence()
{
    clear();
}


void DRTMeasuredDoseReferenceSequence::clear()
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


OFBool DRTMeasuredDoseReferenceSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTMeasuredDoseReferenceSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTMeasuredDoseReferenceSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTMeasuredDoseReferenceSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTMeasuredDoseReferenceSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTMeasuredDoseReferenceSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTMeasuredDoseReferenceSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTMeasuredDoseReferenceSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTMeasuredDoseReferenceSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTMeasuredDoseReferenceSequence::Item &DRTMeasuredDoseReferenceSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTMeasuredDoseReferenceSequence::Item &DRTMeasuredDoseReferenceSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTMeasuredDoseReferenceSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTMeasuredDoseReferenceSequence::Item &DRTMeasuredDoseReferenceSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTMeasuredDoseReferenceSequence::Item &DRTMeasuredDoseReferenceSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTMeasuredDoseReferenceSequence::Item &DRTMeasuredDoseReferenceSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTMeasuredDoseReferenceSequence::Item &DRTMeasuredDoseReferenceSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTMeasuredDoseReferenceSequence::addItem(Item *&item)
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


OFCondition DRTMeasuredDoseReferenceSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTMeasuredDoseReferenceSequence::removeItem(const unsigned long pos)
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


OFCondition DRTMeasuredDoseReferenceSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_MeasuredDoseReferenceSequence, sequence);
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
            DcmSequenceOfItems element(DCM_MeasuredDoseReferenceSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTMeasuredDoseReferenceSequence::write(DcmItem &dataset,
                                                    const OFString &card,
                                                    const OFString &type,
                                                    const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_MeasuredDoseReferenceSequence);
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
