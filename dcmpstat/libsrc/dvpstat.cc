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
 *  Update Date:      $Date: 1998-11-27 14:50:47 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpstat.h"
#include "ofstring.h"
#include "dcmimage.h"    /* for DicomImage */

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
, currentImageOutput(NULL)
{
}

DVPresentationState::DVPresentationState(const DVPresentationState& copy)
: patientName(copy.patientName)
, patientID(copy.patientID)
, patientBirthDate(copy.patientBirthDate)
, patientSex(copy.patientSex)
, studyInstanceUID(copy.studyInstanceUID)
, studyDate(copy.studyDate)
, studyTime(copy.studyTime)
, referringPhysiciansName(copy.referringPhysiciansName)
, studyID(copy.studyID)
, accessionNumber(copy.accessionNumber)
, seriesInstanceUID(copy.seriesInstanceUID)
, seriesNumber(copy.seriesNumber)
, manufacturer(copy.manufacturer)
, displayedAreaTLHC(copy.displayedAreaTLHC)
, displayedAreaBRHC(copy.displayedAreaBRHC)
, presentationLUT(copy.presentationLUT)
, presentationLUTDescriptor(copy.presentationLUTDescriptor)
, presentationLUTExplanation(copy.presentationLUTExplanation)
, presentationLUTData(copy.presentationLUTData)
, imageNumber(copy.imageNumber)
, presentationLabel(copy.presentationLabel)
, presentationDescription(copy.presentationDescription)
, presentationCreationDate(copy.presentationCreationDate)
, presentationCreationTime(copy.presentationCreationTime)
, presentationCreatorsName(copy.presentationCreatorsName)
, referencedSeriesList(copy.referencedSeriesList)
, sOPInstanceUID(copy.sOPInstanceUID)
, specificCharacterSet(copy.specificCharacterSet)
, instanceCreationDate(copy.instanceCreationDate)
, instanceCreationTime(copy.instanceCreationTime)
, instanceCreatorUID(copy.instanceCreatorUID)
, useShutterRectangular(copy.useShutterRectangular)
, useShutterCircular(copy.useShutterCircular)
, useShutterPolygonal(copy.useShutterPolygonal)
, useShutterBitmap(copy.useShutterBitmap)
, shutterShape(copy.shutterShape)
, shutterLeftVerticalEdge(copy.shutterLeftVerticalEdge)
, shutterRightVerticalEdge(copy.shutterRightVerticalEdge)
, shutterUpperHorizontalEdge(copy.shutterUpperHorizontalEdge)
, shutterLowerHorizontalEdge(copy.shutterLowerHorizontalEdge)
, centerOfCircularShutter(copy.centerOfCircularShutter)
, radiusOfCircularShutter(copy.radiusOfCircularShutter)
, verticesOfThePolygonalShutter(copy.verticesOfThePolygonalShutter)
, shutterPresentationValue(copy.shutterPresentationValue)
, shutterOverlayGroup(copy.shutterOverlayGroup)
, overlayList(copy.overlayList)
, activationLayerList(copy.activationLayerList)
, graphicAnnotationList(copy.graphicAnnotationList)
, imageRotation(copy.imageRotation)
, imageHorizontalFlip(copy.imageHorizontalFlip)
, graphicLayerList(copy.graphicLayerList)
, useModalityRescale(copy.useModalityRescale)
, useModalityLUT(copy.useModalityLUT)
, modalityLUTDescriptor(copy.modalityLUTDescriptor)
, modalityLUTExplanation(copy.modalityLUTExplanation)
, modalityLUTType(copy.modalityLUTType)
, modalityLUTData(copy.modalityLUTData)
, rescaleIntercept(copy.rescaleIntercept)
, rescaleSlope(copy.rescaleSlope)
, rescaleType(copy.rescaleType)
, useVOIWindow(copy.useVOIWindow)
, useVOILUT(copy.useVOILUT)
, voiLUTDescriptor(copy.voiLUTDescriptor)
, voiLUTExplanation(copy.voiLUTExplanation)
, voiLUTData(copy.voiLUTData)
, windowCenter(copy.windowCenter)
, windowWidth(copy.windowWidth)
, windowCenterWidthExplanation(copy.windowCenterWidthExplanation)
, currentImageDataset(NULL)
, currentImageFileformat(NULL)
, currentImage(NULL)
, currentImageOutput(NULL)
{
  if (copy.currentImageFileformat)
  {
    DcmFileFormat *newff = new DcmFileFormat(*(copy.currentImageFileformat));
    if (newff) attachImage(newff);
  } 
  else if (copy.currentImageDataset)
  {
    DcmDataset *newds = new DcmDataset(*(copy.currentImageDataset));
    if (newds) attachImage(newds);
  }
}

DVPresentationState::~DVPresentationState()
{
  detachImage();
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
  DcmIntegerString numberOfFrames(DCM_NumberOfFrames);
  DcmUniqueIdentifier seriesUID(DCM_SeriesInstanceUID);
  DcmUniqueIdentifier sopclassUID(DCM_SOPClassUID);
  DcmUniqueIdentifier imageUID(DCM_SOPInstanceUID);
  DcmCodeString presentationLUTShape(DCM_PresentationLUTShape);
 
  clear(); // re-initialize Presentation State object 

  /* copy some image attributes we will need later */
  READ_FROM_DATASET(DcmUnsignedShort, rows)
  READ_FROM_DATASET(DcmUnsignedShort, columns)
  READ_FROM_DATASET(DcmCodeString, photometricInterpretation)
  READ_FROM_DATASET(DcmIntegerString, numberOfFrames)
  READ_FROM_DATASET(DcmUniqueIdentifier, seriesUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, sopclassUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, imageUID)

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

  if (EC_Normal == result)
  {
    // create referencedSeriesList
    OFString SseriesUID;
    OFString SsopclassUID;
    OFString SimageUID;
    Sint32 SnumberOfFrames=0;
    Sint32 frame=0;
    
    seriesUID.getOFString(SseriesUID,0);
    sopclassUID.getOFString(SsopclassUID,0);
    imageUID.getOFString(SimageUID,0);
    
    /* if the image is multiframe, reference frame 1 */
    numberOfFrames.getSint32(SnumberOfFrames,0);
    if (SnumberOfFrames > 1) frame=1;
    
    result = referencedSeriesList.addImageReference(
      SseriesUID.c_str(), SsopclassUID.c_str(), SimageUID.c_str(), frame);
  }

  return result;
}


E_Condition DVPresentationState::write(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmElement *delem=NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;
  DcmCodeString presentationLUTShape(DCM_PresentationLUTShape);
  
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


E_Condition DVPresentationState::attachImage(DcmDataset *dataset)
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
      modalityLUTData, modalityLUTDescriptor, CIF_UsePresentationState);     
  }
  else
  {
    image = new DicomImage(dataset, dataset->getOriginalXfer(), CIF_UsePresentationState);
  }
  
  if (image)
  {
    if (EIS_Normal == image->getStatus())
    {
      detachImage();
      currentImage = image;
      currentImageDataset = dataset;
    } else {
      delete image;
      return EC_IllegalCall;
    }
  } else return EC_MemoryExhausted;
  return EC_Normal;
}


E_Condition DVPresentationState::attachImage(DcmFileFormat *fileformat)
{
  if (fileformat == NULL) return EC_IllegalCall;
  E_Condition result = attachImage(fileformat->getDataset());
  if (EC_Normal == result) currentImageFileformat = fileformat;
  return result;
}

void DVPresentationState::detachImage()
{
  if (currentImageOutput) delete currentImageOutput;
  if (currentImage) delete currentImage;
  if (currentImageFileformat) delete currentImageFileformat;
  else if (currentImageDataset) delete currentImageDataset;
  currentImageOutput = NULL;
  currentImage = NULL;
  currentImageFileformat = NULL;
  currentImageDataset = NULL;
  return;
}
  	
E_Condition DVPresentationState::getPixelData(
     const void *&pixelData,
     unsigned long &width,
     unsigned long &height)
{
   if (currentImage)
   {

     /* set VOI transformation */
     int result=0;
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
       else result = currentImage->setNoVOITransformation();
     } 
     else if (useVOILUT)
     {
       result = currentImage->setVoiLut(voiLUTData, voiLUTDescriptor);    
     }
     else result = currentImage->setNoVOITransformation();

#ifdef DEBUG
     if (!result) cerr << "warning: unable to set VOI transformation, ignoring." << endl;
#endif
     
     /* set Presentation LUT transformation */
     
//    inline int setPresentationLutShape(const ES_PresentationLut shape = ESP_Identity)
//    inline int setPresentationLut(const DcmUnsignedShort, const DcmUnsignedShort &descriptor)
    
     /* apply Barten transformation */
     /* rotate and flip (only if this is not implemented in the GUI */
     
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
  

/*
 *  $Log: dvpstat.cc,v $
 *  Revision 1.1  1998-11-27 14:50:47  meichel
 *  Initial Release.
 *
 *
 */

