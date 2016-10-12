/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTContourSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtcs.h"


// --- item class ---

DRTContourSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AttachedContours(DCM_AttachedContours),
    ContourData(DCM_ContourData),
    ContourGeometricType(DCM_ContourGeometricType),
    ContourImageSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ContourNumber(DCM_ContourNumber),
    ContourOffsetVector(DCM_ContourOffsetVector),
    ContourSlabThickness(DCM_ContourSlabThickness),
    NumberOfContourPoints(DCM_NumberOfContourPoints)
{
}


DRTContourSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AttachedContours(copy.AttachedContours),
    ContourData(copy.ContourData),
    ContourGeometricType(copy.ContourGeometricType),
    ContourImageSequence(copy.ContourImageSequence),
    ContourNumber(copy.ContourNumber),
    ContourOffsetVector(copy.ContourOffsetVector),
    ContourSlabThickness(copy.ContourSlabThickness),
    NumberOfContourPoints(copy.NumberOfContourPoints)
{
}


DRTContourSequence::Item::~Item()
{
}


DRTContourSequence::Item &DRTContourSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AttachedContours = copy.AttachedContours;
        ContourData = copy.ContourData;
        ContourGeometricType = copy.ContourGeometricType;
        ContourImageSequence = copy.ContourImageSequence;
        ContourNumber = copy.ContourNumber;
        ContourOffsetVector = copy.ContourOffsetVector;
        ContourSlabThickness = copy.ContourSlabThickness;
        NumberOfContourPoints = copy.NumberOfContourPoints;
    }
    return *this;
}


void DRTContourSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ContourNumber.clear();
        AttachedContours.clear();
        ContourImageSequence.clear();
        ContourGeometricType.clear();
        ContourSlabThickness.clear();
        ContourOffsetVector.clear();
        NumberOfContourPoints.clear();
        ContourData.clear();
    }
}


OFBool DRTContourSequence::Item::isEmpty()
{
    return ContourNumber.isEmpty() &&
           AttachedContours.isEmpty() &&
           ContourImageSequence.isEmpty() &&
           ContourGeometricType.isEmpty() &&
           ContourSlabThickness.isEmpty() &&
           ContourOffsetVector.isEmpty() &&
           NumberOfContourPoints.isEmpty() &&
           ContourData.isEmpty();
}


OFBool DRTContourSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTContourSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ContourNumber, "1", "3", "ContourSequence");
        getAndCheckElementFromDataset(item, AttachedContours, "1-n", "3", "ContourSequence");
        ContourImageSequence.read(item, "1-n", "3", "ContourSequence");
        getAndCheckElementFromDataset(item, ContourGeometricType, "1", "1", "ContourSequence");
        getAndCheckElementFromDataset(item, ContourSlabThickness, "1", "3", "ContourSequence");
        getAndCheckElementFromDataset(item, ContourOffsetVector, "3", "3", "ContourSequence");
        getAndCheckElementFromDataset(item, NumberOfContourPoints, "1", "1", "ContourSequence");
        getAndCheckElementFromDataset(item, ContourData, "3-3n", "1", "ContourSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTContourSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ContourNumber), "1", "3", "ContourSequence");
        addElementToDataset(result, item, new DcmIntegerString(AttachedContours), "1-n", "3", "ContourSequence");
        if (result.good()) result = ContourImageSequence.write(item, "1-n", "3", "ContourSequence");
        addElementToDataset(result, item, new DcmCodeString(ContourGeometricType), "1", "1", "ContourSequence");
        addElementToDataset(result, item, new DcmDecimalString(ContourSlabThickness), "1", "3", "ContourSequence");
        addElementToDataset(result, item, new DcmDecimalString(ContourOffsetVector), "3", "3", "ContourSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfContourPoints), "1", "1", "ContourSequence");
        addElementToDataset(result, item, new DcmDecimalString(ContourData), "3-3n", "1", "ContourSequence");
    }
    return result;
}


OFCondition DRTContourSequence::Item::getAttachedContours(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AttachedContours, value, pos);
}


OFCondition DRTContourSequence::Item::getAttachedContours(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, AttachedContours).getSint32(value, pos);
}


OFCondition DRTContourSequence::Item::getContourData(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContourData, value, pos);
}


OFCondition DRTContourSequence::Item::getContourData(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ContourData).getFloat64(value, pos);
}


OFCondition DRTContourSequence::Item::getContourData(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ContourData).getFloat64Vector(value);
}


OFCondition DRTContourSequence::Item::getContourGeometricType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContourGeometricType, value, pos);
}


OFCondition DRTContourSequence::Item::getContourNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContourNumber, value, pos);
}


OFCondition DRTContourSequence::Item::getContourNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ContourNumber).getSint32(value, pos);
}


OFCondition DRTContourSequence::Item::getContourOffsetVector(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContourOffsetVector, value, pos);
}


OFCondition DRTContourSequence::Item::getContourOffsetVector(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ContourOffsetVector).getFloat64(value, pos);
}


OFCondition DRTContourSequence::Item::getContourOffsetVector(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ContourOffsetVector).getFloat64Vector(value);
}


OFCondition DRTContourSequence::Item::getContourSlabThickness(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContourSlabThickness, value, pos);
}


OFCondition DRTContourSequence::Item::getContourSlabThickness(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ContourSlabThickness).getFloat64(value, pos);
}


OFCondition DRTContourSequence::Item::getNumberOfContourPoints(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfContourPoints, value, pos);
}


OFCondition DRTContourSequence::Item::getNumberOfContourPoints(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfContourPoints).getSint32(value, pos);
}


OFCondition DRTContourSequence::Item::setAttachedContours(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1-n") : EC_Normal;
        if (result.good())
            result = AttachedContours.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContourSequence::Item::setContourData(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "3-3n") : EC_Normal;
        if (result.good())
            result = ContourData.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContourSequence::Item::setContourGeometricType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ContourGeometricType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContourSequence::Item::setContourNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ContourNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContourSequence::Item::setContourOffsetVector(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "3") : EC_Normal;
        if (result.good())
            result = ContourOffsetVector.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContourSequence::Item::setContourSlabThickness(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ContourSlabThickness.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContourSequence::Item::setNumberOfContourPoints(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfContourPoints.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTContourSequence::DRTContourSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTContourSequence::DRTContourSequence(const DRTContourSequence &copy)
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


DRTContourSequence &DRTContourSequence::operator=(const DRTContourSequence &copy)
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


DRTContourSequence::~DRTContourSequence()
{
    clear();
}


void DRTContourSequence::clear()
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


OFBool DRTContourSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTContourSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTContourSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTContourSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTContourSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTContourSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTContourSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTContourSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTContourSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTContourSequence::Item &DRTContourSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTContourSequence::Item &DRTContourSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTContourSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTContourSequence::Item &DRTContourSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTContourSequence::Item &DRTContourSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTContourSequence::Item &DRTContourSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTContourSequence::Item &DRTContourSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTContourSequence::addItem(Item *&item)
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


OFCondition DRTContourSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTContourSequence::removeItem(const unsigned long pos)
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


OFCondition DRTContourSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ContourSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ContourSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTContourSequence::write(DcmItem &dataset,
                                      const OFString &card,
                                      const OFString &type,
                                      const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ContourSequence);
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
