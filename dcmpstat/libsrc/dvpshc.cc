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
 *  Author: Andreas Thiel
 *
 *  Purpose:
 *    classes: DVPSHardCopy
 *
 *  Last Update:      $Author: thiel $
 *  Update Date:      $Date: 1999-08-26 09:29:47 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dvpshc.h"
#include "dcuid.h"
#include "dvpsib.h"
#ifdef HAVE_TIME_H
#include <time.h>
#endif


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
  a_name = *((a_type *)(stack.top()));     			\
}

#define READ_FROM_DATASET2(a_type, a_name)                           \
stack.clear();                                                      \
if (EC_Normal == item->search((DcmTagKey &)a_name.getTag(), stack, ESM_fromHere, OFFalse)) \
{                                                                   \
  a_name = *((a_type *)(stack.top()));                              \
}

/* --------------- static helper functions --------------- */
/** not used at the moment
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

*/
/* --------------- class DVPSHardCopy --------------- */

DVPSHardCopy::DVPSHardCopy()
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
  , sOPInstanceUID(DCM_SOPInstanceUID)
  , specificCharacterSet(DCM_SpecificCharacterSet)
  , imageSeriesInstanceUID(DCM_SeriesInstanceUID)
  , instanceNumber(DCM_InstanceNumber)
  ,samplesPerPixel(DCM_SamplesPerPixel)
  ,photometricInterpretation(DCM_PhotometricInterpretation)
  ,rows(DCM_Rows)
  ,columns(DCM_Columns)
  ,pixelAspectRatio(DCM_PixelAspectRatio)
  ,bitsAllocated(DCM_BitsAllocated)
  ,bitsStored(DCM_BitsStored)
  ,highBit(DCM_HighBit)
  ,pixelData(DCM_PixelData)

{
}

DVPSHardCopy::DVPSHardCopy(const DVPSHardCopy& copy)
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
  , sOPInstanceUID(copy.sOPInstanceUID)
  , specificCharacterSet(copy.specificCharacterSet)
  , imageSeriesInstanceUID(copy.imageSeriesInstanceUID)
  , instanceNumber(copy.instanceNumber)
  ,samplesPerPixel(copy.samplesPerPixel)
  ,photometricInterpretation(copy.photometricInterpretation)
  ,rows(copy.rows)
  ,columns(copy.columns)
  ,pixelAspectRatio(copy.pixelAspectRatio)
  ,bitsAllocated(copy.bitsAllocated)
  ,bitsStored(copy.bitsStored)
  ,highBit(copy.highBit)
  ,pixelData(copy.pixelData)

{
}

DVPSHardCopy::~DVPSHardCopy()
{
}

void DVPSHardCopy::clear()
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
  sOPInstanceUID.clear();
  imageSeriesInstanceUID.clear();
  instanceNumber.clear();
  specificCharacterSet.clear();
  samplesPerPixel.clear();
  photometricInterpretation.clear();
  rows.clear();
  columns.clear();
  pixelAspectRatio.clear();
  bitsAllocated.clear();
  bitsStored.clear();
  highBit.clear();
  pixelData.clear();
}



OFBool DVPSHardCopy::isImageStorageSOPClass(OFString& sopclassuid)
{
  for (int i=0; i<numberOfDcmImageSOPClassUIDs; i++)
    {
      if (dcmImageSOPClassUIDs[i] && (sopclassuid == dcmImageSOPClassUIDs[i])) return OFTrue;
    }
  return OFFalse;
}

E_Condition DVPSHardCopy::createFromItem(DcmItem &dset)
{
	
  E_Condition result = EC_Normal;
  DcmStack stack;
  OFString itemSOPClass;
  clear(); 
    
  DcmUniqueIdentifier sopclassuid(DCM_SOPClassUID);  
  READ_FROM_DATASET(DcmUniqueIdentifier, sopclassuid)
    sopclassuid.getOFString(itemSOPClass,0);

  if (itemSOPClass == UID_HardcopyGrayscaleImageStorage){
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
    //	manufacturer.putString("");
    READ_FROM_DATASET(DcmCodeString, specificCharacterSet)
    READ_FROM_DATASET(DcmUnsignedShort,samplesPerPixel)
    READ_FROM_DATASET(DcmCodeString,      photometricInterpretation)
    READ_FROM_DATASET(DcmUnsignedShort,   rows)
    READ_FROM_DATASET(DcmUnsignedShort,   columns)
    READ_FROM_DATASET(DcmIntegerString,   pixelAspectRatio)
    READ_FROM_DATASET(DcmUnsignedShort,   bitsAllocated)
    READ_FROM_DATASET(DcmUnsignedShort,	  bitsStored)
    READ_FROM_DATASET(DcmUnsignedShort,   highBit)

    //###check and convert data
    READ_FROM_DATASET(DcmPixelData, pixelData)


    char uid[100];
  if(result==EC_Normal)	SET_UID(seriesInstanceUID)
			  if(result==EC_Normal) result = seriesNumber.putString("1");
	
  return result;
}


E_Condition DVPSHardCopy::read(DcmItem &dset)
{
  // DcmSequenceOfItems *seq;
  // DcmItem *item;
  E_Condition result = EC_Normal;
  DcmStack stack;
  OFString aString;
  OFString aString2;

  clear(); // re-initialize Stored Print object 
    
  DcmUniqueIdentifier sopclassuid(DCM_SOPClassUID);  
  DcmUniqueIdentifier refsopclassuid(DCM_ReferencedSOPClassUID);  
  READ_FROM_DATASET(DcmUniqueIdentifier, sopclassuid)
    sopclassuid.getOFString(aString,0);
  if (aString != UID_HardcopyGrayscaleImageStorage)
    {
      result=EC_IllegalCall;
#ifdef DEBUG
      cerr << "Error: SOP Class UID does not match HardCopyStorage" << endl;
#endif
    }

  DcmCodeString modality(DCM_Modality);
  READ_FROM_DATASET(DcmCodeString, modality)
    if (modality.getLength() == 0)
      {
	result=EC_IllegalCall;
#ifdef DEBUG
	cerr << "Error: Modality missing or empty in Harccopy Print" << endl;
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
    READ_FROM_DATASET(DcmUniqueIdentifier, sOPInstanceUID)
      
    READ_FROM_DATASET(DcmUnsignedShort,   samplesPerPixel)
    READ_FROM_DATASET(DcmCodeString,      photometricInterpretation)
    READ_FROM_DATASET(DcmUnsignedShort,   rows)
    READ_FROM_DATASET(DcmUnsignedShort,   columns)
    READ_FROM_DATASET(DcmIntegerString,   pixelAspectRatio)
    READ_FROM_DATASET(DcmUnsignedShort,   bitsAllocated)
    READ_FROM_DATASET(DcmUnsignedShort,	 bitsStored)
    READ_FROM_DATASET(DcmUnsignedShort,   highBit)
    READ_FROM_DATASET(DcmPixelData, pixelData)


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
	 
      }

  return result;
}


E_Condition DVPSHardCopy::createDefaultValues()
{
  E_Condition result = EC_Normal;
  char uid[100];
  OFString aString;

  if ((studyInstanceUID.getLength() == 0)||(studyInstanceUID.getVM() != 1))
    SET_UID(studyInstanceUID)
      SET_UID(seriesInstanceUID)
      SET_UID(sOPInstanceUID)

      if ((result==EC_Normal)&&(sOPInstanceUID.getLength()==0))
	{
	  result = sOPInstanceUID.putString(dcmGenerateUniqueIdentifer(uid));
	}
  return result;
}



E_Condition DVPSHardCopy::write(DcmItem &dset)
{
  DcmElement *delem=NULL;

	
  E_Condition result = createDefaultValues();
   
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
    //ADD_TO_DATASET(DcmLongString, manufacturer)
    ADD_TO_DATASET(DcmIntegerString, instanceNumber) 
    ADD_TO_DATASET(DcmUniqueIdentifier, sOPInstanceUID)
	
    ADD_TO_DATASET(DcmUnsignedShort,   samplesPerPixel)
    ADD_TO_DATASET(DcmCodeString,      photometricInterpretation)
    ADD_TO_DATASET(DcmUnsignedShort,   rows)
    ADD_TO_DATASET(DcmUnsignedShort,   columns)
    ADD_TO_DATASET(DcmIntegerString,   pixelAspectRatio)
    ADD_TO_DATASET(DcmUnsignedShort,   bitsAllocated)
    ADD_TO_DATASET(DcmUnsignedShort,	 bitsStored)
    ADD_TO_DATASET(DcmUnsignedShort,   highBit)
    ADD_TO_DATASET(DcmPixelData, pixelData)

    DcmUnsignedShort pixelRepresentation(DCM_PixelRepresentation);
  result = pixelRepresentation.putString("0");

  ADD_TO_DATASET(DcmUnsignedShort,   pixelRepresentation);

  /* add SOP Class UID */  
  DcmUniqueIdentifier sopclassuid(DCM_SOPClassUID);
  if (result==EC_Normal)
    {
      result = sopclassuid.putString(UID_HardcopyGrayscaleImageStorage);
    }
  ADD_TO_DATASET(DcmUniqueIdentifier, sopclassuid)

    /* add Modality */  
    DcmCodeString modality(DCM_Modality);
  if (result==EC_Normal)
    {
      result = modality.putString("HC");
    }
  ADD_TO_DATASET(DcmCodeString, modality)

    return result;
}


