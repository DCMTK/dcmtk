/*
 *
 *  Copyright (C) 1998-2003, OFFIS
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
 *    classes: DVPresentationState
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2003-06-04 12:30:28 $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsga.h"
#include "dvpsri.h"
#include "dvpstyp.h"
#include "dvpsdef.h"     /* for constants and macros */
#include "dvpstx.h"      /* for DVPSTextObject, needed by MSVC5 with STL */
#include "dvpsgr.h"      /* for DVPSGraphicObject, needed by MSVC5 with STL */

/* --------------- class DVPSGraphicAnnotation --------------- */

DVPSGraphicAnnotation::DVPSGraphicAnnotation()
: referencedImageList()
, graphicAnnotationLayer(DCM_GraphicLayer)
, textObjectList()
, graphicObjectList()
, logstream(&ofConsole)
, verboseMode(OFFalse)
, debugMode(OFFalse)

{
}

DVPSGraphicAnnotation::DVPSGraphicAnnotation(const DVPSGraphicAnnotation& copy)
: referencedImageList(copy.referencedImageList)
, graphicAnnotationLayer(copy.graphicAnnotationLayer)
, textObjectList(copy.textObjectList)
, graphicObjectList(copy.graphicObjectList)
, logstream(copy.logstream)
, verboseMode(copy.verboseMode)
, debugMode(copy.debugMode)
{
}

DVPSGraphicAnnotation::~DVPSGraphicAnnotation()
{
}

OFCondition DVPSGraphicAnnotation::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;

  READ_FROM_DATASET(DcmCodeString, graphicAnnotationLayer)
  if (result==EC_Normal) result = textObjectList.read(dset);
  if (result==EC_Normal) result = graphicObjectList.read(dset);
  if (result==EC_Normal) result = referencedImageList.read(dset);
  
  /* Now perform basic sanity checks */

  if (graphicAnnotationLayer.getLength() == 0)
  {
    result=EC_IllegalCall;
    if (verboseMode)
    {
      logstream->lockCerr() << "Error: presentation state contains a graphic annotation SQ item with graphicAnnotationLayer absent or empty" << endl;
      logstream->unlockCerr();
    }
  }
  else if (graphicAnnotationLayer.getVM() != 1)
  {
    result=EC_IllegalCall;
    if (verboseMode)
    {
      logstream->lockCerr() << "Error: presentation state contains a graphic annotation SQ item with graphicAnnotationLayer VM != 1" << endl;
      logstream->unlockCerr();
    }
  }

  if ((textObjectList.size() ==0)&&(graphicObjectList.size() ==0))
  {
    result=EC_IllegalCall;
    if (verboseMode)
    {
      logstream->lockCerr() << "Error: presentation state contains a graphic annotation SQ item without any graphic or text objects" << endl;
      logstream->unlockCerr();
    }
  }
  
  return result;
}

OFCondition DVPSGraphicAnnotation::write(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmElement *delem=NULL;
  
  ADD_TO_DATASET(DcmCodeString, graphicAnnotationLayer)
  
  if (result == EC_Normal) result = textObjectList.write(dset);
  if (result == EC_Normal) result = graphicObjectList.write(dset);
  if ((result == EC_Normal)&&(referencedImageList.size() >0)) result = referencedImageList.write(dset);
  return result;
}

void DVPSGraphicAnnotation::setAnnotationLayer(const char *aLayer)
{
  if (aLayer) graphicAnnotationLayer.putString(aLayer); else graphicAnnotationLayer.clear();
  return;
}

const char *DVPSGraphicAnnotation::getAnnotationLayer()
{
  char *c = NULL;
  if (EC_Normal == graphicAnnotationLayer.getString(c)) return c; else return NULL;
}

OFBool DVPSGraphicAnnotation::isEmpty()
{
  if ((textObjectList.size() == 0)&&(graphicObjectList.size()==0)) return OFTrue;
  else return OFFalse;
}

size_t DVPSGraphicAnnotation::getNumberOfTextObjects()
{
  return textObjectList.size();
}

size_t DVPSGraphicAnnotation::getNumberOfGraphicObjects()
{
  return graphicObjectList.size();
}

DVPSTextObject *DVPSGraphicAnnotation::getTextObject(size_t idx)
{
  return textObjectList.getTextObject(idx);
}

DVPSGraphicObject *DVPSGraphicAnnotation::getGraphicObject(size_t idx)
{
  return graphicObjectList.getGraphicObject(idx);
}

void DVPSGraphicAnnotation::addTextObject(DVPSTextObject *text)
{
  textObjectList.addTextObject(text);
}

void DVPSGraphicAnnotation::addGraphicObject(DVPSGraphicObject *graphic)
{
  graphicObjectList.addGraphicObject(graphic);
}

DVPSTextObject *DVPSGraphicAnnotation::removeTextObject(size_t idx)
{
  return textObjectList.removeTextObject(idx);
}

DVPSGraphicObject *DVPSGraphicAnnotation::removeGraphicObject(size_t idx)
{
  return graphicObjectList.removeGraphicObject(idx);
}

OFCondition DVPSGraphicAnnotation::addImageReference(
    const char *sopclassUID,
    const char *instanceUID, 
    unsigned long frame,
    DVPSObjectApplicability applicability)
{
  if ((sopclassUID==NULL)||(instanceUID==NULL)||(applicability==DVPSB_allImages)) return EC_IllegalCall;
  const char *framenumber=NULL;
  if (applicability==DVPSB_currentFrame)
  {
    char frameString[100];
    sprintf(frameString, "%ld", frame);
    framenumber = frameString;
  }
  return referencedImageList.addImageReference(sopclassUID, instanceUID, framenumber);
}

OFBool DVPSGraphicAnnotation::isApplicable(    
    const char *instanceUID, 
    unsigned long frame,
    DVPSObjectApplicability applicability)
{
  if (referencedImageList.size() == 0) return OFTrue; // if no image references exist, the annotation is valid "globally".
  DVPSReferencedImage *imageRef = NULL;
  switch (applicability)
  {
    case DVPSB_currentFrame: // check if annotation applies to current frame
      imageRef = referencedImageList.findImageReference(instanceUID);
      if (imageRef) return imageRef->appliesToFrame(frame);
      break;
    case DVPSB_currentImage: // check if annotation applies to all frames of current image
      imageRef = referencedImageList.findImageReference(instanceUID);
      if (imageRef) return imageRef->appliesToAllFrames();
      break;
    default: /* do nothing */
      break;
  }
  return OFFalse;
}

void DVPSGraphicAnnotation::setLog(OFConsole *stream, OFBool verbMode, OFBool dbgMode)
{
  if (stream) logstream = stream; else logstream = &ofConsole;
  verboseMode = verbMode;
  debugMode = dbgMode;
  referencedImageList.setLog(logstream, verbMode, dbgMode);
  textObjectList.setLog(logstream, verbMode, dbgMode);
  graphicObjectList.setLog(logstream, verbMode, dbgMode);
}


/*
 *  $Log: dvpsga.cc,v $
 *  Revision 1.10  2003-06-04 12:30:28  meichel
 *  Added various includes needed by MSVC5 with STL
 *
 *  Revision 1.9  2001/09/26 15:36:25  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.8  2001/06/01 15:50:30  meichel
 *  Updated copyright header
 *
 *  Revision 1.7  2000/06/02 16:01:00  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.6  2000/05/31 13:02:36  meichel
 *  Moved dcmpstat macros and constants into a common header file
 *
 *  Revision 1.5  2000/03/08 16:29:04  meichel
 *  Updated copyright header.
 *
 *  Revision 1.4  2000/03/03 14:13:59  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.3  1999/07/22 16:39:57  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *  Revision 1.2  1998/12/14 16:10:40  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:40  meichel
 *  Initial Release.
 *
 *
 */

