/*
 *
 *  Copyright (C) 2016-2024, Open Connections GmbH
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
 *  Purpose: Class for managing the Image Pixel Module
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/modimagepixel.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrobow.h"
#include "dcmtk/dcmiod/iodutil.h"

template <typename T>
const OFString IODImagePixelModule<T>::m_ModuleName = "ImagePixelModule";
template <typename T>
const DcmTagKey IODImagePixelModule<T>::pixel_data_tag = DCM_PixelData;

template <typename T>
IODImagePixelModule<T>::IODImagePixelModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules)
    : IODImagePixelBase(item, rules)
{
    // reset element rules
    resetRules();
}

template <typename T>
OFString IODImagePixelModule<T>::getName() const
{
    return m_ModuleName;
}

template <typename T>
IODImagePixelModule<T>::IODImagePixelModule()
    : IODImagePixelBase()
{
    resetRules();
}

template <typename T>
IODImagePixelModule<T>::~IODImagePixelModule()
{
}

template <typename T>
void IODImagePixelModule<T>::resetRules()
{
    // parameters are tag, VM, type. Overwrite old rules if any.
    m_Rules->addRule(new IODRule(DCM_SamplesPerPixel, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_PhotometricInterpretation, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_Rows, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_Columns, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_BitsAllocated, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_BitsStored, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_HighBit, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_PixelRepresentation, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_PlanarConfiguration, "1", "1C", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_PixelAspectRatio, "2", "1C", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_ICCProfile, "1", "3", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
}

template <typename T>
OFCondition IODImagePixelModule<T>::read(DcmItem& source, const OFBool clearOldData)
{
    // Read common attributes
    IODImagePixelBase::read(source, clearOldData);
    // Read extra attributes of Image Pixel Module
    IODModule::read(source, clearOldData);
    return EC_Normal;
}

template <typename T>
OFCondition IODImagePixelModule<T>::write(DcmItem& destination)
{
    // Write common attributes
    OFCondition result = IODImagePixelBase::write(destination);
    // Write extra attributes of Image Pixel Module
    if (result.good())
    {
        IODModule::write(destination);
    }
    return result;
}

template <typename T>
IODImagePixelBase::DataType IODImagePixelModule<T>::getDataType() const
{
    return IODImagePixelBase::DATA_TYPE_INTEGER;
}

template <typename T>
OFCondition IODImagePixelModule<T>::getBitsStored(Uint16& value, const unsigned long pos)
{
    return m_Item->findAndGetUint16(DCM_BitsStored, value, pos);
}

template <typename T>
OFCondition IODImagePixelModule<T>::getHighBit(Uint16& value, const unsigned long pos)
{
    return m_Item->findAndGetUint16(DCM_HighBit, value, pos);
}

template <typename T>
OFCondition IODImagePixelModule<T>::getPixelRepresentation(Uint16& value, const unsigned long pos)
{
    return m_Item->findAndGetUint16(DCM_PixelRepresentation, value, pos);
}

template <typename T>
OFCondition IODImagePixelModule<T>::getPlanarConfiguration(Uint16& value, const unsigned long pos)
{
    return m_Item->findAndGetUint16(DCM_PlanarConfiguration, value, pos);
}

template <typename T>
OFCondition IODImagePixelModule<T>::getICCProfile(OFVector<Uint8>& values)
{
    DcmElement* elem   = NULL;
    OFCondition result = m_Item->findAndGetElement(DCM_ICCProfile, elem);
    if (result.good())
    {
        DcmOtherByteOtherWord* ob = OFstatic_cast(DcmOtherByteOtherWord*, elem);
        if (ob)
            return DcmIODUtil::copyFromUint8Array<OFVector<Uint8> >(ob, values);
        else
            return EC_InternalError;
    }
    else
        return EC_TagNotFound;
}

template <typename T>
OFCondition IODImagePixelModule<T>::setSamplesPerPixel(const Uint16 value, const OFBool checkValue)
{
    OFCondition result = EC_Normal;

    // only values 1, 3 and 4 (retired) are permitted
    if (checkValue)
    {
        if ((value == 0) || (value == 2) || (value > 4))
        {
            DCMIOD_ERROR("Value " << value << " not permitted for attribute Samples Per Pixel");
            result = EC_InvalidValue;
        }
        else if (value == 4)
        {
            DCMIOD_WARN("Value " << value << " is retired for attribute Samples Per Pixel");
        }
    }

    if (result.good())
        result = m_Item->putAndInsertUint16(DCM_SamplesPerPixel, value);

    return result;
}

template <typename T>
OFCondition IODImagePixelModule<T>::setPhotometricInterpretation(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_PhotometricInterpretation, value);
    return result;
}

template <typename T>
OFCondition IODImagePixelModule<T>::setBitsAllocated(const Uint16 value, const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertUint16(DCM_BitsAllocated, value);
}

template <typename T>
OFCondition IODImagePixelModule<T>::setBitsStored(const Uint16 value, const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertUint16(DCM_BitsStored, value);
}

template <typename T>
OFCondition IODImagePixelModule<T>::setHighBit(const Uint16 value, const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertUint16(DCM_HighBit, value);
}

template <typename T>
OFCondition IODImagePixelModule<T>::setPixelRepresentation(const Uint16 value, const OFBool checkValue)
{
    OFCondition result = EC_Normal;
    if (checkValue)
    {
        if ((value != 0) && (value != 1))
        {
            DCMIOD_ERROR("Value " << value << " not permitted for attribute Pixel Representation");
            result = EC_InvalidValue;
        }
    }
    if (result.good())
    {
        result = m_Item->putAndInsertUint16(DCM_PixelRepresentation, value);
    }
    return result;
}

template <typename T>
OFCondition IODImagePixelModule<T>::setPlanarConfiguration(const Uint16 value, const OFBool checkValue)
{
    OFCondition result = EC_Normal;
    if (checkValue)
    {
        if ((value != 0) && (value != 1))
        {
            DCMIOD_ERROR("Value " << value << " not permitted for attribute Planar Configuration");
            result = EC_InvalidValue;
        }
    }
    if (result.good())
    {
        result = m_Item->putAndInsertUint16(DCM_PlanarConfiguration, value);
    }
    return result;
}

template <typename T>
OFCondition IODImagePixelModule<T>::setICCProfile(const Uint8* values, const size_t length)
{
    return m_Item->putAndInsertUint8Array(DCM_ICCProfile, values, OFstatic_cast(unsigned long, length));
}

template class IODImagePixelModule<Uint8>;
template class IODImagePixelModule<Sint8>;
template class IODImagePixelModule<Uint16>;
template class IODImagePixelModule<Sint16>;
