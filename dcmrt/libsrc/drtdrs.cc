/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTDoseReferenceSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtdrs.h"


// --- item class ---

DRTDoseReferenceSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ConstraintWeight(DCM_ConstraintWeight),
    DeliveryMaximumDose(DCM_DeliveryMaximumDose),
    DeliveryWarningDose(DCM_DeliveryWarningDose),
    DoseReferenceDescription(DCM_DoseReferenceDescription),
    DoseReferenceNumber(DCM_DoseReferenceNumber),
    DoseReferencePointCoordinates(DCM_DoseReferencePointCoordinates),
    DoseReferenceStructureType(DCM_DoseReferenceStructureType),
    DoseReferenceType(DCM_DoseReferenceType),
    DoseReferenceUID(DCM_DoseReferenceUID),
    NominalPriorDose(DCM_NominalPriorDose),
    OrganAtRiskFullVolumeDose(DCM_OrganAtRiskFullVolumeDose),
    OrganAtRiskLimitDose(DCM_OrganAtRiskLimitDose),
    OrganAtRiskMaximumDose(DCM_OrganAtRiskMaximumDose),
    OrganAtRiskOverdoseVolumeFraction(DCM_OrganAtRiskOverdoseVolumeFraction),
    ReferencedROINumber(DCM_ReferencedROINumber),
    TargetMaximumDose(DCM_TargetMaximumDose),
    TargetMinimumDose(DCM_TargetMinimumDose),
    TargetPrescriptionDose(DCM_TargetPrescriptionDose),
    TargetUnderdoseVolumeFraction(DCM_TargetUnderdoseVolumeFraction)
{
}


DRTDoseReferenceSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ConstraintWeight(copy.ConstraintWeight),
    DeliveryMaximumDose(copy.DeliveryMaximumDose),
    DeliveryWarningDose(copy.DeliveryWarningDose),
    DoseReferenceDescription(copy.DoseReferenceDescription),
    DoseReferenceNumber(copy.DoseReferenceNumber),
    DoseReferencePointCoordinates(copy.DoseReferencePointCoordinates),
    DoseReferenceStructureType(copy.DoseReferenceStructureType),
    DoseReferenceType(copy.DoseReferenceType),
    DoseReferenceUID(copy.DoseReferenceUID),
    NominalPriorDose(copy.NominalPriorDose),
    OrganAtRiskFullVolumeDose(copy.OrganAtRiskFullVolumeDose),
    OrganAtRiskLimitDose(copy.OrganAtRiskLimitDose),
    OrganAtRiskMaximumDose(copy.OrganAtRiskMaximumDose),
    OrganAtRiskOverdoseVolumeFraction(copy.OrganAtRiskOverdoseVolumeFraction),
    ReferencedROINumber(copy.ReferencedROINumber),
    TargetMaximumDose(copy.TargetMaximumDose),
    TargetMinimumDose(copy.TargetMinimumDose),
    TargetPrescriptionDose(copy.TargetPrescriptionDose),
    TargetUnderdoseVolumeFraction(copy.TargetUnderdoseVolumeFraction)
{
}


DRTDoseReferenceSequence::Item::~Item()
{
}


DRTDoseReferenceSequence::Item &DRTDoseReferenceSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ConstraintWeight = copy.ConstraintWeight;
        DeliveryMaximumDose = copy.DeliveryMaximumDose;
        DeliveryWarningDose = copy.DeliveryWarningDose;
        DoseReferenceDescription = copy.DoseReferenceDescription;
        DoseReferenceNumber = copy.DoseReferenceNumber;
        DoseReferencePointCoordinates = copy.DoseReferencePointCoordinates;
        DoseReferenceStructureType = copy.DoseReferenceStructureType;
        DoseReferenceType = copy.DoseReferenceType;
        DoseReferenceUID = copy.DoseReferenceUID;
        NominalPriorDose = copy.NominalPriorDose;
        OrganAtRiskFullVolumeDose = copy.OrganAtRiskFullVolumeDose;
        OrganAtRiskLimitDose = copy.OrganAtRiskLimitDose;
        OrganAtRiskMaximumDose = copy.OrganAtRiskMaximumDose;
        OrganAtRiskOverdoseVolumeFraction = copy.OrganAtRiskOverdoseVolumeFraction;
        ReferencedROINumber = copy.ReferencedROINumber;
        TargetMaximumDose = copy.TargetMaximumDose;
        TargetMinimumDose = copy.TargetMinimumDose;
        TargetPrescriptionDose = copy.TargetPrescriptionDose;
        TargetUnderdoseVolumeFraction = copy.TargetUnderdoseVolumeFraction;
    }
    return *this;
}


void DRTDoseReferenceSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        DoseReferenceNumber.clear();
        DoseReferenceUID.clear();
        DoseReferenceStructureType.clear();
        DoseReferenceDescription.clear();
        ReferencedROINumber.clear();
        DoseReferencePointCoordinates.clear();
        NominalPriorDose.clear();
        DoseReferenceType.clear();
        ConstraintWeight.clear();
        DeliveryWarningDose.clear();
        DeliveryMaximumDose.clear();
        TargetMinimumDose.clear();
        TargetPrescriptionDose.clear();
        TargetMaximumDose.clear();
        TargetUnderdoseVolumeFraction.clear();
        OrganAtRiskFullVolumeDose.clear();
        OrganAtRiskLimitDose.clear();
        OrganAtRiskMaximumDose.clear();
        OrganAtRiskOverdoseVolumeFraction.clear();
    }
}


OFBool DRTDoseReferenceSequence::Item::isEmpty()
{
    return DoseReferenceNumber.isEmpty() &&
           DoseReferenceUID.isEmpty() &&
           DoseReferenceStructureType.isEmpty() &&
           DoseReferenceDescription.isEmpty() &&
           ReferencedROINumber.isEmpty() &&
           DoseReferencePointCoordinates.isEmpty() &&
           NominalPriorDose.isEmpty() &&
           DoseReferenceType.isEmpty() &&
           ConstraintWeight.isEmpty() &&
           DeliveryWarningDose.isEmpty() &&
           DeliveryMaximumDose.isEmpty() &&
           TargetMinimumDose.isEmpty() &&
           TargetPrescriptionDose.isEmpty() &&
           TargetMaximumDose.isEmpty() &&
           TargetUnderdoseVolumeFraction.isEmpty() &&
           OrganAtRiskFullVolumeDose.isEmpty() &&
           OrganAtRiskLimitDose.isEmpty() &&
           OrganAtRiskMaximumDose.isEmpty() &&
           OrganAtRiskOverdoseVolumeFraction.isEmpty();
}


OFBool DRTDoseReferenceSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTDoseReferenceSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, DoseReferenceNumber, "1", "1", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, DoseReferenceUID, "1", "3", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, DoseReferenceStructureType, "1", "1", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, DoseReferenceDescription, "1", "3", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, ReferencedROINumber, "1", "1C", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, DoseReferencePointCoordinates, "3", "1C", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, NominalPriorDose, "1", "3", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, DoseReferenceType, "1", "1", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, ConstraintWeight, "1", "3", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, DeliveryWarningDose, "1", "3", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, DeliveryMaximumDose, "1", "3", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, TargetMinimumDose, "1", "3", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, TargetPrescriptionDose, "1", "3", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, TargetMaximumDose, "1", "3", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, TargetUnderdoseVolumeFraction, "1", "3", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, OrganAtRiskFullVolumeDose, "1", "3", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, OrganAtRiskLimitDose, "1", "3", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, OrganAtRiskMaximumDose, "1", "3", "DoseReferenceSequence");
        getAndCheckElementFromDataset(item, OrganAtRiskOverdoseVolumeFraction, "1", "3", "DoseReferenceSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(DoseReferenceNumber), "1", "1", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(DoseReferenceUID), "1", "3", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmCodeString(DoseReferenceStructureType), "1", "1", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmLongString(DoseReferenceDescription), "1", "3", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferencedROINumber), "1", "1C", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(DoseReferencePointCoordinates), "3", "1C", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(NominalPriorDose), "1", "3", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmCodeString(DoseReferenceType), "1", "1", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(ConstraintWeight), "1", "3", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(DeliveryWarningDose), "1", "3", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(DeliveryMaximumDose), "1", "3", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(TargetMinimumDose), "1", "3", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(TargetPrescriptionDose), "1", "3", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(TargetMaximumDose), "1", "3", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(TargetUnderdoseVolumeFraction), "1", "3", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(OrganAtRiskFullVolumeDose), "1", "3", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(OrganAtRiskLimitDose), "1", "3", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(OrganAtRiskMaximumDose), "1", "3", "DoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(OrganAtRiskOverdoseVolumeFraction), "1", "3", "DoseReferenceSequence");
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::getConstraintWeight(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ConstraintWeight, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getConstraintWeight(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ConstraintWeight).getFloat64(value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getDeliveryMaximumDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeliveryMaximumDose, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getDeliveryMaximumDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DeliveryMaximumDose).getFloat64(value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getDeliveryWarningDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeliveryWarningDose, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getDeliveryWarningDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DeliveryWarningDose).getFloat64(value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getDoseReferenceDescription(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DoseReferenceDescription, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getDoseReferenceNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DoseReferenceNumber, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getDoseReferenceNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, DoseReferenceNumber).getSint32(value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getDoseReferencePointCoordinates(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DoseReferencePointCoordinates, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getDoseReferencePointCoordinates(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DoseReferencePointCoordinates).getFloat64(value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getDoseReferencePointCoordinates(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DoseReferencePointCoordinates).getFloat64Vector(value);
}


OFCondition DRTDoseReferenceSequence::Item::getDoseReferenceStructureType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DoseReferenceStructureType, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getDoseReferenceType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DoseReferenceType, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getDoseReferenceUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DoseReferenceUID, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getNominalPriorDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NominalPriorDose, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getNominalPriorDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, NominalPriorDose).getFloat64(value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getOrganAtRiskFullVolumeDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(OrganAtRiskFullVolumeDose, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getOrganAtRiskFullVolumeDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, OrganAtRiskFullVolumeDose).getFloat64(value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getOrganAtRiskLimitDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(OrganAtRiskLimitDose, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getOrganAtRiskLimitDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, OrganAtRiskLimitDose).getFloat64(value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getOrganAtRiskMaximumDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(OrganAtRiskMaximumDose, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getOrganAtRiskMaximumDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, OrganAtRiskMaximumDose).getFloat64(value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getOrganAtRiskOverdoseVolumeFraction(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(OrganAtRiskOverdoseVolumeFraction, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getOrganAtRiskOverdoseVolumeFraction(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, OrganAtRiskOverdoseVolumeFraction).getFloat64(value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getReferencedROINumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedROINumber, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getReferencedROINumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedROINumber).getSint32(value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getTargetMaximumDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TargetMaximumDose, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getTargetMaximumDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TargetMaximumDose).getFloat64(value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getTargetMinimumDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TargetMinimumDose, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getTargetMinimumDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TargetMinimumDose).getFloat64(value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getTargetPrescriptionDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TargetPrescriptionDose, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getTargetPrescriptionDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TargetPrescriptionDose).getFloat64(value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getTargetUnderdoseVolumeFraction(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TargetUnderdoseVolumeFraction, value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::getTargetUnderdoseVolumeFraction(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TargetUnderdoseVolumeFraction).getFloat64(value, pos);
}


OFCondition DRTDoseReferenceSequence::Item::setConstraintWeight(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ConstraintWeight.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::setDeliveryMaximumDose(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DeliveryMaximumDose.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::setDeliveryWarningDose(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DeliveryWarningDose.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::setDoseReferenceDescription(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DoseReferenceDescription.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::setDoseReferenceNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DoseReferenceNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::setDoseReferencePointCoordinates(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "3") : EC_Normal;
        if (result.good())
            result = DoseReferencePointCoordinates.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::setDoseReferenceStructureType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DoseReferenceStructureType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::setDoseReferenceType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DoseReferenceType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::setDoseReferenceUID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DoseReferenceUID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::setNominalPriorDose(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NominalPriorDose.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::setOrganAtRiskFullVolumeDose(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = OrganAtRiskFullVolumeDose.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::setOrganAtRiskLimitDose(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = OrganAtRiskLimitDose.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::setOrganAtRiskMaximumDose(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = OrganAtRiskMaximumDose.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::setOrganAtRiskOverdoseVolumeFraction(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = OrganAtRiskOverdoseVolumeFraction.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::setReferencedROINumber(const OFString &value, const OFBool check)
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


OFCondition DRTDoseReferenceSequence::Item::setTargetMaximumDose(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TargetMaximumDose.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::setTargetMinimumDose(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TargetMinimumDose.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::setTargetPrescriptionDose(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TargetPrescriptionDose.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::Item::setTargetUnderdoseVolumeFraction(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TargetUnderdoseVolumeFraction.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTDoseReferenceSequence::DRTDoseReferenceSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTDoseReferenceSequence::DRTDoseReferenceSequence(const DRTDoseReferenceSequence &copy)
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


DRTDoseReferenceSequence &DRTDoseReferenceSequence::operator=(const DRTDoseReferenceSequence &copy)
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


DRTDoseReferenceSequence::~DRTDoseReferenceSequence()
{
    clear();
}


void DRTDoseReferenceSequence::clear()
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


OFBool DRTDoseReferenceSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTDoseReferenceSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTDoseReferenceSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTDoseReferenceSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTDoseReferenceSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTDoseReferenceSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTDoseReferenceSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTDoseReferenceSequence::Item &DRTDoseReferenceSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTDoseReferenceSequence::Item &DRTDoseReferenceSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTDoseReferenceSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTDoseReferenceSequence::Item &DRTDoseReferenceSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTDoseReferenceSequence::Item &DRTDoseReferenceSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTDoseReferenceSequence::Item &DRTDoseReferenceSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTDoseReferenceSequence::Item &DRTDoseReferenceSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTDoseReferenceSequence::addItem(Item *&item)
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


OFCondition DRTDoseReferenceSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTDoseReferenceSequence::removeItem(const unsigned long pos)
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


OFCondition DRTDoseReferenceSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_DoseReferenceSequence, sequence);
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
            DcmSequenceOfItems element(DCM_DoseReferenceSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTDoseReferenceSequence::write(DcmItem &dataset,
                                            const OFString &card,
                                            const OFString &type,
                                            const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_DoseReferenceSequence);
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
