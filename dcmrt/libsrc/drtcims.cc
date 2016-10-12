/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTContentItemModifierSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtcims.h"


// --- item class ---

DRTContentItemModifierSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ConceptCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ConceptNameCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    Date(DCM_Date),
    DateTime(DCM_DateTime),
    FloatingPointValue(DCM_FloatingPointValue),
    MeasurementUnitsCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    NumericValue(DCM_NumericValue),
    PersonName(DCM_PersonName),
    RationalDenominatorValue(DCM_RationalDenominatorValue),
    RationalNumeratorValue(DCM_RationalNumeratorValue),
    ReferencedSOPSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    TextValue(DCM_TextValue),
    Time(DCM_Time),
    UID(DCM_UID),
    ValueType(DCM_ValueType)
{
}


DRTContentItemModifierSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ConceptCodeSequence(copy.ConceptCodeSequence),
    ConceptNameCodeSequence(copy.ConceptNameCodeSequence),
    Date(copy.Date),
    DateTime(copy.DateTime),
    FloatingPointValue(copy.FloatingPointValue),
    MeasurementUnitsCodeSequence(copy.MeasurementUnitsCodeSequence),
    NumericValue(copy.NumericValue),
    PersonName(copy.PersonName),
    RationalDenominatorValue(copy.RationalDenominatorValue),
    RationalNumeratorValue(copy.RationalNumeratorValue),
    ReferencedSOPSequence(copy.ReferencedSOPSequence),
    TextValue(copy.TextValue),
    Time(copy.Time),
    UID(copy.UID),
    ValueType(copy.ValueType)
{
}


DRTContentItemModifierSequence::Item::~Item()
{
}


DRTContentItemModifierSequence::Item &DRTContentItemModifierSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ConceptCodeSequence = copy.ConceptCodeSequence;
        ConceptNameCodeSequence = copy.ConceptNameCodeSequence;
        Date = copy.Date;
        DateTime = copy.DateTime;
        FloatingPointValue = copy.FloatingPointValue;
        MeasurementUnitsCodeSequence = copy.MeasurementUnitsCodeSequence;
        NumericValue = copy.NumericValue;
        PersonName = copy.PersonName;
        RationalDenominatorValue = copy.RationalDenominatorValue;
        RationalNumeratorValue = copy.RationalNumeratorValue;
        ReferencedSOPSequence = copy.ReferencedSOPSequence;
        TextValue = copy.TextValue;
        Time = copy.Time;
        UID = copy.UID;
        ValueType = copy.ValueType;
    }
    return *this;
}


void DRTContentItemModifierSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ValueType.clear();
        ConceptNameCodeSequence.clear();
        DateTime.clear();
        Date.clear();
        Time.clear();
        PersonName.clear();
        UID.clear();
        TextValue.clear();
        ConceptCodeSequence.clear();
        NumericValue.clear();
        FloatingPointValue.clear();
        RationalNumeratorValue.clear();
        RationalDenominatorValue.clear();
        MeasurementUnitsCodeSequence.clear();
        ReferencedSOPSequence.clear();
    }
}


OFBool DRTContentItemModifierSequence::Item::isEmpty()
{
    return ValueType.isEmpty() &&
           ConceptNameCodeSequence.isEmpty() &&
           DateTime.isEmpty() &&
           Date.isEmpty() &&
           Time.isEmpty() &&
           PersonName.isEmpty() &&
           UID.isEmpty() &&
           TextValue.isEmpty() &&
           ConceptCodeSequence.isEmpty() &&
           NumericValue.isEmpty() &&
           FloatingPointValue.isEmpty() &&
           RationalNumeratorValue.isEmpty() &&
           RationalDenominatorValue.isEmpty() &&
           MeasurementUnitsCodeSequence.isEmpty() &&
           ReferencedSOPSequence.isEmpty();
}


OFBool DRTContentItemModifierSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTContentItemModifierSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ValueType, "1", "1", "ContentItemModifierSequence");
        ConceptNameCodeSequence.read(item, "1-n", "1", "ContentItemModifierSequence");
        getAndCheckElementFromDataset(item, DateTime, "1", "1C", "ContentItemModifierSequence");
        getAndCheckElementFromDataset(item, Date, "1", "1C", "ContentItemModifierSequence");
        getAndCheckElementFromDataset(item, Time, "1", "1C", "ContentItemModifierSequence");
        getAndCheckElementFromDataset(item, PersonName, "1", "1C", "ContentItemModifierSequence");
        getAndCheckElementFromDataset(item, UID, "1", "1C", "ContentItemModifierSequence");
        getAndCheckElementFromDataset(item, TextValue, "1", "1C", "ContentItemModifierSequence");
        ConceptCodeSequence.read(item, "1-n", "1C", "ContentItemModifierSequence");
        getAndCheckElementFromDataset(item, NumericValue, "1-n", "1C", "ContentItemModifierSequence");
        getAndCheckElementFromDataset(item, FloatingPointValue, "1-n", "1C", "ContentItemModifierSequence");
        getAndCheckElementFromDataset(item, RationalNumeratorValue, "1-n", "1C", "ContentItemModifierSequence");
        getAndCheckElementFromDataset(item, RationalDenominatorValue, "1-n", "1C", "ContentItemModifierSequence");
        MeasurementUnitsCodeSequence.read(item, "1-n", "1C", "ContentItemModifierSequence");
        ReferencedSOPSequence.read(item, "1-n", "1C", "ContentItemModifierSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTContentItemModifierSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmCodeString(ValueType), "1", "1", "ContentItemModifierSequence");
        if (result.good()) result = ConceptNameCodeSequence.write(item, "1-n", "1", "ContentItemModifierSequence");
        addElementToDataset(result, item, new DcmDateTime(DateTime), "1", "1C", "ContentItemModifierSequence");
        addElementToDataset(result, item, new DcmDate(Date), "1", "1C", "ContentItemModifierSequence");
        addElementToDataset(result, item, new DcmTime(Time), "1", "1C", "ContentItemModifierSequence");
        addElementToDataset(result, item, new DcmPersonName(PersonName), "1", "1C", "ContentItemModifierSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(UID), "1", "1C", "ContentItemModifierSequence");
        addElementToDataset(result, item, new DcmUnlimitedText(TextValue), "1", "1C", "ContentItemModifierSequence");
        if (result.good()) result = ConceptCodeSequence.write(item, "1-n", "1C", "ContentItemModifierSequence");
        addElementToDataset(result, item, new DcmDecimalString(NumericValue), "1-n", "1C", "ContentItemModifierSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(FloatingPointValue), "1-n", "1C", "ContentItemModifierSequence");
        addElementToDataset(result, item, new DcmSignedLong(RationalNumeratorValue), "1-n", "1C", "ContentItemModifierSequence");
        addElementToDataset(result, item, new DcmUnsignedLong(RationalDenominatorValue), "1-n", "1C", "ContentItemModifierSequence");
        if (result.good()) result = MeasurementUnitsCodeSequence.write(item, "1-n", "1C", "ContentItemModifierSequence");
        if (result.good()) result = ReferencedSOPSequence.write(item, "1-n", "1C", "ContentItemModifierSequence");
    }
    return result;
}


OFCondition DRTContentItemModifierSequence::Item::getDate(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(Date, value, pos);
}


OFCondition DRTContentItemModifierSequence::Item::getDateTime(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DateTime, value, pos);
}


OFCondition DRTContentItemModifierSequence::Item::getFloatingPointValue(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, FloatingPointValue).getFloat64(value, pos);
}


OFCondition DRTContentItemModifierSequence::Item::getNumericValue(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumericValue, value, pos);
}


OFCondition DRTContentItemModifierSequence::Item::getNumericValue(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, NumericValue).getFloat64(value, pos);
}


OFCondition DRTContentItemModifierSequence::Item::getNumericValue(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, NumericValue).getFloat64Vector(value);
}


OFCondition DRTContentItemModifierSequence::Item::getPersonName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PersonName, value, pos);
}


OFCondition DRTContentItemModifierSequence::Item::getRationalDenominatorValue(Uint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedLong &, RationalDenominatorValue).getUint32(value, pos);
}


OFCondition DRTContentItemModifierSequence::Item::getRationalNumeratorValue(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmSignedLong &, RationalNumeratorValue).getSint32(value, pos);
}


OFCondition DRTContentItemModifierSequence::Item::getTextValue(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TextValue, value, pos);
}


OFCondition DRTContentItemModifierSequence::Item::getTime(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(Time, value, pos);
}


OFCondition DRTContentItemModifierSequence::Item::getUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(UID, value, pos);
}


OFCondition DRTContentItemModifierSequence::Item::getValueType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ValueType, value, pos);
}


OFCondition DRTContentItemModifierSequence::Item::setDate(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = Date.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContentItemModifierSequence::Item::setDateTime(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DateTime.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContentItemModifierSequence::Item::setFloatingPointValue(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return FloatingPointValue.putFloat64(value, pos);
}


OFCondition DRTContentItemModifierSequence::Item::setNumericValue(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1-n") : EC_Normal;
        if (result.good())
            result = NumericValue.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContentItemModifierSequence::Item::setPersonName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PersonName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContentItemModifierSequence::Item::setRationalDenominatorValue(const Uint32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RationalDenominatorValue.putUint32(value, pos);
}


OFCondition DRTContentItemModifierSequence::Item::setRationalNumeratorValue(const Sint32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RationalNumeratorValue.putSint32(value, pos);
}


OFCondition DRTContentItemModifierSequence::Item::setTextValue(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUnlimitedText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = TextValue.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContentItemModifierSequence::Item::setTime(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = Time.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContentItemModifierSequence::Item::setUID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = UID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContentItemModifierSequence::Item::setValueType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ValueType.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTContentItemModifierSequence::DRTContentItemModifierSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTContentItemModifierSequence::DRTContentItemModifierSequence(const DRTContentItemModifierSequence &copy)
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


DRTContentItemModifierSequence &DRTContentItemModifierSequence::operator=(const DRTContentItemModifierSequence &copy)
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


DRTContentItemModifierSequence::~DRTContentItemModifierSequence()
{
    clear();
}


void DRTContentItemModifierSequence::clear()
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


OFBool DRTContentItemModifierSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTContentItemModifierSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTContentItemModifierSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTContentItemModifierSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTContentItemModifierSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTContentItemModifierSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTContentItemModifierSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTContentItemModifierSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTContentItemModifierSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTContentItemModifierSequence::Item &DRTContentItemModifierSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTContentItemModifierSequence::Item &DRTContentItemModifierSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTContentItemModifierSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTContentItemModifierSequence::Item &DRTContentItemModifierSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTContentItemModifierSequence::Item &DRTContentItemModifierSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTContentItemModifierSequence::Item &DRTContentItemModifierSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTContentItemModifierSequence::Item &DRTContentItemModifierSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTContentItemModifierSequence::addItem(Item *&item)
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


OFCondition DRTContentItemModifierSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTContentItemModifierSequence::removeItem(const unsigned long pos)
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


OFCondition DRTContentItemModifierSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ContentItemModifierSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ContentItemModifierSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTContentItemModifierSequence::write(DcmItem &dataset,
                                                  const OFString &card,
                                                  const OFString &type,
                                                  const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ContentItemModifierSequence);
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
