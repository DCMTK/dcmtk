/*
 *
 *  Copyright (C) 1998-2000, OFFIS
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
 *    classes: DVPSTextObject
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-05-31 13:02:41 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpstx.h"
#include "ofstring.h"
#include "dvpsdef.h"     /* for constants and macros */

#include <string.h>


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

E_Condition DVPSTextObject::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
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
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item with unformattedTextValue absent or empty" << endl;
#endif
  }
  else if (unformattedTextValue.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item with unformattedTextValue VM != 1" << endl;
#endif
  }

  if (boundingBoxAnnotationUnits.getVM() > 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item with boundingBoxAnnotationUnits VM > 1" << endl;
#endif
  }

  if (anchorPointAnnotationUnits.getVM() > 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item with anchorPointAnnotationUnits VM > 1" << endl;
#endif
  }

  if (anchorPointVisibility.getVM() > 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item with anchorPointVisibility VM > 1" << endl;
#endif
  }

  if (boundingBoxTLHC.getVM() ==1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item with boundingBoxTLHC VM == 1" << endl;
#endif
  }
  else if (boundingBoxTLHC.getVM() > 2)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item with boundingBoxTLHC VM > 2" << endl;
#endif
  }

  if (boundingBoxBRHC.getVM() ==1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item with boundingBoxBRHC VM == 1" << endl;
#endif
  }
  else if (boundingBoxBRHC.getVM() > 2)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item with boundingBoxBRHC VM > 2" << endl;
#endif
  }

  if (boundingBoxTextHorizontalJustification.getVM() > 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item with boundingBoxTextHorizontalJustification VM > 1" << endl;
#endif
  }

  if (anchorPoint.getVM() ==1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item with anchorPoint VM == 1" << endl;
#endif
  }
  else if (anchorPoint.getVM() > 2)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item with anchorPoint VM > 2" << endl;
#endif
  }

  /* test for the various type 1c conditions */
  
  if ((boundingBoxAnnotationUnits.getVM() == 0)
     && ((boundingBoxTLHC.getVM() >0) || (boundingBoxBRHC.getVM() >0)))
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item with bounding box but boundingBoxAnnotationUnits absent or empty" << endl;
#endif
  }

  if ((boundingBoxTextHorizontalJustification.getVM() == 0)
     && ((boundingBoxTLHC.getVM() >0) || (boundingBoxBRHC.getVM() >0)))
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item with bounding box but boundingBoxTextHorizontalJustification absent or empty" << endl;
#endif
  }

  if ((anchorPointAnnotationUnits.getVM() == 0) && (anchorPoint.getVM() >0)) 
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item with anchor point but anchorPointAnnotationUnits absent or empty" << endl;
#endif
  }

  if ((anchorPointVisibility.getVM() == 0) && (anchorPoint.getVM() >0)) 
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item with anchor point but anchorPointVisibility absent or empty" << endl;
#endif
  }

  if ((boundingBoxTLHC.getVM() == 0) && (anchorPoint.getVM() == 0)) 
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item without bounding box and anchor point" << endl;
#endif
  }

  if ((boundingBoxBRHC.getVM() == 0) && (anchorPoint.getVM() == 0)) 
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a text object SQ item without bounding box and anchor point" << endl;
#endif
  }

  return result;
}


E_Condition DVPSTextObject::write(DcmItem &dset)
{
  E_Condition result = EC_Normal;
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
   

E_Condition DVPSTextObject::setAnchorPoint(double x, double y, DVPSannotationUnit unit, OFBool isVisible)
{
  Float32 aPoint[2];
  
  anchorPoint.clear();
  anchorPointVisibility.clear();
  anchorPointAnnotationUnits.clear();
 
  aPoint[0] = (Float32) x;
  aPoint[1] = (Float32) y;  
  E_Condition result = anchorPoint.putFloat32Array(aPoint, 2);
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


E_Condition DVPSTextObject::setBoundingBox(
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
  E_Condition result = boundingBoxTLHC.putFloat32Array(aPoint, 2);
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

E_Condition DVPSTextObject::setText(const char *text)
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
  E_Condition result = boundingBoxAnnotationUnits.getOFString(aString,0);
  if ((result==EC_Normal)&&(aString == "DISPLAY")) aresult = DVPSA_display;
  return aresult;
}

DVPSTextJustification DVPSTextObject::getBoundingBoxHorizontalJustification()
{
  DVPSTextJustification aresult = DVPSX_left;
  OFString aString;
  E_Condition result = boundingBoxTextHorizontalJustification.getOFString(aString,0);
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
  E_Condition result = anchorPointAnnotationUnits.getOFString(aString,0);
  if ((result==EC_Normal)&&(aString == "DISPLAY")) aresult = DVPSA_display;
  return aresult;
}

/*
 *  $Log: dvpstx.cc,v $
 *  Revision 1.8  2000-05-31 13:02:41  meichel
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

