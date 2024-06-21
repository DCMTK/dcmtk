/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTSourcePixelPlanesCharacteristicsSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtsppcs.h"


// --- item class ---

DRTSourcePixelPlanesCharacteristicsSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    Columns(DCM_Columns),
    ImageOrientationPatient(DCM_ImageOrientationPatient),
    ImagePositionPatient(DCM_ImagePositionPatient),
    NumberOfFrames(DCM_NumberOfFrames),
    PixelSpacing(DCM_PixelSpacing),
    Rows(DCM_Rows),
    SpacingBetweenSlices(DCM_SpacingBetweenSlices)
{
}


DRTSourcePixelPlanesCharacteristicsSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    Columns(copy.Columns),
    ImageOrientationPatient(copy.ImageOrientationPatient),
    ImagePositionPatient(copy.ImagePositionPatient),
    NumberOfFrames(copy.NumberOfFrames),
    PixelSpacing(copy.PixelSpacing),
    Rows(copy.Rows),
    SpacingBetweenSlices(copy.SpacingBetweenSlices)
{
}


DRTSourcePixelPlanesCharacteristicsSequence::Item::~Item()
{
}


DRTSourcePixelPlanesCharacteristicsSequence::Item &DRTSourcePixelPlanesCharacteristicsSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        Columns = copy.Columns;
        ImageOrientationPatient = copy.ImageOrientationPatient;
        ImagePositionPatient = copy.ImagePositionPatient;
        NumberOfFrames = copy.NumberOfFrames;
        PixelSpacing = copy.PixelSpacing;
        Rows = copy.Rows;
        SpacingBetweenSlices = copy.SpacingBetweenSlices;
    }
    return *this;
}


void DRTSourcePixelPlanesCharacteristicsSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        PixelSpacing.clear();
        SpacingBetweenSlices.clear();
        ImageOrientationPatient.clear();
        ImagePositionPatient.clear();
        NumberOfFrames.clear();
        Rows.clear();
        Columns.clear();
    }
}


OFBool DRTSourcePixelPlanesCharacteristicsSequence::Item::isEmpty()
{
    return PixelSpacing.isEmpty() &&
           SpacingBetweenSlices.isEmpty() &&
           ImageOrientationPatient.isEmpty() &&
           ImagePositionPatient.isEmpty() &&
           NumberOfFrames.isEmpty() &&
           Rows.isEmpty() &&
           Columns.isEmpty();
}


OFBool DRTSourcePixelPlanesCharacteristicsSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, PixelSpacing, "2", "1", "SourcePixelPlanesCharacteristicsSequence");
        getAndCheckElementFromDataset(item, SpacingBetweenSlices, "1", "1", "SourcePixelPlanesCharacteristicsSequence");
        getAndCheckElementFromDataset(item, ImageOrientationPatient, "6", "1", "SourcePixelPlanesCharacteristicsSequence");
        getAndCheckElementFromDataset(item, ImagePositionPatient, "3", "1", "SourcePixelPlanesCharacteristicsSequence");
        getAndCheckElementFromDataset(item, NumberOfFrames, "1", "1", "SourcePixelPlanesCharacteristicsSequence");
        getAndCheckElementFromDataset(item, Rows, "1", "1", "SourcePixelPlanesCharacteristicsSequence");
        getAndCheckElementFromDataset(item, Columns, "1", "1", "SourcePixelPlanesCharacteristicsSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmDecimalString(PixelSpacing), "2", "1", "SourcePixelPlanesCharacteristicsSequence");
        addElementToDataset(result, item, new DcmDecimalString(SpacingBetweenSlices), "1", "1", "SourcePixelPlanesCharacteristicsSequence");
        addElementToDataset(result, item, new DcmDecimalString(ImageOrientationPatient), "6", "1", "SourcePixelPlanesCharacteristicsSequence");
        addElementToDataset(result, item, new DcmDecimalString(ImagePositionPatient), "3", "1", "SourcePixelPlanesCharacteristicsSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfFrames), "1", "1", "SourcePixelPlanesCharacteristicsSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(Rows), "1", "1", "SourcePixelPlanesCharacteristicsSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(Columns), "1", "1", "SourcePixelPlanesCharacteristicsSequence");
    }
    return result;
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::getColumns(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, Columns).getUint16(value, pos);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::getImageOrientationPatient(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ImageOrientationPatient, value, pos);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::getImageOrientationPatient(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ImageOrientationPatient).getFloat64(value, pos);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::getImageOrientationPatient(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ImageOrientationPatient).getFloat64Vector(value);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::getImagePositionPatient(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ImagePositionPatient, value, pos);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::getImagePositionPatient(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ImagePositionPatient).getFloat64(value, pos);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::getImagePositionPatient(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ImagePositionPatient).getFloat64Vector(value);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::getNumberOfFrames(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfFrames, value, pos);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::getNumberOfFrames(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfFrames).getSint32(value, pos);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::getPixelSpacing(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PixelSpacing, value, pos);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::getPixelSpacing(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, PixelSpacing).getFloat64(value, pos);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::getPixelSpacing(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, PixelSpacing).getFloat64Vector(value);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::getRows(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, Rows).getUint16(value, pos);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::getSpacingBetweenSlices(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SpacingBetweenSlices, value, pos);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::getSpacingBetweenSlices(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SpacingBetweenSlices).getFloat64(value, pos);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::setColumns(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return Columns.putUint16(value, pos);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::setImageOrientationPatient(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "6") : EC_Normal;
        if (result.good())
            result = ImageOrientationPatient.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::setImagePositionPatient(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "3") : EC_Normal;
        if (result.good())
            result = ImagePositionPatient.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::setNumberOfFrames(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfFrames.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::setPixelSpacing(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "2") : EC_Normal;
        if (result.good())
            result = PixelSpacing.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::setRows(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return Rows.putUint16(value, pos);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::Item::setSpacingBetweenSlices(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SpacingBetweenSlices.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTSourcePixelPlanesCharacteristicsSequence::DRTSourcePixelPlanesCharacteristicsSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTSourcePixelPlanesCharacteristicsSequence::DRTSourcePixelPlanesCharacteristicsSequence(const DRTSourcePixelPlanesCharacteristicsSequence &copy)
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


DRTSourcePixelPlanesCharacteristicsSequence &DRTSourcePixelPlanesCharacteristicsSequence::operator=(const DRTSourcePixelPlanesCharacteristicsSequence &copy)
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


DRTSourcePixelPlanesCharacteristicsSequence::~DRTSourcePixelPlanesCharacteristicsSequence()
{
    clear();
}


void DRTSourcePixelPlanesCharacteristicsSequence::clear()
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


OFBool DRTSourcePixelPlanesCharacteristicsSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTSourcePixelPlanesCharacteristicsSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTSourcePixelPlanesCharacteristicsSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::gotoNextItem()
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


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTSourcePixelPlanesCharacteristicsSequence::Item &DRTSourcePixelPlanesCharacteristicsSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTSourcePixelPlanesCharacteristicsSequence::Item &DRTSourcePixelPlanesCharacteristicsSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTSourcePixelPlanesCharacteristicsSequence::Item &DRTSourcePixelPlanesCharacteristicsSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTSourcePixelPlanesCharacteristicsSequence::Item &DRTSourcePixelPlanesCharacteristicsSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTSourcePixelPlanesCharacteristicsSequence::Item &DRTSourcePixelPlanesCharacteristicsSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTSourcePixelPlanesCharacteristicsSequence::Item &DRTSourcePixelPlanesCharacteristicsSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::addItem(Item *&item)
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


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::removeItem(const size_t pos)
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


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_SourcePixelPlanesCharacteristicsSequence, sequence);
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
            DcmSequenceOfItems element(DCM_SourcePixelPlanesCharacteristicsSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTSourcePixelPlanesCharacteristicsSequence::write(DcmItem &dataset,
                                                               const OFString &card,
                                                               const OFString &type,
                                                               const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_SourcePixelPlanesCharacteristicsSequence);
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
