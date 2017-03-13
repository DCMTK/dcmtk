/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTReferencedBeamSequenceInRTFractionSchemeModule
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtrbs8.h"


// --- item class ---

DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AlternateBeamDose(DCM_AlternateBeamDose),
    AlternateBeamDoseType(DCM_AlternateBeamDoseType),
    BeamDeliveryDurationLimit(DCM_BeamDeliveryDurationLimit),
    BeamDose(DCM_BeamDose),
    BeamDoseSpecificationPoint(DCM_BeamDoseSpecificationPoint),
    BeamDoseType(DCM_BeamDoseType),
    BeamDoseVerificationControlPointSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    BeamMeterset(DCM_BeamMeterset),
    ReferencedBeamNumber(DCM_ReferencedBeamNumber)
{
}


DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AlternateBeamDose(copy.AlternateBeamDose),
    AlternateBeamDoseType(copy.AlternateBeamDoseType),
    BeamDeliveryDurationLimit(copy.BeamDeliveryDurationLimit),
    BeamDose(copy.BeamDose),
    BeamDoseSpecificationPoint(copy.BeamDoseSpecificationPoint),
    BeamDoseType(copy.BeamDoseType),
    BeamDoseVerificationControlPointSequence(copy.BeamDoseVerificationControlPointSequence),
    BeamMeterset(copy.BeamMeterset),
    ReferencedBeamNumber(copy.ReferencedBeamNumber)
{
}


DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::~Item()
{
}


DRTReferencedBeamSequenceInRTFractionSchemeModule::Item &DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AlternateBeamDose = copy.AlternateBeamDose;
        AlternateBeamDoseType = copy.AlternateBeamDoseType;
        BeamDeliveryDurationLimit = copy.BeamDeliveryDurationLimit;
        BeamDose = copy.BeamDose;
        BeamDoseSpecificationPoint = copy.BeamDoseSpecificationPoint;
        BeamDoseType = copy.BeamDoseType;
        BeamDoseVerificationControlPointSequence = copy.BeamDoseVerificationControlPointSequence;
        BeamMeterset = copy.BeamMeterset;
        ReferencedBeamNumber = copy.ReferencedBeamNumber;
    }
    return *this;
}


void DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ReferencedBeamNumber.clear();
        BeamDoseSpecificationPoint.clear();
        BeamDose.clear();
        BeamDoseType.clear();
        AlternateBeamDose.clear();
        AlternateBeamDoseType.clear();
        BeamDoseVerificationControlPointSequence.clear();
        BeamMeterset.clear();
        BeamDeliveryDurationLimit.clear();
    }
}


OFBool DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::isEmpty()
{
    return ReferencedBeamNumber.isEmpty() &&
           BeamDoseSpecificationPoint.isEmpty() &&
           BeamDose.isEmpty() &&
           BeamDoseType.isEmpty() &&
           AlternateBeamDose.isEmpty() &&
           AlternateBeamDoseType.isEmpty() &&
           BeamDoseVerificationControlPointSequence.isEmpty() &&
           BeamMeterset.isEmpty() &&
           BeamDeliveryDurationLimit.isEmpty();
}


OFBool DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ReferencedBeamNumber, "1", "1", "ReferencedBeamSequence");
        getAndCheckElementFromDataset(item, BeamDoseSpecificationPoint, "3", "3", "ReferencedBeamSequence");
        getAndCheckElementFromDataset(item, BeamDose, "1", "3", "ReferencedBeamSequence");
        getAndCheckElementFromDataset(item, BeamDoseType, "1", "1C", "ReferencedBeamSequence");
        getAndCheckElementFromDataset(item, AlternateBeamDose, "1", "3", "ReferencedBeamSequence");
        getAndCheckElementFromDataset(item, AlternateBeamDoseType, "1", "1C", "ReferencedBeamSequence");
        BeamDoseVerificationControlPointSequence.read(item, "1-n", "3", "ReferencedBeamSequence");
        getAndCheckElementFromDataset(item, BeamMeterset, "1", "3", "ReferencedBeamSequence");
        getAndCheckElementFromDataset(item, BeamDeliveryDurationLimit, "1", "3", "ReferencedBeamSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ReferencedBeamNumber), "1", "1", "ReferencedBeamSequence");
        addElementToDataset(result, item, new DcmDecimalString(BeamDoseSpecificationPoint), "3", "3", "ReferencedBeamSequence");
        addElementToDataset(result, item, new DcmDecimalString(BeamDose), "1", "3", "ReferencedBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(BeamDoseType), "1", "1C", "ReferencedBeamSequence");
        addElementToDataset(result, item, new DcmDecimalString(AlternateBeamDose), "1", "3", "ReferencedBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(AlternateBeamDoseType), "1", "1C", "ReferencedBeamSequence");
        if (result.good()) result = BeamDoseVerificationControlPointSequence.write(item, "1-n", "3", "ReferencedBeamSequence");
        addElementToDataset(result, item, new DcmDecimalString(BeamMeterset), "1", "3", "ReferencedBeamSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(BeamDeliveryDurationLimit), "1", "3", "ReferencedBeamSequence");
    }
    return result;
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::getAlternateBeamDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AlternateBeamDose, value, pos);
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::getAlternateBeamDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, AlternateBeamDose).getFloat64(value, pos);
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::getAlternateBeamDoseType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AlternateBeamDoseType, value, pos);
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::getBeamDeliveryDurationLimit(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, BeamDeliveryDurationLimit).getFloat64(value, pos);
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::getBeamDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamDose, value, pos);
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::getBeamDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, BeamDose).getFloat64(value, pos);
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::getBeamDoseSpecificationPoint(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamDoseSpecificationPoint, value, pos);
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::getBeamDoseSpecificationPoint(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, BeamDoseSpecificationPoint).getFloat64(value, pos);
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::getBeamDoseSpecificationPoint(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, BeamDoseSpecificationPoint).getFloat64Vector(value);
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::getBeamDoseType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamDoseType, value, pos);
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::getBeamMeterset(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamMeterset, value, pos);
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::getBeamMeterset(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, BeamMeterset).getFloat64(value, pos);
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::getReferencedBeamNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedBeamNumber, value, pos);
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::getReferencedBeamNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedBeamNumber).getSint32(value, pos);
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::setAlternateBeamDose(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = AlternateBeamDose.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::setAlternateBeamDoseType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = AlternateBeamDoseType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::setBeamDeliveryDurationLimit(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return BeamDeliveryDurationLimit.putFloat64(value, pos);
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::setBeamDose(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BeamDose.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::setBeamDoseSpecificationPoint(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "3") : EC_Normal;
        if (result.good())
            result = BeamDoseSpecificationPoint.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::setBeamDoseType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BeamDoseType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::setBeamMeterset(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BeamMeterset.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::Item::setReferencedBeamNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedBeamNumber.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTReferencedBeamSequenceInRTFractionSchemeModule::DRTReferencedBeamSequenceInRTFractionSchemeModule(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTReferencedBeamSequenceInRTFractionSchemeModule::DRTReferencedBeamSequenceInRTFractionSchemeModule(const DRTReferencedBeamSequenceInRTFractionSchemeModule &copy)
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


DRTReferencedBeamSequenceInRTFractionSchemeModule &DRTReferencedBeamSequenceInRTFractionSchemeModule::operator=(const DRTReferencedBeamSequenceInRTFractionSchemeModule &copy)
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


DRTReferencedBeamSequenceInRTFractionSchemeModule::~DRTReferencedBeamSequenceInRTFractionSchemeModule()
{
    clear();
}


void DRTReferencedBeamSequenceInRTFractionSchemeModule::clear()
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


OFBool DRTReferencedBeamSequenceInRTFractionSchemeModule::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTReferencedBeamSequenceInRTFractionSchemeModule::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTReferencedBeamSequenceInRTFractionSchemeModule::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTReferencedBeamSequenceInRTFractionSchemeModule::Item &DRTReferencedBeamSequenceInRTFractionSchemeModule::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTReferencedBeamSequenceInRTFractionSchemeModule::Item &DRTReferencedBeamSequenceInRTFractionSchemeModule::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTReferencedBeamSequenceInRTFractionSchemeModule::Item &DRTReferencedBeamSequenceInRTFractionSchemeModule::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTReferencedBeamSequenceInRTFractionSchemeModule::Item &DRTReferencedBeamSequenceInRTFractionSchemeModule::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTReferencedBeamSequenceInRTFractionSchemeModule::Item &DRTReferencedBeamSequenceInRTFractionSchemeModule::operator[](const size_t num)
{
    return getItem(num);
}


const DRTReferencedBeamSequenceInRTFractionSchemeModule::Item &DRTReferencedBeamSequenceInRTFractionSchemeModule::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::addItem(Item *&item)
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


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::removeItem(const size_t pos)
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


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ReferencedBeamSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ReferencedBeamSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTReferencedBeamSequenceInRTFractionSchemeModule::write(DcmItem &dataset,
                                                                     const OFString &card,
                                                                     const OFString &type,
                                                                     const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ReferencedBeamSequence);
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
