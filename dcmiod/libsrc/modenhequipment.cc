/*
 *
 *  Copyright (C) 2015-2024, Open Connections GmbH
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
 *  Purpose: Class for managing the Enhanced General Equipment Module
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/modenhequipment.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmiod/iodutil.h"

IODEnhGeneralEquipmentModule::IODEnhGeneralEquipmentModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules)
    : IODModule(item, rules)
    , m_ModuleName("EnhancedGeneralEquipmentModule")
{
    // reset element rules
    resetRules();
}

IODEnhGeneralEquipmentModule::IODEnhGeneralEquipmentModule()
    : IODModule()
    , m_ModuleName("EnhancedGeneralEquipmentModule")
{
    resetRules();
}

OFCondition IODEnhGeneralEquipmentModule::create(const IODEnhGeneralEquipmentModule::EquipmentInfo& info,
                                                 IODEnhGeneralEquipmentModule* equipment)
{
    equipment = new IODEnhGeneralEquipmentModule();
    if (!equipment)
    {
        return EC_MemoryExhausted;
    }
    return equipment->set(info);
}

void IODEnhGeneralEquipmentModule::resetRules()
{
    m_Rules->addRule(new IODRule(DCM_Manufacturer, "1", "1", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
    m_Rules->addRule(new IODRule(DCM_ManufacturerModelName, "1", "1", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
    m_Rules->addRule(new IODRule(DCM_DeviceSerialNumber, "1", "1", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
    m_Rules->addRule(new IODRule(DCM_SoftwareVersions, "1-n", "1", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
}

IODEnhGeneralEquipmentModule::~IODEnhGeneralEquipmentModule()
{
    // Nothing to do
}

OFString IODEnhGeneralEquipmentModule::getName() const
{
    return m_ModuleName;
}

OFCondition IODEnhGeneralEquipmentModule::getDeviceSerialNumber(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_DeviceSerialNumber, *m_Item, value, pos);
}

OFCondition IODEnhGeneralEquipmentModule::getManufacturer(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_Manufacturer, *m_Item, value, pos);
}

OFCondition IODEnhGeneralEquipmentModule::getManufacturerModelName(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_ManufacturerModelName, *m_Item, value, pos);
}

OFCondition IODEnhGeneralEquipmentModule::getSoftwareVersions(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_SoftwareVersions, *m_Item, value, pos);
}

OFCondition IODEnhGeneralEquipmentModule::setDeviceSerialNumber(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_DeviceSerialNumber, value);
    return result;
}

OFCondition IODEnhGeneralEquipmentModule::setManufacturer(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_Manufacturer, value);
    return result;
}

OFCondition IODEnhGeneralEquipmentModule::setManufacturerModelName(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_ManufacturerModelName, value);
    return result;
}

OFCondition IODEnhGeneralEquipmentModule::setSoftwareVersions(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_SoftwareVersions, value);
    return result;
}

OFCondition IODEnhGeneralEquipmentModule::set(const IODEnhGeneralEquipmentModule::EquipmentInfo& info)
{
    if (info.m_DeviceSerialNumber.empty() || info.m_Manufacturer.empty() || info.m_ManufacturerModelName.empty()
        || info.m_SoftwareVersions.empty())
        return IOD_EC_InvalidElementValue;

    OFCondition result = setManufacturer(info.m_Manufacturer);
    if (result.good())
        result = setManufacturerModelName(info.m_ManufacturerModelName);
    if (result.good())
        result = setDeviceSerialNumber(info.m_DeviceSerialNumber);
    if (result.good())
        result = setSoftwareVersions(info.m_SoftwareVersions);
    return result;
}
