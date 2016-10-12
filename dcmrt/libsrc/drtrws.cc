/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTRecordedWedgeSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtrws.h"


// --- item class ---

DRTRecordedWedgeSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AccessoryCode(DCM_AccessoryCode),
    WedgeAngle(DCM_WedgeAngle),
    WedgeID(DCM_WedgeID),
    WedgeNumber(DCM_WedgeNumber),
    WedgeOrientation(DCM_WedgeOrientation),
    WedgeType(DCM_WedgeType)
{
}


DRTRecordedWedgeSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AccessoryCode(copy.AccessoryCode),
    WedgeAngle(copy.WedgeAngle),
    WedgeID(copy.WedgeID),
    WedgeNumber(copy.WedgeNumber),
    WedgeOrientation(copy.WedgeOrientation),
    WedgeType(copy.WedgeType)
{
}


DRTRecordedWedgeSequence::Item::~Item()
{
}


DRTRecordedWedgeSequence::Item &DRTRecordedWedgeSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AccessoryCode = copy.AccessoryCode;
        WedgeAngle = copy.WedgeAngle;
        WedgeID = copy.WedgeID;
        WedgeNumber = copy.WedgeNumber;
        WedgeOrientation = copy.WedgeOrientation;
        WedgeType = copy.WedgeType;
    }
    return *this;
}


void DRTRecordedWedgeSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        WedgeNumber.clear();
        WedgeType.clear();
        WedgeID.clear();
        AccessoryCode.clear();
        WedgeAngle.clear();
        WedgeOrientation.clear();
    }
}


OFBool DRTRecordedWedgeSequence::Item::isEmpty()
{
    return WedgeNumber.isEmpty() &&
           WedgeType.isEmpty() &&
           WedgeID.isEmpty() &&
           AccessoryCode.isEmpty() &&
           WedgeAngle.isEmpty() &&
           WedgeOrientation.isEmpty();
}


OFBool DRTRecordedWedgeSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTRecordedWedgeSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, WedgeNumber, "1", "1", "RecordedWedgeSequence");
        getAndCheckElementFromDataset(item, WedgeType, "1", "2", "RecordedWedgeSequence");
        getAndCheckElementFromDataset(item, WedgeID, "1", "3", "RecordedWedgeSequence");
        getAndCheckElementFromDataset(item, AccessoryCode, "1", "3", "RecordedWedgeSequence");
        getAndCheckElementFromDataset(item, WedgeAngle, "1", "2", "RecordedWedgeSequence");
        getAndCheckElementFromDataset(item, WedgeOrientation, "1", "2", "RecordedWedgeSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTRecordedWedgeSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(WedgeNumber), "1", "1", "RecordedWedgeSequence");
        addElementToDataset(result, item, new DcmCodeString(WedgeType), "1", "2", "RecordedWedgeSequence");
        addElementToDataset(result, item, new DcmShortString(WedgeID), "1", "3", "RecordedWedgeSequence");
        addElementToDataset(result, item, new DcmLongString(AccessoryCode), "1", "3", "RecordedWedgeSequence");
        addElementToDataset(result, item, new DcmIntegerString(WedgeAngle), "1", "2", "RecordedWedgeSequence");
        addElementToDataset(result, item, new DcmDecimalString(WedgeOrientation), "1", "2", "RecordedWedgeSequence");
    }
    return result;
}


OFCondition DRTRecordedWedgeSequence::Item::getAccessoryCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AccessoryCode, value, pos);
}


OFCondition DRTRecordedWedgeSequence::Item::getWedgeAngle(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(WedgeAngle, value, pos);
}


OFCondition DRTRecordedWedgeSequence::Item::getWedgeAngle(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, WedgeAngle).getSint32(value, pos);
}


OFCondition DRTRecordedWedgeSequence::Item::getWedgeID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(WedgeID, value, pos);
}


OFCondition DRTRecordedWedgeSequence::Item::getWedgeNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(WedgeNumber, value, pos);
}


OFCondition DRTRecordedWedgeSequence::Item::getWedgeNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, WedgeNumber).getSint32(value, pos);
}


OFCondition DRTRecordedWedgeSequence::Item::getWedgeOrientation(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(WedgeOrientation, value, pos);
}


OFCondition DRTRecordedWedgeSequence::Item::getWedgeOrientation(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, WedgeOrientation).getFloat64(value, pos);
}


OFCondition DRTRecordedWedgeSequence::Item::getWedgeType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(WedgeType, value, pos);
}


OFCondition DRTRecordedWedgeSequence::Item::setAccessoryCode(const OFString &value, const OFBool check)
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


OFCondition DRTRecordedWedgeSequence::Item::setWedgeAngle(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = WedgeAngle.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRecordedWedgeSequence::Item::setWedgeID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = WedgeID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRecordedWedgeSequence::Item::setWedgeNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = WedgeNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRecordedWedgeSequence::Item::setWedgeOrientation(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = WedgeOrientation.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRecordedWedgeSequence::Item::setWedgeType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = WedgeType.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTRecordedWedgeSequence::DRTRecordedWedgeSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTRecordedWedgeSequence::DRTRecordedWedgeSequence(const DRTRecordedWedgeSequence &copy)
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


DRTRecordedWedgeSequence &DRTRecordedWedgeSequence::operator=(const DRTRecordedWedgeSequence &copy)
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


DRTRecordedWedgeSequence::~DRTRecordedWedgeSequence()
{
    clear();
}


void DRTRecordedWedgeSequence::clear()
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


OFBool DRTRecordedWedgeSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTRecordedWedgeSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTRecordedWedgeSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTRecordedWedgeSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTRecordedWedgeSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTRecordedWedgeSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTRecordedWedgeSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTRecordedWedgeSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTRecordedWedgeSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTRecordedWedgeSequence::Item &DRTRecordedWedgeSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTRecordedWedgeSequence::Item &DRTRecordedWedgeSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTRecordedWedgeSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTRecordedWedgeSequence::Item &DRTRecordedWedgeSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTRecordedWedgeSequence::Item &DRTRecordedWedgeSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTRecordedWedgeSequence::Item &DRTRecordedWedgeSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTRecordedWedgeSequence::Item &DRTRecordedWedgeSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTRecordedWedgeSequence::addItem(Item *&item)
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


OFCondition DRTRecordedWedgeSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTRecordedWedgeSequence::removeItem(const unsigned long pos)
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


OFCondition DRTRecordedWedgeSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_RecordedWedgeSequence, sequence);
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
            DcmSequenceOfItems element(DCM_RecordedWedgeSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTRecordedWedgeSequence::write(DcmItem &dataset,
                                            const OFString &card,
                                            const OFString &type,
                                            const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_RecordedWedgeSequence);
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
