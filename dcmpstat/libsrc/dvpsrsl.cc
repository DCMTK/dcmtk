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
 *    classes: DVPSReferencedSeries_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-01-15 17:32:58 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dvpsrsl.h"
#include "dvpsrs.h"      /* for DVPSReferencedSeries */

DVPSReferencedSeries_PList::DVPSReferencedSeries_PList()
: OFList<DVPSReferencedSeries *>()
{
}

DVPSReferencedSeries_PList::DVPSReferencedSeries_PList(const DVPSReferencedSeries_PList &arg)
: OFList<DVPSReferencedSeries *>()
{
  OFListIterator(DVPSReferencedSeries *) first = arg.begin();
  OFListIterator(DVPSReferencedSeries *) last = arg.end();
  while (first != last)
  {     
    push_back((*first)->clone());
    ++first;
  }
}

DVPSReferencedSeries_PList::~DVPSReferencedSeries_PList()
{
  clear();
}

void DVPSReferencedSeries_PList::clear()
{
  OFListIterator(DVPSReferencedSeries *) first = begin();
  OFListIterator(DVPSReferencedSeries *) last = end();
  while (first != last)
  {     
    delete (*first);
    first = erase(first);
  }
}

E_Condition DVPSReferencedSeries_PList::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
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
          push_back(newSeries);
        } else result = EC_MemoryExhausted;
      }
    }
  }    
  
  return result;
}

E_Condition DVPSReferencedSeries_PList::write(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  dseq = new DcmSequenceOfItems(DCM_ReferencedSeriesSequence);
  if (dseq)
  {
    OFListIterator(DVPSReferencedSeries *) first = begin();
    OFListIterator(DVPSReferencedSeries *) last = end();
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

OFBool DVPSReferencedSeries_PList::isValid()
{
  if (size() == 0) 
  {
#ifdef DEBUG
    cerr << "referenced series SQ is empty in presentation state" << endl;
#endif
    return OFFalse;
  }
  
  OFBool result = OFTrue;
  OFString sopclassuid;
   
  OFListIterator(DVPSReferencedSeries *) first = begin();
  OFListIterator(DVPSReferencedSeries *) last = end();
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
  OFListIterator(DVPSReferencedSeries *) first = begin();
  OFListIterator(DVPSReferencedSeries *) last = end();
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
  OFListIterator(DVPSReferencedSeries *) first = begin();
  OFListIterator(DVPSReferencedSeries *) last = end();
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
  OFListIterator(DVPSReferencedSeries *) first = begin();
  OFListIterator(DVPSReferencedSeries *) last = end();
  while (first != last)
  {
    if ((*first)->isSeriesUID(seriesUID))
    {
      delete (*first);
      first = erase(first);     
    } else ++first;
  }
  return;
}

E_Condition DVPSReferencedSeries_PList::addImageReference(
    const char *seriesUID,
    const char *sopclassUID,
    const char *instanceUID, 
    const char *frames)
{
  if ((seriesUID==NULL) || (sopclassUID==NULL) || (instanceUID==NULL)) return EC_IllegalCall;
  
  E_Condition result = EC_Normal;
  if (checkSOPClass(sopclassUID))
  {
    DVPSReferencedSeries *series = findSeriesReference(seriesUID);
    if (series)
    {
      result = series->addImageReference(sopclassUID, instanceUID, frames);
    } else {
      series = new DVPSReferencedSeries();
      if (series)
      {
        series->setSeriesInstanceUID(seriesUID);
        result = series->addImageReference(sopclassUID, instanceUID, frames);
        if (result==EC_Normal) push_back(series); else delete series;
      } else result = EC_MemoryExhausted;
    }
  } else result = EC_IllegalCall;

  return result;
}


size_t DVPSReferencedSeries_PList::numberOfImageReferences()
{
  size_t result=0;
  OFListIterator(DVPSReferencedSeries *) first = begin();
  OFListIterator(DVPSReferencedSeries *) last = end();
  while (first != last)
  {
    result += (*first)->numberOfImageReferences();
    ++first;
  }
  return result;
}

E_Condition DVPSReferencedSeries_PList::getImageReference(
    size_t idx,
    OFString& seriesUID,
    OFString& sopclassUID,
    OFString& instanceUID, 
    OFString& frames)
{
  OFListIterator(DVPSReferencedSeries *) first = begin();
  OFListIterator(DVPSReferencedSeries *) last = end();
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
  if (found) return (*first)->getImageReference(idx, seriesUID, sopclassUID, instanceUID, frames);
  return EC_IllegalCall;
}

/*
 *  $Log: dvpsrsl.cc,v $
 *  Revision 1.3  1999-01-15 17:32:58  meichel
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

