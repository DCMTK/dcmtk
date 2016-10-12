/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTReferringPhysicianIdentificationSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtrpis.h"


// --- item class ---

DRTReferringPhysicianIdentificationSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    InstitutionAddress(DCM_InstitutionAddress),
    InstitutionCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    InstitutionName(DCM_InstitutionName),
    PersonAddress(DCM_PersonAddress),
    PersonIdentificationCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    PersonTelecomInformation(DCM_PersonTelecomInformation),
    PersonTelephoneNumbers(DCM_PersonTelephoneNumbers)
{
}


DRTReferringPhysicianIdentificationSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    InstitutionAddress(copy.InstitutionAddress),
    InstitutionCodeSequence(copy.InstitutionCodeSequence),
    InstitutionName(copy.InstitutionName),
    PersonAddress(copy.PersonAddress),
    PersonIdentificationCodeSequence(copy.PersonIdentificationCodeSequence),
    PersonTelecomInformation(copy.PersonTelecomInformation),
    PersonTelephoneNumbers(copy.PersonTelephoneNumbers)
{
}


DRTReferringPhysicianIdentificationSequence::Item::~Item()
{
}


DRTReferringPhysicianIdentificationSequence::Item &DRTReferringPhysicianIdentificationSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        InstitutionAddress = copy.InstitutionAddress;
        InstitutionCodeSequence = copy.InstitutionCodeSequence;
        InstitutionName = copy.InstitutionName;
        PersonAddress = copy.PersonAddress;
        PersonIdentificationCodeSequence = copy.PersonIdentificationCodeSequence;
        PersonTelecomInformation = copy.PersonTelecomInformation;
        PersonTelephoneNumbers = copy.PersonTelephoneNumbers;
    }
    return *this;
}


void DRTReferringPhysicianIdentificationSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        PersonIdentificationCodeSequence.clear();
        PersonAddress.clear();
        PersonTelephoneNumbers.clear();
        PersonTelecomInformation.clear();
        InstitutionName.clear();
        InstitutionAddress.clear();
        InstitutionCodeSequence.clear();
    }
}


OFBool DRTReferringPhysicianIdentificationSequence::Item::isEmpty()
{
    return PersonIdentificationCodeSequence.isEmpty() &&
           PersonAddress.isEmpty() &&
           PersonTelephoneNumbers.isEmpty() &&
           PersonTelecomInformation.isEmpty() &&
           InstitutionName.isEmpty() &&
           InstitutionAddress.isEmpty() &&
           InstitutionCodeSequence.isEmpty();
}


OFBool DRTReferringPhysicianIdentificationSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTReferringPhysicianIdentificationSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        PersonIdentificationCodeSequence.read(item, "1-n", "1", "ReferringPhysicianIdentificationSequence");
        getAndCheckElementFromDataset(item, PersonAddress, "1", "3", "ReferringPhysicianIdentificationSequence");
        getAndCheckElementFromDataset(item, PersonTelephoneNumbers, "1-n", "3", "ReferringPhysicianIdentificationSequence");
        getAndCheckElementFromDataset(item, PersonTelecomInformation, "1", "3", "ReferringPhysicianIdentificationSequence");
        getAndCheckElementFromDataset(item, InstitutionName, "1", "1C", "ReferringPhysicianIdentificationSequence");
        getAndCheckElementFromDataset(item, InstitutionAddress, "1", "3", "ReferringPhysicianIdentificationSequence");
        InstitutionCodeSequence.read(item, "1-n", "1C", "ReferringPhysicianIdentificationSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferringPhysicianIdentificationSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        if (result.good()) result = PersonIdentificationCodeSequence.write(item, "1-n", "1", "ReferringPhysicianIdentificationSequence");
        addElementToDataset(result, item, new DcmShortText(PersonAddress), "1", "3", "ReferringPhysicianIdentificationSequence");
        addElementToDataset(result, item, new DcmLongString(PersonTelephoneNumbers), "1-n", "3", "ReferringPhysicianIdentificationSequence");
        addElementToDataset(result, item, new DcmLongText(PersonTelecomInformation), "1", "3", "ReferringPhysicianIdentificationSequence");
        addElementToDataset(result, item, new DcmLongString(InstitutionName), "1", "1C", "ReferringPhysicianIdentificationSequence");
        addElementToDataset(result, item, new DcmShortText(InstitutionAddress), "1", "3", "ReferringPhysicianIdentificationSequence");
        if (result.good()) result = InstitutionCodeSequence.write(item, "1-n", "1C", "ReferringPhysicianIdentificationSequence");
    }
    return result;
}


OFCondition DRTReferringPhysicianIdentificationSequence::Item::getInstitutionAddress(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(InstitutionAddress, value, pos);
}


OFCondition DRTReferringPhysicianIdentificationSequence::Item::getInstitutionName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(InstitutionName, value, pos);
}


OFCondition DRTReferringPhysicianIdentificationSequence::Item::getPersonAddress(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PersonAddress, value, pos);
}


OFCondition DRTReferringPhysicianIdentificationSequence::Item::getPersonTelecomInformation(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PersonTelecomInformation, value, pos);
}


OFCondition DRTReferringPhysicianIdentificationSequence::Item::getPersonTelephoneNumbers(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PersonTelephoneNumbers, value, pos);
}


OFCondition DRTReferringPhysicianIdentificationSequence::Item::setInstitutionAddress(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = InstitutionAddress.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferringPhysicianIdentificationSequence::Item::setInstitutionName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = InstitutionName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferringPhysicianIdentificationSequence::Item::setPersonAddress(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = PersonAddress.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferringPhysicianIdentificationSequence::Item::setPersonTelecomInformation(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = PersonTelecomInformation.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferringPhysicianIdentificationSequence::Item::setPersonTelephoneNumbers(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
        if (result.good())
            result = PersonTelephoneNumbers.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTReferringPhysicianIdentificationSequence::DRTReferringPhysicianIdentificationSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTReferringPhysicianIdentificationSequence::DRTReferringPhysicianIdentificationSequence(const DRTReferringPhysicianIdentificationSequence &copy)
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


DRTReferringPhysicianIdentificationSequence &DRTReferringPhysicianIdentificationSequence::operator=(const DRTReferringPhysicianIdentificationSequence &copy)
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


DRTReferringPhysicianIdentificationSequence::~DRTReferringPhysicianIdentificationSequence()
{
    clear();
}


void DRTReferringPhysicianIdentificationSequence::clear()
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


OFBool DRTReferringPhysicianIdentificationSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTReferringPhysicianIdentificationSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTReferringPhysicianIdentificationSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTReferringPhysicianIdentificationSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferringPhysicianIdentificationSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferringPhysicianIdentificationSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTReferringPhysicianIdentificationSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTReferringPhysicianIdentificationSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTReferringPhysicianIdentificationSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTReferringPhysicianIdentificationSequence::Item &DRTReferringPhysicianIdentificationSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTReferringPhysicianIdentificationSequence::Item &DRTReferringPhysicianIdentificationSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTReferringPhysicianIdentificationSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTReferringPhysicianIdentificationSequence::Item &DRTReferringPhysicianIdentificationSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTReferringPhysicianIdentificationSequence::Item &DRTReferringPhysicianIdentificationSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTReferringPhysicianIdentificationSequence::Item &DRTReferringPhysicianIdentificationSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTReferringPhysicianIdentificationSequence::Item &DRTReferringPhysicianIdentificationSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTReferringPhysicianIdentificationSequence::addItem(Item *&item)
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


OFCondition DRTReferringPhysicianIdentificationSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTReferringPhysicianIdentificationSequence::removeItem(const unsigned long pos)
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


OFCondition DRTReferringPhysicianIdentificationSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ReferringPhysicianIdentificationSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ReferringPhysicianIdentificationSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTReferringPhysicianIdentificationSequence::write(DcmItem &dataset,
                                                               const OFString &card,
                                                               const OFString &type,
                                                               const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ReferringPhysicianIdentificationSequence);
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
