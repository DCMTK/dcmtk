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
 *    classes: DVPSTextObject
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-12-14 16:10:49 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpstx.h"
#include "ofstring.h"

#include <string.h>

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

/* --------------- class DVPSTextObject --------------- */

DVPSTextObject::DVPSTextObject()
: specificCharacterSet(DCM_SpecificCharacterSet)
, boundingBoxAnnotationUnits(DCM_BoundingBoxAnnotationUnits)
, anchorPointAnnotationUnits(DCM_AnchorPointAnnotationUnits)
, unformattedTextValue(DCM_UnformattedTextValue)
, boundingBoxTLHC(DCM_BoundingBoxTLHC)
, boundingBoxBRHC(DCM_BoundingBoxBRHC)
, anchorPoint(DCM_AnchorPoint)
, anchorPointVisibility(DCM_AnchorPointVisibility)
{
}

DVPSTextObject::DVPSTextObject(const DVPSTextObject& copy)
: specificCharacterSet(copy.specificCharacterSet)
, boundingBoxAnnotationUnits(copy.boundingBoxAnnotationUnits)
, anchorPointAnnotationUnits(copy.anchorPointAnnotationUnits)
, unformattedTextValue(copy.unformattedTextValue)
, boundingBoxTLHC(copy.boundingBoxTLHC)
, boundingBoxBRHC(copy.boundingBoxBRHC)
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

  READ_FROM_DATASET(DcmCodeString, specificCharacterSet)
  READ_FROM_DATASET(DcmCodeString, boundingBoxAnnotationUnits)
  READ_FROM_DATASET(DcmCodeString, anchorPointAnnotationUnits)
  READ_FROM_DATASET(DcmShortText, unformattedTextValue)
  READ_FROM_DATASET(DcmFloatingPointSingle, boundingBoxTLHC)
  READ_FROM_DATASET(DcmFloatingPointSingle, boundingBoxBRHC)
  READ_FROM_DATASET(DcmFloatingPointSingle, anchorPoint)
  READ_FROM_DATASET(DcmCodeString, anchorPointVisibility)
  
  /* Now perform basic sanity checks */
  
  if (unformattedTextValue.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a text object SQ item with unformattedTextValue absent or empty" << endl;
#endif
  }
  else if (unformattedTextValue.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a text object SQ item with unformattedTextValue VM != 1" << endl;
#endif
  }

  if (boundingBoxAnnotationUnits.getVM() > 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a text object SQ item with boundingBoxAnnotationUnits VM > 1" << endl;
#endif
  }

  if (anchorPointAnnotationUnits.getVM() > 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a text object SQ item with anchorPointAnnotationUnits VM > 1" << endl;
#endif
  }

  if (anchorPointVisibility.getVM() > 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a text object SQ item with anchorPointVisibility VM > 1" << endl;
#endif
  }

  if (boundingBoxTLHC.getVM() ==1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a text object SQ item with boundingBoxTLHC VM == 1" << endl;
#endif
  }
  else if (boundingBoxTLHC.getVM() > 2)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a text object SQ item with boundingBoxTLHC VM > 2" << endl;
#endif
  }

  if (boundingBoxBRHC.getVM() ==1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a text object SQ item with boundingBoxBRHC VM == 1" << endl;
#endif
  }
  else if (boundingBoxBRHC.getVM() > 2)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a text object SQ item with boundingBoxBRHC VM > 2" << endl;
#endif
  }

  if (anchorPoint.getVM() ==1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a text object SQ item with anchorPoint VM == 1" << endl;
#endif
  }
  else if (anchorPoint.getVM() > 2)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a text object SQ item with anchorPoint VM > 2" << endl;
#endif
  }

  /* test for the various type 1c conditions */
  
  if ((boundingBoxAnnotationUnits.getVM() == 0)
     && ((boundingBoxTLHC.getVM() >0) || (boundingBoxBRHC.getVM() >0)))
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a text object SQ item with bounding box but boundingBoxAnnotationUnits absent or empty" << endl;
#endif
  }

  if ((anchorPointAnnotationUnits.getVM() == 0) && (anchorPoint.getVM() >0)) 
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a text object SQ item with anchor point but anchorPointAnnotationUnits absent or empty" << endl;
#endif
  }

  if ((anchorPointVisibility.getVM() == 0) && (anchorPoint.getVM() >0)) 
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a text object SQ item with anchor point but anchorPointVisibility absent or empty" << endl;
#endif
  }

  if ((boundingBoxTLHC.getVM() == 0) && (anchorPoint.getVM() == 0)) 
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a text object SQ item without bounding box and anchor point" << endl;
#endif
  }

  if ((boundingBoxBRHC.getVM() == 0) && (anchorPoint.getVM() == 0)) 
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a text object SQ item without bounding box and anchor point" << endl;
#endif
  }

  return result;
}


E_Condition DVPSTextObject::write(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmElement *delem=NULL;
  
  ADD_TO_DATASET(DcmShortText, unformattedTextValue)
  if (specificCharacterSet.getLength() > 0)
  {
    ADD_TO_DATASET(DcmCodeString, specificCharacterSet)
  }
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
 
  aPoint[0] = x;
  aPoint[1] = y;  
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


E_Condition DVPSTextObject::setBoundingBox(double TLHC_x, double TLHC_y, double BRHC_x, double BRHC_y, DVPSannotationUnit unit) 
{
  Float32 aPoint[2];
	
  boundingBoxAnnotationUnits.clear();
  boundingBoxTLHC.clear();
  boundingBoxBRHC.clear();
  
  aPoint[0] = TLHC_x;
  aPoint[1] = TLHC_y;  
  E_Condition result = boundingBoxTLHC.putFloat32Array(aPoint, 2);
  if (result==EC_Normal) 
  {
    aPoint[0] = BRHC_x;
    aPoint[1] = BRHC_y;  
    result = boundingBoxBRHC.putFloat32Array(aPoint, 2);
  }
  if (result==EC_Normal) 
  {
    if (unit==DVPSA_display) result=boundingBoxAnnotationUnits.putString("DISPLAY"); 
    else result=boundingBoxAnnotationUnits.putString("PIXEL"); 
  }
  return result;
}

E_Condition DVPSTextObject::setText(const char *text)
{
  if ((text==NULL)||(strlen(text)==0)) return EC_IllegalCall;
  return unformattedTextValue.putString(text);
}


E_Condition DVPSTextObject::setCharset(DVPScharacterSet charset, DVPScharacterSet higherLevelCharset)
{
  if (charset==DVPSC_other) return EC_IllegalCall;
  if (charset==higherLevelCharset)
  {
    // char sets are identical, no need to specify here.
    specificCharacterSet.clear();
    return EC_Normal;
  }
  const char *cname=NULL;
  switch (charset)
  {
    case DVPSC_latin1:
      cname = "ISO_IR 100"; 
      break;    
    case DVPSC_latin2:
      cname = "ISO_IR 101"; 
      break;    
    case DVPSC_latin3:
      cname = "ISO_IR 109"; 
      break;    
    case DVPSC_latin4:
      cname = "ISO_IR 110"; 
      break;    
    case DVPSC_latin5:
      cname = "ISO_IR 148"; 
      break;    
    case DVPSC_cyrillic:
      cname = "ISO_IR 144"; 
      break;    
    case DVPSC_arabic:
      cname = "ISO_IR 127"; 
      break;    
    case DVPSC_greek:
      cname = "ISO_IR 126"; 
      break;    
    case DVPSC_hebrew:
      cname = "ISO_IR 138"; 
      break;    
    case DVPSC_japanese:
      cname = "ISO_IR 13"; 
      break;    
    default: // can only be DVPSC_ascii
      cname = "ISO_IR 6"; 
      break;    
  }
  return specificCharacterSet.putString(cname);
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

DVPScharacterSet DVPSTextObject::getCharset(DVPScharacterSet higherLevelCharset)
{
  if (specificCharacterSet.getLength()==0) return higherLevelCharset;
  
  OFString aString;
  specificCharacterSet.getOFString(aString,0);
  if (aString == "ISO_IR 6") return DVPSC_ascii;
  else if (aString == "ISO_IR 100") return DVPSC_latin1;
  else if (aString == "ISO_IR 101") return DVPSC_latin2;
  else if (aString == "ISO_IR 109") return DVPSC_latin3;
  else if (aString == "ISO_IR 110") return DVPSC_latin4;
  else if (aString == "ISO_IR 148") return DVPSC_latin5;
  else if (aString == "ISO_IR 144") return DVPSC_cyrillic;
  else if (aString == "ISO_IR 127") return DVPSC_arabic;
  else if (aString == "ISO_IR 126") return DVPSC_greek;
  else if (aString == "ISO_IR 138") return DVPSC_hebrew;
  else if (aString == "ISO_IR 13")  return DVPSC_japanese;
  /* default */
  return DVPSC_other;
}

const char *DVPSTextObject::getCharsetString()
{
  char *c = NULL;
  if (EC_Normal == specificCharacterSet.getString(c)) return c; else return NULL;
}

/*
 *  $Log: dvpstx.cc,v $
 *  Revision 1.2  1998-12-14 16:10:49  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:48  meichel
 *  Initial Release.
 *
 *
 */

