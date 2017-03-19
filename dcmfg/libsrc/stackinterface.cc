/*
 *
 *  Copyright (C) 2015-2017, Open Connections GmbH
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
 *  Purpose: Class for reading, accessing and writing stacks
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmfg/stackinterface.h"
#include "dcmtk/dcmfg/fgfracon.h"


FGStackInterface::FGStackInterface()
: m_Stacks()
{
}


FGStackInterface::~FGStackInterface()
{
  while (m_Stacks.size() != 0)
  {
    OFMap<OFString,FGStack*>::iterator it = m_Stacks.begin();
    FGStack *toDelete = (*it).second;
    m_Stacks.erase(it);
    delete toDelete;
  }
}


void FGStackInterface::clear()
{
  size_t stackSize = m_Stacks.size();
  OFMap<OFString, FGStack*>::iterator it;
  for (size_t count = 0; count < stackSize; count++)
  {
    it = m_Stacks.begin();
    m_Stacks.erase(it);
  }
}


OFCondition FGStackInterface::read(FGInterface& fgSource)
{
  clear();
  size_t numFrames = fgSource.getNumberOfFrames();
  for (size_t count = 0; count < numFrames; count++)
  {
    // Get frame content FG if existing
    FGFrameContent* fracon = OFstatic_cast(FGFrameContent*, fgSource.get(OFstatic_cast(Uint32, count), DcmFGTypes::EFG_FRAMECONTENT));
    if (fracon != NULL)
    {
      OFString stackID;
      Uint32 inStackPos;
      // Check whether stack ID is actually present and get value
      if ( (fracon->getStackID(stackID).good()) && (fracon->getInStackPositionNumber(inStackPos).good()) )
      {
        // Check whether this is a stack ID we do not know yet
        OFMap<OFString, FGStack*>::iterator it = m_Stacks.begin();
        while (it != m_Stacks.end())
        {
          // If this is an old stack, add frame to it
          if ( (*it).second->getStackID() == stackID)
          {
            // Add frame to stack if it is not in yet
            if ( (*it).second->getInStackPos(OFstatic_cast(Uint32, count)) == 0)
            {
              (*it).second->addFrame(OFstatic_cast(Uint32, count), inStackPos);
            }
          }
          it++;
        }
        // If this is a new stack, add it to the list
        if (it == m_Stacks.end())
        {
          FGStack *stack = new FGStack(stackID);
          if (stack == NULL)
          {
            return EC_MemoryExhausted;
          }
          stack->addFrame(OFstatic_cast(Uint32, count), inStackPos);
          if ( !m_Stacks.insert(OFMake_pair(stack->getStackID(), stack)).second )
          {
            delete stack;
            DCMFG_ERROR("Could not add stack to internal list (internal error, ignored)");
          }
        }
      }
      else
      {
        DCMFG_WARN("Reading stacks but Frame " << count << " does not provide Stack ID or In-Stack Position");
      }
    }
    else
    {
      DCMFG_WARN("Reading stacks but Frame " << count << " does not provide a Frame Content functional group");
    }
  }

  if (m_Stacks.size() == 0)
  {
    return FG_EC_NoStacksFound;
  }

  return EC_Normal;
}


OFCondition FGStackInterface::write(FGInterface& fgDestination)
{
  size_t numFrames = fgDestination.getNumberOfFrames();
  if (numFrames == 0)
  {
    return FG_EC_NotEnoughFrames;
  }

  // Check whether maximum in-stack position does not exceed number of frames in image
  if (!checkConsistency(&fgDestination))
  {
    return FG_EC_InvalidData;
  }

  // Walk through stacks, and for each, add/overwrite stack information to
  // the respective Frame Content functional groups
  OFCondition result;
  OFMap<OFString, FGStack*>::iterator stack = m_Stacks.begin();
  while (result.good() && (stack != m_Stacks.end()) )
  {
    OFString stackID =  (*stack).second->getStackID();
    // Walk over all referenced frames and add Stack information
    OFMap<Uint32, Uint32>::const_iterator stackEntry = (*stack).second->begin();
    while (stackEntry != (*stack).second->end())
    {
      Uint32 frameNo = (*stackEntry).first;
      Uint32 inStackPos = (*stackEntry).second;
      FGFrameContent* fg = ensureFrameContentFG(frameNo, fgDestination);
      // Create functional group if not already existing
      if (fg == NULL)
      {
        result = FG_EC_CouldNotAddFG;
      }
      if (result.good()) result = fg->setStackID(stackID);
      if (result.good()) result = fg->setInStackPositionNumber(inStackPos+1 /* vector counts from 0, position in DICOM starts with 1 */);
      if (result.bad())
      {
        DCMFG_ERROR("Could not create or add stack with ID " << stackID << " to frame " << frameNo << ": " << result.text());
      }
      stackEntry++;
    }
    stack++;
  }
  return result;
}


FGFrameContent* FGStackInterface::ensureFrameContentFG(const Uint32 frameNo,
                                                       FGInterface& fg)
{
  FGFrameContent* content = OFstatic_cast(FGFrameContent*, fg.get(frameNo, DcmFGTypes::EFG_FRAMECONTENT));
  if (!content)
  {
    FGFrameContent newContent;
    OFCondition result = fg.addPerFrame(frameNo, newContent);
    if (result.bad())
    {
      DCMFG_ERROR("Could not add Frame Content FG for frame " << frameNo);
    }
    else
    {
      content = OFstatic_cast(FGFrameContent*, fg.get(frameNo, DcmFGTypes::EFG_FRAMECONTENT));
    }
  }
  return content;
}


OFBool FGStackInterface::addStack(FGStack* stack)
{
  if (stack == NULL)
    return OFFalse;

  if (stack->getStackID().empty())
  {
    DCMFG_ERROR("Stack ID cannot be empty");
    return OFFalse;
  }

  return m_Stacks.insert(OFMake_pair(stack->getStackID(), stack)).second;
}


size_t FGStackInterface::numStacks() const
{
  return m_Stacks.size();
}


OFBool FGStackInterface::checkConsistency(FGInterface* fgContext)
{
  size_t errors = 0;
  size_t count = 0;
  // Run over stacks and check whether each has a Stack ID.
  // Also check whether stacks can be valid in the context
  // of the given functional groups (e.g. frame range ok)
  OFMap<OFString, FGStack*>::iterator it = m_Stacks.begin();
  while (it != m_Stacks.end())
  {
    if ( (*it).second->getStackID().empty())
    {
      errors++;
      DCMFG_ERROR("Stack ID for stack #" << count << " is empty");
    }
    if (fgContext)
    {
      errors += checkContext((*it).second, fgContext);
    }
    it++;
    count++;
  }
  return (errors == 0);
}


size_t FGStackInterface::checkContext(FGStack* stack,
                                      FGInterface* context)
{
  // Must be non-empty
  if ( !stack || !context)
    return 1;

  // Check whether we have any frames
  size_t numFrames = context->getNumberOfFrames();
  if (numFrames == 0)
  {
    DCMFG_ERROR("Cannot have stacks without frames");
    return 1;
  }

  // Walk over frames in stack and check that frame number and
  // in-stack position is in range with the number of available frames
  size_t errors = 0;
  const OFMap<Uint32,Uint32>& frames = stack->m_FrameNumbers;
  FGStack::const_iterator it = frames.begin();
  for (size_t count = 0; count < numFrames; count++)
  {
    // Frame number must be in range
    if ( (*it).first > numFrames )
    {
      DCMFG_ERROR("Stack references frame #" << (*it).first << " but only #" << numFrames << " frames exist");
      errors++;
    }
    if ( (*it).second> numFrames )
    {
      DCMFG_ERROR("Stack references in-stack position #" << (*it).first << " but only #" << numFrames << " frames exist");
      errors++;
    }
  }
  return errors;
}
