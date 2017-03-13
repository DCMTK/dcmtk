/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTIonControlPointSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drticps.h"


// --- item class ---

DRTIonControlPointSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    BeamLimitingDeviceAngle(DCM_BeamLimitingDeviceAngle),
    BeamLimitingDevicePositionSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    BeamLimitingDeviceRotationDirection(DCM_BeamLimitingDeviceRotationDirection),
    ChairHeadFramePosition(DCM_ChairHeadFramePosition),
    ControlPointIndex(DCM_ControlPointIndex),
    CumulativeMetersetWeight(DCM_CumulativeMetersetWeight),
    ExternalContourEntryPoint(DCM_ExternalContourEntryPoint),
    GantryAngle(DCM_GantryAngle),
    GantryPitchAngle(DCM_GantryPitchAngle),
    GantryPitchRotationDirection(DCM_GantryPitchRotationDirection),
    GantryRotationDirection(DCM_GantryRotationDirection),
    HeadFixationAngle(DCM_HeadFixationAngle),
    IonWedgePositionSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    IsocenterPosition(DCM_IsocenterPosition),
    KVP(DCM_KVP),
    LateralSpreadingDeviceSettingsSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    MetersetRate(DCM_MetersetRate),
    NominalBeamEnergy(DCM_NominalBeamEnergy),
    NumberOfPaintings(DCM_NumberOfPaintings),
    NumberOfScanSpotPositions(DCM_NumberOfScanSpotPositions),
    PatientSupportAngle(DCM_PatientSupportAngle),
    PatientSupportRotationDirection(DCM_PatientSupportRotationDirection),
    RangeModulatorSettingsSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RangeShifterSettingsSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedDoseReferenceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ScanSpotMetersetWeights(DCM_ScanSpotMetersetWeights),
    ScanSpotPositionMap(DCM_ScanSpotPositionMap),
    ScanSpotReorderingAllowed(DCM_ScanSpotReorderingAllowed),
    ScanSpotTuneID(DCM_ScanSpotTuneID),
    ScanningSpotSize(DCM_ScanningSpotSize),
    SnoutPosition(DCM_SnoutPosition),
    SurfaceEntryPoint(DCM_SurfaceEntryPoint),
    TableTopLateralPosition(DCM_TableTopLateralPosition),
    TableTopLongitudinalPosition(DCM_TableTopLongitudinalPosition),
    TableTopPitchAngle(DCM_TableTopPitchAngle),
    TableTopPitchRotationDirection(DCM_TableTopPitchRotationDirection),
    TableTopRollAngle(DCM_TableTopRollAngle),
    TableTopRollRotationDirection(DCM_TableTopRollRotationDirection),
    TableTopVerticalPosition(DCM_TableTopVerticalPosition)
{
}


DRTIonControlPointSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    BeamLimitingDeviceAngle(copy.BeamLimitingDeviceAngle),
    BeamLimitingDevicePositionSequence(copy.BeamLimitingDevicePositionSequence),
    BeamLimitingDeviceRotationDirection(copy.BeamLimitingDeviceRotationDirection),
    ChairHeadFramePosition(copy.ChairHeadFramePosition),
    ControlPointIndex(copy.ControlPointIndex),
    CumulativeMetersetWeight(copy.CumulativeMetersetWeight),
    ExternalContourEntryPoint(copy.ExternalContourEntryPoint),
    GantryAngle(copy.GantryAngle),
    GantryPitchAngle(copy.GantryPitchAngle),
    GantryPitchRotationDirection(copy.GantryPitchRotationDirection),
    GantryRotationDirection(copy.GantryRotationDirection),
    HeadFixationAngle(copy.HeadFixationAngle),
    IonWedgePositionSequence(copy.IonWedgePositionSequence),
    IsocenterPosition(copy.IsocenterPosition),
    KVP(copy.KVP),
    LateralSpreadingDeviceSettingsSequence(copy.LateralSpreadingDeviceSettingsSequence),
    MetersetRate(copy.MetersetRate),
    NominalBeamEnergy(copy.NominalBeamEnergy),
    NumberOfPaintings(copy.NumberOfPaintings),
    NumberOfScanSpotPositions(copy.NumberOfScanSpotPositions),
    PatientSupportAngle(copy.PatientSupportAngle),
    PatientSupportRotationDirection(copy.PatientSupportRotationDirection),
    RangeModulatorSettingsSequence(copy.RangeModulatorSettingsSequence),
    RangeShifterSettingsSequence(copy.RangeShifterSettingsSequence),
    ReferencedDoseReferenceSequence(copy.ReferencedDoseReferenceSequence),
    ScanSpotMetersetWeights(copy.ScanSpotMetersetWeights),
    ScanSpotPositionMap(copy.ScanSpotPositionMap),
    ScanSpotReorderingAllowed(copy.ScanSpotReorderingAllowed),
    ScanSpotTuneID(copy.ScanSpotTuneID),
    ScanningSpotSize(copy.ScanningSpotSize),
    SnoutPosition(copy.SnoutPosition),
    SurfaceEntryPoint(copy.SurfaceEntryPoint),
    TableTopLateralPosition(copy.TableTopLateralPosition),
    TableTopLongitudinalPosition(copy.TableTopLongitudinalPosition),
    TableTopPitchAngle(copy.TableTopPitchAngle),
    TableTopPitchRotationDirection(copy.TableTopPitchRotationDirection),
    TableTopRollAngle(copy.TableTopRollAngle),
    TableTopRollRotationDirection(copy.TableTopRollRotationDirection),
    TableTopVerticalPosition(copy.TableTopVerticalPosition)
{
}


DRTIonControlPointSequence::Item::~Item()
{
}


DRTIonControlPointSequence::Item &DRTIonControlPointSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        BeamLimitingDeviceAngle = copy.BeamLimitingDeviceAngle;
        BeamLimitingDevicePositionSequence = copy.BeamLimitingDevicePositionSequence;
        BeamLimitingDeviceRotationDirection = copy.BeamLimitingDeviceRotationDirection;
        ChairHeadFramePosition = copy.ChairHeadFramePosition;
        ControlPointIndex = copy.ControlPointIndex;
        CumulativeMetersetWeight = copy.CumulativeMetersetWeight;
        ExternalContourEntryPoint = copy.ExternalContourEntryPoint;
        GantryAngle = copy.GantryAngle;
        GantryPitchAngle = copy.GantryPitchAngle;
        GantryPitchRotationDirection = copy.GantryPitchRotationDirection;
        GantryRotationDirection = copy.GantryRotationDirection;
        HeadFixationAngle = copy.HeadFixationAngle;
        IonWedgePositionSequence = copy.IonWedgePositionSequence;
        IsocenterPosition = copy.IsocenterPosition;
        KVP = copy.KVP;
        LateralSpreadingDeviceSettingsSequence = copy.LateralSpreadingDeviceSettingsSequence;
        MetersetRate = copy.MetersetRate;
        NominalBeamEnergy = copy.NominalBeamEnergy;
        NumberOfPaintings = copy.NumberOfPaintings;
        NumberOfScanSpotPositions = copy.NumberOfScanSpotPositions;
        PatientSupportAngle = copy.PatientSupportAngle;
        PatientSupportRotationDirection = copy.PatientSupportRotationDirection;
        RangeModulatorSettingsSequence = copy.RangeModulatorSettingsSequence;
        RangeShifterSettingsSequence = copy.RangeShifterSettingsSequence;
        ReferencedDoseReferenceSequence = copy.ReferencedDoseReferenceSequence;
        ScanSpotMetersetWeights = copy.ScanSpotMetersetWeights;
        ScanSpotPositionMap = copy.ScanSpotPositionMap;
        ScanSpotReorderingAllowed = copy.ScanSpotReorderingAllowed;
        ScanSpotTuneID = copy.ScanSpotTuneID;
        ScanningSpotSize = copy.ScanningSpotSize;
        SnoutPosition = copy.SnoutPosition;
        SurfaceEntryPoint = copy.SurfaceEntryPoint;
        TableTopLateralPosition = copy.TableTopLateralPosition;
        TableTopLongitudinalPosition = copy.TableTopLongitudinalPosition;
        TableTopPitchAngle = copy.TableTopPitchAngle;
        TableTopPitchRotationDirection = copy.TableTopPitchRotationDirection;
        TableTopRollAngle = copy.TableTopRollAngle;
        TableTopRollRotationDirection = copy.TableTopRollRotationDirection;
        TableTopVerticalPosition = copy.TableTopVerticalPosition;
    }
    return *this;
}


void DRTIonControlPointSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ControlPointIndex.clear();
        CumulativeMetersetWeight.clear();
        ReferencedDoseReferenceSequence.clear();
        NominalBeamEnergy.clear();
        KVP.clear();
        MetersetRate.clear();
        IonWedgePositionSequence.clear();
        RangeShifterSettingsSequence.clear();
        LateralSpreadingDeviceSettingsSequence.clear();
        RangeModulatorSettingsSequence.clear();
        BeamLimitingDevicePositionSequence.clear();
        GantryAngle.clear();
        GantryRotationDirection.clear();
        GantryPitchAngle.clear();
        GantryPitchRotationDirection.clear();
        BeamLimitingDeviceAngle.clear();
        BeamLimitingDeviceRotationDirection.clear();
        ScanSpotTuneID.clear();
        ScanSpotReorderingAllowed.clear();
        NumberOfScanSpotPositions.clear();
        ScanSpotPositionMap.clear();
        ScanSpotMetersetWeights.clear();
        ScanningSpotSize.clear();
        NumberOfPaintings.clear();
        PatientSupportAngle.clear();
        PatientSupportRotationDirection.clear();
        TableTopPitchAngle.clear();
        TableTopPitchRotationDirection.clear();
        TableTopRollAngle.clear();
        TableTopRollRotationDirection.clear();
        HeadFixationAngle.clear();
        ChairHeadFramePosition.clear();
        TableTopVerticalPosition.clear();
        TableTopLongitudinalPosition.clear();
        TableTopLateralPosition.clear();
        SnoutPosition.clear();
        IsocenterPosition.clear();
        SurfaceEntryPoint.clear();
        ExternalContourEntryPoint.clear();
    }
}


OFBool DRTIonControlPointSequence::Item::isEmpty()
{
    return ControlPointIndex.isEmpty() &&
           CumulativeMetersetWeight.isEmpty() &&
           ReferencedDoseReferenceSequence.isEmpty() &&
           NominalBeamEnergy.isEmpty() &&
           KVP.isEmpty() &&
           MetersetRate.isEmpty() &&
           IonWedgePositionSequence.isEmpty() &&
           RangeShifterSettingsSequence.isEmpty() &&
           LateralSpreadingDeviceSettingsSequence.isEmpty() &&
           RangeModulatorSettingsSequence.isEmpty() &&
           BeamLimitingDevicePositionSequence.isEmpty() &&
           GantryAngle.isEmpty() &&
           GantryRotationDirection.isEmpty() &&
           GantryPitchAngle.isEmpty() &&
           GantryPitchRotationDirection.isEmpty() &&
           BeamLimitingDeviceAngle.isEmpty() &&
           BeamLimitingDeviceRotationDirection.isEmpty() &&
           ScanSpotTuneID.isEmpty() &&
           ScanSpotReorderingAllowed.isEmpty() &&
           NumberOfScanSpotPositions.isEmpty() &&
           ScanSpotPositionMap.isEmpty() &&
           ScanSpotMetersetWeights.isEmpty() &&
           ScanningSpotSize.isEmpty() &&
           NumberOfPaintings.isEmpty() &&
           PatientSupportAngle.isEmpty() &&
           PatientSupportRotationDirection.isEmpty() &&
           TableTopPitchAngle.isEmpty() &&
           TableTopPitchRotationDirection.isEmpty() &&
           TableTopRollAngle.isEmpty() &&
           TableTopRollRotationDirection.isEmpty() &&
           HeadFixationAngle.isEmpty() &&
           ChairHeadFramePosition.isEmpty() &&
           TableTopVerticalPosition.isEmpty() &&
           TableTopLongitudinalPosition.isEmpty() &&
           TableTopLateralPosition.isEmpty() &&
           SnoutPosition.isEmpty() &&
           IsocenterPosition.isEmpty() &&
           SurfaceEntryPoint.isEmpty() &&
           ExternalContourEntryPoint.isEmpty();
}


OFBool DRTIonControlPointSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTIonControlPointSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ControlPointIndex, "1", "1", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, CumulativeMetersetWeight, "1", "2", "IonControlPointSequence");
        ReferencedDoseReferenceSequence.read(item, "1-n", "3", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, NominalBeamEnergy, "1", "1C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, KVP, "1", "1C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, MetersetRate, "1", "3", "IonControlPointSequence");
        IonWedgePositionSequence.read(item, "1-n", "1C", "IonControlPointSequence");
        RangeShifterSettingsSequence.read(item, "1-n", "1C", "IonControlPointSequence");
        LateralSpreadingDeviceSettingsSequence.read(item, "1-n", "1C", "IonControlPointSequence");
        RangeModulatorSettingsSequence.read(item, "1-n", "1C", "IonControlPointSequence");
        BeamLimitingDevicePositionSequence.read(item, "1-n", "1C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, GantryAngle, "1", "1C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, GantryRotationDirection, "1", "1C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, GantryPitchAngle, "1", "2C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, GantryPitchRotationDirection, "1", "2C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, BeamLimitingDeviceAngle, "1", "1C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, BeamLimitingDeviceRotationDirection, "1", "1C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, ScanSpotTuneID, "1", "1C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, ScanSpotReorderingAllowed, "1", "3", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, NumberOfScanSpotPositions, "1", "1C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, ScanSpotPositionMap, "1-n", "1C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, ScanSpotMetersetWeights, "1-n", "1C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, ScanningSpotSize, "2", "3", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, NumberOfPaintings, "1", "1C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, PatientSupportAngle, "1", "1C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, PatientSupportRotationDirection, "1", "1C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, TableTopPitchAngle, "1", "2C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, TableTopPitchRotationDirection, "1", "2C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, TableTopRollAngle, "1", "2C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, TableTopRollRotationDirection, "1", "2C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, HeadFixationAngle, "1", "3", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, ChairHeadFramePosition, "1", "3", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, TableTopVerticalPosition, "1", "2C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, TableTopLongitudinalPosition, "1", "2C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, TableTopLateralPosition, "1", "2C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, SnoutPosition, "1", "2C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, IsocenterPosition, "3", "2C", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, SurfaceEntryPoint, "3", "3", "IonControlPointSequence");
        getAndCheckElementFromDataset(item, ExternalContourEntryPoint, "3", "3", "IonControlPointSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonControlPointSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ControlPointIndex), "1", "1", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(CumulativeMetersetWeight), "1", "2", "IonControlPointSequence");
        if (result.good()) result = ReferencedDoseReferenceSequence.write(item, "1-n", "3", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(NominalBeamEnergy), "1", "1C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(KVP), "1", "1C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(MetersetRate), "1", "3", "IonControlPointSequence");
        if (result.good()) result = IonWedgePositionSequence.write(item, "1-n", "1C", "IonControlPointSequence");
        if (result.good()) result = RangeShifterSettingsSequence.write(item, "1-n", "1C", "IonControlPointSequence");
        if (result.good()) result = LateralSpreadingDeviceSettingsSequence.write(item, "1-n", "1C", "IonControlPointSequence");
        if (result.good()) result = RangeModulatorSettingsSequence.write(item, "1-n", "1C", "IonControlPointSequence");
        if (result.good()) result = BeamLimitingDevicePositionSequence.write(item, "1-n", "1C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(GantryAngle), "1", "1C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmCodeString(GantryRotationDirection), "1", "1C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(GantryPitchAngle), "1", "2C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmCodeString(GantryPitchRotationDirection), "1", "2C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(BeamLimitingDeviceAngle), "1", "1C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmCodeString(BeamLimitingDeviceRotationDirection), "1", "1C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmShortString(ScanSpotTuneID), "1", "1C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmCodeString(ScanSpotReorderingAllowed), "1", "3", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfScanSpotPositions), "1", "1C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(ScanSpotPositionMap), "1-n", "1C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(ScanSpotMetersetWeights), "1-n", "1C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(ScanningSpotSize), "2", "3", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfPaintings), "1", "1C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(PatientSupportAngle), "1", "1C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmCodeString(PatientSupportRotationDirection), "1", "1C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(TableTopPitchAngle), "1", "2C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmCodeString(TableTopPitchRotationDirection), "1", "2C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(TableTopRollAngle), "1", "2C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmCodeString(TableTopRollRotationDirection), "1", "2C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(HeadFixationAngle), "1", "3", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(ChairHeadFramePosition), "1", "3", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(TableTopVerticalPosition), "1", "2C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(TableTopLongitudinalPosition), "1", "2C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(TableTopLateralPosition), "1", "2C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(SnoutPosition), "1", "2C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(IsocenterPosition), "3", "2C", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmDecimalString(SurfaceEntryPoint), "3", "3", "IonControlPointSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(ExternalContourEntryPoint), "3", "3", "IonControlPointSequence");
    }
    return result;
}


OFCondition DRTIonControlPointSequence::Item::getBeamLimitingDeviceAngle(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamLimitingDeviceAngle, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getBeamLimitingDeviceAngle(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, BeamLimitingDeviceAngle).getFloat64(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getBeamLimitingDeviceRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamLimitingDeviceRotationDirection, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getChairHeadFramePosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ChairHeadFramePosition, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getChairHeadFramePosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ChairHeadFramePosition).getFloat64(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getControlPointIndex(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ControlPointIndex, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getControlPointIndex(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ControlPointIndex).getSint32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getCumulativeMetersetWeight(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CumulativeMetersetWeight, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getCumulativeMetersetWeight(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, CumulativeMetersetWeight).getFloat64(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getExternalContourEntryPoint(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, ExternalContourEntryPoint).getFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getGantryAngle(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GantryAngle, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getGantryAngle(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, GantryAngle).getFloat64(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getGantryPitchAngle(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, GantryPitchAngle).getFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getGantryPitchRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GantryPitchRotationDirection, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getGantryRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GantryRotationDirection, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getHeadFixationAngle(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, HeadFixationAngle).getFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getIsocenterPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(IsocenterPosition, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getIsocenterPosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, IsocenterPosition).getFloat64(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getIsocenterPosition(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, IsocenterPosition).getFloat64Vector(value);
}


OFCondition DRTIonControlPointSequence::Item::getKVP(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(KVP, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getKVP(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, KVP).getFloat64(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getMetersetRate(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, MetersetRate).getFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getNominalBeamEnergy(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NominalBeamEnergy, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getNominalBeamEnergy(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, NominalBeamEnergy).getFloat64(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getNumberOfPaintings(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfPaintings, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getNumberOfPaintings(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfPaintings).getSint32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getNumberOfScanSpotPositions(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfScanSpotPositions, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getNumberOfScanSpotPositions(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfScanSpotPositions).getSint32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getPatientSupportAngle(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientSupportAngle, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getPatientSupportAngle(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, PatientSupportAngle).getFloat64(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getPatientSupportRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientSupportRotationDirection, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getScanSpotMetersetWeights(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, ScanSpotMetersetWeights).getFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getScanSpotPositionMap(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, ScanSpotPositionMap).getFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getScanSpotReorderingAllowed(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ScanSpotReorderingAllowed, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getScanSpotTuneID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ScanSpotTuneID, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getScanningSpotSize(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, ScanningSpotSize).getFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getSnoutPosition(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, SnoutPosition).getFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getSurfaceEntryPoint(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SurfaceEntryPoint, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getSurfaceEntryPoint(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SurfaceEntryPoint).getFloat64(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getSurfaceEntryPoint(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SurfaceEntryPoint).getFloat64Vector(value);
}


OFCondition DRTIonControlPointSequence::Item::getTableTopLateralPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopLateralPosition, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getTableTopLateralPosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TableTopLateralPosition).getFloat64(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getTableTopLongitudinalPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopLongitudinalPosition, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getTableTopLongitudinalPosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TableTopLongitudinalPosition).getFloat64(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getTableTopPitchAngle(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, TableTopPitchAngle).getFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getTableTopPitchRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopPitchRotationDirection, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getTableTopRollAngle(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, TableTopRollAngle).getFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getTableTopRollRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopRollRotationDirection, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getTableTopVerticalPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopVerticalPosition, value, pos);
}


OFCondition DRTIonControlPointSequence::Item::getTableTopVerticalPosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TableTopVerticalPosition).getFloat64(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::setBeamLimitingDeviceAngle(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointSequence::Item::setBeamLimitingDeviceRotationDirection(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointSequence::Item::setChairHeadFramePosition(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ChairHeadFramePosition.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonControlPointSequence::Item::setControlPointIndex(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointSequence::Item::setCumulativeMetersetWeight(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointSequence::Item::setExternalContourEntryPoint(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ExternalContourEntryPoint.putFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::setGantryAngle(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointSequence::Item::setGantryPitchAngle(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return GantryPitchAngle.putFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::setGantryPitchRotationDirection(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointSequence::Item::setGantryRotationDirection(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointSequence::Item::setHeadFixationAngle(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return HeadFixationAngle.putFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::setIsocenterPosition(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointSequence::Item::setKVP(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = KVP.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonControlPointSequence::Item::setMetersetRate(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return MetersetRate.putFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::setNominalBeamEnergy(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointSequence::Item::setNumberOfPaintings(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfPaintings.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonControlPointSequence::Item::setNumberOfScanSpotPositions(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfScanSpotPositions.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonControlPointSequence::Item::setPatientSupportAngle(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointSequence::Item::setPatientSupportRotationDirection(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointSequence::Item::setScanSpotMetersetWeights(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ScanSpotMetersetWeights.putFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::setScanSpotPositionMap(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ScanSpotPositionMap.putFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::setScanSpotReorderingAllowed(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ScanSpotReorderingAllowed.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonControlPointSequence::Item::setScanSpotTuneID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ScanSpotTuneID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonControlPointSequence::Item::setScanningSpotSize(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ScanningSpotSize.putFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::setSnoutPosition(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return SnoutPosition.putFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::setSurfaceEntryPoint(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointSequence::Item::setTableTopLateralPosition(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointSequence::Item::setTableTopLongitudinalPosition(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointSequence::Item::setTableTopPitchAngle(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return TableTopPitchAngle.putFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::setTableTopPitchRotationDirection(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointSequence::Item::setTableTopRollAngle(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return TableTopRollAngle.putFloat32(value, pos);
}


OFCondition DRTIonControlPointSequence::Item::setTableTopRollRotationDirection(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointSequence::Item::setTableTopVerticalPosition(const OFString &value, const OFBool check)
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

DRTIonControlPointSequence::DRTIonControlPointSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTIonControlPointSequence::DRTIonControlPointSequence(const DRTIonControlPointSequence &copy)
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


DRTIonControlPointSequence &DRTIonControlPointSequence::operator=(const DRTIonControlPointSequence &copy)
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


DRTIonControlPointSequence::~DRTIonControlPointSequence()
{
    clear();
}


void DRTIonControlPointSequence::clear()
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


OFBool DRTIonControlPointSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTIonControlPointSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTIonControlPointSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTIonControlPointSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonControlPointSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonControlPointSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTIonControlPointSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTIonControlPointSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTIonControlPointSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTIonControlPointSequence::Item &DRTIonControlPointSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTIonControlPointSequence::Item &DRTIonControlPointSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTIonControlPointSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTIonControlPointSequence::Item &DRTIonControlPointSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTIonControlPointSequence::Item &DRTIonControlPointSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTIonControlPointSequence::Item &DRTIonControlPointSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTIonControlPointSequence::Item &DRTIonControlPointSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTIonControlPointSequence::addItem(Item *&item)
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


OFCondition DRTIonControlPointSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTIonControlPointSequence::removeItem(const size_t pos)
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


OFCondition DRTIonControlPointSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_IonControlPointSequence, sequence);
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
            DcmSequenceOfItems element(DCM_IonControlPointSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTIonControlPointSequence::write(DcmItem &dataset,
                                              const OFString &card,
                                              const OFString &type,
                                              const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_IonControlPointSequence);
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
