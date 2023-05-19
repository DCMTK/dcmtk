/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtgas6.h"


// --- item class ---

DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AccessoryCode(DCM_AccessoryCode),
    GeneralAccessoryDescription(DCM_GeneralAccessoryDescription),
    GeneralAccessoryID(DCM_GeneralAccessoryID),
    GeneralAccessoryNumber(DCM_GeneralAccessoryNumber),
    GeneralAccessoryType(DCM_GeneralAccessoryType)
{
}


DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AccessoryCode(copy.AccessoryCode),
    GeneralAccessoryDescription(copy.GeneralAccessoryDescription),
    GeneralAccessoryID(copy.GeneralAccessoryID),
    GeneralAccessoryNumber(copy.GeneralAccessoryNumber),
    GeneralAccessoryType(copy.GeneralAccessoryType)
{
}


DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::~Item()
{
}


DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item &DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AccessoryCode = copy.AccessoryCode;
        GeneralAccessoryDescription = copy.GeneralAccessoryDescription;
        GeneralAccessoryID = copy.GeneralAccessoryID;
        GeneralAccessoryNumber = copy.GeneralAccessoryNumber;
        GeneralAccessoryType = copy.GeneralAccessoryType;
    }
    return *this;
}


void DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        GeneralAccessoryNumber.clear();
        GeneralAccessoryID.clear();
        GeneralAccessoryDescription.clear();
        GeneralAccessoryType.clear();
        AccessoryCode.clear();
    }
}


OFBool DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::isEmpty()
{
    return GeneralAccessoryNumber.isEmpty() &&
           GeneralAccessoryID.isEmpty() &&
           GeneralAccessoryDescription.isEmpty() &&
           GeneralAccessoryType.isEmpty() &&
           AccessoryCode.isEmpty();
}


OFBool DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::read(DcmItem &item)
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
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::write(DcmItem &item)
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
    }
    return result;
}


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::getAccessoryCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AccessoryCode, value, pos);
}


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::getGeneralAccessoryDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GeneralAccessoryDescription, value, pos);
}


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::getGeneralAccessoryID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GeneralAccessoryID, value, pos);
}


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::getGeneralAccessoryNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GeneralAccessoryNumber, value, pos);
}


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::getGeneralAccessoryNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, GeneralAccessoryNumber).getSint32(value, pos);
}


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::getGeneralAccessoryType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GeneralAccessoryType, value, pos);
}


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::setAccessoryCode(const OFString &value, const OFBool check)
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


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::setGeneralAccessoryDescription(const OFString &value, const OFBool check)
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


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::setGeneralAccessoryID(const OFString &value, const OFBool check)
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


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::setGeneralAccessoryNumber(const OFString &value, const OFBool check)
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


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item::setGeneralAccessoryType(const OFString &value, const OFBool check)
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


// --- sequence class ---

DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule(const DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule &copy)
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


DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule &DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::operator=(const DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule &copy)
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


DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::~DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule()
{
    clear();
}


void DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::clear()
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


OFBool DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::gotoNextItem()
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


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item &DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item &DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item &DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item &DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item &DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::operator[](const size_t num)
{
    return getItem(num);
}


const DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::Item &DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::addItem(Item *&item)
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


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::removeItem(const size_t pos)
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


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::read(DcmItem &dataset,
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


OFCondition DRTGeneralAccessorySequenceInRTIonBeamsSessionRecordModule::write(DcmItem &dataset,
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
