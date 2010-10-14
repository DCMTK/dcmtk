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
 *    classes: DVPSTextObject_PList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:33 $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpstxl.h"
#include "dcmtk/dcmpstat/dvpstx.h"      /* for DVPSTextObject */


DVPSTextObject_PList::DVPSTextObject_PList()
: list_()
{
}

DVPSTextObject_PList::DVPSTextObject_PList(const DVPSTextObject_PList &arg)
: list_()
{
  OFListConstIterator(DVPSTextObject *) first = arg.list_.begin();
  OFListConstIterator(DVPSTextObject *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSTextObject_PList::~DVPSTextObject_PList()
{
  clear();
}

void DVPSTextObject_PList::clear()
{
  OFListIterator(DVPSTextObject *) first = list_.begin();
  OFListIterator(DVPSTextObject *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
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
          result = newObject->read(*ditem);
          list_.push_back(newObject);
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
    OFListIterator(DVPSTextObject *) first = list_.begin();
    OFListIterator(DVPSTextObject *) last = list_.end();
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
    if (result==EC_Normal) dset.insert(dseq, OFTrue /*replaceOld*/); else delete dseq;
  } else result = EC_MemoryExhausted;
  return result;
}


DVPSTextObject *DVPSTextObject_PList::getTextObject(size_t idx)
{
  OFListIterator(DVPSTextObject *) first = list_.begin();
  OFListIterator(DVPSTextObject *) last = list_.end();
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
  if (text) list_.push_back(text);
}

DVPSTextObject *DVPSTextObject_PList::removeTextObject(size_t idx)
{
  OFListIterator(DVPSTextObject *) first = list_.begin();
  OFListIterator(DVPSTextObject *) last = list_.end();
  while (first != last)
  {
    if (idx==0)
    {
      DVPSTextObject *result = *first;
      list_.erase(first);
      return result;
    }
    idx--;
    ++first;
  }
  return NULL;
}


/*
 *  $Log: dvpstxl.cc,v $
 *  Revision 1.13  2010-10-14 13:14:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.12  2009-11-24 14:12:59  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.11  2005-12-08 15:46:54  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.10  2004/02/04 15:57:49  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.9  2003/06/12 18:23:11  joergr
 *  Modified code to use const_iterators where appropriate (required for STL).
 *
 *  Revision 1.8  2003/06/04 10:18:07  meichel
 *  Replaced private inheritance from template with aggregation
 *
 *  Revision 1.7  2001/11/28 13:57:07  joergr
 *  Check return value of DcmItem::insert() statements where appropriate to
 *  avoid memory leaks when insert procedure fails.
 *
 *  Revision 1.6  2001/09/26 15:36:35  meichel
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
