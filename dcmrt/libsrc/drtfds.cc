/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTFixationDeviceSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtfds.h"


// --- item class ---

DRTFixationDeviceSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AccessoryCode(DCM_AccessoryCode),
    FixationDeviceDescription(DCM_FixationDeviceDescription),
    FixationDeviceLabel(DCM_FixationDeviceLabel),
    FixationDevicePitchAngle(DCM_FixationDevicePitchAngle),
    FixationDevicePosition(DCM_FixationDevicePosition),
    FixationDeviceRollAngle(DCM_FixationDeviceRollAngle),
    FixationDeviceType(DCM_FixationDeviceType)
{
}


DRTFixationDeviceSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AccessoryCode(copy.AccessoryCode),
    FixationDeviceDescription(copy.FixationDeviceDescription),
    FixationDeviceLabel(copy.FixationDeviceLabel),
    FixationDevicePitchAngle(copy.FixationDevicePitchAngle),
    FixationDevicePosition(copy.FixationDevicePosition),
    FixationDeviceRollAngle(copy.FixationDeviceRollAngle),
    FixationDeviceType(copy.FixationDeviceType)
{
}


DRTFixationDeviceSequence::Item::~Item()
{
}


DRTFixationDeviceSequence::Item &DRTFixationDeviceSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AccessoryCode = copy.AccessoryCode;
        FixationDeviceDescription = copy.FixationDeviceDescription;
        FixationDeviceLabel = copy.FixationDeviceLabel;
        FixationDevicePitchAngle = copy.FixationDevicePitchAngle;
        FixationDevicePosition = copy.FixationDevicePosition;
        FixationDeviceRollAngle = copy.FixationDeviceRollAngle;
        FixationDeviceType = copy.FixationDeviceType;
    }
    return *this;
}


void DRTFixationDeviceSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        FixationDeviceType.clear();
        FixationDeviceLabel.clear();
        FixationDeviceDescription.clear();
        FixationDevicePosition.clear();
        FixationDevicePitchAngle.clear();
        FixationDeviceRollAngle.clear();
        AccessoryCode.clear();
    }
}


OFBool DRTFixationDeviceSequence::Item::isEmpty()
{
    return FixationDeviceType.isEmpty() &&
           FixationDeviceLabel.isEmpty() &&
           FixationDeviceDescription.isEmpty() &&
           FixationDevicePosition.isEmpty() &&
           FixationDevicePitchAngle.isEmpty() &&
           FixationDeviceRollAngle.isEmpty() &&
           AccessoryCode.isEmpty();
}


OFBool DRTFixationDeviceSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTFixationDeviceSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, FixationDeviceType, "1", "1", "FixationDeviceSequence");
        getAndCheckElementFromDataset(item, FixationDeviceLabel, "1", "2", "FixationDeviceSequence");
        getAndCheckElementFromDataset(item, FixationDeviceDescription, "1", "3", "FixationDeviceSequence");
        getAndCheckElementFromDataset(item, FixationDevicePosition, "1", "3", "FixationDeviceSequence");
        getAndCheckElementFromDataset(item, FixationDevicePitchAngle, "1", "3", "FixationDeviceSequence");
        getAndCheckElementFromDataset(item, FixationDeviceRollAngle, "1", "3", "FixationDeviceSequence");
        getAndCheckElementFromDataset(item, AccessoryCode, "1", "3", "FixationDeviceSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTFixationDeviceSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmCodeString(FixationDeviceType), "1", "1", "FixationDeviceSequence");
        addElementToDataset(result, item, new DcmShortString(FixationDeviceLabel), "1", "2", "FixationDeviceSequence");
        addElementToDataset(result, item, new DcmShortText(FixationDeviceDescription), "1", "3", "FixationDeviceSequence");
        addElementToDataset(result, item, new DcmShortString(FixationDevicePosition), "1", "3", "FixationDeviceSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(FixationDevicePitchAngle), "1", "3", "FixationDeviceSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(FixationDeviceRollAngle), "1", "3", "FixationDeviceSequence");
        addElementToDataset(result, item, new DcmLongString(AccessoryCode), "1", "3", "FixationDeviceSequence");
    }
    return result;
}


OFCondition DRTFixationDeviceSequence::Item::getAccessoryCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AccessoryCode, value, pos);
}


OFCondition DRTFixationDeviceSequence::Item::getFixationDeviceDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FixationDeviceDescription, value, pos);
}


OFCondition DRTFixationDeviceSequence::Item::getFixationDeviceLabel(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FixationDeviceLabel, value, pos);
}


OFCondition DRTFixationDeviceSequence::Item::getFixationDevicePitchAngle(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, FixationDevicePitchAngle).getFloat32(value, pos);
}


OFCondition DRTFixationDeviceSequence::Item::getFixationDevicePosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FixationDevicePosition, value, pos);
}


OFCondition DRTFixationDeviceSequence::Item::getFixationDeviceRollAngle(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, FixationDeviceRollAngle).getFloat32(value, pos);
}


OFCondition DRTFixationDeviceSequence::Item::getFixationDeviceType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FixationDeviceType, value, pos);
}


OFCondition DRTFixationDeviceSequence::Item::setAccessoryCode(const OFString &value, const OFBool check)
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


OFCondition DRTFixationDeviceSequence::Item::setFixationDeviceDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = FixationDeviceDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFixationDeviceSequence::Item::setFixationDeviceLabel(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = FixationDeviceLabel.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFixationDeviceSequence::Item::setFixationDevicePitchAngle(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return FixationDevicePitchAngle.putFloat32(value, pos);
}


OFCondition DRTFixationDeviceSequence::Item::setFixationDevicePosition(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = FixationDevicePosition.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFixationDeviceSequence::Item::setFixationDeviceRollAngle(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return FixationDeviceRollAngle.putFloat32(value, pos);
}


OFCondition DRTFixationDeviceSequence::Item::setFixationDeviceType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = FixationDeviceType.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTFixationDeviceSequence::DRTFixationDeviceSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTFixationDeviceSequence::DRTFixationDeviceSequence(const DRTFixationDeviceSequence &copy)
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


DRTFixationDeviceSequence &DRTFixationDeviceSequence::operator=(const DRTFixationDeviceSequence &copy)
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


DRTFixationDeviceSequence::~DRTFixationDeviceSequence()
{
    clear();
}


void DRTFixationDeviceSequence::clear()
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


OFBool DRTFixationDeviceSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTFixationDeviceSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTFixationDeviceSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTFixationDeviceSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTFixationDeviceSequence::gotoNextItem()
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


OFCondition DRTFixationDeviceSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTFixationDeviceSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTFixationDeviceSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTFixationDeviceSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTFixationDeviceSequence::Item &DRTFixationDeviceSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTFixationDeviceSequence::Item &DRTFixationDeviceSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTFixationDeviceSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTFixationDeviceSequence::Item &DRTFixationDeviceSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTFixationDeviceSequence::Item &DRTFixationDeviceSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTFixationDeviceSequence::Item &DRTFixationDeviceSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTFixationDeviceSequence::Item &DRTFixationDeviceSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTFixationDeviceSequence::addItem(Item *&item)
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


OFCondition DRTFixationDeviceSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTFixationDeviceSequence::removeItem(const size_t pos)
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


OFCondition DRTFixationDeviceSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_FixationDeviceSequence, sequence);
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
            DcmSequenceOfItems element(DCM_FixationDeviceSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTFixationDeviceSequence::write(DcmItem &dataset,
                                             const OFString &card,
                                             const OFString &type,
                                             const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_FixationDeviceSequence);
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
