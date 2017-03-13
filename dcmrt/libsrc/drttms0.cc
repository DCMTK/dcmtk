/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drttms0.h"


// --- item class ---

DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    DeviceSerialNumber(DCM_DeviceSerialNumber),
    InstitutionAddress(DCM_InstitutionAddress),
    InstitutionName(DCM_InstitutionName),
    InstitutionalDepartmentName(DCM_InstitutionalDepartmentName),
    Manufacturer(DCM_Manufacturer),
    ManufacturerModelName(DCM_ManufacturerModelName),
    TreatmentMachineName(DCM_TreatmentMachineName)
{
}


DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    DeviceSerialNumber(copy.DeviceSerialNumber),
    InstitutionAddress(copy.InstitutionAddress),
    InstitutionName(copy.InstitutionName),
    InstitutionalDepartmentName(copy.InstitutionalDepartmentName),
    Manufacturer(copy.Manufacturer),
    ManufacturerModelName(copy.ManufacturerModelName),
    TreatmentMachineName(copy.TreatmentMachineName)
{
}


DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::~Item()
{
}


DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item &DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        DeviceSerialNumber = copy.DeviceSerialNumber;
        InstitutionAddress = copy.InstitutionAddress;
        InstitutionName = copy.InstitutionName;
        InstitutionalDepartmentName = copy.InstitutionalDepartmentName;
        Manufacturer = copy.Manufacturer;
        ManufacturerModelName = copy.ManufacturerModelName;
        TreatmentMachineName = copy.TreatmentMachineName;
    }
    return *this;
}


void DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        TreatmentMachineName.clear();
        Manufacturer.clear();
        InstitutionName.clear();
        InstitutionAddress.clear();
        InstitutionalDepartmentName.clear();
        ManufacturerModelName.clear();
        DeviceSerialNumber.clear();
    }
}


OFBool DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::isEmpty()
{
    return TreatmentMachineName.isEmpty() &&
           Manufacturer.isEmpty() &&
           InstitutionName.isEmpty() &&
           InstitutionAddress.isEmpty() &&
           InstitutionalDepartmentName.isEmpty() &&
           ManufacturerModelName.isEmpty() &&
           DeviceSerialNumber.isEmpty();
}


OFBool DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, TreatmentMachineName, "1", "2", "TreatmentMachineSequence");
        getAndCheckElementFromDataset(item, Manufacturer, "1", "2", "TreatmentMachineSequence");
        getAndCheckElementFromDataset(item, InstitutionName, "1", "2", "TreatmentMachineSequence");
        getAndCheckElementFromDataset(item, InstitutionAddress, "1", "3", "TreatmentMachineSequence");
        getAndCheckElementFromDataset(item, InstitutionalDepartmentName, "1", "3", "TreatmentMachineSequence");
        getAndCheckElementFromDataset(item, ManufacturerModelName, "1", "2", "TreatmentMachineSequence");
        getAndCheckElementFromDataset(item, DeviceSerialNumber, "1", "2", "TreatmentMachineSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmShortString(TreatmentMachineName), "1", "2", "TreatmentMachineSequence");
        addElementToDataset(result, item, new DcmLongString(Manufacturer), "1", "2", "TreatmentMachineSequence");
        addElementToDataset(result, item, new DcmLongString(InstitutionName), "1", "2", "TreatmentMachineSequence");
        addElementToDataset(result, item, new DcmShortText(InstitutionAddress), "1", "3", "TreatmentMachineSequence");
        addElementToDataset(result, item, new DcmLongString(InstitutionalDepartmentName), "1", "3", "TreatmentMachineSequence");
        addElementToDataset(result, item, new DcmLongString(ManufacturerModelName), "1", "2", "TreatmentMachineSequence");
        addElementToDataset(result, item, new DcmLongString(DeviceSerialNumber), "1", "2", "TreatmentMachineSequence");
    }
    return result;
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::getDeviceSerialNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeviceSerialNumber, value, pos);
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::getInstitutionAddress(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(InstitutionAddress, value, pos);
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::getInstitutionName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(InstitutionName, value, pos);
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::getInstitutionalDepartmentName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(InstitutionalDepartmentName, value, pos);
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::getManufacturer(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(Manufacturer, value, pos);
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::getManufacturerModelName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ManufacturerModelName, value, pos);
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::getTreatmentMachineName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TreatmentMachineName, value, pos);
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::setDeviceSerialNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DeviceSerialNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::setInstitutionAddress(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::setInstitutionName(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::setInstitutionalDepartmentName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = InstitutionalDepartmentName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::setManufacturer(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = Manufacturer.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::setManufacturerModelName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ManufacturerModelName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item::setTreatmentMachineName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TreatmentMachineName.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule(const DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule &copy)
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


DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule &DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::operator=(const DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule &copy)
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


DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::~DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule()
{
    clear();
}


void DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::clear()
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


OFBool DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item &DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item &DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item &DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item &DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item &DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::operator[](const size_t num)
{
    return getItem(num);
}


const DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::Item &DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::addItem(Item *&item)
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


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::removeItem(const size_t pos)
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


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_TreatmentMachineSequence, sequence);
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
            DcmSequenceOfItems element(DCM_TreatmentMachineSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTTreatmentMachineSequenceInRTTreatmentMachineRecordModule::write(DcmItem &dataset,
                                                                               const OFString &card,
                                                                               const OFString &type,
                                                                               const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_TreatmentMachineSequence);
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
