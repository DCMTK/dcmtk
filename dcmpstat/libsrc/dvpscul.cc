/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *    classes: DVPSCurve_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-12-22 17:57:15 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpscul.h"
#include "dvpscu.h"      /* for DVPSCurve */


DVPSCurve_PList::DVPSCurve_PList()
: OFList<DVPSCurve *>()
{
}

DVPSCurve_PList::DVPSCurve_PList(const DVPSCurve_PList &arg)
: OFList<DVPSCurve *>()
{
  OFListIterator(DVPSCurve *) first = arg.begin();
  OFListIterator(DVPSCurve *) last = arg.end();
  while (first != last)
  {     
    push_back((*first)->clone());
    ++first;
  }
}

DVPSCurve_PList::~DVPSCurve_PList()
{
  clear();
}

void DVPSCurve_PList::clear()
{
  OFListIterator(DVPSCurve *) first = begin();
  OFListIterator(DVPSCurve *) last = end();
  while (first != last)
  {     
    delete (*first);
    first = erase(first);
  }
}

E_Condition DVPSCurve_PList::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DVPSCurve *newCurve = NULL;
  
  for (Uint8 i=0; i<16; i+=2)
  {
    newCurve = new DVPSCurve();
    if (newCurve)
    {
      result = newCurve->read(dset,i);
      if (result==EC_Normal) push_back(newCurve); else delete newCurve;
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
  OFListIterator(DVPSCurve *) first = begin();
  OFListIterator(DVPSCurve *) last = end();
  while (first != last)
  {
    if ((*first)->getCurveGroup() == lowergroup) return *first;
    ++first;
  }
  return NULL;
}

DVPSCurve *DVPSCurve_PList::getCurve(size_t idx)
{
  OFListIterator(DVPSCurve *) first = begin();
  OFListIterator(DVPSCurve *) last = end();
  while (first != last)
  {
    if (idx==0) return *first;
    idx--;
    ++first;
  }
  return NULL;
}

/*
 *  $Log: dvpscul.cc,v $
 *  Revision 1.1  1998-12-22 17:57:15  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *
 */

