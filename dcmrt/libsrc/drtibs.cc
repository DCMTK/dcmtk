/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTIonBeamSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtibs.h"


// --- item class ---

DRTIonBeamSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ApplicatorSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    BeamDescription(DCM_BeamDescription),
    BeamName(DCM_BeamName),
    BeamNumber(DCM_BeamNumber),
    BeamType(DCM_BeamType),
    DepthDoseParametersSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    DeviceSerialNumber(DCM_DeviceSerialNumber),
    FinalCumulativeMetersetWeight(DCM_FinalCumulativeMetersetWeight),
    FixationEye(DCM_FixationEye),
    FixationLightAzimuthalAngle(DCM_FixationLightAzimuthalAngle),
    FixationLightPolarAngle(DCM_FixationLightPolarAngle),
    GeneralAccessorySequence(emptyDefaultItem /*emptyDefaultSequence*/),
    InstitutionAddress(DCM_InstitutionAddress),
    InstitutionName(DCM_InstitutionName),
    InstitutionalDepartmentName(DCM_InstitutionalDepartmentName),
    IonBeamLimitingDeviceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    IonBlockSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    IonControlPointSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    IonRangeCompensatorSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    IonWedgeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    LateralSpreadingDeviceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    Manufacturer(DCM_Manufacturer),
    ManufacturerModelName(DCM_ManufacturerModelName),
    ModulatedScanModeType(DCM_ModulatedScanModeType),
    NumberOfBlocks(DCM_NumberOfBlocks),
    NumberOfBoli(DCM_NumberOfBoli),
    NumberOfCompensators(DCM_NumberOfCompensators),
    NumberOfControlPoints(DCM_NumberOfControlPoints),
    NumberOfLateralSpreadingDevices(DCM_NumberOfLateralSpreadingDevices),
    NumberOfRangeModulators(DCM_NumberOfRangeModulators),
    NumberOfRangeShifters(DCM_NumberOfRangeShifters),
    NumberOfWedges(DCM_NumberOfWedges),
    PatientSupportAccessoryCode(DCM_PatientSupportAccessoryCode),
    PatientSupportID(DCM_PatientSupportID),
    PatientSupportType(DCM_PatientSupportType),
    PrimaryDosimeterUnit(DCM_PrimaryDosimeterUnit),
    RadiationAtomicNumber(DCM_RadiationAtomicNumber),
    RadiationChargeState(DCM_RadiationChargeState),
    RadiationMassNumber(DCM_RadiationMassNumber),
    RadiationType(DCM_RadiationType),
    RangeModulatorSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RangeShifterSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedBolusSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedDoseSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedPatientSetupNumber(DCM_ReferencedPatientSetupNumber),
    ReferencedReferenceImageSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedToleranceTableNumber(DCM_ReferencedToleranceTableNumber),
    ScanMode(DCM_ScanMode),
    SnoutSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    TotalBlockTrayWaterEquivalentThickness(DCM_TotalBlockTrayWaterEquivalentThickness),
    TotalCompensatorTrayWaterEquivalentThickness(DCM_TotalCompensatorTrayWaterEquivalentThickness),
    TotalWedgeTrayWaterEquivalentThickness(DCM_TotalWedgeTrayWaterEquivalentThickness),
    TreatmentDeliveryType(DCM_TreatmentDeliveryType),
    TreatmentMachineName(DCM_TreatmentMachineName),
    VirtualSourceAxisDistances(DCM_VirtualSourceAxisDistances)
{
}


DRTIonBeamSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ApplicatorSequence(copy.ApplicatorSequence),
    BeamDescription(copy.BeamDescription),
    BeamName(copy.BeamName),
    BeamNumber(copy.BeamNumber),
    BeamType(copy.BeamType),
    DepthDoseParametersSequence(copy.DepthDoseParametersSequence),
    DeviceSerialNumber(copy.DeviceSerialNumber),
    FinalCumulativeMetersetWeight(copy.FinalCumulativeMetersetWeight),
    FixationEye(copy.FixationEye),
    FixationLightAzimuthalAngle(copy.FixationLightAzimuthalAngle),
    FixationLightPolarAngle(copy.FixationLightPolarAngle),
    GeneralAccessorySequence(copy.GeneralAccessorySequence),
    InstitutionAddress(copy.InstitutionAddress),
    InstitutionName(copy.InstitutionName),
    InstitutionalDepartmentName(copy.InstitutionalDepartmentName),
    IonBeamLimitingDeviceSequence(copy.IonBeamLimitingDeviceSequence),
    IonBlockSequence(copy.IonBlockSequence),
    IonControlPointSequence(copy.IonControlPointSequence),
    IonRangeCompensatorSequence(copy.IonRangeCompensatorSequence),
    IonWedgeSequence(copy.IonWedgeSequence),
    LateralSpreadingDeviceSequence(copy.LateralSpreadingDeviceSequence),
    Manufacturer(copy.Manufacturer),
    ManufacturerModelName(copy.ManufacturerModelName),
    ModulatedScanModeType(copy.ModulatedScanModeType),
    NumberOfBlocks(copy.NumberOfBlocks),
    NumberOfBoli(copy.NumberOfBoli),
    NumberOfCompensators(copy.NumberOfCompensators),
    NumberOfControlPoints(copy.NumberOfControlPoints),
    NumberOfLateralSpreadingDevices(copy.NumberOfLateralSpreadingDevices),
    NumberOfRangeModulators(copy.NumberOfRangeModulators),
    NumberOfRangeShifters(copy.NumberOfRangeShifters),
    NumberOfWedges(copy.NumberOfWedges),
    PatientSupportAccessoryCode(copy.PatientSupportAccessoryCode),
    PatientSupportID(copy.PatientSupportID),
    PatientSupportType(copy.PatientSupportType),
    PrimaryDosimeterUnit(copy.PrimaryDosimeterUnit),
    RadiationAtomicNumber(copy.RadiationAtomicNumber),
    RadiationChargeState(copy.RadiationChargeState),
    RadiationMassNumber(copy.RadiationMassNumber),
    RadiationType(copy.RadiationType),
    RangeModulatorSequence(copy.RangeModulatorSequence),
    RangeShifterSequence(copy.RangeShifterSequence),
    ReferencedBolusSequence(copy.ReferencedBolusSequence),
    ReferencedDoseSequence(copy.ReferencedDoseSequence),
    ReferencedPatientSetupNumber(copy.ReferencedPatientSetupNumber),
    ReferencedReferenceImageSequence(copy.ReferencedReferenceImageSequence),
    ReferencedToleranceTableNumber(copy.ReferencedToleranceTableNumber),
    ScanMode(copy.ScanMode),
    SnoutSequence(copy.SnoutSequence),
    TotalBlockTrayWaterEquivalentThickness(copy.TotalBlockTrayWaterEquivalentThickness),
    TotalCompensatorTrayWaterEquivalentThickness(copy.TotalCompensatorTrayWaterEquivalentThickness),
    TotalWedgeTrayWaterEquivalentThickness(copy.TotalWedgeTrayWaterEquivalentThickness),
    TreatmentDeliveryType(copy.TreatmentDeliveryType),
    TreatmentMachineName(copy.TreatmentMachineName),
    VirtualSourceAxisDistances(copy.VirtualSourceAxisDistances)
{
}


DRTIonBeamSequence::Item::~Item()
{
}


DRTIonBeamSequence::Item &DRTIonBeamSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ApplicatorSequence = copy.ApplicatorSequence;
        BeamDescription = copy.BeamDescription;
        BeamName = copy.BeamName;
        BeamNumber = copy.BeamNumber;
        BeamType = copy.BeamType;
        DepthDoseParametersSequence = copy.DepthDoseParametersSequence;
        DeviceSerialNumber = copy.DeviceSerialNumber;
        FinalCumulativeMetersetWeight = copy.FinalCumulativeMetersetWeight;
        FixationEye = copy.FixationEye;
        FixationLightAzimuthalAngle = copy.FixationLightAzimuthalAngle;
        FixationLightPolarAngle = copy.FixationLightPolarAngle;
        GeneralAccessorySequence = copy.GeneralAccessorySequence;
        InstitutionAddress = copy.InstitutionAddress;
        InstitutionName = copy.InstitutionName;
        InstitutionalDepartmentName = copy.InstitutionalDepartmentName;
        IonBeamLimitingDeviceSequence = copy.IonBeamLimitingDeviceSequence;
        IonBlockSequence = copy.IonBlockSequence;
        IonControlPointSequence = copy.IonControlPointSequence;
        IonRangeCompensatorSequence = copy.IonRangeCompensatorSequence;
        IonWedgeSequence = copy.IonWedgeSequence;
        LateralSpreadingDeviceSequence = copy.LateralSpreadingDeviceSequence;
        Manufacturer = copy.Manufacturer;
        ManufacturerModelName = copy.ManufacturerModelName;
        ModulatedScanModeType = copy.ModulatedScanModeType;
        NumberOfBlocks = copy.NumberOfBlocks;
        NumberOfBoli = copy.NumberOfBoli;
        NumberOfCompensators = copy.NumberOfCompensators;
        NumberOfControlPoints = copy.NumberOfControlPoints;
        NumberOfLateralSpreadingDevices = copy.NumberOfLateralSpreadingDevices;
        NumberOfRangeModulators = copy.NumberOfRangeModulators;
        NumberOfRangeShifters = copy.NumberOfRangeShifters;
        NumberOfWedges = copy.NumberOfWedges;
        PatientSupportAccessoryCode = copy.PatientSupportAccessoryCode;
        PatientSupportID = copy.PatientSupportID;
        PatientSupportType = copy.PatientSupportType;
        PrimaryDosimeterUnit = copy.PrimaryDosimeterUnit;
        RadiationAtomicNumber = copy.RadiationAtomicNumber;
        RadiationChargeState = copy.RadiationChargeState;
        RadiationMassNumber = copy.RadiationMassNumber;
        RadiationType = copy.RadiationType;
        RangeModulatorSequence = copy.RangeModulatorSequence;
        RangeShifterSequence = copy.RangeShifterSequence;
        ReferencedBolusSequence = copy.ReferencedBolusSequence;
        ReferencedDoseSequence = copy.ReferencedDoseSequence;
        ReferencedPatientSetupNumber = copy.ReferencedPatientSetupNumber;
        ReferencedReferenceImageSequence = copy.ReferencedReferenceImageSequence;
        ReferencedToleranceTableNumber = copy.ReferencedToleranceTableNumber;
        ScanMode = copy.ScanMode;
        SnoutSequence = copy.SnoutSequence;
        TotalBlockTrayWaterEquivalentThickness = copy.TotalBlockTrayWaterEquivalentThickness;
        TotalCompensatorTrayWaterEquivalentThickness = copy.TotalCompensatorTrayWaterEquivalentThickness;
        TotalWedgeTrayWaterEquivalentThickness = copy.TotalWedgeTrayWaterEquivalentThickness;
        TreatmentDeliveryType = copy.TreatmentDeliveryType;
        TreatmentMachineName = copy.TreatmentMachineName;
        VirtualSourceAxisDistances = copy.VirtualSourceAxisDistances;
    }
    return *this;
}


void DRTIonBeamSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        BeamNumber.clear();
        BeamName.clear();
        BeamDescription.clear();
        BeamType.clear();
        RadiationType.clear();
        RadiationMassNumber.clear();
        RadiationAtomicNumber.clear();
        RadiationChargeState.clear();
        ScanMode.clear();
        ModulatedScanModeType.clear();
        TreatmentMachineName.clear();
        Manufacturer.clear();
        InstitutionName.clear();
        InstitutionAddress.clear();
        InstitutionalDepartmentName.clear();
        ManufacturerModelName.clear();
        DeviceSerialNumber.clear();
        PrimaryDosimeterUnit.clear();
        ReferencedToleranceTableNumber.clear();
        VirtualSourceAxisDistances.clear();
        DepthDoseParametersSequence.clear();
        IonBeamLimitingDeviceSequence.clear();
        ReferencedPatientSetupNumber.clear();
        ReferencedReferenceImageSequence.clear();
        TreatmentDeliveryType.clear();
        ReferencedDoseSequence.clear();
        NumberOfWedges.clear();
        TotalWedgeTrayWaterEquivalentThickness.clear();
        IonWedgeSequence.clear();
        NumberOfCompensators.clear();
        TotalCompensatorTrayWaterEquivalentThickness.clear();
        IonRangeCompensatorSequence.clear();
        NumberOfBoli.clear();
        ReferencedBolusSequence.clear();
        NumberOfBlocks.clear();
        TotalBlockTrayWaterEquivalentThickness.clear();
        IonBlockSequence.clear();
        SnoutSequence.clear();
        ApplicatorSequence.clear();
        GeneralAccessorySequence.clear();
        NumberOfRangeShifters.clear();
        RangeShifterSequence.clear();
        NumberOfLateralSpreadingDevices.clear();
        LateralSpreadingDeviceSequence.clear();
        NumberOfRangeModulators.clear();
        RangeModulatorSequence.clear();
        PatientSupportType.clear();
        PatientSupportID.clear();
        PatientSupportAccessoryCode.clear();
        FixationLightAzimuthalAngle.clear();
        FixationLightPolarAngle.clear();
        FixationEye.clear();
        FinalCumulativeMetersetWeight.clear();
        NumberOfControlPoints.clear();
        IonControlPointSequence.clear();
    }
}


OFBool DRTIonBeamSequence::Item::isEmpty()
{
    return BeamNumber.isEmpty() &&
           BeamName.isEmpty() &&
           BeamDescription.isEmpty() &&
           BeamType.isEmpty() &&
           RadiationType.isEmpty() &&
           RadiationMassNumber.isEmpty() &&
           RadiationAtomicNumber.isEmpty() &&
           RadiationChargeState.isEmpty() &&
           ScanMode.isEmpty() &&
           ModulatedScanModeType.isEmpty() &&
           TreatmentMachineName.isEmpty() &&
           Manufacturer.isEmpty() &&
           InstitutionName.isEmpty() &&
           InstitutionAddress.isEmpty() &&
           InstitutionalDepartmentName.isEmpty() &&
           ManufacturerModelName.isEmpty() &&
           DeviceSerialNumber.isEmpty() &&
           PrimaryDosimeterUnit.isEmpty() &&
           ReferencedToleranceTableNumber.isEmpty() &&
           VirtualSourceAxisDistances.isEmpty() &&
           DepthDoseParametersSequence.isEmpty() &&
           IonBeamLimitingDeviceSequence.isEmpty() &&
           ReferencedPatientSetupNumber.isEmpty() &&
           ReferencedReferenceImageSequence.isEmpty() &&
           TreatmentDeliveryType.isEmpty() &&
           ReferencedDoseSequence.isEmpty() &&
           NumberOfWedges.isEmpty() &&
           TotalWedgeTrayWaterEquivalentThickness.isEmpty() &&
           IonWedgeSequence.isEmpty() &&
           NumberOfCompensators.isEmpty() &&
           TotalCompensatorTrayWaterEquivalentThickness.isEmpty() &&
           IonRangeCompensatorSequence.isEmpty() &&
           NumberOfBoli.isEmpty() &&
           ReferencedBolusSequence.isEmpty() &&
           NumberOfBlocks.isEmpty() &&
           TotalBlockTrayWaterEquivalentThickness.isEmpty() &&
           IonBlockSequence.isEmpty() &&
           SnoutSequence.isEmpty() &&
           ApplicatorSequence.isEmpty() &&
           GeneralAccessorySequence.isEmpty() &&
           NumberOfRangeShifters.isEmpty() &&
           RangeShifterSequence.isEmpty() &&
           NumberOfLateralSpreadingDevices.isEmpty() &&
           LateralSpreadingDeviceSequence.isEmpty() &&
           NumberOfRangeModulators.isEmpty() &&
           RangeModulatorSequence.isEmpty() &&
           PatientSupportType.isEmpty() &&
           PatientSupportID.isEmpty() &&
           PatientSupportAccessoryCode.isEmpty() &&
           FixationLightAzimuthalAngle.isEmpty() &&
           FixationLightPolarAngle.isEmpty() &&
           FixationEye.isEmpty() &&
           FinalCumulativeMetersetWeight.isEmpty() &&
           NumberOfControlPoints.isEmpty() &&
           IonControlPointSequence.isEmpty();
}


OFBool DRTIonBeamSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTIonBeamSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, BeamNumber, "1", "1", "IonBeamSequence");
        getAndCheckElementFromDataset(item, BeamName, "1", "1", "IonBeamSequence");
        getAndCheckElementFromDataset(item, BeamDescription, "1", "3", "IonBeamSequence");
        getAndCheckElementFromDataset(item, BeamType, "1", "1", "IonBeamSequence");
        getAndCheckElementFromDataset(item, RadiationType, "1", "1", "IonBeamSequence");
        getAndCheckElementFromDataset(item, RadiationMassNumber, "1", "1C", "IonBeamSequence");
        getAndCheckElementFromDataset(item, RadiationAtomicNumber, "1", "1C", "IonBeamSequence");
        getAndCheckElementFromDataset(item, RadiationChargeState, "1", "1C", "IonBeamSequence");
        getAndCheckElementFromDataset(item, ScanMode, "1", "1", "IonBeamSequence");
        getAndCheckElementFromDataset(item, ModulatedScanModeType, "1", "1C", "IonBeamSequence");
        getAndCheckElementFromDataset(item, TreatmentMachineName, "1", "2", "IonBeamSequence");
        getAndCheckElementFromDataset(item, Manufacturer, "1", "3", "IonBeamSequence");
        getAndCheckElementFromDataset(item, InstitutionName, "1", "3", "IonBeamSequence");
        getAndCheckElementFromDataset(item, InstitutionAddress, "1", "3", "IonBeamSequence");
        getAndCheckElementFromDataset(item, InstitutionalDepartmentName, "1", "3", "IonBeamSequence");
        getAndCheckElementFromDataset(item, ManufacturerModelName, "1", "3", "IonBeamSequence");
        getAndCheckElementFromDataset(item, DeviceSerialNumber, "1", "3", "IonBeamSequence");
        getAndCheckElementFromDataset(item, PrimaryDosimeterUnit, "1", "1", "IonBeamSequence");
        getAndCheckElementFromDataset(item, ReferencedToleranceTableNumber, "1", "3", "IonBeamSequence");
        getAndCheckElementFromDataset(item, VirtualSourceAxisDistances, "2", "1", "IonBeamSequence");
        DepthDoseParametersSequence.read(item, "1-n", "3", "IonBeamSequence");
        IonBeamLimitingDeviceSequence.read(item, "1-n", "3", "IonBeamSequence");
        getAndCheckElementFromDataset(item, ReferencedPatientSetupNumber, "1", "3", "IonBeamSequence");
        ReferencedReferenceImageSequence.read(item, "1-n", "3", "IonBeamSequence");
        getAndCheckElementFromDataset(item, TreatmentDeliveryType, "1", "1", "IonBeamSequence");
        ReferencedDoseSequence.read(item, "1-n", "3", "IonBeamSequence");
        getAndCheckElementFromDataset(item, NumberOfWedges, "1", "1", "IonBeamSequence");
        getAndCheckElementFromDataset(item, TotalWedgeTrayWaterEquivalentThickness, "1", "3", "IonBeamSequence");
        IonWedgeSequence.read(item, "1-n", "1C", "IonBeamSequence");
        getAndCheckElementFromDataset(item, NumberOfCompensators, "1", "1", "IonBeamSequence");
        getAndCheckElementFromDataset(item, TotalCompensatorTrayWaterEquivalentThickness, "1", "3", "IonBeamSequence");
        IonRangeCompensatorSequence.read(item, "1-n", "1C", "IonBeamSequence");
        getAndCheckElementFromDataset(item, NumberOfBoli, "1", "1", "IonBeamSequence");
        ReferencedBolusSequence.read(item, "1-n", "1C", "IonBeamSequence");
        getAndCheckElementFromDataset(item, NumberOfBlocks, "1", "1", "IonBeamSequence");
        getAndCheckElementFromDataset(item, TotalBlockTrayWaterEquivalentThickness, "1", "3", "IonBeamSequence");
        IonBlockSequence.read(item, "1-n", "1C", "IonBeamSequence");
        SnoutSequence.read(item, "1-n", "3", "IonBeamSequence");
        ApplicatorSequence.read(item, "1-n", "3", "IonBeamSequence");
        GeneralAccessorySequence.read(item, "1-n", "3", "IonBeamSequence");
        getAndCheckElementFromDataset(item, NumberOfRangeShifters, "1", "1", "IonBeamSequence");
        RangeShifterSequence.read(item, "1-n", "1C", "IonBeamSequence");
        getAndCheckElementFromDataset(item, NumberOfLateralSpreadingDevices, "1", "1", "IonBeamSequence");
        LateralSpreadingDeviceSequence.read(item, "1-n", "1C", "IonBeamSequence");
        getAndCheckElementFromDataset(item, NumberOfRangeModulators, "1", "1", "IonBeamSequence");
        RangeModulatorSequence.read(item, "1-n", "1C", "IonBeamSequence");
        getAndCheckElementFromDataset(item, PatientSupportType, "1", "1", "IonBeamSequence");
        getAndCheckElementFromDataset(item, PatientSupportID, "1", "3", "IonBeamSequence");
        getAndCheckElementFromDataset(item, PatientSupportAccessoryCode, "1", "3", "IonBeamSequence");
        getAndCheckElementFromDataset(item, FixationLightAzimuthalAngle, "1", "3", "IonBeamSequence");
        getAndCheckElementFromDataset(item, FixationLightPolarAngle, "1", "3", "IonBeamSequence");
        getAndCheckElementFromDataset(item, FixationEye, "1", "3", "IonBeamSequence");
        getAndCheckElementFromDataset(item, FinalCumulativeMetersetWeight, "1", "1C", "IonBeamSequence");
        getAndCheckElementFromDataset(item, NumberOfControlPoints, "1", "1", "IonBeamSequence");
        IonControlPointSequence.read(item, "1-n", "1", "IonBeamSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(BeamNumber), "1", "1", "IonBeamSequence");
        addElementToDataset(result, item, new DcmLongString(BeamName), "1", "1", "IonBeamSequence");
        addElementToDataset(result, item, new DcmShortText(BeamDescription), "1", "3", "IonBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(BeamType), "1", "1", "IonBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(RadiationType), "1", "1", "IonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(RadiationMassNumber), "1", "1C", "IonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(RadiationAtomicNumber), "1", "1C", "IonBeamSequence");
        addElementToDataset(result, item, new DcmSignedShort(RadiationChargeState), "1", "1C", "IonBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(ScanMode), "1", "1", "IonBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(ModulatedScanModeType), "1", "1C", "IonBeamSequence");
        addElementToDataset(result, item, new DcmShortString(TreatmentMachineName), "1", "2", "IonBeamSequence");
        addElementToDataset(result, item, new DcmLongString(Manufacturer), "1", "3", "IonBeamSequence");
        addElementToDataset(result, item, new DcmLongString(InstitutionName), "1", "3", "IonBeamSequence");
        addElementToDataset(result, item, new DcmShortText(InstitutionAddress), "1", "3", "IonBeamSequence");
        addElementToDataset(result, item, new DcmLongString(InstitutionalDepartmentName), "1", "3", "IonBeamSequence");
        addElementToDataset(result, item, new DcmLongString(ManufacturerModelName), "1", "3", "IonBeamSequence");
        addElementToDataset(result, item, new DcmLongString(DeviceSerialNumber), "1", "3", "IonBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(PrimaryDosimeterUnit), "1", "1", "IonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferencedToleranceTableNumber), "1", "3", "IonBeamSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(VirtualSourceAxisDistances), "2", "1", "IonBeamSequence");
        if (result.good()) result = DepthDoseParametersSequence.write(item, "1-n", "3", "IonBeamSequence");
        if (result.good()) result = IonBeamLimitingDeviceSequence.write(item, "1-n", "3", "IonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferencedPatientSetupNumber), "1", "3", "IonBeamSequence");
        if (result.good()) result = ReferencedReferenceImageSequence.write(item, "1-n", "3", "IonBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(TreatmentDeliveryType), "1", "1", "IonBeamSequence");
        if (result.good()) result = ReferencedDoseSequence.write(item, "1-n", "3", "IonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfWedges), "1", "1", "IonBeamSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(TotalWedgeTrayWaterEquivalentThickness), "1", "3", "IonBeamSequence");
        if (result.good()) result = IonWedgeSequence.write(item, "1-n", "1C", "IonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfCompensators), "1", "1", "IonBeamSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(TotalCompensatorTrayWaterEquivalentThickness), "1", "3", "IonBeamSequence");
        if (result.good()) result = IonRangeCompensatorSequence.write(item, "1-n", "1C", "IonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfBoli), "1", "1", "IonBeamSequence");
        if (result.good()) result = ReferencedBolusSequence.write(item, "1-n", "1C", "IonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfBlocks), "1", "1", "IonBeamSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(TotalBlockTrayWaterEquivalentThickness), "1", "3", "IonBeamSequence");
        if (result.good()) result = IonBlockSequence.write(item, "1-n", "1C", "IonBeamSequence");
        if (result.good()) result = SnoutSequence.write(item, "1-n", "3", "IonBeamSequence");
        if (result.good()) result = ApplicatorSequence.write(item, "1-n", "3", "IonBeamSequence");
        if (result.good()) result = GeneralAccessorySequence.write(item, "1-n", "3", "IonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfRangeShifters), "1", "1", "IonBeamSequence");
        if (result.good()) result = RangeShifterSequence.write(item, "1-n", "1C", "IonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfLateralSpreadingDevices), "1", "1", "IonBeamSequence");
        if (result.good()) result = LateralSpreadingDeviceSequence.write(item, "1-n", "1C", "IonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfRangeModulators), "1", "1", "IonBeamSequence");
        if (result.good()) result = RangeModulatorSequence.write(item, "1-n", "1C", "IonBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(PatientSupportType), "1", "1", "IonBeamSequence");
        addElementToDataset(result, item, new DcmShortString(PatientSupportID), "1", "3", "IonBeamSequence");
        addElementToDataset(result, item, new DcmLongString(PatientSupportAccessoryCode), "1", "3", "IonBeamSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(FixationLightAzimuthalAngle), "1", "3", "IonBeamSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(FixationLightPolarAngle), "1", "3", "IonBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(FixationEye), "1", "3", "IonBeamSequence");
        addElementToDataset(result, item, new DcmDecimalString(FinalCumulativeMetersetWeight), "1", "1C", "IonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfControlPoints), "1", "1", "IonBeamSequence");
        if (result.good()) result = IonControlPointSequence.write(item, "1-n", "1", "IonBeamSequence");
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::getBeamDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamDescription, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getBeamName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamName, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getBeamNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamNumber, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getBeamNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, BeamNumber).getSint32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getBeamType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamType, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getDeviceSerialNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeviceSerialNumber, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getFinalCumulativeMetersetWeight(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FinalCumulativeMetersetWeight, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getFinalCumulativeMetersetWeight(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, FinalCumulativeMetersetWeight).getFloat64(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getFixationEye(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FixationEye, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getFixationLightAzimuthalAngle(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, FixationLightAzimuthalAngle).getFloat32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getFixationLightPolarAngle(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, FixationLightPolarAngle).getFloat32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getInstitutionAddress(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(InstitutionAddress, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getInstitutionName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(InstitutionName, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getInstitutionalDepartmentName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(InstitutionalDepartmentName, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getManufacturer(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(Manufacturer, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getManufacturerModelName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ManufacturerModelName, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getModulatedScanModeType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ModulatedScanModeType, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getNumberOfBlocks(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfBlocks, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getNumberOfBlocks(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfBlocks).getSint32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getNumberOfBoli(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfBoli, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getNumberOfBoli(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfBoli).getSint32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getNumberOfCompensators(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfCompensators, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getNumberOfCompensators(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfCompensators).getSint32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getNumberOfControlPoints(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfControlPoints, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getNumberOfControlPoints(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfControlPoints).getSint32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getNumberOfLateralSpreadingDevices(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfLateralSpreadingDevices, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getNumberOfLateralSpreadingDevices(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfLateralSpreadingDevices).getSint32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getNumberOfRangeModulators(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfRangeModulators, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getNumberOfRangeModulators(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfRangeModulators).getSint32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getNumberOfRangeShifters(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfRangeShifters, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getNumberOfRangeShifters(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfRangeShifters).getSint32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getNumberOfWedges(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfWedges, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getNumberOfWedges(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfWedges).getSint32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getPatientSupportAccessoryCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientSupportAccessoryCode, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getPatientSupportID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientSupportID, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getPatientSupportType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientSupportType, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getPrimaryDosimeterUnit(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PrimaryDosimeterUnit, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getRadiationAtomicNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RadiationAtomicNumber, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getRadiationAtomicNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, RadiationAtomicNumber).getSint32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getRadiationChargeState(Sint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmSignedShort &, RadiationChargeState).getSint16(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getRadiationMassNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RadiationMassNumber, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getRadiationMassNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, RadiationMassNumber).getSint32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getRadiationType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RadiationType, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getReferencedPatientSetupNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedPatientSetupNumber, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getReferencedPatientSetupNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedPatientSetupNumber).getSint32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getReferencedToleranceTableNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedToleranceTableNumber, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getReferencedToleranceTableNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedToleranceTableNumber).getSint32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getScanMode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ScanMode, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getTotalBlockTrayWaterEquivalentThickness(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, TotalBlockTrayWaterEquivalentThickness).getFloat32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getTotalCompensatorTrayWaterEquivalentThickness(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, TotalCompensatorTrayWaterEquivalentThickness).getFloat32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getTotalWedgeTrayWaterEquivalentThickness(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, TotalWedgeTrayWaterEquivalentThickness).getFloat32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::getTreatmentDeliveryType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TreatmentDeliveryType, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getTreatmentMachineName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TreatmentMachineName, value, pos);
}


OFCondition DRTIonBeamSequence::Item::getVirtualSourceAxisDistances(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, VirtualSourceAxisDistances).getFloat32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::setBeamDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = BeamDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setBeamName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BeamName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setBeamNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BeamNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setBeamType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BeamType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setDeviceSerialNumber(const OFString &value, const OFBool check)
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


OFCondition DRTIonBeamSequence::Item::setFinalCumulativeMetersetWeight(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = FinalCumulativeMetersetWeight.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setFixationEye(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = FixationEye.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setFixationLightAzimuthalAngle(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return FixationLightAzimuthalAngle.putFloat32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::setFixationLightPolarAngle(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return FixationLightPolarAngle.putFloat32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::setInstitutionAddress(const OFString &value, const OFBool check)
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


OFCondition DRTIonBeamSequence::Item::setInstitutionName(const OFString &value, const OFBool check)
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


OFCondition DRTIonBeamSequence::Item::setInstitutionalDepartmentName(const OFString &value, const OFBool check)
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


OFCondition DRTIonBeamSequence::Item::setManufacturer(const OFString &value, const OFBool check)
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


OFCondition DRTIonBeamSequence::Item::setManufacturerModelName(const OFString &value, const OFBool check)
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


OFCondition DRTIonBeamSequence::Item::setModulatedScanModeType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ModulatedScanModeType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setNumberOfBlocks(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfBlocks.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setNumberOfBoli(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfBoli.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setNumberOfCompensators(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfCompensators.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setNumberOfControlPoints(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfControlPoints.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setNumberOfLateralSpreadingDevices(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfLateralSpreadingDevices.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setNumberOfRangeModulators(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfRangeModulators.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setNumberOfRangeShifters(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfRangeShifters.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setNumberOfWedges(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfWedges.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setPatientSupportAccessoryCode(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PatientSupportAccessoryCode.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setPatientSupportID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PatientSupportID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setPatientSupportType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PatientSupportType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setPrimaryDosimeterUnit(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PrimaryDosimeterUnit.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setRadiationAtomicNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RadiationAtomicNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setRadiationChargeState(const Sint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RadiationChargeState.putSint16(value, pos);
}


OFCondition DRTIonBeamSequence::Item::setRadiationMassNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RadiationMassNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setRadiationType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RadiationType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setReferencedPatientSetupNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedPatientSetupNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setReferencedToleranceTableNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedToleranceTableNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setScanMode(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ScanMode.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setTotalBlockTrayWaterEquivalentThickness(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return TotalBlockTrayWaterEquivalentThickness.putFloat32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::setTotalCompensatorTrayWaterEquivalentThickness(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return TotalCompensatorTrayWaterEquivalentThickness.putFloat32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::setTotalWedgeTrayWaterEquivalentThickness(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return TotalWedgeTrayWaterEquivalentThickness.putFloat32(value, pos);
}


OFCondition DRTIonBeamSequence::Item::setTreatmentDeliveryType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TreatmentDeliveryType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIonBeamSequence::Item::setTreatmentMachineName(const OFString &value, const OFBool check)
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


OFCondition DRTIonBeamSequence::Item::setVirtualSourceAxisDistances(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return VirtualSourceAxisDistances.putFloat32(value, pos);
}


// --- sequence class ---

DRTIonBeamSequence::DRTIonBeamSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTIonBeamSequence::DRTIonBeamSequence(const DRTIonBeamSequence &copy)
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


DRTIonBeamSequence &DRTIonBeamSequence::operator=(const DRTIonBeamSequence &copy)
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


DRTIonBeamSequence::~DRTIonBeamSequence()
{
    clear();
}


void DRTIonBeamSequence::clear()
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


OFBool DRTIonBeamSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTIonBeamSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTIonBeamSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTIonBeamSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonBeamSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonBeamSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTIonBeamSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTIonBeamSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTIonBeamSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTIonBeamSequence::Item &DRTIonBeamSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTIonBeamSequence::Item &DRTIonBeamSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTIonBeamSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTIonBeamSequence::Item &DRTIonBeamSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTIonBeamSequence::Item &DRTIonBeamSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTIonBeamSequence::Item &DRTIonBeamSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTIonBeamSequence::Item &DRTIonBeamSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTIonBeamSequence::addItem(Item *&item)
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


OFCondition DRTIonBeamSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTIonBeamSequence::removeItem(const unsigned long pos)
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


OFCondition DRTIonBeamSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_IonBeamSequence, sequence);
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
            DcmSequenceOfItems element(DCM_IonBeamSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTIonBeamSequence::write(DcmItem &dataset,
                                      const OFString &card,
                                      const OFString &type,
                                      const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_IonBeamSequence);
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
