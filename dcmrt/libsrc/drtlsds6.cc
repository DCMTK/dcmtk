/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtlsds6.h"


// --- item class ---

DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    IsocenterToLateralSpreadingDeviceDistance(DCM_IsocenterToLateralSpreadingDeviceDistance),
    LateralSpreadingDeviceSetting(DCM_LateralSpreadingDeviceSetting),
    LateralSpreadingDeviceWaterEquivalentThickness(DCM_LateralSpreadingDeviceWaterEquivalentThickness),
    ReferencedLateralSpreadingDeviceNumber(DCM_ReferencedLateralSpreadingDeviceNumber)
{
}


DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    IsocenterToLateralSpreadingDeviceDistance(copy.IsocenterToLateralSpreadingDeviceDistance),
    LateralSpreadingDeviceSetting(copy.LateralSpreadingDeviceSetting),
    LateralSpreadingDeviceWaterEquivalentThickness(copy.LateralSpreadingDeviceWaterEquivalentThickness),
    ReferencedLateralSpreadingDeviceNumber(copy.ReferencedLateralSpreadingDeviceNumber)
{
}


DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::~Item()
{
}


DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item &DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        IsocenterToLateralSpreadingDeviceDistance = copy.IsocenterToLateralSpreadingDeviceDistance;
        LateralSpreadingDeviceSetting = copy.LateralSpreadingDeviceSetting;
        LateralSpreadingDeviceWaterEquivalentThickness = copy.LateralSpreadingDeviceWaterEquivalentThickness;
        ReferencedLateralSpreadingDeviceNumber = copy.ReferencedLateralSpreadingDeviceNumber;
    }
    return *this;
}


void DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ReferencedLateralSpreadingDeviceNumber.clear();
        LateralSpreadingDeviceSetting.clear();
        IsocenterToLateralSpreadingDeviceDistance.clear();
        LateralSpreadingDeviceWaterEquivalentThickness.clear();
    }
}


OFBool DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::isEmpty()
{
    return ReferencedLateralSpreadingDeviceNumber.isEmpty() &&
           LateralSpreadingDeviceSetting.isEmpty() &&
           IsocenterToLateralSpreadingDeviceDistance.isEmpty() &&
           LateralSpreadingDeviceWaterEquivalentThickness.isEmpty();
}


OFBool DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ReferencedLateralSpreadingDeviceNumber, "1", "1", "LateralSpreadingDeviceSettingsSequence");
        getAndCheckElementFromDataset(item, LateralSpreadingDeviceSetting, "1", "1", "LateralSpreadingDeviceSettingsSequence");
        getAndCheckElementFromDataset(item, IsocenterToLateralSpreadingDeviceDistance, "1", "3", "LateralSpreadingDeviceSettingsSequence");
        getAndCheckElementFromDataset(item, LateralSpreadingDeviceWaterEquivalentThickness, "1", "3", "LateralSpreadingDeviceSettingsSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ReferencedLateralSpreadingDeviceNumber), "1", "1", "LateralSpreadingDeviceSettingsSequence");
        addElementToDataset(result, item, new DcmLongString(LateralSpreadingDeviceSetting), "1", "1", "LateralSpreadingDeviceSettingsSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(IsocenterToLateralSpreadingDeviceDistance), "1", "3", "LateralSpreadingDeviceSettingsSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(LateralSpreadingDeviceWaterEquivalentThickness), "1", "3", "LateralSpreadingDeviceSettingsSequence");
    }
    return result;
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::getIsocenterToLateralSpreadingDeviceDistance(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, IsocenterToLateralSpreadingDeviceDistance).getFloat32(value, pos);
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::getLateralSpreadingDeviceSetting(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(LateralSpreadingDeviceSetting, value, pos);
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::getLateralSpreadingDeviceWaterEquivalentThickness(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, LateralSpreadingDeviceWaterEquivalentThickness).getFloat32(value, pos);
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::getReferencedLateralSpreadingDeviceNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedLateralSpreadingDeviceNumber, value, pos);
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::getReferencedLateralSpreadingDeviceNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedLateralSpreadingDeviceNumber).getSint32(value, pos);
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::setIsocenterToLateralSpreadingDeviceDistance(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return IsocenterToLateralSpreadingDeviceDistance.putFloat32(value, pos);
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::setLateralSpreadingDeviceSetting(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = LateralSpreadingDeviceSetting.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::setLateralSpreadingDeviceWaterEquivalentThickness(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return LateralSpreadingDeviceWaterEquivalentThickness.putFloat32(value, pos);
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item::setReferencedLateralSpreadingDeviceNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedLateralSpreadingDeviceNumber.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule(const DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule &copy)
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


DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule &DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::operator=(const DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule &copy)
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


DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::~DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule()
{
    clear();
}


void DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::clear()
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


OFBool DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item &DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item &DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item &DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item &DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item &DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::Item &DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::addItem(Item *&item)
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


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::removeItem(const unsigned long pos)
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


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_LateralSpreadingDeviceSettingsSequence, sequence);
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
            DcmSequenceOfItems element(DCM_LateralSpreadingDeviceSettingsSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTLateralSpreadingDeviceSettingsSequenceInRTIonBeamsModule::write(DcmItem &dataset,
                                                                               const OFString &card,
                                                                               const OFString &type,
                                                                               const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_LateralSpreadingDeviceSettingsSequence);
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
