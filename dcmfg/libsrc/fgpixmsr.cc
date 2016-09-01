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
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for managing Pixel Measures Functional Group
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmfg/fgpixmsr.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmiod/iodutil.h"


FGPixelMeasures::FGPixelMeasures()
: FGBase(DcmFGTypes::EFG_PIXELMEASURES),
  m_PixelSpacing(DCM_PixelSpacing),
  m_SliceThickness(DCM_SliceThickness),
  m_SpacingBetweenSlices(DCM_SpacingBetweenSlices)
{
}


FGPixelMeasures::~FGPixelMeasures()
{
  // nothing to do
}


FGBase* FGPixelMeasures::clone() const
{
  FGPixelMeasures* copy = new FGPixelMeasures();
  if (copy)
  {
    copy->m_PixelSpacing = this->m_PixelSpacing;
    copy->m_SliceThickness = this->m_SliceThickness;
    copy->m_SpacingBetweenSlices = this->m_SpacingBetweenSlices;
  }
  return copy;
}


void FGPixelMeasures::clearData()
{
  m_PixelSpacing.clear();
  m_SliceThickness.clear();
  m_SpacingBetweenSlices.clear();
}


OFCondition FGPixelMeasures::check() const
{
  // Checks in read() and write() are sufficient for now
  return EC_Normal;
}


int FGPixelMeasures::compare(const FGBase& rhs) const
{
  int result = FGBase::compare(rhs);
  if (result != 0)
    return result;

  const FGPixelMeasures* myRhs = OFstatic_cast(const FGPixelMeasures*, &rhs);

  // Compare all elements
  result = m_PixelSpacing.compare(myRhs->m_PixelSpacing);
  if (result == 0)
    result = m_SliceThickness.compare(myRhs->m_SliceThickness);
  if (result == 0)
    result = m_SpacingBetweenSlices.compare(myRhs->m_SpacingBetweenSlices);

  return result;
}


/// Read from Frame Content Sequence
OFCondition FGPixelMeasures::read(DcmItem& item)
{
  clearData();

  OFCondition result;
  DcmItem* seqItem = NULL;
  result = getItemFromFGSequence(item, DCM_PixelMeasuresSequence, 0, seqItem);
  if (result.bad())
    return result;

  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_PixelSpacing,   "2", "1C", "PixelMeasuresMacro");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_SliceThickness, "1", "1C", "PixelMeasuresMacro");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_SpacingBetweenSlices, "1", "3", "PixelMeasuresMacro");

  return EC_Normal;
}

OFCondition FGPixelMeasures::write(DcmItem& item)
{
  DcmItem* seqItem = NULL;
  OFCondition result = createNewFGSequence(item, DCM_PixelMeasuresSequence, 0, seqItem);
  if (result.bad())
    return result;

  // --- write frame content macro attributes ---
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_PixelSpacing, "2", "1C", "PixelMeasuresMacro");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_SliceThickness, "1", "1C", "PixelMeasuresMacro");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_SpacingBetweenSlices, "1", "3", "PixelMeasuresMacro");

  return result;
}


OFCondition FGPixelMeasures::getPixelSpacing(Float64& value,
                                             const signed long pos)
{
  return m_PixelSpacing.getFloat64(value, pos);
}


OFCondition FGPixelMeasures:: getSliceThickness(Float64& value,
                                                const signed long pos)
{
  return m_SliceThickness.getFloat64(value, pos);
}


OFCondition FGPixelMeasures:: getSpacingBetweenSlices(Float64& value,
                                                      const signed long pos)
{
  return m_SpacingBetweenSlices.getFloat64(value, pos);
}


OFCondition FGPixelMeasures::setPixelSpacing(const OFString& value,
                                             const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "2") : EC_Normal;
  if (result.good())
  {
    result = m_PixelSpacing.putOFStringArray(value);
  }
  return result;
}


OFCondition FGPixelMeasures::setSliceThickness(const OFString& value,
                                               const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
  {
    result = m_SliceThickness.putOFStringArray(value);
  }
  return result;
}

OFCondition FGPixelMeasures::setSpacingBetweenSlices(const OFString& value,
                                                     const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
  {
    result = m_SpacingBetweenSlices.putOFStringArray(value);
  }
  return result;
}
