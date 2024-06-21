/*
 *
 *  Copyright (C) 1997-2023, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Andreas Barth
 *
 *  Purpose: abstract class DcmCodec and the class DcmCodecStruct
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dccodec.h"
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofthread.h"
#include "dcmtk/dcmdata/dcdeftag.h"  /* for tag constants */
#include "dcmtk/dcmdata/dcuid.h"     /* for dcmGenerateUniqueIdentifer()*/
#include "dcmtk/dcmdata/dcitem.h"    /* for class DcmItem */
#include "dcmtk/dcmdata/dcsequen.h"  /* for DcmSequenceOfItems */
#include "dcmtk/dcmdata/dcpixseq.h"  /* for DcmPixelSequence */
#include "dcmtk/dcmdata/dcpxitem.h"  /* for DcmPixelItem */
#include "dcmtk/dcmdata/dcswap.h"    /* for swapIfNecessary */
#include "dcmtk/dcmdata/dcvrcs.h"    /* for DcmCodeString */
#include "dcmtk/dcmdata/dcvrui.h"    /* for DcmUniqueIdentifier */

// static member variables
OFList<DcmCodecList *> DcmCodecList::registeredCodecs;

#ifdef WITH_THREADS
OFReadWriteLock DcmCodecList::codecLock;
#endif

/* --------------------------------------------------------------- */

// DcmCodec static helper methods

OFCondition DcmCodec::insertStringIfMissing(DcmItem *dataset, const DcmTagKey& tag, const char *val)
{
  DcmStack stack;
  if ((dataset->search(tag, stack, ESM_fromHere, OFFalse)).bad())
  {
    return dataset->putAndInsertString(tag, val, OFTrue);
  }
  return EC_Normal;
}


OFCondition DcmCodec::convertToSecondaryCapture(DcmItem *dataset)
{
  if (dataset == NULL) return EC_IllegalCall;

  OFCondition result = EC_Normal;
  char buf[70];

  // SOP Class UID - always replace
  if (result.good()) result = dataset->putAndInsertString(DCM_SOPClassUID, UID_SecondaryCaptureImageStorage);

  // SOP Instance UID - only insert if missing.
  dcmGenerateUniqueIdentifier(buf);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_SOPInstanceUID, buf);

  // Type 1 attributes - insert with value if missing
  dcmGenerateUniqueIdentifier(buf, SITE_STUDY_UID_ROOT);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_StudyInstanceUID, buf);
  dcmGenerateUniqueIdentifier(buf, SITE_SERIES_UID_ROOT);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_SeriesInstanceUID, buf);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_ConversionType, "WSD");
  if (result.good()) result = insertStringIfMissing(dataset, DCM_Modality, "OT");

  // Type 2 attributes - insert without value if missing
  if (result.good()) result = insertStringIfMissing(dataset, DCM_PatientName, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_PatientID, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_PatientBirthDate, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_PatientSex, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_PatientOrientation, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_StudyDate, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_StudyTime, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_ReferringPhysicianName, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_StudyID, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_AccessionNumber, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_SeriesNumber, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_InstanceNumber, NULL);

  return result;
}


OFCondition DcmCodec::insertCodeSequence(
  DcmItem *dataset,
  const DcmTagKey &tagKey,
  const char *codingSchemeDesignator,
  const char *codeValue,
  const char *codeMeaning)
{
  if (dataset == NULL || codingSchemeDesignator == NULL ||
     codeValue == NULL || codeMeaning == NULL) return EC_IllegalCall;

  OFCondition result = EC_Normal;
  DcmSequenceOfItems *dseq = new DcmSequenceOfItems(tagKey);
  if (dseq)
  {
    DcmItem *ditem = new DcmItem();
    if (ditem)
    {
      dseq->insert(ditem);
      result = ditem->putAndInsertString(DCM_CodingSchemeDesignator, codingSchemeDesignator);
      if (result.good()) result = ditem->putAndInsertString(DCM_CodeValue, codeValue);
      if (result.good()) result = ditem->putAndInsertString(DCM_CodeMeaning, codeMeaning);
    } else result = EC_MemoryExhausted;

    // insert sequence into dataset if everything went well
    if (result.good()) dataset->insert(dseq, OFTrue /*replaceOld*/); else delete dseq;
  } else result = EC_MemoryExhausted;

  return result;
}


OFCondition DcmCodec::newInstance(
  DcmItem *dataset,
  const char *purposeOfReferenceCodingScheme,
  const char *purposeOfReferenceCodeValue,
  const char *purposeOfReferenceCodeMeaning)
{
  if (dataset == NULL) return EC_IllegalCall;
  OFCondition result = EC_Normal;

  // look up current SOP Class UID and SOP Instance UID
  const char *classUID = NULL;
  const char *instanceUID = NULL;

  // search for existing SOP Class UID / SOP Instance UID
  OFCondition tempResult = dataset->findAndGetString(DCM_SOPClassUID, classUID);
  if (tempResult.good()) tempResult = dataset->findAndGetString(DCM_SOPInstanceUID, instanceUID);
  if (tempResult.good() && classUID && instanceUID)
  {
    // create source image sequence
    DcmSequenceOfItems *dseq = new DcmSequenceOfItems(DCM_SourceImageSequence);
    if (dseq)
    {
      DcmItem *ditem = new DcmItem();
      if (ditem)
      {
        dseq->insert(ditem);
        DcmElement *elem1 = new DcmUniqueIdentifier(DCM_ReferencedSOPClassUID);
        if (elem1)
        {
          result = elem1->putString(classUID);
          ditem->insert(elem1, OFTrue /*replaceOld*/);
          if (result.good())
          {
            DcmElement *elem2 = new DcmUniqueIdentifier(DCM_ReferencedSOPInstanceUID);
            if (elem2)
            {
              result = elem2->putString(instanceUID);
              ditem->insert(elem2, OFTrue /*replaceOld*/);
            } else result = EC_MemoryExhausted;
          }
        } else result = EC_MemoryExhausted;

        if (result.good() && purposeOfReferenceCodingScheme &&
           purposeOfReferenceCodeValue && purposeOfReferenceCodeMeaning)
        {
          // add purpose of reference code sequence
          result = DcmCodec::insertCodeSequence(ditem, DCM_PurposeOfReferenceCodeSequence,
            purposeOfReferenceCodingScheme, purposeOfReferenceCodeValue, purposeOfReferenceCodeMeaning);
        }
      } else result = EC_MemoryExhausted;
      if (result.good()) dataset->insert(dseq, OFTrue); else delete dseq;
    } else result = EC_MemoryExhausted;
  }

  // create new SOP instance UID
  if (result.good())
  {
    char new_uid[100];
    DcmElement *elem = new DcmUniqueIdentifier(DCM_SOPInstanceUID);
    if (elem)
    {
      if (EC_Normal == (result = elem->putString(dcmGenerateUniqueIdentifier(new_uid))))
        dataset->insert(elem, OFTrue); // replace SOP Instance UID
        else delete elem;
    } else result = EC_MemoryExhausted;
  }

  return result;
}


OFCondition DcmCodec::updateImageType(DcmItem *dataset)
{
  if (dataset == NULL) return EC_IllegalCall;

  DcmElement *elem = NULL;
  /* check for the data element (with non-empty value) */
  if (dataset->findAndGetElement(DCM_ImageType, elem).good() && !elem->isEmpty())
  {
    /* case 1: there is a single value only */
    if (elem->getNumberOfValues() == 1)
    {
      DCMDATA_DEBUG("DcmCodec::updateImageType() setting data element value 'DERIVED\\SECONDARY'");
      /* overwrite with a valid value (VM=2-n) */
      return elem->putString("DERIVED\\SECONDARY");
    } else {
      OFString elemValue;
      /* case 2: value 1 is different from "DERIVED" */
      if (elem->getOFString(elemValue, 0 /*pos*/).good() && (elemValue != "DERIVED"))
      {
        if (elem->ident() == EVR_CS)
        {
          DCMDATA_DEBUG("DcmCodec::updateImageType() setting data element value 1 to 'DERIVED'");
          /* overwrite value 1 */
          return OFstatic_cast(DcmCodeString *, elem)->putOFStringAtPos("DERIVED", 0);
        } else {
          /* cannot overwrite value with wrong VR (should never happen) */
          DCMDATA_ERROR("DcmCodec: Internal ERROR: Cannot update element ImageType " << DCM_ImageType << " with wrong VR");
          return EC_InvalidVR;
        }
      }
    }
  }

  /* nothing to do */
  return EC_Normal;
}


OFCondition DcmCodec::determineStartFragment(
  Uint32 frameNo,
  Sint32 numberOfFrames,
  DcmPixelSequence * fromPixSeq,
  Uint32& currentItem)
{
  Uint32 numberOfFragments = OFstatic_cast(Uint32, fromPixSeq->card());
  if (numberOfFrames < 1 || numberOfFragments <= OFstatic_cast(Uint32, numberOfFrames) || frameNo >= OFstatic_cast(Uint32, numberOfFrames))
    return EC_IllegalCall;

  if (frameNo == 0)
  {
    // simple case: first frame is always at second fragment
    currentItem = 1;
    return EC_Normal;
  }

  if (numberOfFragments == OFstatic_cast(Uint32, numberOfFrames) + 1)
  {
    // standard case: there is one fragment per frame.
    currentItem = frameNo + 1;
    return EC_Normal;
  }

  // non-standard case: multiple fragments per frame.
  // We now try to consult the offset table.
  DcmPixelItem *pixItem = NULL;
  Uint8 *rawOffsetTable = NULL;

  // get first pixel item, i.e. the fragment containing the offset table
  OFCondition result = fromPixSeq->getItem(pixItem, 0);
  if (result.good())
  {
    Uint32 tableLength = pixItem->getLength();
    result = pixItem->getUint8Array(rawOffsetTable);
    if (result.good())
    {
      // check if the offset table is empty
      if (tableLength == 0)
        result = makeOFCondition(OFM_dcmdata, EC_CODE_CannotDetermineStartFragment, OF_error, "Cannot determine start fragment: basic offset table is empty");
      // check if the offset table has the right size: 4 bytes for each frame (not fragment!)
      else if (tableLength != 4 * OFstatic_cast(Uint32, numberOfFrames))
        result = makeOFCondition(OFM_dcmdata, EC_CODE_CannotDetermineStartFragment, OF_error, "Cannot determine start fragment: basic offset table has wrong size");
      else {

        // byte swap offset table into local byte order. In file, the offset table is always in little endian
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, rawOffsetTable, tableLength, sizeof(Uint32));

        // cast offset table to Uint32.
        Uint32 *offsetTable = OFreinterpret_cast(Uint32 *, rawOffsetTable);

        // now access offset of the frame we're looking for
        Uint32 offset = offsetTable[frameNo];

        // OK, now let's look if we can find a fragment that actually corresponds to that offset.
        // In counter we compute the offset for each frame by adding all fragment lengths
        Uint32 counter = 0;
        // now iterate over all fragments except the index table. The start of the first fragment
        // is defined as zero.
        for (Uint32 idx = 1; idx < numberOfFragments; ++idx)
        {
          if (counter == offset)
          {
            // hooray, we are lucky. We have found the fragment we're looking for
            currentItem = idx;
            return EC_Normal;
          }

          // access pixel item in order to determine its length
          result = fromPixSeq->getItem(pixItem, idx);
          if (result.bad())
            return makeOFCondition(OFM_dcmdata, EC_CODE_CannotDetermineStartFragment, OF_error, "Cannot determine start fragment: cannot access referenced pixel item");

          // add pixel item length plus 8 bytes overhead for the item tag and length field
          counter += pixItem->getLength() + 8;
        }

        // bad luck. We have not found a fragment corresponding to the offset in the offset table.
        // Either we cannot correctly add numbers, or they cannot :-)
        result = makeOFCondition(OFM_dcmdata, EC_CODE_CannotDetermineStartFragment, OF_error, "Cannot determine start fragment: possibly wrong value in basic offset table");
      }
    } else
      result = makeOFCondition(OFM_dcmdata, EC_CODE_CannotDetermineStartFragment, OF_error, "Cannot determine start fragment: cannot access content of basic offset table");
  } else
    result = makeOFCondition(OFM_dcmdata, EC_CODE_CannotDetermineStartFragment, OF_error, "Cannot determine start fragment: cannot access basic offset table (first item)");
  return result;
}


/* --------------------------------------------------------------- */

DcmCodecList::DcmCodecList(
  const DcmCodec *aCodec,
  const DcmRepresentationParameter *aDefaultRepParam,
  const DcmCodecParameter *aCodecParameter)
: codec(aCodec)
, defaultRepParam(aDefaultRepParam)
, codecParameter(aCodecParameter)
{
}

DcmCodecList::~DcmCodecList()
{
}

OFCondition DcmCodecList::registerCodec(
  const DcmCodec *aCodec,
  const DcmRepresentationParameter *aDefaultRepParam,
  const DcmCodecParameter *aCodecParameter)
{
  if ((aCodec == NULL)||(aCodecParameter == NULL)) return EC_IllegalParameter;
#ifdef WITH_THREADS
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif

  // acquire write lock on codec list.  Will block if some codec is currently active.
  OFCondition result = EC_Normal;
#ifdef WITH_THREADS
  OFReadWriteLocker locker(codecLock);
  if (0 == locker.wrlock())
  {
#endif
    DcmCodecList *listEntry = new DcmCodecList(aCodec, aDefaultRepParam, aCodecParameter);
    if (listEntry)
    {
      // prevent codec from being registered twice
      OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
      OFListIterator(DcmCodecList *) last = registeredCodecs.end();
      while (first != last)
      {
        if ((*first)->codec == aCodec)
        {
          // this codec is already registered.
          first = last;
          result = EC_IllegalCall;
        } else ++first;
      }
      if (result.good()) registeredCodecs.push_back(listEntry); else delete listEntry;
    } else result = EC_MemoryExhausted;
#ifdef WITH_THREADS
  } else result = EC_IllegalCall;
#endif
  return result;
}

OFCondition DcmCodecList::deregisterCodec(const DcmCodec *aCodec)
{
  if (aCodec == NULL) return EC_IllegalParameter;
#ifdef WITH_THREADS
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif
  // acquire write lock on codec list.  Will block if some codec is currently active.
  OFCondition result = EC_Normal;

#ifdef WITH_THREADS
  OFReadWriteLocker locker(codecLock);
  if (0 == locker.wrlock())
  {
#endif
    OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
    OFListIterator(DcmCodecList *) last = registeredCodecs.end();
    while (first != last)
    {
      if ((*first)->codec == aCodec)
      {
        delete *first;
        first = registeredCodecs.erase(first);
      } else ++first;
    }
#ifdef WITH_THREADS
  } else result = EC_IllegalCall;
#endif
  return result;
}

OFCondition DcmCodecList::updateCodecParameter(
  const DcmCodec *aCodec,
  const DcmCodecParameter *aCodecParameter)
{
  if ((aCodec == NULL)||(aCodecParameter == NULL)) return EC_IllegalParameter;
#ifdef WITH_THREADS
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif
  // acquire write lock on codec list.  Will block if some codec is currently active.
  OFCondition result = EC_Normal;

#ifdef WITH_THREADS
  OFReadWriteLocker locker(codecLock);
  if (0 == locker.wrlock())
  {
#endif
    OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
    OFListIterator(DcmCodecList *) last = registeredCodecs.end();
    while (first != last)
    {
      if ((*first)->codec == aCodec) (*first)->codecParameter = aCodecParameter;
      ++first;
    }
#ifdef WITH_THREADS
  } else result = EC_IllegalCall;
#endif
  return result;
}


OFCondition DcmCodecList::decode(
  const DcmXfer & fromType,
  const DcmRepresentationParameter * fromParam,
  DcmPixelSequence * fromPixSeq,
  DcmPolymorphOBOW& uncompressedPixelData,
  DcmStack & pixelStack,
  OFBool& removeOldRep)
{
#ifdef WITH_THREADS
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif
  OFCondition result = EC_CannotChangeRepresentation;

  // acquire write lock on codec list.  Will block if some write lock is currently active.
#ifdef WITH_THREADS
  OFReadWriteLocker locker(codecLock);
  if (0 == locker.rdlock())
  {
#endif
    E_TransferSyntax fromXfer = fromType.getXfer();
    OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
    OFListIterator(DcmCodecList *) last = registeredCodecs.end();
    while (first != last)
    {
      if ((*first)->codec->canChangeCoding(fromXfer, EXS_LittleEndianExplicit))
      {
        result = (*first)->codec->decode(fromParam, fromPixSeq, uncompressedPixelData, (*first)->codecParameter, pixelStack, removeOldRep);
        first = last;
      } else ++first;
    }
#ifdef WITH_THREADS
  } else result = EC_IllegalCall;
#endif
  return result;
}


OFCondition DcmCodecList::decodeFrame(
  const DcmXfer & fromType,
  const DcmRepresentationParameter * fromParam,
  DcmPixelSequence * fromPixSeq,
  DcmItem *dataset,
  Uint32 frameNo,
  Uint32& startFragment,
  void *buffer,
  Uint32 bufSize,
  OFString& decompressedColorModel)
{
#ifdef WITH_THREADS
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif
  OFCondition result = EC_CannotChangeRepresentation;

  // acquire write lock on codec list.  Will block if some write lock is currently active.
#ifdef WITH_THREADS
  OFReadWriteLocker locker(codecLock);
  if (0 == locker.rdlock())
  {
#endif
    E_TransferSyntax fromXfer = fromType.getXfer();
    OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
    OFListIterator(DcmCodecList *) last = registeredCodecs.end();
    while (first != last)
    {
      if ((*first)->codec->canChangeCoding(fromXfer, EXS_LittleEndianExplicit))
      {
        result = (*first)->codec->decodeFrame(fromParam, fromPixSeq, (*first)->codecParameter,
                 dataset, frameNo, startFragment, buffer, bufSize, decompressedColorModel);
        first = last;
      } else ++first;
    }
#ifdef WITH_THREADS
  } else result = EC_IllegalCall;
#endif
  return result;
}


OFCondition DcmCodecList::encode(
  const E_TransferSyntax fromRepType,
  const DcmRepresentationParameter * fromParam,
  DcmPixelSequence * fromPixSeq,
  const E_TransferSyntax toRepType,
  const DcmRepresentationParameter * toRepParam,
  DcmPixelSequence * & toPixSeq,
  DcmStack & pixelStack,
  OFBool& removeOldRep)
{
  toPixSeq = NULL;
#ifdef WITH_THREADS
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif
  OFCondition result = EC_CannotChangeRepresentation;

  // acquire write lock on codec list.  Will block if some write lock is currently active.
#ifdef WITH_THREADS
  OFReadWriteLocker locker(codecLock);
  if (0 == locker.rdlock())
  {
#endif
    OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
    OFListIterator(DcmCodecList *) last = registeredCodecs.end();
    while (first != last)
    {
      if ((*first)->codec->canChangeCoding(fromRepType, toRepType))
      {
        if (!toRepParam) toRepParam = (*first)->defaultRepParam;
        result = (*first)->codec->encode(fromRepType, fromParam, fromPixSeq,
                 toRepParam, toPixSeq, (*first)->codecParameter, pixelStack, removeOldRep);
        first = last;
      } else ++first;
    }
#ifdef WITH_THREADS
  } else result = EC_IllegalCall;
#endif

  return result;
}

OFCondition DcmCodecList::encode(
  const E_TransferSyntax fromRepType,
  const Uint16 * pixelData,
  const Uint32 length,
  const E_TransferSyntax toRepType,
  const DcmRepresentationParameter * toRepParam,
  DcmPixelSequence * & toPixSeq,
  DcmStack & pixelStack,
  OFBool& removeOldRep)
{
  toPixSeq = NULL;
#ifdef WITH_THREADS
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif
  OFCondition result = EC_CannotChangeRepresentation;

  // acquire write lock on codec list.  Will block if some write lock is currently active.
#ifdef WITH_THREADS
  OFReadWriteLocker locker(codecLock);
  if (0 == locker.rdlock())
  {
#endif
    OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
    OFListIterator(DcmCodecList *) last = registeredCodecs.end();
    while (first != last)
    {
      if ((*first)->codec->canChangeCoding(fromRepType, toRepType))
      {
        if (!toRepParam) toRepParam = (*first)->defaultRepParam;
        result = (*first)->codec->encode(pixelData, length, toRepParam, toPixSeq,
                 (*first)->codecParameter, pixelStack, removeOldRep);
        first = last;
      } else ++first;
    }
#ifdef WITH_THREADS
  } else result = EC_IllegalCall;
#endif

  return result;
}

OFBool DcmCodecList::canChangeCoding(
  const E_TransferSyntax fromRepType,
  const E_TransferSyntax toRepType)
{
#ifdef WITH_THREADS
  if (! codecLock.initialized()) return OFFalse; // should never happen
#endif
  OFBool result = OFFalse;

  // acquire write lock on codec list.  Will block if some write lock is currently active.
#ifdef WITH_THREADS
  OFReadWriteLocker locker(codecLock);
  if (0 == locker.rdlock())
  {
#endif
    OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
    OFListIterator(DcmCodecList *) last = registeredCodecs.end();
    while (first != last)
    {
      if ((*first)->codec->canChangeCoding(fromRepType, toRepType))
      {
        result = OFTrue;
        first = last;
      } else ++first;
    }
#ifdef WITH_THREADS
  }
#endif

  return result;
}

OFCondition DcmCodecList::determineDecompressedColorModel(
  const DcmXfer &fromType,
  const DcmRepresentationParameter *fromParam,
  DcmPixelSequence *fromPixSeq,
  DcmItem *dataset,
  OFString &decompressedColorModel)
{
#ifdef WITH_THREADS
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif
  OFCondition result = EC_CannotChangeRepresentation;

  // acquire write lock on codec list.  Will block if some write lock is currently active.
#ifdef WITH_THREADS
  OFReadWriteLocker locker(codecLock);
  if (0 == locker.rdlock())
  {
#endif
    E_TransferSyntax fromXfer = fromType.getXfer();
    OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
    OFListIterator(DcmCodecList *) last = registeredCodecs.end();
    while (first != last)
    {
      if ((*first)->codec->canChangeCoding(fromXfer, EXS_LittleEndianExplicit))
      {
        result = (*first)->codec->determineDecompressedColorModel(fromParam, fromPixSeq, (*first)->codecParameter,
                 dataset, decompressedColorModel);
        first = last;
      } else ++first;
    }
#ifdef WITH_THREADS
  } else result = EC_IllegalCall;
#endif
  return result;
}
