/*
 *
 *  Copyright (C) 1998-2000, OFFIS
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
 *    classes: DVPSImageBoxContent
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:29:06 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dvpsib.h"
#include "dvpspll.h"

/* --------------- a few macros avoiding copy/paste --------------- */

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

#define SET_UID(a_name)                                             \
if (result==EC_Normal)                                              \
{                                                                   \
     if (a_name.getLength()==0) result = a_name.putString(dcmGenerateUniqueIdentifer(uid)); \
}

/* --------------- class DVPSImageBoxContent --------------- */

DVPSImageBoxContent::DVPSImageBoxContent()
: sOPInstanceUID(DCM_SOPInstanceUID)
, imageBoxPosition(DCM_ImageBoxPosition)
, polarity(DCM_Polarity)
, magnificationType(DCM_MagnificationType)
, configurationInformation(DCM_ConfigurationInformation)
, smoothingType(DCM_SmoothingType)
, requestedImageSize(DCM_RequestedImageSize)
, requestedDecimateCropBehavior(DCM_RequestedDecimateCropBehavior)
, retrieveAETitle(DCM_RetrieveAETitle)
, referencedSOPClassUID(DCM_ReferencedSOPClassUID)
, referencedSOPInstanceUID(DCM_ReferencedSOPInstanceUID)
, studyInstanceUID(DCM_StudyInstanceUID)
, seriesInstanceUID(DCM_SeriesInstanceUID)
, referencedFrameNumber(DCM_ReferencedFrameNumber)
, patientID(DCM_PatientID)
, referencedPresentationLUTInstanceUID(DCM_ReferencedSOPInstanceUID)
, logstream(&CERR)
{
}

DVPSImageBoxContent::DVPSImageBoxContent(const DVPSImageBoxContent& copy)
: sOPInstanceUID(copy.sOPInstanceUID)
, imageBoxPosition(copy.imageBoxPosition)
, polarity(copy.polarity)
, magnificationType(copy.magnificationType)
, configurationInformation(copy.configurationInformation)
, smoothingType(copy.smoothingType)
, requestedImageSize(copy.requestedImageSize)
, requestedDecimateCropBehavior(copy.requestedDecimateCropBehavior)
, retrieveAETitle(copy.retrieveAETitle)
, referencedSOPClassUID(copy.referencedSOPClassUID)
, referencedSOPInstanceUID(copy.referencedSOPInstanceUID)
, studyInstanceUID(copy.studyInstanceUID)
, seriesInstanceUID(copy.seriesInstanceUID)
, referencedFrameNumber(copy.referencedFrameNumber)
, patientID(copy.patientID)
, referencedPresentationLUTInstanceUID(copy.referencedPresentationLUTInstanceUID)
, logstream(copy.logstream)
{
}

DVPSImageBoxContent::~DVPSImageBoxContent()
{
}

void DVPSImageBoxContent::clear()
{
  sOPInstanceUID.clear();
  imageBoxPosition.clear();
  polarity.clear();
  magnificationType.clear();
  configurationInformation.clear();
  smoothingType.clear();
  requestedImageSize.clear();
  requestedDecimateCropBehavior.clear();
  retrieveAETitle.clear();
  referencedSOPClassUID.clear();
  referencedSOPInstanceUID.clear();
  studyInstanceUID.clear();
  seriesInstanceUID.clear();
  referencedFrameNumber.clear();
  patientID.clear();
  referencedPresentationLUTInstanceUID.clear();
  return;
}

E_Condition DVPSImageBoxContent::setContent(
  const char *instanceuid,
  const char *retrieveaetitle,
  const char *refstudyuid,
  const char *refseriesuid,
  const char *refsopclassuid,
  const char *refsopinstanceuid,
  const char *requestedimagesize,
  const char *patientid,
  const char *presentationlutreference)
{
  E_Condition result = EC_Normal;
  if (refstudyuid && refseriesuid && instanceuid && retrieveaetitle && refsopclassuid && refsopinstanceuid)
  {
    clear();
    result = sOPInstanceUID.putString(instanceuid);
    if (EC_Normal == result) result = retrieveAETitle.putString(retrieveaetitle);
    if (EC_Normal == result) result = referencedSOPClassUID.putString(refsopclassuid);
    if (EC_Normal == result) result = referencedSOPInstanceUID.putString(refsopinstanceuid);
    if (EC_Normal == result) result = studyInstanceUID.putString(refstudyuid);
    if (EC_Normal == result) result = seriesInstanceUID.putString(refseriesuid);
    if (requestedimagesize && (EC_Normal == result)) result = requestedImageSize.putString(requestedimagesize);
    if (patientid && (EC_Normal == result)) result = patientID.putString(patientid);
    if (presentationlutreference && (EC_Normal == result)) result = referencedPresentationLUTInstanceUID.putString(presentationlutreference);
  } else result = EC_IllegalCall;
  return result;
}

E_Condition DVPSImageBoxContent::read(DcmItem &dset, DVPSPresentationLUT_PList& presentationLUTList)
{
  DcmSequenceOfItems *seq;
  DcmItem *item;
  E_Condition result = EC_Normal;
  DcmStack stack;
  OFString aString;
  
  READ_FROM_DATASET(DcmUniqueIdentifier, sOPInstanceUID)
  READ_FROM_DATASET(DcmUnsignedShort, imageBoxPosition)
  READ_FROM_DATASET(DcmCodeString, polarity)
  READ_FROM_DATASET(DcmCodeString, magnificationType)
	if (result==EC_TagNotFound) result = EC_Normal;
  READ_FROM_DATASET(DcmShortText, configurationInformation)
		if (result==EC_TagNotFound) result = EC_Normal;
  READ_FROM_DATASET(DcmCodeString, smoothingType)
		if (result==EC_TagNotFound) result = EC_Normal;
  READ_FROM_DATASET(DcmDecimalString, requestedImageSize)
		if (result==EC_TagNotFound) result = EC_Normal;
  READ_FROM_DATASET(DcmCodeString, requestedDecimateCropBehavior)
		if (result==EC_TagNotFound) result = EC_Normal;

  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_ReferencedImageSequence, stack, ESM_fromHere, OFFalse))
    {
      seq=(DcmSequenceOfItems *)stack.top();
      if (seq->card() ==1)
      {
         item = seq->getItem(0);
         stack.clear();
         
         READ_FROM_DATASET2(DcmApplicationEntity, retrieveAETitle)
         READ_FROM_DATASET2(DcmUniqueIdentifier, referencedSOPClassUID)
         READ_FROM_DATASET2(DcmUniqueIdentifier, referencedSOPInstanceUID)
         READ_FROM_DATASET2(DcmUniqueIdentifier, studyInstanceUID)
         READ_FROM_DATASET2(DcmUniqueIdentifier, seriesInstanceUID)
         READ_FROM_DATASET2(DcmIntegerString, referencedFrameNumber)
				 if (result==EC_TagNotFound) result = EC_Normal;
         READ_FROM_DATASET2(DcmLongString, patientID)
      } else {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: found ReferencedImageSequence in Stored Print Image Box with number of items != 1" << endl;
#endif
      } 
    }
  }
  
  if (result==EC_Normal)
  {
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
             *logstream << "Error: ImageBoxContentSequence presentation LUT reference cannot be resolved" << endl;
#endif
           }
         }
      } else {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: found ImageBoxContentSequence in Stored Print with ReferencedPresentationLUTSequence number of items != 1" << endl;
#endif
      }
    }
  }
  
  /* the following attributes belong to the ReferencedImageSequence */

  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_RETIRED_ReferencedImageOverlayBoxSequence, stack, ESM_fromHere, OFFalse))
    {
      result=EC_IllegalCall;
#ifdef DEBUG
      *logstream << "Error: Stored Print: retired ReferencedImageOverlayBoxSequence not supported" << endl;
#endif
    }
  }
      
  /* Now perform basic sanity checks */

  if (result==EC_Normal)
  {
    if ((sOPInstanceUID.getLength() == 0)||(sOPInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: SOPInstanceUID missing or incorrect in Stored Print Image Box" << endl;
#endif
    }
    if ((imageBoxPosition.getLength() == 0)||(imageBoxPosition.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: ImageBoxPosition missing or incorrect in Stored Print Image Box" << endl;
#endif
    }
    if (retrieveAETitle.getLength() == 0)
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: RetrieveAETitle missing in Stored Print Image Box" << endl;
#endif
    }
    if ((referencedSOPClassUID.getLength() == 0)||(referencedSOPClassUID.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: ReferencedSOPClassUID missing or incorrect in Stored Print Image Box" << endl;
#endif
    }
    if ((referencedSOPInstanceUID.getLength() == 0)||(referencedSOPInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: ReferencedSOPInstanceUID missing or incorrect in Stored Print Image Box" << endl;
#endif
    }
    if ((studyInstanceUID.getLength() == 0)||(studyInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: StudyInstanceUID missing or incorrect in Stored Print Image Box" << endl;
#endif
    }
    if ((seriesInstanceUID.getLength() == 0)||(seriesInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: SeriesInstanceUID missing or incorrect in Stored Print Image Box" << endl;
#endif
    }
    if (referencedFrameNumber.getVM() > 1)
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: ReferencedFrameNumber VM>1 in Stored Print Image Box" << endl;
#endif
    }
  }

  return result;
}

E_Condition DVPSImageBoxContent::write(DcmItem &dset, OFBool writeRequestedImageSize)
{
  E_Condition result = EC_Normal;
  DcmElement *delem=NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  /* before writing anything, check that we are able to write a correct item */
  if (sOPInstanceUID.getLength() == 0)
  {
    result=EC_TagNotFound;
#ifdef DEBUG
    *logstream << "Error: cannot write Stored Print Image Box: SOPInstanceUID empty" << endl;
#endif
  }
  if (imageBoxPosition.getLength() == 0)
  {
    result=EC_TagNotFound;
#ifdef DEBUG
    *logstream << "Error: cannot write Stored Print Image Box: ImageBoxPosition empty" << endl;
#endif
  }
  if (retrieveAETitle.getLength() == 0)
  {
    result=EC_TagNotFound;
#ifdef DEBUG
    *logstream << "Error: cannot write Stored Print Image Box: RetrieveAETitle empty" << endl;
#endif
  }
  if (referencedSOPClassUID.getLength() == 0)
  {
    result=EC_TagNotFound;
#ifdef DEBUG
    *logstream << "Error: cannot write Stored Print Image Box: ReferencedSOPClassUID empty" << endl;
#endif
  }
  if (referencedSOPInstanceUID.getLength() == 0)
  {
    result=EC_TagNotFound;
#ifdef DEBUG
    *logstream << "Error: cannot write Stored Print Image Box: ReferencedSOPInstanceUID empty" << endl;
#endif
  }
  if (studyInstanceUID.getLength() == 0)
  {
    result=EC_TagNotFound;
#ifdef DEBUG
    *logstream << "Error: cannot write Stored Print Image Box: StudyInstanceUID empty" << endl;
#endif
  }
  if (seriesInstanceUID.getLength() == 0)
  {
    result=EC_TagNotFound;
#ifdef DEBUG
    *logstream << "Error: cannot write Stored Print Image Box: SeriesInstanceUID empty" << endl;
#endif
  }

  ADD_TO_DATASET(DcmUniqueIdentifier, sOPInstanceUID)
  ADD_TO_DATASET(DcmUnsignedShort, imageBoxPosition)
  ADD_TO_DATASET(DcmCodeString, polarity)
  if (magnificationType.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, magnificationType) }
  if (configurationInformation.getLength() > 0) { ADD_TO_DATASET(DcmShortText, configurationInformation) }
  if (smoothingType.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, smoothingType) }
  if ((writeRequestedImageSize)&&(requestedImageSize.getLength() > 0)) { ADD_TO_DATASET(DcmDecimalString, requestedImageSize) }
  if (requestedDecimateCropBehavior.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, requestedDecimateCropBehavior) }

  if (result == EC_Normal)
  {
    ditem = new DcmItem();
    if (ditem)
    {
      dseq = new DcmSequenceOfItems(DCM_ReferencedImageSequence);
      if (dseq)
      {
        delem = new DcmApplicationEntity(retrieveAETitle);
        if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
        delem = new DcmUniqueIdentifier(referencedSOPClassUID);
        if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
        delem = new DcmUniqueIdentifier(referencedSOPInstanceUID);
        if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
        delem = new DcmUniqueIdentifier(studyInstanceUID);
        if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
        delem = new DcmUniqueIdentifier(seriesInstanceUID);
        if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
        if (referencedFrameNumber.getLength() >0)
        {
          delem = new DcmIntegerString(referencedFrameNumber);
          if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
        }
        delem = new DcmLongString(patientID);
        if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
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

  if (EC_Normal == result) result = addReferencedPLUTSQ(dset);

  return result;
}

E_Condition DVPSImageBoxContent::addReferencedPLUTSQ(DcmItem &dset)
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

E_Condition DVPSImageBoxContent::prepareBasicImageBox(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmElement *delem=NULL;

  ADD_TO_DATASET(DcmUnsignedShort, imageBoxPosition)
  if (polarity.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, polarity) }
  if (magnificationType.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, magnificationType) }
  if (configurationInformation.getLength() > 0) { ADD_TO_DATASET(DcmShortText, configurationInformation) }
  if (smoothingType.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, smoothingType) }
  if (requestedImageSize.getLength() > 0) { ADD_TO_DATASET(DcmDecimalString, requestedImageSize) }
  if (requestedDecimateCropBehavior.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, requestedDecimateCropBehavior) }

  return result;
}

E_Condition DVPSImageBoxContent::createDefaultValues(OFBool renumber, unsigned long number)
{
  E_Condition result = EC_Normal;
  char uid[100];
  OFString aString;

  SET_UID(sOPInstanceUID)
  
  if (renumber)
  {
    imageBoxPosition.clear();
    result = imageBoxPosition.putUint16((Uint16)number, 0);
  } else {
    if (imageBoxPosition.getVM() != 1) result = EC_TagNotFound;
  }

  if (EC_Normal == result)
  {  
    if (retrieveAETitle.getLength() == 0) result = EC_TagNotFound;
    if (referencedSOPClassUID.getLength() == 0) result = EC_TagNotFound;
    if (referencedSOPInstanceUID.getLength() == 0) result = EC_TagNotFound;
    if (studyInstanceUID.getLength() == 0) result = EC_TagNotFound;
    if (seriesInstanceUID.getLength() == 0) result = EC_TagNotFound;
  }  
  return result;
}

const char *DVPSImageBoxContent::getSOPClassUID()
{
  char *c = NULL;
  if (EC_Normal == referencedSOPClassUID.getString(c)) return c; else return NULL;
}

E_Condition DVPSImageBoxContent::setRequestedDecimateCropBehaviour(DVPSDecimateCropBehaviour value)
{
  switch (value)
  {
    case DVPSI_decimate:
      return requestedDecimateCropBehavior.putString("DECIMATE");
      /* break; */
    case DVPSI_crop:
      return requestedDecimateCropBehavior.putString("CROP");
      /* break; */
    case DVPSI_fail:
      return requestedDecimateCropBehavior.putString("FAIL");
      /* break; */
    case DVPSI_default:
      requestedDecimateCropBehavior.clear();
      break;            
  }  
  return EC_Normal;
}

E_Condition DVPSImageBoxContent::getImageReference(
  const char *&studyUID, 
  const char *&seriesUID, 
  const char *&instanceUID)
{
  E_Condition result=EC_Normal;
  char *astudyUID = NULL;
  char *aseriesUID = NULL;
  char *aninstanceUID = NULL;
  result = studyInstanceUID.getString(astudyUID);
  if (EC_Normal == result) result = seriesInstanceUID.getString(aseriesUID);
  if (EC_Normal == result) result = referencedSOPInstanceUID.getString(aninstanceUID);
  studyUID = astudyUID;
  seriesUID = aseriesUID;
  instanceUID = aninstanceUID;
  return result;
}

DVPSDecimateCropBehaviour DVPSImageBoxContent::getRequestedDecimateCropBehaviour()
{
  DVPSDecimateCropBehaviour result = DVPSI_default;
  char *c = NULL;
  if ((EC_Normal == requestedDecimateCropBehavior.getString(c))&& c)
  {
    OFString aString(c);
    if (aString == "DECIMATE") result = DVPSI_decimate;
    else if (aString == "CROP") result = DVPSI_crop;
    else if (aString == "FAIL") result = DVPSI_fail;
  }
  return result;
}

const char *DVPSImageBoxContent::getMagnificationType()
{
  char *c = NULL;
  if (EC_Normal == magnificationType.getString(c)) return c; else return NULL;
}

const char *DVPSImageBoxContent::getSmoothingType()
{
  char *c = NULL;
  if (EC_Normal == smoothingType.getString(c)) return c; else return NULL;
}

const char *DVPSImageBoxContent::getConfigurationInformation()
{
  char *c = NULL;
  if (EC_Normal == configurationInformation.getString(c)) return c; else return NULL;
}

const char *DVPSImageBoxContent::getReferencedPresentationLUTInstanceUID()
{
  char *c = NULL;
  if (EC_Normal == referencedPresentationLUTInstanceUID.getString(c)) return c; else return NULL;
}

E_Condition DVPSImageBoxContent::setMagnificationType(const char *value)
{
  if ((value==NULL)||(strlen(value)==0)) 
  {
    magnificationType.clear();
    return EC_Normal;
  }
  return magnificationType.putString(value);
}

E_Condition DVPSImageBoxContent::setSmoothingType(const char *value)
{
  if ((value==NULL)||(strlen(value)==0)) 
  {
    smoothingType.clear();
    return EC_Normal;
  }
  return smoothingType.putString(value);
}

E_Condition DVPSImageBoxContent::setConfigurationInformation(const char *value)
{
  if ((value==NULL)||(strlen(value)==0)) 
  {
    configurationInformation.clear();
    return EC_Normal;
  }
  return configurationInformation.putString(value);
}

E_Condition DVPSImageBoxContent::setSOPInstanceUID(const char *value)
{
  if ((value==NULL)||(strlen(value)==0)) return EC_IllegalCall;
  return sOPInstanceUID.putString(value);
}

const char *DVPSImageBoxContent::getSOPInstanceUID()
{
  char *c = NULL;
  if (EC_Normal == sOPInstanceUID.getString(c)) return c; else return NULL;
}

E_Condition DVPSImageBoxContent::setDefault()
{
  magnificationType.clear();
  smoothingType.clear();
  configurationInformation.clear();
  return EC_Normal;
}

OFBool DVPSImageBoxContent::hasAdditionalSettings()
{
  if (magnificationType.getLength() > 0) return OFTrue;
  if (smoothingType.getLength() > 0) return OFTrue;
  if (configurationInformation.getLength() > 0) return OFTrue;
  return OFFalse;
}

/*
 *  $Log: dvpsib.cc,v $
 *  Revision 1.15  2000-03-08 16:29:06  meichel
 *  Updated copyright header.
 *
 *  Revision 1.14  2000/03/03 14:14:00  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.13  2000/02/03 11:48:27  meichel
 *  Rebuilt data dictionary based on 1999 standard text,
 *    latest supplements and CP packet 6.
 *
 *  Revision 1.12  1999/10/07 17:21:58  meichel
 *  Reworked management of Presentation LUTs in order to create tighter
 *    coupling between Softcopy and Print.
 *
 *  Revision 1.11  1999/09/24 15:24:06  meichel
 *  Print spooler (dcmprtsv) now logs diagnostic messages in log files
 *    when operating in spool mode.
 *
 *  Revision 1.10  1999/09/17 14:33:51  meichel
 *  Completed print spool functionality including Supplement 22 support
 *
 *  Revision 1.9  1999/09/15 17:43:33  meichel
 *  Implemented print job dispatcher code for dcmpstat, adapted dcmprtsv
 *    and dcmpsprt applications.
 *
 *  Revision 1.8  1999/09/10 12:46:55  meichel
 *  Added implementations for a number of print API methods.
 *
 *  Revision 1.7  1999/09/09 14:57:50  thiel
 *  Added methods for print spooler
 *
 *  Revision 1.6  1999/09/08 16:46:14  meichel
 *  Fixed sequence tag used for writing image box references in Stored Print objects.
 *
 *  Revision 1.5  1999/09/01 16:15:08  meichel
 *  Added support for requested image size to print routines
 *
 *  Revision 1.4  1999/08/31 14:09:26  meichel
 *  Added get/set methods for stored print attributes
 *
 *  Revision 1.3  1999/08/27 15:57:49  meichel
 *  Added methods for saving hardcopy images and stored print objects
 *    either in file or in the local database.
 *
 *  Revision 1.2  1999/08/26 09:29:48  thiel
 *  Extensions for the usage of the StoredPrint
 *
 *  Revision 1.1  1999/07/30 13:34:56  meichel
 *  Added new classes managing Stored Print objects
 *
 *
 */

