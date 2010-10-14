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
 *    classes: DVPSReferencedSeries_PList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:32 $
 *  CVS/RCS Revision: $Revision: 1.21 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmpstat/dvpsrsl.h"
#include "dcmtk/dcmpstat/dvpsrs.h"      /* for DVPSReferencedSeries */
#include "dcmtk/dcmpstat/dvpsri.h"      /* for DVPSReferencedImage, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsdef.h"

DVPSReferencedSeries_PList::DVPSReferencedSeries_PList()
: list_()
{
}

DVPSReferencedSeries_PList::DVPSReferencedSeries_PList(const DVPSReferencedSeries_PList &arg)
: list_()
{
  OFListConstIterator(DVPSReferencedSeries *) first = arg.list_.begin();
  OFListConstIterator(DVPSReferencedSeries *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSReferencedSeries_PList::~DVPSReferencedSeries_PList()
{
  clear();
}

void DVPSReferencedSeries_PList::clear()
{
  OFListIterator(DVPSReferencedSeries *) first = list_.begin();
  OFListIterator(DVPSReferencedSeries *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSReferencedSeries_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DVPSReferencedSeries *newSeries = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  if (EC_Normal == dset.search(DCM_ReferencedSeriesSequence, stack, ESM_fromHere, OFFalse))
  {
    dseq=(DcmSequenceOfItems *)stack.top();
    if (dseq)
    {
      unsigned long numItems = dseq->card();
      for (unsigned int i=0; i<numItems; i++)
      {
        ditem = dseq->getItem(i);
        newSeries = new DVPSReferencedSeries();
        if (newSeries && ditem)
        {
          result = newSeries->read(*ditem);
          list_.push_back(newSeries);
        } else result = EC_MemoryExhausted;
      }
    }
  }

  return result;
}

OFCondition DVPSReferencedSeries_PList::write(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  dseq = new DcmSequenceOfItems(DCM_ReferencedSeriesSequence);
  if (dseq)
  {
    OFListIterator(DVPSReferencedSeries *) first = list_.begin();
    OFListIterator(DVPSReferencedSeries *) last = list_.end();
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

OFBool DVPSReferencedSeries_PList::isValid()
{
  if (list_.size() == 0)
  {
    DCMPSTAT_INFO("referenced series SQ is empty in presentation state");
    return OFFalse;
  }

  OFBool result = OFTrue;
  OFString sopclassuid;

  OFListIterator(DVPSReferencedSeries *) first = list_.begin();
  OFListIterator(DVPSReferencedSeries *) last = list_.end();
  while ((result == OFTrue) && (first != last))
  {
    result = (*first)->isValid(sopclassuid);
    ++first;
  }
  return result;
}

OFBool DVPSReferencedSeries_PList::checkSOPClass(const char *uid)
{

  OFBool result = OFTrue;
  OFString sopclassuid(uid);
  OFListIterator(DVPSReferencedSeries *) first = list_.begin();
  OFListIterator(DVPSReferencedSeries *) last = list_.end();
  while ((result == OFTrue) && (first != last))
  {
    result = (*first)->isValid(sopclassuid);
    ++first;
  }
  return result;
}

void DVPSReferencedSeries_PList::removeImageReference(const char *seriesUID, const char *instanceUID)
{
  DVPSReferencedSeries *series = findSeriesReference(seriesUID);
  if (series) series->removeImageReference(instanceUID);
  return;
}

DVPSReferencedSeries *DVPSReferencedSeries_PList::findSeriesReference(const char *seriesUID)
{
  OFListIterator(DVPSReferencedSeries *) first = list_.begin();
  OFListIterator(DVPSReferencedSeries *) last = list_.end();
  while (first != last)
  {
    if ((*first)->isSeriesUID(seriesUID)) return *first;
    ++first;
  }
  return NULL;
}

DVPSReferencedImage *DVPSReferencedSeries_PList::findImageReference(const char *seriesUID, const char *instanceUID)
{
  DVPSReferencedSeries *series = findSeriesReference(seriesUID);
  if (series) return series->findImageReference(instanceUID); else return NULL;
}

void DVPSReferencedSeries_PList::removeSeriesReference(const char *seriesUID)
{
  OFListIterator(DVPSReferencedSeries *) first = list_.begin();
  OFListIterator(DVPSReferencedSeries *) last = list_.end();
  while (first != last)
  {
    if ((*first)->isSeriesUID(seriesUID))
    {
      delete (*first);
      first = list_.erase(first);
    } else ++first;
  }
  return;
}

OFCondition DVPSReferencedSeries_PList::addImageReference(
    const char *seriesUID,
    const char *sopclassUID,
    const char *instanceUID,
    const char *frames,
    const char *aetitle,
    const char *filesetID,
    const char *filesetUID)

{
  if ((seriesUID==NULL) || (sopclassUID==NULL) || (instanceUID==NULL)) return EC_IllegalCall;

  OFCondition result = EC_Normal;
  if (checkSOPClass(sopclassUID))
  {
    DVPSReferencedSeries *series = findSeriesReference(seriesUID);
    if (series == NULL)
    {
      series = new DVPSReferencedSeries();
      if (series)
      {
        series->setSeriesInstanceUID(seriesUID);
        list_.push_back(series);
      }
    }
    if (series)
    {
      result = series->addImageReference(sopclassUID, instanceUID, frames);
      if (EC_Normal == result) series->setRetrieveLocation(aetitle, filesetID, filesetUID);
    } else result = EC_MemoryExhausted;
  } else result = EC_IllegalCall;
  return result;
}


size_t DVPSReferencedSeries_PList::numberOfImageReferences()
{
  size_t result=0;
  OFListIterator(DVPSReferencedSeries *) first = list_.begin();
  OFListIterator(DVPSReferencedSeries *) last = list_.end();
  while (first != last)
  {
    result += (*first)->numberOfImageReferences();
    ++first;
  }
  return result;
}

OFCondition DVPSReferencedSeries_PList::getImageReference(
    size_t idx,
    OFString& seriesUID,
    OFString& sopclassUID,
    OFString& instanceUID,
    OFString& frames,
    OFString& aetitle,
    OFString& filesetID,
    OFString& filesetUID)
{
  OFListIterator(DVPSReferencedSeries *) first = list_.begin();
  OFListIterator(DVPSReferencedSeries *) last = list_.end();
  OFBool found = OFFalse;
  size_t i;
  while ((!found)&&(first != last))
  {
    i=(*first)->numberOfImageReferences();
    if (i > idx) found = OFTrue; else
    {
      idx -= i;
      ++first;
    }
  }
  if (found) return (*first)->getImageReference(idx, seriesUID, sopclassUID, instanceUID, frames, aetitle, filesetID, filesetUID);
  return EC_IllegalCall;
}

/*
 *  $Log: dvpsrsl.cc,v $
 *  Revision 1.21  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.20  2009-11-24 14:12:59  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.19  2009-09-30 10:42:39  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.18  2006-08-15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.17  2005/12/08 15:46:46  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.16  2004/02/04 15:57:49  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.15  2003/10/15 16:57:14  meichel
 *  Updated error messages generated while parsing presentation states
 *
 *  Revision 1.14  2003/09/05 08:37:46  meichel
 *  Fixed minor issue that caused certain error messages during the
 *    parse process on a GSPS object to be "swallowed".
 *
 *  Revision 1.13  2003/06/12 18:23:11  joergr
 *  Modified code to use const_iterators where appropriate (required for STL).
 *
 *  Revision 1.12  2003/06/04 12:30:28  meichel
 *  Added various includes needed by MSVC5 with STL
 *
 *  Revision 1.11  2003/06/04 10:18:07  meichel
 *  Replaced private inheritance from template with aggregation
 *
 *  Revision 1.10  2001/11/28 13:57:00  joergr
 *  Check return value of DcmItem::insert() statements where appropriate to
 *  avoid memory leaks when insert procedure fails.
 *
 *  Revision 1.9  2001/09/26 15:36:32  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.8  2001/06/01 15:50:37  meichel
 *  Updated copyright header
 *
 *  Revision 1.7  2000/06/02 16:01:06  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.6  2000/03/08 16:29:10  meichel
 *  Updated copyright header.
 *
 *  Revision 1.5  2000/03/03 14:14:04  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.4  1999/07/22 16:40:02  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *  Revision 1.3  1999/01/15 17:32:58  meichel
 *  added methods to DVPresentationState allowing to access the image
 *    references in the presentation state.  Also added methods allowing to
 *    get the width and height of the attached image.
 *
 *  Revision 1.2  1998/12/14 16:10:47  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:47  meichel
 *  Initial Release.
 *
 *
 */
