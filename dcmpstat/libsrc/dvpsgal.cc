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
 *    classes: DVPSGraphicAnnotation_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:50:30 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsgal.h"
#include "dvpsga.h"      /* for DVPSGraphicAnnotation */
#include "ofstring.h"    /* for OFString */
#include "dvpstx.h"      /* for DVPSTextObject */
#include "dvpsgr.h"      /* for DVPSGraphicObject */

DVPSGraphicAnnotation_PList::DVPSGraphicAnnotation_PList()
: OFList<DVPSGraphicAnnotation *>()
, logstream(&ofConsole)
, verboseMode(OFFalse)
, debugMode(OFFalse)
{
}

DVPSGraphicAnnotation_PList::DVPSGraphicAnnotation_PList(const DVPSGraphicAnnotation_PList &arg)
: OFList<DVPSGraphicAnnotation *>()
, logstream(arg.logstream)
, verboseMode(arg.verboseMode)
, debugMode(arg.debugMode)
{
  OFListIterator(DVPSGraphicAnnotation *) first = arg.begin();
  OFListIterator(DVPSGraphicAnnotation *) last = arg.end();
  while (first != last)
  {     
    push_back((*first)->clone());
    ++first;
  }
}

DVPSGraphicAnnotation_PList::~DVPSGraphicAnnotation_PList()
{
  clear();
}

void DVPSGraphicAnnotation_PList::clear()
{
  OFListIterator(DVPSGraphicAnnotation *) first = begin();
  OFListIterator(DVPSGraphicAnnotation *) last = end();
  while (first != last)
  {     
    delete (*first);
    first = erase(first);
  }
}

E_Condition DVPSGraphicAnnotation_PList::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
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
          push_back(newObject);
        } else result = EC_MemoryExhausted;
      }
    }
  }    
  
  return result;
}

E_Condition DVPSGraphicAnnotation_PList::write(DcmItem &dset)
{
  if (size()==0) return EC_Normal; // don't write empty Sequence

  E_Condition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  dseq = new DcmSequenceOfItems(DCM_GraphicAnnotationSequence);
  if (dseq)
  {
    OFListIterator(DVPSGraphicAnnotation *) first = begin();
    OFListIterator(DVPSGraphicAnnotation *) last = end();
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


void DVPSGraphicAnnotation_PList::renameLayer(const char *oldName, const char *newName)
{
  if ((oldName==NULL)||(newName==NULL)) return;
  
  OFString aString(oldName);
  OFListIterator(DVPSGraphicAnnotation *) first = begin();
  OFListIterator(DVPSGraphicAnnotation *) last = end();
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
  OFListIterator(DVPSGraphicAnnotation *) first = begin();
  OFListIterator(DVPSGraphicAnnotation *) last = end();
  while (first != last)
  {
    if (aString == (*first)->getAnnotationLayer())
    {
      delete (*first);
      first = erase(first);
    } else ++first;
  }
  return;
}

void DVPSGraphicAnnotation_PList::cleanupLayers()
{
  OFListIterator(DVPSGraphicAnnotation *) first = begin();
  OFListIterator(DVPSGraphicAnnotation *) last = end();
  while (first != last)
  {
    if ((*first)->isEmpty())
    {
      delete (*first);
      first = erase(first);
    } else ++first;
  }
  return;
}


OFBool DVPSGraphicAnnotation_PList::usesLayerName(const char *name)
{
  if (name==NULL) return OFFalse;
  
  OFString aString(name);
  OFListIterator(DVPSGraphicAnnotation *) first = begin();
  OFListIterator(DVPSGraphicAnnotation *) last = end();
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
  OFListIterator(DVPSGraphicAnnotation *) first = begin();
  OFListIterator(DVPSGraphicAnnotation *) last = end();
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
  OFListIterator(DVPSGraphicAnnotation *) first = begin();
  OFListIterator(DVPSGraphicAnnotation *) last = end();
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
    if (text) text->setLog(logstream, verboseMode, debugMode);
  }
  if (text==NULL) return NULL;
  
  DVPSGraphicAnnotation *annotation = NULL;
  OFString aString(layer);
  OFListIterator(DVPSGraphicAnnotation *) first = begin();
  OFListIterator(DVPSGraphicAnnotation *) last = end();
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
      push_back(annotation);
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


E_Condition DVPSGraphicAnnotation_PList::removeTextObject(const char *layer, const char *instanceUID, unsigned long frame, size_t idx)
{
  if (layer==NULL) return EC_IllegalCall;

  size_t tmp = 0;  
  OFString aString(layer);
  OFListIterator(DVPSGraphicAnnotation *) first = begin();
  OFListIterator(DVPSGraphicAnnotation *) last = end();
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


E_Condition DVPSGraphicAnnotation_PList::moveTextObject(
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
  OFListIterator(DVPSGraphicAnnotation *) first = begin();
  OFListIterator(DVPSGraphicAnnotation *) last = end();
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
  OFListIterator(DVPSGraphicAnnotation *) first = begin();
  OFListIterator(DVPSGraphicAnnotation *) last = end();
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
  OFListIterator(DVPSGraphicAnnotation *) first = begin();
  OFListIterator(DVPSGraphicAnnotation *) last = end();
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
  OFListIterator(DVPSGraphicAnnotation *) first = begin();
  OFListIterator(DVPSGraphicAnnotation *) last = end();
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
      push_back(annotation);
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


E_Condition DVPSGraphicAnnotation_PList::removeGraphicObject(const char *layer, const char *instanceUID, unsigned long frame, size_t idx)
{
  if (layer==NULL) return EC_IllegalCall;

  size_t tmp = 0;  
  OFString aString(layer);
  OFListIterator(DVPSGraphicAnnotation *) first = begin();
  OFListIterator(DVPSGraphicAnnotation *) last = end();
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


E_Condition DVPSGraphicAnnotation_PList::moveGraphicObject(
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
  OFListIterator(DVPSGraphicAnnotation *) first = begin();
  OFListIterator(DVPSGraphicAnnotation *) last = end();
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

void DVPSGraphicAnnotation_PList::setLog(OFConsole *stream, OFBool verbMode, OFBool dbgMode)
{
  if (stream) logstream = stream; else logstream = &ofConsole;
  verboseMode = verbMode;
  debugMode = dbgMode;
  OFListIterator(DVPSGraphicAnnotation *) first = begin();
  OFListIterator(DVPSGraphicAnnotation *) last = end();
  while (first != last)
  {
    (*first)->setLog(logstream, verbMode, dbgMode);
    ++first;
  }	
}


/*
 *  $Log: dvpsgal.cc,v $
 *  Revision 1.7  2001-06-01 15:50:30  meichel
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

