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
 *    classes: DVPSDisplayedArea
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:32 $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmpstat/dvpsda.h"
#include "dcmtk/dcmpstat/dvpsri.h"      /* for DVPSReferencedImage */
#include "dcmtk/dcmpstat/dvpsrsl.h"     /* DVPSReferencedSeries_PList */
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmpstat/dvpsrs.h"      /* for DVPSReferencedSeries, needed by MSVC5 with STL */

#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

/* --------------- class DVPSDisplayedArea --------------- */

DVPSDisplayedArea::DVPSDisplayedArea()
: referencedImageList()
, displayedAreaTopLeftHandCorner(DCM_DisplayedAreaTopLeftHandCorner)
, displayedAreaBottomRightHandCorner(DCM_DisplayedAreaBottomRightHandCorner)
, presentationSizeMode(DCM_PresentationSizeMode)
, presentationPixelSpacing(DCM_PresentationPixelSpacing)
, presentationPixelAspectRatio(DCM_PresentationPixelAspectRatio)
, presentationPixelMagnificationRatio(DCM_PresentationPixelMagnificationRatio)
{
}

DVPSDisplayedArea::DVPSDisplayedArea(const DVPSDisplayedArea& copy)
: referencedImageList(copy.referencedImageList)
, displayedAreaTopLeftHandCorner(copy.displayedAreaTopLeftHandCorner)
, displayedAreaBottomRightHandCorner(copy.displayedAreaBottomRightHandCorner)
, presentationSizeMode(copy.presentationSizeMode)
, presentationPixelSpacing(copy.presentationPixelSpacing)
, presentationPixelAspectRatio(copy.presentationPixelAspectRatio)
, presentationPixelMagnificationRatio(copy.presentationPixelMagnificationRatio)
{
}

DVPSDisplayedArea::~DVPSDisplayedArea()
{
}

OFCondition DVPSDisplayedArea::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  OFString aString;
  
  READ_FROM_DATASET(DcmSignedLong, displayedAreaTopLeftHandCorner)
  READ_FROM_DATASET(DcmSignedLong, displayedAreaBottomRightHandCorner)
  READ_FROM_DATASET(DcmCodeString, presentationSizeMode)
  READ_FROM_DATASET(DcmDecimalString, presentationPixelSpacing)
  READ_FROM_DATASET(DcmIntegerString, presentationPixelAspectRatio)
  READ_FROM_DATASET(DcmFloatingPointSingle, presentationPixelMagnificationRatio)

  if (result==EC_Normal) result = referencedImageList.read(dset);

  /* Now perform basic sanity checks */

  if (displayedAreaTopLeftHandCorner.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a display area selection SQ item with displayedAreaTopLeftHandCorner absent or empty");
  }
  else if (displayedAreaTopLeftHandCorner.getVM() != 2)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a display area selection SQ item with displayedAreaTopLeftHandCorner VM != 2");
  }

  if (displayedAreaBottomRightHandCorner.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a display area selection SQ item with displayedAreaBottomRightHandCorner absent or empty");
  }
  else if (displayedAreaBottomRightHandCorner.getVM() != 2)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a display area selection SQ item with displayedAreaBottomRightHandCorner VM != 2");
  }

  if (presentationSizeMode.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a display area selection SQ item with presentationSizeMode absent or empty");
  }
  else if (presentationSizeMode.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a display area selection SQ item with presentationSizeMode VM != 1");
  } else {
    aString.clear();
    presentationSizeMode.getOFString(aString,0);
    if (aString=="TRUE SIZE")
    {
      if (presentationPixelSpacing.getVM() != 2)
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("presentation state contains a display area selection SQ item with mode 'TRUE SIZE' but presentationPixelSpacing VM != 2");
      }
    } else if (aString == "MAGNIFY")
    {
      if (presentationPixelMagnificationRatio.getVM() != 1)
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("presentation state contains a display area selection SQ item with mode 'MAGNIFY' but presentationPixelSpacing VM != 1");
      }
    } else if (aString != "SCALE TO FIT")
    {
      result=EC_IllegalCall;
      DCMPSTAT_INFO("presentation state contains a display area selection SQ item with unknown presentation size mode: " << aString);
    }
  }

  if ((presentationPixelSpacing.getLength() > 0)&&(presentationPixelSpacing.getVM() != 2))
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a display area selection SQ item with presentationPixelSpacing VM != 2");
}

  if ((presentationPixelAspectRatio.getLength() > 0)&&(presentationPixelAspectRatio.getVM() != 2))
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a display area selection SQ item with presentationPixelAspectRatio VM != 2");
  }

  if ((presentationPixelMagnificationRatio.getLength() > 0)&&(presentationPixelMagnificationRatio.getVM() != 1))
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a display area selection SQ item with presentationPixelMagnificationRatio VM != 1");
  }

  if ((presentationPixelSpacing.getLength() == 0)&&(presentationPixelAspectRatio.getVM() != 2))
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a display area selection SQ item with both presentationPixelSpacing and presentationPixelAspectRatio missing/incorrect");
  }

  return result;
}

OFCondition DVPSDisplayedArea::write(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmElement *delem=NULL;

  if ((presentationPixelSpacing.getLength()==0)&&(presentationPixelAspectRatio.getLength()==0)) 
    presentationPixelAspectRatio.putString("1\\1");

  ADD_TO_DATASET(DcmSignedLong, displayedAreaTopLeftHandCorner)
  ADD_TO_DATASET(DcmSignedLong, displayedAreaBottomRightHandCorner)
  ADD_TO_DATASET(DcmCodeString, presentationSizeMode)
  if (presentationPixelSpacing.getLength() >0) { ADD_TO_DATASET(DcmDecimalString, presentationPixelSpacing) }
  if (presentationPixelAspectRatio.getLength() >0) { ADD_TO_DATASET(DcmIntegerString, presentationPixelAspectRatio) }
  if (presentationPixelMagnificationRatio.getLength() >0) { ADD_TO_DATASET(DcmFloatingPointSingle, presentationPixelMagnificationRatio) }
  if ((result == EC_Normal)&&(referencedImageList.size() >0)) result = referencedImageList.write(dset);

  return result;
}

OFBool DVPSDisplayedArea::isApplicable(const char *instanceUID, unsigned long frame)
{
  return referencedImageList.isApplicable(instanceUID, frame);
}

OFBool DVPSDisplayedArea::matchesApplicability(const char *instanceUID, unsigned long frame, DVPSObjectApplicability applicability)
{
  return referencedImageList.matchesApplicability(instanceUID, frame, applicability);
}

void DVPSDisplayedArea::removeImageReference(
    DVPSReferencedSeries_PList& allReferences,
    const char *instanceUID,
    unsigned long frame, 
    unsigned long numberOfFrames, 
    DVPSObjectApplicability applicability)
{
  referencedImageList.removeImageReference(allReferences, instanceUID, frame, numberOfFrames, applicability);
  return;
}

DVPSPresentationSizeMode DVPSDisplayedArea::getPresentationSizeMode()
{
  OFString aString;
  presentationSizeMode.getOFString(aString,0);
  if (aString=="TRUE SIZE") return DVPSD_trueSize;
  else if (aString=="MAGNIFY") return DVPSD_magnify;
  return DVPSD_scaleToFit;
}
  
double DVPSDisplayedArea::getPresentationPixelAspectRatio()
{
  double result = 1.0; // default in case something goes wrong
  if (presentationPixelSpacing.getVM() == 2)
  {
    Float64 flX=0.0;
    Float64 flY=0.0;
    presentationPixelSpacing.getFloat64(flY, 0);
    presentationPixelSpacing.getFloat64(flX, 1);
    if (flY != 0.0) result = flX/flY;
  } 
  else if (presentationPixelAspectRatio.getVM() == 2)
  {
    Sint32 siX=0;
    Sint32 siY=0;
    presentationPixelAspectRatio.getSint32(siY, 0);
    presentationPixelAspectRatio.getSint32(siX, 1);
    if (siY != 0) result = ((double)siX) / ((double)siY);
  }
  return result;
}

void DVPSDisplayedArea::getDisplayedArea(Sint32& tlhcX, Sint32& tlhcY, Sint32& brhcX, Sint32& brhcY)
{
  if (EC_Normal != displayedAreaTopLeftHandCorner.getSint32(tlhcX, 0)) tlhcX = 0;
  if (EC_Normal != displayedAreaTopLeftHandCorner.getSint32(tlhcY, 1)) tlhcY = 0;
  if (EC_Normal != displayedAreaBottomRightHandCorner.getSint32(brhcX, 0)) brhcX = 0;
  if (EC_Normal != displayedAreaBottomRightHandCorner.getSint32(brhcY, 1)) brhcY = 0;
  return; 
}

OFCondition DVPSDisplayedArea::getPresentationPixelSpacing(double& x, double& y)
{
  if (presentationPixelSpacing.getVM() == 2)
  {
    Float64 psX=0.0;
    Float64 psY=0.0;
    // presentationPixelSpacing uses the format row_spacing\\col_spacing
    presentationPixelSpacing.getFloat64(psY, 0);
    presentationPixelSpacing.getFloat64(psX, 1); 
    x = psX;
    y = psY;
    return EC_Normal;
  }
  return EC_IllegalCall;
}

OFCondition DVPSDisplayedArea::getPresentationPixelMagnificationRatio(double& magnification)
{
  if (presentationPixelMagnificationRatio.getVM() == 1)
  {
    Float32 mag=0.0;
    presentationPixelMagnificationRatio.getFloat32(mag, 0);
    magnification = (double)mag;
    return EC_Normal;
  }
  return EC_IllegalCall;
}

OFCondition DVPSDisplayedArea::addImageReference(
    const char *sopclassUID,
    const char *instanceUID, 
    unsigned long frame,
    DVPSObjectApplicability applicability)
{
  return referencedImageList.addImageReference(sopclassUID, instanceUID, frame, applicability);
}

OFBool DVPSDisplayedArea::canUseTrueSize()
{
  if (presentationPixelSpacing.getVM() == 2) return OFTrue; else return OFFalse;
}

OFCondition DVPSDisplayedArea::setDisplayedAreaPixelSpacing(double spacingX, double spacingY)
{
  char str[66];
  OFStandard::ftoa(str, 32, spacingY, OFStandard::ftoa_format_f);
  strcat(str, "\\");
  OFStandard::ftoa(strchr(str, 0), 32, spacingX, OFStandard::ftoa_format_f);

  return setDisplayedAreaPixelSpacing(str);
}

OFCondition DVPSDisplayedArea::setDisplayedAreaPixelSpacing(const char *spacing)
{
  if (spacing==NULL) return EC_IllegalCall;
  presentationPixelAspectRatio.clear();
  OFCondition result = presentationPixelSpacing.putString(spacing);
  if (EC_Normal == result)
  {
    Float64 fl=0.0; 
    presentationPixelSpacing.getFloat64(fl, 0);
    if (fl == 0.0) result = EC_IllegalCall;
    else if (fl < 0) presentationPixelSpacing.putFloat64(-fl, 0); // silently correct negative values
    fl =0.0;
    presentationPixelSpacing.getFloat64(fl, 1);
    if (fl == 0.0) result = EC_IllegalCall;
    else if (fl < 0) presentationPixelSpacing.putFloat64(-fl, 1); // silently correct negative values
  }
  if (EC_Normal != result) presentationPixelSpacing.clear();
  return result;
}

OFCondition DVPSDisplayedArea::setDisplayedAreaPixelAspectRatio(double ratio)
{
  char str[100];
  sprintf(str, "10000\\%ld", (long)(ratio*10000.0));
  return setDisplayedAreaPixelAspectRatio(str);
}

OFCondition DVPSDisplayedArea::setDisplayedAreaPixelAspectRatio(const char *ratio)
{
  if (ratio==NULL) return EC_IllegalCall;
  presentationPixelSpacing.clear();

  OFCondition result = presentationPixelAspectRatio.putString(ratio);
  if (EC_Normal == result)
  {
    Sint32 si=0;
    presentationPixelAspectRatio.getSint32(si, 0);
    if (si == 0) result = EC_IllegalCall;
    else if (si < 0) presentationPixelAspectRatio.putSint32(-si, 0); // silently correct negative values
    si = 0;
    presentationPixelAspectRatio.getSint32(si, 1);
    if (si == 0) result = EC_IllegalCall;
    else if (si < 0) presentationPixelAspectRatio.putSint32(-si, 1); // silently correct negative values
  }
  if (EC_Normal != result) presentationPixelAspectRatio.clear();
  return result;
}

OFCondition DVPSDisplayedArea::setDisplayedArea(
    DVPSPresentationSizeMode sizeMode,
    Sint32 tlhcX, 
    Sint32 tlhcY, 
    Sint32 brhcX, 
    Sint32 brhcY,
    double magnification)
{
  OFCondition result = EC_Normal;
  switch (sizeMode)
  {
    case DVPSD_scaleToFit:
      result = presentationSizeMode.putString("SCALE TO FIT");
      if (EC_Normal == result) presentationPixelMagnificationRatio.clear();
      break;
    case DVPSD_trueSize:
      if (! canUseTrueSize()) return EC_IllegalCall;
      result = presentationSizeMode.putString("TRUE SIZE");
      if (EC_Normal == result) presentationPixelMagnificationRatio.clear();
      break;
    case DVPSD_magnify:
      if (magnification == 0.0) return EC_IllegalCall;
      result = presentationSizeMode.putString("MAGNIFY");
      if (EC_Normal == result) presentationPixelMagnificationRatio.clear();
      if (EC_Normal == result) result = presentationPixelMagnificationRatio.putFloat32((Float32)magnification,0);    
      break;
  }  
  if (EC_Normal == result) result = displayedAreaTopLeftHandCorner.putSint32(tlhcX,0);
  if (EC_Normal == result) result = displayedAreaTopLeftHandCorner.putSint32(tlhcY,1);
  if (EC_Normal == result) result = displayedAreaBottomRightHandCorner.putSint32(brhcX,0);
  if (EC_Normal == result) result = displayedAreaBottomRightHandCorner.putSint32(brhcY,1);
  return result;
}

void DVPSDisplayedArea::switchHorizontalCorners()
{
  Sint32 tlhcX = 0;
  Sint32 brhcX = 0;

  displayedAreaTopLeftHandCorner.getSint32(tlhcX, 0);
  displayedAreaBottomRightHandCorner.getSint32(brhcX, 0);

  displayedAreaTopLeftHandCorner.putSint32(brhcX,0);
  displayedAreaBottomRightHandCorner.putSint32(tlhcX,0);
}

void DVPSDisplayedArea::switchVerticalCorners()
{
  Sint32 tlhcY = 0;
  Sint32 brhcY = 0;

  displayedAreaTopLeftHandCorner.getSint32(tlhcY, 1);
  displayedAreaBottomRightHandCorner.getSint32(brhcY, 1);

  displayedAreaTopLeftHandCorner.putSint32(brhcY,1);
  displayedAreaBottomRightHandCorner.putSint32(tlhcY,1);
}

void DVPSDisplayedArea::rotateAndFlipFromOrTo(DVPSRotationType rotation, OFBool isFlipped)
{
  switch (rotation)
  {
    case DVPSR_0_deg:
      if (isFlipped) switchHorizontalCorners();
      break;
    case DVPSR_90_deg:
      if (!isFlipped) switchVerticalCorners();
      break;
    case DVPSR_180_deg:
      if (isFlipped) 
        switchVerticalCorners();
      else
      {
        switchHorizontalCorners();
        switchVerticalCorners();
      }
      break;
    case DVPSR_270_deg:
      if (isFlipped) 
      {
        switchHorizontalCorners();
        switchVerticalCorners();
      }
      else switchHorizontalCorners();
      break;
  }
}

void DVPSDisplayedArea::rotateAndFlip(
  DVPSRotationType rotationFrom, 
  OFBool isFlippedFrom,
  DVPSRotationType rotationTo, 
  OFBool isFlippedTo)
{
  // restore original coordinates (no rotation, no flipping)
  rotateAndFlipFromOrTo(rotationFrom, isFlippedFrom);

  // create new coordinates
  rotateAndFlipFromOrTo(rotationTo, isFlippedTo);
}



/*
 *  $Log: dvpsda.cc,v $
 *  Revision 1.16  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.15  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.14  2006-08-15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.13  2005/12/08 15:46:23  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.12  2003/09/05 14:30:08  meichel
 *  Introduced new API methods that allow Displayed Areas to be queried
 *    and set either relative to the image (ignoring rotation and flip) or
 *    in absolute values as defined in the standard.  Rotate and flip methods
 *    now adjust displayed areas in the presentation state.
 *
 *  Revision 1.11  2003/06/04 12:30:28  meichel
 *  Added various includes needed by MSVC5 with STL
 *
 *  Revision 1.10  2002/12/04 10:41:37  meichel
 *  Changed toolkit to use OFStandard::ftoa instead of sprintf for all
 *    double to string conversions that are supposed to be locale independent
 *
 *  Revision 1.9  2001/09/26 15:36:24  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.8  2001/06/01 15:50:29  meichel
 *  Updated copyright header
 *
 *  Revision 1.7  2000/11/23 09:47:24  meichel
 *  Silently correct negative pixel aspect ratio or pixel spacing values
 *
 *  Revision 1.6  2000/06/02 16:00:59  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.5  2000/05/31 13:02:36  meichel
 *  Moved dcmpstat macros and constants into a common header file
 *
 *  Revision 1.4  2000/03/08 16:29:03  meichel
 *  Updated copyright header.
 *
 *  Revision 1.3  2000/03/03 14:13:58  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.2  1999/10/22 09:08:22  joergr
 *  Added validity check to methods setting pixel aspect ratio and pixel
 *  spacing (>0). Fixed problems with incorrect pixel spacing (0\0) stored in
 *  sample images.
 *
 *  Revision 1.1  1999/07/22 16:39:55  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *
 */

