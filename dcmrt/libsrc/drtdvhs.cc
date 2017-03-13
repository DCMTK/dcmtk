/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTDVHSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtdvhs.h"


// --- item class ---

DRTDVHSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    DVHData(DCM_DVHData),
    DVHDoseScaling(DCM_DVHDoseScaling),
    DVHMaximumDose(DCM_DVHMaximumDose),
    DVHMeanDose(DCM_DVHMeanDose),
    DVHMinimumDose(DCM_DVHMinimumDose),
    DVHNumberOfBins(DCM_DVHNumberOfBins),
    DVHReferencedROISequence(emptyDefaultItem /*emptyDefaultSequence*/),
    DVHType(DCM_DVHType),
    DVHVolumeUnits(DCM_DVHVolumeUnits),
    DoseType(DCM_DoseType),
    DoseUnits(DCM_DoseUnits)
{
}


DRTDVHSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    DVHData(copy.DVHData),
    DVHDoseScaling(copy.DVHDoseScaling),
    DVHMaximumDose(copy.DVHMaximumDose),
    DVHMeanDose(copy.DVHMeanDose),
    DVHMinimumDose(copy.DVHMinimumDose),
    DVHNumberOfBins(copy.DVHNumberOfBins),
    DVHReferencedROISequence(copy.DVHReferencedROISequence),
    DVHType(copy.DVHType),
    DVHVolumeUnits(copy.DVHVolumeUnits),
    DoseType(copy.DoseType),
    DoseUnits(copy.DoseUnits)
{
}


DRTDVHSequence::Item::~Item()
{
}


DRTDVHSequence::Item &DRTDVHSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        DVHData = copy.DVHData;
        DVHDoseScaling = copy.DVHDoseScaling;
        DVHMaximumDose = copy.DVHMaximumDose;
        DVHMeanDose = copy.DVHMeanDose;
        DVHMinimumDose = copy.DVHMinimumDose;
        DVHNumberOfBins = copy.DVHNumberOfBins;
        DVHReferencedROISequence = copy.DVHReferencedROISequence;
        DVHType = copy.DVHType;
        DVHVolumeUnits = copy.DVHVolumeUnits;
        DoseType = copy.DoseType;
        DoseUnits = copy.DoseUnits;
    }
    return *this;
}


void DRTDVHSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        DVHReferencedROISequence.clear();
        DVHType.clear();
        DoseUnits.clear();
        DoseType.clear();
        DVHDoseScaling.clear();
        DVHVolumeUnits.clear();
        DVHNumberOfBins.clear();
        DVHData.clear();
        DVHMinimumDose.clear();
        DVHMaximumDose.clear();
        DVHMeanDose.clear();
    }
}


OFBool DRTDVHSequence::Item::isEmpty()
{
    return DVHReferencedROISequence.isEmpty() &&
           DVHType.isEmpty() &&
           DoseUnits.isEmpty() &&
           DoseType.isEmpty() &&
           DVHDoseScaling.isEmpty() &&
           DVHVolumeUnits.isEmpty() &&
           DVHNumberOfBins.isEmpty() &&
           DVHData.isEmpty() &&
           DVHMinimumDose.isEmpty() &&
           DVHMaximumDose.isEmpty() &&
           DVHMeanDose.isEmpty();
}


OFBool DRTDVHSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTDVHSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        DVHReferencedROISequence.read(item, "1-n", "1", "DVHSequence");
        getAndCheckElementFromDataset(item, DVHType, "1", "1", "DVHSequence");
        getAndCheckElementFromDataset(item, DoseUnits, "1", "1", "DVHSequence");
        getAndCheckElementFromDataset(item, DoseType, "1", "1", "DVHSequence");
        getAndCheckElementFromDataset(item, DVHDoseScaling, "1", "1", "DVHSequence");
        getAndCheckElementFromDataset(item, DVHVolumeUnits, "1", "1", "DVHSequence");
        getAndCheckElementFromDataset(item, DVHNumberOfBins, "1", "1", "DVHSequence");
        getAndCheckElementFromDataset(item, DVHData, "2-2n", "1", "DVHSequence");
        getAndCheckElementFromDataset(item, DVHMinimumDose, "1", "3", "DVHSequence");
        getAndCheckElementFromDataset(item, DVHMaximumDose, "1", "3", "DVHSequence");
        getAndCheckElementFromDataset(item, DVHMeanDose, "1", "3", "DVHSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTDVHSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        if (result.good()) result = DVHReferencedROISequence.write(item, "1-n", "1", "DVHSequence");
        addElementToDataset(result, item, new DcmCodeString(DVHType), "1", "1", "DVHSequence");
        addElementToDataset(result, item, new DcmCodeString(DoseUnits), "1", "1", "DVHSequence");
        addElementToDataset(result, item, new DcmCodeString(DoseType), "1", "1", "DVHSequence");
        addElementToDataset(result, item, new DcmDecimalString(DVHDoseScaling), "1", "1", "DVHSequence");
        addElementToDataset(result, item, new DcmCodeString(DVHVolumeUnits), "1", "1", "DVHSequence");
        addElementToDataset(result, item, new DcmIntegerString(DVHNumberOfBins), "1", "1", "DVHSequence");
        addElementToDataset(result, item, new DcmDecimalString(DVHData), "2-2n", "1", "DVHSequence");
        addElementToDataset(result, item, new DcmDecimalString(DVHMinimumDose), "1", "3", "DVHSequence");
        addElementToDataset(result, item, new DcmDecimalString(DVHMaximumDose), "1", "3", "DVHSequence");
        addElementToDataset(result, item, new DcmDecimalString(DVHMeanDose), "1", "3", "DVHSequence");
    }
    return result;
}


OFCondition DRTDVHSequence::Item::getDVHData(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DVHData, value, pos);
}


OFCondition DRTDVHSequence::Item::getDVHData(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DVHData).getFloat64(value, pos);
}


OFCondition DRTDVHSequence::Item::getDVHData(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DVHData).getFloat64Vector(value);
}


OFCondition DRTDVHSequence::Item::getDVHDoseScaling(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DVHDoseScaling, value, pos);
}


OFCondition DRTDVHSequence::Item::getDVHDoseScaling(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DVHDoseScaling).getFloat64(value, pos);
}


OFCondition DRTDVHSequence::Item::getDVHMaximumDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DVHMaximumDose, value, pos);
}


OFCondition DRTDVHSequence::Item::getDVHMaximumDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DVHMaximumDose).getFloat64(value, pos);
}


OFCondition DRTDVHSequence::Item::getDVHMeanDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DVHMeanDose, value, pos);
}


OFCondition DRTDVHSequence::Item::getDVHMeanDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DVHMeanDose).getFloat64(value, pos);
}


OFCondition DRTDVHSequence::Item::getDVHMinimumDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DVHMinimumDose, value, pos);
}


OFCondition DRTDVHSequence::Item::getDVHMinimumDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DVHMinimumDose).getFloat64(value, pos);
}


OFCondition DRTDVHSequence::Item::getDVHNumberOfBins(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DVHNumberOfBins, value, pos);
}


OFCondition DRTDVHSequence::Item::getDVHNumberOfBins(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, DVHNumberOfBins).getSint32(value, pos);
}


OFCondition DRTDVHSequence::Item::getDVHType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DVHType, value, pos);
}


OFCondition DRTDVHSequence::Item::getDVHVolumeUnits(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DVHVolumeUnits, value, pos);
}


OFCondition DRTDVHSequence::Item::getDoseType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DoseType, value, pos);
}


OFCondition DRTDVHSequence::Item::getDoseUnits(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DoseUnits, value, pos);
}


OFCondition DRTDVHSequence::Item::setDVHData(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "2-2n") : EC_Normal;
        if (result.good())
            result = DVHData.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDVHSequence::Item::setDVHDoseScaling(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DVHDoseScaling.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDVHSequence::Item::setDVHMaximumDose(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DVHMaximumDose.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDVHSequence::Item::setDVHMeanDose(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DVHMeanDose.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDVHSequence::Item::setDVHMinimumDose(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DVHMinimumDose.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDVHSequence::Item::setDVHNumberOfBins(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DVHNumberOfBins.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDVHSequence::Item::setDVHType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DVHType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDVHSequence::Item::setDVHVolumeUnits(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DVHVolumeUnits.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDVHSequence::Item::setDoseType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DoseType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDVHSequence::Item::setDoseUnits(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DoseUnits.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTDVHSequence::DRTDVHSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTDVHSequence::DRTDVHSequence(const DRTDVHSequence &copy)
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


DRTDVHSequence &DRTDVHSequence::operator=(const DRTDVHSequence &copy)
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


DRTDVHSequence::~DRTDVHSequence()
{
    clear();
}


void DRTDVHSequence::clear()
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


OFBool DRTDVHSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTDVHSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTDVHSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTDVHSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTDVHSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTDVHSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTDVHSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTDVHSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTDVHSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTDVHSequence::Item &DRTDVHSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTDVHSequence::Item &DRTDVHSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTDVHSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTDVHSequence::Item &DRTDVHSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTDVHSequence::Item &DRTDVHSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTDVHSequence::Item &DRTDVHSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTDVHSequence::Item &DRTDVHSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTDVHSequence::addItem(Item *&item)
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


OFCondition DRTDVHSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTDVHSequence::removeItem(const size_t pos)
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


OFCondition DRTDVHSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_DVHSequence, sequence);
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
            DcmSequenceOfItems element(DCM_DVHSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTDVHSequence::write(DcmItem &dataset,
                                  const OFString &card,
                                  const OFString &type,
                                  const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_DVHSequence);
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
