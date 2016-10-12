/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTStructureSetROISequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtssrs.h"


// --- item class ---

DRTStructureSetROISequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    DerivationCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ROIDescription(DCM_ROIDescription),
    ROIGenerationAlgorithm(DCM_ROIGenerationAlgorithm),
    ROIGenerationDescription(DCM_ROIGenerationDescription),
    ROIName(DCM_ROIName),
    ROINumber(DCM_ROINumber),
    ROIVolume(DCM_ROIVolume),
    ReferencedFrameOfReferenceUID(DCM_ReferencedFrameOfReferenceUID)
{
}


DRTStructureSetROISequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    DerivationCodeSequence(copy.DerivationCodeSequence),
    ROIDescription(copy.ROIDescription),
    ROIGenerationAlgorithm(copy.ROIGenerationAlgorithm),
    ROIGenerationDescription(copy.ROIGenerationDescription),
    ROIName(copy.ROIName),
    ROINumber(copy.ROINumber),
    ROIVolume(copy.ROIVolume),
    ReferencedFrameOfReferenceUID(copy.ReferencedFrameOfReferenceUID)
{
}


DRTStructureSetROISequence::Item::~Item()
{
}


DRTStructureSetROISequence::Item &DRTStructureSetROISequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        DerivationCodeSequence = copy.DerivationCodeSequence;
        ROIDescription = copy.ROIDescription;
        ROIGenerationAlgorithm = copy.ROIGenerationAlgorithm;
        ROIGenerationDescription = copy.ROIGenerationDescription;
        ROIName = copy.ROIName;
        ROINumber = copy.ROINumber;
        ROIVolume = copy.ROIVolume;
        ReferencedFrameOfReferenceUID = copy.ReferencedFrameOfReferenceUID;
    }
    return *this;
}


void DRTStructureSetROISequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ROINumber.clear();
        ReferencedFrameOfReferenceUID.clear();
        ROIName.clear();
        ROIDescription.clear();
        ROIVolume.clear();
        ROIGenerationAlgorithm.clear();
        ROIGenerationDescription.clear();
        DerivationCodeSequence.clear();
    }
}


OFBool DRTStructureSetROISequence::Item::isEmpty()
{
    return ROINumber.isEmpty() &&
           ReferencedFrameOfReferenceUID.isEmpty() &&
           ROIName.isEmpty() &&
           ROIDescription.isEmpty() &&
           ROIVolume.isEmpty() &&
           ROIGenerationAlgorithm.isEmpty() &&
           ROIGenerationDescription.isEmpty() &&
           DerivationCodeSequence.isEmpty();
}


OFBool DRTStructureSetROISequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTStructureSetROISequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ROINumber, "1", "1", "StructureSetROISequence");
        getAndCheckElementFromDataset(item, ReferencedFrameOfReferenceUID, "1", "1", "StructureSetROISequence");
        getAndCheckElementFromDataset(item, ROIName, "1", "2", "StructureSetROISequence");
        getAndCheckElementFromDataset(item, ROIDescription, "1", "3", "StructureSetROISequence");
        getAndCheckElementFromDataset(item, ROIVolume, "1", "3", "StructureSetROISequence");
        getAndCheckElementFromDataset(item, ROIGenerationAlgorithm, "1", "2", "StructureSetROISequence");
        getAndCheckElementFromDataset(item, ROIGenerationDescription, "1", "3", "StructureSetROISequence");
        DerivationCodeSequence.read(item, "1-n", "3", "StructureSetROISequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTStructureSetROISequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ROINumber), "1", "1", "StructureSetROISequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(ReferencedFrameOfReferenceUID), "1", "1", "StructureSetROISequence");
        addElementToDataset(result, item, new DcmLongString(ROIName), "1", "2", "StructureSetROISequence");
        addElementToDataset(result, item, new DcmShortText(ROIDescription), "1", "3", "StructureSetROISequence");
        addElementToDataset(result, item, new DcmDecimalString(ROIVolume), "1", "3", "StructureSetROISequence");
        addElementToDataset(result, item, new DcmCodeString(ROIGenerationAlgorithm), "1", "2", "StructureSetROISequence");
        addElementToDataset(result, item, new DcmLongString(ROIGenerationDescription), "1", "3", "StructureSetROISequence");
        if (result.good()) result = DerivationCodeSequence.write(item, "1-n", "3", "StructureSetROISequence");
    }
    return result;
}


OFCondition DRTStructureSetROISequence::Item::getROIDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ROIDescription, value, pos);
}


OFCondition DRTStructureSetROISequence::Item::getROIGenerationAlgorithm(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ROIGenerationAlgorithm, value, pos);
}


OFCondition DRTStructureSetROISequence::Item::getROIGenerationDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ROIGenerationDescription, value, pos);
}


OFCondition DRTStructureSetROISequence::Item::getROIName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ROIName, value, pos);
}


OFCondition DRTStructureSetROISequence::Item::getROINumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ROINumber, value, pos);
}


OFCondition DRTStructureSetROISequence::Item::getROINumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ROINumber).getSint32(value, pos);
}


OFCondition DRTStructureSetROISequence::Item::getROIVolume(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ROIVolume, value, pos);
}


OFCondition DRTStructureSetROISequence::Item::getROIVolume(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ROIVolume).getFloat64(value, pos);
}


OFCondition DRTStructureSetROISequence::Item::getReferencedFrameOfReferenceUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedFrameOfReferenceUID, value, pos);
}


OFCondition DRTStructureSetROISequence::Item::setROIDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = ROIDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTStructureSetROISequence::Item::setROIGenerationAlgorithm(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ROIGenerationAlgorithm.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTStructureSetROISequence::Item::setROIGenerationDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ROIGenerationDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTStructureSetROISequence::Item::setROIName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ROIName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTStructureSetROISequence::Item::setROINumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ROINumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTStructureSetROISequence::Item::setROIVolume(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ROIVolume.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTStructureSetROISequence::Item::setReferencedFrameOfReferenceUID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedFrameOfReferenceUID.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTStructureSetROISequence::DRTStructureSetROISequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTStructureSetROISequence::DRTStructureSetROISequence(const DRTStructureSetROISequence &copy)
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


DRTStructureSetROISequence &DRTStructureSetROISequence::operator=(const DRTStructureSetROISequence &copy)
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


DRTStructureSetROISequence::~DRTStructureSetROISequence()
{
    clear();
}


void DRTStructureSetROISequence::clear()
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


OFBool DRTStructureSetROISequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTStructureSetROISequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTStructureSetROISequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTStructureSetROISequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTStructureSetROISequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTStructureSetROISequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTStructureSetROISequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTStructureSetROISequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTStructureSetROISequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTStructureSetROISequence::Item &DRTStructureSetROISequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTStructureSetROISequence::Item &DRTStructureSetROISequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTStructureSetROISequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTStructureSetROISequence::Item &DRTStructureSetROISequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTStructureSetROISequence::Item &DRTStructureSetROISequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTStructureSetROISequence::Item &DRTStructureSetROISequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTStructureSetROISequence::Item &DRTStructureSetROISequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTStructureSetROISequence::addItem(Item *&item)
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


OFCondition DRTStructureSetROISequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTStructureSetROISequence::removeItem(const unsigned long pos)
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


OFCondition DRTStructureSetROISequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_StructureSetROISequence, sequence);
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
            DcmSequenceOfItems element(DCM_StructureSetROISequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTStructureSetROISequence::write(DcmItem &dataset,
                                              const OFString &card,
                                              const OFString &type,
                                              const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_StructureSetROISequence);
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
