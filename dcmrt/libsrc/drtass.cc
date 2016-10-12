/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTApplicationSetupSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtass.h"


// --- item class ---

DRTApplicationSetupSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ApplicationSetupManufacturer(DCM_ApplicationSetupManufacturer),
    ApplicationSetupName(DCM_ApplicationSetupName),
    ApplicationSetupNumber(DCM_ApplicationSetupNumber),
    ApplicationSetupType(DCM_ApplicationSetupType),
    BrachyAccessoryDeviceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ChannelSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedReferenceImageSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    TemplateName(DCM_TemplateName),
    TemplateNumber(DCM_TemplateNumber),
    TemplateType(DCM_TemplateType),
    TotalReferenceAirKerma(DCM_TotalReferenceAirKerma)
{
}


DRTApplicationSetupSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ApplicationSetupManufacturer(copy.ApplicationSetupManufacturer),
    ApplicationSetupName(copy.ApplicationSetupName),
    ApplicationSetupNumber(copy.ApplicationSetupNumber),
    ApplicationSetupType(copy.ApplicationSetupType),
    BrachyAccessoryDeviceSequence(copy.BrachyAccessoryDeviceSequence),
    ChannelSequence(copy.ChannelSequence),
    ReferencedReferenceImageSequence(copy.ReferencedReferenceImageSequence),
    TemplateName(copy.TemplateName),
    TemplateNumber(copy.TemplateNumber),
    TemplateType(copy.TemplateType),
    TotalReferenceAirKerma(copy.TotalReferenceAirKerma)
{
}


DRTApplicationSetupSequence::Item::~Item()
{
}


DRTApplicationSetupSequence::Item &DRTApplicationSetupSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ApplicationSetupManufacturer = copy.ApplicationSetupManufacturer;
        ApplicationSetupName = copy.ApplicationSetupName;
        ApplicationSetupNumber = copy.ApplicationSetupNumber;
        ApplicationSetupType = copy.ApplicationSetupType;
        BrachyAccessoryDeviceSequence = copy.BrachyAccessoryDeviceSequence;
        ChannelSequence = copy.ChannelSequence;
        ReferencedReferenceImageSequence = copy.ReferencedReferenceImageSequence;
        TemplateName = copy.TemplateName;
        TemplateNumber = copy.TemplateNumber;
        TemplateType = copy.TemplateType;
        TotalReferenceAirKerma = copy.TotalReferenceAirKerma;
    }
    return *this;
}


void DRTApplicationSetupSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ApplicationSetupType.clear();
        ApplicationSetupNumber.clear();
        ApplicationSetupName.clear();
        ApplicationSetupManufacturer.clear();
        TemplateNumber.clear();
        TemplateType.clear();
        TemplateName.clear();
        ReferencedReferenceImageSequence.clear();
        TotalReferenceAirKerma.clear();
        BrachyAccessoryDeviceSequence.clear();
        ChannelSequence.clear();
    }
}


OFBool DRTApplicationSetupSequence::Item::isEmpty()
{
    return ApplicationSetupType.isEmpty() &&
           ApplicationSetupNumber.isEmpty() &&
           ApplicationSetupName.isEmpty() &&
           ApplicationSetupManufacturer.isEmpty() &&
           TemplateNumber.isEmpty() &&
           TemplateType.isEmpty() &&
           TemplateName.isEmpty() &&
           ReferencedReferenceImageSequence.isEmpty() &&
           TotalReferenceAirKerma.isEmpty() &&
           BrachyAccessoryDeviceSequence.isEmpty() &&
           ChannelSequence.isEmpty();
}


OFBool DRTApplicationSetupSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTApplicationSetupSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ApplicationSetupType, "1", "1", "ApplicationSetupSequence");
        getAndCheckElementFromDataset(item, ApplicationSetupNumber, "1", "1", "ApplicationSetupSequence");
        getAndCheckElementFromDataset(item, ApplicationSetupName, "1", "3", "ApplicationSetupSequence");
        getAndCheckElementFromDataset(item, ApplicationSetupManufacturer, "1", "3", "ApplicationSetupSequence");
        getAndCheckElementFromDataset(item, TemplateNumber, "1", "3", "ApplicationSetupSequence");
        getAndCheckElementFromDataset(item, TemplateType, "1", "3", "ApplicationSetupSequence");
        getAndCheckElementFromDataset(item, TemplateName, "1", "3", "ApplicationSetupSequence");
        ReferencedReferenceImageSequence.read(item, "1-n", "3", "ApplicationSetupSequence");
        getAndCheckElementFromDataset(item, TotalReferenceAirKerma, "1", "1", "ApplicationSetupSequence");
        BrachyAccessoryDeviceSequence.read(item, "1-n", "3", "ApplicationSetupSequence");
        ChannelSequence.read(item, "1-n", "1", "ApplicationSetupSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTApplicationSetupSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmCodeString(ApplicationSetupType), "1", "1", "ApplicationSetupSequence");
        addElementToDataset(result, item, new DcmIntegerString(ApplicationSetupNumber), "1", "1", "ApplicationSetupSequence");
        addElementToDataset(result, item, new DcmLongString(ApplicationSetupName), "1", "3", "ApplicationSetupSequence");
        addElementToDataset(result, item, new DcmLongString(ApplicationSetupManufacturer), "1", "3", "ApplicationSetupSequence");
        addElementToDataset(result, item, new DcmIntegerString(TemplateNumber), "1", "3", "ApplicationSetupSequence");
        addElementToDataset(result, item, new DcmShortString(TemplateType), "1", "3", "ApplicationSetupSequence");
        addElementToDataset(result, item, new DcmLongString(TemplateName), "1", "3", "ApplicationSetupSequence");
        if (result.good()) result = ReferencedReferenceImageSequence.write(item, "1-n", "3", "ApplicationSetupSequence");
        addElementToDataset(result, item, new DcmDecimalString(TotalReferenceAirKerma), "1", "1", "ApplicationSetupSequence");
        if (result.good()) result = BrachyAccessoryDeviceSequence.write(item, "1-n", "3", "ApplicationSetupSequence");
        if (result.good()) result = ChannelSequence.write(item, "1-n", "1", "ApplicationSetupSequence");
    }
    return result;
}


OFCondition DRTApplicationSetupSequence::Item::getApplicationSetupManufacturer(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ApplicationSetupManufacturer, value, pos);
}


OFCondition DRTApplicationSetupSequence::Item::getApplicationSetupName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ApplicationSetupName, value, pos);
}


OFCondition DRTApplicationSetupSequence::Item::getApplicationSetupNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ApplicationSetupNumber, value, pos);
}


OFCondition DRTApplicationSetupSequence::Item::getApplicationSetupNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ApplicationSetupNumber).getSint32(value, pos);
}


OFCondition DRTApplicationSetupSequence::Item::getApplicationSetupType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ApplicationSetupType, value, pos);
}


OFCondition DRTApplicationSetupSequence::Item::getTemplateName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TemplateName, value, pos);
}


OFCondition DRTApplicationSetupSequence::Item::getTemplateNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TemplateNumber, value, pos);
}


OFCondition DRTApplicationSetupSequence::Item::getTemplateNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, TemplateNumber).getSint32(value, pos);
}


OFCondition DRTApplicationSetupSequence::Item::getTemplateType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TemplateType, value, pos);
}


OFCondition DRTApplicationSetupSequence::Item::getTotalReferenceAirKerma(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TotalReferenceAirKerma, value, pos);
}


OFCondition DRTApplicationSetupSequence::Item::getTotalReferenceAirKerma(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TotalReferenceAirKerma).getFloat64(value, pos);
}


OFCondition DRTApplicationSetupSequence::Item::setApplicationSetupManufacturer(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ApplicationSetupManufacturer.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTApplicationSetupSequence::Item::setApplicationSetupName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ApplicationSetupName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTApplicationSetupSequence::Item::setApplicationSetupNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ApplicationSetupNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTApplicationSetupSequence::Item::setApplicationSetupType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ApplicationSetupType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTApplicationSetupSequence::Item::setTemplateName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TemplateName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTApplicationSetupSequence::Item::setTemplateNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TemplateNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTApplicationSetupSequence::Item::setTemplateType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TemplateType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTApplicationSetupSequence::Item::setTotalReferenceAirKerma(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TotalReferenceAirKerma.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTApplicationSetupSequence::DRTApplicationSetupSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTApplicationSetupSequence::DRTApplicationSetupSequence(const DRTApplicationSetupSequence &copy)
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


DRTApplicationSetupSequence &DRTApplicationSetupSequence::operator=(const DRTApplicationSetupSequence &copy)
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


DRTApplicationSetupSequence::~DRTApplicationSetupSequence()
{
    clear();
}


void DRTApplicationSetupSequence::clear()
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


OFBool DRTApplicationSetupSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTApplicationSetupSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTApplicationSetupSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTApplicationSetupSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTApplicationSetupSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTApplicationSetupSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTApplicationSetupSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTApplicationSetupSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTApplicationSetupSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTApplicationSetupSequence::Item &DRTApplicationSetupSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTApplicationSetupSequence::Item &DRTApplicationSetupSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTApplicationSetupSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTApplicationSetupSequence::Item &DRTApplicationSetupSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTApplicationSetupSequence::Item &DRTApplicationSetupSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTApplicationSetupSequence::Item &DRTApplicationSetupSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTApplicationSetupSequence::Item &DRTApplicationSetupSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTApplicationSetupSequence::addItem(Item *&item)
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


OFCondition DRTApplicationSetupSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTApplicationSetupSequence::removeItem(const unsigned long pos)
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


OFCondition DRTApplicationSetupSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ApplicationSetupSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ApplicationSetupSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTApplicationSetupSequence::write(DcmItem &dataset,
                                               const OFString &card,
                                               const OFString &type,
                                               const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ApplicationSetupSequence);
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
