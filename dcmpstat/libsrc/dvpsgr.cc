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
 *    classes: DVPSGraphicObject
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-11-27 14:50:42 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsgr.h"

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


/* --------------- class DVPSGraphicObject --------------- */

DVPSGraphicObject::DVPSGraphicObject()
: graphicAnnotationUnits(DCM_GraphicAnnotationUnits)
, graphicDimensions(DCM_GraphicDimensions)
, numberOfGraphicPoints(DCM_NumberOfGraphicPoints)
, graphicData(DCM_GraphicData)
, graphicType(DCM_GraphicType)
, graphicFilled(DCM_GraphicFilled)
{
}

DVPSGraphicObject::DVPSGraphicObject(const DVPSGraphicObject& copy)
: graphicAnnotationUnits(copy.graphicAnnotationUnits)
, graphicDimensions(copy.graphicDimensions)
, numberOfGraphicPoints(copy.numberOfGraphicPoints)
, graphicData(copy.graphicData)
, graphicType(copy.graphicType)
, graphicFilled(copy.graphicFilled)
{
}

DVPSGraphicObject::~DVPSGraphicObject()
{
}

E_Condition DVPSGraphicObject::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmStack stack;

  READ_FROM_DATASET(DcmCodeString, graphicAnnotationUnits)
  READ_FROM_DATASET(DcmUnsignedShort, graphicDimensions)
  READ_FROM_DATASET(DcmUnsignedShort, numberOfGraphicPoints)
  READ_FROM_DATASET(DcmFloatingPointSingle, graphicData)
  READ_FROM_DATASET(DcmCodeString, graphicType)
  READ_FROM_DATASET(DcmCodeString, graphicFilled)
  
  /* Now perform basic sanity checks */
  
  if (graphicAnnotationUnits.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic object SQ item with graphicAnnotationUnits absent or empty" << endl;
#endif
  }
  else if (graphicAnnotationUnits.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic object SQ item with graphicAnnotationUnits VM != 1" << endl;
#endif
  }

  if (graphicDimensions.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic object SQ item with graphicDimensions absent or empty" << endl;
#endif
  }
  else if (graphicDimensions.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic object SQ item with graphicDimensions VM != 1" << endl;
#endif
  }
  
  if (numberOfGraphicPoints.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic object SQ item with numberOfGraphicPoints absent or empty" << endl;
#endif
  }
  else if (numberOfGraphicPoints.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic object SQ item with numberOfGraphicPoints VM != 1" << endl;
#endif
  }
  
  if (graphicData.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic object SQ item with graphicData absent or empty" << endl;
#endif
  }
  else if (graphicData.getVM() < 2)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic object SQ item with graphicData VM < 2" << endl;
#endif
  }
  
  if (graphicType.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic object SQ item with graphicType absent or empty" << endl;
#endif
  }
  else if (graphicType.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic object SQ item with graphicType VM != 1" << endl;
#endif
  }
  
  if ((graphicFilled.getLength() > 0)&&(graphicFilled.getVM() != 1))
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a graphic object SQ item with graphicFilled present but VM != 1" << endl;
#endif
  }
  
  return result;
}

E_Condition DVPSGraphicObject::write(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmElement *delem=NULL;
  
  ADD_TO_DATASET(DcmCodeString, graphicAnnotationUnits)
  ADD_TO_DATASET(DcmUnsignedShort, graphicDimensions)
  ADD_TO_DATASET(DcmUnsignedShort, numberOfGraphicPoints)
  ADD_TO_DATASET(DcmFloatingPointSingle, graphicData)
  ADD_TO_DATASET(DcmCodeString, graphicType)
  
  // strictly speaking we are not allowed to include graphicFilled
  // when the graphicType is "POLYLINE" or "INTERPOLATED" and the
  // first point of graphicData is not equal the last point.
  // For simplicity reasons we always include the attribute.
  ADD_TO_DATASET(DcmCodeString, graphicFilled)

  return result;
}

/*
 *  $Log: dvpsgr.cc,v $
 *  Revision 1.1  1998-11-27 14:50:42  meichel
 *  Initial Release.
 *
 *
 */

