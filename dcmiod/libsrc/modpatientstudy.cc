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
 *  Purpose: Class for managing the Patient Study Module
 *
 */


#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/modpatientstudy.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvras.h"
#include "dcmtk/dcmdata/dcvrds.h"

const OFString IODPatientStudyModule::m_ModuleName = "PatientStudyModule";

IODPatientStudyModule::IODPatientStudyModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules)
    : IODModule(item, rules)
{
    resetRules();
}

OFString IODPatientStudyModule::getName() const
{
    return m_ModuleName;
}

IODPatientStudyModule::IODPatientStudyModule()
    : IODModule()
{
    resetRules();
}

void IODPatientStudyModule::resetRules()
{
    // parameters are tag, VM, type. Overwrite old rules if any.
    m_Rules->addRule(new IODRule(DCM_AdmittingDiagnosesDescription, "1-n", "3", getName(), DcmIODTypes::IE_STUDY),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_PatientAge, "1", "3", getName(), DcmIODTypes::IE_STUDY), OFTrue);
    m_Rules->addRule(new IODRule(DCM_PatientSize, "1", "3", getName(), DcmIODTypes::IE_STUDY), OFTrue);
    m_Rules->addRule(new IODRule(DCM_PatientWeight, "1", "3", getName(), DcmIODTypes::IE_STUDY), OFTrue);
}

IODPatientStudyModule::~IODPatientStudyModule()
{
}

// --- get attributes (C++ string) ---

OFCondition IODPatientStudyModule::getAdmittingDiagnosesDescription(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_AdmittingDiagnosesDescription, *m_Item, value, pos);
}

OFCondition IODPatientStudyModule::getPatientAge(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_PatientAge, *m_Item, value, pos);
}

OFCondition IODPatientStudyModule::getPatientWeight(Float64& value, const unsigned long pos) const
{
    return DcmIODUtil::getFloat64ValueFromItem(DCM_PatientWeight, *m_Item, value, pos);
}

OFCondition IODPatientStudyModule::getPatientSize(Float64& value, const unsigned long pos) const
{
    return DcmIODUtil::getFloat64ValueFromItem(DCM_PatientSize, *m_Item, value, pos);
}

// --- set attributes ---

OFCondition IODPatientStudyModule::setAdmittingDiagnosesDescription(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_AdmittingDiagnosesDescription, value);
    return result;
}

OFCondition IODPatientStudyModule::setPatientAge(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmAgeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_PatientAge, value);
    return result;
}

OFCondition IODPatientStudyModule::setPatientSize(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_PatientSize, value);
    return result;
}

OFCondition IODPatientStudyModule::setPatientWeight(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_PatientWeight, value);
    return result;
}
