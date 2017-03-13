/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTIonToleranceTableSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtitts.h"


// --- item class ---

DRTIonToleranceTableSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    BeamLimitingDeviceAngleTolerance(DCM_BeamLimitingDeviceAngleTolerance),
    BeamLimitingDeviceToleranceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ChairHeadFramePositionTolerance(DCM_ChairHeadFramePositionTolerance),
    FixationLightAzimuthalAngleTolerance(DCM_FixationLightAzimuthalAngleTolerance),
    FixationLightPolarAngleTolerance(DCM_FixationLightPolarAngleTolerance),
    GantryAngleTolerance(DCM_GantryAngleTolerance),
    HeadFixationAngleTolerance(DCM_HeadFixationAngleTolerance),
    PatientSupportAngleTolerance(DCM_PatientSupportAngleTolerance),
    SnoutPositionTolerance(DCM_SnoutPositionTolerance),
    TableTopLateralPositionTolerance(DCM_TableTopLateralPositionTolerance),
    TableTopLongitudinalPositionTolerance(DCM_TableTopLongitudinalPositionTolerance),
    TableTopPitchAngleTolerance(DCM_TableTopPitchAngleTolerance),
    TableTopRollAngleTolerance(DCM_TableTopRollAngleTolerance),
    TableTopVerticalPositionTolerance(DCM_TableTopVerticalPositionTolerance),
    ToleranceTableLabel(DCM_ToleranceTableLabel),
    ToleranceTableNumber(DCM_ToleranceTableNumber)
{
}


DRTIonToleranceTableSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    BeamLimitingDeviceAngleTolerance(copy.BeamLimitingDeviceAngleTolerance),
    BeamLimitingDeviceToleranceSequence(copy.BeamLimitingDeviceToleranceSequence),
    ChairHeadFramePositionTolerance(copy.ChairHeadFramePositionTolerance),
    FixationLightAzimuthalAngleTolerance(copy.FixationLightAzimuthalAngleTolerance),
    FixationLightPolarAngleTolerance(copy.FixationLightPolarAngleTolerance),
    GantryAngleTolerance(copy.GantryAngleTolerance),
    HeadFixationAngleTolerance(copy.HeadFixationAngleTolerance),
    PatientSupportAngleTolerance(copy.PatientSupportAngleTolerance),
    SnoutPositionTolerance(copy.SnoutPositionTolerance),
    TableTopLateralPositionTolerance(copy.TableTopLateralPositionTolerance),
    TableTopLongitudinalPositionTolerance(copy.TableTopLongitudinalPositionTolerance),
    TableTopPitchAngleTolerance(copy.TableTopPitchAngleTolerance),
    TableTopRollAngleTolerance(copy.TableTopRollAngleTolerance),
    TableTopVerticalPositionTolerance(copy.TableTopVerticalPositionTolerance),
    ToleranceTableLabel(copy.ToleranceTableLabel),
    ToleranceTableNumber(copy.ToleranceTableNumber)
{
}


DRTIonToleranceTableSequence::Item::~Item()
{
}


DRTIonToleranceTableSequence::Item &DRTIonToleranceTableSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        BeamLimitingDeviceAngleTolerance = copy.BeamLimitingDeviceAngleTolerance;
        BeamLimitingDeviceToleranceSequence = copy.BeamLimitingDeviceToleranceSequence;
        ChairHeadFramePositionTolerance = copy.ChairHeadFramePositionTolerance;
        FixationLightAzimuthalAngleTolerance = copy.FixationLightAzimuthalAngleTolerance;
        FixationLightPolarAngleTolerance = copy.FixationLightPolarAngleTolerance;
        GantryAngleTolerance = copy.GantryAngleTolerance;
        HeadFixationAngleTolerance = copy.HeadFixationAngleTolerance;
        PatientSupportAngleTolerance = copy.PatientSupportAngleTolerance;
        SnoutPositionTolerance = copy.SnoutPositionTolerance;
        TableTopLateralPositionTolerance = copy.TableTopLateralPositionTolerance;
        TableTopLongitudinalPositionTolerance = copy.TableTopLongitudinalPositionTolerance;
        TableTopPitchAngleTolerance = copy.TableTopPitchAngleTolerance;
        TableTopRollAngleTolerance = copy.TableTopRollAngleTolerance;
        TableTopVerticalPositionTolerance = copy.TableTopVerticalPositionTolerance;
        ToleranceTableLabel = copy.ToleranceTableLabel;
        ToleranceTableNumber = copy.ToleranceTableNumber;
    }
    return *this;
}


void DRTIonToleranceTableSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ToleranceTableNumber.clear();
        ToleranceTableLabel.clear();
        GantryAngleTolerance.clear();
        BeamLimitingDeviceAngleTolerance.clear();
        BeamLimitingDeviceToleranceSequence.clear();
        PatientSupportAngleTolerance.clear();
        TableTopVerticalPositionTolerance.clear();
        TableTopLongitudinalPositionTolerance.clear();
        TableTopLateralPositionTolerance.clear();
        TableTopPitchAngleTolerance.clear();
        TableTopRollAngleTolerance.clear();
        SnoutPositionTolerance.clear();
        HeadFixationAngleTolerance.clear();
        ChairHeadFramePositionTolerance.clear();
        FixationLightAzimuthalAngleTolerance.clear();
        FixationLightPolarAngleTolerance.clear();
    }
}


OFBool DRTIonToleranceTableSequence::Item::isEmpty()
{
    return ToleranceTableNumber.isEmpty() &&
           ToleranceTableLabel.isEmpty() &&
           GantryAngleTolerance.isEmpty() &&
           BeamLimitingDeviceAngleTolerance.isEmpty() &&
           BeamLimitingDeviceToleranceSequence.isEmpty() &&
           PatientSupportAngleTolerance.isEmpty() &&
           TableTopVerticalPositionTolerance.isEmpty() &&
           TableTopLongitudinalPositionTolerance.isEmpty() &&
           TableTopLateralPositionTolerance.isEmpty() &&
           TableTopPitchAngleTolerance.isEmpty() &&
           TableTopRollAngleTolerance.isEmpty() &&
           SnoutPositionTolerance.isEmpty() &&
           HeadFixationAngleTolerance.isEmpty() &&
           ChairHeadFramePositionTolerance.isEmpty() &&
           FixationLightAzimuthalAngleTolerance.isEmpty() &&
           FixationLightPolarAngleTolerance.isEmpty();
}


OFBool DRTIonToleranceTableSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTIonToleranceTableSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ToleranceTableNumber, "1", "1", "IonToleranceTableSequence");
        getAndCheckElementFromDataset(item, ToleranceTableLabel, "1", "3", "IonToleranceTableSequence");
        getAndCheckElementFromDataset(item, GantryAngleTolerance, "1", "3", "IonToleranceTableSequence");
        getAndCheckElementFromDataset(item, BeamLimitingDeviceAngleTolerance, "1", "3", "IonToleranceTableSequence");
        BeamLimitingDeviceToleranceSequence.read(item, "1-n", "3", "IonToleranceTableSequence");
        getAndCheckElementFromDataset(item, PatientSupportAngleTolerance, "1", "3", "IonToleranceTableSequence");
        getAndCheckElementFromDataset(item, TableTopVerticalPositionTolerance, "1", "3", "IonToleranceTableSequence");
        getAndCheckElementFromDataset(item, TableTopLongitudinalPositionTolerance, "1", "3", "IonToleranceTableSequence");
        getAndCheckElementFromDataset(item, TableTopLateralPositionTolerance, "1", "3", "IonToleranceTableSequence");
        getAndCheckElementFromDataset(item, TableTopPitchAngleTolerance, "1", "3", "IonToleranceTableSequence");
        getAndCheckElementFromDataset(item, TableTopRollAngleTolerance, "1", "3", "IonToleranceTableSequence");
        getAndCheckElementFromDataset(item, SnoutPositionTolerance, "1", "3", "IonToleranceTableSequence");
        getAndCheckElementFromDataset(item, HeadFixationAngleTolerance, "1", "3", "IonToleranceTableSequence");
        getAndCheckElementFromDataset(item, ChairHeadFramePositionTolerance, "1", "3", "IonToleranceTableSequence");
        getAndCheckElementFromDataset(item, FixationLightAzimuthalAngleTolerance, "1", "3", "IonToleranceTableSequence");
        getAndCheckElementFromDataset(item, FixationLightPolarAngleTolerance, "1", "3", "IonToleranceTableSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonToleranceTableSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ToleranceTableNumber), "1", "1", "IonToleranceTableSequence");
        addElementToDataset(result, item, new DcmShortString(ToleranceTableLabel), "1", "3", "IonToleranceTableSequence");
        addElementToDataset(result, item, new DcmDecimalString(GantryAngleTolerance), "1", "3", "IonToleranceTableSequence");
        addElementToDataset(result, item, new DcmDecimalString(BeamLimitingDeviceAngleTolerance), "1", "3", "IonToleranceTableSequence");
        if (result.good()) result = BeamLimitingDeviceToleranceSequence.write(item, "1-n", "3", "IonToleranceTableSequence");
        addElementToDataset(result, item, new DcmDecimalString(PatientSupportAngleTolerance), "1", "3", "IonToleranceTableSequence");
        addElementToDataset(result, item, new DcmDecimalString(TableTopVerticalPositionTolerance), "1", "3", "IonToleranceTableSequence");
        addElementToDataset(result, item, new DcmDecimalString(TableTopLongitudinalPositionTolerance), "1", "3", "IonToleranceTableSequence");
        addElementToDataset(result, item, new DcmDecimalString(TableTopLateralPositionTolerance), "1", "3", "IonToleranceTableSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(TableTopPitchAngleTolerance), "1", "3", "IonToleranceTableSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(TableTopRollAngleTolerance), "1", "3", "IonToleranceTableSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(SnoutPositionTolerance), "1", "3", "IonToleranceTableSequence");
        addElementToDataset(result, item, new DcmDecimalString(HeadFixationAngleTolerance), "1", "3", "IonToleranceTableSequence");
        addElementToDataset(result, item, new DcmDecimalString(ChairHeadFramePositionTolerance), "1", "3", "IonToleranceTableSequence");
        addElementToDataset(result, item, new DcmDecimalString(FixationLightAzimuthalAngleTolerance), "1", "3", "IonToleranceTableSequence");
        addElementToDataset(result, item, new DcmDecimalString(FixationLightPolarAngleTolerance), "1", "3", "IonToleranceTableSequence");
    }
    return result;
}


OFCondition DRTIonToleranceTableSequence::Item::getBeamLimitingDeviceAngleTolerance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamLimitingDeviceAngleTolerance, value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getBeamLimitingDeviceAngleTolerance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, BeamLimitingDeviceAngleTolerance).getFloat64(value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getChairHeadFramePositionTolerance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ChairHeadFramePositionTolerance, value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getChairHeadFramePositionTolerance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ChairHeadFramePositionTolerance).getFloat64(value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getFixationLightAzimuthalAngleTolerance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FixationLightAzimuthalAngleTolerance, value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getFixationLightAzimuthalAngleTolerance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, FixationLightAzimuthalAngleTolerance).getFloat64(value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getFixationLightPolarAngleTolerance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FixationLightPolarAngleTolerance, value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getFixationLightPolarAngleTolerance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, FixationLightPolarAngleTolerance).getFloat64(value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getGantryAngleTolerance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GantryAngleTolerance, value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getGantryAngleTolerance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, GantryAngleTolerance).getFloat64(value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getHeadFixationAngleTolerance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(HeadFixationAngleTolerance, value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getHeadFixationAngleTolerance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, HeadFixationAngleTolerance).getFloat64(value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getPatientSupportAngleTolerance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientSupportAngleTolerance, value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getPatientSupportAngleTolerance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, PatientSupportAngleTolerance).getFloat64(value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getSnoutPositionTolerance(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, SnoutPositionTolerance).getFloat32(value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getTableTopLateralPositionTolerance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopLateralPositionTolerance, value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getTableTopLateralPositionTolerance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TableTopLateralPositionTolerance).getFloat64(value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getTableTopLongitudinalPositionTolerance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopLongitudinalPositionTolerance, value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getTableTopLongitudinalPositionTolerance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TableTopLongitudinalPositionTolerance).getFloat64(value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getTableTopPitchAngleTolerance(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, TableTopPitchAngleTolerance).getFloat32(value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getTableTopRollAngleTolerance(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, TableTopRollAngleTolerance).getFloat32(value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getTableTopVerticalPositionTolerance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopVerticalPositionTolerance, value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getTableTopVerticalPositionTolerance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TableTopVerticalPositionTolerance).getFloat64(value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getToleranceTableLabel(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ToleranceTableLabel, value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getToleranceTableNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ToleranceTableNumber, value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::getToleranceTableNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ToleranceTableNumber).getSint32(value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::setBeamLimitingDeviceAngleTolerance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BeamLimitingDeviceAngleTolerance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonToleranceTableSequence::Item::setChairHeadFramePositionTolerance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ChairHeadFramePositionTolerance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonToleranceTableSequence::Item::setFixationLightAzimuthalAngleTolerance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = FixationLightAzimuthalAngleTolerance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonToleranceTableSequence::Item::setFixationLightPolarAngleTolerance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = FixationLightPolarAngleTolerance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonToleranceTableSequence::Item::setGantryAngleTolerance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = GantryAngleTolerance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonToleranceTableSequence::Item::setHeadFixationAngleTolerance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = HeadFixationAngleTolerance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonToleranceTableSequence::Item::setPatientSupportAngleTolerance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PatientSupportAngleTolerance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonToleranceTableSequence::Item::setSnoutPositionTolerance(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return SnoutPositionTolerance.putFloat32(value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::setTableTopLateralPositionTolerance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TableTopLateralPositionTolerance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonToleranceTableSequence::Item::setTableTopLongitudinalPositionTolerance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TableTopLongitudinalPositionTolerance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonToleranceTableSequence::Item::setTableTopPitchAngleTolerance(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return TableTopPitchAngleTolerance.putFloat32(value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::setTableTopRollAngleTolerance(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return TableTopRollAngleTolerance.putFloat32(value, pos);
}


OFCondition DRTIonToleranceTableSequence::Item::setTableTopVerticalPositionTolerance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TableTopVerticalPositionTolerance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonToleranceTableSequence::Item::setToleranceTableLabel(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ToleranceTableLabel.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonToleranceTableSequence::Item::setToleranceTableNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ToleranceTableNumber.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTIonToleranceTableSequence::DRTIonToleranceTableSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTIonToleranceTableSequence::DRTIonToleranceTableSequence(const DRTIonToleranceTableSequence &copy)
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


DRTIonToleranceTableSequence &DRTIonToleranceTableSequence::operator=(const DRTIonToleranceTableSequence &copy)
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


DRTIonToleranceTableSequence::~DRTIonToleranceTableSequence()
{
    clear();
}


void DRTIonToleranceTableSequence::clear()
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


OFBool DRTIonToleranceTableSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTIonToleranceTableSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTIonToleranceTableSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTIonToleranceTableSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonToleranceTableSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonToleranceTableSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTIonToleranceTableSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTIonToleranceTableSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTIonToleranceTableSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTIonToleranceTableSequence::Item &DRTIonToleranceTableSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTIonToleranceTableSequence::Item &DRTIonToleranceTableSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTIonToleranceTableSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTIonToleranceTableSequence::Item &DRTIonToleranceTableSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTIonToleranceTableSequence::Item &DRTIonToleranceTableSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTIonToleranceTableSequence::Item &DRTIonToleranceTableSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTIonToleranceTableSequence::Item &DRTIonToleranceTableSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTIonToleranceTableSequence::addItem(Item *&item)
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


OFCondition DRTIonToleranceTableSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTIonToleranceTableSequence::removeItem(const size_t pos)
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


OFCondition DRTIonToleranceTableSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_IonToleranceTableSequence, sequence);
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
            DcmSequenceOfItems element(DCM_IonToleranceTableSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTIonToleranceTableSequence::write(DcmItem &dataset,
                                                const OFString &card,
                                                const OFString &type,
                                                const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_IonToleranceTableSequence);
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
