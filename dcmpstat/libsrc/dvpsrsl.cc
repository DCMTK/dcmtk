/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *    classes: DVPSReferencedSeries_PList
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmpstat/dvpsrsl.h"
#include "dcmtk/dcmpstat/dvpsrs.h"      /* for DVPSReferencedSeries */
#include "dcmtk/dcmpstat/dvpsri.h"      /* for DVPSReferencedImage, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsdef.h"

DVPSReferencedSeries_PList::DVPSReferencedSeries_PList()
: list_()
{
}

DVPSReferencedSeries_PList::DVPSReferencedSeries_PList(const DVPSReferencedSeries_PList &arg)
: list_()
{
  OFListConstIterator(DVPSReferencedSeries *) first = arg.list_.begin();
  OFListConstIterator(DVPSReferencedSeries *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSReferencedSeries_PList::~DVPSReferencedSeries_PList()
{
  clear();
}

void DVPSReferencedSeries_PList::clear()
{
  OFListIterator(DVPSReferencedSeries *) first = list_.begin();
  OFListIterator(DVPSReferencedSeries *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSReferencedSeries_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DVPSReferencedSeries *newSeries = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  if (EC_Normal == dset.search(DCM_ReferencedSeriesSequence, stack, ESM_fromHere, OFFalse))
  {
    dseq=(DcmSequenceOfItems *)stack.top();
    if (dseq)
    {
      unsigned long numItems = dseq->card();
      for (unsigned int i=0; i<numItems; i++)
      {
        ditem = dseq->getItem(i);
        newSeries = new DVPSReferencedSeries();
        if (newSeries && ditem)
        {
          result = newSeries->read(*ditem);
          list_.push_back(newSeries);
        } else result = EC_MemoryExhausted;
      }
    }
  }

  return result;
}

OFCondition DVPSReferencedSeries_PList::write(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  dseq = new DcmSequenceOfItems(DCM_ReferencedSeriesSequence);
  if (dseq)
  {
    OFListIterator(DVPSReferencedSeries *) first = list_.begin();
    OFListIterator(DVPSReferencedSeries *) last = list_.end();
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

OFBool DVPSReferencedSeries_PList::isValid()
{
  if (list_.size() == 0)
  {
    DCMPSTAT_WARN("referenced series SQ is empty in presentation state");
    return OFFalse;
  }

  OFBool result = OFTrue;
  OFString sopclassuid;

  OFListIterator(DVPSReferencedSeries *) first = list_.begin();
  OFListIterator(DVPSReferencedSeries *) last = list_.end();
  while ((result == OFTrue) && (first != last))
  {
    result = (*first)->isValid(sopclassuid);
    ++first;
  }
  return result;
}

OFBool DVPSReferencedSeries_PList::checkSOPClass(const char *uid)
{

  OFBool result = OFTrue;
  OFString sopclassuid(uid);
  OFListIterator(DVPSReferencedSeries *) first = list_.begin();
  OFListIterator(DVPSReferencedSeries *) last = list_.end();
  while ((result == OFTrue) && (first != last))
  {
    result = (*first)->isValid(sopclassuid);
    ++first;
  }
  return result;
}

void DVPSReferencedSeries_PList::removeImageReference(const char *seriesUID, const char *instanceUID)
{
  DVPSReferencedSeries *series = findSeriesReference(seriesUID);
  if (series) series->removeImageReference(instanceUID);
  return;
}

DVPSReferencedSeries *DVPSReferencedSeries_PList::findSeriesReference(const char *seriesUID)
{
  OFListIterator(DVPSReferencedSeries *) first = list_.begin();
  OFListIterator(DVPSReferencedSeries *) last = list_.end();
  while (first != last)
  {
    if ((*first)->isSeriesUID(seriesUID)) return *first;
    ++first;
  }
  return NULL;
}

DVPSReferencedImage *DVPSReferencedSeries_PList::findImageReference(const char *seriesUID, const char *instanceUID)
{
  DVPSReferencedSeries *series = findSeriesReference(seriesUID);
  if (series) return series->findImageReference(instanceUID); else return NULL;
}

void DVPSReferencedSeries_PList::removeSeriesReference(const char *seriesUID)
{
  OFListIterator(DVPSReferencedSeries *) first = list_.begin();
  OFListIterator(DVPSReferencedSeries *) last = list_.end();
  while (first != last)
  {
    if ((*first)->isSeriesUID(seriesUID))
    {
      delete (*first);
      first = list_.erase(first);
    } else ++first;
  }
  return;
}

OFCondition DVPSReferencedSeries_PList::addImageReference(
    const char *seriesUID,
    const char *sopclassUID,
    const char *instanceUID,
    const char *frames,
    const char *aetitle,
    const char *filesetID,
    const char *filesetUID)

{
  if ((seriesUID==NULL) || (sopclassUID==NULL) || (instanceUID==NULL)) return EC_IllegalCall;

  OFCondition result = EC_Normal;
  if (checkSOPClass(sopclassUID))
  {
    DVPSReferencedSeries *series = findSeriesReference(seriesUID);
    if (series == NULL)
    {
      series = new DVPSReferencedSeries();
      if (series)
      {
        series->setSeriesInstanceUID(seriesUID);
        list_.push_back(series);
      }
    }
    if (series)
    {
      result = series->addImageReference(sopclassUID, instanceUID, frames);
      if (EC_Normal == result) series->setRetrieveLocation(aetitle, filesetID, filesetUID);
    } else result = EC_MemoryExhausted;
  } else result = EC_IllegalCall;
  return result;
}


size_t DVPSReferencedSeries_PList::numberOfImageReferences()
{
  size_t result=0;
  OFListIterator(DVPSReferencedSeries *) first = list_.begin();
  OFListIterator(DVPSReferencedSeries *) last = list_.end();
  while (first != last)
  {
    result += (*first)->numberOfImageReferences();
    ++first;
  }
  return result;
}

OFCondition DVPSReferencedSeries_PList::getImageReference(
    size_t idx,
    OFString& seriesUID,
    OFString& sopclassUID,
    OFString& instanceUID,
    OFString& frames,
    OFString& aetitle,
    OFString& filesetID,
    OFString& filesetUID)
{
  OFListIterator(DVPSReferencedSeries *) first = list_.begin();
  OFListIterator(DVPSReferencedSeries *) last = list_.end();
  OFBool found = OFFalse;
  size_t i;
  while ((!found)&&(first != last))
  {
    i=(*first)->numberOfImageReferences();
    if (i > idx) found = OFTrue; else
    {
      idx -= i;
      ++first;
    }
  }
  if (found) return (*first)->getImageReference(idx, seriesUID, sopclassUID, instanceUID, frames, aetitle, filesetID, filesetUID);
  return EC_IllegalCall;
}
