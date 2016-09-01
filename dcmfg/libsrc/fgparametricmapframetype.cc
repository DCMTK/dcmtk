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
 *  Module: dcmfg
 *
 *  Author: Jan Schlamelcher
 *
 *  Purpose: Class for managing the Parametric Map Frame Type
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmfg/fgparametricmapframetype.h"

FGParametricMapFrameType::FGParametricMapFrameType()
: FGBase(DcmFGTypes::EFG_PARAMETRICMAPFRAMETYPE)
, m_FrameType(DCM_FrameType)
{

}

FGParametricMapFrameType::~FGParametricMapFrameType()
{

}

void FGParametricMapFrameType::clearData()
{
  m_FrameType.clear();
}

FGBase* FGParametricMapFrameType::clone() const
{
  if(FGParametricMapFrameType* copy = new FGParametricMapFrameType)
  {
    copy->m_FrameType = m_FrameType;
    return copy;
  }
  return OFnullptr;
}

OFCondition FGParametricMapFrameType::read(DcmItem& item)
{
  clearData();

  DcmItem* seqItem;
  OFCondition result;

  seqItem = OFnullptr;
  result = getItemFromFGSequence(item, DCM_ParametricMapFrameTypeSequence, 0, seqItem);
  if(result.bad())
    return result;
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_FrameType, "4", "1", "Parametric Map Frame Type");

  return EC_Normal;
}


OFCondition FGParametricMapFrameType::write(DcmItem& item)
{
  OFCondition result = check();
  if (result.good())
  {
    DcmItem* seqItem;
    seqItem = OFnullptr;
    result = createNewFGSequence(item, DCM_ParametricMapFrameTypeSequence, 0, seqItem);
    if(result.good())
    {
      DcmIODUtil::copyElementToDataset(result, *seqItem, m_FrameType, "4", "1", "Parametric Map Frame Type");
    }
  }
  return result;
}


int FGParametricMapFrameType::compare(const FGBase& rhs) const
{
  int result = FGBase::compare(rhs);
  if(result == 0)
  {
    const FGParametricMapFrameType* myRhs = OFstatic_cast(const FGParametricMapFrameType*, &rhs);

    // Compare all elements
    result = m_FrameType.compare(myRhs->m_FrameType);
  }

  return result;
}


OFCondition FGParametricMapFrameType::check() const
{
  DcmCodeString myFrameType = m_FrameType;
  OFCondition result = myFrameType.checkValue("4");
  if (result.good())
  {
    OFString val;
    myFrameType.getOFString(val, 0);
    if (val == "DERIVED")
    {
      val.clear();
      myFrameType.getOFString(val, 1);
      if (val == "PRIMARY")
      {
        val.clear();
        myFrameType.getOFString(val, 1);
        return EC_Normal;
      }
      else
        DCMFG_ERROR("Frame Type' 2nd value must be \"PRIMARY\" but is \"" << val << "\"");
    }
    else
      DCMFG_ERROR("Frame Type 1st value must be \"DERIVED\" but is \"" << val << "\"");
  }
  return FG_EC_InvalidData;
}


OFCondition FGParametricMapFrameType::getFrameType(OFString &value,
                                                   const signed long pos) const
{
  return DcmIODUtil::getStringValueFromElement(m_FrameType, value, pos);
}


OFCondition FGParametricMapFrameType::setFrameType(const OFString &value,
                                                   const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "4") : EC_Normal;
  if (result.good())
    result = m_FrameType.putString(value.c_str());
  return result;
}
