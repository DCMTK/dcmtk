/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTReferencedReferenceImageSequenceInRTBeamsModule
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtrris1.h"


// --- item class ---

DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    EndCumulativeMetersetWeight(DCM_EndCumulativeMetersetWeight),
    ReferenceImageNumber(DCM_ReferenceImageNumber),
    ReferencedSOPClassUID(DCM_ReferencedSOPClassUID),
    ReferencedSOPInstanceUID(DCM_ReferencedSOPInstanceUID),
    StartCumulativeMetersetWeight(DCM_StartCumulativeMetersetWeight)
{
}


DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    EndCumulativeMetersetWeight(copy.EndCumulativeMetersetWeight),
    ReferenceImageNumber(copy.ReferenceImageNumber),
    ReferencedSOPClassUID(copy.ReferencedSOPClassUID),
    ReferencedSOPInstanceUID(copy.ReferencedSOPInstanceUID),
    StartCumulativeMetersetWeight(copy.StartCumulativeMetersetWeight)
{
}


DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::~Item()
{
}


DRTReferencedReferenceImageSequenceInRTBeamsModule::Item &DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        EndCumulativeMetersetWeight = copy.EndCumulativeMetersetWeight;
        ReferenceImageNumber = copy.ReferenceImageNumber;
        ReferencedSOPClassUID = copy.ReferencedSOPClassUID;
        ReferencedSOPInstanceUID = copy.ReferencedSOPInstanceUID;
        StartCumulativeMetersetWeight = copy.StartCumulativeMetersetWeight;
    }
    return *this;
}


void DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ReferencedSOPClassUID.clear();
        ReferencedSOPInstanceUID.clear();
        ReferenceImageNumber.clear();
        StartCumulativeMetersetWeight.clear();
        EndCumulativeMetersetWeight.clear();
    }
}


OFBool DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::isEmpty()
{
    return ReferencedSOPClassUID.isEmpty() &&
           ReferencedSOPInstanceUID.isEmpty() &&
           ReferenceImageNumber.isEmpty() &&
           StartCumulativeMetersetWeight.isEmpty() &&
           EndCumulativeMetersetWeight.isEmpty();
}


OFBool DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ReferencedSOPClassUID, "1", "1", "ReferencedReferenceImageSequence");
        getAndCheckElementFromDataset(item, ReferencedSOPInstanceUID, "1", "1", "ReferencedReferenceImageSequence");
        getAndCheckElementFromDataset(item, ReferenceImageNumber, "1", "1", "ReferencedReferenceImageSequence");
        getAndCheckElementFromDataset(item, StartCumulativeMetersetWeight, "1", "3", "ReferencedReferenceImageSequence");
        getAndCheckElementFromDataset(item, EndCumulativeMetersetWeight, "1", "3", "ReferencedReferenceImageSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmUniqueIdentifier(ReferencedSOPClassUID), "1", "1", "ReferencedReferenceImageSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(ReferencedSOPInstanceUID), "1", "1", "ReferencedReferenceImageSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferenceImageNumber), "1", "1", "ReferencedReferenceImageSequence");
        addElementToDataset(result, item, new DcmDecimalString(StartCumulativeMetersetWeight), "1", "3", "ReferencedReferenceImageSequence");
        addElementToDataset(result, item, new DcmDecimalString(EndCumulativeMetersetWeight), "1", "3", "ReferencedReferenceImageSequence");
    }
    return result;
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::getEndCumulativeMetersetWeight(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(EndCumulativeMetersetWeight, value, pos);
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::getEndCumulativeMetersetWeight(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, EndCumulativeMetersetWeight).getFloat64(value, pos);
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::getReferenceImageNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferenceImageNumber, value, pos);
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::getReferenceImageNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferenceImageNumber).getSint32(value, pos);
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::getReferencedSOPClassUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedSOPClassUID, value, pos);
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::getReferencedSOPInstanceUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedSOPInstanceUID, value, pos);
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::getStartCumulativeMetersetWeight(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(StartCumulativeMetersetWeight, value, pos);
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::getStartCumulativeMetersetWeight(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, StartCumulativeMetersetWeight).getFloat64(value, pos);
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::setEndCumulativeMetersetWeight(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = EndCumulativeMetersetWeight.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::setReferenceImageNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferenceImageNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::setReferencedSOPClassUID(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::setReferencedSOPInstanceUID(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::Item::setStartCumulativeMetersetWeight(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = StartCumulativeMetersetWeight.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTReferencedReferenceImageSequenceInRTBeamsModule::DRTReferencedReferenceImageSequenceInRTBeamsModule(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTReferencedReferenceImageSequenceInRTBeamsModule::DRTReferencedReferenceImageSequenceInRTBeamsModule(const DRTReferencedReferenceImageSequenceInRTBeamsModule &copy)
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


DRTReferencedReferenceImageSequenceInRTBeamsModule &DRTReferencedReferenceImageSequenceInRTBeamsModule::operator=(const DRTReferencedReferenceImageSequenceInRTBeamsModule &copy)
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


DRTReferencedReferenceImageSequenceInRTBeamsModule::~DRTReferencedReferenceImageSequenceInRTBeamsModule()
{
    clear();
}


void DRTReferencedReferenceImageSequenceInRTBeamsModule::clear()
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


OFBool DRTReferencedReferenceImageSequenceInRTBeamsModule::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTReferencedReferenceImageSequenceInRTBeamsModule::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTReferencedReferenceImageSequenceInRTBeamsModule::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTReferencedReferenceImageSequenceInRTBeamsModule::Item &DRTReferencedReferenceImageSequenceInRTBeamsModule::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTReferencedReferenceImageSequenceInRTBeamsModule::Item &DRTReferencedReferenceImageSequenceInRTBeamsModule::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTReferencedReferenceImageSequenceInRTBeamsModule::Item &DRTReferencedReferenceImageSequenceInRTBeamsModule::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTReferencedReferenceImageSequenceInRTBeamsModule::Item &DRTReferencedReferenceImageSequenceInRTBeamsModule::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTReferencedReferenceImageSequenceInRTBeamsModule::Item &DRTReferencedReferenceImageSequenceInRTBeamsModule::operator[](const size_t num)
{
    return getItem(num);
}


const DRTReferencedReferenceImageSequenceInRTBeamsModule::Item &DRTReferencedReferenceImageSequenceInRTBeamsModule::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::addItem(Item *&item)
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


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::removeItem(const size_t pos)
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


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ReferencedReferenceImageSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ReferencedReferenceImageSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTReferencedReferenceImageSequenceInRTBeamsModule::write(DcmItem &dataset,
                                                                      const OFString &card,
                                                                      const OFString &type,
                                                                      const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ReferencedReferenceImageSequence);
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
