/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTPrivateDataElementDefinitionSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtpdeds.h"


// --- item class ---

DRTPrivateDataElementDefinitionSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    PrivateDataElement(DCM_PrivateDataElement),
    PrivateDataElementDescription(DCM_PrivateDataElementDescription),
    PrivateDataElementEncoding(DCM_PrivateDataElementEncoding),
    PrivateDataElementKeyword(DCM_PrivateDataElementKeyword),
    PrivateDataElementName(DCM_PrivateDataElementName),
    PrivateDataElementNumberOfItems(DCM_PrivateDataElementNumberOfItems),
    PrivateDataElementValueMultiplicity(DCM_PrivateDataElementValueMultiplicity),
    PrivateDataElementValueRepresentation(DCM_PrivateDataElementValueRepresentation),
    RetrieveURI(DCM_RetrieveURI)
{
}


DRTPrivateDataElementDefinitionSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    PrivateDataElement(copy.PrivateDataElement),
    PrivateDataElementDescription(copy.PrivateDataElementDescription),
    PrivateDataElementEncoding(copy.PrivateDataElementEncoding),
    PrivateDataElementKeyword(copy.PrivateDataElementKeyword),
    PrivateDataElementName(copy.PrivateDataElementName),
    PrivateDataElementNumberOfItems(copy.PrivateDataElementNumberOfItems),
    PrivateDataElementValueMultiplicity(copy.PrivateDataElementValueMultiplicity),
    PrivateDataElementValueRepresentation(copy.PrivateDataElementValueRepresentation),
    RetrieveURI(copy.RetrieveURI)
{
}


DRTPrivateDataElementDefinitionSequence::Item::~Item()
{
}


DRTPrivateDataElementDefinitionSequence::Item &DRTPrivateDataElementDefinitionSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        PrivateDataElement = copy.PrivateDataElement;
        PrivateDataElementDescription = copy.PrivateDataElementDescription;
        PrivateDataElementEncoding = copy.PrivateDataElementEncoding;
        PrivateDataElementKeyword = copy.PrivateDataElementKeyword;
        PrivateDataElementName = copy.PrivateDataElementName;
        PrivateDataElementNumberOfItems = copy.PrivateDataElementNumberOfItems;
        PrivateDataElementValueMultiplicity = copy.PrivateDataElementValueMultiplicity;
        PrivateDataElementValueRepresentation = copy.PrivateDataElementValueRepresentation;
        RetrieveURI = copy.RetrieveURI;
    }
    return *this;
}


void DRTPrivateDataElementDefinitionSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        PrivateDataElement.clear();
        PrivateDataElementValueMultiplicity.clear();
        PrivateDataElementValueRepresentation.clear();
        PrivateDataElementNumberOfItems.clear();
        PrivateDataElementKeyword.clear();
        PrivateDataElementName.clear();
        PrivateDataElementDescription.clear();
        PrivateDataElementEncoding.clear();
        RetrieveURI.clear();
    }
}


OFBool DRTPrivateDataElementDefinitionSequence::Item::isEmpty()
{
    return PrivateDataElement.isEmpty() &&
           PrivateDataElementValueMultiplicity.isEmpty() &&
           PrivateDataElementValueRepresentation.isEmpty() &&
           PrivateDataElementNumberOfItems.isEmpty() &&
           PrivateDataElementKeyword.isEmpty() &&
           PrivateDataElementName.isEmpty() &&
           PrivateDataElementDescription.isEmpty() &&
           PrivateDataElementEncoding.isEmpty() &&
           RetrieveURI.isEmpty();
}


OFBool DRTPrivateDataElementDefinitionSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, PrivateDataElement, "1", "1", "PrivateDataElementDefinitionSequence");
        getAndCheckElementFromDataset(item, PrivateDataElementValueMultiplicity, "1-3", "1", "PrivateDataElementDefinitionSequence");
        getAndCheckElementFromDataset(item, PrivateDataElementValueRepresentation, "1", "1", "PrivateDataElementDefinitionSequence");
        getAndCheckElementFromDataset(item, PrivateDataElementNumberOfItems, "1-2", "1C", "PrivateDataElementDefinitionSequence");
        getAndCheckElementFromDataset(item, PrivateDataElementKeyword, "1", "1", "PrivateDataElementDefinitionSequence");
        getAndCheckElementFromDataset(item, PrivateDataElementName, "1", "1", "PrivateDataElementDefinitionSequence");
        getAndCheckElementFromDataset(item, PrivateDataElementDescription, "1", "3", "PrivateDataElementDefinitionSequence");
        getAndCheckElementFromDataset(item, PrivateDataElementEncoding, "1", "3", "PrivateDataElementDefinitionSequence");
        getAndCheckElementFromDataset(item, RetrieveURI, "1", "3", "PrivateDataElementDefinitionSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmUnsignedShort(PrivateDataElement), "1", "1", "PrivateDataElementDefinitionSequence");
        addElementToDataset(result, item, new DcmUnsignedLong(PrivateDataElementValueMultiplicity), "1-3", "1", "PrivateDataElementDefinitionSequence");
        addElementToDataset(result, item, new DcmCodeString(PrivateDataElementValueRepresentation), "1", "1", "PrivateDataElementDefinitionSequence");
        addElementToDataset(result, item, new DcmUnsignedLong(PrivateDataElementNumberOfItems), "1-2", "1C", "PrivateDataElementDefinitionSequence");
        addElementToDataset(result, item, new DcmUnlimitedCharacters(PrivateDataElementKeyword), "1", "1", "PrivateDataElementDefinitionSequence");
        addElementToDataset(result, item, new DcmUnlimitedCharacters(PrivateDataElementName), "1", "1", "PrivateDataElementDefinitionSequence");
        addElementToDataset(result, item, new DcmUnlimitedText(PrivateDataElementDescription), "1", "3", "PrivateDataElementDefinitionSequence");
        addElementToDataset(result, item, new DcmUnlimitedText(PrivateDataElementEncoding), "1", "3", "PrivateDataElementDefinitionSequence");
        addElementToDataset(result, item, new DcmUniversalResourceIdentifierOrLocator(RetrieveURI), "1", "3", "PrivateDataElementDefinitionSequence");
    }
    return result;
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::getPrivateDataElement(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, PrivateDataElement).getUint16(value, pos);
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::getPrivateDataElementDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PrivateDataElementDescription, value, pos);
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::getPrivateDataElementEncoding(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PrivateDataElementEncoding, value, pos);
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::getPrivateDataElementKeyword(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PrivateDataElementKeyword, value, pos);
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::getPrivateDataElementName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PrivateDataElementName, value, pos);
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::getPrivateDataElementNumberOfItems(Uint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedLong &, PrivateDataElementNumberOfItems).getUint32(value, pos);
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::getPrivateDataElementValueMultiplicity(Uint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedLong &, PrivateDataElementValueMultiplicity).getUint32(value, pos);
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::getPrivateDataElementValueRepresentation(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PrivateDataElementValueRepresentation, value, pos);
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::getRetrieveURI(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RetrieveURI, value, pos);
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::setPrivateDataElement(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return PrivateDataElement.putUint16(value, pos);
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::setPrivateDataElementDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUnlimitedText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = PrivateDataElementDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::setPrivateDataElementEncoding(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUnlimitedText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = PrivateDataElementEncoding.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::setPrivateDataElementKeyword(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUnlimitedCharacters::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PrivateDataElementKeyword.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::setPrivateDataElementName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUnlimitedCharacters::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PrivateDataElementName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::setPrivateDataElementNumberOfItems(const Uint32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return PrivateDataElementNumberOfItems.putUint32(value, pos);
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::setPrivateDataElementValueMultiplicity(const Uint32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return PrivateDataElementValueMultiplicity.putUint32(value, pos);
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::setPrivateDataElementValueRepresentation(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PrivateDataElementValueRepresentation.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPrivateDataElementDefinitionSequence::Item::setRetrieveURI(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniversalResourceIdentifierOrLocator::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = RetrieveURI.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTPrivateDataElementDefinitionSequence::DRTPrivateDataElementDefinitionSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTPrivateDataElementDefinitionSequence::DRTPrivateDataElementDefinitionSequence(const DRTPrivateDataElementDefinitionSequence &copy)
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


DRTPrivateDataElementDefinitionSequence &DRTPrivateDataElementDefinitionSequence::operator=(const DRTPrivateDataElementDefinitionSequence &copy)
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


DRTPrivateDataElementDefinitionSequence::~DRTPrivateDataElementDefinitionSequence()
{
    clear();
}


void DRTPrivateDataElementDefinitionSequence::clear()
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


OFBool DRTPrivateDataElementDefinitionSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTPrivateDataElementDefinitionSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTPrivateDataElementDefinitionSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTPrivateDataElementDefinitionSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTPrivateDataElementDefinitionSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTPrivateDataElementDefinitionSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTPrivateDataElementDefinitionSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTPrivateDataElementDefinitionSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTPrivateDataElementDefinitionSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTPrivateDataElementDefinitionSequence::Item &DRTPrivateDataElementDefinitionSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTPrivateDataElementDefinitionSequence::Item &DRTPrivateDataElementDefinitionSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTPrivateDataElementDefinitionSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTPrivateDataElementDefinitionSequence::Item &DRTPrivateDataElementDefinitionSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTPrivateDataElementDefinitionSequence::Item &DRTPrivateDataElementDefinitionSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTPrivateDataElementDefinitionSequence::Item &DRTPrivateDataElementDefinitionSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTPrivateDataElementDefinitionSequence::Item &DRTPrivateDataElementDefinitionSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTPrivateDataElementDefinitionSequence::addItem(Item *&item)
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


OFCondition DRTPrivateDataElementDefinitionSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTPrivateDataElementDefinitionSequence::removeItem(const unsigned long pos)
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


OFCondition DRTPrivateDataElementDefinitionSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_PrivateDataElementDefinitionSequence, sequence);
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
            DcmSequenceOfItems element(DCM_PrivateDataElementDefinitionSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTPrivateDataElementDefinitionSequence::write(DcmItem &dataset,
                                                           const OFString &card,
                                                           const OFString &type,
                                                           const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_PrivateDataElementDefinitionSequence);
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
