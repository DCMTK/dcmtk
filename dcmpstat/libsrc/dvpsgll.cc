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
 *  Update Date:      $Date: 1998-11-27 14:50:42 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dvpsgll.h"
#include "dvpsgl.h"      /* for DVPSGraphicLayer */


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


/*
 *  $Log: dvpsgll.cc,v $
 *  Revision 1.1  1998-11-27 14:50:42  meichel
 *  Initial Release.
 *
 *
 */

