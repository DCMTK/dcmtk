/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTReferencedDefinedProtocolSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtrdps.h"


// --- item class ---

DRTReferencedDefinedProtocolSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ReferencedSOPClassUID(DCM_ReferencedSOPClassUID),
    ReferencedSOPInstanceUID(DCM_ReferencedSOPInstanceUID),
    SourceAcquisitionProtocolElementNumber(DCM_SourceAcquisitionProtocolElementNumber),
    SourceReconstructionProtocolElementNumber(DCM_SourceReconstructionProtocolElementNumber)
{
}


DRTReferencedDefinedProtocolSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ReferencedSOPClassUID(copy.ReferencedSOPClassUID),
    ReferencedSOPInstanceUID(copy.ReferencedSOPInstanceUID),
    SourceAcquisitionProtocolElementNumber(copy.SourceAcquisitionProtocolElementNumber),
    SourceReconstructionProtocolElementNumber(copy.SourceReconstructionProtocolElementNumber)
{
}


DRTReferencedDefinedProtocolSequence::Item::~Item()
{
}


DRTReferencedDefinedProtocolSequence::Item &DRTReferencedDefinedProtocolSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ReferencedSOPClassUID = copy.ReferencedSOPClassUID;
        ReferencedSOPInstanceUID = copy.ReferencedSOPInstanceUID;
        SourceAcquisitionProtocolElementNumber = copy.SourceAcquisitionProtocolElementNumber;
        SourceReconstructionProtocolElementNumber = copy.SourceReconstructionProtocolElementNumber;
    }
    return *this;
}


void DRTReferencedDefinedProtocolSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ReferencedSOPClassUID.clear();
        ReferencedSOPInstanceUID.clear();
        SourceAcquisitionProtocolElementNumber.clear();
        SourceReconstructionProtocolElementNumber.clear();
    }
}


OFBool DRTReferencedDefinedProtocolSequence::Item::isEmpty()
{
    return ReferencedSOPClassUID.isEmpty() &&
           ReferencedSOPInstanceUID.isEmpty() &&
           SourceAcquisitionProtocolElementNumber.isEmpty() &&
           SourceReconstructionProtocolElementNumber.isEmpty();
}


OFBool DRTReferencedDefinedProtocolSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTReferencedDefinedProtocolSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ReferencedSOPClassUID, "1", "1", "ReferencedDefinedProtocolSequence");
        getAndCheckElementFromDataset(item, ReferencedSOPInstanceUID, "1", "1", "ReferencedDefinedProtocolSequence");
        getAndCheckElementFromDataset(item, SourceAcquisitionProtocolElementNumber, "1-n", "3", "ReferencedDefinedProtocolSequence");
        getAndCheckElementFromDataset(item, SourceReconstructionProtocolElementNumber, "1-n", "3", "ReferencedDefinedProtocolSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedDefinedProtocolSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmUniqueIdentifier(ReferencedSOPClassUID), "1", "1", "ReferencedDefinedProtocolSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(ReferencedSOPInstanceUID), "1", "1", "ReferencedDefinedProtocolSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(SourceAcquisitionProtocolElementNumber), "1-n", "3", "ReferencedDefinedProtocolSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(SourceReconstructionProtocolElementNumber), "1-n", "3", "ReferencedDefinedProtocolSequence");
    }
    return result;
}


OFCondition DRTReferencedDefinedProtocolSequence::Item::getReferencedSOPClassUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedSOPClassUID, value, pos);
}


OFCondition DRTReferencedDefinedProtocolSequence::Item::getReferencedSOPInstanceUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedSOPInstanceUID, value, pos);
}


OFCondition DRTReferencedDefinedProtocolSequence::Item::getSourceAcquisitionProtocolElementNumber(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, SourceAcquisitionProtocolElementNumber).getUint16(value, pos);
}


OFCondition DRTReferencedDefinedProtocolSequence::Item::getSourceReconstructionProtocolElementNumber(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, SourceReconstructionProtocolElementNumber).getUint16(value, pos);
}


OFCondition DRTReferencedDefinedProtocolSequence::Item::setReferencedSOPClassUID(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedDefinedProtocolSequence::Item::setReferencedSOPInstanceUID(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedDefinedProtocolSequence::Item::setSourceAcquisitionProtocolElementNumber(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return SourceAcquisitionProtocolElementNumber.putUint16(value, pos);
}


OFCondition DRTReferencedDefinedProtocolSequence::Item::setSourceReconstructionProtocolElementNumber(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return SourceReconstructionProtocolElementNumber.putUint16(value, pos);
}


// --- sequence class ---

DRTReferencedDefinedProtocolSequence::DRTReferencedDefinedProtocolSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTReferencedDefinedProtocolSequence::DRTReferencedDefinedProtocolSequence(const DRTReferencedDefinedProtocolSequence &copy)
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


DRTReferencedDefinedProtocolSequence &DRTReferencedDefinedProtocolSequence::operator=(const DRTReferencedDefinedProtocolSequence &copy)
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


DRTReferencedDefinedProtocolSequence::~DRTReferencedDefinedProtocolSequence()
{
    clear();
}


void DRTReferencedDefinedProtocolSequence::clear()
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


OFBool DRTReferencedDefinedProtocolSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTReferencedDefinedProtocolSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTReferencedDefinedProtocolSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTReferencedDefinedProtocolSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedDefinedProtocolSequence::gotoNextItem()
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


OFCondition DRTReferencedDefinedProtocolSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTReferencedDefinedProtocolSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTReferencedDefinedProtocolSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTReferencedDefinedProtocolSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTReferencedDefinedProtocolSequence::Item &DRTReferencedDefinedProtocolSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTReferencedDefinedProtocolSequence::Item &DRTReferencedDefinedProtocolSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTReferencedDefinedProtocolSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTReferencedDefinedProtocolSequence::Item &DRTReferencedDefinedProtocolSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTReferencedDefinedProtocolSequence::Item &DRTReferencedDefinedProtocolSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTReferencedDefinedProtocolSequence::Item &DRTReferencedDefinedProtocolSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTReferencedDefinedProtocolSequence::Item &DRTReferencedDefinedProtocolSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTReferencedDefinedProtocolSequence::addItem(Item *&item)
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


OFCondition DRTReferencedDefinedProtocolSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTReferencedDefinedProtocolSequence::removeItem(const size_t pos)
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


OFCondition DRTReferencedDefinedProtocolSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ReferencedDefinedProtocolSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ReferencedDefinedProtocolSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTReferencedDefinedProtocolSequence::write(DcmItem &dataset,
                                                        const OFString &card,
                                                        const OFString &type,
                                                        const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ReferencedDefinedProtocolSequence);
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
