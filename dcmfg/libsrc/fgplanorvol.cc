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
 *  Purpose: Class for managing Plane Orientation (Volume) Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcvrfd.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmfg/fgplanorvol.h"
#include "dcmtk/dcmfg/fgtypes.h"


// Constructor
FGPlaneOrientationVolume::FGPlaneOrientationVolume() :
  FGBase(DcmFGTypes::EFG_PLANEORIENTVOLUME),
  m_ImageOrientationVolume(DCM_ImageOrientationVolume)
{
}


FGPlaneOrientationVolume::~FGPlaneOrientationVolume()
{
  // nothing to do
}


FGBase* FGPlaneOrientationVolume::clone() const
{
  FGPlaneOrientationVolume* copy = new FGPlaneOrientationVolume();
  if (copy)
  {
    copy->m_ImageOrientationVolume = this->m_ImageOrientationVolume;
  }
  return copy;
}


void FGPlaneOrientationVolume::clearData()
{
  m_ImageOrientationVolume.clear();
}


OFCondition FGPlaneOrientationVolume::check() const
{
  // Checks in read() and write() are sufficient for now
  return EC_Normal;
}


// --- get() functionality ---

OFCondition FGPlaneOrientationVolume::getImageOrientationVolume(Float64& value,
                                                                const long unsigned int pos)
{
  return m_ImageOrientationVolume.getFloat64(value, pos);
}



OFCondition FGPlaneOrientationVolume::getImageOrientationVolume(Float64& rowX,
                                                                Float64& rowY,
                                                                Float64& rowZ,
                                                                Float64& colX,
                                                                Float64& colY,
                                                                Float64& colZ)
{
  OFCondition result = m_ImageOrientationVolume.getFloat64(rowX, 0);
  if (result.good()) result = m_ImageOrientationVolume.getFloat64(rowY, 1);
  if (result.good()) result = m_ImageOrientationVolume.getFloat64(rowZ, 2);
  if (result.good()) result = m_ImageOrientationVolume.getFloat64(colX, 3);
  if (result.good()) result = m_ImageOrientationVolume.getFloat64(colY, 4);
  if (result.good()) result = m_ImageOrientationVolume.getFloat64(colZ, 5);
  return result;
}


// --- set() functionality ---


OFCondition FGPlaneOrientationVolume::setImageOrientationVolume(const Float64& rowX,
                                                                const Float64& rowY,
                                                                const Float64& rowZ,
                                                                const Float64& colX,
                                                                const Float64& colY,
                                                                const Float64& colZ,
                                                                const OFBool)
{
  OFCondition result = m_ImageOrientationVolume.putFloat64(rowX, 0);
  if (result.good()) m_ImageOrientationVolume.putFloat64(rowY, 1);
  if (result.good()) m_ImageOrientationVolume.putFloat64(rowZ, 2);
  if (result.good()) m_ImageOrientationVolume.putFloat64(colX, 3);
  if (result.good()) m_ImageOrientationVolume.putFloat64(colY, 4);
  if (result.good()) m_ImageOrientationVolume.putFloat64(colZ, 5);
  return result;
}


/// Read Plane Orientation (Volume) Sequence from given item
OFCondition FGPlaneOrientationVolume::read(DcmItem& item)
{
  clearData();

  DcmItem* seqItem = NULL;
  OFCondition result = getItemFromFGSequence(item, DCM_PlaneOrientationVolumeSequence, 0, seqItem);
  if (result.bad())
    return result;

  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_ImageOrientationVolume, "6", "1", "PlaneOrientationVolume");

  return EC_Normal;

}

/// Writes single Plane Orientation (Volume) Sequence into given item
OFCondition FGPlaneOrientationVolume::write(DcmItem& item)
{
  DcmItem *seqItem = NULL;
  OFCondition result = createNewFGSequence(item, DCM_PlaneOrientationVolumeSequence, 0, seqItem);
  if (result.bad())
    return result;

  // --- write frame content macro attributes ---
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_ImageOrientationVolume, "6", "1", "PlaneOrientationVolume");
  return result;
}


int FGPlaneOrientationVolume::compare(const FGBase& rhs) const
{
  int result = FGBase::compare(rhs);
  if (result != 0)
    return result;

  const FGPlaneOrientationVolume* myRhs = OFstatic_cast(const FGPlaneOrientationVolume*, &rhs);

  // Compare all elements
  result = m_ImageOrientationVolume.compare(myRhs->m_ImageOrientationVolume);
  return result;
}



