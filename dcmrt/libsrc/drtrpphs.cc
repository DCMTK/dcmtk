/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTReferencedPatientPhotoSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtrpphs.h"


// --- item class ---

DRTReferencedPatientPhotoSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    DICOMMediaRetrievalSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    DICOMRetrievalSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedSOPSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    SeriesInstanceUID(DCM_SeriesInstanceUID),
    StudyInstanceUID(DCM_StudyInstanceUID),
    TypeOfInstances(DCM_TypeOfInstances),
    WADORSRetrievalSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    WADORetrievalSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    XDSRetrievalSequence(emptyDefaultItem /*emptyDefaultSequence*/)
{
}


DRTReferencedPatientPhotoSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    DICOMMediaRetrievalSequence(copy.DICOMMediaRetrievalSequence),
    DICOMRetrievalSequence(copy.DICOMRetrievalSequence),
    ReferencedSOPSequence(copy.ReferencedSOPSequence),
    SeriesInstanceUID(copy.SeriesInstanceUID),
    StudyInstanceUID(copy.StudyInstanceUID),
    TypeOfInstances(copy.TypeOfInstances),
    WADORSRetrievalSequence(copy.WADORSRetrievalSequence),
    WADORetrievalSequence(copy.WADORetrievalSequence),
    XDSRetrievalSequence(copy.XDSRetrievalSequence)
{
}


DRTReferencedPatientPhotoSequence::Item::~Item()
{
}


DRTReferencedPatientPhotoSequence::Item &DRTReferencedPatientPhotoSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        DICOMMediaRetrievalSequence = copy.DICOMMediaRetrievalSequence;
        DICOMRetrievalSequence = copy.DICOMRetrievalSequence;
        ReferencedSOPSequence = copy.ReferencedSOPSequence;
        SeriesInstanceUID = copy.SeriesInstanceUID;
        StudyInstanceUID = copy.StudyInstanceUID;
        TypeOfInstances = copy.TypeOfInstances;
        WADORSRetrievalSequence = copy.WADORSRetrievalSequence;
        WADORetrievalSequence = copy.WADORetrievalSequence;
        XDSRetrievalSequence = copy.XDSRetrievalSequence;
    }
    return *this;
}


void DRTReferencedPatientPhotoSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        TypeOfInstances.clear();
        StudyInstanceUID.clear();
        SeriesInstanceUID.clear();
        ReferencedSOPSequence.clear();
        DICOMRetrievalSequence.clear();
        DICOMMediaRetrievalSequence.clear();
        WADORetrievalSequence.clear();
        XDSRetrievalSequence.clear();
        WADORSRetrievalSequence.clear();
    }
}


OFBool DRTReferencedPatientPhotoSequence::Item::isEmpty()
{
    return TypeOfInstances.isEmpty() &&
           StudyInstanceUID.isEmpty() &&
           SeriesInstanceUID.isEmpty() &&
           ReferencedSOPSequence.isEmpty() &&
           DICOMRetrievalSequence.isEmpty() &&
           DICOMMediaRetrievalSequence.isEmpty() &&
           WADORetrievalSequence.isEmpty() &&
           XDSRetrievalSequence.isEmpty() &&
           WADORSRetrievalSequence.isEmpty();
}


OFBool DRTReferencedPatientPhotoSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTReferencedPatientPhotoSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, TypeOfInstances, "1", "1", "ReferencedPatientPhotoSequence");
        getAndCheckElementFromDataset(item, StudyInstanceUID, "1", "1C", "ReferencedPatientPhotoSequence");
        getAndCheckElementFromDataset(item, SeriesInstanceUID, "1", "1C", "ReferencedPatientPhotoSequence");
        ReferencedSOPSequence.read(item, "1-n", "1", "ReferencedPatientPhotoSequence");
        DICOMRetrievalSequence.read(item, "1-n", "1C", "ReferencedPatientPhotoSequence");
        DICOMMediaRetrievalSequence.read(item, "1-n", "1C", "ReferencedPatientPhotoSequence");
        WADORetrievalSequence.read(item, "1-n", "1C", "ReferencedPatientPhotoSequence");
        XDSRetrievalSequence.read(item, "1-n", "1C", "ReferencedPatientPhotoSequence");
        WADORSRetrievalSequence.read(item, "1-n", "1C", "ReferencedPatientPhotoSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedPatientPhotoSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmCodeString(TypeOfInstances), "1", "1", "ReferencedPatientPhotoSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(StudyInstanceUID), "1", "1C", "ReferencedPatientPhotoSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(SeriesInstanceUID), "1", "1C", "ReferencedPatientPhotoSequence");
        if (result.good()) result = ReferencedSOPSequence.write(item, "1-n", "1", "ReferencedPatientPhotoSequence");
        if (result.good()) result = DICOMRetrievalSequence.write(item, "1-n", "1C", "ReferencedPatientPhotoSequence");
        if (result.good()) result = DICOMMediaRetrievalSequence.write(item, "1-n", "1C", "ReferencedPatientPhotoSequence");
        if (result.good()) result = WADORetrievalSequence.write(item, "1-n", "1C", "ReferencedPatientPhotoSequence");
        if (result.good()) result = XDSRetrievalSequence.write(item, "1-n", "1C", "ReferencedPatientPhotoSequence");
        if (result.good()) result = WADORSRetrievalSequence.write(item, "1-n", "1C", "ReferencedPatientPhotoSequence");
    }
    return result;
}


OFCondition DRTReferencedPatientPhotoSequence::Item::getSeriesInstanceUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SeriesInstanceUID, value, pos);
}


OFCondition DRTReferencedPatientPhotoSequence::Item::getStudyInstanceUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(StudyInstanceUID, value, pos);
}


OFCondition DRTReferencedPatientPhotoSequence::Item::getTypeOfInstances(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TypeOfInstances, value, pos);
}


OFCondition DRTReferencedPatientPhotoSequence::Item::setSeriesInstanceUID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SeriesInstanceUID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedPatientPhotoSequence::Item::setStudyInstanceUID(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedPatientPhotoSequence::Item::setTypeOfInstances(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TypeOfInstances.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTReferencedPatientPhotoSequence::DRTReferencedPatientPhotoSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTReferencedPatientPhotoSequence::DRTReferencedPatientPhotoSequence(const DRTReferencedPatientPhotoSequence &copy)
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


DRTReferencedPatientPhotoSequence &DRTReferencedPatientPhotoSequence::operator=(const DRTReferencedPatientPhotoSequence &copy)
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


DRTReferencedPatientPhotoSequence::~DRTReferencedPatientPhotoSequence()
{
    clear();
}


void DRTReferencedPatientPhotoSequence::clear()
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


OFBool DRTReferencedPatientPhotoSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTReferencedPatientPhotoSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTReferencedPatientPhotoSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTReferencedPatientPhotoSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedPatientPhotoSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedPatientPhotoSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTReferencedPatientPhotoSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTReferencedPatientPhotoSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTReferencedPatientPhotoSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTReferencedPatientPhotoSequence::Item &DRTReferencedPatientPhotoSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTReferencedPatientPhotoSequence::Item &DRTReferencedPatientPhotoSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTReferencedPatientPhotoSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTReferencedPatientPhotoSequence::Item &DRTReferencedPatientPhotoSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTReferencedPatientPhotoSequence::Item &DRTReferencedPatientPhotoSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTReferencedPatientPhotoSequence::Item &DRTReferencedPatientPhotoSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTReferencedPatientPhotoSequence::Item &DRTReferencedPatientPhotoSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTReferencedPatientPhotoSequence::addItem(Item *&item)
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


OFCondition DRTReferencedPatientPhotoSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTReferencedPatientPhotoSequence::removeItem(const size_t pos)
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


OFCondition DRTReferencedPatientPhotoSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ReferencedPatientPhotoSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ReferencedPatientPhotoSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTReferencedPatientPhotoSequence::write(DcmItem &dataset,
                                                     const OFString &card,
                                                     const OFString &type,
                                                     const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ReferencedPatientPhotoSequence);
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
