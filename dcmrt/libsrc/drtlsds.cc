/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTLateralSpreadingDeviceSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtlsds.h"


// --- item class ---

DRTLateralSpreadingDeviceSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AccessoryCode(DCM_AccessoryCode),
    LateralSpreadingDeviceDescription(DCM_LateralSpreadingDeviceDescription),
    LateralSpreadingDeviceID(DCM_LateralSpreadingDeviceID),
    LateralSpreadingDeviceNumber(DCM_LateralSpreadingDeviceNumber),
    LateralSpreadingDeviceType(DCM_LateralSpreadingDeviceType)
{
}


DRTLateralSpreadingDeviceSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AccessoryCode(copy.AccessoryCode),
    LateralSpreadingDeviceDescription(copy.LateralSpreadingDeviceDescription),
    LateralSpreadingDeviceID(copy.LateralSpreadingDeviceID),
    LateralSpreadingDeviceNumber(copy.LateralSpreadingDeviceNumber),
    LateralSpreadingDeviceType(copy.LateralSpreadingDeviceType)
{
}


DRTLateralSpreadingDeviceSequence::Item::~Item()
{
}


DRTLateralSpreadingDeviceSequence::Item &DRTLateralSpreadingDeviceSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AccessoryCode = copy.AccessoryCode;
        LateralSpreadingDeviceDescription = copy.LateralSpreadingDeviceDescription;
        LateralSpreadingDeviceID = copy.LateralSpreadingDeviceID;
        LateralSpreadingDeviceNumber = copy.LateralSpreadingDeviceNumber;
        LateralSpreadingDeviceType = copy.LateralSpreadingDeviceType;
    }
    return *this;
}


void DRTLateralSpreadingDeviceSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        LateralSpreadingDeviceNumber.clear();
        LateralSpreadingDeviceID.clear();
        AccessoryCode.clear();
        LateralSpreadingDeviceType.clear();
        LateralSpreadingDeviceDescription.clear();
    }
}


OFBool DRTLateralSpreadingDeviceSequence::Item::isEmpty()
{
    return LateralSpreadingDeviceNumber.isEmpty() &&
           LateralSpreadingDeviceID.isEmpty() &&
           AccessoryCode.isEmpty() &&
           LateralSpreadingDeviceType.isEmpty() &&
           LateralSpreadingDeviceDescription.isEmpty();
}


OFBool DRTLateralSpreadingDeviceSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTLateralSpreadingDeviceSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, LateralSpreadingDeviceNumber, "1", "1", "LateralSpreadingDeviceSequence");
        getAndCheckElementFromDataset(item, LateralSpreadingDeviceID, "1", "1", "LateralSpreadingDeviceSequence");
        getAndCheckElementFromDataset(item, AccessoryCode, "1", "3", "LateralSpreadingDeviceSequence");
        getAndCheckElementFromDataset(item, LateralSpreadingDeviceType, "1", "1", "LateralSpreadingDeviceSequence");
        getAndCheckElementFromDataset(item, LateralSpreadingDeviceDescription, "1", "3", "LateralSpreadingDeviceSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTLateralSpreadingDeviceSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(LateralSpreadingDeviceNumber), "1", "1", "LateralSpreadingDeviceSequence");
        addElementToDataset(result, item, new DcmShortString(LateralSpreadingDeviceID), "1", "1", "LateralSpreadingDeviceSequence");
        addElementToDataset(result, item, new DcmLongString(AccessoryCode), "1", "3", "LateralSpreadingDeviceSequence");
        addElementToDataset(result, item, new DcmCodeString(LateralSpreadingDeviceType), "1", "1", "LateralSpreadingDeviceSequence");
        addElementToDataset(result, item, new DcmLongString(LateralSpreadingDeviceDescription), "1", "3", "LateralSpreadingDeviceSequence");
    }
    return result;
}


OFCondition DRTLateralSpreadingDeviceSequence::Item::getAccessoryCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AccessoryCode, value, pos);
}


OFCondition DRTLateralSpreadingDeviceSequence::Item::getLateralSpreadingDeviceDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(LateralSpreadingDeviceDescription, value, pos);
}


OFCondition DRTLateralSpreadingDeviceSequence::Item::getLateralSpreadingDeviceID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(LateralSpreadingDeviceID, value, pos);
}


OFCondition DRTLateralSpreadingDeviceSequence::Item::getLateralSpreadingDeviceNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(LateralSpreadingDeviceNumber, value, pos);
}


OFCondition DRTLateralSpreadingDeviceSequence::Item::getLateralSpreadingDeviceNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, LateralSpreadingDeviceNumber).getSint32(value, pos);
}


OFCondition DRTLateralSpreadingDeviceSequence::Item::getLateralSpreadingDeviceType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(LateralSpreadingDeviceType, value, pos);
}


OFCondition DRTLateralSpreadingDeviceSequence::Item::setAccessoryCode(const OFString &value, const OFBool check)
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


OFCondition DRTLateralSpreadingDeviceSequence::Item::setLateralSpreadingDeviceDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = LateralSpreadingDeviceDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTLateralSpreadingDeviceSequence::Item::setLateralSpreadingDeviceID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = LateralSpreadingDeviceID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTLateralSpreadingDeviceSequence::Item::setLateralSpreadingDeviceNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = LateralSpreadingDeviceNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTLateralSpreadingDeviceSequence::Item::setLateralSpreadingDeviceType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = LateralSpreadingDeviceType.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTLateralSpreadingDeviceSequence::DRTLateralSpreadingDeviceSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTLateralSpreadingDeviceSequence::DRTLateralSpreadingDeviceSequence(const DRTLateralSpreadingDeviceSequence &copy)
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


DRTLateralSpreadingDeviceSequence &DRTLateralSpreadingDeviceSequence::operator=(const DRTLateralSpreadingDeviceSequence &copy)
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


DRTLateralSpreadingDeviceSequence::~DRTLateralSpreadingDeviceSequence()
{
    clear();
}


void DRTLateralSpreadingDeviceSequence::clear()
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


OFBool DRTLateralSpreadingDeviceSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTLateralSpreadingDeviceSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTLateralSpreadingDeviceSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTLateralSpreadingDeviceSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTLateralSpreadingDeviceSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTLateralSpreadingDeviceSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTLateralSpreadingDeviceSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTLateralSpreadingDeviceSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTLateralSpreadingDeviceSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTLateralSpreadingDeviceSequence::Item &DRTLateralSpreadingDeviceSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTLateralSpreadingDeviceSequence::Item &DRTLateralSpreadingDeviceSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTLateralSpreadingDeviceSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTLateralSpreadingDeviceSequence::Item &DRTLateralSpreadingDeviceSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTLateralSpreadingDeviceSequence::Item &DRTLateralSpreadingDeviceSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTLateralSpreadingDeviceSequence::Item &DRTLateralSpreadingDeviceSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTLateralSpreadingDeviceSequence::Item &DRTLateralSpreadingDeviceSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTLateralSpreadingDeviceSequence::addItem(Item *&item)
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


OFCondition DRTLateralSpreadingDeviceSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTLateralSpreadingDeviceSequence::removeItem(const size_t pos)
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


OFCondition DRTLateralSpreadingDeviceSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_LateralSpreadingDeviceSequence, sequence);
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
            DcmSequenceOfItems element(DCM_LateralSpreadingDeviceSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTLateralSpreadingDeviceSequence::write(DcmItem &dataset,
                                                     const OFString &card,
                                                     const OFString &type,
                                                     const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_LateralSpreadingDeviceSequence);
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
