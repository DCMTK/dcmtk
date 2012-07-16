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
 *    classes: DVPSTextObject_PList
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpstxl.h"
#include "dcmtk/dcmpstat/dvpstx.h"      /* for DVPSTextObject */


DVPSTextObject_PList::DVPSTextObject_PList()
: list_()
{
}

DVPSTextObject_PList::DVPSTextObject_PList(const DVPSTextObject_PList &arg)
: list_()
{
  OFListConstIterator(DVPSTextObject *) first = arg.list_.begin();
  OFListConstIterator(DVPSTextObject *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSTextObject_PList::~DVPSTextObject_PList()
{
  clear();
}

void DVPSTextObject_PList::clear()
{
  OFListIterator(DVPSTextObject *) first = list_.begin();
  OFListIterator(DVPSTextObject *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSTextObject_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DVPSTextObject *newObject = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  if (EC_Normal == dset.search(DCM_TextObjectSequence, stack, ESM_fromHere, OFFalse))
  {
    dseq=(DcmSequenceOfItems *)stack.top();
    if (dseq)
    {
      unsigned long numItems = dseq->card();
      for (unsigned int i=0; i<numItems; i++)
      {
        ditem = dseq->getItem(i);
        newObject = new DVPSTextObject();
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

OFCondition DVPSTextObject_PList::write(DcmItem &dset)
{
  if (size()==0) return EC_Normal; // don't write empty Sequence

  OFCondition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  dseq = new DcmSequenceOfItems(DCM_TextObjectSequence);
  if (dseq)
  {
    OFListIterator(DVPSTextObject *) first = list_.begin();
    OFListIterator(DVPSTextObject *) last = list_.end();
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


DVPSTextObject *DVPSTextObject_PList::getTextObject(size_t idx)
{
  OFListIterator(DVPSTextObject *) first = list_.begin();
  OFListIterator(DVPSTextObject *) last = list_.end();
  while (first != last)
  {
    if (idx==0) return *first;
    idx--;
    ++first;
  }
  return NULL;
}

void DVPSTextObject_PList::addTextObject(DVPSTextObject *text)
{
  if (text) list_.push_back(text);
}

DVPSTextObject *DVPSTextObject_PList::removeTextObject(size_t idx)
{
  OFListIterator(DVPSTextObject *) first = list_.begin();
  OFListIterator(DVPSTextObject *) last = list_.end();
  while (first != last)
  {
    if (idx==0)
    {
      DVPSTextObject *result = *first;
      list_.erase(first);
      return result;
    }
    idx--;
    ++first;
  }
  return NULL;
}
