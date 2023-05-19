/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTDoseCalibrationConditionsSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtdccs.h"


// --- item class ---

DRTDoseCalibrationConditionsSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AbsorbedDoseToMetersetRatio(DCM_AbsorbedDoseToMetersetRatio),
    CalibrationDateTime(DCM_CalibrationDateTime),
    CalibrationReferencePointDepth(DCM_CalibrationReferencePointDepth),
    DelineatedRadiationFieldSize(DCM_DelineatedRadiationFieldSize),
    SourceToSurfaceDistance(DCM_SourceToSurfaceDistance)
{
}


DRTDoseCalibrationConditionsSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AbsorbedDoseToMetersetRatio(copy.AbsorbedDoseToMetersetRatio),
    CalibrationDateTime(copy.CalibrationDateTime),
    CalibrationReferencePointDepth(copy.CalibrationReferencePointDepth),
    DelineatedRadiationFieldSize(copy.DelineatedRadiationFieldSize),
    SourceToSurfaceDistance(copy.SourceToSurfaceDistance)
{
}


DRTDoseCalibrationConditionsSequence::Item::~Item()
{
}


DRTDoseCalibrationConditionsSequence::Item &DRTDoseCalibrationConditionsSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AbsorbedDoseToMetersetRatio = copy.AbsorbedDoseToMetersetRatio;
        CalibrationDateTime = copy.CalibrationDateTime;
        CalibrationReferencePointDepth = copy.CalibrationReferencePointDepth;
        DelineatedRadiationFieldSize = copy.DelineatedRadiationFieldSize;
        SourceToSurfaceDistance = copy.SourceToSurfaceDistance;
    }
    return *this;
}


void DRTDoseCalibrationConditionsSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        AbsorbedDoseToMetersetRatio.clear();
        DelineatedRadiationFieldSize.clear();
        CalibrationReferencePointDepth.clear();
        SourceToSurfaceDistance.clear();
        CalibrationDateTime.clear();
    }
}


OFBool DRTDoseCalibrationConditionsSequence::Item::isEmpty()
{
    return AbsorbedDoseToMetersetRatio.isEmpty() &&
           DelineatedRadiationFieldSize.isEmpty() &&
           CalibrationReferencePointDepth.isEmpty() &&
           SourceToSurfaceDistance.isEmpty() &&
           CalibrationDateTime.isEmpty();
}


OFBool DRTDoseCalibrationConditionsSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTDoseCalibrationConditionsSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, AbsorbedDoseToMetersetRatio, "1", "1", "DoseCalibrationConditionsSequence");
        getAndCheckElementFromDataset(item, DelineatedRadiationFieldSize, "2", "1", "DoseCalibrationConditionsSequence");
        getAndCheckElementFromDataset(item, CalibrationReferencePointDepth, "1", "1", "DoseCalibrationConditionsSequence");
        getAndCheckElementFromDataset(item, SourceToSurfaceDistance, "1", "1", "DoseCalibrationConditionsSequence");
        getAndCheckElementFromDataset(item, CalibrationDateTime, "1", "2", "DoseCalibrationConditionsSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTDoseCalibrationConditionsSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmFloatingPointDouble(AbsorbedDoseToMetersetRatio), "1", "1", "DoseCalibrationConditionsSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(DelineatedRadiationFieldSize), "2", "1", "DoseCalibrationConditionsSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(CalibrationReferencePointDepth), "1", "1", "DoseCalibrationConditionsSequence");
        addElementToDataset(result, item, new DcmDecimalString(SourceToSurfaceDistance), "1", "1", "DoseCalibrationConditionsSequence");
        addElementToDataset(result, item, new DcmDateTime(CalibrationDateTime), "1", "2", "DoseCalibrationConditionsSequence");
    }
    return result;
}


OFCondition DRTDoseCalibrationConditionsSequence::Item::getAbsorbedDoseToMetersetRatio(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, AbsorbedDoseToMetersetRatio).getFloat64(value, pos);
}


OFCondition DRTDoseCalibrationConditionsSequence::Item::getCalibrationDateTime(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CalibrationDateTime, value, pos);
}


OFCondition DRTDoseCalibrationConditionsSequence::Item::getCalibrationReferencePointDepth(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, CalibrationReferencePointDepth).getFloat64(value, pos);
}


OFCondition DRTDoseCalibrationConditionsSequence::Item::getDelineatedRadiationFieldSize(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, DelineatedRadiationFieldSize).getFloat64(value, pos);
}


OFCondition DRTDoseCalibrationConditionsSequence::Item::getSourceToSurfaceDistance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceToSurfaceDistance, value, pos);
}


OFCondition DRTDoseCalibrationConditionsSequence::Item::getSourceToSurfaceDistance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SourceToSurfaceDistance).getFloat64(value, pos);
}


OFCondition DRTDoseCalibrationConditionsSequence::Item::setAbsorbedDoseToMetersetRatio(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return AbsorbedDoseToMetersetRatio.putFloat64(value, pos);
}


OFCondition DRTDoseCalibrationConditionsSequence::Item::setCalibrationDateTime(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CalibrationDateTime.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseCalibrationConditionsSequence::Item::setCalibrationReferencePointDepth(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return CalibrationReferencePointDepth.putFloat64(value, pos);
}


OFCondition DRTDoseCalibrationConditionsSequence::Item::setDelineatedRadiationFieldSize(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return DelineatedRadiationFieldSize.putFloat64(value, pos);
}


OFCondition DRTDoseCalibrationConditionsSequence::Item::setSourceToSurfaceDistance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceToSurfaceDistance.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTDoseCalibrationConditionsSequence::DRTDoseCalibrationConditionsSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTDoseCalibrationConditionsSequence::DRTDoseCalibrationConditionsSequence(const DRTDoseCalibrationConditionsSequence &copy)
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


DRTDoseCalibrationConditionsSequence &DRTDoseCalibrationConditionsSequence::operator=(const DRTDoseCalibrationConditionsSequence &copy)
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


DRTDoseCalibrationConditionsSequence::~DRTDoseCalibrationConditionsSequence()
{
    clear();
}


void DRTDoseCalibrationConditionsSequence::clear()
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


OFBool DRTDoseCalibrationConditionsSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTDoseCalibrationConditionsSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTDoseCalibrationConditionsSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTDoseCalibrationConditionsSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTDoseCalibrationConditionsSequence::gotoNextItem()
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


OFCondition DRTDoseCalibrationConditionsSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTDoseCalibrationConditionsSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTDoseCalibrationConditionsSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTDoseCalibrationConditionsSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTDoseCalibrationConditionsSequence::Item &DRTDoseCalibrationConditionsSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTDoseCalibrationConditionsSequence::Item &DRTDoseCalibrationConditionsSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTDoseCalibrationConditionsSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTDoseCalibrationConditionsSequence::Item &DRTDoseCalibrationConditionsSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTDoseCalibrationConditionsSequence::Item &DRTDoseCalibrationConditionsSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTDoseCalibrationConditionsSequence::Item &DRTDoseCalibrationConditionsSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTDoseCalibrationConditionsSequence::Item &DRTDoseCalibrationConditionsSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTDoseCalibrationConditionsSequence::addItem(Item *&item)
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


OFCondition DRTDoseCalibrationConditionsSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTDoseCalibrationConditionsSequence::removeItem(const size_t pos)
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


OFCondition DRTDoseCalibrationConditionsSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_DoseCalibrationConditionsSequence, sequence);
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
            DcmSequenceOfItems element(DCM_DoseCalibrationConditionsSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTDoseCalibrationConditionsSequence::write(DcmItem &dataset,
                                                        const OFString &card,
                                                        const OFString &type,
                                                        const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_DoseCalibrationConditionsSequence);
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
