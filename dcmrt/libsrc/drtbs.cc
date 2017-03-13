/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTBeamSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtbs.h"


// --- item class ---

DRTBeamSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ApplicatorSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    BeamDescription(DCM_BeamDescription),
    BeamLimitingDeviceSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    BeamName(DCM_BeamName),
    BeamNumber(DCM_BeamNumber),
    BeamType(DCM_BeamType),
    BlockSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    CompensatorSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ControlPointSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    DeviceSerialNumber(DCM_DeviceSerialNumber),
    FinalCumulativeMetersetWeight(DCM_FinalCumulativeMetersetWeight),
    GeneralAccessorySequence(emptyDefaultItem /*emptyDefaultSequence*/),
    HighDoseTechniqueType(DCM_HighDoseTechniqueType),
    InstitutionAddress(DCM_InstitutionAddress),
    InstitutionName(DCM_InstitutionName),
    InstitutionalDepartmentName(DCM_InstitutionalDepartmentName),
    Manufacturer(DCM_Manufacturer),
    ManufacturerModelName(DCM_ManufacturerModelName),
    NumberOfBlocks(DCM_NumberOfBlocks),
    NumberOfBoli(DCM_NumberOfBoli),
    NumberOfCompensators(DCM_NumberOfCompensators),
    NumberOfControlPoints(DCM_NumberOfControlPoints),
    NumberOfWedges(DCM_NumberOfWedges),
    PlannedVerificationImageSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    PrimaryDosimeterUnit(DCM_PrimaryDosimeterUnit),
    PrimaryFluenceModeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RadiationType(DCM_RadiationType),
    ReferencedBolusSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedDoseSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedPatientSetupNumber(DCM_ReferencedPatientSetupNumber),
    ReferencedReferenceImageSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedToleranceTableNumber(DCM_ReferencedToleranceTableNumber),
    SourceAxisDistance(DCM_SourceAxisDistance),
    TotalBlockTrayFactor(DCM_TotalBlockTrayFactor),
    TotalCompensatorTrayFactor(DCM_TotalCompensatorTrayFactor),
    TreatmentDeliveryType(DCM_TreatmentDeliveryType),
    TreatmentMachineName(DCM_TreatmentMachineName),
    WedgeSequence(emptyDefaultItem /*emptyDefaultSequence*/)
{
}


DRTBeamSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ApplicatorSequence(copy.ApplicatorSequence),
    BeamDescription(copy.BeamDescription),
    BeamLimitingDeviceSequence(copy.BeamLimitingDeviceSequence),
    BeamName(copy.BeamName),
    BeamNumber(copy.BeamNumber),
    BeamType(copy.BeamType),
    BlockSequence(copy.BlockSequence),
    CompensatorSequence(copy.CompensatorSequence),
    ControlPointSequence(copy.ControlPointSequence),
    DeviceSerialNumber(copy.DeviceSerialNumber),
    FinalCumulativeMetersetWeight(copy.FinalCumulativeMetersetWeight),
    GeneralAccessorySequence(copy.GeneralAccessorySequence),
    HighDoseTechniqueType(copy.HighDoseTechniqueType),
    InstitutionAddress(copy.InstitutionAddress),
    InstitutionName(copy.InstitutionName),
    InstitutionalDepartmentName(copy.InstitutionalDepartmentName),
    Manufacturer(copy.Manufacturer),
    ManufacturerModelName(copy.ManufacturerModelName),
    NumberOfBlocks(copy.NumberOfBlocks),
    NumberOfBoli(copy.NumberOfBoli),
    NumberOfCompensators(copy.NumberOfCompensators),
    NumberOfControlPoints(copy.NumberOfControlPoints),
    NumberOfWedges(copy.NumberOfWedges),
    PlannedVerificationImageSequence(copy.PlannedVerificationImageSequence),
    PrimaryDosimeterUnit(copy.PrimaryDosimeterUnit),
    PrimaryFluenceModeSequence(copy.PrimaryFluenceModeSequence),
    RadiationType(copy.RadiationType),
    ReferencedBolusSequence(copy.ReferencedBolusSequence),
    ReferencedDoseSequence(copy.ReferencedDoseSequence),
    ReferencedPatientSetupNumber(copy.ReferencedPatientSetupNumber),
    ReferencedReferenceImageSequence(copy.ReferencedReferenceImageSequence),
    ReferencedToleranceTableNumber(copy.ReferencedToleranceTableNumber),
    SourceAxisDistance(copy.SourceAxisDistance),
    TotalBlockTrayFactor(copy.TotalBlockTrayFactor),
    TotalCompensatorTrayFactor(copy.TotalCompensatorTrayFactor),
    TreatmentDeliveryType(copy.TreatmentDeliveryType),
    TreatmentMachineName(copy.TreatmentMachineName),
    WedgeSequence(copy.WedgeSequence)
{
}


DRTBeamSequence::Item::~Item()
{
}


DRTBeamSequence::Item &DRTBeamSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ApplicatorSequence = copy.ApplicatorSequence;
        BeamDescription = copy.BeamDescription;
        BeamLimitingDeviceSequence = copy.BeamLimitingDeviceSequence;
        BeamName = copy.BeamName;
        BeamNumber = copy.BeamNumber;
        BeamType = copy.BeamType;
        BlockSequence = copy.BlockSequence;
        CompensatorSequence = copy.CompensatorSequence;
        ControlPointSequence = copy.ControlPointSequence;
        DeviceSerialNumber = copy.DeviceSerialNumber;
        FinalCumulativeMetersetWeight = copy.FinalCumulativeMetersetWeight;
        GeneralAccessorySequence = copy.GeneralAccessorySequence;
        HighDoseTechniqueType = copy.HighDoseTechniqueType;
        InstitutionAddress = copy.InstitutionAddress;
        InstitutionName = copy.InstitutionName;
        InstitutionalDepartmentName = copy.InstitutionalDepartmentName;
        Manufacturer = copy.Manufacturer;
        ManufacturerModelName = copy.ManufacturerModelName;
        NumberOfBlocks = copy.NumberOfBlocks;
        NumberOfBoli = copy.NumberOfBoli;
        NumberOfCompensators = copy.NumberOfCompensators;
        NumberOfControlPoints = copy.NumberOfControlPoints;
        NumberOfWedges = copy.NumberOfWedges;
        PlannedVerificationImageSequence = copy.PlannedVerificationImageSequence;
        PrimaryDosimeterUnit = copy.PrimaryDosimeterUnit;
        PrimaryFluenceModeSequence = copy.PrimaryFluenceModeSequence;
        RadiationType = copy.RadiationType;
        ReferencedBolusSequence = copy.ReferencedBolusSequence;
        ReferencedDoseSequence = copy.ReferencedDoseSequence;
        ReferencedPatientSetupNumber = copy.ReferencedPatientSetupNumber;
        ReferencedReferenceImageSequence = copy.ReferencedReferenceImageSequence;
        ReferencedToleranceTableNumber = copy.ReferencedToleranceTableNumber;
        SourceAxisDistance = copy.SourceAxisDistance;
        TotalBlockTrayFactor = copy.TotalBlockTrayFactor;
        TotalCompensatorTrayFactor = copy.TotalCompensatorTrayFactor;
        TreatmentDeliveryType = copy.TreatmentDeliveryType;
        TreatmentMachineName = copy.TreatmentMachineName;
        WedgeSequence = copy.WedgeSequence;
    }
    return *this;
}


void DRTBeamSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        BeamNumber.clear();
        BeamName.clear();
        BeamDescription.clear();
        BeamType.clear();
        RadiationType.clear();
        PrimaryFluenceModeSequence.clear();
        HighDoseTechniqueType.clear();
        TreatmentMachineName.clear();
        Manufacturer.clear();
        InstitutionName.clear();
        InstitutionAddress.clear();
        InstitutionalDepartmentName.clear();
        ManufacturerModelName.clear();
        DeviceSerialNumber.clear();
        PrimaryDosimeterUnit.clear();
        ReferencedToleranceTableNumber.clear();
        SourceAxisDistance.clear();
        BeamLimitingDeviceSequence.clear();
        ReferencedPatientSetupNumber.clear();
        ReferencedReferenceImageSequence.clear();
        PlannedVerificationImageSequence.clear();
        TreatmentDeliveryType.clear();
        ReferencedDoseSequence.clear();
        NumberOfWedges.clear();
        WedgeSequence.clear();
        NumberOfCompensators.clear();
        TotalCompensatorTrayFactor.clear();
        CompensatorSequence.clear();
        NumberOfBoli.clear();
        ReferencedBolusSequence.clear();
        NumberOfBlocks.clear();
        TotalBlockTrayFactor.clear();
        BlockSequence.clear();
        ApplicatorSequence.clear();
        GeneralAccessorySequence.clear();
        FinalCumulativeMetersetWeight.clear();
        NumberOfControlPoints.clear();
        ControlPointSequence.clear();
    }
}


OFBool DRTBeamSequence::Item::isEmpty()
{
    return BeamNumber.isEmpty() &&
           BeamName.isEmpty() &&
           BeamDescription.isEmpty() &&
           BeamType.isEmpty() &&
           RadiationType.isEmpty() &&
           PrimaryFluenceModeSequence.isEmpty() &&
           HighDoseTechniqueType.isEmpty() &&
           TreatmentMachineName.isEmpty() &&
           Manufacturer.isEmpty() &&
           InstitutionName.isEmpty() &&
           InstitutionAddress.isEmpty() &&
           InstitutionalDepartmentName.isEmpty() &&
           ManufacturerModelName.isEmpty() &&
           DeviceSerialNumber.isEmpty() &&
           PrimaryDosimeterUnit.isEmpty() &&
           ReferencedToleranceTableNumber.isEmpty() &&
           SourceAxisDistance.isEmpty() &&
           BeamLimitingDeviceSequence.isEmpty() &&
           ReferencedPatientSetupNumber.isEmpty() &&
           ReferencedReferenceImageSequence.isEmpty() &&
           PlannedVerificationImageSequence.isEmpty() &&
           TreatmentDeliveryType.isEmpty() &&
           ReferencedDoseSequence.isEmpty() &&
           NumberOfWedges.isEmpty() &&
           WedgeSequence.isEmpty() &&
           NumberOfCompensators.isEmpty() &&
           TotalCompensatorTrayFactor.isEmpty() &&
           CompensatorSequence.isEmpty() &&
           NumberOfBoli.isEmpty() &&
           ReferencedBolusSequence.isEmpty() &&
           NumberOfBlocks.isEmpty() &&
           TotalBlockTrayFactor.isEmpty() &&
           BlockSequence.isEmpty() &&
           ApplicatorSequence.isEmpty() &&
           GeneralAccessorySequence.isEmpty() &&
           FinalCumulativeMetersetWeight.isEmpty() &&
           NumberOfControlPoints.isEmpty() &&
           ControlPointSequence.isEmpty();
}


OFBool DRTBeamSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTBeamSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, BeamNumber, "1", "1", "BeamSequence");
        getAndCheckElementFromDataset(item, BeamName, "1", "3", "BeamSequence");
        getAndCheckElementFromDataset(item, BeamDescription, "1", "3", "BeamSequence");
        getAndCheckElementFromDataset(item, BeamType, "1", "1", "BeamSequence");
        getAndCheckElementFromDataset(item, RadiationType, "1", "2", "BeamSequence");
        PrimaryFluenceModeSequence.read(item, "1-n", "3", "BeamSequence");
        getAndCheckElementFromDataset(item, HighDoseTechniqueType, "1", "1C", "BeamSequence");
        getAndCheckElementFromDataset(item, TreatmentMachineName, "1", "2", "BeamSequence");
        getAndCheckElementFromDataset(item, Manufacturer, "1", "3", "BeamSequence");
        getAndCheckElementFromDataset(item, InstitutionName, "1", "3", "BeamSequence");
        getAndCheckElementFromDataset(item, InstitutionAddress, "1", "3", "BeamSequence");
        getAndCheckElementFromDataset(item, InstitutionalDepartmentName, "1", "3", "BeamSequence");
        getAndCheckElementFromDataset(item, ManufacturerModelName, "1", "3", "BeamSequence");
        getAndCheckElementFromDataset(item, DeviceSerialNumber, "1", "3", "BeamSequence");
        getAndCheckElementFromDataset(item, PrimaryDosimeterUnit, "1", "3", "BeamSequence");
        getAndCheckElementFromDataset(item, ReferencedToleranceTableNumber, "1", "3", "BeamSequence");
        getAndCheckElementFromDataset(item, SourceAxisDistance, "1", "3", "BeamSequence");
        BeamLimitingDeviceSequence.read(item, "1-n", "1", "BeamSequence");
        getAndCheckElementFromDataset(item, ReferencedPatientSetupNumber, "1", "3", "BeamSequence");
        ReferencedReferenceImageSequence.read(item, "1-n", "3", "BeamSequence");
        PlannedVerificationImageSequence.read(item, "1-n", "3", "BeamSequence");
        getAndCheckElementFromDataset(item, TreatmentDeliveryType, "1", "3", "BeamSequence");
        ReferencedDoseSequence.read(item, "1-n", "3", "BeamSequence");
        getAndCheckElementFromDataset(item, NumberOfWedges, "1", "1", "BeamSequence");
        WedgeSequence.read(item, "1-n", "1C", "BeamSequence");
        getAndCheckElementFromDataset(item, NumberOfCompensators, "1", "1", "BeamSequence");
        getAndCheckElementFromDataset(item, TotalCompensatorTrayFactor, "1", "3", "BeamSequence");
        CompensatorSequence.read(item, "1-n", "1C", "BeamSequence");
        getAndCheckElementFromDataset(item, NumberOfBoli, "1", "1", "BeamSequence");
        ReferencedBolusSequence.read(item, "1-n", "1C", "BeamSequence");
        getAndCheckElementFromDataset(item, NumberOfBlocks, "1", "1", "BeamSequence");
        getAndCheckElementFromDataset(item, TotalBlockTrayFactor, "1", "3", "BeamSequence");
        BlockSequence.read(item, "1-n", "1C", "BeamSequence");
        ApplicatorSequence.read(item, "1-n", "3", "BeamSequence");
        GeneralAccessorySequence.read(item, "1-n", "3", "BeamSequence");
        getAndCheckElementFromDataset(item, FinalCumulativeMetersetWeight, "1", "1C", "BeamSequence");
        getAndCheckElementFromDataset(item, NumberOfControlPoints, "1", "1", "BeamSequence");
        ControlPointSequence.read(item, "1-n", "1", "BeamSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTBeamSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(BeamNumber), "1", "1", "BeamSequence");
        addElementToDataset(result, item, new DcmLongString(BeamName), "1", "3", "BeamSequence");
        addElementToDataset(result, item, new DcmShortText(BeamDescription), "1", "3", "BeamSequence");
        addElementToDataset(result, item, new DcmCodeString(BeamType), "1", "1", "BeamSequence");
        addElementToDataset(result, item, new DcmCodeString(RadiationType), "1", "2", "BeamSequence");
        if (result.good()) result = PrimaryFluenceModeSequence.write(item, "1-n", "3", "BeamSequence");
        addElementToDataset(result, item, new DcmCodeString(HighDoseTechniqueType), "1", "1C", "BeamSequence");
        addElementToDataset(result, item, new DcmShortString(TreatmentMachineName), "1", "2", "BeamSequence");
        addElementToDataset(result, item, new DcmLongString(Manufacturer), "1", "3", "BeamSequence");
        addElementToDataset(result, item, new DcmLongString(InstitutionName), "1", "3", "BeamSequence");
        addElementToDataset(result, item, new DcmShortText(InstitutionAddress), "1", "3", "BeamSequence");
        addElementToDataset(result, item, new DcmLongString(InstitutionalDepartmentName), "1", "3", "BeamSequence");
        addElementToDataset(result, item, new DcmLongString(ManufacturerModelName), "1", "3", "BeamSequence");
        addElementToDataset(result, item, new DcmLongString(DeviceSerialNumber), "1", "3", "BeamSequence");
        addElementToDataset(result, item, new DcmCodeString(PrimaryDosimeterUnit), "1", "3", "BeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferencedToleranceTableNumber), "1", "3", "BeamSequence");
        addElementToDataset(result, item, new DcmDecimalString(SourceAxisDistance), "1", "3", "BeamSequence");
        if (result.good()) result = BeamLimitingDeviceSequence.write(item, "1-n", "1", "BeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferencedPatientSetupNumber), "1", "3", "BeamSequence");
        if (result.good()) result = ReferencedReferenceImageSequence.write(item, "1-n", "3", "BeamSequence");
        if (result.good()) result = PlannedVerificationImageSequence.write(item, "1-n", "3", "BeamSequence");
        addElementToDataset(result, item, new DcmCodeString(TreatmentDeliveryType), "1", "3", "BeamSequence");
        if (result.good()) result = ReferencedDoseSequence.write(item, "1-n", "3", "BeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfWedges), "1", "1", "BeamSequence");
        if (result.good()) result = WedgeSequence.write(item, "1-n", "1C", "BeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfCompensators), "1", "1", "BeamSequence");
        addElementToDataset(result, item, new DcmDecimalString(TotalCompensatorTrayFactor), "1", "3", "BeamSequence");
        if (result.good()) result = CompensatorSequence.write(item, "1-n", "1C", "BeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfBoli), "1", "1", "BeamSequence");
        if (result.good()) result = ReferencedBolusSequence.write(item, "1-n", "1C", "BeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfBlocks), "1", "1", "BeamSequence");
        addElementToDataset(result, item, new DcmDecimalString(TotalBlockTrayFactor), "1", "3", "BeamSequence");
        if (result.good()) result = BlockSequence.write(item, "1-n", "1C", "BeamSequence");
        if (result.good()) result = ApplicatorSequence.write(item, "1-n", "3", "BeamSequence");
        if (result.good()) result = GeneralAccessorySequence.write(item, "1-n", "3", "BeamSequence");
        addElementToDataset(result, item, new DcmDecimalString(FinalCumulativeMetersetWeight), "1", "1C", "BeamSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfControlPoints), "1", "1", "BeamSequence");
        if (result.good()) result = ControlPointSequence.write(item, "1-n", "1", "BeamSequence");
    }
    return result;
}


OFCondition DRTBeamSequence::Item::getBeamDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamDescription, value, pos);
}


OFCondition DRTBeamSequence::Item::getBeamName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamName, value, pos);
}


OFCondition DRTBeamSequence::Item::getBeamNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamNumber, value, pos);
}


OFCondition DRTBeamSequence::Item::getBeamNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, BeamNumber).getSint32(value, pos);
}


OFCondition DRTBeamSequence::Item::getBeamType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BeamType, value, pos);
}


OFCondition DRTBeamSequence::Item::getDeviceSerialNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeviceSerialNumber, value, pos);
}


OFCondition DRTBeamSequence::Item::getFinalCumulativeMetersetWeight(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FinalCumulativeMetersetWeight, value, pos);
}


OFCondition DRTBeamSequence::Item::getFinalCumulativeMetersetWeight(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, FinalCumulativeMetersetWeight).getFloat64(value, pos);
}


OFCondition DRTBeamSequence::Item::getHighDoseTechniqueType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(HighDoseTechniqueType, value, pos);
}


OFCondition DRTBeamSequence::Item::getInstitutionAddress(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(InstitutionAddress, value, pos);
}


OFCondition DRTBeamSequence::Item::getInstitutionName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(InstitutionName, value, pos);
}


OFCondition DRTBeamSequence::Item::getInstitutionalDepartmentName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(InstitutionalDepartmentName, value, pos);
}


OFCondition DRTBeamSequence::Item::getManufacturer(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(Manufacturer, value, pos);
}


OFCondition DRTBeamSequence::Item::getManufacturerModelName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ManufacturerModelName, value, pos);
}


OFCondition DRTBeamSequence::Item::getNumberOfBlocks(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfBlocks, value, pos);
}


OFCondition DRTBeamSequence::Item::getNumberOfBlocks(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfBlocks).getSint32(value, pos);
}


OFCondition DRTBeamSequence::Item::getNumberOfBoli(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfBoli, value, pos);
}


OFCondition DRTBeamSequence::Item::getNumberOfBoli(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfBoli).getSint32(value, pos);
}


OFCondition DRTBeamSequence::Item::getNumberOfCompensators(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfCompensators, value, pos);
}


OFCondition DRTBeamSequence::Item::getNumberOfCompensators(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfCompensators).getSint32(value, pos);
}


OFCondition DRTBeamSequence::Item::getNumberOfControlPoints(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfControlPoints, value, pos);
}


OFCondition DRTBeamSequence::Item::getNumberOfControlPoints(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfControlPoints).getSint32(value, pos);
}


OFCondition DRTBeamSequence::Item::getNumberOfWedges(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfWedges, value, pos);
}


OFCondition DRTBeamSequence::Item::getNumberOfWedges(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfWedges).getSint32(value, pos);
}


OFCondition DRTBeamSequence::Item::getPrimaryDosimeterUnit(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PrimaryDosimeterUnit, value, pos);
}


OFCondition DRTBeamSequence::Item::getRadiationType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RadiationType, value, pos);
}


OFCondition DRTBeamSequence::Item::getReferencedPatientSetupNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedPatientSetupNumber, value, pos);
}


OFCondition DRTBeamSequence::Item::getReferencedPatientSetupNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedPatientSetupNumber).getSint32(value, pos);
}


OFCondition DRTBeamSequence::Item::getReferencedToleranceTableNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedToleranceTableNumber, value, pos);
}


OFCondition DRTBeamSequence::Item::getReferencedToleranceTableNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedToleranceTableNumber).getSint32(value, pos);
}


OFCondition DRTBeamSequence::Item::getSourceAxisDistance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceAxisDistance, value, pos);
}


OFCondition DRTBeamSequence::Item::getSourceAxisDistance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SourceAxisDistance).getFloat64(value, pos);
}


OFCondition DRTBeamSequence::Item::getTotalBlockTrayFactor(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TotalBlockTrayFactor, value, pos);
}


OFCondition DRTBeamSequence::Item::getTotalBlockTrayFactor(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TotalBlockTrayFactor).getFloat64(value, pos);
}


OFCondition DRTBeamSequence::Item::getTotalCompensatorTrayFactor(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TotalCompensatorTrayFactor, value, pos);
}


OFCondition DRTBeamSequence::Item::getTotalCompensatorTrayFactor(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TotalCompensatorTrayFactor).getFloat64(value, pos);
}


OFCondition DRTBeamSequence::Item::getTreatmentDeliveryType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TreatmentDeliveryType, value, pos);
}


OFCondition DRTBeamSequence::Item::getTreatmentMachineName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TreatmentMachineName, value, pos);
}


OFCondition DRTBeamSequence::Item::setBeamDescription(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setBeamName(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setBeamNumber(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setBeamType(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setDeviceSerialNumber(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setFinalCumulativeMetersetWeight(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setHighDoseTechniqueType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = HighDoseTechniqueType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBeamSequence::Item::setInstitutionAddress(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setInstitutionName(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setInstitutionalDepartmentName(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setManufacturer(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setManufacturerModelName(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setNumberOfBlocks(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setNumberOfBoli(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setNumberOfCompensators(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setNumberOfControlPoints(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setNumberOfWedges(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setPrimaryDosimeterUnit(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setRadiationType(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setReferencedPatientSetupNumber(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setReferencedToleranceTableNumber(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setSourceAxisDistance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceAxisDistance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBeamSequence::Item::setTotalBlockTrayFactor(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TotalBlockTrayFactor.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBeamSequence::Item::setTotalCompensatorTrayFactor(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TotalCompensatorTrayFactor.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBeamSequence::Item::setTreatmentDeliveryType(const OFString &value, const OFBool check)
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


OFCondition DRTBeamSequence::Item::setTreatmentMachineName(const OFString &value, const OFBool check)
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


// --- sequence class ---

DRTBeamSequence::DRTBeamSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTBeamSequence::DRTBeamSequence(const DRTBeamSequence &copy)
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


DRTBeamSequence &DRTBeamSequence::operator=(const DRTBeamSequence &copy)
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


DRTBeamSequence::~DRTBeamSequence()
{
    clear();
}


void DRTBeamSequence::clear()
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


OFBool DRTBeamSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTBeamSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTBeamSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTBeamSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTBeamSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTBeamSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTBeamSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTBeamSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTBeamSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTBeamSequence::Item &DRTBeamSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTBeamSequence::Item &DRTBeamSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTBeamSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTBeamSequence::Item &DRTBeamSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTBeamSequence::Item &DRTBeamSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTBeamSequence::Item &DRTBeamSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTBeamSequence::Item &DRTBeamSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTBeamSequence::addItem(Item *&item)
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


OFCondition DRTBeamSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTBeamSequence::removeItem(const size_t pos)
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


OFCondition DRTBeamSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_BeamSequence, sequence);
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
            DcmSequenceOfItems element(DCM_BeamSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTBeamSequence::write(DcmItem &dataset,
                                   const OFString &card,
                                   const OFString &type,
                                   const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_BeamSequence);
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
