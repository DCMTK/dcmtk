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
 *  Last Update:      $Author: thiel $
 *  Update Date:      $Date: 1999-09-09 14:57:50 $
 *  CVS/RCS Revision: $Revision: 1.10 $
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
#ifdef HAVE_TIME_H
#include <time.h>
#endif

// Helper class
class MyPrintEventHandler : public DVPSPrintEventHandler
{
public:
   DIC_US handleEvent(
    T_DIMSE_N_EventReportRQ& eventMessage, 
    DcmDataset *eventInformation, 
    DcmDataset *statusDetail)
	 {
			cerr<<"Event"<<endl;
			return 0;
	 }
};


/* some defaults for creating Stored Print objects */
#define DEFAULT_imageDisplayFormat        "STANDARD\\1,1"

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
  delem = new a_type(a_name);																	      \
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

/* --------------- static helper functions --------------- */


static char * printStatusString(int status)
{
	
	char * buf;
	char * out;
	switch(status)
	{
		case 0x0:
			out="Status ok";
			break;
		case 0x105:
			out="No Such Attribute";
			break;
		case	0x106:
			out="Invalid Attribute Value";
			break;
		case 0x107:
			out="Attribute List Error";
			break;
		case 0x116:
			out="Attribute Value Out of Range";
			break;
		case 0x120:
			out="Missing Attribute";
		break;

		default:
		sprintf(out,"Unknown Status Code:%d",status);
		break;
	}

	buf=(char *) malloc(strlen(out)+1);
	if (buf)
		strcpy(buf,out);
	else
		buf=NULL;
	
	return buf;
}

static DcmElement * getThisElement(DcmItem * in,Uint16 g, Uint16 e)
{
  DcmElement *elem;
  for (unsigned int i=0;in->card()>i;i++)
    {
      elem=in->getElement(i);
      if (elem->getGTag()==g && elem->getETag()==e ) 
	{
	  return elem;
	}
    }
  return NULL;
}

static CONDITION addCopyElement(DcmItem *src,DcmItem *target,Uint16 g, Uint16 e)
{
   return (target->insert(getThisElement(src,g,e)));
}

static CONDITION addThisValue(DcmItem *in,Uint16 g, Uint16 e,const char * instring)
{
  DcmElement * elem;
  newDicomElement(elem,DcmTag(g,e));
  elem->putString(instring);
  return (in->insert(elem));
}




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
, decimateCropBehaviour(DVPSI_default)
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
, decimateCropBehaviour(copy.decimateCropBehaviour)
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
  decimateCropBehaviour = DVPSI_default;
	nextImage=0;
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
      cerr << "Warning: cannot parse image display format '" << aString.c_str() << "'" << endl;
#endif    	
    }
  } else {
#ifdef DEBUG
    cerr << "Warning: unknown image display format '" << aString.c_str() << "'" << endl;
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

E_Condition DVPSStoredPrint::createFromItem(DcmItem &dset)
{
	
  E_Condition result = EC_Normal;
	DcmStack stack;
	OFString itemSOPClass;
	clear(); // re-initialize Stored Print object 
    
	DcmUniqueIdentifier sopclassuid(DCM_SOPClassUID);  
	READ_FROM_DATASET(DcmUniqueIdentifier, sopclassuid)
  sopclassuid.getOFString(itemSOPClass,0);

  if (itemSOPClass == UID_StoredPrintStorage){
			return read(dset);
		}

	result = createDefaultValues();
 
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
	//### who is it
  //READ_FROM_DATASET(DcmLongString, manufacturer)
	manufacturer.putString("");
	READ_FROM_DATASET(DcmCodeString, specificCharacterSet)

	char uid[100];
	SET_UID(seriesInstanceUID)
 	seriesNumber.putString("1");
	//### read from item 
	
	if (itemSOPClass == UID_GrayscaleSoftcopyPresentationStateStorage){
			SET_UID(presentationLUTInstanceUID)
			//### just for testing 
			//### missing read
			presentationLUT.setType(DVPSP_identity);
	}
	//###Missing Film Box 
		imageBoxContentList.addImage(dset,"DCMPRTSV");
		if (EC_Normal==result) result = imageBoxContentList.createDefaultValues(true); // renumber if limitImages is true
	//###
	return result;
}
  
E_Condition DVPSStoredPrint::addImage(DcmItem &image,char *aETitle)
{
	E_Condition result;
	result = imageBoxContentList.addImage(image,aETitle);
	return result;
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
       // result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Warning: PatientsName missing or incorrect in Stored Print" << endl;
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

  SET_UID(seriesInstanceUID)
  SET_UID(imageSeriesInstanceUID)

  if ((result==EC_Normal)&&(sOPInstanceUID.getLength()==0))
  {
    result = sOPInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
    currentDate(aString);
    if (result==EC_Normal) result = instanceCreationDate.putString(aString.c_str());
    currentTime(aString);
    if (result==EC_Normal) result = instanceCreationTime.putString(aString.c_str());
  }

  if ((result==EC_Normal)&&(studyInstanceUID.getLength()==0))
  {
    result = studyInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
    currentDate(aString);
    if (result==EC_Normal) result = studyDate.putString(aString.c_str());
    currentTime(aString);
    if (result==EC_Normal) result = studyTime.putString(aString.c_str());
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
     ADD_TO_DATASET2(DcmUniqueIdentifier, pLUTUID)
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
        ADD_TO_DATASET2(DcmUniqueIdentifier, presentationLUTInstanceUID)
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
     result = modality.putString("STORED_PRINT");  // defined term
  }
  ADD_TO_DATASET(DcmCodeString, modality)

  // compute number of image boxes to write
  updateCache();
  unsigned long writeImageBoxes=0; // default: write all
  if (limitImages && currentValuesValid) writeImageBoxes = currentNumCols * currentNumRows;
  
  // write imageBoxContentList
  if (EC_Normal == result) result = imageBoxContentList.write(dset, writeRequestedImageSize, writeImageBoxes);

  // write PrintManagementCapabilitiesSequence
  dseq = new DcmSequenceOfItems(DCM_PrintManagementCapabilitiesSequence);
  if (dseq)
  {
    if (EC_Normal == result) result = imageBoxContentList.addReferencedUIDItem(*dseq, UID_BasicFilmSessionSOPClass);
    if (EC_Normal == result) result = imageBoxContentList.addReferencedUIDItem(*dseq, UID_BasicFilmBoxSOPClass);
    if (EC_Normal == result) result = imageBoxContentList.addReferencedUIDItem(*dseq, UID_BasicGrayscaleImageBoxSOPClass);
    if (EC_Normal == result) result = imageBoxContentList.addImageSOPClasses(*dseq, writeImageBoxes);
    if ((result == EC_Normal)&&(presentationLUTInstanceUID.getLength() > 0))
    {
      result = imageBoxContentList.addReferencedUIDItem(*dseq, UID_PresentationLUTSOPClass);
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
  const char *refsopinstanceuid,
  const char *requestedimagesize,
  const char *patientid)
{
  char instanceuid[100];
  char *refstudyuid=NULL;
  char *refseriesuid=NULL;

  createDefaultValues(); // make sure that all UIDs are defined
  
  studyInstanceUID.getString(refstudyuid); // same study UID for stored print and hardcopy image
  imageSeriesInstanceUID.getString(refseriesuid); // but separate series for the hardcopy images

  return imageBoxContentList.addImageBox(dcmGenerateUniqueIdentifer(instanceuid), 
     retrieveaetitle, refstudyuid, refseriesuid, UID_HardcopyGrayscaleImageStorage, 
     refsopinstanceuid, requestedimagesize, patientid);
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
    filmOrientation.clear();
    trim.clear();
    return setRequestedDecimateCropBehaviour(DVPSI_default);
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

DVPSPrintPresentationLUTType DVPSStoredPrint::getPresentationLUT()
{
  if (presentationLUTInstanceUID.getLength() == 0) return DVPSQ_none;

  switch (presentationLUT.getType())
  {
      case DVPSP_identity:
        break;
      case DVPSP_table:
        return DVPSQ_table;
        /* break; */
      case DVPSP_inverse: /* should not happen */
#ifdef DEBUG
        cerr << "Warning: INVERSE presentation LUT shape found in stored print" << endl;
#endif    	
        break;
  }
  return DVPSQ_identity;
}
  
E_Condition DVPSStoredPrint::setCurrentPresentationLUT(DVPSPrintPresentationLUTType newType)
{
  char uid[70];
  E_Condition result = EC_Normal;
  
  switch (newType)
  {
    case DVPSQ_identity:
      result = presentationLUT.setType(DVPSP_identity);
      if (EC_Normal == result) result = presentationLUTInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
      break;
    case DVPSQ_table:
      result = presentationLUT.setType(DVPSP_table);
      if (EC_Normal == result) result = presentationLUTInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
      break;
    case DVPSQ_none:
      presentationLUTInstanceUID.clear();
      break;
  }
  return result;
}

E_Condition DVPSStoredPrint::setPresentationLookupTable(
    DcmUnsignedShort& lutDescriptor,
    DcmUnsignedShort& lutData,
    DcmLongString& lutExplanation)
{
  char uid[70];
  E_Condition result = presentationLUT.setLUT(lutDescriptor, lutData, lutExplanation);
  if (EC_Normal == result) result = presentationLUTInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
  return result;
}
    
const char *DVPSStoredPrint::getCurrentPresentationLUTExplanation()
{ 
  if (presentationLUTInstanceUID.getLength() > 0) return presentationLUT.getCurrentExplanation();
  return NULL;
}



E_Condition DVPSStoredPrint::startPrint(DVPSPrintMessageHandler *printHandler)
 {
		E_Condition result=EC_Normal;
	
   
    CONDITION cond;
    Uint16 status;
  
		DcmDataset *listOut; 
    DcmDataset *listOut2;
  
		OFString	ImageBoxInstanceUID;
    OFString   tmpString;
    listOut=new DcmDataset();
    const Uint16 list[18] = { 0x2110,0x10,0x2110,0x20,0x2110,0x30,0x8,0x70,0x8,0x1090,0x18,0x1000,0x18,0x1020,0x18,0x1200,0x18,0x1201};
    DcmDataset * listIn=new DcmDataset();
    DcmItem * sqitem=new DcmItem();
    DcmElement *elem=NULL;
  
		currentPrintHandler=printHandler;

		MyPrintEventHandler myeve;

		currentPrintHandler->setEventHandler(&myeve);


    cond=currentPrintHandler->getRQ(UID_PrinterSOPClass,"1.2.840.10008.5.1.1.17",
			    list,18,status,listOut);
 
#ifdef DEBUG 
		cerr << "Response PrinterSOPClass " << endl;
		if (listOut) listOut->print(cerr,0,1);
		cerr << "PrinterSOPCLasss "<< printStatusString(status)<<endl;
#endif
    if (!SUCCESS(cond))
      {
	
			COND_DumpConditions();
			result=	EC_CorruptedData;
			return(result);
      
		}

#ifdef DEBUG
	  cerr << "CREATE Filmsession" << endl;
#endif
    
    filmSessionInstanceUID.clear();
		if (listOut) listOut->clear();
		if (listIn) listIn->clear();
		addThisValue(listIn,0x2000,0x10,"1");//Number of Copies

    cond=currentPrintHandler->createRQ((const char *)UID_BasicFilmSessionSOPClass,
			       filmSessionInstanceUID,listIn,status,listOut);
    if (!SUCCESS(cond))
      {
				COND_DumpConditions(); 
				result=	EC_CorruptedData;
				return(result);
  
      }
#ifdef DEBUG  
		cerr << "Response FilmSession " << endl;
		cerr << "FilmSessionInstanceUID: " << filmSessionInstanceUID << endl;
		if (listOut) listOut->print(cerr,0,1);
		cerr << "Session "<<printStatusString(status)<<endl;
#endif
		
		if (presentationLUTInstanceUID.getLength() != 0) {
#ifdef DEBUG

			cerr << "Setting Presentation LUT" << endl;
#endif
			if (currentPrintHandler->printerSupportsPresentationLUT()){
				listIn->clear();
				presentationLUT.write(*(DcmItem *)listIn);
#ifdef DEBUG
				cerr << "Creating Presentation LUT"<< endl;
				listIn->print(cerr,0,1);
#endif
				cond=currentPrintHandler->createRQ((const char *)UID_PresentationLUTSOPClass,
			       tmpString,listIn,status,listOut);
				    if (!SUCCESS(cond))
				{
					COND_DumpConditions(); 
					result=	EC_CorruptedData;
					return(result);
				}
#ifdef DEBUG
				presentationLUTInstanceUID.putString(tmpString.c_str());
				cerr << "Response  Presentation LUT" << endl;
				cerr << "presentationLUTInstanceUID: " << tmpString << endl;

				if (listOut) listOut->print(cerr,0,1);
				cerr << "Presentation LUT "<<printStatusString(status)<<endl;

#endif
		
			}else{
				cerr << "Warning Printer does not support Presentation LUT" << endl;
			}
		}		
		DcmSequenceOfItems *seq = new DcmSequenceOfItems(DcmTag(0x2010,0x500),0);
		addThisValue(sqitem,0x0008,0x1150,"=BasicFilmSessionSOPClass");
    addThisValue(sqitem,0x0008,0x1155,filmSessionInstanceUID.c_str());
    seq->append(sqitem);
		
				DcmDataset dset;
				DcmElement *delem;
        ADD_TO_DATASET(DcmShortText, imageDisplayFormat)
        	if (currentPrintHandler->printerSupportsPresentationLUT()){//###workaround for old printer
				ADD_TO_DATASET(DcmCodeString, filmOrientation)
        ADD_TO_DATASET(DcmCodeString, filmSizeID)
        ADD_TO_DATASET(DcmCodeString, magnificationType)
        ADD_TO_DATASET(DcmUnsignedShort, maxDensity)
        ADD_TO_DATASET(DcmShortText, configurationInformation)
        if (annotationDisplayFormatID.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, annotationDisplayFormatID) }
        if (smoothingType.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, smoothingType) }
        if (borderDensity.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, borderDensity) }
        if (emptyImageDensity.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, emptyImageDensity) }
        if (minDensity.getLength() > 0) { ADD_TO_DATASET(DcmUnsignedShort, minDensity) }
        if (trim.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, trim) }
        if (requestedResolutionID.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, requestedResolutionID) }
					}
        if (presentationLUTInstanceUID.getLength() > 0)
        {
					if (currentPrintHandler->printerSupportsPresentationLUT()){
						ADD_TO_DATASET(DcmUnsignedShort, illumination)
					  ADD_TO_DATASET(DcmUnsignedShort, reflectedAmbientLight)
						if (EC_Normal == result) result = addReferencedPLUTSQ(*(DcmItem*)&dset);
					}else{
						cerr << "Warning Presentation LUT not included in Box" << endl;
					}
        }
				dset.insert((DcmElement*)seq,0);
		
#ifdef DEBUG
			cerr << "Sending Create BasicFilmBox" << endl;
			dset.print();
#endif	


    cond=currentPrintHandler->createRQ((const char *)UID_BasicFilmBoxSOPClass,
			      filmBoxInstanceUID,&dset,status,listOut2);
	
    if (!SUCCESS(cond))
      {
				COND_DumpConditions();
				cond=currentPrintHandler->deleteRQ((const char *)UID_BasicFilmSessionSOPClass,
					filmSessionInstanceUID.c_str(),status);   
 
				result=	EC_CorruptedData;
				return(result);
      }

#ifdef DEBUG
		cerr << "Response create BasicFilmBox"<<endl;
		if (listOut2) listOut2->print();
    cerr<<"Film Box  "<<printStatusString(status)<<endl;

#endif
		if (listOut2==NULL)
		{
			cerr << "No imageBoxInstanceUID created" << endl;
			return EC_CorruptedData;
		}
		elem = getThisElement(listOut2,0x2010,0x510);
    seq=(DcmSequenceOfItems *)elem;
		imageBoxInstanceUID=(char **)malloc(sizeof(char *)*(imageBoxContentList.size()+1));
		
		for (unsigned int i=0;i<imageBoxContentList.size();i++){
			DcmItem *item=seq->getItem(i);
			elem = getThisElement((DcmDataset* ) item,0x0008,0x1155);
			elem->getString(imageBoxInstanceUID[i]);
		}  

		currentImageBox = imageBoxContentList.begin();

		return result;
 }



E_Condition DVPSStoredPrint::getNextImageReference(char *&aETitle,char *&patID,
																									char *&studyUID,char *&seriesUID,
																									char *&instanceUID)  
{
  E_Condition result=EC_Normal;
  static int flag=0;
  static OFListIterator(DVPSImageBoxContent *) last = imageBoxContentList.end();
  if ((currentImageBox != last))
  {     
		if (flag)
			++currentImageBox;	
		else
			flag=1;
		result = (*currentImageBox)->getImageReference(aETitle,patID,studyUID,
			seriesUID,instanceUID);
		nextImage++;
    if (EC_Normal != result) return result;
    
  }
 
	return result;
}
E_Condition DVPSStoredPrint::setImage(DcmItem *image)
{
  E_Condition result;
  DcmItem * sqitem=new DcmItem();
  DcmSequenceOfItems *seq=new DcmSequenceOfItems(DcmTag(0x2020,0x110),0);
  DcmDataset * listIn=new DcmDataset();
	DcmDataset * listOut=new DcmDataset();
	DcmItem ditem;
	  CONDITION cond;
    Uint16 status;
  
	(*currentImageBox)->write(ditem,OFTrue);
	ditem.print();
 


 		char buf[10];
		sprintf(buf,"%d",nextImage);
	
		addCopyElement(&ditem,(DcmItem *)listIn,0x2010,0x60);
		addCopyElement(&ditem,(DcmItem *)listIn,0x2010,0x80);
		addCopyElement(&ditem,(DcmItem *)listIn,0x2010,0x120);
		addCopyElement(&ditem,(DcmItem *)listIn,0x2010,0x130);
		addCopyElement(&ditem,(DcmItem *)listIn,0x2010,0x150);


		addCopyElement(&ditem,(DcmItem *)listIn,0x2020,0x20);
		addCopyElement(&ditem,(DcmItem *)listIn,0x2020,0x30);
		addCopyElement(&ditem,(DcmItem *)listIn,0x2020,0x40);

		//###Missing PresentationLUT

	
		addThisValue(listIn,0x2020,0x10,	buf);
 
		addCopyElement(image,sqitem,0x28,0x2);
    addCopyElement(image,sqitem,0x28,0x4);
    addCopyElement(image,sqitem,0x28,0x10);
    addCopyElement(image,sqitem,0x28,0x11);
    addCopyElement(image,sqitem,0x28,0x34);
    addCopyElement(image,sqitem,0x28,0x100);
    addCopyElement(image,sqitem,0x28,0x101);
    addCopyElement(image,sqitem,0x28,0x102);
    addCopyElement(image,sqitem,0x28,0x103);
    addCopyElement(image,sqitem,0x7fe0,0x10);
    seq->append(sqitem);
    listIn->insert((DcmElement*)seq,0);

#ifdef DEBUG
		cerr<< "Send Set RQ :" << endl;
		if (listIn) listIn->print(cerr,0,0);
#endif
    cond=currentPrintHandler->setRQ(UID_BasicGrayscaleImageBoxSOPClass,
		       imageBoxInstanceUID[nextImage-1],listIn,status,listOut);

    if (!SUCCESS(cond))
      {
				COND_DumpConditions();
				result=	EC_CorruptedData;
		}else{
			result=EC_Normal;

#ifdef DEBUG

    cerr<<"set ImageBox " << printStatusString(status )<< endl; 
#endif
		}
	if (sqitem) delete sqitem;

	if (!printPending()){
		//doPrint();
		if (listOut) listOut->clear();
		   currentPrintHandler->actionRQ(UID_BasicFilmBoxSOPClass,
			  filmBoxInstanceUID.c_str(),1,NULL,status,listOut);
    if (!SUCCESS(cond))
      {
				COND_DumpConditions();
				result=EC_CorruptedData;
      }
#ifdef DEBUG
    cerr << "print Action " << printStatusString(status )<< endl;
    cerr<<"Delete"<<endl;
#endif
/*
    cond=printHandler.deleteRQ((const char *)UID_BasicFilmBoxSOPClass,
			      FilmBoxInstanceUID.c_str(),status);   
    if (!SUCCESS(cond))
      {
				COND_DumpConditions();
				return 1;
      }
    cond=printHandler.deleteRQ((const char *)UID_BasicFilmSessionSOPClass,
			       FilmSessionInstanceUID.c_str(),status);   
    if (!SUCCESS(cond))
     {
       COND_DumpConditions();
       return 1;
     }
   cerr << "ReleaseAssoc" << endl;
    
   cond=printHandler.releaseAssociation();
   if (!SUCCESS(cond))
     {
       COND_DumpConditions();
       return 1;
     }
*/
		}
	return result;
}

OFBool DVPSStoredPrint::printPending()
 {
		if ((unsigned)nextImage<imageBoxContentList.size())
			return (OFTrue);

		return(OFFalse);
 }
  


E_Condition DVPSStoredPrint::setBorderDensity(const char *value)
{
  return EC_IllegalCall; // UNIMPLEMENTED
}

E_Condition DVPSStoredPrint::setEmtpyImageDensity(const char *value)
{
  return EC_IllegalCall; // UNIMPLEMENTED
}

const char *DVPSStoredPrint::getBorderDensity()
{
  return NULL; // UNIMPLEMENTED
}

const char *DVPSStoredPrint::getEmtpyImageDensity()
{
  return NULL; // UNIMPLEMENTED
}

OFBool DVPSStoredPrint::imageHasAdditionalSettings(size_t idx)
{
  return OFFalse; // UNIMPLEMENTED
}

E_Condition DVPSStoredPrint::setImageMagnificationType(size_t idx, const char *value)
{
  return EC_IllegalCall; // UNIMPLEMENTED
}

E_Condition DVPSStoredPrint::setImageSmoothingType(size_t idx, const char *value)
{
  return EC_IllegalCall; // UNIMPLEMENTED
}

E_Condition DVPSStoredPrint::setImageConfigurationInformation(size_t idx, const char *value)
{
  return EC_IllegalCall; // UNIMPLEMENTED
}

const char *DVPSStoredPrint::getImageMagnificationType(size_t idx)
{
  return NULL; // UNIMPLEMENTED
}

const char *DVPSStoredPrint::getImageSmoothingType(size_t idx)
{
  return NULL; // UNIMPLEMENTED
}

const char *DVPSStoredPrint::getImageConfigurationInformation(size_t idx)
{
  return NULL; // UNIMPLEMENTED
}



/*
 *  $Log: dvpssp.cc,v $
 *  Revision 1.10  1999-09-09 14:57:50  thiel
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

