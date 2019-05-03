/*
 *
 *  Copyright (C) 2015-2019, Open Connections GmbH
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
 *  Purpose: Main interface class for managing Functional Groups
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofmap.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/dcmiod/iodutil.h" // for static helpers
#include "dcmtk/dcmfg/fginterface.h"
#include "dcmtk/dcmfg/fg.h"
#include "dcmtk/dcmfg/fgfact.h"   // for creating new functional groups


FGInterface::FGInterface() :
m_shared(),
m_perFrame(),
m_checkOnWrite(OFTrue)
{
}


FGInterface::~FGInterface()
{
  clear();
}


void FGInterface::clear()
{
  // Clear per frame functional groups
  while (m_perFrame.size() > 0)
  {
    OFMap<Uint32, FunctionalGroups*>::iterator it = m_perFrame.begin();
    FunctionalGroups* fg = (*it).second;
    m_perFrame.erase(it);
    delete fg;
  }

  // Clear shared functional groups
  m_shared.clear();
}


size_t FGInterface::getNumberOfFrames()
{
  return m_perFrame.size();
}


OFCondition FGInterface::addShared(const FGBase& group)
{
  DcmFGTypes::E_FGSharedType sharedType = group.getSharedType();
  if ( sharedType == DcmFGTypes::EFGS_ONLYPERFRAME )
  {
    DCMFG_ERROR("Cannot add group as shared, per DICOM, group type " << DcmFGTypes::FGType2OFString(group.getType()) << " is always per-frame");
    return FG_EC_CouldNotAddFG;
  }

  // Delete all per frame groups of this type
  for (size_t count = 0; count < m_perFrame.size(); count++)
  {
    deletePerFrame(OFstatic_cast(Uint32, count), group.getType());
  }

  // Create copy for insertion
  FGBase* copy = group.clone();
  if (!copy)
  {
    return EC_MemoryExhausted;
  }

  // Insert shared one, replace old one if existing
  OFCondition result = insertShared(copy, OFTrue /* replace */);
  if (result.bad())
  {
    DCMFG_ERROR("Could not add shared group of type: " << DcmFGTypes::FGType2OFString(group.getType()));
    delete copy;
  }

  return result;
}


OFCondition FGInterface::addPerFrame(const Uint32 frameNo,
                                     const FGBase& group)
{
  OFCondition result = EC_Normal;
  DcmFGTypes::E_FGSharedType sharedType = group.getSharedType();
  if ( sharedType == DcmFGTypes::EFGS_ONLYSHARED)
  {
    DCMFG_ERROR("Cannot add group as per-frame, group type " << DcmFGTypes::FGType2OFString(group.getType()) << " is always shared");
    return FG_EC_CouldNotAddFG;
  }

  // Check whether there is already a shared group of this type.
  // If the content is equal to the given group, we re-use the shared one
  FGBase* shared = getShared(group.getType());
  // If there is a shared group
  if ( shared )
  {
    // If shared has identical values as given group, nothing has to be done.
    // Else if shared group with such type exists, but content differs,
    // we must the make the existing shared FG "per-frame", i.e. distribute
    // it to all frames, and add the given group for the given frame.
    if ( (*shared).compare(group) != 0 )
    {
      // We need to unshare this group, i.e. distribute it to frames
      DCMFG_DEBUG("Converting shared group of type " << DcmFGTypes::FGType2OFString(group.getType()) << " to per-frame, triggered by deviating per-frame insertion");
      result = convertSharedToPerFrame(group.getType());
    }
    else
    {
      DCMFG_DEBUG("Re-using shared group instead of adding per-frame for frame " << frameNo << ", type " << DcmFGTypes::FGType2OFString(group.getType()));
      return EC_Normal;
    }
  }

  if (result.good())
  {
    FGBase* copy = group.clone();
    if (!copy)
    {
      return EC_MemoryExhausted;
    }
    result = insertPerFrame(frameNo, copy);
    if (result.bad())
      delete copy;
  }

  return result;
}


// Get specific functional group for a frame,
// no matter whether it is stored per frame or shared
FGBase* FGInterface::get(const Uint32 frameNo,
                         const DcmFGTypes::E_FGType fgType)
{
  OFBool helpShared; // throw-away variable
  return get(frameNo, fgType, helpShared);
}


const FunctionalGroups* FGInterface::getPerFrame(const Uint32 frameNo) const
{
 if (frameNo > m_perFrame.size())
 {
   return NULL;
 }
 else
 {
   return (*(m_perFrame.find(frameNo))).second;
 }
}


const FunctionalGroups* FGInterface::getShared() const
{
  return &m_shared;
}


// Read enhanced multi-frame information from DICOM item, usually DcmDataset
OFCondition FGInterface::read(DcmItem& dataset)
{
  OFCondition result = EC_Normal;

  // clear any old values
  clear();

  /* read shared functional groups */
  if (result.good())
  {
    result = readSharedFG(dataset);
  }

  /* read per frame functional groups */
  if (result.good())
  {
    result = readPerFrameFG(dataset);
  }

  return result;

}


OFCondition FGInterface::readSharedFG(DcmItem& dataset)
{
  /* read shared functional groups */
  DcmSequenceOfItems *shared = NULL;
  OFCondition result = dataset.findAndGetSequence(DCM_SharedFunctionalGroupsSequence, shared);
  if (result.bad())
  {
    DCMFG_ERROR("Could not find Shared Functional Group Sequence");
    return FG_EC_NoSharedFG;
  }

  if (shared->card() > 1)
  {
    DCMFG_WARN("More than one item in Shared Functional Group Sequence, only considering the first one");
  }
  else if (shared->card() == 0)
  {
    DCMFG_WARN("No Item in Shared Functional Group Sequence but exactly one expected");
    return FG_EC_NoSharedFG;
  }

  // get the only item of shared functional group sequence
  DcmItem* sharedFGs = shared->getItem(0);
  // read all functional groups from shared fg sequence item
  result = readSingleFG(*sharedFGs, m_shared);

  return result;
}



OFCondition FGInterface::readPerFrameFG(DcmItem& dataset)
{
  /* read per-frame functional groups */
  DcmSequenceOfItems *perFrame = NULL;
  OFCondition result = dataset.findAndGetSequence(DCM_PerFrameFunctionalGroupsSequence, perFrame);
  if (result.bad())
  {
    DCMFG_ERROR("Could not find Per-Frame Functional Group Sequence");
    return FG_EC_NoPerFrameFG;
  }

  /* 1-n items required */
  size_t numFrames = perFrame->card();
  if (numFrames == 0)
  {
    DCMFG_WARN("No Item in Shared Functional Group Sequence but exactly one or more expected");
    return FG_EC_NoPerFrameFG;
  }

  /* Read functional groups for each item (one per frame) */
  DcmItem *oneFrameItem = OFstatic_cast(DcmItem*, perFrame->nextInContainer(NULL));
  Uint32 count = 0;
  while (oneFrameItem != NULL)
  {
    OFunique_ptr<FunctionalGroups> perFrameGroups(new FunctionalGroups());
    if (!oneFrameItem)
    {
      DCMFG_ERROR("Could not get functional group item for frame #" << count << " (internal error)");
    }
    else if (!perFrameGroups.get())
    {
      DCMFG_ERROR("Could not create functional groups for frame #" << count << ": Memory exhausted?");
    }
    else
    {
      result = readSingleFG(*oneFrameItem, *perFrameGroups);
      if (result.good())
      {
        if ( !m_perFrame.insert( OFMake_pair(count, perFrameGroups.release()) ).second )
        {
          DCMFG_ERROR("Could not store functional groups for frame #" << count << " (internal error)");
        }
      }
      else
      {
        DCMFG_ERROR("Could not read functional groups for frame #" << count << ": " << result.text());
      }
    }
    oneFrameItem = OFstatic_cast(DcmItem*, perFrame->nextInContainer(oneFrameItem));
    count++;
  }
  return EC_Normal; // for now we always return EC_Normal...
}



OFCondition FGInterface::readSingleFG(DcmItem& fgItem,
                                      FunctionalGroups& groups)
{
  OFCondition result;
  size_t card = fgItem.card();
  OFString fgname;
  for (size_t count = 0; count < card; count++)
  {
    DcmElement *elem = fgItem.getElement(OFstatic_cast(unsigned long, count));
    // TODO: non-sequence elements are not explicitly forbidden here(?), we could store them and re-store them later
    if (elem->getVR() != EVR_SQ)
    {
      DCMFG_WARN("Found non-sequence element in functional group sequence item (ignored): " << elem->getTag());
    }
    else
    {
      FGBase *fg = FGFactory::instance().create(elem->getTag());
      if (fg != NULL)
      {
        OFStringStream stream;
        stream << DcmFGTypes::tagKey2FGString(elem->getTag()) << " " << elem->getTag();
        OFSTRINGSTREAM_GETSTR(stream,tmpstr)
        fgname = tmpstr;
        OFSTRINGSTREAM_FREESTR(tmpstr)
        result = fg->read(fgItem);
        if (result.bad())
        {
          DCMFG_WARN("Cannot read functional group: " << fgname << " (ignored)");
        }
        // we also accept groups while reading which could instantiated but not could not be read
        result = groups.insert(fg, OFTrue);
        if (result.good())
        {
          DCMFG_DEBUG("Inserted functional group: " << fgname );
        }
        else
        {
          DCMFG_ERROR("Could not insert functional group: " << fgname << " (internal error)");
          delete fg;
        }
      }
      else
      {
        DCMFG_WARN("Cannot understand functional group for sequence tag: " << elem->getTag());
      }
    }
  }
  return EC_Normal; // for now we always return EC_Normal...
}


// Write enhanced multi-frame information to DICOM item, usually DcmDataset
OFCondition FGInterface::write(DcmItem& dataset)
{
  //Check data integrity of functional group macros */
  if (m_checkOnWrite)
  {
    if ( !check() )
      return FG_EC_CouldNotWriteFG;
  }

  // Write shared functional Groups
  OFCondition result = writeSharedFG(dataset);

  // Write per frame functional groups
  if ( result.good() ) result = writePerFrameFG(dataset);

  return result;
}


FGBase* FGInterface::getShared(const DcmFGTypes::E_FGType fgType)
{
  return m_shared.find(fgType);
}


OFCondition FGInterface::insertShared(FGBase* group,
                                      const OFBool replaceExisting)
{
  return m_shared.insert(group, replaceExisting);
}


FGBase* FGInterface::getPerFrame(const Uint32 frameNo,
                                 const DcmFGTypes::E_FGType fgType)
{
  FGBase* group = NULL;
  OFMap<Uint32, FunctionalGroups*>::iterator it = m_perFrame.find(frameNo);
  if ( it != m_perFrame.end() )
  {
    FunctionalGroups* perFrameGroups = (*it).second;
    group = perFrameGroups->find(fgType);
  }

  return group;
}


OFBool FGInterface::deleteShared(const DcmFGTypes::E_FGType fgType)
{
  FGBase* group = m_shared.find(fgType);
  if (group)
  {
    delete m_shared.remove(fgType);
    return OFTrue;
  }
  return OFFalse;
}



OFBool FGInterface::deletePerFrame(const Uint32 frameNo,
                                   const DcmFGTypes::E_FGType fgType)
{
  OFMap<Uint32, FunctionalGroups*>::iterator it = m_perFrame.find(frameNo);
  if (it != m_perFrame.end())
  {
    if ( (*it).second )
    {
      FGBase *remove = (*it).second->remove(fgType);
      if (remove)
      {
        DCMFG_DEBUG("Deleting FG for frame " << frameNo << ", type: " << DcmFGTypes::FGType2OFString(fgType));
        delete remove;
        remove = NULL;
        return OFTrue;
      }
    }
  }
  return OFFalse;
}


size_t FGInterface::deletePerFrame(const DcmFGTypes::E_FGType fgType)
{
  size_t numDeleted = 0;
  const size_t numFrames = m_perFrame.size();
  for (size_t frameNo = 0; frameNo < numFrames; frameNo++)
  {
    if (deletePerFrame(OFstatic_cast(Uint32, frameNo), fgType))
    {
      numDeleted++;
    }
  }
  return numDeleted;
}


size_t FGInterface::deleteFrame(const Uint32 frameNo)
{
  OFMap<Uint32, FunctionalGroups*>::iterator it = m_perFrame.find(frameNo);
  if (it != m_perFrame.end())
  {
    if ( (*it).second )
    {
      FunctionalGroups::iterator fg = (*it).second->begin();
      while (fg != (*it).second->end())
      {
        delete (*fg).second;
        fg++;
      }
    }
    m_perFrame.erase(it);
  }
  return OFFalse;
}


void FGInterface::setCheckOnWrite(const OFBool doCheck)
{
  m_checkOnWrite = doCheck;
}


OFBool FGInterface::getCheckOnWrite()
{
  return m_checkOnWrite;
}


FunctionalGroups* FGInterface::getOrCreatePerFrameGroups(const Uint32 frameNo)
{
  OFMap<Uint32, FunctionalGroups*>::iterator it = m_perFrame.find(frameNo);
  if (it != m_perFrame.end())
    return (*it).second;

  FunctionalGroups *fg = new FunctionalGroups();
  if (fg != NULL)
  {
    if ( !(m_perFrame.insert(OFMake_pair(frameNo, fg))).second )
    {
      DCMFG_ERROR("Could not insert Per-frame Functional Groups for frame " << frameNo << ": " << "Internal error");
      delete fg;
      fg = NULL;
    }
  }
  else
  {
    DCMFG_ERROR("Could not create Per-frame Functional Groups for frame " << frameNo << ": " << "Memory exhausted");
  }

  return fg;
}


OFCondition FGInterface::writePerFrameFG(DcmItem& dataset)
{
  DCMFG_DEBUG("Writing per-frame functional groups");
  OFCondition result = dataset.insertEmptyElement(DCM_PerFrameFunctionalGroupsSequence, OFTrue); // start with empty sequence
  if (result.bad())
  {
    DCMFG_ERROR("Could not create Per-frame Functional Groups Sequence");
    return result;
  }

  /* Iterate over frames */
  OFMap<Uint32, FunctionalGroups*>::iterator it = m_perFrame.begin();
  size_t numFrames = m_perFrame.size();
  for ( size_t count = 0; (count < numFrames)  && result.good(); count++)
  {
    DcmItem* perFrameItem = NULL;
    result = dataset.findOrCreateSequenceItem(DCM_PerFrameFunctionalGroupsSequence, perFrameItem, OFstatic_cast(long, count));
    if (result.good())
    {
      /* Iterate over groups for each frame */
      FunctionalGroups::iterator groupIt = (*it).second->begin();
      while ( result.good() && (groupIt != (*it).second->end()) )
      {
        DCMFG_DEBUG("Writing per-frame group: " << DcmFGTypes::FGType2OFString((*groupIt).second->getType()) << " for frame #" << count);
        result = (*groupIt).second->write(*perFrameItem);
        groupIt++;
      }
    }
    else
    {
      DCMFG_ERROR("Cannot create item in Per-frame Functional Groups Sequence");
    }
    it++;
  }
  return result;
}


OFCondition FGInterface::writeSharedFG(DcmItem& dataset)
{
  DCMFG_DEBUG("Writing shared functional groups");
  OFCondition result = dataset.insertEmptyElement(DCM_SharedFunctionalGroupsSequence, OFTrue); // start with empty sequence
  DcmItem *sharedFGItem = NULL;
  if (result.good())
  {
    result = dataset.findOrCreateSequenceItem(DCM_SharedFunctionalGroupsSequence, sharedFGItem, 0);
  }
  if (result.bad())
  {
    DCMFG_ERROR("Could not create Shared Functional Groups Sequence with single item");
    return result;
  }

  FunctionalGroups::iterator it = m_shared.begin();
  FunctionalGroups::iterator end = m_shared.end();
  while ( (it != end) && result.good() )
  {
    DCMFG_DEBUG("Writing shared group: " << DcmFGTypes::FGType2OFString((*it).second->getType()));
    result = (*it).second->write(*sharedFGItem);
    it++;
  }
  return result;
}


OFCondition FGInterface::insertPerFrame(const Uint32 frameNo,
                                        FGBase* group,
                                        const OFBool replaceExisting)
{
  if (group == NULL)
    return EC_IllegalParameter;

  OFCondition result = EC_Normal;
  FGBase* existing = getPerFrame(frameNo, group->getType());
  if (existing)
  {
    if (replaceExisting)
    {
      DCMFG_DEBUG("Replacing per-frame FG for frame: " << frameNo << ", type: " << DcmFGTypes::FGType2OFString(group->getType()));
      deletePerFrame(frameNo, group->getType());
    }
    else
    {
      result = FG_EC_DoubledFG;
    }
  }

  // Insert per-frame functional group
  if (result.good())
  {
    FunctionalGroups* perFrameGroups = getOrCreatePerFrameGroups(frameNo);
    if (perFrameGroups != NULL)
    {
      result = perFrameGroups->insert(group, replaceExisting);
    }
    else
    {
      result = FG_EC_CouldNotInsertFG;
    }
  }
  return result;
}


OFCondition FGInterface::convertSharedToPerFrame(const DcmFGTypes::E_FGType fgType)
{
  FGBase* shared = m_shared.remove(fgType);
  if (!shared)
  {
    return FG_EC_NoSuchGroup;
  }

  OFCondition result;
  size_t numFrames = m_perFrame.size();
  // Walk over all existing frames and copy "old" shared group to them
  size_t count = 0;
  for (count = 0; result.good() && (count < numFrames); count++)
  {
    FGBase* clone = shared->clone();
    if (!clone)
    {
      result = EC_MemoryExhausted;
    }
    else
    {
      result = insertPerFrame(OFstatic_cast(Uint32, count), clone, OFTrue /* replace existing */);
      if (result.bad())
      {
        delete clone;
      }
    }
  }
  return result ;
}


FGBase* FGInterface::get(const Uint32 frameNo,
                         const DcmFGTypes::E_FGType fgType,
                         OFBool& isPerFrame)
{
  FGBase *group = m_shared.find(fgType);
  if ( !group )
  {
    group = getPerFrame(frameNo, fgType);
    isPerFrame = OFTrue;
  }
  else
  {
    isPerFrame = OFFalse;
  }

  return group;
}


OFBool FGInterface::check()
{
  size_t numFrames = m_perFrame.size();
  DCMFG_DEBUG("Checking functional group structure for " << numFrames << " frames");
  size_t numErrors = 0;
  for (size_t frameCount = 0; frameCount < numFrames; frameCount++)
  {
    DCMFG_TRACE("Checking frame " << frameCount << "...");
    // Every frame requires the FrameContent functional group, check "en passant"
    OFBool foundFrameContent = OFFalse;
    OFMap<Uint32, FunctionalGroups*>::iterator frameFG = m_perFrame.begin();
    OFMap<Uint32, FunctionalGroups*>::iterator end = m_perFrame.end();
    while (frameFG != end)
    {
      FunctionalGroups::iterator group = (*frameFG).second->begin();
      FunctionalGroups::iterator groupEnd = (*frameFG).second->end();
      while (group != groupEnd)
      {
        // Check that per-frame group is not a shared group at the same time
        DcmFGTypes::E_FGType groupType = group->second->getType();
        if ( (groupType != DcmFGTypes::EFG_UNDEFINED) &&
          (groupType != DcmFGTypes::EFG_UNKNOWN) )
        {
          if (m_shared.find(groupType) != NULL)
          {
            DCMFG_ERROR("Functional group of type " << DcmFGTypes::FGType2OFString(groupType) << " is shared AND per-frame for frame " << frameCount);
            numErrors++;
          }
          if (groupType == DcmFGTypes::EFG_FRAMECONTENT)
            foundFrameContent = OFTrue;
        }
        // Check if "per-frame" is allowed for this group;
        if (group->second->getSharedType() == DcmFGTypes::EFGS_ONLYSHARED)
        {
          DCMFG_ERROR("Functional group of type " << DcmFGTypes::FGType2OFString(groupType) << " can never be per-frame, but found for frame " << frameCount);
          numErrors++;
        }
        group++;
      }
      frameFG++;
    }
    if (!foundFrameContent)
    {
      DCMFG_ERROR("Frame Content Functional group missing for frame #" << frameCount);
      numErrors++;
    }
  }

  // Check whether shared groups contain FGs that are only permitted per-frame
  FunctionalGroups::iterator it = m_shared.begin();
  FunctionalGroups::iterator end = m_shared.end();
  while (it != end)
  {
    if ( (*it).second->getSharedType() == DcmFGTypes::EFGS_ONLYPERFRAME )
    {
      DCMFG_ERROR("Functional group of type " << DcmFGTypes::FGType2OFString((*it).second->getType()) << " used as shared functional group but must be per-frame");
      numErrors++;
    }
    it++;
  }

  if (numErrors > 0)
    return OFFalse;

  return OFTrue;
}
