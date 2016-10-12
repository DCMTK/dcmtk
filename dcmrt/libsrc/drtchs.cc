/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTChannelSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtchs.h"


// --- item class ---

DRTChannelSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    BrachyControlPointSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ChannelLength(DCM_ChannelLength),
    ChannelNumber(DCM_ChannelNumber),
    ChannelShieldSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ChannelTotalTime(DCM_ChannelTotalTime),
    FinalCumulativeTimeWeight(DCM_FinalCumulativeTimeWeight),
    MaterialID(DCM_MaterialID),
    NumberOfControlPoints(DCM_NumberOfControlPoints),
    NumberOfPulses(DCM_NumberOfPulses),
    PulseRepetitionInterval(DCM_PulseRepetitionInterval),
    ReferencedROINumber(DCM_ReferencedROINumber),
    ReferencedSourceNumber(DCM_ReferencedSourceNumber),
    SourceApplicatorID(DCM_SourceApplicatorID),
    SourceApplicatorLength(DCM_SourceApplicatorLength),
    SourceApplicatorManufacturer(DCM_SourceApplicatorManufacturer),
    SourceApplicatorName(DCM_SourceApplicatorName),
    SourceApplicatorNumber(DCM_SourceApplicatorNumber),
    SourceApplicatorStepSize(DCM_SourceApplicatorStepSize),
    SourceApplicatorType(DCM_SourceApplicatorType),
    SourceApplicatorWallNominalThickness(DCM_SourceApplicatorWallNominalThickness),
    SourceApplicatorWallNominalTransmission(DCM_SourceApplicatorWallNominalTransmission),
    SourceMovementType(DCM_SourceMovementType),
    TransferTubeLength(DCM_TransferTubeLength),
    TransferTubeNumber(DCM_TransferTubeNumber)
{
}


DRTChannelSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    BrachyControlPointSequence(copy.BrachyControlPointSequence),
    ChannelLength(copy.ChannelLength),
    ChannelNumber(copy.ChannelNumber),
    ChannelShieldSequence(copy.ChannelShieldSequence),
    ChannelTotalTime(copy.ChannelTotalTime),
    FinalCumulativeTimeWeight(copy.FinalCumulativeTimeWeight),
    MaterialID(copy.MaterialID),
    NumberOfControlPoints(copy.NumberOfControlPoints),
    NumberOfPulses(copy.NumberOfPulses),
    PulseRepetitionInterval(copy.PulseRepetitionInterval),
    ReferencedROINumber(copy.ReferencedROINumber),
    ReferencedSourceNumber(copy.ReferencedSourceNumber),
    SourceApplicatorID(copy.SourceApplicatorID),
    SourceApplicatorLength(copy.SourceApplicatorLength),
    SourceApplicatorManufacturer(copy.SourceApplicatorManufacturer),
    SourceApplicatorName(copy.SourceApplicatorName),
    SourceApplicatorNumber(copy.SourceApplicatorNumber),
    SourceApplicatorStepSize(copy.SourceApplicatorStepSize),
    SourceApplicatorType(copy.SourceApplicatorType),
    SourceApplicatorWallNominalThickness(copy.SourceApplicatorWallNominalThickness),
    SourceApplicatorWallNominalTransmission(copy.SourceApplicatorWallNominalTransmission),
    SourceMovementType(copy.SourceMovementType),
    TransferTubeLength(copy.TransferTubeLength),
    TransferTubeNumber(copy.TransferTubeNumber)
{
}


DRTChannelSequence::Item::~Item()
{
}


DRTChannelSequence::Item &DRTChannelSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        BrachyControlPointSequence = copy.BrachyControlPointSequence;
        ChannelLength = copy.ChannelLength;
        ChannelNumber = copy.ChannelNumber;
        ChannelShieldSequence = copy.ChannelShieldSequence;
        ChannelTotalTime = copy.ChannelTotalTime;
        FinalCumulativeTimeWeight = copy.FinalCumulativeTimeWeight;
        MaterialID = copy.MaterialID;
        NumberOfControlPoints = copy.NumberOfControlPoints;
        NumberOfPulses = copy.NumberOfPulses;
        PulseRepetitionInterval = copy.PulseRepetitionInterval;
        ReferencedROINumber = copy.ReferencedROINumber;
        ReferencedSourceNumber = copy.ReferencedSourceNumber;
        SourceApplicatorID = copy.SourceApplicatorID;
        SourceApplicatorLength = copy.SourceApplicatorLength;
        SourceApplicatorManufacturer = copy.SourceApplicatorManufacturer;
        SourceApplicatorName = copy.SourceApplicatorName;
        SourceApplicatorNumber = copy.SourceApplicatorNumber;
        SourceApplicatorStepSize = copy.SourceApplicatorStepSize;
        SourceApplicatorType = copy.SourceApplicatorType;
        SourceApplicatorWallNominalThickness = copy.SourceApplicatorWallNominalThickness;
        SourceApplicatorWallNominalTransmission = copy.SourceApplicatorWallNominalTransmission;
        SourceMovementType = copy.SourceMovementType;
        TransferTubeLength = copy.TransferTubeLength;
        TransferTubeNumber = copy.TransferTubeNumber;
    }
    return *this;
}


void DRTChannelSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ChannelNumber.clear();
        ChannelLength.clear();
        ChannelTotalTime.clear();
        SourceMovementType.clear();
        NumberOfPulses.clear();
        PulseRepetitionInterval.clear();
        SourceApplicatorNumber.clear();
        SourceApplicatorID.clear();
        SourceApplicatorType.clear();
        SourceApplicatorName.clear();
        SourceApplicatorLength.clear();
        SourceApplicatorManufacturer.clear();
        MaterialID.clear();
        SourceApplicatorWallNominalThickness.clear();
        SourceApplicatorWallNominalTransmission.clear();
        SourceApplicatorStepSize.clear();
        ReferencedROINumber.clear();
        TransferTubeNumber.clear();
        TransferTubeLength.clear();
        ChannelShieldSequence.clear();
        ReferencedSourceNumber.clear();
        NumberOfControlPoints.clear();
        FinalCumulativeTimeWeight.clear();
        BrachyControlPointSequence.clear();
    }
}


OFBool DRTChannelSequence::Item::isEmpty()
{
    return ChannelNumber.isEmpty() &&
           ChannelLength.isEmpty() &&
           ChannelTotalTime.isEmpty() &&
           SourceMovementType.isEmpty() &&
           NumberOfPulses.isEmpty() &&
           PulseRepetitionInterval.isEmpty() &&
           SourceApplicatorNumber.isEmpty() &&
           SourceApplicatorID.isEmpty() &&
           SourceApplicatorType.isEmpty() &&
           SourceApplicatorName.isEmpty() &&
           SourceApplicatorLength.isEmpty() &&
           SourceApplicatorManufacturer.isEmpty() &&
           MaterialID.isEmpty() &&
           SourceApplicatorWallNominalThickness.isEmpty() &&
           SourceApplicatorWallNominalTransmission.isEmpty() &&
           SourceApplicatorStepSize.isEmpty() &&
           ReferencedROINumber.isEmpty() &&
           TransferTubeNumber.isEmpty() &&
           TransferTubeLength.isEmpty() &&
           ChannelShieldSequence.isEmpty() &&
           ReferencedSourceNumber.isEmpty() &&
           NumberOfControlPoints.isEmpty() &&
           FinalCumulativeTimeWeight.isEmpty() &&
           BrachyControlPointSequence.isEmpty();
}


OFBool DRTChannelSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTChannelSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ChannelNumber, "1", "1", "ChannelSequence");
        getAndCheckElementFromDataset(item, ChannelLength, "1", "2", "ChannelSequence");
        getAndCheckElementFromDataset(item, ChannelTotalTime, "1", "1", "ChannelSequence");
        getAndCheckElementFromDataset(item, SourceMovementType, "1", "1", "ChannelSequence");
        getAndCheckElementFromDataset(item, NumberOfPulses, "1", "1C", "ChannelSequence");
        getAndCheckElementFromDataset(item, PulseRepetitionInterval, "1", "1C", "ChannelSequence");
        getAndCheckElementFromDataset(item, SourceApplicatorNumber, "1", "3", "ChannelSequence");
        getAndCheckElementFromDataset(item, SourceApplicatorID, "1", "2C", "ChannelSequence");
        getAndCheckElementFromDataset(item, SourceApplicatorType, "1", "1C", "ChannelSequence");
        getAndCheckElementFromDataset(item, SourceApplicatorName, "1", "3", "ChannelSequence");
        getAndCheckElementFromDataset(item, SourceApplicatorLength, "1", "1C", "ChannelSequence");
        getAndCheckElementFromDataset(item, SourceApplicatorManufacturer, "1", "3", "ChannelSequence");
        getAndCheckElementFromDataset(item, MaterialID, "1", "3", "ChannelSequence");
        getAndCheckElementFromDataset(item, SourceApplicatorWallNominalThickness, "1", "3", "ChannelSequence");
        getAndCheckElementFromDataset(item, SourceApplicatorWallNominalTransmission, "1", "3", "ChannelSequence");
        getAndCheckElementFromDataset(item, SourceApplicatorStepSize, "1", "1C", "ChannelSequence");
        getAndCheckElementFromDataset(item, ReferencedROINumber, "1", "2C", "ChannelSequence");
        getAndCheckElementFromDataset(item, TransferTubeNumber, "1", "2", "ChannelSequence");
        getAndCheckElementFromDataset(item, TransferTubeLength, "1", "2C", "ChannelSequence");
        ChannelShieldSequence.read(item, "1-n", "3", "ChannelSequence");
        getAndCheckElementFromDataset(item, ReferencedSourceNumber, "1", "1", "ChannelSequence");
        getAndCheckElementFromDataset(item, NumberOfControlPoints, "1", "1", "ChannelSequence");
        getAndCheckElementFromDataset(item, FinalCumulativeTimeWeight, "1", "1C", "ChannelSequence");
        BrachyControlPointSequence.read(item, "1-n", "1", "ChannelSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTChannelSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ChannelNumber), "1", "1", "ChannelSequence");
        addElementToDataset(result, item, new DcmDecimalString(ChannelLength), "1", "2", "ChannelSequence");
        addElementToDataset(result, item, new DcmDecimalString(ChannelTotalTime), "1", "1", "ChannelSequence");
        addElementToDataset(result, item, new DcmCodeString(SourceMovementType), "1", "1", "ChannelSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfPulses), "1", "1C", "ChannelSequence");
        addElementToDataset(result, item, new DcmDecimalString(PulseRepetitionInterval), "1", "1C", "ChannelSequence");
        addElementToDataset(result, item, new DcmIntegerString(SourceApplicatorNumber), "1", "3", "ChannelSequence");
        addElementToDataset(result, item, new DcmShortString(SourceApplicatorID), "1", "2C", "ChannelSequence");
        addElementToDataset(result, item, new DcmCodeString(SourceApplicatorType), "1", "1C", "ChannelSequence");
        addElementToDataset(result, item, new DcmLongString(SourceApplicatorName), "1", "3", "ChannelSequence");
        addElementToDataset(result, item, new DcmDecimalString(SourceApplicatorLength), "1", "1C", "ChannelSequence");
        addElementToDataset(result, item, new DcmLongString(SourceApplicatorManufacturer), "1", "3", "ChannelSequence");
        addElementToDataset(result, item, new DcmShortString(MaterialID), "1", "3", "ChannelSequence");
        addElementToDataset(result, item, new DcmDecimalString(SourceApplicatorWallNominalThickness), "1", "3", "ChannelSequence");
        addElementToDataset(result, item, new DcmDecimalString(SourceApplicatorWallNominalTransmission), "1", "3", "ChannelSequence");
        addElementToDataset(result, item, new DcmDecimalString(SourceApplicatorStepSize), "1", "1C", "ChannelSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferencedROINumber), "1", "2C", "ChannelSequence");
        addElementToDataset(result, item, new DcmIntegerString(TransferTubeNumber), "1", "2", "ChannelSequence");
        addElementToDataset(result, item, new DcmDecimalString(TransferTubeLength), "1", "2C", "ChannelSequence");
        if (result.good()) result = ChannelShieldSequence.write(item, "1-n", "3", "ChannelSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferencedSourceNumber), "1", "1", "ChannelSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfControlPoints), "1", "1", "ChannelSequence");
        addElementToDataset(result, item, new DcmDecimalString(FinalCumulativeTimeWeight), "1", "1C", "ChannelSequence");
        if (result.good()) result = BrachyControlPointSequence.write(item, "1-n", "1", "ChannelSequence");
    }
    return result;
}


OFCondition DRTChannelSequence::Item::getChannelLength(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ChannelLength, value, pos);
}


OFCondition DRTChannelSequence::Item::getChannelLength(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ChannelLength).getFloat64(value, pos);
}


OFCondition DRTChannelSequence::Item::getChannelNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ChannelNumber, value, pos);
}


OFCondition DRTChannelSequence::Item::getChannelNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ChannelNumber).getSint32(value, pos);
}


OFCondition DRTChannelSequence::Item::getChannelTotalTime(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ChannelTotalTime, value, pos);
}


OFCondition DRTChannelSequence::Item::getChannelTotalTime(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ChannelTotalTime).getFloat64(value, pos);
}


OFCondition DRTChannelSequence::Item::getFinalCumulativeTimeWeight(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FinalCumulativeTimeWeight, value, pos);
}


OFCondition DRTChannelSequence::Item::getFinalCumulativeTimeWeight(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, FinalCumulativeTimeWeight).getFloat64(value, pos);
}


OFCondition DRTChannelSequence::Item::getMaterialID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MaterialID, value, pos);
}


OFCondition DRTChannelSequence::Item::getNumberOfControlPoints(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfControlPoints, value, pos);
}


OFCondition DRTChannelSequence::Item::getNumberOfControlPoints(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfControlPoints).getSint32(value, pos);
}


OFCondition DRTChannelSequence::Item::getNumberOfPulses(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfPulses, value, pos);
}


OFCondition DRTChannelSequence::Item::getNumberOfPulses(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfPulses).getSint32(value, pos);
}


OFCondition DRTChannelSequence::Item::getPulseRepetitionInterval(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PulseRepetitionInterval, value, pos);
}


OFCondition DRTChannelSequence::Item::getPulseRepetitionInterval(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, PulseRepetitionInterval).getFloat64(value, pos);
}


OFCondition DRTChannelSequence::Item::getReferencedROINumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedROINumber, value, pos);
}


OFCondition DRTChannelSequence::Item::getReferencedROINumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedROINumber).getSint32(value, pos);
}


OFCondition DRTChannelSequence::Item::getReferencedSourceNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedSourceNumber, value, pos);
}


OFCondition DRTChannelSequence::Item::getReferencedSourceNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedSourceNumber).getSint32(value, pos);
}


OFCondition DRTChannelSequence::Item::getSourceApplicatorID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceApplicatorID, value, pos);
}


OFCondition DRTChannelSequence::Item::getSourceApplicatorLength(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceApplicatorLength, value, pos);
}


OFCondition DRTChannelSequence::Item::getSourceApplicatorLength(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SourceApplicatorLength).getFloat64(value, pos);
}


OFCondition DRTChannelSequence::Item::getSourceApplicatorManufacturer(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceApplicatorManufacturer, value, pos);
}


OFCondition DRTChannelSequence::Item::getSourceApplicatorName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceApplicatorName, value, pos);
}


OFCondition DRTChannelSequence::Item::getSourceApplicatorNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceApplicatorNumber, value, pos);
}


OFCondition DRTChannelSequence::Item::getSourceApplicatorNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, SourceApplicatorNumber).getSint32(value, pos);
}


OFCondition DRTChannelSequence::Item::getSourceApplicatorStepSize(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceApplicatorStepSize, value, pos);
}


OFCondition DRTChannelSequence::Item::getSourceApplicatorStepSize(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SourceApplicatorStepSize).getFloat64(value, pos);
}


OFCondition DRTChannelSequence::Item::getSourceApplicatorType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceApplicatorType, value, pos);
}


OFCondition DRTChannelSequence::Item::getSourceApplicatorWallNominalThickness(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceApplicatorWallNominalThickness, value, pos);
}


OFCondition DRTChannelSequence::Item::getSourceApplicatorWallNominalThickness(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SourceApplicatorWallNominalThickness).getFloat64(value, pos);
}


OFCondition DRTChannelSequence::Item::getSourceApplicatorWallNominalTransmission(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceApplicatorWallNominalTransmission, value, pos);
}


OFCondition DRTChannelSequence::Item::getSourceApplicatorWallNominalTransmission(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SourceApplicatorWallNominalTransmission).getFloat64(value, pos);
}


OFCondition DRTChannelSequence::Item::getSourceMovementType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceMovementType, value, pos);
}


OFCondition DRTChannelSequence::Item::getTransferTubeLength(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TransferTubeLength, value, pos);
}


OFCondition DRTChannelSequence::Item::getTransferTubeLength(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TransferTubeLength).getFloat64(value, pos);
}


OFCondition DRTChannelSequence::Item::getTransferTubeNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TransferTubeNumber, value, pos);
}


OFCondition DRTChannelSequence::Item::getTransferTubeNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, TransferTubeNumber).getSint32(value, pos);
}


OFCondition DRTChannelSequence::Item::setChannelLength(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ChannelLength.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setChannelNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ChannelNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setChannelTotalTime(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ChannelTotalTime.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setFinalCumulativeTimeWeight(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = FinalCumulativeTimeWeight.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setMaterialID(const OFString &value, const OFBool check)
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


OFCondition DRTChannelSequence::Item::setNumberOfControlPoints(const OFString &value, const OFBool check)
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


OFCondition DRTChannelSequence::Item::setNumberOfPulses(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfPulses.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setPulseRepetitionInterval(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PulseRepetitionInterval.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setReferencedROINumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedROINumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setReferencedSourceNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedSourceNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setSourceApplicatorID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceApplicatorID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setSourceApplicatorLength(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceApplicatorLength.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setSourceApplicatorManufacturer(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceApplicatorManufacturer.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setSourceApplicatorName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceApplicatorName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setSourceApplicatorNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceApplicatorNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setSourceApplicatorStepSize(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceApplicatorStepSize.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setSourceApplicatorType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceApplicatorType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setSourceApplicatorWallNominalThickness(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceApplicatorWallNominalThickness.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setSourceApplicatorWallNominalTransmission(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceApplicatorWallNominalTransmission.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setSourceMovementType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceMovementType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setTransferTubeLength(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TransferTubeLength.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTChannelSequence::Item::setTransferTubeNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TransferTubeNumber.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTChannelSequence::DRTChannelSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTChannelSequence::DRTChannelSequence(const DRTChannelSequence &copy)
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


DRTChannelSequence &DRTChannelSequence::operator=(const DRTChannelSequence &copy)
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


DRTChannelSequence::~DRTChannelSequence()
{
    clear();
}


void DRTChannelSequence::clear()
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


OFBool DRTChannelSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTChannelSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTChannelSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTChannelSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTChannelSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTChannelSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTChannelSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTChannelSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTChannelSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTChannelSequence::Item &DRTChannelSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTChannelSequence::Item &DRTChannelSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTChannelSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTChannelSequence::Item &DRTChannelSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTChannelSequence::Item &DRTChannelSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTChannelSequence::Item &DRTChannelSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTChannelSequence::Item &DRTChannelSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTChannelSequence::addItem(Item *&item)
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


OFCondition DRTChannelSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTChannelSequence::removeItem(const unsigned long pos)
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


OFCondition DRTChannelSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ChannelSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ChannelSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTChannelSequence::write(DcmItem &dataset,
                                      const OFString &card,
                                      const OFString &type,
                                      const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ChannelSequence);
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
