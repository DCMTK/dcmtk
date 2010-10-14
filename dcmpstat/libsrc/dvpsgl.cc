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
 *    classes: DVPSGraphicLayer
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

  READ_FROM_DATASET(DcmCodeString, graphicLayer)
  READ_FROM_DATASET(DcmIntegerString, graphicLayerOrder)
  READ_FROM_DATASET(DcmUnsignedShort, graphicLayerRecommendedDisplayGrayscaleValue)
  READ_FROM_DATASET(DcmUnsignedShort, graphicLayerRecommendedDisplayRGBValue)
  READ_FROM_DATASET(DcmLongString, graphicLayerDescription)
  
  /* Now perform basic sanity checks */

  if (graphicLayer.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a graphic layer SQ item with graphicLayer absent or empty");
  }
  else if (graphicLayer.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a graphic layer SQ item with graphicLayer VM != 1");
  }
 
  if (graphicLayerOrder.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a graphic layer SQ item with graphicLayerOrder absent or empty");
  }
  else if (graphicLayerOrder.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a graphic layer SQ item with graphicLayerOrder VM != 1");
  }

  if (graphicLayerRecommendedDisplayGrayscaleValue.getVM()>1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a graphic layer SQ item with graphicLayerRecommendedDisplayGrayscaleValue VM != 1");
  }

  if ((graphicLayerRecommendedDisplayRGBValue.getVM()>0)&&(graphicLayerRecommendedDisplayRGBValue.getVM() != 3))
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a graphic layer SQ item with graphicLayerRecommendedDisplayRGBValue VM != 3");
  }

  if (graphicLayerDescription.getVM() > 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a graphic layer SQ item with graphicLayerDescription VM > 1");
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

/*
 *  $Log: dvpsgl.cc,v $
 *  Revision 1.17  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.16  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.15  2008-04-30 12:38:43  meichel
 *  Fixed compile errors due to changes in attribute tag names
 *
 *  Revision 1.14  2006/08/15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.13  2005/12/08 15:46:28  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.12  2002/11/27 15:48:10  meichel
 *  Adapted module dcmpstat to use of new header file ofstdinc.h
 *
 *  Revision 1.11  2002/08/20 12:41:00  meichel
 *  Added explicit includes for header files included implicitly
 *    via dcstream before.
 *
 *  Revision 1.10  2001/09/26 15:36:26  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.9  2001/06/01 15:50:31  meichel
 *  Updated copyright header
 *
 *  Revision 1.8  2000/06/02 16:01:00  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.7  2000/05/31 13:02:36  meichel
 *  Moved dcmpstat macros and constants into a common header file
 *
 *  Revision 1.6  2000/03/08 16:29:05  meichel
 *  Updated copyright header.
 *
 *  Revision 1.5  2000/03/03 14:13:59  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.4  1999/07/22 16:39:58  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *  Revision 1.3  1999/05/04 15:27:26  meichel
 *  Minor code purifications to keep gcc on OSF1 quiet.
 *
 *  Revision 1.2  1998/12/14 16:10:41  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:41  meichel
 *  Initial Release.
 *
 *
 */

