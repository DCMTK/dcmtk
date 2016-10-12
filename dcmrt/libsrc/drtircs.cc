/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTIonRangeCompensatorSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtircs.h"


// --- item class ---

DRTIonRangeCompensatorSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AccessoryCode(DCM_AccessoryCode),
    CompensatorColumnOffset(DCM_CompensatorColumnOffset),
    CompensatorColumns(DCM_CompensatorColumns),
    CompensatorDescription(DCM_CompensatorDescription),
    CompensatorDivergence(DCM_CompensatorDivergence),
    CompensatorID(DCM_CompensatorID),
    CompensatorMillingToolDiameter(DCM_CompensatorMillingToolDiameter),
    CompensatorMountingPosition(DCM_CompensatorMountingPosition),
    CompensatorNumber(DCM_CompensatorNumber),
    CompensatorPixelSpacing(DCM_CompensatorPixelSpacing),
    CompensatorPosition(DCM_CompensatorPosition),
    CompensatorRelativeStoppingPowerRatio(DCM_CompensatorRelativeStoppingPowerRatio),
    CompensatorRows(DCM_CompensatorRows),
    CompensatorThicknessData(DCM_CompensatorThicknessData),
    IsocenterToCompensatorDistances(DCM_IsocenterToCompensatorDistances),
    IsocenterToCompensatorTrayDistance(DCM_IsocenterToCompensatorTrayDistance),
    MaterialID(DCM_MaterialID)
{
}


DRTIonRangeCompensatorSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AccessoryCode(copy.AccessoryCode),
    CompensatorColumnOffset(copy.CompensatorColumnOffset),
    CompensatorColumns(copy.CompensatorColumns),
    CompensatorDescription(copy.CompensatorDescription),
    CompensatorDivergence(copy.CompensatorDivergence),
    CompensatorID(copy.CompensatorID),
    CompensatorMillingToolDiameter(copy.CompensatorMillingToolDiameter),
    CompensatorMountingPosition(copy.CompensatorMountingPosition),
    CompensatorNumber(copy.CompensatorNumber),
    CompensatorPixelSpacing(copy.CompensatorPixelSpacing),
    CompensatorPosition(copy.CompensatorPosition),
    CompensatorRelativeStoppingPowerRatio(copy.CompensatorRelativeStoppingPowerRatio),
    CompensatorRows(copy.CompensatorRows),
    CompensatorThicknessData(copy.CompensatorThicknessData),
    IsocenterToCompensatorDistances(copy.IsocenterToCompensatorDistances),
    IsocenterToCompensatorTrayDistance(copy.IsocenterToCompensatorTrayDistance),
    MaterialID(copy.MaterialID)
{
}


DRTIonRangeCompensatorSequence::Item::~Item()
{
}


DRTIonRangeCompensatorSequence::Item &DRTIonRangeCompensatorSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AccessoryCode = copy.AccessoryCode;
        CompensatorColumnOffset = copy.CompensatorColumnOffset;
        CompensatorColumns = copy.CompensatorColumns;
        CompensatorDescription = copy.CompensatorDescription;
        CompensatorDivergence = copy.CompensatorDivergence;
        CompensatorID = copy.CompensatorID;
        CompensatorMillingToolDiameter = copy.CompensatorMillingToolDiameter;
        CompensatorMountingPosition = copy.CompensatorMountingPosition;
        CompensatorNumber = copy.CompensatorNumber;
        CompensatorPixelSpacing = copy.CompensatorPixelSpacing;
        CompensatorPosition = copy.CompensatorPosition;
        CompensatorRelativeStoppingPowerRatio = copy.CompensatorRelativeStoppingPowerRatio;
        CompensatorRows = copy.CompensatorRows;
        CompensatorThicknessData = copy.CompensatorThicknessData;
        IsocenterToCompensatorDistances = copy.IsocenterToCompensatorDistances;
        IsocenterToCompensatorTrayDistance = copy.IsocenterToCompensatorTrayDistance;
        MaterialID = copy.MaterialID;
    }
    return *this;
}


void DRTIonRangeCompensatorSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        CompensatorDescription.clear();
        CompensatorNumber.clear();
        MaterialID.clear();
        CompensatorID.clear();
        AccessoryCode.clear();
        IsocenterToCompensatorTrayDistance.clear();
        CompensatorDivergence.clear();
        CompensatorMountingPosition.clear();
        CompensatorRows.clear();
        CompensatorColumns.clear();
        CompensatorPixelSpacing.clear();
        CompensatorPosition.clear();
        CompensatorColumnOffset.clear();
        CompensatorThicknessData.clear();
        IsocenterToCompensatorDistances.clear();
        CompensatorRelativeStoppingPowerRatio.clear();
        CompensatorMillingToolDiameter.clear();
    }
}


OFBool DRTIonRangeCompensatorSequence::Item::isEmpty()
{
    return CompensatorDescription.isEmpty() &&
           CompensatorNumber.isEmpty() &&
           MaterialID.isEmpty() &&
           CompensatorID.isEmpty() &&
           AccessoryCode.isEmpty() &&
           IsocenterToCompensatorTrayDistance.isEmpty() &&
           CompensatorDivergence.isEmpty() &&
           CompensatorMountingPosition.isEmpty() &&
           CompensatorRows.isEmpty() &&
           CompensatorColumns.isEmpty() &&
           CompensatorPixelSpacing.isEmpty() &&
           CompensatorPosition.isEmpty() &&
           CompensatorColumnOffset.isEmpty() &&
           CompensatorThicknessData.isEmpty() &&
           IsocenterToCompensatorDistances.isEmpty() &&
           CompensatorRelativeStoppingPowerRatio.isEmpty() &&
           CompensatorMillingToolDiameter.isEmpty();
}


OFBool DRTIonRangeCompensatorSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTIonRangeCompensatorSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, CompensatorDescription, "1", "3", "IonRangeCompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorNumber, "1", "1", "IonRangeCompensatorSequence");
        getAndCheckElementFromDataset(item, MaterialID, "1", "2", "IonRangeCompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorID, "1", "3", "IonRangeCompensatorSequence");
        getAndCheckElementFromDataset(item, AccessoryCode, "1", "3", "IonRangeCompensatorSequence");
        getAndCheckElementFromDataset(item, IsocenterToCompensatorTrayDistance, "1", "1C", "IonRangeCompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorDivergence, "1", "1", "IonRangeCompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorMountingPosition, "1", "1", "IonRangeCompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorRows, "1", "1", "IonRangeCompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorColumns, "1", "1", "IonRangeCompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorPixelSpacing, "2", "1", "IonRangeCompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorPosition, "2", "1", "IonRangeCompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorColumnOffset, "1", "1C", "IonRangeCompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorThicknessData, "1-n", "1", "IonRangeCompensatorSequence");
        getAndCheckElementFromDataset(item, IsocenterToCompensatorDistances, "1-n", "1C", "IonRangeCompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorRelativeStoppingPowerRatio, "1", "3", "IonRangeCompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorMillingToolDiameter, "1", "3", "IonRangeCompensatorSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonRangeCompensatorSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmLongText(CompensatorDescription), "1", "3", "IonRangeCompensatorSequence");
        addElementToDataset(result, item, new DcmIntegerString(CompensatorNumber), "1", "1", "IonRangeCompensatorSequence");
        addElementToDataset(result, item, new DcmShortString(MaterialID), "1", "2", "IonRangeCompensatorSequence");
        addElementToDataset(result, item, new DcmShortString(CompensatorID), "1", "3", "IonRangeCompensatorSequence");
        addElementToDataset(result, item, new DcmLongString(AccessoryCode), "1", "3", "IonRangeCompensatorSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(IsocenterToCompensatorTrayDistance), "1", "1C", "IonRangeCompensatorSequence");
        addElementToDataset(result, item, new DcmCodeString(CompensatorDivergence), "1", "1", "IonRangeCompensatorSequence");
        addElementToDataset(result, item, new DcmCodeString(CompensatorMountingPosition), "1", "1", "IonRangeCompensatorSequence");
        addElementToDataset(result, item, new DcmIntegerString(CompensatorRows), "1", "1", "IonRangeCompensatorSequence");
        addElementToDataset(result, item, new DcmIntegerString(CompensatorColumns), "1", "1", "IonRangeCompensatorSequence");
        addElementToDataset(result, item, new DcmDecimalString(CompensatorPixelSpacing), "2", "1", "IonRangeCompensatorSequence");
        addElementToDataset(result, item, new DcmDecimalString(CompensatorPosition), "2", "1", "IonRangeCompensatorSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(CompensatorColumnOffset), "1", "1C", "IonRangeCompensatorSequence");
        addElementToDataset(result, item, new DcmDecimalString(CompensatorThicknessData), "1-n", "1", "IonRangeCompensatorSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(IsocenterToCompensatorDistances), "1-n", "1C", "IonRangeCompensatorSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(CompensatorRelativeStoppingPowerRatio), "1", "3", "IonRangeCompensatorSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(CompensatorMillingToolDiameter), "1", "3", "IonRangeCompensatorSequence");
    }
    return result;
}


OFCondition DRTIonRangeCompensatorSequence::Item::getAccessoryCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AccessoryCode, value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorColumnOffset(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, CompensatorColumnOffset).getFloat32(value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorColumns(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorColumns, value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorColumns(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, CompensatorColumns).getSint32(value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorDescription, value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorDivergence(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorDivergence, value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorID, value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorMillingToolDiameter(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, CompensatorMillingToolDiameter).getFloat32(value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorMountingPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorMountingPosition, value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorNumber, value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, CompensatorNumber).getSint32(value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorPixelSpacing(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorPixelSpacing, value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorPixelSpacing(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CompensatorPixelSpacing).getFloat64(value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorPixelSpacing(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CompensatorPixelSpacing).getFloat64Vector(value);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorPosition, value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorPosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CompensatorPosition).getFloat64(value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorPosition(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CompensatorPosition).getFloat64Vector(value);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorRelativeStoppingPowerRatio(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, CompensatorRelativeStoppingPowerRatio).getFloat32(value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorRows(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorRows, value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorRows(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, CompensatorRows).getSint32(value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorThicknessData(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorThicknessData, value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorThicknessData(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CompensatorThicknessData).getFloat64(value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getCompensatorThicknessData(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CompensatorThicknessData).getFloat64Vector(value);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getIsocenterToCompensatorDistances(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, IsocenterToCompensatorDistances).getFloat32(value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getIsocenterToCompensatorTrayDistance(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, IsocenterToCompensatorTrayDistance).getFloat32(value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::getMaterialID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MaterialID, value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::setAccessoryCode(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = AccessoryCode.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonRangeCompensatorSequence::Item::setCompensatorColumnOffset(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return CompensatorColumnOffset.putFloat32(value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::setCompensatorColumns(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CompensatorColumns.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonRangeCompensatorSequence::Item::setCompensatorDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = CompensatorDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonRangeCompensatorSequence::Item::setCompensatorDivergence(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CompensatorDivergence.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonRangeCompensatorSequence::Item::setCompensatorID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CompensatorID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonRangeCompensatorSequence::Item::setCompensatorMillingToolDiameter(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return CompensatorMillingToolDiameter.putFloat32(value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::setCompensatorMountingPosition(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CompensatorMountingPosition.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonRangeCompensatorSequence::Item::setCompensatorNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CompensatorNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonRangeCompensatorSequence::Item::setCompensatorPixelSpacing(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "2") : EC_Normal;
        if (result.good())
            result = CompensatorPixelSpacing.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonRangeCompensatorSequence::Item::setCompensatorPosition(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "2") : EC_Normal;
        if (result.good())
            result = CompensatorPosition.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonRangeCompensatorSequence::Item::setCompensatorRelativeStoppingPowerRatio(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return CompensatorRelativeStoppingPowerRatio.putFloat32(value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::setCompensatorRows(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CompensatorRows.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonRangeCompensatorSequence::Item::setCompensatorThicknessData(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1-n") : EC_Normal;
        if (result.good())
            result = CompensatorThicknessData.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonRangeCompensatorSequence::Item::setIsocenterToCompensatorDistances(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return IsocenterToCompensatorDistances.putFloat32(value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::setIsocenterToCompensatorTrayDistance(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return IsocenterToCompensatorTrayDistance.putFloat32(value, pos);
}


OFCondition DRTIonRangeCompensatorSequence::Item::setMaterialID(const OFString &value, const OFBool check)
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


// --- sequence class ---

DRTIonRangeCompensatorSequence::DRTIonRangeCompensatorSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTIonRangeCompensatorSequence::DRTIonRangeCompensatorSequence(const DRTIonRangeCompensatorSequence &copy)
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


DRTIonRangeCompensatorSequence &DRTIonRangeCompensatorSequence::operator=(const DRTIonRangeCompensatorSequence &copy)
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


DRTIonRangeCompensatorSequence::~DRTIonRangeCompensatorSequence()
{
    clear();
}


void DRTIonRangeCompensatorSequence::clear()
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


OFBool DRTIonRangeCompensatorSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTIonRangeCompensatorSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTIonRangeCompensatorSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTIonRangeCompensatorSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonRangeCompensatorSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonRangeCompensatorSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTIonRangeCompensatorSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTIonRangeCompensatorSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTIonRangeCompensatorSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTIonRangeCompensatorSequence::Item &DRTIonRangeCompensatorSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTIonRangeCompensatorSequence::Item &DRTIonRangeCompensatorSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTIonRangeCompensatorSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTIonRangeCompensatorSequence::Item &DRTIonRangeCompensatorSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTIonRangeCompensatorSequence::Item &DRTIonRangeCompensatorSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTIonRangeCompensatorSequence::Item &DRTIonRangeCompensatorSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTIonRangeCompensatorSequence::Item &DRTIonRangeCompensatorSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTIonRangeCompensatorSequence::addItem(Item *&item)
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


OFCondition DRTIonRangeCompensatorSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTIonRangeCompensatorSequence::removeItem(const unsigned long pos)
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


OFCondition DRTIonRangeCompensatorSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_IonRangeCompensatorSequence, sequence);
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
            DcmSequenceOfItems element(DCM_IonRangeCompensatorSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTIonRangeCompensatorSequence::write(DcmItem &dataset,
                                                  const OFString &card,
                                                  const OFString &type,
                                                  const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_IonRangeCompensatorSequence);
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
