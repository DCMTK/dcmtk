/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTConsentForClinicalTrialUseSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtcctus.h"


// --- item class ---

DRTConsentForClinicalTrialUseSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ClinicalTrialProtocolID(DCM_ClinicalTrialProtocolID),
    ConsentForDistributionFlag(DCM_ConsentForDistributionFlag),
    DistributionType(DCM_DistributionType)
{
}


DRTConsentForClinicalTrialUseSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ClinicalTrialProtocolID(copy.ClinicalTrialProtocolID),
    ConsentForDistributionFlag(copy.ConsentForDistributionFlag),
    DistributionType(copy.DistributionType)
{
}


DRTConsentForClinicalTrialUseSequence::Item::~Item()
{
}


DRTConsentForClinicalTrialUseSequence::Item &DRTConsentForClinicalTrialUseSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ClinicalTrialProtocolID = copy.ClinicalTrialProtocolID;
        ConsentForDistributionFlag = copy.ConsentForDistributionFlag;
        DistributionType = copy.DistributionType;
    }
    return *this;
}


void DRTConsentForClinicalTrialUseSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        DistributionType.clear();
        ClinicalTrialProtocolID.clear();
        ConsentForDistributionFlag.clear();
    }
}


OFBool DRTConsentForClinicalTrialUseSequence::Item::isEmpty()
{
    return DistributionType.isEmpty() &&
           ClinicalTrialProtocolID.isEmpty() &&
           ConsentForDistributionFlag.isEmpty();
}


OFBool DRTConsentForClinicalTrialUseSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTConsentForClinicalTrialUseSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, DistributionType, "1", "1C", "ConsentForClinicalTrialUseSequence");
        getAndCheckElementFromDataset(item, ClinicalTrialProtocolID, "1", "1C", "ConsentForClinicalTrialUseSequence");
        getAndCheckElementFromDataset(item, ConsentForDistributionFlag, "1", "1", "ConsentForClinicalTrialUseSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTConsentForClinicalTrialUseSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmCodeString(DistributionType), "1", "1C", "ConsentForClinicalTrialUseSequence");
        addElementToDataset(result, item, new DcmLongString(ClinicalTrialProtocolID), "1", "1C", "ConsentForClinicalTrialUseSequence");
        addElementToDataset(result, item, new DcmCodeString(ConsentForDistributionFlag), "1", "1", "ConsentForClinicalTrialUseSequence");
    }
    return result;
}


OFCondition DRTConsentForClinicalTrialUseSequence::Item::getClinicalTrialProtocolID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ClinicalTrialProtocolID, value, pos);
}


OFCondition DRTConsentForClinicalTrialUseSequence::Item::getConsentForDistributionFlag(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ConsentForDistributionFlag, value, pos);
}


OFCondition DRTConsentForClinicalTrialUseSequence::Item::getDistributionType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DistributionType, value, pos);
}


OFCondition DRTConsentForClinicalTrialUseSequence::Item::setClinicalTrialProtocolID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ClinicalTrialProtocolID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTConsentForClinicalTrialUseSequence::Item::setConsentForDistributionFlag(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ConsentForDistributionFlag.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTConsentForClinicalTrialUseSequence::Item::setDistributionType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DistributionType.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTConsentForClinicalTrialUseSequence::DRTConsentForClinicalTrialUseSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTConsentForClinicalTrialUseSequence::DRTConsentForClinicalTrialUseSequence(const DRTConsentForClinicalTrialUseSequence &copy)
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


DRTConsentForClinicalTrialUseSequence &DRTConsentForClinicalTrialUseSequence::operator=(const DRTConsentForClinicalTrialUseSequence &copy)
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


DRTConsentForClinicalTrialUseSequence::~DRTConsentForClinicalTrialUseSequence()
{
    clear();
}


void DRTConsentForClinicalTrialUseSequence::clear()
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


OFBool DRTConsentForClinicalTrialUseSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTConsentForClinicalTrialUseSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTConsentForClinicalTrialUseSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTConsentForClinicalTrialUseSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTConsentForClinicalTrialUseSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTConsentForClinicalTrialUseSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTConsentForClinicalTrialUseSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTConsentForClinicalTrialUseSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTConsentForClinicalTrialUseSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTConsentForClinicalTrialUseSequence::Item &DRTConsentForClinicalTrialUseSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTConsentForClinicalTrialUseSequence::Item &DRTConsentForClinicalTrialUseSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTConsentForClinicalTrialUseSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTConsentForClinicalTrialUseSequence::Item &DRTConsentForClinicalTrialUseSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTConsentForClinicalTrialUseSequence::Item &DRTConsentForClinicalTrialUseSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTConsentForClinicalTrialUseSequence::Item &DRTConsentForClinicalTrialUseSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTConsentForClinicalTrialUseSequence::Item &DRTConsentForClinicalTrialUseSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTConsentForClinicalTrialUseSequence::addItem(Item *&item)
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


OFCondition DRTConsentForClinicalTrialUseSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTConsentForClinicalTrialUseSequence::removeItem(const unsigned long pos)
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


OFCondition DRTConsentForClinicalTrialUseSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ConsentForClinicalTrialUseSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ConsentForClinicalTrialUseSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTConsentForClinicalTrialUseSequence::write(DcmItem &dataset,
                                                         const OFString &card,
                                                         const OFString &type,
                                                         const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ConsentForClinicalTrialUseSequence);
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
