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
 *    classes: DVPSTextObject_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-09-26 15:36:35 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpstxl.h"
#include "dvpstx.h"      /* for DVPSTextObject */


DVPSTextObject_PList::DVPSTextObject_PList()
: OFList<DVPSTextObject *>()
, logstream(&ofConsole)
, verboseMode(OFFalse)
, debugMode(OFFalse)
{
}

DVPSTextObject_PList::DVPSTextObject_PList(const DVPSTextObject_PList &arg)
: OFList<DVPSTextObject *>()
, logstream(arg.logstream)
, verboseMode(arg.verboseMode)
, debugMode(arg.debugMode)
{
  OFListIterator(DVPSTextObject *) first = arg.begin();
  OFListIterator(DVPSTextObject *) last = arg.end();
  while (first != last)
  {     
    push_back((*first)->clone());
    ++first;
  }
}

DVPSTextObject_PList::~DVPSTextObject_PList()
{
  clear();
}

void DVPSTextObject_PList::clear()
{
  OFListIterator(DVPSTextObject *) first = begin();
  OFListIterator(DVPSTextObject *) last = end();
  while (first != last)
  {     
    delete (*first);
    first = erase(first);
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
          newObject->setLog(logstream, verboseMode, debugMode);
          result = newObject->read(*ditem);
          push_back(newObject);
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
    OFListIterator(DVPSTextObject *) first = begin();
    OFListIterator(DVPSTextObject *) last = end();
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
    if (result==EC_Normal) dset.insert(dseq); else delete dseq;
  } else result = EC_MemoryExhausted;
  return result;
}


DVPSTextObject *DVPSTextObject_PList::getTextObject(size_t idx)
{
  OFListIterator(DVPSTextObject *) first = begin();
  OFListIterator(DVPSTextObject *) last = end();
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
  if (text) push_back(text);
}

DVPSTextObject *DVPSTextObject_PList::removeTextObject(size_t idx)
{
  OFListIterator(DVPSTextObject *) first = begin();
  OFListIterator(DVPSTextObject *) last = end();
  while (first != last)
  {
    if (idx==0) 
    {
      DVPSTextObject *result = *first;
      erase(first);
      return result;
    }
    idx--;
    ++first;
  }
  return NULL;
}

void DVPSTextObject_PList::setLog(OFConsole *stream, OFBool verbMode, OFBool dbgMode)
{
  if (stream) logstream = stream; else logstream = &ofConsole;
  verboseMode = verbMode;
  debugMode = dbgMode;
  OFListIterator(DVPSTextObject *) first = begin();
  OFListIterator(DVPSTextObject *) last = end();
  while (first != last)
  {
    (*first)->setLog(logstream, verbMode, dbgMode);
    ++first;
  }	
}


/*
 *  $Log: dvpstxl.cc,v $
 *  Revision 1.6  2001-09-26 15:36:35  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.5  2001/06/01 15:50:41  meichel
 *  Updated copyright header
 *
 *  Revision 1.4  2000/06/02 16:01:09  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.3  2000/03/08 16:29:13  meichel
 *  Updated copyright header.
 *
 *  Revision 1.2  1998/12/14 16:10:49  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:49  meichel
 *  Initial Release.
 *
 *
 */

