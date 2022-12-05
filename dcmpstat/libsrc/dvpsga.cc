/*
 *
 *  Copyright (C) 1998-2022, OFFIS e.V.
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
 *    classes: DVPresentationState
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpsga.h"
#include "dcmtk/dcmpstat/dvpsri.h"
#include "dcmtk/dcmpstat/dvpstyp.h"
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */
#include "dcmtk/dcmpstat/dvpstx.h"      /* for DVPSTextObject, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsgr.h"      /* for DVPSGraphicObject, needed by MSVC5 with STL */

/* --------------- class DVPSGraphicAnnotation --------------- */

DVPSGraphicAnnotation::DVPSGraphicAnnotation()
: referencedImageList()
, graphicAnnotationLayer(DCM_GraphicLayer)
, textObjectList()
, graphicObjectList()
{
}

DVPSGraphicAnnotation::DVPSGraphicAnnotation(const DVPSGraphicAnnotation& copy)
: referencedImageList(copy.referencedImageList)
, graphicAnnotationLayer(copy.graphicAnnotationLayer)
, textObjectList(copy.textObjectList)
, graphicObjectList(copy.graphicObjectList)
{
}

DVPSGraphicAnnotation::~DVPSGraphicAnnotation()
{
}

OFCondition DVPSGraphicAnnotation::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;

  READ_FROM_DATASET(DcmCodeString, EVR_CS, graphicAnnotationLayer)
  if (result==EC_Normal) result = textObjectList.read(dset);
  if (result==EC_Normal) result = graphicObjectList.read(dset);
  if (result==EC_Normal) result = referencedImageList.read(dset);
  
  /* Now perform basic sanity checks */

  if (graphicAnnotationLayer.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a graphic annotation SQ item with graphicAnnotationLayer absent or empty");
  }
  else if (graphicAnnotationLayer.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a graphic annotation SQ item with graphicAnnotationLayer VM != 1");
  }

  if ((textObjectList.size() ==0)&&(graphicObjectList.size() ==0))
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a graphic annotation SQ item without any graphic or text objects");
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
  char frameString[24];
  const char *framenumber=NULL;
  if (applicability==DVPSB_currentFrame)
  {
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
