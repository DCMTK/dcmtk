/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTChannelShieldSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtcshs.h"


// --- item class ---

DRTChannelShieldSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ChannelShieldID(DCM_ChannelShieldID),
    ChannelShieldName(DCM_ChannelShieldName),
    ChannelShieldNominalThickness(DCM_ChannelShieldNominalThickness),
    ChannelShieldNominalTransmission(DCM_ChannelShieldNominalTransmission),
    ChannelShieldNumber(DCM_ChannelShieldNumber),
    MaterialID(DCM_MaterialID),
    ReferencedROINumber(DCM_ReferencedROINumber)
{
}


DRTChannelShieldSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ChannelShieldID(copy.ChannelShieldID),
    ChannelShieldName(copy.ChannelShieldName),
    ChannelShieldNominalThickness(copy.ChannelShieldNominalThickness),
    ChannelShieldNominalTransmission(copy.ChannelShieldNominalTransmission),
    ChannelShieldNumber(copy.ChannelShieldNumber),
    MaterialID(copy.MaterialID),
    ReferencedROINumber(copy.ReferencedROINumber)
{
}


DRTChannelShieldSequence::Item::~Item()
{
}


DRTChannelShieldSequence::Item &DRTChannelShieldSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ChannelShieldID = copy.ChannelShieldID;
        ChannelShieldName = copy.ChannelShieldName;
        ChannelShieldNominalThickness = copy.ChannelShieldNominalThickness;
        ChannelShieldNominalTransmission = copy.ChannelShieldNominalTransmission;
        ChannelShieldNumber = copy.ChannelShieldNumber;
        MaterialID = copy.MaterialID;
        ReferencedROINumber = copy.ReferencedROINumber;
    }
    return *this;
}


void DRTChannelShieldSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ChannelShieldNumber.clear();
        ChannelShieldID.clear();
        ChannelShieldName.clear();
        MaterialID.clear();
        ChannelShieldNominalThickness.clear();
        ChannelShieldNominalTransmission.clear();
        ReferencedROINumber.clear();
    }
}


OFBool DRTChannelShieldSequence::Item::isEmpty()
{
    return ChannelShieldNumber.isEmpty() &&
           ChannelShieldID.isEmpty() &&
           ChannelShieldName.isEmpty() &&
           MaterialID.isEmpty() &&
           ChannelShieldNominalThickness.isEmpty() &&
           ChannelShieldNominalTransmission.isEmpty() &&
           ReferencedROINumber.isEmpty();
}


OFBool DRTChannelShieldSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTChannelShieldSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ChannelShieldNumber, "1", "1", "ChannelShieldSequence");
        getAndCheckElementFromDataset(item, ChannelShieldID, "1", "2", "ChannelShieldSequence");
        getAndCheckElementFromDataset(item, ChannelShieldName, "1", "3", "ChannelShieldSequence");
        getAndCheckElementFromDataset(item, MaterialID, "1", "3", "ChannelShieldSequence");
        getAndCheckElementFromDataset(item, ChannelShieldNominalThickness, "1", "3", "ChannelShieldSequence");
        getAndCheckElementFromDataset(item, ChannelShieldNominalTransmission, "1", "3", "ChannelShieldSequence");
        getAndCheckElementFromDataset(item, ReferencedROINumber, "1", "2", "ChannelShieldSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTChannelShieldSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ChannelShieldNumber), "1", "1", "ChannelShieldSequence");
        addElementToDataset(result, item, new DcmShortString(ChannelShieldID), "1", "2", "ChannelShieldSequence");
        addElementToDataset(result, item, new DcmLongString(ChannelShieldName), "1", "3", "ChannelShieldSequence");
        addElementToDataset(result, item, new DcmShortString(MaterialID), "1", "3", "ChannelShieldSequence");
        addElementToDataset(result, item, new DcmDecimalString(ChannelShieldNominalThickness), "1", "3", "ChannelShieldSequence");
        addElementToDataset(result, item, new DcmDecimalString(ChannelShieldNominalTransmission), "1", "3", "ChannelShieldSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferencedROINumber), "1", "2", "ChannelShieldSequence");
    }
    return result;
}


OFCondition DRTChannelShieldSequence::Item::getChannelShieldID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ChannelShieldID, value, pos);
}


OFCondition DRTChannelShieldSequence::Item::getChannelShieldName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ChannelShieldName, value, pos);
}


OFCondition DRTChannelShieldSequence::Item::getChannelShieldNominalThickness(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ChannelShieldNominalThickness, value, pos);
}


OFCondition DRTChannelShieldSequence::Item::getChannelShieldNominalThickness(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ChannelShieldNominalThickness).getFloat64(value, pos);
}


OFCondition DRTChannelShieldSequence::Item::getChannelShieldNominalTransmission(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ChannelShieldNominalTransmission, value, pos);
}


OFCondition DRTChannelShieldSequence::Item::getChannelShieldNominalTransmission(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ChannelShieldNominalTransmission).getFloat64(value, pos);
}


OFCondition DRTChannelShieldSequence::Item::getChannelShieldNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ChannelShieldNumber, value, pos);
}


OFCondition DRTChannelShieldSequence::Item::getChannelShieldNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ChannelShieldNumber).getSint32(value, pos);
}


OFCondition DRTChannelShieldSequence::Item::getMaterialID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MaterialID, value, pos);
}


OFCondition DRTChannelShieldSequence::Item::getReferencedROINumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedROINumber, value, pos);
}


OFCondition DRTChannelShieldSequence::Item::getReferencedROINumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedROINumber).getSint32(value, pos);
}


OFCondition DRTChannelShieldSequence::Item::setChannelShieldID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ChannelShieldID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelShieldSequence::Item::setChannelShieldName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ChannelShieldName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelShieldSequence::Item::setChannelShieldNominalThickness(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ChannelShieldNominalThickness.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelShieldSequence::Item::setChannelShieldNominalTransmission(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ChannelShieldNominalTransmission.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelShieldSequence::Item::setChannelShieldNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ChannelShieldNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelShieldSequence::Item::setMaterialID(const OFString &value, const OFBool check)
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


OFCondition DRTChannelShieldSequence::Item::setReferencedROINumber(const OFString &value, const OFBool check)
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

DRTChannelShieldSequence::DRTChannelShieldSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTChannelShieldSequence::DRTChannelShieldSequence(const DRTChannelShieldSequence &copy)
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


DRTChannelShieldSequence &DRTChannelShieldSequence::operator=(const DRTChannelShieldSequence &copy)
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


DRTChannelShieldSequence::~DRTChannelShieldSequence()
{
    clear();
}


void DRTChannelShieldSequence::clear()
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


OFBool DRTChannelShieldSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTChannelShieldSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTChannelShieldSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTChannelShieldSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTChannelShieldSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTChannelShieldSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTChannelShieldSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTChannelShieldSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTChannelShieldSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTChannelShieldSequence::Item &DRTChannelShieldSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTChannelShieldSequence::Item &DRTChannelShieldSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTChannelShieldSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTChannelShieldSequence::Item &DRTChannelShieldSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTChannelShieldSequence::Item &DRTChannelShieldSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTChannelShieldSequence::Item &DRTChannelShieldSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTChannelShieldSequence::Item &DRTChannelShieldSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTChannelShieldSequence::addItem(Item *&item)
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


OFCondition DRTChannelShieldSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTChannelShieldSequence::removeItem(const size_t pos)
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


OFCondition DRTChannelShieldSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ChannelShieldSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ChannelShieldSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTChannelShieldSequence::write(DcmItem &dataset,
                                            const OFString &card,
                                            const OFString &type,
                                            const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ChannelShieldSequence);
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
