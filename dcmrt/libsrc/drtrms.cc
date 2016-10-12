/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTRangeModulatorSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtrms.h"


// --- item class ---

DRTRangeModulatorSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AccessoryCode(DCM_AccessoryCode),
    BeamCurrentModulationID(DCM_BeamCurrentModulationID),
    RangeModulatorDescription(DCM_RangeModulatorDescription),
    RangeModulatorID(DCM_RangeModulatorID),
    RangeModulatorNumber(DCM_RangeModulatorNumber),
    RangeModulatorType(DCM_RangeModulatorType)
{
}


DRTRangeModulatorSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AccessoryCode(copy.AccessoryCode),
    BeamCurrentModulationID(copy.BeamCurrentModulationID),
    RangeModulatorDescription(copy.RangeModulatorDescription),
    RangeModulatorID(copy.RangeModulatorID),
    RangeModulatorNumber(copy.RangeModulatorNumber),
    RangeModulatorType(copy.RangeModulatorType)
{
}


DRTRangeModulatorSequence::Item::~Item()
{
}


DRTRangeModulatorSequence::Item &DRTRangeModulatorSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AccessoryCode = copy.AccessoryCode;
        BeamCurrentModulationID = copy.BeamCurrentModulationID;
        RangeModulatorDescription = copy.RangeModulatorDescription;
        RangeModulatorID = copy.RangeModulatorID;
        RangeModulatorNumber = copy.RangeModulatorNumber;
        RangeModulatorType = copy.RangeModulatorType;
    }
    return *this;
}


void DRTRangeModulatorSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        RangeModulatorNumber.clear();
        RangeModulatorID.clear();
        AccessoryCode.clear();
        RangeModulatorType.clear();
        RangeModulatorDescription.clear();
        BeamCurrentModulationID.clear();
    }
}


OFBool DRTRangeModulatorSequence::Item::isEmpty()
{
    return RangeModulatorNumber.isEmpty() &&
           RangeModulatorID.isEmpty() &&
           AccessoryCode.isEmpty() &&
           RangeModulatorType.isEmpty() &&
           RangeModulatorDescription.isEmpty() &&
           BeamCurrentModulationID.isEmpty();
}


OFBool DRTRangeModulatorSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTRangeModulatorSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, RangeModulatorNumber, "1", "1", "RangeModulatorSequence");
        getAndCheckElementFromDataset(item, RangeModulatorID, "1", "1", "RangeModulatorSequence");
        getAndCheckElementFromDataset(item, AccessoryCode, "1", "3", "RangeModulatorSequence");
        getAndCheckElementFromDataset(item, RangeModulatorType, "1", "1", "RangeModulatorSequence");
        getAndCheckElementFromDataset(item, RangeModulatorDescription, "1", "3", "RangeModulatorSequence");
        getAndCheckElementFromDataset(item, BeamCurrentModulationID, "1", "1C", "RangeModulatorSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTRangeModulatorSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(RangeModulatorNumber), "1", "1", "RangeModulatorSequence");
        addElementToDataset(result, item, new DcmShortString(RangeModulatorID), "1", "1", "RangeModulatorSequence");
        addElementToDataset(result, item, new DcmLongString(AccessoryCode), "1", "3", "RangeModulatorSequence");
        addElementToDataset(result, item, new DcmCodeString(RangeModulatorType), "1", "1", "RangeModulatorSequence");
        addElementToDataset(result, item, new DcmLongString(RangeModulatorDescription), "1", "3", "RangeModulatorSequence");
        addElementToDataset(result, item, new DcmShortString(BeamCurrentModulationID), "1", "1C", "RangeModulatorSequence");
    }
    return result;
}


OFCondition DRTRangeModulatorSequence::Item::getAccessoryCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AccessoryCode, value, pos);
}


OFCondition DRTRangeModulatorSequence::Item::getBeamCurrentModulationID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamCurrentModulationID, value, pos);
}


OFCondition DRTRangeModulatorSequence::Item::getRangeModulatorDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RangeModulatorDescription, value, pos);
}


OFCondition DRTRangeModulatorSequence::Item::getRangeModulatorID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RangeModulatorID, value, pos);
}


OFCondition DRTRangeModulatorSequence::Item::getRangeModulatorNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RangeModulatorNumber, value, pos);
}


OFCondition DRTRangeModulatorSequence::Item::getRangeModulatorNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, RangeModulatorNumber).getSint32(value, pos);
}


OFCondition DRTRangeModulatorSequence::Item::getRangeModulatorType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RangeModulatorType, value, pos);
}


OFCondition DRTRangeModulatorSequence::Item::setAccessoryCode(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = AccessoryCode.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRangeModulatorSequence::Item::setBeamCurrentModulationID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BeamCurrentModulationID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRangeModulatorSequence::Item::setRangeModulatorDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RangeModulatorDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRangeModulatorSequence::Item::setRangeModulatorID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RangeModulatorID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRangeModulatorSequence::Item::setRangeModulatorNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RangeModulatorNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRangeModulatorSequence::Item::setRangeModulatorType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RangeModulatorType.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTRangeModulatorSequence::DRTRangeModulatorSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTRangeModulatorSequence::DRTRangeModulatorSequence(const DRTRangeModulatorSequence &copy)
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


DRTRangeModulatorSequence &DRTRangeModulatorSequence::operator=(const DRTRangeModulatorSequence &copy)
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


DRTRangeModulatorSequence::~DRTRangeModulatorSequence()
{
    clear();
}


void DRTRangeModulatorSequence::clear()
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


OFBool DRTRangeModulatorSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTRangeModulatorSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTRangeModulatorSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTRangeModulatorSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTRangeModulatorSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTRangeModulatorSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTRangeModulatorSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTRangeModulatorSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTRangeModulatorSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTRangeModulatorSequence::Item &DRTRangeModulatorSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTRangeModulatorSequence::Item &DRTRangeModulatorSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTRangeModulatorSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTRangeModulatorSequence::Item &DRTRangeModulatorSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTRangeModulatorSequence::Item &DRTRangeModulatorSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTRangeModulatorSequence::Item &DRTRangeModulatorSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTRangeModulatorSequence::Item &DRTRangeModulatorSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTRangeModulatorSequence::addItem(Item *&item)
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


OFCondition DRTRangeModulatorSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTRangeModulatorSequence::removeItem(const unsigned long pos)
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


OFCondition DRTRangeModulatorSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_RangeModulatorSequence, sequence);
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
            DcmSequenceOfItems element(DCM_RangeModulatorSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTRangeModulatorSequence::write(DcmItem &dataset,
                                             const OFString &card,
                                             const OFString &type,
                                             const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_RangeModulatorSequence);
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
