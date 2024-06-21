/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTDerivationConceptualVolumeSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtdcvs.h"


// --- item class ---

DRTDerivationConceptualVolumeSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ConceptualVolumeDerivationAlgorithmSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    DerivationDescription(DCM_DerivationDescription),
    SourceConceptualVolumeSequence(emptyDefaultItem /*emptyDefaultSequence*/)
{
}


DRTDerivationConceptualVolumeSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ConceptualVolumeDerivationAlgorithmSequence(copy.ConceptualVolumeDerivationAlgorithmSequence),
    DerivationDescription(copy.DerivationDescription),
    SourceConceptualVolumeSequence(copy.SourceConceptualVolumeSequence)
{
}


DRTDerivationConceptualVolumeSequence::Item::~Item()
{
}


DRTDerivationConceptualVolumeSequence::Item &DRTDerivationConceptualVolumeSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ConceptualVolumeDerivationAlgorithmSequence = copy.ConceptualVolumeDerivationAlgorithmSequence;
        DerivationDescription = copy.DerivationDescription;
        SourceConceptualVolumeSequence = copy.SourceConceptualVolumeSequence;
    }
    return *this;
}


void DRTDerivationConceptualVolumeSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        DerivationDescription.clear();
        SourceConceptualVolumeSequence.clear();
        ConceptualVolumeDerivationAlgorithmSequence.clear();
    }
}


OFBool DRTDerivationConceptualVolumeSequence::Item::isEmpty()
{
    return DerivationDescription.isEmpty() &&
           SourceConceptualVolumeSequence.isEmpty() &&
           ConceptualVolumeDerivationAlgorithmSequence.isEmpty();
}


OFBool DRTDerivationConceptualVolumeSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTDerivationConceptualVolumeSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, DerivationDescription, "1", "3", "DerivationConceptualVolumeSequence");
        SourceConceptualVolumeSequence.read(item, "1-n", "1", "DerivationConceptualVolumeSequence");
        ConceptualVolumeDerivationAlgorithmSequence.read(item, "1-n", "3", "DerivationConceptualVolumeSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTDerivationConceptualVolumeSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmShortText(DerivationDescription), "1", "3", "DerivationConceptualVolumeSequence");
        if (result.good()) result = SourceConceptualVolumeSequence.write(item, "1-n", "1", "DerivationConceptualVolumeSequence");
        if (result.good()) result = ConceptualVolumeDerivationAlgorithmSequence.write(item, "1-n", "3", "DerivationConceptualVolumeSequence");
    }
    return result;
}


OFCondition DRTDerivationConceptualVolumeSequence::Item::getDerivationDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DerivationDescription, value, pos);
}


OFCondition DRTDerivationConceptualVolumeSequence::Item::setDerivationDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = DerivationDescription.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTDerivationConceptualVolumeSequence::DRTDerivationConceptualVolumeSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTDerivationConceptualVolumeSequence::DRTDerivationConceptualVolumeSequence(const DRTDerivationConceptualVolumeSequence &copy)
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


DRTDerivationConceptualVolumeSequence &DRTDerivationConceptualVolumeSequence::operator=(const DRTDerivationConceptualVolumeSequence &copy)
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


DRTDerivationConceptualVolumeSequence::~DRTDerivationConceptualVolumeSequence()
{
    clear();
}


void DRTDerivationConceptualVolumeSequence::clear()
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


OFBool DRTDerivationConceptualVolumeSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTDerivationConceptualVolumeSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTDerivationConceptualVolumeSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTDerivationConceptualVolumeSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTDerivationConceptualVolumeSequence::gotoNextItem()
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


OFCondition DRTDerivationConceptualVolumeSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTDerivationConceptualVolumeSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTDerivationConceptualVolumeSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTDerivationConceptualVolumeSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTDerivationConceptualVolumeSequence::Item &DRTDerivationConceptualVolumeSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTDerivationConceptualVolumeSequence::Item &DRTDerivationConceptualVolumeSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTDerivationConceptualVolumeSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTDerivationConceptualVolumeSequence::Item &DRTDerivationConceptualVolumeSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTDerivationConceptualVolumeSequence::Item &DRTDerivationConceptualVolumeSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTDerivationConceptualVolumeSequence::Item &DRTDerivationConceptualVolumeSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTDerivationConceptualVolumeSequence::Item &DRTDerivationConceptualVolumeSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTDerivationConceptualVolumeSequence::addItem(Item *&item)
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


OFCondition DRTDerivationConceptualVolumeSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTDerivationConceptualVolumeSequence::removeItem(const size_t pos)
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


OFCondition DRTDerivationConceptualVolumeSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_DerivationConceptualVolumeSequence, sequence);
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
            DcmSequenceOfItems element(DCM_DerivationConceptualVolumeSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTDerivationConceptualVolumeSequence::write(DcmItem &dataset,
                                                         const OFString &card,
                                                         const OFString &type,
                                                         const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_DerivationConceptualVolumeSequence);
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
