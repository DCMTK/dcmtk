/*
 *
 *  Copyright (C) 1998-2018, OFFIS e.V.
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
 *    classes: DVPSVOIWindow_PList
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvrds.h"
#include "dcmtk/dcmpstat/dvpsvwl.h"
#include "dcmtk/dcmpstat/dvpsvw.h"      /* for DVPSVOIWindow */
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */


DVPSVOIWindow_PList::DVPSVOIWindow_PList()
: list_()
{
}

DVPSVOIWindow_PList::DVPSVOIWindow_PList(const DVPSVOIWindow_PList &arg)
: list_()
{
  OFListConstIterator(DVPSVOIWindow *) first = arg.list_.begin();
  OFListConstIterator(DVPSVOIWindow *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSVOIWindow_PList::~DVPSVOIWindow_PList()
{
  clear();
}

void DVPSVOIWindow_PList::clear()
{
  OFListIterator(DVPSVOIWindow *) first = list_.begin();
  OFListIterator(DVPSVOIWindow *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSVOIWindow_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DVPSVOIWindow *newObject = NULL;

  DcmDecimalString windowCenter(DCM_WindowCenter);
  DcmDecimalString windowWidth(DCM_WindowWidth);
  DcmLongString windowCenterWidthExplanation(DCM_WindowCenterWidthExplanation);

  READ_FROM_DATASET(DcmDecimalString, EVR_DS, windowCenter)
  READ_FROM_DATASET(DcmDecimalString, EVR_DS, windowWidth)
  READ_FROM_DATASET(DcmLongString, EVR_LO, windowCenterWidthExplanation)

  DcmLongString *expl=NULL;
  if (windowCenterWidthExplanation.getVM() > 0) expl = &windowCenterWidthExplanation;

  size_t vm = (size_t)windowCenter.getVM();
  for (size_t i=0; i<vm; i++)
  {
      newObject = new DVPSVOIWindow();
      if (newObject)
      {
        if (EC_Normal == newObject->read(i, windowCenter, windowWidth, expl)) list_.push_back(newObject);
        else delete(newObject);
      } else result = EC_MemoryExhausted;
  }
  return result;
}


DVPSVOIWindow *DVPSVOIWindow_PList::getVOIWindow(size_t idx)
{
  OFListIterator(DVPSVOIWindow *) first = list_.begin();
  OFListIterator(DVPSVOIWindow *) last = list_.end();
  while (first != last)
  {
    if (idx==0) return *first;
    idx--;
    ++first;
  }
  return NULL;
}
