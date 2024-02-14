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
 *  Purpose: Class for managing the General Study Module
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/modgeneralstudy.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvrpn.h"
#include "dcmtk/dcmdata/dcvrsh.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmiod/iodutil.h"

const OFString IODGeneralStudyModule::m_ModuleName = "GeneralStudyModule";

IODGeneralStudyModule::IODGeneralStudyModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules)
    : IODModule(item, rules)
    , m_IssuerOfAccessionNumberSequence()
    , m_ProcedureCodeSequence()
    , m_ReasonForPerformedProcedureCodeSequence()
{
    // reset element rules
    resetRules();
}

IODGeneralStudyModule::IODGeneralStudyModule()
    : IODModule()
    , m_IssuerOfAccessionNumberSequence()
    , m_ProcedureCodeSequence()
    , m_ReasonForPerformedProcedureCodeSequence()
{
    resetRules();
}

void IODGeneralStudyModule::inventMissing()
{
    // Study Instance UID
    ensureInstanceUID();

    IODComponent::inventMissing();
}

void IODGeneralStudyModule::resetRules()
{
    // parameters are tag, VM, type. Overwrite old rules if any.
    m_Rules->addRule(new IODRule(DCM_StudyInstanceUID, "1", "1", getName(), DcmIODTypes::IE_STUDY), OFTrue);
    m_Rules->addRule(new IODRule(DCM_StudyDate, "1", "2", getName(), DcmIODTypes::IE_STUDY), OFTrue);
    m_Rules->addRule(new IODRule(DCM_StudyTime, "1", "2", getName(), DcmIODTypes::IE_STUDY), OFTrue);
    m_Rules->addRule(new IODRule(DCM_ReferringPhysicianName, "1", "2", getName(), DcmIODTypes::IE_STUDY), OFTrue);
    m_Rules->addRule(new IODRule(DCM_StudyID, "1", "2", getName(), DcmIODTypes::IE_STUDY), OFTrue);
    m_Rules->addRule(new IODRule(DCM_AccessionNumber, "1", "2", getName(), DcmIODTypes::IE_STUDY), OFTrue);
    m_Rules->addRule(new IODRule(DCM_StudyDescription, "1", "3", getName(), DcmIODTypes::IE_STUDY), OFTrue);
    m_Rules->addRule(new IODRule(DCM_IssuerOfAccessionNumberSequence, "1", "3", getName(), DcmIODTypes::IE_STUDY),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_ProcedureCodeSequence, "1-n", "3", getName(), DcmIODTypes::IE_STUDY), OFTrue);
    m_Rules->addRule(
        new IODRule(DCM_ReasonForPerformedProcedureCodeSequence, "1-n", "3", getName(), DcmIODTypes::IE_STUDY), OFTrue);
}

OFString IODGeneralStudyModule::getName() const
{
    return m_ModuleName;
}

void IODGeneralStudyModule::clearData()
{
    m_IssuerOfAccessionNumberSequence.clearData();
    DcmIODUtil::freeContainer(m_ProcedureCodeSequence);
    DcmIODUtil::freeContainer(m_ReasonForPerformedProcedureCodeSequence);
}

OFCondition IODGeneralStudyModule::read(DcmItem& source, const OFBool clearOldData)
{
    if (clearOldData)
    {
        clearData();
    }

    IODComponent::read(source, OFFalse /* data already cleared above */);

    DcmIODUtil::readSingleItem(source,
                               DCM_IssuerOfAccessionNumberSequence,
                               m_IssuerOfAccessionNumberSequence,
                               m_Rules->getByTag(DCM_IssuerOfAccessionNumberSequence));
    DcmIODUtil::readSubSequence(
        source, DCM_ProcedureCodeSequence, m_ProcedureCodeSequence, m_Rules->getByTag(DCM_ProcedureCodeSequence));
    DcmIODUtil::readSubSequence(source,
                                DCM_ReasonForPerformedProcedureCodeSequence,
                                m_ReasonForPerformedProcedureCodeSequence,
                                m_Rules->getByTag(DCM_ReasonForPerformedProcedureCodeSequence));

    return EC_Normal;
}

OFCondition IODGeneralStudyModule::write(DcmItem& destination)
{
    OFCondition result;
    DcmIODUtil::writeSingleItem(result,
                                DCM_IssuerOfAccessionNumberSequence,
                                m_IssuerOfAccessionNumberSequence,
                                *m_Item,
                                m_Rules->getByTag(DCM_IssuerOfAccessionNumberSequence));
    DcmIODUtil::writeSubSequence(result,
                                 DCM_ProcedureCodeSequence,
                                 m_ProcedureCodeSequence,
                                 *m_Item,
                                 m_Rules->getByTag(DCM_ProcedureCodeSequence));
    DcmIODUtil::writeSubSequence(result,
                                 DCM_ReasonForPerformedProcedureCodeSequence,
                                 m_ReasonForPerformedProcedureCodeSequence,
                                 *m_Item,
                                 m_Rules->getByTag(DCM_ReasonForPerformedProcedureCodeSequence));

    if (result.good())
        result = IODComponent::write(destination);

    return result;
}

IODGeneralStudyModule::~IODGeneralStudyModule()
{
    DcmIODUtil::freeContainer(m_ProcedureCodeSequence);
    DcmIODUtil::freeContainer(m_ReasonForPerformedProcedureCodeSequence);
}

void IODGeneralStudyModule::ensureInstanceUID(const OFBool correctInvalid)
{
    OFString uidstr;

    /* create new sop instance UID if required */
    if (getStudyInstanceUID(uidstr).bad() || uidstr.empty())
    {
        setStudyInstanceUID(DcmIODUtil::createUID(2 /* Study Level */));
    }
    else if (!uidstr.empty() && correctInvalid)
    {
        if (DcmUniqueIdentifier::checkStringValue(uidstr, "1").bad())
        {
            setStudyInstanceUID(DcmIODUtil::createUID(2 /* Study Level */));
        }
    }
}

// --- get attributes (C++ string) ---

OFCondition IODGeneralStudyModule::getStudyInstanceUID(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_StudyInstanceUID, *m_Item, value, pos);
}

OFCondition IODGeneralStudyModule::getReferringPhysicianName(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_ReferringPhysicianName, *m_Item, value, pos);
}

OFCondition IODGeneralStudyModule::getStudyDescription(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_StudyDescription, *m_Item, value, pos);
}

OFCondition IODGeneralStudyModule::getStudyDate(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_StudyDate, *m_Item, value, pos);
}

OFCondition IODGeneralStudyModule::getStudyTime(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_StudyTime, *m_Item, value, pos);
}

OFCondition IODGeneralStudyModule::getStudyID(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_StudyID, *m_Item, value, pos);
}

OFCondition IODGeneralStudyModule::getAccessionNumber(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_AccessionNumber, *m_Item, value, pos);
}

HL7HierarchicDesignatorMacro& IODGeneralStudyModule::getIssuerOfAccessionNumber()
{
    return m_IssuerOfAccessionNumberSequence;
}

OFVector<CodeSequenceMacro*>& IODGeneralStudyModule::getProcedureCodeSequence()
{
    return m_ProcedureCodeSequence;
}

OFVector<CodeSequenceMacro*>& IODGeneralStudyModule::getReasonForPerformedProcedureCodeSequence()
{
    return m_ReasonForPerformedProcedureCodeSequence;
}

// --- set attributes ---

OFCondition IODGeneralStudyModule::setReferringPhysicianName(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_ReferringPhysicianName, value);
    return result;
}

OFCondition IODGeneralStudyModule::setStudyDescription(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_StudyDescription, value);
    return result;
}

OFCondition IODGeneralStudyModule::setStudyInstanceUID(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_StudyInstanceUID, value);
    return result;
}

OFCondition IODGeneralStudyModule::setStudyDate(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_StudyDate, value);
    return result;
}

OFCondition IODGeneralStudyModule::setStudyTime(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_StudyTime, value);
    return result;
}

OFCondition IODGeneralStudyModule::setStudyID(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_StudyID, value);
    return result;
}

OFCondition IODGeneralStudyModule::setAccessionNumber(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_AccessionNumber, value);
    return result;
}
