/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtgas3.h"


// --- item class ---

DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AccessoryCode(DCM_AccessoryCode),
    GeneralAccessoryDescription(DCM_GeneralAccessoryDescription),
    GeneralAccessoryID(DCM_GeneralAccessoryID),
    GeneralAccessoryNumber(DCM_GeneralAccessoryNumber),
    GeneralAccessoryType(DCM_GeneralAccessoryType),
    SourceToGeneralAccessoryDistance(DCM_SourceToGeneralAccessoryDistance)
{
}


DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AccessoryCode(copy.AccessoryCode),
    GeneralAccessoryDescription(copy.GeneralAccessoryDescription),
    GeneralAccessoryID(copy.GeneralAccessoryID),
    GeneralAccessoryNumber(copy.GeneralAccessoryNumber),
    GeneralAccessoryType(copy.GeneralAccessoryType),
    SourceToGeneralAccessoryDistance(copy.SourceToGeneralAccessoryDistance)
{
}


DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::~Item()
{
}


DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item &DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AccessoryCode = copy.AccessoryCode;
        GeneralAccessoryDescription = copy.GeneralAccessoryDescription;
        GeneralAccessoryID = copy.GeneralAccessoryID;
        GeneralAccessoryNumber = copy.GeneralAccessoryNumber;
        GeneralAccessoryType = copy.GeneralAccessoryType;
        SourceToGeneralAccessoryDistance = copy.SourceToGeneralAccessoryDistance;
    }
    return *this;
}


void DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        GeneralAccessoryNumber.clear();
        GeneralAccessoryID.clear();
        GeneralAccessoryDescription.clear();
        GeneralAccessoryType.clear();
        AccessoryCode.clear();
        SourceToGeneralAccessoryDistance.clear();
    }
}


OFBool DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::isEmpty()
{
    return GeneralAccessoryNumber.isEmpty() &&
           GeneralAccessoryID.isEmpty() &&
           GeneralAccessoryDescription.isEmpty() &&
           GeneralAccessoryType.isEmpty() &&
           AccessoryCode.isEmpty() &&
           SourceToGeneralAccessoryDistance.isEmpty();
}


OFBool DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, GeneralAccessoryNumber, "1", "1", "GeneralAccessorySequence");
        getAndCheckElementFromDataset(item, GeneralAccessoryID, "1", "1", "GeneralAccessorySequence");
        getAndCheckElementFromDataset(item, GeneralAccessoryDescription, "1", "3", "GeneralAccessorySequence");
        getAndCheckElementFromDataset(item, GeneralAccessoryType, "1", "3", "GeneralAccessorySequence");
        getAndCheckElementFromDataset(item, AccessoryCode, "1", "3", "GeneralAccessorySequence");
        getAndCheckElementFromDataset(item, SourceToGeneralAccessoryDistance, "1", "3", "GeneralAccessorySequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(GeneralAccessoryNumber), "1", "1", "GeneralAccessorySequence");
        addElementToDataset(result, item, new DcmShortString(GeneralAccessoryID), "1", "1", "GeneralAccessorySequence");
        addElementToDataset(result, item, new DcmShortText(GeneralAccessoryDescription), "1", "3", "GeneralAccessorySequence");
        addElementToDataset(result, item, new DcmCodeString(GeneralAccessoryType), "1", "3", "GeneralAccessorySequence");
        addElementToDataset(result, item, new DcmLongString(AccessoryCode), "1", "3", "GeneralAccessorySequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(SourceToGeneralAccessoryDistance), "1", "3", "GeneralAccessorySequence");
    }
    return result;
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::getAccessoryCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AccessoryCode, value, pos);
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::getGeneralAccessoryDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GeneralAccessoryDescription, value, pos);
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::getGeneralAccessoryID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GeneralAccessoryID, value, pos);
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::getGeneralAccessoryNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GeneralAccessoryNumber, value, pos);
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::getGeneralAccessoryNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, GeneralAccessoryNumber).getSint32(value, pos);
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::getGeneralAccessoryType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GeneralAccessoryType, value, pos);
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::getSourceToGeneralAccessoryDistance(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, SourceToGeneralAccessoryDistance).getFloat32(value, pos);
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::setAccessoryCode(const OFString &value, const OFBool check)
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


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::setGeneralAccessoryDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = GeneralAccessoryDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::setGeneralAccessoryID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = GeneralAccessoryID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::setGeneralAccessoryNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = GeneralAccessoryNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::setGeneralAccessoryType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = GeneralAccessoryType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item::setSourceToGeneralAccessoryDistance(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return SourceToGeneralAccessoryDistance.putFloat32(value, pos);
}


// --- sequence class ---

DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule(const DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule &copy)
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


DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule &DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::operator=(const DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule &copy)
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


DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::~DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule()
{
    clear();
}


void DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::clear()
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


OFBool DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::gotoNextItem()
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


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item &DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item &DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item &DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item &DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item &DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::operator[](const size_t num)
{
    return getItem(num);
}


const DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::Item &DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::addItem(Item *&item)
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


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::removeItem(const size_t pos)
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


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_GeneralAccessorySequence, sequence);
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
            DcmSequenceOfItems element(DCM_GeneralAccessorySequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTGeneralAccessorySequenceInRTBeamsSessionRecordModule::write(DcmItem &dataset,
                                                                           const OFString &card,
                                                                           const OFString &type,
                                                                           const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_GeneralAccessorySequence);
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
