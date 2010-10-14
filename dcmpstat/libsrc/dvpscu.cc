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
 *    classes: DVPSCurve
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:32 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpscu.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/ofstd/ofbmanip.h"

DVPSCurve::DVPSCurve()
: curveGroup(0)
, numberOfPoints(0)
, typeOfData(DVPSL_polylineCurve)
, curveData(NULL)
, curveDescription()
, curveLabel()
, axisUnitsX()
, axisUnitsY()
{
}

DVPSCurve::DVPSCurve(const DVPSCurve& copy)
: curveGroup(copy.curveGroup)
, numberOfPoints(copy.numberOfPoints)
, typeOfData(copy.typeOfData)
, curveData(NULL)
, curveDescription(copy.curveDescription)
, curveLabel(copy.curveLabel)
, axisUnitsX(copy.axisUnitsX)
, axisUnitsY(copy.axisUnitsY)
{
  if (copy.curveData)
  {
    curveData = new double[2*numberOfPoints];
    if (curveData)
    {
      OFBitmanipTemplate<double>::copyMem(copy.curveData, curveData, 2*numberOfPoints);
    }
  }
}

DVPSCurve::~DVPSCurve()
{
  if (curveData) delete[] curveData;
}


OFCondition DVPSCurve::read(DcmItem &dset, Uint8 group)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  
  curveGroup = group;
  DcmElement *d_curveDimensions = NULL;
  DcmElement *d_numberOfPoints = NULL;
  DcmElement *d_typeOfData = NULL;
  DcmElement *d_dataVR = NULL;
  DcmElement *d_curveData = NULL;
  DcmElement *d_curveDescription = NULL;
  DcmElement *d_axisUnits = NULL;
  DcmElement *d_curveLabel = NULL;
  
  /* first we look for the Curve Data */
  DcmTagKey key(0x5000 + group,0x3000);
  if (EC_Normal == dset.search(key, stack, ESM_fromHere, OFFalse))
  {
    d_curveData = (DcmElement *)(stack.top());  
  } else return EC_IllegalCall;
  
  key.setElement(0x0005); // Curve Dimensions
  stack.clear();
  if (EC_Normal == dset.search(key, stack, ESM_fromHere, OFFalse))
  {
    d_curveDimensions = (DcmElement *)(stack.top());  
  } else return EC_IllegalCall;

  key.setElement(0x0110); // Curve Data Descriptor
  stack.clear();
  if (EC_Normal == dset.search(key, stack, ESM_fromHere, OFFalse))
  {
    return EC_IllegalCall; // we don't support curves with Curve Data Descriptor!
  }

  key.setElement(0x0010); // Number of Points
  stack.clear();
  if (EC_Normal == dset.search(key, stack, ESM_fromHere, OFFalse))
  {
    d_numberOfPoints = (DcmElement *)(stack.top());  
  } else return EC_IllegalCall;

  key.setElement(0x0020); // Type of Data
  stack.clear();
  if (EC_Normal == dset.search(key, stack, ESM_fromHere, OFFalse))
  {
    d_typeOfData = (DcmElement *)(stack.top());  
  } else return EC_IllegalCall;

  key.setElement(0x0103); // Data Value Representation
  stack.clear();
  if (EC_Normal == dset.search(key, stack, ESM_fromHere, OFFalse))
  {
    d_dataVR = (DcmElement *)(stack.top());  
  } else return EC_IllegalCall;

  key.setElement(0x0022); // Curve Description
  stack.clear();
  if (EC_Normal == dset.search(key, stack, ESM_fromHere, OFFalse))
  {
    d_curveDescription = (DcmElement *)(stack.top());  
  }

  key.setElement(0x0030); // Axis Units
  stack.clear();
  if (EC_Normal == dset.search(key, stack, ESM_fromHere, OFFalse))
  {
    d_axisUnits = (DcmElement *)(stack.top());  
  }

  key.setElement(0x2500); // Curve Label
  stack.clear();
  if (EC_Normal == dset.search(key, stack, ESM_fromHere, OFFalse))
  {
    d_curveLabel = (DcmElement *)(stack.top());  
  }

  Uint16 u=0;
  if ((EC_Normal != d_curveDimensions->getUint16(u,0))||(u != 2))
  {
    // Curve Dimensions unreadable or != 2, bail out
    return EC_IllegalCall;
  }
  
  u=0;
  if ((EC_Normal != d_numberOfPoints->getUint16(u,0))||(u == 0))
  {
    // Number of Points == 0 or unreadable, bail out.
    return EC_IllegalCall;
  } else numberOfPoints = u;

  OFString aString;
  if (EC_Normal != d_typeOfData->getOFString(aString,0))
  {
    // cannot read type of data, bail out
    return EC_IllegalCall;
  } else {
    if (aString == "ROI") typeOfData=DVPSL_roiCurve;
    else if (aString == "POLY") typeOfData=DVPSL_polylineCurve;
    else return EC_IllegalCall; // unsupported type of data, bail out
  }

  if ((d_curveDescription) &&(EC_Normal == d_curveDescription->getOFString(aString,0)))
  {
    curveDescription = aString; 
  }
  
  if ((d_axisUnits) &&(EC_Normal == d_axisUnits->getOFString(aString,0)))
  {
    axisUnitsX = aString; 
  }
  if ((d_axisUnits) &&(EC_Normal == d_axisUnits->getOFString(aString,1)))
  {
    axisUnitsY = aString; 
  }

  if ((d_curveLabel) &&(EC_Normal == d_curveLabel->getOFString(aString,0)))
  {
    curveLabel = aString; 
  }
  
  Uint16 dataVR=0;
  if (EC_Normal != d_dataVR->getUint16(dataVR,0))
  {
    // cannot read curve data VR, bail out
    return EC_IllegalCall;
  } 

  curveData = new double[numberOfPoints*2];
  if (curveData==NULL) return EC_MemoryExhausted;

  void *rawData=NULL;
  size_t align=0;
  
  Uint8 *pui8=NULL;
  Uint16 *pui16=NULL;
  Sint16 *psi16=NULL;
  Float32 *pfl32=NULL;
  Float64 *pfl64=NULL;
  Sint32 *psi32=NULL;
  switch (d_curveData->ident())
  {
    case EVR_OB:
    case EVR_OW:
    case EVR_ox:
    case EVR_US:
      
      if (EC_Normal == d_curveData->getUint8Array(pui8))
      {
      	rawData = pui8;
        align = 1;
      } 
      else if (EC_Normal == d_curveData->getUint16Array(pui16))
      {
      	rawData = pui16;
        align = sizeof(Uint16);
      }
      else result = EC_IllegalCall;
      break;
    case EVR_SS:
      if (EC_Normal == d_curveData->getSint16Array(psi16))
      {
      	rawData = psi16;
        align = sizeof(Sint16);
      } else result = EC_IllegalCall;
      break;
    case EVR_FL:
      if (EC_Normal == d_curveData->getFloat32Array(pfl32))
      {
      	rawData = pfl32;
        align = sizeof(Float32);
      } else result = EC_IllegalCall;
      break;
    case EVR_FD:
      if (EC_Normal == d_curveData->getFloat64Array(pfl64))
      {
      	rawData = pfl64;
        align = sizeof(Float64);
      } else result = EC_IllegalCall;
      break;
    case EVR_SL:
      if (EC_Normal == d_curveData->getSint32Array(psi32))
      {
      	rawData = psi32;
        align = sizeof(Sint32);
      } else result = EC_IllegalCall;
      break;
    default:
      result = EC_IllegalCall;
      break;
  }
  
  size_t byteLength = (size_t)(d_curveData->getLength());
  size_t maxidx = 2*numberOfPoints;
  size_t i;
  
  switch (dataVR)
  {
    case 0: // VR=US. align can be 1 or 2
      if ((align==1)||(align==sizeof(Uint16)))
      {
        if (align==1) swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, rawData, byteLength, sizeof(Uint16));
        if (maxidx*sizeof(Uint16) <= byteLength) 
        {
          for (i=0; i<maxidx; i++) curveData[i] = (double)(((Uint16 *)rawData)[i]);
        } else result=EC_IllegalCall;
      } else result=EC_IllegalCall;
      break;
    case 1: // VR=SS. align can be 1 or 2
      if ((align==1)||(align==sizeof(Sint16)))
      {
        if (align==1) swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, rawData, byteLength, sizeof(Sint16));
        if (maxidx*sizeof(Sint16) <= byteLength) 
        {
          for (i=0; i<maxidx; i++) curveData[i] = (double)(((Sint16 *)rawData)[i]);
        } else result=EC_IllegalCall;
      } else result=EC_IllegalCall;
      break;
    case 2: // VR=FL. align can be 1, 2, 4
      if ((align==1)||(align==sizeof(Uint16))||(align==sizeof(Float32)))
      {
      	// if data is word aligned, first swap back to little endian OB
      	if (align==sizeof(Uint16)) swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, rawData, byteLength, sizeof(Uint16));
        if ((align==1)||(align==sizeof(Uint16))) 
          swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, rawData, byteLength, sizeof(Float32));
        if (maxidx*sizeof(Float32) <= byteLength) 
        {
          for (i=0; i<maxidx; i++) curveData[i] = (double)(((Float32 *)rawData)[i]);
        } else result=EC_IllegalCall;
      } else result=EC_IllegalCall;
      break;
    case 3: // VR=FD. align can be 1, 2, 8
      if ((align==1)||(align==sizeof(Uint16))||(align==sizeof(Float64)))
      {
      	// if data is word aligned, first swap back to little endian OB
      	if (align==sizeof(Uint16)) swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, rawData, byteLength, sizeof(Uint16));
        if ((align==1)||(align==sizeof(Uint16)))
          swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, rawData, byteLength, sizeof(Float64));
        if (maxidx*sizeof(Float64) <= byteLength) 
        {
          for (i=0; i<maxidx; i++) curveData[i] = (double)(((Float64 *)rawData)[i]);
        } else result=EC_IllegalCall;
      } else result=EC_IllegalCall;
      break;
    case 4: // VR=SL. align can be 1, 2, 4
      if ((align==1)||(align==sizeof(Uint16))||(align==sizeof(Sint32)))
      {
      	// if data is word aligned, first swap back to little endian OB
      	if (align==sizeof(Uint16)) swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, rawData, byteLength, sizeof(Uint16));
        if ((align==1)||(align==sizeof(Uint16)))
          swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, rawData, byteLength, sizeof(Sint32));
        if (maxidx*sizeof(Sint32) <= byteLength) 
        {
          for (i=0; i<maxidx; i++) curveData[i] = (double)(((Sint32 *)rawData)[i]);
        } else result=EC_IllegalCall;
      } else result=EC_IllegalCall;
      break;
    default:
      result = EC_IllegalCall;
      break;
  }  
    
  return result;
}


OFCondition DVPSCurve::getPoint(size_t idx, double& x, double& y)
{
  x=0.0;
  y=0.0;
  if (idx < numberOfPoints)
  {
    x = curveData[2*idx];
    y = curveData[2*idx+1];
  } else return EC_IllegalCall;
  return EC_Normal;
}

/*
 *  $Log: dvpscu.cc,v $
 *  Revision 1.8  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.7  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.6  2005-12-08 15:46:21  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.5  2001/09/26 15:36:23  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.4  2001/06/01 15:50:28  meichel
 *  Updated copyright header
 *
 *  Revision 1.3  2000/06/02 16:00:58  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.2  2000/03/08 16:29:03  meichel
 *  Updated copyright header.
 *
 *  Revision 1.1  1998/12/22 17:57:14  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *
 */

