/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtrdrs8.h"


// --- item class ---

DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    ConstraintWeight(DCM_ConstraintWeight),
    DeliveryMaximumDose(DCM_DeliveryMaximumDose),
    DeliveryWarningDose(DCM_DeliveryWarningDose),
    OrganAtRiskFullVolumeDose(DCM_OrganAtRiskFullVolumeDose),
    OrganAtRiskLimitDose(DCM_OrganAtRiskLimitDose),
    OrganAtRiskMaximumDose(DCM_OrganAtRiskMaximumDose),
    OrganAtRiskOverdoseVolumeFraction(DCM_OrganAtRiskOverdoseVolumeFraction),
    ReferencedDoseReferenceNumber(DCM_ReferencedDoseReferenceNumber),
    TargetMaximumDose(DCM_TargetMaximumDose),
    TargetMinimumDose(DCM_TargetMinimumDose),
    TargetPrescriptionDose(DCM_TargetPrescriptionDose),
    TargetUnderdoseVolumeFraction(DCM_TargetUnderdoseVolumeFraction)
{
}


DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    ConstraintWeight(copy.ConstraintWeight),
    DeliveryMaximumDose(copy.DeliveryMaximumDose),
    DeliveryWarningDose(copy.DeliveryWarningDose),
    OrganAtRiskFullVolumeDose(copy.OrganAtRiskFullVolumeDose),
    OrganAtRiskLimitDose(copy.OrganAtRiskLimitDose),
    OrganAtRiskMaximumDose(copy.OrganAtRiskMaximumDose),
    OrganAtRiskOverdoseVolumeFraction(copy.OrganAtRiskOverdoseVolumeFraction),
    ReferencedDoseReferenceNumber(copy.ReferencedDoseReferenceNumber),
    TargetMaximumDose(copy.TargetMaximumDose),
    TargetMinimumDose(copy.TargetMinimumDose),
    TargetPrescriptionDose(copy.TargetPrescriptionDose),
    TargetUnderdoseVolumeFraction(copy.TargetUnderdoseVolumeFraction)
{
}


DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::~Item()
{
}


DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item &DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        ConstraintWeight = copy.ConstraintWeight;
        DeliveryMaximumDose = copy.DeliveryMaximumDose;
        DeliveryWarningDose = copy.DeliveryWarningDose;
        OrganAtRiskFullVolumeDose = copy.OrganAtRiskFullVolumeDose;
        OrganAtRiskLimitDose = copy.OrganAtRiskLimitDose;
        OrganAtRiskMaximumDose = copy.OrganAtRiskMaximumDose;
        OrganAtRiskOverdoseVolumeFraction = copy.OrganAtRiskOverdoseVolumeFraction;
        ReferencedDoseReferenceNumber = copy.ReferencedDoseReferenceNumber;
        TargetMaximumDose = copy.TargetMaximumDose;
        TargetMinimumDose = copy.TargetMinimumDose;
        TargetPrescriptionDose = copy.TargetPrescriptionDose;
        TargetUnderdoseVolumeFraction = copy.TargetUnderdoseVolumeFraction;
    }
    return *this;
}


void DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ReferencedDoseReferenceNumber.clear();
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


OFBool DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::isEmpty()
{
    return ReferencedDoseReferenceNumber.isEmpty() &&
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


OFBool DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ReferencedDoseReferenceNumber, "1", "1", "ReferencedDoseReferenceSequence");
        getAndCheckElementFromDataset(item, ConstraintWeight, "1", "3", "ReferencedDoseReferenceSequence");
        getAndCheckElementFromDataset(item, DeliveryWarningDose, "1", "3", "ReferencedDoseReferenceSequence");
        getAndCheckElementFromDataset(item, DeliveryMaximumDose, "1", "3", "ReferencedDoseReferenceSequence");
        getAndCheckElementFromDataset(item, TargetMinimumDose, "1", "3", "ReferencedDoseReferenceSequence");
        getAndCheckElementFromDataset(item, TargetPrescriptionDose, "1", "3", "ReferencedDoseReferenceSequence");
        getAndCheckElementFromDataset(item, TargetMaximumDose, "1", "3", "ReferencedDoseReferenceSequence");
        getAndCheckElementFromDataset(item, TargetUnderdoseVolumeFraction, "1", "3", "ReferencedDoseReferenceSequence");
        getAndCheckElementFromDataset(item, OrganAtRiskFullVolumeDose, "1", "3", "ReferencedDoseReferenceSequence");
        getAndCheckElementFromDataset(item, OrganAtRiskLimitDose, "1", "3", "ReferencedDoseReferenceSequence");
        getAndCheckElementFromDataset(item, OrganAtRiskMaximumDose, "1", "3", "ReferencedDoseReferenceSequence");
        getAndCheckElementFromDataset(item, OrganAtRiskOverdoseVolumeFraction, "1", "3", "ReferencedDoseReferenceSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmIntegerString(ReferencedDoseReferenceNumber), "1", "1", "ReferencedDoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(ConstraintWeight), "1", "3", "ReferencedDoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(DeliveryWarningDose), "1", "3", "ReferencedDoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(DeliveryMaximumDose), "1", "3", "ReferencedDoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(TargetMinimumDose), "1", "3", "ReferencedDoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(TargetPrescriptionDose), "1", "3", "ReferencedDoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(TargetMaximumDose), "1", "3", "ReferencedDoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(TargetUnderdoseVolumeFraction), "1", "3", "ReferencedDoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(OrganAtRiskFullVolumeDose), "1", "3", "ReferencedDoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(OrganAtRiskLimitDose), "1", "3", "ReferencedDoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(OrganAtRiskMaximumDose), "1", "3", "ReferencedDoseReferenceSequence");
        addElementToDataset(result, item, new DcmDecimalString(OrganAtRiskOverdoseVolumeFraction), "1", "3", "ReferencedDoseReferenceSequence");
    }
    return result;
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getConstraintWeight(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ConstraintWeight, value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getConstraintWeight(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, ConstraintWeight).getFloat64(value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getDeliveryMaximumDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeliveryMaximumDose, value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getDeliveryMaximumDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DeliveryMaximumDose).getFloat64(value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getDeliveryWarningDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DeliveryWarningDose, value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getDeliveryWarningDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, DeliveryWarningDose).getFloat64(value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getOrganAtRiskFullVolumeDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(OrganAtRiskFullVolumeDose, value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getOrganAtRiskFullVolumeDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, OrganAtRiskFullVolumeDose).getFloat64(value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getOrganAtRiskLimitDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(OrganAtRiskLimitDose, value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getOrganAtRiskLimitDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, OrganAtRiskLimitDose).getFloat64(value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getOrganAtRiskMaximumDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(OrganAtRiskMaximumDose, value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getOrganAtRiskMaximumDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, OrganAtRiskMaximumDose).getFloat64(value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getOrganAtRiskOverdoseVolumeFraction(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(OrganAtRiskOverdoseVolumeFraction, value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getOrganAtRiskOverdoseVolumeFraction(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, OrganAtRiskOverdoseVolumeFraction).getFloat64(value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getReferencedDoseReferenceNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedDoseReferenceNumber, value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getReferencedDoseReferenceNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedDoseReferenceNumber).getSint32(value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getTargetMaximumDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TargetMaximumDose, value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getTargetMaximumDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TargetMaximumDose).getFloat64(value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getTargetMinimumDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TargetMinimumDose, value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getTargetMinimumDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TargetMinimumDose).getFloat64(value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getTargetPrescriptionDose(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TargetPrescriptionDose, value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getTargetPrescriptionDose(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TargetPrescriptionDose).getFloat64(value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getTargetUnderdoseVolumeFraction(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TargetUnderdoseVolumeFraction, value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::getTargetUnderdoseVolumeFraction(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, TargetUnderdoseVolumeFraction).getFloat64(value, pos);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::setConstraintWeight(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::setDeliveryMaximumDose(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::setDeliveryWarningDose(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::setOrganAtRiskFullVolumeDose(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::setOrganAtRiskLimitDose(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::setOrganAtRiskMaximumDose(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::setOrganAtRiskOverdoseVolumeFraction(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::setReferencedDoseReferenceNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedDoseReferenceNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::setTargetMaximumDose(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::setTargetMinimumDose(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::setTargetPrescriptionDose(const OFString &value, const OFBool check)
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


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item::setTargetUnderdoseVolumeFraction(const OFString &value, const OFBool check)
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

DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule(const DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule &copy)
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


DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule &DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::operator=(const DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule &copy)
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


DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::~DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule()
{
    clear();
}


void DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::clear()
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


OFBool DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item &DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item &DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item &DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item &DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item &DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::operator[](const size_t num)
{
    return getItem(num);
}


const DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::Item &DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::addItem(Item *&item)
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


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::removeItem(const size_t pos)
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


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ReferencedDoseReferenceSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ReferencedDoseReferenceSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTReferencedDoseReferenceSequenceInRTFractionSchemeModule::write(DcmItem &dataset,
                                                                              const OFString &card,
                                                                              const OFString &type,
                                                                              const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ReferencedDoseReferenceSequence);
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
