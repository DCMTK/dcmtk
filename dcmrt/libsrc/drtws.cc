/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTWedgeSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtws.h"


// --- item class ---

DRTWedgeSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AccessoryCode(DCM_AccessoryCode),
    EffectiveWedgeAngle(DCM_EffectiveWedgeAngle),
    SourceToWedgeTrayDistance(DCM_SourceToWedgeTrayDistance),
    WedgeAngle(DCM_WedgeAngle),
    WedgeFactor(DCM_WedgeFactor),
    WedgeID(DCM_WedgeID),
    WedgeNumber(DCM_WedgeNumber),
    WedgeOrientation(DCM_WedgeOrientation),
    WedgeType(DCM_WedgeType)
{
}


DRTWedgeSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AccessoryCode(copy.AccessoryCode),
    EffectiveWedgeAngle(copy.EffectiveWedgeAngle),
    SourceToWedgeTrayDistance(copy.SourceToWedgeTrayDistance),
    WedgeAngle(copy.WedgeAngle),
    WedgeFactor(copy.WedgeFactor),
    WedgeID(copy.WedgeID),
    WedgeNumber(copy.WedgeNumber),
    WedgeOrientation(copy.WedgeOrientation),
    WedgeType(copy.WedgeType)
{
}


DRTWedgeSequence::Item::~Item()
{
}


DRTWedgeSequence::Item &DRTWedgeSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AccessoryCode = copy.AccessoryCode;
        EffectiveWedgeAngle = copy.EffectiveWedgeAngle;
        SourceToWedgeTrayDistance = copy.SourceToWedgeTrayDistance;
        WedgeAngle = copy.WedgeAngle;
        WedgeFactor = copy.WedgeFactor;
        WedgeID = copy.WedgeID;
        WedgeNumber = copy.WedgeNumber;
        WedgeOrientation = copy.WedgeOrientation;
        WedgeType = copy.WedgeType;
    }
    return *this;
}


void DRTWedgeSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        WedgeNumber.clear();
        WedgeType.clear();
        WedgeID.clear();
        AccessoryCode.clear();
        WedgeAngle.clear();
        WedgeFactor.clear();
        WedgeOrientation.clear();
        SourceToWedgeTrayDistance.clear();
        EffectiveWedgeAngle.clear();
    }
}


OFBool DRTWedgeSequence::Item::isEmpty()
{
    return WedgeNumber.isEmpty() &&
           WedgeType.isEmpty() &&
           WedgeID.isEmpty() &&
           AccessoryCode.isEmpty() &&
           WedgeAngle.isEmpty() &&
           WedgeFactor.isEmpty() &&
           WedgeOrientation.isEmpty() &&
           SourceToWedgeTrayDistance.isEmpty() &&
           EffectiveWedgeAngle.isEmpty();
}


OFBool DRTWedgeSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTWedgeSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, WedgeNumber, "1", "1", "WedgeSequence");
        getAndCheckElementFromDataset(item, WedgeType, "1", "2", "WedgeSequence");
        getAndCheckElementFromDataset(item, WedgeID, "1", "3", "WedgeSequence");
        getAndCheckElementFromDataset(item, AccessoryCode, "1", "3", "WedgeSequence");
        getAndCheckElementFromDataset(item, WedgeAngle, "1", "2", "WedgeSequence");
        getAndCheckElementFromDataset(item, WedgeFactor, "1", "2", "WedgeSequence");
        getAndCheckElementFromDataset(item, WedgeOrientation, "1", "2", "WedgeSequence");
        getAndCheckElementFromDataset(item, SourceToWedgeTrayDistance, "1", "3", "WedgeSequence");
        getAndCheckElementFromDataset(item, EffectiveWedgeAngle, "1", "3", "WedgeSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTWedgeSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(WedgeNumber), "1", "1", "WedgeSequence");
        addElementToDataset(result, item, new DcmCodeString(WedgeType), "1", "2", "WedgeSequence");
        addElementToDataset(result, item, new DcmShortString(WedgeID), "1", "3", "WedgeSequence");
        addElementToDataset(result, item, new DcmLongString(AccessoryCode), "1", "3", "WedgeSequence");
        addElementToDataset(result, item, new DcmIntegerString(WedgeAngle), "1", "2", "WedgeSequence");
        addElementToDataset(result, item, new DcmDecimalString(WedgeFactor), "1", "2", "WedgeSequence");
        addElementToDataset(result, item, new DcmDecimalString(WedgeOrientation), "1", "2", "WedgeSequence");
        addElementToDataset(result, item, new DcmDecimalString(SourceToWedgeTrayDistance), "1", "3", "WedgeSequence");
        addElementToDataset(result, item, new DcmDecimalString(EffectiveWedgeAngle), "1", "3", "WedgeSequence");
    }
    return result;
}


OFCondition DRTWedgeSequence::Item::getAccessoryCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AccessoryCode, value, pos);
}


OFCondition DRTWedgeSequence::Item::getEffectiveWedgeAngle(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(EffectiveWedgeAngle, value, pos);
}


OFCondition DRTWedgeSequence::Item::getEffectiveWedgeAngle(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, EffectiveWedgeAngle).getFloat64(value, pos);
}


OFCondition DRTWedgeSequence::Item::getSourceToWedgeTrayDistance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceToWedgeTrayDistance, value, pos);
}


OFCondition DRTWedgeSequence::Item::getSourceToWedgeTrayDistance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SourceToWedgeTrayDistance).getFloat64(value, pos);
}


OFCondition DRTWedgeSequence::Item::getWedgeAngle(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(WedgeAngle, value, pos);
}


OFCondition DRTWedgeSequence::Item::getWedgeAngle(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, WedgeAngle).getSint32(value, pos);
}


OFCondition DRTWedgeSequence::Item::getWedgeFactor(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(WedgeFactor, value, pos);
}


OFCondition DRTWedgeSequence::Item::getWedgeFactor(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, WedgeFactor).getFloat64(value, pos);
}


OFCondition DRTWedgeSequence::Item::getWedgeID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(WedgeID, value, pos);
}


OFCondition DRTWedgeSequence::Item::getWedgeNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(WedgeNumber, value, pos);
}


OFCondition DRTWedgeSequence::Item::getWedgeNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, WedgeNumber).getSint32(value, pos);
}


OFCondition DRTWedgeSequence::Item::getWedgeOrientation(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(WedgeOrientation, value, pos);
}


OFCondition DRTWedgeSequence::Item::getWedgeOrientation(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, WedgeOrientation).getFloat64(value, pos);
}


OFCondition DRTWedgeSequence::Item::getWedgeType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(WedgeType, value, pos);
}


OFCondition DRTWedgeSequence::Item::setAccessoryCode(const OFString &value, const OFBool check)
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


OFCondition DRTWedgeSequence::Item::setEffectiveWedgeAngle(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = EffectiveWedgeAngle.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTWedgeSequence::Item::setSourceToWedgeTrayDistance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceToWedgeTrayDistance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTWedgeSequence::Item::setWedgeAngle(const OFString &value, const OFBool check)
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


OFCondition DRTWedgeSequence::Item::setWedgeFactor(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = WedgeFactor.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTWedgeSequence::Item::setWedgeID(const OFString &value, const OFBool check)
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


OFCondition DRTWedgeSequence::Item::setWedgeNumber(const OFString &value, const OFBool check)
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


OFCondition DRTWedgeSequence::Item::setWedgeOrientation(const OFString &value, const OFBool check)
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


OFCondition DRTWedgeSequence::Item::setWedgeType(const OFString &value, const OFBool check)
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

DRTWedgeSequence::DRTWedgeSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTWedgeSequence::DRTWedgeSequence(const DRTWedgeSequence &copy)
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


DRTWedgeSequence &DRTWedgeSequence::operator=(const DRTWedgeSequence &copy)
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


DRTWedgeSequence::~DRTWedgeSequence()
{
    clear();
}


void DRTWedgeSequence::clear()
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


OFBool DRTWedgeSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTWedgeSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTWedgeSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTWedgeSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTWedgeSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTWedgeSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTWedgeSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTWedgeSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTWedgeSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTWedgeSequence::Item &DRTWedgeSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTWedgeSequence::Item &DRTWedgeSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTWedgeSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTWedgeSequence::Item &DRTWedgeSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTWedgeSequence::Item &DRTWedgeSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTWedgeSequence::Item &DRTWedgeSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTWedgeSequence::Item &DRTWedgeSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTWedgeSequence::addItem(Item *&item)
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


OFCondition DRTWedgeSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTWedgeSequence::removeItem(const size_t pos)
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


OFCondition DRTWedgeSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_WedgeSequence, sequence);
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
            DcmSequenceOfItems element(DCM_WedgeSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTWedgeSequence::write(DcmItem &dataset,
                                    const OFString &card,
                                    const OFString &type,
                                    const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_WedgeSequence);
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
