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
 *    classes: DVPSCurve_PList
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpscul.h"
#include "dcmtk/dcmpstat/dvpscu.h"      /* for DVPSCurve */


DVPSCurve_PList::DVPSCurve_PList()
: list_()
{
}

DVPSCurve_PList::DVPSCurve_PList(const DVPSCurve_PList &arg)
: list_()
{
  OFListConstIterator(DVPSCurve *) first = arg.list_.begin();
  OFListConstIterator(DVPSCurve *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSCurve_PList::~DVPSCurve_PList()
{
  clear();
}

void DVPSCurve_PList::clear()
{
  OFListIterator(DVPSCurve *) first = list_.begin();
  OFListIterator(DVPSCurve *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSCurve_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DVPSCurve *newCurve = NULL;

  for (Uint8 i=0; i<16; i+=2)
  {
    newCurve = new DVPSCurve();
    if (newCurve)
    {
      result = newCurve->read(dset,i);
      if (result==EC_Normal) list_.push_back(newCurve); else delete newCurve;
    } else return EC_MemoryExhausted;
  }
  return EC_Normal;
}


OFBool DVPSCurve_PList::haveCurveGroup(Uint16 group)
{
  if (getCurveGroup(group)) return OFTrue; else return OFFalse;
}


DVPSCurve *DVPSCurve_PList::getCurveGroup(Uint16 group)
{
  Uint8  lowergroup = (Uint8)(group & 0x00FF);
  OFListIterator(DVPSCurve *) first = list_.begin();
  OFListIterator(DVPSCurve *) last = list_.end();
  while (first != last)
  {
    if ((*first)->getCurveGroup() == lowergroup) return *first;
    ++first;
  }
  return NULL;
}

DVPSCurve *DVPSCurve_PList::getCurve(size_t idx)
{
  OFListIterator(DVPSCurve *) first = list_.begin();
  OFListIterator(DVPSCurve *) last = list_.end();
  while (first != last)
  {
    if (idx==0) return *first;
    idx--;
    ++first;
  }
  return NULL;
}
