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
 *  Purpose: Class for managing Segmentation Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmfg/fgseg.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmiod/iodutil.h" // for static element helpers


FGSegmentation::FGSegmentation() :
  FGBase(DcmFGTypes::EFG_SEGMENTATION),
  m_ReferencedSegmentNumber(DCM_ReferencedSegmentNumber)
{
}


FGSegmentation::~FGSegmentation()
{
  clearData();
}

FGBase* FGSegmentation::clone() const
{
  FGSegmentation* copy = new FGSegmentation();
  if (copy)
  {
    copy->m_ReferencedSegmentNumber = this->m_ReferencedSegmentNumber;
  }
  return copy;
}



void FGSegmentation::clearData()
{
  m_ReferencedSegmentNumber.clear();
}


OFCondition FGSegmentation::check() const
{
  // Checks in read() and write() are sufficient for now
  return EC_Normal;
}


OFCondition FGSegmentation::getReferencedSegmentNumber(Uint16& value,
                                                       const unsigned long pos)
{
  return m_ReferencedSegmentNumber.getUint16(value, pos);
}


OFCondition FGSegmentation::setReferencedSegmentNumber(const Uint16& segmentNumber)
{
  return m_ReferencedSegmentNumber.putUint16(segmentNumber);
}


OFCondition FGSegmentation::write(DcmItem& item)
{
  OFCondition result = EC_Normal;

  /* Clear old data */
  item.findAndDeleteElement(DCM_SegmentIdentificationSequence);

  /* write and get Segment Identification Sequence with one item */
  DcmItem *newItem = NULL;
  result = item.findOrCreateSequenceItem(DCM_SegmentIdentificationSequence, newItem, 0);
  if ( result.bad() )
  {
    DCMFG_ERROR("Could not create Segment Identification Sequence with one item (internal error): " << result.text());
    return FG_EC_CouldNotWriteFG;
  }
  DcmIODUtil::copyElementToDataset(result, *newItem, m_ReferencedSegmentNumber, "1-n", "1", "SegmentationMacro");

  /* Return result */
  return result;
}


OFCondition FGSegmentation::read(DcmItem& item)
{
  OFCondition result = EC_Normal;

  /* re-initialize object */
  clearData();

  DcmSequenceOfItems *segmentIdentificationSequence = NULL;
  result = item.findAndGetSequence(DCM_SegmentIdentificationSequence, segmentIdentificationSequence);

  if ( result.good() )
  {
    /* Referenced Segment Number */
    // check whether structure is ok (single item in sequence) and report problems
    DcmItem *idItem= NULL;
    DcmIODUtil::getAndCheckSingleItem(*segmentIdentificationSequence, idItem, DCM_SegmentIdentificationSequence);
    if (idItem)
    {
      // so far we ignore the result when reading (but report problems to the logger)
      DcmIODUtil::getAndCheckElementFromDataset(*idItem, m_ReferencedSegmentNumber, "1-n" /* vm */, "1" /* type */, "SegmentationMacro");
    }
  }
  return result;
}


int FGSegmentation::compare(const FGBase& rhs) const
{
  int result = FGBase::compare(rhs);
  if (result == 0)
  {
    const FGSegmentation* myRhs = OFstatic_cast(const FGSegmentation*, &rhs);

    // Compare all elements
    result = m_ReferencedSegmentNumber.compare(myRhs->m_ReferencedSegmentNumber);
  }

  return result;
}
