/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTIonWedgeSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtiws.h"


// --- item class ---

DRTIonWedgeSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AccessoryCode(DCM_AccessoryCode),
    IsocenterToWedgeTrayDistance(DCM_IsocenterToWedgeTrayDistance),
    WedgeAngle(DCM_WedgeAngle),
    WedgeID(DCM_WedgeID),
    WedgeNumber(DCM_WedgeNumber),
    WedgeOrientation(DCM_WedgeOrientation),
    WedgeType(DCM_WedgeType)
{
}


DRTIonWedgeSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AccessoryCode(copy.AccessoryCode),
    IsocenterToWedgeTrayDistance(copy.IsocenterToWedgeTrayDistance),
    WedgeAngle(copy.WedgeAngle),
    WedgeID(copy.WedgeID),
    WedgeNumber(copy.WedgeNumber),
    WedgeOrientation(copy.WedgeOrientation),
    WedgeType(copy.WedgeType)
{
}


DRTIonWedgeSequence::Item::~Item()
{
}


DRTIonWedgeSequence::Item &DRTIonWedgeSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AccessoryCode = copy.AccessoryCode;
        IsocenterToWedgeTrayDistance = copy.IsocenterToWedgeTrayDistance;
        WedgeAngle = copy.WedgeAngle;
        WedgeID = copy.WedgeID;
        WedgeNumber = copy.WedgeNumber;
        WedgeOrientation = copy.WedgeOrientation;
        WedgeType = copy.WedgeType;
    }
    return *this;
}


void DRTIonWedgeSequence::Item::clear()
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
        IsocenterToWedgeTrayDistance.clear();
    }
}


OFBool DRTIonWedgeSequence::Item::isEmpty()
{
    return WedgeNumber.isEmpty() &&
           WedgeType.isEmpty() &&
           WedgeID.isEmpty() &&
           AccessoryCode.isEmpty() &&
           WedgeAngle.isEmpty() &&
           WedgeOrientation.isEmpty() &&
           IsocenterToWedgeTrayDistance.isEmpty();
}


OFBool DRTIonWedgeSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTIonWedgeSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, WedgeNumber, "1", "1", "IonWedgeSequence");
        getAndCheckElementFromDataset(item, WedgeType, "1", "2", "IonWedgeSequence");
        getAndCheckElementFromDataset(item, WedgeID, "1", "3", "IonWedgeSequence");
        getAndCheckElementFromDataset(item, AccessoryCode, "1", "3", "IonWedgeSequence");
        getAndCheckElementFromDataset(item, WedgeAngle, "1", "2", "IonWedgeSequence");
        getAndCheckElementFromDataset(item, WedgeOrientation, "1", "2", "IonWedgeSequence");
        getAndCheckElementFromDataset(item, IsocenterToWedgeTrayDistance, "1", "1", "IonWedgeSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonWedgeSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(WedgeNumber), "1", "1", "IonWedgeSequence");
        addElementToDataset(result, item, new DcmCodeString(WedgeType), "1", "2", "IonWedgeSequence");
        addElementToDataset(result, item, new DcmShortString(WedgeID), "1", "3", "IonWedgeSequence");
        addElementToDataset(result, item, new DcmLongString(AccessoryCode), "1", "3", "IonWedgeSequence");
        addElementToDataset(result, item, new DcmIntegerString(WedgeAngle), "1", "2", "IonWedgeSequence");
        addElementToDataset(result, item, new DcmDecimalString(WedgeOrientation), "1", "2", "IonWedgeSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(IsocenterToWedgeTrayDistance), "1", "1", "IonWedgeSequence");
    }
    return result;
}


OFCondition DRTIonWedgeSequence::Item::getAccessoryCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AccessoryCode, value, pos);
}


OFCondition DRTIonWedgeSequence::Item::getIsocenterToWedgeTrayDistance(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, IsocenterToWedgeTrayDistance).getFloat32(value, pos);
}


OFCondition DRTIonWedgeSequence::Item::getWedgeAngle(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(WedgeAngle, value, pos);
}


OFCondition DRTIonWedgeSequence::Item::getWedgeAngle(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, WedgeAngle).getSint32(value, pos);
}


OFCondition DRTIonWedgeSequence::Item::getWedgeID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(WedgeID, value, pos);
}


OFCondition DRTIonWedgeSequence::Item::getWedgeNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(WedgeNumber, value, pos);
}


OFCondition DRTIonWedgeSequence::Item::getWedgeNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, WedgeNumber).getSint32(value, pos);
}


OFCondition DRTIonWedgeSequence::Item::getWedgeOrientation(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(WedgeOrientation, value, pos);
}


OFCondition DRTIonWedgeSequence::Item::getWedgeOrientation(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, WedgeOrientation).getFloat64(value, pos);
}


OFCondition DRTIonWedgeSequence::Item::getWedgeType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(WedgeType, value, pos);
}


OFCondition DRTIonWedgeSequence::Item::setAccessoryCode(const OFString &value, const OFBool check)
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


OFCondition DRTIonWedgeSequence::Item::setIsocenterToWedgeTrayDistance(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return IsocenterToWedgeTrayDistance.putFloat32(value, pos);
}


OFCondition DRTIonWedgeSequence::Item::setWedgeAngle(const OFString &value, const OFBool check)
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


OFCondition DRTIonWedgeSequence::Item::setWedgeID(const OFString &value, const OFBool check)
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


OFCondition DRTIonWedgeSequence::Item::setWedgeNumber(const OFString &value, const OFBool check)
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


OFCondition DRTIonWedgeSequence::Item::setWedgeOrientation(const OFString &value, const OFBool check)
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


OFCondition DRTIonWedgeSequence::Item::setWedgeType(const OFString &value, const OFBool check)
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

DRTIonWedgeSequence::DRTIonWedgeSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTIonWedgeSequence::DRTIonWedgeSequence(const DRTIonWedgeSequence &copy)
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


DRTIonWedgeSequence &DRTIonWedgeSequence::operator=(const DRTIonWedgeSequence &copy)
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


DRTIonWedgeSequence::~DRTIonWedgeSequence()
{
    clear();
}


void DRTIonWedgeSequence::clear()
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


OFBool DRTIonWedgeSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTIonWedgeSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTIonWedgeSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTIonWedgeSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonWedgeSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonWedgeSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTIonWedgeSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTIonWedgeSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTIonWedgeSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTIonWedgeSequence::Item &DRTIonWedgeSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTIonWedgeSequence::Item &DRTIonWedgeSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTIonWedgeSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTIonWedgeSequence::Item &DRTIonWedgeSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTIonWedgeSequence::Item &DRTIonWedgeSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTIonWedgeSequence::Item &DRTIonWedgeSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTIonWedgeSequence::Item &DRTIonWedgeSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTIonWedgeSequence::addItem(Item *&item)
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


OFCondition DRTIonWedgeSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTIonWedgeSequence::removeItem(const unsigned long pos)
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


OFCondition DRTIonWedgeSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_IonWedgeSequence, sequence);
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
            DcmSequenceOfItems element(DCM_IonWedgeSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTIonWedgeSequence::write(DcmItem &dataset,
                                       const OFString &card,
                                       const OFString &type,
                                       const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_IonWedgeSequence);
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
