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
 *    classes: DVPSVOILUT_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-12-22 17:57:21 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsvll.h"
#include "dvpsvl.h"      /* for DVPSVOILUT */


DVPSVOILUT_PList::DVPSVOILUT_PList()
: OFList<DVPSVOILUT *>()
{
}

DVPSVOILUT_PList::DVPSVOILUT_PList(const DVPSVOILUT_PList &arg)
: OFList<DVPSVOILUT *>()
{
  OFListIterator(DVPSVOILUT *) first = arg.begin();
  OFListIterator(DVPSVOILUT *) last = arg.end();
  while (first != last)
  {     
    push_back((*first)->clone());
    ++first;
  }
}

DVPSVOILUT_PList::~DVPSVOILUT_PList()
{
  clear();
}

void DVPSVOILUT_PList::clear()
{
  OFListIterator(DVPSVOILUT *) first = begin();
  OFListIterator(DVPSVOILUT *) last = end();
  while (first != last)
  {     
    delete (*first);
    first = erase(first);
  }
}

E_Condition DVPSVOILUT_PList::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
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
          if (EC_Normal == newObject->read(*ditem)) push_back(newObject); else delete(newObject);
        } else result = EC_MemoryExhausted;
      }
    }
  }    
  
  return result;
}


DVPSVOILUT *DVPSVOILUT_PList::getVOILUT(size_t idx)
{
  OFListIterator(DVPSVOILUT *) first = begin();
  OFListIterator(DVPSVOILUT *) last = end();
  while (first != last)
  {
    if (idx==0) return *first;
    idx--;
    ++first;
  }
  return NULL;
}

/*
 *  $Log: dvpsvll.cc,v $
 *  Revision 1.1  1998-12-22 17:57:21  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *
 */

