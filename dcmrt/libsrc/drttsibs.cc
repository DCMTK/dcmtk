/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTTreatmentSessionIonBeamSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drttsibs.h"


// --- item class ---

DRTTreatmentSessionIonBeamSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ApplicatorSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    BeamDescription(DCM_BeamDescription),
    BeamLimitingDeviceLeafPairsSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    BeamName(DCM_BeamName),
    BeamType(DCM_BeamType),
    CurrentFractionNumber(DCM_CurrentFractionNumber),
    DeliveredDepthDoseParametersSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    DeliveredPrimaryMeterset(DCM_DeliveredPrimaryMeterset),
    DeliveredSecondaryMeterset(DCM_DeliveredSecondaryMeterset),
    DeliveredTreatmentTime(DCM_DeliveredTreatmentTime),
    FixationEye(DCM_FixationEye),
    FixationLightAzimuthalAngle(DCM_FixationLightAzimuthalAngle),
    FixationLightPolarAngle(DCM_FixationLightPolarAngle),
    GeneralAccessorySequence(emptyDefaultItem /*emptyDefaultSequence*/),
    IonControlPointDeliverySequence(emptyDefaultItem /*emptyDefaultSequence*/),
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
    RadiationAtomicNumber(DCM_RadiationAtomicNumber),
    RadiationChargeState(DCM_RadiationChargeState),
    RadiationMassNumber(DCM_RadiationMassNumber),
    RadiationType(DCM_RadiationType),
    RecordedBlockSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RecordedCompensatorSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RecordedLateralSpreadingDeviceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RecordedRangeModulatorSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RecordedRangeShifterSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RecordedSnoutSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RecordedWedgeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedBeamNumber(DCM_ReferencedBeamNumber),
    ReferencedBolusSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedCalculatedDoseReferenceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedMeasuredDoseReferenceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedPatientSetupNumber(DCM_ReferencedPatientSetupNumber),
    ReferencedToleranceTableNumber(DCM_ReferencedToleranceTableNumber),
    ReferencedVerificationImageSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ScanMode(DCM_ScanMode),
    SpecifiedPrimaryMeterset(DCM_SpecifiedPrimaryMeterset),
    SpecifiedSecondaryMeterset(DCM_SpecifiedSecondaryMeterset),
    SpecifiedTreatmentTime(DCM_SpecifiedTreatmentTime),
    TreatmentDeliveryType(DCM_TreatmentDeliveryType),
    TreatmentTerminationCode(DCM_TreatmentTerminationCode),
    TreatmentTerminationStatus(DCM_TreatmentTerminationStatus),
    TreatmentVerificationStatus(DCM_TreatmentVerificationStatus)
{
}


DRTTreatmentSessionIonBeamSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ApplicatorSequence(copy.ApplicatorSequence),
    BeamDescription(copy.BeamDescription),
    BeamLimitingDeviceLeafPairsSequence(copy.BeamLimitingDeviceLeafPairsSequence),
    BeamName(copy.BeamName),
    BeamType(copy.BeamType),
    CurrentFractionNumber(copy.CurrentFractionNumber),
    DeliveredDepthDoseParametersSequence(copy.DeliveredDepthDoseParametersSequence),
    DeliveredPrimaryMeterset(copy.DeliveredPrimaryMeterset),
    DeliveredSecondaryMeterset(copy.DeliveredSecondaryMeterset),
    DeliveredTreatmentTime(copy.DeliveredTreatmentTime),
    FixationEye(copy.FixationEye),
    FixationLightAzimuthalAngle(copy.FixationLightAzimuthalAngle),
    FixationLightPolarAngle(copy.FixationLightPolarAngle),
    GeneralAccessorySequence(copy.GeneralAccessorySequence),
    IonControlPointDeliverySequence(copy.IonControlPointDeliverySequence),
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
    RadiationAtomicNumber(copy.RadiationAtomicNumber),
    RadiationChargeState(copy.RadiationChargeState),
    RadiationMassNumber(copy.RadiationMassNumber),
    RadiationType(copy.RadiationType),
    RecordedBlockSequence(copy.RecordedBlockSequence),
    RecordedCompensatorSequence(copy.RecordedCompensatorSequence),
    RecordedLateralSpreadingDeviceSequence(copy.RecordedLateralSpreadingDeviceSequence),
    RecordedRangeModulatorSequence(copy.RecordedRangeModulatorSequence),
    RecordedRangeShifterSequence(copy.RecordedRangeShifterSequence),
    RecordedSnoutSequence(copy.RecordedSnoutSequence),
    RecordedWedgeSequence(copy.RecordedWedgeSequence),
    ReferencedBeamNumber(copy.ReferencedBeamNumber),
    ReferencedBolusSequence(copy.ReferencedBolusSequence),
    ReferencedCalculatedDoseReferenceSequence(copy.ReferencedCalculatedDoseReferenceSequence),
    ReferencedMeasuredDoseReferenceSequence(copy.ReferencedMeasuredDoseReferenceSequence),
    ReferencedPatientSetupNumber(copy.ReferencedPatientSetupNumber),
    ReferencedToleranceTableNumber(copy.ReferencedToleranceTableNumber),
    ReferencedVerificationImageSequence(copy.ReferencedVerificationImageSequence),
    ScanMode(copy.ScanMode),
    SpecifiedPrimaryMeterset(copy.SpecifiedPrimaryMeterset),
    SpecifiedSecondaryMeterset(copy.SpecifiedSecondaryMeterset),
    SpecifiedTreatmentTime(copy.SpecifiedTreatmentTime),
    TreatmentDeliveryType(copy.TreatmentDeliveryType),
    TreatmentTerminationCode(copy.TreatmentTerminationCode),
    TreatmentTerminationStatus(copy.TreatmentTerminationStatus),
    TreatmentVerificationStatus(copy.TreatmentVerificationStatus)
{
}


DRTTreatmentSessionIonBeamSequence::Item::~Item()
{
}


DRTTreatmentSessionIonBeamSequence::Item &DRTTreatmentSessionIonBeamSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ApplicatorSequence = copy.ApplicatorSequence;
        BeamDescription = copy.BeamDescription;
        BeamLimitingDeviceLeafPairsSequence = copy.BeamLimitingDeviceLeafPairsSequence;
        BeamName = copy.BeamName;
        BeamType = copy.BeamType;
        CurrentFractionNumber = copy.CurrentFractionNumber;
        DeliveredDepthDoseParametersSequence = copy.DeliveredDepthDoseParametersSequence;
        DeliveredPrimaryMeterset = copy.DeliveredPrimaryMeterset;
        DeliveredSecondaryMeterset = copy.DeliveredSecondaryMeterset;
        DeliveredTreatmentTime = copy.DeliveredTreatmentTime;
        FixationEye = copy.FixationEye;
        FixationLightAzimuthalAngle = copy.FixationLightAzimuthalAngle;
        FixationLightPolarAngle = copy.FixationLightPolarAngle;
        GeneralAccessorySequence = copy.GeneralAccessorySequence;
        IonControlPointDeliverySequence = copy.IonControlPointDeliverySequence;
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
        RadiationAtomicNumber = copy.RadiationAtomicNumber;
        RadiationChargeState = copy.RadiationChargeState;
        RadiationMassNumber = copy.RadiationMassNumber;
        RadiationType = copy.RadiationType;
        RecordedBlockSequence = copy.RecordedBlockSequence;
        RecordedCompensatorSequence = copy.RecordedCompensatorSequence;
        RecordedLateralSpreadingDeviceSequence = copy.RecordedLateralSpreadingDeviceSequence;
        RecordedRangeModulatorSequence = copy.RecordedRangeModulatorSequence;
        RecordedRangeShifterSequence = copy.RecordedRangeShifterSequence;
        RecordedSnoutSequence = copy.RecordedSnoutSequence;
        RecordedWedgeSequence = copy.RecordedWedgeSequence;
        ReferencedBeamNumber = copy.ReferencedBeamNumber;
        ReferencedBolusSequence = copy.ReferencedBolusSequence;
        ReferencedCalculatedDoseReferenceSequence = copy.ReferencedCalculatedDoseReferenceSequence;
        ReferencedMeasuredDoseReferenceSequence = copy.ReferencedMeasuredDoseReferenceSequence;
        ReferencedPatientSetupNumber = copy.ReferencedPatientSetupNumber;
        ReferencedToleranceTableNumber = copy.ReferencedToleranceTableNumber;
        ReferencedVerificationImageSequence = copy.ReferencedVerificationImageSequence;
        ScanMode = copy.ScanMode;
        SpecifiedPrimaryMeterset = copy.SpecifiedPrimaryMeterset;
        SpecifiedSecondaryMeterset = copy.SpecifiedSecondaryMeterset;
        SpecifiedTreatmentTime = copy.SpecifiedTreatmentTime;
        TreatmentDeliveryType = copy.TreatmentDeliveryType;
        TreatmentTerminationCode = copy.TreatmentTerminationCode;
        TreatmentTerminationStatus = copy.TreatmentTerminationStatus;
        TreatmentVerificationStatus = copy.TreatmentVerificationStatus;
    }
    return *this;
}


void DRTTreatmentSessionIonBeamSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ReferencedBeamNumber.clear();
        BeamName.clear();
        BeamDescription.clear();
        BeamType.clear();
        RadiationType.clear();
        RadiationMassNumber.clear();
        RadiationAtomicNumber.clear();
        RadiationChargeState.clear();
        ScanMode.clear();
        ModulatedScanModeType.clear();
        ReferencedToleranceTableNumber.clear();
        BeamLimitingDeviceLeafPairsSequence.clear();
        ReferencedPatientSetupNumber.clear();
        ReferencedVerificationImageSequence.clear();
        ReferencedMeasuredDoseReferenceSequence.clear();
        ReferencedCalculatedDoseReferenceSequence.clear();
        NumberOfWedges.clear();
        RecordedWedgeSequence.clear();
        NumberOfCompensators.clear();
        RecordedCompensatorSequence.clear();
        NumberOfBoli.clear();
        ReferencedBolusSequence.clear();
        NumberOfBlocks.clear();
        RecordedBlockSequence.clear();
        RecordedSnoutSequence.clear();
        ApplicatorSequence.clear();
        GeneralAccessorySequence.clear();
        NumberOfRangeShifters.clear();
        RecordedRangeShifterSequence.clear();
        NumberOfLateralSpreadingDevices.clear();
        RecordedLateralSpreadingDeviceSequence.clear();
        NumberOfRangeModulators.clear();
        RecordedRangeModulatorSequence.clear();
        PatientSupportType.clear();
        PatientSupportID.clear();
        PatientSupportAccessoryCode.clear();
        FixationLightAzimuthalAngle.clear();
        FixationLightPolarAngle.clear();
        FixationEye.clear();
        CurrentFractionNumber.clear();
        TreatmentDeliveryType.clear();
        TreatmentTerminationStatus.clear();
        TreatmentTerminationCode.clear();
        TreatmentVerificationStatus.clear();
        SpecifiedPrimaryMeterset.clear();
        SpecifiedSecondaryMeterset.clear();
        DeliveredPrimaryMeterset.clear();
        DeliveredSecondaryMeterset.clear();
        SpecifiedTreatmentTime.clear();
        DeliveredTreatmentTime.clear();
        DeliveredDepthDoseParametersSequence.clear();
        NumberOfControlPoints.clear();
        IonControlPointDeliverySequence.clear();
    }
}


OFBool DRTTreatmentSessionIonBeamSequence::Item::isEmpty()
{
    return ReferencedBeamNumber.isEmpty() &&
           BeamName.isEmpty() &&
           BeamDescription.isEmpty() &&
           BeamType.isEmpty() &&
           RadiationType.isEmpty() &&
           RadiationMassNumber.isEmpty() &&
           RadiationAtomicNumber.isEmpty() &&
           RadiationChargeState.isEmpty() &&
           ScanMode.isEmpty() &&
           ModulatedScanModeType.isEmpty() &&
           ReferencedToleranceTableNumber.isEmpty() &&
           BeamLimitingDeviceLeafPairsSequence.isEmpty() &&
           ReferencedPatientSetupNumber.isEmpty() &&
           ReferencedVerificationImageSequence.isEmpty() &&
           ReferencedMeasuredDoseReferenceSequence.isEmpty() &&
           ReferencedCalculatedDoseReferenceSequence.isEmpty() &&
           NumberOfWedges.isEmpty() &&
           RecordedWedgeSequence.isEmpty() &&
           NumberOfCompensators.isEmpty() &&
           RecordedCompensatorSequence.isEmpty() &&
           NumberOfBoli.isEmpty() &&
           ReferencedBolusSequence.isEmpty() &&
           NumberOfBlocks.isEmpty() &&
           RecordedBlockSequence.isEmpty() &&
           RecordedSnoutSequence.isEmpty() &&
           ApplicatorSequence.isEmpty() &&
           GeneralAccessorySequence.isEmpty() &&
           NumberOfRangeShifters.isEmpty() &&
           RecordedRangeShifterSequence.isEmpty() &&
           NumberOfLateralSpreadingDevices.isEmpty() &&
           RecordedLateralSpreadingDeviceSequence.isEmpty() &&
           NumberOfRangeModulators.isEmpty() &&
           RecordedRangeModulatorSequence.isEmpty() &&
           PatientSupportType.isEmpty() &&
           PatientSupportID.isEmpty() &&
           PatientSupportAccessoryCode.isEmpty() &&
           FixationLightAzimuthalAngle.isEmpty() &&
           FixationLightPolarAngle.isEmpty() &&
           FixationEye.isEmpty() &&
           CurrentFractionNumber.isEmpty() &&
           TreatmentDeliveryType.isEmpty() &&
           TreatmentTerminationStatus.isEmpty() &&
           TreatmentTerminationCode.isEmpty() &&
           TreatmentVerificationStatus.isEmpty() &&
           SpecifiedPrimaryMeterset.isEmpty() &&
           SpecifiedSecondaryMeterset.isEmpty() &&
           DeliveredPrimaryMeterset.isEmpty() &&
           DeliveredSecondaryMeterset.isEmpty() &&
           SpecifiedTreatmentTime.isEmpty() &&
           DeliveredTreatmentTime.isEmpty() &&
           DeliveredDepthDoseParametersSequence.isEmpty() &&
           NumberOfControlPoints.isEmpty() &&
           IonControlPointDeliverySequence.isEmpty();
}


OFBool DRTTreatmentSessionIonBeamSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ReferencedBeamNumber, "1", "1", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, BeamName, "1", "1", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, BeamDescription, "1", "3", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, BeamType, "1", "1", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, RadiationType, "1", "1", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, RadiationMassNumber, "1", "1C", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, RadiationAtomicNumber, "1", "1C", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, RadiationChargeState, "1", "1C", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, ScanMode, "1", "1", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, ModulatedScanModeType, "1", "1C", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, ReferencedToleranceTableNumber, "1", "3", "TreatmentSessionIonBeamSequence");
        BeamLimitingDeviceLeafPairsSequence.read(item, "1-n", "3", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, ReferencedPatientSetupNumber, "1", "3", "TreatmentSessionIonBeamSequence");
        ReferencedVerificationImageSequence.read(item, "1-n", "3", "TreatmentSessionIonBeamSequence");
        ReferencedMeasuredDoseReferenceSequence.read(item, "1-n", "3", "TreatmentSessionIonBeamSequence");
        ReferencedCalculatedDoseReferenceSequence.read(item, "1-n", "3", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, NumberOfWedges, "1", "1", "TreatmentSessionIonBeamSequence");
        RecordedWedgeSequence.read(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, NumberOfCompensators, "1", "1", "TreatmentSessionIonBeamSequence");
        RecordedCompensatorSequence.read(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, NumberOfBoli, "1", "1", "TreatmentSessionIonBeamSequence");
        ReferencedBolusSequence.read(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, NumberOfBlocks, "1", "1", "TreatmentSessionIonBeamSequence");
        RecordedBlockSequence.read(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        RecordedSnoutSequence.read(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        ApplicatorSequence.read(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        GeneralAccessorySequence.read(item, "1-n", "3", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, NumberOfRangeShifters, "1", "1", "TreatmentSessionIonBeamSequence");
        RecordedRangeShifterSequence.read(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, NumberOfLateralSpreadingDevices, "1", "1", "TreatmentSessionIonBeamSequence");
        RecordedLateralSpreadingDeviceSequence.read(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, NumberOfRangeModulators, "1", "1", "TreatmentSessionIonBeamSequence");
        RecordedRangeModulatorSequence.read(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, PatientSupportType, "1", "1", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, PatientSupportID, "1", "3", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, PatientSupportAccessoryCode, "1", "3", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, FixationLightAzimuthalAngle, "1", "3", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, FixationLightPolarAngle, "1", "3", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, FixationEye, "1", "3", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, CurrentFractionNumber, "1", "2", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, TreatmentDeliveryType, "1", "2", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, TreatmentTerminationStatus, "1", "1", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, TreatmentTerminationCode, "1", "3", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, TreatmentVerificationStatus, "1", "2", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, SpecifiedPrimaryMeterset, "1", "3", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, SpecifiedSecondaryMeterset, "1", "3", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, DeliveredPrimaryMeterset, "1", "3", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, DeliveredSecondaryMeterset, "1", "3", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, SpecifiedTreatmentTime, "1", "3", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, DeliveredTreatmentTime, "1", "3", "TreatmentSessionIonBeamSequence");
        DeliveredDepthDoseParametersSequence.read(item, "1-n", "3", "TreatmentSessionIonBeamSequence");
        getAndCheckElementFromDataset(item, NumberOfControlPoints, "1", "1", "TreatmentSessionIonBeamSequence");
        IonControlPointDeliverySequence.read(item, "1-n", "1", "TreatmentSessionIonBeamSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ReferencedBeamNumber), "1", "1", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmLongString(BeamName), "1", "1", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmShortText(BeamDescription), "1", "3", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(BeamType), "1", "1", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(RadiationType), "1", "1", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(RadiationMassNumber), "1", "1C", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(RadiationAtomicNumber), "1", "1C", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmSignedShort(RadiationChargeState), "1", "1C", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(ScanMode), "1", "1", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(ModulatedScanModeType), "1", "1C", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferencedToleranceTableNumber), "1", "3", "TreatmentSessionIonBeamSequence");
        if (result.good()) result = BeamLimitingDeviceLeafPairsSequence.write(item, "1-n", "3", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferencedPatientSetupNumber), "1", "3", "TreatmentSessionIonBeamSequence");
        if (result.good()) result = ReferencedVerificationImageSequence.write(item, "1-n", "3", "TreatmentSessionIonBeamSequence");
        if (result.good()) result = ReferencedMeasuredDoseReferenceSequence.write(item, "1-n", "3", "TreatmentSessionIonBeamSequence");
        if (result.good()) result = ReferencedCalculatedDoseReferenceSequence.write(item, "1-n", "3", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfWedges), "1", "1", "TreatmentSessionIonBeamSequence");
        if (result.good()) result = RecordedWedgeSequence.write(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfCompensators), "1", "1", "TreatmentSessionIonBeamSequence");
        if (result.good()) result = RecordedCompensatorSequence.write(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfBoli), "1", "1", "TreatmentSessionIonBeamSequence");
        if (result.good()) result = ReferencedBolusSequence.write(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfBlocks), "1", "1", "TreatmentSessionIonBeamSequence");
        if (result.good()) result = RecordedBlockSequence.write(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        if (result.good()) result = RecordedSnoutSequence.write(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        if (result.good()) result = ApplicatorSequence.write(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        if (result.good()) result = GeneralAccessorySequence.write(item, "1-n", "3", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfRangeShifters), "1", "1", "TreatmentSessionIonBeamSequence");
        if (result.good()) result = RecordedRangeShifterSequence.write(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfLateralSpreadingDevices), "1", "1", "TreatmentSessionIonBeamSequence");
        if (result.good()) result = RecordedLateralSpreadingDeviceSequence.write(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfRangeModulators), "1", "1", "TreatmentSessionIonBeamSequence");
        if (result.good()) result = RecordedRangeModulatorSequence.write(item, "1-n", "1C", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(PatientSupportType), "1", "1", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmShortString(PatientSupportID), "1", "3", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmLongString(PatientSupportAccessoryCode), "1", "3", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(FixationLightAzimuthalAngle), "1", "3", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(FixationLightPolarAngle), "1", "3", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(FixationEye), "1", "3", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(CurrentFractionNumber), "1", "2", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(TreatmentDeliveryType), "1", "2", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(TreatmentTerminationStatus), "1", "1", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmShortString(TreatmentTerminationCode), "1", "3", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmCodeString(TreatmentVerificationStatus), "1", "2", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmDecimalString(SpecifiedPrimaryMeterset), "1", "3", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmDecimalString(SpecifiedSecondaryMeterset), "1", "3", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmDecimalString(DeliveredPrimaryMeterset), "1", "3", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmDecimalString(DeliveredSecondaryMeterset), "1", "3", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmDecimalString(SpecifiedTreatmentTime), "1", "3", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmDecimalString(DeliveredTreatmentTime), "1", "3", "TreatmentSessionIonBeamSequence");
        if (result.good()) result = DeliveredDepthDoseParametersSequence.write(item, "1-n", "3", "TreatmentSessionIonBeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfControlPoints), "1", "1", "TreatmentSessionIonBeamSequence");
        if (result.good()) result = IonControlPointDeliverySequence.write(item, "1-n", "1", "TreatmentSessionIonBeamSequence");
    }
    return result;
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getBeamDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamDescription, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getBeamName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamName, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getBeamType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamType, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getCurrentFractionNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CurrentFractionNumber, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getCurrentFractionNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, CurrentFractionNumber).getSint32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getDeliveredPrimaryMeterset(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeliveredPrimaryMeterset, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getDeliveredPrimaryMeterset(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DeliveredPrimaryMeterset).getFloat64(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getDeliveredSecondaryMeterset(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeliveredSecondaryMeterset, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getDeliveredSecondaryMeterset(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DeliveredSecondaryMeterset).getFloat64(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getDeliveredTreatmentTime(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeliveredTreatmentTime, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getDeliveredTreatmentTime(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DeliveredTreatmentTime).getFloat64(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getFixationEye(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FixationEye, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getFixationLightAzimuthalAngle(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, FixationLightAzimuthalAngle).getFloat32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getFixationLightPolarAngle(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, FixationLightPolarAngle).getFloat32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getModulatedScanModeType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ModulatedScanModeType, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getNumberOfBlocks(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfBlocks, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getNumberOfBlocks(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfBlocks).getSint32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getNumberOfBoli(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfBoli, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getNumberOfBoli(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfBoli).getSint32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getNumberOfCompensators(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfCompensators, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getNumberOfCompensators(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfCompensators).getSint32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getNumberOfControlPoints(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfControlPoints, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getNumberOfControlPoints(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfControlPoints).getSint32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getNumberOfLateralSpreadingDevices(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfLateralSpreadingDevices, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getNumberOfLateralSpreadingDevices(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfLateralSpreadingDevices).getSint32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getNumberOfRangeModulators(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfRangeModulators, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getNumberOfRangeModulators(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfRangeModulators).getSint32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getNumberOfRangeShifters(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfRangeShifters, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getNumberOfRangeShifters(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfRangeShifters).getSint32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getNumberOfWedges(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfWedges, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getNumberOfWedges(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfWedges).getSint32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getPatientSupportAccessoryCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientSupportAccessoryCode, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getPatientSupportID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientSupportID, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getPatientSupportType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PatientSupportType, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getRadiationAtomicNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RadiationAtomicNumber, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getRadiationAtomicNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, RadiationAtomicNumber).getSint32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getRadiationChargeState(Sint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmSignedShort &, RadiationChargeState).getSint16(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getRadiationMassNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RadiationMassNumber, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getRadiationMassNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, RadiationMassNumber).getSint32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getRadiationType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RadiationType, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getReferencedBeamNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedBeamNumber, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getReferencedBeamNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedBeamNumber).getSint32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getReferencedPatientSetupNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedPatientSetupNumber, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getReferencedPatientSetupNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedPatientSetupNumber).getSint32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getReferencedToleranceTableNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedToleranceTableNumber, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getReferencedToleranceTableNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedToleranceTableNumber).getSint32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getScanMode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ScanMode, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getSpecifiedPrimaryMeterset(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SpecifiedPrimaryMeterset, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getSpecifiedPrimaryMeterset(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SpecifiedPrimaryMeterset).getFloat64(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getSpecifiedSecondaryMeterset(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SpecifiedSecondaryMeterset, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getSpecifiedSecondaryMeterset(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SpecifiedSecondaryMeterset).getFloat64(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getSpecifiedTreatmentTime(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SpecifiedTreatmentTime, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getSpecifiedTreatmentTime(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SpecifiedTreatmentTime).getFloat64(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getTreatmentDeliveryType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TreatmentDeliveryType, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getTreatmentTerminationCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TreatmentTerminationCode, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getTreatmentTerminationStatus(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TreatmentTerminationStatus, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::getTreatmentVerificationStatus(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TreatmentVerificationStatus, value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setBeamDescription(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setBeamName(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setBeamType(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setCurrentFractionNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CurrentFractionNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setDeliveredPrimaryMeterset(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DeliveredPrimaryMeterset.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setDeliveredSecondaryMeterset(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DeliveredSecondaryMeterset.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setDeliveredTreatmentTime(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DeliveredTreatmentTime.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setFixationEye(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setFixationLightAzimuthalAngle(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return FixationLightAzimuthalAngle.putFloat32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setFixationLightPolarAngle(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return FixationLightPolarAngle.putFloat32(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setModulatedScanModeType(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setNumberOfBlocks(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setNumberOfBoli(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setNumberOfCompensators(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setNumberOfControlPoints(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setNumberOfLateralSpreadingDevices(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setNumberOfRangeModulators(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setNumberOfRangeShifters(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setNumberOfWedges(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setPatientSupportAccessoryCode(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setPatientSupportID(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setPatientSupportType(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setRadiationAtomicNumber(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setRadiationChargeState(const Sint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RadiationChargeState.putSint16(value, pos);
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setRadiationMassNumber(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setRadiationType(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setReferencedBeamNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedBeamNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setReferencedPatientSetupNumber(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setReferencedToleranceTableNumber(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setScanMode(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setSpecifiedPrimaryMeterset(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SpecifiedPrimaryMeterset.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setSpecifiedSecondaryMeterset(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SpecifiedSecondaryMeterset.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setSpecifiedTreatmentTime(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SpecifiedTreatmentTime.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setTreatmentDeliveryType(const OFString &value, const OFBool check)
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


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setTreatmentTerminationCode(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TreatmentTerminationCode.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setTreatmentTerminationStatus(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TreatmentTerminationStatus.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTTreatmentSessionIonBeamSequence::Item::setTreatmentVerificationStatus(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TreatmentVerificationStatus.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTTreatmentSessionIonBeamSequence::DRTTreatmentSessionIonBeamSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTTreatmentSessionIonBeamSequence::DRTTreatmentSessionIonBeamSequence(const DRTTreatmentSessionIonBeamSequence &copy)
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


DRTTreatmentSessionIonBeamSequence &DRTTreatmentSessionIonBeamSequence::operator=(const DRTTreatmentSessionIonBeamSequence &copy)
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


DRTTreatmentSessionIonBeamSequence::~DRTTreatmentSessionIonBeamSequence()
{
    clear();
}


void DRTTreatmentSessionIonBeamSequence::clear()
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


OFBool DRTTreatmentSessionIonBeamSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTTreatmentSessionIonBeamSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTTreatmentSessionIonBeamSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTTreatmentSessionIonBeamSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTTreatmentSessionIonBeamSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTTreatmentSessionIonBeamSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTTreatmentSessionIonBeamSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTTreatmentSessionIonBeamSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTTreatmentSessionIonBeamSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTTreatmentSessionIonBeamSequence::Item &DRTTreatmentSessionIonBeamSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTTreatmentSessionIonBeamSequence::Item &DRTTreatmentSessionIonBeamSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTTreatmentSessionIonBeamSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTTreatmentSessionIonBeamSequence::Item &DRTTreatmentSessionIonBeamSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTTreatmentSessionIonBeamSequence::Item &DRTTreatmentSessionIonBeamSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTTreatmentSessionIonBeamSequence::Item &DRTTreatmentSessionIonBeamSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTTreatmentSessionIonBeamSequence::Item &DRTTreatmentSessionIonBeamSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTTreatmentSessionIonBeamSequence::addItem(Item *&item)
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


OFCondition DRTTreatmentSessionIonBeamSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTTreatmentSessionIonBeamSequence::removeItem(const unsigned long pos)
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


OFCondition DRTTreatmentSessionIonBeamSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_TreatmentSessionIonBeamSequence, sequence);
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
            DcmSequenceOfItems element(DCM_TreatmentSessionIonBeamSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTTreatmentSessionIonBeamSequence::write(DcmItem &dataset,
                                                      const OFString &card,
                                                      const OFString &type,
                                                      const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_TreatmentSessionIonBeamSequence);
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
