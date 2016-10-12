/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTROIContourSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtrcs.h"


// --- item class ---

DRTROIContourSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ContourSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ROIDisplayColor(DCM_ROIDisplayColor),
    RecommendedDisplayCIELabValue(DCM_RecommendedDisplayCIELabValue),
    RecommendedDisplayGrayscaleValue(DCM_RecommendedDisplayGrayscaleValue),
    ReferencedROINumber(DCM_ReferencedROINumber)
{
}


DRTROIContourSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ContourSequence(copy.ContourSequence),
    ROIDisplayColor(copy.ROIDisplayColor),
    RecommendedDisplayCIELabValue(copy.RecommendedDisplayCIELabValue),
    RecommendedDisplayGrayscaleValue(copy.RecommendedDisplayGrayscaleValue),
    ReferencedROINumber(copy.ReferencedROINumber)
{
}


DRTROIContourSequence::Item::~Item()
{
}


DRTROIContourSequence::Item &DRTROIContourSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ContourSequence = copy.ContourSequence;
        ROIDisplayColor = copy.ROIDisplayColor;
        RecommendedDisplayCIELabValue = copy.RecommendedDisplayCIELabValue;
        RecommendedDisplayGrayscaleValue = copy.RecommendedDisplayGrayscaleValue;
        ReferencedROINumber = copy.ReferencedROINumber;
    }
    return *this;
}


void DRTROIContourSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ReferencedROINumber.clear();
        ROIDisplayColor.clear();
        RecommendedDisplayGrayscaleValue.clear();
        RecommendedDisplayCIELabValue.clear();
        ContourSequence.clear();
    }
}


OFBool DRTROIContourSequence::Item::isEmpty()
{
    return ReferencedROINumber.isEmpty() &&
           ROIDisplayColor.isEmpty() &&
           RecommendedDisplayGrayscaleValue.isEmpty() &&
           RecommendedDisplayCIELabValue.isEmpty() &&
           ContourSequence.isEmpty();
}


OFBool DRTROIContourSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTROIContourSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ReferencedROINumber, "1", "1", "ROIContourSequence");
        getAndCheckElementFromDataset(item, ROIDisplayColor, "3", "3", "ROIContourSequence");
        getAndCheckElementFromDataset(item, RecommendedDisplayGrayscaleValue, "1", "3", "ROIContourSequence");
        getAndCheckElementFromDataset(item, RecommendedDisplayCIELabValue, "3", "3", "ROIContourSequence");
        ContourSequence.read(item, "1-n", "3", "ROIContourSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTROIContourSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ReferencedROINumber), "1", "1", "ROIContourSequence");
        addElementToDataset(result, item, new DcmIntegerString(ROIDisplayColor), "3", "3", "ROIContourSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(RecommendedDisplayGrayscaleValue), "1", "3", "ROIContourSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(RecommendedDisplayCIELabValue), "3", "3", "ROIContourSequence");
        if (result.good()) result = ContourSequence.write(item, "1-n", "3", "ROIContourSequence");
    }
    return result;
}


OFCondition DRTROIContourSequence::Item::getROIDisplayColor(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ROIDisplayColor, value, pos);
}


OFCondition DRTROIContourSequence::Item::getROIDisplayColor(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ROIDisplayColor).getSint32(value, pos);
}


OFCondition DRTROIContourSequence::Item::getRecommendedDisplayCIELabValue(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, RecommendedDisplayCIELabValue).getUint16(value, pos);
}


OFCondition DRTROIContourSequence::Item::getRecommendedDisplayGrayscaleValue(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, RecommendedDisplayGrayscaleValue).getUint16(value, pos);
}


OFCondition DRTROIContourSequence::Item::getReferencedROINumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedROINumber, value, pos);
}


OFCondition DRTROIContourSequence::Item::getReferencedROINumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedROINumber).getSint32(value, pos);
}


OFCondition DRTROIContourSequence::Item::setROIDisplayColor(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "3") : EC_Normal;
        if (result.good())
            result = ROIDisplayColor.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTROIContourSequence::Item::setRecommendedDisplayCIELabValue(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RecommendedDisplayCIELabValue.putUint16(value, pos);
}


OFCondition DRTROIContourSequence::Item::setRecommendedDisplayGrayscaleValue(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RecommendedDisplayGrayscaleValue.putUint16(value, pos);
}


OFCondition DRTROIContourSequence::Item::setReferencedROINumber(const OFString &value, const OFBool check)
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

DRTROIContourSequence::DRTROIContourSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTROIContourSequence::DRTROIContourSequence(const DRTROIContourSequence &copy)
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


DRTROIContourSequence &DRTROIContourSequence::operator=(const DRTROIContourSequence &copy)
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


DRTROIContourSequence::~DRTROIContourSequence()
{
    clear();
}


void DRTROIContourSequence::clear()
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


OFBool DRTROIContourSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTROIContourSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTROIContourSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTROIContourSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTROIContourSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTROIContourSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTROIContourSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTROIContourSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTROIContourSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTROIContourSequence::Item &DRTROIContourSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTROIContourSequence::Item &DRTROIContourSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTROIContourSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTROIContourSequence::Item &DRTROIContourSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTROIContourSequence::Item &DRTROIContourSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTROIContourSequence::Item &DRTROIContourSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTROIContourSequence::Item &DRTROIContourSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTROIContourSequence::addItem(Item *&item)
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


OFCondition DRTROIContourSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTROIContourSequence::removeItem(const unsigned long pos)
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


OFCondition DRTROIContourSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ROIContourSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ROIContourSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTROIContourSequence::write(DcmItem &dataset,
                                         const OFString &card,
                                         const OFString &type,
                                         const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ROIContourSequence);
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
