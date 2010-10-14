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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:32 $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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

/*
 *  $Log: dvpsovl.cc,v $
 *  Revision 1.14  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.13  2009-11-24 14:12:59  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.12  2005-12-08 15:46:37  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.11  2004/02/04 15:57:49  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.10  2003/09/05 08:37:46  meichel
 *  Fixed minor issue that caused certain error messages during the
 *    parse process on a GSPS object to be "swallowed".
 *
 *  Revision 1.9  2003/06/12 18:23:11  joergr
 *  Modified code to use const_iterators where appropriate (required for STL).
 *
 *  Revision 1.8  2003/06/04 10:18:07  meichel
 *  Replaced private inheritance from template with aggregation
 *
 *  Revision 1.7  2001/09/26 15:36:29  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.6  2001/06/01 15:50:34  meichel
 *  Updated copyright header
 *
 *  Revision 1.5  2000/06/02 16:01:03  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.4  2000/03/08 16:29:07  meichel
 *  Updated copyright header.
 *
 *  Revision 1.3  1998/12/22 17:57:17  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *  Revision 1.2  1998/12/14 16:10:45  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:44  meichel
 *  Initial Release.
 *
 *
 */
