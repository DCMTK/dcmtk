/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTReferencedSOPSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtrsos.h"


// --- item class ---

DRTReferencedSOPSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    HL7InstanceIdentifier(DCM_HL7InstanceIdentifier),
    ReferencedFrameNumber(DCM_ReferencedFrameNumber),
    ReferencedSOPClassUID(DCM_ReferencedSOPClassUID),
    ReferencedSOPInstanceUID(DCM_ReferencedSOPInstanceUID),
    ReferencedSegmentNumber(DCM_ReferencedSegmentNumber)
{
}


DRTReferencedSOPSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    HL7InstanceIdentifier(copy.HL7InstanceIdentifier),
    ReferencedFrameNumber(copy.ReferencedFrameNumber),
    ReferencedSOPClassUID(copy.ReferencedSOPClassUID),
    ReferencedSOPInstanceUID(copy.ReferencedSOPInstanceUID),
    ReferencedSegmentNumber(copy.ReferencedSegmentNumber)
{
}


DRTReferencedSOPSequence::Item::~Item()
{
}


DRTReferencedSOPSequence::Item &DRTReferencedSOPSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        HL7InstanceIdentifier = copy.HL7InstanceIdentifier;
        ReferencedFrameNumber = copy.ReferencedFrameNumber;
        ReferencedSOPClassUID = copy.ReferencedSOPClassUID;
        ReferencedSOPInstanceUID = copy.ReferencedSOPInstanceUID;
        ReferencedSegmentNumber = copy.ReferencedSegmentNumber;
    }
    return *this;
}


void DRTReferencedSOPSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ReferencedSOPClassUID.clear();
        ReferencedSOPInstanceUID.clear();
        HL7InstanceIdentifier.clear();
        ReferencedFrameNumber.clear();
        ReferencedSegmentNumber.clear();
    }
}


OFBool DRTReferencedSOPSequence::Item::isEmpty()
{
    return ReferencedSOPClassUID.isEmpty() &&
           ReferencedSOPInstanceUID.isEmpty() &&
           HL7InstanceIdentifier.isEmpty() &&
           ReferencedFrameNumber.isEmpty() &&
           ReferencedSegmentNumber.isEmpty();
}


OFBool DRTReferencedSOPSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTReferencedSOPSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ReferencedSOPClassUID, "1", "1", "ReferencedSOPSequence");
        getAndCheckElementFromDataset(item, ReferencedSOPInstanceUID, "1", "1", "ReferencedSOPSequence");
        getAndCheckElementFromDataset(item, HL7InstanceIdentifier, "1", "1C", "ReferencedSOPSequence");
        getAndCheckElementFromDataset(item, ReferencedFrameNumber, "1-n", "1C", "ReferencedSOPSequence");
        getAndCheckElementFromDataset(item, ReferencedSegmentNumber, "1-n", "1C", "ReferencedSOPSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedSOPSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmUniqueIdentifier(ReferencedSOPClassUID), "1", "1", "ReferencedSOPSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(ReferencedSOPInstanceUID), "1", "1", "ReferencedSOPSequence");
        addElementToDataset(result, item, new DcmShortText(HL7InstanceIdentifier), "1", "1C", "ReferencedSOPSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferencedFrameNumber), "1-n", "1C", "ReferencedSOPSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(ReferencedSegmentNumber), "1-n", "1C", "ReferencedSOPSequence");
    }
    return result;
}


OFCondition DRTReferencedSOPSequence::Item::getHL7InstanceIdentifier(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(HL7InstanceIdentifier, value, pos);
}


OFCondition DRTReferencedSOPSequence::Item::getReferencedFrameNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedFrameNumber, value, pos);
}


OFCondition DRTReferencedSOPSequence::Item::getReferencedFrameNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedFrameNumber).getSint32(value, pos);
}


OFCondition DRTReferencedSOPSequence::Item::getReferencedSOPClassUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedSOPClassUID, value, pos);
}


OFCondition DRTReferencedSOPSequence::Item::getReferencedSOPInstanceUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedSOPInstanceUID, value, pos);
}


OFCondition DRTReferencedSOPSequence::Item::getReferencedSegmentNumber(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, ReferencedSegmentNumber).getUint16(value, pos);
}


OFCondition DRTReferencedSOPSequence::Item::setHL7InstanceIdentifier(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = HL7InstanceIdentifier.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedSOPSequence::Item::setReferencedFrameNumber(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedSOPSequence::Item::setReferencedSOPClassUID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedSOPClassUID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedSOPSequence::Item::setReferencedSOPInstanceUID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedSOPInstanceUID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedSOPSequence::Item::setReferencedSegmentNumber(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ReferencedSegmentNumber.putUint16(value, pos);
}


// --- sequence class ---

DRTReferencedSOPSequence::DRTReferencedSOPSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTReferencedSOPSequence::DRTReferencedSOPSequence(const DRTReferencedSOPSequence &copy)
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


DRTReferencedSOPSequence &DRTReferencedSOPSequence::operator=(const DRTReferencedSOPSequence &copy)
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


DRTReferencedSOPSequence::~DRTReferencedSOPSequence()
{
    clear();
}


void DRTReferencedSOPSequence::clear()
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


OFBool DRTReferencedSOPSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTReferencedSOPSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTReferencedSOPSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTReferencedSOPSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedSOPSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedSOPSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTReferencedSOPSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTReferencedSOPSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTReferencedSOPSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTReferencedSOPSequence::Item &DRTReferencedSOPSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTReferencedSOPSequence::Item &DRTReferencedSOPSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTReferencedSOPSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTReferencedSOPSequence::Item &DRTReferencedSOPSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTReferencedSOPSequence::Item &DRTReferencedSOPSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTReferencedSOPSequence::Item &DRTReferencedSOPSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTReferencedSOPSequence::Item &DRTReferencedSOPSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTReferencedSOPSequence::addItem(Item *&item)
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


OFCondition DRTReferencedSOPSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTReferencedSOPSequence::removeItem(const unsigned long pos)
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


OFCondition DRTReferencedSOPSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ReferencedSOPSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ReferencedSOPSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTReferencedSOPSequence::write(DcmItem &dataset,
                                            const OFString &card,
                                            const OFString &type,
                                            const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ReferencedSOPSequence);
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
