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
 *    classes: DVPSGraphicLayer
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-12-14 16:10:41 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsgl.h"


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


/* --------------- class DVPSGraphicLayer --------------- */

DVPSGraphicLayer::DVPSGraphicLayer()
: graphicLayer(DCM_GraphicLayer)
, graphicLayerOrder(DCM_GraphicLayerOrder)
, graphicLayerRecommendedDisplayValue(DCM_GraphicLayerRecommendedDisplayValue)
, graphicLayerDescription(DCM_GraphicLayerDescription)
{
}

DVPSGraphicLayer::DVPSGraphicLayer(const DVPSGraphicLayer& copy)
: graphicLayer(copy.graphicLayer)
, graphicLayerOrder(copy.graphicLayerOrder)
, graphicLayerRecommendedDisplayValue(copy.graphicLayerRecommendedDisplayValue)
, graphicLayerDescription(copy.graphicLayerDescription)
{
}

DVPSGraphicLayer::~DVPSGraphicLayer()
{
}

E_Condition DVPSGraphicLayer::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmStack stack;

  READ_FROM_DATASET(DcmCodeString, graphicLayer)
  READ_FROM_DATASET(DcmIntegerString, graphicLayerOrder)
  READ_FROM_DATASET(DcmUnsignedShort, graphicLayerRecommendedDisplayValue)
  READ_FROM_DATASET(DcmLongString, graphicLayerDescription)
  
  /* Now perform basic sanity checks */

  if (graphicLayer.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic layer SQ item with graphicLayer absent or empty" << endl;
#endif
  }
  else if (graphicLayer.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic layer SQ item with graphicLayer VM != 1" << endl;
#endif
  }
 
  if (graphicLayerOrder.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic layer SQ item with graphicLayerOrder absent or empty" << endl;
#endif
  }
  else if (graphicLayerOrder.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic layer SQ item with graphicLayerOrder VM != 1" << endl;
#endif
  }

  if (graphicLayerRecommendedDisplayValue.getVM() >3)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic layer SQ item with graphicLayerRecommendedDisplayValue VM > 3" << endl;
#endif
  }
  else if (graphicLayerRecommendedDisplayValue.getVM() == 2)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic layer SQ item with graphicLayerRecommendedDisplayValue VM == 2" << endl;
#endif
  }

  if (graphicLayerDescription.getVM() > 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic layer SQ item with graphicLayerDescription VM > 1" << endl;
#endif
  }

  return result;
}

E_Condition DVPSGraphicLayer::write(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmElement *delem=NULL;
  
  ADD_TO_DATASET(DcmCodeString, graphicLayer)
  ADD_TO_DATASET(DcmIntegerString, graphicLayerOrder)
  if (graphicLayerRecommendedDisplayValue.getLength() >0) { ADD_TO_DATASET(DcmUnsignedShort, graphicLayerRecommendedDisplayValue) }
  if (graphicLayerDescription.getLength() >0) { ADD_TO_DATASET(DcmLongString, graphicLayerDescription) }

  return result;
}

const char *DVPSGraphicLayer::getGL()
{
  char *c = NULL;
  if (EC_Normal == graphicLayer.getString(c)) return c; else return NULL;
}

const char *DVPSGraphicLayer::getGLDescription()
{
  char *c = NULL;
  if (EC_Normal == graphicLayerDescription.getString(c)) return c; else return NULL;
}


void DVPSGraphicLayer::setGL(const char *gl)
{
  if (gl) graphicLayer.putString(gl); else graphicLayer.clear();
  return;
}

void DVPSGraphicLayer::setGLOrder(Sint32 glOrder)
{
  char buf[100];
  sprintf(buf, "%ld", glOrder);
  graphicLayerOrder.putString(buf);
  return;
}

void DVPSGraphicLayer::setGLRecommendedDisplayValue(Uint16 gray)
{
  graphicLayerRecommendedDisplayValue.clear();
  graphicLayerRecommendedDisplayValue.putUint16(gray,0);
  return;
}

void DVPSGraphicLayer::setGLRecommendedDisplayValue(Uint16 r, Uint16 g, Uint16 b)
{
  graphicLayerRecommendedDisplayValue.clear();
  graphicLayerRecommendedDisplayValue.putUint16(r,0);
  graphicLayerRecommendedDisplayValue.putUint16(g,1);
  graphicLayerRecommendedDisplayValue.putUint16(b,2);
  return;
}

void DVPSGraphicLayer::setGLDescription(const char *glDescription)
{
  if (glDescription) graphicLayerDescription.putString(glDescription); else graphicLayerDescription.clear();
  return;
}


Sint32 DVPSGraphicLayer::getGLOrder()
{
  Sint32 result=0;
  if (EC_Normal == graphicLayerOrder.getSint32(result,0)) return result; else return 0;
}

OFBool DVPSGraphicLayer::haveGLRecommendedDisplayValue()
{
  unsigned long vm = graphicLayerRecommendedDisplayValue.getVM();
  if ((vm==1)||(vm==3)) return OFTrue; else return OFFalse;
}

OFBool DVPSGraphicLayer::isGrayGLRecommendedDisplayValue()
{
  unsigned long vm = graphicLayerRecommendedDisplayValue.getVM();
  if (vm==1) return OFTrue; else return OFFalse;
}


E_Condition DVPSGraphicLayer::getGLRecommendedDisplayValueGray(Uint16& gray)
{
  gray = 0;
  E_Condition result = EC_Normal;
  unsigned long vm = graphicLayerRecommendedDisplayValue.getVM();
  if (vm==1)
  {
    Uint16 gr=0;
    result = graphicLayerRecommendedDisplayValue.getUint16(gr,0);
    if (result==EC_Normal) gray = gr;
  } else if (vm==3)
  {
    Uint16 r=0;
    Uint16 g=0;
    Uint16 b=0;
    result = graphicLayerRecommendedDisplayValue.getUint16(r,0);
    if (EC_Normal==result) result = graphicLayerRecommendedDisplayValue.getUint16(g,1);
    if (EC_Normal==result) result = graphicLayerRecommendedDisplayValue.getUint16(b,2);
    if (result==EC_Normal)
    {
      double dg = 0.299*(double)r + 0.587*(double)g +0.114*(double)b;
      gray = (Uint16) dg;
    }
  } else result=EC_IllegalCall;
  return result;
}

E_Condition DVPSGraphicLayer::getGLRecommendedDisplayValueRGB(Uint16& r, Uint16& g, Uint16& b)
{
  r = 0;
  g = 0;
  b = 0;
  E_Condition result = EC_Normal;
  unsigned long vm = graphicLayerRecommendedDisplayValue.getVM();
  if (vm==1)
  {
    Uint16 gr=0;
    result = graphicLayerRecommendedDisplayValue.getUint16(gr,0);
    if (result==EC_Normal) 
    {
      r = gr;
      g = gr;
      b = gr;
    }
  } else if (vm==3)
  {
    Uint16 rr=0;
    Uint16 gg=0;
    Uint16 bb=0;
    result = graphicLayerRecommendedDisplayValue.getUint16(rr,0);
    if (EC_Normal==result) result = graphicLayerRecommendedDisplayValue.getUint16(gg,1);
    if (EC_Normal==result) result = graphicLayerRecommendedDisplayValue.getUint16(bb,2);
    if (result==EC_Normal)
    {
      r = rr;
      g = gg;
      b = bb;
    }
  } else result=EC_IllegalCall;
  return result;
}

/*
 *  $Log: dvpsgl.cc,v $
 *  Revision 1.2  1998-12-14 16:10:41  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:41  meichel
 *  Initial Release.
 *
 *
 */

