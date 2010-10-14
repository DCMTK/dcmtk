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
 *    classes: DVPSGraphicAnnotation_PList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:32 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpsgal.h"
#include "dcmtk/dcmpstat/dvpsga.h"      /* for DVPSGraphicAnnotation */
#include "dcmtk/ofstd/ofstring.h"    /* for OFString */
#include "dcmtk/dcmpstat/dvpstx.h"      /* for DVPSTextObject */
#include "dcmtk/dcmpstat/dvpsgr.h"      /* for DVPSGraphicObject */
#include "dcmtk/dcmpstat/dvpsri.h"      /* for DVPSReferencedImage, needed by MSVC5 with STL */

DVPSGraphicAnnotation_PList::DVPSGraphicAnnotation_PList()
: list_()
{
}

DVPSGraphicAnnotation_PList::DVPSGraphicAnnotation_PList(const DVPSGraphicAnnotation_PList &arg)
: list_()
{
  OFListConstIterator(DVPSGraphicAnnotation *) first = arg.list_.begin();
  OFListConstIterator(DVPSGraphicAnnotation *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSGraphicAnnotation_PList::~DVPSGraphicAnnotation_PList()
{
  clear();
}

void DVPSGraphicAnnotation_PList::clear()
{
  OFListIterator(DVPSGraphicAnnotation *) first = list_.begin();
  OFListIterator(DVPSGraphicAnnotation *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSGraphicAnnotation_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DVPSGraphicAnnotation *newObject = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  if (EC_Normal == dset.search(DCM_GraphicAnnotationSequence, stack, ESM_fromHere, OFFalse))
  {
    dseq=(DcmSequenceOfItems *)stack.top();
    if (dseq)
    {
      unsigned long numItems = dseq->card();
      for (unsigned int i=0; i<numItems; i++)
      {
        ditem = dseq->getItem(i);
        newObject = new DVPSGraphicAnnotation();
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

OFCondition DVPSGraphicAnnotation_PList::write(DcmItem &dset)
{
  if (list_.size()==0) return EC_Normal; // don't write empty Sequence

  OFCondition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  dseq = new DcmSequenceOfItems(DCM_GraphicAnnotationSequence);
  if (dseq)
  {
    OFListIterator(DVPSGraphicAnnotation *) first = list_.begin();
    OFListIterator(DVPSGraphicAnnotation *) last = list_.end();
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


void DVPSGraphicAnnotation_PList::renameLayer(const char *oldName, const char *newName)
{
  if ((oldName==NULL)||(newName==NULL)) return;

  OFString aString(oldName);
  OFListIterator(DVPSGraphicAnnotation *) first = list_.begin();
  OFListIterator(DVPSGraphicAnnotation *) last = list_.end();
  while (first != last)
  {
    if (aString == (*first)->getAnnotationLayer())
    {
      (*first)->setAnnotationLayer(newName);
    }
    ++first;
  }
  return;
}

void DVPSGraphicAnnotation_PList::removeLayer(const char *name)
{
  if (name==NULL) return;

  OFString aString(name);
  OFListIterator(DVPSGraphicAnnotation *) first = list_.begin();
  OFListIterator(DVPSGraphicAnnotation *) last = list_.end();
  while (first != last)
  {
    if (aString == (*first)->getAnnotationLayer())
    {
      delete (*first);
      first = list_.erase(first);
    } else ++first;
  }
  return;
}

void DVPSGraphicAnnotation_PList::cleanupLayers()
{
  OFListIterator(DVPSGraphicAnnotation *) first = list_.begin();
  OFListIterator(DVPSGraphicAnnotation *) last = list_.end();
  while (first != last)
  {
    if ((*first)->isEmpty())
    {
      delete (*first);
      first = list_.erase(first);
    } else ++first;
  }
  return;
}


OFBool DVPSGraphicAnnotation_PList::usesLayerName(const char *name)
{
  if (name==NULL) return OFFalse;

  OFString aString(name);
  OFListIterator(DVPSGraphicAnnotation *) first = list_.begin();
  OFListIterator(DVPSGraphicAnnotation *) last = list_.end();
  while (first != last)
  {
    if (aString == (*first)->getAnnotationLayer()) return OFTrue;
    ++first;
  }
  return OFFalse;
}


size_t DVPSGraphicAnnotation_PList::getNumberOfTextObjects(const char *layer, const char *instanceUID, unsigned long frame)
{
  if (layer==NULL) return 0;

  size_t result = 0;
  OFString aString(layer);
  OFListIterator(DVPSGraphicAnnotation *) first = list_.begin();
  OFListIterator(DVPSGraphicAnnotation *) last = list_.end();
  while (first != last)
  {
    if ((aString == (*first)->getAnnotationLayer()) && ((*first)->isApplicable(instanceUID, frame)))
    {
      result += (*first)->getNumberOfTextObjects();
    }
    ++first;
  }
  return result;
}

DVPSTextObject *DVPSGraphicAnnotation_PList::getTextObject(const char *layer, const char *instanceUID, unsigned long frame, size_t idx)
{
  if (layer==NULL) return NULL;

  size_t tmp = 0;
  OFString aString(layer);
  OFListIterator(DVPSGraphicAnnotation *) first = list_.begin();
  OFListIterator(DVPSGraphicAnnotation *) last = list_.end();
  while (first != last)
  {
    if ((aString == (*first)->getAnnotationLayer()) && ((*first)->isApplicable(instanceUID, frame)))
    {
      tmp = (*first)->getNumberOfTextObjects();
      if (idx < tmp) return (*first)->getTextObject(idx); else idx -= tmp;
    }
    ++first;
  }
  return NULL;
}


DVPSTextObject *DVPSGraphicAnnotation_PList::addTextObject(
    const char *layer,
    const char *sopclassUID,
    const char *instanceUID,
    unsigned long frame,
    DVPSObjectApplicability applicability,
    DVPSTextObject *text)
{
  if (layer==NULL) return NULL;

  if (text==NULL)
  {
    text = new DVPSTextObject();
  }
  if (text==NULL) return NULL;

  DVPSGraphicAnnotation *annotation = NULL;
  OFString aString(layer);
  OFListIterator(DVPSGraphicAnnotation *) first = list_.begin();
  OFListIterator(DVPSGraphicAnnotation *) last = list_.end();
  while (first != last)
  {
    if ((aString == (*first)->getAnnotationLayer()) && ((*first)->isApplicable(instanceUID, frame, applicability)))
    {
      annotation = *first;
      first = last;
    } else ++first;
  }

  if (annotation==NULL)
  {
    annotation = new DVPSGraphicAnnotation();
    if (annotation)
    {
      annotation->setAnnotationLayer(layer);
      if (applicability != DVPSB_allImages) annotation->addImageReference(sopclassUID, instanceUID, frame, applicability);
      list_.push_back(annotation);
    }
  }

  if (annotation)
  {
    annotation->addTextObject(text);
    return text;
  }

  delete text;
  return NULL;
}


OFCondition DVPSGraphicAnnotation_PList::removeTextObject(const char *layer, const char *instanceUID, unsigned long frame, size_t idx)
{
  if (layer==NULL) return EC_IllegalCall;

  size_t tmp = 0;
  OFString aString(layer);
  OFListIterator(DVPSGraphicAnnotation *) first = list_.begin();
  OFListIterator(DVPSGraphicAnnotation *) last = list_.end();
  while (first != last)
  {
    if ((aString == (*first)->getAnnotationLayer()) && ((*first)->isApplicable(instanceUID, frame)))
    {
      tmp = (*first)->getNumberOfTextObjects();
      if (idx < tmp)
      {
        DVPSTextObject *textObject = (*first)->removeTextObject(idx);
        if (textObject)
        {
          delete textObject;
          return EC_Normal;
        } else return EC_IllegalCall;
      } else idx -= tmp;
    }
    ++first;
  }
  return EC_IllegalCall;
}


OFCondition DVPSGraphicAnnotation_PList::moveTextObject(
    const char *old_layer,
    const char *sopclassUID,
    const char *instanceUID,
    unsigned long frame,
    size_t idx,
    DVPSObjectApplicability applicability,
    const char *new_layer)
{
  if (old_layer==NULL) return EC_IllegalCall;
  if (new_layer==NULL) return EC_IllegalCall;

  size_t tmp = 0;
  OFString aString(old_layer);
  OFListIterator(DVPSGraphicAnnotation *) first = list_.begin();
  OFListIterator(DVPSGraphicAnnotation *) last = list_.end();
  while (first != last)
  {
    if ((aString == (*first)->getAnnotationLayer()) && ((*first)->isApplicable(instanceUID, frame)))
    {
      tmp = (*first)->getNumberOfTextObjects();
      if (idx < tmp)
      {
        DVPSTextObject *textObject = (*first)->removeTextObject(idx);
        if (textObject)
        {
          textObject = addTextObject(new_layer, sopclassUID, instanceUID, frame, applicability, textObject);
          if (textObject) return EC_Normal; else return EC_MemoryExhausted;
        } else return EC_IllegalCall;
      } else idx -= tmp;
    }
    ++first;
  }
  return EC_IllegalCall;
}


size_t DVPSGraphicAnnotation_PList::getNumberOfGraphicObjects(const char *layer, const char *instanceUID, unsigned long frame)
{
  if (layer==NULL) return 0;

  size_t result = 0;
  OFString aString(layer);
  OFListIterator(DVPSGraphicAnnotation *) first = list_.begin();
  OFListIterator(DVPSGraphicAnnotation *) last = list_.end();
  while (first != last)
  {
    if ((aString == (*first)->getAnnotationLayer()) && ((*first)->isApplicable(instanceUID, frame)))
    {
      result += (*first)->getNumberOfGraphicObjects();
    }
    ++first;
  }
  return result;
}

DVPSGraphicObject *DVPSGraphicAnnotation_PList::getGraphicObject(const char *layer, const char *instanceUID, unsigned long frame, size_t idx)
{
  if (layer==NULL) return NULL;

  size_t tmp = 0;
  OFString aString(layer);
  OFListIterator(DVPSGraphicAnnotation *) first = list_.begin();
  OFListIterator(DVPSGraphicAnnotation *) last = list_.end();
  while (first != last)
  {
    if ((aString == (*first)->getAnnotationLayer()) && ((*first)->isApplicable(instanceUID, frame)))
    {
      tmp = (*first)->getNumberOfGraphicObjects();
      if (idx < tmp) return (*first)->getGraphicObject(idx); else idx -= tmp;
    }
    ++first;
  }
  return NULL;
}


DVPSGraphicObject *DVPSGraphicAnnotation_PList::addGraphicObject(
    const char *layer,
    const char *sopclassUID,
    const char *instanceUID,
    unsigned long frame,
    DVPSObjectApplicability applicability,
    DVPSGraphicObject *graphic)
{
  if (layer==NULL) return NULL;

  if (graphic==NULL) graphic = new DVPSGraphicObject();
  if (graphic==NULL) return NULL;

  DVPSGraphicAnnotation *annotation = NULL;
  OFString aString(layer);
  OFListIterator(DVPSGraphicAnnotation *) first = list_.begin();
  OFListIterator(DVPSGraphicAnnotation *) last = list_.end();
  while (first != last)
  {
    if ((aString == (*first)->getAnnotationLayer()) && ((*first)->isApplicable(instanceUID, frame, applicability)))
    {
      annotation = *first;
      first = last;
    } else ++first;
  }

  if (annotation==NULL)
  {
    annotation = new DVPSGraphicAnnotation();
    if (annotation)
    {
      annotation->setAnnotationLayer(layer);
      if (applicability != DVPSB_allImages) annotation->addImageReference(sopclassUID, instanceUID, frame, applicability);
      list_.push_back(annotation);
    }
  }

  if (annotation)
  {
    annotation->addGraphicObject(graphic);
    return graphic;
  }

  delete graphic;
  return NULL;
}


OFCondition DVPSGraphicAnnotation_PList::removeGraphicObject(const char *layer, const char *instanceUID, unsigned long frame, size_t idx)
{
  if (layer==NULL) return EC_IllegalCall;

  size_t tmp = 0;
  OFString aString(layer);
  OFListIterator(DVPSGraphicAnnotation *) first = list_.begin();
  OFListIterator(DVPSGraphicAnnotation *) last = list_.end();
  while (first != last)
  {
    if ((aString == (*first)->getAnnotationLayer()) && ((*first)->isApplicable(instanceUID, frame)))
    {
      tmp = (*first)->getNumberOfGraphicObjects();
      if (idx < tmp)
      {
        DVPSGraphicObject *graphicObject = (*first)->removeGraphicObject(idx);
        if (graphicObject)
        {
          delete graphicObject;
          return EC_Normal;
        } else return EC_IllegalCall;
      } else idx -= tmp;
    }
    ++first;
  }
  return EC_IllegalCall;
}


OFCondition DVPSGraphicAnnotation_PList::moveGraphicObject(
    const char *old_layer,
    const char *sopclassUID,
    const char *instanceUID,
    unsigned long frame,
    size_t idx,
    DVPSObjectApplicability applicability,
    const char *new_layer)
{
  if (old_layer==NULL) return EC_IllegalCall;
  if (new_layer==NULL) return EC_IllegalCall;

  size_t tmp = 0;
  OFString aString(old_layer);
  OFListIterator(DVPSGraphicAnnotation *) first = list_.begin();
  OFListIterator(DVPSGraphicAnnotation *) last = list_.end();
  while (first != last)
  {
    if ((aString == (*first)->getAnnotationLayer()) && ((*first)->isApplicable(instanceUID, frame)))
    {
      tmp = (*first)->getNumberOfGraphicObjects();
      if (idx < tmp)
      {
        DVPSGraphicObject *graphicObject = (*first)->removeGraphicObject(idx);
        if (graphicObject)
        {
          graphicObject = addGraphicObject(new_layer, sopclassUID, instanceUID, frame, applicability, graphicObject);
          if (graphicObject) return EC_Normal; else return EC_MemoryExhausted;
        } else return EC_IllegalCall;
      } else idx -= tmp;
    }
    ++first;
  }
  return EC_IllegalCall;
}


/*
 *  $Log: dvpsgal.cc,v $
 *  Revision 1.17  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.16  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.15  2005-12-08 15:46:27  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.14  2004/02/04 15:57:49  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.13  2003/09/05 08:37:46  meichel
 *  Fixed minor issue that caused certain error messages during the
 *    parse process on a GSPS object to be "swallowed".
 *
 *  Revision 1.12  2003/06/12 18:23:11  joergr
 *  Modified code to use const_iterators where appropriate (required for STL).
 *
 *  Revision 1.11  2003/06/04 12:30:28  meichel
 *  Added various includes needed by MSVC5 with STL
 *
 *  Revision 1.10  2003/06/04 10:18:07  meichel
 *  Replaced private inheritance from template with aggregation
 *
 *  Revision 1.9  2001/11/28 13:56:54  joergr
 *  Check return value of DcmItem::insert() statements where appropriate to
 *  avoid memory leaks when insert procedure fails.
 *
 *  Revision 1.8  2001/09/26 15:36:25  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.7  2001/06/01 15:50:30  meichel
 *  Updated copyright header
 *
 *  Revision 1.6  2000/06/02 16:01:00  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.5  2000/03/08 16:29:04  meichel
 *  Updated copyright header.
 *
 *  Revision 1.4  1999/07/22 16:39:57  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *  Revision 1.3  1999/04/30 16:40:49  meichel
 *  Minor code purifications to keep Sun CC 2.0.1 quiet
 *
 *  Revision 1.2  1998/12/14 16:10:41  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:40  meichel
 *  Initial Release.
 *
 *
 */
