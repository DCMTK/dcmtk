/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTGroupOfPatientsIdentificationSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtgpis.h"


// --- item class ---

DRTGroupOfPatientsIdentificationSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    IssuerOfPatientID(DCM_IssuerOfPatientID),
    IssuerOfPatientIDQualifiersSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    PatientID(DCM_PatientID),
    PatientPosition(DCM_PatientPosition),
    SubjectRelativePositionInImage(DCM_SubjectRelativePositionInImage)
{
}


DRTGroupOfPatientsIdentificationSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    IssuerOfPatientID(copy.IssuerOfPatientID),
    IssuerOfPatientIDQualifiersSequence(copy.IssuerOfPatientIDQualifiersSequence),
    PatientID(copy.PatientID),
    PatientPosition(copy.PatientPosition),
    SubjectRelativePositionInImage(copy.SubjectRelativePositionInImage)
{
}


DRTGroupOfPatientsIdentificationSequence::Item::~Item()
{
}


DRTGroupOfPatientsIdentificationSequence::Item &DRTGroupOfPatientsIdentificationSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        IssuerOfPatientID = copy.IssuerOfPatientID;
        IssuerOfPatientIDQualifiersSequence = copy.IssuerOfPatientIDQualifiersSequence;
        PatientID = copy.PatientID;
        PatientPosition = copy.PatientPosition;
        SubjectRelativePositionInImage = copy.SubjectRelativePositionInImage;
    }
    return *this;
}


void DRTGroupOfPatientsIdentificationSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        PatientID.clear();
        IssuerOfPatientID.clear();
        IssuerOfPatientIDQualifiersSequence.clear();
        SubjectRelativePositionInImage.clear();
        PatientPosition.clear();
    }
}


OFBool DRTGroupOfPatientsIdentificationSequence::Item::isEmpty()
{
    return PatientID.isEmpty() &&
           IssuerOfPatientID.isEmpty() &&
           IssuerOfPatientIDQualifiersSequence.isEmpty() &&
           SubjectRelativePositionInImage.isEmpty() &&
           PatientPosition.isEmpty();
}


OFBool DRTGroupOfPatientsIdentificationSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTGroupOfPatientsIdentificationSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, PatientID, "1", "1", "GroupOfPatientsIdentificationSequence");
        getAndCheckElementFromDataset(item, IssuerOfPatientID, "1", "3", "GroupOfPatientsIdentificationSequence");
        IssuerOfPatientIDQualifiersSequence.read(item, "1-n", "3", "GroupOfPatientsIdentificationSequence");
        getAndCheckElementFromDataset(item, SubjectRelativePositionInImage, "3", "3", "GroupOfPatientsIdentificationSequence");
        getAndCheckElementFromDataset(item, PatientPosition, "1", "3", "GroupOfPatientsIdentificationSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTGroupOfPatientsIdentificationSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmLongString(PatientID), "1", "1", "GroupOfPatientsIdentificationSequence");
        addElementToDataset(result, item, new DcmLongString(IssuerOfPatientID), "1", "3", "GroupOfPatientsIdentificationSequence");
        if (result.good()) result = IssuerOfPatientIDQualifiersSequence.write(item, "1-n", "3", "GroupOfPatientsIdentificationSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(SubjectRelativePositionInImage), "3", "3", "GroupOfPatientsIdentificationSequence");
        addElementToDataset(result, item, new DcmCodeString(PatientPosition), "1", "3", "GroupOfPatientsIdentificationSequence");
    }
    return result;
}


OFCondition DRTGroupOfPatientsIdentificationSequence::Item::getIssuerOfPatientID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(IssuerOfPatientID, value, pos);
}


OFCondition DRTGroupOfPatientsIdentificationSequence::Item::getPatientID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientID, value, pos);
}


OFCondition DRTGroupOfPatientsIdentificationSequence::Item::getPatientPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientPosition, value, pos);
}


OFCondition DRTGroupOfPatientsIdentificationSequence::Item::getSubjectRelativePositionInImage(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, SubjectRelativePositionInImage).getUint16(value, pos);
}


OFCondition DRTGroupOfPatientsIdentificationSequence::Item::setIssuerOfPatientID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = IssuerOfPatientID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTGroupOfPatientsIdentificationSequence::Item::setPatientID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PatientID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTGroupOfPatientsIdentificationSequence::Item::setPatientPosition(const OFString &value, const OFBool check)
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


OFCondition DRTGroupOfPatientsIdentificationSequence::Item::setSubjectRelativePositionInImage(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return SubjectRelativePositionInImage.putUint16(value, pos);
}


// --- sequence class ---

DRTGroupOfPatientsIdentificationSequence::DRTGroupOfPatientsIdentificationSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTGroupOfPatientsIdentificationSequence::DRTGroupOfPatientsIdentificationSequence(const DRTGroupOfPatientsIdentificationSequence &copy)
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


DRTGroupOfPatientsIdentificationSequence &DRTGroupOfPatientsIdentificationSequence::operator=(const DRTGroupOfPatientsIdentificationSequence &copy)
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


DRTGroupOfPatientsIdentificationSequence::~DRTGroupOfPatientsIdentificationSequence()
{
    clear();
}


void DRTGroupOfPatientsIdentificationSequence::clear()
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


OFBool DRTGroupOfPatientsIdentificationSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTGroupOfPatientsIdentificationSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTGroupOfPatientsIdentificationSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTGroupOfPatientsIdentificationSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTGroupOfPatientsIdentificationSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTGroupOfPatientsIdentificationSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTGroupOfPatientsIdentificationSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTGroupOfPatientsIdentificationSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTGroupOfPatientsIdentificationSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTGroupOfPatientsIdentificationSequence::Item &DRTGroupOfPatientsIdentificationSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTGroupOfPatientsIdentificationSequence::Item &DRTGroupOfPatientsIdentificationSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTGroupOfPatientsIdentificationSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTGroupOfPatientsIdentificationSequence::Item &DRTGroupOfPatientsIdentificationSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTGroupOfPatientsIdentificationSequence::Item &DRTGroupOfPatientsIdentificationSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTGroupOfPatientsIdentificationSequence::Item &DRTGroupOfPatientsIdentificationSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTGroupOfPatientsIdentificationSequence::Item &DRTGroupOfPatientsIdentificationSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTGroupOfPatientsIdentificationSequence::addItem(Item *&item)
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


OFCondition DRTGroupOfPatientsIdentificationSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTGroupOfPatientsIdentificationSequence::removeItem(const size_t pos)
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


OFCondition DRTGroupOfPatientsIdentificationSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_GroupOfPatientsIdentificationSequence, sequence);
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
            DcmSequenceOfItems element(DCM_GroupOfPatientsIdentificationSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTGroupOfPatientsIdentificationSequence::write(DcmItem &dataset,
                                                            const OFString &card,
                                                            const OFString &type,
                                                            const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_GroupOfPatientsIdentificationSequence);
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
