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
 *  Purpose: Class for managing Plane Orientation (Patient) Functional Group
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmfg/fgplanor.h"
#include "dcmtk/dcmfg/fgfact.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmiod/iodutil.h"


FGPlaneOrientationPatient::FGPlaneOrientationPatient() :
  FGBase(DcmFGTypes::EFG_PLANEORIENTPATIENT),
  m_ImageOrientationPatient(DCM_ImageOrientationPatient)
{
}

/// clear old values
void FGPlaneOrientationPatient::clearData()
{
  m_ImageOrientationPatient.clear();
}


OFCondition FGPlaneOrientationPatient::check() const
{
  // Checks in read() and write() are sufficient for now
  return EC_Normal;
}


FGBase* FGPlaneOrientationPatient::clone() const
{
  FGPlaneOrientationPatient* copy = new FGPlaneOrientationPatient();
  if (copy)
  {
    copy->m_ImageOrientationPatient = this->m_ImageOrientationPatient;
  }
  return copy;
}


FGPlaneOrientationPatient* FGPlaneOrientationPatient::createMinimal(const OFString& imageOrientationPatientRowX,
                                                                    const OFString& imageOrientationPatientRowY,
                                                                    const OFString& imageOrientationPatientRowZ,
                                                                    const OFString& imageOrientationPatientColX,
                                                                    const OFString& imageOrientationPatientColY,
                                                                    const OFString& imageOrientationPatientColZ)
{
  FGPlaneOrientationPatient *fg = OFstatic_cast(FGPlaneOrientationPatient*, FGFactory::instance().create(DcmFGTypes::EFG_PLANEORIENTPATIENT));
  if (fg)
  {
    OFCondition result = fg->setImageOrientationPatient(imageOrientationPatientRowX,
                                                        imageOrientationPatientRowY,
                                                        imageOrientationPatientRowZ,
                                                        imageOrientationPatientColX,
                                                        imageOrientationPatientColY,
                                                        imageOrientationPatientColZ);
    if (result.bad())
    {
      DCMFG_ERROR("Could not create new FGPlaneOrientationPatient: Invalid data for Image Orientation Patient: " << result.text());
      delete fg;
      return NULL;
    }
  }
  return fg;
}



/// Read from Plane Position Sequence
OFCondition FGPlaneOrientationPatient::read(DcmItem& item)
{
  clearData();

  DcmItem *seqItem = NULL;
  OFCondition result = getItemFromFGSequence(item, DCM_PlaneOrientationSequence, 0, seqItem);
  if (result.bad())
    return result;

  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_ImageOrientationPatient,   "6", "1C", "PlaneOrientationPatient");

  return EC_Normal;
}


OFCondition FGPlaneOrientationPatient::write(DcmItem& item)
{
  DcmItem *seqItem = NULL;
  OFCondition result = createNewFGSequence(item, DCM_PlaneOrientationSequence, 0, seqItem);
  if (result.bad())
    return result;

  // --- write plane position (patient) attributes ---
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_ImageOrientationPatient, "6", "1C", "PlaneOrientationPatient");

  return result;
}



int FGPlaneOrientationPatient::compare(const FGBase& rhs) const
{
  int result = FGBase::compare(rhs);
  if (result == 0)
  {
    const FGPlaneOrientationPatient* myRhs = OFstatic_cast(const FGPlaneOrientationPatient*, &rhs);

    // Compare all elements
    result = m_ImageOrientationPatient.compare(myRhs->m_ImageOrientationPatient);
  }

  return result;
}


OFCondition FGPlaneOrientationPatient::getImageOrientationPatient(OFString& value,
                                                                  const signed long pos = 0)

{
  return DcmIODUtil::getStringValueFromElement(m_ImageOrientationPatient, value, pos);
}


OFCondition FGPlaneOrientationPatient::getImageOrientationPatient(Float64& rowX,
                                                                  Float64& rowY,
                                                                  Float64& rowZ,
                                                                  Float64& colX,
                                                                  Float64& colY,
                                                                  Float64& colZ)
{
  OFCondition result = m_ImageOrientationPatient.getFloat64(rowX, 0);
  if (result.good()) m_ImageOrientationPatient.getFloat64(rowY, 1);
  if (result.good()) m_ImageOrientationPatient.getFloat64(rowZ, 2);
  if (result.good()) m_ImageOrientationPatient.getFloat64(colX, 3);
  if (result.good()) m_ImageOrientationPatient.getFloat64(colY, 4);
  if (result.good()) m_ImageOrientationPatient.getFloat64(colZ, 5);
  return result;
}


OFCondition FGPlaneOrientationPatient::setImageOrientationPatient(const OFString& rowX,
                                                                  const OFString& rowY,
                                                                  const OFString& rowZ,
                                                                  const OFString& colX,
                                                                  const OFString& colY,
                                                                  const OFString& colZ,
                                                                  const OFBool checkValue)
{
  OFString str(rowX); str += "\\";
  str += rowY; str += "\\";
  str += rowZ; str += "\\";
  str += colX; str += "\\";
  str += colY; str += "\\";
  str += colZ;

  OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(str, "6") : EC_Normal;
  if (result.good())
    result = m_ImageOrientationPatient.putOFStringArray(str);
  return result;
}


FGPlaneOrientationPatient::~FGPlaneOrientationPatient()
{
  // nothing to do
}
