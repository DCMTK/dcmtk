/*
 *
 *  Copyright (C) 1999-2010, OFFIS e.V.
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
 *    classes: DVPSAnnotationContent_PList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:31 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmpstat/dvpsabl.h"
#include "dcmtk/dcmpstat/dvpsab.h"      /* for DVPSAnnotationContent */
#include "dcmtk/dcmpstat/dvpshlp.h"     /* for class DVPSHelper */

/* --------------- class DVPSAnnotationContent_PList --------------- */

DVPSAnnotationContent_PList::DVPSAnnotationContent_PList()
: list_()
{
}

DVPSAnnotationContent_PList::DVPSAnnotationContent_PList(const DVPSAnnotationContent_PList &arg)
: list_()
{
  OFListConstIterator(DVPSAnnotationContent *) first = arg.list_.begin();
  OFListConstIterator(DVPSAnnotationContent *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSAnnotationContent_PList::~DVPSAnnotationContent_PList()
{
  clear();
}

void DVPSAnnotationContent_PList::clear()
{
  OFListIterator(DVPSAnnotationContent *) first = list_.begin();
  OFListIterator(DVPSAnnotationContent *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSAnnotationContent_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DVPSAnnotationContent *newAnnotation = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  if (EC_Normal == dset.search(DCM_RETIRED_AnnotationContentSequence, stack, ESM_fromHere, OFFalse))
  {
    dseq=(DcmSequenceOfItems *)stack.top();
    if (dseq)
    {
      unsigned long numItems = dseq->card();
      for (unsigned int i=0; i<numItems; i++)
      {
        ditem = dseq->getItem(i);
        newAnnotation = new DVPSAnnotationContent();
        if (newAnnotation && ditem)
        {
          result = newAnnotation->read(*ditem);
          list_.push_back(newAnnotation);
        } else result = EC_MemoryExhausted;
      }
    }
  }

  return result;
}

OFCondition DVPSAnnotationContent_PList::write(DcmItem &dset)
{
  if (size()==0) return EC_Normal; // don't write if sequence is empty

  OFCondition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  dseq = new DcmSequenceOfItems(DCM_RETIRED_AnnotationContentSequence);
  if (dseq)
  {
    OFListIterator(DVPSAnnotationContent *) first = list_.begin();
    OFListIterator(DVPSAnnotationContent *) last = list_.end();
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


OFCondition DVPSAnnotationContent_PList::addAnnotationBox(
    const char *instanceuid,
    const char *text,
    Uint16 position)
{
  OFCondition result = EC_Normal;
  DVPSAnnotationContent *newAnnotation = new DVPSAnnotationContent();
  if (newAnnotation)
  {
    result = newAnnotation->setContent(instanceuid, text, position);
    if (EC_Normal == result) list_.push_back(newAnnotation); else delete newAnnotation;
  } else result = EC_MemoryExhausted;
  return result;
}


OFCondition DVPSAnnotationContent_PList::deleteAnnotation(size_t idx)
{
  OFListIterator(DVPSAnnotationContent *) first = list_.begin();
  OFListIterator(DVPSAnnotationContent *) last = list_.end();
  while ((first != last)&&(idx--)) ++first;
  if (first != last)
  {
    delete (*first);
    list_.erase(first);
    return EC_Normal;
  }
  return EC_IllegalCall;
}

OFCondition DVPSAnnotationContent_PList::deleteMultipleAnnotations(size_t number)
{
  OFListIterator(DVPSAnnotationContent *) first = list_.begin();
  OFListIterator(DVPSAnnotationContent *) last = list_.end();
  while ((first != last)&&(number--))
  {
    delete (*first);
    first = list_.erase(first);
  }
  return EC_Normal;
}

DVPSAnnotationContent *DVPSAnnotationContent_PList::getAnnotationBox(size_t idx)
{
  OFListIterator(DVPSAnnotationContent *) first = list_.begin();
  OFListIterator(DVPSAnnotationContent *) last = list_.end();
  while (first != last)
  {
    if (idx==0) return *first;
    idx--;
    ++first;
  }
  return NULL;
}

OFCondition DVPSAnnotationContent_PList::setAnnotationSOPInstanceUID(size_t idx, const char *value)
{
  DVPSAnnotationContent *box = getAnnotationBox(idx);
  if (box) return box->setSOPInstanceUID(value);
  return EC_IllegalCall;
}

const char *DVPSAnnotationContent_PList::getSOPInstanceUID(size_t idx)
{
  DVPSAnnotationContent *box = getAnnotationBox(idx);
  if (box) return box->getSOPInstanceUID();
  return NULL;
}

OFCondition DVPSAnnotationContent_PList::prepareBasicAnnotationBox(size_t idx, DcmItem &dset)
{
  DVPSAnnotationContent *box = getAnnotationBox(idx);
  if (box) return box->prepareBasicAnnotationBox(dset);
  return EC_IllegalCall;
}

void DVPSAnnotationContent_PList::clearAnnotationSOPInstanceUIDs()
{
  OFListIterator(DVPSAnnotationContent *) first = list_.begin();
  OFListIterator(DVPSAnnotationContent *) last = list_.end();
  while (first != last)
  {
    (*first)->setSOPInstanceUID(NULL);
    ++first;
  }
}

/*
 *  $Log: dvpsabl.cc,v $
 *  Revision 1.15  2010-10-14 13:14:31  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.14  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.13  2009-09-30 10:42:38  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.12  2008-04-30 12:38:43  meichel
 *  Fixed compile errors due to changes in attribute tag names
 *
 *  Revision 1.11  2005/12/08 15:46:17  meichel
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
 *  Revision 1.7  2001/11/28 13:56:52  joergr
 *  Check return value of DcmItem::insert() statements where appropriate to
 *  avoid memory leaks when insert procedure fails.
 *
 *  Revision 1.6  2001/09/26 15:36:22  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.5  2001/06/01 15:50:27  meichel
 *  Updated copyright header
 *
 *  Revision 1.4  2000/06/02 16:00:57  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.3  2000/03/08 16:29:01  meichel
 *  Updated copyright header.
 *
 *  Revision 1.2  2000/03/03 14:13:57  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.1  1999/10/19 14:48:27  meichel
 *  added support for the Basic Annotation Box SOP Class
 *    as well as access methods for Max Density and Min Density.
 *
 *
 */
