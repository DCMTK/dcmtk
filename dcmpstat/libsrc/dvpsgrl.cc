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
 *    classes: DVPSGraphicObject_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-11-27 14:50:43 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsgrl.h"
#include "dvpsgr.h"      /* for DVPSGraphicObject */


DVPSGraphicObject_PList::DVPSGraphicObject_PList()
: OFList<DVPSGraphicObject *>()
{
}

DVPSGraphicObject_PList::DVPSGraphicObject_PList(const DVPSGraphicObject_PList &arg)
: OFList<DVPSGraphicObject *>()
{
  OFListIterator(DVPSGraphicObject *) first = arg.begin();
  OFListIterator(DVPSGraphicObject *) last = arg.end();
  while (first != last)
  {     
    push_back((*first)->clone());
    ++first;
  }
}

DVPSGraphicObject_PList::~DVPSGraphicObject_PList()
{
  clear();
}

void DVPSGraphicObject_PList::clear()
{
  OFListIterator(DVPSGraphicObject *) first = begin();
  OFListIterator(DVPSGraphicObject *) last = end();
  while (first != last)
  {     
    delete (*first);
    first = erase(first);
  }
}

E_Condition DVPSGraphicObject_PList::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmStack stack;
  DVPSGraphicObject *newObject = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;
  
  if (EC_Normal == dset.search(DCM_GraphicObjectSequence, stack, ESM_fromHere, OFFalse))
  {
    dseq=(DcmSequenceOfItems *)stack.top();
    if (dseq)
    {
      unsigned long numItems = dseq->card();
      for (unsigned int i=0; i<numItems; i++)
      {
        ditem = dseq->getItem(i);
        newObject = new DVPSGraphicObject();
        if (newObject && ditem)
        {
          result = newObject->read(*ditem);
          push_back(newObject);
        } else result = EC_MemoryExhausted;
      }
    }
  }    
  
  return result;
}

E_Condition DVPSGraphicObject_PList::write(DcmItem &dset)
{
  if (size()==0) return EC_Normal; // don't write empty Sequence
  
  E_Condition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  dseq = new DcmSequenceOfItems(DCM_GraphicObjectSequence);
  if (dseq)
  {
    OFListIterator(DVPSGraphicObject *) first = begin();
    OFListIterator(DVPSGraphicObject *) last = end();
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


/*
 *  $Log: dvpsgrl.cc,v $
 *  Revision 1.1  1998-11-27 14:50:43  meichel
 *  Initial Release.
 *
 *
 */

