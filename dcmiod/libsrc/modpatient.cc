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
 *  Purpose: Class for managing the Patient Module
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmiod/modpatient.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmdata/dcvrpn.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrcs.h"

const OFString IODPatientModule::m_ModuleName = "PatientModule";

IODPatientModule::IODPatientModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules)
    : IODModule(item, rules)
{
    // reset element rules
    resetRules();
}

OFString IODPatientModule::getName() const
{
    return m_ModuleName;
}

IODPatientModule::IODPatientModule()
    : IODModule()
{
    resetRules();
}

void IODPatientModule::resetRules()
{
    // parameters are tag, VM, type. Overwrite old rules if any.
    m_Rules->addRule(new IODRule(DCM_PatientName, "1", "2", getName(), DcmIODTypes::IE_PATIENT), OFTrue);
    m_Rules->addRule(new IODRule(DCM_PatientID, "1", "2", getName(), DcmIODTypes::IE_PATIENT), OFTrue);
    m_Rules->addRule(new IODRule(DCM_PatientBirthDate, "1", "2", getName(), DcmIODTypes::IE_PATIENT), OFTrue);
    m_Rules->addRule(new IODRule(DCM_PatientSex, "1", "2", getName(), DcmIODTypes::IE_PATIENT), OFTrue);
}

IODPatientModule::~IODPatientModule()
{
}

// --- get attributes (C++ string) ---

OFCondition IODPatientModule::getPatientName(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_PatientName, *m_Item, value, pos);
}

OFCondition IODPatientModule::getPatientBirthDate(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_PatientBirthDate, *m_Item, value, pos);
}

OFCondition IODPatientModule::getPatientSex(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_PatientSex, *m_Item, value, pos);
}

OFCondition IODPatientModule::getPatientID(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_PatientID, *m_Item, value, pos);
}

// --- set attributes ---

OFCondition IODPatientModule::setPatientName(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_PatientName, value);
    return result;
}

OFCondition IODPatientModule::setPatientID(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_PatientID, value);
    return result;
}

OFCondition IODPatientModule::setPatientBirthDate(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_PatientBirthDate, value);
    return result;
}

OFCondition IODPatientModule::setPatientSex(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_PatientSex, value);
    return result;
}
