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
 *  Purpose: Class for managing Plane Position (Patient) Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmfg/fgplanpo.h"
#include "dcmtk/dcmfg/fgfact.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmiod/iodutil.h"

FGPlanePosPatient::FGPlanePosPatient() :
  FGBase(DcmFGTypes::EFG_PLANEPOSPATIENT),
  m_ImagePositionPatient(DCM_ImagePositionPatient)
{
}

FGBase* FGPlanePosPatient::clone() const
{
  FGPlanePosPatient* copy = new FGPlanePosPatient();
  if (copy)
  {
    copy->m_ImagePositionPatient = this->m_ImagePositionPatient;
  }
  return copy;
}


FGPlanePosPatient* FGPlanePosPatient::createMinimal(const OFString& imagePositionPatientX,
                                                    const OFString& imagePositionPatientY,
                                                    const OFString& imagePositionPatientZ)
{
  FGPlanePosPatient *fg = OFstatic_cast(FGPlanePosPatient*, FGFactory::instance().create(DcmFGTypes::EFG_PLANEPOSPATIENT));
  if (fg)
  {
    OFCondition result = fg->setImagePositionPatient(imagePositionPatientX, imagePositionPatientY, imagePositionPatientZ);
    if (result.bad())
    {
      DCMFG_ERROR("Could not create new FGPlanePosPatient: Invalid data for Image Position Patient: " << result.text());
      delete fg;
      return NULL;
    }
  }
  return fg;
}



/// clear old values
void FGPlanePosPatient::clearData()
{
  m_ImagePositionPatient.clear();
}


OFCondition FGPlanePosPatient::check() const
{
  // Checks in read() and write() are sufficient for now
  return EC_Normal;
}


/// Read from Plane Position Sequence
OFCondition FGPlanePosPatient::read(DcmItem& item)
{
  clearData();

  DcmItem* seqItem = NULL;
  OFCondition result =  getItemFromFGSequence(item, DCM_PlanePositionSequence, 0, seqItem);
  if (result.bad())
    return result;

  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_ImagePositionPatient,   "3", "1C", "PlanePositionSequence");

  return EC_Normal;
}


OFCondition FGPlanePosPatient::write(DcmItem& item)
{
  DcmItem* seqItem = NULL;
  OFCondition result = createNewFGSequence(item, DCM_PlanePositionSequence, 0, seqItem);
  if (result.bad())
    return result;

  // --- write plane position (patient) attributes ---
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_ImagePositionPatient, "3", "1C", "PlanePositionSequence");

  return result;
}


int FGPlanePosPatient::compare(const FGBase& rhs) const
{
  int result = FGBase::compare(rhs);
  if (result == 0)
  {
    const FGPlanePosPatient* myRhs = OFstatic_cast(const FGPlanePosPatient*, &rhs);

    // Compare all elements
    result = m_ImagePositionPatient.compare(myRhs->m_ImagePositionPatient);
  }

  return result;
}



OFCondition FGPlanePosPatient::getImagePositionPatient(OFString& value,
                                                       const signed long pos = 0)

{
  return DcmIODUtil::getStringValueFromElement(m_ImagePositionPatient, value, pos);
}


OFCondition FGPlanePosPatient::getImagePositionPatient(Float64& coordinateX,
                                                       Float64& coordinateY,
                                                       Float64& coordinateZ)
{
  OFCondition result = m_ImagePositionPatient.getFloat64(coordinateX, 0);
  if (result.good()) result = m_ImagePositionPatient.getFloat64(coordinateY, 1);
  if (result.good()) result = m_ImagePositionPatient.getFloat64(coordinateZ, 2);
  return result;
}


OFCondition FGPlanePosPatient::setImagePositionPatient(const OFString& coordinateX,
                                                       const OFString& coordinateY,
                                                       const OFString& coordinateZ,
                                                       const OFBool checkValue)

{

  OFString str(coordinateX);
  str += "\\";
  str += coordinateY;
  str += "\\";
  str += coordinateZ;
  OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(str, "3") : EC_Normal;
  if (result.good())
    result = m_ImagePositionPatient.putOFStringArray(str);
  return result;
}


FGPlanePosPatient::~FGPlanePosPatient()
{
  // nothing to do
}
