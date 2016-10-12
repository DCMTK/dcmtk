/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTIonBeamLimitingDeviceSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtiblds.h"


// --- item class ---

DRTIonBeamLimitingDeviceSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    IsocenterToBeamLimitingDeviceDistance(DCM_IsocenterToBeamLimitingDeviceDistance),
    LeafPositionBoundaries(DCM_LeafPositionBoundaries),
    NumberOfLeafJawPairs(DCM_NumberOfLeafJawPairs),
    RTBeamLimitingDeviceType(DCM_RTBeamLimitingDeviceType)
{
}


DRTIonBeamLimitingDeviceSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    IsocenterToBeamLimitingDeviceDistance(copy.IsocenterToBeamLimitingDeviceDistance),
    LeafPositionBoundaries(copy.LeafPositionBoundaries),
    NumberOfLeafJawPairs(copy.NumberOfLeafJawPairs),
    RTBeamLimitingDeviceType(copy.RTBeamLimitingDeviceType)
{
}


DRTIonBeamLimitingDeviceSequence::Item::~Item()
{
}


DRTIonBeamLimitingDeviceSequence::Item &DRTIonBeamLimitingDeviceSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        IsocenterToBeamLimitingDeviceDistance = copy.IsocenterToBeamLimitingDeviceDistance;
        LeafPositionBoundaries = copy.LeafPositionBoundaries;
        NumberOfLeafJawPairs = copy.NumberOfLeafJawPairs;
        RTBeamLimitingDeviceType = copy.RTBeamLimitingDeviceType;
    }
    return *this;
}


void DRTIonBeamLimitingDeviceSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        RTBeamLimitingDeviceType.clear();
        IsocenterToBeamLimitingDeviceDistance.clear();
        NumberOfLeafJawPairs.clear();
        LeafPositionBoundaries.clear();
    }
}


OFBool DRTIonBeamLimitingDeviceSequence::Item::isEmpty()
{
    return RTBeamLimitingDeviceType.isEmpty() &&
           IsocenterToBeamLimitingDeviceDistance.isEmpty() &&
           NumberOfLeafJawPairs.isEmpty() &&
           LeafPositionBoundaries.isEmpty();
}


OFBool DRTIonBeamLimitingDeviceSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTIonBeamLimitingDeviceSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, RTBeamLimitingDeviceType, "1", "1", "IonBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, IsocenterToBeamLimitingDeviceDistance, "1", "2", "IonBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, NumberOfLeafJawPairs, "1", "1", "IonBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, LeafPositionBoundaries, "3-n", "1C", "IonBeamLimitingDeviceSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonBeamLimitingDeviceSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmCodeString(RTBeamLimitingDeviceType), "1", "1", "IonBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(IsocenterToBeamLimitingDeviceDistance), "1", "2", "IonBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfLeafJawPairs), "1", "1", "IonBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmDecimalString(LeafPositionBoundaries), "3-n", "1C", "IonBeamLimitingDeviceSequence");
    }
    return result;
}


OFCondition DRTIonBeamLimitingDeviceSequence::Item::getIsocenterToBeamLimitingDeviceDistance(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, IsocenterToBeamLimitingDeviceDistance).getFloat32(value, pos);
}


OFCondition DRTIonBeamLimitingDeviceSequence::Item::getLeafPositionBoundaries(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(LeafPositionBoundaries, value, pos);
}


OFCondition DRTIonBeamLimitingDeviceSequence::Item::getLeafPositionBoundaries(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, LeafPositionBoundaries).getFloat64(value, pos);
}


OFCondition DRTIonBeamLimitingDeviceSequence::Item::getLeafPositionBoundaries(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, LeafPositionBoundaries).getFloat64Vector(value);
}


OFCondition DRTIonBeamLimitingDeviceSequence::Item::getNumberOfLeafJawPairs(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfLeafJawPairs, value, pos);
}


OFCondition DRTIonBeamLimitingDeviceSequence::Item::getNumberOfLeafJawPairs(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfLeafJawPairs).getSint32(value, pos);
}


OFCondition DRTIonBeamLimitingDeviceSequence::Item::getRTBeamLimitingDeviceType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RTBeamLimitingDeviceType, value, pos);
}


OFCondition DRTIonBeamLimitingDeviceSequence::Item::setIsocenterToBeamLimitingDeviceDistance(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return IsocenterToBeamLimitingDeviceDistance.putFloat32(value, pos);
}


OFCondition DRTIonBeamLimitingDeviceSequence::Item::setLeafPositionBoundaries(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "3-n") : EC_Normal;
        if (result.good())
            result = LeafPositionBoundaries.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamLimitingDeviceSequence::Item::setNumberOfLeafJawPairs(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfLeafJawPairs.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamLimitingDeviceSequence::Item::setRTBeamLimitingDeviceType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RTBeamLimitingDeviceType.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTIonBeamLimitingDeviceSequence::DRTIonBeamLimitingDeviceSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTIonBeamLimitingDeviceSequence::DRTIonBeamLimitingDeviceSequence(const DRTIonBeamLimitingDeviceSequence &copy)
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


DRTIonBeamLimitingDeviceSequence &DRTIonBeamLimitingDeviceSequence::operator=(const DRTIonBeamLimitingDeviceSequence &copy)
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


DRTIonBeamLimitingDeviceSequence::~DRTIonBeamLimitingDeviceSequence()
{
    clear();
}


void DRTIonBeamLimitingDeviceSequence::clear()
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


OFBool DRTIonBeamLimitingDeviceSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTIonBeamLimitingDeviceSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTIonBeamLimitingDeviceSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTIonBeamLimitingDeviceSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonBeamLimitingDeviceSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonBeamLimitingDeviceSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTIonBeamLimitingDeviceSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTIonBeamLimitingDeviceSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTIonBeamLimitingDeviceSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTIonBeamLimitingDeviceSequence::Item &DRTIonBeamLimitingDeviceSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTIonBeamLimitingDeviceSequence::Item &DRTIonBeamLimitingDeviceSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTIonBeamLimitingDeviceSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTIonBeamLimitingDeviceSequence::Item &DRTIonBeamLimitingDeviceSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTIonBeamLimitingDeviceSequence::Item &DRTIonBeamLimitingDeviceSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTIonBeamLimitingDeviceSequence::Item &DRTIonBeamLimitingDeviceSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTIonBeamLimitingDeviceSequence::Item &DRTIonBeamLimitingDeviceSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTIonBeamLimitingDeviceSequence::addItem(Item *&item)
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


OFCondition DRTIonBeamLimitingDeviceSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTIonBeamLimitingDeviceSequence::removeItem(const unsigned long pos)
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


OFCondition DRTIonBeamLimitingDeviceSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_IonBeamLimitingDeviceSequence, sequence);
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
            DcmSequenceOfItems element(DCM_IonBeamLimitingDeviceSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTIonBeamLimitingDeviceSequence::write(DcmItem &dataset,
                                                    const OFString &card,
                                                    const OFString &type,
                                                    const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_IonBeamLimitingDeviceSequence);
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
