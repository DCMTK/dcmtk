/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTPrivateDataElementCharacteristicsSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtpdecs.h"


// --- item class ---

DRTPrivateDataElementCharacteristicsSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    BlockIdentifyingInformationStatus(DCM_BlockIdentifyingInformationStatus),
    DeidentificationActionSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    NonidentifyingPrivateElements(DCM_NonidentifyingPrivateElements),
    PrivateCreatorReference(DCM_PrivateCreatorReference),
    PrivateDataElementDefinitionSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    PrivateGroupReference(DCM_PrivateGroupReference)
{
}


DRTPrivateDataElementCharacteristicsSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    BlockIdentifyingInformationStatus(copy.BlockIdentifyingInformationStatus),
    DeidentificationActionSequence(copy.DeidentificationActionSequence),
    NonidentifyingPrivateElements(copy.NonidentifyingPrivateElements),
    PrivateCreatorReference(copy.PrivateCreatorReference),
    PrivateDataElementDefinitionSequence(copy.PrivateDataElementDefinitionSequence),
    PrivateGroupReference(copy.PrivateGroupReference)
{
}


DRTPrivateDataElementCharacteristicsSequence::Item::~Item()
{
}


DRTPrivateDataElementCharacteristicsSequence::Item &DRTPrivateDataElementCharacteristicsSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        BlockIdentifyingInformationStatus = copy.BlockIdentifyingInformationStatus;
        DeidentificationActionSequence = copy.DeidentificationActionSequence;
        NonidentifyingPrivateElements = copy.NonidentifyingPrivateElements;
        PrivateCreatorReference = copy.PrivateCreatorReference;
        PrivateDataElementDefinitionSequence = copy.PrivateDataElementDefinitionSequence;
        PrivateGroupReference = copy.PrivateGroupReference;
    }
    return *this;
}


void DRTPrivateDataElementCharacteristicsSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        PrivateGroupReference.clear();
        PrivateCreatorReference.clear();
        PrivateDataElementDefinitionSequence.clear();
        BlockIdentifyingInformationStatus.clear();
        NonidentifyingPrivateElements.clear();
        DeidentificationActionSequence.clear();
    }
}


OFBool DRTPrivateDataElementCharacteristicsSequence::Item::isEmpty()
{
    return PrivateGroupReference.isEmpty() &&
           PrivateCreatorReference.isEmpty() &&
           PrivateDataElementDefinitionSequence.isEmpty() &&
           BlockIdentifyingInformationStatus.isEmpty() &&
           NonidentifyingPrivateElements.isEmpty() &&
           DeidentificationActionSequence.isEmpty();
}


OFBool DRTPrivateDataElementCharacteristicsSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTPrivateDataElementCharacteristicsSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, PrivateGroupReference, "1", "1", "PrivateDataElementCharacteristicsSequence");
        getAndCheckElementFromDataset(item, PrivateCreatorReference, "1", "1", "PrivateDataElementCharacteristicsSequence");
        PrivateDataElementDefinitionSequence.read(item, "1-n", "3", "PrivateDataElementCharacteristicsSequence");
        getAndCheckElementFromDataset(item, BlockIdentifyingInformationStatus, "1", "1", "PrivateDataElementCharacteristicsSequence");
        getAndCheckElementFromDataset(item, NonidentifyingPrivateElements, "1-n", "1C", "PrivateDataElementCharacteristicsSequence");
        DeidentificationActionSequence.read(item, "1-n", "3", "PrivateDataElementCharacteristicsSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTPrivateDataElementCharacteristicsSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmUnsignedShort(PrivateGroupReference), "1", "1", "PrivateDataElementCharacteristicsSequence");
        addElementToDataset(result, item, new DcmLongString(PrivateCreatorReference), "1", "1", "PrivateDataElementCharacteristicsSequence");
        if (result.good()) result = PrivateDataElementDefinitionSequence.write(item, "1-n", "3", "PrivateDataElementCharacteristicsSequence");
        addElementToDataset(result, item, new DcmCodeString(BlockIdentifyingInformationStatus), "1", "1", "PrivateDataElementCharacteristicsSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(NonidentifyingPrivateElements), "1-n", "1C", "PrivateDataElementCharacteristicsSequence");
        if (result.good()) result = DeidentificationActionSequence.write(item, "1-n", "3", "PrivateDataElementCharacteristicsSequence");
    }
    return result;
}


OFCondition DRTPrivateDataElementCharacteristicsSequence::Item::getBlockIdentifyingInformationStatus(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockIdentifyingInformationStatus, value, pos);
}


OFCondition DRTPrivateDataElementCharacteristicsSequence::Item::getNonidentifyingPrivateElements(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, NonidentifyingPrivateElements).getUint16(value, pos);
}


OFCondition DRTPrivateDataElementCharacteristicsSequence::Item::getPrivateCreatorReference(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PrivateCreatorReference, value, pos);
}


OFCondition DRTPrivateDataElementCharacteristicsSequence::Item::getPrivateGroupReference(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, PrivateGroupReference).getUint16(value, pos);
}


OFCondition DRTPrivateDataElementCharacteristicsSequence::Item::setBlockIdentifyingInformationStatus(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BlockIdentifyingInformationStatus.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPrivateDataElementCharacteristicsSequence::Item::setNonidentifyingPrivateElements(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return NonidentifyingPrivateElements.putUint16(value, pos);
}


OFCondition DRTPrivateDataElementCharacteristicsSequence::Item::setPrivateCreatorReference(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PrivateCreatorReference.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPrivateDataElementCharacteristicsSequence::Item::setPrivateGroupReference(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return PrivateGroupReference.putUint16(value, pos);
}


// --- sequence class ---

DRTPrivateDataElementCharacteristicsSequence::DRTPrivateDataElementCharacteristicsSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTPrivateDataElementCharacteristicsSequence::DRTPrivateDataElementCharacteristicsSequence(const DRTPrivateDataElementCharacteristicsSequence &copy)
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


DRTPrivateDataElementCharacteristicsSequence &DRTPrivateDataElementCharacteristicsSequence::operator=(const DRTPrivateDataElementCharacteristicsSequence &copy)
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


DRTPrivateDataElementCharacteristicsSequence::~DRTPrivateDataElementCharacteristicsSequence()
{
    clear();
}


void DRTPrivateDataElementCharacteristicsSequence::clear()
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


OFBool DRTPrivateDataElementCharacteristicsSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTPrivateDataElementCharacteristicsSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTPrivateDataElementCharacteristicsSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTPrivateDataElementCharacteristicsSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTPrivateDataElementCharacteristicsSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTPrivateDataElementCharacteristicsSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTPrivateDataElementCharacteristicsSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTPrivateDataElementCharacteristicsSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTPrivateDataElementCharacteristicsSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTPrivateDataElementCharacteristicsSequence::Item &DRTPrivateDataElementCharacteristicsSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTPrivateDataElementCharacteristicsSequence::Item &DRTPrivateDataElementCharacteristicsSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTPrivateDataElementCharacteristicsSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTPrivateDataElementCharacteristicsSequence::Item &DRTPrivateDataElementCharacteristicsSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTPrivateDataElementCharacteristicsSequence::Item &DRTPrivateDataElementCharacteristicsSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTPrivateDataElementCharacteristicsSequence::Item &DRTPrivateDataElementCharacteristicsSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTPrivateDataElementCharacteristicsSequence::Item &DRTPrivateDataElementCharacteristicsSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTPrivateDataElementCharacteristicsSequence::addItem(Item *&item)
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


OFCondition DRTPrivateDataElementCharacteristicsSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTPrivateDataElementCharacteristicsSequence::removeItem(const size_t pos)
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


OFCondition DRTPrivateDataElementCharacteristicsSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_PrivateDataElementCharacteristicsSequence, sequence);
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
            DcmSequenceOfItems element(DCM_PrivateDataElementCharacteristicsSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTPrivateDataElementCharacteristicsSequence::write(DcmItem &dataset,
                                                                const OFString &card,
                                                                const OFString &type,
                                                                const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_PrivateDataElementCharacteristicsSequence);
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
