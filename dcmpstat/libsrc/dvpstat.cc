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
 *    classes: DVPresentationState
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-01-15 17:32:59 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpstat.h"
#include "ofstring.h"
#include "dcmimage.h"    /* for DicomImage */
#include "dvpscu.h"      /* for DVPSCurve */
#include "dvpsvl.h"      /* for DVPSVOILUT */
#include "dvpsvw.h"      /* for DVPSVOIWindow */
#include "dvpsov.h"      /* for DVPSOverlay */

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <stdio.h>
#include <string.h>
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_LIBC_H
#include <libc.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

/* some defaults for creating Presentation States */
#define DEFAULT_patientName               "^^^^"
#define DEFAULT_imageNumber               "1"
#define DEFAULT_seriesNumber              "1"
#define DEFAULT_presentationLabel         "UNNAMED"
#define DEFAULT_specificCharacterSet      "ISO_IR 100"
#define DEFAULT_presentationLUTShape      "IDENTITY"
#define DEFAULT_shutterPresentationValue  0

/* --------------- a few macros avoiding copy/paste --------------- */

#define ADD_TO_DATASET(a_type, a_name)                              \
if (result==EC_Normal)                                              \
{                                                                   \
  delem = new a_type(a_name);                                       \
  if (delem) dset.insert(delem); else result=EC_MemoryExhausted;    \
}

#define SET_DEFAULT(a_name, a_value)                                \
if (result==EC_Normal)                                              \
{                                                                   \
     if (a_name.getLength()==0) result = a_name.putString(a_value); \
}
   
#define SET_UID(a_name)                                             \
if (result==EC_Normal)                                              \
{                                                                   \
     if (a_name.getLength()==0) result = a_name.putString(dcmGenerateUniqueIdentifer(uid)); \
}

#define READ_FROM_DATASET(a_type, a_name)                           \
stack.clear();                                                      \
if (EC_Normal == dset.search((DcmTagKey &)a_name.getTag(), stack, ESM_fromHere, OFFalse)) \
{                                                                   \
  a_name = *((a_type *)(stack.top()));                              \
}

/* --------------- static helper functions --------------- */

static void currentDate(OFString &str)
{
  char buf[32];
  time_t tt = time(NULL);
  struct tm *ts = localtime(&tt);
  if (ts)
  {
    int year = 1900 + ts->tm_year;
    sprintf(buf, "%04d%02d%02d", year, ts->tm_mon + 1, ts->tm_mday);
    str = buf;
  } else str = "19000101";
  return;
}

static void currentTime(OFString &str)
{
  char buf[32];
  time_t tt = time(NULL);
  struct tm *ts = localtime(&tt);
  if (ts)
  {
    sprintf(buf, "%02d%02d%02d", ts->tm_hour, ts->tm_min, ts->tm_sec);
    str = buf;
  } else str = "000000";
  return;
}


/* --------------- class DVPresentationState --------------- */

DVPresentationState::DVPresentationState()
: patientName(DCM_PatientName)
, patientID(DCM_PatientID)
, patientBirthDate(DCM_PatientBirthDate)
, patientSex(DCM_PatientSex)
, studyInstanceUID(DCM_StudyInstanceUID)
, studyDate(DCM_StudyDate)
, studyTime(DCM_StudyTime)
, referringPhysiciansName(DCM_ReferringPhysiciansName)
, studyID(DCM_StudyID)
, accessionNumber(DCM_AccessionNumber)
, seriesInstanceUID(DCM_SeriesInstanceUID)
, seriesNumber(DCM_SeriesNumber)
, manufacturer(DCM_Manufacturer)
, displayedAreaTLHC(DCM_DisplayedAreaTLHC)
, displayedAreaBRHC(DCM_DisplayedAreaBRHC)
, presentationLUT(DVPSP_identity)
, presentationLUTDescriptor(DCM_LUTDescriptor)
, presentationLUTExplanation(DCM_LUTExplanation)
, presentationLUTData(DCM_LUTData)
, imageNumber(DCM_ImageNumber)
, presentationLabel(DCM_PresentationLabel)
, presentationDescription(DCM_PresentationDescription)
, presentationCreationDate(DCM_PresentationCreationDate)
, presentationCreationTime(DCM_PresentationCreationTime)
, presentationCreatorsName(DCM_PresentationCreatorsName)
, referencedSeriesList()
, sOPInstanceUID(DCM_SOPInstanceUID)
, specificCharacterSet(DCM_SpecificCharacterSet)
, instanceCreationDate(DCM_InstanceCreationDate)
, instanceCreationTime(DCM_InstanceCreationTime)
, instanceCreatorUID(DCM_InstanceCreatorUID)
, useShutterRectangular(OFFalse)
, useShutterCircular(OFFalse)
, useShutterPolygonal(OFFalse)
, useShutterBitmap(OFFalse)
, shutterShape(DCM_ShutterShape)
, shutterLeftVerticalEdge(DCM_ShutterLeftVerticalEdge)
, shutterRightVerticalEdge(DCM_ShutterRightVerticalEdge)
, shutterUpperHorizontalEdge(DCM_ShutterUpperHorizontalEdge)
, shutterLowerHorizontalEdge(DCM_ShutterLowerHorizontalEdge)
, centerOfCircularShutter(DCM_CenterOfCircularShutter)
, radiusOfCircularShutter(DCM_RadiusOfCircularShutter)
, verticesOfThePolygonalShutter(DCM_VerticesOfThePolygonalShutter)
, shutterPresentationValue(DCM_ShutterPresentationValue)
, shutterOverlayGroup(DCM_ShutterOverlayGroup)
, overlayList()
, activationLayerList()
, graphicAnnotationList()
, imageRotation(DCM_ImageRotation)
, imageHorizontalFlip(DCM_ImageHorizontalFlip)
, graphicLayerList()
, useModalityRescale(OFFalse)
, useModalityLUT(OFFalse)
, modalityLUTDescriptor(DCM_LUTDescriptor)
, modalityLUTExplanation(DCM_LUTExplanation)
, modalityLUTType(DCM_ModalityLUTType)
, modalityLUTData(DCM_LUTData)
, rescaleIntercept(DCM_RescaleIntercept)
, rescaleSlope(DCM_RescaleSlope)
, rescaleType(DCM_RescaleType)
, useVOIWindow(OFFalse)
, useVOILUT(OFFalse)
, voiLUTDescriptor(DCM_LUTDescriptor)
, voiLUTExplanation(DCM_LUTExplanation)
, voiLUTData(DCM_LUTData)
, windowCenter(DCM_WindowCenter)
, windowWidth(DCM_WindowWidth)
, windowCenterWidthExplanation(DCM_WindowCenterWidthExplanation)
, currentImageDataset(NULL)
, currentImageFileformat(NULL)
, currentImage(NULL)
, currentImageWidth(0)
, currentImageHeight(0)
, currentImageOwned(OFFalse)
, currentImageVOIValid(OFFalse)
, currentImagePLUTValid(OFFalse)
, currentImageFlip(OFFalse)
, currentImageRotation(DVPSR_0_deg)
, currentImageOverlaysValid(0)
, currentImageCurveList()
, currentImageVOILUTList()
, currentImageVOIWindowList()
{
}


DVPresentationState::~DVPresentationState()
{
  detachImage();
}

void DVPresentationState::detachImage()
{
  if (currentImage) delete currentImage;
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
  // reset flags
  currentImageWidth = 0;
  currentImageHeight = 0;
  currentImageOwned = OFFalse;
  currentImageVOIValid = OFFalse;
  currentImagePLUTValid = OFFalse;
  currentImageFlip = OFFalse;
  currentImageRotation = DVPSR_0_deg;
  currentImageOverlaysValid = 0;

  return;
}
  	

void DVPresentationState::clear()
{
  patientName.clear();
  patientID.clear();
  patientBirthDate.clear();
  patientSex.clear();
  studyInstanceUID.clear();
  studyDate.clear();
  studyTime.clear();
  referringPhysiciansName.clear();
  studyID.clear();
  accessionNumber.clear();
  seriesInstanceUID.clear();
  seriesNumber.clear();
  manufacturer.clear();
  displayedAreaTLHC.clear();
  displayedAreaBRHC.clear();
  presentationLUT = DVPSP_identity;
  presentationLUTDescriptor.clear();
  presentationLUTExplanation.clear();
  presentationLUTData.clear();
  imageNumber.clear();
  presentationLabel.clear();
  presentationDescription.clear();
  presentationCreationDate.clear();
  presentationCreationTime.clear();
  presentationCreatorsName.clear();
  referencedSeriesList.clear();
  sOPInstanceUID.clear();
  specificCharacterSet.clear();
  instanceCreationDate.clear();
  instanceCreationTime.clear();
  instanceCreatorUID.clear();
  shutterShape.clear();
  shutterLeftVerticalEdge.clear();
  shutterRightVerticalEdge.clear();
  shutterUpperHorizontalEdge.clear();
  shutterLowerHorizontalEdge.clear();
  centerOfCircularShutter.clear();
  radiusOfCircularShutter.clear();
  verticesOfThePolygonalShutter.clear();
  shutterPresentationValue.clear();
  shutterOverlayGroup.clear();
  overlayList.clear();
  activationLayerList.clear();
  graphicAnnotationList.clear();
  imageRotation.clear();
  imageHorizontalFlip.clear();
  graphicLayerList.clear();
  useModalityRescale = OFFalse;
  useModalityLUT = OFFalse;
  modalityLUTDescriptor.clear();
  modalityLUTExplanation.clear();
  modalityLUTType.clear();
  modalityLUTData.clear();
  rescaleIntercept.clear();
  rescaleSlope.clear();
  rescaleType.clear();
  useVOIWindow = OFFalse;
  useVOILUT = OFFalse;
  voiLUTDescriptor.clear();
  voiLUTExplanation.clear();
  voiLUTData.clear();
  windowCenter.clear();
  windowWidth.clear();
  windowCenterWidthExplanation.clear();
  detachImage(); // clears all currentImageXX attributes
  return;
}

E_Condition DVPresentationState::createDummyValues()
{
  E_Condition result = EC_Normal;
  char uid[100];
  OFString aString;
  
  SET_DEFAULT(patientName, DEFAULT_patientName)
  SET_UID(studyInstanceUID)
  SET_UID(seriesInstanceUID)
  if (displayedAreaTLHC.getVM() != 2) 
  { // default is 1\1
    displayedAreaTLHC.putUint16(1,0);
    displayedAreaTLHC.putUint16(1,1);
  }
  if (displayedAreaBRHC.getVM() != 2) 
  { // default is 1024\1024
    displayedAreaBRHC.putUint16(1024,0);
    displayedAreaBRHC.putUint16(1024,1);
  }

  SET_DEFAULT(imageNumber, DEFAULT_imageNumber)
  SET_DEFAULT(presentationLabel, DEFAULT_presentationLabel)
  currentDate(aString);
  SET_DEFAULT(presentationCreationDate, aString.c_str() )
  currentTime(aString);
  SET_DEFAULT(presentationCreationTime, aString.c_str() )

  if (result==EC_Normal && (sOPInstanceUID.getLength()==0))
  {
    sOPInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
    currentDate(aString);
    SET_DEFAULT(instanceCreationDate, aString.c_str() )
    currentTime(aString);
    SET_DEFAULT(instanceCreationTime, aString.c_str() )
  }
    
  // default for specific character set is -absent-.
  // SET_DEFAULT(specificCharacterSet, DEFAULT_specificCharacterSet )

  // create some dummy defaults for shutters
  if (shutterPresentationValue.getVM() != 1) shutterPresentationValue.putUint16(0);
  SET_DEFAULT(shutterLeftVerticalEdge, "1");
  SET_DEFAULT(shutterRightVerticalEdge, "1024");
  SET_DEFAULT(shutterUpperHorizontalEdge, "1");
  SET_DEFAULT(shutterLowerHorizontalEdge, "1024"); 
  SET_DEFAULT(centerOfCircularShutter, "512\\512");
  SET_DEFAULT(radiusOfCircularShutter, "512");
  SET_DEFAULT(verticesOfThePolygonalShutter, "1\\1\\1\\1024\\1024\\1024\\1024\\1\\1\\1");
  if (shutterOverlayGroup.getVM() != 1) shutterOverlayGroup.putUint16(0x6000);

  // create defaults for Spatial Transformation Module
  SET_DEFAULT(imageRotation, "0");
  SET_DEFAULT(imageHorizontalFlip, "N");
 
  // create defaults for Modality Rescale
  SET_DEFAULT(rescaleIntercept, "0");
  SET_DEFAULT(rescaleSlope, "1");
  SET_DEFAULT(rescaleType, "UNSPECIFIED");

  // create defaults for VOI Window
  SET_DEFAULT(windowCenter, "128");
  SET_DEFAULT(windowWidth, "256");

  return result;
}


E_Condition DVPresentationState::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmStack stack;
  DcmSequenceOfItems *seq;
  DcmItem *item;
 
  clear(); // re-initialize Presentation State object 

  /* check SOP class UID and modality first */
  DcmUniqueIdentifier sopclassuid(DCM_SOPClassUID);
  DcmCodeString modality(DCM_Modality);
  DcmCodeString presentationLUTShape(DCM_PresentationLUTShape);
  OFString aString;
  READ_FROM_DATASET(DcmUniqueIdentifier, sopclassuid)
  READ_FROM_DATASET(DcmCodeString, modality)
  
  if (sopclassuid.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: SOPClassUID absent or empty in presentation state" << endl;
#endif
  }
  else if (sopclassuid.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: SOPClassUID VM != 1 in presentation state" << endl;
#endif
  }
  
  if (modality.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: Modality absent or empty in presentation state" << endl;
#endif
  }
  else if (modality.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: Modality VM != 1 in presentation state" << endl;
#endif
  }

  sopclassuid.getOFString(aString,0);
  if (aString != UID_GrayscaleSoftcopyPresentationStateStorage)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: SOP Class UID does not match GrayscaleSoftcopyPresentationStateStorage" << endl;
#endif
  }

  modality.getOFString(aString,0);
  if (aString != "PR")
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: Modality does not match 'PR' for presentation state" << endl;
#endif
  }
  
  if (result==EC_Normal)
  {
    READ_FROM_DATASET(DcmPersonName, patientName)
    READ_FROM_DATASET(DcmLongString, patientID)
    READ_FROM_DATASET(DcmDate, patientBirthDate)
    READ_FROM_DATASET(DcmCodeString, patientSex)
    READ_FROM_DATASET(DcmUniqueIdentifier, studyInstanceUID)
    READ_FROM_DATASET(DcmDate, studyDate)
    READ_FROM_DATASET(DcmTime, studyTime)
    READ_FROM_DATASET(DcmPersonName, referringPhysiciansName)
    READ_FROM_DATASET(DcmShortString, studyID)
    READ_FROM_DATASET(DcmShortString, accessionNumber)
    READ_FROM_DATASET(DcmUniqueIdentifier, seriesInstanceUID)
    READ_FROM_DATASET(DcmIntegerString, seriesNumber)
    READ_FROM_DATASET(DcmLongString, manufacturer)
    READ_FROM_DATASET(DcmUnsignedShort, displayedAreaTLHC)
    READ_FROM_DATASET(DcmUnsignedShort, displayedAreaBRHC)
    READ_FROM_DATASET(DcmIntegerString, imageNumber)
    READ_FROM_DATASET(DcmCodeString, presentationLabel)
    READ_FROM_DATASET(DcmLongString, presentationDescription)
    READ_FROM_DATASET(DcmDate, presentationCreationDate)
    READ_FROM_DATASET(DcmTime, presentationCreationTime)
    READ_FROM_DATASET(DcmPersonName, presentationCreatorsName)
    READ_FROM_DATASET(DcmUniqueIdentifier, sOPInstanceUID)
    READ_FROM_DATASET(DcmCodeString, specificCharacterSet)
    READ_FROM_DATASET(DcmDate, instanceCreationDate)
    READ_FROM_DATASET(DcmTime, instanceCreationTime)
    READ_FROM_DATASET(DcmUniqueIdentifier, instanceCreatorUID)
    READ_FROM_DATASET(DcmCodeString, shutterShape)
    READ_FROM_DATASET(DcmIntegerString, shutterLeftVerticalEdge)
    READ_FROM_DATASET(DcmIntegerString, shutterRightVerticalEdge)
    READ_FROM_DATASET(DcmIntegerString, shutterUpperHorizontalEdge)
    READ_FROM_DATASET(DcmIntegerString, shutterLowerHorizontalEdge)
    READ_FROM_DATASET(DcmIntegerString, centerOfCircularShutter)
    READ_FROM_DATASET(DcmIntegerString, radiusOfCircularShutter)
    READ_FROM_DATASET(DcmIntegerString, verticesOfThePolygonalShutter)
    READ_FROM_DATASET(DcmUnsignedShort, shutterPresentationValue)
    READ_FROM_DATASET(DcmUnsignedShort, shutterOverlayGroup)
    READ_FROM_DATASET(DcmIntegerString, imageRotation)
    READ_FROM_DATASET(DcmCodeString, imageHorizontalFlip)
    READ_FROM_DATASET(DcmDecimalString, rescaleIntercept)
    READ_FROM_DATASET(DcmDecimalString, rescaleSlope)
    READ_FROM_DATASET(DcmLongString, rescaleType) 
    READ_FROM_DATASET(DcmDecimalString, windowCenter)
    READ_FROM_DATASET(DcmDecimalString, windowWidth)
    READ_FROM_DATASET(DcmLongString, windowCenterWidthExplanation)
    READ_FROM_DATASET(DcmCodeString, presentationLUTShape)
  }
  
  /* read Presentation LUT Sequence */
  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_PresentationLUTSequence, stack, ESM_fromHere, OFFalse))
    {
      seq=(DcmSequenceOfItems *)stack.top();
      if (seq->card() ==1)
      {
         item = seq->getItem(0);
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)presentationLUTDescriptor.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           presentationLUTDescriptor = *((DcmUnsignedShort *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)presentationLUTExplanation.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           presentationLUTExplanation = *((DcmLongString *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)presentationLUTData.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           presentationLUTData = *((DcmUnsignedShort *)(stack.top()));
         }
      } else {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: Presentation LUT SQ does not have exactly one item in presentation state" << endl;
#endif
      } 
    }
  }

  /* read Modality LUT Sequence */
  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_ModalityLUTSequence, stack, ESM_fromHere, OFFalse))
    {
      seq=(DcmSequenceOfItems *)stack.top();
      if (seq->card() ==1)
      {
         item = seq->getItem(0);
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)modalityLUTDescriptor.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           modalityLUTDescriptor = *((DcmUnsignedShort *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)modalityLUTExplanation.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           modalityLUTExplanation = *((DcmLongString *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)modalityLUTData.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           modalityLUTData = *((DcmUnsignedShort *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)modalityLUTType.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           modalityLUTType = *((DcmLongString *)(stack.top()));
         }
      } else {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: Modality LUT SQ does not have exactly one item in presentation state" << endl;
#endif
      } 
    }
  }

  /* read VOI LUT Sequence */
  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_VOILUTSequence, stack, ESM_fromHere, OFFalse))
    {
      seq=(DcmSequenceOfItems *)stack.top();
      if (seq->card() ==1)
      {
         item = seq->getItem(0);
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)voiLUTDescriptor.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           voiLUTDescriptor = *((DcmUnsignedShort *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)voiLUTExplanation.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           voiLUTExplanation = *((DcmLongString *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)voiLUTData.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           voiLUTData = *((DcmUnsignedShort *)(stack.top()));
         }
      } else {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: VOI LUT SQ does not have exactly one item in presentation state" << endl;
#endif
      } 
    }
  }

  if (result==EC_Normal) result = overlayList.read(dset);
  if (result==EC_Normal) result = activationLayerList.read(dset);
  if (result==EC_Normal) result = graphicLayerList.read(dset);
  if (result==EC_Normal) result = referencedSeriesList.read(dset);
  if (result==EC_Normal) result = graphicAnnotationList.read(dset);

  /* Now perform basic sanity checks and adjust use flags */
  
  if (result==EC_Normal)
  { 

  if (patientName.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: patientName absent or empty in presentation state" << endl;
#endif
  }
  else if (patientName.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: patientName VM != 1 in presentation state" << endl;
#endif
  }

  if (studyInstanceUID.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: studyInstanceUID absent or empty in presentation state" << endl;
#endif
  }
  else if (studyInstanceUID.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: studyInstanceUID VM != 1 in presentation state" << endl;
#endif
  }

  if (displayedAreaTLHC.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: displayedAreaTLHC absent or empty in presentation state" << endl;
#endif
  }
  else if (displayedAreaTLHC.getVM() != 2)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: displayedAreaTLHC VM != 2 in presentation state" << endl;
#endif
  }

  if (displayedAreaBRHC.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: displayedAreaBRHC absent or empty in presentation state" << endl;
#endif
  }
  else if (displayedAreaBRHC.getVM() != 2)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: displayedAreaBRHC VM != 2 in presentation state" << endl;
#endif
  }

  if (imageNumber.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: instanceNumber absent or empty in presentation state" << endl;
#endif
  }
  else if (imageNumber.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: instanceNumber VM != 1 in presentation state" << endl;
#endif
  }

  if (presentationLabel.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentationLabel absent or empty in presentation state" << endl;
#endif
  }
  else if (presentationLabel.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentationLabel VM != 1 in presentation state" << endl;
#endif
  }

  if (presentationCreationDate.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentationCreationDate absent or empty in presentation state" << endl;
#endif
  }
  else if (presentationCreationDate.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentationCreationDate VM != 1 in presentation state" << endl;
#endif
  }

  if (presentationCreationTime.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentationCreationTime absent or empty in presentation state" << endl;
#endif
  }
  else if (presentationCreationTime.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentationCreationTime VM != 1 in presentation state" << endl;
#endif
  }

  if (sOPInstanceUID.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: sOPInstanceUID absent or empty in presentation state" << endl;
#endif
  }
  else if (sOPInstanceUID.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: sOPInstanceUID VM != 1 in presentation state" << endl;
#endif
  }

    /* Presentation LUT */
    if (presentationLUTShape.getLength() == 0)
    {
      presentationLUT = DVPSP_table;

      if (presentationLUTDescriptor.getLength() == 0)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: presentationLUTShape and presentationLUTDescriptor absent or empty in presentation state" << endl;
#endif
      }
      else if (presentationLUTDescriptor.getVM() != 3)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: presentationLUTDescriptor present but VM != 3 in presentation state" << endl;
#endif
      }
      if (presentationLUTData.getLength() == 0)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: presentationLUTShape and presentationLUTData absent or empty in presentation state" << endl;
#endif
      }
    } else {
      if (presentationLUTShape.getVM() != 1)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: presentationLUTShape present but VM != 1 in presentation state" << endl;
#endif
      } else {
      	// check presentation LUT shape
      	aString.clear();
      	presentationLUTShape.getOFString(aString,0);
      	if (aString=="IDENTITY") presentationLUT = DVPSP_identity;
      	else if (aString=="INVERSE") presentationLUT = DVPSP_inverse;
      	else
      	{
      	  result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: unknown presentationLUTShape keyword: " << aString << endl;
#endif
      	}
      }
    }

    /* if imageRotation or imageHorizontalFlip are present, then both must be present. */
    if ((imageRotation.getLength() > 0)&&(imageHorizontalFlip.getLength() == 0))
    {
      result=EC_IllegalCall;
#ifdef DEBUG
      cerr << "Error: imageRotation present but imageHorizontalFlip absent or empty in presentation state" << endl;
#endif
    }
    
    if ((imageRotation.getLength() == 0)&&(imageHorizontalFlip.getLength() > 0))
    {
      result=EC_IllegalCall;
#ifdef DEBUG
      cerr << "Error: imageHorizontalFlip present but imageRotation absent or empty in presentation state" << endl;
#endif
    }
   
    /* Modality LUT */
    if (rescaleIntercept.getLength() > 0)
    {
      useModalityRescale = OFTrue;

      if (rescaleSlope.getLength() == 0)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: rescaleIntercept present but rescaleSlope absent or empty in presentation state" << endl;
#endif
      }
      else if (rescaleSlope.getVM() != 1)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: rescaleIntercept present but rescaleSlope VM != 1 in presentation state" << endl;
#endif
      }
      if (rescaleType.getLength() == 0)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: rescaleIntercept present but rescaleType absent or empty in presentation state" << endl;
#endif
      }
      else if (rescaleType.getVM() != 1)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: rescaleIntercept present but rescaleType VM != 1 in presentation state" << endl;
#endif
      }
      if (rescaleIntercept.getVM() != 1)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: rescaleIntercept present but VM != 1 in presentation state" << endl;
#endif
      }
    } else useModalityRescale = OFFalse;
    if (modalityLUTData.getLength() > 0)
    {
      useModalityLUT = OFTrue;

      if (modalityLUTDescriptor.getLength() == 0)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: modalityLUTData present but modalityLUTDescriptor absent or empty in presentation state" << endl;
#endif
      }
      else if (modalityLUTDescriptor.getVM() != 3)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: modalityLUTData present but modalityLUTDescriptor VM != 3 in presentation state" << endl;
#endif
      }

    } else useModalityLUT = OFFalse;
    /* we may not have both modality rescale and LUT */
    if (useModalityRescale && useModalityLUT)
    {
      result=EC_IllegalCall;
#ifdef DEBUG
      cerr << "Error: both modality rescale and LUT present in presentation state" << endl;
#endif
    }

    /* VOI LUT */
    if (windowCenter.getLength() > 0)
    {
      useVOIWindow = OFTrue;
      
      if (windowWidth.getLength() == 0)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: windowCenter present but windowWidth absent or empty in presentation state" << endl;
#endif
      }
      else if (windowWidth.getVM() != 1)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: windowCenter present but windowWidth VM != 1 in presentation state" << endl;
#endif
      }
      if (windowCenter.getVM() != 1)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: windowCenter present but VM != 1 in presentation state" << endl;
#endif
      }
    } else useVOIWindow = OFFalse;
    if (voiLUTData.getLength() > 0)
    {
      useVOILUT = OFTrue;

      if (voiLUTDescriptor.getLength() == 0)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: voiLUTData present but voiLUTDescriptor absent or empty in presentation state" << endl;
#endif
      }
      else if (voiLUTDescriptor.getVM() != 3)
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: voiLUTData present but voiLUTDescriptor VM != 3 in presentation state" << endl;
#endif
      }
    } else useVOILUT = OFFalse;
    /* we may not have both VOI window and LUT */
    if (useVOIWindow && useVOILUT)
    {
      result=EC_IllegalCall;
#ifdef DEBUG
      cerr << "Error: both VOI window and LUT present in presentation state" << endl;
#endif
    }

  }
 
  /* check shutters */
  if (result==EC_Normal)
  { 
    // now we examine the shutterShape for the Shutter modules
    Uint32 i;
    aString.clear();
    useShutterRectangular=OFFalse;
    useShutterCircular=OFFalse;
    useShutterPolygonal=OFFalse;
    useShutterBitmap=OFFalse;
    for (i=0; i<shutterShape.getVM(); i++)
    {
      shutterShape.getOFString(aString, i, OFTrue);
      if (aString == "RECTANGULAR") useShutterRectangular=OFTrue;
      else if (aString == "CIRCULAR") useShutterCircular=OFTrue;
      else if (aString == "POLYGONAL") useShutterPolygonal=OFTrue;
      else if (aString == "BITMAP") useShutterBitmap=OFTrue;
      else result=EC_IllegalCall; /* reject presentation state with unknown shutter type */
    }
    
    if (useShutterRectangular)
    {
      if ((shutterLeftVerticalEdge.getLength() == 0)||(shutterLeftVerticalEdge.getVM() != 1)) 
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: rectangular shutter used but shutterLeftVerticalEdge absent or VM != 1 in presentation state" << endl;
#endif
      }
      if ((shutterRightVerticalEdge.getLength() == 0)||(shutterRightVerticalEdge.getVM() != 1))
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: rectangular shutter used but shutterRightVerticalEdge absent or VM != 1 in presentation state" << endl;
#endif
      }
      if ((shutterUpperHorizontalEdge.getLength() == 0)||(shutterUpperHorizontalEdge.getVM() != 1))
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: rectangular shutter used but shutterUpperHorizontalEdge absent or VM != 1 in presentation state" << endl;
#endif
      }
      if ((shutterLowerHorizontalEdge.getLength() == 0)||(shutterLowerHorizontalEdge.getVM() != 1))
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: rectangular shutter used but shutterLowerHorizontalEdge absent or VM != 1 in presentation state" << endl;
#endif
      }
    }
    if (useShutterCircular)
    {
      if ((centerOfCircularShutter.getLength() == 0)||(centerOfCircularShutter.getVM() != 2))
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: circular shutter used but centerOfCircularShutter absent or VM != 2 in presentation state" << endl;
#endif
      }
      if ((radiusOfCircularShutter.getLength() == 0)||(radiusOfCircularShutter.getVM() != 1))
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: circular shutter used but radiusOfCircularShutter absent or VM != 1 in presentation state" << endl;
#endif
      }
    }
    if (useShutterPolygonal)
    {
      /* check that VM == 2-2n */
      if ((verticesOfThePolygonalShutter.getLength() == 0) ||
          (verticesOfThePolygonalShutter.getVM() < 2) ||
          ((verticesOfThePolygonalShutter.getVM() % 2) != 0)) 
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: polygonal shutter used but verticesOfThePolygonalShutter absent or VM != 2-2n in presentation state" << endl;
#endif
      }
    }
    if (useShutterBitmap)
    {
      if ((shutterOverlayGroup.getLength() == 0)||(shutterOverlayGroup.getVM() != 1)) 
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: bitmap shutter used but shutterOverlayGroup absent or VM != 1 in presentation state" << endl;
#endif
      }
    }
    if (useShutterRectangular || useShutterCircular || useShutterPolygonal || useShutterBitmap)
    {
      if ((shutterPresentationValue.getLength() == 0)||(shutterPresentationValue.getVM() != 1)) 
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: shutter used but shutterPresentationValue absent or VM != 1 in presentation state" << endl;
#endif
      }
    }
    /* we can either have rect/circ/poly shutter or bitmap shutter but not both */
    if (useShutterBitmap && (useShutterRectangular || useShutterCircular || useShutterPolygonal))
    {
      result=EC_IllegalCall;
#ifdef DEBUG
      cerr << "Error: both bitmap and rect/circ/poly shutter specified in presentation state" << endl;
#endif
    }
  }
  
  /* check that overlays referenced as bitmap shutter really exist */
  if ((result==EC_Normal) && useShutterBitmap)
  {
    Uint16 shuttergroup = 0;
    shutterOverlayGroup.getUint16(shuttergroup);
    if (shuttergroup >= 0x6000)
    {
      if (!overlayList.haveOverlayGroup(shuttergroup)) 
      {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: bitmap shutter specified but referenced overlay group missing in presentation state" << endl;
#endif
      }
    } else {
        result=EC_IllegalCall;
#ifdef DEBUG
        cerr << "Error: wrong overlay group specified for bitmap shutter in presentation state" << endl;
#endif
    }
  }
  
  /* check that referenced series list is non-empty and that every series contains images 
     and that all referenced images share the same SOP class UID */
  if (!referencedSeriesList.isValid()) result = EC_IllegalCall;
  
  /* check that there is no mask module present. We don't support Mask and therefore
     are obliged to reject any presentation state containing a mask! */
  
  stack.clear();
  if (EC_Normal == dset.search(DCM_MaskSubtractionSequence, stack, ESM_fromHere, OFFalse))
  {
     result=EC_IllegalCall;
#ifdef DEBUG
     cerr << "Error: mask module present in presentation state, but unsupported" << endl;
#endif
  }
  stack.clear();
  if (EC_Normal == dset.search(DCM_RecommendedViewingMode, stack, ESM_fromHere, OFFalse))
  {
     result=EC_IllegalCall;
#ifdef DEBUG
     cerr << "Error: mask module present in presentation state, but unsupported" << endl;
#endif
  }
    
  return result;
}


E_Condition DVPresentationState::createFromImage(
  DcmItem &dset,
  DVPSoverlayActivation overlayActivation,
  DVPSVOIActivation voiActivation,
  OFBool curveActivation,
  OFBool shutterActivation,
  OFBool presentationActivation,
  DVPSGraphicLayering layering)
{
  E_Condition result = EC_Normal;
  DcmStack stack;
  DcmSequenceOfItems *seq;
  DcmItem *item;
  char uid[100];
  OFString aString;
  OFBool isMonochrome1 = OFFalse;
  
  DcmUnsignedShort rows(DCM_Rows);
  DcmUnsignedShort columns(DCM_Columns);
  DcmCodeString photometricInterpretation(DCM_PhotometricInterpretation);
  DcmCodeString presentationLUTShape(DCM_PresentationLUTShape);
 
  clear(); // re-initialize Presentation State object 

  /* copy some image attributes we will need later */
  READ_FROM_DATASET(DcmUnsignedShort, rows)
  READ_FROM_DATASET(DcmUnsignedShort, columns)
  READ_FROM_DATASET(DcmCodeString, photometricInterpretation)

  /* copy attributes for Patient, General Study and General Equipment
   * modules from image object. Also copy specific character set (SOP common). */
  READ_FROM_DATASET(DcmPersonName, patientName)
  READ_FROM_DATASET(DcmLongString, patientID)
  READ_FROM_DATASET(DcmDate, patientBirthDate)
  READ_FROM_DATASET(DcmCodeString, patientSex)
  READ_FROM_DATASET(DcmUniqueIdentifier, studyInstanceUID)
  READ_FROM_DATASET(DcmDate, studyDate)
  READ_FROM_DATASET(DcmTime, studyTime)
  READ_FROM_DATASET(DcmPersonName, referringPhysiciansName)
  READ_FROM_DATASET(DcmShortString, studyID)
  READ_FROM_DATASET(DcmShortString, accessionNumber)
  READ_FROM_DATASET(DcmLongString, manufacturer)
  READ_FROM_DATASET(DcmCodeString, specificCharacterSet)

  /* if there is a modality rescale or VOI window, copy it now. 
   * Later we decide whether we want to use it.
   */
  READ_FROM_DATASET(DcmDecimalString, rescaleIntercept)
  READ_FROM_DATASET(DcmDecimalString, rescaleSlope)
  READ_FROM_DATASET(DcmLongString, rescaleType) 
  READ_FROM_DATASET(DcmDecimalString, windowCenter)
  READ_FROM_DATASET(DcmDecimalString, windowWidth)
  READ_FROM_DATASET(DcmLongString, windowCenterWidthExplanation)

  if ((result==EC_Normal) && shutterActivation)
  {
    /* if there is a display shutter in the image (i.e. in XA, RF, DX), copy it. 
     * We never copy bitmap shutters.
     */
    READ_FROM_DATASET(DcmCodeString, shutterShape)
    READ_FROM_DATASET(DcmIntegerString, shutterLeftVerticalEdge)
    READ_FROM_DATASET(DcmIntegerString, shutterRightVerticalEdge)
    READ_FROM_DATASET(DcmIntegerString, shutterUpperHorizontalEdge)
    READ_FROM_DATASET(DcmIntegerString, shutterLowerHorizontalEdge)
    READ_FROM_DATASET(DcmIntegerString, centerOfCircularShutter) 
    READ_FROM_DATASET(DcmIntegerString, radiusOfCircularShutter)
    READ_FROM_DATASET(DcmIntegerString, verticesOfThePolygonalShutter)
    READ_FROM_DATASET(DcmUnsignedShort, shutterPresentationValue)
  }
 
  if ((result==EC_Normal) && presentationActivation)
  { 
    /* if there is a Presentation LUT shape in the image (i.e. in DX), copy it. 
     * We never copy presentation LUT sequences.
     */
    READ_FROM_DATASET(DcmCodeString, presentationLUTShape)
  }
      
  /* read Modality LUT Sequence */
  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_ModalityLUTSequence, stack, ESM_fromHere, OFFalse))
    {
      seq=(DcmSequenceOfItems *)stack.top();
      if (seq->card() >0)
      {
         item = seq->getItem(0);
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)modalityLUTDescriptor.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           modalityLUTDescriptor = *((DcmUnsignedShort *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)modalityLUTExplanation.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           modalityLUTExplanation = *((DcmLongString *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)modalityLUTData.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           modalityLUTData = *((DcmUnsignedShort *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)modalityLUTType.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           modalityLUTType = *((DcmLongString *)(stack.top()));
         }
      } else result=EC_TagNotFound;
    }
  }

  /* read VOI LUT Sequence */
  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_VOILUTSequence, stack, ESM_fromHere, OFFalse))
    {
      seq=(DcmSequenceOfItems *)stack.top();
      if (seq->card() > 0)
      {
         item = seq->getItem(0);
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)voiLUTDescriptor.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           voiLUTDescriptor = *((DcmUnsignedShort *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)voiLUTExplanation.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           voiLUTExplanation = *((DcmLongString *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)voiLUTData.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           voiLUTData = *((DcmUnsignedShort *)(stack.top()));
         }
      } else result=EC_TagNotFound;
    }
  }

  /* copy overlays if appropriate */
  if ((result==EC_Normal)&&(overlayActivation==DVPSO_copyOverlays)) result = overlayList.read(dset);

  /* Now perform basic sanity checks and adjust use flags */
  
  if ((studyInstanceUID.getLength() == 0)||(studyInstanceUID.getVM() != 1)) result=EC_IllegalCall;


  if (photometricInterpretation.getVM() == 1)
  {
    aString.clear();
    photometricInterpretation.getOFString(aString,0);
    if ((aString == "MONOCHROME1")||(aString == "MONOCHROME 1")) isMonochrome1 = OFTrue;
    else if ((aString != "MONOCHROME2")&&(aString != "MONOCHROME 2"))
    {
      result = EC_IllegalCall;
#ifdef DEBUG
      cerr << "Error: Wrong image photometric interpretation - not MONOCHROME1/2" << endl;
#endif
    }
  }

  if (result==EC_Normal)
  { 
    if ((patientName.getLength() == 0)||(patientName.getVM() != 1)) 
      result = patientName.putString(DEFAULT_patientName);
  }
  if (result==EC_Normal)
  { 
    result = imageNumber.putString(DEFAULT_imageNumber);
  }
  if (result==EC_Normal)
  { 
    result = presentationLabel.putString(DEFAULT_presentationLabel);
  }
  if (result==EC_Normal)
  { 
    aString.clear();
    currentDate(aString);
    result = presentationCreationDate.putString(aString.c_str());
  }
  if (result==EC_Normal)
  { 
    aString.clear();
    currentTime(aString);
    result = presentationCreationTime.putString(aString.c_str());
  }
  if (result==EC_Normal) result = seriesInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
  if (result==EC_Normal) result = sOPInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
  if (result==EC_Normal) result = seriesNumber.putString(DEFAULT_seriesNumber);
  if (result==EC_Normal) result = specificCharacterSet.putString(DEFAULT_specificCharacterSet);

  if (result==EC_Normal)
  {
    /* Presentation LUT Shape */
    if (isMonochrome1) presentationLUT=DVPSP_inverse; else presentationLUT=DVPSP_identity;
    if (presentationLUTShape.getVM() == 1)
    {
      aString.clear();
      presentationLUTShape.getOFString(aString,0);
      if (aString == "IDENTITY") presentationLUT=DVPSP_identity;
      if (aString == "INVERSE") presentationLUT=DVPSP_inverse;     
    }
  }
   
  if (result==EC_Normal)
  {
    /* Modality LUT */
    if ((rescaleIntercept.getVM() == 1) && (rescaleSlope.getVM() == 1) && (rescaleType.getVM() == 1))
    {
      useModalityRescale = OFTrue;
      modalityLUTData.clear();
      modalityLUTDescriptor.clear();
      modalityLUTExplanation.clear();
      modalityLUTType.clear();
    } else useModalityRescale = OFFalse;

    if ((modalityLUTDescriptor.getVM() == 3) && (modalityLUTData.getLength() > 0))
    {
      useModalityLUT = OFTrue;
      rescaleIntercept.clear();
      rescaleSlope.clear();
      rescaleType.clear();
    } else useModalityLUT = OFFalse;
  }       

  if (result==EC_Normal)
  {
    /* VOI LUT */
    if ((windowCenter.getVM() > 0) && (windowWidth.getVM() > 0))
    {
      useVOIWindow = OFTrue;
      // delete all values except the first if VM>1
      aString.clear();
      windowCenter.getOFString(aString, 0);
      windowCenter.putString(aString.c_str());
      aString.clear();
      windowWidth.getOFString(aString, 0);
      windowWidth.putString(aString.c_str());
    } else useVOIWindow = OFFalse;
    if ((voiLUTDescriptor.getVM() == 3) && (voiLUTData.getLength() > 0)) useVOILUT = OFTrue;
    else useVOILUT = OFFalse;
    switch (voiActivation)
    {
      case DVPSV_ignoreVOI:
        useVOIWindow = OFFalse;
        useVOILUT = OFFalse;
        windowCenter.clear();
        windowWidth.clear();
        windowCenterWidthExplanation.clear();
        voiLUTDescriptor.clear();
        voiLUTData.clear();
        voiLUTExplanation.clear();
        break;
      case DVPSV_preferVOIWindow:
        if (useVOIWindow && useVOILUT)
        {
          useVOILUT = OFFalse;
          voiLUTDescriptor.clear();
          voiLUTData.clear();
          voiLUTExplanation.clear();
        }
        break;
      case DVPSV_preferVOILUT:
        if (useVOIWindow && useVOILUT)
        {
          useVOIWindow = OFFalse;
          windowCenter.clear();
          windowWidth.clear();
          windowCenterWidthExplanation.clear();
        }
        break;
    }
  }       

  /* check shutters */
  if (result==EC_Normal)
  { 
    // now we examine the shutterShape for the Shutter modules
    Uint32 i;
    aString.clear();
    useShutterRectangular=OFFalse;
    useShutterCircular=OFFalse;
    useShutterPolygonal=OFFalse;
    useShutterBitmap=OFFalse;
    for (i=0; i<shutterShape.getVM(); i++)
    {
      shutterShape.getOFString(aString, i, OFTrue);
      if (aString == "RECTANGULAR") useShutterRectangular=OFTrue;
      if (aString == "CIRCULAR") useShutterCircular=OFTrue;
      if (aString == "POLYGONAL") useShutterPolygonal=OFTrue;
      else { /* forget this shutter! */ }
    }
    
    if (useShutterRectangular)
    {
      if ((shutterLeftVerticalEdge.getLength() == 0)||(shutterLeftVerticalEdge.getVM() != 1)) useShutterRectangular=OFFalse;
      if ((shutterRightVerticalEdge.getLength() == 0)||(shutterRightVerticalEdge.getVM() != 1)) useShutterRectangular=OFFalse;
      if ((shutterUpperHorizontalEdge.getLength() == 0)||(shutterUpperHorizontalEdge.getVM() != 1)) useShutterRectangular=OFFalse;
      if ((shutterLowerHorizontalEdge.getLength() == 0)||(shutterLowerHorizontalEdge.getVM() != 1)) useShutterRectangular=OFFalse;
    }
    if (useShutterCircular)
    {
      if ((centerOfCircularShutter.getLength() == 0)||(centerOfCircularShutter.getVM() != 2)) useShutterCircular=OFFalse;
      if ((radiusOfCircularShutter.getLength() == 0)||(radiusOfCircularShutter.getVM() != 1)) useShutterCircular=OFFalse;
    }
    if (useShutterPolygonal)
    {
      /* check that VM == 2-2n */
      if ((verticesOfThePolygonalShutter.getLength() == 0) ||
          (verticesOfThePolygonalShutter.getVM() < 2) ||
          ((verticesOfThePolygonalShutter.getVM() % 2) != 0)) useShutterPolygonal=OFFalse;
    }
    if (useShutterRectangular || useShutterCircular || useShutterPolygonal)
    {
      aString.clear();
      if (useShutterRectangular) aString = "RECTANGULAR";
      if (useShutterCircular)
      {
        if (aString.size() >0) aString += '\\';
        aString += "CIRCULAR";
      }
      if (useShutterPolygonal)
      {
        if (aString.size() >0) aString += '\\';
        aString += "POLYGONAL";
      }
      shutterShape.putString(aString.c_str());
      if ((shutterPresentationValue.getLength() == 0)||(shutterPresentationValue.getVM() != 1))
      {
        shutterPresentationValue.clear();
        shutterPresentationValue.putUint16(DEFAULT_shutterPresentationValue,0);
      }
    }
  }
   
  /* browse the image for overlays and curves, create activation tags and
   * graphic layers as appropriate.
   */
  if (result==EC_Normal) result = activationLayerList.createFromImage(dset,
    graphicLayerList, overlayList, overlayActivation, curveActivation, layering);

  /* set displayedAreaTLHC/BRHC to image size */

  displayedAreaTLHC.putUint16(1,0);
  displayedAreaTLHC.putUint16(1,1);
  if (result==EC_Normal)
  {
    Uint16 ui=0;
    result = columns.getUint16(ui,0);
    if (result == EC_Normal)
    {
      displayedAreaBRHC.putUint16(ui,0);
      result = rows.getUint16(ui,0);
      if (result == EC_Normal) displayedAreaBRHC.putUint16(ui,1);
    }
  }

  /* create reference for this image */
  if (EC_Normal == result) result = addImageReference(dset);

  return result;
}


E_Condition DVPresentationState::write(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmElement *delem=NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;
  DcmCodeString presentationLUTShape(DCM_PresentationLUTShape);
  
  cleanupLayers(); /* remove unused layers */
  createDummyValues();
  
  /* add SOP Class UID */  
  DcmUniqueIdentifier sopclassuid(DCM_SOPClassUID);
  if (result==EC_Normal)
  {
     result = sopclassuid.putString(UID_GrayscaleSoftcopyPresentationStateStorage);
  }
  ADD_TO_DATASET(DcmUniqueIdentifier, sopclassuid)

  /* add Modality */  
  DcmCodeString modality(DCM_Modality);
  if (result==EC_Normal)
  {
     result = modality.putString("PR");
  }
  ADD_TO_DATASET(DcmCodeString, modality)
  
  ADD_TO_DATASET(DcmPersonName, patientName)
  ADD_TO_DATASET(DcmLongString, patientID)
  ADD_TO_DATASET(DcmDate, patientBirthDate)
  ADD_TO_DATASET(DcmCodeString, patientSex)
  ADD_TO_DATASET(DcmUniqueIdentifier, studyInstanceUID)
  ADD_TO_DATASET(DcmDate, studyDate)
  ADD_TO_DATASET(DcmTime, studyTime)
  ADD_TO_DATASET(DcmPersonName, referringPhysiciansName)
  ADD_TO_DATASET(DcmShortString, studyID)
  ADD_TO_DATASET(DcmShortString, accessionNumber)
  ADD_TO_DATASET(DcmCodeString, modality)
  ADD_TO_DATASET(DcmUniqueIdentifier, seriesInstanceUID)
  ADD_TO_DATASET(DcmIntegerString, seriesNumber)
  ADD_TO_DATASET(DcmLongString, manufacturer)
  ADD_TO_DATASET(DcmUnsignedShort, displayedAreaTLHC)
  ADD_TO_DATASET(DcmUnsignedShort, displayedAreaBRHC)

  if (presentationLUT==DVPSP_table)
  {
    if (result == EC_Normal)
    {
      ditem = new DcmItem();
      if (ditem)
      {
        dseq = new DcmSequenceOfItems(DCM_PresentationLUTSequence);
        if (dseq)
        {
          delem = new DcmUnsignedShort(presentationLUTDescriptor);
          if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
          delem = new DcmUnsignedShort(presentationLUTData);
          if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
          if (presentationLUTExplanation.getLength() >0)
          {
            delem = new DcmLongString(presentationLUTExplanation);
            if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
          }
          if (result==EC_Normal)
          {
            dseq->insert(ditem);
            dset.insert(dseq);
          } else {
            // out of memory during creation of sequence contents.
            delete dseq;
            delete ditem;
            result = EC_MemoryExhausted;
          }
        } else {
          // could allocate item but not sequence. Bail out.
          delete ditem;
          result = EC_MemoryExhausted;
        }
      }
      else result = EC_MemoryExhausted;
    }
  } else {
    if (presentationLUT==DVPSP_inverse) presentationLUTShape.putString("INVERSE");
    else presentationLUTShape.putString("IDENTITY");
    ADD_TO_DATASET(DcmCodeString, presentationLUTShape)
  }
 
  ADD_TO_DATASET(DcmIntegerString, imageNumber)
  ADD_TO_DATASET(DcmCodeString, presentationLabel)
  ADD_TO_DATASET(DcmLongString, presentationDescription)
  ADD_TO_DATASET(DcmDate, presentationCreationDate)
  ADD_TO_DATASET(DcmTime, presentationCreationTime)
  ADD_TO_DATASET(DcmPersonName, presentationCreatorsName)

  referencedSeriesList.write(dset);

  ADD_TO_DATASET(DcmUniqueIdentifier, sOPInstanceUID)
  if (specificCharacterSet.getLength() >0) { ADD_TO_DATASET(DcmCodeString, specificCharacterSet) }
  if (instanceCreationDate.getLength() >0) { ADD_TO_DATASET(DcmDate, instanceCreationDate) }
  if (instanceCreationTime.getLength() >0) { ADD_TO_DATASET(DcmTime, instanceCreationTime) }
  if (instanceCreatorUID.getLength() >0) { ADD_TO_DATASET(DcmUniqueIdentifier, instanceCreatorUID) }

  if (useShutterRectangular || useShutterCircular || useShutterPolygonal)
  {
    ADD_TO_DATASET(DcmCodeString, shutterShape)
    ADD_TO_DATASET(DcmUnsignedShort, shutterPresentationValue)
    if (useShutterRectangular)
    {  
      ADD_TO_DATASET(DcmIntegerString, shutterLeftVerticalEdge)
      ADD_TO_DATASET(DcmIntegerString, shutterRightVerticalEdge)
      ADD_TO_DATASET(DcmIntegerString, shutterUpperHorizontalEdge)
      ADD_TO_DATASET(DcmIntegerString, shutterLowerHorizontalEdge)
    }
    if (useShutterCircular)
    {  
      ADD_TO_DATASET(DcmIntegerString, centerOfCircularShutter)
      ADD_TO_DATASET(DcmIntegerString, radiusOfCircularShutter)
    }
    if (useShutterPolygonal)
    {  
      ADD_TO_DATASET(DcmIntegerString, verticesOfThePolygonalShutter)
    }
  }
  else if (useShutterBitmap)
  {
    ADD_TO_DATASET(DcmCodeString, shutterShape)
    ADD_TO_DATASET(DcmUnsignedShort, shutterPresentationValue)
    ADD_TO_DATASET(DcmUnsignedShort, shutterOverlayGroup)
  }

  overlayList.write(dset);
  activationLayerList.write(dset);
  graphicAnnotationList.write(dset);
  
  // strictly speaking we are not allowed to include the Spatial Transformation
  // Module if neither rotation nor flipping are needed.
  OFString aString;
  OFBool haveRotation=OFFalse;
  OFBool haveFlip=OFFalse;
  if (imageRotation.getLength() > 0)
  {
    imageRotation.getOFString(aString, 0, OFTrue);
    if (!(aString == "0")) haveRotation=OFTrue;
  }
  if (imageHorizontalFlip.getLength() > 0)
  {
    imageHorizontalFlip.getOFString(aString, 0, OFTrue);
    if (aString == "Y") haveFlip=OFTrue;
  }

  if (haveRotation || haveFlip)
  {
    ADD_TO_DATASET(DcmIntegerString, imageRotation)
    ADD_TO_DATASET(DcmCodeString, imageHorizontalFlip)
  }
  
  graphicLayerList.write(dset);

  if (useModalityRescale)
  {
    ADD_TO_DATASET(DcmDecimalString, rescaleIntercept)
    ADD_TO_DATASET(DcmDecimalString, rescaleSlope)
    ADD_TO_DATASET(DcmLongString, rescaleType)
  } 
  else if (useModalityLUT)
  {
    if (result == EC_Normal)
    {
      ditem = new DcmItem();
      if (ditem)
      {
        dseq = new DcmSequenceOfItems(DCM_ModalityLUTSequence);
        if (dseq)
        {
          delem = new DcmUnsignedShort(modalityLUTDescriptor);
          if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
          delem = new DcmUnsignedShort(modalityLUTData);
          if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
          delem = new DcmLongString(modalityLUTType);
          if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
          if (modalityLUTExplanation.getLength() >0)
          {
            delem = new DcmLongString(modalityLUTExplanation);
            if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
          }
          if (result==EC_Normal)
          {
            dseq->insert(ditem);
            dset.insert(dseq);
          } else {
            // out of memory during creation of sequence contents.
            delete dseq;
            delete ditem;
            result = EC_MemoryExhausted;
          }
        } else {
          // could allocate item but not sequence. Bail out.
          delete ditem;
          result = EC_MemoryExhausted;
        }
      }
      else result = EC_MemoryExhausted;
    }
  }

  if (useVOIWindow)
  {
    ADD_TO_DATASET(DcmDecimalString, windowCenter)
    ADD_TO_DATASET(DcmDecimalString, windowWidth)
    if (windowCenterWidthExplanation.getLength() > 0) { ADD_TO_DATASET(DcmLongString, windowCenterWidthExplanation) }
  }
  else if (useVOILUT)
  {
    if (result == EC_Normal)
    {
      ditem = new DcmItem();
      if (ditem)
      {
        dseq = new DcmSequenceOfItems(DCM_VOILUTSequence);
        if (dseq)
        {
          delem = new DcmUnsignedShort(voiLUTDescriptor);
          if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
          delem = new DcmUnsignedShort(voiLUTData);
          if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
          if (voiLUTExplanation.getLength() >0)
          {
            delem = new DcmLongString(voiLUTExplanation);
            if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
          }
          if (result==EC_Normal)
          {
            dseq->insert(ditem);
            dset.insert(dseq);
          } else {
            // out of memory during creation of sequence contents.
            delete dseq;
            delete ditem;
            result = EC_MemoryExhausted;
          }
        } else {
          // could allocate item but not sequence. Bail out.
          delete ditem;
          result = EC_MemoryExhausted;
        }
      }
      else result = EC_MemoryExhausted;
    }
  }
  
  return result;
}


E_Condition DVPresentationState::attachImage(DcmDataset *dataset, OFBool transferOwnership)
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
#ifdef DEBUG
      cerr << "warning: unable to evaluate Modality Rescale Slope, ignoring." << endl;
#endif
      slope = 1.0;
    }
    if (EC_Normal != rescaleIntercept.getFloat64(intercept, 0))
    {
#ifdef DEBUG
      cerr << "warning: unable to evaluate Modality Rescale Slope, ignoring." << endl;
#endif
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
  
  E_Condition result = EC_Normal;
  if (image)
  {
    if (EIS_Normal == image->getStatus())
    {
      detachImage();
      currentImage = image;
      currentImageWidth = image->getWidth();
      currentImageHeight = image->getHeight();
      currentImageDataset = dataset;
      currentImageOwned = transferOwnership;
      result = currentImageCurveList.read(*dataset);
      if (EC_Normal==result) result = currentImageVOILUTList.read(*dataset);
      if (EC_Normal==result) result = currentImageVOIWindowList.read(*dataset);      
    } else {
      delete image;
      result = EC_IllegalCall;
    }
  } else result = EC_MemoryExhausted;
  return result;
}

E_Condition DVPresentationState::getImageWidth(unsigned long &width)
{
  E_Condition result=EC_Normal;
  if (currentImage) width = currentImageWidth;
  else
  {
    width = 0;
    result = EC_IllegalCall;
  }
  return result;
}

E_Condition DVPresentationState::getImageHeight(unsigned long &height)
{
  E_Condition result=EC_Normal;
  if (currentImage) height = currentImageHeight;
  else
  {
    height = 0;
    result = EC_IllegalCall;
  }
  return result;
}


E_Condition DVPresentationState::attachImage(DcmFileFormat *fileformat, OFBool transferOwnership)
{
  if (fileformat == NULL) return EC_IllegalCall;
  E_Condition result = attachImage(fileformat->getDataset(), transferOwnership);
  if (EC_Normal == result) currentImageFileformat = fileformat;
  return result;
}


E_Condition DVPresentationState::addImageReference(
    const char *studyUID, 
    const char *seriesUID, 
    const char *sopclassUID, 
    const char *instanceUID,
    const char *frames)
{
  if ((studyUID==NULL)||(seriesUID)||(sopclassUID==NULL)||(instanceUID==NULL)) return EC_IllegalCall;

  OFString study;
  studyInstanceUID.getOFString(study,0);
  if (study != studyUID)
  {
#ifdef DEBUG
    cerr << "Error: cannot add reference to image with different Study Instance UID." << endl;
#endif
    return EC_IllegalCall;
  }
  return referencedSeriesList.addImageReference(seriesUID, sopclassUID, instanceUID, frames);
}


E_Condition DVPresentationState::addImageReference(DcmItem &dset)
{
  DcmIntegerString numberOfFrames(DCM_NumberOfFrames);
  DcmUniqueIdentifier studyUID(DCM_StudyInstanceUID);
  DcmUniqueIdentifier seriesUID(DCM_SeriesInstanceUID);
  DcmUniqueIdentifier sopclassUID(DCM_SOPClassUID);
  DcmUniqueIdentifier imageUID(DCM_SOPInstanceUID);

  OFString ofstudyInstanceUID;
  OFString ofstudyUID;
  OFString ofseriesUID;
  OFString ofsopclassUID;
  OFString ofimageUID;
  OFString aString;
  char buf[20];
  Sint32 i=0;
  Sint32 ofnumberOfFrames=0;
  DcmStack stack;
  
  READ_FROM_DATASET(DcmIntegerString, numberOfFrames)
  READ_FROM_DATASET(DcmUniqueIdentifier, studyUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, seriesUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, sopclassUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, imageUID)
        
  numberOfFrames.getSint32(ofnumberOfFrames,0);
  seriesUID.getOFString(ofseriesUID,0);
  sopclassUID.getOFString(ofsopclassUID,0);
  imageUID.getOFString(ofimageUID,0);
  studyUID.getOFString(ofstudyUID,0);
  studyInstanceUID.getOFString(ofstudyInstanceUID,0);

  if (ofstudyInstanceUID != ofstudyUID)
  {
#ifdef DEBUG
    cerr << "Error: cannot add reference to image with different Study Instance UID." << endl;
#endif
    return EC_IllegalCall;
  }
     
  E_Condition result = EC_Normal;
  /* if the image is multiframe, reference all frames */
  if (ofnumberOfFrames > 1) 
  {
    for (i=0; i<ofnumberOfFrames; i++)
    {
    	if (aString.length() > 0) sprintf(buf, "\\%ld", i+1); else sprintf(buf, "%ld", i+1);
    	aString += buf;
    }
    result = referencedSeriesList.addImageReference(
      ofseriesUID.c_str(), ofsopclassUID.c_str(), ofimageUID.c_str(), aString.c_str());
  } else {
    result = referencedSeriesList.addImageReference(
      ofseriesUID.c_str(), ofsopclassUID.c_str(), ofimageUID.c_str());
  }   
  return result;
}


E_Condition DVPresentationState::addImageReferenceAttached()
{
  if (currentImageDataset) return addImageReference(*currentImageDataset);
  else return EC_IllegalCall;
}


E_Condition DVPresentationState::removeImageReference(
    const char *studyUID, 
    const char *seriesUID, 
    const char *instanceUID)
{
  if ((studyUID==NULL)||(seriesUID)||(instanceUID==NULL)) return EC_IllegalCall;

  OFString study;
  studyInstanceUID.getOFString(study,0);
  if (study != studyUID) return EC_IllegalCall; 
  referencedSeriesList.removeImageReference(seriesUID, instanceUID);
  return EC_Normal;
}
     

E_Condition DVPresentationState::removeImageReference(DcmItem &dset)
{
  DcmUniqueIdentifier studyUID(DCM_StudyInstanceUID);
  DcmUniqueIdentifier seriesUID(DCM_SeriesInstanceUID);
  DcmUniqueIdentifier imageUID(DCM_SOPInstanceUID);

  OFString ofstudyInstanceUID;
  OFString ofstudyUID;
  OFString ofseriesUID;
  OFString ofimageUID;
  DcmStack stack;
  
  READ_FROM_DATASET(DcmUniqueIdentifier, studyUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, seriesUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, imageUID)
        
  seriesUID.getOFString(ofseriesUID,0);
  studyInstanceUID.getOFString(ofstudyInstanceUID,0);
  if (ofstudyInstanceUID != ofstudyUID) return EC_IllegalCall;     

  imageUID.getOFString(ofimageUID,0);
  studyUID.getOFString(ofstudyUID,0);
  referencedSeriesList.removeImageReference(ofseriesUID.c_str(), ofimageUID.c_str());
  return EC_Normal;
}


E_Condition DVPresentationState::removeImageReferenceAttached()
{
  if (currentImageDataset) return removeImageReference(*currentImageDataset);
  else return EC_IllegalCall;
}

size_t DVPresentationState::numberOfImageReferences()
{
  return referencedSeriesList.numberOfImageReferences();
}
  
E_Condition DVPresentationState::getImageReference(
    size_t idx,
    OFString& studyUID,
    OFString& seriesUID,
    OFString& sopclassUID,
    OFString& instanceUID, 
    OFString& frames)
{
  E_Condition result = referencedSeriesList.getImageReference(idx, seriesUID, sopclassUID, instanceUID, frames);
  if (EC_Normal == result) result = studyInstanceUID.getOFString(studyUID,0);
  return result;
}

OFBool DVPresentationState::havePresentationLookupTable()
{
  if ((presentationLUTDescriptor.getVM()==3)&&(presentationLUTData.getLength() > 0)) return OFTrue;
  else return OFFalse;
}

E_Condition DVPresentationState::setCurrentPresentationLUT(DVPSPresentationLUTType newType)
{
  if ((newType == DVPSP_table)&&(! havePresentationLookupTable())) return EC_IllegalCall;
  presentationLUT = newType;
  currentImagePLUTValid = OFFalse; // PLUT has changed
  return EC_Normal;
}

E_Condition DVPresentationState::setPresentationLookupTable(
    DcmUnsignedShort& lutDescriptor,
    DcmUnsignedShort& lutData,
    DcmLongString& lutExplanation)
{
  if ((lutDescriptor.getVM()==3)&&(lutData.getLength() > 0))
  {
    presentationLUTDescriptor = lutDescriptor;
    presentationLUTData = lutData;
    presentationLUTExplanation = lutExplanation;
    presentationLUT = DVPSP_table;
    currentImagePLUTValid = OFFalse; // PLUT has changed
  } else return EC_IllegalCall;
  return EC_Normal;
}
    

const char *DVPresentationState::getCurrentPresentationLUTExplanation()
{
  const char *value = NULL;
  switch (presentationLUT)
  {
    case DVPSP_identity:
      value = "Identity Presentation LUT Shape";
      break;
    case DVPSP_inverse:
      value = "Inverse Presentation LUT Shape";
      break;
    case DVPSP_table:
      value = getPresentationLUTExplanation();
      if (value==NULL) value = "Unnamed Presentation LUT";
      break;
  }
  return value;
}


const char *DVPresentationState::getPresentationLUTExplanation()
{
  char *value = NULL;
  if (EC_Normal != presentationLUTExplanation.getString(value)) return NULL;
  return value;
}


DVPSRotationType DVPresentationState::getRotation()
{
  DVPSRotationType result = DVPSR_0_deg;
  OFString rotation;
  imageRotation.getOFString(rotation,0);
  if (rotation=="90") result = DVPSR_90_deg;
  else if (rotation=="180") result = DVPSR_180_deg;
  else if (rotation=="270") result = DVPSR_270_deg;
  return result;
}


OFBool DVPresentationState::getFlip()
{
  OFBool result = OFFalse;
  OFString flip;
  imageHorizontalFlip.getOFString(flip,0);
  if (flip=="Y") result = OFTrue;
  return result;
}
 

E_Condition DVPresentationState::setRotation(DVPSRotationType rotation)
{
  E_Condition result = EC_Normal;
  switch (rotation)
  {
    case DVPSR_0_deg:
      result = imageRotation.putString("0");
      break;
    case DVPSR_90_deg:
      result = imageRotation.putString("90");
      break;
    case DVPSR_180_deg:
      result = imageRotation.putString("180");
      break;
    case DVPSR_270_deg:
      result = imageRotation.putString("270");
      break;
  }
  return result;
}


E_Condition DVPresentationState::setFlip(OFBool isFlipped)
{
  if (isFlipped) return imageHorizontalFlip.putString("Y"); 
  else return imageHorizontalFlip.putString("N"); 
}
  

Uint16 DVPresentationState::getDisplayedAreaTLHC_x()
{
  Uint16 result = 0;
  displayedAreaTLHC.getUint16(result,0);
  return result;
}

Uint16 DVPresentationState::getDisplayedAreaTLHC_y()
{
  Uint16 result = 0;
  displayedAreaTLHC.getUint16(result,1);
  return result;
}
 
Uint16 DVPresentationState::getDisplayedAreaBRHC_x()
{
  Uint16 result = 0;
  displayedAreaBRHC.getUint16(result,0);
  return result;
}
 
Uint16 DVPresentationState::getDisplayedAreaBRHC_y()
{
  Uint16 result = 0;
  displayedAreaBRHC.getUint16(result,1);
  return result;
}
  
E_Condition DVPresentationState::setDisplayedAreaTLHC(Uint16 x, Uint16 y)
{
  E_Condition result = displayedAreaTLHC.putUint16(x,0);
  if (result==EC_Normal) result = displayedAreaTLHC.putUint16(y,1);
  return result;
}

E_Condition DVPresentationState::setDisplayedAreaBRHC(Uint16 x, Uint16 y)
{
  E_Condition result = displayedAreaBRHC.putUint16(x,0);
  if (result==EC_Normal) result = displayedAreaBRHC.putUint16(y,1);
  return result;
}


OFBool DVPresentationState::haveShutter(DVPSShutterType type)
{
  OFBool result = OFFalse;
  switch (type)
  {
    case DVPSU_rectangular:
      result = useShutterRectangular;
      break;
    case DVPSU_circular:
      result = useShutterCircular;
      break;
    case DVPSU_polygonal:
      result = useShutterPolygonal;
      break;
    case DVPSU_bitmap:
      result = useShutterBitmap;
      break;
  }
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
 

Sint32 DVPresentationState::getRectShutterLV()
{
  Sint32 result=0;
  shutterLeftVerticalEdge.getSint32(result,0);
  return result;
} 

Sint32 DVPresentationState::getRectShutterRV()
{
  Sint32 result=0;
  shutterRightVerticalEdge.getSint32(result,0);
  return result;
} 

Sint32 DVPresentationState::getRectShutterUH()
{
  Sint32 result=0;
  shutterUpperHorizontalEdge.getSint32(result,0);
  return result;
} 

Sint32 DVPresentationState::getRectShutterLH()
{
  Sint32 result=0;
  shutterLowerHorizontalEdge.getSint32(result,0);
  return result;
} 
  

E_Condition DVPresentationState::setRectShutter(Sint32 lv, Sint32 rv, Sint32 uh, Sint32 lh)
{
  E_Condition result=EC_Normal;
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
  
Sint32 DVPresentationState::getCenterOfCircularShutter_x()
{
  Sint32 result=0;
  centerOfCircularShutter.getSint32(result,0);
  return result;
} 

Sint32 DVPresentationState::getCenterOfCircularShutter_y()
{
  Sint32 result=0;
  centerOfCircularShutter.getSint32(result,1);
  return result;
} 

Sint32 DVPresentationState::getRadiusOfCircularShutter()
{
  Sint32 result=0;
  radiusOfCircularShutter.getSint32(result,0);
  return result;
} 

E_Condition DVPresentationState::setCircularShutter(Sint32 centerX, Sint32 centerY, Sint32 radius)
{
  E_Condition result=EC_Normal;
  char buf[80];
  
  sprintf(buf, "%ld\\%ld", (long)centerX, (long)centerY);
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
  
 
size_t DVPresentationState::getNumberOfPolyShutterVertices()
{
  return (verticesOfThePolygonalShutter.getVM() / 2);
} 

E_Condition DVPresentationState::getPolyShutterVertex(size_t idx, Sint32& x, Sint32& y)
{
  x=0;
  y=0;
  E_Condition result = verticesOfThePolygonalShutter.getSint32(x,2*idx);
  if (EC_Normal==result) result = verticesOfThePolygonalShutter.getSint32(y,2*idx+1);
  return result;
} 

E_Condition DVPresentationState::setPolyShutterOrigin(Sint32 x, Sint32 y)
{
  char buf[80];
  useShutterPolygonal = OFFalse;
  verticesOfThePolygonalShutter.clear();
  sprintf(buf, "%ld\\%ld", (long)x, (long)y);
  return verticesOfThePolygonalShutter.putString(buf);
} 

E_Condition DVPresentationState::addPolyShutterVertex(Sint32 x, Sint32 y)
{
  if (verticesOfThePolygonalShutter.getLength()==0) return EC_IllegalCall;
  OFString aString;
  E_Condition result = verticesOfThePolygonalShutter.getOFStringArray(aString,OFTrue);
  if (result==EC_Normal)
  {
    char buf[80];
    sprintf(buf, "\\%ld\\%ld", (long)x, (long)y);
    aString += buf;
    result = verticesOfThePolygonalShutter.putOFStringArray(aString);
  }
  
  if (result==EC_Normal)
  {
    Sint32 x0 = 0;
    Sint32 y0 = 0;
    result = getPolyShutterVertex(0, x0, y0);
    if (result==EC_Normal)
    {
      if ((x0==x)&&(y0==y)) // polygon is closed now, activate.
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
  

Uint16 DVPresentationState::getShutterPresentationValue()
{
  Uint16 result=0;
  shutterPresentationValue.getUint16(result,0);
  return result;
} 
  
E_Condition DVPresentationState::setShutterPresentationValue(Uint16 pvalue)
{
  return shutterPresentationValue.putUint16(pvalue,0);
} 

const char *DVPresentationState::getPresentationLabel()
{
  char *value = NULL;
  if (EC_Normal != presentationLabel.getString(value)) return NULL;
  return value;
}

const char *DVPresentationState::getPresentationDescription()
{
  char *value = NULL;
  if (EC_Normal != presentationDescription.getString(value)) return NULL;
  return value;
}

const char *DVPresentationState::getPresentationCreatorsName()
{
  char *value = NULL;
  if (EC_Normal != presentationCreatorsName.getString(value)) return NULL;
  return value;
}
  
E_Condition DVPresentationState::setPresentationLabel(const char *label)
{
  return presentationLabel.putString(label);
}

E_Condition DVPresentationState::setPresentationDescription(const char *descr)
{
  return presentationLabel.putString(descr);
}
  
E_Condition DVPresentationState::setPresentationCreatorsName(const char *name)
{
  return presentationLabel.putString(name);
}


void DVPresentationState::sortGraphicLayers()
{
  graphicLayerList.sortGraphicLayers();
  return;
}

size_t DVPresentationState::getNumberOfGraphicLayers()
{
  return graphicLayerList.size();
}

const char *DVPresentationState::getGraphicLayerName(size_t idx)
{
  return graphicLayerList.getGraphicLayerName(idx);
}

size_t DVPresentationState::getGraphicLayerIndex(const char *name)
{
  return graphicLayerList.getGraphicLayerIndex(name);
}

const char *DVPresentationState::getGraphicLayerDescription(size_t idx)
{
  return graphicLayerList.getGraphicLayerDescription(idx);
}

OFBool DVPresentationState::haveGraphicLayerRecommendedDisplayValue(size_t idx)
{
  return graphicLayerList.haveGraphicLayerRecommendedDisplayValue(idx);
}

OFBool DVPresentationState::isGrayGraphicLayerRecommendedDisplayValue(size_t idx)
{
  return graphicLayerList.isGrayGraphicLayerRecommendedDisplayValue(idx);
}

E_Condition DVPresentationState::getGraphicLayerRecommendedDisplayValueGray(size_t idx, Uint16& gray)
{
  return graphicLayerList.getGraphicLayerRecommendedDisplayValueGray(idx,gray);
}

E_Condition DVPresentationState::getGraphicLayerRecommendedDisplayValueRGB(size_t idx, Uint16& r, Uint16& g, Uint16& b)
{
  return graphicLayerList.getGraphicLayerRecommendedDisplayValueRGB(idx,r,g,b);
}

E_Condition DVPresentationState::setGraphicLayerRecommendedDisplayValueGray(size_t idx, Uint16 gray)
{
  return graphicLayerList.setGraphicLayerRecommendedDisplayValueGray(idx,gray);
}

E_Condition DVPresentationState::setGraphicLayerRecommendedDisplayValueRGB(size_t idx, Uint16 r, Uint16 g, Uint16 b)
{
  return graphicLayerList.setGraphicLayerRecommendedDisplayValueRGB(idx,r,g,b);
}

E_Condition DVPresentationState::setGraphicLayerName(size_t idx, const char *name)  
{
  const char *oname = graphicLayerList.getGraphicLayerName(idx);
  if (name==NULL) return EC_IllegalCall;
  OFString oldName(oname); // make a copy of the pointer which will become invalid
  E_Condition result=graphicLayerList.setGraphicLayerName(idx, name);
  if (EC_Normal==result)
  {
    activationLayerList.renameLayer(oldName.c_str(), name);
    graphicAnnotationList.renameLayer(oldName.c_str(), name);
  }
  return result;
}

E_Condition DVPresentationState::setGraphicLayerDescription(size_t idx, const char *descr)
{
  return graphicLayerList.setGraphicLayerDescription(idx, descr);
}

E_Condition DVPresentationState::toFrontGraphicLayer(size_t idx)
{
  return graphicLayerList.toFrontGraphicLayer(idx);
}

E_Condition DVPresentationState::toBackGraphicLayer(size_t idx)
{
  return graphicLayerList.toBackGraphicLayer(idx);
}

E_Condition DVPresentationState::addGraphicLayer(
     const char *gLayer, 
     const char *gLayerDescription)
{
  return graphicLayerList.addGraphicLayer(gLayer,gLayerDescription);
}

E_Condition DVPresentationState::removeGraphicLayer(size_t idx)
{
  const char *name = graphicLayerList.getGraphicLayerName(idx);
  if (name==NULL) return EC_IllegalCall;
  activationLayerList.removeLayer(name);
  currentImageOverlaysValid = 1; // invalid but nothing added
  graphicAnnotationList.removeLayer(name);	
  return graphicLayerList.removeGraphicLayer(idx);
}

void DVPresentationState::cleanupLayers()
{
  graphicAnnotationList.cleanupLayers();
  graphicLayerList.cleanupLayers(activationLayerList, graphicAnnotationList);
}

size_t DVPresentationState::getNumberOfTextObjects(size_t layer)
{
  return graphicAnnotationList.getNumberOfTextObjects(graphicLayerList.getGraphicLayerName(layer));
}

DVPSTextObject *DVPresentationState::getTextObject(size_t layer, size_t idx)
{
  return graphicAnnotationList.getTextObject(graphicLayerList.getGraphicLayerName(layer), idx);
}

DVPSTextObject *DVPresentationState::addTextObject(size_t layer)
{
  return graphicAnnotationList.addTextObject(graphicLayerList.getGraphicLayerName(layer));
}

E_Condition DVPresentationState::removeTextObject(size_t layer, size_t idx)
{
  return graphicAnnotationList.removeTextObject(graphicLayerList.getGraphicLayerName(layer), idx);
}

E_Condition DVPresentationState::moveTextObject(size_t old_layer, size_t idx, size_t new_layer)
{
  if (old_layer==new_layer) return EC_Normal;
  return graphicAnnotationList.moveTextObject(
    graphicLayerList.getGraphicLayerName(old_layer), idx,
    graphicLayerList.getGraphicLayerName(new_layer));
}

size_t DVPresentationState::getNumberOfGraphicObjects(size_t layer)
{
  return graphicAnnotationList.getNumberOfGraphicObjects(graphicLayerList.getGraphicLayerName(layer));
}

DVPSGraphicObject *DVPresentationState::getGraphicObject(size_t layer, size_t idx)
{
  return graphicAnnotationList.getGraphicObject(graphicLayerList.getGraphicLayerName(layer), idx);
}

DVPSGraphicObject *DVPresentationState::addGraphicObject(size_t layer)
{
  return graphicAnnotationList.addGraphicObject(graphicLayerList.getGraphicLayerName(layer));
}

E_Condition DVPresentationState::removeGraphicObject(size_t layer, size_t idx)
{
  return graphicAnnotationList.removeGraphicObject(graphicLayerList.getGraphicLayerName(layer), idx);
}
  
E_Condition DVPresentationState::moveGraphicObject(size_t old_layer, size_t idx, size_t new_layer)
{
  if (old_layer==new_layer) return EC_Normal;
  return graphicAnnotationList.moveGraphicObject(
    graphicLayerList.getGraphicLayerName(old_layer), idx,
    graphicLayerList.getGraphicLayerName(new_layer));
}


E_Condition DVPresentationState::setCharset(DVPScharacterSet charset)
{
  if (charset==DVPSC_other) return EC_IllegalCall;
  
  const char *cname=NULL;
  switch (charset)
  {
    case DVPSC_latin1:
      cname = "ISO_IR 100"; 
      break;    
    case DVPSC_latin2:
      cname = "ISO_IR 101"; 
      break;    
    case DVPSC_latin3:
      cname = "ISO_IR 109"; 
      break;    
    case DVPSC_latin4:
      cname = "ISO_IR 110"; 
      break;    
    case DVPSC_latin5:
      cname = "ISO_IR 148"; 
      break;    
    case DVPSC_cyrillic:
      cname = "ISO_IR 144"; 
      break;    
    case DVPSC_arabic:
      cname = "ISO_IR 127"; 
      break;    
    case DVPSC_greek:
      cname = "ISO_IR 126"; 
      break;    
    case DVPSC_hebrew:
      cname = "ISO_IR 138"; 
      break;    
    case DVPSC_japanese:
      cname = "ISO_IR 13"; 
      break;    
    default: // can only be DVPSC_ascii
      break;    
  }
  if (cname) return specificCharacterSet.putString(cname);
  else 
  {
    specificCharacterSet.clear();
    return EC_Normal;
  }
}
  

DVPScharacterSet DVPresentationState::getCharset()
{
  if (specificCharacterSet.getLength()==0) return DVPSC_ascii;
  
  OFString aString;
  specificCharacterSet.getOFString(aString,0);
  if (aString == "ISO_IR 6") return DVPSC_ascii;
  else if (aString == "ISO_IR 100") return DVPSC_latin1;
  else if (aString == "ISO_IR 101") return DVPSC_latin2;
  else if (aString == "ISO_IR 109") return DVPSC_latin3;
  else if (aString == "ISO_IR 110") return DVPSC_latin4;
  else if (aString == "ISO_IR 148") return DVPSC_latin5;
  else if (aString == "ISO_IR 144") return DVPSC_cyrillic;
  else if (aString == "ISO_IR 127") return DVPSC_arabic;
  else if (aString == "ISO_IR 126") return DVPSC_greek;
  else if (aString == "ISO_IR 138") return DVPSC_hebrew;
  else if (aString == "ISO_IR 13")  return DVPSC_japanese;
  /* default */
  return DVPSC_other;
}

const char *DVPresentationState::getCharsetString()
{
  char *c = NULL;
  if (EC_Normal == specificCharacterSet.getString(c)) return c; else return NULL;
}


size_t DVPresentationState::getNumberOfCurves(size_t layer)
{
  return activationLayerList.getNumberOfActivations(
    graphicLayerList.getGraphicLayerName(layer), OFTrue);
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

E_Condition DVPresentationState::addCurve(size_t layer, size_t curveidxinimage)
{
  const char *lname = graphicLayerList.getGraphicLayerName(layer);
  DVPSCurve *curve = currentImageCurveList.getCurve(curveidxinimage);
  if ((curve==NULL)||(lname==NULL)) return EC_IllegalCall;
  return activationLayerList.setActivation(0x5000 + curve->getCurveGroup(), lname);
}

E_Condition DVPresentationState::removeCurve(size_t layer, size_t idx)
{
  Uint16 group = activationLayerList.getActivationGroup(
    graphicLayerList.getGraphicLayerName(layer), idx, OFTrue);
  if (group == 0) return EC_IllegalCall;
  activationLayerList.removeActivation(group);
  return EC_Normal;
}

E_Condition DVPresentationState::moveCurve(size_t old_layer, size_t idx, size_t new_layer)
{
  const char *lname = graphicLayerList.getGraphicLayerName(new_layer);
  if (lname==NULL) return EC_IllegalCall;

  Uint16 group = activationLayerList.getActivationGroup(
    graphicLayerList.getGraphicLayerName(old_layer), idx, OFTrue);
  if (group == 0) return EC_IllegalCall;
  
  activationLayerList.removeActivation(group);
  return activationLayerList.setActivation(group, lname);
}


const char *DVPresentationState::getCurrentVOIDescription()
{
  char *c=NULL;
  if (useVOIWindow)
  {
    if (EC_Normal == windowCenterWidthExplanation.getString(c)) return c;
  } 
  else if (useVOILUT)
  {
    if (EC_Normal == voiLUTExplanation.getString(c)) return c;
  } 
  return NULL;
}


E_Condition DVPresentationState::getCurrentWindowWidth(double &w)
{
  E_Condition result = EC_IllegalCall;
  if (useVOIWindow)
  {
    Float64 temp=0.0;
    result = windowWidth.getFloat64(temp,0);
    if (EC_Normal==result) w = (double)temp;
  }
  return result;
}
  
E_Condition DVPresentationState::getCurrentWindowCenter(double &c)
{
  E_Condition result = EC_IllegalCall;
  if (useVOIWindow)
  {
    Float64 temp=0.0;
    result = windowCenter.getFloat64(temp,0);
    if (EC_Normal==result) c = (double)temp;
  }
  return result;
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
   
E_Condition DVPresentationState::setVOILUTFromImage(size_t idx)
{
  DVPSVOILUT *lut = currentImageVOILUTList.getVOILUT(idx);
  if (lut) 
  {
    lut->assign(voiLUTDescriptor, voiLUTData, voiLUTExplanation);
    useVOILUT = OFTrue;
    useVOIWindow = OFFalse;
    currentImageVOIValid = OFFalse; // VOI has changed
    return EC_Normal;
  }
  return EC_IllegalCall;
}

E_Condition DVPresentationState::setVOIWindowFromImage(size_t idx)
{
  DVPSVOIWindow *window = currentImageVOIWindowList.getVOIWindow(idx);
  if (window) 
  {
    return setVOIWindow(window->getWindowCenter(), 
      window->getWindowWidth(), window->getExplanation());
  }
  return EC_IllegalCall;
}
 
E_Condition DVPresentationState::setVOIWindow(double wCenter, double wWidth, const char *description)
{
  DcmDecimalString wc(DCM_WindowCenter);
  DcmDecimalString ww(DCM_WindowWidth);
  DcmLongString expl(DCM_WindowCenterWidthExplanation);
  char buf[80];
  sprintf(buf, "%G", wCenter);
  E_Condition result = wc.putString(buf);
  sprintf(buf, "%G", wWidth);
  if (EC_Normal == result) result = ww.putString(buf);
  if ((EC_Normal == result)&&(description)) result = expl.putString(description);
  if (EC_Normal == result)
  {
    // everything worked fine, now copy.
    windowCenter = wc;
    windowWidth = ww;
    windowCenterWidthExplanation = expl;
    useVOILUT = OFFalse;
    useVOIWindow = OFTrue;
    currentImageVOIValid = OFFalse; // VOI has changed
  }
  return result;
}

E_Condition DVPresentationState::setVOILUT( 
    DcmUnsignedShort& lutDescriptor,
    DcmUnsignedShort& lutData,
    DcmLongString& lutExplanation)
{
  if (lutData.getLength() == 0) return EC_IllegalCall;
  if (lutDescriptor.getVM() != 3) return EC_IllegalCall;
  voiLUTDescriptor = lutDescriptor;
  voiLUTData = lutData;
  voiLUTExplanation = lutExplanation;
  useVOILUT = OFTrue;
  useVOIWindow = OFFalse;
  currentImageVOIValid = OFFalse; // VOI has changed
  return EC_Normal;  
}
 
void DVPresentationState::deactivateVOI()
{
  useVOILUT = OFFalse;
  useVOIWindow = OFFalse;
  currentImageVOIValid = OFFalse; // VOI has changed
}


size_t DVPresentationState::getNumberOfActiveOverlays(size_t layer)
{
  return activationLayerList.getNumberOfActivations(
    graphicLayerList.getGraphicLayerName(layer), OFFalse);
}

Uint16 DVPresentationState::getActiveOverlayGroup(size_t layer, size_t idx)
{
  return activationLayerList.getActivationGroup(graphicLayerList.getGraphicLayerName(layer),
           idx, OFFalse);
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


size_t DVPresentationState::getNumberOfOverlaysInPresentationState()
{
  return overlayList.size();
}

Uint16 DVPresentationState::getOverlayInPresentationStateGroup(size_t idx)
{
  DVPSOverlay *overlay = overlayList.getOverlay(idx);
  if (overlay) return (0x6000+overlay->getOverlayGroup());
  return 0;
}

const char *DVPresentationState::getOverlayInPresentationStateLabel(size_t idx)
{
  DVPSOverlay *overlay = overlayList.getOverlay(idx);
  if (overlay) return overlay->getOverlayLabel();
  return NULL;
}

const char *DVPresentationState::getOverlayInPresentationStateDescription(size_t idx)
{
  DVPSOverlay *overlay = overlayList.getOverlay(idx);
  if (overlay) return overlay->getOverlayDescription();
  return NULL;
}
   
size_t DVPresentationState::getOverlayInPresentationStateActivationLayer(size_t idx) 
{
  DVPSOverlay *overlay = overlayList.getOverlay(idx);
  if (overlay) 
  {
    const char *layerName = activationLayerList.getActivationLayer(0x6000+overlay->getOverlayGroup());
    if (layerName) return graphicLayerList.getGraphicLayerIndex(layerName);
  }
  return DVPS_IDX_NONE;
}
  
OFBool DVPresentationState::overlayIsBitmapShutter(size_t idx)
{
  if (shutterOverlayGroup.getLength() == 0) return OFFalse;

  Uint16 shutterGroup=0;
  if (EC_Normal == shutterOverlayGroup.getUint16(shutterGroup,0))
  {
    DVPSOverlay *overlay = overlayList.getOverlay(idx);
    if (overlay) return (shutterGroup == 0x6000+overlay->getOverlayGroup());
  }
  return OFFalse;
}

OFBool DVPresentationState::overlayInPresentationStateIsROI(size_t idx)
{
  DVPSOverlay *overlay = overlayList.getOverlay(idx);
  if (overlay) return overlay->isROI();
  return OFFalse;
}
  
E_Condition DVPresentationState::removeOverlayFromPresentationState(size_t idx)
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
  Uint16 group;

  for (i=0; i<16; i++) allocated[i]=0;
  max = overlayList.size();
  for (i=0; i<max; i++)
  { // mark all group numbers used in presentation state
    group = getOverlayInPresentationStateGroup(i);
    if ((group >= 0x6000)&&(group <= 0x601F)) allocated[(group - 0x6000) >> 1] = 2;
  }
  max = getNumberOfOverlaysInImage();
  for (i=0; i<max; i++)
  { // mark all group numbers used in image
    group = getOverlayInImageGroup(i);
    if ((group >= 0x6000)&&(group <= 0x601F)) allocated[(group - 0x6000) >> 1] += 1;
  }
  // now we have 0=unused, 1=used in image, 2=used in pstate, 3=used in both.
  
  // check if the current group can be left unchanged
  if ((currentGroup >= 0x6000)&&(currentGroup <= 0x601F))
  {
    if (allocated[(group - 0x6000) >> 1] == 2) return currentGroup;
  }
  // find a free group
  for (i=0; i<16; i++) if (allocated[i]==0) return 0x6000+(i<<1);
  // find a group not used in the presentation state
  for (i=0; i<16; i++) if (allocated[i]<2) return 0x6000+(i<<1);
  // not found.
  return 0;
}

E_Condition DVPresentationState::changeOverlayGroupInPresentationState(size_t idx, Uint16 newGroup)
{
  Uint16 group = getOverlayInPresentationStateGroup(idx);
  if (group != 0)
  {
    if (newGroup==0) newGroup = findOverlayGroup(group);
    if (newGroup==group) return EC_Normal; // shortcut - no change needed
    E_Condition result = overlayList.changeOverlayGroup(idx, newGroup);
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
  
E_Condition DVPresentationState::addOverlayToPresentationState(DcmItem& overlayIOD, Uint16 groupInItem, Uint16 newGroup)
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
  
E_Condition DVPresentationState::activateOverlayInImage(size_t layer, size_t idx)
{
  Uint16 group = getOverlayInImageGroup(idx);
  if (group==0) return EC_IllegalCall;
  if (activationLayerList.getActivationLayer(group) != NULL) return EC_IllegalCall; //already activated

  const char *layerName = getGraphicLayerName(layer);
  if (layerName==NULL) return EC_IllegalCall;
  currentImageOverlaysValid = 1; // invalid but nothing (external) added
  return activationLayerList.setActivation(group, layerName);
}

E_Condition DVPresentationState::activateOverlayInPresentationState(size_t layer, size_t idx)
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
  
E_Condition DVPresentationState::activateOverlayAsBitmapShutter(size_t idx)
{
  Uint16 group = getOverlayInPresentationStateGroup(idx);
  if (group==0) return EC_IllegalCall;
  if (activationLayerList.getActivationLayer(group) != NULL) return EC_IllegalCall; // activated as overlay
  if (overlayIsSuitableAsBitmapShutter(idx))
  {
    shutterOverlayGroup.clear();
    E_Condition result = shutterOverlayGroup.putUint16(group,0);
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

E_Condition DVPresentationState::deactivateOverlay(size_t layer, size_t idx)
{
  Uint16 group = activationLayerList.getActivationGroup(
    graphicLayerList.getGraphicLayerName(layer), idx, OFFalse);
  if (group == 0) return EC_IllegalCall;
  activationLayerList.removeActivation(group);
  currentImageOverlaysValid = 1; // invalid but nothing added
  return EC_Normal;
}

E_Condition DVPresentationState::moveOverlay(size_t old_layer, size_t idx, size_t new_layer)
{
  const char *lname = graphicLayerList.getGraphicLayerName(new_layer);
  if (lname==NULL) return EC_IllegalCall;

  Uint16 group = activationLayerList.getActivationGroup(
    graphicLayerList.getGraphicLayerName(old_layer), idx, OFFalse);
  if (group == 0) return EC_IllegalCall;
  
  activationLayerList.removeActivation(group);
  return activationLayerList.setActivation(group, lname);
}

void DVPresentationState::renderPixelData()
{
  if (currentImage == NULL) return;

  int result=0;

  if (! currentImageVOIValid)
  {
     currentImageVOIValid = OFTrue;
     
     /* set VOI transformation */
     if (useVOIWindow)
     {
       Float64 wCenter = 0.0;
       Float64 wWidth = 0.0;
       OFBool useWindow = OFTrue;
       
       if (EC_Normal != windowCenter.getFloat64(wCenter, 0))
       {
       	 useWindow = OFFalse;
#ifdef DEBUG
         cerr << "warning: unable to evaluate Window Center, ignoring." << endl;
#endif
       }
       if (EC_Normal != windowWidth.getFloat64(wWidth, 0))
       {
       	 useWindow = OFFalse;
#ifdef DEBUG
         cerr << "warning: unable to evaluate Window Width, ignoring." << endl;
#endif
       }
       if (useWindow) result = currentImage->setWindow(wCenter, wWidth);
       else result = currentImage->setNoVoiTransformation();
     } 
     else if (useVOILUT)
     {
       result = currentImage->setVoiLut(voiLUTData, voiLUTDescriptor);    
     }
     else result = currentImage->setNoVoiTransformation();

#ifdef DEBUG
     if (!result) cerr << "warning: unable to set VOI transformation, ignoring." << endl;
#endif
  } /* VOI transform */


  if (! currentImagePLUTValid)
  {
     currentImagePLUTValid = OFTrue;
     
     /* set Presentation LUT transformation */
     switch (presentationLUT)
     {
       case DVPSP_identity:
         result = currentImage->setPresentationLutShape(ESP_Identity);
#ifdef DEBUG
         if (!result) cerr << "warning: unable to set identity presentation LUT shape, ignoring." << endl;
#endif
         break;
       case DVPSP_inverse:
         result = currentImage->setPresentationLutShape(ESP_Inverse);
#ifdef DEBUG
         if (!result) cerr << "warning: unable to set inverse presentation LUT shape, ignoring." << endl;
#endif
         break;
       case DVPSP_table:
         result = currentImage->setPresentationLut(presentationLUTData, presentationLUTDescriptor,
           &presentationLUTExplanation);
#ifdef DEBUG
         if (!result) cerr << "warning: unable to set identity presentation LUT shape, ignoring." << endl;
#endif
         break;
     }
  } /* Presentation LUT */

  if (currentImageOverlaysValid==1)
  {
    /* overlays are invalid but no external overlays have been added */
    /* remove all external overlays that are not active */
    for (unsigned int remgroup=0x6000; remgroup <= 0x601F; remgroup += 2)
    {
      if ((! overlayList.haveOverlayGroup(remgroup))||
          (NULL == activationLayerList.getActivationLayer(remgroup)))
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
#ifdef DEBUG
       if (!result) cerr << "warning: unable to flip image horizontally, ignoring." << endl;
#endif
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
#ifdef DEBUG
       if (!result) cerr << "warning: unable to rotate image by " << srot << " degrees, ignoring." << endl;
#endif
    }
    currentImageRotation = DVPSR_0_deg;
    
    // deactivate all overlays first
    result = currentImage->removeAllOverlays();
#ifdef DEBUG
       if (!result) cerr << "warning: unable to disable external overlays, ignoring." << endl;
#endif
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
          if (EC_Normal != overlay->activate(*currentImage))
          {
#ifdef DEBUG
       if (!result) cerr << "warning: unable to set external overlay group 0x" 
                         << hex << ovgroup << dec << ", ignoring." << endl;
#endif
          }
        }
      }
    }
    currentImageOverlaysValid = 2; // valid
  }
  
  // we can always reach the final rotation/flip status with
  // at most one rotation and one flip. The following formula
  // derives the operations to perform.
  OFBool pstateFlip = getFlip();
  DVPSRotationType pstateRotation = getRotation();
  
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
#ifdef DEBUG
     if (!result) cerr << "warning: unable to rotate image by " << rot << " degrees, ignoring." << endl;
#endif
  }  

  if (flp)
  {
    result = currentImage->flipImage();
#ifdef DEBUG
     if (!result) cerr << "warning: unable to flip image horizontally, ignoring." << endl;
#endif
  }  
  
  currentImageRotation = pstateRotation;
  currentImageFlip = pstateFlip;
  
  return;
}


E_Condition DVPresentationState::getOverlayData(
     size_t layer,
     size_t idx,
     const void *&overlayData,
     unsigned int &width,
     unsigned int &height,
     unsigned int &left,
     unsigned int &top,
     OFBool &isROI,
     unsigned long frame)
{
   EM_Overlay mode = EMO_Default;
   if (currentImage)
   {
     renderPixelData();
     Uint16 group = activationLayerList.getActivationGroup(graphicLayerList.getGraphicLayerName(layer),idx,OFFalse);
     if (group==0) return EC_IllegalCall;
     const Uint8 *data = currentImage->getOverlayData((unsigned int)group, left, top, width, height, mode, frame);
     if (EMO_RegionOfInterest == mode) isROI=OFTrue; else isROI=OFFalse;
     if (data) overlayData = (void*)data; 
     else 
     {
       overlayData = NULL;
       return EC_IllegalCall;
     }
   } else {
     overlayData = NULL;
     width = 0;
     height = 0;
     left = 0;
     top = 0;
     return EC_IllegalCall;
   }
   return EC_Normal;
}

E_Condition DVPresentationState::getPixelData(
     const void *&pixelData,
     unsigned long &width,
     unsigned long &height)
{
   if (currentImage)
   {
     renderPixelData();
     width = currentImage->getWidth();
     height = currentImage->getHeight();
     pixelData = currentImage->getOutputData(8, 0 /*frame*/);
   } else {
     pixelData = NULL;
     width = 0;
     height = 0;
     return EC_IllegalCall;
   }
   return EC_Normal;
}

E_Condition DVPresentationState::getImageAspectRatio(double &ratio)
{
  if (currentImage)
  {
    ratio = currentImage->getWidthHeightRatio();
    return EC_Normal;
  }
  return EC_IllegalCall;
}

E_Condition DVPresentationState::getImageMinMaxPixelRange(double &minValue, double& maxValue)
{
  E_Condition result = EC_IllegalCall;
  if (currentImage)
  {
    if (currentImage->getMinMaxValues(minValue, maxValue, 1)) result = EC_Normal;
  }
  return result;
}

E_Condition DVPresentationState::getImageMinMaxPixelValue(double &minValue, double& maxValue)
{
  E_Condition result = EC_IllegalCall;
  if (currentImage)
  {
    if (currentImage->getMinMaxValues(minValue, maxValue, 0)) result = EC_Normal;
  }
  return result;
}

/*
 *  $Log: dvpstat.cc,v $
 *  Revision 1.6  1999-01-15 17:32:59  meichel
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
 *
 */

