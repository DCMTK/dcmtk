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
 *  Purpose: Class for managing the General Equipment Module
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/modequipment.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvrsh.h"
#include "dcmtk/dcmiod/iodutil.h"

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

    // parameters are tag, VM, type. Overwrite old rules if any.
    m_Rules->addRule(new IODRule(DCM_Manufacturer, "1", "2", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
    m_Rules->addRule(new IODRule(DCM_InstitutionName, "1", "3", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
    m_Rules->addRule(new IODRule(DCM_InstitutionAddress, "1", "3", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
    m_Rules->addRule(new IODRule(DCM_StationName, "1", "3", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
    m_Rules->addRule(new IODRule(DCM_InstitutionalDepartmentName, "1", "3", getName(), DcmIODTypes::IE_EQUIPMENT),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_ManufacturerModelName, "1", "3", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
    m_Rules->addRule(new IODRule(DCM_DeviceSerialNumber, "1", "3", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
    m_Rules->addRule(new IODRule(DCM_SoftwareVersions, "1-n", "3", getName(), DcmIODTypes::IE_EQUIPMENT), OFTrue);
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
    return DcmIODUtil::getStringValueFromItem(DCM_StationName, *m_Item, value, pos);
}

OFCondition IODGeneralEquipmentModule::getManufacturerModelName(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_ManufacturerModelName, *m_Item, value, pos);
}

OFCondition IODGeneralEquipmentModule::getSoftwareVersions(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_SoftwareVersions, *m_Item, value, pos);
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
