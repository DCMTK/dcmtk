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
 *    classes: DVPSStoredPrint
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-10-13 14:11:14 $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dvpssp.h"
#include "dcuid.h"
#include "dvpsib.h"
#include "dvpshlp.h"
#include "dcmimage.h"

#ifdef HAVE_TIME_H
#include <time.h>
#endif


/* some defaults for creating Stored Print objects */
#define DEFAULT_imageDisplayFormat        "STANDARD\\1,1"
#define DEFAULT_patientName               "^^^^"

/* --------------- a few macros avoiding copy/paste --------------- */

#define SET_UID(a_name)                                             \
if (result==EC_Normal)                                              \
{                                                                   \
     if (a_name.getLength()==0) result = a_name.putString(dcmGenerateUniqueIdentifer(uid)); \
}
#undef ADD_TO_DATASET
#define ADD_TO_DATASET(a_type, a_name)                              \
if (result==EC_Normal)                                              \
{                                                                   \
  delem = new a_type(a_name);                                       \
  if (delem) dset.insert(delem); else result=EC_MemoryExhausted;    \
}

#define ADD_TO_DATASET2(a_type, a_name)                             \
if (result==EC_Normal)                                              \
{                                                                   \
  delem = new a_type(a_name);                                       \
  if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;  \
}

#define READ_FROM_DATASET(a_type, a_name)                           \
stack.clear();                                                      \
if (EC_Normal == dset.search((DcmTagKey &)a_name.getTag(), stack, ESM_fromHere, OFFalse)) \
{                                                                   \
  a_name = *((a_type *)(stack.top()));     \
}

#define READ_FROM_DATASET2(a_type, a_name)                           \
stack.clear();                                                      \
if (EC_Normal == item->search((DcmTagKey &)a_name.getTag(), stack, ESM_fromHere, OFFalse)) \
{                                                                   \
  a_name = *((a_type *)(stack.top()));                              \
}

/* --------------- class DVPSStoredPrint --------------- */

DVPSStoredPrint::DVPSStoredPrint(Uint16 illumin, Uint16 reflection)
: patientName(DCM_PatientsName)
, patientID(DCM_PatientID)
, patientBirthDate(DCM_PatientsBirthDate)
, patientSex(DCM_PatientsSex)
, studyInstanceUID(DCM_StudyInstanceUID)
, studyDate(DCM_StudyDate)
, studyTime(DCM_StudyTime)
, referringPhysiciansName(DCM_ReferringPhysiciansName)
, studyID(DCM_StudyID)
, accessionNumber(DCM_AccessionNumber)
, seriesInstanceUID(DCM_SeriesInstanceUID)
, seriesNumber(DCM_SeriesNumber)
, manufacturer(DCM_Manufacturer)
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
, imageBoxContentList()
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
, transmitImagesIn12Bit(OFTrue)
, renderPresentationLUTinSCP(OFFalse)
, logstream(&cerr)
{
  illumination.putUint16(illumin,0);
  reflectedAmbientLight.putUint16(reflection,0);
}

DVPSStoredPrint::DVPSStoredPrint(const DVPSStoredPrint& copy)
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
, imageBoxContentList(copy.imageBoxContentList)
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
, transmitImagesIn12Bit(copy.transmitImagesIn12Bit)
, renderPresentationLUTinSCP(copy.renderPresentationLUTinSCP)
, logstream(copy.logstream)
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
  referringPhysiciansName.clear();
  studyID.clear();
  accessionNumber.clear();
  seriesInstanceUID.clear();
  seriesNumber.clear();
  manufacturer.clear();
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
  imageBoxContentList.clear();
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
  transmitImagesIn12Bit = OFTrue;
  renderPresentationLUTinSCP = OFFalse;
  // we don't change the log stream
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
    char format[30];
    aString.copy(format,OFString_npos,9);
    if (2==sscanf(format, "%ld,%ld", &columns, &rows))
    {
      currentNumCols = columns;
      currentNumRows = rows;
      currentValuesValid = OFTrue;
    } else {
#ifdef DEBUG
      *logstream << "Warning: cannot parse image display format '" << aString.c_str() << "'" << endl;
#endif      
    }
  } else {
#ifdef DEBUG
    *logstream << "Warning: unknown image display format '" << aString.c_str() << "'" << endl;
#endif      
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

  
E_Condition DVPSStoredPrint::read(DcmItem &dset)
{
  DcmSequenceOfItems *seq;
  DcmItem *item;
  E_Condition result = EC_Normal;
  DcmStack stack;
  OFString aString;
  OFString aString2;

  clear(); // re-initialize Stored Print object 
    
  DcmUniqueIdentifier sopclassuid(DCM_SOPClassUID);  
  DcmUniqueIdentifier refsopclassuid(DCM_ReferencedSOPClassUID);  
  READ_FROM_DATASET(DcmUniqueIdentifier, sopclassuid)
  sopclassuid.getOFString(aString,0);
  if (aString != UID_StoredPrintStorage)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    *logstream << "Error: SOP Class UID does not match StoredPrintStorage" << endl;
#endif
  }

  DcmCodeString modality(DCM_Modality);
  READ_FROM_DATASET(DcmCodeString, modality)
  if (modality.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    *logstream << "Error: Modality missing or empty in Stored Print" << endl;
#endif
  }

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
  READ_FROM_DATASET(DcmIntegerString, instanceNumber) 
  READ_FROM_DATASET(DcmUniqueIdentifier, sOPInstanceUID)
  READ_FROM_DATASET(DcmCodeString, specificCharacterSet)
  READ_FROM_DATASET(DcmDate, instanceCreationDate)
  READ_FROM_DATASET(DcmTime, instanceCreationTime)

  if (EC_Normal==result) result = presentationLUTList.read(dset);

  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_FilmBoxContentSequence, stack, ESM_fromHere, OFFalse))
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
         if (EC_Normal == dset.search(DCM_ReferencedPresentationLUTSequence, stack, ESM_fromHere, OFFalse))
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
#ifdef DEBUG
                  *logstream << "Error: FilmBoxContentSequence presentation LUT reference cannot be resolved" << endl;
#endif
                }
              }
           } else {
             result=EC_TagNotFound;
#ifdef DEBUG
             *logstream << "Error: found FilmBoxContentSequence in Stored Print with ReferencedPresentationLUTSequence number of items != 1" << endl;
#endif
           }
         }
      } else {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: found FilmBoxContentSequence in Stored Print with number of items != 1" << endl;
#endif
      } 
    }
  }

  if (EC_Normal==result) result = imageBoxContentList.read(dset, presentationLUTList);
       
  /* Now perform basic sanity checks */

  if (result==EC_Normal)
  {
    if ((studyInstanceUID.getLength() == 0)||(studyInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: StudyInstanceUID missing or incorrect in Stored Print" << endl;
#endif
    }
    if ((seriesInstanceUID.getLength() == 0)||(seriesInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: SeriesInstanceUID missing or incorrect in Stored Print" << endl;
#endif
    }
    if ((sOPInstanceUID.getLength() == 0)||(sOPInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: SOPInstanceUID missing or incorrect in Stored Print" << endl;
#endif
    }
    if ((patientName.getLength() == 0)||(patientName.getVM() != 1))
    {
       // result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Warning: PatientsName missing or incorrect in Stored Print" << endl;
#endif
    }
    if ((imageDisplayFormat.getLength() == 0)||(imageDisplayFormat.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: ImageDisplayFormat missing or incorrect in Stored Print" << endl;
#endif
    }
    if (imageBoxContentList.size() == 0)
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: ImageBoxContentSequence missing or empty in Stored Print" << endl;
#endif
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
   */
  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_PrintManagementCapabilitiesSequence, stack, ESM_fromHere, OFFalse))
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
               ||(aString == UID_PresentationLUTSOPClass)) { }
             else 
               if (isImageStorageSOPClass(aString)) 
                 haveImageStorage=OFTrue;
               else 
                 { 
                   result=EC_IllegalCall;
        #ifdef DEBUG
                   *logstream << "Error: Unsupported SOP Class UID in PrintManagementCapabilitiesSequence" << endl;
        #endif
                 }
              
      }
      if (EC_Normal==result)
      {
        if (! haveFilmBox)
        {
           result=EC_IllegalCall;
#ifdef DEBUG
           *logstream << "Error: Film Box SOP Class not referenced in PrintManagementCapabilitiesSequence" << endl;
#endif
        }
        if (! haveGrayscaleImageBox)
        {
           result=EC_IllegalCall;
#ifdef DEBUG
           *logstream << "Error: Basic Grayscale Image Box SOP Class not referenced in PrintManagementCapabilitiesSequence" << endl;
#endif
        }
        if (! haveImageStorage)
        {
           result=EC_IllegalCall;
#ifdef DEBUG
           *logstream << "Error: No Image Storage SOP Class referenced in PrintManagementCapabilitiesSequence" << endl;
#endif
        }
      }
    } else {
      result=EC_TagNotFound;
#ifdef DEBUG
      *logstream << "Error: PrintManagementCapabilitiesSequence not found" << endl;
#endif
    } 
  }
  return result;
}


E_Condition DVPSStoredPrint::createDefaultValues()
{
  E_Condition result = EC_Normal;
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
    result = sOPInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
    DVPSHelper::currentDate(aString);
    if (result==EC_Normal) result = instanceCreationDate.putString(aString.c_str());
    DVPSHelper::currentTime(aString);
    if (result==EC_Normal) result = instanceCreationTime.putString(aString.c_str());
  }

  if ((result==EC_Normal)&&(studyInstanceUID.getLength()==0))
  {
    result = studyInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
    DVPSHelper::currentDate(aString);
    if (result==EC_Normal) result = studyDate.putString(aString.c_str());
    DVPSHelper::currentTime(aString);
    if (result==EC_Normal) result = studyTime.putString(aString.c_str());
  }

  if ((result==EC_Normal)&&(imageDisplayFormat.getLength()==0)) result = imageDisplayFormat.putString(DEFAULT_imageDisplayFormat);
  return result;
}

E_Condition DVPSStoredPrint::write(DcmItem &dset, OFBool writeRequestedImageSize, OFBool limitImages)
{
  DcmElement *delem=NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;
  
  E_Condition result = createDefaultValues();
  if (EC_Normal==result) result = imageBoxContentList.setRequestedDecimateCropBehaviour(decimateCropBehaviour); // set in all image boxes
  if (EC_Normal==result) result = imageBoxContentList.createDefaultValues(limitImages); // renumber if limitImages is true
   
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
      dseq = new DcmSequenceOfItems(DCM_FilmBoxContentSequence);
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
        if (presentationLUTList.size() > 0)
        {
          ADD_TO_DATASET2(DcmUnsignedShort, illumination)
          ADD_TO_DATASET2(DcmUnsignedShort, reflectedAmbientLight)
          if (EC_Normal == result) result = addReferencedPLUTSQ(*ditem);
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

  /* add SOP Class UID */  
  DcmUniqueIdentifier sopclassuid(DCM_SOPClassUID);
  if (result==EC_Normal)
  {
     result = sopclassuid.putString(UID_StoredPrintStorage);
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
  if (EC_Normal == result) result = presentationLUTList.write(dset);
 
  // write imageBoxContentList
  if (EC_Normal == result) result = imageBoxContentList.write(dset, writeRequestedImageSize, writeImageBoxes);

  // write PrintManagementCapabilitiesSequence
  dseq = new DcmSequenceOfItems(DCM_PrintManagementCapabilitiesSequence);
  if (dseq)
  {
    if (EC_Normal == result) result = DVPSHelper::addReferencedUIDItem(*dseq, UID_BasicFilmSessionSOPClass);
    if (EC_Normal == result) result = DVPSHelper::addReferencedUIDItem(*dseq, UID_BasicFilmBoxSOPClass);
    if (EC_Normal == result) result = DVPSHelper::addReferencedUIDItem(*dseq, UID_BasicGrayscaleImageBoxSOPClass);
    if (EC_Normal == result) result = imageBoxContentList.addImageSOPClasses(*dseq, writeImageBoxes);
    if ((result == EC_Normal)&&(presentationLUTList.size() > 0))
    {
      result = DVPSHelper::addReferencedUIDItem(*dseq, UID_PresentationLUTSOPClass);
    }
    if (result==EC_Normal) dset.insert(dseq); else delete dseq;
  } else result = EC_MemoryExhausted;


  return result;
}


E_Condition DVPSStoredPrint::writeHardcopyImageAttributes(DcmItem &dset)
{
  DcmElement *delem=NULL;
  E_Condition result = createDefaultValues();

  // add general study module
  ADD_TO_DATASET(DcmUniqueIdentifier, studyInstanceUID)
  ADD_TO_DATASET(DcmDate, studyDate)
  ADD_TO_DATASET(DcmTime, studyTime)
  ADD_TO_DATASET(DcmPersonName, referringPhysiciansName)
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


E_Condition DVPSStoredPrint::addImageBox(
  const char *retrieveaetitle,
  const char *refstudyuid,
  const char *refseriesuid,
  const char *refsopclassuid,
  const char *refsopinstanceuid,
  const char *requestedimagesize,
  const char *patientid,
  DVPSPresentationLUT *presentationlut)
{  
  char instanceuid[100];
  const char *lutUID = presentationLUTList.addPresentationLUT(presentationlut);
  return imageBoxContentList.addImageBox(dcmGenerateUniqueIdentifer(instanceuid), 
     retrieveaetitle, refstudyuid, refseriesuid, refsopclassuid, 
     refsopinstanceuid, requestedimagesize, patientid, lutUID);
}

E_Condition DVPSStoredPrint::addImageBox(
  const char *retrieveaetitle,
  const char *refsopinstanceuid,
  const char *requestedimagesize,
  const char *patientid,
  DVPSPresentationLUT *presentationlut)
{
  char *refstudyuid=NULL;
  char *refseriesuid=NULL;

  createDefaultValues(); // make sure that all UIDs are defined  
  studyInstanceUID.getString(refstudyuid); // same study UID for stored print and hardcopy image
  imageSeriesInstanceUID.getString(refseriesuid); // but separate series for the hardcopy images

  return addImageBox(retrieveaetitle, refstudyuid, refseriesuid, UID_HardcopyGrayscaleImageStorage,
     refsopinstanceuid, requestedimagesize, patientid, presentationlut);
}

E_Condition DVPSStoredPrint::setInstanceUID(const char *uid)
{
  if ((uid==NULL)||(strlen(uid)==0)) return EC_IllegalCall;
  return sOPInstanceUID.putString(uid);
}

E_Condition DVPSStoredPrint::setImageDisplayFormat(unsigned long columns, unsigned long rows)
{
  if ((columns==0)||(rows==0)) return EC_IllegalCall;
  char newFormat[80];
  sprintf(newFormat, "STANDARD\\%ld,%ld", columns, rows);
  
  E_Condition result = imageDisplayFormat.putString(newFormat);
  if (EC_Normal == result)
  {
    currentNumCols = columns;
    currentNumRows = rows;
    currentValuesValid = OFTrue;
  } else invalidateCache();
  return result;
}

E_Condition DVPSStoredPrint::setFilmSizeID(const char *value)
{
  if ((value==NULL)||(strlen(value)==0)) 
  {
    filmSizeID.clear();
    return EC_Normal;
  }
  return filmSizeID.putString(value);
}

E_Condition DVPSStoredPrint::setMagnificationType(const char *value)
{
  if ((value==NULL)||(strlen(value)==0)) 
  {
    magnificationType.clear();
    return EC_Normal;
  }
  return magnificationType.putString(value);
}

E_Condition DVPSStoredPrint::setSmoothingType(const char *value)
{
  if ((value==NULL)||(strlen(value)==0)) 
  {
    smoothingType.clear();
    return EC_Normal;
  }
  return smoothingType.putString(value);
}

E_Condition DVPSStoredPrint::setConfigurationInformation(const char *value)
{
  if ((value==NULL)||(strlen(value)==0)) 
  {
    configurationInformation.clear();
    return EC_Normal;
  }
  return configurationInformation.putString(value);
}

E_Condition DVPSStoredPrint::setResolutionID(const char *value)
{
  if ((value==NULL)||(strlen(value)==0)) 
  {
    requestedResolutionID.clear();
    return EC_Normal;
  }
  return requestedResolutionID.putString(value);
}

E_Condition DVPSStoredPrint::setFilmOrientation(DVPSFilmOrientation value)
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
  
E_Condition DVPSStoredPrint::setTrim(DVPSTrimMode value)
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

E_Condition DVPSStoredPrint::setRequestedDecimateCropBehaviour(DVPSDecimateCropBehaviour value)
{
  decimateCropBehaviour = value;
  return EC_Normal;
}
  
  
E_Condition DVPSStoredPrint::newPrinter()
{
  filmSizeID.clear();
  magnificationType.clear();
  smoothingType.clear();
  configurationInformation.clear();
  requestedResolutionID.clear();
  trim.clear();
  borderDensity.clear();
  emptyImageDensity.clear();
  E_Condition result = setRequestedDecimateCropBehaviour(DVPSI_default);
  if (EC_Normal == result) result = imageBoxContentList.setAllImagesToDefault();
  return result;
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

E_Condition DVPSStoredPrint::setBorderDensity(const char *value)
{
  if ((value==NULL)||(strlen(value)==0)) 
  {
    borderDensity.clear();
    return EC_Normal;
  }
  return borderDensity.putString(value);
}

E_Condition DVPSStoredPrint::setEmtpyImageDensity(const char *value)
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

E_Condition DVPSStoredPrint::setPrintIllumination(Uint16 value)
{
  return illumination.putUint16(value, 0);
}

Uint16 DVPSStoredPrint::getPrintIllumination()
{
  Uint16 result = 0;
  if (EC_Normal == illumination.getUint16(result, 0)) return result; else return 0;
}

E_Condition DVPSStoredPrint::setPrintReflectedAmbientLight(Uint16 value)
{
  return reflectedAmbientLight.putUint16(value, 0);
}

Uint16 DVPSStoredPrint::getPrintReflectedAmbientLight()
{
  Uint16 result = 0;
  if (EC_Normal == reflectedAmbientLight.getUint16(result, 0)) return result; else return 0;
}

E_Condition DVPSStoredPrint::deleteImage(size_t idx)
{
  E_Condition result = imageBoxContentList.deleteImage(idx);
  char *c = NULL;
  if (EC_Normal != configurationInformation.getString(c)) c = NULL;
  presentationLUTList.cleanup(c, imageBoxContentList);
  return result;
}
  
E_Condition DVPSStoredPrint::deleteMultipleImages(size_t number)
{
  E_Condition result = imageBoxContentList.deleteMultipleImages(number);
  char *c = NULL;
  if (EC_Normal != configurationInformation.getString(c)) c = NULL;
  presentationLUTList.cleanup(c, imageBoxContentList);
  return result;
}

E_Condition DVPSStoredPrint::deleteSpooledImages()
{
  E_Condition result = EC_IllegalCall;
  char *c = NULL;
  unsigned long deleteImageBoxes=0;

  updateCache();
  if (currentValuesValid) 
  {
  	deleteImageBoxes = currentNumCols * currentNumRows;
    if (deleteImageBoxes > imageBoxContentList.size()) deleteImageBoxes = imageBoxContentList.size();
    result = imageBoxContentList.deleteMultipleImages(deleteImageBoxes);
  }
  if (EC_Normal != configurationInformation.getString(c)) c = NULL;
  presentationLUTList.cleanup(c, imageBoxContentList);
  return result;
}

E_Condition DVPSStoredPrint::printSCUgetPrinterInstance(DVPSPrintMessageHandler& printHandler)
{ 
  DcmDataset *attributeListOut=NULL; 
  Uint16 status=0;
  CONDITION cond=printHandler.getRQ(UID_PrinterSOPClass, UID_PrinterSOPInstance, NULL, 0, status, attributeListOut);

  /* the N-GET response has been dumped somewhere else, we only need to delete it */
  delete attributeListOut;
  
  if (!SUCCESS(cond)) return EC_IllegalCall;
  return EC_Normal;
}

E_Condition DVPSStoredPrint::printSCUpreparePresentationLUT(
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
  	*logstream << "spooler: warning: printer does not support Presentation LUT SOP Class," << endl
  	     << "  presentation LUT related print job settings will be ignored." << endl;
  }

  E_Condition result = EC_Normal;  
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
      CONDITION cond = printHandler.createRQ(UID_PresentationLUTSOPClass, presentationLUTInstanceUID, &dset, status, attributeListOut);
      if ((SUCCESS(cond))&&((status==0)||((status & 0xf000)==0xb000)))
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

E_Condition DVPSStoredPrint::addReferencedPLUTSQ(DcmItem &dset)
{
  if (referencedPresentationLUTInstanceUID.getLength() == 0) return EC_Normal;
  
  E_Condition result = EC_Normal;
  DcmElement *delem=NULL;
  DcmSequenceOfItems *dseq = new DcmSequenceOfItems(DCM_ReferencedPresentationLUTSequence);
  DcmItem *ditem = new DcmItem();

  if (ditem && dseq)
  {
     ADD_TO_DATASET2(DcmUniqueIdentifier, referencedPresentationLUTInstanceUID)
     if (result==EC_Normal)
     {
       dseq->insert(ditem);
       dset.insert(dseq);
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

E_Condition DVPSStoredPrint::addPresentationLUTReference(DcmItem& dset)
{
  DcmElement *delem=NULL;
  E_Condition result = EC_Normal;

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
         dset.insert(dseq);
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

E_Condition DVPSStoredPrint::printSCUcreateBasicFilmSession(
   DVPSPrintMessageHandler& printHandler, 
   DcmDataset& dset,
   OFBool plutInSession)
{
  if (filmSessionInstanceUID.size() > 0) return EC_IllegalCall;

  DcmDataset *attributeListOut=NULL; 
  Uint16 status=0;
  E_Condition result = EC_Normal;

  // we expect 'number of copies', 'print priority', 'medium type' and 'film destination' in dset
  // add illumination and reflection, and presentation LUT reference if necessary.
  if ((printHandler.printerSupportsPresentationLUT()) && plutInSession) result = addPresentationLUTReference(dset);
    
  if (result==EC_Normal)
  {
    CONDITION cond = printHandler.createRQ(UID_BasicFilmSessionSOPClass, filmSessionInstanceUID, &dset, status, attributeListOut);
    if ((! SUCCESS(cond))||((status!=0)&&((status & 0xf000)!=0xb000))) 
    {
      result = EC_IllegalCall;
      filmSessionInstanceUID.clear();
    }
    delete attributeListOut;
  }
  return result;
}

E_Condition DVPSStoredPrint::printSCUcreateBasicFilmBox(DVPSPrintMessageHandler& printHandler, OFBool plutInSession)
{
  if ((filmSessionInstanceUID.size() == 0)||(filmBoxInstanceUID.size() > 0)) return EC_IllegalCall;

  E_Condition result = EC_Normal;  
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
  
  ADD_TO_DATASET(DcmShortText, imageDisplayFormat)
  if (filmOrientation.getLength() > 0)           { ADD_TO_DATASET(DcmCodeString, filmOrientation) }
  if (filmSizeID.getLength() > 0)                { ADD_TO_DATASET(DcmCodeString, filmSizeID) }
  if (magnificationType.getLength() > 0)         { ADD_TO_DATASET(DcmCodeString, magnificationType) }
  if (maxDensity.getLength() > 0)                { ADD_TO_DATASET(DcmUnsignedShort, maxDensity) }
  if (configurationInformation.getLength() > 0)  { ADD_TO_DATASET(DcmShortText, configurationInformation) }
  if (annotationDisplayFormatID.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, annotationDisplayFormatID) }
  if (smoothingType.getLength() > 0)             { ADD_TO_DATASET(DcmCodeString, smoothingType) }
  if (borderDensity.getLength() > 0)             { ADD_TO_DATASET(DcmCodeString, borderDensity) }
  if (emptyImageDensity.getLength() > 0)         { ADD_TO_DATASET(DcmCodeString, emptyImageDensity) }
  if (minDensity.getLength() > 0)                { ADD_TO_DATASET(DcmUnsignedShort, minDensity) }
  if (trim.getLength() > 0)                      { ADD_TO_DATASET(DcmCodeString, trim) }
  if (requestedResolutionID.getLength() > 0)     { ADD_TO_DATASET(DcmCodeString, requestedResolutionID) }
  
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

  // add illumination and reflection, and presentation LUT reference if necessary.
  if ((result==EC_Normal) && (printHandler.printerSupportsPresentationLUT()) && (!plutInSession)) result = addPresentationLUTReference(dset);

  if (result==EC_Normal)
  {
    CONDITION cond = printHandler.createRQ(UID_BasicFilmBoxSOPClass, filmBoxInstanceUID, &dset, status, attributeListOut);
    if ((SUCCESS(cond))&&((status==0)||((status & 0xf000)==0xb000))&& attributeListOut)
    {
      // N-CREATE was successful, now evaluate Referenced Image Box SQ
      stack.clear();
      if (EC_Normal == attributeListOut->search(DCM_ReferencedImageBoxSequence, stack, ESM_fromHere, OFFalse))
      {
        seq=(DcmSequenceOfItems *)stack.top();
        size_t numItems = seq->card();
        if (numItems > imageBoxContentList.size()) numItems = imageBoxContentList.size();
        for (size_t i=0; i<numItems; i++)
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
    } else {
      filmBoxInstanceUID.clear();
      result = EC_IllegalCall;
    }
    delete attributeListOut;
  }
  return result;
}


E_Condition DVPSStoredPrint::printSCUprintBasicFilmBox(DVPSPrintMessageHandler& printHandler)
{
  if (filmBoxInstanceUID.size() == 0) return EC_IllegalCall;
  DcmDataset *attributeListOut=NULL; 
  Uint16 status=0;
 
  CONDITION cond = printHandler.actionRQ(UID_BasicFilmBoxSOPClass, filmBoxInstanceUID.c_str(), 
    1 /* action type ID 1 = print */, NULL /* no action information */, status, attributeListOut);
  delete attributeListOut; // should be empty anyway

  if ((SUCCESS(cond))&&((status==0)||((status & 0xf000)==0xb000))) return EC_Normal;
  return EC_IllegalCall; // otherwise
}

E_Condition DVPSStoredPrint::printSCUdelete(DVPSPrintMessageHandler& printHandler)
{  
  CONDITION cond;
  Uint16 status=0;
  E_Condition result = EC_Normal;
    
  // delete basic film box
  if (filmBoxInstanceUID.size() > 0)
  {
  	cond = printHandler.deleteRQ(UID_BasicFilmBoxSOPClass, filmBoxInstanceUID.c_str(), status);
    if ((! SUCCESS(cond))||((status!=0)&&((status & 0xf000)!=0xb000))) result = EC_IllegalCall;
    filmBoxInstanceUID.clear();
  }
  
  // delete basic film session
  if (filmSessionInstanceUID.size() > 0)
  {
  	cond = printHandler.deleteRQ(UID_BasicFilmSessionSOPClass, filmSessionInstanceUID.c_str(), status);
    if ((! SUCCESS(cond))||((status!=0)&&((status & 0xf000)!=0xb000))) result = EC_IllegalCall;
    filmSessionInstanceUID.clear();
  }

  // delete presentation LUT
  if ((presentationLUTInstanceUID.size() > 0)&&(printHandler.printerSupportsPresentationLUT()))
  {
    cond = printHandler.deleteRQ(UID_PresentationLUTSOPClass, presentationLUTInstanceUID.c_str(), status);
    if ((! SUCCESS(cond))||((status!=0)&&((status & 0xf000)!=0xb000))) result = EC_IllegalCall;
    presentationLUTInstanceUID.clear();
  }
  return result;  
}

E_Condition DVPSStoredPrint::printSCUsetBasicImageBox(
    DVPSPrintMessageHandler& printHandler,
    size_t idx,
    DicomImage& image)
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
          *logstream << "Warning: rendering Presentation LUT into 8-bit bitmap, image quality loss possible." << endl;
        }
      } else {
#ifdef DEBUG
        *logstream << "Warning: unable to activate Presentation LUT, using IDENTITY instead." << endl;
#endif      
        image.setPresentationLutShape(ESP_Identity);
      }
    } else image.setPresentationLutShape(ESP_Identity); // default
  } else image.setPresentationLutShape(ESP_Identity); // default
  
  E_Condition result = imageBoxContentList.prepareBasicImageBox(idx, dataset);
  if (EC_Normal == result)
  {
    ditem = new DcmItem();
    if (ditem)
    {
      dseq = new DcmSequenceOfItems(DCM_BasicGrayscaleImageSequence);
      if (dseq)
      {
        if (EC_Normal==result) result = DVPSHelper::putUint16Value(ditem, DCM_SamplesPerPixel, 1);
        if (EC_Normal==result) result = DVPSHelper::putStringValue(ditem, DCM_PhotometricInterpretation, "MONOCHROME2");
        if (EC_Normal==result) result = DVPSHelper::putUint16Value(ditem, DCM_PixelRepresentation, 0);
        if (EC_Normal==result) result = DVPSHelper::putUint16Value(ditem, DCM_Rows, (Uint16)height);
        if (EC_Normal==result) result = DVPSHelper::putUint16Value(ditem, DCM_Columns, (Uint16)width);
        double aspectRatio = image.getHeightWidthRatio();
        if ((aspectRatio != 1.0)&&(aspectRatio != 0))
        {
          sprintf(str, "10000\\%ld", (long)(aspectRatio*10000.0));
          if (EC_Normal==result) result = DVPSHelper::putStringValue(ditem, DCM_Columns, str);
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
               if (EC_Normal==result) result = ditem->insert(pxData); else delete pxData;
            } else status = EC_IllegalCall;
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
               if (EC_Normal==result) result = ditem->insert(pxData); else delete pxData;
            } else status = EC_IllegalCall;
          }
        }
        
        if (result==EC_Normal)
        {
          dseq->insert(ditem);
          dataset.insert(dseq);
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
    CONDITION cond = printHandler.setRQ(UID_BasicGrayscaleImageBoxSOPClass, imageSopInstanceUID, &dataset, status, attributeListOut);
    if ((! SUCCESS(cond))||((status!=0)&&((status & 0xf000)!=0xb000))) result = EC_IllegalCall;
  }
  delete attributeListOut;
  return result;
}    

void DVPSStoredPrint::setLog(ostream *o)
{
  if (o)
  {
  	logstream = o;
  	imageBoxContentList.setLog(o);
  	presentationLUTList.setLog(o);
  }
}

/*
 *  $Log: dvpssp.cc,v $
 *  Revision 1.18  1999-10-13 14:11:14  meichel
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

