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
 *    classes: DVPSGraphicLayer_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-12-14 16:10:42 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dvpsgll.h"
#include "dvpsgl.h"      /* for DVPSGraphicLayer */
#include "dvpstyp.h"     /* for DVPS_IDX_NONE */
#include "dvpsgal.h"     /* for DVPSOverlayCurveActivationLayer_PList */
#include "dvpsall.h"     /* for DVPSGraphicAnnotation_PList& annotations */

DVPSGraphicLayer_PList::DVPSGraphicLayer_PList()
: OFList<DVPSGraphicLayer *>()
{
}

DVPSGraphicLayer_PList::DVPSGraphicLayer_PList(const DVPSGraphicLayer_PList &arg)
: OFList<DVPSGraphicLayer *>()
{
  OFListIterator(DVPSGraphicLayer *) first = arg.begin();
  OFListIterator(DVPSGraphicLayer *) last = arg.end();
  while (first != last)
  {     
    push_back((*first)->clone());
    ++first;
  }
}

DVPSGraphicLayer_PList::~DVPSGraphicLayer_PList()
{
  clear();
}

void DVPSGraphicLayer_PList::clear()
{
  OFListIterator(DVPSGraphicLayer *) first = begin();
  OFListIterator(DVPSGraphicLayer *) last = end();
  while (first != last)
  {     
    delete (*first);
    first = erase(first);
  }
}

E_Condition DVPSGraphicLayer_PList::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmStack stack;
  DVPSGraphicLayer *newLayer = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;
  
  if (EC_Normal == dset.search(DCM_GraphicLayerSequence, stack, ESM_fromHere, OFFalse))
  {
    dseq=(DcmSequenceOfItems *)stack.top();
    if (dseq)
    {
      unsigned long numItems = dseq->card();
      for (unsigned int i=0; i<numItems; i++)
      {
        ditem = dseq->getItem(i);
        newLayer = new DVPSGraphicLayer();
        if (newLayer && ditem)
        {
          result = newLayer->read(*ditem);
          push_back(newLayer);
        } else result = EC_MemoryExhausted;
      }
    }
  }    
  
  return result;
}

E_Condition DVPSGraphicLayer_PList::write(DcmItem &dset)
{
  if (size()==0) return EC_Normal; // don't write empty Sequence

  E_Condition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  dseq = new DcmSequenceOfItems(DCM_GraphicLayerSequence);
  if (dseq)
  {
    OFListIterator(DVPSGraphicLayer *) first = begin();
    OFListIterator(DVPSGraphicLayer *) last = end();
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

E_Condition DVPSGraphicLayer_PList::addGraphicLayer(const char *gLayer, 
    const Sint32 gLayerOrder,
    const char *gLayerDescription)
{
  if (gLayer==NULL) return EC_IllegalCall;
  OFString ggLayer(gLayer);
  
  /* check that no graphic layer with the same name exist */  
  OFListIterator(DVPSGraphicLayer *) first = begin();
  OFListIterator(DVPSGraphicLayer *) last = end();
  while (first != last)
  {
    if (ggLayer == (*first)->getGL()) return EC_IllegalCall;
    ++first;
  }
  
  DVPSGraphicLayer *newLayer = new DVPSGraphicLayer;
  if (newLayer)
  {
    newLayer->setGL(gLayer);
    newLayer->setGLOrder(gLayerOrder);
    if (gLayerDescription) newLayer->setGLDescription(gLayerDescription);
    push_back(newLayer);
  } else return EC_MemoryExhausted;
  return EC_Normal;
}


DVPSGraphicLayer *DVPSGraphicLayer_PList::getGraphicLayer(size_t idx)
{
  OFListIterator(DVPSGraphicLayer *) first = begin();
  OFListIterator(DVPSGraphicLayer *) last = end();
  while (first != last)
  {
    if ((idx--) == 0) return *first;
    ++first;
  }
  return NULL;
}


E_Condition DVPSGraphicLayer_PList::addGraphicLayer(
     const char *gLayer, 
     const char *gLayerDescription)
{
  sortGraphicLayers(1);
  Sint32 layerOrder = size()+1;
  return addGraphicLayer(gLayer, layerOrder, gLayerDescription);
}


void DVPSGraphicLayer_PList::sortGraphicLayers(Sint32 lowestLayer)
{
  /* this sorting routine is a simple straight selection.
   * since we won't have many graphic layers, this should be sufficient.
   */
  DVPSGraphicLayer_PList tempList;
  tempList.splice(tempList.end(),*this); // move all entries to tempList
  OFListIterator(DVPSGraphicLayer *) current;
  OFListIterator(DVPSGraphicLayer *) first;
  OFListIterator(DVPSGraphicLayer *) last;
  Sint32 currentSize;
  while (tempList.size() > 0)
  {
    current = tempList.begin();
    currentSize = (*current)->getGLOrder();
    first = tempList.begin();
    last  = tempList.end();
    while (first != last)
    {
      if ((*first)->getGLOrder() < currentSize)
      {
      	current = first;
      	currentSize = (*first)->getGLOrder();
      }
      ++first;
    }
    push_back(*current);
    tempList.erase(current);    
  }
  
  /* now renumber layer orders */
  first = begin();
  last = end();
  while (first != last) (*first++)->setGLOrder(lowestLayer++);
  return;
}


const char *DVPSGraphicLayer_PList::getGraphicLayerName(size_t idx)
{
  DVPSGraphicLayer *layer = getGraphicLayer(idx);
  if (layer) return layer->getGL(); else return NULL;
}


size_t DVPSGraphicLayer_PList::getGraphicLayerIndex(const char *name)
{
  if (name==NULL) return DVPS_IDX_NONE;
  size_t idx =0;
  OFString aString(name);
  
  OFListIterator(DVPSGraphicLayer *) first = begin();
  OFListIterator(DVPSGraphicLayer *) last = end();
  while (first != last)
  {
    if (aString == (*first)->getGL()) return idx;
    idx++;
    ++first;
  }
  return DVPS_IDX_NONE;
} 

const char *DVPSGraphicLayer_PList::getGraphicLayerDescription(size_t idx)
{
  DVPSGraphicLayer *layer = getGraphicLayer(idx);
  if (layer) return layer->getGLDescription(); else return NULL;
}

OFBool DVPSGraphicLayer_PList::haveGraphicLayerRecommendedDisplayValue(size_t idx)
{
  DVPSGraphicLayer *layer = getGraphicLayer(idx);
  if (layer) return layer->haveGLRecommendedDisplayValue(); else return OFFalse;
}

OFBool DVPSGraphicLayer_PList::isGrayGraphicLayerRecommendedDisplayValue(size_t idx)
{
  DVPSGraphicLayer *layer = getGraphicLayer(idx);
  if (layer) return layer->isGrayGLRecommendedDisplayValue(); else return OFFalse;
}

E_Condition DVPSGraphicLayer_PList::getGraphicLayerRecommendedDisplayValueGray(size_t idx, Uint16& gray)
{
  DVPSGraphicLayer *layer = getGraphicLayer(idx);
  if (layer) return layer->getGLRecommendedDisplayValueGray(gray); else return EC_IllegalCall;
}

E_Condition DVPSGraphicLayer_PList::getGraphicLayerRecommendedDisplayValueRGB(size_t idx, Uint16& r, Uint16& g, Uint16& b)
{
  DVPSGraphicLayer *layer = getGraphicLayer(idx);
  if (layer) return layer->getGLRecommendedDisplayValueRGB(r, g, b); else return EC_IllegalCall;
}

E_Condition DVPSGraphicLayer_PList::setGraphicLayerRecommendedDisplayValueGray(size_t idx, Uint16 gray)
{
  DVPSGraphicLayer *layer = getGraphicLayer(idx);
  if (layer) 
  {
    layer->setGLRecommendedDisplayValue(gray);
    return EC_Normal;
  } else return EC_IllegalCall;
}

E_Condition DVPSGraphicLayer_PList::setGraphicLayerRecommendedDisplayValueRGB(size_t idx, Uint16 r, Uint16 g, Uint16 b)
{
  DVPSGraphicLayer *layer = getGraphicLayer(idx);
  if (layer) 
  {
    layer->setGLRecommendedDisplayValue(r, g, b);
    return EC_Normal;
  } else return EC_IllegalCall;
}


E_Condition DVPSGraphicLayer_PList::setGraphicLayerName(size_t idx, const char *name)
{
  if (name==NULL) return EC_IllegalCall;
  
  OFString aString(name);
  /* check that no graphic layer with the same name exist */  
  OFListIterator(DVPSGraphicLayer *) first = begin();
  OFListIterator(DVPSGraphicLayer *) last = end();
  while (first != last)
  {
    if (aString == (*first)->getGL()) return EC_IllegalCall;
    ++first;
  }
  
  DVPSGraphicLayer *layer = getGraphicLayer(idx);
  if (layer) 
  {
    layer->setGL(name);
    return EC_Normal;
  } else return EC_IllegalCall;
}  


E_Condition DVPSGraphicLayer_PList::setGraphicLayerDescription(size_t idx, const char *descr)
{
  DVPSGraphicLayer *layer = getGraphicLayer(idx);
  if (layer) 
  {
    layer->setGLDescription(descr);
    return EC_Normal;
  } else return EC_IllegalCall;
}


E_Condition DVPSGraphicLayer_PList::toFrontGraphicLayer(size_t idx)
{
  OFListIterator(DVPSGraphicLayer *) first = begin();
  OFListIterator(DVPSGraphicLayer *) last = end();
  OFBool found = OFFalse;
  while ((!found) && (first != last))
  {
    if ((idx--) == 0) found=OFTrue; else ++first;
  }
  if (found)
  {
    DVPSGraphicLayer *layer = *first;
    erase(first);
    sortGraphicLayers(1);
    Sint32 layerOrder = size()+1;
    layer->setGLOrder(layerOrder);
    push_back(layer);
  } else return EC_IllegalCall;
  return EC_Normal;
}

E_Condition DVPSGraphicLayer_PList::toBackGraphicLayer(size_t idx)
{
  OFListIterator(DVPSGraphicLayer *) first = begin();
  OFListIterator(DVPSGraphicLayer *) last = end();
  OFBool found = OFFalse;
  while ((!found) && (first != last))
  {
    if ((idx--) == 0) found=OFTrue; else ++first;
  }
  if (found)
  {
    DVPSGraphicLayer *layer = *first;
    erase(first);
    sortGraphicLayers(2);
    layer->setGLOrder(1);
    push_front(layer);
  } else return EC_IllegalCall;
  return EC_Normal;
}
 
E_Condition DVPSGraphicLayer_PList::removeGraphicLayer(size_t idx)
{
  OFListIterator(DVPSGraphicLayer *) first = begin();
  OFListIterator(DVPSGraphicLayer *) last = end();
  OFBool found = OFFalse;
  while ((!found) && (first != last))
  {
    if ((idx--) == 0) found=OFTrue; else ++first;
  }
  if (found)
  {
    delete *first;
    erase(first);
  } else return EC_IllegalCall;
  return EC_Normal;
}


void DVPSGraphicLayer_PList::cleanupLayers(
     DVPSOverlayCurveActivationLayer_PList& activations, 
     DVPSGraphicAnnotation_PList& annotations)
{
  OFListIterator(DVPSGraphicLayer *) first = begin();
  OFListIterator(DVPSGraphicLayer *) last = end();
  const char *name=NULL;
  while (first != last)
  {
    name = (*first)->getGL();
    if ((! activations.usesLayerName(name))&&(! annotations.usesLayerName(name)))
    {
      delete (*first);
      first = erase(first);
    } else ++first;
  }
  return;
}


/*
 *  $Log: dvpsgll.cc,v $
 *  Revision 1.2  1998-12-14 16:10:42  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:42  meichel
 *  Initial Release.
 *
 *
 */

