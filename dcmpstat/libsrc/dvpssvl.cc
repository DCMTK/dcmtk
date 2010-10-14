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
 *    classes: DVPSSoftcopyVOI_PList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:33 $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmpstat/dvpssvl.h"
#include "dcmtk/dcmpstat/dvpssv.h"      /* for DVPSSoftcopyVOI */
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */

#include "dcmtk/dcmpstat/dvpsri.h"      /* for DVPSReferencedImage, needed by MSVC5 with STL */

/* --------------- class DVPSSoftcopyVOI_PList --------------- */

DVPSSoftcopyVOI_PList::DVPSSoftcopyVOI_PList()
: list_()
{
}

DVPSSoftcopyVOI_PList::DVPSSoftcopyVOI_PList(const DVPSSoftcopyVOI_PList &arg)
: list_()
{
  OFListConstIterator(DVPSSoftcopyVOI *) first = arg.list_.begin();
  OFListConstIterator(DVPSSoftcopyVOI *) last = arg.list_.end();
  while (first != last)
  {     
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSSoftcopyVOI_PList::~DVPSSoftcopyVOI_PList()
{
  clear();
}

void DVPSSoftcopyVOI_PList::clear()
{
  OFListIterator(DVPSSoftcopyVOI *) first = list_.begin();
  OFListIterator(DVPSSoftcopyVOI *) last = list_.end();
  while (first != last)
  {     
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSSoftcopyVOI_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DVPSSoftcopyVOI *newImage = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;
  
  if (EC_Normal == dset.search(DCM_SoftcopyVOILUTSequence, stack, ESM_fromHere, OFFalse))
  {
    dseq=(DcmSequenceOfItems *)stack.top();
    if (dseq)
    {
      unsigned long numItems = dseq->card();
      for (unsigned int i=0; i<numItems; i++)
      {
        ditem = dseq->getItem(i);
        newImage = new DVPSSoftcopyVOI();
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

OFCondition DVPSSoftcopyVOI_PList::write(DcmItem &dset)
{
  if (size()==0) return EC_Normal; // don't write empty Sequence

  OFCondition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  dseq = new DcmSequenceOfItems(DCM_SoftcopyVOILUTSequence);
  if (dseq)
  {
    OFListIterator(DVPSSoftcopyVOI *) first = list_.begin();
    OFListIterator(DVPSSoftcopyVOI *) last = list_.end();
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

DVPSSoftcopyVOI *DVPSSoftcopyVOI_PList::findSoftcopyVOI(const char *instanceUID, unsigned long frame)
{
  OFListIterator(DVPSSoftcopyVOI *) first = list_.begin();
  OFListIterator(DVPSSoftcopyVOI *) last = list_.end();
  while (first != last)
  {
    if ((*first)->isApplicable(instanceUID, frame)) return (*first);
    ++first;
  }
  return NULL;
}


DVPSSoftcopyVOI *DVPSSoftcopyVOI_PList::createSoftcopyVOI(
    DVPSReferencedSeries_PList& allReferences,
    const char *sopclassUID, 
    const char *instanceUID, 
    unsigned long frame, 
    unsigned long numberOfFrames, 
    DVPSObjectApplicability applicability)
{
  
  DVPSSoftcopyVOI *oldArea = findSoftcopyVOI(instanceUID, frame);
  DVPSSoftcopyVOI *newArea = NULL;
  if (oldArea == NULL) newArea = new DVPSSoftcopyVOI();
  else
  {
    // Check if the existing VOI LUT item happens to match the new
    // applicability. If yes, we only need to return the existing item.
    if (oldArea->matchesApplicability(instanceUID, frame, applicability)) return oldArea;

    // the applicability has changed. Rework the complete sequence.
    newArea = new DVPSSoftcopyVOI(*oldArea); // create copy
    if (newArea) newArea->clearImageReferences();

    OFListIterator(DVPSSoftcopyVOI *) first = list_.begin();
    OFListIterator(DVPSSoftcopyVOI *) last = list_.end();
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
        clear(); // delete all softcopy VOI LUT items
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

void DVPSSoftcopyVOI_PList::removeSoftcopyVOI(
    DVPSReferencedSeries_PList& allReferences,
    const char *instanceUID, 
    unsigned long frame, 
    unsigned long numberOfFrames, 
    DVPSObjectApplicability applicability)
{
  OFListIterator(DVPSSoftcopyVOI *) first = list_.begin();
  OFListIterator(DVPSSoftcopyVOI *) last = list_.end();
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
      clear(); // delete all softcopy VOI LUT items
      break;  
  }
  return;
}

OFCondition DVPSSoftcopyVOI_PList::createFromImage(
    DcmItem &dset, 
    DVPSReferencedSeries_PList& allReferences,
    const char *sopclassUID, 
    const char *instanceUID, 
    DVPSVOIActivation voiActivation)
{
  if (voiActivation == DVPSV_ignoreVOI) return EC_Normal;
  
  OFCondition result = EC_Normal;
  DcmStack stack;
  DcmSequenceOfItems *seq;
  DcmItem *item;
  DcmUnsignedShort         voiLUTDescriptor(DCM_LUTDescriptor);
  DcmLongString            voiLUTExplanation(DCM_LUTExplanation);
  DcmUnsignedShort         voiLUTData(DCM_LUTData);
  DcmDecimalString         windowCenter(DCM_WindowCenter);
  DcmDecimalString         windowWidth(DCM_WindowWidth);
  DcmLongString            windowCenterWidthExplanation(DCM_WindowCenterWidthExplanation);

  READ_FROM_DATASET(DcmDecimalString, windowCenter)
  READ_FROM_DATASET(DcmDecimalString, windowWidth)
  READ_FROM_DATASET(DcmLongString, windowCenterWidthExplanation)

  /* read VOI LUT Sequence */
  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_VOILUTSequence, stack, ESM_fromHere, OFFalse))
    {
      seq=(DcmSequenceOfItems *)stack.top();
      if (seq->card() > 0)
      {
         item = seq->getItem(0);
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)voiLUTDescriptor.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           voiLUTDescriptor = *((DcmUnsignedShort *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)voiLUTExplanation.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           voiLUTExplanation = *((DcmLongString *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)voiLUTData.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           voiLUTData = *((DcmUnsignedShort *)(stack.top()));
         }
      } else result=EC_TagNotFound;
    }
  }

  OFBool haveWindow = ((windowCenter.getVM() > 0) && (windowWidth.getVM() > 0));
  OFBool haveLUT = ((voiLUTDescriptor.getVM() == 3) && (voiLUTData.getLength() > 0));
  
  if (result==EC_Normal)
  {
    DVPSSoftcopyVOI *voi = NULL;
    if (haveWindow && ((voiActivation==DVPSV_preferVOIWindow)||(! haveLUT)))
    {
      // create VOI window
      Float64 wc, ww;
      char *wexp = NULL;
      result = windowCenter.getFloat64(wc,0);
      if (EC_Normal==result) result = windowWidth.getFloat64(ww,0);
      if (windowCenterWidthExplanation.getLength() > 0)
      {
      	if (EC_Normal != windowCenterWidthExplanation.getString(wexp)) wexp = NULL;
      }
      if (EC_Normal==result)
      {
        voi = createSoftcopyVOI(allReferences, sopclassUID, instanceUID, 1, 1, DVPSB_currentImage);
        if (voi) result = voi->setVOIWindow(wc, ww, wexp); else result = EC_MemoryExhausted;
      }
    }
    else if (haveLUT && ((voiActivation==DVPSV_preferVOILUT)||(! haveWindow)))
    {
      // create VOI LUT
      voi = createSoftcopyVOI(allReferences, sopclassUID, instanceUID, 1, 1, DVPSB_currentImage);
      if (voi) result = voi->setVOILUT(voiLUTDescriptor, voiLUTData, voiLUTExplanation); else result = EC_MemoryExhausted;
    }
  }       

  return result;
}

/*
 *  $Log: dvpssvl.cc,v $
 *  Revision 1.16  2010-10-14 13:14:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.15  2009-11-24 14:12:59  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.14  2009-09-30 10:42:39  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.13  2005-12-08 15:46:50  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.12  2004/02/04 15:57:49  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.11  2003/06/12 18:23:11  joergr
 *  Modified code to use const_iterators where appropriate (required for STL).
 *
 *  Revision 1.10  2003/06/04 12:30:29  meichel
 *  Added various includes needed by MSVC5 with STL
 *
 *  Revision 1.9  2003/06/04 10:18:07  meichel
 *  Replaced private inheritance from template with aggregation
 *
 *  Revision 1.8  2002/10/18 08:34:52  meichel
 *  Fixed minor bug in presentation state code that caused error messages
 *    in the Softcopy VOI LUT module to be "swallowed" even if verbose mode
 *    was enabled.
 *
 *  Revision 1.7  2001/11/28 13:57:04  joergr
 *  Check return value of DcmItem::insert() statements where appropriate to
 *  avoid memory leaks when insert procedure fails.
 *
 *  Revision 1.6  2001/09/26 15:36:33  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.5  2001/06/01 15:50:39  meichel
 *  Updated copyright header
 *
 *  Revision 1.4  2000/06/02 16:01:08  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.3  2000/05/31 13:02:40  meichel
 *  Moved dcmpstat macros and constants into a common header file
 *
 *  Revision 1.2  2000/03/08 16:29:11  meichel
 *  Updated copyright header.
 *
 *  Revision 1.1  1999/07/22 16:40:03  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *
 */
