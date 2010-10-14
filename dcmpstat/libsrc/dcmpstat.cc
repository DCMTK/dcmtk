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
 *    classes: DcmPresentationState
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:31 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dcmpstat.h"

#include "dcmtk/ofstd/ofstd.h"       /* for class OFStandard */
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */
#include "dcmtk/dcmpstat/dvpscu.h"      /* for DVPSCurve */
#include "dcmtk/dcmpstat/dvpsvl.h"      /* for DVPSVOILUT */
#include "dcmtk/dcmpstat/dvpsvw.h"      /* for DVPSVOIWindow */
#include "dcmtk/dcmpstat/dvpsov.h"      /* for DVPSOverlay */
#include "dcmtk/dcmpstat/dvpsda.h"      /* for DVPSDisplayedArea */
#include "dcmtk/dcmpstat/dvpssv.h"      /* for DVPSSoftcopyVOI */
#include "dcmtk/dcmpstat/dvpshlp.h"     /* for class DVPSHelper */

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

OFLogger DCM_dcmpstatGetLogger()
{
    static OFLogger DCM_dcmpstatLogger = OFLog::getLogger("dcmtk.dcmpstat");
    return DCM_dcmpstatLogger;
}

OFLogger DCM_dcmpstatDumpGetLogger()
{
    static OFLogger DCM_dcmpstatLogger = OFLog::getLogger("dcmtk.dcmpstat.dump");
    return DCM_dcmpstatLogger;
}

OFLogger DCM_dcmpstatLogfileGetLogger()
{
    static OFLogger DCM_dcmpstatLogger = OFLog::getLogger("dcmtk.dcmpstat.logfile");
    return DCM_dcmpstatLogger;
}

/* --------------- class DcmPresentationState --------------- */

DcmPresentationState::DcmPresentationState()
: patientName(DCM_PatientName)
, patientID(DCM_PatientID)
, patientBirthDate(DCM_PatientBirthDate)
, patientSex(DCM_PatientSex)
, studyInstanceUID(DCM_StudyInstanceUID)
, studyDate(DCM_StudyDate)
, studyTime(DCM_StudyTime)
, referringPhysicianName(DCM_ReferringPhysicianName)
, studyID(DCM_StudyID)
, accessionNumber(DCM_AccessionNumber)
, seriesInstanceUID(DCM_SeriesInstanceUID)
, seriesNumber(DCM_SeriesNumber)
, manufacturer(DCM_Manufacturer)
, displayedAreaSelectionList()
, presentationLUT()
, imageNumber(DCM_InstanceNumber)
, presentationLabel(DCM_ContentLabel)
, presentationDescription(DCM_ContentDescription)
, presentationCreationDate(DCM_PresentationCreationDate)
, presentationCreationTime(DCM_PresentationCreationTime)
, presentationCreatorsName(DCM_ContentCreatorName)
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
, softcopyVOIList()
{
  createInstanceUID();
}


DcmPresentationState::~DcmPresentationState()
{
}

void DcmPresentationState::clear()
{
  patientName.clear();
  patientID.clear();
  patientBirthDate.clear();
  patientSex.clear();
  studyInstanceUID.clear();
  studyDate.clear();
  studyTime.clear();
  referringPhysicianName.clear();
  studyID.clear();
  accessionNumber.clear();
  seriesInstanceUID.clear();
  seriesNumber.clear();
  manufacturer.clear();
  displayedAreaSelectionList.clear();
  presentationLUT.clear();
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
  softcopyVOIList.clear();
  return;
}


const char *DcmPresentationState::createInstanceUID()
{
  char uid[100];
  OFString aString;
  char *puid = NULL;

  OFCondition result = sOPInstanceUID.putString(dcmGenerateUniqueIdentifier(uid));
  DVPSHelper::currentDate(aString);
  DVPSHelper::setDefault(result, instanceCreationDate, aString.c_str());
  DVPSHelper::currentTime(aString);
  DVPSHelper::setDefault(result, instanceCreationTime, aString.c_str());
  if (EC_Normal == result)
  {
    if (EC_Normal != sOPInstanceUID.getString(puid)) puid=NULL;
  }
  return puid;
}


const char *DcmPresentationState::getInstanceUID()
{
  char *puid = NULL;
  if (EC_Normal != sOPInstanceUID.getString(puid)) puid=NULL;
  return puid;
}


const char *DcmPresentationState::getSOPClassUID()
{
  return UID_GrayscaleSoftcopyPresentationStateStorage;
}


const char *DcmPresentationState::getPatientID()
{
  char *c = NULL;
  if (EC_Normal == patientID.getString(c)) return c; else return NULL;
}


const char *DcmPresentationState::getStudyUID()
{
  char *c = NULL;
  if (EC_Normal == studyInstanceUID.getString(c)) return c; else return NULL;
}


OFCondition DcmPresentationState::createDummyValues(OFBool replaceSOPInstanceUID)
{
  OFCondition result = EC_Normal;
  char uid[100];
  OFString aString;

  DVPSHelper::setDefault(result, patientName, DEFAULT_patientName);
  SET_UID(studyInstanceUID)
  SET_UID(seriesInstanceUID)

  DVPSHelper::setDefault(result, imageNumber, DEFAULT_imageNumber);
  DVPSHelper::setDefault(result, presentationLabel, DEFAULT_presentationLabel);
  DVPSHelper::currentDate(aString);
  DVPSHelper::setDefault(result, presentationCreationDate, aString.c_str() );
  DVPSHelper::currentTime(aString);
  DVPSHelper::setDefault(result, presentationCreationTime, aString.c_str() );

  if ((result==EC_Normal)&&(replaceSOPInstanceUID ||(sOPInstanceUID.getLength()==0)))
  {
    sOPInstanceUID.putString(dcmGenerateUniqueIdentifier(uid));
    DVPSHelper::currentDate(aString);
    DVPSHelper::setDefault(result, instanceCreationDate, aString.c_str() );
    DVPSHelper::currentTime(aString);
    DVPSHelper::setDefault(result, instanceCreationTime, aString.c_str() );
  }

  // default for specific character set is -absent-.
  // DVPSHelper::setDefault(result, specificCharacterSet, DEFAULT_specificCharacterSet );

  // create some dummy defaults for shutters
  if (shutterPresentationValue.getVM() != 1) shutterPresentationValue.putUint16(0);
  DVPSHelper::setDefault(result, shutterLeftVerticalEdge, "1");
  DVPSHelper::setDefault(result, shutterRightVerticalEdge, "1024");
  DVPSHelper::setDefault(result, shutterUpperHorizontalEdge, "1");
  DVPSHelper::setDefault(result, shutterLowerHorizontalEdge, "1024");
  DVPSHelper::setDefault(result, centerOfCircularShutter, "512\\512");
  DVPSHelper::setDefault(result, radiusOfCircularShutter, "512");
  DVPSHelper::setDefault(result, verticesOfThePolygonalShutter, "1\\1\\1\\1024\\1024\\1024\\1024\\1\\1\\1");
  if (shutterOverlayGroup.getVM() != 1) shutterOverlayGroup.putUint16(0x6000);

  // create defaults for Spatial Transformation Module
  if (result==EC_Normal)
  {
    if (imageRotation.getVM() != 1) result = imageRotation.putUint16(0);
  }
  DVPSHelper::setDefault(result, imageHorizontalFlip, "N");

  // create defaults for Modality Rescale
  DVPSHelper::setDefault(result, rescaleIntercept, "0");
  DVPSHelper::setDefault(result, rescaleSlope, "1");
  DVPSHelper::setDefault(result, rescaleType, "UNSPECIFIED");

  return result;
}


OFCondition DcmPresentationState::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DcmSequenceOfItems *seq;
  DcmItem *item;

  clear(); // re-initialize Presentation State object

  /* check SOP class UID and modality first */
  DcmUniqueIdentifier sopclassuid(DCM_SOPClassUID);
  DcmCodeString modality(DCM_Modality);
  OFString aString;
  READ_FROM_DATASET(DcmUniqueIdentifier, sopclassuid)
  READ_FROM_DATASET(DcmCodeString, modality)

  if (sopclassuid.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("SOPClassUID absent or empty in presentation state");
  }
  else if (sopclassuid.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("SOPClassUID VM != 1 in presentation state");
  }

  if (modality.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("Modality absent or empty in presentation state");
  }
  else if (modality.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("Modality VM != 1 in presentation state");
  }

  sopclassuid.getOFString(aString,0);
  if (aString != UID_GrayscaleSoftcopyPresentationStateStorage)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("SOP Class UID does not match GrayscaleSoftcopyPresentationStateStorage");
  }

  modality.getOFString(aString,0);
  if (aString != "PR")
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("Modality does not match 'PR' for presentation state");
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
    READ_FROM_DATASET(DcmPersonName, referringPhysicianName)
    READ_FROM_DATASET(DcmShortString, studyID)
    READ_FROM_DATASET(DcmShortString, accessionNumber)
    READ_FROM_DATASET(DcmUniqueIdentifier, seriesInstanceUID)
    READ_FROM_DATASET(DcmIntegerString, seriesNumber)
    READ_FROM_DATASET(DcmLongString, manufacturer)
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
    READ_FROM_DATASET(DcmUnsignedShort, imageRotation)
    READ_FROM_DATASET(DcmCodeString, imageHorizontalFlip)
    READ_FROM_DATASET(DcmDecimalString, rescaleIntercept)
    READ_FROM_DATASET(DcmDecimalString, rescaleSlope)
    READ_FROM_DATASET(DcmLongString, rescaleType)
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
        DCMPSTAT_INFO("Modality LUT SQ does not have exactly one item in presentation state");
      }
    }
  }

  if (result==EC_Normal) result = overlayList.read(dset);
  if (result==EC_Normal) result = activationLayerList.read(dset);
  if (result==EC_Normal) result = graphicLayerList.read(dset);
  if (result==EC_Normal) result = referencedSeriesList.read(dset);
  if (result==EC_Normal) result = graphicAnnotationList.read(dset);
  if (result==EC_Normal) result = displayedAreaSelectionList.read(dset);
  if (result==EC_Normal) result = softcopyVOIList.read(dset);
  if (result==EC_Normal) result = presentationLUT.read(dset, OFFalse);

  /* Now perform basic sanity checks and adjust use flags */

  if (result==EC_Normal)
  {

  if (patientName.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("patientName absent or empty in presentation state");
  }
  else if (patientName.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("patientName VM != 1 in presentation state");
  }

  if (studyInstanceUID.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("studyInstanceUID absent or empty in presentation state");
  }
  else if (studyInstanceUID.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("studyInstanceUID VM != 1 in presentation state");
  }

  if (displayedAreaSelectionList.size() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("displayedAreaSelectionSQ absent or empty in presentation state");
  }

  if (imageNumber.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("instanceNumber absent or empty in presentation state");
  }
  else if (imageNumber.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("instanceNumber VM != 1 in presentation state");
  }

  if (presentationLabel.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentationLabel absent or empty in presentation state");
  }
  else if (presentationLabel.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentationLabel VM != 1 in presentation state");
  }

  if (presentationCreationDate.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentationCreationDate absent or empty in presentation state");
  }
  else if (presentationCreationDate.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentationCreationDate VM != 1 in presentation state");
  }

  if (presentationCreationTime.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentationCreationTime absent or empty in presentation state");
  }
  else if (presentationCreationTime.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentationCreationTime VM != 1 in presentation state");
  }

  if (sOPInstanceUID.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("sOPInstanceUID absent or empty in presentation state");
  }
  else if (sOPInstanceUID.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("sOPInstanceUID VM != 1 in presentation state");
  }

    /* if imageRotation or imageHorizontalFlip are present, then both must be present. */
    if ((imageRotation.getLength() > 0)&&(imageHorizontalFlip.getLength() == 0))
    {
      result=EC_IllegalCall;
      DCMPSTAT_INFO("imageRotation present but imageHorizontalFlip absent or empty in presentation state");
    }

    if ((imageRotation.getLength() == 0)&&(imageHorizontalFlip.getLength() > 0))
    {
      result=EC_IllegalCall;
      DCMPSTAT_INFO("imageHorizontalFlip present but imageRotation absent or empty in presentation state");
    }

    /* Modality LUT */
    if (rescaleIntercept.getLength() > 0)
    {
      useModalityRescale = OFTrue;

      if (rescaleSlope.getLength() == 0)
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("rescaleIntercept present but rescaleSlope absent or empty in presentation state");
      }
      else if (rescaleSlope.getVM() != 1)
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("rescaleIntercept present but rescaleSlope VM != 1 in presentation state");
      }
      if (rescaleType.getLength() == 0)
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("rescaleIntercept present but rescaleType absent or empty in presentation state");
      }
      else if (rescaleType.getVM() != 1)
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("rescaleIntercept present but rescaleType VM != 1 in presentation state");
      }
      if (rescaleIntercept.getVM() != 1)
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("rescaleIntercept present but VM != 1 in presentation state");
      }
    } else useModalityRescale = OFFalse;
    if (modalityLUTData.getLength() > 0)
    {
      useModalityLUT = OFTrue;

      if (modalityLUTDescriptor.getLength() == 0)
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("modalityLUTData present but modalityLUTDescriptor absent or empty in presentation state");
      }
      else if (modalityLUTDescriptor.getVM() != 3)
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("modalityLUTData present but modalityLUTDescriptor VM != 3 in presentation state");
      }

    } else useModalityLUT = OFFalse;
    /* we may not have both modality rescale and LUT */
    if (useModalityRescale && useModalityLUT)
    {
      result=EC_IllegalCall;
      DCMPSTAT_INFO("both modality rescale and LUT present in presentation state");
    }

  } /* end result==EC_Normal */

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
      else
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("unknown shutter type '" << aString << "'");
      }
    }

    if (useShutterRectangular)
    {
      if ((shutterLeftVerticalEdge.getLength() == 0)||(shutterLeftVerticalEdge.getVM() != 1))
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("rectangular shutter used but shutterLeftVerticalEdge absent or VM != 1 in presentation state");
      }
      if ((shutterRightVerticalEdge.getLength() == 0)||(shutterRightVerticalEdge.getVM() != 1))
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("rectangular shutter used but shutterRightVerticalEdge absent or VM != 1 in presentation state");
      }
      if ((shutterUpperHorizontalEdge.getLength() == 0)||(shutterUpperHorizontalEdge.getVM() != 1))
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("rectangular shutter used but shutterUpperHorizontalEdge absent or VM != 1 in presentation state");
      }
      if ((shutterLowerHorizontalEdge.getLength() == 0)||(shutterLowerHorizontalEdge.getVM() != 1))
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("rectangular shutter used but shutterLowerHorizontalEdge absent or VM != 1 in presentation state");
      }
    }
    if (useShutterCircular)
    {
      if ((centerOfCircularShutter.getLength() == 0)||(centerOfCircularShutter.getVM() != 2))
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("circular shutter used but centerOfCircularShutter absent or VM != 2 in presentation state");
      }
      if ((radiusOfCircularShutter.getLength() == 0)||(radiusOfCircularShutter.getVM() != 1))
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("circular shutter used but radiusOfCircularShutter absent or VM != 1 in presentation state");
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
        DCMPSTAT_INFO("polygonal shutter used but verticesOfThePolygonalShutter absent or VM != 2-2n in presentation state");
      }
    }
    if (useShutterBitmap)
    {
      if ((shutterOverlayGroup.getLength() == 0)||(shutterOverlayGroup.getVM() != 1))
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("bitmap shutter used but shutterOverlayGroup absent or VM != 1 in presentation state");
      }
    }
    if (useShutterRectangular || useShutterCircular || useShutterPolygonal || useShutterBitmap)
    {
      if ((shutterPresentationValue.getLength() == 0)||(shutterPresentationValue.getVM() != 1))
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("shutter used but shutterPresentationValue absent or VM != 1 in presentation state");
      }
    }
    /* we can either have rect/circ/poly shutter or bitmap shutter but not both */
    if (useShutterBitmap && (useShutterRectangular || useShutterCircular || useShutterPolygonal))
    {
      result=EC_IllegalCall;
      DCMPSTAT_INFO("both bitmap and rect/circ/poly shutter specified in presentation state");
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
        DCMPSTAT_INFO("bitmap shutter specified but referenced overlay group missing in presentation state");
      }
    } else {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("wrong overlay group specified for bitmap shutter in presentation state");
    }
  }

  /* check that referenced series list is non-empty and that every series contains images
     and that all referenced images share the same SOP class UID.
   */
  if (result.good() && (!referencedSeriesList.isValid()))
  {
    // referencedSeriesList.isValid() has already displayed an error message in verbose mode
    result = EC_IllegalCall;
  }

  /* check that there is no mask module present. We don't support Mask and therefore
     are obliged to reject any presentation state containing a mask! */

  stack.clear();
  if (EC_Normal == dset.search(DCM_MaskSubtractionSequence, stack, ESM_fromHere, OFFalse))
  {
      result=EC_IllegalCall;
      DCMPSTAT_INFO("mask module present in presentation state, but unsupported");
  }
  stack.clear();
  if (EC_Normal == dset.search(DCM_RecommendedViewingMode, stack, ESM_fromHere, OFFalse))
  {
      result=EC_IllegalCall;
      DCMPSTAT_INFO("mask module present in presentation state, but unsupported");
  }

  return result;
}

OFCondition DcmPresentationState::createDefaultDisplayedArea(DcmItem &dset)
{
  DcmUnsignedShort rows(DCM_Rows);
  DcmUnsignedShort columns(DCM_Columns);
  DcmUniqueIdentifier sopclassUID(DCM_SOPClassUID);
  DcmUniqueIdentifier imageUID(DCM_SOPInstanceUID);
  DcmDecimalString imagerPixelSpacing(DCM_ImagerPixelSpacing);
  DcmDecimalString pixelSpacing(DCM_PixelSpacing);
  DcmIntegerString pixelAspectRatio(DCM_PixelAspectRatio);
  Uint16 cols_uint=0;
  Uint16 rows_uint=0;
  char *c_sopclassUID = NULL;
  char *c_imageUID = NULL;
  char *pixelspacing = NULL;
  OFCondition result = EC_Normal;
  DcmStack stack;

  READ_FROM_DATASET(DcmUnsignedShort, rows)
  READ_FROM_DATASET(DcmUnsignedShort, columns)
  READ_FROM_DATASET(DcmUniqueIdentifier, sopclassUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, imageUID)
  READ_FROM_DATASET(DcmDecimalString, imagerPixelSpacing)
  READ_FROM_DATASET(DcmDecimalString, pixelSpacing)
  READ_FROM_DATASET(DcmIntegerString, pixelAspectRatio)

  if (result == EC_Normal) result = columns.getUint16(cols_uint,0);
  if (result == EC_Normal) result = rows.getUint16(rows_uint,0);
  if (result == EC_Normal) result = sopclassUID.getString(c_sopclassUID);
  if (result == EC_Normal) result = imageUID.getString(c_imageUID);

  /* create displayed area selection item for this image */
  if (EC_Normal == result)
  {
    DVPSDisplayedArea *area = displayedAreaSelectionList.createDisplayedArea(
      referencedSeriesList, c_sopclassUID, c_imageUID, 1, 1, DVPSB_currentImage);
    if (area)
    {
      if (EC_Normal == result) result = area->setDisplayedArea(DVPSD_scaleToFit, (Sint32)1, (Sint32)1, (Sint32)cols_uint, (Sint32)rows_uint);
      if (EC_Normal == result)
      {
        OFBool foundRatio = OFFalse;
        if ((imagerPixelSpacing.getVM()==2)&&(EC_Normal==imagerPixelSpacing.getString(pixelspacing)))
        {
          result = area->setDisplayedAreaPixelSpacing(pixelspacing);
          if (EC_Normal==result) foundRatio = OFTrue;
        }
        if ((! foundRatio)&&(pixelSpacing.getVM()==2)&&(EC_Normal==pixelSpacing.getString(pixelspacing)))
        {
          result = area->setDisplayedAreaPixelSpacing(pixelspacing);
          if (EC_Normal==result) foundRatio = OFTrue;
        }
        if ((! foundRatio)&&(pixelAspectRatio.getVM()==2)&&(EC_Normal==pixelAspectRatio.getString(pixelspacing)))
        {
          result = area->setDisplayedAreaPixelAspectRatio(pixelspacing);
          if (EC_Normal==result) foundRatio = OFTrue;
        }
        if (! foundRatio) result = area->setDisplayedAreaPixelAspectRatio(1.0);
      }
    } else result = EC_MemoryExhausted;
  }
  return result;
}

OFCondition DcmPresentationState::createFromImage(
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
  OFCondition result = EC_Normal;
  DcmStack stack;
  DcmSequenceOfItems *seq;
  DcmItem *item;
  char uid[100];
  OFString aString;
  OFString ofsopclassUID;
  OFString ofimageUID;
  OFBool isMonochrome1 = OFFalse;
  DcmUniqueIdentifier sopclassUID(DCM_SOPClassUID);
  DcmUniqueIdentifier imageUID(DCM_SOPInstanceUID);
  DcmCodeString photometricInterpretation(DCM_PhotometricInterpretation);
  DcmCodeString presentationLUTShape(DCM_PresentationLUTShape);

  clear(); // re-initialize Presentation State object

  /* copy some image attributes we will need later */
  READ_FROM_DATASET(DcmCodeString, photometricInterpretation)
  READ_FROM_DATASET(DcmUniqueIdentifier, sopclassUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, imageUID)
  sopclassUID.getOFString(ofsopclassUID,0);
  imageUID.getOFString(ofimageUID,0);

  /* copy attributes for Patient, General Study and General Equipment
   * modules from image object. Also copy specific character set (SOP common). */
  READ_FROM_DATASET(DcmPersonName, patientName)
  READ_FROM_DATASET(DcmLongString, patientID)
  READ_FROM_DATASET(DcmDate, patientBirthDate)
  READ_FROM_DATASET(DcmCodeString, patientSex)
  READ_FROM_DATASET(DcmUniqueIdentifier, studyInstanceUID)
  READ_FROM_DATASET(DcmDate, studyDate)
  READ_FROM_DATASET(DcmTime, studyTime)
  READ_FROM_DATASET(DcmPersonName, referringPhysicianName)
  READ_FROM_DATASET(DcmShortString, studyID)
  READ_FROM_DATASET(DcmShortString, accessionNumber)
  READ_FROM_DATASET(DcmLongString, manufacturer)
  READ_FROM_DATASET(DcmCodeString, specificCharacterSet)

  /* if there is a modality rescale, copy it now.
   * Later we decide whether we want to use it.
   */
  READ_FROM_DATASET(DcmDecimalString, rescaleIntercept)
  READ_FROM_DATASET(DcmDecimalString, rescaleSlope)
  READ_FROM_DATASET(DcmLongString, rescaleType)

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
      DCMPSTAT_INFO("Wrong image photometric interpretation - not MONOCHROME1/2");
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
    DVPSHelper::currentDate(aString);
    result = presentationCreationDate.putString(aString.c_str());
  }
  if (result==EC_Normal)
  {
    aString.clear();
    DVPSHelper::currentTime(aString);
    result = presentationCreationTime.putString(aString.c_str());
  }
  if (result==EC_Normal) result = seriesInstanceUID.putString(dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT));
  if (result==EC_Normal) result = sOPInstanceUID.putString(dcmGenerateUniqueIdentifier(uid));
  if (result==EC_Normal) result = seriesNumber.putString(DEFAULT_seriesNumber);
  if (result==EC_Normal) result = specificCharacterSet.putString(DEFAULT_specificCharacterSet);

  if (result==EC_Normal)
  {
    /* Presentation LUT Shape */
    if (isMonochrome1) presentationLUT.setType(DVPSP_inverse); else presentationLUT.setType(DVPSP_identity);
    if (presentationLUTShape.getVM() == 1)
    {
      aString.clear();
      presentationLUTShape.getOFString(aString,0);
      if (aString == "IDENTITY") presentationLUT.setType(DVPSP_identity);
      if (aString == "INVERSE") presentationLUT.setType(DVPSP_inverse);
      if (aString == "LIN OD")
      {
        DCMPSTAT_INFO("LIN OD found in presentation state; set to IDENTITY");
        presentationLUT.setType(DVPSP_identity);
      }
    }
  }

  if (result==EC_Normal)
  {
    /* Modality LUT */
    if ((rescaleIntercept.getVM() == 1) && (rescaleSlope.getVM() == 1))
    {
      useModalityRescale = OFTrue;
      modalityLUTData.clear();
      modalityLUTDescriptor.clear();
      modalityLUTExplanation.clear();
      modalityLUTType.clear();
      // be lenient with Modality rescale type. If missing or wrong, just invent a value.
      if (rescaleType.getVM() != 1) rescaleType.putString("US");
    } else useModalityRescale = OFFalse;

    // these three SOP classes use the X-Ray Image Module in which the meaning
    // of the Modality LUT transformation is "inversersed" and, therefore,
    // must not be copied into the presentation state
    if ((ofsopclassUID == UID_XRayAngiographicImageStorage) ||
        (ofsopclassUID == UID_XRayRadiofluoroscopicImageStorage) ||
        (ofsopclassUID == UID_RETIRED_XRayAngiographicBiPlaneImageStorage))
    {
      modalityLUTData.clear();
      modalityLUTDescriptor.clear();
      modalityLUTExplanation.clear();
      modalityLUTType.clear();
      useModalityLUT = OFFalse;
    }

    if ((modalityLUTDescriptor.getVM() == 3) && (modalityLUTData.getLength() > 0))
    {
      useModalityLUT = OFTrue;
      rescaleIntercept.clear();
      rescaleSlope.clear();
      rescaleType.clear();
    } else useModalityLUT = OFFalse;
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

  /* create reference for this image */
  if (EC_Normal == result) result = addImageReference(dset, aetitle, filesetID, filesetUID);

  /* create displayed area selection item for this image */
  if (EC_Normal == result) result = createDefaultDisplayedArea(dset);

  /* create softcopy VOI LUT sequence item for this image if necessary */
  if (EC_Normal == result) result = softcopyVOIList.createFromImage(dset,
    referencedSeriesList, ofsopclassUID.c_str(), ofimageUID.c_str(), voiActivation);

  return result;
}


OFCondition DcmPresentationState::write(DcmItem &dset, OFBool replaceSOPInstanceUID)
{
  OFCondition result = EC_Normal;
  DcmElement *delem=NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  cleanupLayers(); /* remove unused layers */
  createDummyValues(replaceSOPInstanceUID);

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
  ADD_TO_DATASET(DcmPersonName, referringPhysicianName)
  ADD_TO_DATASET(DcmShortString, studyID)
  ADD_TO_DATASET(DcmShortString, accessionNumber)
  ADD_TO_DATASET(DcmCodeString, modality)
  ADD_TO_DATASET(DcmUniqueIdentifier, seriesInstanceUID)
  ADD_TO_DATASET(DcmIntegerString, seriesNumber)
  ADD_TO_DATASET(DcmLongString, manufacturer)
  ADD_TO_DATASET(DcmIntegerString, imageNumber)
  ADD_TO_DATASET(DcmCodeString, presentationLabel)
  ADD_TO_DATASET(DcmLongString, presentationDescription)
  ADD_TO_DATASET(DcmDate, presentationCreationDate)
  ADD_TO_DATASET(DcmTime, presentationCreationTime)
  ADD_TO_DATASET(DcmPersonName, presentationCreatorsName)


  ADD_TO_DATASET(DcmUniqueIdentifier, sOPInstanceUID)
  if (specificCharacterSet.getLength() >0) { ADD_TO_DATASET(DcmCodeString, specificCharacterSet) }
  if (instanceCreationDate.getLength() >0) { ADD_TO_DATASET(DcmDate, instanceCreationDate) }
  if (instanceCreationTime.getLength() >0) { ADD_TO_DATASET(DcmTime, instanceCreationTime) }
  if (instanceCreatorUID.getLength() >0) { ADD_TO_DATASET(DcmUniqueIdentifier, instanceCreatorUID) }

  shutterShape.clear();
  if (useShutterRectangular || useShutterCircular || useShutterPolygonal)
  {
    OFString aString;
    if (useShutterRectangular) aString = "RECTANGULAR";
    if (useShutterCircular)
    {
      if (aString.size() > 0) aString += '\\';
      aString += "CIRCULAR";
    }
    if (useShutterPolygonal)
    {
      if (aString.size() > 0) aString += '\\';
      aString += "POLYGONAL";
    }
    shutterShape.putString(aString.c_str());
  } else if (useShutterBitmap)
  {
    shutterShape.putString("BITMAP");
  }
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

  if (EC_Normal == result) result = referencedSeriesList.write(dset);
  if (EC_Normal == result) result = overlayList.write(dset);
  if (EC_Normal == result) result = activationLayerList.write(dset);
  if (EC_Normal == result) result = graphicAnnotationList.write(dset);
  if (EC_Normal == result) result = displayedAreaSelectionList.write(dset);
  if (EC_Normal == result) result = softcopyVOIList.write(dset);
  if (EC_Normal == result) result = graphicLayerList.write(dset);
  if (EC_Normal == result) result = presentationLUT.write(dset, OFFalse);

  // strictly speaking we are not allowed to include the Spatial Transformation
  // Module if neither rotation nor flipping are needed.
  OFString aString;
  OFBool haveRotation=OFFalse;
  OFBool haveFlip=OFFalse;
  if (imageRotation.getVM() == 1)
  {
    Uint16 urot=0;
    imageRotation.getUint16(urot,0);
    if (urot) haveRotation=OFTrue;
  }
  if (imageHorizontalFlip.getLength() > 0)
  {
    imageHorizontalFlip.getOFString(aString, 0, OFTrue);
    if (aString == "Y") haveFlip=OFTrue;
  }

  if (haveRotation || haveFlip)
  {
    ADD_TO_DATASET(DcmUnsignedShort, imageRotation)
    ADD_TO_DATASET(DcmCodeString, imageHorizontalFlip)
  }

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
          if (delem) ditem->insert(delem, OFTrue /*replaceOld*/); else result=EC_MemoryExhausted;
          delem = new DcmUnsignedShort(modalityLUTData);
          if (delem) ditem->insert(delem, OFTrue /*replaceOld*/); else result=EC_MemoryExhausted;
          delem = new DcmLongString(modalityLUTType);
          if (delem) ditem->insert(delem, OFTrue /*replaceOld*/); else result=EC_MemoryExhausted;
          if (modalityLUTExplanation.getLength() >0)
          {
            delem = new DcmLongString(modalityLUTExplanation);
            if (delem) ditem->insert(delem, OFTrue /*replaceOld*/); else result=EC_MemoryExhausted;
          }
          if (result==EC_Normal)
          {
            dseq->insert(ditem);
            dset.insert(dseq, OFTrue /*replaceOld*/);
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


OFCondition DcmPresentationState::addImageReference(
    const char *studyUID,
    const char *seriesUID,
    const char *sopclassUID,
    const char *instanceUID,
    const char *frames,
    const char *aetitle,
    const char *filesetID,
    const char *filesetUID)
{
  if ((studyUID==NULL)||(seriesUID==NULL)||(sopclassUID==NULL)||(instanceUID==NULL)) return EC_IllegalCall;

  OFString study;
  studyInstanceUID.getOFString(study,0);
  if (study != studyUID)
  {
    DCMPSTAT_INFO("cannot add reference to image with different Study Instance UID.");
    return EC_IllegalCall;
  }
  return referencedSeriesList.addImageReference(seriesUID, sopclassUID, instanceUID, frames, aetitle, filesetID, filesetUID);
}


OFCondition DcmPresentationState::addImageReference(
    DcmItem &dset,
    const char *aetitle,
    const char *filesetID,
    const char *filesetUID)
{
  DcmIntegerString numberOfFrames(DCM_NumberOfFrames);
  DcmUniqueIdentifier studyUID(DCM_StudyInstanceUID);
  DcmUniqueIdentifier seriesUID(DCM_SeriesInstanceUID);
  DcmUniqueIdentifier sopclassUID(DCM_SOPClassUID);
  DcmUniqueIdentifier imageUID(DCM_SOPInstanceUID);

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

  OFCondition result = EC_Normal;
  /* if the image is multiframe, reference all frames */
  if (ofnumberOfFrames > 1)
  {
    for (i=0; i<ofnumberOfFrames; i++)
    {
        if (aString.length() > 0) sprintf(buf, "\\%ld", (long)(i+1)); else sprintf(buf, "%ld", (long)(i+1));
        aString += buf;
    }
    result = addImageReference(ofstudyUID.c_str(), ofseriesUID.c_str(), ofsopclassUID.c_str(),
      ofimageUID.c_str(), aString.c_str(), aetitle, filesetID, filesetUID);
  } else {
    result = addImageReference(ofstudyUID.c_str(), ofseriesUID.c_str(), ofsopclassUID.c_str(),
      ofimageUID.c_str(), NULL, aetitle, filesetID, filesetUID);
  }
  /* create default displayed area entry for the new referenced image */
  if (result == EC_Normal)
    createDefaultDisplayedArea(dset);
  /* what is about VOI LUT sequence, annotations, etc. ? */
  return result;
}


OFCondition DcmPresentationState::removeImageReference(
    const char *studyUID,
    const char *seriesUID,
    const char *instanceUID)
{
  if ((studyUID==NULL)||(seriesUID)||(instanceUID==NULL)) return EC_IllegalCall;

  OFString study;
  studyInstanceUID.getOFString(study,0);
  if (study != studyUID) return EC_IllegalCall;
  referencedSeriesList.removeImageReference(seriesUID, instanceUID);

  /* still need to remove references from displayed area list, etc. */

  return EC_Normal;
}


OFCondition DcmPresentationState::removeImageReference(DcmItem &dset)
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

  /* still need to remove references from displayed area list, etc. */

  return EC_Normal;
}

size_t DcmPresentationState::numberOfImageReferences()
{
  return referencedSeriesList.numberOfImageReferences();
}

OFCondition DcmPresentationState::getImageReference(
    size_t idx,
    OFString& studyUID,
    OFString& seriesUID,
    OFString& sopclassUID,
    OFString& instanceUID,
    OFString& frames,
    OFString& aetitle,
    OFString& filesetID,
    OFString& filesetUID)
{
  OFCondition result = referencedSeriesList.getImageReference(idx, seriesUID, sopclassUID,
    instanceUID, frames, aetitle, filesetID, filesetUID);
  if (EC_Normal == result) result = studyInstanceUID.getOFString(studyUID,0);
  return result;
}

DVPSRotationType DcmPresentationState::getRotation()
{
  DVPSRotationType result = DVPSR_0_deg;
  Uint16 rotation=0;
  imageRotation.getUint16(rotation,0);
  if (rotation==90) result = DVPSR_90_deg;
  else if (rotation==180) result = DVPSR_180_deg;
  else if (rotation==270) result = DVPSR_270_deg;
  return result;
}


OFBool DcmPresentationState::getFlip()
{
  OFBool result = OFFalse;
  OFString flip;
  imageHorizontalFlip.getOFString(flip,0);
  if (flip=="Y") result = OFTrue;
  return result;
}


OFCondition DcmPresentationState::setRotation(DVPSRotationType rotation)
{
  // Re-compute all displayed areas
  DVPSRotationType oldRotation = getRotation();
  OFBool flip = getFlip();
  displayedAreaSelectionList.rotateAndFlip(oldRotation, flip, rotation, flip);

  OFCondition result = EC_Normal;
  switch (rotation)
  {
    case DVPSR_0_deg:
      result = imageRotation.putUint16(0);
      break;
    case DVPSR_90_deg:
      result = imageRotation.putUint16(90);
      break;
    case DVPSR_180_deg:
      result = imageRotation.putUint16(180);
      break;
    case DVPSR_270_deg:
      result = imageRotation.putUint16(270);
      break;
  }

  return result;
}


OFCondition DcmPresentationState::setFlip(OFBool isFlipped)
{
  // Re-compute all displayed areas
  DVPSRotationType rotation = getRotation();
  OFBool oldFlip = getFlip();
  displayedAreaSelectionList.rotateAndFlip(rotation, oldFlip, rotation, isFlipped);

  if (isFlipped) return imageHorizontalFlip.putString("Y");
  else return imageHorizontalFlip.putString("N");
}


OFBool DcmPresentationState::haveShutter(DVPSShutterType type)
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


Sint32 DcmPresentationState::getRectShutterLV()
{
  Sint32 result=0;
  shutterLeftVerticalEdge.getSint32(result,0);
  return result;
}

Sint32 DcmPresentationState::getRectShutterRV()
{
  Sint32 result=0;
  shutterRightVerticalEdge.getSint32(result,0);
  return result;
}

Sint32 DcmPresentationState::getRectShutterUH()
{
  Sint32 result=0;
  shutterUpperHorizontalEdge.getSint32(result,0);
  return result;
}

Sint32 DcmPresentationState::getRectShutterLH()
{
  Sint32 result=0;
  shutterLowerHorizontalEdge.getSint32(result,0);
  return result;
}


Sint32 DcmPresentationState::getCenterOfCircularShutter_x()
{
  Sint32 result=0;
  centerOfCircularShutter.getSint32(result,1);
  return result;
}

Sint32 DcmPresentationState::getCenterOfCircularShutter_y()
{
  Sint32 result=0;
  centerOfCircularShutter.getSint32(result,0);
  return result;
}

Sint32 DcmPresentationState::getRadiusOfCircularShutter()
{
  Sint32 result=0;
  radiusOfCircularShutter.getSint32(result,0);
  return result;
}


size_t DcmPresentationState::getNumberOfPolyShutterVertices()
{
  return (verticesOfThePolygonalShutter.getVM() / 2);
}

OFCondition DcmPresentationState::getPolyShutterVertex(size_t idx, Sint32& x, Sint32& y)
{
  x=0;
  y=0;
  OFCondition result = verticesOfThePolygonalShutter.getSint32(y,2*idx);
  if (EC_Normal==result) result = verticesOfThePolygonalShutter.getSint32(x,2*idx+1);
  return result;
}

OFCondition DcmPresentationState::setPolyShutterOrigin(Sint32 x, Sint32 y)
{
  char buf[80];
  useShutterPolygonal = OFFalse;
  verticesOfThePolygonalShutter.clear();
  sprintf(buf, "%ld\\%ld", (long)y, (long)x);
  return verticesOfThePolygonalShutter.putString(buf);
}


Uint16 DcmPresentationState::getShutterPresentationValue()
{
  Uint16 result=0;
  shutterPresentationValue.getUint16(result,0);
  return result;
}

OFCondition DcmPresentationState::setShutterPresentationValue(Uint16 pvalue)
{
  return shutterPresentationValue.putUint16(pvalue,0);
}

const char *DcmPresentationState::getPresentationLabel()
{
  char *value = NULL;
  if (EC_Normal != presentationLabel.getString(value)) return NULL;
  return value;
}

const char *DcmPresentationState::getPresentationDescription()
{
  char *value = NULL;
  if (EC_Normal != presentationDescription.getString(value)) return NULL;
  return value;
}

const char *DcmPresentationState::getPresentationCreatorsName()
{
  char *value = NULL;
  if (EC_Normal != presentationCreatorsName.getString(value)) return NULL;
  return value;
}

OFCondition DcmPresentationState::setPresentationLabel(const char *label)
{
  return presentationLabel.putString(label);
}

OFCondition DcmPresentationState::setPresentationDescription(const char *descr)
{
  return presentationDescription.putString(descr);
}

OFCondition DcmPresentationState::setPresentationCreatorsName(const char *name)
{
  return presentationCreatorsName.putString(name);
}


void DcmPresentationState::sortGraphicLayers()
{
  graphicLayerList.sortGraphicLayers();
  return;
}

size_t DcmPresentationState::getNumberOfGraphicLayers()
{
  return graphicLayerList.size();
}

const char *DcmPresentationState::getGraphicLayerName(size_t idx)
{
  return graphicLayerList.getGraphicLayerName(idx);
}

size_t DcmPresentationState::getGraphicLayerIndex(const char *name)
{
  return graphicLayerList.getGraphicLayerIndex(name);
}

const char *DcmPresentationState::getGraphicLayerDescription(size_t idx)
{
  return graphicLayerList.getGraphicLayerDescription(idx);
}

OFBool DcmPresentationState::haveGraphicLayerRecommendedDisplayValue(size_t idx)
{
  return graphicLayerList.haveGraphicLayerRecommendedDisplayValue(idx);
}

OFCondition DcmPresentationState::getGraphicLayerRecommendedDisplayValueGray(size_t idx, Uint16& gray)
{
  return graphicLayerList.getGraphicLayerRecommendedDisplayValueGray(idx,gray);
}

OFCondition DcmPresentationState::getGraphicLayerRecommendedDisplayValueRGB(size_t idx, Uint16& r, Uint16& g, Uint16& b)
{
  return graphicLayerList.getGraphicLayerRecommendedDisplayValueRGB(idx,r,g,b);
}

OFCondition DcmPresentationState::setGraphicLayerRecommendedDisplayValueGray(size_t idx, Uint16 gray)
{
  return graphicLayerList.setGraphicLayerRecommendedDisplayValueGray(idx,gray);
}

OFCondition DcmPresentationState::setGraphicLayerRecommendedDisplayValueRGB(size_t idx, Uint16 r, Uint16 g, Uint16 b)
{
  return graphicLayerList.setGraphicLayerRecommendedDisplayValueRGB(idx,r,g,b);
}

void DcmPresentationState::removeGraphicLayerRecommendedDisplayValue(size_t idx, OFBool rgb, OFBool monochrome)
{
  graphicLayerList.removeGraphicLayerRecommendedDisplayValue(idx, rgb, monochrome);
  return;
}

OFCondition DcmPresentationState::setGraphicLayerName(size_t idx, const char *name)
{
  const char *oname = graphicLayerList.getGraphicLayerName(idx);
  if (name==NULL) return EC_IllegalCall;
  OFString oldName(oname); // make a copy of the pointer which will become invalid
  OFCondition result=graphicLayerList.setGraphicLayerName(idx, name);
  if (EC_Normal==result)
  {
    activationLayerList.renameLayer(oldName.c_str(), name);
    graphicAnnotationList.renameLayer(oldName.c_str(), name);
  }
  return result;
}

OFCondition DcmPresentationState::setGraphicLayerDescription(size_t idx, const char *descr)
{
  return graphicLayerList.setGraphicLayerDescription(idx, descr);
}

OFCondition DcmPresentationState::toFrontGraphicLayer(size_t idx)
{
  return graphicLayerList.toFrontGraphicLayer(idx);
}

OFCondition DcmPresentationState::toBackGraphicLayer(size_t idx)
{
  return graphicLayerList.toBackGraphicLayer(idx);
}

OFCondition DcmPresentationState::exchangeGraphicLayers(size_t idx1, size_t idx2)
{
  return graphicLayerList.exchangeGraphicLayers(idx1, idx2);
}

OFCondition DcmPresentationState::addGraphicLayer(
     const char *gLayer,
     const char *gLayerDescription)
{
  return graphicLayerList.addGraphicLayer(gLayer,gLayerDescription);
}

void DcmPresentationState::cleanupLayers()
{
  graphicAnnotationList.cleanupLayers();
  graphicLayerList.cleanupLayers(activationLayerList, graphicAnnotationList);
}


OFCondition DcmPresentationState::setCharset(DVPScharacterSet charset)
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


DVPScharacterSet DcmPresentationState::getCharset()
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

const char *DcmPresentationState::getCharsetString()
{
  char *c = NULL;
  if (EC_Normal == specificCharacterSet.getString(c)) return c; else return NULL;
}


size_t DcmPresentationState::getNumberOfCurves(size_t layer)
{
  return activationLayerList.getNumberOfActivations(
    graphicLayerList.getGraphicLayerName(layer), OFTrue);
}


OFCondition DcmPresentationState::removeCurve(size_t layer, size_t idx)
{
  Uint16 group = activationLayerList.getActivationGroup(
    graphicLayerList.getGraphicLayerName(layer), idx, OFTrue);
  if (group == 0) return EC_IllegalCall;
  activationLayerList.removeActivation(group);
  return EC_Normal;
}

OFCondition DcmPresentationState::moveCurve(size_t old_layer, size_t idx, size_t new_layer)
{
  const char *lname = graphicLayerList.getGraphicLayerName(new_layer);
  if (lname==NULL) return EC_IllegalCall;

  Uint16 group = activationLayerList.getActivationGroup(
    graphicLayerList.getGraphicLayerName(old_layer), idx, OFTrue);
  if (group == 0) return EC_IllegalCall;

  activationLayerList.removeActivation(group);
  return activationLayerList.setActivation(group, lname);
}


size_t DcmPresentationState::getNumberOfActiveOverlays(size_t layer)
{
  return activationLayerList.getNumberOfActivations(
    graphicLayerList.getGraphicLayerName(layer), OFFalse);
}

Uint16 DcmPresentationState::getActiveOverlayGroup(size_t layer, size_t idx)
{
  return activationLayerList.getActivationGroup(graphicLayerList.getGraphicLayerName(layer),
           idx, OFFalse);
}

size_t DcmPresentationState::getNumberOfOverlaysInPresentationState()
{
  return overlayList.size();
}

Uint16 DcmPresentationState::getOverlayInPresentationStateGroup(size_t idx)
{
  DVPSOverlay *overlay = overlayList.getOverlay(idx);
  if (overlay) return (0x6000+overlay->getOverlayGroup());
  return 0;
}

const char *DcmPresentationState::getOverlayInPresentationStateLabel(size_t idx)
{
  DVPSOverlay *overlay = overlayList.getOverlay(idx);
  if (overlay) return overlay->getOverlayLabel();
  return NULL;
}

const char *DcmPresentationState::getOverlayInPresentationStateDescription(size_t idx)
{
  DVPSOverlay *overlay = overlayList.getOverlay(idx);
  if (overlay) return overlay->getOverlayDescription();
  return NULL;
}

size_t DcmPresentationState::getOverlayInPresentationStateActivationLayer(size_t idx)
{
  DVPSOverlay *overlay = overlayList.getOverlay(idx);
  if (overlay)
  {
    const char *layerName = activationLayerList.getActivationLayer(0x6000+overlay->getOverlayGroup());
    if (layerName) return graphicLayerList.getGraphicLayerIndex(layerName);
  }
  return DVPS_IDX_NONE;
}

OFBool DcmPresentationState::overlayIsBitmapShutter(size_t idx)
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

OFBool DcmPresentationState::overlayInPresentationStateIsROI(size_t idx)
{
  DVPSOverlay *overlay = overlayList.getOverlay(idx);
  if (overlay) return overlay->isROI();
  return OFFalse;
}

OFCondition DcmPresentationState::moveOverlay(size_t old_layer, size_t idx, size_t new_layer)
{
  const char *lname = graphicLayerList.getGraphicLayerName(new_layer);
  if (lname==NULL) return EC_IllegalCall;

  Uint16 group = activationLayerList.getActivationGroup(
    graphicLayerList.getGraphicLayerName(old_layer), idx, OFFalse);
  if (group == 0) return EC_IllegalCall;

  activationLayerList.removeActivation(group);
  return activationLayerList.setActivation(group, lname);
}


/*
 *  $Log: dcmpstat.cc,v $
 *  Revision 1.15  2010-10-14 13:14:31  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.14  2010-09-24 13:32:58  joergr
 *  Compared names of SOP Class UIDs with 2009 edition of the DICOM standard. The
 *  resulting name changes are mainly caused by the fact that the corresponding
 *  SOP Class is now retired.
 *
 *  Revision 1.13  2010-08-09 13:21:56  joergr
 *  Updated data dictionary to 2009 edition of the DICOM standard. From now on,
 *  the official "keyword" is used for the attribute name which results in a
 *  number of minor changes (e.g. "PatientsName" is now called "PatientName").
 *
 *  Revision 1.12  2010-04-29 10:36:20  joergr
 *  Fixed typo in log message.
 *
 *  Revision 1.11  2009-12-15 14:50:49  uli
 *  Fixes some issues with --logfile and the config's log options.
 *
 *  Revision 1.10  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.9  2006-08-15 16:57:01  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.8  2005/12/08 15:46:14  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.7  2004/08/03 11:43:18  meichel
 *  Headers libc.h and unistd.h are now included via ofstdinc.h
 *
 *  Revision 1.6  2004/02/13 11:49:36  joergr
 *  Adapted code for changed tag names (e.g. PresentationLabel -> ContentLabel).
 *
 *  Revision 1.5  2004/02/04 15:57:48  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.4  2003/12/18 16:37:49  meichel
 *  During creation of default presentation state from image the Modality LUT
 *    is now ignored for XA, RF and XA Biplane SOP instances
 *
 *  Revision 1.3  2003/09/05 14:30:08  meichel
 *  Introduced new API methods that allow Displayed Areas to be queried
 *    and set either relative to the image (ignoring rotation and flip) or
 *    in absolute values as defined in the standard.  Rotate and flip methods
 *    now adjust displayed areas in the presentation state.
 *
 *  Revision 1.2  2003/09/05 08:37:46  meichel
 *  Fixed minor issue that caused certain error messages during the
 *    parse process on a GSPS object to be "swallowed".
 *
 *  Revision 1.1  2003/08/27 14:57:20  meichel
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
 *  Added new method convertPValueToDDL() to DcmPresentationState
 *    that maps P-Values to DDLs.
 *
 *  Revision 1.10  1999/02/17 10:05:35  meichel
 *  Moved creation of Display Function object from DcmPresentationState to
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
 *  added methods to DcmPresentationState allowing to access the image
 *    references in the presentation state.  Also added methods allowing to
 *    get the width and height of the attached image.
 *
 *  Revision 1.5  1999/01/11 13:35:51  meichel
 *  added new methods getImageAspectRatio, getImageMinMaxPixelRange and
 *    getImageMinMaxPixelValue to class DcmPresentationState.
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
