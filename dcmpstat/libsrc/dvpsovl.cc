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
 *    classes: DVPSOverlay_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-12-14 16:10:45 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsovl.h"
#include "dvpsov.h"      /* for DVPSOverlay */


DVPSOverlay_PList::DVPSOverlay_PList()
: OFList<DVPSOverlay *>()
{
}

DVPSOverlay_PList::DVPSOverlay_PList(const DVPSOverlay_PList &arg)
: OFList<DVPSOverlay *>()
{
  OFListIterator(DVPSOverlay *) first = arg.begin();
  OFListIterator(DVPSOverlay *) last = arg.end();
  while (first != last)
  {     
    push_back((*first)->clone());
    ++first;
  }
}

DVPSOverlay_PList::~DVPSOverlay_PList()
{
  clear();
}

void DVPSOverlay_PList::clear()
{
  OFListIterator(DVPSOverlay *) first = begin();
  OFListIterator(DVPSOverlay *) last = end();
  while (first != last)
  {     
    delete (*first);
    first = erase(first);
  }
}

E_Condition DVPSOverlay_PList::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
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
          push_back(newOverlay);
        } else result = EC_MemoryExhausted;
      }
    }
  }
  return result;
}

E_Condition DVPSOverlay_PList::write(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  OFListIterator(DVPSOverlay *) first = begin();
  OFListIterator(DVPSOverlay *) last = end();
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


OFBool DVPSOverlay_PList::overlaySizeMatches(Uint16 group, unsigned long x, unsigned long y)
{
  DVPSOverlay *ol = getOverlayGroup(group);
  if (ol) return ol->overlaySizeMatches(x, y); else return OFFalse;
}

DVPSOverlay *DVPSOverlay_PList::getOverlayGroup(Uint16 group)
{
  Uint8  lowergroup = (Uint8)(group & 0x00FF);
  OFListIterator(DVPSOverlay *) first = begin();
  OFListIterator(DVPSOverlay *) last = end();
  while (first != last)
  {
    if ((*first)->getOverlayGroup() == lowergroup) return *first;
    ++first;
  }
  return NULL;
}

/*
 *  $Log: dvpsovl.cc,v $
 *  Revision 1.2  1998-12-14 16:10:45  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:44  meichel
 *  Initial Release.
 *
 *
 */

