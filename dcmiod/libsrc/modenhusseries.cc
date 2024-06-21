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
 *  Purpose: Class for managing the Enhanced US Series Module
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/modenhusseries.h"
#include "dcmtk/dcmiod/iodutil.h"

const OFString IODEnhancedUSSeriesModule::m_ModuleName = "EnhancedUSSeries";

IODEnhancedUSSeriesModule::IODEnhancedUSSeriesModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules)
    : IODModule(item, rules)
    , m_ReferencedPerformedProcedureStep()
    , m_PerformedProtocolCode()
{
    // reset element rules
    resetRules();
}

IODEnhancedUSSeriesModule::IODEnhancedUSSeriesModule()
    : IODModule()
    , m_ReferencedPerformedProcedureStep()
    , m_PerformedProtocolCode()
{
    // reset element rules
    resetRules();
}

OFString IODEnhancedUSSeriesModule::getName() const
{
    return m_ModuleName;
}

void IODEnhancedUSSeriesModule::resetRules()
{
    // parameters are tag, VM, type. Overwrite old rules if any.
    m_Rules->addRule(new IODRule(DCM_Modality, "1", "1", getName(), DcmIODTypes::IE_SERIES, "US"), OFTrue);
    m_Rules->addRule(
        new IODRule(DCM_ReferencedPerformedProcedureStepSequence, "1", "1C", getName(), DcmIODTypes::IE_SERIES),
        OFTrue);
    m_Rules->addRule(new IODRule(DCM_PerformedProtocolCodeSequence, "1", "1C", getName(), DcmIODTypes::IE_SERIES),
                     OFTrue);
}

OFCondition IODEnhancedUSSeriesModule::read(DcmItem& source, const OFBool clearOldData)
{
    if (clearOldData)
        clearData();

    IODComponent::read(source, OFFalse /* data already cleared */);

    DcmIODUtil::readSingleItem<SOPInstanceReferenceMacro>(
        source,
        DCM_ReferencedPerformedProcedureStepSequence,
        m_ReferencedPerformedProcedureStep,
        m_Rules->getByTag(DCM_ReferencedPerformedProcedureStepSequence));
    DcmIODUtil::readSingleItem<CodeSequenceMacro>(source,
                                                  DCM_PerformedProtocolCodeSequence,
                                                  m_PerformedProtocolCode,
                                                  m_Rules->getByTag(DCM_PerformedProtocolCodeSequence));

    return EC_Normal;
}

OFCondition IODEnhancedUSSeriesModule::write(DcmItem& destination)
{
    OFCondition result = EC_Normal;

    result = IODComponent::write(destination);
    DcmIODUtil::writeSingleItem<SOPInstanceReferenceMacro>(
        result,
        DCM_ReferencedPerformedProcedureStepSequence,
        m_ReferencedPerformedProcedureStep,
        destination,
        m_Rules->getByTag(DCM_ReferencedPerformedProcedureStepSequence));
    DcmIODUtil::writeSingleItem<CodeSequenceMacro>(result,
                                                   DCM_PerformedProtocolCodeSequence,
                                                   m_PerformedProtocolCode,
                                                   destination,
                                                   m_Rules->getByTag(DCM_PerformedProtocolCodeSequence));

    return result;
}

IODEnhancedUSSeriesModule::~IODEnhancedUSSeriesModule()
{
}

OFCondition IODEnhancedUSSeriesModule::getModality(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_Modality, *m_Item, value, pos);
}

CodeSequenceMacro& IODEnhancedUSSeriesModule::getPerformedProtocolCode()
{
    return m_PerformedProtocolCode;
}

SOPInstanceReferenceMacro& IODEnhancedUSSeriesModule::getReferencedPPS()
{
    return m_ReferencedPerformedProcedureStep;
}

OFCondition IODEnhancedUSSeriesModule::getPerformedProtocolType(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_PerformedProtocolType, *m_Item, value, pos);
}
