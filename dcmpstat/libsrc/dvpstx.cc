/*
 *
 *  Copyright (C) 1998-2021, OFFIS e.V.
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
 *    classes: DVPSTextObject
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpstx.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */


/* --------------- class DVPSTextObject --------------- */

DVPSTextObject::DVPSTextObject()
: boundingBoxAnnotationUnits(DCM_BoundingBoxAnnotationUnits)
, anchorPointAnnotationUnits(DCM_AnchorPointAnnotationUnits)
, unformattedTextValue(DCM_UnformattedTextValue)
, boundingBoxTLHC(DCM_BoundingBoxTopLeftHandCorner)
, boundingBoxBRHC(DCM_BoundingBoxBottomRightHandCorner)
, boundingBoxTextHorizontalJustification(DCM_BoundingBoxTextHorizontalJustification)
, anchorPoint(DCM_AnchorPoint)
, anchorPointVisibility(DCM_AnchorPointVisibility)
{
}

DVPSTextObject::DVPSTextObject(const DVPSTextObject& copy)
: boundingBoxAnnotationUnits(copy.boundingBoxAnnotationUnits)
, anchorPointAnnotationUnits(copy.anchorPointAnnotationUnits)
, unformattedTextValue(copy.unformattedTextValue)
, boundingBoxTLHC(copy.boundingBoxTLHC)
, boundingBoxBRHC(copy.boundingBoxBRHC)
, boundingBoxTextHorizontalJustification(copy.boundingBoxTextHorizontalJustification)
, anchorPoint(copy.anchorPoint)
, anchorPointVisibility(copy.anchorPointVisibility)
{
}

DVPSTextObject::~DVPSTextObject()
{
}

OFCondition DVPSTextObject::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;

  READ_FROM_DATASET(DcmCodeString, EVR_CS, boundingBoxAnnotationUnits)
  READ_FROM_DATASET(DcmCodeString, EVR_CS, anchorPointAnnotationUnits)
  READ_FROM_DATASET(DcmShortText, EVR_ST, unformattedTextValue)
  READ_FROM_DATASET(DcmFloatingPointSingle, EVR_FL, boundingBoxTLHC)
  READ_FROM_DATASET(DcmFloatingPointSingle, EVR_FL, boundingBoxBRHC)
  READ_FROM_DATASET(DcmCodeString, EVR_CS, boundingBoxTextHorizontalJustification)
  READ_FROM_DATASET(DcmFloatingPointSingle, EVR_FL, anchorPoint)
  READ_FROM_DATASET(DcmCodeString, EVR_CS, anchorPointVisibility)
  
  /* Now perform basic sanity checks */
  
  if (unformattedTextValue.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item with unformattedTextValue absent or empty");
  }
  else if (unformattedTextValue.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item with unformattedTextValue VM != 1");
  }

  if (boundingBoxAnnotationUnits.getVM() > 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item with boundingBoxAnnotationUnits VM > 1");
  }

  if (anchorPointAnnotationUnits.getVM() > 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item with anchorPointAnnotationUnits VM > 1");
  }

  if (anchorPointVisibility.getVM() > 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item with anchorPointVisibility VM > 1");
  }

  if (boundingBoxTLHC.getVM() ==1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item with boundingBoxTLHC VM == 1");
  }
  else if (boundingBoxTLHC.getVM() > 2)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item with boundingBoxTLHC VM > 2");
  }

  if (boundingBoxBRHC.getVM() ==1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item with boundingBoxBRHC VM == 1");
  }
  else if (boundingBoxBRHC.getVM() > 2)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item with boundingBoxBRHC VM > 2");
  }

  if (boundingBoxTextHorizontalJustification.getVM() > 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item with boundingBoxTextHorizontalJustification VM > 1");
  }

  if (anchorPoint.getVM() ==1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item with anchorPoint VM == 1");
  }
  else if (anchorPoint.getVM() > 2)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item with anchorPoint VM > 2");
  }

  /* test for the various type 1c conditions */
  
  if ((boundingBoxAnnotationUnits.getVM() == 0)
     && ((boundingBoxTLHC.getVM() >0) || (boundingBoxBRHC.getVM() >0)))
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item with bounding box but boundingBoxAnnotationUnits absent or empty");
  }

  if ((boundingBoxTextHorizontalJustification.getVM() == 0)
     && ((boundingBoxTLHC.getVM() >0) || (boundingBoxBRHC.getVM() >0)))
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item with bounding box but boundingBoxTextHorizontalJustification absent or empty");
  }

  if ((anchorPointAnnotationUnits.getVM() == 0) && (anchorPoint.getVM() >0)) 
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item with anchor point but anchorPointAnnotationUnits absent or empty");
  }

  if ((anchorPointVisibility.getVM() == 0) && (anchorPoint.getVM() >0)) 
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item with anchor point but anchorPointVisibility absent or empty");
  }

  if ((boundingBoxTLHC.getVM() == 0) && (anchorPoint.getVM() == 0)) 
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item without bounding box and anchor point");
  }

  if ((boundingBoxBRHC.getVM() == 0) && (anchorPoint.getVM() == 0)) 
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a text object SQ item without bounding box and anchor point");
  }

  return result;
}


OFCondition DVPSTextObject::write(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmElement *delem=NULL;
  
  ADD_TO_DATASET(DcmShortText, unformattedTextValue)
  if (anchorPoint.getLength() > 0)
  {
    ADD_TO_DATASET(DcmFloatingPointSingle, anchorPoint)
    ADD_TO_DATASET(DcmCodeString, anchorPointAnnotationUnits)
    ADD_TO_DATASET(DcmCodeString, anchorPointVisibility)
  }
  if (boundingBoxTLHC.getLength() > 0)
  {
    ADD_TO_DATASET(DcmFloatingPointSingle, boundingBoxTLHC)
    ADD_TO_DATASET(DcmFloatingPointSingle, boundingBoxBRHC)
    ADD_TO_DATASET(DcmCodeString, boundingBoxAnnotationUnits)
    ADD_TO_DATASET(DcmCodeString, boundingBoxTextHorizontalJustification)
  }
  return result;
}


OFBool DVPSTextObject::haveAnchorPoint()
{
  if (anchorPoint.getLength() > 0) return OFTrue; else return OFFalse;
}

OFBool DVPSTextObject::haveBoundingBox()
{
  if (boundingBoxTLHC.getLength() > 0) return OFTrue; else return OFFalse;
}
   

OFCondition DVPSTextObject::setAnchorPoint(double x, double y, DVPSannotationUnit unit, OFBool isVisible)
{
  Float32 aPoint[2];
  
  anchorPoint.clear();
  anchorPointVisibility.clear();
  anchorPointAnnotationUnits.clear();
 
  aPoint[0] = (Float32) x;
  aPoint[1] = (Float32) y;  
  OFCondition result = anchorPoint.putFloat32Array(aPoint, 2);
  if (result==EC_Normal) 
  {
    if (isVisible) result=anchorPointVisibility.putString("Y"); 
    else result=anchorPointVisibility.putString("N"); 
  }
  if (result==EC_Normal) 
  {
    if (unit==DVPSA_display) result=anchorPointAnnotationUnits.putString("DISPLAY"); 
    else result=anchorPointAnnotationUnits.putString("PIXEL"); 
  }
  return result;
}


OFCondition DVPSTextObject::setBoundingBox(
  double TLHC_x, 
  double TLHC_y, 
  double BRHC_x, 
  double BRHC_y, 
  DVPSannotationUnit unit,
  DVPSTextJustification justification)
{
  Float32 aPoint[2];
	
  boundingBoxAnnotationUnits.clear();
  boundingBoxTLHC.clear();
  boundingBoxBRHC.clear();
  
  aPoint[0] = (Float32)TLHC_x;
  aPoint[1] = (Float32)TLHC_y;  
  OFCondition result = boundingBoxTLHC.putFloat32Array(aPoint, 2);
  if (result==EC_Normal) 
  {
    aPoint[0] = (Float32)BRHC_x;
    aPoint[1] = (Float32)BRHC_y;  
    result = boundingBoxBRHC.putFloat32Array(aPoint, 2);
  }
  if (result==EC_Normal) 
  {
    if (unit==DVPSA_display) result=boundingBoxAnnotationUnits.putString("DISPLAY"); 
    else result=boundingBoxAnnotationUnits.putString("PIXEL"); 
    switch (justification)
    {
      case DVPSX_left:
        boundingBoxTextHorizontalJustification.putString("LEFT");
        break;
      case DVPSX_right:
        boundingBoxTextHorizontalJustification.putString("RIGHT");
        break;
      case DVPSX_center:
        boundingBoxTextHorizontalJustification.putString("CENTER");
        break;  
    }
  }
  return result;
}

OFCondition DVPSTextObject::setText(const char *text)
{
  if ((text==NULL)||(strlen(text)==0)) return EC_IllegalCall;
  return unformattedTextValue.putString(text);
}


void DVPSTextObject::removeAnchorPoint()
{
  anchorPoint.clear();
  anchorPointVisibility.clear();
  anchorPointAnnotationUnits.clear();
  return;
}

void DVPSTextObject::removeBoundingBox()
{
  boundingBoxAnnotationUnits.clear();
  boundingBoxTLHC.clear();
  boundingBoxBRHC.clear();
  return;
} 

const char *DVPSTextObject::getText()
{
  char *c = NULL;
  if (EC_Normal == unformattedTextValue.getString(c)) return c; else return NULL;
}

double DVPSTextObject::getBoundingBoxTLHC_x()
{
  Float32 result =0.0;
  if (boundingBoxTLHC.getVM() == 2)
  {
    boundingBoxTLHC.getFloat32(result,0);
  }
  return result;
}

double DVPSTextObject::getBoundingBoxTLHC_y()
{
  Float32 result =0.0;
  if (boundingBoxTLHC.getVM() == 2)
  {
    boundingBoxTLHC.getFloat32(result,1);
  }
  return result;
}

double DVPSTextObject::getBoundingBoxBRHC_x()
{
  Float32 result =0.0;
  if (boundingBoxBRHC.getVM() == 2)
  {
    boundingBoxBRHC.getFloat32(result,0);
  }
  return result;
}

double DVPSTextObject::getBoundingBoxBRHC_y()
{
  Float32 result =0.0;
  if (boundingBoxBRHC.getVM() == 2)
  {
    boundingBoxBRHC.getFloat32(result,1);
  }
  return result;
}
   

DVPSannotationUnit DVPSTextObject::getBoundingBoxAnnotationUnits()
{
  DVPSannotationUnit aresult = DVPSA_pixels;
  OFString aString;
  OFCondition result = boundingBoxAnnotationUnits.getOFString(aString,0);
  if ((result==EC_Normal)&&(aString == "DISPLAY")) aresult = DVPSA_display;
  return aresult;
}

DVPSTextJustification DVPSTextObject::getBoundingBoxHorizontalJustification()
{
  DVPSTextJustification aresult = DVPSX_left;
  OFString aString;
  OFCondition result = boundingBoxTextHorizontalJustification.getOFString(aString,0);
  if ((result==EC_Normal)&&(aString == "RIGHT")) aresult = DVPSX_right;
  if ((result==EC_Normal)&&(aString == "CENTER")) aresult = DVPSX_center;
  return aresult;
}

double DVPSTextObject::getAnchorPoint_x()
{
  Float32 result =0.0;
  if (anchorPoint.getVM() == 2)
  {
    anchorPoint.getFloat32(result,0);
  }
  return result;
}
 
double DVPSTextObject::getAnchorPoint_y()
{
  Float32 result =0.0;
  if (anchorPoint.getVM() == 2)
  {
    anchorPoint.getFloat32(result,1);
  }
  return result;
}
 
OFBool DVPSTextObject::anchorPointIsVisible()
{
  OFString aString;
  anchorPointVisibility.getOFString(aString,0);
  if (aString == "Y") return OFTrue; else return OFFalse;
} 

DVPSannotationUnit DVPSTextObject::getAnchorPointAnnotationUnits()
{
  DVPSannotationUnit aresult = DVPSA_pixels;
  OFString aString;
  OFCondition result = anchorPointAnnotationUnits.getOFString(aString,0);
  if ((result==EC_Normal)&&(aString == "DISPLAY")) aresult = DVPSA_display;
  return aresult;
}
