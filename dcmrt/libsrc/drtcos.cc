/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTCompensatorSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtcos.h"


// --- item class ---

DRTCompensatorSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AccessoryCode(DCM_AccessoryCode),
    CompensatorColumns(DCM_CompensatorColumns),
    CompensatorDescription(DCM_CompensatorDescription),
    CompensatorDivergence(DCM_CompensatorDivergence),
    CompensatorID(DCM_CompensatorID),
    CompensatorMountingPosition(DCM_CompensatorMountingPosition),
    CompensatorNumber(DCM_CompensatorNumber),
    CompensatorPixelSpacing(DCM_CompensatorPixelSpacing),
    CompensatorPosition(DCM_CompensatorPosition),
    CompensatorRows(DCM_CompensatorRows),
    CompensatorThicknessData(DCM_CompensatorThicknessData),
    CompensatorTransmissionData(DCM_CompensatorTransmissionData),
    CompensatorTrayID(DCM_CompensatorTrayID),
    CompensatorType(DCM_CompensatorType),
    MaterialID(DCM_MaterialID),
    SourceToCompensatorDistance(DCM_SourceToCompensatorDistance),
    SourceToCompensatorTrayDistance(DCM_SourceToCompensatorTrayDistance),
    TrayAccessoryCode(DCM_TrayAccessoryCode)
{
}


DRTCompensatorSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AccessoryCode(copy.AccessoryCode),
    CompensatorColumns(copy.CompensatorColumns),
    CompensatorDescription(copy.CompensatorDescription),
    CompensatorDivergence(copy.CompensatorDivergence),
    CompensatorID(copy.CompensatorID),
    CompensatorMountingPosition(copy.CompensatorMountingPosition),
    CompensatorNumber(copy.CompensatorNumber),
    CompensatorPixelSpacing(copy.CompensatorPixelSpacing),
    CompensatorPosition(copy.CompensatorPosition),
    CompensatorRows(copy.CompensatorRows),
    CompensatorThicknessData(copy.CompensatorThicknessData),
    CompensatorTransmissionData(copy.CompensatorTransmissionData),
    CompensatorTrayID(copy.CompensatorTrayID),
    CompensatorType(copy.CompensatorType),
    MaterialID(copy.MaterialID),
    SourceToCompensatorDistance(copy.SourceToCompensatorDistance),
    SourceToCompensatorTrayDistance(copy.SourceToCompensatorTrayDistance),
    TrayAccessoryCode(copy.TrayAccessoryCode)
{
}


DRTCompensatorSequence::Item::~Item()
{
}


DRTCompensatorSequence::Item &DRTCompensatorSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AccessoryCode = copy.AccessoryCode;
        CompensatorColumns = copy.CompensatorColumns;
        CompensatorDescription = copy.CompensatorDescription;
        CompensatorDivergence = copy.CompensatorDivergence;
        CompensatorID = copy.CompensatorID;
        CompensatorMountingPosition = copy.CompensatorMountingPosition;
        CompensatorNumber = copy.CompensatorNumber;
        CompensatorPixelSpacing = copy.CompensatorPixelSpacing;
        CompensatorPosition = copy.CompensatorPosition;
        CompensatorRows = copy.CompensatorRows;
        CompensatorThicknessData = copy.CompensatorThicknessData;
        CompensatorTransmissionData = copy.CompensatorTransmissionData;
        CompensatorTrayID = copy.CompensatorTrayID;
        CompensatorType = copy.CompensatorType;
        MaterialID = copy.MaterialID;
        SourceToCompensatorDistance = copy.SourceToCompensatorDistance;
        SourceToCompensatorTrayDistance = copy.SourceToCompensatorTrayDistance;
        TrayAccessoryCode = copy.TrayAccessoryCode;
    }
    return *this;
}


void DRTCompensatorSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        CompensatorDescription.clear();
        CompensatorNumber.clear();
        CompensatorType.clear();
        MaterialID.clear();
        CompensatorID.clear();
        AccessoryCode.clear();
        CompensatorTrayID.clear();
        TrayAccessoryCode.clear();
        SourceToCompensatorTrayDistance.clear();
        CompensatorDivergence.clear();
        CompensatorMountingPosition.clear();
        CompensatorRows.clear();
        CompensatorColumns.clear();
        CompensatorPixelSpacing.clear();
        CompensatorPosition.clear();
        CompensatorTransmissionData.clear();
        CompensatorThicknessData.clear();
        SourceToCompensatorDistance.clear();
    }
}


OFBool DRTCompensatorSequence::Item::isEmpty()
{
    return CompensatorDescription.isEmpty() &&
           CompensatorNumber.isEmpty() &&
           CompensatorType.isEmpty() &&
           MaterialID.isEmpty() &&
           CompensatorID.isEmpty() &&
           AccessoryCode.isEmpty() &&
           CompensatorTrayID.isEmpty() &&
           TrayAccessoryCode.isEmpty() &&
           SourceToCompensatorTrayDistance.isEmpty() &&
           CompensatorDivergence.isEmpty() &&
           CompensatorMountingPosition.isEmpty() &&
           CompensatorRows.isEmpty() &&
           CompensatorColumns.isEmpty() &&
           CompensatorPixelSpacing.isEmpty() &&
           CompensatorPosition.isEmpty() &&
           CompensatorTransmissionData.isEmpty() &&
           CompensatorThicknessData.isEmpty() &&
           SourceToCompensatorDistance.isEmpty();
}


OFBool DRTCompensatorSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTCompensatorSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, CompensatorDescription, "1", "3", "CompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorNumber, "1", "1C", "CompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorType, "1", "3", "CompensatorSequence");
        getAndCheckElementFromDataset(item, MaterialID, "1", "2C", "CompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorID, "1", "3", "CompensatorSequence");
        getAndCheckElementFromDataset(item, AccessoryCode, "1", "3", "CompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorTrayID, "1", "3", "CompensatorSequence");
        getAndCheckElementFromDataset(item, TrayAccessoryCode, "1", "3", "CompensatorSequence");
        getAndCheckElementFromDataset(item, SourceToCompensatorTrayDistance, "1", "2", "CompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorDivergence, "1", "3", "CompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorMountingPosition, "1", "3", "CompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorRows, "1", "1", "CompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorColumns, "1", "1", "CompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorPixelSpacing, "2", "1", "CompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorPosition, "2", "1", "CompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorTransmissionData, "1-n", "1C", "CompensatorSequence");
        getAndCheckElementFromDataset(item, CompensatorThicknessData, "1-n", "1C", "CompensatorSequence");
        getAndCheckElementFromDataset(item, SourceToCompensatorDistance, "1-n", "1C", "CompensatorSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTCompensatorSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmLongText(CompensatorDescription), "1", "3", "CompensatorSequence");
        addElementToDataset(result, item, new DcmIntegerString(CompensatorNumber), "1", "1C", "CompensatorSequence");
        addElementToDataset(result, item, new DcmCodeString(CompensatorType), "1", "3", "CompensatorSequence");
        addElementToDataset(result, item, new DcmShortString(MaterialID), "1", "2C", "CompensatorSequence");
        addElementToDataset(result, item, new DcmShortString(CompensatorID), "1", "3", "CompensatorSequence");
        addElementToDataset(result, item, new DcmLongString(AccessoryCode), "1", "3", "CompensatorSequence");
        addElementToDataset(result, item, new DcmShortString(CompensatorTrayID), "1", "3", "CompensatorSequence");
        addElementToDataset(result, item, new DcmLongString(TrayAccessoryCode), "1", "3", "CompensatorSequence");
        addElementToDataset(result, item, new DcmDecimalString(SourceToCompensatorTrayDistance), "1", "2", "CompensatorSequence");
        addElementToDataset(result, item, new DcmCodeString(CompensatorDivergence), "1", "3", "CompensatorSequence");
        addElementToDataset(result, item, new DcmCodeString(CompensatorMountingPosition), "1", "3", "CompensatorSequence");
        addElementToDataset(result, item, new DcmIntegerString(CompensatorRows), "1", "1", "CompensatorSequence");
        addElementToDataset(result, item, new DcmIntegerString(CompensatorColumns), "1", "1", "CompensatorSequence");
        addElementToDataset(result, item, new DcmDecimalString(CompensatorPixelSpacing), "2", "1", "CompensatorSequence");
        addElementToDataset(result, item, new DcmDecimalString(CompensatorPosition), "2", "1", "CompensatorSequence");
        addElementToDataset(result, item, new DcmDecimalString(CompensatorTransmissionData), "1-n", "1C", "CompensatorSequence");
        addElementToDataset(result, item, new DcmDecimalString(CompensatorThicknessData), "1-n", "1C", "CompensatorSequence");
        addElementToDataset(result, item, new DcmDecimalString(SourceToCompensatorDistance), "1-n", "1C", "CompensatorSequence");
    }
    return result;
}


OFCondition DRTCompensatorSequence::Item::getAccessoryCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AccessoryCode, value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorColumns(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorColumns, value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorColumns(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, CompensatorColumns).getSint32(value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorDescription, value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorDivergence(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorDivergence, value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorID, value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorMountingPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorMountingPosition, value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorNumber, value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, CompensatorNumber).getSint32(value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorPixelSpacing(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorPixelSpacing, value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorPixelSpacing(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CompensatorPixelSpacing).getFloat64(value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorPixelSpacing(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CompensatorPixelSpacing).getFloat64Vector(value);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorPosition, value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorPosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CompensatorPosition).getFloat64(value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorPosition(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CompensatorPosition).getFloat64Vector(value);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorRows(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorRows, value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorRows(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, CompensatorRows).getSint32(value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorThicknessData(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorThicknessData, value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorThicknessData(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CompensatorThicknessData).getFloat64(value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorThicknessData(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CompensatorThicknessData).getFloat64Vector(value);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorTransmissionData(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorTransmissionData, value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorTransmissionData(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CompensatorTransmissionData).getFloat64(value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorTransmissionData(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CompensatorTransmissionData).getFloat64Vector(value);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorTrayID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorTrayID, value, pos);
}


OFCondition DRTCompensatorSequence::Item::getCompensatorType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CompensatorType, value, pos);
}


OFCondition DRTCompensatorSequence::Item::getMaterialID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MaterialID, value, pos);
}


OFCondition DRTCompensatorSequence::Item::getSourceToCompensatorDistance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceToCompensatorDistance, value, pos);
}


OFCondition DRTCompensatorSequence::Item::getSourceToCompensatorDistance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SourceToCompensatorDistance).getFloat64(value, pos);
}


OFCondition DRTCompensatorSequence::Item::getSourceToCompensatorDistance(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SourceToCompensatorDistance).getFloat64Vector(value);
}


OFCondition DRTCompensatorSequence::Item::getSourceToCompensatorTrayDistance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceToCompensatorTrayDistance, value, pos);
}


OFCondition DRTCompensatorSequence::Item::getSourceToCompensatorTrayDistance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SourceToCompensatorTrayDistance).getFloat64(value, pos);
}


OFCondition DRTCompensatorSequence::Item::getTrayAccessoryCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TrayAccessoryCode, value, pos);
}


OFCondition DRTCompensatorSequence::Item::setAccessoryCode(const OFString &value, const OFBool check)
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


OFCondition DRTCompensatorSequence::Item::setCompensatorColumns(const OFString &value, const OFBool check)
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


OFCondition DRTCompensatorSequence::Item::setCompensatorDescription(const OFString &value, const OFBool check)
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


OFCondition DRTCompensatorSequence::Item::setCompensatorDivergence(const OFString &value, const OFBool check)
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


OFCondition DRTCompensatorSequence::Item::setCompensatorID(const OFString &value, const OFBool check)
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


OFCondition DRTCompensatorSequence::Item::setCompensatorMountingPosition(const OFString &value, const OFBool check)
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


OFCondition DRTCompensatorSequence::Item::setCompensatorNumber(const OFString &value, const OFBool check)
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


OFCondition DRTCompensatorSequence::Item::setCompensatorPixelSpacing(const OFString &value, const OFBool check)
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


OFCondition DRTCompensatorSequence::Item::setCompensatorPosition(const OFString &value, const OFBool check)
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


OFCondition DRTCompensatorSequence::Item::setCompensatorRows(const OFString &value, const OFBool check)
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


OFCondition DRTCompensatorSequence::Item::setCompensatorThicknessData(const OFString &value, const OFBool check)
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


OFCondition DRTCompensatorSequence::Item::setCompensatorTransmissionData(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1-n") : EC_Normal;
        if (result.good())
            result = CompensatorTransmissionData.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTCompensatorSequence::Item::setCompensatorTrayID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CompensatorTrayID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTCompensatorSequence::Item::setCompensatorType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CompensatorType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTCompensatorSequence::Item::setMaterialID(const OFString &value, const OFBool check)
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


OFCondition DRTCompensatorSequence::Item::setSourceToCompensatorDistance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1-n") : EC_Normal;
        if (result.good())
            result = SourceToCompensatorDistance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTCompensatorSequence::Item::setSourceToCompensatorTrayDistance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceToCompensatorTrayDistance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTCompensatorSequence::Item::setTrayAccessoryCode(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TrayAccessoryCode.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTCompensatorSequence::DRTCompensatorSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTCompensatorSequence::DRTCompensatorSequence(const DRTCompensatorSequence &copy)
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


DRTCompensatorSequence &DRTCompensatorSequence::operator=(const DRTCompensatorSequence &copy)
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


DRTCompensatorSequence::~DRTCompensatorSequence()
{
    clear();
}


void DRTCompensatorSequence::clear()
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


OFBool DRTCompensatorSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTCompensatorSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTCompensatorSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTCompensatorSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTCompensatorSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTCompensatorSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTCompensatorSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTCompensatorSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTCompensatorSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTCompensatorSequence::Item &DRTCompensatorSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTCompensatorSequence::Item &DRTCompensatorSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTCompensatorSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTCompensatorSequence::Item &DRTCompensatorSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTCompensatorSequence::Item &DRTCompensatorSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTCompensatorSequence::Item &DRTCompensatorSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTCompensatorSequence::Item &DRTCompensatorSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTCompensatorSequence::addItem(Item *&item)
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


OFCondition DRTCompensatorSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTCompensatorSequence::removeItem(const size_t pos)
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


OFCondition DRTCompensatorSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_CompensatorSequence, sequence);
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
            DcmSequenceOfItems element(DCM_CompensatorSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTCompensatorSequence::write(DcmItem &dataset,
                                          const OFString &card,
                                          const OFString &type,
                                          const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_CompensatorSequence);
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
