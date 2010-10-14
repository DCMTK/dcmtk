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
 *    classes: DVPSImageBoxContent
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:32 $
 *  CVS/RCS Revision: $Revision: 1.40 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmpstat/dvpsib.h"
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */
#include "dcmtk/dcmpstat/dvpspll.h"
#include "dcmtk/dcmpstat/dvpscf.h"
#include "dcmtk/dcmpstat/dvpshlp.h"
#include "dcmtk/dcmpstat/dvpspl.h"      /* for DVPSPresentationLUT, needed by MSVC5 with STL */


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

OFCondition DVPSImageBoxContent::setContent(
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
  OFCondition result = EC_Normal;
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

OFCondition DVPSImageBoxContent::read(DcmItem &dset, DVPSPresentationLUT_PList& presentationLUTList)
{
  DcmSequenceOfItems *seq;
  DcmItem *item;
  OFCondition result = EC_Normal;
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
        DCMPSTAT_INFO("found ReferencedImageSequence in Stored Print Image Box with number of items != 1");
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
             DCMPSTAT_INFO("ImageBoxContentSequence presentation LUT reference cannot be resolved");
           }
         }
      } else {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("found ImageBoxContentSequence in Stored Print with ReferencedPresentationLUTSequence number of items != 1");
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
      DCMPSTAT_INFO("Stored Print: retired ReferencedImageOverlayBoxSequence not supported");

    }
  }

  /* Now perform basic sanity checks */

  if (result==EC_Normal)
  {
    if ((sOPInstanceUID.getLength() == 0)||(sOPInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("SOPInstanceUID missing or incorrect in Stored Print Image Box");
    }
    if ((imageBoxPosition.getLength() == 0)||(imageBoxPosition.getVM() != 1))
    {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("ImageBoxPosition missing or incorrect in Stored Print Image Box");
    }
    if (retrieveAETitle.getLength() == 0)
    {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("RetrieveAETitle missing in Stored Print Image Box");
    }
    if ((referencedSOPClassUID.getLength() == 0)||(referencedSOPClassUID.getVM() != 1))
    {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("ReferencedSOPClassUID missing or incorrect in Stored Print Image Box");
    }
    if ((referencedSOPInstanceUID.getLength() == 0)||(referencedSOPInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("ReferencedSOPInstanceUID missing or incorrect in Stored Print Image Box");
    }
    if ((studyInstanceUID.getLength() == 0)||(studyInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("StudyInstanceUID missing or incorrect in Stored Print Image Box");
    }
    if ((seriesInstanceUID.getLength() == 0)||(seriesInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("SeriesInstanceUID missing or incorrect in Stored Print Image Box");
    }
    if (referencedFrameNumber.getVM() > 1)
    {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("ReferencedFrameNumber VM>1 in Stored Print Image Box");
    }
  }

  return result;
}

OFCondition DVPSImageBoxContent::write(DcmItem &dset, OFBool writeRequestedImageSize, OFBool writeReferencedPLUTSQ)
{
  OFCondition result = EC_Normal;
  DcmElement *delem=NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  /* before writing anything, check that we are able to write a correct item */
  if (sOPInstanceUID.getLength() == 0)
  {
    result=EC_TagNotFound;
    DCMPSTAT_INFO("cannot write Stored Print Image Box: SOPInstanceUID empty");
  }
  if (imageBoxPosition.getLength() == 0)
  {
    result=EC_TagNotFound;
    DCMPSTAT_INFO("cannot write Stored Print Image Box: ImageBoxPosition empty");
  }
  if (retrieveAETitle.getLength() == 0)
  {
    result=EC_TagNotFound;
    DCMPSTAT_INFO("cannot write Stored Print Image Box: RetrieveAETitle empty");
  }
  if (referencedSOPClassUID.getLength() == 0)
  {
    result=EC_TagNotFound;
    DCMPSTAT_INFO("cannot write Stored Print Image Box: ReferencedSOPClassUID empty");
  }
  if (referencedSOPInstanceUID.getLength() == 0)
  {
    result=EC_TagNotFound;
    DCMPSTAT_INFO("cannot write Stored Print Image Box: ReferencedSOPInstanceUID empty");
  }
  if (studyInstanceUID.getLength() == 0)
  {
    result=EC_TagNotFound;
    DCMPSTAT_INFO("cannot write Stored Print Image Box: StudyInstanceUID empty");
  }
  if (seriesInstanceUID.getLength() == 0)
  {
    result=EC_TagNotFound;
    DCMPSTAT_INFO("cannot write Stored Print Image Box: SeriesInstanceUID empty");
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
        if (delem) ditem->insert(delem, OFTrue /*replaceOld*/); else result=EC_MemoryExhausted;
        delem = new DcmUniqueIdentifier(referencedSOPClassUID);
        if (delem) ditem->insert(delem, OFTrue /*replaceOld*/); else result=EC_MemoryExhausted;
        delem = new DcmUniqueIdentifier(referencedSOPInstanceUID);
        if (delem) ditem->insert(delem, OFTrue /*replaceOld*/); else result=EC_MemoryExhausted;
        delem = new DcmUniqueIdentifier(studyInstanceUID);
        if (delem) ditem->insert(delem, OFTrue /*replaceOld*/); else result=EC_MemoryExhausted;
        delem = new DcmUniqueIdentifier(seriesInstanceUID);
        if (delem) ditem->insert(delem, OFTrue /*replaceOld*/); else result=EC_MemoryExhausted;
        if (referencedFrameNumber.getLength() >0)
        {
          delem = new DcmIntegerString(referencedFrameNumber);
          if (delem) ditem->insert(delem, OFTrue /*replaceOld*/); else result=EC_MemoryExhausted;
        }
        delem = new DcmLongString(patientID);
        if (delem) ditem->insert(delem, OFTrue /*replaceOld*/); else result=EC_MemoryExhausted;
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

  if ((EC_Normal == result) && writeReferencedPLUTSQ)
    result = addReferencedPLUTSQ(dset);

  return result;
}

OFCondition DVPSImageBoxContent::addReferencedPLUTSQ(DcmItem &dset)
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

OFCondition DVPSImageBoxContent::prepareBasicImageBox(DcmItem &dset)
{
  OFCondition result = EC_Normal;
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

OFCondition DVPSImageBoxContent::createDefaultValues(OFBool renumber, unsigned long number, OFBool ignoreEmptyImages)
{
  OFCondition result = EC_Normal;
  char uid[100];
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

OFCondition DVPSImageBoxContent::setRequestedDecimateCropBehaviour(DVPSDecimateCropBehaviour value)
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

OFCondition DVPSImageBoxContent::getImageReference(
  const char *&studyUID,
  const char *&seriesUID,
  const char *&instanceUID)
{
  OFCondition result=EC_Normal;
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

const char *DVPSImageBoxContent::getPolarity()
{
  char *c = NULL;
  if (EC_Normal == polarity.getString(c)) return c; else return NULL;
}

const char *DVPSImageBoxContent::getRequestedImageSize()
{
  char *c = NULL;
  if (EC_Normal == requestedImageSize.getString(c)) return c; else return NULL;
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

OFCondition DVPSImageBoxContent::setPolarity(const char *value)
{
  OFCondition result = EC_IllegalCall;
  if ((value==NULL)||(strlen(value)==0))
  {
    polarity.clear();
    result = EC_Normal;
  } else {
    if ((result = polarity.putString(value)) == EC_Normal)
    {
      OFString str;
      if ((result = polarity.getOFString(str, 0, OFTrue)) == EC_Normal)
      {
      	if ((str == "NORMAL") || (str == "REVERSE"))
      	  result = EC_Normal;
      }
    }
  }
  return result;
}

OFCondition DVPSImageBoxContent::setRequestedImageSize(const char *value)
{
  if ((value==NULL)||(strlen(value)==0))
  {
    requestedImageSize.clear();
    return EC_Normal;
  }
  return requestedImageSize.putString(value);
}

OFCondition DVPSImageBoxContent::setMagnificationType(const char *value)
{
  if ((value==NULL)||(strlen(value)==0))
  {
    magnificationType.clear();
    return EC_Normal;
  }
  return magnificationType.putString(value);
}

OFCondition DVPSImageBoxContent::setSmoothingType(const char *value)
{
  if ((value==NULL)||(strlen(value)==0))
  {
    smoothingType.clear();
    return EC_Normal;
  }
  return smoothingType.putString(value);
}

OFCondition DVPSImageBoxContent::setConfigurationInformation(const char *value)
{
  if ((value==NULL)||(strlen(value)==0))
  {
    configurationInformation.clear();
    return EC_Normal;
  }
  return configurationInformation.putString(value);
}

OFCondition DVPSImageBoxContent::setSOPInstanceUID(const char *value)
{
  if ((value==NULL)||(strlen(value)==0)) return EC_IllegalCall;
  return sOPInstanceUID.putString(value);
}

const char *DVPSImageBoxContent::getSOPInstanceUID()
{
  char *c = NULL;
  if (EC_Normal == sOPInstanceUID.getString(c)) return c; else return NULL;
}

OFCondition DVPSImageBoxContent::setDefault()
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

OFBool DVPSImageBoxContent::matchesPresentationLUT(DVPSPrintPresentationLUTAlignment align) const
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
  DVPSPrintPresentationLUTAlignment align,
  OFBool presentationLUTnegotiated)
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
      DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: attribute (2020,0010) Image Position missing.");
      rsp.msg.NSetRSP.DimseStatus = STATUS_N_MissingAttribute;
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
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: illegal magnification type: '" << theMagnification.c_str() << "'");
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
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: smoothing type requested but not supported.");
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      }
      else if (! found)
      {
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: illegal smoothing type: '" << theSmoothing.c_str() << "'");
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
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
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: configuration information requested but not supported.");
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
          DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: illegal configuration information: '" << theConfiguration.c_str() << "'");
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
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: illegal polarity: '" << thePolarity.c_str() << "'");
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
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
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: requested image size not supported.");
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;
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
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: requested decimate/crop behaviour not supported.");
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      }
      else
      {
        requestedDecimateCropBehavior = *((DcmCodeString *)(stack.top()));

        OFString theBehaviour;
        requestedDecimateCropBehavior.getOFString(theBehaviour, 0, OFTrue);
        if ((theBehaviour != "DECIMATE")&&(theBehaviour != "CROP")&&(theBehaviour != "FAIL"))
        {
          DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: illegal decimate/crop behaviour: '" << theBehaviour.c_str() << "'");
          rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
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
         result = printSCPEvaluateBasicGrayscaleImageSequence(cfg, cfgname, item, rsp, imageDataset, align, presentationLUTnegotiated);
      } else {
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: basic grayscale image sequence number of items != 1");
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    } else {
      DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: basic grayscale image sequence missing.");
      rsp.msg.NSetRSP.DimseStatus = STATUS_N_MissingAttribute;
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
      else if (currentTag == DCM_ImageBoxPosition) /* OK */ ;
      else if (currentTag == DCM_Polarity) /* OK */ ;
      else if (currentTag == DCM_RequestedDecimateCropBehavior) /* OK */ ;
      else if (currentTag == DCM_RequestedImageSize) /* OK */ ;
      else if (currentTag == DCM_MagnificationType) /* OK */ ;
      else if (currentTag == DCM_SmoothingType) /* OK */ ;
      else if (currentTag == DCM_ConfigurationInformation) /* OK */ ;
      else if (currentTag == DCM_BasicGrayscaleImageSequence) /* OK */ ;
      else
      {
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: unsupported attribute received:\n"
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


OFBool DVPSImageBoxContent::printSCPEvaluateBasicGrayscaleImageSequence(
  DVConfiguration& cfg,
  const char *cfgname,
  DcmItem *rqDataset,
  T_DIMSE_Message& rsp,
  DcmDataset &imageDataset,
  DVPSPrintPresentationLUTAlignment align,
  OFBool presentationLUTnegotiated)
{
  OFBool result = OFTrue;
  DcmStack stack;
  OFCondition writeresult = EC_Normal;
  DcmElement *delem = NULL;
  DcmDataset *rspDataset = &imageDataset;

  if (rqDataset == NULL)
  {
    rsp.msg.NSetRSP.DimseStatus = STATUS_N_ProcessingFailure;
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
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: illegal samples per pixel value: " << val);
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    } else {
      DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: samples per pixel missing in basic grayscale image sequence");
      rsp.msg.NSetRSP.DimseStatus = STATUS_N_MissingAttribute;
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
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: illegal rows value: " << val);
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    } else {
      DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: rows missing in basic grayscale image sequence");
      rsp.msg.NSetRSP.DimseStatus = STATUS_N_MissingAttribute;
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
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: illegal columns value: " << val);
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    } else {
      DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: columns missing in basic grayscale image sequence");
      rsp.msg.NSetRSP.DimseStatus = STATUS_N_MissingAttribute;
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
          DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: image transmission with 12 bits/pixel not supported.");
          rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
          result = OFFalse;
        } else {
          if (presentationLUTnegotiated && (cfg.getTargetPrinterPresentationLUTMatchRequired(cfgname)) && (! matchesPresentationLUT(align)))
          {
            DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: image data with " << bitsStoredValue << " bits/pixel does not match characteristics of active Presentation LUT.");
            rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
            result = OFFalse;
          } else {
            ADD_TO_PDATASET(DcmUnsignedShort, bitsStored)
          }
        }
      } else {
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: illegal bits stored value: " << val);
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    } else {
      DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: bits stored missing in basic grayscale image sequence");
      rsp.msg.NSetRSP.DimseStatus = STATUS_N_MissingAttribute;
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
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: illegal bits allocated value: " << val);
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    } else {
      DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: bits allocated missing in basic grayscale image sequence");
      rsp.msg.NSetRSP.DimseStatus = STATUS_N_MissingAttribute;
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
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: illegal high bit value: " << val);
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    } else {
      DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: high bit missing in basic grayscale image sequence");
      rsp.msg.NSetRSP.DimseStatus = STATUS_N_MissingAttribute;
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
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: illegal pixel representation value: " << val);
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    } else {
      DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: pixel representation missing in basic grayscale image sequence");
      rsp.msg.NSetRSP.DimseStatus = STATUS_N_MissingAttribute;
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
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: illegal photometric interpretation: '" << theColorModel.c_str() << "'");
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      } else {
        ADD_TO_PDATASET(DcmCodeString, photometricInterpretation)
      }
    } else {
      DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: photometric interpretation missing in basic grayscale image sequence");
      rsp.msg.NSetRSP.DimseStatus = STATUS_N_MissingAttribute;
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
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: illegal pixel aspect ratio, VM=: '" << pixelAspectRatio.getVM());
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
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
          rspDataset->insert(pixelData, OFTrue /*replaceOld*/);
        }
        else if ((EC_Normal == oldPxData->getUint8Array(pxdata8)) && pxdata8)
        {
          pixelData->putUint8Array(pxdata8, oldPxData->getLength()/sizeof(Uint8));
          rspDataset->insert(pixelData, OFTrue /*replaceOld*/);
        } else {
          DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: cannot access pixel data");
          rsp.msg.NSetRSP.DimseStatus = STATUS_N_ProcessingFailure;
          result = OFFalse;
        }
      } else writeresult=EC_MemoryExhausted;
    } else {
      DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: pixel data missing in basic grayscale image sequence");
      rsp.msg.NSetRSP.DimseStatus = STATUS_N_MissingAttribute;
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
      else if (currentTag == DCM_SamplesPerPixel) /* OK */ ;
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
        DCMPSTAT_INFO("cannot update Basic Grayscale Image Box: unsupported attribute in basic grayscale image sequence:\n"
            << DcmObject::PrintHelper(*stack.top(), DCMTypes::PF_shortenLongTagValues));
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      }
    }
  }

  // if n-set was successful, send back response dataset
  if (result && (EC_Normal == writeresult))
  {
    // complete Hardcopy Grayscale Image

    // write patient module
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_PatientName, DEFAULT_patientName);
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_PatientID);
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_PatientBirthDate);
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_PatientSex);

    // general study and general series modules are written somewhere else

    // Hardcopy Equipment Module
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_RETIRED_HardcopyDeviceManufacturer, "OFFIS");
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_RETIRED_HardcopyDeviceSoftwareVersion, OFFIS_DTK_IMPLEMENTATION_VERSION_NAME);

    // General Image Module
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_InstanceNumber);
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_PatientOrientation);
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_ImageType, "DERIVED\\SECONDARY");
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_DerivationDescription, "Hardcopy");

    // SOP Common Module
    if (EC_Normal==writeresult) writeresult = DVPSHelper::putStringValue(rspDataset, DCM_SOPClassUID, UID_RETIRED_HardcopyGrayscaleImageStorage);
    if (EC_Normal==writeresult) writeresult = referencedSOPClassUID.putString(UID_RETIRED_HardcopyGrayscaleImageStorage);
    dcmGenerateUniqueIdentifier(newuid);
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

OFCondition DVPSImageBoxContent::setUIDsAndAETitle(DcmUniqueIdentifier& studyUID, DcmUniqueIdentifier& seriesUID, const char *aetitle)
{
  if (aetitle==NULL) return EC_IllegalCall;

  studyInstanceUID = studyUID;
  seriesInstanceUID = seriesUID;
  return retrieveAETitle.putString(aetitle);
}

/*
 *  $Log: dvpsib.cc,v $
 *  Revision 1.40  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.39  2010-09-24 13:32:58  joergr
 *  Compared names of SOP Class UIDs with 2009 edition of the DICOM standard. The
 *  resulting name changes are mainly caused by the fact that the corresponding
 *  SOP Class is now retired.
 *
 *  Revision 1.38  2010-08-09 13:21:56  joergr
 *  Updated data dictionary to 2009 edition of the DICOM standard. From now on,
 *  the official "keyword" is used for the attribute name which results in a
 *  number of minor changes (e.g. "PatientsName" is now called "PatientName").
 *
 *  Revision 1.37  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.36  2008-04-30 12:38:43  meichel
 *  Fixed compile errors due to changes in attribute tag names
 *
 *  Revision 1.35  2007/10/31 14:16:40  meichel
 *  Fixed bug in DICOMscope support code that caused the Print SCU to send
 *    wrong values for Decimate/Crop Behavior.
 *
 *  Revision 1.34  2006/08/15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.33  2005/12/08 15:46:33  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.32  2003/06/04 12:30:28  meichel
 *  Added various includes needed by MSVC5 with STL
 *
 *  Revision 1.31  2003/03/12 17:34:22  meichel
 *  Updated DcmObject::print() flags
 *
 *  Revision 1.30  2002/06/20 12:19:49  meichel
 *  Adapted toolkit to name changes in latest data dictionary
 *
 *  Revision 1.29  2002/01/08 10:35:45  joergr
 *  Corrected spelling of function dcmGenerateUniqueIdentifier().
 *
 *  Revision 1.28  2001/11/28 13:56:56  joergr
 *  Check return value of DcmItem::insert() statements where appropriate to
 *  avoid memory leaks when insert procedure fails.
 *
 *  Revision 1.27  2001/09/26 15:36:27  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.26  2001/06/07 14:31:34  joergr
 *  Removed unused variable (reported by gcc 2.5.8 on NeXTSTEP).
 *
 *  Revision 1.25  2001/06/01 15:50:32  meichel
 *  Updated copyright header
 *
 *  Revision 1.24  2001/05/25 10:07:57  meichel
 *  Corrected some DIMSE error status codes for Print SCP
 *
 *  Revision 1.23  2000/09/06 08:55:37  meichel
 *  Updated Print SCP to accept and silently ignore group length attributes.
 *
 *  Revision 1.22  2000/07/04 16:06:47  joergr
 *  Added support for overriding the presentation LUT settings made for the
 *  image boxes.
 *
 *  Revision 1.21  2000/06/14 11:28:59  joergr
 *  Added methods to access the attributes Polarity and Requested Image Size.
 *
 *  Revision 1.20  2000/06/08 10:44:35  meichel
 *  Implemented Referenced Presentation LUT Sequence on Basic Film Session level.
 *    Empty film boxes (pages) are not written to file anymore.
 *
 *  Revision 1.19  2000/06/07 14:26:45  joergr
 *  Added methods to access the image polarity attribute.
 *
 *  Revision 1.18  2000/06/07 13:17:06  meichel
 *  now using DIMSE status constants and log facilities defined in dcmnet
 *
 *  Revision 1.17  2000/06/02 16:01:02  meichel
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

