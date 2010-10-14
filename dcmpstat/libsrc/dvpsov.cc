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
 *    classes: DVPSOverlay
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:32 $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
  READ_FROM_DATASET(DcmUnsignedShort, overlayRows)
  READ_FROM_DATASET(DcmUnsignedShort, overlayColumns)
  READ_FROM_DATASET(DcmCodeString, overlayType)
  READ_FROM_DATASET(DcmSignedShort, overlayOrigin)
  READ_FROM_DATASET(DcmUnsignedShort, overlayBitsAllocated)
  READ_FROM_DATASET(DcmUnsignedShort, overlayBitPosition)
  READ_FROM_DATASET(DcmOverlayData, overlayData)
  READ_FROM_DATASET(DcmLongString, overlayDescription)
  READ_FROM_DATASET(DcmLongString, overlayLabel)

  /* Now perform basic sanity checks and adjust use flags */
  if (overlayRows.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains an overlay with overlayRows absent or empty");
  }
  else if (overlayRows.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains an overlay with overlayRows VM != 1");
  }

  if (overlayColumns.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains an overlay with overlayColumns absent or empty");
  }
  else if (overlayColumns.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains an overlay with overlayColumns VM != 1");
  }

  if (overlayType.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains an overlay with overlayType absent or empty");
  }
  else if (overlayType.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains an overlay with overlayType VM != 1");
  }

  if (overlayOrigin.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains an overlay with overlayOrigin absent or empty");
  }
  else if (overlayOrigin.getVM() != 2)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains an overlay with overlayOrigin VM != 2");
  }

  if (overlayBitsAllocated.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains an overlay with overlayBitsAllocated absent or empty");
  }
  else if (overlayBitsAllocated.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains an overlay with overlayBitsAllocated VM != 1");
  }

  if (overlayBitPosition.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains an overlay with overlayBitPosition absent or empty");
  }
  else if (overlayBitPosition.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains an overlay with overlayBitPosition VM != 1");
  }

  /* in a presentation state object, the overlay data must always be present in this group */
  if (overlayData.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains an overlay with overlayData absent or empty");
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


/*
 *  $Log: dvpsov.cc,v $
 *  Revision 1.19  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.18  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.17  2006-08-15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.16  2005/12/08 15:46:36  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.15  2003/08/27 14:59:42  meichel
 *  Changed API of class DVPSOverlay to avoid dependency on module dcmimgle
 *
 *  Revision 1.14  2002/12/09 13:28:16  joergr
 *  Renamed parameter/local variable to avoid name clashes with global
 *  declaration left and/or right (used for as iostream manipulators).
 *
 *  Revision 1.13  2001/12/18 10:36:41  meichel
 *  Minor modifications to avoid warning on Sun CC 2.0.1
 *
 *  Revision 1.12  2001/09/26 15:36:28  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.11  2001/06/01 15:50:34  meichel
 *  Updated copyright header
 *
 *  Revision 1.10  2000/06/02 16:01:03  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.9  2000/05/31 13:02:37  meichel
 *  Moved dcmpstat macros and constants into a common header file
 *
 *  Revision 1.8  2000/03/08 16:29:07  meichel
 *  Updated copyright header.
 *
 *  Revision 1.7  2000/03/03 14:14:01  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.6  1999/05/03 11:01:37  joergr
 *  Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 *  Revision 1.5  1999/02/09 15:59:08  meichel
 *  Implemented bitmap shutter activation amd method for
 *    exchanging graphic layers.
 *
 *  Revision 1.4  1998/12/23 14:02:26  meichel
 *  Updated for changed interfaces in dcmimage overlays.
 *    Fixed bug affecting overlay origin delivered to dcmimage.
 *
 *  Revision 1.3  1998/12/22 17:57:17  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *  Revision 1.2  1998/12/14 16:10:44  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:44  meichel
 *  Initial Release.
 *
 *
 */

