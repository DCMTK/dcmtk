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
 *    classes: DVPSVOILUT_PList
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpsvll.h"
#include "dcmtk/dcmpstat/dvpsvl.h"      /* for DVPSVOILUT */


DVPSVOILUT_PList::DVPSVOILUT_PList()
: list_()
{
}

DVPSVOILUT_PList::DVPSVOILUT_PList(const DVPSVOILUT_PList &arg)
: list_()
{
  OFListConstIterator(DVPSVOILUT *) first = arg.list_.begin();
  OFListConstIterator(DVPSVOILUT *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSVOILUT_PList::~DVPSVOILUT_PList()
{
  clear();
}

void DVPSVOILUT_PList::clear()
{
  OFListIterator(DVPSVOILUT *) first = list_.begin();
  OFListIterator(DVPSVOILUT *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSVOILUT_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DVPSVOILUT *newObject = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  if (EC_Normal == dset.search(DCM_VOILUTSequence, stack, ESM_fromHere, OFFalse))
  {
    dseq=(DcmSequenceOfItems *)stack.top();
    if (dseq)
    {
      unsigned long numItems = dseq->card();
      for (unsigned int i=0; i<numItems; i++)
      {
        ditem = dseq->getItem(i);
        newObject = new DVPSVOILUT();
        if (newObject && ditem)
        {
          if (EC_Normal == newObject->read(*ditem)) list_.push_back(newObject); else delete(newObject);
        } else result = EC_MemoryExhausted;
      }
    }
  }

  return result;
}


DVPSVOILUT *DVPSVOILUT_PList::getVOILUT(size_t idx)
{
  OFListIterator(DVPSVOILUT *) first = list_.begin();
  OFListIterator(DVPSVOILUT *) last = list_.end();
  while (first != last)
  {
    if (idx==0) return *first;
    idx--;
    ++first;
  }
  return NULL;
}
