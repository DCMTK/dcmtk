/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTControlPointSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtcps.h"


// --- item class ---

DRTControlPointSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    BeamLimitingDeviceAngle(DCM_BeamLimitingDeviceAngle),
    BeamLimitingDevicePositionSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    BeamLimitingDeviceRotationDirection(DCM_BeamLimitingDeviceRotationDirection),
    ControlPointIndex(DCM_ControlPointIndex),
    CumulativeMetersetWeight(DCM_CumulativeMetersetWeight),
    DoseRateSet(DCM_DoseRateSet),
    ExternalContourEntryPoint(DCM_ExternalContourEntryPoint),
    GantryAngle(DCM_GantryAngle),
    GantryPitchAngle(DCM_GantryPitchAngle),
    GantryPitchRotationDirection(DCM_GantryPitchRotationDirection),
    GantryRotationDirection(DCM_GantryRotationDirection),
    IsocenterPosition(DCM_IsocenterPosition),
    NominalBeamEnergy(DCM_NominalBeamEnergy),
    PatientSupportAngle(DCM_PatientSupportAngle),
    PatientSupportRotationDirection(DCM_PatientSupportRotationDirection),
    ReferencedDoseReferenceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedDoseSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    SourceToExternalContourDistance(DCM_SourceToExternalContourDistance),
    SourceToSurfaceDistance(DCM_SourceToSurfaceDistance),
    SurfaceEntryPoint(DCM_SurfaceEntryPoint),
    TableTopEccentricAngle(DCM_TableTopEccentricAngle),
    TableTopEccentricAxisDistance(DCM_TableTopEccentricAxisDistance),
    TableTopEccentricRotationDirection(DCM_TableTopEccentricRotationDirection),
    TableTopLateralPosition(DCM_TableTopLateralPosition),
    TableTopLongitudinalPosition(DCM_TableTopLongitudinalPosition),
    TableTopPitchAngle(DCM_TableTopPitchAngle),
    TableTopPitchRotationDirection(DCM_TableTopPitchRotationDirection),
    TableTopRollAngle(DCM_TableTopRollAngle),
    TableTopRollRotationDirection(DCM_TableTopRollRotationDirection),
    TableTopVerticalPosition(DCM_TableTopVerticalPosition),
    WedgePositionSequence(emptyDefaultItem /*emptyDefaultSequence*/)
{
}


DRTControlPointSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    BeamLimitingDeviceAngle(copy.BeamLimitingDeviceAngle),
    BeamLimitingDevicePositionSequence(copy.BeamLimitingDevicePositionSequence),
    BeamLimitingDeviceRotationDirection(copy.BeamLimitingDeviceRotationDirection),
    ControlPointIndex(copy.ControlPointIndex),
    CumulativeMetersetWeight(copy.CumulativeMetersetWeight),
    DoseRateSet(copy.DoseRateSet),
    ExternalContourEntryPoint(copy.ExternalContourEntryPoint),
    GantryAngle(copy.GantryAngle),
    GantryPitchAngle(copy.GantryPitchAngle),
    GantryPitchRotationDirection(copy.GantryPitchRotationDirection),
    GantryRotationDirection(copy.GantryRotationDirection),
    IsocenterPosition(copy.IsocenterPosition),
    NominalBeamEnergy(copy.NominalBeamEnergy),
    PatientSupportAngle(copy.PatientSupportAngle),
    PatientSupportRotationDirection(copy.PatientSupportRotationDirection),
    ReferencedDoseReferenceSequence(copy.ReferencedDoseReferenceSequence),
    ReferencedDoseSequence(copy.ReferencedDoseSequence),
    SourceToExternalContourDistance(copy.SourceToExternalContourDistance),
    SourceToSurfaceDistance(copy.SourceToSurfaceDistance),
    SurfaceEntryPoint(copy.SurfaceEntryPoint),
    TableTopEccentricAngle(copy.TableTopEccentricAngle),
    TableTopEccentricAxisDistance(copy.TableTopEccentricAxisDistance),
    TableTopEccentricRotationDirection(copy.TableTopEccentricRotationDirection),
    TableTopLateralPosition(copy.TableTopLateralPosition),
    TableTopLongitudinalPosition(copy.TableTopLongitudinalPosition),
    TableTopPitchAngle(copy.TableTopPitchAngle),
    TableTopPitchRotationDirection(copy.TableTopPitchRotationDirection),
    TableTopRollAngle(copy.TableTopRollAngle),
    TableTopRollRotationDirection(copy.TableTopRollRotationDirection),
    TableTopVerticalPosition(copy.TableTopVerticalPosition),
    WedgePositionSequence(copy.WedgePositionSequence)
{
}


DRTControlPointSequence::Item::~Item()
{
}


DRTControlPointSequence::Item &DRTControlPointSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        BeamLimitingDeviceAngle = copy.BeamLimitingDeviceAngle;
        BeamLimitingDevicePositionSequence = copy.BeamLimitingDevicePositionSequence;
        BeamLimitingDeviceRotationDirection = copy.BeamLimitingDeviceRotationDirection;
        ControlPointIndex = copy.ControlPointIndex;
        CumulativeMetersetWeight = copy.CumulativeMetersetWeight;
        DoseRateSet = copy.DoseRateSet;
        ExternalContourEntryPoint = copy.ExternalContourEntryPoint;
        GantryAngle = copy.GantryAngle;
        GantryPitchAngle = copy.GantryPitchAngle;
        GantryPitchRotationDirection = copy.GantryPitchRotationDirection;
        GantryRotationDirection = copy.GantryRotationDirection;
        IsocenterPosition = copy.IsocenterPosition;
        NominalBeamEnergy = copy.NominalBeamEnergy;
        PatientSupportAngle = copy.PatientSupportAngle;
        PatientSupportRotationDirection = copy.PatientSupportRotationDirection;
        ReferencedDoseReferenceSequence = copy.ReferencedDoseReferenceSequence;
        ReferencedDoseSequence = copy.ReferencedDoseSequence;
        SourceToExternalContourDistance = copy.SourceToExternalContourDistance;
        SourceToSurfaceDistance = copy.SourceToSurfaceDistance;
        SurfaceEntryPoint = copy.SurfaceEntryPoint;
        TableTopEccentricAngle = copy.TableTopEccentricAngle;
        TableTopEccentricAxisDistance = copy.TableTopEccentricAxisDistance;
        TableTopEccentricRotationDirection = copy.TableTopEccentricRotationDirection;
        TableTopLateralPosition = copy.TableTopLateralPosition;
        TableTopLongitudinalPosition = copy.TableTopLongitudinalPosition;
        TableTopPitchAngle = copy.TableTopPitchAngle;
        TableTopPitchRotationDirection = copy.TableTopPitchRotationDirection;
        TableTopRollAngle = copy.TableTopRollAngle;
        TableTopRollRotationDirection = copy.TableTopRollRotationDirection;
        TableTopVerticalPosition = copy.TableTopVerticalPosition;
        WedgePositionSequence = copy.WedgePositionSequence;
    }
    return *this;
}


void DRTControlPointSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ControlPointIndex.clear();
        CumulativeMetersetWeight.clear();
        ReferencedDoseReferenceSequence.clear();
        ReferencedDoseSequence.clear();
        NominalBeamEnergy.clear();
        DoseRateSet.clear();
        WedgePositionSequence.clear();
        BeamLimitingDevicePositionSequence.clear();
        GantryAngle.clear();
        GantryRotationDirection.clear();
        GantryPitchAngle.clear();
        GantryPitchRotationDirection.clear();
        BeamLimitingDeviceAngle.clear();
        BeamLimitingDeviceRotationDirection.clear();
        PatientSupportAngle.clear();
        PatientSupportRotationDirection.clear();
        TableTopEccentricAxisDistance.clear();
        TableTopEccentricAngle.clear();
        TableTopEccentricRotationDirection.clear();
        TableTopPitchAngle.clear();
        TableTopPitchRotationDirection.clear();
        TableTopRollAngle.clear();
        TableTopRollRotationDirection.clear();
        TableTopVerticalPosition.clear();
        TableTopLongitudinalPosition.clear();
        TableTopLateralPosition.clear();
        IsocenterPosition.clear();
        SurfaceEntryPoint.clear();
        ExternalContourEntryPoint.clear();
        SourceToSurfaceDistance.clear();
        SourceToExternalContourDistance.clear();
    }
}


OFBool DRTControlPointSequence::Item::isEmpty()
{
    return ControlPointIndex.isEmpty() &&
           CumulativeMetersetWeight.isEmpty() &&
           ReferencedDoseReferenceSequence.isEmpty() &&
           ReferencedDoseSequence.isEmpty() &&
           NominalBeamEnergy.isEmpty() &&
           DoseRateSet.isEmpty() &&
           WedgePositionSequence.isEmpty() &&
           BeamLimitingDevicePositionSequence.isEmpty() &&
           GantryAngle.isEmpty() &&
           GantryRotationDirection.isEmpty() &&
           GantryPitchAngle.isEmpty() &&
           GantryPitchRotationDirection.isEmpty() &&
           BeamLimitingDeviceAngle.isEmpty() &&
           BeamLimitingDeviceRotationDirection.isEmpty() &&
           PatientSupportAngle.isEmpty() &&
           PatientSupportRotationDirection.isEmpty() &&
           TableTopEccentricAxisDistance.isEmpty() &&
           TableTopEccentricAngle.isEmpty() &&
           TableTopEccentricRotationDirection.isEmpty() &&
           TableTopPitchAngle.isEmpty() &&
           TableTopPitchRotationDirection.isEmpty() &&
           TableTopRollAngle.isEmpty() &&
           TableTopRollRotationDirection.isEmpty() &&
           TableTopVerticalPosition.isEmpty() &&
           TableTopLongitudinalPosition.isEmpty() &&
           TableTopLateralPosition.isEmpty() &&
           IsocenterPosition.isEmpty() &&
           SurfaceEntryPoint.isEmpty() &&
           ExternalContourEntryPoint.isEmpty() &&
           SourceToSurfaceDistance.isEmpty() &&
           SourceToExternalContourDistance.isEmpty();
}


OFBool DRTControlPointSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTControlPointSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ControlPointIndex, "1", "1", "ControlPointSequence");
        getAndCheckElementFromDataset(item, CumulativeMetersetWeight, "1", "2", "ControlPointSequence");
        ReferencedDoseReferenceSequence.read(item, "1-n", "3", "ControlPointSequence");
        ReferencedDoseSequence.read(item, "1-n", "1C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, NominalBeamEnergy, "1", "3", "ControlPointSequence");
        getAndCheckElementFromDataset(item, DoseRateSet, "1", "3", "ControlPointSequence");
        WedgePositionSequence.read(item, "1-n", "1C", "ControlPointSequence");
        BeamLimitingDevicePositionSequence.read(item, "1-n", "1C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, GantryAngle, "1", "1C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, GantryRotationDirection, "1", "1C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, GantryPitchAngle, "1", "3", "ControlPointSequence");
        getAndCheckElementFromDataset(item, GantryPitchRotationDirection, "1", "3", "ControlPointSequence");
        getAndCheckElementFromDataset(item, BeamLimitingDeviceAngle, "1", "1C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, BeamLimitingDeviceRotationDirection, "1", "1C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, PatientSupportAngle, "1", "1C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, PatientSupportRotationDirection, "1", "1C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, TableTopEccentricAxisDistance, "1", "3", "ControlPointSequence");
        getAndCheckElementFromDataset(item, TableTopEccentricAngle, "1", "1C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, TableTopEccentricRotationDirection, "1", "1C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, TableTopPitchAngle, "1", "1C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, TableTopPitchRotationDirection, "1", "1C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, TableTopRollAngle, "1", "1C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, TableTopRollRotationDirection, "1", "1C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, TableTopVerticalPosition, "1", "2C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, TableTopLongitudinalPosition, "1", "2C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, TableTopLateralPosition, "1", "2C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, IsocenterPosition, "3", "2C", "ControlPointSequence");
        getAndCheckElementFromDataset(item, SurfaceEntryPoint, "3", "3", "ControlPointSequence");
        getAndCheckElementFromDataset(item, ExternalContourEntryPoint, "3", "3", "ControlPointSequence");
        getAndCheckElementFromDataset(item, SourceToSurfaceDistance, "1", "3", "ControlPointSequence");
        getAndCheckElementFromDataset(item, SourceToExternalContourDistance, "1", "3", "ControlPointSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ControlPointIndex), "1", "1", "ControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(CumulativeMetersetWeight), "1", "2", "ControlPointSequence");
        if (result.good()) result = ReferencedDoseReferenceSequence.write(item, "1-n", "3", "ControlPointSequence");
        if (result.good()) result = ReferencedDoseSequence.write(item, "1-n", "1C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(NominalBeamEnergy), "1", "3", "ControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(DoseRateSet), "1", "3", "ControlPointSequence");
        if (result.good()) result = WedgePositionSequence.write(item, "1-n", "1C", "ControlPointSequence");
        if (result.good()) result = BeamLimitingDevicePositionSequence.write(item, "1-n", "1C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(GantryAngle), "1", "1C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmCodeString(GantryRotationDirection), "1", "1C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(GantryPitchAngle), "1", "3", "ControlPointSequence");
        addElementToDataset(result, item, new DcmCodeString(GantryPitchRotationDirection), "1", "3", "ControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(BeamLimitingDeviceAngle), "1", "1C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmCodeString(BeamLimitingDeviceRotationDirection), "1", "1C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(PatientSupportAngle), "1", "1C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmCodeString(PatientSupportRotationDirection), "1", "1C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(TableTopEccentricAxisDistance), "1", "3", "ControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(TableTopEccentricAngle), "1", "1C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmCodeString(TableTopEccentricRotationDirection), "1", "1C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(TableTopPitchAngle), "1", "1C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmCodeString(TableTopPitchRotationDirection), "1", "1C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(TableTopRollAngle), "1", "1C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmCodeString(TableTopRollRotationDirection), "1", "1C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(TableTopVerticalPosition), "1", "2C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(TableTopLongitudinalPosition), "1", "2C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(TableTopLateralPosition), "1", "2C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(IsocenterPosition), "3", "2C", "ControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(SurfaceEntryPoint), "3", "3", "ControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(ExternalContourEntryPoint), "3", "3", "ControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(SourceToSurfaceDistance), "1", "3", "ControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(SourceToExternalContourDistance), "1", "3", "ControlPointSequence");
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::getBeamLimitingDeviceAngle(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamLimitingDeviceAngle, value, pos);
}


OFCondition DRTControlPointSequence::Item::getBeamLimitingDeviceAngle(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, BeamLimitingDeviceAngle).getFloat64(value, pos);
}


OFCondition DRTControlPointSequence::Item::getBeamLimitingDeviceRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamLimitingDeviceRotationDirection, value, pos);
}


OFCondition DRTControlPointSequence::Item::getControlPointIndex(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ControlPointIndex, value, pos);
}


OFCondition DRTControlPointSequence::Item::getControlPointIndex(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ControlPointIndex).getSint32(value, pos);
}


OFCondition DRTControlPointSequence::Item::getCumulativeMetersetWeight(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CumulativeMetersetWeight, value, pos);
}


OFCondition DRTControlPointSequence::Item::getCumulativeMetersetWeight(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CumulativeMetersetWeight).getFloat64(value, pos);
}


OFCondition DRTControlPointSequence::Item::getDoseRateSet(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DoseRateSet, value, pos);
}


OFCondition DRTControlPointSequence::Item::getDoseRateSet(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DoseRateSet).getFloat64(value, pos);
}


OFCondition DRTControlPointSequence::Item::getExternalContourEntryPoint(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, ExternalContourEntryPoint).getFloat32(value, pos);
}


OFCondition DRTControlPointSequence::Item::getGantryAngle(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GantryAngle, value, pos);
}


OFCondition DRTControlPointSequence::Item::getGantryAngle(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, GantryAngle).getFloat64(value, pos);
}


OFCondition DRTControlPointSequence::Item::getGantryPitchAngle(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, GantryPitchAngle).getFloat32(value, pos);
}


OFCondition DRTControlPointSequence::Item::getGantryPitchRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GantryPitchRotationDirection, value, pos);
}


OFCondition DRTControlPointSequence::Item::getGantryRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GantryRotationDirection, value, pos);
}


OFCondition DRTControlPointSequence::Item::getIsocenterPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(IsocenterPosition, value, pos);
}


OFCondition DRTControlPointSequence::Item::getIsocenterPosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, IsocenterPosition).getFloat64(value, pos);
}


OFCondition DRTControlPointSequence::Item::getIsocenterPosition(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, IsocenterPosition).getFloat64Vector(value);
}


OFCondition DRTControlPointSequence::Item::getNominalBeamEnergy(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NominalBeamEnergy, value, pos);
}


OFCondition DRTControlPointSequence::Item::getNominalBeamEnergy(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, NominalBeamEnergy).getFloat64(value, pos);
}


OFCondition DRTControlPointSequence::Item::getPatientSupportAngle(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientSupportAngle, value, pos);
}


OFCondition DRTControlPointSequence::Item::getPatientSupportAngle(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, PatientSupportAngle).getFloat64(value, pos);
}


OFCondition DRTControlPointSequence::Item::getPatientSupportRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientSupportRotationDirection, value, pos);
}


OFCondition DRTControlPointSequence::Item::getSourceToExternalContourDistance(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, SourceToExternalContourDistance).getFloat32(value, pos);
}


OFCondition DRTControlPointSequence::Item::getSourceToSurfaceDistance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceToSurfaceDistance, value, pos);
}


OFCondition DRTControlPointSequence::Item::getSourceToSurfaceDistance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SourceToSurfaceDistance).getFloat64(value, pos);
}


OFCondition DRTControlPointSequence::Item::getSurfaceEntryPoint(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SurfaceEntryPoint, value, pos);
}


OFCondition DRTControlPointSequence::Item::getSurfaceEntryPoint(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SurfaceEntryPoint).getFloat64(value, pos);
}


OFCondition DRTControlPointSequence::Item::getSurfaceEntryPoint(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SurfaceEntryPoint).getFloat64Vector(value);
}


OFCondition DRTControlPointSequence::Item::getTableTopEccentricAngle(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopEccentricAngle, value, pos);
}


OFCondition DRTControlPointSequence::Item::getTableTopEccentricAngle(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TableTopEccentricAngle).getFloat64(value, pos);
}


OFCondition DRTControlPointSequence::Item::getTableTopEccentricAxisDistance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopEccentricAxisDistance, value, pos);
}


OFCondition DRTControlPointSequence::Item::getTableTopEccentricAxisDistance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TableTopEccentricAxisDistance).getFloat64(value, pos);
}


OFCondition DRTControlPointSequence::Item::getTableTopEccentricRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopEccentricRotationDirection, value, pos);
}


OFCondition DRTControlPointSequence::Item::getTableTopLateralPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopLateralPosition, value, pos);
}


OFCondition DRTControlPointSequence::Item::getTableTopLateralPosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TableTopLateralPosition).getFloat64(value, pos);
}


OFCondition DRTControlPointSequence::Item::getTableTopLongitudinalPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopLongitudinalPosition, value, pos);
}


OFCondition DRTControlPointSequence::Item::getTableTopLongitudinalPosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TableTopLongitudinalPosition).getFloat64(value, pos);
}


OFCondition DRTControlPointSequence::Item::getTableTopPitchAngle(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, TableTopPitchAngle).getFloat32(value, pos);
}


OFCondition DRTControlPointSequence::Item::getTableTopPitchRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopPitchRotationDirection, value, pos);
}


OFCondition DRTControlPointSequence::Item::getTableTopRollAngle(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, TableTopRollAngle).getFloat32(value, pos);
}


OFCondition DRTControlPointSequence::Item::getTableTopRollRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopRollRotationDirection, value, pos);
}


OFCondition DRTControlPointSequence::Item::getTableTopVerticalPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopVerticalPosition, value, pos);
}


OFCondition DRTControlPointSequence::Item::getTableTopVerticalPosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TableTopVerticalPosition).getFloat64(value, pos);
}


OFCondition DRTControlPointSequence::Item::setBeamLimitingDeviceAngle(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BeamLimitingDeviceAngle.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setBeamLimitingDeviceRotationDirection(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BeamLimitingDeviceRotationDirection.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setControlPointIndex(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ControlPointIndex.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setCumulativeMetersetWeight(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CumulativeMetersetWeight.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setDoseRateSet(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DoseRateSet.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setExternalContourEntryPoint(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ExternalContourEntryPoint.putFloat32(value, pos);
}


OFCondition DRTControlPointSequence::Item::setGantryAngle(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = GantryAngle.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setGantryPitchAngle(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return GantryPitchAngle.putFloat32(value, pos);
}


OFCondition DRTControlPointSequence::Item::setGantryPitchRotationDirection(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = GantryPitchRotationDirection.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setGantryRotationDirection(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = GantryRotationDirection.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setIsocenterPosition(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "3") : EC_Normal;
        if (result.good())
            result = IsocenterPosition.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setNominalBeamEnergy(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NominalBeamEnergy.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setPatientSupportAngle(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PatientSupportAngle.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setPatientSupportRotationDirection(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PatientSupportRotationDirection.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setSourceToExternalContourDistance(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return SourceToExternalContourDistance.putFloat32(value, pos);
}


OFCondition DRTControlPointSequence::Item::setSourceToSurfaceDistance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceToSurfaceDistance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setSurfaceEntryPoint(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "3") : EC_Normal;
        if (result.good())
            result = SurfaceEntryPoint.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setTableTopEccentricAngle(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TableTopEccentricAngle.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setTableTopEccentricAxisDistance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TableTopEccentricAxisDistance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setTableTopEccentricRotationDirection(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TableTopEccentricRotationDirection.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setTableTopLateralPosition(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TableTopLateralPosition.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setTableTopLongitudinalPosition(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TableTopLongitudinalPosition.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setTableTopPitchAngle(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return TableTopPitchAngle.putFloat32(value, pos);
}


OFCondition DRTControlPointSequence::Item::setTableTopPitchRotationDirection(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TableTopPitchRotationDirection.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setTableTopRollAngle(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return TableTopRollAngle.putFloat32(value, pos);
}


OFCondition DRTControlPointSequence::Item::setTableTopRollRotationDirection(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TableTopRollRotationDirection.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTControlPointSequence::Item::setTableTopVerticalPosition(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TableTopVerticalPosition.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTControlPointSequence::DRTControlPointSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTControlPointSequence::DRTControlPointSequence(const DRTControlPointSequence &copy)
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


DRTControlPointSequence &DRTControlPointSequence::operator=(const DRTControlPointSequence &copy)
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


DRTControlPointSequence::~DRTControlPointSequence()
{
    clear();
}


void DRTControlPointSequence::clear()
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


OFBool DRTControlPointSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTControlPointSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTControlPointSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTControlPointSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTControlPointSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTControlPointSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTControlPointSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTControlPointSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTControlPointSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTControlPointSequence::Item &DRTControlPointSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTControlPointSequence::Item &DRTControlPointSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTControlPointSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTControlPointSequence::Item &DRTControlPointSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTControlPointSequence::Item &DRTControlPointSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTControlPointSequence::Item &DRTControlPointSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTControlPointSequence::Item &DRTControlPointSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTControlPointSequence::addItem(Item *&item)
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


OFCondition DRTControlPointSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTControlPointSequence::removeItem(const unsigned long pos)
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


OFCondition DRTControlPointSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ControlPointSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ControlPointSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTControlPointSequence::write(DcmItem &dataset,
                                           const OFString &card,
                                           const OFString &type,
                                           const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ControlPointSequence);
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
