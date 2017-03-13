/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTOverrideSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtos.h"


// --- item class ---

DRTOverrideSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    OperatorIdentificationSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    OperatorsName(DCM_OperatorsName),
    OverrideParameterPointer(DCM_OverrideParameterPointer),
    OverrideReason(DCM_OverrideReason),
    ParameterItemIndex(DCM_ParameterItemIndex),
    ParameterSequencePointer(DCM_ParameterSequencePointer),
    ParameterValueNumber(DCM_ParameterValueNumber)
{
}


DRTOverrideSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    OperatorIdentificationSequence(copy.OperatorIdentificationSequence),
    OperatorsName(copy.OperatorsName),
    OverrideParameterPointer(copy.OverrideParameterPointer),
    OverrideReason(copy.OverrideReason),
    ParameterItemIndex(copy.ParameterItemIndex),
    ParameterSequencePointer(copy.ParameterSequencePointer),
    ParameterValueNumber(copy.ParameterValueNumber)
{
}


DRTOverrideSequence::Item::~Item()
{
}


DRTOverrideSequence::Item &DRTOverrideSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        OperatorIdentificationSequence = copy.OperatorIdentificationSequence;
        OperatorsName = copy.OperatorsName;
        OverrideParameterPointer = copy.OverrideParameterPointer;
        OverrideReason = copy.OverrideReason;
        ParameterItemIndex = copy.ParameterItemIndex;
        ParameterSequencePointer = copy.ParameterSequencePointer;
        ParameterValueNumber = copy.ParameterValueNumber;
    }
    return *this;
}


void DRTOverrideSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ParameterSequencePointer.clear();
        OverrideParameterPointer.clear();
        ParameterItemIndex.clear();
        ParameterValueNumber.clear();
        OperatorsName.clear();
        OperatorIdentificationSequence.clear();
        OverrideReason.clear();
    }
}


OFBool DRTOverrideSequence::Item::isEmpty()
{
    return ParameterSequencePointer.isEmpty() &&
           OverrideParameterPointer.isEmpty() &&
           ParameterItemIndex.isEmpty() &&
           ParameterValueNumber.isEmpty() &&
           OperatorsName.isEmpty() &&
           OperatorIdentificationSequence.isEmpty() &&
           OverrideReason.isEmpty();
}


OFBool DRTOverrideSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTOverrideSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ParameterSequencePointer, "1", "1", "OverrideSequence");
        getAndCheckElementFromDataset(item, OverrideParameterPointer, "1", "1", "OverrideSequence");
        getAndCheckElementFromDataset(item, ParameterItemIndex, "1", "1", "OverrideSequence");
        getAndCheckElementFromDataset(item, ParameterValueNumber, "1", "3", "OverrideSequence");
        getAndCheckElementFromDataset(item, OperatorsName, "1-n", "2", "OverrideSequence");
        OperatorIdentificationSequence.read(item, "1-n", "3", "OverrideSequence");
        getAndCheckElementFromDataset(item, OverrideReason, "1", "3", "OverrideSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTOverrideSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmAttributeTag(ParameterSequencePointer), "1", "1", "OverrideSequence");
        addElementToDataset(result, item, new DcmAttributeTag(OverrideParameterPointer), "1", "1", "OverrideSequence");
        addElementToDataset(result, item, new DcmIntegerString(ParameterItemIndex), "1", "1", "OverrideSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(ParameterValueNumber), "1", "3", "OverrideSequence");
        addElementToDataset(result, item, new DcmPersonName(OperatorsName), "1-n", "2", "OverrideSequence");
        if (result.good()) result = OperatorIdentificationSequence.write(item, "1-n", "3", "OverrideSequence");
        addElementToDataset(result, item, new DcmShortText(OverrideReason), "1", "3", "OverrideSequence");
    }
    return result;
}


OFCondition DRTOverrideSequence::Item::getOperatorsName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(OperatorsName, value, pos);
}


OFCondition DRTOverrideSequence::Item::getOverrideParameterPointer(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(OverrideParameterPointer, value, pos);
}


OFCondition DRTOverrideSequence::Item::getOverrideReason(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(OverrideReason, value, pos);
}


OFCondition DRTOverrideSequence::Item::getParameterItemIndex(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ParameterItemIndex, value, pos);
}


OFCondition DRTOverrideSequence::Item::getParameterItemIndex(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ParameterItemIndex).getSint32(value, pos);
}


OFCondition DRTOverrideSequence::Item::getParameterSequencePointer(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ParameterSequencePointer, value, pos);
}


OFCondition DRTOverrideSequence::Item::getParameterValueNumber(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, ParameterValueNumber).getUint16(value, pos);
}


OFCondition DRTOverrideSequence::Item::setOperatorsName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
        if (result.good())
            result = OperatorsName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTOverrideSequence::Item::setOverrideParameterPointer(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmAttributeTag::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = OverrideParameterPointer.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTOverrideSequence::Item::setOverrideReason(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = OverrideReason.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTOverrideSequence::Item::setParameterItemIndex(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ParameterItemIndex.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTOverrideSequence::Item::setParameterSequencePointer(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmAttributeTag::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ParameterSequencePointer.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTOverrideSequence::Item::setParameterValueNumber(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ParameterValueNumber.putUint16(value, pos);
}


// --- sequence class ---

DRTOverrideSequence::DRTOverrideSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTOverrideSequence::DRTOverrideSequence(const DRTOverrideSequence &copy)
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


DRTOverrideSequence &DRTOverrideSequence::operator=(const DRTOverrideSequence &copy)
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


DRTOverrideSequence::~DRTOverrideSequence()
{
    clear();
}


void DRTOverrideSequence::clear()
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


OFBool DRTOverrideSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTOverrideSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTOverrideSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTOverrideSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTOverrideSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTOverrideSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTOverrideSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTOverrideSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTOverrideSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTOverrideSequence::Item &DRTOverrideSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTOverrideSequence::Item &DRTOverrideSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTOverrideSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTOverrideSequence::Item &DRTOverrideSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTOverrideSequence::Item &DRTOverrideSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTOverrideSequence::Item &DRTOverrideSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTOverrideSequence::Item &DRTOverrideSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTOverrideSequence::addItem(Item *&item)
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


OFCondition DRTOverrideSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTOverrideSequence::removeItem(const size_t pos)
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


OFCondition DRTOverrideSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_OverrideSequence, sequence);
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
            DcmSequenceOfItems element(DCM_OverrideSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTOverrideSequence::write(DcmItem &dataset,
                                       const OFString &card,
                                       const OFString &type,
                                       const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_OverrideSequence);
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
