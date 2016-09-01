/*
 *
 *  Copyright (C) 2016, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmpmap
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for managing common paramatric map specific types.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmpmap/dpmtypes.h"

OFLogger DCM_dcmpmapLogger = OFLog::getLogger("dcmtk.dcmpmap");

/*---------------------------------*
 *  constant definitions
 *---------------------------------*/

// conditions
makeOFConditionConst(DPM_InvalidDimensions,      OFM_dcmpmap, 1, OF_error, "Invalid Dimensions");
makeOFConditionConst(DPM_InvalidAttributeValue,  OFM_dcmpmap, 2, OF_error, "Invalid Attribute Value");
makeOFConditionConst(DPM_InvalidPixelInfo,       OFM_dcmpmap, 3, OF_error, "Invalid information in pixel data or related attributes");
makeOFConditionConst(DPM_InvalidPixelData,       OFM_dcmpmap, 4, OF_error, "Invalid pixel data");
makeOFConditionConst(DPM_NoPixelData,            OFM_dcmpmap, 5, OF_error, "No pixel data found");
makeOFConditionConst(DPM_InvalidSOPClass,        OFM_dcmpmap, 6, OF_error, "SOP Class not supported");

OFString DPMTypes::rvf2Str(const RecognizableVisibleFeatures rvf)
{
  switch (rvf)
  {
    case RVF_NO: return "NO";
    case RVF_YES: return "YES";
    default: return "";
  }
}

DPMTypes::RecognizableVisibleFeatures DPMTypes::str2Rvf(const OFString& rvf)
{
  if (rvf == "NO")
    return RVF_NO;
  if (rvf == "YES")
    return RVF_YES;
  else
    return RVF_UNKNOWN;
}


OFString DPMTypes::cq2Str(const DPMTypes::ContentQualification cq)
{
   switch (cq)
  {
    case CQ_PRODUCT:  return "PRODUCT";
    case CQ_RESEARCH: return "RESEARCH";
    case CQ_SERVICE:  return "SERVICE";
    case CQ_UNKNOWN:  return "";
  }
  return "";
}


DPMTypes::ContentQualification DPMTypes::str2Cq(const OFString& cq)
{
  if (cq == "PRODUCT")
    return CQ_PRODUCT;
  if (cq == "RESEARCH")
    return CQ_RESEARCH;
  if (cq == "SERVICE")
    return CQ_SERVICE;
  else
    return CQ_UNKNOWN;
}



OFBool DPMTypes::cqValid(const DPMTypes::ContentQualification cq)
{
   switch (cq)
  {
    case CQ_PRODUCT:
    case CQ_RESEARCH:
    case CQ_SERVICE:  return OFTrue;
    case CQ_UNKNOWN:  return OFFalse;
  }
  return OFFalse;
}


OFBool DPMTypes::rvfValid(const DPMTypes::RecognizableVisibleFeatures rvf)
{
  switch (rvf)
  {
    case RVF_NO: return OFTrue;
    case RVF_YES: return OFTrue;
    case RVF_UNKNOWN: return OFFalse;
  }
  return OFFalse;
}

