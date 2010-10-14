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
 *    classes: DVPSGraphicObject
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:32 $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpsgr.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */

/* --------------- class DVPSGraphicObject --------------- */

DVPSGraphicObject::DVPSGraphicObject()
: graphicAnnotationUnits(DCM_GraphicAnnotationUnits)
, numberOfGraphicPoints(DCM_NumberOfGraphicPoints)
, graphicData(DCM_GraphicData)
, graphicType(DCM_GraphicType)
, graphicFilled(DCM_GraphicFilled)
{
}

DVPSGraphicObject::DVPSGraphicObject(const DVPSGraphicObject& copy)
: graphicAnnotationUnits(copy.graphicAnnotationUnits)
, numberOfGraphicPoints(copy.numberOfGraphicPoints)
, graphicData(copy.graphicData)
, graphicType(copy.graphicType)
, graphicFilled(copy.graphicFilled)
{
}

DVPSGraphicObject::~DVPSGraphicObject()
{
}

OFCondition DVPSGraphicObject::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DcmUnsignedShort graphicDimensions(DCM_GraphicDimensions); // VR=US, VM=1, Type 1 

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
    DCMPSTAT_INFO("presentation state contains a graphic object SQ item with graphicAnnotationUnits absent or empty");
  }
  else if (graphicAnnotationUnits.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a graphic object SQ item with graphicAnnotationUnits VM != 1");
  }

  if (graphicDimensions.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a graphic object SQ item with graphicDimensions VM != 1");
  } else {
    Uint16 dimension=0;
    if ((EC_Normal != graphicDimensions.getUint16(dimension,0))||(dimension != 2))
    {
      result=EC_IllegalCall;
      DCMPSTAT_INFO("presentation state contains a graphic object SQ item with graphicDimensions != 2");
    }
  }
  
  if (numberOfGraphicPoints.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a graphic object SQ item with numberOfGraphicPoints absent or empty");
  }
  else if (numberOfGraphicPoints.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a graphic object SQ item with numberOfGraphicPoints VM != 1");
  }
  
  if (graphicData.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a graphic object SQ item with graphicData absent or empty");
  }
  else if (graphicData.getVM() < 2)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a graphic object SQ item with graphicData VM < 2");
  }
  
  if (graphicType.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a graphic object SQ item with graphicType absent or empty");
  }
  else if (graphicType.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a graphic object SQ item with graphicType VM != 1");
  }
  
  if ((graphicFilled.getLength() > 0)&&(graphicFilled.getVM() != 1))
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a graphic object SQ item with graphicFilled present but VM != 1");
  }
  
  return result;
}

OFCondition DVPSGraphicObject::write(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmElement *delem=NULL;
  DcmUnsignedShort graphicDimensions(DCM_GraphicDimensions); // VR=US, VM=1, Type 1 
  Uint16 dimensions=2;
  graphicDimensions.putUint16(dimensions,0);
  
  ADD_TO_DATASET(DcmCodeString, graphicAnnotationUnits)
  ADD_TO_DATASET(DcmUnsignedShort, graphicDimensions)
  ADD_TO_DATASET(DcmUnsignedShort, numberOfGraphicPoints)
  ADD_TO_DATASET(DcmFloatingPointSingle, graphicData)
  ADD_TO_DATASET(DcmCodeString, graphicType)
  
  // strictly speaking we are not allowed to include graphicFilled
  // when the graphicType is "POLYLINE" or "INTERPOLATED" and the
  // first point of graphicData is not equal the last point.
  // For simplicity reasons we always include the attribute unless
  // the graphic type is POINT.
  if (DVPST_point != getGraphicType())
  {
    ADD_TO_DATASET(DcmCodeString, graphicFilled)
  }
  return result;
}


DVPSannotationUnit DVPSGraphicObject::getAnnotationUnits()
{
  DVPSannotationUnit aresult = DVPSA_pixels;
  OFString aString;
  OFCondition result = graphicAnnotationUnits.getOFString(aString,0);
  if ((result==EC_Normal)&&(aString == "DISPLAY")) aresult = DVPSA_display;
  return aresult;
}

size_t DVPSGraphicObject::getNumberOfPoints()
{
  Uint16 result=0;
  if (EC_Normal == numberOfGraphicPoints.getUint16(result,0)) return result;
  else return 0;
}

OFCondition DVPSGraphicObject::getPoint(size_t idx, Float32& x, Float32& y)
{
  x=0.0;
  y=0.0;
  OFCondition result = EC_IllegalCall;
  if ((idx*2+1)<graphicData.getVM())
  {
    result = graphicData.getFloat32(x, 2*idx);
    if (result==EC_Normal) result = graphicData.getFloat32(y, 2*idx+1);
  }
  return result;
}


DVPSGraphicType DVPSGraphicObject::getGraphicType()
{
  OFString aString;
  graphicType.getOFString(aString,0);
  if (aString == "INTERPOLATED") return DVPST_interpolated;
  else if (aString == "CIRCLE") return DVPST_circle;
  else if (aString == "ELLIPSE") return DVPST_ellipse;
  else if (aString == "POINT") return DVPST_point;
  /* default */
  return DVPST_polyline;
}


OFBool DVPSGraphicObject::isFilled()
{
  OFString aString;
  graphicFilled.getOFString(aString,0);
  if (aString == "Y") return OFTrue; else return OFFalse;
} 
  

OFCondition DVPSGraphicObject::setData(size_t number, const Float32 *data, DVPSannotationUnit unit)
{
  if ((data==NULL)||(number==0)) return EC_IllegalCall;
  Uint16 npoints = (Uint16) number;
  numberOfGraphicPoints.clear();
  graphicData.clear();
  graphicAnnotationUnits.clear();
  
  OFCondition result = graphicData.putFloat32Array(data, 2*number);
  if (EC_Normal==result) result = numberOfGraphicPoints.putUint16(npoints,0);
  if (EC_Normal==result)
  {
    if (unit==DVPSA_display) result=graphicAnnotationUnits.putString("DISPLAY"); 
    else result=graphicAnnotationUnits.putString("PIXEL");     
  }
  return result;
}

OFCondition DVPSGraphicObject::setGraphicType(DVPSGraphicType gtype)
{
  const char *cname=NULL;
  switch (gtype)
  {
    case DVPST_interpolated:
      cname = "INTERPOLATED"; 
      break;    
    case DVPST_circle:
      cname = "CIRCLE"; 
      break;    
    case DVPST_ellipse:
      cname = "ELLIPSE"; 
      break;    
    case DVPST_polyline:
      cname = "POLYLINE"; 
      break;    
    case DVPST_point:
      cname = "POINT"; 
      break;    
  }
  return graphicType.putString(cname);
}

OFCondition DVPSGraphicObject::setFilled(OFBool filled)
{
  OFCondition result = EC_Normal;
  if (filled) result=graphicFilled.putString("Y"); 
  else result=graphicFilled.putString("N");
  return result;
}


/*
 *  $Log: dvpsgr.cc,v $
 *  Revision 1.14  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.13  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.12  2006-08-15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.11  2005/12/08 15:46:30  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.10  2001/09/26 15:36:27  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.9  2001/06/01 15:50:31  meichel
 *  Updated copyright header
 *
 *  Revision 1.8  2000/06/02 16:01:01  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.7  2000/05/31 13:02:37  meichel
 *  Moved dcmpstat macros and constants into a common header file
 *
 *  Revision 1.6  2000/03/08 16:29:05  meichel
 *  Updated copyright header.
 *
 *  Revision 1.5  2000/03/03 14:13:59  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.4  1999/07/22 16:39:59  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *  Revision 1.3  1998/12/22 17:57:16  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *  Revision 1.2  1998/12/14 16:10:43  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:42  meichel
 *  Initial Release.
 *
 *
 */


