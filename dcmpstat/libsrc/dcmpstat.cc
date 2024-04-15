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
 *    classes: DcmPresentationState
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

OFLogger DCM_dcmpstatLogger = OFLog::getLogger("dcmtk.dcmpstat");
OFLogger DCM_dcmpstatDumpLogger = OFLog::getLogger("dcmtk.dcmpstat.dump");
OFLogger DCM_dcmpstatLogfileLogger = OFLog::getLogger("dcmtk.dcmpstat.logfile");

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
  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, sopclassuid)
  READ_FROM_DATASET(DcmCodeString, EVR_CS, modality)

  if (sopclassuid.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("SOPClassUID absent or empty in presentation state");
  }
  else if (sopclassuid.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("SOPClassUID VM != 1 in presentation state");
  }

  if (modality.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("Modality absent or empty in presentation state");
  }
  else if (modality.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("Modality VM != 1 in presentation state");
  }

  sopclassuid.getOFString(aString,0);
  if (aString != UID_GrayscaleSoftcopyPresentationStateStorage)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("SOP Class UID does not match GrayscaleSoftcopyPresentationStateStorage");
  }

  modality.getOFString(aString,0);
  if (aString != "PR")
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("Modality does not match 'PR' for presentation state");
  }

  if (result==EC_Normal)
  {
    READ_FROM_DATASET(DcmPersonName, EVR_PN, patientName)
    READ_FROM_DATASET(DcmLongString, EVR_LO, patientID)
    READ_FROM_DATASET(DcmDate, EVR_DA, patientBirthDate)
    READ_FROM_DATASET(DcmCodeString, EVR_CS, patientSex)
    READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, studyInstanceUID)
    READ_FROM_DATASET(DcmDate, EVR_DA, studyDate)
    READ_FROM_DATASET(DcmTime, EVR_TM, studyTime)
    READ_FROM_DATASET(DcmPersonName, EVR_PN, referringPhysicianName)
    READ_FROM_DATASET(DcmShortString, EVR_SH, studyID)
    READ_FROM_DATASET(DcmShortString, EVR_SH, accessionNumber)
    READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, seriesInstanceUID)
    READ_FROM_DATASET(DcmIntegerString, EVR_IS, seriesNumber)
    READ_FROM_DATASET(DcmLongString, EVR_LO, manufacturer)
    READ_FROM_DATASET(DcmIntegerString, EVR_IS, imageNumber)
    READ_FROM_DATASET(DcmCodeString, EVR_CS, presentationLabel)
    READ_FROM_DATASET(DcmLongString, EVR_LO, presentationDescription)
    READ_FROM_DATASET(DcmDate, EVR_DA, presentationCreationDate)
    READ_FROM_DATASET(DcmTime, EVR_TM, presentationCreationTime)
    READ_FROM_DATASET(DcmPersonName, EVR_PN, presentationCreatorsName)
    READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, sOPInstanceUID)
    READ_FROM_DATASET(DcmCodeString, EVR_CS, specificCharacterSet)
    READ_FROM_DATASET(DcmDate, EVR_DA, instanceCreationDate)
    READ_FROM_DATASET(DcmTime, EVR_TM, instanceCreationTime)
    READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, instanceCreatorUID)
    READ_FROM_DATASET(DcmCodeString, EVR_CS, shutterShape)
    READ_FROM_DATASET(DcmIntegerString, EVR_IS, shutterLeftVerticalEdge)
    READ_FROM_DATASET(DcmIntegerString, EVR_IS, shutterRightVerticalEdge)
    READ_FROM_DATASET(DcmIntegerString, EVR_IS, shutterUpperHorizontalEdge)
    READ_FROM_DATASET(DcmIntegerString, EVR_IS, shutterLowerHorizontalEdge)
    READ_FROM_DATASET(DcmIntegerString, EVR_IS, centerOfCircularShutter)
    READ_FROM_DATASET(DcmIntegerString, EVR_IS, radiusOfCircularShutter)
    READ_FROM_DATASET(DcmIntegerString, EVR_IS, verticesOfThePolygonalShutter)
    READ_FROM_DATASET(DcmUnsignedShort, EVR_US, shutterPresentationValue)
    READ_FROM_DATASET(DcmUnsignedShort, EVR_US, shutterOverlayGroup)
    READ_FROM_DATASET(DcmUnsignedShort, EVR_US, imageRotation)
    READ_FROM_DATASET(DcmCodeString, EVR_CS, imageHorizontalFlip)
    READ_FROM_DATASET(DcmDecimalString, EVR_DS, rescaleIntercept)
    READ_FROM_DATASET(DcmDecimalString, EVR_DS, rescaleSlope)
    READ_FROM_DATASET(DcmLongString, EVR_LO, rescaleType)
  }

  /* read Modality LUT Sequence */
  if (result==EC_Normal)
  {
    stack.clear();
    if ((EC_Normal == dset.search(DCM_ModalityLUTSequence, stack, ESM_fromHere, OFFalse)) && (stack.top()->ident() == EVR_SQ))
    {
      seq=(DcmSequenceOfItems *)stack.top();
      if (seq->card() ==1)
      {
         item = seq->getItem(0);
         stack.clear();

         // LUTDescriptor can be US or SS
         if ((EC_Normal == item->search((DcmTagKey &)modalityLUTDescriptor.getTag(),
           stack, ESM_fromHere, OFFalse)) && (stack.top()->ident() == EVR_US || stack.top()->ident() == EVR_SS))
         {
           // We explicitly use DcmElement::operator=(), which works for US and SS
           DcmElement *mLUTDescriptor = &modalityLUTDescriptor;
           mLUTDescriptor->operator=(* OFstatic_cast(DcmElement *, stack.top()));
         }

         stack.clear();
         if ((EC_Normal == item->search((DcmTagKey &)modalityLUTExplanation.getTag(),
           stack, ESM_fromHere, OFFalse)) && (stack.top()->ident() == EVR_LO))
         {
           modalityLUTExplanation = *((DcmLongString *)(stack.top()));
         }
         stack.clear();

         // LUTData can be OW, US or SS. For now we only handle US.
         if ((EC_Normal == item->search((DcmTagKey &)modalityLUTData.getTag(),
           stack, ESM_fromHere, OFFalse)) && (stack.top()->ident() == EVR_US || stack.top()->ident() == EVR_OW))
         {
           // we deliberately call DcmElement::operator=() here, which will work for both DcmUnsignedShort and DcmOtherByteOtherWord parameters
           DcmElement *mdata = &modalityLUTData;
           mdata->operator=(*(DcmElement *)(stack.top()));
         }
         stack.clear();
         if ((EC_Normal == item->search((DcmTagKey &)modalityLUTType.getTag(),
           stack, ESM_fromHere, OFFalse)) && (stack.top()->ident() == EVR_LO))
         {
           modalityLUTType = *((DcmLongString *)(stack.top()));
         }
      } else {
        result=EC_TagNotFound;
        DCMPSTAT_WARN("Modality LUT SQ does not have exactly one item in presentation state");
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
    DCMPSTAT_WARN("patientName absent or empty in presentation state");
  }
  else if (patientName.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("patientName VM != 1 in presentation state");
  }

  if (studyInstanceUID.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("studyInstanceUID absent or empty in presentation state");
  }
  else if (studyInstanceUID.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("studyInstanceUID VM != 1 in presentation state");
  }

  if (displayedAreaSelectionList.size() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("displayedAreaSelectionSQ absent or empty in presentation state");
  }

  if (imageNumber.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("instanceNumber absent or empty in presentation state");
  }
  else if (imageNumber.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("instanceNumber VM != 1 in presentation state");
  }

  if (presentationLabel.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentationLabel absent or empty in presentation state");
  }
  else if (presentationLabel.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentationLabel VM != 1 in presentation state");
  }

  if (presentationCreationDate.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentationCreationDate absent or empty in presentation state");
  }
  else if (presentationCreationDate.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentationCreationDate VM != 1 in presentation state");
  }

  if (presentationCreationTime.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentationCreationTime absent or empty in presentation state");
  }
  else if (presentationCreationTime.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentationCreationTime VM != 1 in presentation state");
  }

  if (sOPInstanceUID.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("sOPInstanceUID absent or empty in presentation state");
  }
  else if (sOPInstanceUID.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("sOPInstanceUID VM != 1 in presentation state");
  }

    /* if imageRotation or imageHorizontalFlip are present, then both must be present. */
    if ((imageRotation.getLength() > 0)&&(imageHorizontalFlip.getLength() == 0))
    {
      result=EC_IllegalCall;
      DCMPSTAT_WARN("imageRotation present but imageHorizontalFlip absent or empty in presentation state");
    }

    if ((imageRotation.getLength() == 0)&&(imageHorizontalFlip.getLength() > 0))
    {
      result=EC_IllegalCall;
      DCMPSTAT_WARN("imageHorizontalFlip present but imageRotation absent or empty in presentation state");
    }

    /* Modality LUT */
    if (rescaleIntercept.getLength() > 0)
    {
      useModalityRescale = OFTrue;

      if (rescaleSlope.getLength() == 0)
      {
        result=EC_IllegalCall;
        DCMPSTAT_WARN("rescaleIntercept present but rescaleSlope absent or empty in presentation state");
      }
      else if (rescaleSlope.getVM() != 1)
      {
        result=EC_IllegalCall;
        DCMPSTAT_WARN("rescaleIntercept present but rescaleSlope VM != 1 in presentation state");
      }
      if (rescaleType.getLength() == 0)
      {
        result=EC_IllegalCall;
        DCMPSTAT_WARN("rescaleIntercept present but rescaleType absent or empty in presentation state");
      }
      else if (rescaleType.getVM() != 1)
      {
        result=EC_IllegalCall;
        DCMPSTAT_WARN("rescaleIntercept present but rescaleType VM != 1 in presentation state");
      }
      if (rescaleIntercept.getVM() != 1)
      {
        result=EC_IllegalCall;
        DCMPSTAT_WARN("rescaleIntercept present but VM != 1 in presentation state");
      }
    } else useModalityRescale = OFFalse;
    if (modalityLUTData.getLength() > 0)
    {
      useModalityLUT = OFTrue;

      if (modalityLUTDescriptor.getLength() == 0)
      {
        result=EC_IllegalCall;
        DCMPSTAT_WARN("modalityLUTData present but modalityLUTDescriptor absent or empty in presentation state");
      }
      else if (modalityLUTDescriptor.getVM() != 3)
      {
        result=EC_IllegalCall;
        DCMPSTAT_WARN("modalityLUTData present but modalityLUTDescriptor VM != 3 in presentation state");
      }

    } else useModalityLUT = OFFalse;
    /* we may not have both modality rescale and LUT */
    if (useModalityRescale && useModalityLUT)
    {
      result=EC_IllegalCall;
      DCMPSTAT_WARN("both modality rescale and LUT present in presentation state");
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
        DCMPSTAT_WARN("unknown shutter type '" << aString << "'");
      }
    }

    if (useShutterRectangular)
    {
      if ((shutterLeftVerticalEdge.getLength() == 0)||(shutterLeftVerticalEdge.getVM() != 1))
      {
        result=EC_IllegalCall;
        DCMPSTAT_WARN("rectangular shutter used but shutterLeftVerticalEdge absent or VM != 1 in presentation state");
      }
      if ((shutterRightVerticalEdge.getLength() == 0)||(shutterRightVerticalEdge.getVM() != 1))
      {
        result=EC_IllegalCall;
        DCMPSTAT_WARN("rectangular shutter used but shutterRightVerticalEdge absent or VM != 1 in presentation state");
      }
      if ((shutterUpperHorizontalEdge.getLength() == 0)||(shutterUpperHorizontalEdge.getVM() != 1))
      {
        result=EC_IllegalCall;
        DCMPSTAT_WARN("rectangular shutter used but shutterUpperHorizontalEdge absent or VM != 1 in presentation state");
      }
      if ((shutterLowerHorizontalEdge.getLength() == 0)||(shutterLowerHorizontalEdge.getVM() != 1))
      {
        result=EC_IllegalCall;
        DCMPSTAT_WARN("rectangular shutter used but shutterLowerHorizontalEdge absent or VM != 1 in presentation state");
      }
    }
    if (useShutterCircular)
    {
      if ((centerOfCircularShutter.getLength() == 0)||(centerOfCircularShutter.getVM() != 2))
      {
        result=EC_IllegalCall;
        DCMPSTAT_WARN("circular shutter used but centerOfCircularShutter absent or VM != 2 in presentation state");
      }
      if ((radiusOfCircularShutter.getLength() == 0)||(radiusOfCircularShutter.getVM() != 1))
      {
        result=EC_IllegalCall;
        DCMPSTAT_WARN("circular shutter used but radiusOfCircularShutter absent or VM != 1 in presentation state");
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
        DCMPSTAT_WARN("polygonal shutter used but verticesOfThePolygonalShutter absent or VM != 2-2n in presentation state");
      }
    }
    if (useShutterBitmap)
    {
      if ((shutterOverlayGroup.getLength() == 0)||(shutterOverlayGroup.getVM() != 1))
      {
        result=EC_IllegalCall;
        DCMPSTAT_WARN("bitmap shutter used but shutterOverlayGroup absent or VM != 1 in presentation state");
      }
    }
    if (useShutterRectangular || useShutterCircular || useShutterPolygonal || useShutterBitmap)
    {
      if ((shutterPresentationValue.getLength() == 0)||(shutterPresentationValue.getVM() != 1))
      {
        result=EC_IllegalCall;
        DCMPSTAT_WARN("shutter used but shutterPresentationValue absent or VM != 1 in presentation state");
      }
    }
    /* we can either have rect/circ/poly shutter or bitmap shutter but not both */
    if (useShutterBitmap && (useShutterRectangular || useShutterCircular || useShutterPolygonal))
    {
      result=EC_IllegalCall;
      DCMPSTAT_WARN("both bitmap and rect/circ/poly shutter specified in presentation state");
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
        DCMPSTAT_WARN("bitmap shutter specified but referenced overlay group missing in presentation state");
      }
    } else {
        result=EC_IllegalCall;
        DCMPSTAT_WARN("wrong overlay group specified for bitmap shutter in presentation state");
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
      DCMPSTAT_WARN("mask module present in presentation state, but unsupported");
  }
  stack.clear();
  if (EC_Normal == dset.search(DCM_RecommendedViewingMode, stack, ESM_fromHere, OFFalse))
  {
      result=EC_IllegalCall;
      DCMPSTAT_WARN("mask module present in presentation state, but unsupported");
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

  READ_FROM_DATASET(DcmUnsignedShort, EVR_US, rows)
  READ_FROM_DATASET(DcmUnsignedShort, EVR_US, columns)
  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, sopclassUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, imageUID)
  READ_FROM_DATASET(DcmDecimalString, EVR_DS, imagerPixelSpacing)
  READ_FROM_DATASET(DcmDecimalString, EVR_DS, pixelSpacing)
  READ_FROM_DATASET(DcmIntegerString, EVR_IS, pixelAspectRatio)

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
  READ_FROM_DATASET(DcmCodeString, EVR_CS, photometricInterpretation)
  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, sopclassUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, imageUID)
  sopclassUID.getOFString(ofsopclassUID,0);
  imageUID.getOFString(ofimageUID,0);

  /* copy attributes for Patient, General Study and General Equipment
   * modules from image object. Also copy specific character set (SOP common). */
  READ_FROM_DATASET(DcmPersonName, EVR_PN, patientName)
  READ_FROM_DATASET(DcmLongString, EVR_LO, patientID)
  READ_FROM_DATASET(DcmDate, EVR_DA, patientBirthDate)
  READ_FROM_DATASET(DcmCodeString, EVR_CS, patientSex)
  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, studyInstanceUID)
  READ_FROM_DATASET(DcmDate, EVR_DA, studyDate)
  READ_FROM_DATASET(DcmTime, EVR_TM, studyTime)
  READ_FROM_DATASET(DcmPersonName, EVR_PN, referringPhysicianName)
  READ_FROM_DATASET(DcmShortString, EVR_SH, studyID)
  READ_FROM_DATASET(DcmShortString, EVR_SH, accessionNumber)
  READ_FROM_DATASET(DcmLongString, EVR_LO, manufacturer)
  READ_FROM_DATASET(DcmCodeString, EVR_CS, specificCharacterSet)

  /* if there is a modality rescale, copy it now.
   * Later we decide whether we want to use it.
   */
  READ_FROM_DATASET(DcmDecimalString, EVR_DS, rescaleIntercept)
  READ_FROM_DATASET(DcmDecimalString, EVR_DS, rescaleSlope)
  READ_FROM_DATASET(DcmLongString, EVR_LO, rescaleType)

  if ((result==EC_Normal) && shutterActivation)
  {
    /* if there is a display shutter in the image (i.e. in XA, RF, DX), copy it.
     * We never copy bitmap shutters.
     */
    READ_FROM_DATASET(DcmCodeString, EVR_CS, shutterShape)
    READ_FROM_DATASET(DcmIntegerString, EVR_IS, shutterLeftVerticalEdge)
    READ_FROM_DATASET(DcmIntegerString, EVR_IS, shutterRightVerticalEdge)
    READ_FROM_DATASET(DcmIntegerString, EVR_IS, shutterUpperHorizontalEdge)
    READ_FROM_DATASET(DcmIntegerString, EVR_IS, shutterLowerHorizontalEdge)
    READ_FROM_DATASET(DcmIntegerString, EVR_IS, centerOfCircularShutter)
    READ_FROM_DATASET(DcmIntegerString, EVR_IS, radiusOfCircularShutter)
    READ_FROM_DATASET(DcmIntegerString, EVR_IS, verticesOfThePolygonalShutter)
    READ_FROM_DATASET(DcmUnsignedShort, EVR_US, shutterPresentationValue)
  }

  if ((result==EC_Normal) && presentationActivation)
  {
    /* if there is a Presentation LUT shape in the image (i.e. in DX), copy it.
     * We never copy presentation LUT sequences.
     */
    READ_FROM_DATASET(DcmCodeString, EVR_CS, presentationLUTShape)
  }

  /* read Modality LUT Sequence */
  if (result==EC_Normal)
  {
    stack.clear();
    if ((EC_Normal == dset.search(DCM_ModalityLUTSequence, stack, ESM_fromHere, OFFalse)) && (stack.top()->ident() == EVR_SQ))
    {
      seq=(DcmSequenceOfItems *)stack.top();
      if (seq->card() >0)
      {
         item = seq->getItem(0);
         stack.clear();
         // LUTDescriptor can be US or SS
         if ((EC_Normal == item->search((DcmTagKey &)modalityLUTDescriptor.getTag(),
           stack, ESM_fromHere, OFFalse)) && (stack.top()->ident() == EVR_US || stack.top()->ident() == EVR_SS))
         {
           // We explicitly use DcmElement::operator=(), which works for US and SS
           DcmElement *mLUTDescriptor = &modalityLUTDescriptor;
           mLUTDescriptor->operator=(* OFstatic_cast(DcmElement *, stack.top()));
         }
         stack.clear();
         if ((EC_Normal == item->search((DcmTagKey &)modalityLUTExplanation.getTag(),
           stack, ESM_fromHere, OFFalse)) && (stack.top()->ident() == EVR_LO))
         {
           modalityLUTExplanation = *((DcmLongString *)(stack.top()));
         }
         stack.clear();

         // LUTData can be OW, US or SS. For now we only handle US.
         if ((EC_Normal == item->search((DcmTagKey &)modalityLUTData.getTag(),
           stack, ESM_fromHere, OFFalse)) && (stack.top()->ident() == EVR_US || stack.top()->ident() == EVR_OW))
         {
           // we deliberately call DcmElement::operator=() here, which will work for both DcmUnsignedShort and DcmOtherByteOtherWord parameters
           DcmElement *mdata = &modalityLUTData;
           mdata->operator=(*(DcmElement *)(stack.top()));
         }
         stack.clear();
         if ((EC_Normal == item->search((DcmTagKey &)modalityLUTType.getTag(),
           stack, ESM_fromHere, OFFalse)) && (stack.top()->ident() == EVR_LO))
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
      DCMPSTAT_WARN("Wrong image photometric interpretation - not MONOCHROME1/2");
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

  /* If no other character set is specified by the image, we use ISO_IR 100 as the default */
  if ((result==EC_Normal) && (specificCharacterSet.getLength() == 0)) result = specificCharacterSet.putString(DEFAULT_specificCharacterSet);

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
        DCMPSTAT_WARN("LIN OD found in presentation state; set to IDENTITY");
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
          // we clone modalityLUTDescriptor in order to retain the VR (US or SS)
          delem = OFstatic_cast(DcmElement *, modalityLUTDescriptor.clone());
          if (delem) ditem->insert(delem, OFTrue /*replaceOld*/); else result=EC_MemoryExhausted;

          // we write LUTData as OW in order to avoid the 64 kByte limit for US
          delem = new DcmOtherByteOtherWord(DCM_LUTData);
          delem->operator=(modalityLUTData);
          OFstatic_cast(DcmOtherByteOtherWord *, delem)->setVR(EVR_OW);
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
    DCMPSTAT_WARN("cannot add reference to image with different Study Instance UID.");
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

  READ_FROM_DATASET(DcmIntegerString, EVR_IS, numberOfFrames)
  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, studyUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, seriesUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, sopclassUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, imageUID)

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
        if (!aString.empty()) sprintf(buf, "\\%ld", (long)(i+1)); else sprintf(buf, "%ld", (long)(i+1));
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

  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, studyUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, seriesUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, imageUID)

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
  OFCondition result = verticesOfThePolygonalShutter.getSint32(y,OFstatic_cast(Uint32, 2*idx));
  if (EC_Normal==result) result = verticesOfThePolygonalShutter.getSint32(x,OFstatic_cast(Uint32, 2*idx+1));
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
    case DVPSC_latin9:
      cname = "ISO_IR 203";
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
  else if (aString == "ISO_IR 203") return DVPSC_latin9;
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
