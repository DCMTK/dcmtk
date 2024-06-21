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
 *    classes: DVPSAnnotationContent_PList
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmpstat/dvpsabl.h"
#include "dcmtk/dcmpstat/dvpsab.h"      /* for DVPSAnnotationContent */
#include "dcmtk/dcmpstat/dvpshlp.h"     /* for class DVPSHelper */

/* --------------- class DVPSAnnotationContent_PList --------------- */

DVPSAnnotationContent_PList::DVPSAnnotationContent_PList()
: list_()
{
}

DVPSAnnotationContent_PList::DVPSAnnotationContent_PList(const DVPSAnnotationContent_PList &arg)
: list_()
{
  OFListConstIterator(DVPSAnnotationContent *) first = arg.list_.begin();
  OFListConstIterator(DVPSAnnotationContent *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSAnnotationContent_PList::~DVPSAnnotationContent_PList()
{
  clear();
}

void DVPSAnnotationContent_PList::clear()
{
  OFListIterator(DVPSAnnotationContent *) first = list_.begin();
  OFListIterator(DVPSAnnotationContent *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSAnnotationContent_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DVPSAnnotationContent *newAnnotation = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  if (EC_Normal == dset.search(DCM_RETIRED_AnnotationContentSequence, stack, ESM_fromHere, OFFalse) && (stack.top()->ident() == EVR_SQ))
  {
    dseq=(DcmSequenceOfItems *)stack.top();
    if (dseq)
    {
      unsigned long numItems = dseq->card();
      for (unsigned int i=0; i<numItems; i++)
      {
        ditem = dseq->getItem(i);
        newAnnotation = new DVPSAnnotationContent();
        if (newAnnotation && ditem)
        {
          result = newAnnotation->read(*ditem);
          list_.push_back(newAnnotation);
        } else result = EC_MemoryExhausted;
      }
    }
  }

  return result;
}

OFCondition DVPSAnnotationContent_PList::write(DcmItem &dset)
{
  if (size()==0) return EC_Normal; // don't write if sequence is empty

  OFCondition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  dseq = new DcmSequenceOfItems(DCM_RETIRED_AnnotationContentSequence);
  if (dseq)
  {
    OFListIterator(DVPSAnnotationContent *) first = list_.begin();
    OFListIterator(DVPSAnnotationContent *) last = list_.end();
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


OFCondition DVPSAnnotationContent_PList::addAnnotationBox(
    const char *instanceuid,
    const char *text,
    Uint16 position)
{
  OFCondition result = EC_Normal;
  DVPSAnnotationContent *newAnnotation = new DVPSAnnotationContent();
  if (newAnnotation)
  {
    result = newAnnotation->setContent(instanceuid, text, position);
    if (EC_Normal == result) list_.push_back(newAnnotation); else delete newAnnotation;
  } else result = EC_MemoryExhausted;
  return result;
}


OFCondition DVPSAnnotationContent_PList::deleteAnnotation(size_t idx)
{
  OFListIterator(DVPSAnnotationContent *) first = list_.begin();
  OFListIterator(DVPSAnnotationContent *) last = list_.end();
  while ((first != last)&&(idx--)) ++first;
  if (first != last)
  {
    delete (*first);
    list_.erase(first);
    return EC_Normal;
  }
  return EC_IllegalCall;
}

OFCondition DVPSAnnotationContent_PList::deleteMultipleAnnotations(size_t number)
{
  OFListIterator(DVPSAnnotationContent *) first = list_.begin();
  OFListIterator(DVPSAnnotationContent *) last = list_.end();
  while ((first != last)&&(number--))
  {
    delete (*first);
    first = list_.erase(first);
  }
  return EC_Normal;
}

DVPSAnnotationContent *DVPSAnnotationContent_PList::getAnnotationBox(size_t idx)
{
  OFListIterator(DVPSAnnotationContent *) first = list_.begin();
  OFListIterator(DVPSAnnotationContent *) last = list_.end();
  while (first != last)
  {
    if (idx==0) return *first;
    idx--;
    ++first;
  }
  return NULL;
}

OFCondition DVPSAnnotationContent_PList::setAnnotationSOPInstanceUID(size_t idx, const char *value)
{
  DVPSAnnotationContent *box = getAnnotationBox(idx);
  if (box) return box->setSOPInstanceUID(value);
  return EC_IllegalCall;
}

const char *DVPSAnnotationContent_PList::getSOPInstanceUID(size_t idx)
{
  DVPSAnnotationContent *box = getAnnotationBox(idx);
  if (box) return box->getSOPInstanceUID();
  return NULL;
}

OFCondition DVPSAnnotationContent_PList::prepareBasicAnnotationBox(size_t idx, DcmItem &dset)
{
  DVPSAnnotationContent *box = getAnnotationBox(idx);
  if (box) return box->prepareBasicAnnotationBox(dset);
  return EC_IllegalCall;
}

void DVPSAnnotationContent_PList::clearAnnotationSOPInstanceUIDs()
{
  OFListIterator(DVPSAnnotationContent *) first = list_.begin();
  OFListIterator(DVPSAnnotationContent *) last = list_.end();
  while (first != last)
  {
    (*first)->setSOPInstanceUID(NULL);
    ++first;
  }
}
