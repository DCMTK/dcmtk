/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTEnhancedRTBeamLimitingDeviceSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drteblds.h"


// --- item class ---

DRTEnhancedRTBeamLimitingDeviceSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    BeamModifierOrientationAngle(DCM_BeamModifierOrientationAngle),
    DeviceAlternateIdentifier(DCM_DeviceAlternateIdentifier),
    DeviceAlternateIdentifierFormat(DCM_DeviceAlternateIdentifierFormat),
    DeviceAlternateIdentifierType(DCM_DeviceAlternateIdentifierType),
    DeviceIndex(DCM_DeviceIndex),
    DeviceLabel(DCM_DeviceLabel),
    DeviceSerialNumber(DCM_DeviceSerialNumber),
    DeviceTypeCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    FixedRTBeamDelimiterDeviceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    LongDeviceDescription(DCM_LongDeviceDescription),
    Manufacturer(DCM_Manufacturer),
    ManufacturerDeviceIdentifier(DCM_ManufacturerDeviceIdentifier),
    ManufacturerModelName(DCM_ManufacturerModelName),
    ManufacturerModelVersion(DCM_ManufacturerModelVersion),
    ParallelRTBeamDelimiterDeviceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RTAccessoryDeviceSlotID(DCM_RTAccessoryDeviceSlotID),
    RTAccessoryHolderSlotID(DCM_RTAccessoryHolderSlotID),
    RTAccessorySlotDistance(DCM_RTAccessorySlotDistance),
    RTBeamLimitingDeviceDistalDistance(DCM_RTBeamLimitingDeviceDistalDistance),
    RTBeamLimitingDeviceProximalDistance(DCM_RTBeamLimitingDeviceProximalDistance),
    ReferencedDefinedDeviceIndex(DCM_ReferencedDefinedDeviceIndex),
    ReferencedRTAccessoryHolderDeviceIndex(DCM_ReferencedRTAccessoryHolderDeviceIndex),
    SoftwareVersions(DCM_SoftwareVersions),
    UDISequence(emptyDefaultItem /*emptyDefaultSequence*/)
{
}


DRTEnhancedRTBeamLimitingDeviceSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    BeamModifierOrientationAngle(copy.BeamModifierOrientationAngle),
    DeviceAlternateIdentifier(copy.DeviceAlternateIdentifier),
    DeviceAlternateIdentifierFormat(copy.DeviceAlternateIdentifierFormat),
    DeviceAlternateIdentifierType(copy.DeviceAlternateIdentifierType),
    DeviceIndex(copy.DeviceIndex),
    DeviceLabel(copy.DeviceLabel),
    DeviceSerialNumber(copy.DeviceSerialNumber),
    DeviceTypeCodeSequence(copy.DeviceTypeCodeSequence),
    FixedRTBeamDelimiterDeviceSequence(copy.FixedRTBeamDelimiterDeviceSequence),
    LongDeviceDescription(copy.LongDeviceDescription),
    Manufacturer(copy.Manufacturer),
    ManufacturerDeviceIdentifier(copy.ManufacturerDeviceIdentifier),
    ManufacturerModelName(copy.ManufacturerModelName),
    ManufacturerModelVersion(copy.ManufacturerModelVersion),
    ParallelRTBeamDelimiterDeviceSequence(copy.ParallelRTBeamDelimiterDeviceSequence),
    RTAccessoryDeviceSlotID(copy.RTAccessoryDeviceSlotID),
    RTAccessoryHolderSlotID(copy.RTAccessoryHolderSlotID),
    RTAccessorySlotDistance(copy.RTAccessorySlotDistance),
    RTBeamLimitingDeviceDistalDistance(copy.RTBeamLimitingDeviceDistalDistance),
    RTBeamLimitingDeviceProximalDistance(copy.RTBeamLimitingDeviceProximalDistance),
    ReferencedDefinedDeviceIndex(copy.ReferencedDefinedDeviceIndex),
    ReferencedRTAccessoryHolderDeviceIndex(copy.ReferencedRTAccessoryHolderDeviceIndex),
    SoftwareVersions(copy.SoftwareVersions),
    UDISequence(copy.UDISequence)
{
}


DRTEnhancedRTBeamLimitingDeviceSequence::Item::~Item()
{
}


DRTEnhancedRTBeamLimitingDeviceSequence::Item &DRTEnhancedRTBeamLimitingDeviceSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        BeamModifierOrientationAngle = copy.BeamModifierOrientationAngle;
        DeviceAlternateIdentifier = copy.DeviceAlternateIdentifier;
        DeviceAlternateIdentifierFormat = copy.DeviceAlternateIdentifierFormat;
        DeviceAlternateIdentifierType = copy.DeviceAlternateIdentifierType;
        DeviceIndex = copy.DeviceIndex;
        DeviceLabel = copy.DeviceLabel;
        DeviceSerialNumber = copy.DeviceSerialNumber;
        DeviceTypeCodeSequence = copy.DeviceTypeCodeSequence;
        FixedRTBeamDelimiterDeviceSequence = copy.FixedRTBeamDelimiterDeviceSequence;
        LongDeviceDescription = copy.LongDeviceDescription;
        Manufacturer = copy.Manufacturer;
        ManufacturerDeviceIdentifier = copy.ManufacturerDeviceIdentifier;
        ManufacturerModelName = copy.ManufacturerModelName;
        ManufacturerModelVersion = copy.ManufacturerModelVersion;
        ParallelRTBeamDelimiterDeviceSequence = copy.ParallelRTBeamDelimiterDeviceSequence;
        RTAccessoryDeviceSlotID = copy.RTAccessoryDeviceSlotID;
        RTAccessoryHolderSlotID = copy.RTAccessoryHolderSlotID;
        RTAccessorySlotDistance = copy.RTAccessorySlotDistance;
        RTBeamLimitingDeviceDistalDistance = copy.RTBeamLimitingDeviceDistalDistance;
        RTBeamLimitingDeviceProximalDistance = copy.RTBeamLimitingDeviceProximalDistance;
        ReferencedDefinedDeviceIndex = copy.ReferencedDefinedDeviceIndex;
        ReferencedRTAccessoryHolderDeviceIndex = copy.ReferencedRTAccessoryHolderDeviceIndex;
        SoftwareVersions = copy.SoftwareVersions;
        UDISequence = copy.UDISequence;
    }
    return *this;
}


void DRTEnhancedRTBeamLimitingDeviceSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        DeviceIndex.clear();
        ReferencedDefinedDeviceIndex.clear();
        Manufacturer.clear();
        ManufacturerModelName.clear();
        ManufacturerModelVersion.clear();
        DeviceTypeCodeSequence.clear();
        DeviceLabel.clear();
        LongDeviceDescription.clear();
        DeviceSerialNumber.clear();
        SoftwareVersions.clear();
        UDISequence.clear();
        ManufacturerDeviceIdentifier.clear();
        DeviceAlternateIdentifier.clear();
        DeviceAlternateIdentifierType.clear();
        DeviceAlternateIdentifierFormat.clear();
        RTAccessoryDeviceSlotID.clear();
        RTAccessorySlotDistance.clear();
        ReferencedRTAccessoryHolderDeviceIndex.clear();
        RTAccessoryHolderSlotID.clear();
        BeamModifierOrientationAngle.clear();
        RTBeamLimitingDeviceProximalDistance.clear();
        RTBeamLimitingDeviceDistalDistance.clear();
        ParallelRTBeamDelimiterDeviceSequence.clear();
        FixedRTBeamDelimiterDeviceSequence.clear();
    }
}


OFBool DRTEnhancedRTBeamLimitingDeviceSequence::Item::isEmpty()
{
    return DeviceIndex.isEmpty() &&
           ReferencedDefinedDeviceIndex.isEmpty() &&
           Manufacturer.isEmpty() &&
           ManufacturerModelName.isEmpty() &&
           ManufacturerModelVersion.isEmpty() &&
           DeviceTypeCodeSequence.isEmpty() &&
           DeviceLabel.isEmpty() &&
           LongDeviceDescription.isEmpty() &&
           DeviceSerialNumber.isEmpty() &&
           SoftwareVersions.isEmpty() &&
           UDISequence.isEmpty() &&
           ManufacturerDeviceIdentifier.isEmpty() &&
           DeviceAlternateIdentifier.isEmpty() &&
           DeviceAlternateIdentifierType.isEmpty() &&
           DeviceAlternateIdentifierFormat.isEmpty() &&
           RTAccessoryDeviceSlotID.isEmpty() &&
           RTAccessorySlotDistance.isEmpty() &&
           ReferencedRTAccessoryHolderDeviceIndex.isEmpty() &&
           RTAccessoryHolderSlotID.isEmpty() &&
           BeamModifierOrientationAngle.isEmpty() &&
           RTBeamLimitingDeviceProximalDistance.isEmpty() &&
           RTBeamLimitingDeviceDistalDistance.isEmpty() &&
           ParallelRTBeamDelimiterDeviceSequence.isEmpty() &&
           FixedRTBeamDelimiterDeviceSequence.isEmpty();
}


OFBool DRTEnhancedRTBeamLimitingDeviceSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, DeviceIndex, "1", "1", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, ReferencedDefinedDeviceIndex, "1", "1C", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, Manufacturer, "1", "2", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, ManufacturerModelName, "1", "2", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, ManufacturerModelVersion, "1", "2", "EnhancedRTBeamLimitingDeviceSequence");
        DeviceTypeCodeSequence.read(item, "1-n", "1", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, DeviceLabel, "1", "1", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, LongDeviceDescription, "1", "3", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, DeviceSerialNumber, "1", "2", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, SoftwareVersions, "1-n", "2", "EnhancedRTBeamLimitingDeviceSequence");
        UDISequence.read(item, "1-n", "3", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, ManufacturerDeviceIdentifier, "1", "2", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, DeviceAlternateIdentifier, "1", "2", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, DeviceAlternateIdentifierType, "1", "1C", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, DeviceAlternateIdentifierFormat, "1", "1C", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, RTAccessoryDeviceSlotID, "1", "2C", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, RTAccessorySlotDistance, "1", "2C", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, ReferencedRTAccessoryHolderDeviceIndex, "1", "2C", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, RTAccessoryHolderSlotID, "1", "2C", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, BeamModifierOrientationAngle, "1", "1", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, RTBeamLimitingDeviceProximalDistance, "1", "2", "EnhancedRTBeamLimitingDeviceSequence");
        getAndCheckElementFromDataset(item, RTBeamLimitingDeviceDistalDistance, "1", "2", "EnhancedRTBeamLimitingDeviceSequence");
        ParallelRTBeamDelimiterDeviceSequence.read(item, "1-n", "1C", "EnhancedRTBeamLimitingDeviceSequence");
        FixedRTBeamDelimiterDeviceSequence.read(item, "1-n", "1C", "EnhancedRTBeamLimitingDeviceSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmUnsignedShort(DeviceIndex), "1", "1", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(ReferencedDefinedDeviceIndex), "1", "1C", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmLongString(Manufacturer), "1", "2", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmLongString(ManufacturerModelName), "1", "2", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmLongString(ManufacturerModelVersion), "1", "2", "EnhancedRTBeamLimitingDeviceSequence");
        if (result.good()) result = DeviceTypeCodeSequence.write(item, "1-n", "1", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmLongString(DeviceLabel), "1", "1", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmShortText(LongDeviceDescription), "1", "3", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmLongString(DeviceSerialNumber), "1", "2", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmLongString(SoftwareVersions), "1-n", "2", "EnhancedRTBeamLimitingDeviceSequence");
        if (result.good()) result = UDISequence.write(item, "1-n", "3", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmShortText(ManufacturerDeviceIdentifier), "1", "2", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmUnlimitedCharacters(DeviceAlternateIdentifier), "1", "2", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmCodeString(DeviceAlternateIdentifierType), "1", "1C", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmLongText(DeviceAlternateIdentifierFormat), "1", "1C", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmLongString(RTAccessoryDeviceSlotID), "1", "2C", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(RTAccessorySlotDistance), "1", "2C", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(ReferencedRTAccessoryHolderDeviceIndex), "1", "2C", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmLongString(RTAccessoryHolderSlotID), "1", "2C", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(BeamModifierOrientationAngle), "1", "1", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(RTBeamLimitingDeviceProximalDistance), "1", "2", "EnhancedRTBeamLimitingDeviceSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(RTBeamLimitingDeviceDistalDistance), "1", "2", "EnhancedRTBeamLimitingDeviceSequence");
        if (result.good()) result = ParallelRTBeamDelimiterDeviceSequence.write(item, "1-n", "1C", "EnhancedRTBeamLimitingDeviceSequence");
        if (result.good()) result = FixedRTBeamDelimiterDeviceSequence.write(item, "1-n", "1C", "EnhancedRTBeamLimitingDeviceSequence");
    }
    return result;
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getBeamModifierOrientationAngle(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, BeamModifierOrientationAngle).getFloat64(value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getDeviceAlternateIdentifier(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeviceAlternateIdentifier, value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getDeviceAlternateIdentifierFormat(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeviceAlternateIdentifierFormat, value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getDeviceAlternateIdentifierType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeviceAlternateIdentifierType, value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getDeviceIndex(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, DeviceIndex).getUint16(value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getDeviceLabel(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeviceLabel, value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getDeviceSerialNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeviceSerialNumber, value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getLongDeviceDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(LongDeviceDescription, value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getManufacturer(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(Manufacturer, value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getManufacturerDeviceIdentifier(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ManufacturerDeviceIdentifier, value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getManufacturerModelName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ManufacturerModelName, value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getManufacturerModelVersion(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ManufacturerModelVersion, value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getRTAccessoryDeviceSlotID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RTAccessoryDeviceSlotID, value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getRTAccessoryHolderSlotID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RTAccessoryHolderSlotID, value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getRTAccessorySlotDistance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, RTAccessorySlotDistance).getFloat64(value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getRTBeamLimitingDeviceDistalDistance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, RTBeamLimitingDeviceDistalDistance).getFloat64(value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getRTBeamLimitingDeviceProximalDistance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, RTBeamLimitingDeviceProximalDistance).getFloat64(value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getReferencedDefinedDeviceIndex(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, ReferencedDefinedDeviceIndex).getUint16(value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getReferencedRTAccessoryHolderDeviceIndex(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, ReferencedRTAccessoryHolderDeviceIndex).getUint16(value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::getSoftwareVersions(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SoftwareVersions, value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setBeamModifierOrientationAngle(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return BeamModifierOrientationAngle.putFloat64(value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setDeviceAlternateIdentifier(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUnlimitedCharacters::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DeviceAlternateIdentifier.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setDeviceAlternateIdentifierFormat(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = DeviceAlternateIdentifierFormat.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setDeviceAlternateIdentifierType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DeviceAlternateIdentifierType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setDeviceIndex(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return DeviceIndex.putUint16(value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setDeviceLabel(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DeviceLabel.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setDeviceSerialNumber(const OFString &value, const OFBool check)
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


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setLongDeviceDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = LongDeviceDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setManufacturer(const OFString &value, const OFBool check)
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


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setManufacturerDeviceIdentifier(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = ManufacturerDeviceIdentifier.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setManufacturerModelName(const OFString &value, const OFBool check)
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


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setManufacturerModelVersion(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ManufacturerModelVersion.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setRTAccessoryDeviceSlotID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RTAccessoryDeviceSlotID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setRTAccessoryHolderSlotID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RTAccessoryHolderSlotID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setRTAccessorySlotDistance(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RTAccessorySlotDistance.putFloat64(value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setRTBeamLimitingDeviceDistalDistance(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RTBeamLimitingDeviceDistalDistance.putFloat64(value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setRTBeamLimitingDeviceProximalDistance(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RTBeamLimitingDeviceProximalDistance.putFloat64(value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setReferencedDefinedDeviceIndex(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ReferencedDefinedDeviceIndex.putUint16(value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setReferencedRTAccessoryHolderDeviceIndex(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ReferencedRTAccessoryHolderDeviceIndex.putUint16(value, pos);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::Item::setSoftwareVersions(const OFString &value, const OFBool check)
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


// --- sequence class ---

DRTEnhancedRTBeamLimitingDeviceSequence::DRTEnhancedRTBeamLimitingDeviceSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTEnhancedRTBeamLimitingDeviceSequence::DRTEnhancedRTBeamLimitingDeviceSequence(const DRTEnhancedRTBeamLimitingDeviceSequence &copy)
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


DRTEnhancedRTBeamLimitingDeviceSequence &DRTEnhancedRTBeamLimitingDeviceSequence::operator=(const DRTEnhancedRTBeamLimitingDeviceSequence &copy)
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


DRTEnhancedRTBeamLimitingDeviceSequence::~DRTEnhancedRTBeamLimitingDeviceSequence()
{
    clear();
}


void DRTEnhancedRTBeamLimitingDeviceSequence::clear()
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


OFBool DRTEnhancedRTBeamLimitingDeviceSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTEnhancedRTBeamLimitingDeviceSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTEnhancedRTBeamLimitingDeviceSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::gotoNextItem()
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


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTEnhancedRTBeamLimitingDeviceSequence::Item &DRTEnhancedRTBeamLimitingDeviceSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTEnhancedRTBeamLimitingDeviceSequence::Item &DRTEnhancedRTBeamLimitingDeviceSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTEnhancedRTBeamLimitingDeviceSequence::Item &DRTEnhancedRTBeamLimitingDeviceSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTEnhancedRTBeamLimitingDeviceSequence::Item &DRTEnhancedRTBeamLimitingDeviceSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTEnhancedRTBeamLimitingDeviceSequence::Item &DRTEnhancedRTBeamLimitingDeviceSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTEnhancedRTBeamLimitingDeviceSequence::Item &DRTEnhancedRTBeamLimitingDeviceSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::addItem(Item *&item)
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


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::removeItem(const size_t pos)
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


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_EnhancedRTBeamLimitingDeviceSequence, sequence);
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
            DcmSequenceOfItems element(DCM_EnhancedRTBeamLimitingDeviceSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTEnhancedRTBeamLimitingDeviceSequence::write(DcmItem &dataset,
                                                           const OFString &card,
                                                           const OFString &type,
                                                           const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_EnhancedRTBeamLimitingDeviceSequence);
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
