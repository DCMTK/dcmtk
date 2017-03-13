/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTDeliveredDepthDoseParametersSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtdddps.h"


// --- item class ---

DRTDeliveredDepthDoseParametersSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    DeliveredDistalDepth(DCM_DeliveredDistalDepth),
    DeliveredDistalDepthFraction(DCM_DeliveredDistalDepthFraction),
    DeliveredNominalRangeModulatedRegionDepths(DCM_DeliveredNominalRangeModulatedRegionDepths),
    DeliveredNominalRangeModulationFractions(DCM_DeliveredNominalRangeModulationFractions),
    DeliveredReferenceDoseDefinition(DCM_DeliveredReferenceDoseDefinition)
{
}


DRTDeliveredDepthDoseParametersSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    DeliveredDistalDepth(copy.DeliveredDistalDepth),
    DeliveredDistalDepthFraction(copy.DeliveredDistalDepthFraction),
    DeliveredNominalRangeModulatedRegionDepths(copy.DeliveredNominalRangeModulatedRegionDepths),
    DeliveredNominalRangeModulationFractions(copy.DeliveredNominalRangeModulationFractions),
    DeliveredReferenceDoseDefinition(copy.DeliveredReferenceDoseDefinition)
{
}


DRTDeliveredDepthDoseParametersSequence::Item::~Item()
{
}


DRTDeliveredDepthDoseParametersSequence::Item &DRTDeliveredDepthDoseParametersSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        DeliveredDistalDepth = copy.DeliveredDistalDepth;
        DeliveredDistalDepthFraction = copy.DeliveredDistalDepthFraction;
        DeliveredNominalRangeModulatedRegionDepths = copy.DeliveredNominalRangeModulatedRegionDepths;
        DeliveredNominalRangeModulationFractions = copy.DeliveredNominalRangeModulationFractions;
        DeliveredReferenceDoseDefinition = copy.DeliveredReferenceDoseDefinition;
    }
    return *this;
}


void DRTDeliveredDepthDoseParametersSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        DeliveredReferenceDoseDefinition.clear();
        DeliveredDistalDepth.clear();
        DeliveredDistalDepthFraction.clear();
        DeliveredNominalRangeModulatedRegionDepths.clear();
        DeliveredNominalRangeModulationFractions.clear();
    }
}


OFBool DRTDeliveredDepthDoseParametersSequence::Item::isEmpty()
{
    return DeliveredReferenceDoseDefinition.isEmpty() &&
           DeliveredDistalDepth.isEmpty() &&
           DeliveredDistalDepthFraction.isEmpty() &&
           DeliveredNominalRangeModulatedRegionDepths.isEmpty() &&
           DeliveredNominalRangeModulationFractions.isEmpty();
}


OFBool DRTDeliveredDepthDoseParametersSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTDeliveredDepthDoseParametersSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, DeliveredReferenceDoseDefinition, "1", "1", "DeliveredDepthDoseParametersSequence");
        getAndCheckElementFromDataset(item, DeliveredDistalDepth, "1", "1", "DeliveredDepthDoseParametersSequence");
        getAndCheckElementFromDataset(item, DeliveredDistalDepthFraction, "1", "1", "DeliveredDepthDoseParametersSequence");
        getAndCheckElementFromDataset(item, DeliveredNominalRangeModulatedRegionDepths, "2", "1C", "DeliveredDepthDoseParametersSequence");
        getAndCheckElementFromDataset(item, DeliveredNominalRangeModulationFractions, "2", "1C", "DeliveredDepthDoseParametersSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTDeliveredDepthDoseParametersSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmCodeString(DeliveredReferenceDoseDefinition), "1", "1", "DeliveredDepthDoseParametersSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(DeliveredDistalDepth), "1", "1", "DeliveredDepthDoseParametersSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(DeliveredDistalDepthFraction), "1", "1", "DeliveredDepthDoseParametersSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(DeliveredNominalRangeModulatedRegionDepths), "2", "1C", "DeliveredDepthDoseParametersSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(DeliveredNominalRangeModulationFractions), "2", "1C", "DeliveredDepthDoseParametersSequence");
    }
    return result;
}


OFCondition DRTDeliveredDepthDoseParametersSequence::Item::getDeliveredDistalDepth(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, DeliveredDistalDepth).getFloat32(value, pos);
}


OFCondition DRTDeliveredDepthDoseParametersSequence::Item::getDeliveredDistalDepthFraction(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, DeliveredDistalDepthFraction).getFloat32(value, pos);
}


OFCondition DRTDeliveredDepthDoseParametersSequence::Item::getDeliveredNominalRangeModulatedRegionDepths(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, DeliveredNominalRangeModulatedRegionDepths).getFloat32(value, pos);
}


OFCondition DRTDeliveredDepthDoseParametersSequence::Item::getDeliveredNominalRangeModulationFractions(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, DeliveredNominalRangeModulationFractions).getFloat32(value, pos);
}


OFCondition DRTDeliveredDepthDoseParametersSequence::Item::getDeliveredReferenceDoseDefinition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeliveredReferenceDoseDefinition, value, pos);
}


OFCondition DRTDeliveredDepthDoseParametersSequence::Item::setDeliveredDistalDepth(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return DeliveredDistalDepth.putFloat32(value, pos);
}


OFCondition DRTDeliveredDepthDoseParametersSequence::Item::setDeliveredDistalDepthFraction(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return DeliveredDistalDepthFraction.putFloat32(value, pos);
}


OFCondition DRTDeliveredDepthDoseParametersSequence::Item::setDeliveredNominalRangeModulatedRegionDepths(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return DeliveredNominalRangeModulatedRegionDepths.putFloat32(value, pos);
}


OFCondition DRTDeliveredDepthDoseParametersSequence::Item::setDeliveredNominalRangeModulationFractions(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return DeliveredNominalRangeModulationFractions.putFloat32(value, pos);
}


OFCondition DRTDeliveredDepthDoseParametersSequence::Item::setDeliveredReferenceDoseDefinition(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DeliveredReferenceDoseDefinition.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTDeliveredDepthDoseParametersSequence::DRTDeliveredDepthDoseParametersSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTDeliveredDepthDoseParametersSequence::DRTDeliveredDepthDoseParametersSequence(const DRTDeliveredDepthDoseParametersSequence &copy)
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


DRTDeliveredDepthDoseParametersSequence &DRTDeliveredDepthDoseParametersSequence::operator=(const DRTDeliveredDepthDoseParametersSequence &copy)
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


DRTDeliveredDepthDoseParametersSequence::~DRTDeliveredDepthDoseParametersSequence()
{
    clear();
}


void DRTDeliveredDepthDoseParametersSequence::clear()
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


OFBool DRTDeliveredDepthDoseParametersSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTDeliveredDepthDoseParametersSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTDeliveredDepthDoseParametersSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTDeliveredDepthDoseParametersSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTDeliveredDepthDoseParametersSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTDeliveredDepthDoseParametersSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTDeliveredDepthDoseParametersSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTDeliveredDepthDoseParametersSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTDeliveredDepthDoseParametersSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTDeliveredDepthDoseParametersSequence::Item &DRTDeliveredDepthDoseParametersSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTDeliveredDepthDoseParametersSequence::Item &DRTDeliveredDepthDoseParametersSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTDeliveredDepthDoseParametersSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTDeliveredDepthDoseParametersSequence::Item &DRTDeliveredDepthDoseParametersSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTDeliveredDepthDoseParametersSequence::Item &DRTDeliveredDepthDoseParametersSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTDeliveredDepthDoseParametersSequence::Item &DRTDeliveredDepthDoseParametersSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTDeliveredDepthDoseParametersSequence::Item &DRTDeliveredDepthDoseParametersSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTDeliveredDepthDoseParametersSequence::addItem(Item *&item)
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


OFCondition DRTDeliveredDepthDoseParametersSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTDeliveredDepthDoseParametersSequence::removeItem(const size_t pos)
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


OFCondition DRTDeliveredDepthDoseParametersSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_DeliveredDepthDoseParametersSequence, sequence);
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
            DcmSequenceOfItems element(DCM_DeliveredDepthDoseParametersSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTDeliveredDepthDoseParametersSequence::write(DcmItem &dataset,
                                                           const OFString &card,
                                                           const OFString &type,
                                                           const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_DeliveredDepthDoseParametersSequence);
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
