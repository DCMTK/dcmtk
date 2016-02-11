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
 *  Module: dcmiod
 *
 *  Author: Michael Onken
 *
 *  Purpose: Collection of classes representing DICOM IOD macros
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/dcmiod/iodutil.h" // for static IOD helpers


// --------------------------- Code Sequence Macro ---------------------------


// -- Code Sequence Macro

CodeSequenceMacro::CodeSequenceMacro(OFshared_ptr<DcmItem> item,
                                     OFshared_ptr<IODRules> rules,
                                     IODComponent* parent)
  : IODComponent(item, rules, parent)
{
  // reset element rules
  resetRules();
}


CodeSequenceMacro::CodeSequenceMacro(IODComponent* parent) : IODComponent(parent)
{
  // reset element rules
  resetRules();
}


CodeSequenceMacro::~CodeSequenceMacro()
{
  // nothing to do
}


CodeSequenceMacro::CodeSequenceMacro(const OFString& codeValue,
                                     const OFString& codingSchemeDesignator,
                                     const OFString& codeMeaning,
                                     const OFString& codingSchemeVersion,
                                     IODComponent* parent)
: IODComponent(parent)
{
  // reset element rules
  resetRules();
  setCodeValue(codeValue);
  setCodeMeaning(codeMeaning);
  setCodingSchemeDesignator(codingSchemeDesignator);
  if (!codingSchemeVersion.empty())
  {
    setCodingSchemeVersion(codingSchemeVersion);
  }
}

CodeSequenceMacro::CodeSequenceMacro(OFshared_ptr< DcmItem > item,
                                     OFshared_ptr< IODRules > rules,
                                     IODComponent* parent,
                                     const OFString& codeValue,
                                     const OFString& codingSchemeDesignator,
                                     const OFString& codeMeaning,
                                     const OFString& codingSchemeVersion)
: IODComponent(item, rules, parent)
{
  // reset element rules
  resetRules();
  setCodeValue(codeValue);
  setCodeMeaning(codeMeaning);
  setCodingSchemeDesignator(codingSchemeDesignator);
  if (!codingSchemeVersion.empty())
  {
    setCodingSchemeVersion(codingSchemeVersion);
  }
}


OFString CodeSequenceMacro::getName() const
{
  return "CodeSequenceMacro";
}


void CodeSequenceMacro::resetRules()
{
  m_Rules->addRule(new IODRule(DCM_CodeValue, "1", "1", "CodeSequenceMacro", DcmIODTypes::IE_UNDEFINED), OFTrue /*overwrite old rule*/);
  m_Rules->addRule(new IODRule(DCM_CodingSchemeDesignator, "1", "1", "CodeSequenceMacro", DcmIODTypes::IE_UNDEFINED), OFTrue /*overwrite old rule*/);
  m_Rules->addRule(new IODRule(DCM_CodingSchemeVersion, "1", "1C", "CodeSequenceMacro", DcmIODTypes::IE_UNDEFINED), OFTrue /*overwrite old rule*/);
  m_Rules->addRule(new IODRule(DCM_CodeMeaning, "1", "1", "CodeSequenceMacro", DcmIODTypes::IE_UNDEFINED), OFTrue /*overwrite old rule*/);
}


// -- get dicom attributes --

OFCondition CodeSequenceMacro::getCodeValue(OFString &value,
                                            const signed long pos)
{
  return DcmIODUtil::getStringValueFromItem(DCM_CodeValue, *m_Item, value, pos);
}


OFCondition CodeSequenceMacro::getCodingSchemeDesignator(OFString &value,
                                                         const signed long pos)
{
  return DcmIODUtil::getStringValueFromItem(DCM_CodingSchemeDesignator, *m_Item, value, pos);
}


OFCondition CodeSequenceMacro::getCodingSchemeVersion(OFString &value,
                                                      const signed long pos)
{
  return DcmIODUtil::getStringValueFromItem(DCM_CodingSchemeVersion, *m_Item, value, pos);

}


OFCondition CodeSequenceMacro::getCodeMeaning(OFString &value,
                                              const signed long pos)
{
  return DcmIODUtil::getStringValueFromItem(DCM_CodeMeaning, *m_Item, value, pos);
}


// -- set dicom attributes --

OFCondition CodeSequenceMacro::setCodeValue(const OFString &value,
                                            const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_CodeValue, value);
  return result;
}

OFCondition CodeSequenceMacro::setCodingSchemeDesignator(const OFString &value,
                                                         const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_CodingSchemeDesignator, value);
  return result;
}

OFCondition CodeSequenceMacro::setCodingSchemeVersion(const OFString &value,
                                                      const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmShortString::checkStringValue(value, "1C") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_CodingSchemeVersion, value);
  return result;
}

OFCondition CodeSequenceMacro::setCodeMeaning(const OFString &value,
                                              const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_CodeMeaning, value);
  return result;
}


OFCondition CodeSequenceMacro::set(const OFString& value,
                                   const OFString& scheme,
                                   const OFString& meaning,
                                   const OFString& schemeVersion,
                                   const OFBool checkValue)
{
  if (checkValue)
  {
    if (value.empty() || scheme.empty() || meaning.empty() )
    {
      DCMIOD_ERROR("Could not set code since Code Value, Coding Scheme Designator and Code Meaning must have non-empty values");
      return IOD_EC_InvalidElementValue;
    }
  }
  OFCondition result = setCodeValue(value, checkValue);
  if (result.good()) result = setCodingSchemeDesignator(scheme, checkValue);
  if (result.good()) result = setCodeMeaning(meaning, checkValue);
  if (result.good() && !schemeVersion.empty()) result = setCodingSchemeVersion(schemeVersion, checkValue);
  return result;
}


// ---------------------- SeriesAndInstanceReferenceMacro----------------------

const OFString IODSeriesAndInstanceReferenceMacro::m_ComponentName = "SeriesAndInstanceReferenceMacro";
const OFString IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem::m_ComponentName = "SeriesAndInstanceReferenceMacro";

IODSeriesAndInstanceReferenceMacro::IODSeriesAndInstanceReferenceMacro(OFshared_ptr< DcmItem > data,
                                                                       OFshared_ptr< IODRules > rules,
                                                                       IODComponent* parent)
: IODComponent(data, rules, parent)
{
  // reset element rules
  resetRules();
}


IODSeriesAndInstanceReferenceMacro::IODSeriesAndInstanceReferenceMacro(IODComponent* parent)
: IODComponent(parent)
{
  // reset element rules
  resetRules();
}


OFString IODSeriesAndInstanceReferenceMacro::getName() const
{
  return m_ComponentName;
}


OFCondition IODSeriesAndInstanceReferenceMacro::read(DcmItem& source,
                                                     const OFBool clearOldData)
{
  if (clearOldData)
    clearData();

  DcmIODUtil::readSubSequence<OFVector<ReferencedSeriesItem*> >(source, DCM_ReferencedSeriesSequence, m_ReferencedSeriesItems, m_Rules->getByTag(DCM_ReferencedSeriesSequence));
  return EC_Normal;
}


OFCondition IODSeriesAndInstanceReferenceMacro::write(DcmItem& destination)
{
  OFCondition result = EC_Normal;

  DcmIODUtil::writeSubSequence<OFVector<ReferencedSeriesItem*> >(result, DCM_ReferencedSeriesSequence, m_ReferencedSeriesItems, destination, m_Rules->getByTag(DCM_ReferencedSeriesSequence));

  return result;
}


void IODSeriesAndInstanceReferenceMacro::clearData()
{
  DcmIODUtil::freeContainer(m_ReferencedSeriesItems);
}


void IODSeriesAndInstanceReferenceMacro::resetRules()
{
  m_Rules->addRule(new IODRule(DCM_ReferencedSeriesSequence, "1-n", "1", m_ComponentName, DcmIODTypes::IE_INSTANCE), OFTrue);
}


OFVector< IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem* >& IODSeriesAndInstanceReferenceMacro::getReferencedSeriesItems()
{
  return m_ReferencedSeriesItems;
}


IODSeriesAndInstanceReferenceMacro::~IODSeriesAndInstanceReferenceMacro()
{
  DcmIODUtil::freeContainer(m_ReferencedSeriesItems);
}


IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem::ReferencedSeriesItem(OFshared_ptr< DcmItem > item,
                                                                              OFshared_ptr< IODRules > rules,
                                                                              IODComponent* parent)
: IODComponent(item, rules, parent),
  m_ReferencedInstanceSequence()
{
  // reset element rules
  resetRules();
}


IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem::ReferencedSeriesItem(IODComponent* parent)
: IODComponent(parent),
  m_ReferencedInstanceSequence()
{
  // reset element rules
  resetRules();
}


IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem::~ReferencedSeriesItem()
{
  clearData();
}


OFString IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem::getName() const
{
  return m_ComponentName;
}


void IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem::clearData()
{
  DcmIODUtil::freeContainer(m_ReferencedInstanceSequence);
  IODComponent::clearData();
}



OFCondition IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem::read(DcmItem& source,
                                                                           const OFBool clearOldData)
{
  if (clearOldData)
    clearData();

  IODComponent::read(source, clearOldData);
  DcmIODUtil::readSubSequence(source, DCM_ReferencedInstanceSequence, m_ReferencedInstanceSequence, m_Rules->getByTag(DCM_ReferencedInstanceSequence));
  return EC_Normal;
}


OFCondition IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem::write(DcmItem& destination)
{
  OFCondition result = EC_Normal;

  DcmIODUtil::writeSubSequence<OFVector<SOPInstanceReferenceMacro*> >(result, DCM_ReferencedInstanceSequence, m_ReferencedInstanceSequence, *m_Item, m_Rules->getByTag(DCM_ReferencedInstanceSequence));
  result = IODComponent::write(destination);

  return result;
}


void IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem::resetRules()
{
  // Parameters for Rule are tag, VM, type (1,1C,2,2C,3), module name and logical IOD level
  m_Rules->addRule(new IODRule(DCM_SeriesInstanceUID, "1", "1", m_ComponentName, DcmIODTypes::IE_INSTANCE), OFTrue);
  m_Rules->addRule(new IODRule(DCM_ReferencedInstanceSequence, "1-n", "1", m_ComponentName, DcmIODTypes::IE_INSTANCE), OFTrue);
}


OFVector< SOPInstanceReferenceMacro* >& IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem::getReferencedInstanceItems()
{
  return m_ReferencedInstanceSequence;
}


OFCondition IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem::getSeriesInstanceUID(OFString& value,
                                                                                         const long signed int pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_SeriesInstanceUID, *m_Item, value, pos);
}


OFCondition IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem::setSeriesInstanceUID(const OFString& value,
                                                                                           const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_SeriesInstanceUID, value);
  return result;
}



// ---------------------- SOPInstanceReferenceMacro ----------------------


SOPInstanceReferenceMacro::SOPInstanceReferenceMacro(OFshared_ptr< DcmItem > item,
                                                     OFshared_ptr< IODRules > rules,
                                                     IODComponent* parent)
: IODComponent(item, rules, parent)
{
  // reset element rules
  resetRules();
}


SOPInstanceReferenceMacro::SOPInstanceReferenceMacro(IODComponent* parent)
: IODComponent(parent)
{
  // reset element rules
  resetRules();
}


SOPInstanceReferenceMacro::~SOPInstanceReferenceMacro()
{
  // nothing to do
}


OFString SOPInstanceReferenceMacro::getName() const
{
  return "SOPInstanceReferenceMacro";
}


void SOPInstanceReferenceMacro::resetRules()
{
  // Parameters for Rule are tag, VM, type (1,1C,2,2C,3), module name and logical IOD level
  m_Rules->addRule(new IODRule(DCM_ReferencedSOPClassUID, "1", "1", "SOPInstanceReferenceMacro", DcmIODTypes::IE_UNDEFINED), OFTrue);
  m_Rules->addRule(new IODRule(DCM_ReferencedSOPInstanceUID, "1", "1",  "SOPInstanceReferenceMacro", DcmIODTypes::IE_UNDEFINED), OFTrue);
}


// -- get dicom attributes --

OFCondition SOPInstanceReferenceMacro::getReferencedSOPClassUID(OFString &value,
                                                                const signed long pos)
{
  return DcmIODUtil::getStringValueFromItem(DCM_ReferencedSOPClassUID, *m_Item, value, pos);
}

OFCondition SOPInstanceReferenceMacro::getReferencedSOPInstanceUID(OFString &value,
                                                                   const signed long pos)
{
  return DcmIODUtil::getStringValueFromItem(DCM_ReferencedSOPInstanceUID, *m_Item, value, pos);
}

// -- set dicom attributes --

OFCondition SOPInstanceReferenceMacro::setReferencedSOPClassUID(const OFString& value,
                                                                const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_ReferencedSOPClassUID, value);
  return result;
}


OFCondition SOPInstanceReferenceMacro::setReferencedSOPInstanceUID(const OFString& value,
                                                                   const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_ReferencedSOPInstanceUID, value);
  return result;
}


// ---------------------- ImageSOPInstanceReferenceMacro ----------------------


ImageSOPInstanceReferenceMacro::ImageSOPInstanceReferenceMacro()
  : ReferencedFrameNumber(DCM_ReferencedFrameNumber),
    ReferencedSegmentNumber(DCM_ReferencedSegmentNumber)
{

}


ImageSOPInstanceReferenceMacro::~ImageSOPInstanceReferenceMacro()
{
}

int ImageSOPInstanceReferenceMacro::compare(const IODComponent& rhs) const
{
  ImageSOPInstanceReferenceMacro *macro = OFstatic_cast(ImageSOPInstanceReferenceMacro*, OFconst_cast(IODComponent*, &rhs) );
  if (macro == NULL) return -1;
  int result = ReferencedFrameNumber.compare(macro->ReferencedFrameNumber);
  if (result == 0) ReferencedSegmentNumber.compare(macro->ReferencedSegmentNumber);
  if (result == 0) return SOPInstanceReferenceMacro::compare(rhs);
  return result;
}

void ImageSOPInstanceReferenceMacro::clear()
{
  SOPInstanceReferenceMacro::clearData();
  ReferencedFrameNumber.clear();
  ReferencedSegmentNumber.clear();
}


OFCondition ImageSOPInstanceReferenceMacro::read(DcmItem& source,
                                                 const OFBool clearOldData)
{
  /* re-initialize object */
  if (clearOldData)
    clear();

  OFCondition result = SOPInstanceReferenceMacro::read(source, clearOldData);

  DcmIODUtil::getAndCheckElementFromDataset(source, ReferencedFrameNumber, "1-n", "1C", "ImageSOPInstanceReferenceMacro");
  DcmIODUtil::getAndCheckElementFromDataset(source, ReferencedSegmentNumber, "1-n", "1C", "ImageSOPInstanceReferenceMacro");

  return result;
}


OFCondition ImageSOPInstanceReferenceMacro::write(DcmItem& item)
{
  OFCondition result = EC_Normal;
  /* copy all elements to dataset */
  DcmIODUtil::copyElementToDataset(result, item, ReferencedFrameNumber, "1-n" /* VM */, "1C" /* Type */, "ImageSOPInstanceReferenceMacro");
  DcmIODUtil::copyElementToDataset(result, item, ReferencedSegmentNumber, "1-n", "1C", "ImageSOPInstanceReferenceMacro");

  if ( result.good() )
    result = SOPInstanceReferenceMacro::write(item);

  return result;
}

// -- get dicom attributes --

OFCondition ImageSOPInstanceReferenceMacro::getReferencedFrameNumber(OFVector<Uint16> &values)
{
  // cast away const since underlying dcmdata routine is not const...
  DcmIntegerString *is = OFconst_cast(DcmIntegerString*, &ReferencedFrameNumber);
  return DcmIODUtil::getUint16ValuesFromElement(*is, values);
}


OFCondition ImageSOPInstanceReferenceMacro::getReferencedSegmentNumber(OFVector<Uint16> &values)
{
  // cast away const since underlying dcmdata routine is not const...
  DcmUnsignedShort *us = OFconst_cast(DcmUnsignedShort*, &ReferencedSegmentNumber);
  return DcmIODUtil::getUint16ValuesFromElement(*us, values);
}

// -- set dicom attributes --


OFCondition ImageSOPInstanceReferenceMacro::setReferencedFrameNumber(const OFVector<Uint16>& values,
                                                                     const OFBool checkValue)
{
  return DcmIODUtil::setUint16ValuesOnElement(ReferencedFrameNumber, values, "1-n", checkValue);
}


OFCondition ImageSOPInstanceReferenceMacro::addReferencedFrameNumber(const Uint16& value,
                                                                     const OFBool checkValue)
{
  (void)checkValue;
  const unsigned long count = ReferencedFrameNumber.getVM();
  return ReferencedFrameNumber.putUint16(value, count /* starts with 0, so add new value at the end */);
}



OFCondition ImageSOPInstanceReferenceMacro::setReferencedSegmentNumber(const OFVector<Uint16>& values,
                                                                       const OFBool checkValue)
{
  return DcmIODUtil::setUint16ValuesOnElement(ReferencedSegmentNumber, values, "1-n", checkValue);
}


OFCondition ImageSOPInstanceReferenceMacro::addReferencedSegmentNumber(const Uint16& value,
                                                                       const OFBool checkValue)
{
  (void)checkValue;
  const unsigned long count = ReferencedSegmentNumber.getVM();
  return ReferencedSegmentNumber.putUint16(value, count /* starts with 0, so add new value at the end */);

}



// ---------------------- GeneralAnatomyMacro ----------------------



GeneralAnatomyMacro::GeneralAnatomyMacro(const OFString& type) :
  m_Type(type),
  m_AnatomicRegion(),
  m_AnatomicRegionModifier(),
  m_PrimaryAnatomicStructure()
{
  m_Type = type;
}


GeneralAnatomyMacro::GeneralAnatomyMacro(const GeneralAnatomyMacro& rhs)
:  m_Type(rhs.m_Type),
   m_AnatomicRegion(),
   m_AnatomicRegionModifier(),
   m_PrimaryAnatomicStructure()
{
  if (this != &rhs)
  {
    m_Type = rhs.m_Type;
    m_AnatomicRegion = m_AnatomicRegion;
    m_PrimaryAnatomicStructure = rhs.m_PrimaryAnatomicStructure;

    OFVector<CodeSequenceMacro*>::const_iterator it = rhs.m_AnatomicRegionModifier.begin();
    while ( it != rhs.m_AnatomicRegionModifier.end() )
    {
      m_AnatomicRegionModifier.push_back( new CodeSequenceMacro(**it) );
      it++;
    }
  }
}



GeneralAnatomyMacro::~GeneralAnatomyMacro()
{
  clearData();
}


void GeneralAnatomyMacro::clearData()
{
  // m_Type stays the same
  m_AnatomicRegion.clearData();
  DcmIODUtil::freeContainer(m_AnatomicRegionModifier);
  m_PrimaryAnatomicStructure.clearData();
}


OFCondition GeneralAnatomyMacro::check(const OFBool quiet)
{
  OFCondition result = m_AnatomicRegion.check(quiet);
  if (result.bad())
    return result;

  OFVector<CodeSequenceMacro*>::iterator it = m_AnatomicRegionModifier.begin();
  while (it != m_AnatomicRegionModifier.begin())
  {
    result = (*it)->check(quiet);
    if (result.bad()) return result;
    it++;
  }
  result = m_PrimaryAnatomicStructure.check(quiet);
  return result;
}


CodeSequenceMacro& GeneralAnatomyMacro::getAnatomicRegion()
{
  return m_AnatomicRegion;
}


OFVector<CodeSequenceMacro*>& GeneralAnatomyMacro::getAnatomicRegionModifier()
{
  return m_AnatomicRegionModifier;
}


PrimaryAnatomicStructureMacro& GeneralAnatomyMacro::getPrimaryAnatomicStructure()
{
  return m_PrimaryAnatomicStructure;
}


// Reads Anatomic Region Sequence and Primary Anatomic Structure Macro from given item
OFCondition GeneralAnatomyMacro::read(DcmItem& source,
                                      const OFBool clearOldData)
{
  OFCondition result = EC_Normal;

  /* re-initialize object */
  if (clearOldData)
    clearData();

  /* read Anatomic Region Sequence item into Code Sequence Macro */
  DcmIODUtil::readSingleItem<CodeSequenceMacro>(source, DCM_AnatomicRegionSequence, m_AnatomicRegion, m_Type, "GeneralAnatomyMacro");

  /* Get the single item from Anatomic Region Sequence */
  DcmItem* localItem = NULL;
  if ( source.findAndGetSequenceItem(DCM_AnatomicRegionSequence, localItem).bad() )
  {
    return result;
  }

  /* read Anatomic Region Modifier Sequence from */
  DcmIODUtil::readSubSequence<OFVector<CodeSequenceMacro*> >
  ( *localItem, /* item of Anatomic Region Sequence */
    DCM_AnatomicRegionModifierSequence,
    m_AnatomicRegionModifier,
    "1-n",
    "3",
    "GeneralAnatomyMacro" );

  /* read Primary Anatomic Structure Macro (main level, i.e.\ original item) */
  m_PrimaryAnatomicStructure.read(source);

  return result;
}


/// Write Anatomic Region Sequence from given item
OFCondition GeneralAnatomyMacro::write(DcmItem& item)
{
  OFCondition result = EC_Normal;

  /* delete old data */
  item.findAndDeleteElement(DCM_AnatomicRegionSequence);

  /* Write sub structures */
  DcmIODUtil::writeSingleItem<CodeSequenceMacro>(result, DCM_AnatomicRegionSequence, m_AnatomicRegion, item, m_Type, "GeneralAnatomyMacro");
  DcmIODUtil::writeSubSequence<OFVector<CodeSequenceMacro*> >
  ( result,
    DCM_AnatomicRegionModifierSequence,
    m_AnatomicRegionModifier,
    item,
    "1-n",
    "3",
    "GeneralAnatomyMacro");
  return result;
}


GeneralAnatomyMacro& GeneralAnatomyMacro::operator=(const GeneralAnatomyMacro& rhs)
{
  if (this != &rhs)
  {
    clearData();
    m_Type = rhs.m_Type;
    m_AnatomicRegion = rhs.m_AnatomicRegion;
    m_PrimaryAnatomicStructure = rhs.m_PrimaryAnatomicStructure;

    OFVector<CodeSequenceMacro*>::const_iterator it = rhs.m_AnatomicRegionModifier.begin();
    while ( it != rhs.m_AnatomicRegionModifier.end() )
    {
      m_AnatomicRegionModifier.push_back( new CodeSequenceMacro(**it) );
      it++;
    }
  }
  return *this;
}


// ---------------------- PrimaryAnatomicStructureMacroItem ------------------

PrimaryAnatomicStructureMacroItem::PrimaryAnatomicStructureMacroItem() :
  m_AnatomicStructure(),
  m_AnatomicStructureModifier()
{
}


PrimaryAnatomicStructureMacroItem::PrimaryAnatomicStructureMacroItem(const PrimaryAnatomicStructureMacroItem& rhs) :
  m_AnatomicStructure(),
  m_AnatomicStructureModifier()
{
  if (this != &rhs)
  {
    m_AnatomicStructure = rhs.m_AnatomicStructure;
    OFVector<CodeSequenceMacro*>::const_iterator it = rhs.m_AnatomicStructureModifier.begin();
    while ( it != rhs.m_AnatomicStructureModifier.begin() )
    {
      m_AnatomicStructureModifier.push_back(new CodeSequenceMacro(**it));
      it++;
    }
  }
}


PrimaryAnatomicStructureMacroItem& PrimaryAnatomicStructureMacroItem::operator=(const PrimaryAnatomicStructureMacroItem& rhs)
{
  if (this != &rhs)
  {
    clearData();
    m_AnatomicStructure = rhs.m_AnatomicStructure;
    OFVector<CodeSequenceMacro*>::const_iterator it = rhs.m_AnatomicStructureModifier.begin();
    while ( it != rhs.m_AnatomicStructureModifier.begin() )
    {
      m_AnatomicStructureModifier.push_back(new CodeSequenceMacro(**it));
      it++;
    }
  }
  return *this;
}


PrimaryAnatomicStructureMacroItem::~PrimaryAnatomicStructureMacroItem()
{
  clearData();
}


void PrimaryAnatomicStructureMacroItem::clearData()
{
  m_AnatomicStructure.clearData();
  m_AnatomicStructureModifier.clear();
  DcmIODUtil::freeContainer(m_AnatomicStructureModifier);
}


OFCondition PrimaryAnatomicStructureMacroItem::check(const OFBool quiet)
{
  OFCondition result = m_AnatomicStructure.check();
  if (result.bad()) return result;

  OFVector<CodeSequenceMacro*>::iterator it = m_AnatomicStructureModifier.begin();
  while (it != m_AnatomicStructureModifier.end())
  {
    result = (*it)->check(quiet);
    if (result.bad())
      return result;
    it++;
  }
  return result;
}



CodeSequenceMacro& PrimaryAnatomicStructureMacroItem::getAnatomicStructure()
{
  return m_AnatomicStructure;
}


OFVector<CodeSequenceMacro*>& PrimaryAnatomicStructureMacroItem::getAnatomicStructureModifier()
{
  return m_AnatomicStructureModifier;
}


OFCondition PrimaryAnatomicStructureMacroItem::read(DcmItem& source,
                                                    const OFBool clearOldData)
{
  OFCondition result = EC_Normal;

  /* re-initialize object */
  if (clearOldData)
    clearData();

  m_AnatomicStructure.read(source);

  /* check whether cardinality of Primary Anatomic Structure Modifier Sequence and type is ok (produces warnings if not) */
  DcmIODUtil::checkSubSequence(result, source, DCM_PrimaryAnatomicStructureModifierSequence, "1-n", "3", "PrimaryAnatomicStructureMacro");

  /* read Primary Anatomic Structure Modifier Sequence */
  DcmIODUtil::readSubSequence<OFVector<CodeSequenceMacro*> >
  ( source, /* item of Primary Anatomic Structure Sequence */
    DCM_PrimaryAnatomicStructureModifierSequence,
    m_AnatomicStructureModifier,
    "1-n",
    "3",
    "GeneralAnatomyMacro" );

  return result;
}


/// Write Primary Anatomic Structure Sequence Item to given item
OFCondition PrimaryAnatomicStructureMacroItem::write(DcmItem& source)
{
  OFCondition result;
  m_AnatomicStructure.write(source);
  DcmIODUtil::writeSubSequence<OFVector<CodeSequenceMacro*> >
  (
   result,
   DCM_PrimaryAnatomicStructureModifierSequence,
   m_AnatomicStructureModifier,
   source,
   "1-n",
   "3",
   "PrimaryAnatomicStructureMacro"
  );
  return result;
}


// ---------------------- PrimaryAnatomicStructureMacro ----------------------


PrimaryAnatomicStructureMacro::PrimaryAnatomicStructureMacro() :
  m_PrimaryAnatomicStructure()
{

}


PrimaryAnatomicStructureMacro::PrimaryAnatomicStructureMacro(const PrimaryAnatomicStructureMacro& rhs)
{
  if (this != &rhs)
  {
    OFVector<PrimaryAnatomicStructureMacroItem*>::const_iterator it = rhs.m_PrimaryAnatomicStructure.begin();
    while ( it != rhs.m_PrimaryAnatomicStructure.begin() )
    {
      m_PrimaryAnatomicStructure.push_back(new PrimaryAnatomicStructureMacroItem(**it));
      it++;
    }
  }
}


PrimaryAnatomicStructureMacro::~PrimaryAnatomicStructureMacro()
{
  clearData();
}


void PrimaryAnatomicStructureMacro::clearData()
{
  m_PrimaryAnatomicStructure.clear();
}


OFCondition PrimaryAnatomicStructureMacro::check(const OFBool quiet)
{
  OFCondition result;
  OFVector<PrimaryAnatomicStructureMacroItem*>::iterator it = m_PrimaryAnatomicStructure.begin();
  while (it != m_PrimaryAnatomicStructure.end())
  {
    result = (*it)->check(quiet);
    if (result.bad()) return result;
    it++;
  }
  return result;
}


OFVector<PrimaryAnatomicStructureMacroItem*>& PrimaryAnatomicStructureMacro::getPrimaryAnatomicStructure()
{
  return m_PrimaryAnatomicStructure;
}


/// Reads Primary Anatomic Region Sequence from given item
OFCondition PrimaryAnatomicStructureMacro::read(DcmItem& source,
                                                const OFBool clearOldData)
{
  OFCondition result;

  if (clearOldData)
    clearData();

  // check for sequence and report warnings if not ok */
  DcmIODUtil::checkSubSequence(result, source, DCM_PrimaryAnatomicStructureSequence, "1-n", "3", "PrimaryAnantomicStructureMacro");

  // read sequence into member
  DcmIODUtil::readSubSequence<OFVector<PrimaryAnatomicStructureMacroItem*> >
  ( source,
    DCM_PrimaryAnatomicStructureSequence,
    m_PrimaryAnatomicStructure,
    "1-n",
    "3",
    "PrimaryAnatomicStructureMacro"
  );
  return result;
}


/// Write Anatomic Region Sequence from given item
OFCondition PrimaryAnatomicStructureMacro::write(DcmItem& item)
{
  OFCondition result = EC_Normal;

  /* delete old data */
  item.findAndDeleteElement(DCM_PrimaryAnatomicStructureSequence);
  item.insertEmptyElement(DCM_PrimaryAnatomicStructureSequence);

  DcmIODUtil::writeSubSequence<OFVector<PrimaryAnatomicStructureMacroItem*> >
  (
    result,
    DCM_PrimaryAnatomicStructureSequence,
    m_PrimaryAnatomicStructure,
    item,
    "1-n",
    "3",
    "PrimaryAnatomicStructureMacro"
  );

  return result;
}


PrimaryAnatomicStructureMacro& PrimaryAnatomicStructureMacro::operator=(const PrimaryAnatomicStructureMacro& rhs)
{
  if (this != &rhs)
  {
    clearData();
    OFVector<PrimaryAnatomicStructureMacroItem*>::const_iterator it = rhs.m_PrimaryAnatomicStructure.begin();
    while ( it != rhs.m_PrimaryAnatomicStructure.begin() )
    {
      m_PrimaryAnatomicStructure.push_back(new PrimaryAnatomicStructureMacroItem(**it));
      it++;
    }
  }
  return *this;
}


// ---------------------- AlgorithmIdentificationMacro ----------------------


AlgorithmIdentificationMacro::AlgorithmIdentificationMacro() :
  m_AlgorithmFamilyCode(),
  m_AlgorithmNameCode(),
  m_AlgorithmName(DCM_AlgorithmName),
  m_AlgorithmVersion(DCM_AlgorithmVersion),
  m_AlgorithmParameters(DCM_AlgorithmParameters),
  m_AlgorithmSource(DCM_AlgorithmSource)
{

}
//
AlgorithmIdentificationMacro::~AlgorithmIdentificationMacro()
{
  clearData();
}


void AlgorithmIdentificationMacro::clearData()
{
  m_AlgorithmFamilyCode.clearData();
  m_AlgorithmNameCode.clearData();
  m_AlgorithmName.clear();
  m_AlgorithmVersion.clear();
  m_AlgorithmParameters.clear();
  m_AlgorithmSource.clear();
}


OFCondition AlgorithmIdentificationMacro::check(const OFBool quiet)
{
  OFCondition result;
  result = m_AlgorithmFamilyCode.check(quiet);
  if ( result.good() )
  {
    if ( m_AlgorithmName.isEmpty() || m_AlgorithmVersion.isEmpty() )
    {
      result = EC_MissingValue;
    }
  }
  return result;
}



CodeSequenceMacro& AlgorithmIdentificationMacro::getAlgorithmFamilyCode()
{
  return m_AlgorithmFamilyCode;
}


CodeSequenceMacro& AlgorithmIdentificationMacro::getAlgorithmNameCode()
{
  return m_AlgorithmNameCode;
}


OFCondition AlgorithmIdentificationMacro::getAlgorithmName(OFString& value,
                                                           const signed long pos)
{
  return DcmIODUtil::getStringValueFromElement(m_AlgorithmName, value, pos);
}


OFCondition AlgorithmIdentificationMacro::getAlgorithmVersion(OFString& value,
                                                              const signed long pos)
{
  return DcmIODUtil::getStringValueFromElement(m_AlgorithmVersion, value, pos);
}


OFCondition AlgorithmIdentificationMacro::getAlgorithmParameters(OFString& value,
                                                                 const signed long pos)
{
  return DcmIODUtil::getStringValueFromElement(m_AlgorithmParameters, value, pos);
}


OFCondition AlgorithmIdentificationMacro::getAlgorithmSource(OFString& value,
                                                             const signed long pos)
{
  return DcmIODUtil::getStringValueFromElement(m_AlgorithmSource, value, pos);
}


OFCondition AlgorithmIdentificationMacro::setAlgorithmName(const OFString& value,
                                                           const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_AlgorithmName.putOFStringArray(value);
  return result;
}


OFCondition AlgorithmIdentificationMacro::setAlgorithmVersion(const OFString& value,
                                                              const OFBool checkValue)

{
  OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_AlgorithmVersion.putOFStringArray(value);
  return result;
}


OFCondition AlgorithmIdentificationMacro::setAlgorithmParameters(const OFString& value,
                                                                 const OFBool checkValue)
{
  (void)checkValue;
  return m_AlgorithmParameters.putOFStringArray(value);
}


OFCondition AlgorithmIdentificationMacro::setAlgorithmSource(const OFString& value,
                                                             const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_AlgorithmSource.putOFStringArray(value);
  return result;
}


/// Reads Anatomic Region Sequence and Primary Anatomic Structure Macro from given item
OFCondition AlgorithmIdentificationMacro::read(DcmItem& source,
                                               const OFBool clearOldData)
{
  OFCondition result;

  if (clearOldData)
    clearData();

  DcmIODUtil::readSingleItem<CodeSequenceMacro>
  (
    source,
    DCM_AlgorithmFamilyCodeSequence,
    m_AlgorithmFamilyCode,
    "1",
    "AlgorithmIdentificationMacro"
  );

  DcmIODUtil::readSingleItem<CodeSequenceMacro>
  (
    source,
    DCM_AlgorithmNameCodeSequence,
    m_AlgorithmNameCode,
    "3",
    "AlgorithmIdentificationMacro");

  DcmIODUtil::getAndCheckElementFromDataset(source, m_AlgorithmName, "1", "1", "AlgorithmIdentificationMacro");
  DcmIODUtil::getAndCheckElementFromDataset(source, m_AlgorithmVersion, "1", "1", "AlgorithmIdentificationMacro");
  DcmIODUtil::getAndCheckElementFromDataset(source, m_AlgorithmParameters, "1", "3", "AlgorithmIdentificationMacro");
  DcmIODUtil::getAndCheckElementFromDataset(source, m_AlgorithmSource, "1", "3", "AlgorithmIdentificationMacro");

  return result;
}


OFCondition AlgorithmIdentificationMacro::write(DcmItem& item)
{
  OFCondition result = EC_Normal;

  // write to item
  DcmIODUtil::copyElementToDataset(result, item, m_AlgorithmName, "1", "1", "AlgorithmIdentificationMacro");
  DcmIODUtil::copyElementToDataset(result, item, m_AlgorithmVersion, "1", "1", "AlgorithmIdentificationMacro");
  DcmIODUtil::copyElementToDataset(result, item, m_AlgorithmParameters, "1", "3", "AlgorithmIdentificationMacro");
  DcmIODUtil::copyElementToDataset(result, item, m_AlgorithmSource, "1", "3", "AlgorithmIdentificationMacro");

  DcmIODUtil::writeSingleItem<CodeSequenceMacro>
  (  result,
     DCM_AlgorithmFamilyCodeSequence,
     m_AlgorithmFamilyCode,
     item,
     "1",
     "AlgorithmIdentificationMacro"
  );

  DcmIODUtil::writeSingleItem<CodeSequenceMacro>
  (  result,
     DCM_AlgorithmNameCodeSequence,
     m_AlgorithmNameCode,
     item,
     "3",
     "AlgorithmIdentificationMacro"
  );

  return result;
}


// ---------------------- ContentIdentificationMacro ----------------------

ContentIdentificationMacro::ContentIdentificationMacro() :
  m_InstanceNumber(DCM_InstanceNumber),
  m_ContentLabel(DCM_ContentLabel),
  m_ContentDescription(DCM_ContentDescription),
  m_AlternateContentDescription(),
  m_ContentCreatorName(DCM_ContentCreatorName),
  m_ContentCreatorIdentificationCode(),
  m_IODRules()
{
  resetRules();
}


void ContentIdentificationMacro::resetRules()
{
  m_IODRules.addRule(new IODRule(DCM_InstanceNumber,     "1", "1", "ContentIdentificationMacro", DcmIODTypes::IE_INSTANCE), OFTrue);
  m_IODRules.addRule(new IODRule(DCM_ContentLabel,       "1", "1", "ContentIdentificationMacro", DcmIODTypes::IE_INSTANCE), OFTrue);
  m_IODRules.addRule(new IODRule(DCM_ContentDescription, "1", "2", "ContentIdentificationMacro", DcmIODTypes::IE_INSTANCE), OFTrue);
  m_IODRules.addRule(new IODRule(DCM_ContentCreatorName, "1", "2", "ContentIdentificationMacro", DcmIODTypes::IE_INSTANCE), OFTrue);
  m_IODRules.addRule(new IODRule(DCM_AlternateContentDescriptionSequence, "1-n", "3", "ContentIdentificationMacro", DcmIODTypes::IE_INSTANCE), OFTrue);
  m_IODRules.addRule(new IODRule(DCM_ContentCreatorIdentificationCodeSequence, "1", "3", "ContentIdentificationMacro", DcmIODTypes::IE_INSTANCE), OFTrue);
}


IODRules& ContentIdentificationMacro::getIODRules()
{
  return m_IODRules;
}


ContentIdentificationMacro::ContentIdentificationMacro(const ContentIdentificationMacro& rhs) :
  m_InstanceNumber(DCM_InstanceNumber),
  m_ContentLabel(DCM_ContentLabel),
  m_ContentDescription(DCM_ContentDescription),
  m_AlternateContentDescription(),
  m_ContentCreatorName(DCM_ContentCreatorName),
  m_ContentCreatorIdentificationCode(),
  m_IODRules()
{
  if (&rhs != this)
  {
    resetRules();
    m_InstanceNumber = rhs.m_InstanceNumber;
    m_ContentLabel = rhs.m_ContentLabel;
    m_ContentDescription = rhs.m_ContentDescription;
    m_ContentCreatorName = rhs.m_ContentCreatorName;
    m_ContentCreatorIdentificationCode = rhs.m_ContentCreatorIdentificationCode;
    /* perform deep vector copy */
    OFVector<AlternateContentDescriptionItem*>::const_iterator it = rhs.m_AlternateContentDescription.begin();
    while (it != rhs.m_AlternateContentDescription.end())
    {
      m_AlternateContentDescription.push_back(new AlternateContentDescriptionItem(**it));
      it++;
    }
  }
}


ContentIdentificationMacro& ContentIdentificationMacro::operator=(const ContentIdentificationMacro& rhs)
{
  if (&rhs != this)
  {
    this->clearData();
    m_InstanceNumber = rhs.m_InstanceNumber;
    m_ContentLabel = rhs.m_ContentLabel;
    m_ContentDescription = rhs.m_ContentDescription;
    m_ContentCreatorName = rhs.m_ContentCreatorName;
    m_ContentCreatorIdentificationCode = rhs.m_ContentCreatorIdentificationCode;
    /* perform deep vector copy */
    OFVector<AlternateContentDescriptionItem*>::const_iterator it = rhs.m_AlternateContentDescription.begin();
    while (it != rhs.m_AlternateContentDescription.end())
    {
      AlternateContentDescriptionItem* newItem = new AlternateContentDescriptionItem();
      *newItem = **it;
      m_AlternateContentDescription.push_back(newItem);
      it++;
    }
  }
  return *this;
}


ContentIdentificationMacro::~ContentIdentificationMacro()
{
  clearData();
}


void ContentIdentificationMacro::clearData()
{
  m_InstanceNumber.clear();
  m_ContentLabel.clear();
  m_ContentDescription.clear();
  DcmIODUtil::freeContainer(m_AlternateContentDescription);
  m_ContentCreatorName.clear();
  m_ContentCreatorIdentificationCode.clearData();
}


OFCondition ContentIdentificationMacro::check(const OFBool quiet)
{
  (void)quiet;
  OFCondition result;
  OFBool failure = m_ContentLabel.isEmpty() || m_InstanceNumber.isEmpty();
  if (!failure)
  {
    if (!m_AlternateContentDescription.empty())  // type 3
    {
      OFVector<AlternateContentDescriptionItem*>::iterator it = m_AlternateContentDescription.begin();
      while ( it != m_AlternateContentDescription.end() && !failure)
      {
        OFString str;
        (*it)->getContentDescription(str).good(); // type 1
        failure = str.empty();
        if (!failure)
        {
          OFString meaning, value, designator;
          (*it)->getLanguageCode().getCodeMeaning(meaning);
          (*it)->getLanguageCode().getCodeValue(value);
          (*it)->getLanguageCode().getCodeValue(designator);
          failure = meaning.empty() || value.empty() || designator.empty();
        }
        it++;
      }
    }
  }
  if ( failure )
    result = EC_IllegalParameter;
  return result;
}



OFCondition ContentIdentificationMacro::getInstanceNumber(OFString& value,
                                                          const signed long  pos) const
{
  return DcmIODUtil::getStringValueFromElement(m_InstanceNumber, value, pos);
}



OFCondition ContentIdentificationMacro::getContentLabel(OFString& value,
                                                        const signed long pos) const
{
  return DcmIODUtil::getStringValueFromElement(m_ContentLabel, value, pos);
}


OFCondition ContentIdentificationMacro::getContentDescription(OFString& value,
                                                              const signed long pos) const
{
  return DcmIODUtil::getStringValueFromElement(m_ContentDescription, value, pos);
}


OFCondition ContentIdentificationMacro::getContentCreatorName(OFString& value,
                                          const signed long pos) const
{
  return DcmIODUtil::getStringValueFromElement(m_ContentCreatorName, value, pos);
}


CodeSequenceMacro& ContentIdentificationMacro::getContentCreatorIdentificationCode()
{
  return m_ContentCreatorIdentificationCode;
}


OFVector<ContentIdentificationMacro::AlternateContentDescriptionItem*>& ContentIdentificationMacro::getAlternateContentDescription()
{
  return m_AlternateContentDescription;
}


OFCondition ContentIdentificationMacro::setInstanceNumber(const OFString& value,
                                                          const OFBool checkValue)
{
  OFCondition result;
  if (checkValue)
  {
    result = (checkValue) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
  }
  if (result.good())
  {
    result = m_InstanceNumber.putOFStringArray(value);
  }
  return result;
}


OFCondition ContentIdentificationMacro::setContentLabel(const OFString& value,
                                                        const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, m_IODRules.getByTag(DCM_ContentLabel)->getVM()) : EC_Normal;
  if (result.good())
    result = m_ContentLabel.putOFStringArray(value);
  return result;
}


OFCondition ContentIdentificationMacro::setContentDescription(const OFString& value,
                                                              const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, m_IODRules.getByTag(DCM_ContentDescription)->getVM()) : EC_Normal;
  if (result.good())
    result = m_ContentDescription.putOFStringArray(value);
  return result;
}


OFCondition ContentIdentificationMacro::setContentCreatorName(const OFString& value,
                                                              const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmPersonName::checkStringValue(value, m_IODRules.getByTag(DCM_ContentCreatorName)->getVM()) : EC_Normal;
  if (result.good())
    result = m_ContentCreatorName.putOFStringArray(value);
  return result;
}


OFCondition ContentIdentificationMacro::read(DcmItem& source,
                                             const OFBool clearOldData)
{
  OFCondition result;

  if (clearOldData)
    clearData();

  /* flat elements */

  DcmIODUtil::getAndCheckElementFromDataset(source, m_InstanceNumber, m_IODRules.getByTag(DCM_InstanceNumber));
  DcmIODUtil::getAndCheckElementFromDataset(source, m_ContentLabel, m_IODRules.getByTag(DCM_ContentLabel));
  DcmIODUtil::getAndCheckElementFromDataset(source, m_ContentDescription, m_IODRules.getByTag(DCM_ContentDescription));
  DcmIODUtil::getAndCheckElementFromDataset(source, m_ContentCreatorName, m_IODRules.getByTag(DCM_ContentCreatorName));

  /* sub sequences */
  IODRule *rule = m_IODRules.getByTag(DCM_AlternateContentDescriptionSequence);
  DcmIODUtil::readSubSequence<OFVector<AlternateContentDescriptionItem*> >
  ( source,
    DCM_AlternateContentDescriptionSequence,
    m_AlternateContentDescription,
    rule->getVM(),
    rule->getType(),
    "ContentIdentificationMacro" );

  rule = m_IODRules.getByTag(DCM_ContentCreatorIdentificationCodeSequence);
  DcmIODUtil::readSingleItem<CodeSequenceMacro>
  (
    source,
   DCM_ContentCreatorIdentificationCodeSequence,
   m_ContentCreatorIdentificationCode,
   rule->getType(),
   "ContentIdentificationMacro");

  return result;
}


OFCondition ContentIdentificationMacro::write(DcmItem& item)
{
  OFCondition result = EC_Normal;

  /* flat elements */
  DcmIODUtil::copyElementToDataset(result, item, m_InstanceNumber,  m_IODRules.getByTag(DCM_InstanceNumber));
  DcmIODUtil::copyElementToDataset(result, item, m_ContentLabel, m_IODRules.getByTag(DCM_ContentLabel));
  DcmIODUtil::copyElementToDataset(result, item, m_ContentDescription, m_IODRules.getByTag(DCM_ContentDescription));
  DcmIODUtil::copyElementToDataset(result, item, m_ContentCreatorName,  m_IODRules.getByTag(DCM_ContentCreatorName));

  IODRule *rule = m_IODRules.getByTag(DCM_ContentCreatorIdentificationCodeSequence);
  DcmIODUtil::writeSingleItem<CodeSequenceMacro>
  (
    result,
    DCM_ContentCreatorIdentificationCodeSequence,
    m_ContentCreatorIdentificationCode,
    item,
    rule->getType(),
    "ContentIdentificationMacro"
  );

  rule = m_IODRules.getByTag(DCM_AlternateContentDescriptionSequence);
  DcmIODUtil::writeSubSequence< OFVector<ContentIdentificationMacro::AlternateContentDescriptionItem*> >
  (
    result,
    DCM_AlternateContentDescriptionSequence,
    m_AlternateContentDescription,
    item,
    rule->getVM(),
    rule->getType(),
    "ContentIdentificationMacro"
  );

  return result;

}


// ---------------- ContentIdentificationMacro::AlternateContentDescriptionItem -----------------

ContentIdentificationMacro::AlternateContentDescriptionItem::AlternateContentDescriptionItem()
  : m_ContentDescription(DCM_ContentDescription),
    m_LanguageCode()
{
  // nothing to do
}


ContentIdentificationMacro::AlternateContentDescriptionItem::~AlternateContentDescriptionItem()
{
  m_LanguageCode.clearData();
}


void ContentIdentificationMacro::AlternateContentDescriptionItem::clearData()
{
  m_ContentDescription.clear();
  m_LanguageCode.clearData();
}


OFCondition ContentIdentificationMacro::AlternateContentDescriptionItem::read(DcmItem& source,
                                                                              const OFBool clearOldData)
{
  OFCondition result;
  if (clearOldData)
    clearData();

  DcmIODUtil::getAndCheckElementFromDataset(source, m_ContentDescription, "1", "1", "ContentIdentificationMacro");
  DcmIODUtil::readSingleItem<CodeSequenceMacro>
  (
    source,
    DCM_LanguageCodeSequence,
    m_LanguageCode,
    "1",
    "ContentIdentificationMacro"
  );

  return result;
}


OFCondition ContentIdentificationMacro::AlternateContentDescriptionItem::getContentDescription(OFString& value,
                                                                                               const long signed int pos)
{
  return DcmIODUtil::getStringValueFromElement(m_ContentDescription, value, pos);
}



CodeSequenceMacro& ContentIdentificationMacro::AlternateContentDescriptionItem::getLanguageCode()
{
  return m_LanguageCode;
}



OFCondition ContentIdentificationMacro::AlternateContentDescriptionItem::write(DcmItem& item)
{
  OFCondition result = EC_Normal;
  // write to item
  DcmIODUtil::copyElementToDataset(result, item, m_ContentDescription, "1", "1", "ContentIdentificationMacro");
  DcmIODUtil::writeSingleItem<CodeSequenceMacro>
  (  result,
     DCM_LanguageCodeSequence,
     m_LanguageCode,
     item,
     "1",
     "ContentIdentificationMacro"
  );
  return result;
}


OFCondition ContentIdentificationMacro::AlternateContentDescriptionItem::setContentDescription(const OFString& value,
                                                                                               const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_ContentDescription.putOFStringArray(value);
  return result;
}


// -------------------- HL7HierarchicDesignatorMacro --------------------


HL7HierarchicDesignatorMacro::HL7HierarchicDesignatorMacro(IODComponent* parent)
: IODComponent(parent)
{
  // reset element rules
  resetRules();
}

HL7HierarchicDesignatorMacro::HL7HierarchicDesignatorMacro(OFshared_ptr< DcmItem > item,
                                                           OFshared_ptr< IODRules > rules,
                                                           IODComponent* parent)
: IODComponent(item, rules, parent)
{
  // reset element rules
  resetRules();
}


OFCondition HL7HierarchicDesignatorMacro::getUniversalEntityID(OFString& value, const long signed int pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_UniversalEntityID, *m_Item, value, pos);
}


OFCondition HL7HierarchicDesignatorMacro::getLocalNamespaceEntityID(OFString& value, const long signed int pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_LocalNamespaceEntityID, *m_Item, value, pos);
}


OFCondition HL7HierarchicDesignatorMacro::getUniversalEntityIDType(OFString& value, const long signed int pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_UniversalEntityIDType, *m_Item, value, pos);
}


OFString HL7HierarchicDesignatorMacro::getName() const
{
  return "HL7HierarchicDesignatorMacro";
}


void HL7HierarchicDesignatorMacro::resetRules()
{
  // parameters are tag, VM, type. Overwrite old rules if any.
  m_Rules->addRule(new IODRule(DCM_UniversalEntityID, "1","1C", "HL7HierarchicDesignatorMacro", DcmIODTypes::IE_UNDEFINED), OFTrue);
  m_Rules->addRule(new IODRule(DCM_LocalNamespaceEntityID, "1","1C", "HL7HierarchicDesignatorMacro", DcmIODTypes::IE_UNDEFINED), OFTrue);
  m_Rules->addRule(new IODRule(DCM_UniversalEntityIDType, "1","1C", "HL7HierarchicDesignatorMacro", DcmIODTypes::IE_UNDEFINED), OFTrue);
}


OFCondition HL7HierarchicDesignatorMacro::setLocalNamespaceEntityID(const OFString& value,
                                                                    const OFBool checkValue)
{
  (void)checkValue;
  return m_Item->putAndInsertOFStringArray(DCM_LocalNamespaceEntityID,value);
}


OFCondition HL7HierarchicDesignatorMacro::setUniversalEntityID(const OFString& value,
                                                               const OFBool checkValue)
{
  (void) checkValue;
  return m_Item->putAndInsertOFStringArray(DCM_UniversalEntityID,value);
}


OFCondition HL7HierarchicDesignatorMacro::setUniversalEntityIDType(const OFString& value,
                                                                   const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_UniversalEntityID,value);
  return result;
}



// -------------------- Mandatory View and Slice Progression Direction Macro --------------------

MandatoryViewAndSliceProgressionDirectionMacro::MandatoryViewAndSliceProgressionDirectionMacro(OFshared_ptr< DcmItem > item,
                                                                                               OFshared_ptr< IODRules > rules,
                                                                                               IODComponent* parent)
: IODComponent(item, rules, parent)
{
  resetRules();
}



MandatoryViewAndSliceProgressionDirectionMacro::MandatoryViewAndSliceProgressionDirectionMacro(IODComponent* parent)
: IODComponent(parent)
{
  resetRules();
}


OFString MandatoryViewAndSliceProgressionDirectionMacro::getName() const
{
  return "MandatoryViewAndSliceProgressionDirectionMacro";
}


void MandatoryViewAndSliceProgressionDirectionMacro::resetRules()
{
  m_Rules->addRule(new IODRule(DCM_ViewCodeSequence, "1","1", "MandatoryViewAndSliceProgressionDirectionMacro", DcmIODTypes::IE_UNDEFINED), OFTrue);
  m_Rules->addRule(new IODRule(DCM_ViewModifierCodeSequence, "1-n","2C", "MandatoryViewAndSliceProgressionDirectionMacro", DcmIODTypes::IE_UNDEFINED), OFTrue);
  m_Rules->addRule(new IODRule(DCM_SliceProgressionDirection, "1","1C", "MandatoryViewAndSliceProgressionDirectionMacro", DcmIODTypes::IE_UNDEFINED), OFTrue);
}


void MandatoryViewAndSliceProgressionDirectionMacro::clearData()
{
  DcmIODUtil::freeContainer(m_ViewModifierCode);
  m_ViewCodeSequence.clearData();
  IODComponent::clearData();
}



OFCondition MandatoryViewAndSliceProgressionDirectionMacro::read(DcmItem& source,
                                                                 const OFBool clearOldData)
{
  if (clearOldData)
  {
    m_ViewCodeSequence.clearData();
    DcmIODUtil::freeContainer(m_ViewModifierCode);
  }
  OFCondition result = EC_Normal;
  DcmIODUtil::readSingleItem(source, DCM_ViewCodeSequence, m_ViewCodeSequence, m_Rules->getByTag(DCM_ViewCodeSequence));
  DcmIODUtil::readSubSequence(source, DCM_ViewCodeSequence, m_ViewModifierCode, m_Rules->getByTag(DCM_ViewCodeSequence));
  IODComponent::read(source, clearOldData);
  return EC_Normal;
}


OFCondition MandatoryViewAndSliceProgressionDirectionMacro::write(DcmItem& item)
{
  OFCondition result = EC_Normal;
  DcmIODUtil::writeSingleItem(result, DCM_ViewCodeSequence, m_ViewCodeSequence, *m_Item, m_Rules->getByTag(DCM_ViewCodeSequence));
  DcmIODUtil::writeSubSequence(result, DCM_ViewCodeSequence, m_ViewModifierCode, *m_Item, m_Rules->getByTag(DCM_ViewModifierCodeSequence));
  if (result.good()) result = IODComponent::write(item);
  return result;
}


CodeSequenceMacro& MandatoryViewAndSliceProgressionDirectionMacro::getViewCode()
{
  return m_ViewCodeSequence;
}


OFVector< CodeSequenceMacro* >& MandatoryViewAndSliceProgressionDirectionMacro::getViewModifierCode()
{
  return m_ViewModifierCode;
}
