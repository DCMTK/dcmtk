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
 *    classes: DVPSImageBoxContent
 *
 *  Last Update:      $Author: thiel $
 *  Update Date:      $Date: 1999-08-26 09:29:48 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dvpsib.h"

/* --------------- a few macros avoiding copy/paste --------------- */

#define ADD_TO_DATASET(a_type, a_name)                              \
if (result==EC_Normal)                                              \
{                                                                   \
  delem = new a_type(a_name);                                       \
  if (delem) dset.insert(delem); else result=EC_MemoryExhausted;    \
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
{
}

DVPSImageBoxContent::~DVPSImageBoxContent()
{
}

E_Condition DVPSImageBoxContent::read(DcmItem &dset)
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
  READ_FROM_DATASET(DcmShortText, configurationInformation)
  READ_FROM_DATASET(DcmCodeString, smoothingType)
  READ_FROM_DATASET(DcmDecimalString, requestedImageSize)
  READ_FROM_DATASET(DcmCodeString, requestedDecimateCropBehavior)

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
         READ_FROM_DATASET2(DcmLongString, patientID)
      } else {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: found ReferencedImageSequence in Stored Print Image Box with number of items != 1" << endl;
#endif
      } 
    }
  }
  
  /* the following attributes belong to the ReferencedImageSequence */

  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_ReferencedImageOverlayBoxSequence, stack, ESM_fromHere, OFFalse))
    {
      result=EC_IllegalCall;
#ifdef DEBUG
      cerr << "Error: Stored Print: ReferencedImageOverlayBoxSequence not supported" << endl;
#endif
    }
  }
  
  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_ReferencedPresentationLUTSequence, stack, ESM_fromHere, OFFalse))
    {
      result=EC_IllegalCall;
#ifdef DEBUG
      cerr << "Error: Stored Print: ReferencedPresentationLUTSequence not supported" << endl;
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
        cerr << "Error: SOPInstanceUID missing or incorrect in Stored Print Image Box" << endl;
#endif
    }
    if ((imageBoxPosition.getLength() == 0)||(imageBoxPosition.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: ImageBoxPosition missing or incorrect in Stored Print Image Box" << endl;
#endif
    }
    if (retrieveAETitle.getLength() == 0)
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: RetrieveAETitle missing in Stored Print Image Box" << endl;
#endif
    }
    if ((referencedSOPClassUID.getLength() == 0)||(referencedSOPClassUID.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: ReferencedSOPClassUID missing or incorrect in Stored Print Image Box" << endl;
#endif
    }
    if ((referencedSOPInstanceUID.getLength() == 0)||(referencedSOPInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: ReferencedSOPInstanceUID missing or incorrect in Stored Print Image Box" << endl;
#endif
    }
    if ((studyInstanceUID.getLength() == 0)||(studyInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: StudyInstanceUID missing or incorrect in Stored Print Image Box" << endl;
#endif
    }
    if ((seriesInstanceUID.getLength() == 0)||(seriesInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: SeriesInstanceUID missing or incorrect in Stored Print Image Box" << endl;
#endif
    }
    if (referencedFrameNumber.getVM() > 1)
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        cerr << "Error: ReferencedFrameNumber VM>1 in Stored Print Image Box" << endl;
#endif
    }
  }

  return result;
}

E_Condition DVPSImageBoxContent::write(DcmItem &dset)
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
    cerr << "Error: cannot write Stored Print Image Box: SOPInstanceUID empty" << endl;
#endif
  }
  if (imageBoxPosition.getLength() == 0)
  {
    result=EC_TagNotFound;
#ifdef DEBUG
    cerr << "Error: cannot write Stored Print Image Box: ImageBoxPosition empty" << endl;
#endif
  }
  if (retrieveAETitle.getLength() == 0)
  {
    result=EC_TagNotFound;
#ifdef DEBUG
    cerr << "Error: cannot write Stored Print Image Box: RetrieveAETitle empty" << endl;
#endif
  }
  if (referencedSOPClassUID.getLength() == 0)
  {
    result=EC_TagNotFound;
#ifdef DEBUG
    cerr << "Error: cannot write Stored Print Image Box: ReferencedSOPClassUID empty" << endl;
#endif
  }
  if (referencedSOPInstanceUID.getLength() == 0)
  {
    result=EC_TagNotFound;
#ifdef DEBUG
    cerr << "Error: cannot write Stored Print Image Box: ReferencedSOPInstanceUID empty" << endl;
#endif
  }
  if (studyInstanceUID.getLength() == 0)
  {
    result=EC_TagNotFound;
#ifdef DEBUG
    cerr << "Error: cannot write Stored Print Image Box: StudyInstanceUID empty" << endl;
#endif
  }
  if (seriesInstanceUID.getLength() == 0)
  {
    result=EC_TagNotFound;
#ifdef DEBUG
    cerr << "Error: cannot write Stored Print Image Box: SeriesInstanceUID empty" << endl;
#endif
  }

  ADD_TO_DATASET(DcmUniqueIdentifier, sOPInstanceUID)
  ADD_TO_DATASET(DcmUnsignedShort, imageBoxPosition)
  ADD_TO_DATASET(DcmCodeString, polarity)
  if (magnificationType.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, magnificationType) }
  if (configurationInformation.getLength() > 0) { ADD_TO_DATASET(DcmShortText, configurationInformation) }
  if (smoothingType.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, smoothingType) }
  if (requestedImageSize.getLength() > 0) { ADD_TO_DATASET(DcmDecimalString, requestedImageSize) }
  if (requestedDecimateCropBehavior.getLength() > 0) { ADD_TO_DATASET(DcmCodeString, requestedDecimateCropBehavior) }

  if (result == EC_Normal)
  {
    ditem = new DcmItem();
    if (ditem)
    {
      dseq = new DcmSequenceOfItems(DCM_PresentationLUTSequence);
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


E_Condition DVPSImageBoxContent::addImage(DcmItem & dset,char * aETitle,unsigned long number)
{

	DcmStack stack;
	E_Condition result=EC_Normal;

    result=retrieveAETitle.putString(aETitle);
	READ_FROM_DATASET(DcmUniqueIdentifier, studyInstanceUID)
    READ_FROM_DATASET(DcmUniqueIdentifier, seriesInstanceUID)
	//Add the reference of this Image
	stack.clear();
	dset.search(DCM_SOPClassUID,stack,ESM_fromHere,OFFalse);
	referencedSOPClassUID = *((DcmUniqueIdentifier *)(stack.top()));
	stack.clear();
	dset.search(DCM_SOPInstanceUID,stack,ESM_fromHere,OFFalse);
	referencedSOPInstanceUID = *((DcmUniqueIdentifier *)(stack.top()));

    READ_FROM_DATASET(DcmLongString, patientID)
    //READ_FROM_DATASET(DcmIntegerString, referencedFrameNumber)
 	createDefaultValues(true,number);

	return result;
}

/*
 *  $Log: dvpsib.cc,v $
 *  Revision 1.2  1999-08-26 09:29:48  thiel
 *  Extensions for the usage of the StoredPrint
 *
 *  Revision 1.1  1999/07/30 13:34:56  meichel
 *  Added new classes managing Stored Print objects
 *
 *
 */

