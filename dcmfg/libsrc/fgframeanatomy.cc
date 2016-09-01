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
 *  Purpose: Class for managing Frame Anatomy Functional Group
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmfg/fgframeanatomy.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmiod/iodutil.h"


FGFrameAnatomy::FGFrameAnatomy()
: FGBase(DcmFGTypes::EFG_FRAMEANATOMY),
  m_FrameLaterality(LATERALITY_UNDEFINED),
  m_Anatomy("1" /* Use Mandatory version of macro, i.e. make "Anatomic Region Sequence type 1" */)
{
}


FGFrameAnatomy::~FGFrameAnatomy()
{
  // nothing to do
}


FGBase* FGFrameAnatomy::clone() const
{
  FGFrameAnatomy* copy = new FGFrameAnatomy();
  if (copy)
  {
    copy->m_FrameLaterality= this->m_FrameLaterality;
    copy->m_Anatomy = this->m_Anatomy;
  }
  return copy;
}


void FGFrameAnatomy::clearData()
{
  m_FrameLaterality = LATERALITY_UNDEFINED;
  m_Anatomy.clearData();
}


OFCondition FGFrameAnatomy::check() const
{
  if (!isLateralityValid(m_FrameLaterality))
  {
    DCMFG_ERROR("Frame Laterality invalid");
    return FG_EC_InvalidData;
  }
  return OFconst_cast(GeneralAnatomyMacro*, &m_Anatomy)->check();
}


int FGFrameAnatomy::compare(const FGBase& rhs) const
{
  int result = FGBase::compare(rhs);
  if (result != 0)
    return result;

  const FGFrameAnatomy* myRhs = OFstatic_cast(const FGFrameAnatomy*, &rhs);

  // Compare all elements
  if (m_FrameLaterality != myRhs->m_FrameLaterality)
    return 1;

  return m_Anatomy.compare(myRhs->m_Anatomy);
}


/// Read from Frame Content Sequence
OFCondition FGFrameAnatomy::read(DcmItem& item)
{
  clearData();

  OFCondition result;
  DcmItem* seqItem = NULL;
  result = getItemFromFGSequence(item, DCM_FrameAnatomySequence, 0, seqItem);
  if (result.bad())
    return result;

  DcmCodeString elem(DCM_FrameLaterality);
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, elem,   "1", "1", "FrameAnatomyMacro");
  OFString val;
  elem.getOFStringArray(val);
  m_FrameLaterality = str2Laterality(val);

  m_Anatomy.read(*seqItem);

  return EC_Normal;
}


OFCondition FGFrameAnatomy::write(DcmItem& item)
{
  OFCondition result = check();
  if (result.good())
  {
    DcmItem* seqItem = NULL;
    OFCondition result = createNewFGSequence(item, DCM_FrameAnatomySequence, 0, seqItem);
    if (result.good())
    {
      OFString lat = laterality2Str(m_FrameLaterality);
      result = seqItem->putAndInsertOFStringArray(DCM_FrameLaterality, lat);
      if (result.good())
      {
        result = m_Anatomy.write(*seqItem);
      }
    }
  }

  return result;
}


OFCondition FGFrameAnatomy::getLaterality(FGFrameAnatomy::LATERALITY& value)
{
  value = m_FrameLaterality;
  return EC_Normal;
}


GeneralAnatomyMacro& FGFrameAnatomy::getAnatomy()
{
  return m_Anatomy;
}


OFCondition FGFrameAnatomy::setLaterality(const FGFrameAnatomy::LATERALITY& value)
{
  if (isLateralityValid(value))
  {
    m_FrameLaterality = value;
    return EC_Normal;
  }
  else
  {
    return FG_EC_InvalidData;
  }
}


OFString FGFrameAnatomy::laterality2Str(const FGFrameAnatomy::LATERALITY lat)
{
  switch(lat)
  {
    case LATERALITY_INVALID: return "Invalid";
    case LATERALITY_L: return "L";
    case LATERALITY_R: return "R";
    case LATERALITY_BOTH: return "B";
    case LATERALITY_UNDEFINED: return "Undefined";
    case LATERALITY_UNPAIRED: return "U";
    default: return "Invalid";
  }
}


FGFrameAnatomy::LATERALITY FGFrameAnatomy::str2Laterality(const OFString& lat)
{
  if (lat == "L")
    return LATERALITY_L;
  if (lat == "R")
    return LATERALITY_R;
  if (lat == "U")
    return LATERALITY_UNPAIRED;
  if (lat == "B")
    return LATERALITY_BOTH;

  if (lat.empty())
    return LATERALITY_UNDEFINED;

  return LATERALITY_INVALID;

}


OFBool FGFrameAnatomy::isLateralityValid(const FGFrameAnatomy::LATERALITY lat)
{
  if ( (lat == LATERALITY_L) || (lat == LATERALITY_R) || (lat == LATERALITY_BOTH) ||
       (lat == LATERALITY_UNPAIRED) )
    return OFTrue;
  else
    return OFFalse;
}

