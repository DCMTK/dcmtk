/*
 *
 *  Copyright (C) 2016, Open Connections GmbH
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
 *  Module: dcmpmap
 *
 *  Author: Jan Schlamelcher
 *
 *  Purpose: Class for managing the Parametric Map Series Module
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvris.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmpmap/dpmmodparametricmapseries.h"

const OFString DPMParametricMapSeriesModule::m_ModuleName = "DPMParametricMapSeriesModule";

DPMParametricMapSeriesModule::DPMParametricMapSeriesModule()
: IODModule()
{
  resetRules();
}

DPMParametricMapSeriesModule::DPMParametricMapSeriesModule(OFshared_ptr<DcmItem> item,
                                                           OFshared_ptr<IODRules> rules)
: IODModule(item, rules)
{
  // reset element rules
  resetRules();
}

DPMParametricMapSeriesModule::~DPMParametricMapSeriesModule()
{

}

OFString DPMParametricMapSeriesModule::getName() const
{
  return m_ModuleName;
}

void DPMParametricMapSeriesModule::resetRules()
{
  // parameters are tag, VM, type. Overwrite old rules if any.
  m_Rules->addRule(new IODRule(DCM_Modality, "1","1", getName(), DcmIODTypes::IE_SERIES), OFTrue);
  m_Rules->addRule(new IODRule(DCM_SeriesNumber, "1","1", getName(), DcmIODTypes::IE_SERIES), OFTrue);
  m_Rules->addRule(new IODRule(DCM_ReferencedPerformedProcedureStepSequence, "1","1C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
}

OFCondition DPMParametricMapSeriesModule::read(DcmItem& source,
                                               const OFBool clearOldData)
{
  if (clearOldData)
    clearData();

  IODComponent::read(source, OFFalse /* data already cleared */);
  DcmIODUtil::readSubSequence(source, DCM_ReferencedPerformedProcedureStepSequence, m_ReferencedPerformedProcedureStepSequence, m_Rules->getByTag(DCM_ReferencedPerformedProcedureStepSequence));

  return EC_Normal;
}

OFCondition DPMParametricMapSeriesModule::write(DcmItem& destination)
{
  OFCondition result = EC_Normal;

  if(SOPInstanceReferenceMacro* pReferencedPerformedProcedureStepSequence = getReferencedPerformedProcedureStepSequence())
    DcmIODUtil::writeSingleItem(result, DCM_ReferencedPerformedProcedureStepSequence, *pReferencedPerformedProcedureStepSequence, *m_Item, m_Rules->getByTag(DCM_ReferencedPerformedProcedureStepSequence));
  result = IODComponent::write(destination);

  return result;
}

OFCondition DPMParametricMapSeriesModule::getModality(OFString &value,
                                                      const signed long pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_Modality, *m_Item, value, pos);
}

OFCondition DPMParametricMapSeriesModule::getSeriesNumber(OFString &value,
                                                          const signed long pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_SeriesNumber, *m_Item, value, pos);
}

SOPInstanceReferenceMacro* DPMParametricMapSeriesModule::getReferencedPerformedProcedureStepSequence()
{
  return m_ReferencedPerformedProcedureStepSequence.empty() ? OFnullptr : &*m_ReferencedPerformedProcedureStepSequence.begin();
}

OFVector<SOPInstanceReferenceMacro>& DPMParametricMapSeriesModule::getEntireReferencedPerformedProcedureStepSequence()
{
  return m_ReferencedPerformedProcedureStepSequence;
}

OFCondition DPMParametricMapSeriesModule::setModality(const OFString &value,
                                                      const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_Modality, value);
  return result;
}

OFCondition DPMParametricMapSeriesModule::setSeriesNumber(const OFString &value,
                                                          const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_SeriesNumber, value);
  return result;
}
