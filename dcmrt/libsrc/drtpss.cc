/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTPatientSetupSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtpss.h"


// --- item class ---

DRTPatientSetupSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    FixationDeviceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    MotionSynchronizationSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    PatientAdditionalPosition(DCM_PatientAdditionalPosition),
    PatientPosition(DCM_PatientPosition),
    PatientSetupLabel(DCM_PatientSetupLabel),
    PatientSetupNumber(DCM_PatientSetupNumber),
    ReferencedSetupImageSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    SetupDeviceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    SetupTechnique(DCM_SetupTechnique),
    SetupTechniqueDescription(DCM_SetupTechniqueDescription),
    ShieldingDeviceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    TableTopLateralSetupDisplacement(DCM_TableTopLateralSetupDisplacement),
    TableTopLongitudinalSetupDisplacement(DCM_TableTopLongitudinalSetupDisplacement),
    TableTopVerticalSetupDisplacement(DCM_TableTopVerticalSetupDisplacement)
{
}


DRTPatientSetupSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    FixationDeviceSequence(copy.FixationDeviceSequence),
    MotionSynchronizationSequence(copy.MotionSynchronizationSequence),
    PatientAdditionalPosition(copy.PatientAdditionalPosition),
    PatientPosition(copy.PatientPosition),
    PatientSetupLabel(copy.PatientSetupLabel),
    PatientSetupNumber(copy.PatientSetupNumber),
    ReferencedSetupImageSequence(copy.ReferencedSetupImageSequence),
    SetupDeviceSequence(copy.SetupDeviceSequence),
    SetupTechnique(copy.SetupTechnique),
    SetupTechniqueDescription(copy.SetupTechniqueDescription),
    ShieldingDeviceSequence(copy.ShieldingDeviceSequence),
    TableTopLateralSetupDisplacement(copy.TableTopLateralSetupDisplacement),
    TableTopLongitudinalSetupDisplacement(copy.TableTopLongitudinalSetupDisplacement),
    TableTopVerticalSetupDisplacement(copy.TableTopVerticalSetupDisplacement)
{
}


DRTPatientSetupSequence::Item::~Item()
{
}


DRTPatientSetupSequence::Item &DRTPatientSetupSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        FixationDeviceSequence = copy.FixationDeviceSequence;
        MotionSynchronizationSequence = copy.MotionSynchronizationSequence;
        PatientAdditionalPosition = copy.PatientAdditionalPosition;
        PatientPosition = copy.PatientPosition;
        PatientSetupLabel = copy.PatientSetupLabel;
        PatientSetupNumber = copy.PatientSetupNumber;
        ReferencedSetupImageSequence = copy.ReferencedSetupImageSequence;
        SetupDeviceSequence = copy.SetupDeviceSequence;
        SetupTechnique = copy.SetupTechnique;
        SetupTechniqueDescription = copy.SetupTechniqueDescription;
        ShieldingDeviceSequence = copy.ShieldingDeviceSequence;
        TableTopLateralSetupDisplacement = copy.TableTopLateralSetupDisplacement;
        TableTopLongitudinalSetupDisplacement = copy.TableTopLongitudinalSetupDisplacement;
        TableTopVerticalSetupDisplacement = copy.TableTopVerticalSetupDisplacement;
    }
    return *this;
}


void DRTPatientSetupSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        PatientSetupNumber.clear();
        PatientSetupLabel.clear();
        PatientPosition.clear();
        PatientAdditionalPosition.clear();
        ReferencedSetupImageSequence.clear();
        FixationDeviceSequence.clear();
        ShieldingDeviceSequence.clear();
        SetupTechnique.clear();
        SetupTechniqueDescription.clear();
        SetupDeviceSequence.clear();
        TableTopVerticalSetupDisplacement.clear();
        TableTopLongitudinalSetupDisplacement.clear();
        TableTopLateralSetupDisplacement.clear();
        MotionSynchronizationSequence.clear();
    }
}


OFBool DRTPatientSetupSequence::Item::isEmpty()
{
    return PatientSetupNumber.isEmpty() &&
           PatientSetupLabel.isEmpty() &&
           PatientPosition.isEmpty() &&
           PatientAdditionalPosition.isEmpty() &&
           ReferencedSetupImageSequence.isEmpty() &&
           FixationDeviceSequence.isEmpty() &&
           ShieldingDeviceSequence.isEmpty() &&
           SetupTechnique.isEmpty() &&
           SetupTechniqueDescription.isEmpty() &&
           SetupDeviceSequence.isEmpty() &&
           TableTopVerticalSetupDisplacement.isEmpty() &&
           TableTopLongitudinalSetupDisplacement.isEmpty() &&
           TableTopLateralSetupDisplacement.isEmpty() &&
           MotionSynchronizationSequence.isEmpty();
}


OFBool DRTPatientSetupSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTPatientSetupSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, PatientSetupNumber, "1", "1", "PatientSetupSequence");
        getAndCheckElementFromDataset(item, PatientSetupLabel, "1", "3", "PatientSetupSequence");
        getAndCheckElementFromDataset(item, PatientPosition, "1", "1C", "PatientSetupSequence");
        getAndCheckElementFromDataset(item, PatientAdditionalPosition, "1", "1C", "PatientSetupSequence");
        ReferencedSetupImageSequence.read(item, "1-n", "3", "PatientSetupSequence");
        FixationDeviceSequence.read(item, "1-n", "3", "PatientSetupSequence");
        ShieldingDeviceSequence.read(item, "1-n", "3", "PatientSetupSequence");
        getAndCheckElementFromDataset(item, SetupTechnique, "1", "3", "PatientSetupSequence");
        getAndCheckElementFromDataset(item, SetupTechniqueDescription, "1", "3", "PatientSetupSequence");
        SetupDeviceSequence.read(item, "1-n", "3", "PatientSetupSequence");
        getAndCheckElementFromDataset(item, TableTopVerticalSetupDisplacement, "1", "3", "PatientSetupSequence");
        getAndCheckElementFromDataset(item, TableTopLongitudinalSetupDisplacement, "1", "3", "PatientSetupSequence");
        getAndCheckElementFromDataset(item, TableTopLateralSetupDisplacement, "1", "3", "PatientSetupSequence");
        MotionSynchronizationSequence.read(item, "1-n", "3", "PatientSetupSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTPatientSetupSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(PatientSetupNumber), "1", "1", "PatientSetupSequence");
        addElementToDataset(result, item, new DcmLongString(PatientSetupLabel), "1", "3", "PatientSetupSequence");
        addElementToDataset(result, item, new DcmCodeString(PatientPosition), "1", "1C", "PatientSetupSequence");
        addElementToDataset(result, item, new DcmLongString(PatientAdditionalPosition), "1", "1C", "PatientSetupSequence");
        if (result.good()) result = ReferencedSetupImageSequence.write(item, "1-n", "3", "PatientSetupSequence");
        if (result.good()) result = FixationDeviceSequence.write(item, "1-n", "3", "PatientSetupSequence");
        if (result.good()) result = ShieldingDeviceSequence.write(item, "1-n", "3", "PatientSetupSequence");
        addElementToDataset(result, item, new DcmCodeString(SetupTechnique), "1", "3", "PatientSetupSequence");
        addElementToDataset(result, item, new DcmShortText(SetupTechniqueDescription), "1", "3", "PatientSetupSequence");
        if (result.good()) result = SetupDeviceSequence.write(item, "1-n", "3", "PatientSetupSequence");
        addElementToDataset(result, item, new DcmDecimalString(TableTopVerticalSetupDisplacement), "1", "3", "PatientSetupSequence");
        addElementToDataset(result, item, new DcmDecimalString(TableTopLongitudinalSetupDisplacement), "1", "3", "PatientSetupSequence");
        addElementToDataset(result, item, new DcmDecimalString(TableTopLateralSetupDisplacement), "1", "3", "PatientSetupSequence");
        if (result.good()) result = MotionSynchronizationSequence.write(item, "1-n", "3", "PatientSetupSequence");
    }
    return result;
}


OFCondition DRTPatientSetupSequence::Item::getPatientAdditionalPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientAdditionalPosition, value, pos);
}


OFCondition DRTPatientSetupSequence::Item::getPatientPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientPosition, value, pos);
}


OFCondition DRTPatientSetupSequence::Item::getPatientSetupLabel(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientSetupLabel, value, pos);
}


OFCondition DRTPatientSetupSequence::Item::getPatientSetupNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientSetupNumber, value, pos);
}


OFCondition DRTPatientSetupSequence::Item::getPatientSetupNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, PatientSetupNumber).getSint32(value, pos);
}


OFCondition DRTPatientSetupSequence::Item::getSetupTechnique(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SetupTechnique, value, pos);
}


OFCondition DRTPatientSetupSequence::Item::getSetupTechniqueDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SetupTechniqueDescription, value, pos);
}


OFCondition DRTPatientSetupSequence::Item::getTableTopLateralSetupDisplacement(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopLateralSetupDisplacement, value, pos);
}


OFCondition DRTPatientSetupSequence::Item::getTableTopLateralSetupDisplacement(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TableTopLateralSetupDisplacement).getFloat64(value, pos);
}


OFCondition DRTPatientSetupSequence::Item::getTableTopLongitudinalSetupDisplacement(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopLongitudinalSetupDisplacement, value, pos);
}


OFCondition DRTPatientSetupSequence::Item::getTableTopLongitudinalSetupDisplacement(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TableTopLongitudinalSetupDisplacement).getFloat64(value, pos);
}


OFCondition DRTPatientSetupSequence::Item::getTableTopVerticalSetupDisplacement(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopVerticalSetupDisplacement, value, pos);
}


OFCondition DRTPatientSetupSequence::Item::getTableTopVerticalSetupDisplacement(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TableTopVerticalSetupDisplacement).getFloat64(value, pos);
}


OFCondition DRTPatientSetupSequence::Item::setPatientAdditionalPosition(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PatientAdditionalPosition.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPatientSetupSequence::Item::setPatientPosition(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PatientPosition.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPatientSetupSequence::Item::setPatientSetupLabel(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PatientSetupLabel.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPatientSetupSequence::Item::setPatientSetupNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PatientSetupNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPatientSetupSequence::Item::setSetupTechnique(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SetupTechnique.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPatientSetupSequence::Item::setSetupTechniqueDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = SetupTechniqueDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPatientSetupSequence::Item::setTableTopLateralSetupDisplacement(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TableTopLateralSetupDisplacement.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPatientSetupSequence::Item::setTableTopLongitudinalSetupDisplacement(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TableTopLongitudinalSetupDisplacement.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPatientSetupSequence::Item::setTableTopVerticalSetupDisplacement(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TableTopVerticalSetupDisplacement.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTPatientSetupSequence::DRTPatientSetupSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTPatientSetupSequence::DRTPatientSetupSequence(const DRTPatientSetupSequence &copy)
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


DRTPatientSetupSequence &DRTPatientSetupSequence::operator=(const DRTPatientSetupSequence &copy)
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


DRTPatientSetupSequence::~DRTPatientSetupSequence()
{
    clear();
}


void DRTPatientSetupSequence::clear()
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


OFBool DRTPatientSetupSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTPatientSetupSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTPatientSetupSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTPatientSetupSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTPatientSetupSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTPatientSetupSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTPatientSetupSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTPatientSetupSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTPatientSetupSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTPatientSetupSequence::Item &DRTPatientSetupSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTPatientSetupSequence::Item &DRTPatientSetupSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTPatientSetupSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTPatientSetupSequence::Item &DRTPatientSetupSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTPatientSetupSequence::Item &DRTPatientSetupSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTPatientSetupSequence::Item &DRTPatientSetupSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTPatientSetupSequence::Item &DRTPatientSetupSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTPatientSetupSequence::addItem(Item *&item)
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


OFCondition DRTPatientSetupSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTPatientSetupSequence::removeItem(const unsigned long pos)
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


OFCondition DRTPatientSetupSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_PatientSetupSequence, sequence);
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
            DcmSequenceOfItems element(DCM_PatientSetupSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTPatientSetupSequence::write(DcmItem &dataset,
                                           const OFString &card,
                                           const OFString &type,
                                           const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_PatientSetupSequence);
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
