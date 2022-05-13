/*
 *
 *  Copyright (C) 1998-2022, OFFIS e.V.
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
#include "dcmtk/dcmiod/cielabutil.h"

/* static helper functions */

static void DICOM_RGB_to_Lab(Uint16& L, Uint16& a, Uint16& b, Uint16 R, Uint16 G, Uint16 B)
{
  double dL, da, db;
  double dR = OFstatic_cast(double, R) / 65535.0;
  double dG = OFstatic_cast(double, G) / 65535.0;
  double dB = OFstatic_cast(double, B) / 65535.0;

  IODCIELabUtil::rgb2DicomLab(dL, da, db, dR, dG, dB);

  L = OFstatic_cast(Uint16, dL);
  a = OFstatic_cast(Uint16, da);
  b = OFstatic_cast(Uint16, db);
}

static void DICOM_Lab_to_RGB(Uint16& R, Uint16& G, Uint16& B, Uint16 L, Uint16 a, Uint16 b)
{
  double dR, dG, dB;
  IODCIELabUtil::dicomLab2RGB(dR, dG, dB, L, a, b);
  R = OFstatic_cast(Uint16, dR * 65535.0);
  G = OFstatic_cast(Uint16, dG * 65535.0);
  B = OFstatic_cast(Uint16, dB * 65535.0);
}

/* --------------- class DVPSGraphicLayer --------------- */

DVPSGraphicLayer::DVPSGraphicLayer()
: graphicLayer(DCM_GraphicLayer)
, graphicLayerOrder(DCM_GraphicLayerOrder)
, graphicLayerRecommendedDisplayGrayscaleValue(DCM_GraphicLayerRecommendedDisplayGrayscaleValue)
, graphicLayerRecommendedDisplayCIELabValue(DCM_GraphicLayerRecommendedDisplayCIELabValue)
, graphicLayerDescription(DCM_GraphicLayerDescription)
{
}

DVPSGraphicLayer::DVPSGraphicLayer(const DVPSGraphicLayer& copy)
: graphicLayer(copy.graphicLayer)
, graphicLayerOrder(copy.graphicLayerOrder)
, graphicLayerRecommendedDisplayGrayscaleValue(copy.graphicLayerRecommendedDisplayGrayscaleValue)
, graphicLayerRecommendedDisplayCIELabValue(copy.graphicLayerRecommendedDisplayCIELabValue)
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
  DcmUnsignedShort graphicLayerRecommendedDisplayRGBValue(DCM_RETIRED_GraphicLayerRecommendedDisplayRGBValue);

  READ_FROM_DATASET(DcmCodeString, EVR_CS, graphicLayer)
  READ_FROM_DATASET(DcmIntegerString, EVR_IS, graphicLayerOrder)
  READ_FROM_DATASET(DcmUnsignedShort, EVR_US, graphicLayerRecommendedDisplayGrayscaleValue)
  READ_FROM_DATASET(DcmUnsignedShort, EVR_US, graphicLayerRecommendedDisplayRGBValue)
  READ_FROM_DATASET(DcmUnsignedShort, EVR_US, graphicLayerRecommendedDisplayCIELabValue)
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

  if ((graphicLayerRecommendedDisplayCIELabValue.getVM()>0)&&(graphicLayerRecommendedDisplayCIELabValue.getVM() != 3))
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a graphic layer SQ item with graphicLayerRecommendedDisplayCIELabValue VM != 3");
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

  if ((result.good() && (graphicLayerRecommendedDisplayCIELabValue.getVM() == 0) && (graphicLayerRecommendedDisplayRGBValue.getVM() == 3)))
  {
    // convert RGB to L*a*b*
    Uint16 L, a, b, rr=0, gg=0, bb=0;
    graphicLayerRecommendedDisplayCIELabValue.clear();
    result = graphicLayerRecommendedDisplayRGBValue.getUint16(rr,0);
    if (result.good()) result = graphicLayerRecommendedDisplayRGBValue.getUint16(gg,1);
    if (result.good()) result = graphicLayerRecommendedDisplayRGBValue.getUint16(bb,2);
    if (result.good())
    {
      DICOM_RGB_to_Lab(L, a, b, rr, gg, bb);
      graphicLayerRecommendedDisplayCIELabValue.putUint16(L,0);
      graphicLayerRecommendedDisplayCIELabValue.putUint16(a,1);
      graphicLayerRecommendedDisplayCIELabValue.putUint16(b,2);
    }
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
  if (graphicLayerRecommendedDisplayCIELabValue.getLength() >0) { ADD_TO_DATASET(DcmUnsignedShort, graphicLayerRecommendedDisplayCIELabValue) }
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
  // convert RGB to L*a*b*
  graphicLayerRecommendedDisplayCIELabValue.clear();
  Uint16 LL, aa, bb;
  DICOM_RGB_to_Lab(LL, aa, bb, r, g, b);
  graphicLayerRecommendedDisplayCIELabValue.putUint16(LL,0);
  graphicLayerRecommendedDisplayCIELabValue.putUint16(aa,1);
  graphicLayerRecommendedDisplayCIELabValue.putUint16(bb,2);
  return;
}

void DVPSGraphicLayer::setGLRecommendedDisplayValueCIELab(Uint16 L, Uint16 a, Uint16 b)
{
  graphicLayerRecommendedDisplayCIELabValue.clear();
  graphicLayerRecommendedDisplayCIELabValue.putUint16(L,0);
  graphicLayerRecommendedDisplayCIELabValue.putUint16(a,1);
  graphicLayerRecommendedDisplayCIELabValue.putUint16(b,2);
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
      (graphicLayerRecommendedDisplayCIELabValue.getVM() == 3))
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
  }
  else if (graphicLayerRecommendedDisplayCIELabValue.getVM() == 3)
  {
    Uint16 gr=0;
    result = graphicLayerRecommendedDisplayCIELabValue.getUint16(gr,0);
    if (result==EC_Normal) gray = gr; // the L* component of CIELAB is the gray value
  }
  else result=EC_IllegalCall;
  return result;
}

OFCondition DVPSGraphicLayer::getGLRecommendedDisplayValueCIELab(Uint16& L, Uint16& a, Uint16& b)
{
  L = 0;
  a = 0;
  b = 0;
  OFCondition result = EC_Normal;
  if (graphicLayerRecommendedDisplayCIELabValue.getVM()==3)
  {
    Uint16 LL=0;
    Uint16 aa=0;
    Uint16 bb=0;
    result = graphicLayerRecommendedDisplayCIELabValue.getUint16(LL,0);
    if (EC_Normal==result) result = graphicLayerRecommendedDisplayCIELabValue.getUint16(aa,1);
    if (EC_Normal==result) result = graphicLayerRecommendedDisplayCIELabValue.getUint16(bb,2);
    if (result==EC_Normal)
    {
      L = LL;
      a = aa;
      b = bb;
    }
  }
  else if (graphicLayerRecommendedDisplayGrayscaleValue.getVM() == 1)
  {
    Uint16 gr=0;
    result = graphicLayerRecommendedDisplayGrayscaleValue.getUint16(gr,0);
    if (result==EC_Normal)
    {
      L = gr;
      a = 0;
      b = 0;
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
  if (graphicLayerRecommendedDisplayCIELabValue.getVM()==3)
  {
    Uint16 LL=0;
    Uint16 aa=0;
    Uint16 bb=0;
    result = graphicLayerRecommendedDisplayCIELabValue.getUint16(LL,0);
    if (EC_Normal==result) result = graphicLayerRecommendedDisplayCIELabValue.getUint16(aa,1);
    if (EC_Normal==result) result = graphicLayerRecommendedDisplayCIELabValue.getUint16(bb,2);
    if (result==EC_Normal)
    {
      DICOM_Lab_to_RGB(r, g, b, LL, aa, bb);
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

void DVPSGraphicLayer::removeRecommendedDisplayValue(OFBool cielab, OFBool monochrome)
{
  if (cielab) graphicLayerRecommendedDisplayCIELabValue.clear();
  if (monochrome) graphicLayerRecommendedDisplayGrayscaleValue.clear();
  return;
}
