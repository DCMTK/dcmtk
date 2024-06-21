/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTIonWedgePositionSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtiwps.h"


// --- item class ---

DRTIonWedgePositionSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ReferencedWedgeNumber(DCM_ReferencedWedgeNumber),
    WedgePosition(DCM_WedgePosition),
    WedgeThinEdgePosition(DCM_WedgeThinEdgePosition)
{
}


DRTIonWedgePositionSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ReferencedWedgeNumber(copy.ReferencedWedgeNumber),
    WedgePosition(copy.WedgePosition),
    WedgeThinEdgePosition(copy.WedgeThinEdgePosition)
{
}


DRTIonWedgePositionSequence::Item::~Item()
{
}


DRTIonWedgePositionSequence::Item &DRTIonWedgePositionSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ReferencedWedgeNumber = copy.ReferencedWedgeNumber;
        WedgePosition = copy.WedgePosition;
        WedgeThinEdgePosition = copy.WedgeThinEdgePosition;
    }
    return *this;
}


void DRTIonWedgePositionSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ReferencedWedgeNumber.clear();
        WedgePosition.clear();
        WedgeThinEdgePosition.clear();
    }
}


OFBool DRTIonWedgePositionSequence::Item::isEmpty()
{
    return ReferencedWedgeNumber.isEmpty() &&
           WedgePosition.isEmpty() &&
           WedgeThinEdgePosition.isEmpty();
}


OFBool DRTIonWedgePositionSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTIonWedgePositionSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ReferencedWedgeNumber, "1", "1", "IonWedgePositionSequence");
        getAndCheckElementFromDataset(item, WedgePosition, "1", "1", "IonWedgePositionSequence");
        getAndCheckElementFromDataset(item, WedgeThinEdgePosition, "1", "1C", "IonWedgePositionSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonWedgePositionSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ReferencedWedgeNumber), "1", "1", "IonWedgePositionSequence");
        addElementToDataset(result, item, new DcmCodeString(WedgePosition), "1", "1", "IonWedgePositionSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(WedgeThinEdgePosition), "1", "1C", "IonWedgePositionSequence");
    }
    return result;
}


OFCondition DRTIonWedgePositionSequence::Item::getReferencedWedgeNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedWedgeNumber, value, pos);
}


OFCondition DRTIonWedgePositionSequence::Item::getReferencedWedgeNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedWedgeNumber).getSint32(value, pos);
}


OFCondition DRTIonWedgePositionSequence::Item::getWedgePosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(WedgePosition, value, pos);
}


OFCondition DRTIonWedgePositionSequence::Item::getWedgeThinEdgePosition(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, WedgeThinEdgePosition).getFloat32(value, pos);
}


OFCondition DRTIonWedgePositionSequence::Item::setReferencedWedgeNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedWedgeNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonWedgePositionSequence::Item::setWedgePosition(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = WedgePosition.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonWedgePositionSequence::Item::setWedgeThinEdgePosition(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return WedgeThinEdgePosition.putFloat32(value, pos);
}


// --- sequence class ---

DRTIonWedgePositionSequence::DRTIonWedgePositionSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTIonWedgePositionSequence::DRTIonWedgePositionSequence(const DRTIonWedgePositionSequence &copy)
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


DRTIonWedgePositionSequence &DRTIonWedgePositionSequence::operator=(const DRTIonWedgePositionSequence &copy)
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


DRTIonWedgePositionSequence::~DRTIonWedgePositionSequence()
{
    clear();
}


void DRTIonWedgePositionSequence::clear()
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


OFBool DRTIonWedgePositionSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTIonWedgePositionSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTIonWedgePositionSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTIonWedgePositionSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonWedgePositionSequence::gotoNextItem()
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


OFCondition DRTIonWedgePositionSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTIonWedgePositionSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTIonWedgePositionSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTIonWedgePositionSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTIonWedgePositionSequence::Item &DRTIonWedgePositionSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTIonWedgePositionSequence::Item &DRTIonWedgePositionSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTIonWedgePositionSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTIonWedgePositionSequence::Item &DRTIonWedgePositionSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTIonWedgePositionSequence::Item &DRTIonWedgePositionSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTIonWedgePositionSequence::Item &DRTIonWedgePositionSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTIonWedgePositionSequence::Item &DRTIonWedgePositionSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTIonWedgePositionSequence::addItem(Item *&item)
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


OFCondition DRTIonWedgePositionSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTIonWedgePositionSequence::removeItem(const size_t pos)
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


OFCondition DRTIonWedgePositionSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_IonWedgePositionSequence, sequence);
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
            DcmSequenceOfItems element(DCM_IonWedgePositionSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTIonWedgePositionSequence::write(DcmItem &dataset,
                                               const OFString &card,
                                               const OFString &type,
                                               const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_IonWedgePositionSequence);
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
