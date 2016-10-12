/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTIonControlPointDeliverySequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drticpds.h"


// --- item class ---

DRTIonControlPointDeliverySequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    BeamLimitingDeviceAngle(DCM_BeamLimitingDeviceAngle),
    BeamLimitingDevicePositionSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    BeamLimitingDeviceRotationDirection(DCM_BeamLimitingDeviceRotationDirection),
    ChairHeadFramePosition(DCM_ChairHeadFramePosition),
    CorrectedParameterSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    DeliveredMeterset(DCM_DeliveredMeterset),
    GantryAngle(DCM_GantryAngle),
    GantryPitchAngle(DCM_GantryPitchAngle),
    GantryPitchRotationDirection(DCM_GantryPitchRotationDirection),
    GantryRotationDirection(DCM_GantryRotationDirection),
    HeadFixationAngle(DCM_HeadFixationAngle),
    IonWedgePositionSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    KVP(DCM_KVP),
    LateralSpreadingDeviceSettingsSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    MetersetRateDelivered(DCM_MetersetRateDelivered),
    MetersetRateSet(DCM_MetersetRateSet),
    NominalBeamEnergy(DCM_NominalBeamEnergy),
    NumberOfPaintings(DCM_NumberOfPaintings),
    NumberOfScanSpotPositions(DCM_NumberOfScanSpotPositions),
    OverrideSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    PatientSupportAngle(DCM_PatientSupportAngle),
    PatientSupportRotationDirection(DCM_PatientSupportRotationDirection),
    RangeModulatorSettingsSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RangeShifterSettingsSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedControlPointIndex(DCM_ReferencedControlPointIndex),
    ScanSpotMetersetsDelivered(DCM_ScanSpotMetersetsDelivered),
    ScanSpotPositionMap(DCM_ScanSpotPositionMap),
    ScanSpotPrescribedIndices(DCM_ScanSpotPrescribedIndices),
    ScanSpotReordered(DCM_ScanSpotReordered),
    ScanSpotTuneID(DCM_ScanSpotTuneID),
    ScanningSpotSize(DCM_ScanningSpotSize),
    SnoutPosition(DCM_SnoutPosition),
    SpecifiedMeterset(DCM_SpecifiedMeterset),
    TableTopLateralPosition(DCM_TableTopLateralPosition),
    TableTopLongitudinalPosition(DCM_TableTopLongitudinalPosition),
    TableTopPitchAngle(DCM_TableTopPitchAngle),
    TableTopPitchRotationDirection(DCM_TableTopPitchRotationDirection),
    TableTopRollAngle(DCM_TableTopRollAngle),
    TableTopRollRotationDirection(DCM_TableTopRollRotationDirection),
    TableTopVerticalPosition(DCM_TableTopVerticalPosition),
    TreatmentControlPointDate(DCM_TreatmentControlPointDate),
    TreatmentControlPointTime(DCM_TreatmentControlPointTime)
{
}


DRTIonControlPointDeliverySequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    BeamLimitingDeviceAngle(copy.BeamLimitingDeviceAngle),
    BeamLimitingDevicePositionSequence(copy.BeamLimitingDevicePositionSequence),
    BeamLimitingDeviceRotationDirection(copy.BeamLimitingDeviceRotationDirection),
    ChairHeadFramePosition(copy.ChairHeadFramePosition),
    CorrectedParameterSequence(copy.CorrectedParameterSequence),
    DeliveredMeterset(copy.DeliveredMeterset),
    GantryAngle(copy.GantryAngle),
    GantryPitchAngle(copy.GantryPitchAngle),
    GantryPitchRotationDirection(copy.GantryPitchRotationDirection),
    GantryRotationDirection(copy.GantryRotationDirection),
    HeadFixationAngle(copy.HeadFixationAngle),
    IonWedgePositionSequence(copy.IonWedgePositionSequence),
    KVP(copy.KVP),
    LateralSpreadingDeviceSettingsSequence(copy.LateralSpreadingDeviceSettingsSequence),
    MetersetRateDelivered(copy.MetersetRateDelivered),
    MetersetRateSet(copy.MetersetRateSet),
    NominalBeamEnergy(copy.NominalBeamEnergy),
    NumberOfPaintings(copy.NumberOfPaintings),
    NumberOfScanSpotPositions(copy.NumberOfScanSpotPositions),
    OverrideSequence(copy.OverrideSequence),
    PatientSupportAngle(copy.PatientSupportAngle),
    PatientSupportRotationDirection(copy.PatientSupportRotationDirection),
    RangeModulatorSettingsSequence(copy.RangeModulatorSettingsSequence),
    RangeShifterSettingsSequence(copy.RangeShifterSettingsSequence),
    ReferencedControlPointIndex(copy.ReferencedControlPointIndex),
    ScanSpotMetersetsDelivered(copy.ScanSpotMetersetsDelivered),
    ScanSpotPositionMap(copy.ScanSpotPositionMap),
    ScanSpotPrescribedIndices(copy.ScanSpotPrescribedIndices),
    ScanSpotReordered(copy.ScanSpotReordered),
    ScanSpotTuneID(copy.ScanSpotTuneID),
    ScanningSpotSize(copy.ScanningSpotSize),
    SnoutPosition(copy.SnoutPosition),
    SpecifiedMeterset(copy.SpecifiedMeterset),
    TableTopLateralPosition(copy.TableTopLateralPosition),
    TableTopLongitudinalPosition(copy.TableTopLongitudinalPosition),
    TableTopPitchAngle(copy.TableTopPitchAngle),
    TableTopPitchRotationDirection(copy.TableTopPitchRotationDirection),
    TableTopRollAngle(copy.TableTopRollAngle),
    TableTopRollRotationDirection(copy.TableTopRollRotationDirection),
    TableTopVerticalPosition(copy.TableTopVerticalPosition),
    TreatmentControlPointDate(copy.TreatmentControlPointDate),
    TreatmentControlPointTime(copy.TreatmentControlPointTime)
{
}


DRTIonControlPointDeliverySequence::Item::~Item()
{
}


DRTIonControlPointDeliverySequence::Item &DRTIonControlPointDeliverySequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        BeamLimitingDeviceAngle = copy.BeamLimitingDeviceAngle;
        BeamLimitingDevicePositionSequence = copy.BeamLimitingDevicePositionSequence;
        BeamLimitingDeviceRotationDirection = copy.BeamLimitingDeviceRotationDirection;
        ChairHeadFramePosition = copy.ChairHeadFramePosition;
        CorrectedParameterSequence = copy.CorrectedParameterSequence;
        DeliveredMeterset = copy.DeliveredMeterset;
        GantryAngle = copy.GantryAngle;
        GantryPitchAngle = copy.GantryPitchAngle;
        GantryPitchRotationDirection = copy.GantryPitchRotationDirection;
        GantryRotationDirection = copy.GantryRotationDirection;
        HeadFixationAngle = copy.HeadFixationAngle;
        IonWedgePositionSequence = copy.IonWedgePositionSequence;
        KVP = copy.KVP;
        LateralSpreadingDeviceSettingsSequence = copy.LateralSpreadingDeviceSettingsSequence;
        MetersetRateDelivered = copy.MetersetRateDelivered;
        MetersetRateSet = copy.MetersetRateSet;
        NominalBeamEnergy = copy.NominalBeamEnergy;
        NumberOfPaintings = copy.NumberOfPaintings;
        NumberOfScanSpotPositions = copy.NumberOfScanSpotPositions;
        OverrideSequence = copy.OverrideSequence;
        PatientSupportAngle = copy.PatientSupportAngle;
        PatientSupportRotationDirection = copy.PatientSupportRotationDirection;
        RangeModulatorSettingsSequence = copy.RangeModulatorSettingsSequence;
        RangeShifterSettingsSequence = copy.RangeShifterSettingsSequence;
        ReferencedControlPointIndex = copy.ReferencedControlPointIndex;
        ScanSpotMetersetsDelivered = copy.ScanSpotMetersetsDelivered;
        ScanSpotPositionMap = copy.ScanSpotPositionMap;
        ScanSpotPrescribedIndices = copy.ScanSpotPrescribedIndices;
        ScanSpotReordered = copy.ScanSpotReordered;
        ScanSpotTuneID = copy.ScanSpotTuneID;
        ScanningSpotSize = copy.ScanningSpotSize;
        SnoutPosition = copy.SnoutPosition;
        SpecifiedMeterset = copy.SpecifiedMeterset;
        TableTopLateralPosition = copy.TableTopLateralPosition;
        TableTopLongitudinalPosition = copy.TableTopLongitudinalPosition;
        TableTopPitchAngle = copy.TableTopPitchAngle;
        TableTopPitchRotationDirection = copy.TableTopPitchRotationDirection;
        TableTopRollAngle = copy.TableTopRollAngle;
        TableTopRollRotationDirection = copy.TableTopRollRotationDirection;
        TableTopVerticalPosition = copy.TableTopVerticalPosition;
        TreatmentControlPointDate = copy.TreatmentControlPointDate;
        TreatmentControlPointTime = copy.TreatmentControlPointTime;
    }
    return *this;
}


void DRTIonControlPointDeliverySequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ReferencedControlPointIndex.clear();
        TreatmentControlPointDate.clear();
        TreatmentControlPointTime.clear();
        SpecifiedMeterset.clear();
        DeliveredMeterset.clear();
        MetersetRateSet.clear();
        MetersetRateDelivered.clear();
        NominalBeamEnergy.clear();
        KVP.clear();
        IonWedgePositionSequence.clear();
        BeamLimitingDevicePositionSequence.clear();
        RangeShifterSettingsSequence.clear();
        LateralSpreadingDeviceSettingsSequence.clear();
        RangeModulatorSettingsSequence.clear();
        GantryAngle.clear();
        GantryRotationDirection.clear();
        GantryPitchAngle.clear();
        GantryPitchRotationDirection.clear();
        BeamLimitingDeviceAngle.clear();
        BeamLimitingDeviceRotationDirection.clear();
        ScanSpotTuneID.clear();
        NumberOfScanSpotPositions.clear();
        ScanSpotPositionMap.clear();
        ScanSpotMetersetsDelivered.clear();
        ScanningSpotSize.clear();
        NumberOfPaintings.clear();
        ScanSpotReordered.clear();
        ScanSpotPrescribedIndices.clear();
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
        CorrectedParameterSequence.clear();
        OverrideSequence.clear();
    }
}


OFBool DRTIonControlPointDeliverySequence::Item::isEmpty()
{
    return ReferencedControlPointIndex.isEmpty() &&
           TreatmentControlPointDate.isEmpty() &&
           TreatmentControlPointTime.isEmpty() &&
           SpecifiedMeterset.isEmpty() &&
           DeliveredMeterset.isEmpty() &&
           MetersetRateSet.isEmpty() &&
           MetersetRateDelivered.isEmpty() &&
           NominalBeamEnergy.isEmpty() &&
           KVP.isEmpty() &&
           IonWedgePositionSequence.isEmpty() &&
           BeamLimitingDevicePositionSequence.isEmpty() &&
           RangeShifterSettingsSequence.isEmpty() &&
           LateralSpreadingDeviceSettingsSequence.isEmpty() &&
           RangeModulatorSettingsSequence.isEmpty() &&
           GantryAngle.isEmpty() &&
           GantryRotationDirection.isEmpty() &&
           GantryPitchAngle.isEmpty() &&
           GantryPitchRotationDirection.isEmpty() &&
           BeamLimitingDeviceAngle.isEmpty() &&
           BeamLimitingDeviceRotationDirection.isEmpty() &&
           ScanSpotTuneID.isEmpty() &&
           NumberOfScanSpotPositions.isEmpty() &&
           ScanSpotPositionMap.isEmpty() &&
           ScanSpotMetersetsDelivered.isEmpty() &&
           ScanningSpotSize.isEmpty() &&
           NumberOfPaintings.isEmpty() &&
           ScanSpotReordered.isEmpty() &&
           ScanSpotPrescribedIndices.isEmpty() &&
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
           CorrectedParameterSequence.isEmpty() &&
           OverrideSequence.isEmpty();
}


OFBool DRTIonControlPointDeliverySequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTIonControlPointDeliverySequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ReferencedControlPointIndex, "1", "1", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, TreatmentControlPointDate, "1", "1", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, TreatmentControlPointTime, "1", "1", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, SpecifiedMeterset, "1", "2", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, DeliveredMeterset, "1", "1", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, MetersetRateSet, "1", "3", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, MetersetRateDelivered, "1", "3", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, NominalBeamEnergy, "1", "1C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, KVP, "1", "1C", "IonControlPointDeliverySequence");
        IonWedgePositionSequence.read(item, "1-n", "1C", "IonControlPointDeliverySequence");
        BeamLimitingDevicePositionSequence.read(item, "1-n", "1C", "IonControlPointDeliverySequence");
        RangeShifterSettingsSequence.read(item, "1-n", "1C", "IonControlPointDeliverySequence");
        LateralSpreadingDeviceSettingsSequence.read(item, "1-n", "1C", "IonControlPointDeliverySequence");
        RangeModulatorSettingsSequence.read(item, "1-n", "1C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, GantryAngle, "1", "1C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, GantryRotationDirection, "1", "1C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, GantryPitchAngle, "1", "2C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, GantryPitchRotationDirection, "1", "2C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, BeamLimitingDeviceAngle, "1", "1C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, BeamLimitingDeviceRotationDirection, "1", "1C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, ScanSpotTuneID, "1", "1C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, NumberOfScanSpotPositions, "1", "1C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, ScanSpotPositionMap, "1-n", "1C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, ScanSpotMetersetsDelivered, "1-n", "1C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, ScanningSpotSize, "2", "3", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, NumberOfPaintings, "1", "1C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, ScanSpotReordered, "1", "3", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, ScanSpotPrescribedIndices, "1-n", "1C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, PatientSupportAngle, "1", "1C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, PatientSupportRotationDirection, "1", "1C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, TableTopPitchAngle, "1", "2C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, TableTopPitchRotationDirection, "1", "2C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, TableTopRollAngle, "1", "2C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, TableTopRollRotationDirection, "1", "2C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, HeadFixationAngle, "1", "3", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, ChairHeadFramePosition, "1", "3", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, TableTopVerticalPosition, "1", "2C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, TableTopLongitudinalPosition, "1", "2C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, TableTopLateralPosition, "1", "2C", "IonControlPointDeliverySequence");
        getAndCheckElementFromDataset(item, SnoutPosition, "1", "2C", "IonControlPointDeliverySequence");
        CorrectedParameterSequence.read(item, "1-n", "3", "IonControlPointDeliverySequence");
        OverrideSequence.read(item, "1-n", "3", "IonControlPointDeliverySequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonControlPointDeliverySequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ReferencedControlPointIndex), "1", "1", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmDate(TreatmentControlPointDate), "1", "1", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmTime(TreatmentControlPointTime), "1", "1", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmDecimalString(SpecifiedMeterset), "1", "2", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmDecimalString(DeliveredMeterset), "1", "1", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(MetersetRateSet), "1", "3", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(MetersetRateDelivered), "1", "3", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmDecimalString(NominalBeamEnergy), "1", "1C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmDecimalString(KVP), "1", "1C", "IonControlPointDeliverySequence");
        if (result.good()) result = IonWedgePositionSequence.write(item, "1-n", "1C", "IonControlPointDeliverySequence");
        if (result.good()) result = BeamLimitingDevicePositionSequence.write(item, "1-n", "1C", "IonControlPointDeliverySequence");
        if (result.good()) result = RangeShifterSettingsSequence.write(item, "1-n", "1C", "IonControlPointDeliverySequence");
        if (result.good()) result = LateralSpreadingDeviceSettingsSequence.write(item, "1-n", "1C", "IonControlPointDeliverySequence");
        if (result.good()) result = RangeModulatorSettingsSequence.write(item, "1-n", "1C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmDecimalString(GantryAngle), "1", "1C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmCodeString(GantryRotationDirection), "1", "1C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(GantryPitchAngle), "1", "2C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmCodeString(GantryPitchRotationDirection), "1", "2C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmDecimalString(BeamLimitingDeviceAngle), "1", "1C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmCodeString(BeamLimitingDeviceRotationDirection), "1", "1C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmShortString(ScanSpotTuneID), "1", "1C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfScanSpotPositions), "1", "1C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(ScanSpotPositionMap), "1-n", "1C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(ScanSpotMetersetsDelivered), "1-n", "1C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(ScanningSpotSize), "2", "3", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfPaintings), "1", "1C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmCodeString(ScanSpotReordered), "1", "3", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmIntegerString(ScanSpotPrescribedIndices), "1-n", "1C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmDecimalString(PatientSupportAngle), "1", "1C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmCodeString(PatientSupportRotationDirection), "1", "1C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(TableTopPitchAngle), "1", "2C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmCodeString(TableTopPitchRotationDirection), "1", "2C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(TableTopRollAngle), "1", "2C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmCodeString(TableTopRollRotationDirection), "1", "2C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(HeadFixationAngle), "1", "3", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmDecimalString(ChairHeadFramePosition), "1", "3", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmDecimalString(TableTopVerticalPosition), "1", "2C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmDecimalString(TableTopLongitudinalPosition), "1", "2C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmDecimalString(TableTopLateralPosition), "1", "2C", "IonControlPointDeliverySequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(SnoutPosition), "1", "2C", "IonControlPointDeliverySequence");
        if (result.good()) result = CorrectedParameterSequence.write(item, "1-n", "3", "IonControlPointDeliverySequence");
        if (result.good()) result = OverrideSequence.write(item, "1-n", "3", "IonControlPointDeliverySequence");
    }
    return result;
}


OFCondition DRTIonControlPointDeliverySequence::Item::getBeamLimitingDeviceAngle(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamLimitingDeviceAngle, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getBeamLimitingDeviceAngle(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, BeamLimitingDeviceAngle).getFloat64(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getBeamLimitingDeviceRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamLimitingDeviceRotationDirection, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getChairHeadFramePosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ChairHeadFramePosition, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getChairHeadFramePosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ChairHeadFramePosition).getFloat64(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getDeliveredMeterset(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeliveredMeterset, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getDeliveredMeterset(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DeliveredMeterset).getFloat64(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getGantryAngle(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GantryAngle, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getGantryAngle(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, GantryAngle).getFloat64(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getGantryPitchAngle(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, GantryPitchAngle).getFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getGantryPitchRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GantryPitchRotationDirection, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getGantryRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(GantryRotationDirection, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getHeadFixationAngle(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, HeadFixationAngle).getFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getKVP(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(KVP, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getKVP(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, KVP).getFloat64(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getMetersetRateDelivered(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, MetersetRateDelivered).getFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getMetersetRateSet(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, MetersetRateSet).getFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getNominalBeamEnergy(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NominalBeamEnergy, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getNominalBeamEnergy(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, NominalBeamEnergy).getFloat64(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getNumberOfPaintings(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfPaintings, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getNumberOfPaintings(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfPaintings).getSint32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getNumberOfScanSpotPositions(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfScanSpotPositions, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getNumberOfScanSpotPositions(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfScanSpotPositions).getSint32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getPatientSupportAngle(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientSupportAngle, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getPatientSupportAngle(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, PatientSupportAngle).getFloat64(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getPatientSupportRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientSupportRotationDirection, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getReferencedControlPointIndex(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedControlPointIndex, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getReferencedControlPointIndex(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedControlPointIndex).getSint32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getScanSpotMetersetsDelivered(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, ScanSpotMetersetsDelivered).getFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getScanSpotPositionMap(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, ScanSpotPositionMap).getFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getScanSpotPrescribedIndices(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ScanSpotPrescribedIndices, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getScanSpotPrescribedIndices(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ScanSpotPrescribedIndices).getSint32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getScanSpotReordered(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ScanSpotReordered, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getScanSpotTuneID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ScanSpotTuneID, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getScanningSpotSize(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, ScanningSpotSize).getFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getSnoutPosition(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, SnoutPosition).getFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getSpecifiedMeterset(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SpecifiedMeterset, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getSpecifiedMeterset(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SpecifiedMeterset).getFloat64(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getTableTopLateralPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopLateralPosition, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getTableTopLateralPosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TableTopLateralPosition).getFloat64(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getTableTopLongitudinalPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopLongitudinalPosition, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getTableTopLongitudinalPosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TableTopLongitudinalPosition).getFloat64(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getTableTopPitchAngle(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, TableTopPitchAngle).getFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getTableTopPitchRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopPitchRotationDirection, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getTableTopRollAngle(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, TableTopRollAngle).getFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getTableTopRollRotationDirection(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopRollRotationDirection, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getTableTopVerticalPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TableTopVerticalPosition, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getTableTopVerticalPosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TableTopVerticalPosition).getFloat64(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getTreatmentControlPointDate(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TreatmentControlPointDate, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::getTreatmentControlPointTime(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TreatmentControlPointTime, value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::setBeamLimitingDeviceAngle(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setBeamLimitingDeviceRotationDirection(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setChairHeadFramePosition(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setDeliveredMeterset(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DeliveredMeterset.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonControlPointDeliverySequence::Item::setGantryAngle(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setGantryPitchAngle(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return GantryPitchAngle.putFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::setGantryPitchRotationDirection(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setGantryRotationDirection(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setHeadFixationAngle(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return HeadFixationAngle.putFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::setKVP(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setMetersetRateDelivered(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return MetersetRateDelivered.putFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::setMetersetRateSet(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return MetersetRateSet.putFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::setNominalBeamEnergy(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setNumberOfPaintings(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setNumberOfScanSpotPositions(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setPatientSupportAngle(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setPatientSupportRotationDirection(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setReferencedControlPointIndex(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedControlPointIndex.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonControlPointDeliverySequence::Item::setScanSpotMetersetsDelivered(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ScanSpotMetersetsDelivered.putFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::setScanSpotPositionMap(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ScanSpotPositionMap.putFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::setScanSpotPrescribedIndices(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1-n") : EC_Normal;
        if (result.good())
            result = ScanSpotPrescribedIndices.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonControlPointDeliverySequence::Item::setScanSpotReordered(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ScanSpotReordered.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonControlPointDeliverySequence::Item::setScanSpotTuneID(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setScanningSpotSize(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ScanningSpotSize.putFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::setSnoutPosition(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return SnoutPosition.putFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::setSpecifiedMeterset(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SpecifiedMeterset.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonControlPointDeliverySequence::Item::setTableTopLateralPosition(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setTableTopLongitudinalPosition(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setTableTopPitchAngle(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return TableTopPitchAngle.putFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::setTableTopPitchRotationDirection(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setTableTopRollAngle(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return TableTopRollAngle.putFloat32(value, pos);
}


OFCondition DRTIonControlPointDeliverySequence::Item::setTableTopRollRotationDirection(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setTableTopVerticalPosition(const OFString &value, const OFBool check)
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


OFCondition DRTIonControlPointDeliverySequence::Item::setTreatmentControlPointDate(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TreatmentControlPointDate.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonControlPointDeliverySequence::Item::setTreatmentControlPointTime(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TreatmentControlPointTime.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTIonControlPointDeliverySequence::DRTIonControlPointDeliverySequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTIonControlPointDeliverySequence::DRTIonControlPointDeliverySequence(const DRTIonControlPointDeliverySequence &copy)
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


DRTIonControlPointDeliverySequence &DRTIonControlPointDeliverySequence::operator=(const DRTIonControlPointDeliverySequence &copy)
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


DRTIonControlPointDeliverySequence::~DRTIonControlPointDeliverySequence()
{
    clear();
}


void DRTIonControlPointDeliverySequence::clear()
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


OFBool DRTIonControlPointDeliverySequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTIonControlPointDeliverySequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTIonControlPointDeliverySequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTIonControlPointDeliverySequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonControlPointDeliverySequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonControlPointDeliverySequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTIonControlPointDeliverySequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTIonControlPointDeliverySequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTIonControlPointDeliverySequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTIonControlPointDeliverySequence::Item &DRTIonControlPointDeliverySequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTIonControlPointDeliverySequence::Item &DRTIonControlPointDeliverySequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTIonControlPointDeliverySequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTIonControlPointDeliverySequence::Item &DRTIonControlPointDeliverySequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTIonControlPointDeliverySequence::Item &DRTIonControlPointDeliverySequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTIonControlPointDeliverySequence::Item &DRTIonControlPointDeliverySequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTIonControlPointDeliverySequence::Item &DRTIonControlPointDeliverySequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTIonControlPointDeliverySequence::addItem(Item *&item)
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


OFCondition DRTIonControlPointDeliverySequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTIonControlPointDeliverySequence::removeItem(const unsigned long pos)
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


OFCondition DRTIonControlPointDeliverySequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_IonControlPointDeliverySequence, sequence);
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
            DcmSequenceOfItems element(DCM_IonControlPointDeliverySequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTIonControlPointDeliverySequence::write(DcmItem &dataset,
                                                      const OFString &card,
                                                      const OFString &type,
                                                      const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_IonControlPointDeliverySequence);
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
