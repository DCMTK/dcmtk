/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTRTROIObservationsSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtrros.h"


// --- item class ---

DRTRTROIObservationsSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    MaterialID(DCM_MaterialID),
    ObservationNumber(DCM_ObservationNumber),
    ROIInterpreter(DCM_ROIInterpreter),
    ROIObservationDescription(DCM_ROIObservationDescription),
    ROIObservationLabel(DCM_ROIObservationLabel),
    ROIPhysicalPropertiesSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RTROIIdentificationCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RTROIInterpretedType(DCM_RTROIInterpretedType),
    RTRelatedROISequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedROINumber(DCM_ReferencedROINumber),
    RelatedRTROIObservationsSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    SegmentedPropertyCategoryCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/)
{
}


DRTRTROIObservationsSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    MaterialID(copy.MaterialID),
    ObservationNumber(copy.ObservationNumber),
    ROIInterpreter(copy.ROIInterpreter),
    ROIObservationDescription(copy.ROIObservationDescription),
    ROIObservationLabel(copy.ROIObservationLabel),
    ROIPhysicalPropertiesSequence(copy.ROIPhysicalPropertiesSequence),
    RTROIIdentificationCodeSequence(copy.RTROIIdentificationCodeSequence),
    RTROIInterpretedType(copy.RTROIInterpretedType),
    RTRelatedROISequence(copy.RTRelatedROISequence),
    ReferencedROINumber(copy.ReferencedROINumber),
    RelatedRTROIObservationsSequence(copy.RelatedRTROIObservationsSequence),
    SegmentedPropertyCategoryCodeSequence(copy.SegmentedPropertyCategoryCodeSequence)
{
}


DRTRTROIObservationsSequence::Item::~Item()
{
}


DRTRTROIObservationsSequence::Item &DRTRTROIObservationsSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        MaterialID = copy.MaterialID;
        ObservationNumber = copy.ObservationNumber;
        ROIInterpreter = copy.ROIInterpreter;
        ROIObservationDescription = copy.ROIObservationDescription;
        ROIObservationLabel = copy.ROIObservationLabel;
        ROIPhysicalPropertiesSequence = copy.ROIPhysicalPropertiesSequence;
        RTROIIdentificationCodeSequence = copy.RTROIIdentificationCodeSequence;
        RTROIInterpretedType = copy.RTROIInterpretedType;
        RTRelatedROISequence = copy.RTRelatedROISequence;
        ReferencedROINumber = copy.ReferencedROINumber;
        RelatedRTROIObservationsSequence = copy.RelatedRTROIObservationsSequence;
        SegmentedPropertyCategoryCodeSequence = copy.SegmentedPropertyCategoryCodeSequence;
    }
    return *this;
}


void DRTRTROIObservationsSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ObservationNumber.clear();
        ReferencedROINumber.clear();
        ROIObservationLabel.clear();
        ROIObservationDescription.clear();
        RTRelatedROISequence.clear();
        SegmentedPropertyCategoryCodeSequence.clear();
        RTROIIdentificationCodeSequence.clear();
        RelatedRTROIObservationsSequence.clear();
        RTROIInterpretedType.clear();
        ROIInterpreter.clear();
        MaterialID.clear();
        ROIPhysicalPropertiesSequence.clear();
    }
}


OFBool DRTRTROIObservationsSequence::Item::isEmpty()
{
    return ObservationNumber.isEmpty() &&
           ReferencedROINumber.isEmpty() &&
           ROIObservationLabel.isEmpty() &&
           ROIObservationDescription.isEmpty() &&
           RTRelatedROISequence.isEmpty() &&
           SegmentedPropertyCategoryCodeSequence.isEmpty() &&
           RTROIIdentificationCodeSequence.isEmpty() &&
           RelatedRTROIObservationsSequence.isEmpty() &&
           RTROIInterpretedType.isEmpty() &&
           ROIInterpreter.isEmpty() &&
           MaterialID.isEmpty() &&
           ROIPhysicalPropertiesSequence.isEmpty();
}


OFBool DRTRTROIObservationsSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTRTROIObservationsSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ObservationNumber, "1", "1", "RTROIObservationsSequence");
        getAndCheckElementFromDataset(item, ReferencedROINumber, "1", "1", "RTROIObservationsSequence");
        getAndCheckElementFromDataset(item, ROIObservationLabel, "1", "3", "RTROIObservationsSequence");
        getAndCheckElementFromDataset(item, ROIObservationDescription, "1", "3", "RTROIObservationsSequence");
        RTRelatedROISequence.read(item, "1-n", "3", "RTROIObservationsSequence");
        SegmentedPropertyCategoryCodeSequence.read(item, "1-n", "3", "RTROIObservationsSequence");
        RTROIIdentificationCodeSequence.read(item, "1-n", "3", "RTROIObservationsSequence");
        RelatedRTROIObservationsSequence.read(item, "1-n", "3", "RTROIObservationsSequence");
        getAndCheckElementFromDataset(item, RTROIInterpretedType, "1", "2", "RTROIObservationsSequence");
        getAndCheckElementFromDataset(item, ROIInterpreter, "1", "2", "RTROIObservationsSequence");
        getAndCheckElementFromDataset(item, MaterialID, "1", "3", "RTROIObservationsSequence");
        ROIPhysicalPropertiesSequence.read(item, "1-n", "3", "RTROIObservationsSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTRTROIObservationsSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ObservationNumber), "1", "1", "RTROIObservationsSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferencedROINumber), "1", "1", "RTROIObservationsSequence");
        addElementToDataset(result, item, new DcmShortString(ROIObservationLabel), "1", "3", "RTROIObservationsSequence");
        addElementToDataset(result, item, new DcmShortText(ROIObservationDescription), "1", "3", "RTROIObservationsSequence");
        if (result.good()) result = RTRelatedROISequence.write(item, "1-n", "3", "RTROIObservationsSequence");
        if (result.good()) result = SegmentedPropertyCategoryCodeSequence.write(item, "1-n", "3", "RTROIObservationsSequence");
        if (result.good()) result = RTROIIdentificationCodeSequence.write(item, "1-n", "3", "RTROIObservationsSequence");
        if (result.good()) result = RelatedRTROIObservationsSequence.write(item, "1-n", "3", "RTROIObservationsSequence");
        addElementToDataset(result, item, new DcmCodeString(RTROIInterpretedType), "1", "2", "RTROIObservationsSequence");
        addElementToDataset(result, item, new DcmPersonName(ROIInterpreter), "1", "2", "RTROIObservationsSequence");
        addElementToDataset(result, item, new DcmShortString(MaterialID), "1", "3", "RTROIObservationsSequence");
        if (result.good()) result = ROIPhysicalPropertiesSequence.write(item, "1-n", "3", "RTROIObservationsSequence");
    }
    return result;
}


OFCondition DRTRTROIObservationsSequence::Item::getMaterialID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MaterialID, value, pos);
}


OFCondition DRTRTROIObservationsSequence::Item::getObservationNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ObservationNumber, value, pos);
}


OFCondition DRTRTROIObservationsSequence::Item::getObservationNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ObservationNumber).getSint32(value, pos);
}


OFCondition DRTRTROIObservationsSequence::Item::getROIInterpreter(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ROIInterpreter, value, pos);
}


OFCondition DRTRTROIObservationsSequence::Item::getROIObservationDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ROIObservationDescription, value, pos);
}


OFCondition DRTRTROIObservationsSequence::Item::getROIObservationLabel(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ROIObservationLabel, value, pos);
}


OFCondition DRTRTROIObservationsSequence::Item::getRTROIInterpretedType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RTROIInterpretedType, value, pos);
}


OFCondition DRTRTROIObservationsSequence::Item::getReferencedROINumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedROINumber, value, pos);
}


OFCondition DRTRTROIObservationsSequence::Item::getReferencedROINumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedROINumber).getSint32(value, pos);
}


OFCondition DRTRTROIObservationsSequence::Item::setMaterialID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = MaterialID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRTROIObservationsSequence::Item::setObservationNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ObservationNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRTROIObservationsSequence::Item::setROIInterpreter(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ROIInterpreter.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRTROIObservationsSequence::Item::setROIObservationDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = ROIObservationDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRTROIObservationsSequence::Item::setROIObservationLabel(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ROIObservationLabel.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRTROIObservationsSequence::Item::setRTROIInterpretedType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RTROIInterpretedType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTRTROIObservationsSequence::Item::setReferencedROINumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedROINumber.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTRTROIObservationsSequence::DRTRTROIObservationsSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTRTROIObservationsSequence::DRTRTROIObservationsSequence(const DRTRTROIObservationsSequence &copy)
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


DRTRTROIObservationsSequence &DRTRTROIObservationsSequence::operator=(const DRTRTROIObservationsSequence &copy)
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


DRTRTROIObservationsSequence::~DRTRTROIObservationsSequence()
{
    clear();
}


void DRTRTROIObservationsSequence::clear()
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


OFBool DRTRTROIObservationsSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTRTROIObservationsSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTRTROIObservationsSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTRTROIObservationsSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTRTROIObservationsSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTRTROIObservationsSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTRTROIObservationsSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTRTROIObservationsSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTRTROIObservationsSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTRTROIObservationsSequence::Item &DRTRTROIObservationsSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTRTROIObservationsSequence::Item &DRTRTROIObservationsSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTRTROIObservationsSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTRTROIObservationsSequence::Item &DRTRTROIObservationsSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTRTROIObservationsSequence::Item &DRTRTROIObservationsSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTRTROIObservationsSequence::Item &DRTRTROIObservationsSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTRTROIObservationsSequence::Item &DRTRTROIObservationsSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTRTROIObservationsSequence::addItem(Item *&item)
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


OFCondition DRTRTROIObservationsSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTRTROIObservationsSequence::removeItem(const size_t pos)
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


OFCondition DRTRTROIObservationsSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_RTROIObservationsSequence, sequence);
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
            DcmSequenceOfItems element(DCM_RTROIObservationsSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTRTROIObservationsSequence::write(DcmItem &dataset,
                                                const OFString &card,
                                                const OFString &type,
                                                const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_RTROIObservationsSequence);
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
