/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTFractionGroupSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtfgs.h"


// --- item class ---

DRTFractionGroupSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    BeamDoseMeaning(DCM_BeamDoseMeaning),
    FractionGroupDescription(DCM_FractionGroupDescription),
    FractionGroupNumber(DCM_FractionGroupNumber),
    FractionPattern(DCM_FractionPattern),
    NumberOfBeams(DCM_NumberOfBeams),
    NumberOfBrachyApplicationSetups(DCM_NumberOfBrachyApplicationSetups),
    NumberOfFractionPatternDigitsPerDay(DCM_NumberOfFractionPatternDigitsPerDay),
    NumberOfFractionsPlanned(DCM_NumberOfFractionsPlanned),
    ReferencedBeamSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedBrachyApplicationSetupSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedDoseReferenceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedDoseSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RepeatFractionCycleLength(DCM_RepeatFractionCycleLength)
{
}


DRTFractionGroupSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    BeamDoseMeaning(copy.BeamDoseMeaning),
    FractionGroupDescription(copy.FractionGroupDescription),
    FractionGroupNumber(copy.FractionGroupNumber),
    FractionPattern(copy.FractionPattern),
    NumberOfBeams(copy.NumberOfBeams),
    NumberOfBrachyApplicationSetups(copy.NumberOfBrachyApplicationSetups),
    NumberOfFractionPatternDigitsPerDay(copy.NumberOfFractionPatternDigitsPerDay),
    NumberOfFractionsPlanned(copy.NumberOfFractionsPlanned),
    ReferencedBeamSequence(copy.ReferencedBeamSequence),
    ReferencedBrachyApplicationSetupSequence(copy.ReferencedBrachyApplicationSetupSequence),
    ReferencedDoseReferenceSequence(copy.ReferencedDoseReferenceSequence),
    ReferencedDoseSequence(copy.ReferencedDoseSequence),
    RepeatFractionCycleLength(copy.RepeatFractionCycleLength)
{
}


DRTFractionGroupSequence::Item::~Item()
{
}


DRTFractionGroupSequence::Item &DRTFractionGroupSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        BeamDoseMeaning = copy.BeamDoseMeaning;
        FractionGroupDescription = copy.FractionGroupDescription;
        FractionGroupNumber = copy.FractionGroupNumber;
        FractionPattern = copy.FractionPattern;
        NumberOfBeams = copy.NumberOfBeams;
        NumberOfBrachyApplicationSetups = copy.NumberOfBrachyApplicationSetups;
        NumberOfFractionPatternDigitsPerDay = copy.NumberOfFractionPatternDigitsPerDay;
        NumberOfFractionsPlanned = copy.NumberOfFractionsPlanned;
        ReferencedBeamSequence = copy.ReferencedBeamSequence;
        ReferencedBrachyApplicationSetupSequence = copy.ReferencedBrachyApplicationSetupSequence;
        ReferencedDoseReferenceSequence = copy.ReferencedDoseReferenceSequence;
        ReferencedDoseSequence = copy.ReferencedDoseSequence;
        RepeatFractionCycleLength = copy.RepeatFractionCycleLength;
    }
    return *this;
}


void DRTFractionGroupSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        FractionGroupNumber.clear();
        FractionGroupDescription.clear();
        ReferencedDoseSequence.clear();
        ReferencedDoseReferenceSequence.clear();
        NumberOfFractionsPlanned.clear();
        NumberOfFractionPatternDigitsPerDay.clear();
        RepeatFractionCycleLength.clear();
        FractionPattern.clear();
        BeamDoseMeaning.clear();
        NumberOfBeams.clear();
        ReferencedBeamSequence.clear();
        NumberOfBrachyApplicationSetups.clear();
        ReferencedBrachyApplicationSetupSequence.clear();
    }
}


OFBool DRTFractionGroupSequence::Item::isEmpty()
{
    return FractionGroupNumber.isEmpty() &&
           FractionGroupDescription.isEmpty() &&
           ReferencedDoseSequence.isEmpty() &&
           ReferencedDoseReferenceSequence.isEmpty() &&
           NumberOfFractionsPlanned.isEmpty() &&
           NumberOfFractionPatternDigitsPerDay.isEmpty() &&
           RepeatFractionCycleLength.isEmpty() &&
           FractionPattern.isEmpty() &&
           BeamDoseMeaning.isEmpty() &&
           NumberOfBeams.isEmpty() &&
           ReferencedBeamSequence.isEmpty() &&
           NumberOfBrachyApplicationSetups.isEmpty() &&
           ReferencedBrachyApplicationSetupSequence.isEmpty();
}


OFBool DRTFractionGroupSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTFractionGroupSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, FractionGroupNumber, "1", "1", "FractionGroupSequence");
        getAndCheckElementFromDataset(item, FractionGroupDescription, "1", "3", "FractionGroupSequence");
        ReferencedDoseSequence.read(item, "1-n", "3", "FractionGroupSequence");
        ReferencedDoseReferenceSequence.read(item, "1-n", "3", "FractionGroupSequence");
        getAndCheckElementFromDataset(item, NumberOfFractionsPlanned, "1", "2", "FractionGroupSequence");
        getAndCheckElementFromDataset(item, NumberOfFractionPatternDigitsPerDay, "1", "3", "FractionGroupSequence");
        getAndCheckElementFromDataset(item, RepeatFractionCycleLength, "1", "3", "FractionGroupSequence");
        getAndCheckElementFromDataset(item, FractionPattern, "1", "3", "FractionGroupSequence");
        getAndCheckElementFromDataset(item, BeamDoseMeaning, "1", "3", "FractionGroupSequence");
        getAndCheckElementFromDataset(item, NumberOfBeams, "1", "1", "FractionGroupSequence");
        ReferencedBeamSequence.read(item, "1-n", "1C", "FractionGroupSequence");
        getAndCheckElementFromDataset(item, NumberOfBrachyApplicationSetups, "1", "1", "FractionGroupSequence");
        ReferencedBrachyApplicationSetupSequence.read(item, "1-n", "1C", "FractionGroupSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTFractionGroupSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(FractionGroupNumber), "1", "1", "FractionGroupSequence");
        addElementToDataset(result, item, new DcmLongString(FractionGroupDescription), "1", "3", "FractionGroupSequence");
        if (result.good()) result = ReferencedDoseSequence.write(item, "1-n", "3", "FractionGroupSequence");
        if (result.good()) result = ReferencedDoseReferenceSequence.write(item, "1-n", "3", "FractionGroupSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfFractionsPlanned), "1", "2", "FractionGroupSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfFractionPatternDigitsPerDay), "1", "3", "FractionGroupSequence");
        addElementToDataset(result, item, new DcmIntegerString(RepeatFractionCycleLength), "1", "3", "FractionGroupSequence");
        addElementToDataset(result, item, new DcmLongText(FractionPattern), "1", "3", "FractionGroupSequence");
        addElementToDataset(result, item, new DcmCodeString(BeamDoseMeaning), "1", "3", "FractionGroupSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfBeams), "1", "1", "FractionGroupSequence");
        if (result.good()) result = ReferencedBeamSequence.write(item, "1-n", "1C", "FractionGroupSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfBrachyApplicationSetups), "1", "1", "FractionGroupSequence");
        if (result.good()) result = ReferencedBrachyApplicationSetupSequence.write(item, "1-n", "1C", "FractionGroupSequence");
    }
    return result;
}


OFCondition DRTFractionGroupSequence::Item::getBeamDoseMeaning(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamDoseMeaning, value, pos);
}


OFCondition DRTFractionGroupSequence::Item::getFractionGroupDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FractionGroupDescription, value, pos);
}


OFCondition DRTFractionGroupSequence::Item::getFractionGroupNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FractionGroupNumber, value, pos);
}


OFCondition DRTFractionGroupSequence::Item::getFractionGroupNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, FractionGroupNumber).getSint32(value, pos);
}


OFCondition DRTFractionGroupSequence::Item::getFractionPattern(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FractionPattern, value, pos);
}


OFCondition DRTFractionGroupSequence::Item::getNumberOfBeams(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfBeams, value, pos);
}


OFCondition DRTFractionGroupSequence::Item::getNumberOfBeams(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfBeams).getSint32(value, pos);
}


OFCondition DRTFractionGroupSequence::Item::getNumberOfBrachyApplicationSetups(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfBrachyApplicationSetups, value, pos);
}


OFCondition DRTFractionGroupSequence::Item::getNumberOfBrachyApplicationSetups(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfBrachyApplicationSetups).getSint32(value, pos);
}


OFCondition DRTFractionGroupSequence::Item::getNumberOfFractionPatternDigitsPerDay(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfFractionPatternDigitsPerDay, value, pos);
}


OFCondition DRTFractionGroupSequence::Item::getNumberOfFractionPatternDigitsPerDay(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfFractionPatternDigitsPerDay).getSint32(value, pos);
}


OFCondition DRTFractionGroupSequence::Item::getNumberOfFractionsPlanned(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfFractionsPlanned, value, pos);
}


OFCondition DRTFractionGroupSequence::Item::getNumberOfFractionsPlanned(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfFractionsPlanned).getSint32(value, pos);
}


OFCondition DRTFractionGroupSequence::Item::getRepeatFractionCycleLength(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RepeatFractionCycleLength, value, pos);
}


OFCondition DRTFractionGroupSequence::Item::getRepeatFractionCycleLength(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, RepeatFractionCycleLength).getSint32(value, pos);
}


OFCondition DRTFractionGroupSequence::Item::setBeamDoseMeaning(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BeamDoseMeaning.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFractionGroupSequence::Item::setFractionGroupDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = FractionGroupDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFractionGroupSequence::Item::setFractionGroupNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = FractionGroupNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFractionGroupSequence::Item::setFractionPattern(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = FractionPattern.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFractionGroupSequence::Item::setNumberOfBeams(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfBeams.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFractionGroupSequence::Item::setNumberOfBrachyApplicationSetups(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfBrachyApplicationSetups.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFractionGroupSequence::Item::setNumberOfFractionPatternDigitsPerDay(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfFractionPatternDigitsPerDay.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFractionGroupSequence::Item::setNumberOfFractionsPlanned(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfFractionsPlanned.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFractionGroupSequence::Item::setRepeatFractionCycleLength(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RepeatFractionCycleLength.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTFractionGroupSequence::DRTFractionGroupSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTFractionGroupSequence::DRTFractionGroupSequence(const DRTFractionGroupSequence &copy)
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


DRTFractionGroupSequence &DRTFractionGroupSequence::operator=(const DRTFractionGroupSequence &copy)
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


DRTFractionGroupSequence::~DRTFractionGroupSequence()
{
    clear();
}


void DRTFractionGroupSequence::clear()
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


OFBool DRTFractionGroupSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTFractionGroupSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTFractionGroupSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTFractionGroupSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTFractionGroupSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTFractionGroupSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTFractionGroupSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTFractionGroupSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTFractionGroupSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTFractionGroupSequence::Item &DRTFractionGroupSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTFractionGroupSequence::Item &DRTFractionGroupSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTFractionGroupSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTFractionGroupSequence::Item &DRTFractionGroupSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTFractionGroupSequence::Item &DRTFractionGroupSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTFractionGroupSequence::Item &DRTFractionGroupSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTFractionGroupSequence::Item &DRTFractionGroupSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTFractionGroupSequence::addItem(Item *&item)
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


OFCondition DRTFractionGroupSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTFractionGroupSequence::removeItem(const unsigned long pos)
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


OFCondition DRTFractionGroupSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_FractionGroupSequence, sequence);
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
            DcmSequenceOfItems element(DCM_FractionGroupSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTFractionGroupSequence::write(DcmItem &dataset,
                                            const OFString &card,
                                            const OFString &type,
                                            const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_FractionGroupSequence);
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
