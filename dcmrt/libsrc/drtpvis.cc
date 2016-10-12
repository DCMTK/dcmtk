/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTPlannedVerificationImageSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtpvis.h"


// --- item class ---

DRTPlannedVerificationImageSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    EndCumulativeMetersetWeight(DCM_EndCumulativeMetersetWeight),
    ImagingDeviceSpecificAcquisitionParameters(DCM_ImagingDeviceSpecificAcquisitionParameters),
    MetersetExposure(DCM_MetersetExposure),
    RTImageOrientation(DCM_RTImageOrientation),
    RTImagePlane(DCM_RTImagePlane),
    RTImagePosition(DCM_RTImagePosition),
    RTImageSID(DCM_RTImageSID),
    ReferencedReferenceImageNumber(DCM_ReferencedReferenceImageNumber),
    StartCumulativeMetersetWeight(DCM_StartCumulativeMetersetWeight),
    XRayImageReceptorAngle(DCM_XRayImageReceptorAngle)
{
}


DRTPlannedVerificationImageSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    EndCumulativeMetersetWeight(copy.EndCumulativeMetersetWeight),
    ImagingDeviceSpecificAcquisitionParameters(copy.ImagingDeviceSpecificAcquisitionParameters),
    MetersetExposure(copy.MetersetExposure),
    RTImageOrientation(copy.RTImageOrientation),
    RTImagePlane(copy.RTImagePlane),
    RTImagePosition(copy.RTImagePosition),
    RTImageSID(copy.RTImageSID),
    ReferencedReferenceImageNumber(copy.ReferencedReferenceImageNumber),
    StartCumulativeMetersetWeight(copy.StartCumulativeMetersetWeight),
    XRayImageReceptorAngle(copy.XRayImageReceptorAngle)
{
}


DRTPlannedVerificationImageSequence::Item::~Item()
{
}


DRTPlannedVerificationImageSequence::Item &DRTPlannedVerificationImageSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        EndCumulativeMetersetWeight = copy.EndCumulativeMetersetWeight;
        ImagingDeviceSpecificAcquisitionParameters = copy.ImagingDeviceSpecificAcquisitionParameters;
        MetersetExposure = copy.MetersetExposure;
        RTImageOrientation = copy.RTImageOrientation;
        RTImagePlane = copy.RTImagePlane;
        RTImagePosition = copy.RTImagePosition;
        RTImageSID = copy.RTImageSID;
        ReferencedReferenceImageNumber = copy.ReferencedReferenceImageNumber;
        StartCumulativeMetersetWeight = copy.StartCumulativeMetersetWeight;
        XRayImageReceptorAngle = copy.XRayImageReceptorAngle;
    }
    return *this;
}


void DRTPlannedVerificationImageSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        StartCumulativeMetersetWeight.clear();
        MetersetExposure.clear();
        EndCumulativeMetersetWeight.clear();
        RTImagePlane.clear();
        XRayImageReceptorAngle.clear();
        RTImageOrientation.clear();
        RTImagePosition.clear();
        RTImageSID.clear();
        ImagingDeviceSpecificAcquisitionParameters.clear();
        ReferencedReferenceImageNumber.clear();
    }
}


OFBool DRTPlannedVerificationImageSequence::Item::isEmpty()
{
    return StartCumulativeMetersetWeight.isEmpty() &&
           MetersetExposure.isEmpty() &&
           EndCumulativeMetersetWeight.isEmpty() &&
           RTImagePlane.isEmpty() &&
           XRayImageReceptorAngle.isEmpty() &&
           RTImageOrientation.isEmpty() &&
           RTImagePosition.isEmpty() &&
           RTImageSID.isEmpty() &&
           ImagingDeviceSpecificAcquisitionParameters.isEmpty() &&
           ReferencedReferenceImageNumber.isEmpty();
}


OFBool DRTPlannedVerificationImageSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTPlannedVerificationImageSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, StartCumulativeMetersetWeight, "1", "3", "PlannedVerificationImageSequence");
        getAndCheckElementFromDataset(item, MetersetExposure, "1", "3", "PlannedVerificationImageSequence");
        getAndCheckElementFromDataset(item, EndCumulativeMetersetWeight, "1", "3", "PlannedVerificationImageSequence");
        getAndCheckElementFromDataset(item, RTImagePlane, "1", "3", "PlannedVerificationImageSequence");
        getAndCheckElementFromDataset(item, XRayImageReceptorAngle, "1", "3", "PlannedVerificationImageSequence");
        getAndCheckElementFromDataset(item, RTImageOrientation, "6", "3", "PlannedVerificationImageSequence");
        getAndCheckElementFromDataset(item, RTImagePosition, "2", "3", "PlannedVerificationImageSequence");
        getAndCheckElementFromDataset(item, RTImageSID, "1", "3", "PlannedVerificationImageSequence");
        getAndCheckElementFromDataset(item, ImagingDeviceSpecificAcquisitionParameters, "1-n", "3", "PlannedVerificationImageSequence");
        getAndCheckElementFromDataset(item, ReferencedReferenceImageNumber, "1", "3", "PlannedVerificationImageSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTPlannedVerificationImageSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmDecimalString(StartCumulativeMetersetWeight), "1", "3", "PlannedVerificationImageSequence");
        addElementToDataset(result, item, new DcmDecimalString(MetersetExposure), "1", "3", "PlannedVerificationImageSequence");
        addElementToDataset(result, item, new DcmDecimalString(EndCumulativeMetersetWeight), "1", "3", "PlannedVerificationImageSequence");
        addElementToDataset(result, item, new DcmCodeString(RTImagePlane), "1", "3", "PlannedVerificationImageSequence");
        addElementToDataset(result, item, new DcmDecimalString(XRayImageReceptorAngle), "1", "3", "PlannedVerificationImageSequence");
        addElementToDataset(result, item, new DcmDecimalString(RTImageOrientation), "6", "3", "PlannedVerificationImageSequence");
        addElementToDataset(result, item, new DcmDecimalString(RTImagePosition), "2", "3", "PlannedVerificationImageSequence");
        addElementToDataset(result, item, new DcmDecimalString(RTImageSID), "1", "3", "PlannedVerificationImageSequence");
        addElementToDataset(result, item, new DcmLongString(ImagingDeviceSpecificAcquisitionParameters), "1-n", "3", "PlannedVerificationImageSequence");
        addElementToDataset(result, item, new DcmIntegerString(ReferencedReferenceImageNumber), "1", "3", "PlannedVerificationImageSequence");
    }
    return result;
}


OFCondition DRTPlannedVerificationImageSequence::Item::getEndCumulativeMetersetWeight(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(EndCumulativeMetersetWeight, value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getEndCumulativeMetersetWeight(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, EndCumulativeMetersetWeight).getFloat64(value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getImagingDeviceSpecificAcquisitionParameters(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ImagingDeviceSpecificAcquisitionParameters, value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getMetersetExposure(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MetersetExposure, value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getMetersetExposure(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, MetersetExposure).getFloat64(value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getRTImageOrientation(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RTImageOrientation, value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getRTImageOrientation(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, RTImageOrientation).getFloat64(value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getRTImageOrientation(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, RTImageOrientation).getFloat64Vector(value);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getRTImagePlane(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RTImagePlane, value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getRTImagePosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RTImagePosition, value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getRTImagePosition(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, RTImagePosition).getFloat64(value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getRTImagePosition(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, RTImagePosition).getFloat64Vector(value);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getRTImageSID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RTImageSID, value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getRTImageSID(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, RTImageSID).getFloat64(value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getReferencedReferenceImageNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedReferenceImageNumber, value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getReferencedReferenceImageNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, ReferencedReferenceImageNumber).getSint32(value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getStartCumulativeMetersetWeight(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(StartCumulativeMetersetWeight, value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getStartCumulativeMetersetWeight(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, StartCumulativeMetersetWeight).getFloat64(value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getXRayImageReceptorAngle(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(XRayImageReceptorAngle, value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::getXRayImageReceptorAngle(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, XRayImageReceptorAngle).getFloat64(value, pos);
}


OFCondition DRTPlannedVerificationImageSequence::Item::setEndCumulativeMetersetWeight(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = EndCumulativeMetersetWeight.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPlannedVerificationImageSequence::Item::setImagingDeviceSpecificAcquisitionParameters(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
        if (result.good())
            result = ImagingDeviceSpecificAcquisitionParameters.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPlannedVerificationImageSequence::Item::setMetersetExposure(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = MetersetExposure.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPlannedVerificationImageSequence::Item::setRTImageOrientation(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "6") : EC_Normal;
        if (result.good())
            result = RTImageOrientation.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPlannedVerificationImageSequence::Item::setRTImagePlane(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RTImagePlane.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPlannedVerificationImageSequence::Item::setRTImagePosition(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "2") : EC_Normal;
        if (result.good())
            result = RTImagePosition.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPlannedVerificationImageSequence::Item::setRTImageSID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RTImageSID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPlannedVerificationImageSequence::Item::setReferencedReferenceImageNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedReferenceImageNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPlannedVerificationImageSequence::Item::setStartCumulativeMetersetWeight(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = StartCumulativeMetersetWeight.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPlannedVerificationImageSequence::Item::setXRayImageReceptorAngle(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = XRayImageReceptorAngle.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTPlannedVerificationImageSequence::DRTPlannedVerificationImageSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTPlannedVerificationImageSequence::DRTPlannedVerificationImageSequence(const DRTPlannedVerificationImageSequence &copy)
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


DRTPlannedVerificationImageSequence &DRTPlannedVerificationImageSequence::operator=(const DRTPlannedVerificationImageSequence &copy)
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


DRTPlannedVerificationImageSequence::~DRTPlannedVerificationImageSequence()
{
    clear();
}


void DRTPlannedVerificationImageSequence::clear()
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


OFBool DRTPlannedVerificationImageSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTPlannedVerificationImageSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTPlannedVerificationImageSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTPlannedVerificationImageSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTPlannedVerificationImageSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTPlannedVerificationImageSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTPlannedVerificationImageSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTPlannedVerificationImageSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTPlannedVerificationImageSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTPlannedVerificationImageSequence::Item &DRTPlannedVerificationImageSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTPlannedVerificationImageSequence::Item &DRTPlannedVerificationImageSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTPlannedVerificationImageSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTPlannedVerificationImageSequence::Item &DRTPlannedVerificationImageSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTPlannedVerificationImageSequence::Item &DRTPlannedVerificationImageSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTPlannedVerificationImageSequence::Item &DRTPlannedVerificationImageSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTPlannedVerificationImageSequence::Item &DRTPlannedVerificationImageSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTPlannedVerificationImageSequence::addItem(Item *&item)
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


OFCondition DRTPlannedVerificationImageSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTPlannedVerificationImageSequence::removeItem(const unsigned long pos)
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


OFCondition DRTPlannedVerificationImageSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_PlannedVerificationImageSequence, sequence);
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
            DcmSequenceOfItems element(DCM_PlannedVerificationImageSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTPlannedVerificationImageSequence::write(DcmItem &dataset,
                                                       const OFString &card,
                                                       const OFString &type,
                                                       const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_PlannedVerificationImageSequence);
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
