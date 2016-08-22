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
 *  Purpose: Class for managing Functional Groups from a DICOM object
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmfg/fg.h"


FunctionalGroups::FunctionalGroups()
: m_groups()
{
}


FunctionalGroups::~FunctionalGroups()
{
  clear();
}


void FunctionalGroups::clear()
{
  // free memory of underlying map
  while ( m_groups.size() > 0)
  {
    FunctionalGroups::iterator it = m_groups.begin();
    FGBase* fg = (*it).second;
    m_groups.erase(it);
    delete fg;
  }
}



FGBase* FunctionalGroups::find(const DcmFGTypes::E_FGType fgType)
{
  FGBase* group = NULL;
  FunctionalGroups::iterator it = m_groups.find(fgType);
  FunctionalGroups::iterator end = m_groups.end();
  if ( it != end )
  {
    group = (*it).second;
  }
  return group;
}


FunctionalGroups::iterator FunctionalGroups::begin()
{
  return  m_groups.begin();
}


FunctionalGroups::iterator FunctionalGroups::end()
{
  return m_groups.end();
}


FunctionalGroups::const_iterator FunctionalGroups::begin() const
{
  return m_groups.begin();
}


FunctionalGroups::const_iterator FunctionalGroups::end() const
{
  return m_groups.end();
}


OFCondition FunctionalGroups::insert(FGBase* group,
                                     const OFBool replaceOld)
{
  if (group == NULL)
    return EC_IllegalParameter;

  OFCondition result;
  FGBase* existing = find(group->getType());
  if (existing)
  {
    if (replaceOld)
    {
      DCMFG_DEBUG("Replacing existing functional group");
      delete remove(group->getType());
    }
    else
    {
      DCMFG_ERROR("Cannot insert functional group: Group does already exist");
      result = FG_EC_DoubledFG;
    }
  }
  if (result.good())
  {
    if ( (m_groups.insert(OFMake_pair(group->getType(), group)).second) )
    {
      DCMFG_DEBUG("Functional group successfully inserted: " << DcmFGTypes::FGType2OFString(group->getType()));
    }
    else
    {
      DCMFG_ERROR("Cannot insert functional group: Internal error");
      result = FG_EC_CouldNotInsertFG;
    }
  }
  return result;
}


FGBase* FunctionalGroups::remove(const DcmFGTypes::E_FGType fgType)
{
  FGBase* fg = find(fgType);
  m_groups.erase(fgType);
  return fg;
}

