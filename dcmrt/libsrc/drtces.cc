/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTContributingEquipmentSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtces.h"


// --- item class ---

DRTContributingEquipmentSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ContributionDateTime(DCM_ContributionDateTime),
    ContributionDescription(DCM_ContributionDescription),
    DateOfLastCalibration(DCM_DateOfLastCalibration),
    DeviceSerialNumber(DCM_DeviceSerialNumber),
    DeviceUID(DCM_DeviceUID),
    InstitutionAddress(DCM_InstitutionAddress),
    InstitutionName(DCM_InstitutionName),
    InstitutionalDepartmentName(DCM_InstitutionalDepartmentName),
    InstitutionalDepartmentTypeCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    Manufacturer(DCM_Manufacturer),
    ManufacturerModelName(DCM_ManufacturerModelName),
    OperatorIdentificationSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    OperatorsName(DCM_OperatorsName),
    PurposeOfReferenceCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    SoftwareVersions(DCM_SoftwareVersions),
    SpatialResolution(DCM_SpatialResolution),
    StationName(DCM_StationName),
    TimeOfLastCalibration(DCM_TimeOfLastCalibration),
    UDISequence(emptyDefaultItem /*emptyDefaultSequence*/)
{
}


DRTContributingEquipmentSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ContributionDateTime(copy.ContributionDateTime),
    ContributionDescription(copy.ContributionDescription),
    DateOfLastCalibration(copy.DateOfLastCalibration),
    DeviceSerialNumber(copy.DeviceSerialNumber),
    DeviceUID(copy.DeviceUID),
    InstitutionAddress(copy.InstitutionAddress),
    InstitutionName(copy.InstitutionName),
    InstitutionalDepartmentName(copy.InstitutionalDepartmentName),
    InstitutionalDepartmentTypeCodeSequence(copy.InstitutionalDepartmentTypeCodeSequence),
    Manufacturer(copy.Manufacturer),
    ManufacturerModelName(copy.ManufacturerModelName),
    OperatorIdentificationSequence(copy.OperatorIdentificationSequence),
    OperatorsName(copy.OperatorsName),
    PurposeOfReferenceCodeSequence(copy.PurposeOfReferenceCodeSequence),
    SoftwareVersions(copy.SoftwareVersions),
    SpatialResolution(copy.SpatialResolution),
    StationName(copy.StationName),
    TimeOfLastCalibration(copy.TimeOfLastCalibration),
    UDISequence(copy.UDISequence)
{
}


DRTContributingEquipmentSequence::Item::~Item()
{
}


DRTContributingEquipmentSequence::Item &DRTContributingEquipmentSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ContributionDateTime = copy.ContributionDateTime;
        ContributionDescription = copy.ContributionDescription;
        DateOfLastCalibration = copy.DateOfLastCalibration;
        DeviceSerialNumber = copy.DeviceSerialNumber;
        DeviceUID = copy.DeviceUID;
        InstitutionAddress = copy.InstitutionAddress;
        InstitutionName = copy.InstitutionName;
        InstitutionalDepartmentName = copy.InstitutionalDepartmentName;
        InstitutionalDepartmentTypeCodeSequence = copy.InstitutionalDepartmentTypeCodeSequence;
        Manufacturer = copy.Manufacturer;
        ManufacturerModelName = copy.ManufacturerModelName;
        OperatorIdentificationSequence = copy.OperatorIdentificationSequence;
        OperatorsName = copy.OperatorsName;
        PurposeOfReferenceCodeSequence = copy.PurposeOfReferenceCodeSequence;
        SoftwareVersions = copy.SoftwareVersions;
        SpatialResolution = copy.SpatialResolution;
        StationName = copy.StationName;
        TimeOfLastCalibration = copy.TimeOfLastCalibration;
        UDISequence = copy.UDISequence;
    }
    return *this;
}


void DRTContributingEquipmentSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        PurposeOfReferenceCodeSequence.clear();
        Manufacturer.clear();
        InstitutionName.clear();
        InstitutionAddress.clear();
        StationName.clear();
        InstitutionalDepartmentName.clear();
        InstitutionalDepartmentTypeCodeSequence.clear();
        OperatorsName.clear();
        OperatorIdentificationSequence.clear();
        ManufacturerModelName.clear();
        DeviceSerialNumber.clear();
        SoftwareVersions.clear();
        DeviceUID.clear();
        UDISequence.clear();
        SpatialResolution.clear();
        DateOfLastCalibration.clear();
        TimeOfLastCalibration.clear();
        ContributionDateTime.clear();
        ContributionDescription.clear();
    }
}


OFBool DRTContributingEquipmentSequence::Item::isEmpty()
{
    return PurposeOfReferenceCodeSequence.isEmpty() &&
           Manufacturer.isEmpty() &&
           InstitutionName.isEmpty() &&
           InstitutionAddress.isEmpty() &&
           StationName.isEmpty() &&
           InstitutionalDepartmentName.isEmpty() &&
           InstitutionalDepartmentTypeCodeSequence.isEmpty() &&
           OperatorsName.isEmpty() &&
           OperatorIdentificationSequence.isEmpty() &&
           ManufacturerModelName.isEmpty() &&
           DeviceSerialNumber.isEmpty() &&
           SoftwareVersions.isEmpty() &&
           DeviceUID.isEmpty() &&
           UDISequence.isEmpty() &&
           SpatialResolution.isEmpty() &&
           DateOfLastCalibration.isEmpty() &&
           TimeOfLastCalibration.isEmpty() &&
           ContributionDateTime.isEmpty() &&
           ContributionDescription.isEmpty();
}


OFBool DRTContributingEquipmentSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTContributingEquipmentSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        PurposeOfReferenceCodeSequence.read(item, "1-n", "1", "ContributingEquipmentSequence");
        getAndCheckElementFromDataset(item, Manufacturer, "1", "1", "ContributingEquipmentSequence");
        getAndCheckElementFromDataset(item, InstitutionName, "1", "3", "ContributingEquipmentSequence");
        getAndCheckElementFromDataset(item, InstitutionAddress, "1", "3", "ContributingEquipmentSequence");
        getAndCheckElementFromDataset(item, StationName, "1", "3", "ContributingEquipmentSequence");
        getAndCheckElementFromDataset(item, InstitutionalDepartmentName, "1", "3", "ContributingEquipmentSequence");
        InstitutionalDepartmentTypeCodeSequence.read(item, "1-n", "3", "ContributingEquipmentSequence");
        getAndCheckElementFromDataset(item, OperatorsName, "1-n", "3", "ContributingEquipmentSequence");
        OperatorIdentificationSequence.read(item, "1-n", "3", "ContributingEquipmentSequence");
        getAndCheckElementFromDataset(item, ManufacturerModelName, "1", "3", "ContributingEquipmentSequence");
        getAndCheckElementFromDataset(item, DeviceSerialNumber, "1", "3", "ContributingEquipmentSequence");
        getAndCheckElementFromDataset(item, SoftwareVersions, "1-n", "3", "ContributingEquipmentSequence");
        getAndCheckElementFromDataset(item, DeviceUID, "1", "3", "ContributingEquipmentSequence");
        UDISequence.read(item, "1-n", "3", "ContributingEquipmentSequence");
        getAndCheckElementFromDataset(item, SpatialResolution, "1", "3", "ContributingEquipmentSequence");
        getAndCheckElementFromDataset(item, DateOfLastCalibration, "1-n", "3", "ContributingEquipmentSequence");
        getAndCheckElementFromDataset(item, TimeOfLastCalibration, "1-n", "3", "ContributingEquipmentSequence");
        getAndCheckElementFromDataset(item, ContributionDateTime, "1", "3", "ContributingEquipmentSequence");
        getAndCheckElementFromDataset(item, ContributionDescription, "1", "3", "ContributingEquipmentSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTContributingEquipmentSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        if (result.good()) result = PurposeOfReferenceCodeSequence.write(item, "1-n", "1", "ContributingEquipmentSequence");
        addElementToDataset(result, item, new DcmLongString(Manufacturer), "1", "1", "ContributingEquipmentSequence");
        addElementToDataset(result, item, new DcmLongString(InstitutionName), "1", "3", "ContributingEquipmentSequence");
        addElementToDataset(result, item, new DcmShortText(InstitutionAddress), "1", "3", "ContributingEquipmentSequence");
        addElementToDataset(result, item, new DcmShortString(StationName), "1", "3", "ContributingEquipmentSequence");
        addElementToDataset(result, item, new DcmLongString(InstitutionalDepartmentName), "1", "3", "ContributingEquipmentSequence");
        if (result.good()) result = InstitutionalDepartmentTypeCodeSequence.write(item, "1-n", "3", "ContributingEquipmentSequence");
        addElementToDataset(result, item, new DcmPersonName(OperatorsName), "1-n", "3", "ContributingEquipmentSequence");
        if (result.good()) result = OperatorIdentificationSequence.write(item, "1-n", "3", "ContributingEquipmentSequence");
        addElementToDataset(result, item, new DcmLongString(ManufacturerModelName), "1", "3", "ContributingEquipmentSequence");
        addElementToDataset(result, item, new DcmLongString(DeviceSerialNumber), "1", "3", "ContributingEquipmentSequence");
        addElementToDataset(result, item, new DcmLongString(SoftwareVersions), "1-n", "3", "ContributingEquipmentSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(DeviceUID), "1", "3", "ContributingEquipmentSequence");
        if (result.good()) result = UDISequence.write(item, "1-n", "3", "ContributingEquipmentSequence");
        addElementToDataset(result, item, new DcmDecimalString(SpatialResolution), "1", "3", "ContributingEquipmentSequence");
        addElementToDataset(result, item, new DcmDate(DateOfLastCalibration), "1-n", "3", "ContributingEquipmentSequence");
        addElementToDataset(result, item, new DcmTime(TimeOfLastCalibration), "1-n", "3", "ContributingEquipmentSequence");
        addElementToDataset(result, item, new DcmDateTime(ContributionDateTime), "1", "3", "ContributingEquipmentSequence");
        addElementToDataset(result, item, new DcmShortText(ContributionDescription), "1", "3", "ContributingEquipmentSequence");
    }
    return result;
}


OFCondition DRTContributingEquipmentSequence::Item::getContributionDateTime(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContributionDateTime, value, pos);
}


OFCondition DRTContributingEquipmentSequence::Item::getContributionDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContributionDescription, value, pos);
}


OFCondition DRTContributingEquipmentSequence::Item::getDateOfLastCalibration(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DateOfLastCalibration, value, pos);
}


OFCondition DRTContributingEquipmentSequence::Item::getDeviceSerialNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeviceSerialNumber, value, pos);
}


OFCondition DRTContributingEquipmentSequence::Item::getDeviceUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeviceUID, value, pos);
}


OFCondition DRTContributingEquipmentSequence::Item::getInstitutionAddress(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(InstitutionAddress, value, pos);
}


OFCondition DRTContributingEquipmentSequence::Item::getInstitutionName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(InstitutionName, value, pos);
}


OFCondition DRTContributingEquipmentSequence::Item::getInstitutionalDepartmentName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(InstitutionalDepartmentName, value, pos);
}


OFCondition DRTContributingEquipmentSequence::Item::getManufacturer(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(Manufacturer, value, pos);
}


OFCondition DRTContributingEquipmentSequence::Item::getManufacturerModelName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ManufacturerModelName, value, pos);
}


OFCondition DRTContributingEquipmentSequence::Item::getOperatorsName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(OperatorsName, value, pos);
}


OFCondition DRTContributingEquipmentSequence::Item::getSoftwareVersions(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SoftwareVersions, value, pos);
}


OFCondition DRTContributingEquipmentSequence::Item::getSpatialResolution(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SpatialResolution, value, pos);
}


OFCondition DRTContributingEquipmentSequence::Item::getSpatialResolution(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SpatialResolution).getFloat64(value, pos);
}


OFCondition DRTContributingEquipmentSequence::Item::getStationName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(StationName, value, pos);
}


OFCondition DRTContributingEquipmentSequence::Item::getTimeOfLastCalibration(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TimeOfLastCalibration, value, pos);
}


OFCondition DRTContributingEquipmentSequence::Item::setContributionDateTime(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ContributionDateTime.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContributingEquipmentSequence::Item::setContributionDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = ContributionDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContributingEquipmentSequence::Item::setDateOfLastCalibration(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDate::checkStringValue(value, "1-n") : EC_Normal;
        if (result.good())
            result = DateOfLastCalibration.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContributingEquipmentSequence::Item::setDeviceSerialNumber(const OFString &value, const OFBool check)
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


OFCondition DRTContributingEquipmentSequence::Item::setDeviceUID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DeviceUID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContributingEquipmentSequence::Item::setInstitutionAddress(const OFString &value, const OFBool check)
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


OFCondition DRTContributingEquipmentSequence::Item::setInstitutionName(const OFString &value, const OFBool check)
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


OFCondition DRTContributingEquipmentSequence::Item::setInstitutionalDepartmentName(const OFString &value, const OFBool check)
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


OFCondition DRTContributingEquipmentSequence::Item::setManufacturer(const OFString &value, const OFBool check)
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


OFCondition DRTContributingEquipmentSequence::Item::setManufacturerModelName(const OFString &value, const OFBool check)
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


OFCondition DRTContributingEquipmentSequence::Item::setOperatorsName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
        if (result.good())
            result = OperatorsName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContributingEquipmentSequence::Item::setSoftwareVersions(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
        if (result.good())
            result = SoftwareVersions.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContributingEquipmentSequence::Item::setSpatialResolution(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SpatialResolution.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContributingEquipmentSequence::Item::setStationName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = StationName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTContributingEquipmentSequence::Item::setTimeOfLastCalibration(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmTime::checkStringValue(value, "1-n") : EC_Normal;
        if (result.good())
            result = TimeOfLastCalibration.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTContributingEquipmentSequence::DRTContributingEquipmentSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTContributingEquipmentSequence::DRTContributingEquipmentSequence(const DRTContributingEquipmentSequence &copy)
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


DRTContributingEquipmentSequence &DRTContributingEquipmentSequence::operator=(const DRTContributingEquipmentSequence &copy)
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


DRTContributingEquipmentSequence::~DRTContributingEquipmentSequence()
{
    clear();
}


void DRTContributingEquipmentSequence::clear()
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


OFBool DRTContributingEquipmentSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTContributingEquipmentSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTContributingEquipmentSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTContributingEquipmentSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTContributingEquipmentSequence::gotoNextItem()
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


OFCondition DRTContributingEquipmentSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTContributingEquipmentSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTContributingEquipmentSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTContributingEquipmentSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTContributingEquipmentSequence::Item &DRTContributingEquipmentSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTContributingEquipmentSequence::Item &DRTContributingEquipmentSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTContributingEquipmentSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTContributingEquipmentSequence::Item &DRTContributingEquipmentSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTContributingEquipmentSequence::Item &DRTContributingEquipmentSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTContributingEquipmentSequence::Item &DRTContributingEquipmentSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTContributingEquipmentSequence::Item &DRTContributingEquipmentSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTContributingEquipmentSequence::addItem(Item *&item)
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


OFCondition DRTContributingEquipmentSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTContributingEquipmentSequence::removeItem(const size_t pos)
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


OFCondition DRTContributingEquipmentSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ContributingEquipmentSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ContributingEquipmentSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTContributingEquipmentSequence::write(DcmItem &dataset,
                                                    const OFString &card,
                                                    const OFString &type,
                                                    const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ContributingEquipmentSequence);
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
