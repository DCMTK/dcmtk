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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:32 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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


/*
 *  $Log: dvpsdal.cc,v $
 *  Revision 1.15  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.14  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.13  2005-12-08 15:46:24  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.12  2004/02/04 15:57:49  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.11  2003/09/05 14:30:08  meichel
 *  Introduced new API methods that allow Displayed Areas to be queried
 *    and set either relative to the image (ignoring rotation and flip) or
 *    in absolute values as defined in the standard.  Rotate and flip methods
 *    now adjust displayed areas in the presentation state.
 *
 *  Revision 1.10  2003/09/05 08:37:46  meichel
 *  Fixed minor issue that caused certain error messages during the
 *    parse process on a GSPS object to be "swallowed".
 *
 *  Revision 1.9  2003/06/12 18:23:11  joergr
 *  Modified code to use const_iterators where appropriate (required for STL).
 *
 *  Revision 1.8  2003/06/04 12:30:28  meichel
 *  Added various includes needed by MSVC5 with STL
 *
 *  Revision 1.7  2003/06/04 10:18:07  meichel
 *  Replaced private inheritance from template with aggregation
 *
 *  Revision 1.6  2001/11/28 13:56:52  joergr
 *  Check return value of DcmItem::insert() statements where appropriate to
 *  avoid memory leaks when insert procedure fails.
 *
 *  Revision 1.5  2001/09/26 15:36:24  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.4  2001/06/01 15:50:29  meichel
 *  Updated copyright header
 *
 *  Revision 1.3  2000/06/02 16:00:59  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.2  2000/03/08 16:29:04  meichel
 *  Updated copyright header.
 *
 *  Revision 1.1  1999/07/22 16:39:56  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *
 */
