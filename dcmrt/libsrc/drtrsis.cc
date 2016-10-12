/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTReferencedSetupImageSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtrsis.h"


// --- item class ---

DRTReferencedSetupImageSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ReferencedFrameNumber(DCM_ReferencedFrameNumber),
    ReferencedSOPClassUID(DCM_ReferencedSOPClassUID),
    ReferencedSOPInstanceUID(DCM_ReferencedSOPInstanceUID),
    ReferencedSegmentNumber(DCM_ReferencedSegmentNumber),
    SetupImageComment(DCM_SetupImageComment)
{
}


DRTReferencedSetupImageSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ReferencedFrameNumber(copy.ReferencedFrameNumber),
    ReferencedSOPClassUID(copy.ReferencedSOPClassUID),
    ReferencedSOPInstanceUID(copy.ReferencedSOPInstanceUID),
    ReferencedSegmentNumber(copy.ReferencedSegmentNumber),
    SetupImageComment(copy.SetupImageComment)
{
}


DRTReferencedSetupImageSequence::Item::~Item()
{
}


DRTReferencedSetupImageSequence::Item &DRTReferencedSetupImageSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ReferencedFrameNumber = copy.ReferencedFrameNumber;
        ReferencedSOPClassUID = copy.ReferencedSOPClassUID;
        ReferencedSOPInstanceUID = copy.ReferencedSOPInstanceUID;
        ReferencedSegmentNumber = copy.ReferencedSegmentNumber;
        SetupImageComment = copy.SetupImageComment;
    }
    return *this;
}


void DRTReferencedSetupImageSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        SetupImageComment.clear();
        ReferencedSOPClassUID.clear();
        ReferencedSOPInstanceUID.clear();
        ReferencedFrameNumber.clear();
        ReferencedSegmentNumber.clear();
    }
}


OFBool DRTReferencedSetupImageSequence::Item::isEmpty()
{
    return SetupImageComment.isEmpty() &&
           ReferencedSOPClassUID.isEmpty() &&
           ReferencedSOPInstanceUID.isEmpty() &&
           ReferencedFrameNumber.isEmpty() &&
           ReferencedSegmentNumber.isEmpty();
}


OFBool DRTReferencedSetupImageSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTReferencedSetupImageSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, SetupImageComment, "1", "3", "ReferencedSetupImageSequence");
        getAndCheckElementFromDataset(item, ReferencedSOPClassUID, "1", "1", "ReferencedSetupImageSequence");
        getAndCheckElementFromDataset(item, ReferencedSOPInstanceUID, "1", "1", "ReferencedSetupImageSequence");
        getAndCheckElementFromDataset(item, ReferencedFrameNumber, "1-n", "1C", "ReferencedSetupImageSequence");
        getAndCheckElementFromDataset(item, ReferencedSegmentNumber, "1-n", "1C", "ReferencedSetupImageSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedSetupImageSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmShortText(SetupImageComment), "1", "3", "ReferencedSetupImageSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(ReferencedSOPClassUID), "1", "1", "ReferencedSetupImageSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(ReferencedSOPInstanceUID), "1", "1", "ReferencedSetupImageSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferencedFrameNumber), "1-n", "1C", "ReferencedSetupImageSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(ReferencedSegmentNumber), "1-n", "1C", "ReferencedSetupImageSequence");
    }
    return result;
}


OFCondition DRTReferencedSetupImageSequence::Item::getReferencedFrameNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedFrameNumber, value, pos);
}


OFCondition DRTReferencedSetupImageSequence::Item::getReferencedFrameNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedFrameNumber).getSint32(value, pos);
}


OFCondition DRTReferencedSetupImageSequence::Item::getReferencedSOPClassUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedSOPClassUID, value, pos);
}


OFCondition DRTReferencedSetupImageSequence::Item::getReferencedSOPInstanceUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedSOPInstanceUID, value, pos);
}


OFCondition DRTReferencedSetupImageSequence::Item::getReferencedSegmentNumber(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, ReferencedSegmentNumber).getUint16(value, pos);
}


OFCondition DRTReferencedSetupImageSequence::Item::getSetupImageComment(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SetupImageComment, value, pos);
}


OFCondition DRTReferencedSetupImageSequence::Item::setReferencedFrameNumber(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedSetupImageSequence::Item::setReferencedSOPClassUID(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedSetupImageSequence::Item::setReferencedSOPInstanceUID(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedSetupImageSequence::Item::setReferencedSegmentNumber(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ReferencedSegmentNumber.putUint16(value, pos);
}


OFCondition DRTReferencedSetupImageSequence::Item::setSetupImageComment(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = SetupImageComment.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTReferencedSetupImageSequence::DRTReferencedSetupImageSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTReferencedSetupImageSequence::DRTReferencedSetupImageSequence(const DRTReferencedSetupImageSequence &copy)
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


DRTReferencedSetupImageSequence &DRTReferencedSetupImageSequence::operator=(const DRTReferencedSetupImageSequence &copy)
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


DRTReferencedSetupImageSequence::~DRTReferencedSetupImageSequence()
{
    clear();
}


void DRTReferencedSetupImageSequence::clear()
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


OFBool DRTReferencedSetupImageSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTReferencedSetupImageSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTReferencedSetupImageSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTReferencedSetupImageSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedSetupImageSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedSetupImageSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTReferencedSetupImageSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTReferencedSetupImageSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTReferencedSetupImageSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTReferencedSetupImageSequence::Item &DRTReferencedSetupImageSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTReferencedSetupImageSequence::Item &DRTReferencedSetupImageSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTReferencedSetupImageSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTReferencedSetupImageSequence::Item &DRTReferencedSetupImageSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTReferencedSetupImageSequence::Item &DRTReferencedSetupImageSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTReferencedSetupImageSequence::Item &DRTReferencedSetupImageSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTReferencedSetupImageSequence::Item &DRTReferencedSetupImageSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTReferencedSetupImageSequence::addItem(Item *&item)
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


OFCondition DRTReferencedSetupImageSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTReferencedSetupImageSequence::removeItem(const unsigned long pos)
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


OFCondition DRTReferencedSetupImageSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ReferencedSetupImageSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ReferencedSetupImageSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTReferencedSetupImageSequence::write(DcmItem &dataset,
                                                   const OFString &card,
                                                   const OFString &type,
                                                   const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ReferencedSetupImageSequence);
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
