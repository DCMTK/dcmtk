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
 *    classes: DVPresentationState
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-12-14 16:10:40 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsga.h"

/* --------------- a few macros avoiding copy/paste --------------- */

#define ADD_TO_DATASET(a_type, a_name)                              \
if (result==EC_Normal)                                              \
{                                                                   \
  delem = new a_type(a_name);                                       \
  if (delem) dset.insert(delem); else result=EC_MemoryExhausted;    \
}

#define READ_FROM_DATASET(a_type, a_name)                           \
stack.clear();                                                      \
if (EC_Normal == dset.search((DcmTagKey &)a_name.getTag(), stack, ESM_fromHere, OFFalse)) \
{                                                                   \
  a_name = *((a_type *)(stack.top()));                              \
}


/* --------------- class DVPSGraphicAnnotation --------------- */

DVPSGraphicAnnotation::DVPSGraphicAnnotation()
: referencedFrameNumber(DCM_ReferencedFrameNumber)
, graphicAnnotationLayer(DCM_GraphicLayer)
, textObjectList()
, graphicObjectList()
{
}

DVPSGraphicAnnotation::DVPSGraphicAnnotation(const DVPSGraphicAnnotation& copy)
: referencedFrameNumber(copy.referencedFrameNumber)
, graphicAnnotationLayer(copy.graphicAnnotationLayer)
, textObjectList(copy.textObjectList)
, graphicObjectList(copy.graphicObjectList)
{
}

DVPSGraphicAnnotation::~DVPSGraphicAnnotation()
{
}

E_Condition DVPSGraphicAnnotation::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmStack stack;

  READ_FROM_DATASET(DcmIntegerString, referencedFrameNumber)
  READ_FROM_DATASET(DcmCodeString, graphicAnnotationLayer)
  if (result==EC_Normal) result = textObjectList.read(dset);
  if (result==EC_Normal) result = graphicObjectList.read(dset);
  
  /* Now perform basic sanity checks */

  if (graphicAnnotationLayer.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic annotation SQ item with graphicAnnotationLayer absent or empty" << endl;
#endif
  }
  else if (graphicAnnotationLayer.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic annotation SQ item with graphicAnnotationLayer VM != 1" << endl;
#endif
  }

  if ((textObjectList.size() ==0)&&(graphicObjectList.size() ==0))
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic annotation SQ item without any graphic or text objects" << endl;
#endif
  }
  
  return result;
}

E_Condition DVPSGraphicAnnotation::write(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmElement *delem=NULL;
  
  ADD_TO_DATASET(DcmCodeString, graphicAnnotationLayer)
  if (referencedFrameNumber.getLength() > 0)
  {
    ADD_TO_DATASET(DcmIntegerString, referencedFrameNumber)    
  }
  
  if (result == EC_Normal) result = textObjectList.write(dset);
  if (result == EC_Normal) result = graphicObjectList.write(dset);
  return result;
}

void DVPSGraphicAnnotation::setAnnotationLayer(const char *aLayer)
{
  if (aLayer) graphicAnnotationLayer.putString(aLayer); else graphicAnnotationLayer.clear();
  return;
}

const char *DVPSGraphicAnnotation::getAnnotationLayer()
{
  char *c = NULL;
  if (EC_Normal == graphicAnnotationLayer.getString(c)) return c; else return NULL;
}

OFBool DVPSGraphicAnnotation::isEmpty()
{
  if ((textObjectList.size() == 0)&&(graphicObjectList.size()==0)) return OFTrue;
  else return OFFalse;
}

size_t DVPSGraphicAnnotation::getNumberOfTextObjects()
{
  return textObjectList.size();
}

size_t DVPSGraphicAnnotation::getNumberOfGraphicObjects()
{
  return graphicObjectList.size();
}

DVPSTextObject *DVPSGraphicAnnotation::getTextObject(size_t idx)
{
  return textObjectList.getTextObject(idx);
}

DVPSGraphicObject *DVPSGraphicAnnotation::getGraphicObject(size_t idx)
{
  return graphicObjectList.getGraphicObject(idx);
}

void DVPSGraphicAnnotation::addTextObject(DVPSTextObject *text)
{
  textObjectList.addTextObject(text);
}

void DVPSGraphicAnnotation::addGraphicObject(DVPSGraphicObject *graphic)
{
  graphicObjectList.addGraphicObject(graphic);
}

DVPSTextObject *DVPSGraphicAnnotation::removeTextObject(size_t idx)
{
  return textObjectList.removeTextObject(idx);
}

DVPSGraphicObject *DVPSGraphicAnnotation::removeGraphicObject(size_t idx)
{
  return graphicObjectList.removeGraphicObject(idx);
}

/*
 *  $Log: dvpsga.cc,v $
 *  Revision 1.2  1998-12-14 16:10:40  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:40  meichel
 *  Initial Release.
 *
 *
 */

