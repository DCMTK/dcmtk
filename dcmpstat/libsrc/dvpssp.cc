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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-08-25 16:56:14 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dvpssp.h"
#include "dcuid.h"

#ifdef HAVE_TIME_H
#include <time.h>
#endif

/* some defaults for creating Stored Print objects */
#define DEFAULT_imageDisplayFormat        "STANDARD\1,1"

/* --------------- a few macros avoiding copy/paste --------------- */

#define SET_UID(a_name)                                             \
if (result==EC_Normal)                                              \
{                                                                   \
     if (a_name.getLength()==0) result = a_name.putString(dcmGenerateUniqueIdentifer(uid)); \
}

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
  a_name = *((a_type *)(stack.top()));                              \
}

#define READ_FROM_DATASET2(a_type, a_name)                           \
stack.clear();                                                      \
if (EC_Normal == item->search((DcmTagKey &)a_name.getTag(), stack, ESM_fromHere, OFFalse)) \
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

/* --------------- class DVPSStoredPrint --------------- */

DVPSStoredPrint::DVPSStoredPrint()
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
, imageBoxContentList()
, presentationLUT()  
, presentationLUTInstanceUID(DCM_SOPInstanceUID)
, sOPInstanceUID(DCM_SOPInstanceUID)
, specificCharacterSet(DCM_SpecificCharacterSet)
, instanceCreationDate(DCM_InstanceCreationDate)
, instanceCreationTime(DCM_InstanceCreationTime)
, imageSeriesInstanceUID(DCM_SeriesInstanceUID)
, currentValuesValid(OFFalse)
, currentNumCols(0)
, currentNumRows(0)
{
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
, imageBoxContentList(copy.imageBoxContentList)
, presentationLUT(copy.presentationLUT)  
, presentationLUTInstanceUID(copy.presentationLUTInstanceUID)
, sOPInstanceUID(copy.sOPInstanceUID)
, specificCharacterSet(copy.specificCharacterSet)
, instanceCreationDate(copy.instanceCreationDate)
, instanceCreationTime(copy.instanceCreationTime)
, imageSeriesInstanceUID(copy.imageSeriesInstanceUID)
, currentValuesValid(copy.currentValuesValid)
, currentNumCols(copy.currentNumCols)
, currentNumRows(copy.currentNumRows)
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
  imageBoxContentList.clear();
  presentationLUT.clear();  
  presentationLUTInstanceUID.clear();
  sOPInstanceUID.clear();
  specificCharacterSet.clear();
  instanceCreationDate.clear();
  instanceCreationTime.clear();
  imageSeriesInstanceUID.clear();
  invalidateCache();
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
  imageDisplayFormat.getOFString(aString,0);
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
      cerr << "Warning cannot parse image display format '" << aString.c_str() << "'" << endl;
#endif    	
    }
  } else {
#ifdef DEBUG
    cerr << "Warning unknown image display format '" << aString.c_str() << "'" << endl;
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
    cerr << "Error: SOP Class UID does not match StoredPrintStorage" << endl;
#endif
  }

  DcmCodeString modality(DCM_Modality);
  READ_FROM_DATASET(DcmCodeString, modality)
  if (modality.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: Modality missing or empty in Stored Print" << endl;
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

  // read Presentation LUT List Module
  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_PresentationLUTContentSequence, stack, ESM_fromHere, OFFalse))
    {
      seq=(DcmSequenceOfItems *)stack.top();
      if (seq->card() == 1)
      {
         item = seq->getItem(0);
         stack.clear();
         
         READ_FROM_DATASET2(DcmUniqueIdentifier, presentationLUTInstanceUID)
         if (EC_Normal==result) result = presentationLUT.read(*item);
      } else {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Unsupported: found PresentationLUTContentSequence in Stored Print with number of items != 1" << endl;
#endif
      }
    }
  }

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
         READ_FROM_DATASET2(DcmCodeString, filmOrientation)
         READ_FROM_DATASET2(DcmCodeString, filmSizeID)
         READ_FROM_DATASET2(DcmCodeString, magnificationType)
         READ_FROM_DATASET2(DcmCodeString, smoothingType)
         READ_FROM_DATASET2(DcmCodeString, borderDensity)
         READ_FROM_DATASET2(DcmCodeString, emptyImageDensity)
         READ_FROM_DATASET2(DcmUnsignedShort, minDensity)
         READ_FROM_DATASET2(DcmUnsignedShort, maxDensity)
         READ_FROM_DATASET2(DcmCodeString, trim)
         READ_FROM_DATASET2(DcmShortText, configurationInformation)
         READ_FROM_DATASET2(DcmUnsignedShort, illumination)
         READ_FROM_DATASET2(DcmUnsignedShort, reflectedAmbientLight)
         READ_FROM_DATASET2(DcmCodeString, requestedResolutionID)

         // check referenced presentation LUT sequence
         // if there is any reference, it must refer to the (only) presentation LUT we are managing.
         stack.clear();
         if (EC_Normal == dset.search(DCM_ReferencedPresentationLUTSequence, stack, ESM_fromHere, OFFalse))
         {
           seq=(DcmSequenceOfItems *)stack.top();
           if (seq->card() ==1)
           {
              DcmUniqueIdentifier referencedSOPInstanceUID(DCM_ReferencedSOPInstanceUID);
              item = seq->getItem(0);
              stack.clear();
              READ_FROM_DATASET2(DcmUniqueIdentifier, referencedSOPInstanceUID)
              if (referencedSOPInstanceUID.getLength() > 0)
              {
                referencedSOPInstanceUID.getOFString(aString,0);
                presentationLUTInstanceUID.getOFString(aString2,0);
                if (aString != aString2)
                {
                  result=EC_IllegalCall;
#ifdef DEBUG
                  cerr << "Error: FilmBoxContentSequence presentation LUT reference cannot be resolved" << endl;
#endif
                }
              }
           } else {
             result=EC_TagNotFound;
#ifdef DEBUG
             cerr << "Error: found FilmBoxContentSequence in Stored Print with ReferencedPresentationLUTSequence number of items != 1" << endl;
#endif
           }
         }
      } else {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: found FilmBoxContentSequence in Stored Print with number of items != 1" << endl;
#endif
      } 
    }
  }

  if (EC_Normal==result) result = imageBoxContentList.read(dset);
       
  /* Now perform basic sanity checks */

  if (result==EC_Normal)
  {
    if ((studyInstanceUID.getLength() == 0)||(studyInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: StudyInstanceUID missing or incorrect in Stored Print" << endl;
#endif
    }
    if ((seriesInstanceUID.getLength() == 0)||(seriesInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: SeriesInstanceUID missing or incorrect in Stored Print" << endl;
#endif
    }
    if ((sOPInstanceUID.getLength() == 0)||(sOPInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: SOPInstanceUID missing or incorrect in Stored Print" << endl;
#endif
    }
    if ((patientName.getLength() == 0)||(patientName.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: PatientsName missing or incorrect in Stored Print" << endl;
#endif
    }
    if ((imageDisplayFormat.getLength() == 0)||(imageDisplayFormat.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: ImageDisplayFormat missing or incorrect in Stored Print" << endl;
#endif
    }
    if (imageBoxContentList.size() == 0)
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: ImageBoxContentSequence missing or empty in Stored Print" << endl;
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
         if (aString == UID_BasicFilmBoxSOPClass) haveFilmBox=OFTrue;
         else if ((aString == UID_PrinterSOPClass)||(aString == UID_BasicFilmSessionSOPClass)||(aString == UID_PresentationLUTSOPClass)) { }
         else if (isImageStorageSOPClass(aString)) haveImageStorage=OFTrue;
         else 
         {
           result=EC_IllegalCall;
#ifdef DEBUG
           cerr << "Error: Unsupported SOP Class UID in PrintManagementCapabilitiesSequence" << endl;
#endif
         }
      }
      if (EC_Normal==result)
      {
        if (! haveFilmBox)
        {
           result=EC_IllegalCall;
#ifdef DEBUG
           cerr << "Error: Film Box SOP Class not referenced in PrintManagementCapabilitiesSequence" << endl;
#endif
        }
        if (! haveGrayscaleImageBox)
        {
           result=EC_IllegalCall;
#ifdef DEBUG
           cerr << "Error: Basic Grayscale Image Box SOP Class not referenced in PrintManagementCapabilitiesSequence" << endl;
#endif
        }
        if (! haveImageStorage)
        {
           result=EC_IllegalCall;
#ifdef DEBUG
           cerr << "Error: No Image Storage SOP Class referenced in PrintManagementCapabilitiesSequence" << endl;
#endif
        }
      }
    } else {
      result=EC_TagNotFound;
#ifdef DEBUG
      cerr << "Error: PrintManagementCapabilitiesSequence not found" << endl;
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

  SET_UID(studyInstanceUID)
  SET_UID(seriesInstanceUID)
  SET_UID(sOPInstanceUID)

  if ((result==EC_Normal)&&(sOPInstanceUID.getLength()==0))
  {
    result = sOPInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
    currentDate(aString);
    if (result==EC_Normal) result = instanceCreationDate.putString(aString.c_str());
    currentTime(aString);
    if (result==EC_Normal) result = instanceCreationTime.putString(aString.c_str());
  }
  if ((result==EC_Normal)&&(imageDisplayFormat.getLength()==0)) result = imageDisplayFormat.putString(DEFAULT_imageDisplayFormat);
  return result;
}


E_Condition DVPSStoredPrint::addReferencedPLUTSQ(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmElement *delem=NULL;
  DcmSequenceOfItems *dseq = new DcmSequenceOfItems(DCM_ReferencedPresentationLUTSequence);
  DcmItem *ditem = new DcmItem();
  char *c = NULL;

  DcmUniqueIdentifier pLUTUID(DCM_ReferencedSOPInstanceUID);
  if (EC_Normal == presentationLUTInstanceUID.getString(c)) result = pLUTUID.putString(c);

  if ((result == EC_Normal) && ditem && dseq)
  {
     ADD_TO_DATASET2(DcmShortText, imageDisplayFormat)
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

E_Condition DVPSStoredPrint::write(DcmItem &dset, OFBool limitImages)
{
  DcmElement *delem=NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;
	
  E_Condition result = createDefaultValues();
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
        if (presentationLUTInstanceUID.getLength() > 0)
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

  /* create the PresentationLUTContentSequence */
  if ((result == EC_Normal)&&(presentationLUTInstanceUID.getLength() > 0))
  {
    ditem = new DcmItem();
    if (ditem)
    {
      dseq = new DcmSequenceOfItems(DCM_PresentationLUTContentSequence);
      if (dseq)
      {
        ADD_TO_DATASET(DcmUniqueIdentifier, presentationLUTInstanceUID)
        if (EC_Normal == result) result = presentationLUT.write(*ditem);
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
     result = modality.putString("HC");
  }
  ADD_TO_DATASET(DcmCodeString, modality)

  // compute number of image boxes to write
  updateCache();
  unsigned long writeImageBoxes=0; // default: write all
  if (limitImages && currentValuesValid) writeImageBoxes = currentNumCols * currentNumRows;
  
  // write imageBoxContentList
  if (EC_Normal == result) result = imageBoxContentList.write(dset, writeImageBoxes);

  // write PrintManagementCapabilitiesSequence
  dseq = new DcmSequenceOfItems(DCM_PrintManagementCapabilitiesSequence);
  if (dseq)
  {
    if (EC_Normal == result) result = imageBoxContentList.addImageSOPClasses(*dseq, writeImageBoxes);
    if (EC_Normal == result) result = imageBoxContentList.addReferencedUIDItem(*dseq, UID_BasicFilmBoxSOPClass);
    if (EC_Normal == result) result = imageBoxContentList.addReferencedUIDItem(*dseq, UID_BasicGrayscaleImageBoxSOPClass);
    if (EC_Normal == result) result = imageBoxContentList.addReferencedUIDItem(*dseq, UID_BasicGrayscaleImageBoxSOPClass);
    if ((result == EC_Normal)&&(presentationLUTInstanceUID.getLength() > 0))
    {
      result = imageBoxContentList.addReferencedUIDItem(*dseq, UID_PresentationLUTSOPClass);
    }
    if (result==EC_Normal) dset.insert(dseq); else delete dseq;
  } else result = EC_MemoryExhausted;

  return result;
}

/*
 *  $Log: dvpssp.cc,v $
 *  Revision 1.2  1999-08-25 16:56:14  joergr
 *  Added '#include <time.h>' to make MSVC happy.
 *
 *  Revision 1.1  1999/07/30 13:34:59  meichel
 *  Added new classes managing Stored Print objects
 *
 *
 */

