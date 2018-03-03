/*
 *
 *  Copyright (C) 1998-2018, OFFIS e.V.
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
 *    classes: DVPSOverlay
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpsov.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */


/* --------------- class DVPSOverlay --------------- */

DVPSOverlay::DVPSOverlay()
: overlayGroup(0)
, overlayRows(DCM_OverlayRows)
, overlayColumns(DCM_OverlayColumns)
, overlayType(DCM_OverlayType)
, overlayOrigin(DCM_OverlayOrigin)
, overlayBitsAllocated(DCM_OverlayBitsAllocated)
, overlayBitPosition(DCM_OverlayBitPosition)
, overlayData(DCM_OverlayData)
, overlayDescription(DCM_OverlayDescription)
, overlayLabel(DCM_OverlayLabel)
{
}

DVPSOverlay::DVPSOverlay(const DVPSOverlay& copy)
: overlayGroup(copy.overlayGroup)
, overlayRows(copy.overlayRows)
, overlayColumns(copy.overlayColumns)
, overlayType(copy.overlayType)
, overlayOrigin(copy.overlayOrigin)
, overlayBitsAllocated(copy.overlayBitsAllocated)
, overlayBitPosition(copy.overlayBitPosition)
, overlayData(copy.overlayData)
, overlayDescription(copy.overlayDescription)
, overlayLabel(copy.overlayLabel)
{
}

DVPSOverlay::~DVPSOverlay()
{
}

OFCondition DVPSOverlay::read(DcmItem &dset, Uint8 ovGroup, Uint8 asGroup)
{
  OFCondition result = EC_Normal;
  DcmStack stack;

  if (asGroup==0xFF) asGroup=ovGroup;
  overlayGroup = asGroup;
  Uint16 gtag = 0x6000 + ovGroup;

  overlayRows.setGTag(gtag);
  overlayColumns.setGTag(gtag);
  overlayType.setGTag(gtag);
  overlayOrigin.setGTag(gtag);
  overlayBitsAllocated.setGTag(gtag);
  overlayBitPosition.setGTag(gtag);
  overlayData.setGTag(gtag);
  overlayDescription.setGTag(gtag);
  overlayLabel.setGTag(gtag);
  READ_FROM_DATASET(DcmUnsignedShort, EVR_US, overlayRows)
  READ_FROM_DATASET(DcmUnsignedShort, EVR_US, overlayColumns)
  READ_FROM_DATASET(DcmCodeString, EVR_CS, overlayType)
  READ_FROM_DATASET(DcmSignedShort, EVR_SS, overlayOrigin)
  READ_FROM_DATASET(DcmUnsignedShort, EVR_US, overlayBitsAllocated)
  READ_FROM_DATASET(DcmUnsignedShort, EVR_US, overlayBitPosition)
  READ_FROM_DATASET(DcmOverlayData, EVR_OverlayData, overlayData)
  READ_FROM_DATASET(DcmLongString, EVR_LO, overlayDescription)
  READ_FROM_DATASET(DcmLongString, EVR_LO, overlayLabel)

  /* Now perform basic sanity checks and adjust use flags */
  if (overlayRows.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains an overlay with overlayRows absent or empty");
  }
  else if (overlayRows.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains an overlay with overlayRows VM != 1");
  }

  if (overlayColumns.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains an overlay with overlayColumns absent or empty");
  }
  else if (overlayColumns.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains an overlay with overlayColumns VM != 1");
  }

  if (overlayType.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains an overlay with overlayType absent or empty");
  }
  else if (overlayType.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains an overlay with overlayType VM != 1");
  }

  if (overlayOrigin.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains an overlay with overlayOrigin absent or empty");
  }
  else if (overlayOrigin.getVM() != 2)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains an overlay with overlayOrigin VM != 2");
  }

  if (overlayBitsAllocated.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains an overlay with overlayBitsAllocated absent or empty");
  }
  else if (overlayBitsAllocated.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains an overlay with overlayBitsAllocated VM != 1");
  }

  if (overlayBitPosition.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains an overlay with overlayBitPosition absent or empty");
  }
  else if (overlayBitPosition.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains an overlay with overlayBitPosition VM != 1");
  }

  /* in a presentation state object, the overlay data must always be present in this group */
  if (overlayData.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains an overlay with overlayData absent or empty");
  }

  return result;
}


OFCondition DVPSOverlay::write(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmElement *delem=NULL;
  Uint16 repeatingGroup = 0x6000 + overlayGroup;

  ADD_REPEATING_ELEMENT_TO_DATASET(DcmUnsignedShort, overlayRows, repeatingGroup)
  ADD_REPEATING_ELEMENT_TO_DATASET(DcmUnsignedShort, overlayColumns, repeatingGroup)
  ADD_REPEATING_ELEMENT_TO_DATASET(DcmCodeString, overlayType, repeatingGroup)
  ADD_REPEATING_ELEMENT_TO_DATASET(DcmSignedShort, overlayOrigin, repeatingGroup)
  ADD_REPEATING_ELEMENT_TO_DATASET(DcmUnsignedShort, overlayBitsAllocated, repeatingGroup)
  ADD_REPEATING_ELEMENT_TO_DATASET(DcmUnsignedShort, overlayBitPosition, repeatingGroup)
  if (overlayData.getLength() >0)
  {
    ADD_REPEATING_ELEMENT_TO_DATASET(DcmOverlayData, overlayData, repeatingGroup)
  }
  if (overlayDescription.getLength() >0)
  {
    ADD_REPEATING_ELEMENT_TO_DATASET(DcmLongString, overlayDescription, repeatingGroup)
  }
  if (overlayLabel.getLength() >0)
  {
    ADD_REPEATING_ELEMENT_TO_DATASET(DcmLongString, overlayLabel, repeatingGroup)
  }

  return result;
}


OFBool DVPSOverlay::isSuitableAsShutter(unsigned long x, unsigned long y)
{
  // check that overlay is Graphic, not ROI.
  if (isROI()) return OFFalse;

  // check if overlay origin is 1\1
  Sint16 originX=0;
  Sint16 originY=0;
  OFCondition result = overlayOrigin.getSint16(originX,0);
  if (result==EC_Normal) result = overlayOrigin.getSint16(originY,1);
  if ((result != EC_Normal)||(originX != 1)||(originY != 1)) return OFFalse;

  // check if overlay size matches given image size
  Uint16 rows=0;
  Uint16 columns=0;
  result = overlayRows.getUint16(rows,0);
  if (result==EC_Normal) result = overlayColumns.getUint16(columns,0);

  if (result==EC_Normal) return ((columns==x)&&(rows==y));
  return OFFalse;
}

const char *DVPSOverlay::getOverlayLabel()
{
  char *c = NULL;
  if (EC_Normal == overlayLabel.getString(c)) return c; else return NULL;
}

const char *DVPSOverlay::getOverlayDescription()
{
  char *c = NULL;
  if (EC_Normal == overlayDescription.getString(c)) return c; else return NULL;
}

OFBool DVPSOverlay::isROI()
{
  OFString aString;
  if (EC_Normal == overlayType.getOFString(aString,0))
  {
    return (aString == "ROI");
  }
  return OFFalse;
}

OFCondition DVPSOverlay::getValues(
  Sint16& originX,
  Sint16& originY,
  Uint16& sizeX,
  Uint16& sizeY)
{
  OFCondition result = overlayOrigin.getSint16(originX,1);
  if (result.good()) result = overlayOrigin.getSint16(originY,0);
  if (result.good()) result = overlayColumns.getUint16(sizeX,0);
  if (result.good()) result = overlayRows.getUint16(sizeY,0);
  return result;
}
