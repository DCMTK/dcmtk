/*
 *
 *  Copyright (c) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTExposureSequence
 *
 *  Generated automatically from DICOM PS 3.3-2007
 *  File created on 2012-02-01 16:51:15 
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtes.h"


// --- item class ---

DRTExposureSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ApplicatorSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    BeamLimitingDeviceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    BlockSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    DiaphragmPosition(DCM_DiaphragmPosition),
    ExposureTime(DCM_ExposureTime),
    FluenceDataScale(DCM_FluenceDataScale),
    FluenceDataSource(DCM_FluenceDataSource),
    KVP(DCM_KVP),
    MetersetExposure(DCM_MetersetExposure),
    NumberOfBlocks(DCM_NumberOfBlocks),
    ReferencedFrameNumber(DCM_ReferencedFrameNumber),
    XRayTubeCurrent(DCM_XRayTubeCurrent)
{
}


DRTExposureSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ApplicatorSequence(copy.ApplicatorSequence),
    BeamLimitingDeviceSequence(copy.BeamLimitingDeviceSequence),
    BlockSequence(copy.BlockSequence),
    DiaphragmPosition(copy.DiaphragmPosition),
    ExposureTime(copy.ExposureTime),
    FluenceDataScale(copy.FluenceDataScale),
    FluenceDataSource(copy.FluenceDataSource),
    KVP(copy.KVP),
    MetersetExposure(copy.MetersetExposure),
    NumberOfBlocks(copy.NumberOfBlocks),
    ReferencedFrameNumber(copy.ReferencedFrameNumber),
    XRayTubeCurrent(copy.XRayTubeCurrent)
{
}


DRTExposureSequence::Item::~Item()
{
}


DRTExposureSequence::Item &DRTExposureSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ApplicatorSequence = copy.ApplicatorSequence;
        BeamLimitingDeviceSequence = copy.BeamLimitingDeviceSequence;
        BlockSequence = copy.BlockSequence;
        DiaphragmPosition = copy.DiaphragmPosition;
        ExposureTime = copy.ExposureTime;
        FluenceDataScale = copy.FluenceDataScale;
        FluenceDataSource = copy.FluenceDataSource;
        KVP = copy.KVP;
        MetersetExposure = copy.MetersetExposure;
        NumberOfBlocks = copy.NumberOfBlocks;
        ReferencedFrameNumber = copy.ReferencedFrameNumber;
        XRayTubeCurrent = copy.XRayTubeCurrent;
    }
    return *this;
}


void DRTExposureSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ReferencedFrameNumber.clear();
        KVP.clear();
        XRayTubeCurrent.clear();
        ExposureTime.clear();
        MetersetExposure.clear();
        DiaphragmPosition.clear();
        BeamLimitingDeviceSequence.clear();
        ApplicatorSequence.clear();
        NumberOfBlocks.clear();
        BlockSequence.clear();
        FluenceDataSource.clear();
        FluenceDataScale.clear();
    }
}


OFBool DRTExposureSequence::Item::isEmpty()
{
    return ReferencedFrameNumber.isEmpty() &&
           KVP.isEmpty() &&
           XRayTubeCurrent.isEmpty() &&
           ExposureTime.isEmpty() &&
           MetersetExposure.isEmpty() &&
           DiaphragmPosition.isEmpty() &&
           BeamLimitingDeviceSequence.isEmpty() &&
           ApplicatorSequence.isEmpty() &&
           NumberOfBlocks.isEmpty() &&
           BlockSequence.isEmpty() &&
           FluenceDataSource.isEmpty() &&
           FluenceDataScale.isEmpty();
}


OFBool DRTExposureSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTExposureSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ReferencedFrameNumber, "1-n", "1C", "ExposureSequence");
        getAndCheckElementFromDataset(item, KVP, "1", "2C", "ExposureSequence");
        getAndCheckElementFromDataset(item, XRayTubeCurrent, "1", "2C", "ExposureSequence");
        getAndCheckElementFromDataset(item, ExposureTime, "1", "2C", "ExposureSequence");
        getAndCheckElementFromDataset(item, MetersetExposure, "1", "2C", "ExposureSequence");
        getAndCheckElementFromDataset(item, DiaphragmPosition, "4", "3", "ExposureSequence");
        BeamLimitingDeviceSequence.read(item, "1-n", "3", "ExposureSequence");
        ApplicatorSequence.read(item, "1-n", "3", "ExposureSequence");
        getAndCheckElementFromDataset(item, NumberOfBlocks, "1", "1C", "ExposureSequence");
        BlockSequence.read(item, "1-n", "2C", "ExposureSequence");
        getAndCheckElementFromDataset(item, FluenceDataSource, "1", "1", "ExposureSequence");
        getAndCheckElementFromDataset(item, FluenceDataScale, "1", "3", "ExposureSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTExposureSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ReferencedFrameNumber), "1-n", "1C", "ExposureSequence");
        addElementToDataset(result, item, new DcmDecimalString(KVP), "1", "2C", "ExposureSequence");
        addElementToDataset(result, item, new DcmIntegerString(XRayTubeCurrent), "1", "2C", "ExposureSequence");
        addElementToDataset(result, item, new DcmIntegerString(ExposureTime), "1", "2C", "ExposureSequence");
        addElementToDataset(result, item, new DcmDecimalString(MetersetExposure), "1", "2C", "ExposureSequence");
        addElementToDataset(result, item, new DcmDecimalString(DiaphragmPosition), "4", "3", "ExposureSequence");
        if (result.good()) result = BeamLimitingDeviceSequence.write(item, "1-n", "3", "ExposureSequence");
        if (result.good()) result = ApplicatorSequence.write(item, "1-n", "3", "ExposureSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfBlocks), "1", "1C", "ExposureSequence");
        if (result.good()) result = BlockSequence.write(item, "1-n", "2C", "ExposureSequence");
        addElementToDataset(result, item, new DcmCodeString(FluenceDataSource), "1", "1", "ExposureSequence");
        addElementToDataset(result, item, new DcmDecimalString(FluenceDataScale), "1", "3", "ExposureSequence");
    }
    return result;
}


OFCondition DRTExposureSequence::Item::getDiaphragmPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DiaphragmPosition, value, pos);
}


OFCondition DRTExposureSequence::Item::getDiaphragmPosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DiaphragmPosition).getFloat64(value, pos);
}


OFCondition DRTExposureSequence::Item::getDiaphragmPosition(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DiaphragmPosition).getFloat64Vector(value);
}


OFCondition DRTExposureSequence::Item::getExposureTime(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ExposureTime, value, pos);
}


OFCondition DRTExposureSequence::Item::getExposureTime(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ExposureTime).getSint32(value, pos);
}


OFCondition DRTExposureSequence::Item::getFluenceDataScale(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FluenceDataScale, value, pos);
}


OFCondition DRTExposureSequence::Item::getFluenceDataScale(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, FluenceDataScale).getFloat64(value, pos);
}


OFCondition DRTExposureSequence::Item::getFluenceDataSource(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FluenceDataSource, value, pos);
}


OFCondition DRTExposureSequence::Item::getKVP(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(KVP, value, pos);
}


OFCondition DRTExposureSequence::Item::getKVP(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, KVP).getFloat64(value, pos);
}


OFCondition DRTExposureSequence::Item::getMetersetExposure(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MetersetExposure, value, pos);
}


OFCondition DRTExposureSequence::Item::getMetersetExposure(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, MetersetExposure).getFloat64(value, pos);
}


OFCondition DRTExposureSequence::Item::getNumberOfBlocks(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfBlocks, value, pos);
}


OFCondition DRTExposureSequence::Item::getNumberOfBlocks(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfBlocks).getSint32(value, pos);
}


OFCondition DRTExposureSequence::Item::getReferencedFrameNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedFrameNumber, value, pos);
}


OFCondition DRTExposureSequence::Item::getReferencedFrameNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedFrameNumber).getSint32(value, pos);
}


OFCondition DRTExposureSequence::Item::getXRayTubeCurrent(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(XRayTubeCurrent, value, pos);
}


OFCondition DRTExposureSequence::Item::getXRayTubeCurrent(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, XRayTubeCurrent).getSint32(value, pos);
}


OFCondition DRTExposureSequence::Item::setDiaphragmPosition(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "4") : EC_Normal;
        if (result.good())
            result = DiaphragmPosition.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTExposureSequence::Item::setExposureTime(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ExposureTime.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTExposureSequence::Item::setFluenceDataScale(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = FluenceDataScale.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTExposureSequence::Item::setFluenceDataSource(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = FluenceDataSource.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTExposureSequence::Item::setKVP(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = KVP.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTExposureSequence::Item::setMetersetExposure(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = MetersetExposure.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTExposureSequence::Item::setNumberOfBlocks(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfBlocks.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTExposureSequence::Item::setReferencedFrameNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1-n") : EC_Normal;
        if (result.good())
            result = ReferencedFrameNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTExposureSequence::Item::setXRayTubeCurrent(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = XRayTubeCurrent.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTExposureSequence::DRTExposureSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTExposureSequence::DRTExposureSequence(const DRTExposureSequence &copy)
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


DRTExposureSequence &DRTExposureSequence::operator=(const DRTExposureSequence &copy)
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


DRTExposureSequence::~DRTExposureSequence()
{
    clear();
}


void DRTExposureSequence::clear()
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


OFBool DRTExposureSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTExposureSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTExposureSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTExposureSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTExposureSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTExposureSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTExposureSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTExposureSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTExposureSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTExposureSequence::Item &DRTExposureSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTExposureSequence::Item &DRTExposureSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTExposureSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTExposureSequence::Item &DRTExposureSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTExposureSequence::Item &DRTExposureSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTExposureSequence::Item &DRTExposureSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTExposureSequence::Item &DRTExposureSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTExposureSequence::addItem(Item *&item)
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


OFCondition DRTExposureSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTExposureSequence::removeItem(const unsigned long pos)
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


OFCondition DRTExposureSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ExposureSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ExposureSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTExposureSequence::write(DcmItem &dataset,
                                       const OFString &card,
                                       const OFString &type,
                                       const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ExposureSequence);
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
