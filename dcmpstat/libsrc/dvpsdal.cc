/*
 *
 *  Copyright (C) 1999-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSDisplayedArea_PList
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpsdal.h"
#include "dcmtk/dcmpstat/dvpsda.h"      /* for DVPSDisplayedArea */
#include "dcmtk/dcmpstat/dvpsri.h"      /* for DVPSReferencedImage, needed by MSVC5 with STL */


DVPSDisplayedArea_PList::DVPSDisplayedArea_PList()
: list_()
{
}

DVPSDisplayedArea_PList::DVPSDisplayedArea_PList(const DVPSDisplayedArea_PList &arg)
: list_()
{
  OFListConstIterator(DVPSDisplayedArea *) first = arg.list_.begin();
  OFListConstIterator(DVPSDisplayedArea *) last = arg.list_.end();
  while (first != last)
  {     
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSDisplayedArea_PList::~DVPSDisplayedArea_PList()
{
  clear();
}

void DVPSDisplayedArea_PList::clear()
{
  OFListIterator(DVPSDisplayedArea *) first = list_.begin();
  OFListIterator(DVPSDisplayedArea *) last = list_.end();
  while (first != last)
  {     
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSDisplayedArea_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DVPSDisplayedArea *newImage = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;
  
  if (EC_Normal == dset.search(DCM_DisplayedAreaSelectionSequence, stack, ESM_fromHere, OFFalse))
  {
    dseq=(DcmSequenceOfItems *)stack.top();
    if (dseq)
    {
      unsigned long numItems = dseq->card();
      for (unsigned int i=0; i<numItems; i++)
      {
        ditem = dseq->getItem(i);
        newImage = new DVPSDisplayedArea();
        if (newImage && ditem)
        {
          result = newImage->read(*ditem);
          list_.push_back(newImage);
        } else result = EC_MemoryExhausted;
      }
    }
  }    
  
  return result;
}

OFCondition DVPSDisplayedArea_PList::write(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  dseq = new DcmSequenceOfItems(DCM_DisplayedAreaSelectionSequence);
  if (dseq)
  {
    OFListIterator(DVPSDisplayedArea *) first = list_.begin();
    OFListIterator(DVPSDisplayedArea *) last = list_.end();
    while (first != last)
    {
      if (result==EC_Normal)
      {
        ditem = new DcmItem();
        if (ditem)
        {
          result = (*first)->write(*ditem);
          if (result==EC_Normal) dseq->insert(ditem); else delete ditem;
        } else result = EC_MemoryExhausted;
      }
      ++first;
    }
    if (result==EC_Normal) dset.insert(dseq, OFTrue /*replaceOld*/); else delete dseq;
  } else result = EC_MemoryExhausted;
  return result;
}

DVPSDisplayedArea *DVPSDisplayedArea_PList::findDisplayedArea(const char *instanceUID, unsigned long frame)
{
  OFListIterator(DVPSDisplayedArea *) first = list_.begin();
  OFListIterator(DVPSDisplayedArea *) last = list_.end();
  while (first != last)
  {
    if ((*first)->isApplicable(instanceUID, frame)) return (*first);
    ++first;
  }
  return NULL;
}

void DVPSDisplayedArea_PList::rotateAndFlip(
  DVPSRotationType rotationFrom, 
  OFBool isFlippedFrom,
  DVPSRotationType rotationTo, 
  OFBool isFlippedTo)
{
  OFListIterator(DVPSDisplayedArea *) first = list_.begin();
  OFListIterator(DVPSDisplayedArea *) last = list_.end();
  while (first != last)
  {
    (*first)->rotateAndFlip(rotationFrom, isFlippedFrom, rotationTo, isFlippedTo);
    ++first;
  }
}

DVPSDisplayedArea *DVPSDisplayedArea_PList::createDisplayedArea(
    DVPSReferencedSeries_PList& allReferences,
    const char *sopclassUID, 
    const char *instanceUID, 
    unsigned long frame, 
    unsigned long numberOfFrames, 
    DVPSObjectApplicability applicability)
{
  
  DVPSDisplayedArea *oldArea = findDisplayedArea(instanceUID, frame);
  DVPSDisplayedArea *newArea = NULL;
  if (oldArea == NULL) newArea = new DVPSDisplayedArea();
  else
  {
    // Check if the existing displayed area selection happens to match the new
    // applicability. If yes, we only need to return the existing item.
    if (oldArea->matchesApplicability(instanceUID, frame, applicability)) return oldArea;

    // the applicability has changed. Rework the complete sequence.
    newArea = new DVPSDisplayedArea(*oldArea); // create copy
    if (newArea) newArea->clearImageReferences();

    OFListIterator(DVPSDisplayedArea *) first = list_.begin();
    OFListIterator(DVPSDisplayedArea *) last = list_.end();
    switch (applicability)
    {
      case DVPSB_currentFrame:
      case DVPSB_currentImage:
        while (first != last)
        {
          (*first)->removeImageReference(allReferences, instanceUID, frame, numberOfFrames, applicability);
          if ((*first)->imageReferencesEmpty())
          {
            delete (*first);
            first = list_.erase(first);
          } else ++first;
        }
        break;
      case DVPSB_allImages:
        clear(); // delete all area selections
        break;  
    }
  }

  if (newArea)
  {
    if (applicability != DVPSB_allImages) newArea->addImageReference(sopclassUID, instanceUID, frame, applicability);
    list_.push_back(newArea);
  }
  return newArea;
}
