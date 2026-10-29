/*
 *
 *  Copyright (C) 2016-2026, Open Connections GmbH
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
 *  Purpose: Floating Point and Double Floating Point Image Pixel Module
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/modfloatingpointimagepixel.h"
#include "dcmtk/dcmdata/dcdeftag.h"

// Per-class default IODRules shared across instances (copy-on-write; see
// IODComponent). Must live at namespace scope, not inside a function: in
// C++98 the initialization of function-local statics is not thread-safe,
// and that applies to the OFMutex itself -- a mutex cannot guard its own
// construction. Namespace-scope statics are initialized before main() on
// the single startup thread, so both objects are guaranteed ready when
// user threads first try to lock.
namespace
{
    OFshared_ptr<IODRules> s_fpImagePixelRules;
    OFMutex                s_fpImagePixelMutex;
    OFshared_ptr<IODRules> s_doubleFpImagePixelRules;
    OFMutex                s_doubleFpImagePixelMutex;
}

const OFString IODFloatingPointImagePixelModule::m_ModuleName          = "FloatingPointImagePixelModule";
const DcmTagKey IODFloatingPointImagePixelModule::pixel_data_tag       = DCM_FloatPixelData;
const DcmTagKey IODDoubleFloatingPointImagePixelModule::pixel_data_tag = DCM_DoubleFloatPixelData;

IODFloatingPointImagePixelModule::IODFloatingPointImagePixelModule(OFshared_ptr<DcmItem> item,
                                                                   OFshared_ptr<IODRules> rules)
    : IODImagePixelBase(item, rules)
{
    // reset element rules
    resetRules();
    getData().putAndInsertUint16(DCM_BitsAllocated, 32);
    getData().putAndInsertUint16(DCM_SamplesPerPixel, 1);
    getData().putAndInsertUint16(DCM_PixelRepresentation, 1);
    getData().putAndInsertOFStringArray(DCM_PhotometricInterpretation, "MONOCHROME2");
}

OFString IODFloatingPointImagePixelModule::getName() const
{
    return m_ModuleName;
}

IODFloatingPointImagePixelModule::IODFloatingPointImagePixelModule()
    : IODImagePixelBase()
{
    resetRules();
}

IODFloatingPointImagePixelModule::~IODFloatingPointImagePixelModule()
{
    // nothing to do
}

void IODFloatingPointImagePixelModule::resetRules()
{
    s_fpImagePixelMutex.lock();
    if (!s_fpImagePixelRules)
    {
        s_fpImagePixelRules.reset(new IODRules());
        s_fpImagePixelRules->addRule(
            new IODRule(DCM_SamplesPerPixel, "1", "1", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
        s_fpImagePixelRules->addRule(
            new IODRule(DCM_PhotometricInterpretation, "1", "1", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
        s_fpImagePixelRules->addRule(
            new IODRule(DCM_Rows, "1", "1", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
        s_fpImagePixelRules->addRule(
            new IODRule(DCM_Columns, "1", "1", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
        s_fpImagePixelRules->addRule(
            new IODRule(DCM_BitsAllocated, "1", "1", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
        s_fpImagePixelRules->addRule(
            new IODRule(DCM_PixelAspectRatio, "2", "1C", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
        s_fpImagePixelRules->addRule(
            new IODRule(DCM_FloatPixelPaddingValue, "1", "3", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
        s_fpImagePixelRules->addRule(
            new IODRule(DCM_FloatPixelPaddingRangeLimit, "1", "1C", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
    }
    s_fpImagePixelMutex.unlock();
    if (!m_ExternalRules)
    {
        m_Rules       = s_fpImagePixelRules;
        m_HasOwnRules = OFFalse;
    }
    else
    {
        IODRules::iterator it = s_fpImagePixelRules->begin();
        while (it != s_fpImagePixelRules->end())
        {
            m_Rules->addRule(it->second->clone(), OFTrue);
            ++it;
        }
    }
}

OFCondition IODFloatingPointImagePixelModule::read(DcmItem& source, const OFBool clearOldData)
{
    // Read common attributes
    IODImagePixelBase::read(source, clearOldData);
    // Read extra attributes of Floating Point Image Pixel Module
    IODModule::read(source, clearOldData);
    return EC_Normal;
}

OFCondition IODFloatingPointImagePixelModule::write(DcmItem& destination)
{
    // Write Photometric Interpretation fixed value for Floating Point Image Pixel Module
    OFCondition result = m_Item->putAndInsertOFStringArray(DCM_PhotometricInterpretation, "MONOCHROME2");
    // Write common attributes
    if (result.good())
    {
        result = IODImagePixelBase::write(destination);
    }
    // Write extra attributes of Floating Point Image Pixel Module
    if (result.good())
    {
        result = IODModule::write(destination);
    }
    return result;
}

IODImagePixelBase::DataType IODFloatingPointImagePixelModule::getDataType() const
{
    return IODImagePixelBase::DATA_TYPE_FLOAT;
}

OFCondition IODFloatingPointImagePixelModule::getFloatPixelPaddingValue(Float32& value, const unsigned long pos)
{
    return m_Item->findAndGetFloat32(DCM_FloatPixelPaddingValue, value, pos);
}

OFCondition IODFloatingPointImagePixelModule::getFloatPixelPaddingRangeLimit(Float32& value, const unsigned long pos)
{
    return m_Item->findAndGetFloat32(DCM_FloatPixelPaddingRangeLimit, value, pos);
}

OFCondition IODFloatingPointImagePixelModule::setFloatPixelPaddingValue(const Float32 value, const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertFloat32(DCM_FloatPixelPaddingValue, value);
}

OFCondition IODFloatingPointImagePixelModule::setFloatPixelPaddingRangeLimit(const Float32 value,
                                                                             const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertFloat32(DCM_FloatPixelPaddingRangeLimit, value);
}

// ---------------- Double Floating Point Image Pixel Module ------------------

const OFString IODDoubleFloatingPointImagePixelModule::m_ModuleName = "DoubleFloatingPointImagePixelModule";

IODDoubleFloatingPointImagePixelModule::IODDoubleFloatingPointImagePixelModule(OFshared_ptr<DcmItem> item,
                                                                               OFshared_ptr<IODRules> rules)
    : IODImagePixelBase(item, rules)
{
    // reset element rules
    resetRules();

    getData().putAndInsertUint16(DCM_BitsAllocated, 64);
    getData().putAndInsertUint16(DCM_SamplesPerPixel, 1);
    getData().putAndInsertUint16(DCM_PixelRepresentation, 1);
    getData().putAndInsertOFStringArray(DCM_PhotometricInterpretation, "MONOCHROME2");
}

OFString IODDoubleFloatingPointImagePixelModule::getName() const
{
    return m_ModuleName;
}

IODDoubleFloatingPointImagePixelModule::IODDoubleFloatingPointImagePixelModule()
    : IODImagePixelBase()
{
    resetRules();
}

IODDoubleFloatingPointImagePixelModule::~IODDoubleFloatingPointImagePixelModule()
{
    // nothing to do
}

void IODDoubleFloatingPointImagePixelModule::resetRules()
{
    s_doubleFpImagePixelMutex.lock();
    if (!s_doubleFpImagePixelRules)
    {
        s_doubleFpImagePixelRules.reset(new IODRules());
        s_doubleFpImagePixelRules->addRule(
            new IODRule(DCM_SamplesPerPixel, "1", "1", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
        s_doubleFpImagePixelRules->addRule(
            new IODRule(DCM_PhotometricInterpretation, "1", "1", m_ModuleName, DcmIODTypes::IE_IMAGE, "MONOCHROME2"),
            OFTrue);
        s_doubleFpImagePixelRules->addRule(
            new IODRule(DCM_Rows, "1", "1", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
        s_doubleFpImagePixelRules->addRule(
            new IODRule(DCM_Columns, "1", "1", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
        s_doubleFpImagePixelRules->addRule(
            new IODRule(DCM_BitsAllocated, "1", "1", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
        s_doubleFpImagePixelRules->addRule(
            new IODRule(DCM_PixelAspectRatio, "2", "1C", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
        s_doubleFpImagePixelRules->addRule(
            new IODRule(DCM_DoubleFloatPixelPaddingValue, "1", "3", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
        s_doubleFpImagePixelRules->addRule(
            new IODRule(DCM_DoubleFloatPixelPaddingRangeLimit, "1", "1C", m_ModuleName, DcmIODTypes::IE_IMAGE),
            OFTrue);
    }
    s_doubleFpImagePixelMutex.unlock();
    if (!m_ExternalRules)
    {
        m_Rules       = s_doubleFpImagePixelRules;
        m_HasOwnRules = OFFalse;
    }
    else
    {
        IODRules::iterator it = s_doubleFpImagePixelRules->begin();
        while (it != s_doubleFpImagePixelRules->end())
        {
            m_Rules->addRule(it->second->clone(), OFTrue);
            ++it;
        }
    }
}

OFCondition IODDoubleFloatingPointImagePixelModule::read(DcmItem& source, const OFBool clearOldData)
{
    // Read common attributes
    IODImagePixelBase::read(source, clearOldData);
    // Read extra attributes of Floating Point Image Pixel Module
    IODModule::read(source, clearOldData);
    return EC_Normal;
}

OFCondition IODDoubleFloatingPointImagePixelModule::write(DcmItem& destination)
{
    // Write Photometric Interpretation fixed value for Floating Point Image Pixel Module
    OFCondition result = m_Item->putAndInsertOFStringArray(DCM_PhotometricInterpretation, "MONOCHROME2");
    // Write common attributes
    if (result.good())
    {
        result = IODImagePixelBase::write(destination);
    }
    // Write extra attributes of Floating Point Image Pixel Module
    if (result.good())
    {
        result = IODModule::write(destination);
    }
    return result;
}

IODImagePixelBase::DataType IODDoubleFloatingPointImagePixelModule::getDataType() const
{
    return IODImagePixelBase::DATA_TYPE_DOUBLE;
}

OFCondition IODDoubleFloatingPointImagePixelModule::getDoubleFloatPixelPaddingValue(Float64& value,
                                                                                    const unsigned long pos)
{
    return m_Item->findAndGetFloat64(DCM_DoubleFloatPixelPaddingValue, value, pos);
}

OFCondition IODDoubleFloatingPointImagePixelModule::getDoubleFloatPixelPaddingRangeLimit(Float64& value,
                                                                                         const unsigned long pos)
{
    return m_Item->findAndGetFloat64(DCM_DoubleFloatPixelPaddingRangeLimit, value, pos);
}

OFCondition IODDoubleFloatingPointImagePixelModule::setDoubleFloatPixelPaddingValue(const Float64 value,
                                                                                    const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertFloat64(DCM_DoubleFloatPixelPaddingValue, value);
}

OFCondition IODDoubleFloatingPointImagePixelModule::setDoubleFloatPixelPaddingRangeLimit(const Float64 value,
                                                                                         const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertFloat64(DCM_DoubleFloatPixelPaddingRangeLimit, value);
}
