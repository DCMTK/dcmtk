/*
 *
 *  Copyright (C) 2015, Open Connections GmbH
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
 *  Purpose: Class for managing the Common Instance Reference Module
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/modcommoninstanceref.h"
#include "dcmtk/dcmiod/iodutil.h" // for static helpers

const OFString IODCommonInstanceReferenceModule::m_ComponentName = "CommonInstanceReferenceModule";
const OFString IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::m_ComponentName = "StudiesContainingOtherReferencedInstancesSequence";


IODCommonInstanceReferenceModule::IODCommonInstanceReferenceModule(OFshared_ptr< DcmItem > item,
                                                                   OFshared_ptr< IODRules > rules)
: IODModule(item, rules),
  m_StudiesContainingOtherReferencedInstancesSequence()
{
  resetRules();
}


IODCommonInstanceReferenceModule::IODCommonInstanceReferenceModule()
: IODModule()
{
  resetRules();
}


IODCommonInstanceReferenceModule::~IODCommonInstanceReferenceModule()
{
  clearData();
}


OFString IODCommonInstanceReferenceModule::getName() const
{
  return m_ComponentName;
}


OFVector< IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem* >& IODCommonInstanceReferenceModule::getReferencedSeriesItems()
{
  return m_ReferenceSeriesItems;
}


void IODCommonInstanceReferenceModule::clearData()
{
  DcmIODUtil::freeContainer(m_StudiesContainingOtherReferencedInstancesSequence);
  DcmIODUtil::freeContainer(m_ReferenceSeriesItems);
}


OFCondition IODCommonInstanceReferenceModule::read(DcmItem& source,
                                                   const OFBool clearOldData)
{
  if (clearOldData)
    clearData();

  DcmIODUtil::readSubSequence<OFVector<IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem*> >(source, DCM_ReferencedSeriesSequence, m_ReferenceSeriesItems, m_Rules->getByTag(DCM_ReferencedSeriesSequence));
  DcmIODUtil::readSubSequence<OFVector<StudiesOtherInstancesItem*> >(source,
                                                                     DCM_StudiesContainingOtherReferencedInstancesSequence,
                                                                     m_StudiesContainingOtherReferencedInstancesSequence,
                                                                     m_Rules->getByTag(DCM_StudiesContainingOtherReferencedInstancesSequence));

  return EC_Normal;
}


OFCondition IODCommonInstanceReferenceModule::write(DcmItem& destination)
{
  OFCondition result = EC_Normal;

  DcmIODUtil::writeSubSequence<OFVector<IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem*> >(result, DCM_ReferencedSeriesSequence, m_ReferenceSeriesItems, destination, m_Rules->getByTag(DCM_ReferencedSeriesSequence));
  DcmIODUtil::writeSubSequence<OFVector<StudiesOtherInstancesItem*> >(result,
                                                                      DCM_StudiesContainingOtherReferencedInstancesSequence,
                                                                      m_StudiesContainingOtherReferencedInstancesSequence,
                                                                      *m_Item,
                                                                      m_Rules->getByTag(DCM_StudiesContainingOtherReferencedInstancesSequence));
  return result;
}


void IODCommonInstanceReferenceModule::resetRules()
{
  // Parameters for Rule are tag, VM, type (1,1C,2,2C,3), module name and logical IOD level
  m_Rules->addRule(new IODRule(DCM_ReferencedSeriesSequence, "1-n", "1C", m_ComponentName, DcmIODTypes::IE_INSTANCE), OFTrue);
  m_Rules->addRule(new IODRule(DCM_StudiesContainingOtherReferencedInstancesSequence, "1-n", "1C", m_ComponentName, DcmIODTypes::IE_INSTANCE), OFTrue);
}


// -- IODCommonInstanceReferenceModule::StudiesOtherInstancesItem --


IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::StudiesOtherInstancesItem(OFshared_ptr< DcmItem > item,
                                                                                       OFshared_ptr< IODRules > rules,
                                                                                       IODComponent* parent)
: IODComponent(item, rules, parent),
  m_ReferencedSeriesAndInstance()
{
  // reset element rules
  resetRules();
}


IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::StudiesOtherInstancesItem(IODComponent* parent)
: IODComponent(parent),
  m_ReferencedSeriesAndInstance()
{
  // reset element rules
  resetRules();
}


IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::~StudiesOtherInstancesItem()
{
  clearData();
}



OFString IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::getName() const
{
  return m_ComponentName;
}


void IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::clearData()
{
  m_ReferencedSeriesAndInstance.clearData();
  IODComponent::clearData();
}


OFCondition IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::read(DcmItem& source,
                                                                              const OFBool clearOldData)
{
  if (clearOldData)
    clearData();

  IODComponent::read(source, clearOldData);
  m_ReferencedSeriesAndInstance.read(source, clearOldData);
  return EC_Normal;
}


OFCondition IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::write(DcmItem& destination)
{
  OFCondition result = EC_Normal;

  result = IODComponent::write(destination);

  if (result.good()) result = m_ReferencedSeriesAndInstance.write(destination);

  return result;
}


void IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::resetRules()
{
  // Parameters for Rule are tag, VM, type (1,1C,2,2C,3), module name and logical IOD level
  m_Rules->addRule(new IODRule(DCM_StudyInstanceUID, "1", "1", m_ComponentName, DcmIODTypes::IE_INSTANCE), OFTrue);
}


IODSeriesAndInstanceReferenceMacro& IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::getReferencedSeriesAndInstanceReferences()
{
  return m_ReferencedSeriesAndInstance;
}


OFCondition IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::getStudyInstanceUID(OFString& value,
                                                                                             const long signed int pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_StudyInstanceUID, *m_Item, value, pos);
}


OFCondition IODCommonInstanceReferenceModule::StudiesOtherInstancesItem::setStudyInstanceUID(const OFString& value,
                                                                                             const bool checkValue)
{
  OFCondition result = (checkValue) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_StudyInstanceUID, value);
  return result;
}
