/*
 *
 *  Copyright (C) 2024-2025, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmiod
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class for managing the Palette Color LUT Module
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcelem.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmdata/dcvrobow.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmiod/iodtypes.h"
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/dcmiod/modpalettecolorlut.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmimgle/diluptab.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/oftypes.h"

const OFString IODPaletteColorLUTModule::m_ModuleName = "PaletteColorLookupTableModule";

IODPaletteColorLUTModule::IODPaletteColorLUTModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules)
    : IODModule(item, rules)
{
    // reset element rules
    resetRules();
}

IODPaletteColorLUTModule::IODPaletteColorLUTModule()
    : IODModule()
{
    resetRules();
}


IODPaletteColorLUTModule::~IODPaletteColorLUTModule()
{
    // nothing to do
}

void IODPaletteColorLUTModule::resetRules()
{
    m_Rules->addRule(new IODRule(DCM_RedPaletteColorLookupTableDescriptor, "3", "1", getName(), DcmIODTypes::IE_IMAGE),
                     OFTrue);
    m_Rules->addRule(
        new IODRule(DCM_GreenPaletteColorLookupTableDescriptor, "3", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_BluePaletteColorLookupTableDescriptor, "3", "1", getName(), DcmIODTypes::IE_IMAGE),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_PaletteColorLookupTableUID, "1", "3", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_RedPaletteColorLookupTableData, "1", "1C", getName(), DcmIODTypes::IE_IMAGE),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_GreenPaletteColorLookupTableData, "1", "1C", getName(), DcmIODTypes::IE_IMAGE),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_BluePaletteColorLookupTableData, "1", "1C", getName(), DcmIODTypes::IE_IMAGE),
                     OFTrue);
    m_Rules->addRule(
        new IODRule(DCM_SegmentedRedPaletteColorLookupTableData, "1", "1C", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(
        new IODRule(DCM_SegmentedGreenPaletteColorLookupTableData, "1", "1C", getName(), DcmIODTypes::IE_IMAGE),
        OFTrue);
    m_Rules->addRule(
        new IODRule(DCM_SegmentedBluePaletteColorLookupTableData, "1", "1C", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
}

void IODPaletteColorLUTModule::clearData()
{
    IODModule::clearData();
}

OFString IODPaletteColorLUTModule::getName() const
{
    return m_ModuleName;
}

OFCondition IODPaletteColorLUTModule::read(DcmItem& source, const OFBool clearOldData)
{
    if (clearOldData)
        clearData();

    IODComponent::read(source, OFFalse /* data already cleared */);
    OFBool isSegmented;
    checkSegmentConsistency(OFFalse, isSegmented);
    if (!isSegmented)
    {
        checkLUT(DCM_RedPaletteColorLookupTableDescriptor, DCM_RedPaletteColorLookupTableData);
        checkLUT(DCM_GreenPaletteColorLookupTableDescriptor, DCM_GreenPaletteColorLookupTableData);
        checkLUT(DCM_BluePaletteColorLookupTableDescriptor, DCM_BluePaletteColorLookupTableData);
        checkDescriptorConsistency(OFFalse /* only warn */);
        checkDataConsistency(OFFalse /* only warn */);
    }
    return EC_Normal;
}

OFCondition IODPaletteColorLUTModule::write(DcmItem& destination)
{
    OFBool valid, isSegmented;
    valid = checkSegmentConsistency(OFFalse, isSegmented);
    if (valid)
    {
        if (!isSegmented)
        {
            valid = checkLUT(DCM_RedPaletteColorLookupTableDescriptor, DCM_RedPaletteColorLookupTableData);
            if (valid)
                valid = checkLUT(DCM_GreenPaletteColorLookupTableDescriptor, DCM_GreenPaletteColorLookupTableData);
            if (valid)
                valid = checkLUT(DCM_BluePaletteColorLookupTableDescriptor, DCM_BluePaletteColorLookupTableData);
            if (valid)
                valid = checkDescriptorConsistency(OFTrue /* report as errors */);
            if (valid)
                valid = checkDataConsistency(OFTrue /* report as errors */);
            if (valid)
            {
                return IODComponent::write(destination);
            }
            else
            {
                return IOD_EC_InvalidColorPalette;
            }
        }
        else
        {
            return IODComponent::write(destination);
        }
    }

    return IOD_EC_InvalidColorPalette;
}

OFCondition IODPaletteColorLUTModule::getRedPaletteColorLookupTableDescriptor(Uint16& value,
                                                                              const unsigned long pos) const
{
    OFCondition result = m_Item->findAndGetUint16(DCM_RedPaletteColorLookupTableDescriptor, value, pos);
    return result;
}

OFCondition IODPaletteColorLUTModule::getGreenPaletteColorLookupTableDescriptor(Uint16& value,
                                                                              const unsigned long pos) const
{
    OFCondition result = m_Item->findAndGetUint16(DCM_GreenPaletteColorLookupTableDescriptor, value, pos);
    return result;
}

OFCondition IODPaletteColorLUTModule::getBluePaletteColorLookupTableDescriptor(Uint16& value,
                                                                              const unsigned long pos) const
{
    OFCondition result = m_Item->findAndGetUint16(DCM_BluePaletteColorLookupTableDescriptor, value, pos);
    return result;
}


OFCondition IODPaletteColorLUTModule::getPaletteColorLookupTableUID(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_PaletteColorLookupTableUID, *m_Item, value, pos);
}

OFCondition IODPaletteColorLUTModule::getRedPaletteColorLookupTableData(const Uint16*& dataCopy, unsigned long& numEntries)
{
    return getUint16DataCopy(DCM_RedPaletteColorLookupTableData, dataCopy, numEntries);
}

OFCondition IODPaletteColorLUTModule::getGreenPaletteColorLookupTableData(const Uint16*& dataCopy, unsigned long& numEntries)
{
    return getUint16DataCopy(DCM_GreenPaletteColorLookupTableData, dataCopy, numEntries);
}

OFCondition IODPaletteColorLUTModule::getBluePaletteColorLookupTableData(const Uint16*& dataCopy, unsigned long& numEntries)
{
    return getUint16DataCopy(DCM_BluePaletteColorLookupTableData, dataCopy, numEntries);
}

OFCondition IODPaletteColorLUTModule::getSegmentedRedPaletteColorLookupTableData(const Uint16*& dataCopy, unsigned long& numEntries)
{
    return getUint16DataCopy(DCM_SegmentedRedPaletteColorLookupTableData, dataCopy, numEntries);
}

OFCondition IODPaletteColorLUTModule::getSegmentedGreenPaletteColorLookupTableData(const Uint16*& dataCopy, unsigned long& numEntries)
{
    return getUint16DataCopy(DCM_SegmentedGreenPaletteColorLookupTableData, dataCopy, numEntries);
}

OFCondition IODPaletteColorLUTModule::getSegmentedBluePaletteColorLookupTableData(const Uint16*& dataCopy, unsigned long& numEntries)
{
    return getUint16DataCopy(DCM_SegmentedBluePaletteColorLookupTableData, dataCopy, numEntries);
}

// 8 bit versions for LUT data access

OFCondition IODPaletteColorLUTModule::getRedPaletteColorLookupTableData(const Uint8*& dataCopy, unsigned long& numEntries)
{
    return getUint8DataCopy(DCM_RedPaletteColorLookupTableData, dataCopy, numEntries);
}

OFCondition IODPaletteColorLUTModule::getGreenPaletteColorLookupTableData(const Uint8*& dataCopy, unsigned long& numEntries)
{
    return getUint8DataCopy(DCM_GreenPaletteColorLookupTableData, dataCopy, numEntries);
}

OFCondition IODPaletteColorLUTModule::getBluePaletteColorLookupTableData(const Uint8*& dataCopy, unsigned long& numEntries)
{
    return getUint8DataCopy(DCM_BluePaletteColorLookupTableData, dataCopy, numEntries);
}

OFCondition IODPaletteColorLUTModule::getSegmentedRedPaletteColorLookupTableData(const Uint8*& dataCopy, unsigned long& numEntries)
{
    return getUint8DataCopy(DCM_SegmentedRedPaletteColorLookupTableData, dataCopy, numEntries);
}

OFCondition IODPaletteColorLUTModule::getSegmentedGreenPaletteColorLookupTableData(const Uint8*& dataCopy, unsigned long& numEntries)
{
    return getUint8DataCopy(DCM_SegmentedGreenPaletteColorLookupTableData, dataCopy, numEntries);
}

OFCondition IODPaletteColorLUTModule::getSegmentedBluePaletteColorLookupTableData(const Uint8*& dataCopy, unsigned long& numEntries)
{
    return getUint8DataCopy(DCM_SegmentedBluePaletteColorLookupTableData, dataCopy, numEntries);
}

// -------------------- set() --------------------

OFCondition IODPaletteColorLUTModule::setRedPaletteColorLookupTableDescriptor(const Uint16& value,
                                                                              const unsigned long pos)
{
    OFCondition result = m_Item->putAndInsertUint16(DCM_RedPaletteColorLookupTableDescriptor, value, pos);
    return result;
}


OFCondition IODPaletteColorLUTModule::setGreenPaletteColorLookupTableDescriptor(const Uint16& value,
                                                                                const unsigned long pos)
{
    return m_Item->putAndInsertUint16(DCM_GreenPaletteColorLookupTableDescriptor, value, pos);
}


OFCondition IODPaletteColorLUTModule::setBluePaletteColorLookupTableDescriptor(const Uint16& value,
                                                                               const unsigned long pos)
{
    return m_Item->putAndInsertUint16(DCM_BluePaletteColorLookupTableDescriptor, value, pos);
}


OFCondition IODPaletteColorLUTModule::setPaletteColorLookupTableUID(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good()) result = m_Item->putAndInsertOFStringArray(DCM_PaletteColorLookupTableUID, value);
    return result;
}

OFCondition IODPaletteColorLUTModule::setRedPaletteColorLookupTableData(const Uint16* data,
                                                                        const unsigned long numEntries,
                                                                        const OFBool checkValue)
{
    if (checkValue && (numBits() == 8))
    {
        DCMIOD_ERROR("Cannot set 16 bit data for 8 bit LUT");
        return IOD_EC_InvalidColorPalette;
    }
    unsigned long fixZero = numEntries;
    if (numEntries == 0)
    {
        fixZero = 65536;
    }
    return m_Item->putAndInsertUint16Array(DCM_RedPaletteColorLookupTableData, data, fixZero);
}

OFCondition IODPaletteColorLUTModule::setGreenPaletteColorLookupTableData(const Uint16* data,
                                                                          const unsigned long numEntries,
                                                                          const OFBool checkValue)
{
    if (checkValue && (numBits() == 8))
    {
        DCMIOD_ERROR("Cannot set 16 bit data for 8 bit LUT");
        return IOD_EC_InvalidColorPalette;
    }
    unsigned long fixZero = numEntries;
    if (numEntries == 0)
    {
        fixZero = 65536;
    }
    return m_Item->putAndInsertUint16Array(DCM_GreenPaletteColorLookupTableData, data, fixZero);
}

OFCondition IODPaletteColorLUTModule::setBluePaletteColorLookupTableData(const Uint16* data,
                                                                         const unsigned long numEntries,
                                                                         const OFBool checkValue)
{
    if (checkValue && (numBits() == 8))
    {
        DCMIOD_ERROR("Cannot set 16 bit data for 8 bit LUT");
        return IOD_EC_InvalidColorPalette;
    }
    unsigned long zeroFix = numEntries;
    if (numEntries == 0)
    {
        zeroFix = 65536;
    }
    return m_Item->putAndInsertUint16Array(DCM_BluePaletteColorLookupTableData, data, zeroFix);
}

OFCondition IODPaletteColorLUTModule::setRedPaletteColorLookupTableData(const Uint8* data,
                                                                        const unsigned long numEntries,
                                                                        const OFBool checkValue)
{
    if (checkValue && (numBits() == 16))
    {
        DCMIOD_ERROR("Cannot set 8 bit data for 16 bit LUT");
        return IOD_EC_InvalidColorPalette;
    }
    unsigned long zeroFix = numEntries;
    if (numEntries == 0)
    {
        zeroFix = 256;
    }
    return putUint8Data(DCM_RedPaletteColorLookupTableData, data, zeroFix);
}

OFCondition IODPaletteColorLUTModule::setGreenPaletteColorLookupTableData(const Uint8* data,
                                                                          const unsigned long numEntries,
                                                                          const OFBool checkValue)
{
    if (checkValue && (numBits() == 16))
    {
        DCMIOD_ERROR("Cannot set 8 bit data for 16 bit LUT");
        return IOD_EC_InvalidColorPalette;
    }
    unsigned long zeroFix = numEntries;
    if (numEntries == 0)
    {
        zeroFix = 256;
    }
    return putUint8Data(DCM_GreenPaletteColorLookupTableData, data, zeroFix);
}

OFCondition IODPaletteColorLUTModule::setBluePaletteColorLookupTableData(const Uint8* data,
                                                                         const unsigned long numEntries,
                                                                         const OFBool checkValue)
{
    if (checkValue && (numBits() == 16))
    {
        DCMIOD_ERROR("Cannot set 8 bit data for 16 bit LUT");
        return IOD_EC_InvalidColorPalette;
    }
    unsigned long zeroFix = numEntries;
    if (numEntries == 0)
    {
        zeroFix = 256;
    }
    return putUint8Data(DCM_BluePaletteColorLookupTableData, data, zeroFix);
}

OFCondition IODPaletteColorLUTModule::setSegmentedRedPaletteColorLookupTableData(const Uint16* data,
                                                                                 const unsigned long numEntries,
                                                                                 const OFBool checkValue)
{
    if (checkValue && (numBits() == 8))
    {
        DCMIOD_ERROR("Cannot set 16 bit data for 8 bit LUT");
        return IOD_EC_InvalidColorPalette;
    }
    unsigned long zeroFix = numEntries;
    if (numEntries == 0)
    {
        zeroFix = 65536;
    }
    return m_Item->putAndInsertUint16Array(DCM_SegmentedRedPaletteColorLookupTableData, data, zeroFix);
}

OFCondition IODPaletteColorLUTModule::setSegmentedGreenPaletteColorLookupTableData(const Uint16* data,
                                                                                   const unsigned long numEntries,
                                                                                   const OFBool checkValue)
{
    if (checkValue && (numBits() == 8))
    {
        DCMIOD_ERROR("Cannot set 16 bit data for 8 bit LUT");
        return IOD_EC_InvalidColorPalette;
    }
    unsigned long zeroFix = numEntries;
    if (numEntries == 0)
    {
        zeroFix = 65536;
    }
    return m_Item->putAndInsertUint16Array(DCM_SegmentedGreenPaletteColorLookupTableData, data, zeroFix);
}

OFCondition IODPaletteColorLUTModule::setSegmentedBluePaletteColorLookupTableData(const Uint16* data,
                                                                                  const unsigned long numEntries,
                                                                                  const OFBool checkValue)
{
    if (checkValue && (numBits() == 8))
    {
        DCMIOD_ERROR("Cannot set 16 bit data for 8 bit LUT");
        return IOD_EC_InvalidColorPalette;
    }
    unsigned long zeroFix = numEntries;
    if (numEntries == 0)
    {
        zeroFix = 65536;
    }
    return m_Item->putAndInsertUint16Array(DCM_SegmentedBluePaletteColorLookupTableData, data, zeroFix);
}

OFCondition IODPaletteColorLUTModule::setSegmentedRedPaletteColorLookupTableData(const Uint8* data,
                                                                                 const unsigned long numEntries,
                                                                                 const OFBool checkValue)
{
    if (checkValue && (numBits() == 16))
    {
        DCMIOD_ERROR("Cannot set 8 bit data for 16 bit LUT");
        return IOD_EC_InvalidColorPalette;
    }
    unsigned long zeroFix = numEntries;
    if (numEntries == 0)
    {
        zeroFix = 256;
    }
    return putUint8Data(DCM_SegmentedRedPaletteColorLookupTableData, data, zeroFix);
}

OFCondition IODPaletteColorLUTModule::setSegmentedGreenPaletteColorLookupTableData(const Uint8* data,
                                                                                   const unsigned long numEntries,
                                                                                   const OFBool checkValue)
{
    if (checkValue && (numBits() == 16))
    {
        DCMIOD_ERROR("Cannot set 8 bit data for 16 bit LUT");
        return IOD_EC_InvalidColorPalette;
    }
    unsigned long zeroFix = numEntries;
    if (numEntries == 0)
    {
        zeroFix = 256;
    }
    return putUint8Data(DCM_SegmentedGreenPaletteColorLookupTableData, data, zeroFix);
}

OFCondition IODPaletteColorLUTModule::setSegmentedBluePaletteColorLookupTableData(const Uint8* data,
                                                                                  const unsigned long numEntries,
                                                                                  const OFBool checkValue)
{
    if (checkValue && (numBits() == 16))
    {
        DCMIOD_ERROR("Cannot set 8 bit data for 16 bit LUT");
        return IOD_EC_InvalidColorPalette;
    }
    unsigned long zeroFix = numEntries;
    if (numEntries == 0)
    {
        zeroFix = 256;
    }
    return putUint8Data(DCM_SegmentedBluePaletteColorLookupTableData, data, zeroFix);
}


template<typename T>
OFCondition IODPaletteColorLUTModule::setPaletteColorLookupTableData(const T* redData,
                                                                     const T* greenData,
                                                                     const T* blueData,
                                                                     const unsigned long numEntries,
                                                                     const OFBool checkValue)
{
    OFCondition result = setRedPaletteColorLookupTableData(redData, numEntries, checkValue);
    if (result.good()) result = setGreenPaletteColorLookupTableData(greenData, numEntries, checkValue);
    if (result.good()) result = setBluePaletteColorLookupTableData(blueData, numEntries, checkValue);
    return result;
}

template<typename T>
OFCondition IODPaletteColorLUTModule::setSegmentedPaletteColorLookupTableData(const T* redData,
                                                                              const T* greenData,
                                                                              const T* blueData,
                                                                              const unsigned long numEntries,
                                                                              const OFBool checkValue)
{
    OFCondition result = setSegmentedRedPaletteColorLookupTableData(redData, numEntries, checkValue);
    if (result.good()) result = setSegmentedGreenPaletteColorLookupTableData(greenData, numEntries, checkValue);
    if (result.good()) result = setSegmentedBluePaletteColorLookupTableData(blueData, numEntries, checkValue);
    return result;
}

OFCondition IODPaletteColorLUTModule::setRedPaletteColorLookupTableDescriptor(const Uint16 numEntries,
                                                                              const Uint16 firstMapped,
                                                                              const Uint8 bitsPerEntry)
{
    // TODO Check value
    OFCondition result;
    Uint16 values[3];
    values[0] = numEntries;
    values[1] = firstMapped;
    values[2] = bitsPerEntry;

    return m_Item->putAndInsertUint16Array(DCM_RedPaletteColorLookupTableDescriptor, values, 3);
}


OFCondition IODPaletteColorLUTModule::setGreenPaletteColorLookupTableDescriptor(const Uint16 numEntries,
                                                                                const Uint16 firstMapped,
                                                                                const Uint8 bitsPerEntry)
{
    // TODO check value
    OFCondition result;
    Uint16 values[3];
    values[0] = numEntries;
    values[1] = firstMapped;
    values[2] = bitsPerEntry;
    return m_Item->putAndInsertUint16Array(DCM_GreenPaletteColorLookupTableDescriptor, values, 3);
}



OFCondition IODPaletteColorLUTModule::setBluePaletteColorLookupTableDescriptor(const Uint16 numEntries,
                                                                               const Uint16 firstMapped,
                                                                               const Uint8 bitsPerEntry)
{
    OFCondition result;
    Uint16 values[3];
    values[0] = numEntries;
    values[1] = firstMapped;
    values[2] = bitsPerEntry;

    return m_Item->putAndInsertUint16Array(DCM_BluePaletteColorLookupTableDescriptor, values, 3);
}


OFBool IODPaletteColorLUTModule::checkLUT(const DcmTagKey& descriptorTag,
                                          const DcmTagKey& dataTag)
{
    DcmElement* data = NULL;
    DcmElement* descriptor = NULL;
    m_Item->findAndGetElement(dataTag, data);
    m_Item->findAndGetElement(descriptorTag, descriptor);
    if (!data || !descriptor)
    {
        reportLUTError(descriptorTag, "Palette Color LUT Data or Descriptor missing", OFFalse /* warning */);
        return OFFalse;
    }
    // LUT Data always OW in this module, LUT Descriptor is always US or SS
    if ( (data->getTag().getEVR() != EVR_OW) || ( (descriptor->getTag().getEVR() != EVR_US) && (descriptor->getTag().getEVR() != EVR_SS)) )
    {
        reportLUTError(descriptorTag, "Palette Color LUT Data or Descriptor have invalid VR", OFFalse /* warning */);
        return OFFalse;
    }
    DiLookupTable lut(*OFstatic_cast(DcmOtherByteOtherWord*, data), *OFstatic_cast(DcmUnsignedShort*, descriptor), NULL, ELM_CheckValue);
    if (!lut.isValid())
    {
        reportLUTError(descriptorTag, "Palette Color LUT Data is invalid", OFFalse /* warning */);
        return OFFalse;
    }
    return OFTrue;
}


void IODPaletteColorLUTModule::reportLUTError(const DcmTagKey& tag, const OFString& message, const OFBool& warning)
{
    OFString color;
    if (tag == DCM_RedPaletteColorLookupTableDescriptor)
        color = "Red";
    else if (tag == DCM_GreenPaletteColorLookupTableDescriptor)
        color = "Green";
    else if (tag == DCM_BluePaletteColorLookupTableDescriptor)
        color = "Blue";
    if (warning)
        DCMIOD_WARN(color << " " << message);
    else
        DCMIOD_ERROR(color << " " << message);
}

Uint8 IODPaletteColorLUTModule::numBits()
{
    OFCondition result;
    Uint16 numBitsR = 0;
    Uint16 numBitsG = 0;
    Uint16 numBitsB = 0;
    if (getRedPaletteColorLookupTableDescriptor(numBitsR, 2).good() && getGreenPaletteColorLookupTableDescriptor(numBitsG, 2).good() && getBluePaletteColorLookupTableDescriptor(numBitsB, 2).good())
    {
        if ((numBitsR != numBitsG) || (numBitsR != numBitsB) || (numBitsG != numBitsB))
        {
            DCMIOD_WARN("Bits per entry in Red, Green and Blue Palette Color LUT Descriptor are not equal");
            return 0;
        }
    }
    else
    {
        DCMIOD_WARN("Could not read bits per entry in Red, Green or Blue Palette Color LUT Descriptor");
        return 0;
    }
    if ((numBitsR != 8) && (numBitsR != 16))
    {
        DCMIOD_WARN("Bits per entry in Red, Green and Blue Palette Color LUT Descriptor are greater than 16");
        return 0;
    }
    return OFstatic_cast(Uint8, numBitsR);
}


OFBool IODPaletteColorLUTModule::checkDescriptorConsistency(const OFBool& isError)
{
    // Check whether all 2nd values ("first value mapped") have same value.
    Uint16 redFirstMapped = 0;
    Uint16 greenFirstMapped = 0;
    Uint16 blueFirstMapped = 0;
    Sint32 r,g,b;
    r=g=b=0;
    if (getRedPaletteColorLookupTableDescriptor(redFirstMapped, 1).good())
    {
        if (getGreenPaletteColorLookupTableDescriptor(greenFirstMapped, 1).good())
        {
            if (getBluePaletteColorLookupTableDescriptor(blueFirstMapped, 1).good())
            {
                r = redFirstMapped;
                g = greenFirstMapped;
                b = blueFirstMapped;
                if ( (r != g) || (r != b) || (g != b) )
                {
                    if (isError)
                    {
                        DCMIOD_ERROR("First value mapped in Red, Green and Blue Palette Color LUT Descriptor are not equal");
                    }
                    else
                    {
                        DCMIOD_WARN("First value mapped in Red, Green and Blue Palette Color LUT Descriptor are not equal");
                    }
                    return OFFalse;
                }
                Uint16 lastRed, lastGreen, lastBlue;
                lastRed = lastGreen = lastBlue = 0;
                if (getRedPaletteColorLookupTableDescriptor(lastRed, 2).good() && getGreenPaletteColorLookupTableDescriptor(lastGreen, 2).good() && getBluePaletteColorLookupTableDescriptor(lastBlue, 2).good())
                {
                    if (lastRed != lastGreen || lastRed != lastBlue || lastGreen != lastBlue)
                    {
                        if (isError)
                        {
                            DCMIOD_ERROR("Bits per entry in Red, Green and Blue Palette Color LUT Descriptor are not equal");
                        }
                        else
                        {
                            DCMIOD_WARN("Bits per entry in Red, Green and Blue Palette Color LUT Descriptor are not equal");
                        }
                        return OFFalse;
                    }
                    return OFTrue;
                }
            }
        }
    }
    if (isError)
    {
        DCMIOD_ERROR("Could not read first value mapped in Red, Green or Blue Palette Color LUT Descriptor");
    }
    else
    {
        DCMIOD_WARN("Could not read first value mapped in Red, Green or Blue Palette Color LUT Descriptor");
    }
    return OFFalse;
}


OFBool IODPaletteColorLUTModule::checkDataConsistency(const OFBool& isError)
{
    // Check whether all 2nd values ("first value mapped") have same value.
    Uint16 redFirstMapped, greenFirstMapped, blueFirstMapped, redNumEntries, greenNumEntries, blueNumEntries;
    unsigned long redActualNumEntries, greenActualNumEntries, blueActualNumEntries;
    redFirstMapped = greenFirstMapped = blueFirstMapped = redNumEntries = greenNumEntries = blueNumEntries = 0;
    redActualNumEntries = greenActualNumEntries = blueActualNumEntries = 0;
    const Uint8 bits = numBits();
    OFCondition result;
    OFString message;
    result = getRedPaletteColorLookupTableDescriptor(redFirstMapped, 1);
    if (result.good())
    {
        result = getGreenPaletteColorLookupTableDescriptor(greenFirstMapped, 1);
    }
    if (result.good())
    {
        result = getBluePaletteColorLookupTableDescriptor(blueFirstMapped, 1);
    }
    if (result.bad())
    {
        message = "Could not read first value mapped in Red, Green or Blue Palette Color LUT Descriptor";
    }

    if (result.good())
    {
        if (getRedPaletteColorLookupTableDescriptor(redNumEntries, 0).good())
        {
            if (getGreenPaletteColorLookupTableDescriptor(greenNumEntries, 0).good())
            {
                if (getBluePaletteColorLookupTableDescriptor(blueNumEntries, 0).good())
                {
                    unsigned long maxEntries = 0;
                    if (bits == 8)
                    {
                        maxEntries = 256;
                        const Uint8* data = NULL;
                        result = getRedPaletteColorLookupTableData(data, redActualNumEntries);
                        delete[] data;
                        if (result.good())
                        {
                            result = getGreenPaletteColorLookupTableData(data, greenActualNumEntries);
                            delete[] data;
                        }
                        if (result.good())
                        {
                            result = getBluePaletteColorLookupTableData(data, blueActualNumEntries);
                            delete[] data;
                        }
                    }
                    else if (bits == 16)
                    {
                        maxEntries = 65536;
                        const Uint16* data = NULL;
                        result = getRedPaletteColorLookupTableData(data, redActualNumEntries);
                        delete[] data;
                        if (result.good())
                        {
                            result = getGreenPaletteColorLookupTableData(data, greenActualNumEntries);
                            delete[] data;
                        }
                        if (result.good())
                        {
                            result = getBluePaletteColorLookupTableData(data, blueActualNumEntries);
                            delete[] data;
                        }
                    }
                    if (result.good())
                    {
                        if ( (redNumEntries != redActualNumEntries) || (greenNumEntries != greenActualNumEntries) || (blueNumEntries != blueActualNumEntries) )
                        {
                            // also check special case where number descriptor entries is 0 (i.e. max value)
                            if ( (!redNumEntries && (redActualNumEntries != maxEntries)) || (!greenNumEntries && (greenActualNumEntries != maxEntries)) || (!blueNumEntries && (blueActualNumEntries != maxEntries)) )
                            {
                               message = "Number of entries in Red, Green and Blue Palette Color LUT Data does not match descriptor";
                            }
                        }
                        return OFTrue;
                    }
                }
            }
        }
    }
    if (isError)
    {
        DCMIOD_ERROR(message);
    }
    else
    {
        DCMIOD_WARN(message);
    }
    return OFFalse;
}


OFBool IODPaletteColorLUTModule::checkSegmentConsistency(const OFBool& isError, OFBool& isSegmented)
{
    // Check that unsegmented LUTs are used together with segmented LUTs
    isSegmented = OFFalse;
    OFBool hasNonSegmentedLUTs = OFFalse;
    OFString msg;
    // Check for segmented LUT descriptors
    if (m_Item->tagExists(DCM_SegmentedRedPaletteColorLookupTableData) && m_Item->tagExists(DCM_SegmentedGreenPaletteColorLookupTableData) && m_Item->tagExists(DCM_SegmentedBluePaletteColorLookupTableData))
    {
        // Also check for segmented LUT data
        if (m_Item->tagExists(DCM_SegmentedRedPaletteColorLookupTableData) && m_Item->tagExists(DCM_SegmentedGreenPaletteColorLookupTableData) && m_Item->tagExists(DCM_SegmentedBluePaletteColorLookupTableData))
        {
            isSegmented = OFTrue;
        }
    }
    // Check for unsegmented LUT descriptors
    if (m_Item->tagExists(DCM_RedPaletteColorLookupTableDescriptor) || m_Item->tagExists(DCM_GreenPaletteColorLookupTableDescriptor) || m_Item->tagExists(DCM_BluePaletteColorLookupTableDescriptor))
    {
        // Check for unsegmented LUT data
        if (m_Item->tagExists(DCM_RedPaletteColorLookupTableData) || m_Item->tagExists(DCM_GreenPaletteColorLookupTableData) || m_Item->tagExists(DCM_BluePaletteColorLookupTableData))
        {
            hasNonSegmentedLUTs = OFTrue;
        }
    }
    // Check that both are not used together
    if (isSegmented && hasNonSegmentedLUTs)
    {
        msg = "Segmented Palette LUT attributes are used together with Unsegmented LUT attributes";
    }
    if (!isSegmented && !hasNonSegmentedLUTs)
    {
        msg = "No or incomplete Palette LUT attributes found";
    }
    if (msg.length() > 0)
    {
        if (isError)
        {
            DCMIOD_ERROR(msg);
        }
        else
        {
            DCMIOD_WARN(msg);
        }
        return OFFalse;
    }
    return OFTrue;
}


OFCondition IODPaletteColorLUTModule::getUint8DataCopy(const DcmTagKey& dataTag, const Uint8*& lutData, unsigned long& num8BitEntries)
{
    OFCondition result = EC_Normal;
    num8BitEntries = 0;

    const Uint16* data = NULL;
    unsigned long num16BitEntries = 0;
    // check first whether we actually have 8 bit data by checking third descriptor value
    if (numBits() != 8)
    {
        DCMIOD_ERROR("Cannot convert 16 bit data to 8 bit data: Descriptor does not indicate 8 bit data");
        return EC_IllegalParameter;
    }
    result = m_Item->findAndGetUint16Array(dataTag, data, &num16BitEntries);
    if (result.good())
    {
        Uint16 numDescriptor = 0;
        // get number of entries according to first value of descriptor
        result = numEntriesForData(dataTag, numDescriptor);
        if (result.bad())
        {
            DCMIOD_ERROR("Could not determine number of 8 bit entries for " << dataTag);
            return IOD_EC_InvalidColorPalette;
        }
        num8BitEntries = numDescriptor;
        if (num8BitEntries == 0)
        {
            num8BitEntries = 256;
        }
        // check whether number of entries is consistent with number of 16 bit entries
        if ( (num8BitEntries != num16BitEntries*2) && (num8BitEntries != num16BitEntries*2 -1) )
        {
            DCMIOD_DEBUG("Number of 8 bit entries from descriptor: " << numDescriptor << ", Number of 16 bit entries in data: " << num16BitEntries);
            DCMIOD_ERROR("Number of 8 bit entries does not match number of 16 bit entries");
            return IOD_EC_InvalidColorPalette;
        }
        Uint8* newData = new Uint8[num8BitEntries];
        if (num8BitEntries > 1)
        {
            for (unsigned long i = 0; i < num16BitEntries - 1; i++)
            {
                // extract lower and higher byte of 16 bit entry
                newData[i * 2] = OFstatic_cast(Uint8, data[i] >> 8);
                newData[i * 2 + 1] = OFstatic_cast(Uint8, data[i] & 0x00FF);
            }
        }
        if (num8BitEntries % 2)
        {
            // if odd number of 16 bit entries, only extract lower byte of last entry
            newData[num8BitEntries-1] = OFstatic_cast(Uint8, data[num16BitEntries-1] >> 8);
        }
        else
        {
            newData[num8BitEntries-2] = OFstatic_cast(Uint8, data[num16BitEntries-1] >> 8);
            newData[num8BitEntries-1] = OFstatic_cast(Uint8, data[num16BitEntries-1] & 0x00FF);
        }
        lutData = newData;
    }
    return result;
}


OFCondition IODPaletteColorLUTModule::getUint16Data(const DcmTagKey& dataTag, const Uint16*& lutData, unsigned long& numEntries)
{
    OFCondition result;
    const Uint16* data = NULL;

    result = m_Item->findAndGetUint16Array(dataTag, data, &numEntries, OFFalse);
    if (result.good())
    {
        lutData = data;
    }
    return result;
}

OFCondition IODPaletteColorLUTModule::getUint16DataCopy(const DcmTagKey& dataTag, const Uint16*& lutData, unsigned long& numEntries)
{
    const Uint16* buffer;
    OFCondition result = getUint16Data(dataTag, buffer, numEntries);
    if (result.good())
    {
        Uint16* newData = new Uint16[numEntries];
        // copy data
        for (unsigned long i = 0; i < numEntries; i++)
        {
            newData[i] = buffer[i];
        }
        lutData = newData;
    }
    return result;
}


OFCondition IODPaletteColorLUTModule::putUint8Data(const DcmTagKey& dataTag, const Uint8* lutData, const unsigned long num8BitEntries)
{
    if (numBits() != 8)
    {
        DCMIOD_ERROR("Cannot convert 8 bit data to 16 bit data: Descriptor does not indicate 8 bit data");
        return EC_IllegalParameter;
    }
    OFCondition result;
    unsigned long num16BitEntries = num8BitEntries / 2 + (num8BitEntries % 2);
    // put 16 bit data together by combining two 8 bit entries into one 16 bit entry
    Uint16* newData = new Uint16[num16BitEntries];
    for (unsigned long i = 0; i < num16BitEntries; i++)
    {
        if (i != num16BitEntries-1)
        {
            newData[i] = OFstatic_cast(Uint16, lutData[i*2] << 8) | lutData[i*2+1];
        }
        else
        {
            // if odd number of 8 bit entries, only use lower byte of last entry
            newData[i] = OFstatic_cast(Uint16, lutData[i*2] << 8);
        }
    }
    // Reset last entry (second part of 16 bit word) to 0 if odd number of 8 bit entries
    if (num8BitEntries % 2)
    {
        newData[num16BitEntries-1] &= 0xFF00;
    }
    // otherwise also take over last 8 bit entry
    else
    {
        newData[num16BitEntries-1] |= lutData[num8BitEntries-1];
    }
    result = m_Item->putAndInsertUint16Array(dataTag, newData, num16BitEntries);
    delete[] newData;
    return result;
}


OFCondition IODPaletteColorLUTModule::numEntriesForData(const DcmTagKey& dataTag, Uint16& result)
{
    result = 0;
    OFCondition cond;
    if (dataTag == DCM_RedPaletteColorLookupTableData)
    {
        cond = getRedPaletteColorLookupTableDescriptor(result, 0);
    }
    else if (dataTag == DCM_GreenPaletteColorLookupTableData)
    {
        cond = getGreenPaletteColorLookupTableDescriptor(result, 0);
    }
    else if (dataTag == DCM_BluePaletteColorLookupTableData)
    {
        cond = getBluePaletteColorLookupTableDescriptor(result, 0);
    }
    else if (dataTag == DCM_SegmentedRedPaletteColorLookupTableData)
    {
        cond = getRedPaletteColorLookupTableDescriptor(result, 0);
    }
    else if (dataTag == DCM_SegmentedGreenPaletteColorLookupTableData)
    {
        cond = getGreenPaletteColorLookupTableDescriptor(result, 0);
    }
    else if (dataTag == DCM_SegmentedBluePaletteColorLookupTableData)
    {
        cond = getBluePaletteColorLookupTableDescriptor(result, 0);
    }
    else
    {
        return EC_InvalidTag;
    }
    return cond;
}
