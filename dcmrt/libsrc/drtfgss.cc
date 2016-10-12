/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTFractionGroupSummarySequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtfgss.h"


// --- item class ---

DRTFractionGroupSummarySequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    FractionGroupType(DCM_FractionGroupType),
    FractionStatusSummarySequence(emptyDefaultItem /*emptyDefaultSequence*/),
    NumberOfFractionsDelivered(DCM_NumberOfFractionsDelivered),
    NumberOfFractionsPlanned(DCM_NumberOfFractionsPlanned),
    ReferencedFractionGroupNumber(DCM_ReferencedFractionGroupNumber)
{
}


DRTFractionGroupSummarySequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    FractionGroupType(copy.FractionGroupType),
    FractionStatusSummarySequence(copy.FractionStatusSummarySequence),
    NumberOfFractionsDelivered(copy.NumberOfFractionsDelivered),
    NumberOfFractionsPlanned(copy.NumberOfFractionsPlanned),
    ReferencedFractionGroupNumber(copy.ReferencedFractionGroupNumber)
{
}


DRTFractionGroupSummarySequence::Item::~Item()
{
}


DRTFractionGroupSummarySequence::Item &DRTFractionGroupSummarySequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        FractionGroupType = copy.FractionGroupType;
        FractionStatusSummarySequence = copy.FractionStatusSummarySequence;
        NumberOfFractionsDelivered = copy.NumberOfFractionsDelivered;
        NumberOfFractionsPlanned = copy.NumberOfFractionsPlanned;
        ReferencedFractionGroupNumber = copy.ReferencedFractionGroupNumber;
    }
    return *this;
}


void DRTFractionGroupSummarySequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ReferencedFractionGroupNumber.clear();
        FractionGroupType.clear();
        NumberOfFractionsPlanned.clear();
        NumberOfFractionsDelivered.clear();
        FractionStatusSummarySequence.clear();
    }
}


OFBool DRTFractionGroupSummarySequence::Item::isEmpty()
{
    return ReferencedFractionGroupNumber.isEmpty() &&
           FractionGroupType.isEmpty() &&
           NumberOfFractionsPlanned.isEmpty() &&
           NumberOfFractionsDelivered.isEmpty() &&
           FractionStatusSummarySequence.isEmpty();
}


OFBool DRTFractionGroupSummarySequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTFractionGroupSummarySequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ReferencedFractionGroupNumber, "1", "3", "FractionGroupSummarySequence");
        getAndCheckElementFromDataset(item, FractionGroupType, "1", "2", "FractionGroupSummarySequence");
        getAndCheckElementFromDataset(item, NumberOfFractionsPlanned, "1", "2", "FractionGroupSummarySequence");
        getAndCheckElementFromDataset(item, NumberOfFractionsDelivered, "1", "2", "FractionGroupSummarySequence");
        FractionStatusSummarySequence.read(item, "1-n", "3", "FractionGroupSummarySequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTFractionGroupSummarySequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ReferencedFractionGroupNumber), "1", "3", "FractionGroupSummarySequence");
        addElementToDataset(result, item, new DcmCodeString(FractionGroupType), "1", "2", "FractionGroupSummarySequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfFractionsPlanned), "1", "2", "FractionGroupSummarySequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfFractionsDelivered), "1", "2", "FractionGroupSummarySequence");
        if (result.good()) result = FractionStatusSummarySequence.write(item, "1-n", "3", "FractionGroupSummarySequence");
    }
    return result;
}


OFCondition DRTFractionGroupSummarySequence::Item::getFractionGroupType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FractionGroupType, value, pos);
}


OFCondition DRTFractionGroupSummarySequence::Item::getNumberOfFractionsDelivered(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfFractionsDelivered, value, pos);
}


OFCondition DRTFractionGroupSummarySequence::Item::getNumberOfFractionsDelivered(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfFractionsDelivered).getSint32(value, pos);
}


OFCondition DRTFractionGroupSummarySequence::Item::getNumberOfFractionsPlanned(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfFractionsPlanned, value, pos);
}


OFCondition DRTFractionGroupSummarySequence::Item::getNumberOfFractionsPlanned(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfFractionsPlanned).getSint32(value, pos);
}


OFCondition DRTFractionGroupSummarySequence::Item::getReferencedFractionGroupNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedFractionGroupNumber, value, pos);
}


OFCondition DRTFractionGroupSummarySequence::Item::getReferencedFractionGroupNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedFractionGroupNumber).getSint32(value, pos);
}


OFCondition DRTFractionGroupSummarySequence::Item::setFractionGroupType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = FractionGroupType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFractionGroupSummarySequence::Item::setNumberOfFractionsDelivered(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfFractionsDelivered.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFractionGroupSummarySequence::Item::setNumberOfFractionsPlanned(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfFractionsPlanned.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFractionGroupSummarySequence::Item::setReferencedFractionGroupNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedFractionGroupNumber.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTFractionGroupSummarySequence::DRTFractionGroupSummarySequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTFractionGroupSummarySequence::DRTFractionGroupSummarySequence(const DRTFractionGroupSummarySequence &copy)
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


DRTFractionGroupSummarySequence &DRTFractionGroupSummarySequence::operator=(const DRTFractionGroupSummarySequence &copy)
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


DRTFractionGroupSummarySequence::~DRTFractionGroupSummarySequence()
{
    clear();
}


void DRTFractionGroupSummarySequence::clear()
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


OFBool DRTFractionGroupSummarySequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTFractionGroupSummarySequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTFractionGroupSummarySequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTFractionGroupSummarySequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTFractionGroupSummarySequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTFractionGroupSummarySequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTFractionGroupSummarySequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTFractionGroupSummarySequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTFractionGroupSummarySequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTFractionGroupSummarySequence::Item &DRTFractionGroupSummarySequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTFractionGroupSummarySequence::Item &DRTFractionGroupSummarySequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTFractionGroupSummarySequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTFractionGroupSummarySequence::Item &DRTFractionGroupSummarySequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTFractionGroupSummarySequence::Item &DRTFractionGroupSummarySequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTFractionGroupSummarySequence::Item &DRTFractionGroupSummarySequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTFractionGroupSummarySequence::Item &DRTFractionGroupSummarySequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTFractionGroupSummarySequence::addItem(Item *&item)
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


OFCondition DRTFractionGroupSummarySequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTFractionGroupSummarySequence::removeItem(const unsigned long pos)
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


OFCondition DRTFractionGroupSummarySequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_FractionGroupSummarySequence, sequence);
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
            DcmSequenceOfItems element(DCM_FractionGroupSummarySequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTFractionGroupSummarySequence::write(DcmItem &dataset,
                                                   const OFString &card,
                                                   const OFString &type,
                                                   const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_FractionGroupSummarySequence);
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
