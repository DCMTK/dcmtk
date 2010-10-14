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
 *    classes: DVPSGraphicLayer_PList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:32 $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmpstat/dvpsgll.h"
#include "dcmtk/dcmpstat/dvpsgl.h"      /* for DVPSGraphicLayer */
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for DVPS_IDX_NONE */
#include "dcmtk/dcmpstat/dvpsgal.h"     /* for DVPSOverlayCurveActivationLayer_PList */
#include "dcmtk/dcmpstat/dvpsall.h"     /* for DVPSGraphicAnnotation_PList& annotations */

#include "dcmtk/dcmpstat/dvpsal.h"      /* for DVPSOverlayCurveActivationLayer, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsga.h"      /* for DVPSGraphicAnnotation, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpstx.h"      /* for DVPSTextObject, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsgr.h"      /* for DVPSGraphicObject, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsri.h"      /* for DVPSReferencedImage, needed by MSVC5 with STL */

DVPSGraphicLayer_PList::DVPSGraphicLayer_PList()
: list_()
{
}

DVPSGraphicLayer_PList::DVPSGraphicLayer_PList(const DVPSGraphicLayer_PList &arg)
: list_()
{
  OFListConstIterator(DVPSGraphicLayer *) first = arg.list_.begin();
  OFListConstIterator(DVPSGraphicLayer *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSGraphicLayer_PList::~DVPSGraphicLayer_PList()
{
  clear();
}

void DVPSGraphicLayer_PList::clear()
{
  OFListIterator(DVPSGraphicLayer *) first = list_.begin();
  OFListIterator(DVPSGraphicLayer *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSGraphicLayer_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
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
          list_.push_back(newLayer);
        } else result = EC_MemoryExhausted;
      }
    }
  }

  return result;
}

OFCondition DVPSGraphicLayer_PList::write(DcmItem &dset)
{
  if (size()==0) return EC_Normal; // don't write empty Sequence

  OFCondition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  dseq = new DcmSequenceOfItems(DCM_GraphicLayerSequence);
  if (dseq)
  {
    OFListIterator(DVPSGraphicLayer *) first = list_.begin();
    OFListIterator(DVPSGraphicLayer *) last = list_.end();
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

OFCondition DVPSGraphicLayer_PList::addGraphicLayer(const char *gLayer,
    const Sint32 gLayerOrder,
    const char *gLayerDescription)
{
  if (gLayer==NULL) return EC_IllegalCall;
  OFString ggLayer(gLayer);

  /* check that no graphic layer with the same name exist */
  OFListIterator(DVPSGraphicLayer *) first = list_.begin();
  OFListIterator(DVPSGraphicLayer *) last = list_.end();
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
    list_.push_back(newLayer);
  } else return EC_MemoryExhausted;
  return EC_Normal;
}


DVPSGraphicLayer *DVPSGraphicLayer_PList::getGraphicLayer(size_t idx)
{
  OFListIterator(DVPSGraphicLayer *) first = list_.begin();
  OFListIterator(DVPSGraphicLayer *) last = list_.end();
  while (first != last)
  {
    if ((idx--) == 0) return *first;
    ++first;
  }
  return NULL;
}


OFCondition DVPSGraphicLayer_PList::addGraphicLayer(
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
  OFList<DVPSGraphicLayer *> tempList;
  tempList.splice(tempList.end(),list_); // move all entries to tempList
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
    list_.push_back(*current);
    tempList.erase(current);
  }

  /* now renumber layer orders */
  first = list_.begin();
  last = list_.end();
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

  OFListIterator(DVPSGraphicLayer *) first = list_.begin();
  OFListIterator(DVPSGraphicLayer *) last = list_.end();
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

OFCondition DVPSGraphicLayer_PList::getGraphicLayerRecommendedDisplayValueGray(size_t idx, Uint16& gray)
{
  DVPSGraphicLayer *layer = getGraphicLayer(idx);
  if (layer) return layer->getGLRecommendedDisplayValueGray(gray); else return EC_IllegalCall;
}

OFCondition DVPSGraphicLayer_PList::getGraphicLayerRecommendedDisplayValueRGB(size_t idx, Uint16& r, Uint16& g, Uint16& b)
{
  DVPSGraphicLayer *layer = getGraphicLayer(idx);
  if (layer) return layer->getGLRecommendedDisplayValueRGB(r, g, b); else return EC_IllegalCall;
}

OFCondition DVPSGraphicLayer_PList::setGraphicLayerRecommendedDisplayValueGray(size_t idx, Uint16 gray)
{
  DVPSGraphicLayer *layer = getGraphicLayer(idx);
  if (layer)
  {
    layer->setGLRecommendedDisplayValueGray(gray);
    return EC_Normal;
  } else return EC_IllegalCall;
}

OFCondition DVPSGraphicLayer_PList::setGraphicLayerRecommendedDisplayValueRGB(size_t idx, Uint16 r, Uint16 g, Uint16 b)
{
  DVPSGraphicLayer *layer = getGraphicLayer(idx);
  if (layer)
  {
    layer->setGLRecommendedDisplayValueRGB(r, g, b);
    return EC_Normal;
  } else return EC_IllegalCall;
}

void DVPSGraphicLayer_PList::removeGraphicLayerRecommendedDisplayValue(size_t idx, OFBool rgb, OFBool monochrome)
{
  DVPSGraphicLayer *layer = getGraphicLayer(idx);
  if (layer) layer->removeRecommendedDisplayValue(rgb, monochrome);
  return;
}

OFCondition DVPSGraphicLayer_PList::setGraphicLayerName(size_t idx, const char *name)
{
  if (name==NULL) return EC_IllegalCall;

  OFString aString(name);
  /* check that no graphic layer with the same name exist */
  OFListIterator(DVPSGraphicLayer *) first = list_.begin();
  OFListIterator(DVPSGraphicLayer *) last = list_.end();
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
  }
  return EC_IllegalCall;
}


OFCondition DVPSGraphicLayer_PList::setGraphicLayerDescription(size_t idx, const char *descr)
{
  DVPSGraphicLayer *layer = getGraphicLayer(idx);
  if (layer)
  {
    layer->setGLDescription(descr);
    return EC_Normal;
  }
  return EC_IllegalCall;
}


OFCondition DVPSGraphicLayer_PList::toFrontGraphicLayer(size_t idx)
{
  OFListIterator(DVPSGraphicLayer *) first = list_.begin();
  OFListIterator(DVPSGraphicLayer *) last = list_.end();
  OFBool found = OFFalse;
  while ((!found) && (first != last))
  {
    if ((idx--) == 0) found=OFTrue; else ++first;
  }
  if (found)
  {
    DVPSGraphicLayer *layer = *first;
    list_.erase(first);
    sortGraphicLayers(1);
    Sint32 layerOrder = size()+1;
    layer->setGLOrder(layerOrder);
    list_.push_back(layer);
  } else return EC_IllegalCall;
  return EC_Normal;
}

OFCondition DVPSGraphicLayer_PList::toBackGraphicLayer(size_t idx)
{
  OFListIterator(DVPSGraphicLayer *) first = list_.begin();
  OFListIterator(DVPSGraphicLayer *) last = list_.end();
  OFBool found = OFFalse;
  while ((!found) && (first != last))
  {
    if ((idx--) == 0) found=OFTrue; else ++first;
  }
  if (found)
  {
    DVPSGraphicLayer *layer = *first;
    list_.erase(first);
    sortGraphicLayers(2);
    layer->setGLOrder(1);
    list_.push_front(layer);
  } else return EC_IllegalCall;
  return EC_Normal;
}

OFCondition DVPSGraphicLayer_PList::exchangeGraphicLayers(size_t idx1, size_t idx2)
{
  if (idx1 == idx2) return EC_Normal;
  DVPSGraphicLayer *layer1 = getGraphicLayer(idx1);
  DVPSGraphicLayer *layer2 = getGraphicLayer(idx2);
  if ((layer1==NULL)||(layer2==NULL)) return EC_IllegalCall;

  Sint32 order1 = layer1->getGLOrder();
  Sint32 order2 = layer2->getGLOrder();
  if (order1 != order2)
  {
    layer1->setGLOrder(order2);
    layer2->setGLOrder(order1);
  }
  return EC_Normal;
}

OFCondition DVPSGraphicLayer_PList::removeGraphicLayer(size_t idx)
{
  OFListIterator(DVPSGraphicLayer *) first = list_.begin();
  OFListIterator(DVPSGraphicLayer *) last = list_.end();
  OFBool found = OFFalse;
  while ((!found) && (first != last))
  {
    if ((idx--) == 0) found=OFTrue; else ++first;
  }
  if (found)
  {
    delete *first;
    list_.erase(first);
  } else return EC_IllegalCall;
  return EC_Normal;
}


void DVPSGraphicLayer_PList::cleanupLayers(
     DVPSOverlayCurveActivationLayer_PList& activations,
     DVPSGraphicAnnotation_PList& annotations)
{
  OFListIterator(DVPSGraphicLayer *) first = list_.begin();
  OFListIterator(DVPSGraphicLayer *) last = list_.end();
  const char *name=NULL;
  while (first != last)
  {
    name = (*first)->getGL();
    if ((! activations.usesLayerName(name))&&(! annotations.usesLayerName(name)))
    {
      delete (*first);
      first = list_.erase(first);
    } else ++first;
  }
  return;
}

/*
 *  $Log: dvpsgll.cc,v $
 *  Revision 1.19  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.18  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.17  2005-12-08 15:46:29  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.16  2004/02/04 15:57:49  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.15  2003/09/05 08:37:46  meichel
 *  Fixed minor issue that caused certain error messages during the
 *    parse process on a GSPS object to be "swallowed".
 *
 *  Revision 1.14  2003/06/12 18:23:11  joergr
 *  Modified code to use const_iterators where appropriate (required for STL).
 *
 *  Revision 1.13  2003/06/04 12:30:28  meichel
 *  Added various includes needed by MSVC5 with STL
 *
 *  Revision 1.12  2003/06/04 10:18:07  meichel
 *  Replaced private inheritance from template with aggregation
 *
 *  Revision 1.11  2001/11/28 13:56:54  joergr
 *  Check return value of DcmItem::insert() statements where appropriate to
 *  avoid memory leaks when insert procedure fails.
 *
 *  Revision 1.10  2001/09/26 15:36:26  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.9  2001/06/01 15:50:31  meichel
 *  Updated copyright header
 *
 *  Revision 1.8  2000/06/02 16:01:01  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.7  2000/05/31 13:02:37  meichel
 *  Moved dcmpstat macros and constants into a common header file
 *
 *  Revision 1.6  2000/03/08 16:29:05  meichel
 *  Updated copyright header.
 *
 *  Revision 1.5  1999/07/22 16:39:59  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *  Revision 1.4  1999/04/30 16:40:50  meichel
 *  Minor code purifications to keep Sun CC 2.0.1 quiet
 *
 *  Revision 1.3  1999/02/09 15:59:07  meichel
 *  Implemented bitmap shutter activation amd method for
 *    exchanging graphic layers.
 *
 *  Revision 1.2  1998/12/14 16:10:42  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:42  meichel
 *  Initial Release.
 *
 *
 */
