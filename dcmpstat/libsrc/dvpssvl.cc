/*
 *
 *  Copyright (C) 1998-2000, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSSoftcopyVOI_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:29:11 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpssvl.h"
#include "dvpssv.h"      /* for DVPSSoftcopyVOI */

/* --------------- a few macros avoiding copy/paste --------------- */

#define ADD_TO_DATASET(a_type, a_name)                              \
if (result==EC_Normal)                                              \
{                                                                   \
  delem = new a_type(a_name);                                       \
  if (delem) dset.insert(delem); else result=EC_MemoryExhausted;    \
}

#define READ_FROM_DATASET(a_type, a_name)                           \
stack.clear();                                                      \
if (EC_Normal == dset.search((DcmTagKey &)a_name.getTag(), stack, ESM_fromHere, OFFalse)) \
{                                                                   \
  a_name = *((a_type *)(stack.top()));                              \
}

/* --------------- class DVPSSoftcopyVOI_PList --------------- */

DVPSSoftcopyVOI_PList::DVPSSoftcopyVOI_PList()
: OFList<DVPSSoftcopyVOI *>()
{
}

DVPSSoftcopyVOI_PList::DVPSSoftcopyVOI_PList(const DVPSSoftcopyVOI_PList &arg)
: OFList<DVPSSoftcopyVOI *>()
{
  OFListIterator(DVPSSoftcopyVOI *) first = arg.begin();
  OFListIterator(DVPSSoftcopyVOI *) last = arg.end();
  while (first != last)
  {     
    push_back((*first)->clone());
    ++first;
  }
}

DVPSSoftcopyVOI_PList::~DVPSSoftcopyVOI_PList()
{
  clear();
}

void DVPSSoftcopyVOI_PList::clear()
{
  OFListIterator(DVPSSoftcopyVOI *) first = begin();
  OFListIterator(DVPSSoftcopyVOI *) last = end();
  while (first != last)
  {     
    delete (*first);
    first = erase(first);
  }
}

E_Condition DVPSSoftcopyVOI_PList::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
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
          push_back(newImage);
        } else result = EC_MemoryExhausted;
      }
    }
  }    
  
  return result;
}

E_Condition DVPSSoftcopyVOI_PList::write(DcmItem &dset)
{
  if (size()==0) return EC_Normal; // don't write empty Sequence

  E_Condition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  dseq = new DcmSequenceOfItems(DCM_SoftcopyVOILUTSequence);
  if (dseq)
  {
    OFListIterator(DVPSSoftcopyVOI *) first = begin();
    OFListIterator(DVPSSoftcopyVOI *) last = end();
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
    if (result==EC_Normal) dset.insert(dseq); else delete dseq;
  } else result = EC_MemoryExhausted;
  return result;
}

DVPSSoftcopyVOI *DVPSSoftcopyVOI_PList::findSoftcopyVOI(const char *instanceUID, unsigned long frame)
{
  OFListIterator(DVPSSoftcopyVOI *) first = begin();
  OFListIterator(DVPSSoftcopyVOI *) last = end();
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

    OFListIterator(DVPSSoftcopyVOI *) first = begin();
    OFListIterator(DVPSSoftcopyVOI *) last = end();
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
            first = erase(first);
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
    push_back(newArea);
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
  OFListIterator(DVPSSoftcopyVOI *) first = begin();
  OFListIterator(DVPSSoftcopyVOI *) last = end();
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
          first = erase(first);
        } else ++first;
      }
      break;
    case DVPSB_allImages:
      clear(); // delete all softcopy VOI LUT items
      break;  
  }
  return;
}

E_Condition DVPSSoftcopyVOI_PList::createFromImage(
    DcmItem &dset, 
    DVPSReferencedSeries_PList& allReferences,
    const char *sopclassUID, 
    const char *instanceUID, 
    DVPSVOIActivation voiActivation)
{
  if (voiActivation == DVPSV_ignoreVOI) return EC_Normal;
  
  E_Condition result = EC_Normal;
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
 *  Revision 1.2  2000-03-08 16:29:11  meichel
 *  Updated copyright header.
 *
 *  Revision 1.1  1999/07/22 16:40:03  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *
 */

