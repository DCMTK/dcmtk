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
 *    classes: DVPSDisplayedArea
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-03 14:13:58 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dvpsda.h"
#include "dvpsri.h"      /* for DVPSReferencedImage */
#include "dvpsrsl.h"     /* DVPSReferencedSeries_PList */

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

E_Condition DVPSDisplayedArea::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
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
#ifdef DEBUG
    CERR << "Error: presentation state contains a display area selection SQ item with displayedAreaTopLeftHandCorner absent or empty" << endl;
#endif
  }
  else if (displayedAreaTopLeftHandCorner.getVM() != 2)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a display area selection SQ item with displayedAreaTopLeftHandCorner VM != 2" << endl;
#endif
  }

  if (displayedAreaBottomRightHandCorner.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a display area selection SQ item with displayedAreaBottomRightHandCorner absent or empty" << endl;
#endif
  }
  else if (displayedAreaBottomRightHandCorner.getVM() != 2)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a display area selection SQ item with displayedAreaBottomRightHandCorner VM != 2" << endl;
#endif
  }

  if (presentationSizeMode.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a display area selection SQ item with presentationSizeMode absent or empty" << endl;
#endif
  }
  else if (presentationSizeMode.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a display area selection SQ item with presentationSizeMode VM != 1" << endl;
#endif
  } else {
    aString.clear();
    presentationSizeMode.getOFString(aString,0);
    if (aString=="TRUE SIZE")
    {
      if (presentationPixelSpacing.getVM() != 2)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        CERR << "Error: presentation state contains a display area selection SQ item with mode 'TRUE SIZE' but presentationPixelSpacing VM != 2" << endl;
#endif
      }
    } else if (aString == "MAGNIFY")
    {
      if (presentationPixelMagnificationRatio.getVM() != 1)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        CERR << "Error: presentation state contains a display area selection SQ item with mode 'MAGNIFY' but presentationPixelSpacing VM != 1" << endl;
#endif
      }
    } else if (aString != "SCALE TO FIT")
    {
      result=EC_IllegalCall;
#ifdef DEBUG
      CERR << "Error: presentation state contains a display area selection SQ item with unknown presentation size mode: " << aString << endl;
#endif
    }
  }

  if ((presentationPixelSpacing.getLength() > 0)&&(presentationPixelSpacing.getVM() != 2))
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a display area selection SQ item with presentationPixelSpacing VM != 2" << endl;
#endif
}

  if ((presentationPixelAspectRatio.getLength() > 0)&&(presentationPixelAspectRatio.getVM() != 2))
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a display area selection SQ item with presentationPixelAspectRatio VM != 2" << endl;
#endif
  }

  if ((presentationPixelMagnificationRatio.getLength() > 0)&&(presentationPixelMagnificationRatio.getVM() != 1))
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a display area selection SQ item with presentationPixelMagnificationRatio VM != 1" << endl;
#endif
  }

  if ((presentationPixelSpacing.getLength() == 0)&&(presentationPixelAspectRatio.getVM() != 2))
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    CERR << "Error: presentation state contains a display area selection SQ item with both presentationPixelSpacing and presentationPixelAspectRatio missing/incorrect" << endl;
#endif
  }

  return result;
}

E_Condition DVPSDisplayedArea::write(DcmItem &dset)
{
  E_Condition result = EC_Normal;
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

E_Condition DVPSDisplayedArea::getPresentationPixelSpacing(double& x, double& y)
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

E_Condition DVPSDisplayedArea::getPresentationPixelMagnificationRatio(double& magnification)
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

E_Condition DVPSDisplayedArea::addImageReference(
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

E_Condition DVPSDisplayedArea::setDisplayedAreaPixelSpacing(double spacingX, double spacingY)
{
  char str[200];
  sprintf(str, "%f\\%f", spacingY, spacingX);
  return setDisplayedAreaPixelSpacing(str);
}

E_Condition DVPSDisplayedArea::setDisplayedAreaPixelSpacing(const char *spacing)
{
  if (spacing==NULL) return EC_IllegalCall;
  presentationPixelAspectRatio.clear();
  E_Condition result = presentationPixelSpacing.putString(spacing);
  if (EC_Normal == result)
  {
    Float64 fl=0.0; 
    presentationPixelSpacing.getFloat64(fl, 0);
    if (fl == 0.0) result = EC_IllegalCall;
    fl =0.0;
    presentationPixelSpacing.getFloat64(fl, 1);
    if (fl == 0.0) result = EC_IllegalCall;
  }
  if (EC_Normal != result) presentationPixelSpacing.clear();
  return result;
}

E_Condition DVPSDisplayedArea::setDisplayedAreaPixelAspectRatio(double ratio)
{
  char str[100];
  sprintf(str, "10000\\%ld", (long)(ratio*10000.0));
  return setDisplayedAreaPixelAspectRatio(str);
}

E_Condition DVPSDisplayedArea::setDisplayedAreaPixelAspectRatio(const char *ratio)
{
  if (ratio==NULL) return EC_IllegalCall;
  presentationPixelSpacing.clear();

  E_Condition result = presentationPixelAspectRatio.putString(ratio);
  if (EC_Normal == result)
  {
    Sint32 si=0;
    presentationPixelAspectRatio.getSint32(si, 0);
    if (si == 0) result = EC_IllegalCall;
    si = 0;
    presentationPixelAspectRatio.getSint32(si, 1);
    if (si == 0) result = EC_IllegalCall;
  }
  if (EC_Normal != result) presentationPixelAspectRatio.clear();
  return result;
}

E_Condition DVPSDisplayedArea::setDisplayedArea(
    DVPSPresentationSizeMode sizeMode,
    Sint32 tlhcX, 
    Sint32 tlhcY, 
    Sint32 brhcX, 
    Sint32 brhcY,
    double magnification)
{
  E_Condition result = EC_Normal;
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


/*
 *  $Log: dvpsda.cc,v $
 *  Revision 1.3  2000-03-03 14:13:58  meichel
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

