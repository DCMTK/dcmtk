/*
 *
 *  Copyright (C) 1998-2001, OFFIS
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
 *  Update Date:      $Date: 2001-09-26 15:36:29 $
 *  CVS/RCS Revision: $Revision: 1.7 $
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
, logstream(&ofConsole)
, verboseMode(OFFalse)
, debugMode(OFFalse)
{
}

DVPSOverlay_PList::DVPSOverlay_PList(const DVPSOverlay_PList &arg)
: OFList<DVPSOverlay *>()
, logstream(arg.logstream)
, verboseMode(arg.verboseMode)
, debugMode(arg.debugMode)
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
          push_back(newOverlay);
        } else result = EC_MemoryExhausted;
      }
    }
  }
  return result;
}

OFCondition DVPSOverlay_PList::write(DcmItem &dset)
{
  OFCondition result = EC_Normal;
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

DVPSOverlay *DVPSOverlay_PList::getOverlay(size_t idx)
{
  OFListIterator(DVPSOverlay *) first = begin();
  OFListIterator(DVPSOverlay *) last = end();
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
  OFListIterator(DVPSOverlay *) first = begin();
  OFListIterator(DVPSOverlay *) last = end();
  while (first != last)
  {
    if (idx==0) 
    {
      delete (*first);
      first = erase(first);
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
      if (EC_Normal==result) push_back(newOverlay); else delete newOverlay;
    } else result = EC_MemoryExhausted;
  }
  return result;
}

void DVPSOverlay_PList::setLog(OFConsole *stream, OFBool verbMode, OFBool dbgMode)
{
  if (stream) logstream = stream; else logstream = &ofConsole;
  verboseMode = verbMode;
  debugMode = dbgMode;
  OFListIterator(DVPSOverlay *) first = begin();
  OFListIterator(DVPSOverlay *) last = end();
  while (first != last)
  {
    (*first)->setLog(logstream, verbMode, dbgMode);
    ++first;
  }	
}

/*
 *  $Log: dvpsovl.cc,v $
 *  Revision 1.7  2001-09-26 15:36:29  meichel
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

