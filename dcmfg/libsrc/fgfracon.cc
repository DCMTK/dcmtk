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
 *  Purpose: Class for managing the Frame Content Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmfg/fgfracon.h"
#include "dcmtk/dcmfg/fgtypes.h"

// Constructor
FGFrameContent::FGFrameContent() :
  FGBase(DcmFGTypes::EFG_FRAMECONTENT),
  m_FrameAcquisitonNumber(DCM_FrameAcquisitionNumber),
  m_FrameReferenceDateTime(DCM_FrameReferenceDateTime),
  m_FrameAcquisitionDateTime(DCM_FrameAcquisitionDateTime),
  m_FrameAcquisitionDuration(DCM_FrameAcquisitionDuration),
  m_CardiacCyclePosition(DCM_CardiacCyclePosition),
  m_RespiratoryCyclePosition(DCM_RespiratoryCyclePosition),
  m_DimensionIndexValues(DCM_DimensionIndexValues),
  m_TemporalPositionIndex(DCM_TemporalPositionIndex),
  m_StackID(DCM_StackID),
  m_InStackPositionNumber(DCM_InStackPositionNumber),
  m_FrameComments(DCM_FrameComments),
  m_FrameLabel(DCM_FrameLabel)
{
}


FGFrameContent::~FGFrameContent()
{
  // nothing to do
}


FGBase* FGFrameContent::clone() const
{
  FGFrameContent* copy = new FGFrameContent();
  if (copy)
  {
    copy->m_FrameAcquisitonNumber = this->m_FrameAcquisitonNumber;
    copy->m_FrameReferenceDateTime = this->m_FrameReferenceDateTime;
    copy->m_FrameAcquisitionDateTime = this->m_FrameAcquisitionDateTime;
    copy->m_FrameAcquisitionDuration = this->m_FrameAcquisitionDuration;
    copy->m_CardiacCyclePosition = this->m_CardiacCyclePosition;
    copy->m_RespiratoryCyclePosition = this->m_RespiratoryCyclePosition;
    copy->m_DimensionIndexValues = this->m_DimensionIndexValues;
    copy->m_TemporalPositionIndex = this->m_TemporalPositionIndex;
    copy->m_StackID = this->m_StackID;
    copy->m_InStackPositionNumber = this->m_InStackPositionNumber;
    copy->m_FrameComments = this->m_FrameComments;
    copy->m_FrameLabel = this->m_FrameLabel;
  }
  return copy;
}


void FGFrameContent::clearData()
{
  m_FrameAcquisitonNumber.clear();
  m_FrameReferenceDateTime.clear();
  m_FrameAcquisitionDateTime.clear();
  m_FrameAcquisitionDuration.clear();
  m_CardiacCyclePosition.clear();
  m_RespiratoryCyclePosition.clear();
  m_DimensionIndexValues.clear();
  m_TemporalPositionIndex.clear();
  m_StackID.clear();
  m_InStackPositionNumber.clear();
  m_FrameComments.clear();
  m_FrameLabel.clear();
}


OFCondition FGFrameContent::check() const
{
  // Maybe add checks later
  return EC_Normal;
}


/// Read Frame Content Sequence from given item
OFCondition FGFrameContent::read(DcmItem& item)
{
  clearData();

  DcmItem* seqItem = NULL;
  OFCondition result = getItemFromFGSequence(item, DCM_FrameContentSequence, 0, seqItem);
  if (result.bad())
    return result;

  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_FrameAcquisitonNumber, "1", "3", "FrameContentMacro");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_FrameReferenceDateTime, "1", "1C", "FrameContentMacro");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_FrameAcquisitionDateTime, "1", "1C", "FrameContentMacro");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_FrameAcquisitionDuration, "1", "1C", "FrameContentMacro");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_CardiacCyclePosition, "1", "3", "FrameContentMacro");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_RespiratoryCyclePosition, "1", "3", "FrameContentMacro");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_DimensionIndexValues, "1-n", "1C", "FrameContentMacro");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_TemporalPositionIndex, "1", "1C", "FrameContentMacro");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_StackID, "1", "1C", "FrameContentMacro");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_InStackPositionNumber, "1", "1C", "FrameContentMacro");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_FrameComments, "1", "3", "FrameContentMacro");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_FrameLabel, "1", "3", "FrameContentMacro");

  return EC_Normal;

}

/// Writes single Frame Content Sequence into given item
OFCondition FGFrameContent::write(DcmItem& item)
{
  DcmItem *seqItem = NULL;
  OFCondition result = createNewFGSequence(item, DCM_FrameContentSequence, 0, seqItem);
  if (result.bad())
    return result;

  // --- write frame content macro attributes ---
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_FrameAcquisitonNumber, "1", "3", "FrameContentMacro");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_FrameReferenceDateTime, "1", "1C", "FrameContentMacro");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_FrameAcquisitionDateTime, "1", "1C", "FrameContentMacro");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_FrameAcquisitionDuration, "1", "1C", "FrameContentMacro");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_CardiacCyclePosition, "1", "3", "FrameContentMacro");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_RespiratoryCyclePosition, "1", "3", "FrameContentMacro");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_DimensionIndexValues, "1-n", "1C", "FrameContentMacro");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_TemporalPositionIndex, "1", "1C", "FrameContentMacro");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_StackID, "1", "1C", "FrameContentMacro");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_InStackPositionNumber, "1", "1C", "FrameContentMacro");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_FrameComments, "1", "3", "FrameContentMacro");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_FrameLabel, "1", "3", "FrameContentMacro");
  return result;
}


int FGFrameContent::compare(const FGBase& rhs) const
{
  int result = FGBase::compare(rhs);
  if (result != 0)
    return result;

  const FGFrameContent* myRhs = OFstatic_cast(const FGFrameContent*, &rhs);

  // Compare all elements
  result = m_FrameAcquisitonNumber.compare(myRhs->m_FrameAcquisitonNumber);
  if (result == 0)
    result = m_FrameReferenceDateTime.compare(myRhs->m_FrameReferenceDateTime);
  if (result == 0)
    result = m_FrameAcquisitionDateTime.compare(myRhs->m_FrameAcquisitionDateTime);
  if (result == 0)
    result = m_FrameAcquisitionDuration.compare(myRhs->m_FrameAcquisitionDuration);
  if (result == 0)
    result = m_CardiacCyclePosition.compare(myRhs->m_CardiacCyclePosition);
  if (result == 0)
    result = m_RespiratoryCyclePosition.compare(myRhs->m_RespiratoryCyclePosition);
  if (result == 0)
    result = m_DimensionIndexValues.compare(myRhs->m_DimensionIndexValues);
  if (result == 0)
    result = m_TemporalPositionIndex.compare(myRhs->m_TemporalPositionIndex);
  if (result == 0)
    result = m_StackID.compare(myRhs->m_StackID);
  if (result == 0)
    result = m_InStackPositionNumber.compare(myRhs->m_InStackPositionNumber);
  if (result == 0)
    result = m_FrameComments.compare(myRhs->m_FrameComments);
  if (result == 0)
    result = m_FrameLabel.compare(myRhs->m_FrameLabel);

  return result;
}


// --- get() functionality ---

OFCondition FGFrameContent::getFrameAcquisitionNumber(Uint16& value,
                                                      const signed long pos)
{
  return m_FrameAcquisitonNumber.getUint16(value, pos);
}

OFCondition FGFrameContent::getFrameReferenceDateTime(OFString& value,
                                                      const signed long pos)
{
  return DcmIODUtil::getStringValueFromElement(m_FrameReferenceDateTime, value, pos);
}

OFCondition FGFrameContent::getFrameAcquisitionDateTime(OFString& value,
                                                        const signed long pos)
{
  return DcmIODUtil::getStringValueFromElement(m_FrameAcquisitionDateTime, value, pos);
}

OFCondition FGFrameContent::getFrameAcquisitionDuration(Float64& value,
                                                        const unsigned long pos)
{
  return m_FrameAcquisitionDuration.getFloat64(value, pos);
}

OFCondition FGFrameContent::getCardiacCyclePosition(OFString& value,
                                                    const signed long pos)
{
  return DcmIODUtil::getStringValueFromElement(m_CardiacCyclePosition, value, pos);
}

OFCondition FGFrameContent::getRespiratoryCyclePosition(OFString& value,
                                                        const signed long pos)
{
  return DcmIODUtil::getStringValueFromElement(m_RespiratoryCyclePosition, value, pos);
}

OFCondition FGFrameContent::getDimensionIndexValues(Uint32& value,
                                                    const signed long pos)
{
  return m_DimensionIndexValues.getUint32(value, pos);
}

OFCondition FGFrameContent::getTemporalPositionIndex(Uint32& value,
                                                     const signed long pos)
{
  return m_TemporalPositionIndex.getUint32(value, pos);
}

OFCondition FGFrameContent::getStackID(OFString& value,
                                       const signed long pos)
{
  return DcmIODUtil::getStringValueFromElement(m_StackID, value, pos);
}

OFCondition FGFrameContent::getInStackPositionNumber(Uint32& value,
                                                     const long signed int pos)
{
  return m_InStackPositionNumber.getUint32(value, pos);
}


OFCondition FGFrameContent::getFrameComments(OFString& value,
                                             const signed long pos)
{
  return DcmIODUtil::getStringValueFromElement(m_FrameComments, value, pos);
}

OFCondition FGFrameContent::getFrameLabel(OFString& value,
                                          const signed long pos)
{
  return DcmIODUtil::getStringValueFromElement(m_FrameLabel, value, pos);
}

// --- set() functionality ---

OFCondition FGFrameContent::setFrameAcquisitionNumber(const Uint16& value,
                                                      const OFBool checkValue)
{
  (void)checkValue;
  m_FrameAcquisitonNumber.putUint16(value);
  return EC_Normal;
}

OFCondition FGFrameContent::setFrameReferenceDateTime(const OFString& value,
                                                      const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_FrameReferenceDateTime.putOFStringArray(value);
  return result;
}

OFCondition FGFrameContent::setFrameAcquisitionDateTime(const OFString& value,
                                                        const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_FrameAcquisitionDateTime.putOFStringArray(value);
  return result;
}

OFCondition FGFrameContent::setFrameAcquisitionDuration(const Float64& value,
                                                        const OFBool checkValue)
{
  (void)checkValue;
  // basic checking always included
  return m_FrameAcquisitionDuration.putFloat64(value);
}

OFCondition FGFrameContent::setCardiacCyclePosition(const OFString& value,
                                                    const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_CardiacCyclePosition.putOFStringArray(value);
  return result;
}

OFCondition FGFrameContent::setRespiratoryCyclePosition(const OFString& value,
                                                        const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_RespiratoryCyclePosition.putOFStringArray(value);
  return result;
}

OFCondition FGFrameContent::setDimensionIndexValues(const Uint32& value,
                                                    const unsigned int dim,
                                                    const OFBool checkValue)
{
  // no other meaningful checks possible in this context
  (void)checkValue;
  if (value == 0)
  {
    DCMFG_ERROR("Cannot set dimension index value 0, must be >= 1)");
    return EC_InvalidValue;
  }
  return m_DimensionIndexValues.putUint32(value, dim);
}

OFCondition FGFrameContent::setTemporalPositionIndex(const Uint32& value,
                                                     const OFBool checkValue)
{
  // no meaningful check possible in this context
  (void)checkValue;
  return m_TemporalPositionIndex.putUint32(value);
}

OFCondition FGFrameContent::setStackID(const OFString& value,
                                       const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_StackID.putOFStringArray(value);
  return result;
}


OFCondition FGFrameContent::setInStackPositionNumber(const Uint32& value,
                                                     const OFBool checkValue)
{
  // no meaningful check possible in this context
  (void)checkValue;
  return m_InStackPositionNumber.putUint32(value);
}


OFCondition FGFrameContent::setFrameComments(const OFString& value,
                                             const OFBool checkValue)
{
  (void)checkValue;
  return m_FrameComments.putOFStringArray(value);
}


OFCondition FGFrameContent::setFrameLabel(const OFString& value,
                                          const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_FrameLabel.putOFStringArray(value);
  return result;
}

