/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTIssuerOfPatientIDQualifiersSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtipiqs.h"


// --- item class ---

DRTIssuerOfPatientIDQualifiersSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AssigningAgencyOrDepartmentCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    AssigningFacilitySequence(emptyDefaultItem /*emptyDefaultSequence*/),
    AssigningJurisdictionCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    IdentifierTypeCode(DCM_IdentifierTypeCode),
    UniversalEntityID(DCM_UniversalEntityID),
    UniversalEntityIDType(DCM_UniversalEntityIDType)
{
}


DRTIssuerOfPatientIDQualifiersSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AssigningAgencyOrDepartmentCodeSequence(copy.AssigningAgencyOrDepartmentCodeSequence),
    AssigningFacilitySequence(copy.AssigningFacilitySequence),
    AssigningJurisdictionCodeSequence(copy.AssigningJurisdictionCodeSequence),
    IdentifierTypeCode(copy.IdentifierTypeCode),
    UniversalEntityID(copy.UniversalEntityID),
    UniversalEntityIDType(copy.UniversalEntityIDType)
{
}


DRTIssuerOfPatientIDQualifiersSequence::Item::~Item()
{
}


DRTIssuerOfPatientIDQualifiersSequence::Item &DRTIssuerOfPatientIDQualifiersSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AssigningAgencyOrDepartmentCodeSequence = copy.AssigningAgencyOrDepartmentCodeSequence;
        AssigningFacilitySequence = copy.AssigningFacilitySequence;
        AssigningJurisdictionCodeSequence = copy.AssigningJurisdictionCodeSequence;
        IdentifierTypeCode = copy.IdentifierTypeCode;
        UniversalEntityID = copy.UniversalEntityID;
        UniversalEntityIDType = copy.UniversalEntityIDType;
    }
    return *this;
}


void DRTIssuerOfPatientIDQualifiersSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        UniversalEntityID.clear();
        UniversalEntityIDType.clear();
        IdentifierTypeCode.clear();
        AssigningFacilitySequence.clear();
        AssigningJurisdictionCodeSequence.clear();
        AssigningAgencyOrDepartmentCodeSequence.clear();
    }
}


OFBool DRTIssuerOfPatientIDQualifiersSequence::Item::isEmpty()
{
    return UniversalEntityID.isEmpty() &&
           UniversalEntityIDType.isEmpty() &&
           IdentifierTypeCode.isEmpty() &&
           AssigningFacilitySequence.isEmpty() &&
           AssigningJurisdictionCodeSequence.isEmpty() &&
           AssigningAgencyOrDepartmentCodeSequence.isEmpty();
}


OFBool DRTIssuerOfPatientIDQualifiersSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTIssuerOfPatientIDQualifiersSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, UniversalEntityID, "1", "3", "IssuerOfPatientIDQualifiersSequence");
        getAndCheckElementFromDataset(item, UniversalEntityIDType, "1", "1C", "IssuerOfPatientIDQualifiersSequence");
        getAndCheckElementFromDataset(item, IdentifierTypeCode, "1", "3", "IssuerOfPatientIDQualifiersSequence");
        AssigningFacilitySequence.read(item, "1-n", "3", "IssuerOfPatientIDQualifiersSequence");
        AssigningJurisdictionCodeSequence.read(item, "1-n", "3", "IssuerOfPatientIDQualifiersSequence");
        AssigningAgencyOrDepartmentCodeSequence.read(item, "1-n", "3", "IssuerOfPatientIDQualifiersSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIssuerOfPatientIDQualifiersSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmUnlimitedText(UniversalEntityID), "1", "3", "IssuerOfPatientIDQualifiersSequence");
        addElementToDataset(result, item, new DcmCodeString(UniversalEntityIDType), "1", "1C", "IssuerOfPatientIDQualifiersSequence");
        addElementToDataset(result, item, new DcmCodeString(IdentifierTypeCode), "1", "3", "IssuerOfPatientIDQualifiersSequence");
        if (result.good()) result = AssigningFacilitySequence.write(item, "1-n", "3", "IssuerOfPatientIDQualifiersSequence");
        if (result.good()) result = AssigningJurisdictionCodeSequence.write(item, "1-n", "3", "IssuerOfPatientIDQualifiersSequence");
        if (result.good()) result = AssigningAgencyOrDepartmentCodeSequence.write(item, "1-n", "3", "IssuerOfPatientIDQualifiersSequence");
    }
    return result;
}


OFCondition DRTIssuerOfPatientIDQualifiersSequence::Item::getIdentifierTypeCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(IdentifierTypeCode, value, pos);
}


OFCondition DRTIssuerOfPatientIDQualifiersSequence::Item::getUniversalEntityID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(UniversalEntityID, value, pos);
}


OFCondition DRTIssuerOfPatientIDQualifiersSequence::Item::getUniversalEntityIDType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(UniversalEntityIDType, value, pos);
}


OFCondition DRTIssuerOfPatientIDQualifiersSequence::Item::setIdentifierTypeCode(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = IdentifierTypeCode.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIssuerOfPatientIDQualifiersSequence::Item::setUniversalEntityID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUnlimitedText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = UniversalEntityID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIssuerOfPatientIDQualifiersSequence::Item::setUniversalEntityIDType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = UniversalEntityIDType.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTIssuerOfPatientIDQualifiersSequence::DRTIssuerOfPatientIDQualifiersSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTIssuerOfPatientIDQualifiersSequence::DRTIssuerOfPatientIDQualifiersSequence(const DRTIssuerOfPatientIDQualifiersSequence &copy)
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


DRTIssuerOfPatientIDQualifiersSequence &DRTIssuerOfPatientIDQualifiersSequence::operator=(const DRTIssuerOfPatientIDQualifiersSequence &copy)
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


DRTIssuerOfPatientIDQualifiersSequence::~DRTIssuerOfPatientIDQualifiersSequence()
{
    clear();
}


void DRTIssuerOfPatientIDQualifiersSequence::clear()
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


OFBool DRTIssuerOfPatientIDQualifiersSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTIssuerOfPatientIDQualifiersSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTIssuerOfPatientIDQualifiersSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTIssuerOfPatientIDQualifiersSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIssuerOfPatientIDQualifiersSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIssuerOfPatientIDQualifiersSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTIssuerOfPatientIDQualifiersSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTIssuerOfPatientIDQualifiersSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTIssuerOfPatientIDQualifiersSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTIssuerOfPatientIDQualifiersSequence::Item &DRTIssuerOfPatientIDQualifiersSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTIssuerOfPatientIDQualifiersSequence::Item &DRTIssuerOfPatientIDQualifiersSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTIssuerOfPatientIDQualifiersSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTIssuerOfPatientIDQualifiersSequence::Item &DRTIssuerOfPatientIDQualifiersSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTIssuerOfPatientIDQualifiersSequence::Item &DRTIssuerOfPatientIDQualifiersSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTIssuerOfPatientIDQualifiersSequence::Item &DRTIssuerOfPatientIDQualifiersSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTIssuerOfPatientIDQualifiersSequence::Item &DRTIssuerOfPatientIDQualifiersSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTIssuerOfPatientIDQualifiersSequence::addItem(Item *&item)
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


OFCondition DRTIssuerOfPatientIDQualifiersSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTIssuerOfPatientIDQualifiersSequence::removeItem(const size_t pos)
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


OFCondition DRTIssuerOfPatientIDQualifiersSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_IssuerOfPatientIDQualifiersSequence, sequence);
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
            DcmSequenceOfItems element(DCM_IssuerOfPatientIDQualifiersSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTIssuerOfPatientIDQualifiersSequence::write(DcmItem &dataset,
                                                          const OFString &card,
                                                          const OFString &type,
                                                          const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_IssuerOfPatientIDQualifiersSequence);
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
