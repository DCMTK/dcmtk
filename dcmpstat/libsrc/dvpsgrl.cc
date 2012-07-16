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
 *    classes: DVPSGraphicObject_PList
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpsgrl.h"
#include "dcmtk/dcmpstat/dvpsgr.h"      /* for DVPSGraphicObject */


DVPSGraphicObject_PList::DVPSGraphicObject_PList()
: list_()
{
}

DVPSGraphicObject_PList::DVPSGraphicObject_PList(const DVPSGraphicObject_PList &arg)
: list_()
{
  OFListConstIterator(DVPSGraphicObject *) first = arg.list_.begin();
  OFListConstIterator(DVPSGraphicObject *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSGraphicObject_PList::~DVPSGraphicObject_PList()
{
  clear();
}

void DVPSGraphicObject_PList::clear()
{
  OFListIterator(DVPSGraphicObject *) first = list_.begin();
  OFListIterator(DVPSGraphicObject *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSGraphicObject_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DVPSGraphicObject *newObject = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  if (EC_Normal == dset.search(DCM_GraphicObjectSequence, stack, ESM_fromHere, OFFalse))
  {
    dseq=(DcmSequenceOfItems *)stack.top();
    if (dseq)
    {
      unsigned long numItems = dseq->card();
      for (unsigned int i=0; i<numItems; i++)
      {
        ditem = dseq->getItem(i);
        newObject = new DVPSGraphicObject();
        if (newObject && ditem)
        {
          result = newObject->read(*ditem);
          list_.push_back(newObject);
        } else result = EC_MemoryExhausted;
      }
    }
  }

  return result;
}

OFCondition DVPSGraphicObject_PList::write(DcmItem &dset)
{
  if (size()==0) return EC_Normal; // don't write empty Sequence

  OFCondition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  dseq = new DcmSequenceOfItems(DCM_GraphicObjectSequence);
  if (dseq)
  {
    OFListIterator(DVPSGraphicObject *) first = list_.begin();
    OFListIterator(DVPSGraphicObject *) last = list_.end();
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


DVPSGraphicObject *DVPSGraphicObject_PList::getGraphicObject(size_t idx)
{
  OFListIterator(DVPSGraphicObject *) first = list_.begin();
  OFListIterator(DVPSGraphicObject *) last = list_.end();
  while (first != last)
  {
    if (idx==0) return *first;
    idx--;
    ++first;
  }
  return NULL;
}

void DVPSGraphicObject_PList::addGraphicObject(DVPSGraphicObject *graphic)
{
  if (graphic) list_.push_back(graphic);
}

DVPSGraphicObject *DVPSGraphicObject_PList::removeGraphicObject(size_t idx)
{
  OFListIterator(DVPSGraphicObject *) first = list_.begin();
  OFListIterator(DVPSGraphicObject *) last = list_.end();
  while (first != last)
  {
    if (idx==0)
    {
      DVPSGraphicObject *result = *first;
      list_.erase(first);
      return result;
    }
    idx--;
    ++first;
  }
  return NULL;
}
