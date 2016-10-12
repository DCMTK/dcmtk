/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtrbos7.h"


// --- item class ---

DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AccessoryCode(DCM_AccessoryCode),
    ReferencedROINumber(DCM_ReferencedROINumber)
{
}


DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AccessoryCode(copy.AccessoryCode),
    ReferencedROINumber(copy.ReferencedROINumber)
{
}


DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item::~Item()
{
}


DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item &DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AccessoryCode = copy.AccessoryCode;
        ReferencedROINumber = copy.ReferencedROINumber;
    }
    return *this;
}


void DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ReferencedROINumber.clear();
        AccessoryCode.clear();
    }
}


OFBool DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item::isEmpty()
{
    return ReferencedROINumber.isEmpty() &&
           AccessoryCode.isEmpty();
}


OFBool DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ReferencedROINumber, "1", "1", "ReferencedBolusSequence");
        getAndCheckElementFromDataset(item, AccessoryCode, "1", "3", "ReferencedBolusSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ReferencedROINumber), "1", "1", "ReferencedBolusSequence");
        addElementToDataset(result, item, new DcmLongString(AccessoryCode), "1", "3", "ReferencedBolusSequence");
    }
    return result;
}


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item::getAccessoryCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AccessoryCode, value, pos);
}


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item::getReferencedROINumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedROINumber, value, pos);
}


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item::getReferencedROINumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedROINumber).getSint32(value, pos);
}


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item::setAccessoryCode(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item::setReferencedROINumber(const OFString &value, const OFBool check)
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

DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule(const DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule &copy)
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


DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule &DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::operator=(const DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule &copy)
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


DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::~DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule()
{
    clear();
}


void DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::clear()
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


OFBool DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item &DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item &DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item &DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item &DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item &DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::Item &DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::addItem(Item *&item)
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


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::removeItem(const unsigned long pos)
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


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ReferencedBolusSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ReferencedBolusSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTReferencedBolusSequenceInRTIonBeamsSessionRecordModule::write(DcmItem &dataset,
                                                                             const OFString &card,
                                                                             const OFString &type,
                                                                             const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ReferencedBolusSequence);
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
