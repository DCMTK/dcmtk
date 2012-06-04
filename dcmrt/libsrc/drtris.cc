/*
 *
 *  Copyright (c) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTReferencedInstanceSequence
 *
 *  Generated automatically from DICOM PS 3.3-2007
 *  File created on 2012-02-01 16:51:15 
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtris.h"


// --- item class ---

DRTReferencedInstanceSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    BitsAllocated(DCM_BitsAllocated),
    BitsStored(DCM_BitsStored),
    BluePaletteColorLookupTableData(DCM_BluePaletteColorLookupTableData),
    BluePaletteColorLookupTableDescriptor(DCM_BluePaletteColorLookupTableDescriptor),
    Columns(DCM_Columns),
    GreenPaletteColorLookupTableData(DCM_GreenPaletteColorLookupTableData),
    GreenPaletteColorLookupTableDescriptor(DCM_GreenPaletteColorLookupTableDescriptor),
    HighBit(DCM_HighBit),
    ICCProfile(DCM_ICCProfile),
    LargestImagePixelValue(DCM_LargestImagePixelValue),
    PhotometricInterpretation(DCM_PhotometricInterpretation),
    PixelAspectRatio(DCM_PixelAspectRatio),
    PixelData(DCM_PixelData),
    PixelRepresentation(DCM_PixelRepresentation),
    PlanarConfiguration(DCM_PlanarConfiguration),
    PurposeOfReferenceCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RedPaletteColorLookupTableData(DCM_RedPaletteColorLookupTableData),
    RedPaletteColorLookupTableDescriptor(DCM_RedPaletteColorLookupTableDescriptor),
    ReferencedSOPClassUID(DCM_ReferencedSOPClassUID),
    ReferencedSOPInstanceUID(DCM_ReferencedSOPInstanceUID),
    Rows(DCM_Rows),
    SamplesPerPixel(DCM_SamplesPerPixel),
    SmallestImagePixelValue(DCM_SmallestImagePixelValue)
{
}


DRTReferencedInstanceSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    BitsAllocated(copy.BitsAllocated),
    BitsStored(copy.BitsStored),
    BluePaletteColorLookupTableData(copy.BluePaletteColorLookupTableData),
    BluePaletteColorLookupTableDescriptor(copy.BluePaletteColorLookupTableDescriptor),
    Columns(copy.Columns),
    GreenPaletteColorLookupTableData(copy.GreenPaletteColorLookupTableData),
    GreenPaletteColorLookupTableDescriptor(copy.GreenPaletteColorLookupTableDescriptor),
    HighBit(copy.HighBit),
    ICCProfile(copy.ICCProfile),
    LargestImagePixelValue(copy.LargestImagePixelValue),
    PhotometricInterpretation(copy.PhotometricInterpretation),
    PixelAspectRatio(copy.PixelAspectRatio),
    PixelData(copy.PixelData),
    PixelRepresentation(copy.PixelRepresentation),
    PlanarConfiguration(copy.PlanarConfiguration),
    PurposeOfReferenceCodeSequence(copy.PurposeOfReferenceCodeSequence),
    RedPaletteColorLookupTableData(copy.RedPaletteColorLookupTableData),
    RedPaletteColorLookupTableDescriptor(copy.RedPaletteColorLookupTableDescriptor),
    ReferencedSOPClassUID(copy.ReferencedSOPClassUID),
    ReferencedSOPInstanceUID(copy.ReferencedSOPInstanceUID),
    Rows(copy.Rows),
    SamplesPerPixel(copy.SamplesPerPixel),
    SmallestImagePixelValue(copy.SmallestImagePixelValue)
{
}


DRTReferencedInstanceSequence::Item::~Item()
{
}


DRTReferencedInstanceSequence::Item &DRTReferencedInstanceSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        BitsAllocated = copy.BitsAllocated;
        BitsStored = copy.BitsStored;
        BluePaletteColorLookupTableData = copy.BluePaletteColorLookupTableData;
        BluePaletteColorLookupTableDescriptor = copy.BluePaletteColorLookupTableDescriptor;
        Columns = copy.Columns;
        GreenPaletteColorLookupTableData = copy.GreenPaletteColorLookupTableData;
        GreenPaletteColorLookupTableDescriptor = copy.GreenPaletteColorLookupTableDescriptor;
        HighBit = copy.HighBit;
        ICCProfile = copy.ICCProfile;
        LargestImagePixelValue = copy.LargestImagePixelValue;
        PhotometricInterpretation = copy.PhotometricInterpretation;
        PixelAspectRatio = copy.PixelAspectRatio;
        PixelData = copy.PixelData;
        PixelRepresentation = copy.PixelRepresentation;
        PlanarConfiguration = copy.PlanarConfiguration;
        PurposeOfReferenceCodeSequence = copy.PurposeOfReferenceCodeSequence;
        RedPaletteColorLookupTableData = copy.RedPaletteColorLookupTableData;
        RedPaletteColorLookupTableDescriptor = copy.RedPaletteColorLookupTableDescriptor;
        ReferencedSOPClassUID = copy.ReferencedSOPClassUID;
        ReferencedSOPInstanceUID = copy.ReferencedSOPInstanceUID;
        Rows = copy.Rows;
        SamplesPerPixel = copy.SamplesPerPixel;
        SmallestImagePixelValue = copy.SmallestImagePixelValue;
    }
    return *this;
}


void DRTReferencedInstanceSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        ReferencedSOPClassUID.clear();
        ReferencedSOPInstanceUID.clear();
        PurposeOfReferenceCodeSequence.clear();
        SamplesPerPixel.clear();
        PhotometricInterpretation.clear();
        Rows.clear();
        Columns.clear();
        BitsAllocated.clear();
        BitsStored.clear();
        HighBit.clear();
        PixelRepresentation.clear();
        PixelData.clear();
        PlanarConfiguration.clear();
        PixelAspectRatio.clear();
        SmallestImagePixelValue.clear();
        LargestImagePixelValue.clear();
        RedPaletteColorLookupTableDescriptor.clear();
        GreenPaletteColorLookupTableDescriptor.clear();
        BluePaletteColorLookupTableDescriptor.clear();
        RedPaletteColorLookupTableData.clear();
        GreenPaletteColorLookupTableData.clear();
        BluePaletteColorLookupTableData.clear();
        ICCProfile.clear();
    }
}


OFBool DRTReferencedInstanceSequence::Item::isEmpty()
{
    return ReferencedSOPClassUID.isEmpty() &&
           ReferencedSOPInstanceUID.isEmpty() &&
           PurposeOfReferenceCodeSequence.isEmpty() &&
           SamplesPerPixel.isEmpty() &&
           PhotometricInterpretation.isEmpty() &&
           Rows.isEmpty() &&
           Columns.isEmpty() &&
           BitsAllocated.isEmpty() &&
           BitsStored.isEmpty() &&
           HighBit.isEmpty() &&
           PixelRepresentation.isEmpty() &&
           PixelData.isEmpty() &&
           PlanarConfiguration.isEmpty() &&
           PixelAspectRatio.isEmpty() &&
           SmallestImagePixelValue.isEmpty() &&
           LargestImagePixelValue.isEmpty() &&
           RedPaletteColorLookupTableDescriptor.isEmpty() &&
           GreenPaletteColorLookupTableDescriptor.isEmpty() &&
           BluePaletteColorLookupTableDescriptor.isEmpty() &&
           RedPaletteColorLookupTableData.isEmpty() &&
           GreenPaletteColorLookupTableData.isEmpty() &&
           BluePaletteColorLookupTableData.isEmpty() &&
           ICCProfile.isEmpty();
}


OFBool DRTReferencedInstanceSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTReferencedInstanceSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, ReferencedSOPClassUID, "1", "1", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, ReferencedSOPInstanceUID, "1", "1", "ReferencedInstanceSequence");
        PurposeOfReferenceCodeSequence.read(item, "1-n", "1", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, SamplesPerPixel, "1", "1", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, PhotometricInterpretation, "1", "1", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, Rows, "1", "1", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, Columns, "1", "1", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, BitsAllocated, "1", "1", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, BitsStored, "1", "1", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, HighBit, "1", "1", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, PixelRepresentation, "1", "1", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, PixelData, "1", "1C", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, PlanarConfiguration, "1", "1C", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, PixelAspectRatio, "2", "1C", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, SmallestImagePixelValue, "1", "3", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, LargestImagePixelValue, "1", "3", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, RedPaletteColorLookupTableDescriptor, "3", "1C", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, GreenPaletteColorLookupTableDescriptor, "3", "1C", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, BluePaletteColorLookupTableDescriptor, "3", "1C", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, RedPaletteColorLookupTableData, "1", "1C", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, GreenPaletteColorLookupTableData, "1", "1C", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, BluePaletteColorLookupTableData, "1", "1C", "ReferencedInstanceSequence");
        getAndCheckElementFromDataset(item, ICCProfile, "1", "3", "ReferencedInstanceSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedInstanceSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmUniqueIdentifier(ReferencedSOPClassUID), "1", "1", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(ReferencedSOPInstanceUID), "1", "1", "ReferencedInstanceSequence");
        if (result.good()) result = PurposeOfReferenceCodeSequence.write(item, "1-n", "1", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(SamplesPerPixel), "1", "1", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmCodeString(PhotometricInterpretation), "1", "1", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(Rows), "1", "1", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(Columns), "1", "1", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(BitsAllocated), "1", "1", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(BitsStored), "1", "1", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(HighBit), "1", "1", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(PixelRepresentation), "1", "1", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmPixelData(PixelData), "1", "1C", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(PlanarConfiguration), "1", "1C", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmIntegerString(PixelAspectRatio), "2", "1C", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(SmallestImagePixelValue), "1", "3", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(LargestImagePixelValue), "1", "3", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(RedPaletteColorLookupTableDescriptor), "3", "1C", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(GreenPaletteColorLookupTableDescriptor), "3", "1C", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(BluePaletteColorLookupTableDescriptor), "3", "1C", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmOtherByteOtherWord(RedPaletteColorLookupTableData), "1", "1C", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmOtherByteOtherWord(GreenPaletteColorLookupTableData), "1", "1C", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmOtherByteOtherWord(BluePaletteColorLookupTableData), "1", "1C", "ReferencedInstanceSequence");
        addElementToDataset(result, item, new DcmOtherByteOtherWord(ICCProfile), "1", "3", "ReferencedInstanceSequence");
    }
    return result;
}


OFCondition DRTReferencedInstanceSequence::Item::getBitsAllocated(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, BitsAllocated).getUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::getBitsStored(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, BitsStored).getUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::getBluePaletteColorLookupTableData(Uint16 *&value, unsigned long *count) const
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = OFconst_cast(DcmOtherByteOtherWord &, BluePaletteColorLookupTableData).getUint16Array(value);
        /* set optional count parameter */
        if (count != NULL)
        {
            if (result.good())
                *count = OFconst_cast(DcmOtherByteOtherWord &, BluePaletteColorLookupTableData).getLength() / sizeof(Uint16);
            else
                *count = 0;
        }
    }
    return result;
}


OFCondition DRTReferencedInstanceSequence::Item::getBluePaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, BluePaletteColorLookupTableDescriptor).getUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::getColumns(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, Columns).getUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::getGreenPaletteColorLookupTableData(Uint16 *&value, unsigned long *count) const
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = OFconst_cast(DcmOtherByteOtherWord &, GreenPaletteColorLookupTableData).getUint16Array(value);
        /* set optional count parameter */
        if (count != NULL)
        {
            if (result.good())
                *count = OFconst_cast(DcmOtherByteOtherWord &, GreenPaletteColorLookupTableData).getLength() / sizeof(Uint16);
            else
                *count = 0;
        }
    }
    return result;
}


OFCondition DRTReferencedInstanceSequence::Item::getGreenPaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, GreenPaletteColorLookupTableDescriptor).getUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::getHighBit(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, HighBit).getUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::getICCProfile(Uint8 *&value, unsigned long *count) const
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = OFconst_cast(DcmOtherByteOtherWord &, ICCProfile).getUint8Array(value);
        /* set optional count parameter */
        if (count != NULL)
        {
            if (result.good())
                *count = OFconst_cast(DcmOtherByteOtherWord &, ICCProfile).getLength() / sizeof(Uint8);
            else
                *count = 0;
        }
    }
    return result;
}


OFCondition DRTReferencedInstanceSequence::Item::getLargestImagePixelValue(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, LargestImagePixelValue).getUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::getPhotometricInterpretation(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PhotometricInterpretation, value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::getPixelAspectRatio(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PixelAspectRatio, value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::getPixelAspectRatio(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, PixelAspectRatio).getSint32(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::getPixelRepresentation(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, PixelRepresentation).getUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::getPlanarConfiguration(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, PlanarConfiguration).getUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::getRedPaletteColorLookupTableData(Uint16 *&value, unsigned long *count) const
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = OFconst_cast(DcmOtherByteOtherWord &, RedPaletteColorLookupTableData).getUint16Array(value);
        /* set optional count parameter */
        if (count != NULL)
        {
            if (result.good())
                *count = OFconst_cast(DcmOtherByteOtherWord &, RedPaletteColorLookupTableData).getLength() / sizeof(Uint16);
            else
                *count = 0;
        }
    }
    return result;
}


OFCondition DRTReferencedInstanceSequence::Item::getRedPaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, RedPaletteColorLookupTableDescriptor).getUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::getReferencedSOPClassUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedSOPClassUID, value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::getReferencedSOPInstanceUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ReferencedSOPInstanceUID, value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::getRows(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, Rows).getUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::getSamplesPerPixel(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, SamplesPerPixel).getUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::getSmallestImagePixelValue(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, SmallestImagePixelValue).getUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::setBitsAllocated(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return BitsAllocated.putUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::setBitsStored(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return BitsStored.putUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::setBluePaletteColorLookupTableData(const Uint16 *value, const unsigned long count)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return BluePaletteColorLookupTableData.putUint16Array(value, count);
}


OFCondition DRTReferencedInstanceSequence::Item::setBluePaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return BluePaletteColorLookupTableDescriptor.putUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::setColumns(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return Columns.putUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::setGreenPaletteColorLookupTableData(const Uint16 *value, const unsigned long count)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return GreenPaletteColorLookupTableData.putUint16Array(value, count);
}


OFCondition DRTReferencedInstanceSequence::Item::setGreenPaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return GreenPaletteColorLookupTableDescriptor.putUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::setHighBit(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return HighBit.putUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::setICCProfile(const Uint8 *value, const unsigned long count)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ICCProfile.putUint8Array(value, count);
}


OFCondition DRTReferencedInstanceSequence::Item::setLargestImagePixelValue(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return LargestImagePixelValue.putUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::setPhotometricInterpretation(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = PhotometricInterpretation.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedInstanceSequence::Item::setPixelAspectRatio(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "2") : EC_Normal;
        if (result.good())
            result = PixelAspectRatio.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedInstanceSequence::Item::setPixelRepresentation(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return PixelRepresentation.putUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::setPlanarConfiguration(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return PlanarConfiguration.putUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::setRedPaletteColorLookupTableData(const Uint16 *value, const unsigned long count)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RedPaletteColorLookupTableData.putUint16Array(value, count);
}


OFCondition DRTReferencedInstanceSequence::Item::setRedPaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RedPaletteColorLookupTableDescriptor.putUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::setReferencedSOPClassUID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedSOPClassUID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedInstanceSequence::Item::setReferencedSOPInstanceUID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ReferencedSOPInstanceUID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTReferencedInstanceSequence::Item::setRows(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return Rows.putUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::setSamplesPerPixel(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return SamplesPerPixel.putUint16(value, pos);
}


OFCondition DRTReferencedInstanceSequence::Item::setSmallestImagePixelValue(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return SmallestImagePixelValue.putUint16(value, pos);
}


// --- sequence class ---

DRTReferencedInstanceSequence::DRTReferencedInstanceSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTReferencedInstanceSequence::DRTReferencedInstanceSequence(const DRTReferencedInstanceSequence &copy)
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


DRTReferencedInstanceSequence &DRTReferencedInstanceSequence::operator=(const DRTReferencedInstanceSequence &copy)
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


DRTReferencedInstanceSequence::~DRTReferencedInstanceSequence()
{
    clear();
}


void DRTReferencedInstanceSequence::clear()
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


OFBool DRTReferencedInstanceSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTReferencedInstanceSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTReferencedInstanceSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTReferencedInstanceSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedInstanceSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTReferencedInstanceSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTReferencedInstanceSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTReferencedInstanceSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTReferencedInstanceSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTReferencedInstanceSequence::Item &DRTReferencedInstanceSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTReferencedInstanceSequence::Item &DRTReferencedInstanceSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTReferencedInstanceSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTReferencedInstanceSequence::Item &DRTReferencedInstanceSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTReferencedInstanceSequence::Item &DRTReferencedInstanceSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTReferencedInstanceSequence::Item &DRTReferencedInstanceSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTReferencedInstanceSequence::Item &DRTReferencedInstanceSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTReferencedInstanceSequence::addItem(Item *&item)
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


OFCondition DRTReferencedInstanceSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTReferencedInstanceSequence::removeItem(const unsigned long pos)
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


OFCondition DRTReferencedInstanceSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_ReferencedInstanceSequence, sequence);
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
            DcmSequenceOfItems element(DCM_ReferencedInstanceSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTReferencedInstanceSequence::write(DcmItem &dataset,
                                                 const OFString &card,
                                                 const OFString &type,
                                                 const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ReferencedInstanceSequence);
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
