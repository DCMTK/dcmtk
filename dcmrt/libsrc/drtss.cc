/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTSourceSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtss.h"


// --- item class ---

DRTSourceSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ActiveSourceDiameter(DCM_ActiveSourceDiameter),
    ActiveSourceLength(DCM_ActiveSourceLength),
    MaterialID(DCM_MaterialID),
    ReferenceAirKermaRate(DCM_ReferenceAirKermaRate),
    SourceDescription(DCM_SourceDescription),
    SourceEncapsulationNominalThickness(DCM_SourceEncapsulationNominalThickness),
    SourceEncapsulationNominalTransmission(DCM_SourceEncapsulationNominalTransmission),
    SourceIsotopeHalfLife(DCM_SourceIsotopeHalfLife),
    SourceIsotopeName(DCM_SourceIsotopeName),
    SourceManufacturer(DCM_SourceManufacturer),
    SourceModelID(DCM_SourceModelID),
    SourceNumber(DCM_SourceNumber),
    SourceSerialNumber(DCM_SourceSerialNumber),
    SourceStrength(DCM_SourceStrength),
    SourceStrengthReferenceDate(DCM_SourceStrengthReferenceDate),
    SourceStrengthReferenceTime(DCM_SourceStrengthReferenceTime),
    SourceStrengthUnits(DCM_SourceStrengthUnits),
    SourceType(DCM_SourceType)
{
}


DRTSourceSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ActiveSourceDiameter(copy.ActiveSourceDiameter),
    ActiveSourceLength(copy.ActiveSourceLength),
    MaterialID(copy.MaterialID),
    ReferenceAirKermaRate(copy.ReferenceAirKermaRate),
    SourceDescription(copy.SourceDescription),
    SourceEncapsulationNominalThickness(copy.SourceEncapsulationNominalThickness),
    SourceEncapsulationNominalTransmission(copy.SourceEncapsulationNominalTransmission),
    SourceIsotopeHalfLife(copy.SourceIsotopeHalfLife),
    SourceIsotopeName(copy.SourceIsotopeName),
    SourceManufacturer(copy.SourceManufacturer),
    SourceModelID(copy.SourceModelID),
    SourceNumber(copy.SourceNumber),
    SourceSerialNumber(copy.SourceSerialNumber),
    SourceStrength(copy.SourceStrength),
    SourceStrengthReferenceDate(copy.SourceStrengthReferenceDate),
    SourceStrengthReferenceTime(copy.SourceStrengthReferenceTime),
    SourceStrengthUnits(copy.SourceStrengthUnits),
    SourceType(copy.SourceType)
{
}


DRTSourceSequence::Item::~Item()
{
}


DRTSourceSequence::Item &DRTSourceSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ActiveSourceDiameter = copy.ActiveSourceDiameter;
        ActiveSourceLength = copy.ActiveSourceLength;
        MaterialID = copy.MaterialID;
        ReferenceAirKermaRate = copy.ReferenceAirKermaRate;
        SourceDescription = copy.SourceDescription;
        SourceEncapsulationNominalThickness = copy.SourceEncapsulationNominalThickness;
        SourceEncapsulationNominalTransmission = copy.SourceEncapsulationNominalTransmission;
        SourceIsotopeHalfLife = copy.SourceIsotopeHalfLife;
        SourceIsotopeName = copy.SourceIsotopeName;
        SourceManufacturer = copy.SourceManufacturer;
        SourceModelID = copy.SourceModelID;
        SourceNumber = copy.SourceNumber;
        SourceSerialNumber = copy.SourceSerialNumber;
        SourceStrength = copy.SourceStrength;
        SourceStrengthReferenceDate = copy.SourceStrengthReferenceDate;
        SourceStrengthReferenceTime = copy.SourceStrengthReferenceTime;
        SourceStrengthUnits = copy.SourceStrengthUnits;
        SourceType = copy.SourceType;
    }
    return *this;
}


void DRTSourceSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        SourceNumber.clear();
        SourceSerialNumber.clear();
        SourceModelID.clear();
        SourceDescription.clear();
        SourceType.clear();
        SourceManufacturer.clear();
        ActiveSourceDiameter.clear();
        ActiveSourceLength.clear();
        MaterialID.clear();
        SourceEncapsulationNominalThickness.clear();
        SourceEncapsulationNominalTransmission.clear();
        SourceIsotopeName.clear();
        SourceIsotopeHalfLife.clear();
        SourceStrengthUnits.clear();
        ReferenceAirKermaRate.clear();
        SourceStrength.clear();
        SourceStrengthReferenceDate.clear();
        SourceStrengthReferenceTime.clear();
    }
}


OFBool DRTSourceSequence::Item::isEmpty()
{
    return SourceNumber.isEmpty() &&
           SourceSerialNumber.isEmpty() &&
           SourceModelID.isEmpty() &&
           SourceDescription.isEmpty() &&
           SourceType.isEmpty() &&
           SourceManufacturer.isEmpty() &&
           ActiveSourceDiameter.isEmpty() &&
           ActiveSourceLength.isEmpty() &&
           MaterialID.isEmpty() &&
           SourceEncapsulationNominalThickness.isEmpty() &&
           SourceEncapsulationNominalTransmission.isEmpty() &&
           SourceIsotopeName.isEmpty() &&
           SourceIsotopeHalfLife.isEmpty() &&
           SourceStrengthUnits.isEmpty() &&
           ReferenceAirKermaRate.isEmpty() &&
           SourceStrength.isEmpty() &&
           SourceStrengthReferenceDate.isEmpty() &&
           SourceStrengthReferenceTime.isEmpty();
}


OFBool DRTSourceSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTSourceSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, SourceNumber, "1", "1", "SourceSequence");
        getAndCheckElementFromDataset(item, SourceSerialNumber, "1", "3", "SourceSequence");
        getAndCheckElementFromDataset(item, SourceModelID, "1", "3", "SourceSequence");
        getAndCheckElementFromDataset(item, SourceDescription, "1", "3", "SourceSequence");
        getAndCheckElementFromDataset(item, SourceType, "1", "1", "SourceSequence");
        getAndCheckElementFromDataset(item, SourceManufacturer, "1", "3", "SourceSequence");
        getAndCheckElementFromDataset(item, ActiveSourceDiameter, "1", "3", "SourceSequence");
        getAndCheckElementFromDataset(item, ActiveSourceLength, "1", "3", "SourceSequence");
        getAndCheckElementFromDataset(item, MaterialID, "1", "3", "SourceSequence");
        getAndCheckElementFromDataset(item, SourceEncapsulationNominalThickness, "1", "3", "SourceSequence");
        getAndCheckElementFromDataset(item, SourceEncapsulationNominalTransmission, "1", "3", "SourceSequence");
        getAndCheckElementFromDataset(item, SourceIsotopeName, "1", "1", "SourceSequence");
        getAndCheckElementFromDataset(item, SourceIsotopeHalfLife, "1", "1", "SourceSequence");
        getAndCheckElementFromDataset(item, SourceStrengthUnits, "1", "1C", "SourceSequence");
        getAndCheckElementFromDataset(item, ReferenceAirKermaRate, "1", "1", "SourceSequence");
        getAndCheckElementFromDataset(item, SourceStrength, "1", "1C", "SourceSequence");
        getAndCheckElementFromDataset(item, SourceStrengthReferenceDate, "1", "1", "SourceSequence");
        getAndCheckElementFromDataset(item, SourceStrengthReferenceTime, "1", "1", "SourceSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTSourceSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(SourceNumber), "1", "1", "SourceSequence");
        addElementToDataset(result, item, new DcmLongString(SourceSerialNumber), "1", "3", "SourceSequence");
        addElementToDataset(result, item, new DcmShortString(SourceModelID), "1", "3", "SourceSequence");
        addElementToDataset(result, item, new DcmLongString(SourceDescription), "1", "3", "SourceSequence");
        addElementToDataset(result, item, new DcmCodeString(SourceType), "1", "1", "SourceSequence");
        addElementToDataset(result, item, new DcmLongString(SourceManufacturer), "1", "3", "SourceSequence");
        addElementToDataset(result, item, new DcmDecimalString(ActiveSourceDiameter), "1", "3", "SourceSequence");
        addElementToDataset(result, item, new DcmDecimalString(ActiveSourceLength), "1", "3", "SourceSequence");
        addElementToDataset(result, item, new DcmShortString(MaterialID), "1", "3", "SourceSequence");
        addElementToDataset(result, item, new DcmDecimalString(SourceEncapsulationNominalThickness), "1", "3", "SourceSequence");
        addElementToDataset(result, item, new DcmDecimalString(SourceEncapsulationNominalTransmission), "1", "3", "SourceSequence");
        addElementToDataset(result, item, new DcmLongString(SourceIsotopeName), "1", "1", "SourceSequence");
        addElementToDataset(result, item, new DcmDecimalString(SourceIsotopeHalfLife), "1", "1", "SourceSequence");
        addElementToDataset(result, item, new DcmCodeString(SourceStrengthUnits), "1", "1C", "SourceSequence");
        addElementToDataset(result, item, new DcmDecimalString(ReferenceAirKermaRate), "1", "1", "SourceSequence");
        addElementToDataset(result, item, new DcmDecimalString(SourceStrength), "1", "1C", "SourceSequence");
        addElementToDataset(result, item, new DcmDate(SourceStrengthReferenceDate), "1", "1", "SourceSequence");
        addElementToDataset(result, item, new DcmTime(SourceStrengthReferenceTime), "1", "1", "SourceSequence");
    }
    return result;
}


OFCondition DRTSourceSequence::Item::getActiveSourceDiameter(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ActiveSourceDiameter, value, pos);
}


OFCondition DRTSourceSequence::Item::getActiveSourceDiameter(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ActiveSourceDiameter).getFloat64(value, pos);
}


OFCondition DRTSourceSequence::Item::getActiveSourceLength(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ActiveSourceLength, value, pos);
}


OFCondition DRTSourceSequence::Item::getActiveSourceLength(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ActiveSourceLength).getFloat64(value, pos);
}


OFCondition DRTSourceSequence::Item::getMaterialID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MaterialID, value, pos);
}


OFCondition DRTSourceSequence::Item::getReferenceAirKermaRate(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferenceAirKermaRate, value, pos);
}


OFCondition DRTSourceSequence::Item::getReferenceAirKermaRate(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ReferenceAirKermaRate).getFloat64(value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceDescription, value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceEncapsulationNominalThickness(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceEncapsulationNominalThickness, value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceEncapsulationNominalThickness(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SourceEncapsulationNominalThickness).getFloat64(value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceEncapsulationNominalTransmission(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceEncapsulationNominalTransmission, value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceEncapsulationNominalTransmission(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SourceEncapsulationNominalTransmission).getFloat64(value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceIsotopeHalfLife(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceIsotopeHalfLife, value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceIsotopeHalfLife(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SourceIsotopeHalfLife).getFloat64(value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceIsotopeName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceIsotopeName, value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceManufacturer(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceManufacturer, value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceModelID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceModelID, value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceNumber, value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, SourceNumber).getSint32(value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceSerialNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceSerialNumber, value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceStrength(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceStrength, value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceStrength(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SourceStrength).getFloat64(value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceStrengthReferenceDate(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceStrengthReferenceDate, value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceStrengthReferenceTime(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceStrengthReferenceTime, value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceStrengthUnits(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceStrengthUnits, value, pos);
}


OFCondition DRTSourceSequence::Item::getSourceType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceType, value, pos);
}


OFCondition DRTSourceSequence::Item::setActiveSourceDiameter(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ActiveSourceDiameter.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourceSequence::Item::setActiveSourceLength(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ActiveSourceLength.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourceSequence::Item::setMaterialID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = MaterialID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourceSequence::Item::setReferenceAirKermaRate(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferenceAirKermaRate.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourceSequence::Item::setSourceDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourceSequence::Item::setSourceEncapsulationNominalThickness(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceEncapsulationNominalThickness.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourceSequence::Item::setSourceEncapsulationNominalTransmission(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceEncapsulationNominalTransmission.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourceSequence::Item::setSourceIsotopeHalfLife(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceIsotopeHalfLife.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourceSequence::Item::setSourceIsotopeName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceIsotopeName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourceSequence::Item::setSourceManufacturer(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceManufacturer.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourceSequence::Item::setSourceModelID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceModelID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourceSequence::Item::setSourceNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourceSequence::Item::setSourceSerialNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceSerialNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourceSequence::Item::setSourceStrength(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceStrength.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourceSequence::Item::setSourceStrengthReferenceDate(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceStrengthReferenceDate.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourceSequence::Item::setSourceStrengthReferenceTime(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceStrengthReferenceTime.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourceSequence::Item::setSourceStrengthUnits(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceStrengthUnits.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTSourceSequence::Item::setSourceType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceType.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTSourceSequence::DRTSourceSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTSourceSequence::DRTSourceSequence(const DRTSourceSequence &copy)
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


DRTSourceSequence &DRTSourceSequence::operator=(const DRTSourceSequence &copy)
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


DRTSourceSequence::~DRTSourceSequence()
{
    clear();
}


void DRTSourceSequence::clear()
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


OFBool DRTSourceSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTSourceSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTSourceSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTSourceSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTSourceSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTSourceSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTSourceSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTSourceSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTSourceSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTSourceSequence::Item &DRTSourceSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTSourceSequence::Item &DRTSourceSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTSourceSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTSourceSequence::Item &DRTSourceSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTSourceSequence::Item &DRTSourceSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTSourceSequence::Item &DRTSourceSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTSourceSequence::Item &DRTSourceSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTSourceSequence::addItem(Item *&item)
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


OFCondition DRTSourceSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTSourceSequence::removeItem(const unsigned long pos)
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


OFCondition DRTSourceSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_SourceSequence, sequence);
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
            DcmSequenceOfItems element(DCM_SourceSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTSourceSequence::write(DcmItem &dataset,
                                     const OFString &card,
                                     const OFString &type,
                                     const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_SourceSequence);
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
