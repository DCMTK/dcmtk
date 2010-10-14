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
 *    classes: DVPSStoredPrint
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:33 $
 *  CVS/RCS Revision: $Revision: 1.59 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"      /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpssp.h"
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmpstat/dvpsib.h"
#include "dcmtk/dcmpstat/dvpshlp.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmimgle/digsdfn.h"
#include "dcmtk/dcmpstat/dvpscf.h"

#include "dcmtk/dcmpstat/dvpsov.h"      /* for DVPSOverlay, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsgl.h"      /* for DVPSGraphicLayer, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsrs.h"      /* for DVPSReferencedSeries, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsal.h"      /* for DVPSOverlayCurveActivationLayer, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsga.h"      /* for DVPSGraphicAnnotation, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpscu.h"      /* for DVPSCurve, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsvl.h"      /* for DVPSVOILUT, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsvw.h"      /* for DVPSVOIWindow, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsda.h"      /* for DVPSDisplayedArea, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpssv.h"      /* for DVPSSoftcopyVOI, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsab.h"      /* for DVPSAnnotationContent, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpstx.h"      /* for DVPSTextObject, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsgr.h"      /* for DVPSGraphicObject, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsri.h"      /* for DVPSReferencedImage, needed by MSVC5 with STL */

#define INCLUDE_CMATH
#define INCLUDE_CTIME
#include "dcmtk/ofstd/ofstdinc.h"

/* --------------- class DVPSStoredPrint --------------- */

DVPSStoredPrint::DVPSStoredPrint(Uint16 illumin, Uint16 reflection, const char *aetitle)
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
, originator(DCM_Originator)
, destination(DCM_RETIRED_DestinationAE)
, printerName(DCM_PrinterName)
, instanceNumber(DCM_InstanceNumber)
, imageDisplayFormat(DCM_ImageDisplayFormat)
, annotationDisplayFormatID(DCM_AnnotationDisplayFormatID)
, filmOrientation(DCM_FilmOrientation)
, filmSizeID(DCM_FilmSizeID)
, magnificationType(DCM_MagnificationType)
, smoothingType(DCM_SmoothingType)
, borderDensity(DCM_BorderDensity)
, emptyImageDensity(DCM_EmptyImageDensity)
, minDensity(DCM_MinDensity)
, maxDensity(DCM_MaxDensity)
, trim(DCM_Trim)
, configurationInformation(DCM_ConfigurationInformation)
, illumination(DCM_Illumination)
, reflectedAmbientLight(DCM_ReflectedAmbientLight)
, requestedResolutionID(DCM_RequestedResolutionID)
, referencedPresentationLUTInstanceUID(DCM_ReferencedSOPInstanceUID)
, referencedPresentationLUTAlignment(DVPSK_other)
, imageBoxContentList()
, annotationContentList()
, presentationLUTList()
, sOPInstanceUID(DCM_SOPInstanceUID)
, specificCharacterSet(DCM_SpecificCharacterSet)
, instanceCreationDate(DCM_InstanceCreationDate)
, instanceCreationTime(DCM_InstanceCreationTime)
, imageSeriesInstanceUID(DCM_SeriesInstanceUID)
, currentValuesValid(OFFalse)
, currentNumCols(0)
, currentNumRows(0)
, decimateCropBehaviour(DVPSI_default)
, filmSessionInstanceUID()
, filmBoxInstanceUID()
, presentationLUTInstanceUID()
, globalPresentationLUT()
, globalPresentationLUTValid(OFFalse)
, transmitImagesIn12Bit(OFTrue)
, renderPresentationLUTinSCP(OFFalse)
, tempDensity()
{
  illumination.putUint16(illumin, 0);
  reflectedAmbientLight.putUint16(reflection, 0);
  if (aetitle != NULL)
    originator.putString(aetitle);
}

DVPSStoredPrint::DVPSStoredPrint(const DVPSStoredPrint& copy)
: patientName(copy.patientName)
, patientID(copy.patientID)
, patientBirthDate(copy.patientBirthDate)
, patientSex(copy.patientSex)
, studyInstanceUID(copy.studyInstanceUID)
, studyDate(copy.studyDate)
, studyTime(copy.studyTime)
, referringPhysicianName(copy.referringPhysicianName)
, studyID(copy.studyID)
, accessionNumber(copy.accessionNumber)
, seriesInstanceUID(copy.seriesInstanceUID)
, seriesNumber(copy.seriesNumber)
, manufacturer(copy.manufacturer)
, originator(copy.originator)
, destination(copy.destination)
, printerName(copy.printerName)
, instanceNumber(copy.instanceNumber)
, imageDisplayFormat(copy.imageDisplayFormat)
, annotationDisplayFormatID(copy.annotationDisplayFormatID)
, filmOrientation(copy.filmOrientation)
, filmSizeID(copy.filmSizeID)
, magnificationType(copy.magnificationType)
, smoothingType(copy.smoothingType)
, borderDensity(copy.borderDensity)
, emptyImageDensity(copy.emptyImageDensity)
, minDensity(copy.minDensity)
, maxDensity(copy.maxDensity)
, trim(copy.trim)
, configurationInformation(copy.configurationInformation)
, illumination(copy.illumination)
, reflectedAmbientLight(copy.reflectedAmbientLight)
, requestedResolutionID(copy.requestedResolutionID)
, referencedPresentationLUTInstanceUID(copy.referencedPresentationLUTInstanceUID)
, referencedPresentationLUTAlignment(copy.referencedPresentationLUTAlignment)
, imageBoxContentList(copy.imageBoxContentList)
, annotationContentList(copy.annotationContentList)
, presentationLUTList(copy.presentationLUTList)
, sOPInstanceUID(copy.sOPInstanceUID)
, specificCharacterSet(copy.specificCharacterSet)
, instanceCreationDate(copy.instanceCreationDate)
, instanceCreationTime(copy.instanceCreationTime)
, imageSeriesInstanceUID(copy.imageSeriesInstanceUID)
, currentValuesValid(copy.currentValuesValid)
, currentNumCols(copy.currentNumCols)
, currentNumRows(copy.currentNumRows)
, decimateCropBehaviour(copy.decimateCropBehaviour)
, filmSessionInstanceUID(copy.filmSessionInstanceUID)
, filmBoxInstanceUID(copy.filmBoxInstanceUID)
, presentationLUTInstanceUID(copy.presentationLUTInstanceUID)
, globalPresentationLUT(copy.globalPresentationLUT)
, globalPresentationLUTValid(copy.globalPresentationLUTValid)
, transmitImagesIn12Bit(copy.transmitImagesIn12Bit)
, renderPresentationLUTinSCP(copy.renderPresentationLUTinSCP)
, tempDensity(copy.tempDensity)
{
}

DVPSStoredPrint::~DVPSStoredPrint()
{
}

void DVPSStoredPrint::clear()
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
  originator.clear();
  destination.clear();
  printerName.clear();
  instanceNumber.clear();
  imageDisplayFormat.clear();
  annotationDisplayFormatID.clear();
  filmOrientation.clear();
  filmSizeID.clear();
  magnificationType.clear();
  smoothingType.clear();
  borderDensity.clear();
  emptyImageDensity.clear();
  minDensity.clear();
  maxDensity.clear();
  trim.clear();
  configurationInformation.clear();
  illumination.clear();
  reflectedAmbientLight.clear();
  requestedResolutionID.clear();
  referencedPresentationLUTInstanceUID.clear();
  referencedPresentationLUTAlignment = DVPSK_other;
  imageBoxContentList.clear();
  annotationContentList.clear();
  presentationLUTList.clear();
  sOPInstanceUID.clear();
  specificCharacterSet.clear();
  instanceCreationDate.clear();
  instanceCreationTime.clear();
  imageSeriesInstanceUID.clear();
  invalidateCache();
  decimateCropBehaviour = DVPSI_default;
  filmSessionInstanceUID.clear();
  filmBoxInstanceUID.clear();
  presentationLUTInstanceUID.clear();
  globalPresentationLUT.clear();
  globalPresentationLUTValid = OFFalse;
  transmitImagesIn12Bit = OFTrue;
  renderPresentationLUTinSCP = OFFalse;
  tempDensity.clear();
}

void DVPSStoredPrint::invalidateCache()
{
  currentValuesValid = OFFalse;
  currentNumCols=0;
  currentNumRows=0;
}

void DVPSStoredPrint::updateCache()
{
  if (currentValuesValid) return;
  OFString aString;
  imageDisplayFormat.getOFStringArray(aString,OFTrue);
  if (aString.substr(0,9) == "STANDARD\\")
  {
    unsigned long columns=0;
    unsigned long rows=0;
    const char *format = aString.c_str() + 9;

    if (2==sscanf(format, "%lu,%lu", &columns, &rows))
    {
      currentNumCols = columns;
      currentNumRows = rows;
      if ((columns > 0)&&(rows > 0)) currentValuesValid = OFTrue;
    } else {
      DCMPSTAT_INFO("cannot parse image display format '" << aString.c_str() << "'");
    }
  } else {
    DCMPSTAT_INFO("unknown image display format '" << aString.c_str() << "'");
  }
  return;
}

OFBool DVPSStoredPrint::isImageStorageSOPClass(OFString& sopclassuid)
{
  for (int i=0; i<numberOfDcmImageSOPClassUIDs; i++)
  {
    if (dcmImageSOPClassUIDs[i] && (sopclassuid == dcmImageSOPClassUIDs[i])) return OFTrue;
  }
  return OFFalse;
}


OFCondition DVPSStoredPrint::read(DcmItem &dset)
{
  DcmSequenceOfItems *seq;
  DcmItem *item;
  OFCondition result = EC_Normal;
  DcmStack stack;
  OFString aString;

  clear(); // re-initialize Stored Print object

  DcmUniqueIdentifier sopclassuid(DCM_SOPClassUID);
  DcmUniqueIdentifier refsopclassuid(DCM_ReferencedSOPClassUID);
  READ_FROM_DATASET(DcmUniqueIdentifier, sopclassuid)
  sopclassuid.getOFString(aString,0);
  if (aString != UID_RETIRED_StoredPrintStorage)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("SOP Class UID does not match StoredPrintStorage");
  }

  DcmCodeString modality(DCM_Modality);
  READ_FROM_DATASET(DcmCodeString, modality)
  if (modality.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("Modality missing or empty in Stored Print");
  }

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
  READ_FROM_DATASET(DcmIntegerString, instanceNumber)
  READ_FROM_DATASET(DcmUniqueIdentifier, sOPInstanceUID)
  READ_FROM_DATASET(DcmCodeString, specificCharacterSet)
  READ_FROM_DATASET(DcmDate, instanceCreationDate)
  READ_FROM_DATASET(DcmTime, instanceCreationTime)

  if (EC_Normal==result) result = presentationLUTList.read(dset);

  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_RETIRED_FilmBoxContentSequence, stack, ESM_fromHere, OFFalse))
    {
      seq=(DcmSequenceOfItems *)stack.top();
      if (seq->card() ==1)
      {
         item = seq->getItem(0);
         stack.clear();

         READ_FROM_DATASET2(DcmShortText, imageDisplayFormat)
         READ_FROM_DATASET2(DcmCodeString, annotationDisplayFormatID)
         if (result==EC_TagNotFound) result = EC_Normal;
         READ_FROM_DATASET2(DcmCodeString, filmOrientation)
         READ_FROM_DATASET2(DcmCodeString, filmSizeID)
         READ_FROM_DATASET2(DcmCodeString, magnificationType)
         READ_FROM_DATASET2(DcmCodeString, smoothingType)
         if (result==EC_TagNotFound) result = EC_Normal;
         READ_FROM_DATASET2(DcmCodeString, borderDensity)
         if (result==EC_TagNotFound) result = EC_Normal;
         READ_FROM_DATASET2(DcmCodeString, emptyImageDensity)
         if (result==EC_TagNotFound) result = EC_Normal;
         READ_FROM_DATASET2(DcmUnsignedShort, minDensity)
         if (result==EC_TagNotFound) result = EC_Normal;
         READ_FROM_DATASET2(DcmUnsignedShort, maxDensity)
         READ_FROM_DATASET2(DcmCodeString, trim)
         READ_FROM_DATASET2(DcmShortText, configurationInformation)
         READ_FROM_DATASET2(DcmUnsignedShort, illumination)
         if (result==EC_TagNotFound) result = EC_Normal;
         READ_FROM_DATASET2(DcmUnsignedShort, reflectedAmbientLight)
         if (result==EC_TagNotFound) result = EC_Normal;
         READ_FROM_DATASET2(DcmCodeString, requestedResolutionID)
         if (result==EC_TagNotFound) result = EC_Normal;
         // check referenced presentation LUT sequence
         // if there is any reference, it must refer to one of the presentation LUTs we are managing.
         stack.clear();
         if (EC_Normal == item->search(DCM_ReferencedPresentationLUTSequence, stack, ESM_fromHere, OFFalse))
         {
           seq=(DcmSequenceOfItems *)stack.top();
           if (seq->card() ==1)
           {
              item = seq->getItem(0);
              stack.clear();
              READ_FROM_DATASET2(DcmUniqueIdentifier, referencedPresentationLUTInstanceUID)
              if (referencedPresentationLUTInstanceUID.getLength() > 0)
              {
                referencedPresentationLUTInstanceUID.getOFString(aString,0);
                if (NULL == presentationLUTList.findPresentationLUT(aString.c_str()))
                {
                  result=EC_IllegalCall;
                  DCMPSTAT_INFO("FilmBoxContentSequence presentation LUT reference cannot be resolved");
                }
              }
           } else {
             result=EC_TagNotFound;
             DCMPSTAT_INFO("found FilmBoxContentSequence in Stored Print with ReferencedPresentationLUTSequence number of items != 1");
           }
         }
      } else {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("found FilmBoxContentSequence in Stored Print with number of items != 1");
      }
    }
  }

  if (EC_Normal==result) result = imageBoxContentList.read(dset, presentationLUTList);

  if (EC_Normal==result) result = annotationContentList.read(dset);

  /* Now perform basic sanity checks */

  if (result==EC_Normal)
  {
    if ((studyInstanceUID.getLength() == 0)||(studyInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("StudyInstanceUID missing or incorrect in Stored Print");
    }
    if ((seriesInstanceUID.getLength() == 0)||(seriesInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("SeriesInstanceUID missing or incorrect in Stored Print");
    }
    if ((sOPInstanceUID.getLength() == 0)||(sOPInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("SOPInstanceUID missing or incorrect in Stored Print");
    }
    if ((patientName.getLength() == 0)||(patientName.getVM() != 1))
    {
       // result=EC_TagNotFound;
      DCMPSTAT_INFO("PatientName missing or incorrect in Stored Print");
    }
    if ((imageDisplayFormat.getLength() == 0)||(imageDisplayFormat.getVM() != 1))
    {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("ImageDisplayFormat missing or incorrect in Stored Print");
    }
    if (imageBoxContentList.size() == 0)
    {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("ImageBoxContentSequence missing or empty in Stored Print");
    }
  }

  /* Finally check the Print Management Capabilities Sequence
   * we require the following SOP classes to be referenced here:
   *   - Basic Film Box
   *   - Basic Grayscale Image Box
   *   - at least one Image Storage SOP Class
   * we also allow the following SOP classes to be referenced:
   *   - Printer (is part of Basic Grayscale Meta SOP Class)
   *   - Basic Film Session (ditto)
   *   - Presentation LUT
   *   - Basic Annotation Box
   */
  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_RETIRED_PrintManagementCapabilitiesSequence, stack, ESM_fromHere, OFFalse))
    {
      OFBool haveFilmBox = OFFalse;
      OFBool haveGrayscaleImageBox = OFFalse;
      OFBool haveImageStorage = OFFalse;

      seq=(DcmSequenceOfItems *)stack.top();
      unsigned long numItems = seq->card();
      for (unsigned long i=0; i<numItems; i++)
      {
         item = seq->getItem(i);
         stack.clear();
         refsopclassuid.clear();
         READ_FROM_DATASET2(DcmUniqueIdentifier, refsopclassuid)
         refsopclassuid.getOFString(aString,0);
         if (aString == UID_BasicFilmBoxSOPClass)
           haveFilmBox=OFTrue;
         else
           if (aString==UID_BasicGrayscaleImageBoxSOPClass)
             haveGrayscaleImageBox = OFTrue;
           else
             if ((aString == UID_PrinterSOPClass)
               ||(aString == UID_BasicFilmSessionSOPClass)
               ||(aString == UID_PresentationLUTSOPClass)
               ||(aString == UID_BasicAnnotationBoxSOPClass)) { }
             else
               if (isImageStorageSOPClass(aString))
                 haveImageStorage=OFTrue;
               else
                 {
                   result=EC_IllegalCall;
                   DCMPSTAT_INFO("Unsupported SOP Class UID in PrintManagementCapabilitiesSequence");
                 }

      }
      if (EC_Normal==result)
      {
        if (! haveFilmBox)
        {
           result=EC_IllegalCall;
           DCMPSTAT_INFO("Film Box SOP Class not referenced in PrintManagementCapabilitiesSequence");
        }
        if (! haveGrayscaleImageBox)
        {
           result=EC_IllegalCall;
           DCMPSTAT_INFO("Basic Grayscale Image Box SOP Class not referenced in PrintManagementCapabilitiesSequence");
        }
        if (! haveImageStorage)
        {
           result=EC_IllegalCall;
           DCMPSTAT_INFO("No Image Storage SOP Class referenced in PrintManagementCapabilitiesSequence");
        }
      }
    } else {
      result=EC_TagNotFound;
      DCMPSTAT_INFO("PrintManagementCapabilitiesSequence not found");
    }
  }

  /* read PrinterName from PrinterCharacteristicsSequence if available */
  if (result == EC_Normal)
  {
    originator.clear();
    destination.clear();
    printerName.clear();
    stack.clear();
    if (EC_Normal == dset.search(DCM_RETIRED_PrinterCharacteristicsSequence, stack, ESM_fromHere, OFFalse))
    {
      seq = (DcmSequenceOfItems *)stack.top();
      if (seq->card() > 0)
      {
         item = seq->getItem(0);
         stack.clear();
         READ_FROM_DATASET2(DcmApplicationEntity, originator)
         READ_FROM_DATASET2(DcmApplicationEntity, destination)
         READ_FROM_DATASET2(DcmLongString, printerName)
      }
    }
    if (printerName.getLength() == 0)
      DCMPSTAT_INFO("PrinterName missing or incorrect in PrinterCharacteristicsSequence");
  }

  return result;
}


OFCondition DVPSStoredPrint::createDefaultValues()
{
  OFCondition result = EC_Normal;
  char uid[100];
  OFString aString;

  SET_UID(seriesInstanceUID)
  SET_UID(imageSeriesInstanceUID)

  if ((result==EC_Normal)&&(patientName.getLength()==0))
  {
        result = patientName.putString(DEFAULT_patientName);
  }

  if ((result==EC_Normal)&&(sOPInstanceUID.getLength()==0))
  {
    result = sOPInstanceUID.putString(dcmGenerateUniqueIdentifier(uid));
    DVPSHelper::currentDate(aString);
    if (result==EC_Normal) result = instanceCreationDate.putString(aString.c_str());
    DVPSHelper::currentTime(aString);
    if (result==EC_Normal) result = instanceCreationTime.putString(aString.c_str());
  }

  if ((result==EC_Normal)&&(studyInstanceUID.getLength()==0))
  {
    result = studyInstanceUID.putString(dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT));
    DVPSHelper::currentDate(aString);
    if (result==EC_Normal) result = studyDate.putString(aString.c_str());
    DVPSHelper::currentTime(aString);
    if (result==EC_Normal) result = studyTime.putString(aString.c_str());
  }

  if ((result==EC_Normal)&&(imageDisplayFormat.getLength()==0)) result = imageDisplayFormat.putString(DEFAULT_imageDisplayFormat);
  return result;
}

OFCondition DVPSStoredPrint::write(
  DcmItem &dset,
  OFBool writeRequestedImageSize,
  OFBool limitImages,
  OFBool updateDecimateCrop,
  OFBool ignoreEmptyImages)
{
  DcmElement *delem=NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  OFCondition result = createDefaultValues();
  if ((EC_Normal==result)&& updateDecimateCrop) result = imageBoxContentList.setRequestedDecimateCropBehaviour(decimateCropBehaviour); // set in all image boxes
  if (EC_Normal==result) result = imageBoxContentList.createDefaultValues(limitImages, ignoreEmptyImages); // renumber if limitImages is true

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
  ADD_TO_DATASET(DcmUniqueIdentifier, seriesInstanceUID)
  ADD_TO_DATASET(DcmIntegerString, seriesNumber)
  ADD_TO_DATASET(DcmLongString, manufacturer)
  ADD_TO_DATASET(DcmIntegerString, instanceNumber)
  ADD_TO_DATASET(DcmUniqueIdentifier, sOPInstanceUID)

  if (specificCharacterSet.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, specificCharacterSet) }
  if (instanceCreationDate.getLength() > 0) { ADD_TO_DATASET(DcmDate, instanceCreationDate) }
  if (instanceCreationTime.getLength() > 0) { ADD_TO_DATASET(DcmTime, instanceCreationTime) }

  /* create the Film Box Content SQ */
  if (result == EC_Normal)
  {
    ditem = new DcmItem();
    if (ditem)
    {
      dseq = new DcmSequenceOfItems(DCM_RETIRED_FilmBoxContentSequence);
      if (dseq)
      {
        ADD_TO_DATASET2(DcmShortText, imageDisplayFormat)
        ADD_TO_DATASET2(DcmCodeString, filmOrientation)
        ADD_TO_DATASET2(DcmCodeString, filmSizeID)
        ADD_TO_DATASET2(DcmCodeString, magnificationType)
        ADD_TO_DATASET2(DcmUnsignedShort, maxDensity)
        ADD_TO_DATASET2(DcmShortText, configurationInformation)
        if (annotationDisplayFormatID.getLength() > 0) { ADD_TO_DATASET2(DcmCodeString, annotationDisplayFormatID) }
        if (smoothingType.getLength() > 0) { ADD_TO_DATASET2(DcmCodeString, smoothingType) }
        if (borderDensity.getLength() > 0) { ADD_TO_DATASET2(DcmCodeString, borderDensity) }
        if (emptyImageDensity.getLength() > 0) { ADD_TO_DATASET2(DcmCodeString, emptyImageDensity) }
        if (minDensity.getLength() > 0) { ADD_TO_DATASET2(DcmUnsignedShort, minDensity) }
        if (trim.getLength() > 0) { ADD_TO_DATASET2(DcmCodeString, trim) }
        if (requestedResolutionID.getLength() > 0) { ADD_TO_DATASET2(DcmCodeString, requestedResolutionID) }
        if ((presentationLUTList.size() > 0) || globalPresentationLUTValid)
        {
          ADD_TO_DATASET2(DcmUnsignedShort, illumination)
          ADD_TO_DATASET2(DcmUnsignedShort, reflectedAmbientLight)
          if ((result == EC_Normal) && globalPresentationLUTValid)
          {
              // generate a new UID for the "global" presentation LUT
              char uid[100];
              dcmGenerateUniqueIdentifier(uid);
              globalPresentationLUT.setSOPInstanceUID(uid);
              result = referencedPresentationLUTInstanceUID.putString(uid);
          }
          if (EC_Normal == result) result = addReferencedPLUTSQ(*ditem);
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

  /* add SOP Class UID */
  DcmUniqueIdentifier sopclassuid(DCM_SOPClassUID);
  if (result==EC_Normal)
  {
     result = sopclassuid.putString(UID_RETIRED_StoredPrintStorage);
  }
  ADD_TO_DATASET(DcmUniqueIdentifier, sopclassuid)

  /* add Modality */

  DcmCodeString modality(DCM_Modality);
  if (result==EC_Normal)
  {
     result = modality.putString("STORED_PRINT");  // defined term
  }
  ADD_TO_DATASET(DcmCodeString, modality)

  // compute number of image boxes to write
  updateCache();
  unsigned long writeImageBoxes=0; // default: write all
  if (limitImages && currentValuesValid) writeImageBoxes = currentNumCols * currentNumRows;

  // write PresentationLUTContentSequence
  if (EC_Normal == result)
  {
    // write general presentation LUT only
    if (globalPresentationLUTValid)
    {
        dseq = new DcmSequenceOfItems(DCM_RETIRED_PresentationLUTContentSequence);
        if (dseq)
        {
            ditem = new DcmItem();
            if (ditem)
            {
                result = globalPresentationLUT.write(*ditem, OFTrue);
                if (result == EC_Normal) dseq->insert(ditem); else delete ditem;
            } else result = EC_MemoryExhausted;
        } else result = EC_MemoryExhausted;
        if (result == EC_Normal) dset.insert(dseq, OFTrue /*replaceOld*/); else delete dseq;
    } else {
        // write presentation LUT list
        result = presentationLUTList.write(dset);
    }
  }

  // write imageBoxContentList
  if (EC_Normal == result)
    result = imageBoxContentList.write(dset, writeRequestedImageSize, (size_t)writeImageBoxes, ignoreEmptyImages, !globalPresentationLUTValid);

  // write annotationContentList
  if (EC_Normal == result) result = annotationContentList.write(dset);

  // write PrintManagementCapabilitiesSequence
  dseq = new DcmSequenceOfItems(DCM_RETIRED_PrintManagementCapabilitiesSequence);
  if (dseq)
  {
    if (EC_Normal == result) result = DVPSHelper::addReferencedUIDItem(*dseq, UID_BasicFilmSessionSOPClass);
    if (EC_Normal == result) result = DVPSHelper::addReferencedUIDItem(*dseq, UID_BasicFilmBoxSOPClass);
    if (EC_Normal == result) result = DVPSHelper::addReferencedUIDItem(*dseq, UID_BasicGrayscaleImageBoxSOPClass);
    if (EC_Normal == result) result = imageBoxContentList.addImageSOPClasses(*dseq, (size_t)writeImageBoxes);
    if ((result == EC_Normal)&&(presentationLUTList.size() > 0))
    {
      result = DVPSHelper::addReferencedUIDItem(*dseq, UID_PresentationLUTSOPClass);
    }
    if ((result == EC_Normal)&&(annotationContentList.size() > 0))
    {
      result = DVPSHelper::addReferencedUIDItem(*dseq, UID_BasicAnnotationBoxSOPClass);
    }

    if (result==EC_Normal) dset.insert(dseq, OFTrue /*replaceOld*/); else delete dseq;
  } else result = EC_MemoryExhausted;

  // write PrinterCharacteristicsSequence (Type 2)
  if (EC_Normal == result)
  {
    dseq = new DcmSequenceOfItems(DCM_RETIRED_PrinterCharacteristicsSequence);
    if (dseq)
    {
     if (printerName.getLength() > 0)
     {
       ditem = new DcmItem();
       if (ditem)
       {
         ADD_TO_DATASET2(DcmApplicationEntity, originator);
         ADD_TO_DATASET2(DcmApplicationEntity, destination);
         ADD_TO_DATASET2(DcmLongString, printerName);
         if (result == EC_Normal) result = dseq->insert(ditem); else delete ditem;
       } else result = EC_MemoryExhausted;
     }
     if (result == EC_Normal) dset.insert(dseq, OFTrue /*replaceOld*/); else delete dseq;
    } else result = EC_MemoryExhausted;
  }

  return result;
}

OFCondition DVPSStoredPrint::writeHardcopyImageAttributes(DcmItem &dset)
{
  DcmElement *delem=NULL;
  OFCondition result = createDefaultValues();

  // add general study module
  ADD_TO_DATASET(DcmUniqueIdentifier, studyInstanceUID)
  ADD_TO_DATASET(DcmDate, studyDate)
  ADD_TO_DATASET(DcmTime, studyTime)
  ADD_TO_DATASET(DcmPersonName, referringPhysicianName)
  ADD_TO_DATASET(DcmShortString, studyID)
  ADD_TO_DATASET(DcmShortString, accessionNumber)

  // add general series module for hardcopy images
  DcmIntegerString imageSeriesNumber(DCM_SeriesNumber); // always empty
  DcmCodeString modality(DCM_Modality);
  if (result==EC_Normal)
  {
     result = modality.putString("HC");
  }
  ADD_TO_DATASET(DcmUniqueIdentifier, imageSeriesInstanceUID)
  ADD_TO_DATASET(DcmIntegerString, imageSeriesNumber)
  ADD_TO_DATASET(DcmCodeString, modality)

  return result;
}

DVPSPresentationLUT *DVPSStoredPrint::getImagePresentationLUT(size_t idx)
{
    /* look for referenced Presentation LUT in image box */
    const char *plutuid = imageBoxContentList.getReferencedPresentationLUTInstanceUID(idx);
    /* if absent, look for referenced Presentation LUT in film box */
    if ((plutuid == NULL) || (strlen(plutuid) == 0))
    {
        char *uid = NULL;
        if (referencedPresentationLUTInstanceUID.getString(uid) == EC_Normal)
            plutuid = uid;
    }
    DVPSPresentationLUT *plut = NULL;
    if ((plutuid != NULL) && (strlen(plutuid) > 0))
        plut = presentationLUTList.findPresentationLUT(plutuid);
    return plut;
}

DVPSPresentationLUT *DVPSStoredPrint::getPresentationLUT()
{
    if (globalPresentationLUTValid)
        return &globalPresentationLUT;
    return NULL;
}

OFCondition DVPSStoredPrint::setDefaultPresentationLUT()
{
    globalPresentationLUTValid = OFFalse;
    globalPresentationLUT.clear();
    return EC_Normal;
}

OFCondition DVPSStoredPrint::setPresentationLUTShape(DVPSPresentationLUTType shape)
{
    OFCondition result = EC_IllegalCall;
    if ((shape == DVPSP_identity) || (shape == DVPSP_lin_od))
    {
        result = globalPresentationLUT.setType(shape);
        globalPresentationLUTValid = (result == EC_Normal);
    }
    return result;
}

OFCondition DVPSStoredPrint::setPresentationLookupTable(DcmItem &dset)
{
  OFCondition result = globalPresentationLUT.read(dset, OFFalse);
  globalPresentationLUTValid = (result == EC_Normal);
  return result;
}

Sint32 DVPSStoredPrint::convertODtoPValue(Uint16 density, unsigned int bits)
{
  const Uint16 min = getMinDensityValue();
  const Uint16 max = getMaxDensityValue();
  if ((min < max) && ((bits == 8) || (bits == 12) || (bits == 16)))
  {
    if (density >= max)
      return 0;
    else if (density <= min)
      return (Sint32)DicomImageClass::maxval(bits);
    else
    {
      const double l0 = (double)getPrintIllumination();
      const double la = (double)getPrintReflectedAmbientLight();
      const double d0 = (double)density / 100;
      const double dmin = (double)min / 100;
      const double dmax = (double)max / 100;
      const double lmin = la + l0 * pow((double)10, -dmax);
      const double lmax = la + l0 * pow((double)10, -dmin);
      const double jmin = DiGSDFunction::getJNDIndex(lmin);
      const double jmax = DiGSDFunction::getJNDIndex(lmax);
      const double factor = (double)DicomImageClass::maxval(bits) / (jmax - jmin);
      return (Sint32)((DiGSDFunction::getJNDIndex(la + l0 * pow((double)10, -d0)) - jmin) * factor);
    }
  }
  return -1;
}

OFCondition DVPSStoredPrint::addImageBox(
  const char *retrieveaetitle,
  const char *refstudyuid,
  const char *refseriesuid,
  const char *refsopclassuid,
  const char *refsopinstanceuid,
  const char *requestedimagesize,
  const char *patientid,
  DVPSPresentationLUT *presentationlut,
  OFBool inversePLUT)
{
  char instanceuid[100];
  const char *lutUID = presentationLUTList.addPresentationLUT(presentationlut, inversePLUT);
  return imageBoxContentList.addImageBox(dcmGenerateUniqueIdentifier(instanceuid),
     retrieveaetitle, refstudyuid, refseriesuid, refsopclassuid,
     refsopinstanceuid, requestedimagesize, patientid, lutUID);
}

OFCondition DVPSStoredPrint::addImageBox(
  const char *retrieveaetitle,
  const char *refsopinstanceuid,
  const char *requestedimagesize,
  const char *patientid,
  DVPSPresentationLUT *presentationlut,
  OFBool inversePLUT)
{
  char *refstudyuid=NULL;
  char *refseriesuid=NULL;

  createDefaultValues(); // make sure that all UIDs are defined
  studyInstanceUID.getString(refstudyuid); // same study UID for stored print and hardcopy image
  imageSeriesInstanceUID.getString(refseriesuid); // but separate series for the hardcopy images

  return addImageBox(retrieveaetitle, refstudyuid, refseriesuid, UID_RETIRED_HardcopyGrayscaleImageStorage,
     refsopinstanceuid, requestedimagesize, patientid, presentationlut, inversePLUT);
}

OFCondition DVPSStoredPrint::setOriginator(const char *aetitle)
{
  if ((aetitle == NULL) || (strlen(aetitle) == 0))
    return originator.clear();
  else
    return originator.putString(aetitle);
}

OFCondition DVPSStoredPrint::setDestination(const char *aetitle)
{
  if ((aetitle == NULL) || (strlen(aetitle) == 0))
    return destination.clear();
  else
    return destination.putString(aetitle);
}

OFCondition DVPSStoredPrint::setPrinterName(const char *name)
{
  if ((name == NULL) || (strlen(name) == 0))
    return printerName.clear();
  else
    return printerName.putString(name);
}

OFCondition DVPSStoredPrint::setInstanceUID(const char *uid)
{
  if ((uid==NULL)||(strlen(uid)==0)) return EC_IllegalCall;
  return sOPInstanceUID.putString(uid);
}

OFCondition DVPSStoredPrint::setImageDisplayFormat(unsigned long columns, unsigned long rows)
{
  if ((columns==0)||(rows==0)) return EC_IllegalCall;
  char newFormat[80];
  sprintf(newFormat, "STANDARD\\%lu,%lu", columns, rows);

  OFCondition result = imageDisplayFormat.putString(newFormat);
  if (EC_Normal == result)
  {
    currentNumCols = columns;
    currentNumRows = rows;
    currentValuesValid = OFTrue;
  } else invalidateCache();
  return result;
}

OFCondition DVPSStoredPrint::setFilmSizeID(const char *value)
{
  if ((value==NULL)||(strlen(value)==0))
  {
    filmSizeID.clear();
    return EC_Normal;
  }
  return filmSizeID.putString(value);
}

OFCondition DVPSStoredPrint::setMagnificationType(const char *value)
{
  if ((value==NULL)||(strlen(value)==0))
  {
    magnificationType.clear();
    return EC_Normal;
  }
  return magnificationType.putString(value);
}

OFCondition DVPSStoredPrint::setSmoothingType(const char *value)
{
  if ((value==NULL)||(strlen(value)==0))
  {
    smoothingType.clear();
    return EC_Normal;
  }
  return smoothingType.putString(value);
}

OFCondition DVPSStoredPrint::setConfigurationInformation(const char *value)
{
  if ((value==NULL)||(strlen(value)==0))
  {
    configurationInformation.clear();
    return EC_Normal;
  }
  return configurationInformation.putString(value);
}

OFCondition DVPSStoredPrint::setResolutionID(const char *value)
{
  if ((value==NULL)||(strlen(value)==0))
  {
    requestedResolutionID.clear();
    return EC_Normal;
  }
  return requestedResolutionID.putString(value);
}

OFCondition DVPSStoredPrint::setFilmOrientation(DVPSFilmOrientation value)
{
  switch (value)
  {
    case DVPSF_portrait:
      return filmOrientation.putString("PORTRAIT");
      /* break; */
    case DVPSF_landscape:
      return filmOrientation.putString("LANDSCAPE");
      /* break; */
    case DVPSF_default:
      filmOrientation.clear();
      break;
  }
  return EC_Normal;
}

OFCondition DVPSStoredPrint::setTrim(DVPSTrimMode value)
{
  switch (value)
  {
    case DVPSH_trim_on:
      return trim.putString("YES");
      /* break; */
    case DVPSH_trim_off:
      return trim.putString("NO");
      /* break; */
    case DVPSH_default:
      trim.clear();
      break;
  }
  return EC_Normal;
}

OFCondition DVPSStoredPrint::setRequestedDecimateCropBehaviour(DVPSDecimateCropBehaviour value)
{
  decimateCropBehaviour = value;
  return EC_Normal;
}


OFCondition DVPSStoredPrint::newPrinter(const char *name, const char *destinationAE)
{
  filmSizeID.clear();
  magnificationType.clear();
  smoothingType.clear();
  configurationInformation.clear();
  requestedResolutionID.clear();
  trim.clear();
  borderDensity.clear();
  emptyImageDensity.clear();
  minDensity.clear();
  maxDensity.clear();

  if (name != NULL)
    setPrinterName(name);
  if (destinationAE != NULL)
    setDestination(destinationAE);

  OFCondition result = setRequestedDecimateCropBehaviour(DVPSI_default);
  if (EC_Normal == result) result = imageBoxContentList.setAllImagesToDefault();
  return result;
}

const char *DVPSStoredPrint::getOriginator()
{
  char *c = NULL;
  if (EC_Normal == originator.getString(c)) return c; else return NULL;
}

const char *DVPSStoredPrint::getDestination()
{
  char *c = NULL;
  if (EC_Normal == destination.getString(c)) return c; else return NULL;
}

const char *DVPSStoredPrint::getPrinterName()
{
  char *c = NULL;
  if (EC_Normal == printerName.getString(c)) return c; else return NULL;
}

unsigned long DVPSStoredPrint::getImageDisplayFormatColumns()
{
  updateCache();
  return currentNumCols;
}

unsigned long DVPSStoredPrint::getImageDisplayFormatRows()
{
  updateCache();
  return currentNumRows;
}

DVPSFilmOrientation DVPSStoredPrint::getFilmOrientation()
{
  DVPSFilmOrientation result = DVPSF_default;
  char *c = NULL;
  if ((EC_Normal == filmOrientation.getString(c))&& c)
  {
    OFString aString(c);
    if (aString == "PORTRAIT") result = DVPSF_portrait;
    else if (aString == "LANDSCAPE") result = DVPSF_landscape;
  }
  return result;
}

DVPSTrimMode DVPSStoredPrint::getTrim()
{
  DVPSTrimMode result = DVPSH_default;
  char *c = NULL;
  if ((EC_Normal == trim.getString(c))&& c)
  {
    OFString aString(c);
    if (aString == "YES") result = DVPSH_trim_on;
    else if (aString == "NO") result = DVPSH_trim_off;
  }
  return result;
}

const char *DVPSStoredPrint::getStudyInstanceUID()
{
  char *c = NULL;
  if (EC_Normal == studyInstanceUID.getString(c)) return c; else return NULL;
}

const char *DVPSStoredPrint::getSeriesInstanceUID()
{
  char *c = NULL;
  if (EC_Normal == seriesInstanceUID.getString(c)) return c; else return NULL;
}

const char *DVPSStoredPrint::getSOPInstanceUID()
{
  char *c = NULL;
  if (EC_Normal == sOPInstanceUID.getString(c)) return c; else return NULL;
}

const char *DVPSStoredPrint::getFilmSizeID()
{
  char *c = NULL;
  if (EC_Normal == filmSizeID.getString(c)) return c; else return NULL;
}

const char *DVPSStoredPrint::getMagnificationType()
{
  char *c = NULL;
  if (EC_Normal == magnificationType.getString(c)) return c; else return NULL;
}

const char *DVPSStoredPrint::getSmoothingType()
{
  char *c = NULL;
  if (EC_Normal == smoothingType.getString(c)) return c; else return NULL;
}

const char *DVPSStoredPrint::getConfigurationInformation()
{
  char *c = NULL;
  if (EC_Normal == configurationInformation.getString(c)) return c; else return NULL;
}

const char *DVPSStoredPrint::getResolutionID()
{
  char *c = NULL;
  if (EC_Normal == requestedResolutionID.getString(c)) return c; else return NULL;
}

OFCondition DVPSStoredPrint::setBorderDensity(const char *value)
{
  if ((value==NULL)||(strlen(value)==0))
  {
    borderDensity.clear();
    return EC_Normal;
  }
  return borderDensity.putString(value);
}

OFCondition DVPSStoredPrint::setEmtpyImageDensity(const char *value)
{
  if ((value==NULL)||(strlen(value)==0))
  {
    emptyImageDensity.clear();
    return EC_Normal;
  }
  return emptyImageDensity.putString(value);
}

const char *DVPSStoredPrint::getBorderDensity()
{
  char *c = NULL;
  if (EC_Normal == borderDensity.getString(c)) return c; else return NULL;
}

const char *DVPSStoredPrint::getEmtpyImageDensity()
{
  char *c = NULL;
  if (EC_Normal == emptyImageDensity.getString(c)) return c; else return NULL;
}

OFCondition DVPSStoredPrint::setPrintIllumination(Uint16 value)
{
  return illumination.putUint16(value, 0);
}

Uint16 DVPSStoredPrint::getPrintIllumination()
{
  Uint16 result = 0;
  if (EC_Normal == illumination.getUint16(result, 0)) return result; else return 0;
}

OFCondition DVPSStoredPrint::setPrintReflectedAmbientLight(Uint16 value)
{
  return reflectedAmbientLight.putUint16(value, 0);
}

Uint16 DVPSStoredPrint::getPrintReflectedAmbientLight()
{
  Uint16 result = 0;
  if (EC_Normal == reflectedAmbientLight.getUint16(result, 0)) return result; else return 0;
}

OFCondition DVPSStoredPrint::deleteImage(size_t idx)
{
  OFCondition result = imageBoxContentList.deleteImage(idx);
  char *c = NULL;
  if (EC_Normal != configurationInformation.getString(c)) c = NULL;
  presentationLUTList.cleanup(c, imageBoxContentList);
  return result;
}

OFCondition DVPSStoredPrint::deleteMultipleImages(size_t number)
{
  OFCondition result = imageBoxContentList.deleteMultipleImages(number);
  char *c = NULL;
  if (EC_Normal != configurationInformation.getString(c)) c = NULL;
  presentationLUTList.cleanup(c, imageBoxContentList);
  return result;
}

OFCondition DVPSStoredPrint::deleteSpooledImages()
{
  OFCondition result = EC_IllegalCall;
  char *c = NULL;
  unsigned long deleteImageBoxes=0;

  updateCache();
  if (currentValuesValid)
  {
        deleteImageBoxes = currentNumCols * currentNumRows;
    if (deleteImageBoxes > imageBoxContentList.size()) deleteImageBoxes = imageBoxContentList.size();
    result = imageBoxContentList.deleteMultipleImages((size_t)deleteImageBoxes);
  }
  if (EC_Normal != configurationInformation.getString(c)) c = NULL;
  presentationLUTList.cleanup(c, imageBoxContentList);
  return result;
}

OFCondition DVPSStoredPrint::printSCUgetPrinterInstance(DVPSPrintMessageHandler& printHandler)
{
  DcmDataset *attributeListOut=NULL;
  Uint16 status=0;
  OFCondition cond=printHandler.getRQ(UID_PrinterSOPClass, UID_PrinterSOPInstance, NULL, 0, status, attributeListOut);

  /* the N-GET response has been dumped somewhere else, we only need to delete it */
  delete attributeListOut;

  if (cond.bad()) return EC_IllegalCall;
  return EC_Normal;
}

OFCondition DVPSStoredPrint::printSCUpreparePresentationLUT(
  DVPSPrintMessageHandler& printHandler,
  OFBool printerRequiresMatchingLUT,
  OFBool printerLUTRenderingPreferred,
  OFBool printerSupports12Bit)
{
  /* first of all we determine whether we can let the print SCP render Presentation LUT for us. */
  renderPresentationLUTinSCP = OFFalse;   // set to true if we can create a presentation LUT for all images
  transmitImagesIn12Bit = OFTrue;         // set to false later if images should be transmitted in 8-bit depth

  OFBool printerSupportsPresentationLUT = printHandler.printerSupportsPresentationLUT();
  DVPSPresentationLUT *plut = NULL;
  if (printerSupportsPresentationLUT)
  {
    char *filmBox = NULL;
    if (EC_Normal != referencedPresentationLUTInstanceUID.getString(filmBox)) filmBox=NULL;
    const char *plutuid = imageBoxContentList.haveSinglePresentationLUTUsed(filmBox);
    if (plutuid) plut = presentationLUTList.findPresentationLUT(plutuid);
    if (plut && (plut->isLegalPrintPresentationLUT()))
    {
      /* there is a single Presentation LUT that can be used for the complete film,
       * and it is a valid Supplement 22 Presentation LUT.
       */
      if (printerSupports12Bit)
      {
        /* 12-bit printer, we use the LUT if the printer can handle it and if the user wants it */
        if (printerLUTRenderingPreferred)
        {
          if (printerRequiresMatchingLUT)
          {
            if (plut->matchesImageDepth(OFTrue)) renderPresentationLUTinSCP = OFTrue;
            else
            {
              if (plut->matchesImageDepth(OFFalse))
              {
                renderPresentationLUTinSCP = OFTrue;
                transmitImagesIn12Bit = OFFalse;
              }
            }
          } else renderPresentationLUTinSCP = OFTrue;
        }
      } else {
        /* 8-bit printer, we use the LUT if the printer can handle it */
        transmitImagesIn12Bit = OFFalse;
        if (printerRequiresMatchingLUT)
        {
          if (plut->matchesImageDepth(OFFalse)) renderPresentationLUTinSCP = OFTrue;
        } else renderPresentationLUTinSCP = OFTrue;
      }
    } else transmitImagesIn12Bit = printerSupports12Bit;
  } else {
        transmitImagesIn12Bit = printerSupports12Bit;
        DCMPSTAT_INFO("spooler: printer does not support Presentation LUT SOP Class,"
             << "  presentation LUT related print job settings will be ignored.");
  }

  OFCondition result = EC_Normal;
  if (printerSupportsPresentationLUT)
  {
    DcmDataset dset;
    DcmDataset *attributeListOut=NULL;
    Uint16 status=0;
    if (renderPresentationLUTinSCP)
    {
      result = plut->write(dset, OFFalse);
    } else {
      DVPSPresentationLUT identity;
      result = identity.write(dset, OFFalse);
    }

    if (result==EC_Normal)
    {
      OFCondition cond = printHandler.createRQ(UID_PresentationLUTSOPClass, presentationLUTInstanceUID, &dset, status, attributeListOut);
      if ((cond.good())&&((status==0)||((status & 0xf000)==0xb000)))
      {
        /* nothing */
      } else {
        presentationLUTInstanceUID.clear();
        result = EC_IllegalCall;
      }
      delete attributeListOut;
    }
  }

  return result;
}

OFCondition DVPSStoredPrint::addReferencedPLUTSQ(DcmItem &dset)
{
  if (referencedPresentationLUTInstanceUID.getLength() == 0) return EC_Normal;

  OFCondition result = EC_Normal;
  DcmElement *delem=NULL;
  DcmSequenceOfItems *dseq = new DcmSequenceOfItems(DCM_ReferencedPresentationLUTSequence);
  DcmItem *ditem = new DcmItem();

  if (ditem && dseq)
  {
     ADD_TO_DATASET2(DcmUniqueIdentifier, referencedPresentationLUTInstanceUID)
     if (result==EC_Normal)
     {
       dseq->insert(ditem);
       dset.insert(dseq, OFTrue /*replaceOld*/);
     } else {
      delete dseq;
      delete ditem;
     }
  } else {
    delete dseq;
    delete ditem;
    result = EC_MemoryExhausted;
  }
  return result;
}

OFCondition DVPSStoredPrint::addPresentationLUTReference(DcmItem& dset)
{
  DcmElement *delem=NULL;
  OFCondition result = EC_Normal;

  ADD_TO_DATASET(DcmUnsignedShort, illumination)
  ADD_TO_DATASET(DcmUnsignedShort, reflectedAmbientLight)

  if (presentationLUTInstanceUID.size() > 0)
  {

    DcmUniqueIdentifier refsopclassuid(DCM_ReferencedSOPClassUID);
    DcmUniqueIdentifier refsopinstanceuid(DCM_ReferencedSOPInstanceUID);
    if (result==EC_Normal) result = refsopclassuid.putString(UID_PresentationLUTSOPClass);
    if (result==EC_Normal) result = refsopinstanceuid.putString(presentationLUTInstanceUID.c_str());
    DcmSequenceOfItems *dseq = new DcmSequenceOfItems(DCM_ReferencedPresentationLUTSequence);
    DcmItem *ditem = new DcmItem();
    if ((result == EC_Normal) && ditem && dseq)
    {
       ADD_TO_DATASET2(DcmUniqueIdentifier, refsopclassuid)
       ADD_TO_DATASET2(DcmUniqueIdentifier, refsopinstanceuid)
       if (result==EC_Normal)
       {
         dseq->insert(ditem);
         dset.insert(dseq, OFTrue /*replaceOld*/);
       } else {
        delete dseq;
        delete ditem;
       }
    } else {
      delete dseq;
      delete ditem;
      result = EC_MemoryExhausted;
    }
  }
  return result;
}

OFCondition DVPSStoredPrint::printSCUcreateBasicFilmSession(
   DVPSPrintMessageHandler& printHandler,
   DcmDataset& dset,
   OFBool plutInSession)
{
  if (filmSessionInstanceUID.size() > 0) return EC_IllegalCall;

  DcmDataset *attributeListOut=NULL;
  Uint16 status=0;
  OFCondition result = EC_Normal;

  // we expect 'number of copies', 'print priority', 'medium type' and 'film destination' in dset
  // add illumination and reflection, and presentation LUT reference if necessary.
  if ((printHandler.printerSupportsPresentationLUT()) && plutInSession) result = addPresentationLUTReference(dset);

  if (result==EC_Normal)
  {
    OFCondition cond = printHandler.createRQ(UID_BasicFilmSessionSOPClass, filmSessionInstanceUID, &dset, status, attributeListOut);
    if ((cond.bad())||((status!=0)&&((status & 0xf000)!=0xb000)))
    {
      result = EC_IllegalCall;
      filmSessionInstanceUID.clear();
    }
    delete attributeListOut;
  }
  return result;
}

OFCondition DVPSStoredPrint::printSCUcreateBasicFilmBox(DVPSPrintMessageHandler& printHandler, OFBool plutInSession)
{
  if ((filmSessionInstanceUID.size() == 0)||(filmBoxInstanceUID.size() > 0)) return EC_IllegalCall;

  OFCondition result = EC_Normal;
  DcmDataset dset;
  DcmElement *delem=NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmSequenceOfItems *seq=NULL;
  DcmItem *ditem=NULL;
  DcmItem *item=NULL;
  DcmDataset *attributeListOut=NULL;
  char *c = NULL;
  Uint16 status=0;
  DcmStack stack;
  OFString grayscaleIB(UID_BasicGrayscaleImageBoxSOPClass);
  OFString annotationB(UID_BasicAnnotationBoxSOPClass);

  ADD_TO_DATASET(DcmShortText, imageDisplayFormat)
  if (filmOrientation.getLength() > 0)           { ADD_TO_DATASET(DcmCodeString, filmOrientation) }
  if (filmSizeID.getLength() > 0)                { ADD_TO_DATASET(DcmCodeString, filmSizeID) }
  if (magnificationType.getLength() > 0)         { ADD_TO_DATASET(DcmCodeString, magnificationType) }
  if (maxDensity.getLength() > 0)                { ADD_TO_DATASET(DcmUnsignedShort, maxDensity) }
  if (configurationInformation.getLength() > 0)  { ADD_TO_DATASET(DcmShortText, configurationInformation) }
  if (smoothingType.getLength() > 0)             { ADD_TO_DATASET(DcmCodeString, smoothingType) }
  if (borderDensity.getLength() > 0)             { ADD_TO_DATASET(DcmCodeString, borderDensity) }
  if (emptyImageDensity.getLength() > 0)         { ADD_TO_DATASET(DcmCodeString, emptyImageDensity) }
  if (minDensity.getLength() > 0)                { ADD_TO_DATASET(DcmUnsignedShort, minDensity) }
  if (trim.getLength() > 0)                      { ADD_TO_DATASET(DcmCodeString, trim) }
  if (requestedResolutionID.getLength() > 0)     { ADD_TO_DATASET(DcmCodeString, requestedResolutionID) }
  if ((printHandler.printerSupportsAnnotationBox())&&(annotationDisplayFormatID.getLength() > 0))
  {
        ADD_TO_DATASET(DcmCodeString, annotationDisplayFormatID)
  }

  // add Referenced Film Session SQ
  DcmUniqueIdentifier refsopclassuid(DCM_ReferencedSOPClassUID);
  DcmUniqueIdentifier refsopinstanceuid(DCM_ReferencedSOPInstanceUID);
  if (result==EC_Normal) result = refsopclassuid.putString(UID_BasicFilmSessionSOPClass);
  if (result==EC_Normal) result = refsopinstanceuid.putString(filmSessionInstanceUID.c_str());
  if (result==EC_Normal)
  {
    ditem = new DcmItem();
    if (ditem)
    {
      dseq = new DcmSequenceOfItems(DCM_ReferencedFilmSessionSequence);
      if (dseq)
      {
        ADD_TO_DATASET2(DcmUniqueIdentifier, refsopclassuid)
        ADD_TO_DATASET2(DcmUniqueIdentifier, refsopinstanceuid)
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

  // add illumination and reflection, and presentation LUT reference if necessary.
  if ((result==EC_Normal) && (printHandler.printerSupportsPresentationLUT()) && (!plutInSession)) result = addPresentationLUTReference(dset);

  if (result==EC_Normal)
  {
        size_t numItems = 0;
        size_t i;
    OFCondition cond = printHandler.createRQ(UID_BasicFilmBoxSOPClass, filmBoxInstanceUID, &dset, status, attributeListOut);
    if ((cond.good())&&((status==0)||((status & 0xf000)==0xb000))&& attributeListOut)
    {
      // N-CREATE was successful, now evaluate Referenced Image Box SQ
      stack.clear();
      if (EC_Normal == attributeListOut->search(DCM_ReferencedImageBoxSequence, stack, ESM_fromHere, OFFalse))
      {
        seq=(DcmSequenceOfItems *)stack.top();
        numItems = (size_t)seq->card();
        if (numItems > imageBoxContentList.size()) numItems = imageBoxContentList.size();
        for (i=0; i<numItems; i++)
        {
           item = seq->getItem(i);
           stack.clear();
           READ_FROM_DATASET2(DcmUniqueIdentifier, refsopclassuid)
           READ_FROM_DATASET2(DcmUniqueIdentifier, refsopinstanceuid)
           if (EC_Normal==result) result = refsopclassuid.getString(c);
           if ((EC_Normal==result) && c && (grayscaleIB == c))
           {
             result = refsopinstanceuid.getString(c);
                 if (EC_Normal==result) result = imageBoxContentList.setImageSOPInstanceUID(i, c);
           } else result = EC_IllegalCall; /* wrong SOP class or unable to read UID */
        }
      } else result=EC_TagNotFound;

      // evaluate Referenced Basic Annotation Box SQ if present
      stack.clear();
      annotationContentList.clearAnnotationSOPInstanceUIDs();
      if (EC_Normal == attributeListOut->search(DCM_ReferencedBasicAnnotationBoxSequence, stack, ESM_fromHere, OFFalse))
      {
        seq=(DcmSequenceOfItems *)stack.top();
        numItems = (size_t)seq->card();
        if (numItems > annotationContentList.size()) numItems = annotationContentList.size();
        for (i=0; i<numItems; i++)
        {
           item = seq->getItem(i);
           stack.clear();
           READ_FROM_DATASET2(DcmUniqueIdentifier, refsopclassuid)
           READ_FROM_DATASET2(DcmUniqueIdentifier, refsopinstanceuid)
           if (EC_Normal==result) result = refsopclassuid.getString(c);
           if ((EC_Normal==result) && c && (annotationB == c))
           {
             result = refsopinstanceuid.getString(c);
                 if (EC_Normal==result) result = annotationContentList.setAnnotationSOPInstanceUID(i, c);
           } else result = EC_IllegalCall; /* wrong SOP class or unable to read UID */
        }
      }
    } else {
      filmBoxInstanceUID.clear();
      result = EC_IllegalCall;
    }
    delete attributeListOut;
  }
  return result;
}


OFCondition DVPSStoredPrint::printSCUprintBasicFilmBox(DVPSPrintMessageHandler& printHandler)
{
  if (filmBoxInstanceUID.size() == 0) return EC_IllegalCall;
  DcmDataset *attributeListOut=NULL;
  Uint16 status=0;

  OFCondition cond = printHandler.actionRQ(UID_BasicFilmBoxSOPClass, filmBoxInstanceUID.c_str(),
    1 /* action type ID 1 = print */, NULL /* no action information */, status, attributeListOut);
  delete attributeListOut; // should be empty anyway

  if ((cond.good())&&((status==0)||((status & 0xf000)==0xb000))) return EC_Normal;
  return EC_IllegalCall; // otherwise
}

OFCondition DVPSStoredPrint::printSCUprintBasicFilmSession(DVPSPrintMessageHandler& printHandler)
{
  DcmDataset *attributeListOut=NULL;
  Uint16 status=0;

  OFCondition cond = printHandler.actionRQ(UID_BasicFilmSessionSOPClass, filmSessionInstanceUID.c_str(),
    1 /* action type ID 1 = print */, NULL /* no action information */, status, attributeListOut);
  delete attributeListOut; // should be empty anyway

  if ((cond.good())&&((status==0)||((status & 0xf000)==0xb000))) return EC_Normal;
  return EC_IllegalCall; // otherwise
}

OFCondition DVPSStoredPrint::printSCUdelete(DVPSPrintMessageHandler& printHandler)
{
  OFCondition cond = EC_Normal;
  Uint16 status=0;
  OFCondition result = EC_Normal;

  // delete basic film box
  if (filmBoxInstanceUID.size() > 0)
  {
    cond = printHandler.deleteRQ(UID_BasicFilmBoxSOPClass, filmBoxInstanceUID.c_str(), status);
    if ((cond.bad())||((status!=0)&&((status & 0xf000)!=0xb000))) result = EC_IllegalCall;
    filmBoxInstanceUID.clear();
  }

  // delete basic film session
  if (filmSessionInstanceUID.size() > 0)
  {
    cond = printHandler.deleteRQ(UID_BasicFilmSessionSOPClass, filmSessionInstanceUID.c_str(), status);
    if ((cond.bad())||((status!=0)&&((status & 0xf000)!=0xb000))) result = EC_IllegalCall;
    filmSessionInstanceUID.clear();
  }

  // delete presentation LUT
  if ((presentationLUTInstanceUID.size() > 0)&&(printHandler.printerSupportsPresentationLUT()))
  {
    cond = printHandler.deleteRQ(UID_PresentationLUTSOPClass, presentationLUTInstanceUID.c_str(), status);
    if ((cond.bad())||((status!=0)&&((status & 0xf000)!=0xb000))) result = EC_IllegalCall;
    presentationLUTInstanceUID.clear();
  }
  return result;
}

OFCondition DVPSStoredPrint::printSCUsetBasicImageBox(
    DVPSPrintMessageHandler& printHandler,
    size_t idx,
    DicomImage& image,
    OFBool useMonochrome1)
{
  DcmDataset dataset;
  DcmItem *ditem = NULL;
  DcmSequenceOfItems *dseq = NULL;
  char str[100];
  DcmPolymorphOBOW *pxData = NULL;
  const void *pxDataVoid;
  unsigned long width = image.getWidth();
  unsigned long height = image.getHeight();
  DcmDataset *attributeListOut=NULL;
  Uint16 status=0;

  const char *imageSopInstanceUID = imageBoxContentList.getSOPInstanceUID(idx);
  if (imageSopInstanceUID==NULL) return EC_IllegalCall;

  /* any presentation LUT to render on SCU side? */
  if (! renderPresentationLUTinSCP)
  {
    /* look for referenced Presentation LUT in image box */
    const char *imageplutuid = imageBoxContentList.getReferencedPresentationLUTInstanceUID(idx);
    char *filmplutuid = NULL;
    if (EC_Normal != referencedPresentationLUTInstanceUID.getString(filmplutuid)) filmplutuid=NULL;
    /* if absent, look for referenced Presentation LUT in film box */
    if ((imageplutuid == NULL)||(strlen(imageplutuid)==0)) imageplutuid = filmplutuid;
    DVPSPresentationLUT *pLUT = NULL;
    if (imageplutuid && (strlen(imageplutuid)>0)) pLUT = presentationLUTList.findPresentationLUT(imageplutuid);
    if (pLUT)
    {
      /* found presentation LUT, activate */
      if (pLUT->activate(&image))
      {
        if ((pLUT->getType() == DVPSP_table)&&(! transmitImagesIn12Bit))
        {
          DCMPSTAT_INFO("rendering Presentation LUT into 8-bit bitmap, image quality loss possible.");
        }
      } else {
        DCMPSTAT_INFO("unable to activate Presentation LUT, using IDENTITY instead.");
        image.setPresentationLutShape(ESP_Identity);
      }
    } /* else image.setPresentationLutShape(ESP_Identity); -- this does not make sense for MONO1 HG images */
  } /* else image.setPresentationLutShape(ESP_Identity); -- this does not make sense for MONO1 HG images */

  OFCondition result = imageBoxContentList.prepareBasicImageBox(idx, dataset);
  if (EC_Normal == result)
  {
    ditem = new DcmItem();
    if (ditem)
    {
      dseq = new DcmSequenceOfItems(DCM_BasicGrayscaleImageSequence);
      if (dseq)
      {
        if (EC_Normal==result) result = DVPSHelper::putUint16Value(ditem, DCM_SamplesPerPixel, 1);
        if (useMonochrome1)
        {
          image.setPolarity(EPP_Reverse);
          if (EC_Normal==result) result = DVPSHelper::putStringValue(ditem, DCM_PhotometricInterpretation, "MONOCHROME1");
        } else {
          if (EC_Normal==result) result = DVPSHelper::putStringValue(ditem, DCM_PhotometricInterpretation, "MONOCHROME2");
        }
        if (EC_Normal==result) result = DVPSHelper::putUint16Value(ditem, DCM_PixelRepresentation, 0);
        if (EC_Normal==result) result = DVPSHelper::putUint16Value(ditem, DCM_Rows, (Uint16)height);
        if (EC_Normal==result) result = DVPSHelper::putUint16Value(ditem, DCM_Columns, (Uint16)width);
        double aspectRatio = image.getWidthHeightRatio();
        if ((aspectRatio != 1.0)&&(aspectRatio != 0))
        {
          sprintf(str, "10000\\%ld", (long)(aspectRatio*10000.0));
          if (EC_Normal==result) result = DVPSHelper::putStringValue(ditem, DCM_PixelAspectRatio, str);
        }
        if (transmitImagesIn12Bit)
        {
          if (EC_Normal==result) result = DVPSHelper::putUint16Value(ditem, DCM_BitsAllocated, 16);
          if (EC_Normal==result) result = DVPSHelper::putUint16Value(ditem, DCM_BitsStored, 12);
          if (EC_Normal==result) result = DVPSHelper::putUint16Value(ditem, DCM_HighBit, 11);
          if (EC_Normal==result)
          {
             pxDataVoid = image.getOutputData(12);
                 pxData = new DcmPolymorphOBOW(DCM_PixelData);
             if (pxData && pxDataVoid)
             {
               result = pxData->putUint16Array((Uint16 *)pxDataVoid, (width*height));
               if (EC_Normal==result) result = ditem->insert(pxData, OFTrue /*replaceOld*/); else delete pxData;
            } else result = EC_IllegalCall;
          }
        } else {
          if (EC_Normal==result) result = DVPSHelper::putUint16Value(ditem, DCM_BitsAllocated, 8);
          if (EC_Normal==result) result = DVPSHelper::putUint16Value(ditem, DCM_BitsStored, 8);
          if (EC_Normal==result) result = DVPSHelper::putUint16Value(ditem, DCM_HighBit, 7);
          if (EC_Normal==result)
          {
             pxDataVoid = image.getOutputData(8);
                 pxData = new DcmPolymorphOBOW(DCM_PixelData);
             if (pxData && pxDataVoid)
             {
               result = pxData->putUint8Array((Uint8 *)pxDataVoid, (width*height));
               if (EC_Normal==result) result = ditem->insert(pxData, OFTrue /*replaceOld*/); else delete pxData;
            } else result = EC_IllegalCall;
          }
        }

        if (result==EC_Normal)
        {
          dseq->insert(ditem);
          dataset.insert(dseq, OFTrue /*replaceOld*/);
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
        } else result = EC_MemoryExhausted;
  }

  if (EC_Normal == result)
  {
    OFCondition cond = printHandler.setRQ(UID_BasicGrayscaleImageBoxSOPClass, imageSopInstanceUID, &dataset, status, attributeListOut);
    if ((cond.bad())||((status!=0)&&((status & 0xf000)!=0xb000))) result = EC_IllegalCall;
  }
  delete attributeListOut;
  return result;
}

OFCondition DVPSStoredPrint::printSCUsetBasicAnnotationBox(
    DVPSPrintMessageHandler& printHandler,
    size_t idx)
{
  DcmDataset dataset;
  DcmDataset *attributeListOut=NULL;
  Uint16 status=0;
  OFCondition result = EC_Normal;

  if (printHandler.printerSupportsAnnotationBox())
  {
    const char *annotationSopInstanceUID = annotationContentList.getSOPInstanceUID(idx);
    if ((annotationSopInstanceUID==NULL)||(strlen(annotationSopInstanceUID)==0))
    {
       DCMPSTAT_INFO("not enough Annotation Boxes created by printer, ignoring annotation.");
       return EC_Normal;
    }

    result = annotationContentList.prepareBasicAnnotationBox(idx, dataset);

    if (EC_Normal == result)
    {
      OFCondition cond = printHandler.setRQ(UID_BasicAnnotationBoxSOPClass, annotationSopInstanceUID, &dataset, status, attributeListOut);
      if ((cond.bad())||((status!=0)&&((status & 0xf000)!=0xb000))) result = EC_IllegalCall;
    }
    delete attributeListOut;
  } else {
    DCMPSTAT_INFO("printer does not support Annotation Box, ignoring annotation.");
  }

  return result;
}

const char *DVPSStoredPrint::getMaxDensity()
{
  if (maxDensity.getLength() > 0)
  {
    Uint16 density=0;
    if (EC_Normal == maxDensity.getUint16(density,0))
    {
      char buf[20];
      sprintf(buf, "%hu", density);
      tempDensity = buf;
      return tempDensity.c_str();
    }
  }
  return NULL;
}

const char *DVPSStoredPrint::getMinDensity()
{
  if (minDensity.getLength() > 0)
  {
    Uint16 density=0;
    if (EC_Normal == minDensity.getUint16(density,0))
    {
      char buf[20];
      sprintf(buf, "%hu", density);
      tempDensity = buf;
      return tempDensity.c_str();
    }
  }
  return NULL;
}

Uint16 DVPSStoredPrint::getMaxDensityValue()
{
  if (maxDensity.getLength() > 0)
  {
    Uint16 density = 0;
    if (EC_Normal == maxDensity.getUint16(density, 0))
      return density;
  }
  return 300;
}

Uint16 DVPSStoredPrint::getMinDensityValue()
{
  if (minDensity.getLength() > 0)
  {
    Uint16 density = 0;
    if (EC_Normal == minDensity.getUint16(density, 0))
      return density;
  }
  return 20;
}

OFCondition DVPSStoredPrint::setMaxDensity(const char *value)
{
  OFCondition result = EC_Normal;
  if (value && (strlen(value)>0))
  {
        Uint16 density = 0;
        if (1 == (sscanf(value, "%hu", &density)))
        {
          result = maxDensity.putUint16(density, 0);
        } else result = EC_IllegalCall;
  } else maxDensity.clear();
  return result;
}

OFCondition DVPSStoredPrint::setMinDensity(const char *value)
{
  OFCondition result = EC_Normal;
  if (value && (strlen(value)>0))
  {
        Uint16 density = 0;
        if (1 == (sscanf(value, "%hu", &density)))
        {
          result = minDensity.putUint16(density, 0);
        } else result = EC_IllegalCall;
  } else minDensity.clear();
  return result;
}

OFCondition DVPSStoredPrint::setSingleAnnotation(
    const char *displayformat,
    const char *text,
    Uint16 position)
{
  OFCondition result = EC_IllegalCall;
  if (displayformat && text)
  {
    char newuid[70];
    dcmGenerateUniqueIdentifier(newuid);
    deleteAnnotations();
    result = annotationContentList.addAnnotationBox(newuid, text, position);
    if (EC_Normal==result) result = annotationDisplayFormatID.putString(displayformat);
  }
  return result;
}

void DVPSStoredPrint::deleteAnnotations()
{
  annotationContentList.clear();
  annotationDisplayFormatID.clear();
  return;
}



OFBool DVPSStoredPrint::printSCPCreate(
  DVConfiguration& cfg,
  const char *cfgname,
  DcmDataset *rqDataset,
  T_DIMSE_Message& rsp,
  DcmDataset *& rspDataset,
  OFBool presentationLUTnegotiated,
  DVPSPresentationLUT_PList& globalPresentationLUTList,
  const char *filmSessionUID,
  DcmUniqueIdentifier& study,
  DcmUniqueIdentifier& psSeries,
  DcmUniqueIdentifier& imgSeries)
{
  studyInstanceUID = study;
  seriesInstanceUID = psSeries;
  imageSeriesInstanceUID = imgSeries;

  OFBool result = OFTrue;
  DcmStack stack;
  filmBoxInstanceUID = rsp.msg.NCreateRSP.AffectedSOPInstanceUID;

  // filmOrientation
  if (result)
  {
    READ_FROM_PDATASET(DcmCodeString, filmOrientation)
    if (filmOrientation.getLength() == 0) filmOrientation.putString(DEFAULT_filmOrientation);
    else
    {
      OFString aString;
      filmOrientation.getOFString(aString, 0, OFTrue);
      if ((aString != "PORTRAIT")&&(aString != "LANDSCAPE"))
      {
        DCMPSTAT_INFO("cannot create Basic Film Box: illegal film orientation: '" << aString.c_str() << "'");
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    }
  }

  // imageDisplayFormat
  if (result)
  {
    READ_FROM_PDATASET(DcmShortText, imageDisplayFormat)
    if (imageDisplayFormat.getLength() == 0)
    {
        DCMPSTAT_INFO("cannot create Basic Film Box: image display format missing or empty");
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_MissingAttribute;
        result = OFFalse;
    } else {
      currentValuesValid = OFFalse;
      updateCache(); // evaluates image display format and computes number of columns and rows
      if (currentValuesValid)
      {
        // now we check whether this is a supported layout as per the config file
        Uint32 numPortraitDisplayFormats = cfg.getTargetPrinterNumberOfPortraitDisplayFormats(cfgname);
        if (numPortraitDisplayFormats > 0)
        {
          OFBool found = OFFalse;
          DVPSFilmOrientation orientation = getFilmOrientation();
          Uint32 col=0;
          Uint32 row=0;
          for (Uint32 i=0; i<numPortraitDisplayFormats; i++)
          {
             col=cfg.getTargetPrinterPortraitDisplayFormatColumns(cfgname,i);
             row=cfg.getTargetPrinterPortraitDisplayFormatRows(cfgname,i);
             if (orientation == DVPSF_landscape)
             {
               if ((col==currentNumRows)&&(row==currentNumCols))
               {
                 found = OFTrue;
                 break;
               }
             } else {
               if ((col==currentNumCols)&&(row==currentNumRows))
               {
                 found = OFTrue;
                 break;
               }
             }
          }
          if (! found)
          {
            OFString aString;
            imageDisplayFormat.getOFStringArray(aString);
            DCMPSTAT_INFO("cannot create Basic Film Box: unsupported image display format: '" << aString.c_str() << "'");
            rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
            result = OFFalse;
          }
        }
      } else {
        OFString aString;
        imageDisplayFormat.getOFStringArray(aString);
        DCMPSTAT_INFO("cannot create Basic Film Box: illegal image display format: '" << aString.c_str() << "'");
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    }
  }

  // filmSizeID
  if (result)
  {
    Uint32 numFilmSizes = cfg.getTargetPrinterNumberOfFilmSizeIDs(cfgname);
    READ_FROM_PDATASET(DcmCodeString, filmSizeID)
    if (filmSizeID.getLength() == 0)
    {
      if (numFilmSizes > 0)
      {
        OFString aString;
        cfg.getTargetPrinterFilmSizeID(cfgname, 0, aString);
        filmSizeID.putString(aString.c_str());
      } else {
        filmSizeID.putString(DEFAULT_filmSizeID);
      }
    } else {
      // check whether we can accept the proposed medium type
      OFString theSizeID;
      OFString aString;
      OFBool found = OFFalse;
      filmSizeID.getOFString(theSizeID, 0, OFTrue);
      for (Uint32 i=0; i<numFilmSizes; i++)
      {
        cfg.getTargetPrinterFilmSizeID(cfgname, i, aString);
        if (theSizeID == aString)
        {
          found = OFTrue;
          break;
        }
      }
      if (! found)
      {
        DCMPSTAT_INFO("cannot create Basic Film Box: illegal film size ID: '" << theSizeID.c_str() << "'");
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    }
  }

  // magnificationType
  if (result)
  {
    Uint32 numMagnifications = cfg.getTargetPrinterNumberOfMagnificationTypes(cfgname);
    READ_FROM_PDATASET(DcmCodeString, magnificationType)
    if (magnificationType.getLength() == 0)
    {
      if (numMagnifications > 0)
      {
        OFString aString;
        cfg.getTargetPrinterMagnificationType(cfgname, 0, aString);
        magnificationType.putString(aString.c_str());
      } else {
        magnificationType.putString(DEFAULT_magnificationType);
      }
    } else {
      // check whether we can accept the proposed medium type
      OFString theMagnification;
      OFString aString;
      OFBool found = OFFalse;
      magnificationType.getOFString(theMagnification, 0, OFTrue);
      for (Uint32 i=0; i<numMagnifications; i++)
      {
        cfg.getTargetPrinterMagnificationType(cfgname, i, aString);
        if (theMagnification == aString)
        {
          found = OFTrue;
          break;
        }
      }
      if (! found)
      {
        DCMPSTAT_INFO("cannot create Basic Film Box: illegal magnification type: '" << theMagnification.c_str() << "'");
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    }
  }

  // smoothingType
  if (result)
  {
    Uint32 numSmoothings = cfg.getTargetPrinterNumberOfSmoothingTypes(cfgname);
    READ_FROM_PDATASET(DcmCodeString, smoothingType)
    if (smoothingType.getLength() == 0)
    {
      if (numSmoothings > 0)
      {
        OFString aString;
        cfg.getTargetPrinterSmoothingType(cfgname, 0, aString);
        smoothingType.putString(aString.c_str());
      }
    } else {
      // check whether we can accept the proposed smoothing type
      OFString theSmoothing;
      OFString aString;
      OFBool found = OFFalse;
      smoothingType.getOFString(theSmoothing, 0, OFTrue);
      for (Uint32 i=0; i<numSmoothings; i++)
      {
        cfg.getTargetPrinterSmoothingType(cfgname, i, aString);
        if (theSmoothing == aString)
        {
          found = OFTrue;
          break;
        }
      }
      if (numSmoothings == 0)
      {
        DCMPSTAT_INFO("cannot create Basic Film Box: smoothing type requested but not supported.");
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      }
      else if (! found)
      {
        DCMPSTAT_INFO("cannot create Basic Film Box: illegal smoothing type: '" << theSmoothing.c_str() << "'");
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    }
  }


  // borderDensity
  if (result)
  {
    Uint32 numBorderDensities = cfg.getTargetPrinterNumberOfBorderDensities(cfgname);
    READ_FROM_PDATASET(DcmCodeString, borderDensity)
    if (borderDensity.getLength() == 0)
    {
      if (numBorderDensities > 0)
      {
        OFString aString;
        cfg.getTargetPrinterBorderDensity(cfgname, 0, aString);
        borderDensity.putString(aString.c_str());
      }
    } else {
      // check whether we can accept the proposed border density
      if (numBorderDensities == 0) // we don't support border density
      {
        DCMPSTAT_INFO("cannot create Basic Film Box: border density requested but not supported.");
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      } else {
        OFString theBorderDensity;
        OFString aString;
        OFBool found = OFFalse;
        OFBool supportsNumericDensity = OFFalse;
        unsigned long l;
        borderDensity.getOFString(theBorderDensity, 0, OFTrue);
        for (Uint32 i=0; i<numBorderDensities; i++)
        {
          cfg.getTargetPrinterBorderDensity(cfgname, i, aString);
          if (theBorderDensity == aString)
          {
            found = OFTrue;
            break;
          } else {
            if (1 == sscanf(aString.c_str(), "%lu", &l)) supportsNumericDensity = OFTrue;
          }
        }
        if ((! found) && supportsNumericDensity)
        {
          // the density was not found in the list; check whether it is numerical
          if (1 == sscanf(theBorderDensity.c_str(), "%lu", &l)) found = OFTrue;
        }

        if (! found)
        {
          DCMPSTAT_INFO("cannot create Basic Film Box: illegal border density: '" << theBorderDensity.c_str() << "'");
          rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
          result = OFFalse;
        }
      }
    }
  }

  // emptyImageDensity
  if (result)
  {
    Uint32 numEmptyImageDensities = cfg.getTargetPrinterNumberOfEmptyImageDensities(cfgname);
    READ_FROM_PDATASET(DcmCodeString, emptyImageDensity)
    if (emptyImageDensity.getLength() == 0)
    {
      if (numEmptyImageDensities > 0)
      {
        OFString aString;
        cfg.getTargetPrinterEmptyImageDensity(cfgname, 0, aString);
        emptyImageDensity.putString(aString.c_str());
      }
    } else {
      // check whether we can accept the proposed empty image density
      if (numEmptyImageDensities == 0) // we don't support empty image density
      {
        DCMPSTAT_INFO("cannot create Basic Film Box: empty image density requested but not supported.");
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      } else {
        OFString theEIDensity;
        OFString aString;
        OFBool found = OFFalse;
        OFBool supportsNumericDensity = OFFalse;
        unsigned long l;
        emptyImageDensity.getOFString(theEIDensity, 0, OFTrue);
        for (Uint32 i=0; i<numEmptyImageDensities; i++)
        {
          cfg.getTargetPrinterEmptyImageDensity(cfgname, i, aString);
          if (theEIDensity == aString)
          {
            found = OFTrue;
            break;
          } else {
            if (1 == sscanf(aString.c_str(), "%lu", &l)) supportsNumericDensity = OFTrue;
          }
        }
        if ((! found) && supportsNumericDensity)
        {
          // the density was not found in the list; check whether it is numerical
          if (1 == sscanf(theEIDensity.c_str(), "%lu", &l)) found = OFTrue;
        }

        if (! found)
        {
          DCMPSTAT_INFO("cannot create Basic Film Box: illegal empty image density: '" << theEIDensity.c_str() << "'");
          rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
          result = OFFalse;
        }
      }
    }
  }

  // maxDensity
  if (result)
  {
    Uint32 numMaxDensities = cfg.getTargetPrinterNumberOfMaxDensities(cfgname);
    READ_FROM_PDATASET(DcmUnsignedShort, maxDensity)
    if (maxDensity.getLength() == 0)
    {
      if (numMaxDensities > 0)
      {
        OFString aString;
        cfg.getTargetPrinterMaxDensity(cfgname, 0, aString);
        if (EC_Normal != setMaxDensity(aString.c_str())) maxDensity.putUint16(DEFAULT_maxDensity, 0);
      }
    } // we don't check a max density set by the user (for now)
  }

  // minDensity
  if (result)
  {
    Uint32 numMinDensities = cfg.getTargetPrinterNumberOfMinDensities(cfgname);
    READ_FROM_PDATASET(DcmUnsignedShort, minDensity)
    if (minDensity.getLength() == 0)
    {
      if (numMinDensities > 0)
      {
        OFString aString;
        cfg.getTargetPrinterMinDensity(cfgname, 0, aString);
        if (EC_Normal != setMinDensity(aString.c_str())) minDensity.putUint16(DEFAULT_minDensity, 0);
      }
    } else {
      if (numMinDensities == 0) // we don't support min density
      {
        DCMPSTAT_INFO("cannot create Basic Film Box: min density requested but not supported.");
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      }
      // we don't check a min density set by the user (for now)
    }
  }

  // trim
  if (result)
  {
    READ_FROM_PDATASET(DcmCodeString, trim)
    if (trim.getLength() == 0)
    {
      if (cfg.getTargetPrinterSupportsTrim(cfgname)) trim.putString(DEFAULT_trim);
    }
    else
    {
      if (cfg.getTargetPrinterSupportsTrim(cfgname))
      {
        OFString aString;
        trim.getOFString(aString, 0, OFTrue);
        if ((aString != "YES")&&(aString != "NO"))
        {
          DCMPSTAT_INFO("cannot create Basic Film Box: illegal trim: '" << aString.c_str() << "'");
          rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
          result = OFFalse;
        }
      } else {
        DCMPSTAT_INFO("cannot create Basic Film Box: trim requested but not supported.");
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      }
    }
  }

  // configurationInformation
  if (result)
  {
    READ_FROM_PDATASET(DcmShortText, configurationInformation)
    if (configurationInformation.getLength() > 0)
    {
      // check whether we can accept the proposed configuration information
      Uint32 numConfigurationInformation = cfg.getTargetPrinterNumberOfConfigurationSettings(cfgname);
      if (numConfigurationInformation == 0) // we don't support configuration information
      {
        DCMPSTAT_INFO("cannot create Basic Film Box: configuration information requested but not supported.");
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      } else {
        OFString theConfiguration;
        OFBool found = OFFalse;
        configurationInformation.getOFString(theConfiguration, 0, OFTrue);
        for (Uint32 i=0; i<numConfigurationInformation; i++)
        {
          if (theConfiguration == cfg.getTargetPrinterConfigurationSetting(cfgname, i))
          {
            found = OFTrue;
            break;
          }
        }
        if (! found)
        {
          DCMPSTAT_INFO("cannot create Basic Film Box: illegal configuration information: '" << theConfiguration.c_str() << "'");
          rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
          result = OFFalse;
        }
      }
    }
  }

  // requestedResolutionID
  if (result)
  {
    Uint32 numResolutionIDs = cfg.getTargetPrinterNumberOfPrinterResolutionIDs(cfgname);
    READ_FROM_PDATASET(DcmCodeString, requestedResolutionID)
    if (requestedResolutionID.getLength() == 0)
    {
      if (numResolutionIDs > 0)
      {
        OFString aString;
        cfg.getTargetPrinterResolutionID(cfgname, 0, aString);
        requestedResolutionID.putString(aString.c_str());
      }
    } else {
      // check whether we can accept the requested resolution ID
      if (numResolutionIDs == 0) // we don't support requested resolution ID
      {
        DCMPSTAT_INFO("cannot create Basic Film Box: requested resolution ID present but not supported.");
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      } else {
        OFString theResolutionID;
        OFString aString;
        OFBool found = OFFalse;
        requestedResolutionID.getOFString(theResolutionID, 0, OFTrue);
        for (Uint32 i=0; i<numResolutionIDs; i++)
        {
          cfg.getTargetPrinterResolutionID(cfgname, i, aString);
          if (theResolutionID == aString)
          {
            found = OFTrue;
            break;
          }
        }
        if (! found)
        {
          DCMPSTAT_INFO("cannot create Basic Film Box: illegal requested resolution ID: '" << theResolutionID.c_str() << "'");
          rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
          result = OFFalse;
        }
      }
    }
  }

  if (presentationLUTnegotiated)
  {

    // illumination
    if (result)
    {
      READ_FROM_PDATASET(DcmUnsignedShort, illumination)
      if (illumination.getLength() == 0) illumination.putUint16(DEFAULT_illumination, 0);
      // we don't check illumination set by the user (for now)
    }

    // reflectedAmbientLight
    if (result)
    {
      READ_FROM_PDATASET(DcmUnsignedShort, reflectedAmbientLight)
      if (reflectedAmbientLight.getLength() == 0) illumination.putUint16(DEFAULT_reflectedAmbientLight, 0);
      // we don't check reflected ambient light set by the user (for now)
    }

    // referenced presentation LUT sequence
    if (result)
    {
      stack.clear();

      if (rqDataset && (EC_Normal == rqDataset->search(DCM_ReferencedPresentationLUTSequence, stack, ESM_fromHere, OFFalse)))
      {
        DcmSequenceOfItems *seq=(DcmSequenceOfItems *)stack.top();
        if (seq->card() ==1)
        {
           OFString aString;
           DcmItem *item = seq->getItem(0);
           stack.clear();
           READ_FROM_DATASET2(DcmUniqueIdentifier, referencedPresentationLUTInstanceUID)
           if (referencedPresentationLUTInstanceUID.getLength() > 0)
           {
             referencedPresentationLUTInstanceUID.getOFString(aString,0);
             DVPSPresentationLUT *currentPLUT = globalPresentationLUTList.findPresentationLUT(aString.c_str());
             if (NULL == currentPLUT)
             {
               DCMPSTAT_INFO("cannot create Basic Film Box: presentation LUT reference cannot be resolved");
               rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
               result = OFFalse;
             } else {
               // check referenced SOP class UID
               DcmUniqueIdentifier refClassUID(DCM_ReferencedSOPClassUID);
               stack.clear();
               READ_FROM_DATASET2(DcmUniqueIdentifier, refClassUID)
               if (refClassUID.getLength() > 0)
               {
                  aString.clear();
                  refClassUID.getOFString(aString,0, OFTrue);
                  if (aString != UID_PresentationLUTSOPClass)
                  {
                    DCMPSTAT_INFO("cannot create Basic Film Box: referenced SOP class UID in referenced presentation LUT sequence incorrect:"
                        << DcmObject::PrintHelper(*stack.top(), DCMTypes::PF_shortenLongTagValues));
                    rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
                    result = OFFalse;
                  } else {
                    // referenced presentation LUT sequence is OK
                    // synchronize presentationLUTInstanceUID and referencedPresentationLUTInstanceUID
                    presentationLUTInstanceUID.clear();
                    referencedPresentationLUTInstanceUID.getOFString(presentationLUTInstanceUID,0);
                    referencedPresentationLUTAlignment = currentPLUT->getAlignment();
                  }
               } else {
                  DCMPSTAT_INFO("cannot create Basic Film Box: no referenced SOP class UID in referenced presentation LUT sequence");
                  rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
                  result = OFFalse;
               }
             }
           } else {
             DCMPSTAT_INFO("cannot create Basic Film Box: no referenced SOP instance UID in referenced presentation LUT sequence");
             rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
             result = OFFalse;
           }
        } else {
          DCMPSTAT_INFO("cannot create Basic Film Box: referenced presentation LUT sequence number of items != 1");
          rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
          result = OFFalse;
        }
      }
    }

  } /* if presentationLUTnegotiated */

  DcmSequenceOfItems *refFilmSessionSequence = NULL;

  // referenced film session sequence
  if (result)
  {
    stack.clear();

    if (rqDataset && (EC_Normal == rqDataset->search(DCM_ReferencedFilmSessionSequence, stack, ESM_fromHere, OFFalse)))
    {
      DcmUniqueIdentifier classUID(DCM_ReferencedSOPClassUID);
      DcmUniqueIdentifier instanceUID(DCM_ReferencedSOPInstanceUID);
      refFilmSessionSequence =(DcmSequenceOfItems *)stack.top();
      if (refFilmSessionSequence->card() ==1)
      {
         OFString aString;
         DcmItem *item = refFilmSessionSequence->getItem(0);
         stack.clear();
         READ_FROM_DATASET2(DcmUniqueIdentifier, instanceUID)
         if (instanceUID.getLength() > 0)
         {
           instanceUID.getOFString(aString,0);
           if (aString != filmSessionUID)
           {
             DCMPSTAT_INFO("cannot create Basic Film Box: referenced film session instance UID incorrect");
             rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
             result = OFFalse;
           } else {
             // check referenced SOP class UID
             stack.clear();
             READ_FROM_DATASET2(DcmUniqueIdentifier, classUID)
             if (classUID.getLength() > 0)
             {
                aString.clear();
                classUID.getOFString(aString,0, OFTrue);
                if (aString != UID_BasicFilmSessionSOPClass)
                {
                  DCMPSTAT_INFO("cannot create Basic Film Box: referenced SOP class UID in referenced film session sequence incorrect:"
                      << DcmObject::PrintHelper(*stack.top(), DCMTypes::PF_shortenLongTagValues));
                  rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
                  result = OFFalse;
                }
             } else {
                DCMPSTAT_INFO("cannot create Basic Film Box: no referenced SOP class UID in referenced film session sequence");
                rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
                result = OFFalse;
             }
           }
         } else {
           DCMPSTAT_INFO("cannot create Basic Film Box: no referenced SOP instance UID in referenced film session sequence");
           rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
           result = OFFalse;
         }
      } else {
        DCMPSTAT_INFO("cannot create Basic Film Box: referenced film session sequence number of items != 1");
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    } else {
      DCMPSTAT_INFO("cannot create Basic Film Box: referenced film session sequence absent");
      rsp.msg.NCreateRSP.DimseStatus = STATUS_N_MissingAttribute;
      result = OFFalse;
    }
  }

  // browse through rqDataset and check for unsupported attributes
  if (result && rqDataset)
  {
    OFBool intoSub = OFTrue;
    stack.clear();
    while (EC_Normal == rqDataset->nextObject(stack, intoSub))
    {
      intoSub = OFFalse;
      const DcmTagKey& currentTag = (stack.top())->getTag();
      if (currentTag.getElement() == 0x0000) /* group length */ ;
      else if (currentTag == DCM_ImageDisplayFormat) /* OK */ ;
      else if (currentTag == DCM_FilmOrientation) /* OK */ ;
      else if (currentTag == DCM_FilmSizeID) /* OK */ ;
      else if (currentTag == DCM_MagnificationType) /* OK */ ;
      else if (currentTag == DCM_SmoothingType) /* OK */ ;
      else if (currentTag == DCM_BorderDensity) /* OK */ ;
      else if (currentTag == DCM_EmptyImageDensity) /* OK */ ;
      else if (currentTag == DCM_MinDensity) /* OK */ ;
      else if (currentTag == DCM_MaxDensity) /* OK */ ;
      else if (currentTag == DCM_Trim) /* OK */ ;
      else if (currentTag == DCM_ConfigurationInformation) /* OK */ ;
      else if (currentTag == DCM_RequestedResolutionID) /* OK */ ;
      else if (currentTag == DCM_ReferencedFilmSessionSequence) /* OK */ ;
      else if (currentTag == DCM_Illumination)
      {
        if (! presentationLUTnegotiated)
        {
          DCMPSTAT_INFO("cannot create Basic Film Box: illumination received:\n"
               << DcmObject::PrintHelper(*stack.top(), DCMTypes::PF_shortenLongTagValues));
          rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;
          result = OFFalse;
        }
      }
      else if (currentTag == DCM_ReflectedAmbientLight)
      {
        if (! presentationLUTnegotiated)
        {
          DCMPSTAT_INFO("cannot create Basic Film Box: reflected ambient light received:\n"
              << DcmObject::PrintHelper(*stack.top(), DCMTypes::PF_shortenLongTagValues));
          rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;
          result = OFFalse;
        }
      }
      else if (currentTag == DCM_ReferencedPresentationLUTSequence)
      {
        if (! presentationLUTnegotiated)
        {
          DCMPSTAT_INFO("cannot create Basic Film Box: referenced presentation LUT sequence received:\n"
              << DcmObject::PrintHelper(*stack.top(), DCMTypes::PF_shortenLongTagValues));
          rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;
          result = OFFalse;
        }
      }
      else
      {
        DCMPSTAT_INFO("cannot create Basic Film Box: unsupported attribute received:\n"
            << DcmObject::PrintHelper(*stack.top(), DCMTypes::PF_shortenLongTagValues));
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      }
    }
  }

  // if n-create was successful, create response dataset
  if (result)
  {
    rspDataset = new DcmDataset;
    if (rspDataset)
    {
      OFCondition writeresult = EC_Normal;
      DcmElement *delem = NULL;

      if (refFilmSessionSequence) // should never be NULL if we get this far
      {
        DcmSequenceOfItems *newRefFilmSessionSequence = new DcmSequenceOfItems(*refFilmSessionSequence);
        if (newRefFilmSessionSequence) rspDataset->insert(newRefFilmSessionSequence, OFTrue /*replaceOld*/);
        else writeresult = EC_MemoryExhausted;
      }

      ADD_TO_PDATASET(DcmShortText, imageDisplayFormat)
      ADD_TO_PDATASET(DcmCodeString, filmOrientation)
      ADD_TO_PDATASET(DcmCodeString, filmSizeID)
      ADD_TO_PDATASET(DcmCodeString, magnificationType)
      ADD_TO_PDATASET(DcmUnsignedShort, maxDensity)
      ADD_TO_PDATASET(DcmShortText, configurationInformation)
      if (smoothingType.getLength() > 0) { ADD_TO_PDATASET(DcmCodeString, smoothingType) }
      if (borderDensity.getLength() > 0) { ADD_TO_PDATASET(DcmCodeString, borderDensity) }
      if (emptyImageDensity.getLength() > 0) { ADD_TO_PDATASET(DcmCodeString, emptyImageDensity) }
      if (minDensity.getLength() > 0) { ADD_TO_PDATASET(DcmUnsignedShort, minDensity) }
      if (trim.getLength() > 0) { ADD_TO_PDATASET(DcmCodeString, trim) }
      if (requestedResolutionID.getLength() > 0) { ADD_TO_PDATASET(DcmCodeString, requestedResolutionID) }

      if (presentationLUTnegotiated)
      {
        if (referencedPresentationLUTInstanceUID.getLength() == 0)
        {
          referencedPresentationLUTInstanceUID.putString(WELLKNOWN_IDENTITY_PLUT_UID);
          if (NULL == globalPresentationLUTList.findPresentationLUT(WELLKNOWN_IDENTITY_PLUT_UID))
          {
            DVPSPresentationLUT *wellknownlut = new DVPSPresentationLUT();
            if (wellknownlut)
            {
              writeresult = wellknownlut->setType(DVPSP_identity);
              if (EC_Normal == writeresult) writeresult = wellknownlut->setSOPInstanceUID(WELLKNOWN_IDENTITY_PLUT_UID);
              if (EC_Normal == writeresult) globalPresentationLUTList.insert(wellknownlut);
            } else writeresult = EC_MemoryExhausted;
          }
        }
        if (EC_Normal == writeresult) writeresult = addPresentationLUTReference(*rspDataset);
      }

      // create image boxes and referenced image box sequence
      if (imageBoxContentList.printSCPCreate(currentNumRows * currentNumCols, studyInstanceUID, imageSeriesInstanceUID, cfg.getNetworkAETitle()))
      {
        if (EC_Normal == writeresult) writeresult = imageBoxContentList.writeReferencedImageBoxSQ(*rspDataset);
      } else writeresult = EC_MemoryExhausted;

      if (EC_Normal == writeresult)
      {
        rsp.msg.NCreateRSP.DataSetType = DIMSE_DATASET_PRESENT;
      } else {
        delete rspDataset;
        rspDataset = NULL;
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_ProcessingFailure;
        result = OFFalse;
      }
    } else {
      rsp.msg.NCreateRSP.DimseStatus = STATUS_N_ProcessingFailure;
      result = OFFalse;
    }
  }
  return result;
}




OFBool DVPSStoredPrint::printSCPSet(
  DVConfiguration& cfg,
  const char *cfgname,
  DcmDataset *rqDataset,
  T_DIMSE_Message& rsp,
  DcmDataset *& rspDataset,
  OFBool presentationLUTnegotiated,
  DVPSPresentationLUT_PList& globalPresentationLUTList)
{
  OFBool result = OFTrue;
  DcmStack stack;
  OFCondition writeresult = EC_Normal;
  DcmElement *delem = NULL;

  rspDataset = new DcmDataset;
  if ((rqDataset == NULL)||(rspDataset == NULL))
  {
    rsp.msg.NSetRSP.DimseStatus = STATUS_N_ProcessingFailure;
    result = OFFalse;
  }

  // magnificationType
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)magnificationType.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      magnificationType = *((DcmCodeString *)(stack.top()));
      Uint32 numMagnifications = cfg.getTargetPrinterNumberOfMagnificationTypes(cfgname);
      OFString theMagnification;
      OFString aString;
      OFBool found = OFFalse;
      magnificationType.getOFString(theMagnification, 0, OFTrue);
      for (Uint32 i=0; i<numMagnifications; i++)
      {
        cfg.getTargetPrinterMagnificationType(cfgname, i, aString);
        if (theMagnification == aString)
        {
          found = OFTrue;
          break;
        }
      }
      if (! found)
      {
        DCMPSTAT_INFO("cannot update Basic Film Box: illegal magnification type: '" << theMagnification.c_str() << "'");
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      } else {
        ADD_TO_PDATASET(DcmCodeString, magnificationType)
      }
    }
  }

  // smoothingType
  if (result)
  {

    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)smoothingType.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      smoothingType = *((DcmCodeString *)(stack.top()));
      Uint32 numSmoothings = cfg.getTargetPrinterNumberOfSmoothingTypes(cfgname);
      OFString theSmoothing;
      OFString aString;
      OFBool found = OFFalse;
      smoothingType.getOFString(theSmoothing, 0, OFTrue);
      for (Uint32 i=0; i<numSmoothings; i++)
      {
        cfg.getTargetPrinterSmoothingType(cfgname, i, aString);
        if (theSmoothing == aString)
        {
          found = OFTrue;
          break;
        }
      }
      if (numSmoothings == 0)
      {
        DCMPSTAT_INFO("cannot update Basic Film Box: smoothing type requested but not supported.");
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      }
      else if (! found)
      {
        DCMPSTAT_INFO("cannot update Basic Film Box: illegal smoothing type: '" << theSmoothing.c_str() << "'");
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
      else
      {
        ADD_TO_PDATASET(DcmCodeString, smoothingType)
      }
    }
  }


  // borderDensity
  if (result)
  {

    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)borderDensity.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      borderDensity = *((DcmCodeString *)(stack.top()));
      Uint32 numBorderDensities = cfg.getTargetPrinterNumberOfBorderDensities(cfgname);
      if (numBorderDensities == 0) // we don't support border density
      {
        DCMPSTAT_INFO("cannot update Basic Film Box: border density requested but not supported.");
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      } else {
        OFString theBorderDensity;
        OFString aString;
        OFBool found = OFFalse;
        OFBool supportsNumericDensity = OFFalse;
        unsigned long l;
        borderDensity.getOFString(theBorderDensity, 0, OFTrue);
        for (Uint32 i=0; i<numBorderDensities; i++)
        {
          cfg.getTargetPrinterBorderDensity(cfgname, i, aString);
          if (theBorderDensity == aString)
          {
            found = OFTrue;
            break;
          } else {
            if (1 == sscanf(aString.c_str(), "%lu", &l)) supportsNumericDensity = OFTrue;
          }
        }
        if ((! found) && supportsNumericDensity)
        {
          // the density was not found in the list; check whether it is numerical
          if (1 == sscanf(theBorderDensity.c_str(), "%lu", &l)) found = OFTrue;
        }

        if (! found)
        {
          DCMPSTAT_INFO("cannot update Basic Film Box: illegal border density: '" << theBorderDensity.c_str() << "'");
          rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
          result = OFFalse;
        }
        else
        {
          ADD_TO_PDATASET(DcmCodeString, borderDensity)
        }
      }
    }
  }

  // emptyImageDensity
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)emptyImageDensity.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      emptyImageDensity = *((DcmCodeString *)(stack.top()));
      Uint32 numEmptyImageDensities = cfg.getTargetPrinterNumberOfEmptyImageDensities(cfgname);
      if (numEmptyImageDensities == 0) // we don't support empty image density
      {
        DCMPSTAT_INFO("cannot update Basic Film Box: empty image density requested but not supported.");
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      } else {
        OFString theEIDensity;
        OFString aString;
        OFBool found = OFFalse;
        OFBool supportsNumericDensity = OFFalse;
        unsigned long l;
        emptyImageDensity.getOFString(theEIDensity, 0, OFTrue);
        for (Uint32 i=0; i<numEmptyImageDensities; i++)
        {
          cfg.getTargetPrinterEmptyImageDensity(cfgname, i, aString);
          if (theEIDensity == aString)
          {
            found = OFTrue;
            break;
          } else {
            if (1 == sscanf(aString.c_str(), "%lu", &l)) supportsNumericDensity = OFTrue;
          }
        }
        if ((! found) && supportsNumericDensity)
        {
          // the density was not found in the list; check whether it is numerical
          if (1 == sscanf(theEIDensity.c_str(), "%lu", &l)) found = OFTrue;
        }

        if (! found)
        {
          DCMPSTAT_INFO("cannot update Basic Film Box: illegal empty image density: '" << theEIDensity.c_str() << "'");
          rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
          result = OFFalse;
        }
        else
        {
          ADD_TO_PDATASET(DcmCodeString, emptyImageDensity)
        }
      }
    }
  }

  // maxDensity
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)maxDensity.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      maxDensity = *((DcmUnsignedShort *)(stack.top()));
      // we don't check a max density set by the user (for now)
      ADD_TO_PDATASET(DcmUnsignedShort, maxDensity)
    }
  }

  // minDensity
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)minDensity.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      minDensity = *((DcmUnsignedShort *)(stack.top()));
      Uint32 numMinDensities = cfg.getTargetPrinterNumberOfMinDensities(cfgname);
      if (numMinDensities == 0) // we don't support min density
      {
        DCMPSTAT_INFO("cannot update Basic Film Box: min density requested but not supported.");
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      }
      else
      {
        // we don't check a min density set by the user (for now)
        ADD_TO_PDATASET(DcmUnsignedShort, minDensity)
      }
    }
  }

  // trim
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)trim.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      trim = *((DcmCodeString *)(stack.top()));

      if (cfg.getTargetPrinterSupportsTrim(cfgname))
      {
        OFString aString;
        trim.getOFString(aString, 0, OFTrue);
        if ((aString != "YES")&&(aString != "NO"))
        {
          DCMPSTAT_INFO("cannot update Basic Film Box: illegal trim: '" << aString.c_str() << "'");
          rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
          result = OFFalse;
        } else {
          ADD_TO_PDATASET(DcmCodeString, trim)
        }
      } else {
        DCMPSTAT_INFO("cannot update Basic Film Box: trim requested but not supported.");
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      }
    }
  }

  // configurationInformation
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)configurationInformation.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      configurationInformation = *((DcmShortText *)(stack.top()));
      Uint32 numConfigurationInformation = cfg.getTargetPrinterNumberOfConfigurationSettings(cfgname);
      if (numConfigurationInformation == 0) // we don't support configuration information
      {
        DCMPSTAT_INFO("cannot update Basic Film Box: configuration information requested but not supported.");
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      } else {
        OFString theConfiguration;
        OFBool found = OFFalse;
        configurationInformation.getOFString(theConfiguration, 0, OFTrue);
        for (Uint32 i=0; i<numConfigurationInformation; i++)
        {
          if (theConfiguration ==  cfg.getTargetPrinterConfigurationSetting(cfgname, i))
          {
            found = OFTrue;
            break;
          }
        }
        if (! found)
        {
          DCMPSTAT_INFO("cannot update Basic Film Box: illegal configuration information: '" << theConfiguration.c_str() << "'");
          rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
          result = OFFalse;
        }
        else
        {
          ADD_TO_PDATASET(DcmShortText, configurationInformation)
        }
      }
    }
  }

  if (presentationLUTnegotiated)
  {

    // illumination
    if (result)
    {
      stack.clear();
      if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)illumination.getTag(), stack, ESM_fromHere, OFFalse)))
      {
        illumination = *((DcmUnsignedShort *)(stack.top()));
        // we don't check illumination set by the user (for now)
        ADD_TO_PDATASET(DcmUnsignedShort, illumination)
      }
    }

    // reflectedAmbientLight
    if (result)
    {
      stack.clear();
      if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)reflectedAmbientLight.getTag(), stack, ESM_fromHere, OFFalse)))
      {
        reflectedAmbientLight = *((DcmUnsignedShort *)(stack.top()));
        // we don't check reflected ambient light set by the user (for now)
        ADD_TO_PDATASET(DcmUnsignedShort, reflectedAmbientLight)
      }
    }

    // referenced presentation LUT sequence
    if (result)
    {
      stack.clear();

      if (rqDataset && (EC_Normal == rqDataset->search(DCM_ReferencedPresentationLUTSequence, stack, ESM_fromHere, OFFalse)))
      {
        DcmSequenceOfItems *seq=(DcmSequenceOfItems *)stack.top();
        if (seq->card() ==1)
        {
           OFString aString;
           DcmItem *item = seq->getItem(0);
           stack.clear();
           READ_FROM_DATASET2(DcmUniqueIdentifier, referencedPresentationLUTInstanceUID)
           if (referencedPresentationLUTInstanceUID.getLength() > 0)
           {
             referencedPresentationLUTInstanceUID.getOFString(aString,0);
             DVPSPresentationLUT *currentPLUT = globalPresentationLUTList.findPresentationLUT(aString.c_str());
             if (NULL == currentPLUT)
             {
               DCMPSTAT_INFO("cannot update Basic Film Box: presentation LUT reference cannot be resolved");
               rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
               result = OFFalse;
             } else {
               // check referenced SOP class UID
               DcmUniqueIdentifier refClassUID(DCM_ReferencedSOPClassUID);
               stack.clear();
               READ_FROM_DATASET2(DcmUniqueIdentifier, refClassUID)
               if (refClassUID.getLength() > 0)
               {
                  aString.clear();
                  refClassUID.getOFString(aString,0, OFTrue);
                  if (aString != UID_PresentationLUTSOPClass)
                  {
                    DCMPSTAT_INFO("cannot update Basic Film Box: referenced SOP class UID in referenced presentation LUT sequence incorrect:\n"
                        << DcmObject::PrintHelper(*stack.top(), DCMTypes::PF_shortenLongTagValues));
                    rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
                    result = OFFalse;
                  } else {
                    referencedPresentationLUTAlignment = currentPLUT->getAlignment();
                    if ((cfg.getTargetPrinterPresentationLUTMatchRequired(cfgname)) &&
                        (! imageBoxContentList.matchesPresentationLUT(referencedPresentationLUTAlignment)))
                    {
                      DCMPSTAT_INFO("cannot update Basic Film Box: referenced presentation LUT number of entries does not match image bit depth.");
                      rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
                      result = OFFalse;
                    } else {

                      // referenced presentation LUT sequence is OK
                      // synchronize presentationLUTInstanceUID and referencedPresentationLUTInstanceUID
                      presentationLUTInstanceUID.clear();
                      referencedPresentationLUTInstanceUID.getOFString(presentationLUTInstanceUID,0);
                      DcmSequenceOfItems *newSeq = new DcmSequenceOfItems(*seq);
                      if (newSeq) rspDataset->insert(newSeq, OFTrue /*replaceOld*/);
                      else
                      {
                        writeresult = EC_MemoryExhausted;
                      }
                    }
                  }
               } else {
                  DCMPSTAT_INFO("cannot update Basic Film Box: no referenced SOP class UID in referenced presentation LUT sequence");
                  rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
                  result = OFFalse;
               }
             }
           } else {
             DCMPSTAT_INFO("cannot update Basic Film Box: no referenced SOP instance UID in referenced presentation LUT sequence");
             rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
             result = OFFalse;
           }
        } else {
          DCMPSTAT_INFO("cannot update Basic Film Box: referenced presentation LUT sequence number of items != 1");
          rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
          result = OFFalse;
        }
      }
    }

  } /* if presentationLUTnegotiated */


  // browse through rqDataset and check for unsupported attributes
  if (result && rqDataset)
  {
    OFBool intoSub = OFTrue;
    stack.clear();
    while (EC_Normal == rqDataset->nextObject(stack, intoSub))
    {
      intoSub = OFFalse;
      const DcmTagKey& currentTag = (stack.top())->getTag();
      if (currentTag.getElement() == 0x0000) /* group length */ ;
      else if (currentTag == DCM_MagnificationType) /* OK */ ;
      else if (currentTag == DCM_SmoothingType) /* OK */ ;
      else if (currentTag == DCM_BorderDensity) /* OK */ ;
      else if (currentTag == DCM_EmptyImageDensity) /* OK */ ;
      else if (currentTag == DCM_MinDensity) /* OK */ ;
      else if (currentTag == DCM_MaxDensity) /* OK */ ;
      else if (currentTag == DCM_Trim) /* OK */ ;
      else if (currentTag == DCM_ConfigurationInformation) /* OK */ ;
      else if (currentTag == DCM_ReferencedFilmSessionSequence) /* OK */ ;
      else if (currentTag == DCM_Illumination)
      {
        if (! presentationLUTnegotiated)
        {
          DCMPSTAT_INFO("cannot update Basic Film Box: illumination received:\n"
              << DcmObject::PrintHelper(*stack.top(), DCMTypes::PF_shortenLongTagValues));
          rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;
          result = OFFalse;
        }
      }
      else if (currentTag == DCM_ReflectedAmbientLight)
      {
        if (! presentationLUTnegotiated)
        {
          DCMPSTAT_INFO("cannot update Basic Film Box: reflected ambient light received:\n"
              << DcmObject::PrintHelper(*stack.top(), DCMTypes::PF_shortenLongTagValues));
          rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;
          result = OFFalse;
        }
      }
      else if (currentTag == DCM_ReferencedPresentationLUTSequence)
      {
        if (! presentationLUTnegotiated)
        {
          DCMPSTAT_INFO("cannot update Basic Film Box: referenced presentation LUT sequence received:\n"
              << DcmObject::PrintHelper(*stack.top(), DCMTypes::PF_shortenLongTagValues));
          rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;
          result = OFFalse;
        }
      }
      else
      {
        DCMPSTAT_INFO("cannot update Basic Film Box: unsupported attribute received:"
            << DcmObject::PrintHelper(*stack.top(), DCMTypes::PF_shortenLongTagValues));
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      }
    }
  }

  // if n-set was successful, send back response dataset
  if (result && (EC_Normal == writeresult))
  {
    rsp.msg.NSetRSP.DataSetType = DIMSE_DATASET_PRESENT;
  } else {
    delete rspDataset;
    rspDataset = NULL;
    if (rsp.msg.NSetRSP.DimseStatus == 0) rsp.msg.NSetRSP.DimseStatus = STATUS_N_ProcessingFailure;
    result = OFFalse;
  }
  return result;
}


OFBool DVPSStoredPrint::usesPresentationLUT(const char *c)
{
  char *plut = NULL;
  if (c && (EC_Normal == referencedPresentationLUTInstanceUID.getString(plut)) && plut)
  {
    OFString aString(plut);
    if (aString == c) return OFTrue;
  }
  return OFFalse;
}

void DVPSStoredPrint::updatePresentationLUTList(DVPSPresentationLUT_PList& globalPresentationLUTList)
{
  presentationLUTList.clear();
  if (referencedPresentationLUTInstanceUID.getLength() > 0)
  {
    OFString aString;
    referencedPresentationLUTInstanceUID.getOFString(aString,0);
    DVPSPresentationLUT *currentPLUT = globalPresentationLUTList.findPresentationLUT(aString.c_str());
    if (currentPLUT) presentationLUTList.insert(currentPLUT->clone());
    else
    {
      referencedPresentationLUTInstanceUID.clear();
      presentationLUTInstanceUID.clear();
    }
  }
}

void DVPSStoredPrint::overridePresentationLUTSettings(
      DcmUnsignedShort& newIllumination,
      DcmUnsignedShort& newReflectedAmbientLight,
      DcmUniqueIdentifier& newReferencedPLUT,
      DVPSPrintPresentationLUTAlignment newAlignment)
{
  illumination = newIllumination;
  reflectedAmbientLight = newReflectedAmbientLight;
  referencedPresentationLUTInstanceUID = newReferencedPLUT;
  presentationLUTInstanceUID.clear();
  referencedPresentationLUTInstanceUID.getOFString(presentationLUTInstanceUID,0);
  referencedPresentationLUTAlignment = newAlignment;
}

/*
 *  $Log: dvpssp.cc,v $
 *  Revision 1.59  2010-10-14 13:14:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.58  2010-09-24 13:32:58  joergr
 *  Compared names of SOP Class UIDs with 2009 edition of the DICOM standard. The
 *  resulting name changes are mainly caused by the fact that the corresponding
 *  SOP Class is now retired.
 *
 *  Revision 1.57  2010-08-09 13:21:57  joergr
 *  Updated data dictionary to 2009 edition of the DICOM standard. From now on,
 *  the official "keyword" is used for the attribute name which results in a
 *  number of minor changes (e.g. "PatientsName" is now called "PatientName").
 *
 *  Revision 1.56  2009-12-15 14:39:37  uli
 *  Fixed an unsafe usage of OFString.
 *
 *  Revision 1.55  2009-11-24 14:12:59  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.54  2008-04-30 12:38:43  meichel
 *  Fixed compile errors due to changes in attribute tag names
 *
 *  Revision 1.53  2006/08/15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.52  2005/12/08 15:46:47  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.51  2003/06/04 12:30:28  meichel
 *  Added various includes needed by MSVC5 with STL
 *
 *  Revision 1.50  2003/04/14 14:28:05  meichel
 *  Added explicit typecasts in calls to pow(). Needed by Visual C++ .NET 2003.
 *
 *  Revision 1.49  2003/03/12 17:34:22  meichel
 *  Updated DcmObject::print() flags
 *
 *  Revision 1.48  2002/11/27 15:48:13  meichel
 *  Adapted module dcmpstat to use of new header file ofstdinc.h
 *
 *  Revision 1.47  2002/01/08 10:39:56  joergr
 *  Corrected spelling of function dcmGenerateUniqueIdentifier().
 *  Changed prefix of UIDs created for series and studies (now using constants
 *  SITE_SERIES_UID_ROOT and SITE_STUDY_UID_ROOT which are supposed to be used
 *  in these cases).
 *
 *  Revision 1.46  2001/11/28 13:57:01  joergr
 *  Check return value of DcmItem::insert() statements where appropriate to
 *  avoid memory leaks when insert procedure fails.
 *
 *  Revision 1.45  2001/10/12 13:46:56  meichel
 *  Adapted dcmpstat to OFCondition based dcmnet module (supports strict mode).
 *
 *  Revision 1.44  2001/09/28 13:50:54  joergr
 *  Changed formatting.
 *
 *  Revision 1.43  2001/09/26 15:36:32  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.42  2001/06/07 14:31:35  joergr
 *  Removed unused variable (reported by gcc 2.5.8 on NeXTSTEP).
 *
 *  Revision 1.41  2001/06/01 15:50:37  meichel
 *  Updated copyright header
 *
 *  Revision 1.40  2001/05/25 10:07:58  meichel
 *  Corrected some DIMSE error status codes for Print SCP
 *
 *  Revision 1.39  2000/09/06 08:55:39  meichel
 *  Updated Print SCP to accept and silently ignore group length attributes.
 *
 *  Revision 1.38  2000/07/18 16:06:27  joergr
 *  Moved method convertODtoLum/PValue from class DVInterface to DVPSStoredPrint
 *  and corrected implementation.
 *  Changed behaviour of methods getMin/MaxDensityValue (return default value if
 *  attribute empty/absent).
 *
 *  Revision 1.37  2000/07/12 16:39:42  meichel
 *  Print SCP now writes PrinterCharacteristicsSequence when saving Stored Prints.
 *
 *  Revision 1.36  2000/07/07 14:15:15  joergr
 *  Added support for LIN OD presentation LUT shape.
 *
 *  Revision 1.35  2000/07/04 16:06:48  joergr
 *  Added support for overriding the presentation LUT settings made for the
 *  image boxes.
 *
 *  Revision 1.34  2000/06/20 14:50:09  meichel
 *  Added monochrome1 printing mode.
 *
 *  Revision 1.33  2000/06/19 16:29:08  meichel
 *  Added options for session printing and LIN OD to print tools, fixed
 *    pixel aspect ratio related bug.
 *
 *  Revision 1.32  2000/06/08 10:44:37  meichel
 *  Implemented Referenced Presentation LUT Sequence on Basic Film Session level.
 *    Empty film boxes (pages) are not written to file anymore.
 *
 *  Revision 1.31  2000/06/07 14:27:50  joergr
 *  Added missing transformations (polarity, GSDF, presentation LUT, aspect
 *  ratio) to print preview rendering.
 *
 *  Revision 1.30  2000/06/07 13:17:08  meichel
 *  now using DIMSE status constants and log facilities defined in dcmnet
 *
 *  Revision 1.29  2000/06/02 16:01:06  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.28  2000/06/02 12:46:43  joergr
 *  Corrected handling of optional parameters in method newPrinter().
 *
 *  Revision 1.27  2000/05/31 12:58:16  meichel
 *  Added initial Print SCP support
 *
 *  Revision 1.26  2000/05/31 07:56:22  joergr
 *  Added support for Stored Print attributes Originator and Destination
 *  application entity title.
 *
 *  Revision 1.25  2000/05/30 13:57:51  joergr
 *  Added methods to set, get and store the printer name in the stored print
 *  object (PrinterCharacteristicsSequence).
 *
 *  Revision 1.24  2000/03/08 16:29:10  meichel
 *  Updated copyright header.
 *
 *  Revision 1.23  2000/03/07 16:24:01  joergr
 *  Added explicit type casts to make Sun CC 2.0.1 happy.
 *
 *  Revision 1.22  2000/03/03 14:14:05  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.21  2000/02/29 12:16:20  meichel
 *  Fixed bug in dcmpstat library that caused Monochrome1 images
 *    to be printed inverse if a Presentation LUT was applied.
 *
 *  Revision 1.20  1999/11/19 10:58:40  meichel
 *  Fixed bug in dcmpstat module that prevented printing of images
 *
 *  Revision 1.19  1999/10/19 14:48:26  meichel
 *  added support for the Basic Annotation Box SOP Class
 *    as well as access methods for Max Density and Min Density.
 *
 *  Revision 1.18  1999/10/13 14:11:14  meichel
 *  Fixed bug in routine that renders P-LUTs into a print bitmap
 *    before sending an image to the printer
 *
 *  Revision 1.17  1999/10/07 17:22:01  meichel
 *  Reworked management of Presentation LUTs in order to create tighter
 *    coupling between Softcopy and Print.
 *
 *  Revision 1.16  1999/09/24 15:24:34  meichel
 *  Added support for CP 173 (Presentation LUT clarifications)
 *
 *  Revision 1.15  1999/09/17 14:33:54  meichel
 *  Completed print spool functionality including Supplement 22 support
 *
 *  Revision 1.14  1999/09/15 17:43:36  meichel
 *  Implemented print job dispatcher code for dcmpstat, adapted dcmprtsv
 *    and dcmpsprt applications.
 *
 *  Revision 1.13  1999/09/13 15:19:16  meichel
 *  Added implementations for a number of further print API methods.
 *
 *  Revision 1.12  1999/09/10 12:46:57  meichel
 *  Added implementations for a number of print API methods.
 *
 *  Revision 1.11  1999/09/10 07:32:43  thiel
 *  Added Presentation LUT Shape LIN OD
 *
 *  Revision 1.10  1999/09/09 14:57:50  thiel
 *  Added methods for print spooler
 *
 *  Revision 1.9  1999/09/09 12:20:53  meichel
 *  Added print API method declarations and implementations (empty for now).
 *
 *  Revision 1.8  1999/09/08 16:46:32  meichel
 *  Added print API method declarations
 *
 *  Revision 1.7  1999/09/01 16:15:09  meichel
 *  Added support for requested image size to print routines
 *
 *  Revision 1.6  1999/08/31 16:54:47  meichel
 *  Added new sample application that allows to create simple print jobs.
 *
 *  Revision 1.5  1999/08/31 14:09:28  meichel
 *  Added get/set methods for stored print attributes
 *
 *  Revision 1.4  1999/08/27 15:57:50  meichel
 *  Added methods for saving hardcopy images and stored print objects
 *    either in file or in the local database.
 *
 *  Revision 1.3  1999/08/26 09:29:49  thiel
 *  Extensions for the usage of the StoredPrint
 *
 *  Revision 1.2  1999/08/25 16:56:14  joergr
 *  Added '#include <time.h>' to make MSVC happy.
 *
 *  Revision 1.1  1999/07/30 13:34:59  meichel
 *  Added new classes managing Stored Print objects
 *
 *
 */

