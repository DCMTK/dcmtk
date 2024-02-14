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
 *  Purpose: Class for managing the General Series Module
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/modgeneralseries.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvris.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvrpn.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmiod/iodutil.h"

const OFString IODGeneralSeriesModule::m_ModuleName = "GeneralSeriesModule";

IODGeneralSeriesModule::IODGeneralSeriesModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules)
    : IODModule(item, rules)
    , m_ReferencedPPS()
{
    // reset element rules
    resetRules();
}

IODGeneralSeriesModule::IODGeneralSeriesModule()
    : IODModule()
    , m_ReferencedPPS()
{
    resetRules();
}

OFString IODGeneralSeriesModule::getName() const
{
    return m_ModuleName;
}

void IODGeneralSeriesModule::inventMissing()
{
    // Series Instance UID
    ensureInstanceUID();

    // Default handler
    IODComponent::inventMissing();
}

void IODGeneralSeriesModule::resetRules()
{
    // parameters are tag, VM, type. Overwrite old rules if any.
    m_Rules->addRule(new IODRule(DCM_Modality, "1", "1", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_SeriesInstanceUID, "1", "1", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_SeriesNumber, "1", "2", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_Laterality, "1", "2C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_SeriesDate, "1", "3", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_SeriesTime, "1", "3", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_PerformingPhysicianName, "1", "3", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_ProtocolName, "1", "3", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_SeriesDescription, "1", "3", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_OperatorsName, "1-n", "3", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_BodyPartExamined, "1", "3", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_PatientPosition, "1", "2C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(
        new IODRule(DCM_ReferencedPerformedProcedureStepSequence, "1", "3", getName(), DcmIODTypes::IE_SERIES), OFTrue);
}

OFCondition IODGeneralSeriesModule::read(DcmItem& source, const OFBool clearOldData)
{
    if (clearOldData)
        clearData();

    IODComponent::read(source, OFFalse /* data already cleared */);
    DcmIODUtil::readSingleItem<SOPInstanceReferenceMacro>(
        source,
        DCM_ReferencedPerformedProcedureStepSequence,
        m_ReferencedPPS,
        m_Rules->getByTag(DCM_ReferencedPerformedProcedureStepSequence));

    return EC_Normal;
}

OFCondition IODGeneralSeriesModule::write(DcmItem& destination)
{
    OFCondition result = EC_Normal;

    result = IODComponent::write(destination);
    DcmIODUtil::writeSingleItem<SOPInstanceReferenceMacro>(
        result,
        DCM_ReferencedPerformedProcedureStepSequence,
        m_ReferencedPPS,
        destination,
        m_Rules->getByTag(DCM_ReferencedPerformedProcedureStepSequence));

    return result;
}

void IODGeneralSeriesModule::ensureInstanceUID(const OFBool correctInvalid)
{
    OFString uidstr;

    /* create new sop instance UID if required */
    if (getSeriesInstanceUID(uidstr).bad() || uidstr.empty())
    {
        setSeriesInstanceUID(DcmIODUtil::createUID(1 /* Series Level */));
    }
    else if (!uidstr.empty() && correctInvalid)
    {
        if (DcmUniqueIdentifier::checkStringValue(uidstr, "1").bad())
        {
            setSeriesInstanceUID(DcmIODUtil::createUID(1 /* Series Level */));
        }
    }
}

IODGeneralSeriesModule::~IODGeneralSeriesModule()
{
}

OFCondition IODGeneralSeriesModule::getModality(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_Modality, *m_Item, value, pos);
}

OFCondition IODGeneralSeriesModule::getSeriesInstanceUID(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_SeriesInstanceUID, *m_Item, value, pos);
}

OFCondition IODGeneralSeriesModule::getSeriesNumber(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_SeriesNumber, *m_Item, value, pos);
}

OFCondition IODGeneralSeriesModule::getLaterality(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_Laterality, *m_Item, value, pos);
}

OFCondition IODGeneralSeriesModule::getSeriesDate(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_SeriesDate, *m_Item, value, pos);
}

OFCondition IODGeneralSeriesModule::getSeriesTime(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_SeriesTime, *m_Item, value, pos);
}

OFCondition IODGeneralSeriesModule::getPerformingPhysicianName(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_PerformingPhysicianName, *m_Item, value, pos);
}

OFCondition IODGeneralSeriesModule::getProtocolName(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_ProtocolName, *m_Item, value, pos);
}

OFCondition IODGeneralSeriesModule::getSeriesDescription(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_SeriesDescription, *m_Item, value, pos);
}

OFCondition IODGeneralSeriesModule::getOperatorsName(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_OperatorsName, *m_Item, value, pos);
}

OFCondition IODGeneralSeriesModule::getBodyPartExamined(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_BodyPartExamined, *m_Item, value, pos);
}

OFCondition IODGeneralSeriesModule::getPatientPosition(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_PatientPosition, *m_Item, value, pos);
}

SOPInstanceReferenceMacro& IODGeneralSeriesModule::getReferencedPPS()
{
    return m_ReferencedPPS;
}

OFCondition IODGeneralSeriesModule::setModality(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_Modality, value);
    return result;
}

OFCondition IODGeneralSeriesModule::setSeriesInstanceUID(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_SeriesInstanceUID, value);
    return result;
}

OFCondition IODGeneralSeriesModule::setSeriesNumber(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_SeriesNumber, value);
    return result;
}

OFCondition IODGeneralSeriesModule::setLaterality(const DcmIODTypes::IOD_LATERALITY laterality, const OFBool checkValue)
{
    (void)checkValue;
    OFCondition result;
    switch (laterality)
    {
        case DcmIODTypes::LATERALITY_L:
            result = m_Item->putAndInsertOFStringArray(DCM_Laterality, "L");
            break;
        case DcmIODTypes::LATERALITY_R:
            result = m_Item->putAndInsertOFStringArray(DCM_Laterality, "R");
            break;
        case DcmIODTypes::LATERALITY_UNDEFINED:
        default:
            result = IOD_EC_InvalidLaterality;
    }
    return result;
}

OFCondition IODGeneralSeriesModule::setSeriesDate(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_SeriesDate, value);
    return result;
}

OFCondition IODGeneralSeriesModule::setSeriesTime(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_SeriesTime, value);
    return result;
}

OFCondition IODGeneralSeriesModule::setPerformingPhysicianName(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_PerformingPhysicianName, value);
    return result;
}

OFCondition IODGeneralSeriesModule::setProtocolName(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_ProtocolName, value);
    return result;
}

OFCondition IODGeneralSeriesModule::setSeriesDescription(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_SeriesDescription, value);
    return result;
}

OFCondition IODGeneralSeriesModule::setOperatorsName(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmPersonName::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_OperatorsName, value);
    return result;
}

OFCondition IODGeneralSeriesModule::setBodyPartExamined(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_BodyPartExamined, value);
    return result;
}

OFCondition IODGeneralSeriesModule::setPatientPosition(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_PatientPosition, value);
    return result;
}
