/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTConceptualVolumeDerivationAlgorithmSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtcvdas.h"


// --- item class ---

DRTConceptualVolumeDerivationAlgorithmSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AlgorithmFamilyCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    AlgorithmName(DCM_AlgorithmName),
    AlgorithmNameCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    AlgorithmParameters(DCM_AlgorithmParameters),
    AlgorithmSource(DCM_AlgorithmSource),
    AlgorithmVersion(DCM_AlgorithmVersion)
{
}


DRTConceptualVolumeDerivationAlgorithmSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AlgorithmFamilyCodeSequence(copy.AlgorithmFamilyCodeSequence),
    AlgorithmName(copy.AlgorithmName),
    AlgorithmNameCodeSequence(copy.AlgorithmNameCodeSequence),
    AlgorithmParameters(copy.AlgorithmParameters),
    AlgorithmSource(copy.AlgorithmSource),
    AlgorithmVersion(copy.AlgorithmVersion)
{
}


DRTConceptualVolumeDerivationAlgorithmSequence::Item::~Item()
{
}


DRTConceptualVolumeDerivationAlgorithmSequence::Item &DRTConceptualVolumeDerivationAlgorithmSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AlgorithmFamilyCodeSequence = copy.AlgorithmFamilyCodeSequence;
        AlgorithmName = copy.AlgorithmName;
        AlgorithmNameCodeSequence = copy.AlgorithmNameCodeSequence;
        AlgorithmParameters = copy.AlgorithmParameters;
        AlgorithmSource = copy.AlgorithmSource;
        AlgorithmVersion = copy.AlgorithmVersion;
    }
    return *this;
}


void DRTConceptualVolumeDerivationAlgorithmSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        AlgorithmFamilyCodeSequence.clear();
        AlgorithmNameCodeSequence.clear();
        AlgorithmName.clear();
        AlgorithmVersion.clear();
        AlgorithmParameters.clear();
        AlgorithmSource.clear();
    }
}


OFBool DRTConceptualVolumeDerivationAlgorithmSequence::Item::isEmpty()
{
    return AlgorithmFamilyCodeSequence.isEmpty() &&
           AlgorithmNameCodeSequence.isEmpty() &&
           AlgorithmName.isEmpty() &&
           AlgorithmVersion.isEmpty() &&
           AlgorithmParameters.isEmpty() &&
           AlgorithmSource.isEmpty();
}


OFBool DRTConceptualVolumeDerivationAlgorithmSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        AlgorithmFamilyCodeSequence.read(item, "1-n", "1", "ConceptualVolumeDerivationAlgorithmSequence");
        AlgorithmNameCodeSequence.read(item, "1-n", "3", "ConceptualVolumeDerivationAlgorithmSequence");
        getAndCheckElementFromDataset(item, AlgorithmName, "1", "1", "ConceptualVolumeDerivationAlgorithmSequence");
        getAndCheckElementFromDataset(item, AlgorithmVersion, "1", "1", "ConceptualVolumeDerivationAlgorithmSequence");
        getAndCheckElementFromDataset(item, AlgorithmParameters, "1", "3", "ConceptualVolumeDerivationAlgorithmSequence");
        getAndCheckElementFromDataset(item, AlgorithmSource, "1", "3", "ConceptualVolumeDerivationAlgorithmSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        if (result.good()) result = AlgorithmFamilyCodeSequence.write(item, "1-n", "1", "ConceptualVolumeDerivationAlgorithmSequence");
        if (result.good()) result = AlgorithmNameCodeSequence.write(item, "1-n", "3", "ConceptualVolumeDerivationAlgorithmSequence");
        addElementToDataset(result, item, new DcmLongString(AlgorithmName), "1", "1", "ConceptualVolumeDerivationAlgorithmSequence");
        addElementToDataset(result, item, new DcmLongString(AlgorithmVersion), "1", "1", "ConceptualVolumeDerivationAlgorithmSequence");
        addElementToDataset(result, item, new DcmLongText(AlgorithmParameters), "1", "3", "ConceptualVolumeDerivationAlgorithmSequence");
        addElementToDataset(result, item, new DcmLongString(AlgorithmSource), "1", "3", "ConceptualVolumeDerivationAlgorithmSequence");
    }
    return result;
}


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::Item::getAlgorithmName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AlgorithmName, value, pos);
}


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::Item::getAlgorithmParameters(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AlgorithmParameters, value, pos);
}


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::Item::getAlgorithmSource(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AlgorithmSource, value, pos);
}


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::Item::getAlgorithmVersion(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AlgorithmVersion, value, pos);
}


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::Item::setAlgorithmName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = AlgorithmName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::Item::setAlgorithmParameters(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = AlgorithmParameters.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::Item::setAlgorithmSource(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = AlgorithmSource.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::Item::setAlgorithmVersion(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = AlgorithmVersion.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTConceptualVolumeDerivationAlgorithmSequence::DRTConceptualVolumeDerivationAlgorithmSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTConceptualVolumeDerivationAlgorithmSequence::DRTConceptualVolumeDerivationAlgorithmSequence(const DRTConceptualVolumeDerivationAlgorithmSequence &copy)
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


DRTConceptualVolumeDerivationAlgorithmSequence &DRTConceptualVolumeDerivationAlgorithmSequence::operator=(const DRTConceptualVolumeDerivationAlgorithmSequence &copy)
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


DRTConceptualVolumeDerivationAlgorithmSequence::~DRTConceptualVolumeDerivationAlgorithmSequence()
{
    clear();
}


void DRTConceptualVolumeDerivationAlgorithmSequence::clear()
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


OFBool DRTConceptualVolumeDerivationAlgorithmSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTConceptualVolumeDerivationAlgorithmSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTConceptualVolumeDerivationAlgorithmSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::gotoNextItem()
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


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTConceptualVolumeDerivationAlgorithmSequence::Item &DRTConceptualVolumeDerivationAlgorithmSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTConceptualVolumeDerivationAlgorithmSequence::Item &DRTConceptualVolumeDerivationAlgorithmSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTConceptualVolumeDerivationAlgorithmSequence::Item &DRTConceptualVolumeDerivationAlgorithmSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTConceptualVolumeDerivationAlgorithmSequence::Item &DRTConceptualVolumeDerivationAlgorithmSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTConceptualVolumeDerivationAlgorithmSequence::Item &DRTConceptualVolumeDerivationAlgorithmSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTConceptualVolumeDerivationAlgorithmSequence::Item &DRTConceptualVolumeDerivationAlgorithmSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::addItem(Item *&item)
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


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::removeItem(const size_t pos)
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


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ConceptualVolumeDerivationAlgorithmSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ConceptualVolumeDerivationAlgorithmSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTConceptualVolumeDerivationAlgorithmSequence::write(DcmItem &dataset,
                                                                  const OFString &card,
                                                                  const OFString &type,
                                                                  const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ConceptualVolumeDerivationAlgorithmSequence);
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
