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
 *    classes: DVPSTextObject
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:33 $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpstx.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */

#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


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

  READ_FROM_DATASET(DcmCodeString, boundingBoxAnnotationUnits)
  READ_FROM_DATASET(DcmCodeString, anchorPointAnnotationUnits)
  READ_FROM_DATASET(DcmShortText, unformattedTextValue)
  READ_FROM_DATASET(DcmFloatingPointSingle, boundingBoxTLHC)
  READ_FROM_DATASET(DcmFloatingPointSingle, boundingBoxBRHC)
  READ_FROM_DATASET(DcmCodeString, boundingBoxTextHorizontalJustification)
  READ_FROM_DATASET(DcmFloatingPointSingle, anchorPoint)
  READ_FROM_DATASET(DcmCodeString, anchorPointVisibility)
  
  /* Now perform basic sanity checks */
  
  if (unformattedTextValue.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item with unformattedTextValue absent or empty");
  }
  else if (unformattedTextValue.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item with unformattedTextValue VM != 1");
  }

  if (boundingBoxAnnotationUnits.getVM() > 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item with boundingBoxAnnotationUnits VM > 1");
  }

  if (anchorPointAnnotationUnits.getVM() > 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item with anchorPointAnnotationUnits VM > 1");
  }

  if (anchorPointVisibility.getVM() > 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item with anchorPointVisibility VM > 1");
  }

  if (boundingBoxTLHC.getVM() ==1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item with boundingBoxTLHC VM == 1");
  }
  else if (boundingBoxTLHC.getVM() > 2)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item with boundingBoxTLHC VM > 2");
  }

  if (boundingBoxBRHC.getVM() ==1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item with boundingBoxBRHC VM == 1");
  }
  else if (boundingBoxBRHC.getVM() > 2)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item with boundingBoxBRHC VM > 2");
  }

  if (boundingBoxTextHorizontalJustification.getVM() > 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item with boundingBoxTextHorizontalJustification VM > 1");
  }

  if (anchorPoint.getVM() ==1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item with anchorPoint VM == 1");
  }
  else if (anchorPoint.getVM() > 2)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item with anchorPoint VM > 2");
  }

  /* test for the various type 1c conditions */
  
  if ((boundingBoxAnnotationUnits.getVM() == 0)
     && ((boundingBoxTLHC.getVM() >0) || (boundingBoxBRHC.getVM() >0)))
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item with bounding box but boundingBoxAnnotationUnits absent or empty");
  }

  if ((boundingBoxTextHorizontalJustification.getVM() == 0)
     && ((boundingBoxTLHC.getVM() >0) || (boundingBoxBRHC.getVM() >0)))
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item with bounding box but boundingBoxTextHorizontalJustification absent or empty");
  }

  if ((anchorPointAnnotationUnits.getVM() == 0) && (anchorPoint.getVM() >0)) 
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item with anchor point but anchorPointAnnotationUnits absent or empty");
  }

  if ((anchorPointVisibility.getVM() == 0) && (anchorPoint.getVM() >0)) 
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item with anchor point but anchorPointVisibility absent or empty");
  }

  if ((boundingBoxTLHC.getVM() == 0) && (anchorPoint.getVM() == 0)) 
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item without bounding box and anchor point");
  }

  if ((boundingBoxBRHC.getVM() == 0) && (anchorPoint.getVM() == 0)) 
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a text object SQ item without bounding box and anchor point");
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

/*
 *  $Log: dvpstx.cc,v $
 *  Revision 1.16  2010-10-14 13:14:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.15  2009-11-24 14:12:59  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.14  2006-08-15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.13  2005/12/08 15:46:53  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.12  2002/11/27 15:48:17  meichel
 *  Adapted module dcmpstat to use of new header file ofstdinc.h
 *
 *  Revision 1.11  2001/09/26 15:36:35  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.10  2001/06/01 15:50:40  meichel
 *  Updated copyright header
 *
 *  Revision 1.9  2000/06/02 16:01:09  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.8  2000/05/31 13:02:41  meichel
 *  Moved dcmpstat macros and constants into a common header file
 *
 *  Revision 1.7  2000/03/08 16:29:12  meichel
 *  Updated copyright header.
 *
 *  Revision 1.6  2000/03/03 14:14:07  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.5  1999/07/22 16:40:06  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *  Revision 1.4  1999/01/11 13:35:24  meichel
 *  added some explicit type conversions to avoid compiler warnings with VC++.
 *
 *  Revision 1.3  1998/12/22 17:57:19  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *  Revision 1.2  1998/12/14 16:10:49  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:48  meichel
 *  Initial Release.
 *
 *
 */

