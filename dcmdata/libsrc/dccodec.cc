/*
 *
 *  Copyright (C) 1997-2004, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Andreas Barth
 *
 *  Purpose: abstract class DcmCodec and the class DcmCodecStruct
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2004-02-04 16:11:42 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dccodec.h"
#include "oflist.h"
#include "ofthread.h"
#include "dcdeftag.h"  /* for tag constants */
#include "dcuid.h"     /* for dcmGenerateUniqueIdentifer()*/
#include "dcitem.h"    /* for class DcmItem */
#include "dcsequen.h"  /* for DcmSequenceOfItems */

// static member variables
OFList<DcmCodecList *> DcmCodecList::registeredCodecs;

#ifdef _REENTRANT
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
  if (result.good()) result = insertStringIfMissing(dataset, DCM_PatientsName, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_PatientID, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_PatientsBirthDate, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_PatientsSex, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_PatientOrientation, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_StudyDate, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_StudyTime, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_ReferringPhysiciansName, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_StudyID, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_AccessionNumber, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_SeriesNumber, NULL);
  if (result.good()) result = insertStringIfMissing(dataset, DCM_InstanceNumber, NULL);

  return result;
}

OFCondition DcmCodec::newInstance(DcmItem *dataset)
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

  DcmStack stack;
  OFString imageType("DERIVED\\SECONDARY");
  OFString a;

  /* find existing Image Type element */
  OFCondition status = dataset->search(DCM_ImageType, stack, ESM_fromHere, OFFalse);
  if (status.good())
  {
    DcmElement *elem = OFstatic_cast(DcmElement *, stack.top());
    unsigned long pos = 2;

    // append old image type information beginning with third entry
    while ((elem->getOFString(a, pos++)).good())
    {
      imageType += "\\";
      imageType += a;
    }
  }

  // insert new Image Type, replace old value
  return dataset->putAndInsertString(DCM_ImageType, imageType.c_str(), OFTrue);
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
#ifdef _REENTRANT
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif

  // acquire write lock on codec list.  Will block if some codec is currently active.
  OFCondition result = EC_Normal;
#ifdef _REENTRANT
  if (0 == codecLock.wrlock())
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
#ifdef _REENTRANT
    codecLock.unlock();
  } else result = EC_IllegalCall;
#endif
  return result;
}

OFCondition DcmCodecList::deregisterCodec(const DcmCodec *aCodec)
{
  if (aCodec == NULL) return EC_IllegalParameter;
#ifdef _REENTRANT
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif
  // acquire write lock on codec list.  Will block if some codec is currently active.
  OFCondition result = EC_Normal;

#ifdef _REENTRANT
  if (0 == codecLock.wrlock())
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
#ifdef _REENTRANT
    codecLock.unlock();
  } else result = EC_IllegalCall;
#endif
  return result;
}

OFCondition DcmCodecList::updateCodecParameter(
    const DcmCodec *aCodec,
    const DcmCodecParameter *aCodecParameter)
{
  if ((aCodec == NULL)||(aCodecParameter == NULL)) return EC_IllegalParameter;
#ifdef _REENTRANT
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif
  // acquire write lock on codec list.  Will block if some codec is currently active.
  OFCondition result = EC_Normal;

#ifdef _REENTRANT
  if (0 == codecLock.wrlock())
  {
#endif
    OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
    OFListIterator(DcmCodecList *) last = registeredCodecs.end();
    while (first != last)
    {
      if ((*first)->codec == aCodec) (*first)->codecParameter = aCodecParameter;
      ++first;
    }
#ifdef _REENTRANT
    codecLock.unlock();
  } else result = EC_IllegalCall;
#endif
  return result;
}


OFCondition DcmCodecList::decode(
    const DcmXfer & fromType,
    const DcmRepresentationParameter * fromParam,
    DcmPixelSequence * fromPixSeq,
    DcmPolymorphOBOW& uncompressedPixelData,
    DcmStack & pixelStack)
{
#ifdef _REENTRANT
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif
  OFCondition result = EC_CannotChangeRepresentation;

  // acquire write lock on codec list.  Will block if some write lock is currently active.
#ifdef _REENTRANT
  if (0 == codecLock.rdlock())
  {
#endif
    E_TransferSyntax fromXfer = fromType.getXfer();
    OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
    OFListIterator(DcmCodecList *) last = registeredCodecs.end();
    while (first != last)
    {
      if ((*first)->codec->canChangeCoding(fromXfer, EXS_LittleEndianExplicit))
      {
        result = (*first)->codec->decode(fromParam, fromPixSeq, uncompressedPixelData, (*first)->codecParameter, pixelStack);
        first = last;
      } else ++first;
    }
#ifdef _REENTRANT
    codecLock.unlock();
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
    DcmStack & pixelStack)
{
  toPixSeq = NULL;
#ifdef _REENTRANT
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif
  OFCondition result = EC_CannotChangeRepresentation;

  // acquire write lock on codec list.  Will block if some write lock is currently active.
#ifdef _REENTRANT
  if (0 == codecLock.rdlock())
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
                 toRepParam, toPixSeq, (*first)->codecParameter, pixelStack);
        first = last;
      } else ++first;
    }
#ifdef _REENTRANT
    codecLock.unlock();
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
    DcmStack & pixelStack)
{
  toPixSeq = NULL;
#ifdef _REENTRANT
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif
  OFCondition result = EC_CannotChangeRepresentation;

  // acquire write lock on codec list.  Will block if some write lock is currently active.
#ifdef _REENTRANT
  if (0 == codecLock.rdlock())
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
                 (*first)->codecParameter, pixelStack);
        first = last;
      } else ++first;
    }
#ifdef _REENTRANT
    codecLock.unlock();
  } else result = EC_IllegalCall;
#endif

  return result;
}

OFBool DcmCodecList::canChangeCoding(
    const E_TransferSyntax fromRepType,
    const E_TransferSyntax toRepType)
{
#ifdef _REENTRANT
  if (! codecLock.initialized()) return OFFalse; // should never happen
#endif
  OFBool result = OFFalse;

  // acquire write lock on codec list.  Will block if some write lock is currently active.
#ifdef _REENTRANT
  if (0 == codecLock.rdlock())
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
#ifdef _REENTRANT
    codecLock.unlock();
  }
#endif

  return result;
}

/*
** CVS/RCS Log:
** $Log: dccodec.cc,v $
** Revision 1.12  2004-02-04 16:11:42  joergr
** Adapted type casts to new-style typecast operators defined in ofcast.h.
**
** Revision 1.11  2002/06/27 15:15:53  meichel
** Now adding empty Patient Orientation when converting to
**   Secondary Capture.
**
** Revision 1.10  2002/05/24 14:51:50  meichel
** Moved helper methods that are useful for different compression techniques
**   from module dcmjpeg to module dcmdata
**
** Revision 1.9  2002/02/27 14:21:35  meichel
** Declare dcmdata read/write locks only when compiled in multi-thread mode
**
** Revision 1.8  2001/11/08 16:19:42  meichel
** Changed interface for codec registration. Now everything is thread-safe
**   and multiple codecs can be registered for a single transfer syntax (e.g.
**   one encoder and one decoder).
**
** Revision 1.7  2001/09/25 17:19:09  meichel
** Updated abstract class DcmCodecParameter for use with dcmjpeg.
**   Added new function deregisterGlobalCodec().
**
** Revision 1.6  2001/06/01 15:48:59  meichel
** Updated copyright header
**
** Revision 1.5  2000/09/27 08:19:57  meichel
** Minor changes in DcmCodec interface, required for future dcmjpeg module.
**
** Revision 1.4  2000/04/14 16:09:16  meichel
** Made function DcmCodec and related functions thread safe.
**   registerGlobalCodec() should not be called anymore from the constructor
**   of global objects.
**
** Revision 1.3  2000/03/08 16:26:30  meichel
** Updated copyright header.
**
** Revision 1.2  1999/03/31 09:25:18  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.1  1997/07/21 07:55:04  andreas
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
**
*/
