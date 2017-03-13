/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTRealWorldValueMappingSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtrwvms.h"


// --- item class ---

DRTRealWorldValueMappingSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    DoubleFloatRealWorldValueFirstValueMapped(DCM_DoubleFloatRealWorldValueFirstValueMapped),
    DoubleFloatRealWorldValueLastValueMapped(DCM_DoubleFloatRealWorldValueLastValueMapped),
    LUTExplanation(DCM_LUTExplanation),
    LUTLabel(DCM_LUTLabel),
    MeasurementUnitsCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    QuantityDefinitionSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RealWorldValueFirstValueMapped(DCM_RealWorldValueFirstValueMapped),
    RealWorldValueIntercept(DCM_RealWorldValueIntercept),
    RealWorldValueLUTData(DCM_RealWorldValueLUTData),
    RealWorldValueLastValueMapped(DCM_RealWorldValueLastValueMapped),
    RealWorldValueSlope(DCM_RealWorldValueSlope)
{
}


DRTRealWorldValueMappingSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    DoubleFloatRealWorldValueFirstValueMapped(copy.DoubleFloatRealWorldValueFirstValueMapped),
    DoubleFloatRealWorldValueLastValueMapped(copy.DoubleFloatRealWorldValueLastValueMapped),
    LUTExplanation(copy.LUTExplanation),
    LUTLabel(copy.LUTLabel),
    MeasurementUnitsCodeSequence(copy.MeasurementUnitsCodeSequence),
    QuantityDefinitionSequence(copy.QuantityDefinitionSequence),
    RealWorldValueFirstValueMapped(copy.RealWorldValueFirstValueMapped),
    RealWorldValueIntercept(copy.RealWorldValueIntercept),
    RealWorldValueLUTData(copy.RealWorldValueLUTData),
    RealWorldValueLastValueMapped(copy.RealWorldValueLastValueMapped),
    RealWorldValueSlope(copy.RealWorldValueSlope)
{
}


DRTRealWorldValueMappingSequence::Item::~Item()
{
}


DRTRealWorldValueMappingSequence::Item &DRTRealWorldValueMappingSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        DoubleFloatRealWorldValueFirstValueMapped = copy.DoubleFloatRealWorldValueFirstValueMapped;
        DoubleFloatRealWorldValueLastValueMapped = copy.DoubleFloatRealWorldValueLastValueMapped;
        LUTExplanation = copy.LUTExplanation;
        LUTLabel = copy.LUTLabel;
        MeasurementUnitsCodeSequence = copy.MeasurementUnitsCodeSequence;
        QuantityDefinitionSequence = copy.QuantityDefinitionSequence;
        RealWorldValueFirstValueMapped = copy.RealWorldValueFirstValueMapped;
        RealWorldValueIntercept = copy.RealWorldValueIntercept;
        RealWorldValueLUTData = copy.RealWorldValueLUTData;
        RealWorldValueLastValueMapped = copy.RealWorldValueLastValueMapped;
        RealWorldValueSlope = copy.RealWorldValueSlope;
    }
    return *this;
}


void DRTRealWorldValueMappingSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        RealWorldValueFirstValueMapped.clear();
        RealWorldValueLastValueMapped.clear();
        DoubleFloatRealWorldValueFirstValueMapped.clear();
        DoubleFloatRealWorldValueLastValueMapped.clear();
        RealWorldValueIntercept.clear();
        RealWorldValueSlope.clear();
        RealWorldValueLUTData.clear();
        LUTExplanation.clear();
        LUTLabel.clear();
        MeasurementUnitsCodeSequence.clear();
        QuantityDefinitionSequence.clear();
    }
}


OFBool DRTRealWorldValueMappingSequence::Item::isEmpty()
{
    return RealWorldValueFirstValueMapped.isEmpty() &&
           RealWorldValueLastValueMapped.isEmpty() &&
           DoubleFloatRealWorldValueFirstValueMapped.isEmpty() &&
           DoubleFloatRealWorldValueLastValueMapped.isEmpty() &&
           RealWorldValueIntercept.isEmpty() &&
           RealWorldValueSlope.isEmpty() &&
           RealWorldValueLUTData.isEmpty() &&
           LUTExplanation.isEmpty() &&
           LUTLabel.isEmpty() &&
           MeasurementUnitsCodeSequence.isEmpty() &&
           QuantityDefinitionSequence.isEmpty();
}


OFBool DRTRealWorldValueMappingSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTRealWorldValueMappingSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, RealWorldValueFirstValueMapped, "1", "1C", "RealWorldValueMappingSequence");
        getAndCheckElementFromDataset(item, RealWorldValueLastValueMapped, "1", "1C", "RealWorldValueMappingSequence");
        getAndCheckElementFromDataset(item, DoubleFloatRealWorldValueFirstValueMapped, "1", "1C", "RealWorldValueMappingSequence");
        getAndCheckElementFromDataset(item, DoubleFloatRealWorldValueLastValueMapped, "1", "1C", "RealWorldValueMappingSequence");
        getAndCheckElementFromDataset(item, RealWorldValueIntercept, "1", "1C", "RealWorldValueMappingSequence");
        getAndCheckElementFromDataset(item, RealWorldValueSlope, "1", "1C", "RealWorldValueMappingSequence");
        getAndCheckElementFromDataset(item, RealWorldValueLUTData, "1-n", "1C", "RealWorldValueMappingSequence");
        getAndCheckElementFromDataset(item, LUTExplanation, "1", "1", "RealWorldValueMappingSequence");
        getAndCheckElementFromDataset(item, LUTLabel, "1", "1", "RealWorldValueMappingSequence");
        MeasurementUnitsCodeSequence.read(item, "1-n", "1", "RealWorldValueMappingSequence");
        QuantityDefinitionSequence.read(item, "1-n", "3", "RealWorldValueMappingSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTRealWorldValueMappingSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmUnsignedShort(RealWorldValueFirstValueMapped), "1", "1C", "RealWorldValueMappingSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(RealWorldValueLastValueMapped), "1", "1C", "RealWorldValueMappingSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(DoubleFloatRealWorldValueFirstValueMapped), "1", "1C", "RealWorldValueMappingSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(DoubleFloatRealWorldValueLastValueMapped), "1", "1C", "RealWorldValueMappingSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(RealWorldValueIntercept), "1", "1C", "RealWorldValueMappingSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(RealWorldValueSlope), "1", "1C", "RealWorldValueMappingSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(RealWorldValueLUTData), "1-n", "1C", "RealWorldValueMappingSequence");
        addElementToDataset(result, item, new DcmLongString(LUTExplanation), "1", "1", "RealWorldValueMappingSequence");
        addElementToDataset(result, item, new DcmShortString(LUTLabel), "1", "1", "RealWorldValueMappingSequence");
        if (result.good()) result = MeasurementUnitsCodeSequence.write(item, "1-n", "1", "RealWorldValueMappingSequence");
        if (result.good()) result = QuantityDefinitionSequence.write(item, "1-n", "3", "RealWorldValueMappingSequence");
    }
    return result;
}


OFCondition DRTRealWorldValueMappingSequence::Item::getDoubleFloatRealWorldValueFirstValueMapped(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, DoubleFloatRealWorldValueFirstValueMapped).getFloat64(value, pos);
}


OFCondition DRTRealWorldValueMappingSequence::Item::getDoubleFloatRealWorldValueLastValueMapped(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, DoubleFloatRealWorldValueLastValueMapped).getFloat64(value, pos);
}


OFCondition DRTRealWorldValueMappingSequence::Item::getLUTExplanation(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(LUTExplanation, value, pos);
}


OFCondition DRTRealWorldValueMappingSequence::Item::getLUTLabel(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(LUTLabel, value, pos);
}


OFCondition DRTRealWorldValueMappingSequence::Item::getRealWorldValueFirstValueMapped(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, RealWorldValueFirstValueMapped).getUint16(value, pos);
}


OFCondition DRTRealWorldValueMappingSequence::Item::getRealWorldValueIntercept(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, RealWorldValueIntercept).getFloat64(value, pos);
}


OFCondition DRTRealWorldValueMappingSequence::Item::getRealWorldValueLUTData(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, RealWorldValueLUTData).getFloat64(value, pos);
}


OFCondition DRTRealWorldValueMappingSequence::Item::getRealWorldValueLastValueMapped(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, RealWorldValueLastValueMapped).getUint16(value, pos);
}


OFCondition DRTRealWorldValueMappingSequence::Item::getRealWorldValueSlope(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, RealWorldValueSlope).getFloat64(value, pos);
}


OFCondition DRTRealWorldValueMappingSequence::Item::setDoubleFloatRealWorldValueFirstValueMapped(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return DoubleFloatRealWorldValueFirstValueMapped.putFloat64(value, pos);
}


OFCondition DRTRealWorldValueMappingSequence::Item::setDoubleFloatRealWorldValueLastValueMapped(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return DoubleFloatRealWorldValueLastValueMapped.putFloat64(value, pos);
}


OFCondition DRTRealWorldValueMappingSequence::Item::setLUTExplanation(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = LUTExplanation.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRealWorldValueMappingSequence::Item::setLUTLabel(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = LUTLabel.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRealWorldValueMappingSequence::Item::setRealWorldValueFirstValueMapped(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RealWorldValueFirstValueMapped.putUint16(value, pos);
}


OFCondition DRTRealWorldValueMappingSequence::Item::setRealWorldValueIntercept(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RealWorldValueIntercept.putFloat64(value, pos);
}


OFCondition DRTRealWorldValueMappingSequence::Item::setRealWorldValueLUTData(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RealWorldValueLUTData.putFloat64(value, pos);
}


OFCondition DRTRealWorldValueMappingSequence::Item::setRealWorldValueLastValueMapped(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RealWorldValueLastValueMapped.putUint16(value, pos);
}


OFCondition DRTRealWorldValueMappingSequence::Item::setRealWorldValueSlope(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RealWorldValueSlope.putFloat64(value, pos);
}


// --- sequence class ---

DRTRealWorldValueMappingSequence::DRTRealWorldValueMappingSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTRealWorldValueMappingSequence::DRTRealWorldValueMappingSequence(const DRTRealWorldValueMappingSequence &copy)
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


DRTRealWorldValueMappingSequence &DRTRealWorldValueMappingSequence::operator=(const DRTRealWorldValueMappingSequence &copy)
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


DRTRealWorldValueMappingSequence::~DRTRealWorldValueMappingSequence()
{
    clear();
}


void DRTRealWorldValueMappingSequence::clear()
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


OFBool DRTRealWorldValueMappingSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTRealWorldValueMappingSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTRealWorldValueMappingSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTRealWorldValueMappingSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTRealWorldValueMappingSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTRealWorldValueMappingSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTRealWorldValueMappingSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTRealWorldValueMappingSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTRealWorldValueMappingSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTRealWorldValueMappingSequence::Item &DRTRealWorldValueMappingSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTRealWorldValueMappingSequence::Item &DRTRealWorldValueMappingSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTRealWorldValueMappingSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTRealWorldValueMappingSequence::Item &DRTRealWorldValueMappingSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTRealWorldValueMappingSequence::Item &DRTRealWorldValueMappingSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTRealWorldValueMappingSequence::Item &DRTRealWorldValueMappingSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTRealWorldValueMappingSequence::Item &DRTRealWorldValueMappingSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTRealWorldValueMappingSequence::addItem(Item *&item)
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


OFCondition DRTRealWorldValueMappingSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTRealWorldValueMappingSequence::removeItem(const size_t pos)
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


OFCondition DRTRealWorldValueMappingSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_RealWorldValueMappingSequence, sequence);
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
            DcmSequenceOfItems element(DCM_RealWorldValueMappingSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTRealWorldValueMappingSequence::write(DcmItem &dataset,
                                                    const OFString &card,
                                                    const OFString &type,
                                                    const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_RealWorldValueMappingSequence);
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
