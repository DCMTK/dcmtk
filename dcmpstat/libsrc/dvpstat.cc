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
 *    classes: DVPresentationState
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:33 $
 *  CVS/RCS Revision: $Revision: 1.86 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmpstat/dvpstat.h"
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmimgle/dcmimage.h"    /* for DicomImage */
#include "dcmtk/dcmpstat/dvpscu.h"      /* for DVPSCurve */
#include "dcmtk/dcmpstat/dvpsvl.h"      /* for DVPSVOILUT */
#include "dcmtk/dcmpstat/dvpsvw.h"      /* for DVPSVOIWindow */
#include "dcmtk/dcmpstat/dvpsov.h"      /* for DVPSOverlay */
#include "dcmtk/dcmpstat/dvpsda.h"      /* for DVPSDisplayedArea */
#include "dcmtk/dcmpstat/dvpssv.h"      /* for DVPSSoftcopyVOI */
#include "dcmtk/dcmpstat/dvpshlp.h"     /* for class DVPSHelper */
#include "dcmtk/ofstd/ofstd.h"       /* for class OFStandard */

#include "dcmtk/dcmpstat/dvpsgl.h"      /* for DVPSGraphicLayer, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsrs.h"      /* for DVPSReferencedSeries, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsal.h"      /* for DVPSOverlayCurveActivationLayer, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsga.h"      /* for DVPSGraphicAnnotation, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsri.h"      /* for DVPSReferencedImage, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpstx.h"      /* for DVPSTextObject, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsgr.h"      /* for DVPSGraphicObject, needed by MSVC5 with STL */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CMATH
#define INCLUDE_CTIME
#define INCLUDE_LIBC
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

/* --------------- class DVPresentationState --------------- */

DVPresentationState::DVPresentationState(
    DiDisplayFunction **dispFunction,
    unsigned long minPrintBitmapX,
    unsigned long minPrintBitmapY,
    unsigned long maxPrintBitmapX,
    unsigned long maxPrintBitmapY,
    unsigned long maxPreviewImageX,
    unsigned long maxPreviewImageY)
: DcmPresentationState()
, currentImageDataset(NULL)
, currentImageFileformat(NULL)
, currentImage(NULL)
, previewImage(NULL)
, currentImageWidth(0)
, currentImageHeight(0)
, renderedImageWidth(0)
, renderedImageHeight(0)
, renderedImageTop(0)
, renderedImageLeft(0)
, renderedImageBottom(0)
, renderedImageRight(0)
, currentImageSOPClassUID(NULL)
, currentImageSOPInstanceUID(NULL)
, currentImageSelectedFrame(0)
, currentImageOwned(OFFalse)
, currentImageVOIValid(OFFalse)
, currentImagePLUTValid(OFFalse)
, currentImageFlip(OFFalse)
, currentImageRotation(DVPSR_0_deg)
, currentImageOverlaysValid(0)
, currentImageCurveList()
, currentImageVOILUTList()
, currentImageVOIWindowList()
, currentImageModality(DCM_Modality)
, currentImageMonochrome1(OFFalse)
, displayTransform(DVPSD_GSDF)
, imageInverse(OFFalse)
, displayFunction(dispFunction)
, minimumPrintBitmapWidth(minPrintBitmapX)
, minimumPrintBitmapHeight(minPrintBitmapY)
, maximumPrintBitmapWidth(maxPrintBitmapX)
, maximumPrintBitmapHeight(maxPrintBitmapY)
, maximumPreviewImageWidth(maxPreviewImageX)
, maximumPreviewImageHeight(maxPreviewImageY)
{
  createInstanceUID();
}


DVPresentationState::~DVPresentationState()
{
  detachImage();
}

void DVPresentationState::detachImage()
{
  if (currentImage) delete currentImage;
  deletePreviewImage();
  if (currentImageOwned)
  {
    if (currentImageFileformat) delete currentImageFileformat;
    else if (currentImageDataset) delete currentImageDataset;
  }
  currentImage = NULL;
  currentImageFileformat = NULL;
  currentImageDataset = NULL;
  currentImageCurveList.clear();
  currentImageVOILUTList.clear();
  currentImageVOIWindowList.clear();
  currentImageModality.clear();
  currentImageMonochrome1 = OFFalse;

  // reset flags
  currentImageWidth = 0;
  currentImageHeight = 0;
  renderedImageWidth = 0;
  renderedImageHeight = 0;
  renderedImageTop = 0;
  renderedImageLeft = 0;
  renderedImageBottom = 0;
  renderedImageRight = 0;
  currentImageOwned = OFFalse;
  currentImageVOIValid = OFFalse;
  currentImagePLUTValid = OFFalse;
  currentImageFlip = OFFalse;
  currentImageRotation = DVPSR_0_deg;
  currentImageOverlaysValid = 0;
  currentImageSOPClassUID=NULL;
  currentImageSOPInstanceUID=NULL;
  currentImageSelectedFrame=0;
}


void DVPresentationState::clear()
{
  DcmPresentationState::clear();
  detachImage(); // clears all currentImageXX attributes
  // we do not change the display function
  displayTransform = DVPSD_GSDF;
  imageInverse = OFFalse;
  return;
}


OFCondition DVPresentationState::writeHardcopyImageAttributes(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmElement *delem=NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  DVPSHelper::setDefault(result, patientName, DEFAULT_patientName);
  ADD_TO_DATASET(DcmPersonName, patientName)
  ADD_TO_DATASET(DcmLongString, patientID)
  ADD_TO_DATASET(DcmDate, patientBirthDate)
  ADD_TO_DATASET(DcmCodeString, patientSex)

  // write source image sequence
  if (result == EC_Normal)
  {
    dseq = new DcmSequenceOfItems(DCM_SourceImageSequence);
    if (dseq)
    {
      // first item references source image
      if (currentImageSOPClassUID && currentImageSOPInstanceUID)
      {
        ditem = new DcmItem();
        if (ditem)
        {
          delem = new DcmUniqueIdentifier(DCM_SOPClassUID);
          if (delem)
          {
            result = delem->putString(currentImageSOPClassUID);
            ditem->insert(delem, OFTrue /*replaceOld*/);
          } else result=EC_MemoryExhausted;
          delem = new DcmUniqueIdentifier(DCM_SOPInstanceUID);
          if (delem)
          {
            result = delem->putString(currentImageSOPInstanceUID);
            ditem->insert(delem, OFTrue /*replaceOld*/);
          } else result=EC_MemoryExhausted;
          if (EC_Normal == result) dseq->insert(ditem);  else delete ditem;
        } else result = EC_MemoryExhausted;
      }
      // second item references presentation state
      ditem = new DcmItem();
      if (ditem)
      {
        delem = new DcmUniqueIdentifier(sOPInstanceUID);
        if (delem) ditem->insert(delem, OFTrue /*replaceOld*/); else result=EC_MemoryExhausted;
        delem = new DcmUniqueIdentifier(DCM_SOPClassUID);
        if (delem)
        {
          result = delem->putString(UID_GrayscaleSoftcopyPresentationStateStorage);
          ditem->insert(delem, OFTrue /*replaceOld*/);
        } else result=EC_MemoryExhausted;
        if (EC_Normal == result) dseq->insert(ditem);  else delete ditem;
       }
    } else result = EC_MemoryExhausted;
    if (EC_Normal == result) dset.insert(dseq, OFTrue /*replaceOld*/); else delete dseq;
  }

  return result;
}

unsigned long DVPresentationState::getPrintBitmapSize()
{
  unsigned long result = 0;
  unsigned long width;
  unsigned long height;
  if (getPrintBitmapWidthHeight(width, height) == EC_Normal)
    result = width * height * 2;       // print bitmap: 12 bit stored, 16 bit allocated (2 bytes per pixel)
  return result;
}


OFCondition DVPresentationState::setMinimumPrintBitmapWidthHeight(unsigned long width,
                                                                  unsigned long height)
{
  OFCondition result = EC_IllegalCall;
  const unsigned long max = (width > height) ? width : height;
  if (((maximumPrintBitmapWidth == 0) || (maximumPrintBitmapWidth >= 2 * max)) &&
      ((maximumPrintBitmapHeight == 0) || (maximumPrintBitmapHeight >= 2 * max)))
  {
    minimumPrintBitmapWidth = width;
    minimumPrintBitmapHeight = height;
    result = EC_Normal;
  }
  return result;
}


OFCondition DVPresentationState::setMaximumPrintBitmapWidthHeight(unsigned long width,
                                                                  unsigned long height)
{
  OFCondition result = EC_IllegalCall;
  const unsigned long min = (width < height) ? width : height;
  if (((minimumPrintBitmapWidth == 0) || (min >= 2 * minimumPrintBitmapWidth)) &&
      ((minimumPrintBitmapHeight == 0) || (min >= 2 * minimumPrintBitmapHeight)))
  {
    maximumPrintBitmapWidth = width;
    maximumPrintBitmapHeight = height;
    result = EC_Normal;
  }
  return result;
}


OFCondition DVPresentationState::getPrintBitmapWidthHeight(unsigned long &width,
                                                           unsigned long &height)
{
  OFCondition result = EC_Normal;
  if (currentImage)
  {
    renderPixelData(OFFalse);                                 // switch off display function
    width = renderedImageWidth;
    height = renderedImageHeight;
    if ((width > 0) && (height > 0))
    {
      width = (unsigned long)(renderedImageRight - renderedImageLeft + 1);
      height = (unsigned long)(renderedImageBottom - renderedImageTop + 1);

      if ((minimumPrintBitmapWidth > 0) && (width < minimumPrintBitmapWidth))
      {
        if ((minimumPrintBitmapHeight > 0) && (height < minimumPrintBitmapHeight))
        {
          const unsigned long xfactor = (unsigned long)((double)(minimumPrintBitmapWidth - 1) / (double)width) + 1;
          const unsigned long yfactor = (unsigned long)((double)(minimumPrintBitmapHeight - 1) / (double)height) + 1;
          if (xfactor > yfactor)
          {
            width *= xfactor;
            height *= xfactor;
          } else {
            width *= yfactor;
            height *= yfactor;
          }
        } else {
          const unsigned long factor = (unsigned long)((double)(minimumPrintBitmapWidth - 1) / (double)width) + 1;
          width *= factor;
          height *= factor;
        }
      }
      else if ((minimumPrintBitmapHeight > 0) && (height < minimumPrintBitmapHeight))
      {
        const unsigned long factor = (unsigned long)((double)(minimumPrintBitmapHeight - 1) / (double)height) + 1;
        width *= factor;
        height *= factor;
      }

      if ((maximumPrintBitmapWidth > 0) && (width > maximumPrintBitmapWidth))
      {
        if ((maximumPrintBitmapHeight > 0) && (height > maximumPrintBitmapHeight))
        {
          const unsigned long xdivisor = (unsigned long)((double)(width - 1) / (double)maximumPrintBitmapWidth) + 1;
          const unsigned long ydivisor = (unsigned long)((double)(height - 1) / (double)maximumPrintBitmapHeight) + 1;
          if (xdivisor > ydivisor)
          {
            width /= xdivisor;
            height /= xdivisor;
          } else {
            width /= ydivisor;
            height /= ydivisor;
          }
        } else {
          const unsigned long divisor = (unsigned long)((double)(width - 1) / (double)maximumPrintBitmapWidth) + 1;
          width /= divisor;
          height /= divisor;
        }
      }
      else if ((maximumPrintBitmapHeight > 0) && (height > maximumPrintBitmapHeight))
      {
        const unsigned long divisor = (unsigned long)((double)(height - 1) / (double)maximumPrintBitmapHeight) + 1;
        width /= divisor;
        height /= divisor;
      }
    }
  } else {
    width = 0;
    height = 0;
    result = EC_IllegalCall;
  }
  return result;
}


OFCondition DVPresentationState::getPrintBitmapWidth(unsigned long &width)
{
  unsigned long dummy;
  return getPrintBitmapWidthHeight(width, dummy);
}


OFCondition DVPresentationState::getPrintBitmapHeight(unsigned long &height)
{
  unsigned long dummy;
  return getPrintBitmapWidthHeight(dummy, height);
}


double DVPresentationState::getPrintBitmapPixelAspectRatio()
{
  double result = getDisplayedAreaPresentationPixelAspectRatio();
  if (result == 1.0) return result; // handle most frequent case quickly
  if (result == 0.0) result = 1.0; // should never happen

  DVPSRotationType rotation = getRotation();
  if ((rotation==DVPSR_90_deg)||(rotation==DVPSR_270_deg)) result = 1.0/result;
  return result;
}


OFCondition DVPresentationState::getPrintBitmap(void *bitmap,
                                                unsigned long size,
                                                OFBool inversePLUT)
{
  OFCondition result = EC_IllegalCall;
  if ((bitmap != NULL) && (size == getPrintBitmapSize()))       // check given buffer
  {
    if (currentImage)
    {
      renderPixelData(OFFalse);                                 // don't use current display function
      unsigned long width;
      unsigned long height;
      if (getPrintBitmapWidthHeight(width, height) == EC_Normal)
      {
        DicomImage *image = currentImage;

        /* we deactivate any presentation LUT at this point because
         * getPrintBitmapWidthHeight() calls renderPixelData().
         */
        if (presentationLUT.getType() == DVPSP_table)
        {
          if (inversePLUT)
          {
            if (currentImageMonochrome1) currentImage->setPolarity(EPP_Reverse);
            presentationLUT.activateInverseLUT(currentImage);
          } else {
            // we never render a presentation LUT into the print bitmap at this stage.
            if (currentImageMonochrome1) currentImage->setPresentationLutShape(ESP_Inverse);
            else currentImage->setPresentationLutShape(ESP_Identity);
          }
          // make sure the presentation LUT is re-activated for on-screen display
          currentImagePLUTValid = OFFalse;
        }

        /* clip to displayed area if necessary */
        if ((renderedImageLeft != 1) || (renderedImageRight != (signed long)renderedImageWidth) ||
            (renderedImageTop != 1) || (renderedImageBottom != (signed long)renderedImageHeight))
        {
          DicomImage *img = currentImage->createMonoOutputImage(currentImageSelectedFrame-1, 12 /*bits*/);
          if (img == NULL)
            img = currentImage;                                 // fall-back solution
          image = img->createClippedImage(renderedImageLeft - 1, renderedImageTop - 1, renderedImageRight - renderedImageLeft + 1,
            renderedImageBottom - renderedImageTop + 1, getShutterPresentationValue());
          if (img != currentImage)
            delete img;
        }
        /* scale up to minimum size or down to maximum size if necessary */
        if (((signed long)width != renderedImageRight - renderedImageLeft + 1) ||
           ((signed long)height != renderedImageBottom - renderedImageTop + 1))
        {
          DicomImage *img = image;
          image = img->createScaledImage(width, height, 0 /*no interpolation*/, 0 /*ignore aspect ratio*/);
          if (img != currentImage)
            delete img;
        }
        if (image != NULL)
        {
          if (image->getOutputData(bitmap, size, 12 /*bits*/, currentImageSelectedFrame-1)) result = EC_Normal;
        }
        if (image != currentImage) delete image;
      }
    }
  }
  return result;
}


OFCondition DVPresentationState::createPreviewImage(unsigned long maxWidth,
                                                    unsigned long maxHeight,
                                                    OFBool clipMode)
{
  OFCondition result = EC_IllegalCall;
  if ((currentImage != NULL) && (maxWidth > 0) && (maxHeight > 0))
  {
    deletePreviewImage();
    renderPixelData();
    unsigned long width = maxWidth;
    unsigned long height = maxHeight;
    double ratio = getPrintBitmapPixelAspectRatio();    // never 0 !
    if ((double)renderedImageWidth / (double)maxWidth * ratio < (double)renderedImageHeight / (double)maxHeight)
      width = 0;
    else
      height = 0;
    if (clipMode)
    {
      /* not yet implemented: clip preview image to current displayed area */
    }
    double oldRatio = currentImage->getWidthHeightRatio();
    currentImage->setWidthHeightRatio(ratio);
    previewImage = currentImage->createScaledImage(width, height, 1 /*interpolate*/, 1 /*aspect ratio*/);
    currentImage->setWidthHeightRatio(oldRatio);
    if (previewImage != NULL)
    {
      if (previewImage->getStatus() == EIS_Normal)
      {
        previewImage->removeAllOverlays();
        result = EC_Normal;
      } else {
        deletePreviewImage();
      }
    }
  }
  return result;
}


void DVPresentationState::deletePreviewImage()
{
  delete previewImage;
  previewImage = NULL;
}


unsigned long DVPresentationState::getPreviewImageSize()
{
  unsigned long result = 0;
  unsigned long width;
  unsigned long height;
  if (getPreviewImageWidthHeight(width, height) == EC_Normal)
      result = width * height;
  return result;
}


OFCondition DVPresentationState::getPreviewImageWidthHeight(unsigned long &width,
                                                            unsigned long &height)
{
  OFCondition result = EC_IllegalCall;
  if (previewImage != NULL)
  {
    width = previewImage->getWidth();
    height = previewImage->getHeight();
    if ((width > 0) && (height > 0))
      result = EC_Normal;
  } else {
    width = 0;
    height = 0;
  }
  return result;
}


OFCondition DVPresentationState::getPreviewImageWidth(unsigned long &width)
{
  unsigned long dummy;
  return getPreviewImageWidthHeight(width, dummy);
}


OFCondition DVPresentationState::getPreviewImageHeight(unsigned long &height)
{
  unsigned long dummy;
  return getPreviewImageWidthHeight(dummy, height);
}


OFCondition DVPresentationState::getPreviewImageBitmap(void *bitmap,
                                                       unsigned long size)
{
  OFCondition result = EC_IllegalCall;
  if ((previewImage != NULL) && (bitmap != NULL) && (size > 0))
  {
    renderPixelData();
    if (previewImage->getOutputData(bitmap, size, 8 /*bits*/, currentImageSelectedFrame-1))
      result = EC_Normal;
  }
  return result;
}


OFCondition DVPresentationState::attachImage(DcmDataset *dataset, OFBool transferOwnership)
{
  if (!dataset) return EC_IllegalCall;

  // select the right DicomImage constructor depending on the Modality LUT
  DicomImage *image = NULL;
  if (useModalityRescale)
  {
    Float64 slope = 1.0;
    Float64 intercept = 0.0;

    if (EC_Normal != rescaleSlope.getFloat64(slope, 0))
    {
      DCMPSTAT_INFO("unable to evaluate Modality Rescale Slope, ignoring.");
      slope = 1.0;
    }
    if (EC_Normal != rescaleIntercept.getFloat64(intercept, 0))
    {
      DCMPSTAT_INFO("unable to evaluate Modality Rescale Slope, ignoring.");
      intercept = 0.0;
    }
    image = new DicomImage(dataset, dataset->getOriginalXfer(),
      slope, intercept, CIF_UsePresentationState);
  }
  else if (useModalityLUT)
  {
    image = new DicomImage(dataset, dataset->getOriginalXfer(),
      modalityLUTData, modalityLUTDescriptor, &modalityLUTExplanation, CIF_UsePresentationState);
  }
  else
  {
    image = new DicomImage(dataset, dataset->getOriginalXfer(), CIF_UsePresentationState);
  }

  OFCondition result = EC_Normal;
  if (image)
  {
    if (EIS_Normal == image->getStatus())
    {
      OFString aString;
      DcmStack stack;
      detachImage();
      currentImage = image;
      currentImageWidth = image->getWidth();
      currentImageHeight = image->getHeight();
      currentImageDataset = dataset;
      currentImageOwned = transferOwnership;
      currentImageSelectedFrame = 1; // default: first frame

      // get Modality
      if (EC_Normal == dataset->search(DCM_Modality, stack, ESM_fromHere, OFFalse))
      {
        currentImageModality = *((DcmCodeString *)(stack.top()));
      }
      stack.clear();

      // determine default Presentation LUT Shape
      if (EC_Normal == dataset->search(DCM_PhotometricInterpretation, stack, ESM_fromHere, OFFalse))
      {
         DcmCodeString *photometricInterpretation = (DcmCodeString *)(stack.top());
         if (photometricInterpretation->getVM() == 1)
         {
           aString.clear();
           photometricInterpretation->getOFString(aString,0);
           if ((aString == "MONOCHROME1")||(aString == "MONOCHROME 1")) currentImageMonochrome1 = OFTrue;
         }
      }
      stack.clear();

      // get SOP class UID and SOP instance UID.
      if ((EC_Normal == result)&&(EC_Normal == dataset->search(DCM_SOPClassUID, stack, ESM_fromHere, OFFalse)))
      {
        result = ((DcmUniqueIdentifier *)(stack.top()))->getString(currentImageSOPClassUID);
      }
      stack.clear();
      if ((EC_Normal == result)&&(EC_Normal == dataset->search(DCM_SOPInstanceUID, stack, ESM_fromHere, OFFalse)))
      {
        result = ((DcmUniqueIdentifier *)(stack.top()))->getString(currentImageSOPInstanceUID);
      }
      if (EC_Normal==result) result = currentImageCurveList.read(*dataset);
      if (EC_Normal==result) result = currentImageVOILUTList.read(*dataset);
      if (EC_Normal==result) result = currentImageVOIWindowList.read(*dataset);

      createPreviewImage(maximumPreviewImageWidth, maximumPreviewImageHeight);
    } else {
      delete image;
      result = EC_IllegalCall;
    }
  } else result = EC_MemoryExhausted;
  return result;
}

OFCondition DVPresentationState::getImageWidth(unsigned long &width)
{
  OFCondition result=EC_Normal;
  if (currentImage) width = currentImageWidth;
  else
  {
    width = 0;
    result = EC_IllegalCall;
  }
  return result;
}

OFCondition DVPresentationState::getImageHeight(unsigned long &height)
{
  OFCondition result=EC_Normal;
  if (currentImage) height = currentImageHeight;
  else
  {
    height = 0;
    result = EC_IllegalCall;
  }
  return result;
}


OFCondition DVPresentationState::attachImage(DcmFileFormat *fileformat, OFBool transferOwnership)
{
  if (fileformat == NULL) return EC_IllegalCall;
  OFCondition result = attachImage(fileformat->getDataset(), transferOwnership);
  if (EC_Normal == result) currentImageFileformat = fileformat;
  return result;
}


OFCondition DVPresentationState::addImageReferenceAttached(
    const char *aetitle,
    const char *filesetID,
    const char *filesetUID)
{
  if (currentImageDataset) return addImageReference(*currentImageDataset, aetitle, filesetID, filesetUID);
  else return EC_IllegalCall;
}


OFCondition DVPresentationState::removeImageReferenceAttached()
{
  if (currentImageDataset) return removeImageReference(*currentImageDataset);
  else return EC_IllegalCall;
}



OFCondition DVPresentationState::setCurrentPresentationLUT(DVPSPresentationLUTType newType)
{
  OFCondition result = presentationLUT.setType(newType);
  currentImagePLUTValid = OFFalse; // PLUT has changed
  imageInverse = presentationLUT.isInverse();
  return result;
}

OFCondition DVPresentationState::setDefaultPresentationLUTShape()
{
  OFCondition result = EC_Normal;
  if (currentImageMonochrome1) result=presentationLUT.setType(DVPSP_inverse); else result=presentationLUT.setType(DVPSP_identity);
  currentImagePLUTValid = OFFalse; // PLUT has changed
  imageInverse = presentationLUT.isInverse();
  return result;
}

OFCondition DVPresentationState::setPresentationLookupTable(
    DcmUnsignedShort& lutDescriptor,
    DcmUnsignedShort& lutData,
    DcmLongString& lutExplanation)
{
  OFCondition result = presentationLUT.setLUT(lutDescriptor, lutData, lutExplanation);
  currentImagePLUTValid = OFFalse; // PLUT has changed
  OFBool wasInverse = imageInverse;
  imageInverse = presentationLUT.isInverse();

  // keep inverse/normal status as before
  if ((wasInverse && (! imageInverse))||(imageInverse && (! wasInverse))) result = invertImage();
  return result;
}

OFCondition DVPresentationState::setPresentationLookupTable(DcmItem &dset)
{
  OFCondition result = presentationLUT.read(dset, OFFalse);
  if (EC_Normal != result) presentationLUT.setType(DVPSP_identity); // set to well-defined default in case of error
  currentImagePLUTValid = OFFalse; // PLUT has changed
  OFBool wasInverse = imageInverse;
  imageInverse = presentationLUT.isInverse();

  // keep inverse/normal status as before
  if ((wasInverse && (! imageInverse))||(imageInverse && (! wasInverse))) result = invertImage();
  return result;
}


void DVPresentationState::removeShutter(DVPSShutterType type)
{
  switch (type)
  {
    case DVPSU_rectangular:
      useShutterRectangular = OFFalse;
      break;
    case DVPSU_circular:
      useShutterCircular = OFFalse;
      break;
    case DVPSU_polygonal:
      useShutterPolygonal = OFFalse;
      break;
    case DVPSU_bitmap:
      if (useShutterBitmap) currentImageOverlaysValid = 1; // invalid but nothing added
      useShutterBitmap = OFFalse;
      break;
  }
  return;
}


OFCondition DVPresentationState::setRectShutter(Sint32 lv, Sint32 rv, Sint32 uh, Sint32 lh)
{
  OFCondition result=EC_Normal;
  char buf[80];

  sprintf(buf, "%ld", (long)lv);
  result = shutterLeftVerticalEdge.putString(buf);
  sprintf(buf, "%ld", (long)rv);
  if (EC_Normal==result) result = shutterRightVerticalEdge.putString(buf);
  sprintf(buf, "%ld", (long)uh);
  if (EC_Normal==result) result = shutterUpperHorizontalEdge.putString(buf);
  sprintf(buf, "%ld", (long)lh);
  if (EC_Normal==result) result = shutterLowerHorizontalEdge.putString(buf);
  if ((EC_Normal==result)&&(shutterPresentationValue.getLength()==0))
      result = shutterPresentationValue.putUint16(0,0);
  if (EC_Normal==result)
  {
    useShutterRectangular = OFTrue;
    if (useShutterBitmap) currentImageOverlaysValid = 1; // invalid but nothing added
    useShutterBitmap = OFFalse;
  }
  return result;
}


OFCondition DVPresentationState::setCircularShutter(Sint32 centerX, Sint32 centerY, Sint32 radius)
{
  OFCondition result=EC_Normal;
  char buf[80];

  sprintf(buf, "%ld\\%ld", (long)centerY, (long)centerX);
  result = centerOfCircularShutter.putString(buf);
  sprintf(buf, "%ld", (long)radius);
  if (EC_Normal==result) result = radiusOfCircularShutter.putString(buf);
  if ((EC_Normal==result)&&(shutterPresentationValue.getLength()==0))
      result = shutterPresentationValue.putUint16(0,0);
  if (EC_Normal==result)
  {
    useShutterCircular = OFTrue;
    if (useShutterBitmap) currentImageOverlaysValid = 1; // invalid but nothing added
    useShutterBitmap = OFFalse;
  }
  return result;
}


OFCondition DVPresentationState::addPolyShutterVertex(Sint32 x, Sint32 y)
{
  if (verticesOfThePolygonalShutter.getLength()==0) return EC_IllegalCall;
  OFString aString;
  OFCondition result = verticesOfThePolygonalShutter.getOFStringArray(aString,OFTrue);
  if (result==EC_Normal)
  {
    char buf[80];
    sprintf(buf, "\\%ld\\%ld", (long)y, (long)x);
    aString += buf;
    result = verticesOfThePolygonalShutter.putOFStringArray(aString);
  }

  if (result==EC_Normal)
  {
    Sint32 xp0 = 0;
    Sint32 yp0 = 0;
    result = getPolyShutterVertex(0, xp0, yp0);
    if (result==EC_Normal)
    {
      if ((xp0==x)&&(yp0==y)) // polygon is closed now, activate.
      {
        if (shutterPresentationValue.getLength()==0)
           result = shutterPresentationValue.putUint16(0,0);
        if (EC_Normal==result)
        {
          useShutterPolygonal = OFTrue;
          if (useShutterBitmap) currentImageOverlaysValid = 1; // invalid but nothing added
          useShutterBitmap = OFFalse;
        }
      }
    }
  }
  return result;
}

OFCondition DVPresentationState::removeGraphicLayer(size_t idx)
{
  const char *name = graphicLayerList.getGraphicLayerName(idx);
  if (name==NULL) return EC_IllegalCall;
  activationLayerList.removeLayer(name);
  currentImageOverlaysValid = 1; // invalid but nothing added
  graphicAnnotationList.removeLayer(name);
  return graphicLayerList.removeGraphicLayer(idx);
}

size_t DVPresentationState::getNumberOfTextObjects(size_t layer)
{
  if (!currentImage) return 0;
  return graphicAnnotationList.getNumberOfTextObjects(graphicLayerList.getGraphicLayerName(layer), currentImageSOPInstanceUID, currentImageSelectedFrame);
}

DVPSTextObject *DVPresentationState::getTextObject(size_t layer, size_t idx)
{
  if (!currentImage) return NULL;
  return graphicAnnotationList.getTextObject(graphicLayerList.getGraphicLayerName(layer), currentImageSOPInstanceUID, currentImageSelectedFrame, idx);
}

DVPSTextObject *DVPresentationState::addTextObject(size_t layer, DVPSObjectApplicability applicability)
{
  if (!currentImage) return NULL;
  return graphicAnnotationList.addTextObject(graphicLayerList.getGraphicLayerName(layer), currentImageSOPClassUID,
    currentImageSOPInstanceUID, currentImageSelectedFrame, applicability);
}

OFCondition DVPresentationState::removeTextObject(size_t layer, size_t idx)
{
  if (!currentImage) return EC_IllegalCall;
  return graphicAnnotationList.removeTextObject(graphicLayerList.getGraphicLayerName(layer), currentImageSOPInstanceUID, currentImageSelectedFrame, idx);
}

OFCondition DVPresentationState::moveTextObject(size_t old_layer, size_t idx, size_t new_layer, DVPSObjectApplicability applicability)
{
  if (!currentImage) return EC_IllegalCall;
  if (old_layer==new_layer) return EC_Normal;
  return graphicAnnotationList.moveTextObject(
    graphicLayerList.getGraphicLayerName(old_layer),
    currentImageSOPClassUID,
    currentImageSOPInstanceUID,
    currentImageSelectedFrame,
    idx,
    applicability,
    graphicLayerList.getGraphicLayerName(new_layer));
}

size_t DVPresentationState::getNumberOfGraphicObjects(size_t layer)
{
  if (!currentImage) return 0;
  return graphicAnnotationList.getNumberOfGraphicObjects(graphicLayerList.getGraphicLayerName(layer), currentImageSOPInstanceUID, currentImageSelectedFrame);
}

DVPSGraphicObject *DVPresentationState::getGraphicObject(size_t layer, size_t idx)
{
  if (!currentImage) return NULL;
  return graphicAnnotationList.getGraphicObject(graphicLayerList.getGraphicLayerName(layer), currentImageSOPInstanceUID, currentImageSelectedFrame, idx);
}

DVPSGraphicObject *DVPresentationState::addGraphicObject(size_t layer, DVPSObjectApplicability applicability)
{
  if (!currentImage) return NULL;
  return graphicAnnotationList.addGraphicObject(graphicLayerList.getGraphicLayerName(layer), currentImageSOPClassUID,
    currentImageSOPInstanceUID, currentImageSelectedFrame, applicability);
}

OFCondition DVPresentationState::removeGraphicObject(size_t layer, size_t idx)
{
  if (!currentImage) return EC_IllegalCall;
  return graphicAnnotationList.removeGraphicObject(graphicLayerList.getGraphicLayerName(layer), currentImageSOPInstanceUID, currentImageSelectedFrame, idx);
}

OFCondition DVPresentationState::moveGraphicObject(size_t old_layer, size_t idx, size_t new_layer, DVPSObjectApplicability applicability)
{
  if (!currentImage) return EC_IllegalCall;
  if (old_layer==new_layer) return EC_Normal;
  return graphicAnnotationList.moveGraphicObject(
    graphicLayerList.getGraphicLayerName(old_layer),
    currentImageSOPClassUID,
    currentImageSOPInstanceUID,
    currentImageSelectedFrame,
    idx,
    applicability,
    graphicLayerList.getGraphicLayerName(new_layer));
}


DVPSCurve *DVPresentationState::getCurve(size_t layer, size_t idx)
{
  Uint16 rgroup = activationLayerList.getActivationGroup(
    graphicLayerList.getGraphicLayerName(layer), idx, OFTrue);
  if (rgroup==0) return NULL;
  else return currentImageCurveList.getCurveGroup(rgroup);
}

size_t DVPresentationState::getNumberOfCurvesInImage()
{
  return currentImageCurveList.size();
}

DVPSCurve *DVPresentationState::getCurveInImage(size_t idx)
{
  return currentImageCurveList.getCurve(idx);
}

OFCondition DVPresentationState::addCurve(size_t layer, size_t curveidxinimage)
{
  const char *lname = graphicLayerList.getGraphicLayerName(layer);
  DVPSCurve *curve = currentImageCurveList.getCurve(curveidxinimage);
  if ((curve==NULL)||(lname==NULL)) return EC_IllegalCall;
  return activationLayerList.setActivation(0x5000 + curve->getCurveGroup(), lname);
}

OFBool DVPresentationState::haveActiveVOIWindow()
{
  DVPSSoftcopyVOI *voi = getCurrentSoftcopyVOI();
  if (voi)
  {
    if (voi->haveLUT()) return OFFalse; else return OFTrue;
  }
  return OFFalse;
}

OFBool DVPresentationState::haveActiveVOILUT()
{
  DVPSSoftcopyVOI *voi = getCurrentSoftcopyVOI();
  if (voi) return voi->haveLUT();
  return OFFalse;
}

const char *DVPresentationState::getCurrentVOIDescription()
{
  DVPSSoftcopyVOI *voi = getCurrentSoftcopyVOI();
  if (voi) return voi->getCurrentVOIDescription();
  return NULL;
}

OFCondition DVPresentationState::getCurrentWindowWidth(double &w)
{
  DVPSSoftcopyVOI *voi = getCurrentSoftcopyVOI();
  if (voi) return voi->getCurrentWindowWidth(w);
  return EC_IllegalCall;
}

OFCondition DVPresentationState::getCurrentWindowCenter(double &c)
{
  DVPSSoftcopyVOI *voi = getCurrentSoftcopyVOI();
  if (voi) return voi->getCurrentWindowCenter(c);
  return EC_IllegalCall;
}

size_t DVPresentationState::getNumberOfVOILUTsInImage()
{
  return currentImageVOILUTList.size();
}

size_t DVPresentationState::getNumberOfVOIWindowsInImage()
{
  return currentImageVOIWindowList.size();
}

const char *DVPresentationState::getDescriptionOfVOILUTsInImage(size_t idx)
{
  DVPSVOILUT *lut = currentImageVOILUTList.getVOILUT(idx);
  if (lut==NULL) return NULL;
  return lut->getExplanation();
}

const char *DVPresentationState::getDescriptionOfVOIWindowsInImage(size_t idx)
{
  DVPSVOIWindow *window = currentImageVOIWindowList.getVOIWindow(idx);
  if (window==NULL) return NULL;
  return window->getExplanation();
}

OFCondition DVPresentationState::setVOILUTFromImage(size_t idx,
    DVPSObjectApplicability applicability)
{
  if (currentImage == NULL) return EC_IllegalCall;

  DVPSVOILUT *lut = currentImageVOILUTList.getVOILUT(idx);
  if (lut)
  {
    currentImageVOIValid = OFFalse; // VOI has changed

    DVPSSoftcopyVOI *voi = softcopyVOIList.createSoftcopyVOI(
      referencedSeriesList, currentImageSOPClassUID, currentImageSOPInstanceUID,
      currentImageSelectedFrame, currentImage->getFrameCount(), applicability);
    if (voi) return lut->assign(*voi);
  }
  return EC_IllegalCall;
}

OFCondition DVPresentationState::setVOIWindowFromImage(size_t idx,
    DVPSObjectApplicability applicability)
{
  currentImageVOIValid = OFFalse; // VOI has changed
  DVPSVOIWindow *window = currentImageVOIWindowList.getVOIWindow(idx);
  if (window)
  {
    return setVOIWindow(window->getWindowCenter(),
      window->getWindowWidth(), window->getExplanation(), applicability);
  }
  return EC_IllegalCall;
}

OFCondition DVPresentationState::setVOIWindow(double wCenter, double wWidth, const char *description,
    DVPSObjectApplicability applicability)
{
  if (currentImage == NULL) return EC_IllegalCall;
  currentImageVOIValid = OFFalse; // VOI has changed
  DVPSSoftcopyVOI *voi = softcopyVOIList.createSoftcopyVOI(
      referencedSeriesList, currentImageSOPClassUID, currentImageSOPInstanceUID,
      currentImageSelectedFrame, currentImage->getFrameCount(), applicability);
  if (voi) return voi->setVOIWindow(wCenter, wWidth, description);
  return EC_IllegalCall;
}

OFCondition DVPresentationState::setVOILUT(
    DcmUnsignedShort& lutDescriptor,
    DcmUnsignedShort& lutData,
    DcmLongString& lutExplanation,
    DVPSObjectApplicability applicability)
{
  if (lutData.getLength() == 0) return EC_IllegalCall;
  if (lutDescriptor.getVM() != 3) return EC_IllegalCall;
  if (currentImage == NULL) return EC_IllegalCall;

  currentImageVOIValid = OFFalse; // VOI has changed
  DVPSSoftcopyVOI *voi = softcopyVOIList.createSoftcopyVOI(
      referencedSeriesList, currentImageSOPClassUID, currentImageSOPInstanceUID,
      currentImageSelectedFrame, currentImage->getFrameCount(), applicability);
  if (voi) return voi->setVOILUT(lutDescriptor, lutData, lutExplanation);
  return EC_IllegalCall;
}

void DVPresentationState::deactivateVOI(DVPSObjectApplicability applicability)
{
  if (currentImage == NULL) return;
  currentImageVOIValid = OFFalse; // VOI has changed
  softcopyVOIList.removeSoftcopyVOI(
      referencedSeriesList, currentImageSOPInstanceUID,
      currentImageSelectedFrame, currentImage->getFrameCount(), applicability);
  return;
}

OFCondition DVPresentationState::setGammaVOILUT(double gammaValue, DVPSObjectApplicability applicability)
{
  if (currentImage == NULL) return EC_IllegalCall;

  OFCondition status = EC_IllegalCall;
  const unsigned int numberOfBits = 16;
  unsigned long numberOfEntries = 0;
  signed long firstMapped = 0;
  if (haveActiveVOIWindow())    // use active VOI window to specify the LUT descriptor
  {
    double ww, wc;
    if ((getCurrentWindowWidth(ww) == EC_Normal) && (getCurrentWindowCenter(wc) == EC_Normal))
    {
      if (ww <= 65536)
      {
        numberOfEntries = (unsigned long)ww;
        firstMapped = (signed long)(wc - ww / 2);
      }
    }
  }
  if (numberOfEntries == 0)     // no valid VOI window, use whole pixel range
  {
    double min, max;
    if (getImageMinMaxPixelRange(min, max) == EC_Normal)
    {
      if (max - min < 65536.0)
      {
        numberOfEntries = (unsigned long)(max - min + 1.0);
        firstMapped = (signed long)min;
      }
    }
  }

  if ((numberOfEntries > 0) && (numberOfEntries <= 65536) &&
     (((firstMapped >= -32768) && (firstMapped <= 32767)) || ((firstMapped >= 0) && (firstMapped <= 65535))))
  {
    Uint16 *data = new Uint16[numberOfEntries];
    if (data != NULL)
    {
      /* calculate gamma curve */
      const Uint16 maxValue = 0xFFFF >> (16 - numberOfBits);
      double step = (double)maxValue / ((double)numberOfEntries - 1.0);
      double gammaExp = 1.0 / gammaValue;
      double factor = (double)maxValue / pow((double)maxValue, gammaExp);
      unsigned long i;
      for (i = 0; i < numberOfEntries; i++)
        data[i]= (Uint16)(factor * pow(i * step, gammaExp));

      Uint16 numEntries16 = 0;
      if (numberOfEntries < 65536)
        numEntries16 = (Uint16)numberOfEntries;

      /* LUT Descriptor */
      DcmElement *lutDescriptor = NULL;
      if (firstMapped < 0)
      {
        // LUT Descriptor is SS
        lutDescriptor = new DcmSignedShort(DcmTag(DCM_LUTDescriptor, EVR_SS));
        if (lutDescriptor != NULL)
        {
            status = lutDescriptor->putSint16((Sint16)numEntries16, 0);
            if (EC_Normal == status)
              status = lutDescriptor->putSint16((Sint16)firstMapped, 1);
            if (EC_Normal == status)
              status = lutDescriptor->putSint16((Sint16)numberOfBits, 2);
        } else
          status = EC_MemoryExhausted;
      } else {
        // LUT Descriptor is US
        lutDescriptor = new DcmUnsignedShort(DcmTag(DCM_LUTDescriptor, EVR_US));
        if (lutDescriptor != NULL)
        {
            status = lutDescriptor->putUint16(numEntries16, 0);
            if (EC_Normal == status)
              status = lutDescriptor->putUint16((Uint16)firstMapped, 1);
            if (EC_Normal == status)
              status = lutDescriptor->putUint16((Uint16)numberOfBits, 2);
        } else
            status = EC_MemoryExhausted;
      }

      /* LUT Data */
      DcmElement *lutData = NULL;
      if (status == EC_Normal)
      {
        // LUT Data as OW, because of max size = 64K
        lutData = new DcmOtherByteOtherWord(DcmTag(DCM_LUTData, EVR_OW));
        if (lutData != NULL)
          status = lutData->putUint16Array(data, numberOfEntries);
        else
          status = EC_MemoryExhausted;
      }

      /* LUT Explanation */
      DcmLongString *lutExplanation = NULL;
      if (status == EC_Normal)
      {
        char explanation[100];
        char gammabuf[16];
        OFStandard::ftoa(gammabuf, sizeof(gammabuf), gammaValue, OFStandard::ftoa_format_f, 3, 1);

        sprintf(explanation, "LUT with gamma %s, descriptor %u/%ld/%u", gammabuf,
               (numberOfEntries < 65536) ? (Uint16)numberOfEntries : 0, firstMapped, numberOfBits);

        lutExplanation = new DcmLongString(DCM_LUTExplanation);
        if (lutExplanation != NULL)
          status = lutExplanation->putString(explanation);
        else
          status = EC_MemoryExhausted;
      }

      /* set VOI LUT */
      if (status == EC_Normal)
      {
        if ((lutDescriptor != NULL) && (lutData != NULL) && (lutExplanation !=  NULL))
          status = setVOILUT(*(DcmUnsignedShort *)lutDescriptor, *(DcmUnsignedShort *)lutData, *lutExplanation, applicability);
      }

      /* delete temporary dcmtk structures */
      delete lutDescriptor;
      delete lutData;
      delete lutExplanation;
    } else
      status = EC_MemoryExhausted;
    delete[] data;
  }
  return status;
}


const char *DVPresentationState::getActiveOverlayLabel(size_t layer, size_t idx)
{
  Uint16 group = getActiveOverlayGroup(layer, idx);
  if (group==0) return NULL;
  DVPSOverlay *internalOverlay = overlayList.getOverlayGroup(group);
  if (internalOverlay) return internalOverlay->getOverlayLabel();
  if (currentImage) return currentImage->getOverlayLabel(group);
  return NULL;
}

const char *DVPresentationState::getActiveOverlayDescription(size_t layer, size_t idx)
{
  Uint16 group = getActiveOverlayGroup(layer, idx);
  if (group==0) return NULL;
  DVPSOverlay *internalOverlay = overlayList.getOverlayGroup(group);
  if (internalOverlay) return internalOverlay->getOverlayDescription();
  if (currentImage) return currentImage->getOverlayDescription(group);
  return NULL;
}

OFBool DVPresentationState::activeOverlayIsROI(size_t layer, size_t idx)
{
  Uint16 group = getActiveOverlayGroup(layer, idx);
  if (group==0) return OFFalse;
  DVPSOverlay *internalOverlay = overlayList.getOverlayGroup(group);
  if (internalOverlay) return internalOverlay->isROI();
  if ((currentImage)&&(EMO_RegionOfInterest == currentImage->getOverlayMode(group))) return OFTrue;
  return OFFalse;
}

size_t DVPresentationState::getNumberOfOverlaysInImage()
{
  if (currentImage == NULL) return 0;

  unsigned int numOverlays=currentImage->getOverlayCount();
  size_t result = (size_t) numOverlays;
  Uint16 group;
  for (unsigned int i=0; i<numOverlays; i++)
  {
    // ignore all overlays which are shadowed by the presentation state
    group = (Uint16)(currentImage->getOverlayGroupNumber(i));
    if ((group==0)||(overlayList.haveOverlayGroup(group))) result--;
  }
  return result;
}


Uint16 DVPresentationState::getOverlayInImageGroup(size_t idx)
{
  if (currentImage == NULL) return 0;

  size_t currentIndex = 0;
  Uint16 group;
  do
  {
    group = (Uint16) (currentImage->getOverlayGroupNumber(currentIndex++));
    if (!overlayList.haveOverlayGroup(group))
    {
      // group is not shadowed by the presentation state
      if (idx==0) return group; else idx--;
    }
  } while (group != 0);
  return 0;
}


const char *DVPresentationState::getOverlayInImageLabel(size_t idx)
{
  Uint16 group = getOverlayInImageGroup(idx); // returns 0 if currentImage==NULL
  if (group==0) return NULL; else return currentImage->getOverlayLabel(group);
}


const char *DVPresentationState::getOverlayInImageDescription(size_t idx)
{
  Uint16 group = getOverlayInImageGroup(idx); // returns 0 if currentImage==NULL
  if (group==0) return NULL; else return currentImage->getOverlayDescription(group);
}


size_t DVPresentationState::getOverlayInImageActivationLayer(size_t idx)
{
  Uint16 group = getOverlayInImageGroup(idx); // returns 0 if currentImage==NULL
  if (group==0) return DVPS_IDX_NONE;
  const char *layerName = activationLayerList.getActivationLayer(group);
  if (layerName) return graphicLayerList.getGraphicLayerIndex(layerName);
  else return DVPS_IDX_NONE;
}


OFBool DVPresentationState::overlayInImageIsROI(size_t idx)
{
  Uint16 group = getOverlayInImageGroup(idx); // returns 0 if currentImage==NULL
  if (group==0) return OFFalse;
  if (EMO_RegionOfInterest == currentImage->getOverlayMode(group)) return OFTrue;
  else return OFFalse;
}

OFCondition DVPresentationState::removeOverlayFromPresentationState(size_t idx)
{
  Uint16 group = getOverlayInPresentationStateGroup(idx);
  if (group != 0)
  {
    activationLayerList.removeActivation(group);
    currentImageOverlaysValid = 1; // invalid but nothing added
    return overlayList.removeOverlay(idx);
  }
  return EC_IllegalCall; // overlay does not exist
}


Uint16 DVPresentationState::findOverlayGroup(Uint16 currentGroup)
{
  int allocated[16];
  size_t i, max;
  Uint16 group = 0;

  for (i=0; i<16; i++) allocated[i]=0;
  max = overlayList.size();
  for (i=0; i<max; i++)
  { // mark all group numbers used in presentation state
    group = getOverlayInPresentationStateGroup(i);
    if ((group >= 0x6000)&&(group <= 0x601F)) allocated[(Uint16)(group - 0x6000) >> 1] = 2;
  }
  max = getNumberOfOverlaysInImage();
  for (i=0; i<max; i++)
  { // mark all group numbers used in image
    group = getOverlayInImageGroup(i);
    if ((group >= 0x6000)&&(group <= 0x601F)) allocated[(Uint16)(group - 0x6000) >> 1] += 1;
  }
  // now we have 0=unused, 1=used in image, 2=used in pstate, 3=used in both.

  // check if the current group can be left unchanged
  if ((currentGroup >= 0x6000)&&(currentGroup <= 0x601F))
  {
    if (allocated[(Uint16)(group - 0x6000) >> 1] == 2) return currentGroup;
  }
  // find a free group
  for (i=0; i<16; i++) if (allocated[i]==0) return 0x6000+(i<<1);
  // find a group not used in the presentation state
  for (i=0; i<16; i++) if (allocated[i]<2) return 0x6000+(i<<1);
  // not found.
  return 0;
}

OFCondition DVPresentationState::changeOverlayGroupInPresentationState(size_t idx, Uint16 newGroup)
{
  Uint16 group = getOverlayInPresentationStateGroup(idx);
  if (group != 0)
  {
    if (newGroup==0) newGroup = findOverlayGroup(group);
    if (newGroup==group) return EC_Normal; // shortcut - no change needed
    OFCondition result = overlayList.changeOverlayGroup(idx, newGroup);
    if (EC_Normal == result)
    {
      const char *layerName = activationLayerList.getActivationLayer(group);
      if (layerName)
      {
        activationLayerList.removeActivation(group);
        result = activationLayerList.setActivation(newGroup, layerName);
        currentImageOverlaysValid = 0; // invalid
      }
    }
    return result;
  }
  return EC_IllegalCall;
}

OFCondition DVPresentationState::addOverlayToPresentationState(DcmItem& overlayIOD, Uint16 groupInItem, Uint16 newGroup)
{
  if (newGroup==0) newGroup = findOverlayGroup();
  if (newGroup==0) return EC_IllegalCall; // no group number available
  return overlayList.addOverlay(overlayIOD, groupInItem, newGroup);
}

OFBool DVPresentationState::overlayIsSuitableAsBitmapShutter(size_t idx)
{
  if (currentImage)
  {
    DVPSOverlay *overlay = overlayList.getOverlay(idx);
    if (overlay)
    {
      return overlay->isSuitableAsShutter(currentImage->getWidth(), currentImage->getHeight());
    }
  }
  return OFFalse;
}

OFCondition DVPresentationState::activateOverlayInImage(size_t layer, size_t idx)
{
  Uint16 group = getOverlayInImageGroup(idx);
  if (group==0) return EC_IllegalCall;
  if (activationLayerList.getActivationLayer(group) != NULL) return EC_IllegalCall; //already activated

  const char *layerName = getGraphicLayerName(layer);
  if (layerName==NULL) return EC_IllegalCall;
  currentImageOverlaysValid = 1; // invalid but nothing (external) added
  return activationLayerList.setActivation(group, layerName);
}

OFCondition DVPresentationState::activateOverlayInPresentationState(size_t layer, size_t idx)
{
  Uint16 group = getOverlayInPresentationStateGroup(idx);
  if (group==0) return EC_IllegalCall;
  if (activationLayerList.getActivationLayer(group) != NULL) return EC_IllegalCall; //already activated
  if (haveShutter(DVPSU_bitmap))
  { // check if the overlay is used as the bitmap shutter
    Uint16 shutterGroup=0;
    shutterOverlayGroup.getUint16(shutterGroup,0);
    if (shutterGroup == group) return EC_IllegalCall; // used as bitmap shutter
  }
  const char *layerName = getGraphicLayerName(layer);
  if (layerName==NULL) return EC_IllegalCall;
  currentImageOverlaysValid = 0; // invalid
  return activationLayerList.setActivation(group, layerName);
}

OFCondition DVPresentationState::activateOverlayAsBitmapShutter(size_t idx)
{
  Uint16 group = getOverlayInPresentationStateGroup(idx);
  if (group==0) return EC_IllegalCall;
  if (activationLayerList.getActivationLayer(group) != NULL) return EC_IllegalCall; // activated as overlay
  if (overlayIsSuitableAsBitmapShutter(idx))
  {
    shutterOverlayGroup.clear();
    OFCondition result = shutterOverlayGroup.putUint16(group,0);
    if ((EC_Normal==result)&&(shutterPresentationValue.getLength()==0))
        result = shutterPresentationValue.putUint16(0,0);
    if (EC_Normal==result)
    {
      useShutterRectangular = OFFalse;
      useShutterCircular = OFFalse;
      useShutterPolygonal = OFFalse;
      useShutterBitmap = OFTrue;
      currentImageOverlaysValid = 0; // invalid
    }
    return result;
  }
  return EC_IllegalCall;
}

OFCondition DVPresentationState::deactivateOverlay(size_t layer, size_t idx)
{
  Uint16 group = activationLayerList.getActivationGroup(
    graphicLayerList.getGraphicLayerName(layer), idx, OFFalse);
  if (group == 0) return EC_IllegalCall;
  activationLayerList.removeActivation(group);
  currentImageOverlaysValid = 1; // invalid but nothing added
  return EC_Normal;
}

Uint16 DVPresentationState::convertPValueToDDL(Uint16 pvalue, unsigned int bits)
{
  Uint16 result = 0;
  if ((bits == 8) || (bits == 12))
  {
    if (currentImage && (bits == 8))
    {
      /* activate display transform */
      if (displayFunction && (displayTransform != DVPSD_none))
        currentImage->setDisplayFunction(displayFunction[displayTransform]);
      else
        currentImage->setNoDisplayFunction();
      currentImage->convertPValueToDDL(pvalue, result, bits /* 8 */);
    } else
      result = (pvalue >> (16 - bits));
  }
  return result;
}


void DVPresentationState::renderPixelData(OFBool display)
{
  if (currentImage == NULL) return;
  int result=0;

  /* activate Barten transform */
  if (displayFunction && (displayTransform != DVPSD_none) && display)
  {
    currentImage->setDisplayFunction(displayFunction[displayTransform]);
    if (previewImage != NULL)
      previewImage->setDisplayFunction(displayFunction[displayTransform]);
  } else {
    currentImage->setNoDisplayFunction();
    if (previewImage != NULL)
      previewImage->setNoDisplayFunction();
  }

  if (! currentImageVOIValid)
  {
     currentImageVOIValid = OFTrue;

     /* set VOI transformation */
     DVPSSoftcopyVOI *voi = getCurrentSoftcopyVOI();
     if (voi)
     {
       if (voi->haveLUT())
       {
         result = currentImage->setVoiLut(voi->getLUTData(), voi->getLUTDescriptor());
         if (previewImage != NULL)
           previewImage->setVoiLut(voi->getLUTData(), voi->getLUTDescriptor());
       } else {
         double wc=0.0, ww=0.0;
         if ((EC_Normal == voi->getCurrentWindowCenter(wc)) &&
             (EC_Normal == voi->getCurrentWindowWidth(ww)))
         {
           result = currentImage->setWindow(wc, ww);
           if (previewImage != NULL)
             previewImage->setWindow(wc, ww);
         } else {
           result = currentImage->setNoVoiTransformation();
           if (previewImage != NULL)
             previewImage->setNoVoiTransformation();
         }
       }
     } else {
       result = currentImage->setNoVoiTransformation();
       if (previewImage != NULL)
         previewImage->setNoVoiTransformation();
     }
     if (!result)
       DCMPSTAT_INFO("unable to set VOI transformation, ignoring.");
  } /* VOI transform */

  if (! currentImagePLUTValid)
  {
     presentationLUT.activate(currentImage);
     if (previewImage != NULL)
       presentationLUT.activate(previewImage);
     currentImagePLUTValid = OFTrue;
  } /* Presentation LUT */

  Uint16 bitmapShutterGroup = 0;
  Uint16 bitmapShutterPValue = 0;
  if (useShutterBitmap)
  {
    if (EC_Normal != shutterOverlayGroup.getUint16(bitmapShutterGroup, 0)) bitmapShutterGroup=0;
    if (EC_Normal != shutterPresentationValue.getUint16(bitmapShutterPValue, 0)) bitmapShutterPValue=0;
  }

  if (currentImageOverlaysValid==1)
  {
    /* overlays are invalid but no external overlays have been added */
    /* remove all external overlays that are not active as overlay or bitmap shutter */
    for (unsigned int remgroup=0x6000; remgroup <= 0x601F; remgroup += 2)
    {
      if ((remgroup != bitmapShutterGroup)&&((! overlayList.haveOverlayGroup(remgroup))||
          (NULL == activationLayerList.getActivationLayer(remgroup))))
      {
         currentImage->removeOverlay(remgroup); // ignore return value.
      }
    }
    currentImageOverlaysValid = 2; // valid
  }
  else if (currentImageOverlaysValid==0)
  {
    /* overlays are invalid */

    /* since we might be required to add external overlays, we first
     * need to flip the image back to its original rotation and flip state
     */
    if (currentImageFlip)
    {
      result = currentImage->flipImage();
      if (previewImage != NULL) previewImage->flipImage();
      if (!result)
        DCMPSTAT_INFO("unable to flip image horizontally, ignoring.");
      currentImageFlip = OFFalse;
    }
    signed int srot=0;
    switch(currentImageRotation)
    {
      case DVPSR_0_deg:   break;
      case DVPSR_90_deg:  srot=270; break;
      case DVPSR_180_deg: srot=180; break;
      case DVPSR_270_deg: srot=90; break;
    }
    if (srot != 0)
    {
      result = currentImage->rotateImage(srot);
      if (previewImage != NULL)
        previewImage->rotateImage(srot);
      if (!result)
        DCMPSTAT_INFO("unable to rotate image by " << srot << " degrees, ignoring.");
    }
    currentImageRotation = DVPSR_0_deg;

    // deactivate all overlays first
    result = currentImage->removeAllOverlays();
    if (!result)
      DCMPSTAT_INFO("unable to disable external overlays, ignoring.");

    size_t numOverlays = overlayList.size();
    DVPSOverlay *overlay = NULL;
    Uint16 ovgroup;
    for (size_t ovidx=0; ovidx<numOverlays; ovidx++)
    {
      overlay = overlayList.getOverlay(ovidx);
      if (overlay)
      {
        ovgroup = overlay->getOverlayGroup()+ 0x6000;
        if (NULL != activationLayerList.getActivationLayer(ovgroup))
        {
          if (activateOverlayHelper(*overlay, *currentImage).bad())
          {
            if (!result)
              DCMPSTAT_INFO("unable to set external overlay group 0x"
                << STD_NAMESPACE hex << ovgroup << STD_NAMESPACE dec << ", ignoring.");
          }
        }
        else if ((useShutterBitmap)&&(ovgroup == bitmapShutterGroup))
        {
          //activate bitmap overlay
          if (activateOverlayHelper(*overlay, *currentImage, OFTrue, bitmapShutterPValue).bad())
          {
            if (!result)
              DCMPSTAT_INFO("unable to activate bitmap shutter 0x"
                << STD_NAMESPACE hex << ovgroup << STD_NAMESPACE dec << ", ignoring.");
          }
        }
      }
    }
    currentImageOverlaysValid = 2; // valid
  }

  OFBool pstateFlip = getFlip();
  DVPSRotationType pstateRotation = getRotation();

  // store image width and height after application of rotation
  if ((pstateRotation == DVPSR_90_deg) || (pstateRotation == DVPSR_270_deg))
  {
    renderedImageWidth = currentImageHeight;
    renderedImageHeight = currentImageWidth;
  } else {
    renderedImageWidth = currentImageWidth;
    renderedImageHeight = currentImageHeight;
  }

  // get coordinates of current displayed area and store values after the
  // 'virtual' anti-rotation has been applied
  Sint32 tlhcX = 1;
  Sint32 tlhcY = 1;
  Sint32 brhcX = currentImageWidth;
  Sint32 brhcY = currentImageHeight;
  getImageRelativeDisplayedArea(tlhcX, tlhcY, brhcX, brhcY);
  if (tlhcX > brhcX)
  {                                     // swap 'left' and 'right' if necessary
    Sint32 tmp = tlhcX;
    tlhcX = brhcX;
    brhcX = tmp;
  }
  if (tlhcY > brhcY)
  {                                     // swap 'top' and 'bottom' if necessary
    Sint32 tmp = tlhcY;
    tlhcY = brhcY;
    brhcY = tmp;
  }
  switch (pstateRotation)
  {
    case DVPSR_0_deg:
      renderedImageTop = tlhcY;
      renderedImageLeft = tlhcX;
      renderedImageBottom = brhcY;
      renderedImageRight = brhcX;
      break;
    case DVPSR_90_deg:
      renderedImageTop = tlhcX;
      renderedImageLeft = (signed long)currentImageHeight - brhcY + 1;
      renderedImageBottom = brhcX;
      renderedImageRight = (signed long)currentImageHeight - tlhcY + 1;
      break;
    case DVPSR_180_deg:
      renderedImageTop = (signed long)currentImageHeight - brhcY + 1;
      renderedImageLeft = (signed long)currentImageWidth - brhcX + 1;
      renderedImageBottom = (signed long)currentImageHeight - tlhcY + 1;
      renderedImageRight = (signed long)currentImageWidth - tlhcX + 1;
      break;
    case DVPSR_270_deg:
      renderedImageTop = (signed long)currentImageWidth - brhcX + 1;
      renderedImageLeft = tlhcY;
      renderedImageBottom = (signed long)currentImageWidth - tlhcX + 1;
      renderedImageRight = brhcY;
      break;
  }
  if (pstateFlip)
  {
    signed long tmp = renderedImageLeft;
    renderedImageLeft = (signed long)renderedImageWidth - renderedImageRight + 1;
    renderedImageRight = (signed long)renderedImageWidth - tmp + 1;
  }

  // we can always reach the final rotation/flip status with
  // at most one rotation and one flip. The following formula
  // derives the operations to perform.
  OFBool flp=OFFalse;
  if ((pstateFlip && !currentImageFlip)||(!pstateFlip && currentImageFlip)) flp=OFTrue; else flp=OFFalse;

  signed int rot=0;
  switch (pstateRotation)
  {
    case DVPSR_0_deg:
      switch(currentImageRotation)
      {
        case DVPSR_0_deg:   rot=0; break;
        case DVPSR_90_deg:  if (currentImageFlip) rot=90; else rot=270; break;
        case DVPSR_180_deg: rot=180; break;
        case DVPSR_270_deg: if (currentImageFlip) rot=270; else rot=90; break;
      }
      break;
    case DVPSR_90_deg:
      switch(currentImageRotation)
      {
        case DVPSR_0_deg:   if (currentImageFlip) rot=270; else rot=90; break;
        case DVPSR_90_deg:  rot=0; break;
        case DVPSR_180_deg: if (currentImageFlip) rot=90; else rot=270; break;
        case DVPSR_270_deg: rot=180; break;
      }
      break;
    case DVPSR_180_deg:
      switch(currentImageRotation)
      {
        case DVPSR_0_deg:   rot=180; break;
        case DVPSR_90_deg:  if (currentImageFlip) rot=270; else rot=90; break;
        case DVPSR_180_deg: rot=0; break;
        case DVPSR_270_deg: if (currentImageFlip) rot=90; else rot=270; break;
      }
      break;
    case DVPSR_270_deg:
      switch(currentImageRotation)
      {
        case DVPSR_0_deg:   if (currentImageFlip) rot=90; else rot=270; break;
        case DVPSR_90_deg:  rot=180; break;
        case DVPSR_180_deg: if (currentImageFlip) rot=270; else rot=90; break;
        case DVPSR_270_deg: rot=0; break;
      }
      break;
  }

  if (rot != 0)
  {
    result = currentImage->rotateImage(rot);
    if (previewImage != NULL) previewImage->rotateImage(rot);
    if (!result)
      DCMPSTAT_INFO("unable to rotate image by " << rot << " degrees, ignoring.");
  }

  if (flp)
  {
    result = currentImage->flipImage();
    if (previewImage != NULL) previewImage->flipImage();
    if (!result)
      DCMPSTAT_INFO("unable to flip image horizontally, ignoring.");
  }

  currentImageRotation = pstateRotation;
  currentImageFlip = pstateFlip;

  return;
}


OFCondition DVPresentationState::getOverlayData(
     size_t layer,
     size_t idx,
     const void *&overlayData,
     unsigned int &width,
     unsigned int &height,
     unsigned int &left_pos,
     unsigned int &top_pos,
     OFBool &isROI,
     Uint16 &fore,
     unsigned int bits)
{
   EM_Overlay mode = EMO_Default;
   if (currentImage && ((bits == 8) || (bits == 12)))
   {
     renderPixelData();
     Uint16 group = activationLayerList.getActivationGroup(graphicLayerList.getGraphicLayerName(layer), idx, OFFalse);
     if (group==0) return EC_IllegalCall;
     Uint16 back = 0;
     fore = (Uint16)DicomImageClass::maxval(bits);   /* 255 or 4095 */
     Uint16 pvalue = 65535;
     if (graphicLayerList.getGraphicLayerRecommendedDisplayValueGray(layer, pvalue) == EC_Normal)
         currentImage->convertPValueToDDL(pvalue, fore, bits);
     if (fore == 0)
         back = (Uint16)DicomImageClass::maxval(bits);
     const void *data = currentImage->getOverlayData((unsigned int)group, left_pos, top_pos, width, height, mode,
       currentImageSelectedFrame-1, bits, fore, back);
     if (EMO_RegionOfInterest == mode) isROI=OFTrue; else isROI=OFFalse;
     if (data) overlayData = data;
     else
     {
       overlayData = NULL;
       return EC_IllegalCall;
     }
   } else {
     overlayData = NULL;
     width = 0;
     height = 0;
     left_pos = 0;
     top_pos = 0;
     isROI = OFFalse;
     fore = 0;
     return EC_IllegalCall;
   }
   return EC_Normal;
}

OFBool DVPresentationState::isInverse()
{
    return imageInverse;
}

OFCondition DVPresentationState::invertImage()
{
  OFCondition status = presentationLUT.invert();
  currentImagePLUTValid = OFFalse; // PLUT has changed
  imageInverse = (imageInverse ? OFFalse : OFTrue);
  return status;
}


OFCondition DVPresentationState::getPixelData(
     const void *&pixelData,
     unsigned long &width,
     unsigned long &height)
{
   if (currentImage)
   {
     renderPixelData();
     width = currentImage->getWidth();
     height = currentImage->getHeight();
     pixelData = currentImage->getOutputData(8, currentImageSelectedFrame-1);
   } else {
     pixelData = NULL;
     width = 0;
     height = 0;
     return EC_IllegalCall;
   }
   return EC_Normal;
}

OFCondition DVPresentationState::getPixelData(
     void *pixelData,
     unsigned long size)
{
   if (currentImage)
   {
     renderPixelData();
     if (currentImage->getOutputData(pixelData, size, 8, currentImageSelectedFrame-1))
         return EC_Normal;
   }
   return EC_IllegalCall;
}

OFCondition DVPresentationState::getImageMinMaxPixelRange(double &minValue, double& maxValue)
{
  OFCondition result = EC_IllegalCall;
  if (currentImage)
  {
    if (currentImage->getMinMaxValues(minValue, maxValue, 1)) result = EC_Normal;
  }
  return result;
}

OFCondition DVPresentationState::getImageMinMaxPixelValue(double &minValue, double& maxValue)
{
  OFCondition result = EC_IllegalCall;
  if (currentImage)
  {
    if (currentImage->getMinMaxValues(minValue, maxValue, 0)) result = EC_Normal;
  }
  return result;
}

OFCondition DVPresentationState::getImageNumberOfFrames(unsigned long &frames)
{
  if (currentImage)
  {
    frames = (unsigned long)(currentImage->getFrameCount());
    return EC_Normal;
  }
  return EC_IllegalCall;
}

OFCondition DVPresentationState::selectImageFrameNumber(unsigned long frame)
{
  if ((frame > 0) && currentImage && (frame <= currentImage->getFrameCount()))
  {
    if (currentImageSelectedFrame != frame)
      currentImageVOIValid = OFFalse; // VOI might has changed
    currentImageSelectedFrame=frame;
    return EC_Normal;
  }
  return EC_IllegalCall;
}

unsigned long DVPresentationState::getSelectedImageFrameNumber()
{
  if (currentImage)
  {
    if (currentImageSelectedFrame <= currentImage->getFrameCount())
      return currentImageSelectedFrame;
  }
  return 0;
}

DVPSDisplayedArea *DVPresentationState::getDisplayedAreaSelection()
{
  if (currentImage==NULL) return NULL;
  DVPSDisplayedArea * area = displayedAreaSelectionList.findDisplayedArea(currentImageSOPInstanceUID, currentImageSelectedFrame);
  if (area==NULL)
  {
      DCMPSTAT_INFO("no displayed area selection item for current image found, creating default.");
      if ((currentImageDataset)&&(EC_Normal == createDefaultDisplayedArea(*currentImageDataset)))
      {
        area = displayedAreaSelectionList.findDisplayedArea(currentImageSOPInstanceUID, currentImageSelectedFrame);
      }
  }
  return area;
}

DVPSPresentationSizeMode DVPresentationState::getDisplayedAreaPresentationSizeMode()
{
  DVPSDisplayedArea * area = getDisplayedAreaSelection();
  if (area) return area->getPresentationSizeMode(); else return DVPSD_scaleToFit;
}

double DVPresentationState::getDisplayedAreaPresentationPixelAspectRatio()
{
  DVPSDisplayedArea * area = getDisplayedAreaSelection();
  if (area) return area->getPresentationPixelAspectRatio(); else return 1.0;
}

OFCondition DVPresentationState::getStandardDisplayedArea(Sint32& tlhcX, Sint32& tlhcY, Sint32& brhcX, Sint32& brhcY)
{
  DVPSDisplayedArea * area = getDisplayedAreaSelection();
  if (area)
  {
    area->getDisplayedArea(tlhcX, tlhcY, brhcX, brhcY);
    return EC_Normal;
  }
  return EC_IllegalCall;
}

OFCondition DVPresentationState::getImageRelativeDisplayedArea(Sint32& tlhcX, Sint32& tlhcY, Sint32& brhcX, Sint32& brhcY)
{
  DVPSDisplayedArea * area = getDisplayedAreaSelection();
  if (area)
  {
    DVPSRotationType rotation = getRotation();
    OFBool flip = getFlip();
    area = area->clone(); // create temporary copy
    area->rotateAndFlip(rotation, flip, DVPSR_0_deg, OFFalse);
    area->getDisplayedArea(tlhcX, tlhcY, brhcX, brhcY);
    delete area;
    return EC_Normal;
  }
  return EC_IllegalCall;
}

OFCondition DVPresentationState::getDisplayedAreaPresentationPixelSpacing(double& x, double& y)
{
  DVPSDisplayedArea * area = getDisplayedAreaSelection();
  if (area) return area->getPresentationPixelSpacing(x, y); else return EC_IllegalCall;
}

OFCondition DVPresentationState::getDisplayedAreaPresentationPixelMagnificationRatio(double& magnification)
{
  DVPSDisplayedArea * area = getDisplayedAreaSelection();
  if (area) return area->getPresentationPixelMagnificationRatio(magnification); else return EC_IllegalCall;
}

OFBool DVPresentationState::canUseDisplayedAreaTrueSize()
{
  DVPSDisplayedArea * area = getDisplayedAreaSelection();
  if (area) return area->canUseTrueSize(); else return OFFalse;
}

OFCondition DVPresentationState::setStandardDisplayedArea(
     DVPSPresentationSizeMode sizeMode,
     Sint32 tlhcX, Sint32 tlhcY,
     Sint32 brhcX, Sint32 brhcY,
     double magnification,
     DVPSObjectApplicability applicability)
{
  if (currentImage == NULL) return EC_IllegalCall;

  // make sure that we have an old displayed area item that "knows" about pixel spacing/aspect ratio,
  // because we will only copy this data into the new item.
  DVPSDisplayedArea *area = getDisplayedAreaSelection();

  // find appropriate item, create new if necessary.
  area = displayedAreaSelectionList.createDisplayedArea(
      referencedSeriesList, currentImageSOPClassUID, currentImageSOPInstanceUID,
      currentImageSelectedFrame, currentImage->getFrameCount(), applicability);

  if (area) return area->setDisplayedArea(sizeMode, tlhcX, tlhcY, brhcX, brhcY, magnification);
  return EC_IllegalCall;
}

OFCondition DVPresentationState::setImageRelativeDisplayedArea(
     DVPSPresentationSizeMode sizeMode,
     Sint32 tlhcX, Sint32 tlhcY,
     Sint32 brhcX, Sint32 brhcY,
     double magnification,
     DVPSObjectApplicability applicability)
{
  if (currentImage == NULL) return EC_IllegalCall;

  // make sure that we have an old displayed area item that "knows" about pixel spacing/aspect ratio,
  // because we will only copy this data into the new item.
  DVPSDisplayedArea *area = getDisplayedAreaSelection();

  // find appropriate item, create new if necessary.
  area = displayedAreaSelectionList.createDisplayedArea(
      referencedSeriesList, currentImageSOPClassUID, currentImageSOPInstanceUID,
      currentImageSelectedFrame, currentImage->getFrameCount(), applicability);

  if (area)
  {
     // get current rotation and flip status
     DVPSRotationType rotation = getRotation();
     OFBool flip = getFlip();

     // force rotation and flip status back to unrotated/unflipped
     // because in this case standard displayed area and image relative displayed area are the same
     area->rotateAndFlip(rotation, flip, DVPSR_0_deg, OFFalse);

     // set displayed area
     OFCondition result = area->setDisplayedArea(sizeMode, tlhcX, tlhcY, brhcX, brhcY, magnification);

     // restore rotation and flip status
     area->rotateAndFlip(DVPSR_0_deg, OFFalse, rotation, flip);

     return result;
  }
  return EC_IllegalCall;
}


DVPSSoftcopyVOI *DVPresentationState::getCurrentSoftcopyVOI()
{
  if (currentImage==NULL) return NULL;
  return softcopyVOIList.findSoftcopyVOI(currentImageSOPInstanceUID, currentImageSelectedFrame);
}

OFCondition DVPresentationState::getPrintBitmapRequestedImageSize(OFString& requestedImageSize)
{
  requestedImageSize.clear();
  if ((currentImage)&&(getDisplayedAreaPresentationSizeMode()==DVPSD_trueSize))
  {
    double x=0.0, y=0.0;
    if (EC_Normal == getDisplayedAreaPresentationPixelSpacing(x, y))
    {
      char c[80];
      DVPSRotationType rotation = getRotation();
      if ((rotation==DVPSR_90_deg)||(rotation==DVPSR_270_deg))
      {
        x = y * currentImageHeight; // physical height of unrotated image in mm
      } else {
        x *= currentImageWidth;  // physical width of unrotated image in mm
      }
      OFStandard::ftoa(c, sizeof(c), x, OFStandard::ftoa_format_f);

      requestedImageSize = c;
      return EC_Normal;
    }
  }
  return EC_IllegalCall;
}

OFCondition DVPresentationState::writePresentationLUTforPrint(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  if (currentImageMonochrome1)
  {
    // write inverted LUT because image is also converted to MONOCHROME2
    presentationLUT.invert();
    if (EC_Normal==result) result = presentationLUT.write(dset, OFFalse);
    presentationLUT.invert();
  } else result = presentationLUT.write(dset, OFFalse);
  return result;
}

const char *DVPresentationState::getCurrentImageModality()
{
  char *c = NULL;
  if (EC_Normal == currentImageModality.getString(c)) return c; else return NULL;
}

const char *DVPresentationState::getAttachedImageSOPClassUID()
{
  return currentImageSOPClassUID;
}


const char *DVPresentationState::getAttachedImageSOPInstanceUID()
{
  return currentImageSOPInstanceUID;
}


OFCondition DVPresentationState::activateOverlayHelper(DVPSOverlay& ovl, DicomImage &image, OFBool asShutter, Uint16 pvalue)
{
  Sint16 originX=0;
  Sint16 originY=0;
  Uint16 sizeX=0;
  Uint16 sizeY=0;
  unsigned int group = ovl.getOverlayGroup() + 0x6000;
  EM_Overlay mode=EMO_Graphic;
  if (asShutter) mode=EMO_BitmapShutter; else if (ovl.isROI()) mode=EMO_RegionOfInterest;

  OFCondition result = ovl.getValues(originX, originY, sizeX, sizeY);
  if (result.good())
  {
    signed int left_pos = (signed int) originX;
    signed int top_pos = (signed int) originY;
    unsigned int columns = (unsigned int)sizeX;
    unsigned int rows = (unsigned int)sizeY;
    if (0 == image.addOverlay(group, left_pos, top_pos, columns, rows,
      ovl.getData(), ovl.getLabel(), ovl.getDescription(), mode))
        result = EC_IllegalCall;
    if ((asShutter)&&(EC_Normal==result))
    {
      if (0 == image.showOverlay(group, pvalue)) result = EC_IllegalCall;
    }
  }
  return result;
}

OFCondition DVPresentationState::read(DcmItem &dset)
{
  OFCondition result = DcmPresentationState::read(dset);
  imageInverse = presentationLUT.isInverse();
  return result;
}

OFCondition DVPresentationState::createFromImage(
  DcmItem &dset,
  DVPSoverlayActivation overlayActivation,
  DVPSVOIActivation voiActivation,
  OFBool curveActivation,
  OFBool shutterActivation,
  OFBool presentationActivation,
  DVPSGraphicLayering layering,
  const char *aetitle,
  const char *filesetID,
  const char *filesetUID)
{
  OFCondition result = DcmPresentationState::createFromImage(
    dset, overlayActivation, voiActivation, curveActivation, shutterActivation,
    presentationActivation, layering, aetitle, filesetID, filesetUID);
  imageInverse = presentationLUT.isInverse();
  return result;
}


/*
 *  $Log: dvpstat.cc,v $
 *  Revision 1.86  2010-10-14 13:14:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.85  2009-11-24 14:12:59  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.84  2009-09-17 06:56:54  joergr
 *  Added parentheses around && within || in order to avoid compiler warnings.
 *
 *  Revision 1.83  2009-08-26 07:52:21  joergr
 *  Added parentheses around && within || in order to avoid warnings reported by
 *  gcc 4.3.2.
 *
 *  Revision 1.82  2006/08/15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.81  2005/12/08 15:46:51  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.80  2004/08/03 11:43:18  meichel
 *  Headers libc.h and unistd.h are now included via ofstdinc.h
 *
 *  Revision 1.79  2004/02/04 15:57:49  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.78  2003/09/05 14:30:08  meichel
 *  Introduced new API methods that allow Displayed Areas to be queried
 *    and set either relative to the image (ignoring rotation and flip) or
 *    in absolute values as defined in the standard.  Rotate and flip methods
 *    now adjust displayed areas in the presentation state.
 *
 *  Revision 1.77  2003/08/27 14:57:20  meichel
 *  Splitted class DVPresentationState into a base class DcmPresentationState
 *    that does not depend on module dcmimgle and current derived class with
 *    public API identical to the previous version.
 *
 *  Revision 1.76  2003/06/04 12:30:29  meichel
 *  Added various includes needed by MSVC5 with STL
 *
 *  Revision 1.75  2003/04/14 14:28:05  meichel
 *  Added explicit typecasts in calls to pow(). Needed by Visual C++ .NET 2003.
 *
 *  Revision 1.74  2002/12/09 13:28:16  joergr
 *  Renamed parameter/local variable to avoid name clashes with global
 *  declaration left and/or right (used for as iostream manipulators).
 *
 *  Revision 1.73  2002/12/04 10:41:38  meichel
 *  Changed toolkit to use OFStandard::ftoa instead of sprintf for all
 *    double to string conversions that are supposed to be locale independent
 *
 *  Revision 1.72  2002/11/27 15:48:15  meichel
 *  Adapted module dcmpstat to use of new header file ofstdinc.h
 *
 *  Revision 1.71  2002/04/16 14:02:22  joergr
 *  Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 *  Revision 1.70  2002/01/08 10:40:58  joergr
 *  Corrected spelling of function dcmGenerateUniqueIdentifier().
 *  Changed prefix of UIDs created for series and studies (now using constants
 *  SITE_SERIES_UID_ROOT and SITE_STUDY_UID_ROOT which are supposed to be used
 *  in these cases).
 *
 *  Revision 1.69  2001/11/28 13:57:05  joergr
 *  Check return value of DcmItem::insert() statements where appropriate to
 *  avoid memory leaks when insert procedure fails.
 *
 *  Revision 1.68  2001/09/28 13:50:14  joergr
 *  Added "#include <iomanip.h>" to keep gcc 3.0 quiet.
 *
 *  Revision 1.67  2001/09/26 15:36:34  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.66  2001/06/01 15:50:39  meichel
 *  Updated copyright header
 *
 *  Revision 1.65  2000/11/13 15:50:46  meichel
 *  Added dcmpstat support methods for creating image references
 *    in SR documents.
 *
 *  Revision 1.64  2000/07/12 12:49:05  joergr
 *  Added comment.
 *
 *  Revision 1.63  2000/07/03 14:04:01  joergr
 *  Fixed bug: VOI LUT transform defined per frame was not supported by the
 *  method renderPixelData().
 *
 *  Revision 1.62  2000/06/09 10:15:37  joergr
 *  Added support for rendering inverse presentation LUT into print bitmaps.
 *
 *  Revision 1.61  2000/06/08 17:39:07  joergr
 *  Corrected bug in addImageReferenceToPState().
 *
 *  Revision 1.60  2000/06/08 10:44:38  meichel
 *  Implemented Referenced Presentation LUT Sequence on Basic Film Session level.
 *    Empty film boxes (pages) are not written to file anymore.
 *
 *  Revision 1.59  2000/06/02 16:01:08  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.58  2000/06/02 12:48:04  joergr
 *  Reject invalid frame numbers in method selectImageFrameNumber().
 *
 *  Revision 1.57  2000/05/31 13:02:40  meichel
 *  Moved dcmpstat macros and constants into a common header file
 *
 *  Revision 1.56  2000/05/30 14:22:14  joergr
 *  Renamed some variables to avoid compiler warnings (reported by gcc 2.9x with
 *  additional compiler flags).
 *
 *  Revision 1.55  2000/05/30 13:59:16  joergr
 *  Removed methods which were already marked as "retired".
 *  Added new function allowing to set a VOILUT created from a given gamma
 *  value.
 *
 *  Revision 1.54  2000/03/08 16:29:11  meichel
 *  Updated copyright header.
 *
 *  Revision 1.53  2000/03/03 14:14:06  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.52  2000/02/23 15:12:57  meichel
 *  Corrected macro for Borland C++ Builder 4 workaround.
 *
 *  Revision 1.51  2000/02/01 11:54:45  meichel
 *  Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *    workaround for bug in compiler header files.
 *
 *  Revision 1.50  1999/11/15 19:03:13  joergr
 *  Changed behaviour of method getOverlayData(): parameter 'transp' replaced by
 *  'fore' to specify the foreground color used for the overlay plane.
 *  Fixed bug concerning the setting of window center and width for the preview
 *  image.
 *
 *  Revision 1.49  1999/11/12 16:51:54  meichel
 *  Corrected creation of circular shutters, X/Y coordinates were swapped.
 *
 *  Revision 1.48  1999/10/25 18:46:07  joergr
 *  Fixed bug caused by the incorrect order of x/y coordinates for circular
 *  and polygonal shutters (the DICOM standard is somehow inconsistent in this
 *  respect).
 *
 *  Revision 1.47  1999/10/22 09:08:23  joergr
 *  Added validity check to methods setting pixel aspect ratio and pixel
 *  spacing (>0). Fixed problems with incorrect pixel spacing (0\0) stored in
 *  sample images.
 *
 *  Revision 1.46  1999/10/20 18:41:20  joergr
 *  Added explicit type cast to make MSVC happy.
 *
 *  Revision 1.45  1999/10/20 11:01:16  joergr
 *  Enhanced method getOverlayData to support 12 bit data for print.
 *  Enhanced method convertPValueToDDL to support 12 bit data for print.
 *  Added support for a down-scaled preview image of the current DICOM image
 *  (e.g. useful for online-windowing or print preview).
 *  Always use the variable 'currentImageSelectedFrame' as the frame number,
 *  not 1.
 *
 *  Revision 1.44  1999/10/19 16:24:58  meichel
 *  Corrected handling of MONOCHROME1 images when used with P-LUTs
 *
 *  Revision 1.43  1999/10/18 10:18:52  joergr
 *  Use the current display shutter P-value for the border area of print
 *  bitmaps.
 *  Switch off time consuming interpolation for implicite scaling of print
 *  bitmaps.
 *
 *  Revision 1.42  1999/10/13 14:12:02  meichel
 *  Added config file entries and access methods
 *    for user-defined VOI presets, log directory, verbatim logging
 *    and an explicit list of image display formats for each printer.
 *
 *  Revision 1.41  1999/10/07 17:22:03  meichel
 *  Reworked management of Presentation LUTs in order to create tighter
 *    coupling between Softcopy and Print.
 *
 *  Revision 1.40  1999/10/06 13:24:50  joergr
 *  Fixed bug in renderPixelData: images haven't been flipped correctly for
 *  PrintBitmap.
 *  Corrected creation of PrintBitmap pixel data: VOI windows should be applied
 *  before clipping to avoid that the region outside the image (border) is also
 *  windowed (this requires a new method in dcmimgle to create a DicomImage
 *  with the grayscale transformations already applied).
 *
 *  Revision 1.39  1999/09/30 12:04:04  joergr
 *  Corrected typos and formatting; modified comments.
 *
 *  Revision 1.38  1999/09/20 13:22:23  joergr
 *  Corrected bug with clipping of rotated print bitmaps (removed inconsistency
 *  with 90 and 270 degree rotation).
 *
 *  Revision 1.37  1999/09/17 14:29:46  meichel
 *  Moved static helper functions to new class DVPSHelper, removed some unused code.
 *
 *  Revision 1.36  1999/09/13 14:01:23  thiel
 *  correction of DEBUG code for LIN_OD
 *
 *  Revision 1.35  1999/09/10 13:07:38  thiel
 *  correction of LIN OD in createFromImage
 *
 *  Revision 1.34  1999/09/10 12:46:58  meichel
 *  Added implementations for a number of print API methods.
 *
 *  Revision 1.33  1999/09/10 09:16:44  joergr
 *  Added support for CIELAB display function. New methods to handle display
 *  functions. Old methods are marked as retired and should be removed asap.
 *
 *  Revision 1.32  1999/09/10 07:32:44  thiel
 *  Added Presentation LUT Shape LIN OD
 *
 *  Revision 1.31  1999/09/07 09:05:13  joergr
 *  Completed support for getting a print bitmap out of a pstate object.
 *
 *  Revision 1.30  1999/09/01 16:15:11  meichel
 *  Added support for requested image size to print routines
 *
 *  Revision 1.29  1999/08/31 14:01:38  meichel
 *  Fixed print image crop boundary computation problem
 *
 *  Revision 1.28  1999/08/27 15:57:51  meichel
 *  Added methods for saving hardcopy images and stored print objects
 *    either in file or in the local database.
 *
 *  Revision 1.27  1999/08/25 16:51:17  joergr
 *  Added minimal support to get a print bitmap out of a pstate object.
 *
 *  Revision 1.26  1999/07/30 13:35:01  meichel
 *  Added new classes managing Stored Print objects
 *
 *  Revision 1.25  1999/07/28 07:57:26  meichel
 *  Minor correction for VC++ 5.
 *
 *  Revision 1.24  1999/07/22 16:40:04  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *  Revision 1.23  1999/07/14 12:03:44  meichel
 *  Updated data dictionary for supplement 29, 39, 33_lb, CP packet 4 and 5.
 *    Corrected dcmtk applications for changes in attribute name constants.
 *
 *  Revision 1.22  1999/05/04 15:27:27  meichel
 *  Minor code purifications to keep gcc on OSF1 quiet.
 *
 *  Revision 1.21  1999/05/03 11:01:38  joergr
 *  Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 *  Revision 1.20  1999/04/28 11:34:29  meichel
 *  When creating a presentation state for an image, modality rescale/slope
 *    without rescale type is handled now in a more lenient way.
 *
 *  Revision 1.19  1999/04/27 11:26:01  joergr
 *  Added method to check whether current image is inverse or not.
 *
 *  Revision 1.18  1999/03/22 09:52:42  meichel
 *  Reworked data dictionary based on the 1998 DICOM edition and the latest
 *    supplement versions. Corrected dcmtk applications for minor changes
 *    in attribute name constants.
 *
 *  Revision 1.17  1999/03/22 09:06:48  joergr
 *  Added parameter to get value of (transparent) background color for method
 *  getOverlayData.
 *
 *  Revision 1.16  1999/03/03 17:58:07  joergr
 *  Changed implementation of invertImage(): set modify flag for all plut types.
 *
 *  Revision 1.15  1999/03/03 14:02:03  joergr
 *  Changed implementation of invertImage() to avoid compiler errors on MSVC5
 *  ('return' has to be last statement).
 *
 *  Revision 1.14  1999/03/03 13:32:34  joergr
 *  Added method to invert an image by changing the presentation state LUT or
 *  shape.
 *  Changed implementation of method 'getOverlayData()': now conversion from
 *  P-value to DDL is implictly performed and the correct P-value for the related
 *  layer is used.
 *
 *  Revision 1.13  1999/02/25 18:41:42  joergr
 *  Added method to fill pixel data into an externally handled storage area.
 *  Added initialization of local variable to avoid compiler warnings (reported
 *  by gcc 2.7.2.1 on Linux).
 *
 *  Revision 1.12  1999/02/23 11:49:05  joergr
 *  Corrected bug: shutters were not saved correctly (sometimes even ignored).
 *
 *  Revision 1.11  1999/02/18 11:36:40  meichel
 *  Added new method convertPValueToDDL() to DVPresentationState
 *    that maps P-Values to DDLs.
 *
 *  Revision 1.10  1999/02/17 10:05:35  meichel
 *  Moved creation of Display Function object from DVPresentationState to
 *    DVInterface to avoid unnecessary re-reads.
 *
 *  Revision 1.9  1999/02/09 15:59:09  meichel
 *  Implemented bitmap shutter activation amd method for
 *    exchanging graphic layers.
 *
 *  Revision 1.8  1999/02/05 17:45:39  meichel
 *  Added config file entry for monitor characteristics file.  Monitor charac-
 *    teristics are passed to dcmimage if present to activate Barten transform.
 *
 *  Revision 1.7  1999/01/18 17:30:36  meichel
 *  Now preventing VOI Windows with a width <= 0.0.  Presentation label and
 *    creator's name are now correctly set.
 *
 *  Revision 1.6  1999/01/15 17:32:59  meichel
 *  added methods to DVPresentationState allowing to access the image
 *    references in the presentation state.  Also added methods allowing to
 *    get the width and height of the attached image.
 *
 *  Revision 1.5  1999/01/11 13:35:51  meichel
 *  added new methods getImageAspectRatio, getImageMinMaxPixelRange and
 *    getImageMinMaxPixelValue to class DVPresentationState.
 *
 *  Revision 1.4  1998/12/23 14:02:27  meichel
 *  Updated for changed interfaces in dcmimage overlays.
 *    Fixed bug affecting overlay origin delivered to dcmimage.
 *
 *  Revision 1.3  1998/12/22 17:57:18  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *  Revision 1.2  1998/12/14 16:10:48  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:47  meichel
 *  Initial Release.
 *
 */
