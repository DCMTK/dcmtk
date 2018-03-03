/*
 *
 *  Copyright (C) 1998-2018, OFFIS e.V.
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
 *    classes: DVPSGraphicLayer
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpsgl.h"
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"

/* --------------- class DVPSGraphicLayer --------------- */

DVPSGraphicLayer::DVPSGraphicLayer()
: graphicLayer(DCM_GraphicLayer)
, graphicLayerOrder(DCM_GraphicLayerOrder)
, graphicLayerRecommendedDisplayGrayscaleValue(DCM_GraphicLayerRecommendedDisplayGrayscaleValue)
, graphicLayerRecommendedDisplayRGBValue(DCM_RETIRED_GraphicLayerRecommendedDisplayRGBValue)
, graphicLayerDescription(DCM_GraphicLayerDescription)
{
}

DVPSGraphicLayer::DVPSGraphicLayer(const DVPSGraphicLayer& copy)
: graphicLayer(copy.graphicLayer)
, graphicLayerOrder(copy.graphicLayerOrder)
, graphicLayerRecommendedDisplayGrayscaleValue(copy.graphicLayerRecommendedDisplayGrayscaleValue)
, graphicLayerRecommendedDisplayRGBValue(copy.graphicLayerRecommendedDisplayRGBValue)
, graphicLayerDescription(copy.graphicLayerDescription)
{
}

DVPSGraphicLayer::~DVPSGraphicLayer()
{
}

OFCondition DVPSGraphicLayer::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;

  READ_FROM_DATASET(DcmCodeString, EVR_CS, graphicLayer)
  READ_FROM_DATASET(DcmIntegerString, EVR_IS, graphicLayerOrder)
  READ_FROM_DATASET(DcmUnsignedShort, EVR_US, graphicLayerRecommendedDisplayGrayscaleValue)
  READ_FROM_DATASET(DcmUnsignedShort, EVR_US, graphicLayerRecommendedDisplayRGBValue)
  READ_FROM_DATASET(DcmLongString, EVR_LO, graphicLayerDescription)
  
  /* Now perform basic sanity checks */

  if (graphicLayer.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a graphic layer SQ item with graphicLayer absent or empty");
  }
  else if (graphicLayer.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a graphic layer SQ item with graphicLayer VM != 1");
  }
 
  if (graphicLayerOrder.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a graphic layer SQ item with graphicLayerOrder absent or empty");
  }
  else if (graphicLayerOrder.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a graphic layer SQ item with graphicLayerOrder VM != 1");
  }

  if (graphicLayerRecommendedDisplayGrayscaleValue.getVM()>1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a graphic layer SQ item with graphicLayerRecommendedDisplayGrayscaleValue VM != 1");
  }

  if ((graphicLayerRecommendedDisplayRGBValue.getVM()>0)&&(graphicLayerRecommendedDisplayRGBValue.getVM() != 3))
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a graphic layer SQ item with graphicLayerRecommendedDisplayRGBValue VM != 3");
  }

  if (graphicLayerDescription.getVM() > 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a graphic layer SQ item with graphicLayerDescription VM > 1");
  }

  return result;
}

OFCondition DVPSGraphicLayer::write(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmElement *delem=NULL;
  
  ADD_TO_DATASET(DcmCodeString, graphicLayer)
  ADD_TO_DATASET(DcmIntegerString, graphicLayerOrder)
  if (graphicLayerRecommendedDisplayGrayscaleValue.getLength() >0) { ADD_TO_DATASET(DcmUnsignedShort, graphicLayerRecommendedDisplayGrayscaleValue) }
  if (graphicLayerRecommendedDisplayRGBValue.getLength() >0) { ADD_TO_DATASET(DcmUnsignedShort, graphicLayerRecommendedDisplayRGBValue) }
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
  sprintf(buf, "%ld", (long)glOrder);
  graphicLayerOrder.putString(buf);
  return;
}

void DVPSGraphicLayer::setGLRecommendedDisplayValueGray(Uint16 gray)
{
  graphicLayerRecommendedDisplayGrayscaleValue.clear();
  graphicLayerRecommendedDisplayGrayscaleValue.putUint16(gray,0);
  return;
}

void DVPSGraphicLayer::setGLRecommendedDisplayValueRGB(Uint16 r, Uint16 g, Uint16 b)
{
  graphicLayerRecommendedDisplayRGBValue.clear();
  graphicLayerRecommendedDisplayRGBValue.putUint16(r,0);
  graphicLayerRecommendedDisplayRGBValue.putUint16(g,1);
  graphicLayerRecommendedDisplayRGBValue.putUint16(b,2);
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
  if ((graphicLayerRecommendedDisplayGrayscaleValue.getVM() == 1) ||
      (graphicLayerRecommendedDisplayRGBValue.getVM() == 3))
  return OFTrue; else return OFFalse;
}

OFCondition DVPSGraphicLayer::getGLRecommendedDisplayValueGray(Uint16& gray)
{
  gray = 0;
  OFCondition result = EC_Normal;
  if (graphicLayerRecommendedDisplayGrayscaleValue.getVM()==1)
  {
    Uint16 gr=0;
    result = graphicLayerRecommendedDisplayGrayscaleValue.getUint16(gr,0);
    if (result==EC_Normal) gray = gr;
  } else if (graphicLayerRecommendedDisplayRGBValue.getVM() == 3)
  {
    Uint16 r=0;
    Uint16 g=0;
    Uint16 b=0;
    result = graphicLayerRecommendedDisplayRGBValue.getUint16(r,0);
    if (EC_Normal==result) result = graphicLayerRecommendedDisplayRGBValue.getUint16(g,1);
    if (EC_Normal==result) result = graphicLayerRecommendedDisplayRGBValue.getUint16(b,2);
    if (result==EC_Normal)
    {
      double dg = 0.299*(double)r + 0.587*(double)g +0.114*(double)b;
      gray = (Uint16) dg;
    }
  } else result=EC_IllegalCall;
  return result;
}

OFCondition DVPSGraphicLayer::getGLRecommendedDisplayValueRGB(Uint16& r, Uint16& g, Uint16& b)
{
  r = 0;
  g = 0;
  b = 0;
  OFCondition result = EC_Normal;
  if (graphicLayerRecommendedDisplayRGBValue.getVM()==3)
  {
    Uint16 rr=0;
    Uint16 gg=0;
    Uint16 bb=0;
    result = graphicLayerRecommendedDisplayRGBValue.getUint16(rr,0);
    if (EC_Normal==result) result = graphicLayerRecommendedDisplayRGBValue.getUint16(gg,1);
    if (EC_Normal==result) result = graphicLayerRecommendedDisplayRGBValue.getUint16(bb,2);
    if (result==EC_Normal)
    {
      r = rr;
      g = gg;
      b = bb;
    }
  }
  else if (graphicLayerRecommendedDisplayGrayscaleValue.getVM() == 1)
  {
    Uint16 gr=0;
    result = graphicLayerRecommendedDisplayGrayscaleValue.getUint16(gr,0);
    if (result==EC_Normal) 
    {
      r = gr;
      g = gr;
      b = gr;
    }
  } else result=EC_IllegalCall;
  return result;
}

void DVPSGraphicLayer::removeRecommendedDisplayValue(OFBool rgb, OFBool monochrome)
{
  if (rgb) graphicLayerRecommendedDisplayRGBValue.clear();
  if (monochrome) graphicLayerRecommendedDisplayGrayscaleValue.clear();
  return;
}
