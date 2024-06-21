/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTParallelRTBeamDelimiterDeviceSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtpbdds.h"


// --- item class ---

DRTParallelRTBeamDelimiterDeviceSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    NumberOfParallelRTBeamDelimiters(DCM_NumberOfParallelRTBeamDelimiters),
    ParallelRTBeamDelimiterBoundaries(DCM_ParallelRTBeamDelimiterBoundaries),
    ParallelRTBeamDelimiterDeviceOrientationLabelCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ParallelRTBeamDelimiterLeafMountingSide(DCM_ParallelRTBeamDelimiterLeafMountingSide),
    ParallelRTBeamDelimiterOpeningExtents(DCM_ParallelRTBeamDelimiterOpeningExtents),
    ParallelRTBeamDelimiterOpeningMode(DCM_ParallelRTBeamDelimiterOpeningMode)
{
}


DRTParallelRTBeamDelimiterDeviceSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    NumberOfParallelRTBeamDelimiters(copy.NumberOfParallelRTBeamDelimiters),
    ParallelRTBeamDelimiterBoundaries(copy.ParallelRTBeamDelimiterBoundaries),
    ParallelRTBeamDelimiterDeviceOrientationLabelCodeSequence(copy.ParallelRTBeamDelimiterDeviceOrientationLabelCodeSequence),
    ParallelRTBeamDelimiterLeafMountingSide(copy.ParallelRTBeamDelimiterLeafMountingSide),
    ParallelRTBeamDelimiterOpeningExtents(copy.ParallelRTBeamDelimiterOpeningExtents),
    ParallelRTBeamDelimiterOpeningMode(copy.ParallelRTBeamDelimiterOpeningMode)
{
}


DRTParallelRTBeamDelimiterDeviceSequence::Item::~Item()
{
}


DRTParallelRTBeamDelimiterDeviceSequence::Item &DRTParallelRTBeamDelimiterDeviceSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        NumberOfParallelRTBeamDelimiters = copy.NumberOfParallelRTBeamDelimiters;
        ParallelRTBeamDelimiterBoundaries = copy.ParallelRTBeamDelimiterBoundaries;
        ParallelRTBeamDelimiterDeviceOrientationLabelCodeSequence = copy.ParallelRTBeamDelimiterDeviceOrientationLabelCodeSequence;
        ParallelRTBeamDelimiterLeafMountingSide = copy.ParallelRTBeamDelimiterLeafMountingSide;
        ParallelRTBeamDelimiterOpeningExtents = copy.ParallelRTBeamDelimiterOpeningExtents;
        ParallelRTBeamDelimiterOpeningMode = copy.ParallelRTBeamDelimiterOpeningMode;
    }
    return *this;
}


void DRTParallelRTBeamDelimiterDeviceSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        NumberOfParallelRTBeamDelimiters.clear();
        ParallelRTBeamDelimiterDeviceOrientationLabelCodeSequence.clear();
        ParallelRTBeamDelimiterOpeningMode.clear();
        ParallelRTBeamDelimiterBoundaries.clear();
        ParallelRTBeamDelimiterLeafMountingSide.clear();
        ParallelRTBeamDelimiterOpeningExtents.clear();
    }
}


OFBool DRTParallelRTBeamDelimiterDeviceSequence::Item::isEmpty()
{
    return NumberOfParallelRTBeamDelimiters.isEmpty() &&
           ParallelRTBeamDelimiterDeviceOrientationLabelCodeSequence.isEmpty() &&
           ParallelRTBeamDelimiterOpeningMode.isEmpty() &&
           ParallelRTBeamDelimiterBoundaries.isEmpty() &&
           ParallelRTBeamDelimiterLeafMountingSide.isEmpty() &&
           ParallelRTBeamDelimiterOpeningExtents.isEmpty();
}


OFBool DRTParallelRTBeamDelimiterDeviceSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, NumberOfParallelRTBeamDelimiters, "1", "1", "ParallelRTBeamDelimiterDeviceSequence");
        ParallelRTBeamDelimiterDeviceOrientationLabelCodeSequence.read(item, "1-n", "1", "ParallelRTBeamDelimiterDeviceSequence");
        getAndCheckElementFromDataset(item, ParallelRTBeamDelimiterOpeningMode, "1", "1", "ParallelRTBeamDelimiterDeviceSequence");
        getAndCheckElementFromDataset(item, ParallelRTBeamDelimiterBoundaries, "2-n", "1", "ParallelRTBeamDelimiterDeviceSequence");
        getAndCheckElementFromDataset(item, ParallelRTBeamDelimiterLeafMountingSide, "1-n", "1C", "ParallelRTBeamDelimiterDeviceSequence");
        getAndCheckElementFromDataset(item, ParallelRTBeamDelimiterOpeningExtents, "2-2n", "1C", "ParallelRTBeamDelimiterDeviceSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmUnsignedShort(NumberOfParallelRTBeamDelimiters), "1", "1", "ParallelRTBeamDelimiterDeviceSequence");
        if (result.good()) result = ParallelRTBeamDelimiterDeviceOrientationLabelCodeSequence.write(item, "1-n", "1", "ParallelRTBeamDelimiterDeviceSequence");
        addElementToDataset(result, item, new DcmCodeString(ParallelRTBeamDelimiterOpeningMode), "1", "1", "ParallelRTBeamDelimiterDeviceSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(ParallelRTBeamDelimiterBoundaries), "2-n", "1", "ParallelRTBeamDelimiterDeviceSequence");
        addElementToDataset(result, item, new DcmCodeString(ParallelRTBeamDelimiterLeafMountingSide), "1-n", "1C", "ParallelRTBeamDelimiterDeviceSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(ParallelRTBeamDelimiterOpeningExtents), "2-2n", "1C", "ParallelRTBeamDelimiterDeviceSequence");
    }
    return result;
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::Item::getNumberOfParallelRTBeamDelimiters(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, NumberOfParallelRTBeamDelimiters).getUint16(value, pos);
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::Item::getParallelRTBeamDelimiterBoundaries(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, ParallelRTBeamDelimiterBoundaries).getFloat64(value, pos);
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::Item::getParallelRTBeamDelimiterLeafMountingSide(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ParallelRTBeamDelimiterLeafMountingSide, value, pos);
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::Item::getParallelRTBeamDelimiterOpeningExtents(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, ParallelRTBeamDelimiterOpeningExtents).getFloat64(value, pos);
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::Item::getParallelRTBeamDelimiterOpeningMode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ParallelRTBeamDelimiterOpeningMode, value, pos);
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::Item::setNumberOfParallelRTBeamDelimiters(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return NumberOfParallelRTBeamDelimiters.putUint16(value, pos);
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::Item::setParallelRTBeamDelimiterBoundaries(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ParallelRTBeamDelimiterBoundaries.putFloat64(value, pos);
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::Item::setParallelRTBeamDelimiterLeafMountingSide(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1-n") : EC_Normal;
        if (result.good())
            result = ParallelRTBeamDelimiterLeafMountingSide.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::Item::setParallelRTBeamDelimiterOpeningExtents(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ParallelRTBeamDelimiterOpeningExtents.putFloat64(value, pos);
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::Item::setParallelRTBeamDelimiterOpeningMode(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ParallelRTBeamDelimiterOpeningMode.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTParallelRTBeamDelimiterDeviceSequence::DRTParallelRTBeamDelimiterDeviceSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTParallelRTBeamDelimiterDeviceSequence::DRTParallelRTBeamDelimiterDeviceSequence(const DRTParallelRTBeamDelimiterDeviceSequence &copy)
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


DRTParallelRTBeamDelimiterDeviceSequence &DRTParallelRTBeamDelimiterDeviceSequence::operator=(const DRTParallelRTBeamDelimiterDeviceSequence &copy)
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


DRTParallelRTBeamDelimiterDeviceSequence::~DRTParallelRTBeamDelimiterDeviceSequence()
{
    clear();
}


void DRTParallelRTBeamDelimiterDeviceSequence::clear()
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


OFBool DRTParallelRTBeamDelimiterDeviceSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTParallelRTBeamDelimiterDeviceSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTParallelRTBeamDelimiterDeviceSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (++CurrentItem != SequenceOfItems.end())
    {
        if (*CurrentItem != NULL)
            result = EC_Normal;
        else
            result = EC_CorruptedData;
    }
    return result;
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTParallelRTBeamDelimiterDeviceSequence::Item &DRTParallelRTBeamDelimiterDeviceSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTParallelRTBeamDelimiterDeviceSequence::Item &DRTParallelRTBeamDelimiterDeviceSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTParallelRTBeamDelimiterDeviceSequence::Item &DRTParallelRTBeamDelimiterDeviceSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTParallelRTBeamDelimiterDeviceSequence::Item &DRTParallelRTBeamDelimiterDeviceSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTParallelRTBeamDelimiterDeviceSequence::Item &DRTParallelRTBeamDelimiterDeviceSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTParallelRTBeamDelimiterDeviceSequence::Item &DRTParallelRTBeamDelimiterDeviceSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::addItem(Item *&item)
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


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::removeItem(const size_t pos)
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


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ParallelRTBeamDelimiterDeviceSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ParallelRTBeamDelimiterDeviceSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTParallelRTBeamDelimiterDeviceSequence::write(DcmItem &dataset,
                                                            const OFString &card,
                                                            const OFString &type,
                                                            const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ParallelRTBeamDelimiterDeviceSequence);
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
