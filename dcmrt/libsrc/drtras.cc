/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTRequestAttributesSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtras.h"


// --- item class ---

DRTRequestAttributesSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AccessionNumber(DCM_AccessionNumber),
    IssuerOfAccessionNumberSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReasonForRequestedProcedureCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReasonForTheRequestedProcedure(DCM_ReasonForTheRequestedProcedure),
    ReferencedStudySequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RequestedProcedureCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RequestedProcedureDescription(DCM_RequestedProcedureDescription),
    RequestedProcedureID(DCM_RequestedProcedureID),
    ScheduledProcedureStepDescription(DCM_ScheduledProcedureStepDescription),
    ScheduledProcedureStepID(DCM_ScheduledProcedureStepID),
    ScheduledProtocolCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    StudyInstanceUID(DCM_StudyInstanceUID)
{
}


DRTRequestAttributesSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AccessionNumber(copy.AccessionNumber),
    IssuerOfAccessionNumberSequence(copy.IssuerOfAccessionNumberSequence),
    ReasonForRequestedProcedureCodeSequence(copy.ReasonForRequestedProcedureCodeSequence),
    ReasonForTheRequestedProcedure(copy.ReasonForTheRequestedProcedure),
    ReferencedStudySequence(copy.ReferencedStudySequence),
    RequestedProcedureCodeSequence(copy.RequestedProcedureCodeSequence),
    RequestedProcedureDescription(copy.RequestedProcedureDescription),
    RequestedProcedureID(copy.RequestedProcedureID),
    ScheduledProcedureStepDescription(copy.ScheduledProcedureStepDescription),
    ScheduledProcedureStepID(copy.ScheduledProcedureStepID),
    ScheduledProtocolCodeSequence(copy.ScheduledProtocolCodeSequence),
    StudyInstanceUID(copy.StudyInstanceUID)
{
}


DRTRequestAttributesSequence::Item::~Item()
{
}


DRTRequestAttributesSequence::Item &DRTRequestAttributesSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AccessionNumber = copy.AccessionNumber;
        IssuerOfAccessionNumberSequence = copy.IssuerOfAccessionNumberSequence;
        ReasonForRequestedProcedureCodeSequence = copy.ReasonForRequestedProcedureCodeSequence;
        ReasonForTheRequestedProcedure = copy.ReasonForTheRequestedProcedure;
        ReferencedStudySequence = copy.ReferencedStudySequence;
        RequestedProcedureCodeSequence = copy.RequestedProcedureCodeSequence;
        RequestedProcedureDescription = copy.RequestedProcedureDescription;
        RequestedProcedureID = copy.RequestedProcedureID;
        ScheduledProcedureStepDescription = copy.ScheduledProcedureStepDescription;
        ScheduledProcedureStepID = copy.ScheduledProcedureStepID;
        ScheduledProtocolCodeSequence = copy.ScheduledProtocolCodeSequence;
        StudyInstanceUID = copy.StudyInstanceUID;
    }
    return *this;
}


void DRTRequestAttributesSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        RequestedProcedureID.clear();
        AccessionNumber.clear();
        IssuerOfAccessionNumberSequence.clear();
        StudyInstanceUID.clear();
        ReferencedStudySequence.clear();
        RequestedProcedureDescription.clear();
        RequestedProcedureCodeSequence.clear();
        ReasonForTheRequestedProcedure.clear();
        ReasonForRequestedProcedureCodeSequence.clear();
        ScheduledProcedureStepID.clear();
        ScheduledProcedureStepDescription.clear();
        ScheduledProtocolCodeSequence.clear();
    }
}


OFBool DRTRequestAttributesSequence::Item::isEmpty()
{
    return RequestedProcedureID.isEmpty() &&
           AccessionNumber.isEmpty() &&
           IssuerOfAccessionNumberSequence.isEmpty() &&
           StudyInstanceUID.isEmpty() &&
           ReferencedStudySequence.isEmpty() &&
           RequestedProcedureDescription.isEmpty() &&
           RequestedProcedureCodeSequence.isEmpty() &&
           ReasonForTheRequestedProcedure.isEmpty() &&
           ReasonForRequestedProcedureCodeSequence.isEmpty() &&
           ScheduledProcedureStepID.isEmpty() &&
           ScheduledProcedureStepDescription.isEmpty() &&
           ScheduledProtocolCodeSequence.isEmpty();
}


OFBool DRTRequestAttributesSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTRequestAttributesSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, RequestedProcedureID, "1", "1C", "RequestAttributesSequence");
        getAndCheckElementFromDataset(item, AccessionNumber, "1", "3", "RequestAttributesSequence");
        IssuerOfAccessionNumberSequence.read(item, "1-n", "3", "RequestAttributesSequence");
        getAndCheckElementFromDataset(item, StudyInstanceUID, "1", "3", "RequestAttributesSequence");
        ReferencedStudySequence.read(item, "1-n", "3", "RequestAttributesSequence");
        getAndCheckElementFromDataset(item, RequestedProcedureDescription, "1", "3", "RequestAttributesSequence");
        RequestedProcedureCodeSequence.read(item, "1-n", "3", "RequestAttributesSequence");
        getAndCheckElementFromDataset(item, ReasonForTheRequestedProcedure, "1", "3", "RequestAttributesSequence");
        ReasonForRequestedProcedureCodeSequence.read(item, "1-n", "3", "RequestAttributesSequence");
        getAndCheckElementFromDataset(item, ScheduledProcedureStepID, "1", "1C", "RequestAttributesSequence");
        getAndCheckElementFromDataset(item, ScheduledProcedureStepDescription, "1", "3", "RequestAttributesSequence");
        ScheduledProtocolCodeSequence.read(item, "1-n", "3", "RequestAttributesSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTRequestAttributesSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmShortString(RequestedProcedureID), "1", "1C", "RequestAttributesSequence");
        addElementToDataset(result, item, new DcmShortString(AccessionNumber), "1", "3", "RequestAttributesSequence");
        if (result.good()) result = IssuerOfAccessionNumberSequence.write(item, "1-n", "3", "RequestAttributesSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(StudyInstanceUID), "1", "3", "RequestAttributesSequence");
        if (result.good()) result = ReferencedStudySequence.write(item, "1-n", "3", "RequestAttributesSequence");
        addElementToDataset(result, item, new DcmLongString(RequestedProcedureDescription), "1", "3", "RequestAttributesSequence");
        if (result.good()) result = RequestedProcedureCodeSequence.write(item, "1-n", "3", "RequestAttributesSequence");
        addElementToDataset(result, item, new DcmLongString(ReasonForTheRequestedProcedure), "1", "3", "RequestAttributesSequence");
        if (result.good()) result = ReasonForRequestedProcedureCodeSequence.write(item, "1-n", "3", "RequestAttributesSequence");
        addElementToDataset(result, item, new DcmShortString(ScheduledProcedureStepID), "1", "1C", "RequestAttributesSequence");
        addElementToDataset(result, item, new DcmLongString(ScheduledProcedureStepDescription), "1", "3", "RequestAttributesSequence");
        if (result.good()) result = ScheduledProtocolCodeSequence.write(item, "1-n", "3", "RequestAttributesSequence");
    }
    return result;
}


OFCondition DRTRequestAttributesSequence::Item::getAccessionNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AccessionNumber, value, pos);
}


OFCondition DRTRequestAttributesSequence::Item::getReasonForTheRequestedProcedure(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReasonForTheRequestedProcedure, value, pos);
}


OFCondition DRTRequestAttributesSequence::Item::getRequestedProcedureDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RequestedProcedureDescription, value, pos);
}


OFCondition DRTRequestAttributesSequence::Item::getRequestedProcedureID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RequestedProcedureID, value, pos);
}


OFCondition DRTRequestAttributesSequence::Item::getScheduledProcedureStepDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ScheduledProcedureStepDescription, value, pos);
}


OFCondition DRTRequestAttributesSequence::Item::getScheduledProcedureStepID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ScheduledProcedureStepID, value, pos);
}


OFCondition DRTRequestAttributesSequence::Item::getStudyInstanceUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(StudyInstanceUID, value, pos);
}


OFCondition DRTRequestAttributesSequence::Item::setAccessionNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = AccessionNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRequestAttributesSequence::Item::setReasonForTheRequestedProcedure(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReasonForTheRequestedProcedure.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRequestAttributesSequence::Item::setRequestedProcedureDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RequestedProcedureDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRequestAttributesSequence::Item::setRequestedProcedureID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RequestedProcedureID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRequestAttributesSequence::Item::setScheduledProcedureStepDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ScheduledProcedureStepDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRequestAttributesSequence::Item::setScheduledProcedureStepID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ScheduledProcedureStepID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRequestAttributesSequence::Item::setStudyInstanceUID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = StudyInstanceUID.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTRequestAttributesSequence::DRTRequestAttributesSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTRequestAttributesSequence::DRTRequestAttributesSequence(const DRTRequestAttributesSequence &copy)
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


DRTRequestAttributesSequence &DRTRequestAttributesSequence::operator=(const DRTRequestAttributesSequence &copy)
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


DRTRequestAttributesSequence::~DRTRequestAttributesSequence()
{
    clear();
}


void DRTRequestAttributesSequence::clear()
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


OFBool DRTRequestAttributesSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTRequestAttributesSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTRequestAttributesSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTRequestAttributesSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTRequestAttributesSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTRequestAttributesSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTRequestAttributesSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTRequestAttributesSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTRequestAttributesSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTRequestAttributesSequence::Item &DRTRequestAttributesSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTRequestAttributesSequence::Item &DRTRequestAttributesSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTRequestAttributesSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTRequestAttributesSequence::Item &DRTRequestAttributesSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTRequestAttributesSequence::Item &DRTRequestAttributesSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTRequestAttributesSequence::Item &DRTRequestAttributesSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTRequestAttributesSequence::Item &DRTRequestAttributesSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTRequestAttributesSequence::addItem(Item *&item)
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


OFCondition DRTRequestAttributesSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTRequestAttributesSequence::removeItem(const unsigned long pos)
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


OFCondition DRTRequestAttributesSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_RequestAttributesSequence, sequence);
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
            DcmSequenceOfItems element(DCM_RequestAttributesSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTRequestAttributesSequence::write(DcmItem &dataset,
                                                const OFString &card,
                                                const OFString &type,
                                                const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_RequestAttributesSequence);
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
