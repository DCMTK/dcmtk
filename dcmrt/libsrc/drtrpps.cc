/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTROIPhysicalPropertiesSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtrpps.h"


// --- item class ---

DRTROIPhysicalPropertiesSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ROIElementalCompositionSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ROIPhysicalProperty(DCM_ROIPhysicalProperty),
    ROIPhysicalPropertyValue(DCM_ROIPhysicalPropertyValue)
{
}


DRTROIPhysicalPropertiesSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ROIElementalCompositionSequence(copy.ROIElementalCompositionSequence),
    ROIPhysicalProperty(copy.ROIPhysicalProperty),
    ROIPhysicalPropertyValue(copy.ROIPhysicalPropertyValue)
{
}


DRTROIPhysicalPropertiesSequence::Item::~Item()
{
}


DRTROIPhysicalPropertiesSequence::Item &DRTROIPhysicalPropertiesSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ROIElementalCompositionSequence = copy.ROIElementalCompositionSequence;
        ROIPhysicalProperty = copy.ROIPhysicalProperty;
        ROIPhysicalPropertyValue = copy.ROIPhysicalPropertyValue;
    }
    return *this;
}


void DRTROIPhysicalPropertiesSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ROIPhysicalProperty.clear();
        ROIElementalCompositionSequence.clear();
        ROIPhysicalPropertyValue.clear();
    }
}


OFBool DRTROIPhysicalPropertiesSequence::Item::isEmpty()
{
    return ROIPhysicalProperty.isEmpty() &&
           ROIElementalCompositionSequence.isEmpty() &&
           ROIPhysicalPropertyValue.isEmpty();
}


OFBool DRTROIPhysicalPropertiesSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTROIPhysicalPropertiesSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ROIPhysicalProperty, "1", "1", "ROIPhysicalPropertiesSequence");
        ROIElementalCompositionSequence.read(item, "1-n", "1C", "ROIPhysicalPropertiesSequence");
        getAndCheckElementFromDataset(item, ROIPhysicalPropertyValue, "1", "1", "ROIPhysicalPropertiesSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTROIPhysicalPropertiesSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmCodeString(ROIPhysicalProperty), "1", "1", "ROIPhysicalPropertiesSequence");
        if (result.good()) result = ROIElementalCompositionSequence.write(item, "1-n", "1C", "ROIPhysicalPropertiesSequence");
        addElementToDataset(result, item, new DcmDecimalString(ROIPhysicalPropertyValue), "1", "1", "ROIPhysicalPropertiesSequence");
    }
    return result;
}


OFCondition DRTROIPhysicalPropertiesSequence::Item::getROIPhysicalProperty(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ROIPhysicalProperty, value, pos);
}


OFCondition DRTROIPhysicalPropertiesSequence::Item::getROIPhysicalPropertyValue(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ROIPhysicalPropertyValue, value, pos);
}


OFCondition DRTROIPhysicalPropertiesSequence::Item::getROIPhysicalPropertyValue(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ROIPhysicalPropertyValue).getFloat64(value, pos);
}


OFCondition DRTROIPhysicalPropertiesSequence::Item::setROIPhysicalProperty(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ROIPhysicalProperty.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTROIPhysicalPropertiesSequence::Item::setROIPhysicalPropertyValue(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ROIPhysicalPropertyValue.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTROIPhysicalPropertiesSequence::DRTROIPhysicalPropertiesSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTROIPhysicalPropertiesSequence::DRTROIPhysicalPropertiesSequence(const DRTROIPhysicalPropertiesSequence &copy)
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


DRTROIPhysicalPropertiesSequence &DRTROIPhysicalPropertiesSequence::operator=(const DRTROIPhysicalPropertiesSequence &copy)
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


DRTROIPhysicalPropertiesSequence::~DRTROIPhysicalPropertiesSequence()
{
    clear();
}


void DRTROIPhysicalPropertiesSequence::clear()
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


OFBool DRTROIPhysicalPropertiesSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTROIPhysicalPropertiesSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTROIPhysicalPropertiesSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTROIPhysicalPropertiesSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTROIPhysicalPropertiesSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTROIPhysicalPropertiesSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTROIPhysicalPropertiesSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTROIPhysicalPropertiesSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTROIPhysicalPropertiesSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTROIPhysicalPropertiesSequence::Item &DRTROIPhysicalPropertiesSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTROIPhysicalPropertiesSequence::Item &DRTROIPhysicalPropertiesSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTROIPhysicalPropertiesSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTROIPhysicalPropertiesSequence::Item &DRTROIPhysicalPropertiesSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTROIPhysicalPropertiesSequence::Item &DRTROIPhysicalPropertiesSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTROIPhysicalPropertiesSequence::Item &DRTROIPhysicalPropertiesSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTROIPhysicalPropertiesSequence::Item &DRTROIPhysicalPropertiesSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTROIPhysicalPropertiesSequence::addItem(Item *&item)
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


OFCondition DRTROIPhysicalPropertiesSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTROIPhysicalPropertiesSequence::removeItem(const unsigned long pos)
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


OFCondition DRTROIPhysicalPropertiesSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ROIPhysicalPropertiesSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ROIPhysicalPropertiesSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTROIPhysicalPropertiesSequence::write(DcmItem &dataset,
                                                    const OFString &card,
                                                    const OFString &type,
                                                    const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ROIPhysicalPropertiesSequence);
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
