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
 *  Purpose: Class for managing the Synchronization Module
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvrsh.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmiod/modsynchronisation.h"

const OFString IODSynchronizationModule::m_ModuleName = "SynchronizationModule";

IODSynchronizationModule::IODSynchronizationModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules)
    : IODModule(item, rules)
{
    // reset element rules
    resetRules();
}

IODSynchronizationModule::IODSynchronizationModule()
    : IODModule()
{
    resetRules();
}

void IODSynchronizationModule::resetRules()
{
    // parameters are tag, VM, type. Overwrite old rules if any.
    m_Rules->addRule(new IODRule(DCM_SynchronizationFrameOfReferenceUID, "1", "1", getName(), DcmIODTypes::IE_FOR),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_SynchronizationTrigger, "1", "1", getName(), DcmIODTypes::IE_FOR), OFTrue);
    m_Rules->addRule(new IODRule(DCM_TriggerSourceOrType, "1", "3", getName(), DcmIODTypes::IE_FOR), OFTrue);
    m_Rules->addRule(new IODRule(DCM_SynchronizationChannel, "2", "1C", getName(), DcmIODTypes::IE_FOR), OFTrue);
    m_Rules->addRule(new IODRule(DCM_AcquisitionTimeSynchronized, "1", "1", getName(), DcmIODTypes::IE_FOR), OFTrue);
    m_Rules->addRule(new IODRule(DCM_TimeSource, "1", "3", getName(), DcmIODTypes::IE_FOR), OFTrue);
    m_Rules->addRule(new IODRule(DCM_TimeDistributionProtocol, "1", "3", getName(), DcmIODTypes::IE_FOR), OFTrue);
    m_Rules->addRule(new IODRule(DCM_NTPSourceAddress, "1", "3", getName(), DcmIODTypes::IE_FOR), OFTrue);
}

OFString IODSynchronizationModule::getName() const
{
    return m_ModuleName;
}

IODSynchronizationModule::~IODSynchronizationModule()
{
}

OFCondition IODSynchronizationModule::getSynchronizationFrameOfReferenceUID(OFString& value,
                                                                            const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_SynchronizationFrameOfReferenceUID, *m_Item, value, pos);
}

OFCondition IODSynchronizationModule::getSynchronizationTrigger(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_SynchronizationTrigger, *m_Item, value, pos);
}

OFCondition IODSynchronizationModule::getTriggerSourceOrType(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_TriggerSourceOrType, *m_Item, value, pos);
}

OFCondition IODSynchronizationModule::getSynchronizationChannel(OFVector<Uint16>& value) const
{
    DcmElement* elem   = NULL;
    OFCondition result = m_Item->findAndGetElement(DCM_SynchronizationChannel, elem);
    if (elem)
    {
        result = DcmIODUtil::getUint16ValuesFromElement(*elem, value);
    }
    return result;
}

OFCondition IODSynchronizationModule::getAcquisitionTimeSynchronized(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_AcquisitionTimeSynchronized, *m_Item, value, pos);
}

OFCondition IODSynchronizationModule::getTimeSource(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_TimeSource, *m_Item, value, pos);
}

OFCondition IODSynchronizationModule::getTimeDistributionProtocol(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_TimeDistributionProtocol, *m_Item, value, pos);
}

OFCondition IODSynchronizationModule::getNTPSourceAddress(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_NTPSourceAddress, *m_Item, value, pos);
}

OFCondition IODSynchronizationModule::setSynchronizationFrameofReferenceUID(const OFString& value,
                                                                            const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_SynchronizationFrameOfReferenceUID, value);
    return result;
}

OFCondition IODSynchronizationModule::setSynchronizationTrigger(const OFString& value, const OFBool checkValue)
{
    if (checkValue && !isValidSynchronizationTrigger(value))
    {
        DCMIOD_ERROR("Synchronization Trigger does not allow value " << value << " (enumerated values)");
        return IOD_EC_InvalidElementValue;
    }
    return m_Item->putAndInsertOFStringArray(DCM_SynchronizationTrigger, value);
}

OFCondition IODSynchronizationModule::setTriggerSourceOrType(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_TriggerSourceOrType, value);
    return result;
}

OFCondition IODSynchronizationModule::setSynchronizationChannel(const OFPair<Uint16, Uint16>& value,
                                                                const OFBool checkValue)
{
    (void)checkValue;
    OFCondition result = m_Item->putAndInsertUint16(DCM_SynchronizationChannel, value.first, 0);
    if (result.good())
    {
        result = m_Item->putAndInsertUint16(DCM_SynchronizationChannel, value.second, 0);
    }
    return result;
}

OFCondition IODSynchronizationModule::setAcquisitionTimeSynchronized(const OFString& value, const OFBool checkValue)
{
    if (checkValue && !isValidAcquisitionTimeSynchronized(value))
    {
        DCMIOD_ERROR("Acquisition Time Synchronized must only allows values 'Y' and 'N' (enumerated values)");
        return IOD_EC_InvalidElementValue;
    }
    return m_Item->putAndInsertOFStringArray(DCM_AcquisitionTimeSynchronized, value);
}

OFCondition IODSynchronizationModule::setTimeSource(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_TimeSource, value);
    return result;
}

OFCondition IODSynchronizationModule::setTimeDistributionProtocol(const OFString& value, const OFBool checkValue)
{
    if (checkValue && !isValidTimeDistributionProtocol(value))
    {
        DCMIOD_ERROR("Time Distribution Protocol does not allow value " << value << " (enumerated values)");
        return IOD_EC_InvalidElementValue;
    }
    return m_Item->putAndInsertOFStringArray(DCM_TimeDistributionProtocol, value);
}

OFCondition IODSynchronizationModule::setNTPSourceAddress(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_NTPSourceAddress, value);
    return result;
}

OFBool IODSynchronizationModule::isValidAcquisitionTimeSynchronized(const OFString& value)
{
    return ((value == "Y") || (value == "N"));
}

OFBool IODSynchronizationModule::isValidTimeDistributionProtocol(const OFString& value)
{
    return ((value == "NTP") || (value == "IRIG") || (value == "GPS") || (value == "SNTP") || (value == "PTP"));
}

OFBool IODSynchronizationModule::isValidSynchronizationTrigger(const OFString& value)
{
    return ((value == "SOURCE") || (value == "EXTERNAL") || (value == "PASSTHRU") || (value == "NO TRIGGER"));
}
