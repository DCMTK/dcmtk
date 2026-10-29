/*
 *
 *  Copyright (C) 2015-2026, Open Connections GmbH
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
 *  Purpose: Class for managing the General Equipment Module
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/modequipment.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvrsh.h"
#include "dcmtk/dcmiod/iodutil.h"

// Per-class default IODRules shared across instances (copy-on-write; see
// IODComponent). Must live at namespace scope, not inside a function: in
// C++98 the initialization of function-local statics is not thread-safe,
// and that applies to the OFMutex itself -- a mutex cannot guard its own
// construction. Namespace-scope statics are initialized before main() on
// the single startup thread, so both objects are guaranteed ready when
// user threads first try to lock.
namespace
{
OFshared_ptr<IODRules> s_defaultRules;
OFMutex s_defaultRulesMutex;
} // namespace

IODGeneralEquipmentModule::IODGeneralEquipmentModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules)
    : IODModule(item, rules)
    , m_ModuleName("GeneralEquipmentModule")
{
    // reset element rules
    resetRules();
}

IODGeneralEquipmentModule::IODGeneralEquipmentModule()
    : IODModule()
    , m_ModuleName("GeneralEquipmentModule")
{
    resetRules();
}

void IODGeneralEquipmentModule::resetRules()
{
    s_defaultRulesMutex.lock();
    if (!s_defaultRules)
    {
        s_defaultRules.reset(new IODRules());
        s_defaultRules->addRule(new IODRule(DCM_Manufacturer, "1", "2", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
        s_defaultRules->addRule(new IODRule(DCM_InstitutionName, "1", "3", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
        s_defaultRules->addRule(new IODRule(DCM_InstitutionAddress, "1", "3", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
        s_defaultRules->addRule(new IODRule(DCM_StationName, "1", "3", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
        s_defaultRules->addRule(new IODRule(DCM_InstitutionalDepartmentName, "1", "3", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
        s_defaultRules->addRule(new IODRule(DCM_ManufacturerModelName, "1", "3", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
        s_defaultRules->addRule(new IODRule(DCM_DeviceSerialNumber, "1", "3", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
        s_defaultRules->addRule(new IODRule(DCM_SoftwareVersions, "1-n", "3", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
        s_defaultRules->addRule(new IODRule(DCM_PixelPaddingValue, "1", "3", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
    }
    s_defaultRulesMutex.unlock();
    if (!m_ExternalRules)
    {
        m_Rules       = s_defaultRules;
        m_HasOwnRules = OFFalse;
    }
    else
    {
        IODRules::iterator it = s_defaultRules->begin();
        while (it != s_defaultRules->end())
        {
            m_Rules->addRule(it->second->clone(), OFTrue);
            ++it;
        }
    }
}

IODGeneralEquipmentModule::~IODGeneralEquipmentModule()
{
}

OFString IODGeneralEquipmentModule::getName() const
{
    return m_ModuleName;
}

OFCondition IODGeneralEquipmentModule::getDeviceSerialNumber(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_DeviceSerialNumber, *m_Item, value, pos);
}

OFCondition IODGeneralEquipmentModule::getManufacturer(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_Manufacturer, *m_Item, value, pos);
}

OFCondition IODGeneralEquipmentModule::getInstitutionName(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_InstitutionName, *m_Item, value, pos);
}

OFCondition IODGeneralEquipmentModule::getInstitutionAddress(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_InstitutionAddress, *m_Item, value, pos);
}

OFCondition IODGeneralEquipmentModule::getStationName(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_StationName, *m_Item, value, pos);
}

OFCondition IODGeneralEquipmentModule::getInstitutionalDepartmentName(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_InstitutionalDepartmentName, *m_Item, value, pos);
}

OFCondition IODGeneralEquipmentModule::getManufacturerModelName(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_ManufacturerModelName, *m_Item, value, pos);
}

OFCondition IODGeneralEquipmentModule::getSoftwareVersions(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_SoftwareVersions, *m_Item, value, pos);
}

OFCondition IODGeneralEquipmentModule::getPixelPaddingValue(Uint16& value, const unsigned long pos) const
{
    return m_Item->findAndGetUint16(DCM_PixelPaddingValue, value, pos);
}

OFCondition IODGeneralEquipmentModule::getPixelPaddingValue(Sint16& value, const unsigned long pos) const
{
    return m_Item->findAndGetSint16(DCM_PixelPaddingValue, value, pos);
}

OFCondition IODGeneralEquipmentModule::getPixelPaddingValueVR(DcmEVR& vr) const
{
    DcmElement* elem   = NULL;
    OFCondition result = m_Item->findAndGetElement(DCM_PixelPaddingValue, elem);
    if (result.good() && (elem != NULL))
        vr = elem->getVR();
    return result;
}

OFBool IODGeneralEquipmentModule::isPixelPaddingValueSigned() const
{
    DcmEVR vr = EVR_UN;
    return getPixelPaddingValueVR(vr).good() && (vr == EVR_SS);
}

IODGeneralEquipmentModule::EquipmentInfo IODGeneralEquipmentModule::getEquipmentInfo() const
{
    EquipmentInfo info;
    getManufacturer(info.m_Manufacturer);
    getManufacturerModelName(info.m_ManufacturerModelName);
    getDeviceSerialNumber(info.m_DeviceSerialNumber);
    getSoftwareVersions(info.m_SoftwareVersions);
    return info;
}

OFCondition IODGeneralEquipmentModule::setDeviceSerialNumber(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_DeviceSerialNumber, value);
    return result;
}

OFCondition IODGeneralEquipmentModule::setManufacturer(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_Manufacturer, value);
    return result;
}

OFCondition IODGeneralEquipmentModule::setInstitutionName(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_InstitutionName, value);
    return result;
}

OFCondition IODGeneralEquipmentModule::setInstitutionAddress(const OFString& value, const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertOFStringArray(DCM_InstitutionAddress, value);
}

OFCondition IODGeneralEquipmentModule::setStationName(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_StationName, value);
    return result;
}

OFCondition IODGeneralEquipmentModule::setInstutionalDepartmentName(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_InstitutionalDepartmentName, value);
    return result;
}

OFCondition IODGeneralEquipmentModule::setManufacturerModelName(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_ManufacturerModelName, value);
    return result;
}

OFCondition IODGeneralEquipmentModule::setSoftwareVersions(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_SoftwareVersions, value);
    return result;
}

OFCondition IODGeneralEquipmentModule::setPixelPaddingValue(const Uint16 value, const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertUint16(DCM_PixelPaddingValue, value);
}

OFCondition IODGeneralEquipmentModule::setPixelPaddingValue(const Sint16 value, const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertSint16(DCM_PixelPaddingValue, value);
}
