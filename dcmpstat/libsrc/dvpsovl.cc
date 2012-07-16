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
 *    classes: DVPSOverlay_PList
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpsovl.h"
#include "dcmtk/dcmpstat/dvpsov.h"      /* for DVPSOverlay */


DVPSOverlay_PList::DVPSOverlay_PList()
: list_()
{
}

DVPSOverlay_PList::DVPSOverlay_PList(const DVPSOverlay_PList &arg)
: list_()
{
  OFListConstIterator(DVPSOverlay *) first = arg.list_.begin();
  OFListConstIterator(DVPSOverlay *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSOverlay_PList::~DVPSOverlay_PList()
{
  clear();
}

void DVPSOverlay_PList::clear()
{
  OFListIterator(DVPSOverlay *) first = list_.begin();
  OFListIterator(DVPSOverlay *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSOverlay_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DcmTagKey key(0x6000,0x3000);
  DVPSOverlay *newOverlay = NULL;

  for (Uint8 i=0; i<16; i+=2)
  {
    if (result==EC_Normal)
    {
      stack.clear();
      key.setGroup(0x6000+i);
      if (EC_Normal == dset.search(key, stack, ESM_fromHere, OFFalse))
      {
        newOverlay = new DVPSOverlay();
        if (newOverlay)
        {
          result = newOverlay->read(dset,i);
          list_.push_back(newOverlay);
        } else result = EC_MemoryExhausted;
      }
    }
  }
  return result;
}

OFCondition DVPSOverlay_PList::write(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  OFListIterator(DVPSOverlay *) first = list_.begin();
  OFListIterator(DVPSOverlay *) last = list_.end();
  while (first != last)
  {
    if (result==EC_Normal) result = (*first)->write(dset);
    ++first;
  }
  return result;
}

OFBool DVPSOverlay_PList::haveOverlayGroup(Uint16 group)
{
  if (getOverlayGroup(group)) return OFTrue; else return OFFalse;
}

DVPSOverlay *DVPSOverlay_PList::getOverlayGroup(Uint16 group)
{
  Uint8  lowergroup = (Uint8)(group & 0x00FF);
  OFListIterator(DVPSOverlay *) first = list_.begin();
  OFListIterator(DVPSOverlay *) last = list_.end();
  while (first != last)
  {
    if ((*first)->getOverlayGroup() == lowergroup) return *first;
    ++first;
  }
  return NULL;
}

DVPSOverlay *DVPSOverlay_PList::getOverlay(size_t idx)
{
  OFListIterator(DVPSOverlay *) first = list_.begin();
  OFListIterator(DVPSOverlay *) last = list_.end();
  while (first != last)
  {
    if (idx==0) return *first;
    idx--;
    ++first;
  }
  return NULL;
}

OFCondition DVPSOverlay_PList::removeOverlay(size_t idx)
{
  OFListIterator(DVPSOverlay *) first = list_.begin();
  OFListIterator(DVPSOverlay *) last = list_.end();
  while (first != last)
  {
    if (idx==0)
    {
      delete (*first);
      first = list_.erase(first);
      return EC_Normal;
    }
    idx--;
    ++first;
  }
  return EC_IllegalCall;
}


OFCondition DVPSOverlay_PList::changeOverlayGroup(size_t idx, Uint16 newGroup)
{
  if ((newGroup < 0x6000)||(newGroup > 0x601F)) return EC_IllegalCall;
  DVPSOverlay *overlay = getOverlay(idx);
  if (overlay)
  {
    if (newGroup == (overlay->getOverlayGroup()+0x6000)) return EC_Normal;
    if (haveOverlayGroup(newGroup)) return EC_IllegalCall; // don't allocate twice
    overlay->setOverlayGroup((Uint8)(newGroup-0x6000));
    return EC_Normal;
  }
  return EC_IllegalCall;
}

OFCondition DVPSOverlay_PList::addOverlay(DcmItem& overlayIOD, Uint16 groupInItem, Uint16 newGroup)
{
  if ((groupInItem < 0x6000)||(groupInItem > 0x601F)) return EC_IllegalCall;
  if ((newGroup < 0x6000)||(newGroup > 0x601F)) return EC_IllegalCall;
  if (haveOverlayGroup(newGroup)) return EC_IllegalCall; // don't allocate twice

  DcmStack stack;
  DcmTagKey key(groupInItem,0x3000);
  DVPSOverlay *newOverlay = NULL;

  OFCondition result = overlayIOD.search(key, stack, ESM_fromHere, OFFalse);
  if (EC_Normal == result)
  {
    newOverlay = new DVPSOverlay();
    if (newOverlay)
    {
      result = newOverlay->read(overlayIOD,(Uint8)(groupInItem-0x6000), (Uint8)(newGroup-0x6000));
      if (EC_Normal==result) list_.push_back(newOverlay); else delete newOverlay;
    } else result = EC_MemoryExhausted;
  }
  return result;
}
