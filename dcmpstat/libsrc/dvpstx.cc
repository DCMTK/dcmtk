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
 *  Update Date:      $Date: 1998-11-27 14:50:48 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpstx.h"

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


/*
 *  $Log: dvpstx.cc,v $
 *  Revision 1.1  1998-11-27 14:50:48  meichel
 *  Initial Release.
 *
 *
 */

