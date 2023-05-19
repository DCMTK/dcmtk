/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTBeamDoseVerificationControlPointSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtbvcps.h"


// --- item class ---

DRTBeamDoseVerificationControlPointSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    BeamDosePointDepth(DCM_BeamDosePointDepth),
    BeamDosePointEquivalentDepth(DCM_BeamDosePointEquivalentDepth),
    BeamDosePointSSD(DCM_BeamDosePointSSD),
    BeamDosePointSourceToExternalContourDistance(DCM_BeamDosePointSourceToExternalContourDistance),
    CumulativeMetersetWeight(DCM_CumulativeMetersetWeight),
    ReferencedControlPointIndex(DCM_ReferencedControlPointIndex)
{
}


DRTBeamDoseVerificationControlPointSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    BeamDosePointDepth(copy.BeamDosePointDepth),
    BeamDosePointEquivalentDepth(copy.BeamDosePointEquivalentDepth),
    BeamDosePointSSD(copy.BeamDosePointSSD),
    BeamDosePointSourceToExternalContourDistance(copy.BeamDosePointSourceToExternalContourDistance),
    CumulativeMetersetWeight(copy.CumulativeMetersetWeight),
    ReferencedControlPointIndex(copy.ReferencedControlPointIndex)
{
}


DRTBeamDoseVerificationControlPointSequence::Item::~Item()
{
}


DRTBeamDoseVerificationControlPointSequence::Item &DRTBeamDoseVerificationControlPointSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        BeamDosePointDepth = copy.BeamDosePointDepth;
        BeamDosePointEquivalentDepth = copy.BeamDosePointEquivalentDepth;
        BeamDosePointSSD = copy.BeamDosePointSSD;
        BeamDosePointSourceToExternalContourDistance = copy.BeamDosePointSourceToExternalContourDistance;
        CumulativeMetersetWeight = copy.CumulativeMetersetWeight;
        ReferencedControlPointIndex = copy.ReferencedControlPointIndex;
    }
    return *this;
}


void DRTBeamDoseVerificationControlPointSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        CumulativeMetersetWeight.clear();
        ReferencedControlPointIndex.clear();
        BeamDosePointDepth.clear();
        BeamDosePointEquivalentDepth.clear();
        BeamDosePointSSD.clear();
        BeamDosePointSourceToExternalContourDistance.clear();
    }
}


OFBool DRTBeamDoseVerificationControlPointSequence::Item::isEmpty()
{
    return CumulativeMetersetWeight.isEmpty() &&
           ReferencedControlPointIndex.isEmpty() &&
           BeamDosePointDepth.isEmpty() &&
           BeamDosePointEquivalentDepth.isEmpty() &&
           BeamDosePointSSD.isEmpty() &&
           BeamDosePointSourceToExternalContourDistance.isEmpty();
}


OFBool DRTBeamDoseVerificationControlPointSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTBeamDoseVerificationControlPointSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, CumulativeMetersetWeight, "1", "1", "BeamDoseVerificationControlPointSequence");
        getAndCheckElementFromDataset(item, ReferencedControlPointIndex, "1", "1C", "BeamDoseVerificationControlPointSequence");
        getAndCheckElementFromDataset(item, BeamDosePointDepth, "1", "1C", "BeamDoseVerificationControlPointSequence");
        getAndCheckElementFromDataset(item, BeamDosePointEquivalentDepth, "1", "1C", "BeamDoseVerificationControlPointSequence");
        getAndCheckElementFromDataset(item, BeamDosePointSSD, "1", "1C", "BeamDoseVerificationControlPointSequence");
        getAndCheckElementFromDataset(item, BeamDosePointSourceToExternalContourDistance, "1", "3", "BeamDoseVerificationControlPointSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTBeamDoseVerificationControlPointSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmDecimalString(CumulativeMetersetWeight), "1", "1", "BeamDoseVerificationControlPointSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferencedControlPointIndex), "1", "1C", "BeamDoseVerificationControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(BeamDosePointDepth), "1", "1C", "BeamDoseVerificationControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(BeamDosePointEquivalentDepth), "1", "1C", "BeamDoseVerificationControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(BeamDosePointSSD), "1", "1C", "BeamDoseVerificationControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(BeamDosePointSourceToExternalContourDistance), "1", "3", "BeamDoseVerificationControlPointSequence");
    }
    return result;
}


OFCondition DRTBeamDoseVerificationControlPointSequence::Item::getBeamDosePointDepth(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, BeamDosePointDepth).getFloat32(value, pos);
}


OFCondition DRTBeamDoseVerificationControlPointSequence::Item::getBeamDosePointEquivalentDepth(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, BeamDosePointEquivalentDepth).getFloat32(value, pos);
}


OFCondition DRTBeamDoseVerificationControlPointSequence::Item::getBeamDosePointSSD(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, BeamDosePointSSD).getFloat32(value, pos);
}


OFCondition DRTBeamDoseVerificationControlPointSequence::Item::getBeamDosePointSourceToExternalContourDistance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamDosePointSourceToExternalContourDistance, value, pos);
}


OFCondition DRTBeamDoseVerificationControlPointSequence::Item::getBeamDosePointSourceToExternalContourDistance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, BeamDosePointSourceToExternalContourDistance).getFloat64(value, pos);
}


OFCondition DRTBeamDoseVerificationControlPointSequence::Item::getCumulativeMetersetWeight(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CumulativeMetersetWeight, value, pos);
}


OFCondition DRTBeamDoseVerificationControlPointSequence::Item::getCumulativeMetersetWeight(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CumulativeMetersetWeight).getFloat64(value, pos);
}


OFCondition DRTBeamDoseVerificationControlPointSequence::Item::getReferencedControlPointIndex(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedControlPointIndex, value, pos);
}


OFCondition DRTBeamDoseVerificationControlPointSequence::Item::getReferencedControlPointIndex(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedControlPointIndex).getSint32(value, pos);
}


OFCondition DRTBeamDoseVerificationControlPointSequence::Item::setBeamDosePointDepth(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return BeamDosePointDepth.putFloat32(value, pos);
}


OFCondition DRTBeamDoseVerificationControlPointSequence::Item::setBeamDosePointEquivalentDepth(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return BeamDosePointEquivalentDepth.putFloat32(value, pos);
}


OFCondition DRTBeamDoseVerificationControlPointSequence::Item::setBeamDosePointSSD(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return BeamDosePointSSD.putFloat32(value, pos);
}


OFCondition DRTBeamDoseVerificationControlPointSequence::Item::setBeamDosePointSourceToExternalContourDistance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BeamDosePointSourceToExternalContourDistance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBeamDoseVerificationControlPointSequence::Item::setCumulativeMetersetWeight(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CumulativeMetersetWeight.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBeamDoseVerificationControlPointSequence::Item::setReferencedControlPointIndex(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedControlPointIndex.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTBeamDoseVerificationControlPointSequence::DRTBeamDoseVerificationControlPointSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTBeamDoseVerificationControlPointSequence::DRTBeamDoseVerificationControlPointSequence(const DRTBeamDoseVerificationControlPointSequence &copy)
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


DRTBeamDoseVerificationControlPointSequence &DRTBeamDoseVerificationControlPointSequence::operator=(const DRTBeamDoseVerificationControlPointSequence &copy)
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


DRTBeamDoseVerificationControlPointSequence::~DRTBeamDoseVerificationControlPointSequence()
{
    clear();
}


void DRTBeamDoseVerificationControlPointSequence::clear()
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


OFBool DRTBeamDoseVerificationControlPointSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTBeamDoseVerificationControlPointSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTBeamDoseVerificationControlPointSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTBeamDoseVerificationControlPointSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTBeamDoseVerificationControlPointSequence::gotoNextItem()
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


OFCondition DRTBeamDoseVerificationControlPointSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTBeamDoseVerificationControlPointSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTBeamDoseVerificationControlPointSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTBeamDoseVerificationControlPointSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTBeamDoseVerificationControlPointSequence::Item &DRTBeamDoseVerificationControlPointSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTBeamDoseVerificationControlPointSequence::Item &DRTBeamDoseVerificationControlPointSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTBeamDoseVerificationControlPointSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTBeamDoseVerificationControlPointSequence::Item &DRTBeamDoseVerificationControlPointSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTBeamDoseVerificationControlPointSequence::Item &DRTBeamDoseVerificationControlPointSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTBeamDoseVerificationControlPointSequence::Item &DRTBeamDoseVerificationControlPointSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTBeamDoseVerificationControlPointSequence::Item &DRTBeamDoseVerificationControlPointSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTBeamDoseVerificationControlPointSequence::addItem(Item *&item)
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


OFCondition DRTBeamDoseVerificationControlPointSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTBeamDoseVerificationControlPointSequence::removeItem(const size_t pos)
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


OFCondition DRTBeamDoseVerificationControlPointSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_BeamDoseVerificationControlPointSequence, sequence);
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
            DcmSequenceOfItems element(DCM_BeamDoseVerificationControlPointSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTBeamDoseVerificationControlPointSequence::write(DcmItem &dataset,
                                                               const OFString &card,
                                                               const OFString &type,
                                                               const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_BeamDoseVerificationControlPointSequence);
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
