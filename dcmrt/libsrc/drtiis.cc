/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTIconImageSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtiis.h"


// --- item class ---

DRTIconImageSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    BitsAllocated(DCM_BitsAllocated),
    BitsStored(DCM_BitsStored),
    BluePaletteColorLookupTableData(DCM_BluePaletteColorLookupTableData),
    BluePaletteColorLookupTableDescriptor(DCM_BluePaletteColorLookupTableDescriptor),
    ColorSpace(DCM_ColorSpace),
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
    RedPaletteColorLookupTableData(DCM_RedPaletteColorLookupTableData),
    RedPaletteColorLookupTableDescriptor(DCM_RedPaletteColorLookupTableDescriptor),
    Rows(DCM_Rows),
    SamplesPerPixel(DCM_SamplesPerPixel),
    SmallestImagePixelValue(DCM_SmallestImagePixelValue)
{
}


DRTIconImageSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    BitsAllocated(copy.BitsAllocated),
    BitsStored(copy.BitsStored),
    BluePaletteColorLookupTableData(copy.BluePaletteColorLookupTableData),
    BluePaletteColorLookupTableDescriptor(copy.BluePaletteColorLookupTableDescriptor),
    ColorSpace(copy.ColorSpace),
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
    RedPaletteColorLookupTableData(copy.RedPaletteColorLookupTableData),
    RedPaletteColorLookupTableDescriptor(copy.RedPaletteColorLookupTableDescriptor),
    Rows(copy.Rows),
    SamplesPerPixel(copy.SamplesPerPixel),
    SmallestImagePixelValue(copy.SmallestImagePixelValue)
{
}


DRTIconImageSequence::Item::~Item()
{
}


DRTIconImageSequence::Item &DRTIconImageSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        BitsAllocated = copy.BitsAllocated;
        BitsStored = copy.BitsStored;
        BluePaletteColorLookupTableData = copy.BluePaletteColorLookupTableData;
        BluePaletteColorLookupTableDescriptor = copy.BluePaletteColorLookupTableDescriptor;
        ColorSpace = copy.ColorSpace;
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
        RedPaletteColorLookupTableData = copy.RedPaletteColorLookupTableData;
        RedPaletteColorLookupTableDescriptor = copy.RedPaletteColorLookupTableDescriptor;
        Rows = copy.Rows;
        SamplesPerPixel = copy.SamplesPerPixel;
        SmallestImagePixelValue = copy.SmallestImagePixelValue;
    }
    return *this;
}


void DRTIconImageSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        SamplesPerPixel.clear();
        PhotometricInterpretation.clear();
        Rows.clear();
        Columns.clear();
        BitsAllocated.clear();
        BitsStored.clear();
        HighBit.clear();
        PixelRepresentation.clear();
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
        ColorSpace.clear();
        PixelData.clear();
    }
}


OFBool DRTIconImageSequence::Item::isEmpty()
{
    return SamplesPerPixel.isEmpty() &&
           PhotometricInterpretation.isEmpty() &&
           Rows.isEmpty() &&
           Columns.isEmpty() &&
           BitsAllocated.isEmpty() &&
           BitsStored.isEmpty() &&
           HighBit.isEmpty() &&
           PixelRepresentation.isEmpty() &&
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
           ICCProfile.isEmpty() &&
           ColorSpace.isEmpty() &&
           PixelData.isEmpty();
}


OFBool DRTIconImageSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTIconImageSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, SamplesPerPixel, "1", "1", "IconImageSequence");
        getAndCheckElementFromDataset(item, PhotometricInterpretation, "1", "1", "IconImageSequence");
        getAndCheckElementFromDataset(item, Rows, "1", "1", "IconImageSequence");
        getAndCheckElementFromDataset(item, Columns, "1", "1", "IconImageSequence");
        getAndCheckElementFromDataset(item, BitsAllocated, "1", "1", "IconImageSequence");
        getAndCheckElementFromDataset(item, BitsStored, "1", "1", "IconImageSequence");
        getAndCheckElementFromDataset(item, HighBit, "1", "1", "IconImageSequence");
        getAndCheckElementFromDataset(item, PixelRepresentation, "1", "1", "IconImageSequence");
        getAndCheckElementFromDataset(item, PlanarConfiguration, "1", "1C", "IconImageSequence");
        getAndCheckElementFromDataset(item, PixelAspectRatio, "2", "1C", "IconImageSequence");
        getAndCheckElementFromDataset(item, SmallestImagePixelValue, "1", "3", "IconImageSequence");
        getAndCheckElementFromDataset(item, LargestImagePixelValue, "1", "3", "IconImageSequence");
        getAndCheckElementFromDataset(item, RedPaletteColorLookupTableDescriptor, "3", "1C", "IconImageSequence");
        getAndCheckElementFromDataset(item, GreenPaletteColorLookupTableDescriptor, "3", "1C", "IconImageSequence");
        getAndCheckElementFromDataset(item, BluePaletteColorLookupTableDescriptor, "3", "1C", "IconImageSequence");
        getAndCheckElementFromDataset(item, RedPaletteColorLookupTableData, "1", "1C", "IconImageSequence");
        getAndCheckElementFromDataset(item, GreenPaletteColorLookupTableData, "1", "1C", "IconImageSequence");
        getAndCheckElementFromDataset(item, BluePaletteColorLookupTableData, "1", "1C", "IconImageSequence");
        getAndCheckElementFromDataset(item, ICCProfile, "1", "3", "IconImageSequence");
        getAndCheckElementFromDataset(item, ColorSpace, "1", "3", "IconImageSequence");
        getAndCheckElementFromDataset(item, PixelData, "1", "1", "IconImageSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIconImageSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmUnsignedShort(SamplesPerPixel), "1", "1", "IconImageSequence");
        addElementToDataset(result, item, new DcmCodeString(PhotometricInterpretation), "1", "1", "IconImageSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(Rows), "1", "1", "IconImageSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(Columns), "1", "1", "IconImageSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(BitsAllocated), "1", "1", "IconImageSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(BitsStored), "1", "1", "IconImageSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(HighBit), "1", "1", "IconImageSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(PixelRepresentation), "1", "1", "IconImageSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(PlanarConfiguration), "1", "1C", "IconImageSequence");
        addElementToDataset(result, item, new DcmIntegerString(PixelAspectRatio), "2", "1C", "IconImageSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(SmallestImagePixelValue), "1", "3", "IconImageSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(LargestImagePixelValue), "1", "3", "IconImageSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(RedPaletteColorLookupTableDescriptor), "3", "1C", "IconImageSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(GreenPaletteColorLookupTableDescriptor), "3", "1C", "IconImageSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(BluePaletteColorLookupTableDescriptor), "3", "1C", "IconImageSequence");
        addElementToDataset(result, item, new DcmOtherByteOtherWord(RedPaletteColorLookupTableData), "1", "1C", "IconImageSequence");
        addElementToDataset(result, item, new DcmOtherByteOtherWord(GreenPaletteColorLookupTableData), "1", "1C", "IconImageSequence");
        addElementToDataset(result, item, new DcmOtherByteOtherWord(BluePaletteColorLookupTableData), "1", "1C", "IconImageSequence");
        addElementToDataset(result, item, new DcmOtherByteOtherWord(ICCProfile), "1", "3", "IconImageSequence");
        addElementToDataset(result, item, new DcmCodeString(ColorSpace), "1", "3", "IconImageSequence");
        addElementToDataset(result, item, new DcmPixelData(PixelData), "1", "1", "IconImageSequence");
    }
    return result;
}


OFCondition DRTIconImageSequence::Item::getBitsAllocated(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, BitsAllocated).getUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::getBitsStored(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, BitsStored).getUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::getBluePaletteColorLookupTableData(Uint16 *&value, unsigned long *count) const
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


OFCondition DRTIconImageSequence::Item::getBluePaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, BluePaletteColorLookupTableDescriptor).getUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::getColorSpace(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ColorSpace, value, pos);
}


OFCondition DRTIconImageSequence::Item::getColumns(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, Columns).getUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::getGreenPaletteColorLookupTableData(Uint16 *&value, unsigned long *count) const
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


OFCondition DRTIconImageSequence::Item::getGreenPaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, GreenPaletteColorLookupTableDescriptor).getUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::getHighBit(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, HighBit).getUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::getICCProfile(Uint8 *&value, unsigned long *count) const
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


OFCondition DRTIconImageSequence::Item::getLargestImagePixelValue(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, LargestImagePixelValue).getUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::getPhotometricInterpretation(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PhotometricInterpretation, value, pos);
}


OFCondition DRTIconImageSequence::Item::getPixelAspectRatio(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(PixelAspectRatio, value, pos);
}


OFCondition DRTIconImageSequence::Item::getPixelAspectRatio(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, PixelAspectRatio).getSint32(value, pos);
}


OFCondition DRTIconImageSequence::Item::getPixelRepresentation(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, PixelRepresentation).getUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::getPlanarConfiguration(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, PlanarConfiguration).getUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::getRedPaletteColorLookupTableData(Uint16 *&value, unsigned long *count) const
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


OFCondition DRTIconImageSequence::Item::getRedPaletteColorLookupTableDescriptor(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, RedPaletteColorLookupTableDescriptor).getUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::getRows(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, Rows).getUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::getSamplesPerPixel(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, SamplesPerPixel).getUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::getSmallestImagePixelValue(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, SmallestImagePixelValue).getUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::setBitsAllocated(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return BitsAllocated.putUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::setBitsStored(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return BitsStored.putUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::setBluePaletteColorLookupTableData(const Uint16 *value, const unsigned long count)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return BluePaletteColorLookupTableData.putUint16Array(value, count);
}


OFCondition DRTIconImageSequence::Item::setBluePaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return BluePaletteColorLookupTableDescriptor.putUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::setColorSpace(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ColorSpace.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTIconImageSequence::Item::setColumns(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return Columns.putUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::setGreenPaletteColorLookupTableData(const Uint16 *value, const unsigned long count)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return GreenPaletteColorLookupTableData.putUint16Array(value, count);
}


OFCondition DRTIconImageSequence::Item::setGreenPaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return GreenPaletteColorLookupTableDescriptor.putUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::setHighBit(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return HighBit.putUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::setICCProfile(const Uint8 *value, const unsigned long count)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return ICCProfile.putUint8Array(value, count);
}


OFCondition DRTIconImageSequence::Item::setLargestImagePixelValue(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return LargestImagePixelValue.putUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::setPhotometricInterpretation(const OFString &value, const OFBool check)
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


OFCondition DRTIconImageSequence::Item::setPixelAspectRatio(const OFString &value, const OFBool check)
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


OFCondition DRTIconImageSequence::Item::setPixelRepresentation(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return PixelRepresentation.putUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::setPlanarConfiguration(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return PlanarConfiguration.putUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::setRedPaletteColorLookupTableData(const Uint16 *value, const unsigned long count)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RedPaletteColorLookupTableData.putUint16Array(value, count);
}


OFCondition DRTIconImageSequence::Item::setRedPaletteColorLookupTableDescriptor(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return RedPaletteColorLookupTableDescriptor.putUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::setRows(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return Rows.putUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::setSamplesPerPixel(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return SamplesPerPixel.putUint16(value, pos);
}


OFCondition DRTIconImageSequence::Item::setSmallestImagePixelValue(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return SmallestImagePixelValue.putUint16(value, pos);
}


// --- sequence class ---

DRTIconImageSequence::DRTIconImageSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTIconImageSequence::DRTIconImageSequence(const DRTIconImageSequence &copy)
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


DRTIconImageSequence &DRTIconImageSequence::operator=(const DRTIconImageSequence &copy)
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


DRTIconImageSequence::~DRTIconImageSequence()
{
    clear();
}


void DRTIconImageSequence::clear()
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


OFBool DRTIconImageSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTIconImageSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTIconImageSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTIconImageSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIconImageSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIconImageSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTIconImageSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTIconImageSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTIconImageSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTIconImageSequence::Item &DRTIconImageSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTIconImageSequence::Item &DRTIconImageSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTIconImageSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTIconImageSequence::Item &DRTIconImageSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTIconImageSequence::Item &DRTIconImageSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTIconImageSequence::Item &DRTIconImageSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTIconImageSequence::Item &DRTIconImageSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTIconImageSequence::addItem(Item *&item)
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


OFCondition DRTIconImageSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTIconImageSequence::removeItem(const size_t pos)
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


OFCondition DRTIconImageSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_IconImageSequence, sequence);
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
            DcmSequenceOfItems element(DCM_IconImageSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTIconImageSequence::write(DcmItem &dataset,
                                        const OFString &card,
                                        const OFString &type,
                                        const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_IconImageSequence);
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
