/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTBrachyAccessoryDeviceSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtbads.h"


// --- item class ---

DRTBrachyAccessoryDeviceSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    BrachyAccessoryDeviceID(DCM_BrachyAccessoryDeviceID),
    BrachyAccessoryDeviceName(DCM_BrachyAccessoryDeviceName),
    BrachyAccessoryDeviceNominalThickness(DCM_BrachyAccessoryDeviceNominalThickness),
    BrachyAccessoryDeviceNominalTransmission(DCM_BrachyAccessoryDeviceNominalTransmission),
    BrachyAccessoryDeviceNumber(DCM_BrachyAccessoryDeviceNumber),
    BrachyAccessoryDeviceType(DCM_BrachyAccessoryDeviceType),
    MaterialID(DCM_MaterialID),
    ReferencedROINumber(DCM_ReferencedROINumber)
{
}


DRTBrachyAccessoryDeviceSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    BrachyAccessoryDeviceID(copy.BrachyAccessoryDeviceID),
    BrachyAccessoryDeviceName(copy.BrachyAccessoryDeviceName),
    BrachyAccessoryDeviceNominalThickness(copy.BrachyAccessoryDeviceNominalThickness),
    BrachyAccessoryDeviceNominalTransmission(copy.BrachyAccessoryDeviceNominalTransmission),
    BrachyAccessoryDeviceNumber(copy.BrachyAccessoryDeviceNumber),
    BrachyAccessoryDeviceType(copy.BrachyAccessoryDeviceType),
    MaterialID(copy.MaterialID),
    ReferencedROINumber(copy.ReferencedROINumber)
{
}


DRTBrachyAccessoryDeviceSequence::Item::~Item()
{
}


DRTBrachyAccessoryDeviceSequence::Item &DRTBrachyAccessoryDeviceSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        BrachyAccessoryDeviceID = copy.BrachyAccessoryDeviceID;
        BrachyAccessoryDeviceName = copy.BrachyAccessoryDeviceName;
        BrachyAccessoryDeviceNominalThickness = copy.BrachyAccessoryDeviceNominalThickness;
        BrachyAccessoryDeviceNominalTransmission = copy.BrachyAccessoryDeviceNominalTransmission;
        BrachyAccessoryDeviceNumber = copy.BrachyAccessoryDeviceNumber;
        BrachyAccessoryDeviceType = copy.BrachyAccessoryDeviceType;
        MaterialID = copy.MaterialID;
        ReferencedROINumber = copy.ReferencedROINumber;
    }
    return *this;
}


void DRTBrachyAccessoryDeviceSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        BrachyAccessoryDeviceNumber.clear();
        BrachyAccessoryDeviceID.clear();
        BrachyAccessoryDeviceType.clear();
        BrachyAccessoryDeviceName.clear();
        MaterialID.clear();
        BrachyAccessoryDeviceNominalThickness.clear();
        BrachyAccessoryDeviceNominalTransmission.clear();
        ReferencedROINumber.clear();
    }
}


OFBool DRTBrachyAccessoryDeviceSequence::Item::isEmpty()
{
    return BrachyAccessoryDeviceNumber.isEmpty() &&
           BrachyAccessoryDeviceID.isEmpty() &&
           BrachyAccessoryDeviceType.isEmpty() &&
           BrachyAccessoryDeviceName.isEmpty() &&
           MaterialID.isEmpty() &&
           BrachyAccessoryDeviceNominalThickness.isEmpty() &&
           BrachyAccessoryDeviceNominalTransmission.isEmpty() &&
           ReferencedROINumber.isEmpty();
}


OFBool DRTBrachyAccessoryDeviceSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, BrachyAccessoryDeviceNumber, "1", "2", "BrachyAccessoryDeviceSequence");
        getAndCheckElementFromDataset(item, BrachyAccessoryDeviceID, "1", "2", "BrachyAccessoryDeviceSequence");
        getAndCheckElementFromDataset(item, BrachyAccessoryDeviceType, "1", "1", "BrachyAccessoryDeviceSequence");
        getAndCheckElementFromDataset(item, BrachyAccessoryDeviceName, "1", "3", "BrachyAccessoryDeviceSequence");
        getAndCheckElementFromDataset(item, MaterialID, "1", "3", "BrachyAccessoryDeviceSequence");
        getAndCheckElementFromDataset(item, BrachyAccessoryDeviceNominalThickness, "1", "3", "BrachyAccessoryDeviceSequence");
        getAndCheckElementFromDataset(item, BrachyAccessoryDeviceNominalTransmission, "1", "3", "BrachyAccessoryDeviceSequence");
        getAndCheckElementFromDataset(item, ReferencedROINumber, "1", "2", "BrachyAccessoryDeviceSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(BrachyAccessoryDeviceNumber), "1", "2", "BrachyAccessoryDeviceSequence");
        addElementToDataset(result, item, new DcmShortString(BrachyAccessoryDeviceID), "1", "2", "BrachyAccessoryDeviceSequence");
        addElementToDataset(result, item, new DcmCodeString(BrachyAccessoryDeviceType), "1", "1", "BrachyAccessoryDeviceSequence");
        addElementToDataset(result, item, new DcmLongString(BrachyAccessoryDeviceName), "1", "3", "BrachyAccessoryDeviceSequence");
        addElementToDataset(result, item, new DcmShortString(MaterialID), "1", "3", "BrachyAccessoryDeviceSequence");
        addElementToDataset(result, item, new DcmDecimalString(BrachyAccessoryDeviceNominalThickness), "1", "3", "BrachyAccessoryDeviceSequence");
        addElementToDataset(result, item, new DcmDecimalString(BrachyAccessoryDeviceNominalTransmission), "1", "3", "BrachyAccessoryDeviceSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferencedROINumber), "1", "2", "BrachyAccessoryDeviceSequence");
    }
    return result;
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::getBrachyAccessoryDeviceID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BrachyAccessoryDeviceID, value, pos);
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::getBrachyAccessoryDeviceName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BrachyAccessoryDeviceName, value, pos);
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::getBrachyAccessoryDeviceNominalThickness(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BrachyAccessoryDeviceNominalThickness, value, pos);
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::getBrachyAccessoryDeviceNominalThickness(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, BrachyAccessoryDeviceNominalThickness).getFloat64(value, pos);
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::getBrachyAccessoryDeviceNominalTransmission(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BrachyAccessoryDeviceNominalTransmission, value, pos);
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::getBrachyAccessoryDeviceNominalTransmission(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, BrachyAccessoryDeviceNominalTransmission).getFloat64(value, pos);
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::getBrachyAccessoryDeviceNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BrachyAccessoryDeviceNumber, value, pos);
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::getBrachyAccessoryDeviceNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, BrachyAccessoryDeviceNumber).getSint32(value, pos);
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::getBrachyAccessoryDeviceType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BrachyAccessoryDeviceType, value, pos);
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::getMaterialID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MaterialID, value, pos);
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::getReferencedROINumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedROINumber, value, pos);
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::getReferencedROINumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedROINumber).getSint32(value, pos);
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::setBrachyAccessoryDeviceID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BrachyAccessoryDeviceID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::setBrachyAccessoryDeviceName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BrachyAccessoryDeviceName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::setBrachyAccessoryDeviceNominalThickness(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BrachyAccessoryDeviceNominalThickness.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::setBrachyAccessoryDeviceNominalTransmission(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BrachyAccessoryDeviceNominalTransmission.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::setBrachyAccessoryDeviceNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BrachyAccessoryDeviceNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::setBrachyAccessoryDeviceType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BrachyAccessoryDeviceType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::setMaterialID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = MaterialID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBrachyAccessoryDeviceSequence::Item::setReferencedROINumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedROINumber.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTBrachyAccessoryDeviceSequence::DRTBrachyAccessoryDeviceSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTBrachyAccessoryDeviceSequence::DRTBrachyAccessoryDeviceSequence(const DRTBrachyAccessoryDeviceSequence &copy)
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


DRTBrachyAccessoryDeviceSequence &DRTBrachyAccessoryDeviceSequence::operator=(const DRTBrachyAccessoryDeviceSequence &copy)
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


DRTBrachyAccessoryDeviceSequence::~DRTBrachyAccessoryDeviceSequence()
{
    clear();
}


void DRTBrachyAccessoryDeviceSequence::clear()
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


OFBool DRTBrachyAccessoryDeviceSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTBrachyAccessoryDeviceSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTBrachyAccessoryDeviceSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTBrachyAccessoryDeviceSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTBrachyAccessoryDeviceSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTBrachyAccessoryDeviceSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTBrachyAccessoryDeviceSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTBrachyAccessoryDeviceSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTBrachyAccessoryDeviceSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTBrachyAccessoryDeviceSequence::Item &DRTBrachyAccessoryDeviceSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTBrachyAccessoryDeviceSequence::Item &DRTBrachyAccessoryDeviceSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTBrachyAccessoryDeviceSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTBrachyAccessoryDeviceSequence::Item &DRTBrachyAccessoryDeviceSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTBrachyAccessoryDeviceSequence::Item &DRTBrachyAccessoryDeviceSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTBrachyAccessoryDeviceSequence::Item &DRTBrachyAccessoryDeviceSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTBrachyAccessoryDeviceSequence::Item &DRTBrachyAccessoryDeviceSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTBrachyAccessoryDeviceSequence::addItem(Item *&item)
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


OFCondition DRTBrachyAccessoryDeviceSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTBrachyAccessoryDeviceSequence::removeItem(const unsigned long pos)
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


OFCondition DRTBrachyAccessoryDeviceSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_BrachyAccessoryDeviceSequence, sequence);
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
            DcmSequenceOfItems element(DCM_BrachyAccessoryDeviceSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTBrachyAccessoryDeviceSequence::write(DcmItem &dataset,
                                                    const OFString &card,
                                                    const OFString &type,
                                                    const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_BrachyAccessoryDeviceSequence);
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
