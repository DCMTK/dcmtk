/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTBrachyControlPointSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtbcps.h"


// --- item class ---

DRTBrachyControlPointSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    BrachyReferencedDoseReferenceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ControlPoint3DPosition(DCM_ControlPoint3DPosition),
    ControlPointIndex(DCM_ControlPointIndex),
    ControlPointOrientation(DCM_ControlPointOrientation),
    ControlPointRelativePosition(DCM_ControlPointRelativePosition),
    CumulativeTimeWeight(DCM_CumulativeTimeWeight)
{
}


DRTBrachyControlPointSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    BrachyReferencedDoseReferenceSequence(copy.BrachyReferencedDoseReferenceSequence),
    ControlPoint3DPosition(copy.ControlPoint3DPosition),
    ControlPointIndex(copy.ControlPointIndex),
    ControlPointOrientation(copy.ControlPointOrientation),
    ControlPointRelativePosition(copy.ControlPointRelativePosition),
    CumulativeTimeWeight(copy.CumulativeTimeWeight)
{
}


DRTBrachyControlPointSequence::Item::~Item()
{
}


DRTBrachyControlPointSequence::Item &DRTBrachyControlPointSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        BrachyReferencedDoseReferenceSequence = copy.BrachyReferencedDoseReferenceSequence;
        ControlPoint3DPosition = copy.ControlPoint3DPosition;
        ControlPointIndex = copy.ControlPointIndex;
        ControlPointOrientation = copy.ControlPointOrientation;
        ControlPointRelativePosition = copy.ControlPointRelativePosition;
        CumulativeTimeWeight = copy.CumulativeTimeWeight;
    }
    return *this;
}


void DRTBrachyControlPointSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ControlPointIndex.clear();
        CumulativeTimeWeight.clear();
        ControlPointRelativePosition.clear();
        ControlPoint3DPosition.clear();
        ControlPointOrientation.clear();
        BrachyReferencedDoseReferenceSequence.clear();
    }
}


OFBool DRTBrachyControlPointSequence::Item::isEmpty()
{
    return ControlPointIndex.isEmpty() &&
           CumulativeTimeWeight.isEmpty() &&
           ControlPointRelativePosition.isEmpty() &&
           ControlPoint3DPosition.isEmpty() &&
           ControlPointOrientation.isEmpty() &&
           BrachyReferencedDoseReferenceSequence.isEmpty();
}


OFBool DRTBrachyControlPointSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTBrachyControlPointSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ControlPointIndex, "1", "1", "BrachyControlPointSequence");
        getAndCheckElementFromDataset(item, CumulativeTimeWeight, "1", "2", "BrachyControlPointSequence");
        getAndCheckElementFromDataset(item, ControlPointRelativePosition, "1", "1", "BrachyControlPointSequence");
        getAndCheckElementFromDataset(item, ControlPoint3DPosition, "3", "3", "BrachyControlPointSequence");
        getAndCheckElementFromDataset(item, ControlPointOrientation, "3", "3", "BrachyControlPointSequence");
        BrachyReferencedDoseReferenceSequence.read(item, "1-n", "3", "BrachyControlPointSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTBrachyControlPointSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ControlPointIndex), "1", "1", "BrachyControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(CumulativeTimeWeight), "1", "2", "BrachyControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(ControlPointRelativePosition), "1", "1", "BrachyControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(ControlPoint3DPosition), "3", "3", "BrachyControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(ControlPointOrientation), "3", "3", "BrachyControlPointSequence");
        if (result.good()) result = BrachyReferencedDoseReferenceSequence.write(item, "1-n", "3", "BrachyControlPointSequence");
    }
    return result;
}


OFCondition DRTBrachyControlPointSequence::Item::getControlPoint3DPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ControlPoint3DPosition, value, pos);
}


OFCondition DRTBrachyControlPointSequence::Item::getControlPoint3DPosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ControlPoint3DPosition).getFloat64(value, pos);
}


OFCondition DRTBrachyControlPointSequence::Item::getControlPoint3DPosition(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ControlPoint3DPosition).getFloat64Vector(value);
}


OFCondition DRTBrachyControlPointSequence::Item::getControlPointIndex(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ControlPointIndex, value, pos);
}


OFCondition DRTBrachyControlPointSequence::Item::getControlPointIndex(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ControlPointIndex).getSint32(value, pos);
}


OFCondition DRTBrachyControlPointSequence::Item::getControlPointOrientation(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, ControlPointOrientation).getFloat32(value, pos);
}


OFCondition DRTBrachyControlPointSequence::Item::getControlPointRelativePosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ControlPointRelativePosition, value, pos);
}


OFCondition DRTBrachyControlPointSequence::Item::getControlPointRelativePosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ControlPointRelativePosition).getFloat64(value, pos);
}


OFCondition DRTBrachyControlPointSequence::Item::getCumulativeTimeWeight(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CumulativeTimeWeight, value, pos);
}


OFCondition DRTBrachyControlPointSequence::Item::getCumulativeTimeWeight(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CumulativeTimeWeight).getFloat64(value, pos);
}


OFCondition DRTBrachyControlPointSequence::Item::setControlPoint3DPosition(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "3") : EC_Normal;
        if (result.good())
            result = ControlPoint3DPosition.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBrachyControlPointSequence::Item::setControlPointIndex(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ControlPointIndex.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBrachyControlPointSequence::Item::setControlPointOrientation(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ControlPointOrientation.putFloat32(value, pos);
}


OFCondition DRTBrachyControlPointSequence::Item::setControlPointRelativePosition(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ControlPointRelativePosition.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBrachyControlPointSequence::Item::setCumulativeTimeWeight(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CumulativeTimeWeight.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTBrachyControlPointSequence::DRTBrachyControlPointSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTBrachyControlPointSequence::DRTBrachyControlPointSequence(const DRTBrachyControlPointSequence &copy)
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


DRTBrachyControlPointSequence &DRTBrachyControlPointSequence::operator=(const DRTBrachyControlPointSequence &copy)
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


DRTBrachyControlPointSequence::~DRTBrachyControlPointSequence()
{
    clear();
}


void DRTBrachyControlPointSequence::clear()
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


OFBool DRTBrachyControlPointSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTBrachyControlPointSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTBrachyControlPointSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTBrachyControlPointSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTBrachyControlPointSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTBrachyControlPointSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTBrachyControlPointSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTBrachyControlPointSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTBrachyControlPointSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTBrachyControlPointSequence::Item &DRTBrachyControlPointSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTBrachyControlPointSequence::Item &DRTBrachyControlPointSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTBrachyControlPointSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTBrachyControlPointSequence::Item &DRTBrachyControlPointSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTBrachyControlPointSequence::Item &DRTBrachyControlPointSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTBrachyControlPointSequence::Item &DRTBrachyControlPointSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTBrachyControlPointSequence::Item &DRTBrachyControlPointSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTBrachyControlPointSequence::addItem(Item *&item)
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


OFCondition DRTBrachyControlPointSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTBrachyControlPointSequence::removeItem(const size_t pos)
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


OFCondition DRTBrachyControlPointSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_BrachyControlPointSequence, sequence);
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
            DcmSequenceOfItems element(DCM_BrachyControlPointSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTBrachyControlPointSequence::write(DcmItem &dataset,
                                                 const OFString &card,
                                                 const OFString &type,
                                                 const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_BrachyControlPointSequence);
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
