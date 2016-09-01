/*
 *
 *  Copyright (C) 2015-2016, Open Connections GmbH
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
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for managing US Image Description Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmiod/iodtypes.h"
#include "dcmtk/dcmfg/fgusimagedescription.h"

// Constructor
FGUSImageDescription::FGUSImageDescription() :
  FGBase(DcmFGTypes::EFG_USIMAGEDESCRIPTION),
  m_FrameType(DCM_FrameType),
  m_VolumetricProperties(DCM_VolumetricProperties),
  m_VolumeBasedCalculationTechnique(DCM_VolumeBasedCalculationTechnique)
{
}


FGUSImageDescription::~FGUSImageDescription()
{
  // nothing to do
}


FGBase* FGUSImageDescription::clone() const
{
  FGUSImageDescription* copy = new FGUSImageDescription();
  if (copy)
  {
    copy->m_FrameType = this->m_FrameType;
    copy->m_VolumetricProperties = this->m_VolumetricProperties;
    copy->m_VolumeBasedCalculationTechnique = this->m_VolumeBasedCalculationTechnique;
  }
  return copy;
}


void FGUSImageDescription::clearData()
{
  m_FrameType.clear();
  m_VolumetricProperties.clear();
  m_VolumeBasedCalculationTechnique.clear();
}


OFCondition FGUSImageDescription::check() const
{
  // Checks in read() and write() are sufficient for now
  return EC_Normal;
}


// --- get() functionality ---

OFCondition FGUSImageDescription::getFrameType(OFString& value,
                                               const long signed int pos)
{
  return DcmIODUtil::getStringValueFromElement(m_FrameType, value, pos);
}


OFCondition FGUSImageDescription::getVolumetricProperties(OFString& value,
                                                          const long signed int pos)
{
  return DcmIODUtil::getStringValueFromElement(m_VolumetricProperties, value, pos);
}


OFCondition FGUSImageDescription::getVolumeBasedCalculationTechnique(OFString& value,
                                                                     const long signed int pos)
{
  return DcmIODUtil::getStringValueFromElement(m_VolumeBasedCalculationTechnique, value, pos);
}


// --- set() functionality ---


OFCondition FGUSImageDescription::setFrameType(const DcmFGTypes::E_FGUSFrameType pixelDataChar,
                                               const OFString& imageFlavor,
                                               const OFString& derivedPixelContrast,
                                               const OFBool checkValue)
{
  // Build a string of VM 4 with first two values at least set
  OFString value;
  switch (pixelDataChar)
  {
    case(DcmFGTypes::EFGFT_ORIGINAL): value = "ORIGINAL\\PRIMARY\\"; break;
    case(DcmFGTypes::EFGFT_DERIVED) : value = "DERIVED\\PRIMARY\\"; break;
    default: return IOD_EC_InvalidElementValue;
  }
  value += imageFlavor;
  value += "\\";
  value += derivedPixelContrast;

  OFCondition result;
  if (checkValue)
  {
    result = DcmCodeString::checkStringValue(value, "4");
  }
  if (result.good())
  {
    result = m_FrameType.putOFStringArray(value);
  }
  return result;
}


OFCondition FGUSImageDescription::setVolumetricProperties(const OFString& value,
                                                          const OFBool checkValue)
{
 OFCondition result;
 if (checkValue)
 {
   if ( (value != "VOLUME") && (value != "SAMPLED") && (value != "DISTORTED") && (value != "MIXED") )
   {
     result = IOD_EC_InvalidElementValue;
   }
   else
     result = DcmCodeString::checkStringValue(value, "1");
 }
 if (result.good())
 {
   result = m_VolumetricProperties.putOFStringArray(value);
 }
  return result;
}


OFCondition FGUSImageDescription::setVolumeBasedCalculationTechnique(const OFString& value,
                                                                     const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_VolumeBasedCalculationTechnique.putOFStringArray(value);
  return result;
}


/// Read Frame Content Sequence from given item
OFCondition FGUSImageDescription::read(DcmItem& item)
{
  clearData();

  DcmItem* seqItem = NULL;
  OFCondition result = getItemFromFGSequence(item, DCM_USImageDescriptionSequence, 0, seqItem);
  if (result.bad())
    return result;

  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_FrameType, "4", "1", "USImageDescriptionMacro");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_VolumetricProperties, "1", "1", "USImageDescriptionMacro");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_VolumeBasedCalculationTechnique, "1", "1", "USImageDescriptionMacro");
  return result;
}


/// Writes single Frame Content Sequence into given item
OFCondition FGUSImageDescription::write(DcmItem& item)
{
  DcmItem *seqItem = NULL;
  OFCondition result = createNewFGSequence(item, DCM_USImageDescriptionSequence, 0, seqItem);
  if (result.bad())
    return result;

  // --- write frame content macro attributes ---
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_FrameType, "4", "1", "USImageDescriptionMacro");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_VolumetricProperties, "1", "1", "USImageDescriptionMacro");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_VolumeBasedCalculationTechnique, "1", "1", "USImageDescriptionMacro");

  return result;
}


int FGUSImageDescription::compare(const FGBase& rhs) const
{
  int result = FGBase::compare(rhs);
  if (result != 0)
    return result;

  const FGUSImageDescription* myRhs = OFstatic_cast(const FGUSImageDescription*, &rhs);

  // Compare all elements
  result = m_FrameType.compare(myRhs->m_FrameType);
  if (result == 0)
    result = m_VolumetricProperties.compare(myRhs->m_VolumetricProperties);
  if (result == 0)
    result = m_VolumeBasedCalculationTechnique.compare(myRhs->m_VolumeBasedCalculationTechnique);

  return result;
}
