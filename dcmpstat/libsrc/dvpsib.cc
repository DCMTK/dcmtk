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
 *  Update Date:      $Date: 2000-06-02 16:01:02 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dvpsib.h"
#include "dvpsdef.h"     /* for constants and macros */
#include "dvpspll.h"
#include "dvpscf.h"
#include "dvpshlp.h"


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
, imageDepth(DVPSN_undefined)
, logstream(&ofConsole)
, verboseMode(OFFalse)
, debugMode(OFFalse)
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
, imageDepth(copy.imageDepth)
, logstream(copy.logstream)
, verboseMode(copy.verboseMode)
, debugMode(copy.debugMode)
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
  imageDepth = DVPSN_undefined;
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
        if (verboseMode)
        {
          logstream->lockCerr() << "Error: found ReferencedImageSequence in Stored Print Image Box with number of items != 1" << endl;
          logstream->unlockCerr();
        }
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
             if (verboseMode)
             {
               logstream->lockCerr() << "Error: ImageBoxContentSequence presentation LUT reference cannot be resolved" << endl;
               logstream->unlockCerr();
             }
           }
         }
      } else {
        result=EC_TagNotFound;
        if (verboseMode)
        {
          logstream->lockCerr() << "Error: found ImageBoxContentSequence in Stored Print with ReferencedPresentationLUTSequence number of items != 1" << endl;
          logstream->unlockCerr();
        }
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
      if (verboseMode)
      {
        logstream->lockCerr() << "Error: Stored Print: retired ReferencedImageOverlayBoxSequence not supported" << endl;
        logstream->unlockCerr();
      }

    }
  }

  /* Now perform basic sanity checks */

  if (result==EC_Normal)
  {
    if ((sOPInstanceUID.getLength() == 0)||(sOPInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
        if (verboseMode)
        {
          logstream->lockCerr() << "Error: SOPInstanceUID missing or incorrect in Stored Print Image Box" << endl;
          logstream->unlockCerr();
        }
    }
    if ((imageBoxPosition.getLength() == 0)||(imageBoxPosition.getVM() != 1))
    {
        result=EC_TagNotFound;
        if (verboseMode)
        {
          logstream->lockCerr() << "Error: ImageBoxPosition missing or incorrect in Stored Print Image Box" << endl;
          logstream->unlockCerr();
        }
    }
    if (retrieveAETitle.getLength() == 0)
    {
        result=EC_TagNotFound;
        if (verboseMode)
        {
          logstream->lockCerr() << "Error: RetrieveAETitle missing in Stored Print Image Box" << endl;
          logstream->unlockCerr();
        }
    }
    if ((referencedSOPClassUID.getLength() == 0)||(referencedSOPClassUID.getVM() != 1))
    {
        result=EC_TagNotFound;
        if (verboseMode)
        {
          logstream->lockCerr() << "Error: ReferencedSOPClassUID missing or incorrect in Stored Print Image Box" << endl;
          logstream->unlockCerr();
        }
    }
    if ((referencedSOPInstanceUID.getLength() == 0)||(referencedSOPInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
        if (verboseMode)
        {
          logstream->lockCerr() << "Error: ReferencedSOPInstanceUID missing or incorrect in Stored Print Image Box" << endl;
          logstream->unlockCerr();
        }
    }
    if ((studyInstanceUID.getLength() == 0)||(studyInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
        if (verboseMode)
        {
          logstream->lockCerr() << "Error: StudyInstanceUID missing or incorrect in Stored Print Image Box" << endl;
          logstream->unlockCerr();
        }
    }
    if ((seriesInstanceUID.getLength() == 0)||(seriesInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
        if (verboseMode)
        {
          logstream->lockCerr() << "Error: SeriesInstanceUID missing or incorrect in Stored Print Image Box" << endl;
          logstream->unlockCerr();
        }
    }
    if (referencedFrameNumber.getVM() > 1)
    {
        result=EC_TagNotFound;
        if (verboseMode)
        {
          logstream->lockCerr() << "Error: ReferencedFrameNumber VM>1 in Stored Print Image Box" << endl;
          logstream->unlockCerr();
        }
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
    if (verboseMode)
    {
      logstream->lockCerr() << "Error: cannot write Stored Print Image Box: SOPInstanceUID empty" << endl;
      logstream->unlockCerr();
    }
  }
  if (imageBoxPosition.getLength() == 0)
  {
    result=EC_TagNotFound;
    if (verboseMode)
    {
      logstream->lockCerr() << "Error: cannot write Stored Print Image Box: ImageBoxPosition empty" << endl;
      logstream->unlockCerr();
    }
  }
  if (retrieveAETitle.getLength() == 0)
  {
    result=EC_TagNotFound;
    if (verboseMode)
    {
      logstream->lockCerr() << "Error: cannot write Stored Print Image Box: RetrieveAETitle empty" << endl;
      logstream->unlockCerr();
    }    
  }
  if (referencedSOPClassUID.getLength() == 0)
  {
    result=EC_TagNotFound;
    if (verboseMode)
    {
      logstream->lockCerr() << "Error: cannot write Stored Print Image Box: ReferencedSOPClassUID empty" << endl;
      logstream->unlockCerr();
    }
  }
  if (referencedSOPInstanceUID.getLength() == 0)
  {
    result=EC_TagNotFound;
    if (verboseMode)
    {
      logstream->lockCerr() << "Error: cannot write Stored Print Image Box: ReferencedSOPInstanceUID empty" << endl;
      logstream->unlockCerr();
    }
  }
  if (studyInstanceUID.getLength() == 0)
  {
    result=EC_TagNotFound;
    if (verboseMode)
    {
      logstream->lockCerr() << "Error: cannot write Stored Print Image Box: StudyInstanceUID empty" << endl;
      logstream->unlockCerr();
    }
  }
  if (seriesInstanceUID.getLength() == 0)
  {
    result=EC_TagNotFound;
    if (verboseMode)
    {
      logstream->lockCerr() << "Error: cannot write Stored Print Image Box: SeriesInstanceUID empty" << endl;
      logstream->unlockCerr();
    }
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

E_Condition DVPSImageBoxContent::createDefaultValues(OFBool renumber, unsigned long number, OFBool ignoreEmptyImages)
{
  E_Condition result = EC_Normal;
  char uid[100];
  OFString aString;
  OFBool isEmpty = OFFalse;
  
  SET_UID(sOPInstanceUID)

  if (renumber)
  {
    imageBoxPosition.clear();
    result = imageBoxPosition.putUint16((Uint16)number, 0);
  } else {
    if (imageBoxPosition.getVM() != 1) isEmpty = OFTrue;
    if ((!ignoreEmptyImages)&& isEmpty) result = EC_TagNotFound;
  }

  if ((EC_Normal == result)&&((! isEmpty)||(! ignoreEmptyImages)))
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

OFBool DVPSImageBoxContent::matchesPresentationLUT(DVPSPrintPresentationLUTAlignment align)
{
  OFBool result = OFFalse;
  switch (align)
  {
    case DVPSK_shape: // always matches
      result = OFTrue;
      break;
    case DVPSK_table8:
      if (imageDepth != DVPSN_12bit) result = OFTrue;
      break;
    case DVPSK_table12:
      if (imageDepth != DVPSN_8bit) result = OFTrue;
      break;
    case DVPSK_other: // never matches
      break;
  }
  return result;
}


OFBool DVPSImageBoxContent::printSCPSet(
  DVConfiguration& cfg,
  const char *cfgname,
  DcmDataset *rqDataset,
  T_DIMSE_Message& rsp,
  DcmDataset *& rspDataset,
  DcmDataset &imageDataset,
  DVPSPrintPresentationLUTAlignment align)
{
  OFBool result = OFTrue;
  DcmStack stack;
  E_Condition writeresult = EC_Normal;
  DcmElement *delem = NULL;

  rspDataset = new DcmDataset;
  if ((rqDataset == NULL)||(rspDataset == NULL))
  {
    rsp.msg.NSetRSP.DimseStatus = DIMSE_N_ProcessingFailure;
    result = OFFalse;
  }

  // image(Box)Position
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)imageBoxPosition.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      imageBoxPosition = *((DcmUnsignedShort *)(stack.top()));
      // the image box position is checked elsewhere
      ADD_TO_PDATASET(DcmUnsignedShort, imageBoxPosition)
    } else {
      if (verboseMode)
      {
        logstream->lockCerr() << "cannot update Basic Grayscale Image Box: attribute (2020,0010) Image Position missing." << endl;
        logstream->unlockCerr();
      }
      rsp.msg.NSetRSP.DimseStatus = DIMSE_N_MissingAttribute;
      result = OFFalse;
    }
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
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Grayscale Image Box: illegal magnification type: '" << theMagnification.c_str() << "'" << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
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
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Grayscale Image Box: smoothing type requested but not supported." << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_AttributeListError;
        result = OFFalse;
      }
      else if (! found)
      {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Grayscale Image Box: illegal smoothing type: '" << theSmoothing.c_str() << "'" << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
        result = OFFalse;
      }
      else
      {
        ADD_TO_PDATASET(DcmCodeString, smoothingType)
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
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Grayscale Image Box: configuration information requested but not supported." << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_AttributeListError;
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
          if (verboseMode)
          {
            logstream->lockCerr() << "cannot update Basic Grayscale Image Box: illegal configuration information: '" << theConfiguration.c_str() << "'" << endl;
            logstream->unlockCerr();
          }
          rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
          result = OFFalse;
        }
        else
        {
          ADD_TO_PDATASET(DcmShortText, configurationInformation)
        }
      }
    }
  }

  // polarity
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)polarity.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      polarity = *((DcmCodeString *)(stack.top()));
      OFString thePolarity;
      polarity.getOFString(thePolarity, 0, OFTrue);
      if ((thePolarity != "NORMAL")&&(thePolarity != "REVERSE"))
      {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Grayscale Image Box: illegal polarity: '" << thePolarity.c_str() << "'" << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
        result = OFFalse;
      } else {
        ADD_TO_PDATASET(DcmCodeString, polarity)
      }
    }
  }

  // requestedImageSize
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)requestedImageSize.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      if (! cfg.getTargetPrinterSupportsRequestedImageSize(cfgname))
      {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Grayscale Image Box: requested image size not supported." << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_AttributeListError;
        result = OFFalse;
      }
      else
      {
        requestedImageSize = *((DcmDecimalString *)(stack.top()));
        // we don't check a requested image size set by the user (for now)
        ADD_TO_PDATASET(DcmDecimalString, requestedImageSize)
      }
    }
  }

  // requestedDecimateCropBehavior
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)requestedDecimateCropBehavior.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      if (! cfg.getTargetPrinterSupportsDecimateCrop(cfgname))
      {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Grayscale Image Box: requested decimate/crop behaviour not supported." << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_AttributeListError;
        result = OFFalse;
      }
      else
      {
        requestedDecimateCropBehavior = *((DcmCodeString *)(stack.top()));

        OFString theBehaviour;
        polarity.getOFString(theBehaviour, 0, OFTrue);
        if ((theBehaviour != "DECIMATE")&&(theBehaviour != "CROP")&&(theBehaviour != "FAIL"))
        {
          if (verboseMode)
          {
            logstream->lockCerr() << "cannot update Basic Grayscale Image Box: illegal decimate/crop behaviour: '" << theBehaviour.c_str() << "'" << endl;
            logstream->unlockCerr();
          }
          rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
          result = OFFalse;
        } else {
          ADD_TO_PDATASET(DcmCodeString, requestedDecimateCropBehavior)
        }
      }
    }
  }

  // basicGrayscaleImageSequence
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search(DCM_BasicGrayscaleImageSequence, stack, ESM_fromHere, OFFalse)))
    {
      DcmSequenceOfItems *seq=(DcmSequenceOfItems *)stack.top();
      if (seq->card() ==1)
      {
         DcmItem *item = seq->getItem(0);
         result = printSCPEvaluateBasicGrayscaleImageSequence(cfg, cfgname, item, rsp, imageDataset, align);
      } else {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Grayscale Image Box: basic grayscale image sequence number of items != 1" << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
        result = OFFalse;
      }
    } else {
      if (verboseMode)
      {
        logstream->lockCerr() << "cannot update Basic Grayscale Image Box: basic grayscale image sequence missing." << endl;
        logstream->unlockCerr();
      }
      rsp.msg.NSetRSP.DimseStatus = DIMSE_N_MissingAttribute;
      result = OFFalse;
    }
  }

  // browse through rqDataset and check for unsupported attributes
  if (result && rqDataset)
  {
    stack.clear();
    while (EC_Normal == rqDataset->nextObject(stack, OFFalse))
    {
      const DcmTagKey& currentTag = (stack.top())->getTag();
      if (currentTag == DCM_ImageBoxPosition) /* OK */ ;
      else if (currentTag == DCM_Polarity) /* OK */ ;
      else if (currentTag == DCM_RequestedDecimateCropBehavior) /* OK */ ;
      else if (currentTag == DCM_RequestedImageSize) /* OK */ ;
      else if (currentTag == DCM_MagnificationType) /* OK */ ;
      else if (currentTag == DCM_SmoothingType) /* OK */ ;
      else if (currentTag == DCM_ConfigurationInformation) /* OK */ ;
      else if (currentTag == DCM_BasicGrayscaleImageSequence) /* OK */ ;
      else
      {
        if (verboseMode)
        {
          ostream &mycerr = logstream->lockCerr();
          mycerr << "cannot update Basic Grayscale Image Box: unsupported attribute received:" << endl;
          (stack.top())->print(mycerr, OFFalse);
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_AttributeListError;
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
    if (rsp.msg.NSetRSP.DimseStatus == 0) rsp.msg.NSetRSP.DimseStatus = DIMSE_N_ProcessingFailure;
    result = OFFalse;
  }
  return result;
}


OFBool DVPSImageBoxContent::printSCPEvaluateBasicGrayscaleImageSequence(
  DVConfiguration& cfg,
  const char *cfgname,
  DcmItem *rqDataset,
  T_DIMSE_Message& rsp,
  DcmDataset &imageDataset,
  DVPSPrintPresentationLUTAlignment align)
{
  OFBool result = OFTrue;
  DcmStack stack;
  E_Condition writeresult = EC_Normal;
  DcmElement *delem = NULL;
  DcmDataset *rspDataset = &imageDataset;

  if (rqDataset == NULL)
  {
    rsp.msg.NSetRSP.DimseStatus = DIMSE_N_ProcessingFailure;
    result = OFFalse;
  }

  DcmUnsignedShort samplesPerPixel(DCM_SamplesPerPixel);
  DcmCodeString photometricInterpretation(DCM_PhotometricInterpretation);
  DcmUnsignedShort rows(DCM_Rows);
  DcmUnsignedShort columns(DCM_Columns);
  DcmIntegerString pixelAspectRatio(DCM_PixelAspectRatio);
  DcmUnsignedShort bitsAllocated(DCM_BitsAllocated);
  DcmUnsignedShort bitsStored(DCM_BitsStored);
  DcmUnsignedShort highBit(DCM_HighBit);
  DcmUnsignedShort pixelRepresentation(DCM_PixelRepresentation);
  DcmPixelData *pixelData = NULL;
  Uint16 val=0;
  char newuid[70];
  OFString aString;
    
  // samplesPerPixel
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)samplesPerPixel.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      samplesPerPixel = *((DcmUnsignedShort *)(stack.top()));
      val = 0;
      if ((EC_Normal == samplesPerPixel.getUint16(val,0)) && (val == 1))
      {
        ADD_TO_PDATASET(DcmUnsignedShort, samplesPerPixel)
      } else {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Grayscale Image Box: illegal samples per pixel value: " << val << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
        result = OFFalse;
      }
    } else {
      if (verboseMode)
      {
        logstream->lockCerr() << "cannot update Basic Grayscale Image Box: samples per pixel missing in basic grayscale image sequence" << endl;
        logstream->unlockCerr();
      }
      rsp.msg.NSetRSP.DimseStatus = DIMSE_N_MissingAttribute;
      result = OFFalse;
    }
  }

  // rows
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)rows.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      rows = *((DcmUnsignedShort *)(stack.top()));
      val = 0;
      if ((EC_Normal == rows.getUint16(val,0)) && (val > 0))
      {
        ADD_TO_PDATASET(DcmUnsignedShort, rows)
      } else {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Grayscale Image Box: illegal rows value: " << val << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
        result = OFFalse;
      }
    } else {
      if (verboseMode)
      {
        logstream->lockCerr() << "cannot update Basic Grayscale Image Box: rows missing in basic grayscale image sequence" << endl;
        logstream->unlockCerr();
      }
      rsp.msg.NSetRSP.DimseStatus = DIMSE_N_MissingAttribute;
      result = OFFalse;
    }
  }

  // columns
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)columns.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      columns = *((DcmUnsignedShort *)(stack.top()));
      val = 0;
      if ((EC_Normal == columns.getUint16(val,0)) && (val > 0))
      {
        ADD_TO_PDATASET(DcmUnsignedShort, columns)
      } else {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Grayscale Image Box: illegal columns value: " << val << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
        result = OFFalse;
      }
    } else {
      if (verboseMode)
      {
        logstream->lockCerr() << "cannot update Basic Grayscale Image Box: columns missing in basic grayscale image sequence" << endl;
        logstream->unlockCerr();
      }
      rsp.msg.NSetRSP.DimseStatus = DIMSE_N_MissingAttribute;
      result = OFFalse;
    }
  }

  Uint16 bitsStoredValue = 0;
  
  // bitsStored
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)bitsStored.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      bitsStored = *((DcmUnsignedShort *)(stack.top()));
      val = 0;
      if ((EC_Normal == bitsStored.getUint16(val,0)) && ((val == 8) || (val == 12)))
      {
        bitsStoredValue = val;
        if (bitsStoredValue==8) imageDepth = DVPSN_8bit; else imageDepth = DVPSN_12bit;
        OFBool supports12Bit = cfg.getTargetPrinterSupports12BitTransmission(cfgname);
        if ((bitsStoredValue == 12)&&(! supports12Bit))
        {
          if (verboseMode)
          {
            logstream->lockCerr() << "cannot update Basic Grayscale Image Box: image transmission with 12 bits/pixel not supported." << endl;
            logstream->unlockCerr();
          }
          rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
          result = OFFalse;
        } else {
          if ((cfg.getTargetPrinterPresentationLUTMatchRequired(cfgname)) && (! matchesPresentationLUT(align)))
          {
            if (verboseMode)
            {
              logstream->lockCerr() << "cannot update Basic Grayscale Image Box: image data with " << bitsStoredValue << " bits/pixel does not match characteristics of active Presentation LUT." << endl;
              logstream->unlockCerr();
            }
            rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
            result = OFFalse;
          } else {
            ADD_TO_PDATASET(DcmUnsignedShort, bitsStored)
          }
        }      
      } else {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Grayscale Image Box: illegal bits stored value: " << val << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
        result = OFFalse;
      }
    } else {
      if (verboseMode)
      {
        logstream->lockCerr() << "cannot update Basic Grayscale Image Box: bits stored missing in basic grayscale image sequence" << endl;
        logstream->unlockCerr();
      }
      rsp.msg.NSetRSP.DimseStatus = DIMSE_N_MissingAttribute;
      result = OFFalse;
    }
  }

  // bitsAllocated
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)bitsAllocated.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      bitsAllocated = *((DcmUnsignedShort *)(stack.top()));
      val = 0;
      if ((EC_Normal == bitsAllocated.getUint16(val,0)) && (((val == 8)&&(bitsStoredValue == 8)) || ((val == 16)&&(bitsStoredValue == 12))))
      {
        ADD_TO_PDATASET(DcmUnsignedShort, bitsAllocated)
      } else {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Grayscale Image Box: illegal bits allocated value: " << val << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
        result = OFFalse;
      }
    } else {
      if (verboseMode)
      {
        logstream->lockCerr() << "cannot update Basic Grayscale Image Box: bits allocated missing in basic grayscale image sequence" << endl;
        logstream->unlockCerr();
      }
      rsp.msg.NSetRSP.DimseStatus = DIMSE_N_MissingAttribute;
      result = OFFalse;
    }
  }

  // highBit
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)highBit.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      highBit = *((DcmUnsignedShort *)(stack.top()));
      val = 0;
      if ((EC_Normal == highBit.getUint16(val,0)) && (((val == 7)&&(bitsStoredValue == 8)) || ((val == 11)&&(bitsStoredValue == 12))))
      {
        ADD_TO_PDATASET(DcmUnsignedShort, highBit)
      } else {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Grayscale Image Box: illegal high bit value: " << val << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
        result = OFFalse;
      }
    } else {
      if (verboseMode)
      {
        logstream->lockCerr() << "cannot update Basic Grayscale Image Box: high bit missing in basic grayscale image sequence" << endl;
        logstream->unlockCerr();
      }
      rsp.msg.NSetRSP.DimseStatus = DIMSE_N_MissingAttribute;
      result = OFFalse;
    }
  }

  // pixelRepresentation
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)pixelRepresentation.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      pixelRepresentation = *((DcmUnsignedShort *)(stack.top()));
      val = 0;
      if ((EC_Normal == pixelRepresentation.getUint16(val,0)) && (val == 0))
      {
        ADD_TO_PDATASET(DcmUnsignedShort, pixelRepresentation)
      } else {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Grayscale Image Box: illegal pixel representation value: " << val << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
        result = OFFalse;
      }
    } else {
      if (verboseMode)
      {
        logstream->lockCerr() << "cannot update Basic Grayscale Image Box: pixel representation missing in basic grayscale image sequence" << endl;
        logstream->unlockCerr();
      }
      rsp.msg.NSetRSP.DimseStatus = DIMSE_N_MissingAttribute;
      result = OFFalse;
    }
  }

  // photometricInterpretation
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)photometricInterpretation.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      photometricInterpretation = *((DcmCodeString *)(stack.top()));
      OFString theColorModel;
      photometricInterpretation.getOFString(theColorModel, 0, OFTrue);
      if ((theColorModel != "MONOCHROME1")&&(theColorModel != "MONOCHROME2"))
      {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Grayscale Image Box: illegal photometric interpretation: '" << theColorModel.c_str() << "'" << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
        result = OFFalse;
      } else {
        ADD_TO_PDATASET(DcmCodeString, photometricInterpretation)
      }
    } else {
      if (verboseMode)
      {
        logstream->lockCerr() << "cannot update Basic Grayscale Image Box: photometric interpretation missing in basic grayscale image sequence" << endl;
        logstream->unlockCerr();
      }
      rsp.msg.NSetRSP.DimseStatus = DIMSE_N_MissingAttribute;
      result = OFFalse;
    }
  }

  // pixelAspectRatio
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)pixelAspectRatio.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      pixelAspectRatio = *((DcmIntegerString *)(stack.top()));
      if (pixelAspectRatio.getVM() != 2)
      {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Grayscale Image Box: illegal pixel aspect ratio, VM=: '" << pixelAspectRatio.getVM() << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
        result = OFFalse;
      } else {
        ADD_TO_PDATASET(DcmIntegerString, pixelAspectRatio)
      }
    }
  }

  // pixelData
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search(DCM_PixelData, stack, ESM_fromHere, OFFalse)))
    {
      pixelData = new DcmPixelData(DCM_PixelData);
      if (pixelData)
      {
        DcmElement *oldPxData = (DcmElement *)(stack.top());
        Uint16 *pxdata16 = NULL;
        Uint8 *pxdata8 = NULL;
        if ((EC_Normal == oldPxData->getUint16Array(pxdata16)) && pxdata16)
        {
          pixelData->putUint16Array(pxdata16, oldPxData->getLength()/sizeof(Uint16));
          rspDataset->insert(pixelData);         	
        } 
        else if ((EC_Normal == oldPxData->getUint8Array(pxdata8)) && pxdata8)
        {
          pixelData->putUint8Array(pxdata8, oldPxData->getLength()/sizeof(Uint8));
          rspDataset->insert(pixelData);         	
        } else {
          if (verboseMode)
          {
            logstream->lockCerr() << "cannot update Basic Grayscale Image Box: cannot access pixel data" << endl;
            logstream->unlockCerr();
          }
          rsp.msg.NSetRSP.DimseStatus = DIMSE_N_ProcessingFailure;
          result = OFFalse;    
        }
      } else writeresult=EC_MemoryExhausted; 
    } else {
      if (verboseMode)
      {
        logstream->lockCerr() << "cannot update Basic Grayscale Image Box: pixel data missing in basic grayscale image sequence" << endl;
        logstream->unlockCerr();
      }
      rsp.msg.NSetRSP.DimseStatus = DIMSE_N_MissingAttribute;
      result = OFFalse;
    }
  }

  // browse through rqDataset and check for unsupported attributes
  if (result && rqDataset)
  {
    stack.clear();
    while (EC_Normal == rqDataset->nextObject(stack, OFFalse))
    {
      const DcmTagKey& currentTag = (stack.top())->getTag();
      if (currentTag == DCM_SamplesPerPixel) /* OK */ ;
      else if (currentTag == DCM_PhotometricInterpretation) /* OK */ ;
      else if (currentTag == DCM_Rows) /* OK */ ;
      else if (currentTag == DCM_Columns) /* OK */ ;
      else if (currentTag == DCM_PixelAspectRatio) /* OK */ ;
      else if (currentTag == DCM_BitsAllocated) /* OK */ ;
      else if (currentTag == DCM_BitsStored) /* OK */ ;
      else if (currentTag == DCM_HighBit) /* OK */ ;
      else if (currentTag == DCM_PixelRepresentation) /* OK */ ;
      else if (currentTag == DCM_PixelData) /* OK */ ;
      else
      {
        if (verboseMode)
        {
          ostream &mycerr = logstream->lockCerr();
          mycerr << "cannot update Basic Grayscale Image Box: unsupported attribute in basic grayscale image sequence:" << endl;
          (stack.top())->print(mycerr, OFFalse);
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_AttributeListError;
        result = OFFalse;
      }
    }
  }

  // if n-set was successful, send back response dataset
  if (result && (EC_Normal == writeresult))
  {
    // complete Hardcopy Grayscale Image
    
    // write patient module
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_PatientsName, DEFAULT_patientName);
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_PatientID);
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_PatientsBirthDate);
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_PatientsSex);

    // general study and general series modules are written somewhere else

    // Hardcopy Equipment Module
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_HardcopyDeviceManufacturer, "OFFIS");
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_HardcopyDeviceSoftwareVersion, OFFIS_DTK_IMPLEMENTATION_VERSION_NAME);

    // General Image Module
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_InstanceNumber);
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_PatientOrientation);
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_ImageType, "DERIVED\\SECONDARY");
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_DerivationDescription, "Hardcopy");

    // SOP Common Module
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_SOPClassUID, UID_HardcopyGrayscaleImageStorage);
    if (EC_Normal==writeresult) writeresult = referencedSOPClassUID.putString(UID_HardcopyGrayscaleImageStorage);
    dcmGenerateUniqueIdentifer(newuid);
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_SOPInstanceUID, newuid);
    if (EC_Normal==writeresult) writeresult = referencedSOPInstanceUID.putString(newuid);
    DVPSHelper::currentDate(aString);
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_InstanceCreationDate, aString.c_str());
    DVPSHelper::currentTime(aString);
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_InstanceCreationTime, aString.c_str());

  } else result = OFFalse;
  return result;
}

Uint16 DVPSImageBoxContent::getImageBoxPosition()
{
  Uint16 result = 0;
  if (EC_Normal == imageBoxPosition.getUint16(result, 0)) return result; else return 0;
}

E_Condition DVPSImageBoxContent::setUIDsAndAETitle(DcmUniqueIdentifier& studyUID, DcmUniqueIdentifier& seriesUID, const char *aetitle)
{
  if (aetitle==NULL) return EC_IllegalCall;
  
  studyInstanceUID = studyUID;
  seriesInstanceUID = seriesUID;
  return retrieveAETitle.putString(aetitle);
}

void DVPSImageBoxContent::setLog(OFConsole *stream, OFBool verbMode, OFBool dbgMode)
{
  if (stream) logstream = stream; else logstream = &ofConsole;
  verboseMode = verbMode;
  debugMode = dbgMode;
}


/*
 *  $Log: dvpsib.cc,v $
 *  Revision 1.17  2000-06-02 16:01:02  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.16  2000/05/31 12:58:15  meichel
 *  Added initial Print SCP support
 *
 *  Revision 1.15  2000/03/08 16:29:06  meichel
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

