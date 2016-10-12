/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTApplicatorGeometrySequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtags.h"


// --- item class ---

DRTApplicatorGeometrySequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ApplicatorApertureShape(DCM_ApplicatorApertureShape),
    ApplicatorOpening(DCM_ApplicatorOpening),
    ApplicatorOpeningX(DCM_ApplicatorOpeningX),
    ApplicatorOpeningY(DCM_ApplicatorOpeningY)
{
}


DRTApplicatorGeometrySequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ApplicatorApertureShape(copy.ApplicatorApertureShape),
    ApplicatorOpening(copy.ApplicatorOpening),
    ApplicatorOpeningX(copy.ApplicatorOpeningX),
    ApplicatorOpeningY(copy.ApplicatorOpeningY)
{
}


DRTApplicatorGeometrySequence::Item::~Item()
{
}


DRTApplicatorGeometrySequence::Item &DRTApplicatorGeometrySequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ApplicatorApertureShape = copy.ApplicatorApertureShape;
        ApplicatorOpening = copy.ApplicatorOpening;
        ApplicatorOpeningX = copy.ApplicatorOpeningX;
        ApplicatorOpeningY = copy.ApplicatorOpeningY;
    }
    return *this;
}


void DRTApplicatorGeometrySequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ApplicatorApertureShape.clear();
        ApplicatorOpening.clear();
        ApplicatorOpeningX.clear();
        ApplicatorOpeningY.clear();
    }
}


OFBool DRTApplicatorGeometrySequence::Item::isEmpty()
{
    return ApplicatorApertureShape.isEmpty() &&
           ApplicatorOpening.isEmpty() &&
           ApplicatorOpeningX.isEmpty() &&
           ApplicatorOpeningY.isEmpty();
}


OFBool DRTApplicatorGeometrySequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTApplicatorGeometrySequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ApplicatorApertureShape, "1", "1", "ApplicatorGeometrySequence");
        getAndCheckElementFromDataset(item, ApplicatorOpening, "1", "1C", "ApplicatorGeometrySequence");
        getAndCheckElementFromDataset(item, ApplicatorOpeningX, "1", "1C", "ApplicatorGeometrySequence");
        getAndCheckElementFromDataset(item, ApplicatorOpeningY, "1", "1C", "ApplicatorGeometrySequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTApplicatorGeometrySequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmCodeString(ApplicatorApertureShape), "1", "1", "ApplicatorGeometrySequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(ApplicatorOpening), "1", "1C", "ApplicatorGeometrySequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(ApplicatorOpeningX), "1", "1C", "ApplicatorGeometrySequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(ApplicatorOpeningY), "1", "1C", "ApplicatorGeometrySequence");
    }
    return result;
}


OFCondition DRTApplicatorGeometrySequence::Item::getApplicatorApertureShape(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ApplicatorApertureShape, value, pos);
}


OFCondition DRTApplicatorGeometrySequence::Item::getApplicatorOpening(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, ApplicatorOpening).getFloat32(value, pos);
}


OFCondition DRTApplicatorGeometrySequence::Item::getApplicatorOpeningX(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, ApplicatorOpeningX).getFloat32(value, pos);
}


OFCondition DRTApplicatorGeometrySequence::Item::getApplicatorOpeningY(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, ApplicatorOpeningY).getFloat32(value, pos);
}


OFCondition DRTApplicatorGeometrySequence::Item::setApplicatorApertureShape(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ApplicatorApertureShape.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTApplicatorGeometrySequence::Item::setApplicatorOpening(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ApplicatorOpening.putFloat32(value, pos);
}


OFCondition DRTApplicatorGeometrySequence::Item::setApplicatorOpeningX(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ApplicatorOpeningX.putFloat32(value, pos);
}


OFCondition DRTApplicatorGeometrySequence::Item::setApplicatorOpeningY(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ApplicatorOpeningY.putFloat32(value, pos);
}


// --- sequence class ---

DRTApplicatorGeometrySequence::DRTApplicatorGeometrySequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTApplicatorGeometrySequence::DRTApplicatorGeometrySequence(const DRTApplicatorGeometrySequence &copy)
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


DRTApplicatorGeometrySequence &DRTApplicatorGeometrySequence::operator=(const DRTApplicatorGeometrySequence &copy)
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


DRTApplicatorGeometrySequence::~DRTApplicatorGeometrySequence()
{
    clear();
}


void DRTApplicatorGeometrySequence::clear()
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


OFBool DRTApplicatorGeometrySequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTApplicatorGeometrySequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTApplicatorGeometrySequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTApplicatorGeometrySequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTApplicatorGeometrySequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTApplicatorGeometrySequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTApplicatorGeometrySequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTApplicatorGeometrySequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTApplicatorGeometrySequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTApplicatorGeometrySequence::Item &DRTApplicatorGeometrySequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTApplicatorGeometrySequence::Item &DRTApplicatorGeometrySequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTApplicatorGeometrySequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTApplicatorGeometrySequence::Item &DRTApplicatorGeometrySequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTApplicatorGeometrySequence::Item &DRTApplicatorGeometrySequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTApplicatorGeometrySequence::Item &DRTApplicatorGeometrySequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTApplicatorGeometrySequence::Item &DRTApplicatorGeometrySequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTApplicatorGeometrySequence::addItem(Item *&item)
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


OFCondition DRTApplicatorGeometrySequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTApplicatorGeometrySequence::removeItem(const unsigned long pos)
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


OFCondition DRTApplicatorGeometrySequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ApplicatorGeometrySequence, sequence);
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
            DcmSequenceOfItems element(DCM_ApplicatorGeometrySequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTApplicatorGeometrySequence::write(DcmItem &dataset,
                                                 const OFString &card,
                                                 const OFString &type,
                                                 const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ApplicatorGeometrySequence);
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
