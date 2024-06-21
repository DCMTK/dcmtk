/*
 *
 *  Copyright (C) 1999-2024, OFFIS e.V.
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

  if (EC_Normal == dset.search(DCM_SoftcopyVOILUTSequence, stack, ESM_fromHere, OFFalse) && (stack.top()->ident() == EVR_SQ))
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
          if (result.bad()) return result;
          list_.push_back(newImage);
        } else return EC_MemoryExhausted;
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

  READ_FROM_DATASET(DcmDecimalString, EVR_DS, windowCenter)
  READ_FROM_DATASET(DcmDecimalString, EVR_DS, windowWidth)
  READ_FROM_DATASET(DcmLongString, EVR_LO, windowCenterWidthExplanation)

  /* read VOI LUT Sequence */
  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_VOILUTSequence, stack, ESM_fromHere, OFFalse) && (stack.top()->ident() == EVR_SQ))
    {
      seq=(DcmSequenceOfItems *)stack.top();
      if (seq->card() > 0)
      {
         item = seq->getItem(0);
         stack.clear();

         // LUTDescriptor can be US or SS
         if ((EC_Normal == item->search((DcmTagKey &)voiLUTDescriptor.getTag(),
           stack, ESM_fromHere, OFFalse)) && (stack.top()->ident() == EVR_US || stack.top()->ident() == EVR_SS))
         {
           // We explicitly use DcmElement::operator=(), which works for US and SS
           DcmElement *vLUTDescriptor = &voiLUTDescriptor;
           vLUTDescriptor->operator=(* OFstatic_cast(DcmElement *, stack.top()));
         }

         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)voiLUTExplanation.getTag(),
           stack, ESM_fromHere, OFFalse) && (stack.top()->ident() == EVR_LO))
         {
           voiLUTExplanation = *((DcmLongString *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)voiLUTData.getTag(),
           stack, ESM_fromHere, OFFalse) && (stack.top()->ident() == EVR_US || stack.top()->ident() == EVR_OW))
         {
           // we deliberately call DcmElement::operator=() here, which will work for both DcmUnsignedShort and DcmOtherByteOtherWord parameters
           DcmElement *vldata = &voiLUTData;
           vldata->operator=(*(DcmElement *)(stack.top()));
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
      Float64 wc = 0.0;
      Float64 ww = 0.0;
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
