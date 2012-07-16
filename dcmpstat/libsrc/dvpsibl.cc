/*
 *
 *  Copyright (C) 1999-2010, OFFIS e.V.
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
 *    classes: DVPSImageBoxContent_PList
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpsibl.h"
#include "dcmtk/dcmpstat/dvpsib.h"      /* for DVPSImageBoxContent */
#include "dcmtk/dcmpstat/dvpshlp.h"     /* for class DVPSHelper */

/* --------------- class DVPSImageBoxContent_PList --------------- */

DVPSImageBoxContent_PList::DVPSImageBoxContent_PList()
: list_()
{
}

DVPSImageBoxContent_PList::DVPSImageBoxContent_PList(const DVPSImageBoxContent_PList &arg)
: list_()
{
  OFListConstIterator(DVPSImageBoxContent *) first = arg.list_.begin();
  OFListConstIterator(DVPSImageBoxContent *) last = arg.list_.end();
  while (first != last)
  {     
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSImageBoxContent_PList::~DVPSImageBoxContent_PList()
{
  clear();
}

void DVPSImageBoxContent_PList::clear()
{
  OFListIterator(DVPSImageBoxContent *) first = list_.begin();
  OFListIterator(DVPSImageBoxContent *) last = list_.end();
  while (first != last)
  {     
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSImageBoxContent_PList::read(DcmItem &dset, DVPSPresentationLUT_PList& presentationLUTList)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DVPSImageBoxContent *newImage = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;
  
  if (EC_Normal == dset.search(DCM_RETIRED_ImageBoxContentSequence, stack, ESM_fromHere, OFFalse))
  {
    dseq=(DcmSequenceOfItems *)stack.top();
    if (dseq)
    {
      unsigned long numItems = dseq->card();
      for (unsigned int i=0; i<numItems; i++)
      {
        ditem = dseq->getItem(i);
        newImage = new DVPSImageBoxContent();
        if (newImage && ditem)
        {
          result = newImage->read(*ditem, presentationLUTList);
          list_.push_back(newImage);
        } else result = EC_MemoryExhausted;
      }
    }
  }    
  
  return result;
}

OFCondition DVPSImageBoxContent_PList::write(
  DcmItem &dset,
  OFBool writeRequestedImageSize, 
  size_t numItems,
  OFBool ignoreEmptyImages,
  OFBool writeReferencedPLUTSQ)
{
  if (size()==0) return EC_IllegalCall; // can't write if sequence is empty

  OFCondition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;
  OFBool working = OFTrue;
  unsigned long numWritten = 0;
  
  dseq = new DcmSequenceOfItems(DCM_RETIRED_ImageBoxContentSequence);
  if (dseq)
  {
    OFListIterator(DVPSImageBoxContent *) first = list_.begin();
    OFListIterator(DVPSImageBoxContent *) last = list_.end();
    while ((first != last) && working)
    {
      if ((result==EC_Normal) && ((! ignoreEmptyImages)||((*first)->getImageBoxPosition() > 0)))
      {
        ditem = new DcmItem();
        if (ditem)
        {
          result = (*first)->write(*ditem, writeRequestedImageSize, writeReferencedPLUTSQ);
          if (result==EC_Normal) 
          {
            dseq->insert(ditem); 
            numWritten++;
          } else delete ditem;
        } else result = EC_MemoryExhausted;
      }
      ++first;
      if (numItems && (--numItems==0)) working=OFFalse;
    }
    // we're not allowed to store SP objects with empty image box list sequence
    if ((result==EC_Normal) && (numWritten > 0)) dset.insert(dseq, OFTrue /*replaceOld*/); else delete dseq;
  } else result = EC_MemoryExhausted;
  return result;
}

OFCondition DVPSImageBoxContent_PList::createDefaultValues(OFBool renumber, OFBool ignoreEmptyImages)
{
  if (size()==0) return EC_IllegalCall; // can't write if sequence is empty
  OFCondition result = EC_Normal;
  unsigned long counter = 1;
  
  OFListIterator(DVPSImageBoxContent *) first = list_.begin();
  OFListIterator(DVPSImageBoxContent *) last = list_.end();
  while ((first != last)&&(EC_Normal == result))
  {     
    result = (*first)->createDefaultValues(renumber, counter++, ignoreEmptyImages);
    ++first;
  }
  return result;
}

OFCondition DVPSImageBoxContent_PList::addImageSOPClasses(DcmSequenceOfItems& seq, size_t numItems)
{
  OFCondition result = EC_Normal;
  OFBool working = OFTrue;
  const char *c = NULL;
  OFListIterator(DVPSImageBoxContent *) first = list_.begin();
  OFListIterator(DVPSImageBoxContent *) last = list_.end();
  while ((first != last) && working)
  {
    if (EC_Normal == result)
    {
      c = (*first)->getSOPClassUID();
      if (c && (! DVPSHelper::haveReferencedUIDItem(seq, c))) result = DVPSHelper::addReferencedUIDItem(seq, c);
    }
    ++first;
    if (numItems && (--numItems==0)) working=OFFalse;
  }
  return result;
}

OFCondition DVPSImageBoxContent_PList::addImageBox(
  const char *instanceuid,
  const char *retrieveaetitle,
  const char *refstudyuid,
  const char *refseriesuid,
  const char *refsopclassuid,
  const char *refsopinstanceuid,
  const char *requestedimagesize,
  const char *patientid,
  const char *presentationlutuid)
{
  OFCondition result = EC_Normal;
  DVPSImageBoxContent *newImage = new DVPSImageBoxContent();
  if (newImage)
  {
    result = newImage->setContent(instanceuid, retrieveaetitle, refstudyuid,
               refseriesuid, refsopclassuid, refsopinstanceuid,
               requestedimagesize, patientid, presentationlutuid);
    if (EC_Normal == result) list_.push_back(newImage); else delete newImage;
  } else result = EC_MemoryExhausted;
  return result;
}

OFCondition DVPSImageBoxContent_PList::addImageBox(DVPSImageBoxContent * box)
{
   list_.push_back(box);
   return(EC_Normal); 
}

OFCondition DVPSImageBoxContent_PList::setRequestedDecimateCropBehaviour(DVPSDecimateCropBehaviour value)
{
  OFCondition result=EC_Normal;
  OFListIterator(DVPSImageBoxContent *) first = list_.begin();
  OFListIterator(DVPSImageBoxContent *) last = list_.end();
  while (first != last)
  {     
    result = (*first)->setRequestedDecimateCropBehaviour(value);
    if (EC_Normal != result) return result;
    ++first;
  }
  return result;
}

OFCondition DVPSImageBoxContent_PList::deleteImage(size_t idx)
{
  OFListIterator(DVPSImageBoxContent *) first = list_.begin();
  OFListIterator(DVPSImageBoxContent *) last = list_.end();
  while ((first != last)&&(idx--)) ++first;
  if (first != last)
  {
    delete (*first);
    list_.erase(first);
    return EC_Normal;
  }
  return EC_IllegalCall;
}

OFCondition DVPSImageBoxContent_PList::deleteMultipleImages(size_t number)
{
  OFListIterator(DVPSImageBoxContent *) first = list_.begin();
  OFListIterator(DVPSImageBoxContent *) last = list_.end();
  while ((first != last)&&(number--))
  {
    delete (*first);
    first = list_.erase(first);
  }
  return EC_Normal;
}

DVPSImageBoxContent *DVPSImageBoxContent_PList::getImageBox(size_t idx)
{
  OFListIterator(DVPSImageBoxContent *) first = list_.begin();
  OFListIterator(DVPSImageBoxContent *) last = list_.end();
  while (first != last)
  {
    if (idx==0) return *first;
    idx--;
    ++first;
  }
  return NULL;
}

OFBool DVPSImageBoxContent_PList::imageHasAdditionalSettings(size_t idx)
{
  DVPSImageBoxContent *box = getImageBox(idx);
  if (box) return box->hasAdditionalSettings();
  return OFFalse; 
}

OFCondition DVPSImageBoxContent_PList::setImagePolarity(size_t idx, const char *value)
{
  DVPSImageBoxContent *box = getImageBox(idx);
  if (box) return box->setPolarity(value);
  return EC_IllegalCall; 
}

OFCondition DVPSImageBoxContent_PList::setImageRequestedSize(size_t idx, const char *value)
{
  DVPSImageBoxContent *box = getImageBox(idx);
  if (box) return box->setRequestedImageSize(value);
  return EC_IllegalCall; 
}

OFCondition DVPSImageBoxContent_PList::setImageMagnificationType(size_t idx, const char *value)
{
  DVPSImageBoxContent *box = getImageBox(idx);
  if (box) return box->setMagnificationType(value);
  return EC_IllegalCall; 
}

OFCondition DVPSImageBoxContent_PList::setImageSmoothingType(size_t idx, const char *value)
{
  DVPSImageBoxContent *box = getImageBox(idx);
  if (box) return box->setSmoothingType(value);
  return EC_IllegalCall; 
}

OFCondition DVPSImageBoxContent_PList::setImageConfigurationInformation(size_t idx, const char *value)
{
  DVPSImageBoxContent *box = getImageBox(idx);
  if (box) return box->setConfigurationInformation(value);
  return EC_IllegalCall; 
}

OFCondition DVPSImageBoxContent_PList::setImageSOPInstanceUID(size_t idx, const char *value)
{
  DVPSImageBoxContent *box = getImageBox(idx);
  if (box) return box->setSOPInstanceUID(value);
  return EC_IllegalCall; 
}

const char *DVPSImageBoxContent_PList::getImagePolarity(size_t idx)
{
  DVPSImageBoxContent *box = getImageBox(idx);
  if (box) return box->getPolarity();
  return NULL; 
}

const char *DVPSImageBoxContent_PList::getImageRequestedSize(size_t idx)
{
  DVPSImageBoxContent *box = getImageBox(idx);
  if (box) return box->getRequestedImageSize();
  return NULL; 
}

const char *DVPSImageBoxContent_PList::getImageMagnificationType(size_t idx)
{
  DVPSImageBoxContent *box = getImageBox(idx);
  if (box) return box->getMagnificationType();
  return NULL; 
}

const char *DVPSImageBoxContent_PList::getImageSmoothingType(size_t idx)
{
  DVPSImageBoxContent *box = getImageBox(idx);
  if (box) return box->getSmoothingType();
  return NULL; 
}

const char *DVPSImageBoxContent_PList::getImageConfigurationInformation(size_t idx)
{
  DVPSImageBoxContent *box = getImageBox(idx);
  if (box) return box->getConfigurationInformation();
  return NULL; 
}

const char *DVPSImageBoxContent_PList::getSOPInstanceUID(size_t idx)
{
  DVPSImageBoxContent *box = getImageBox(idx);
  if (box) return box->getSOPInstanceUID();
  return NULL; 
}

const char *DVPSImageBoxContent_PList::getReferencedPresentationLUTInstanceUID(size_t idx)
{
  DVPSImageBoxContent *box = getImageBox(idx);
  if (box) return box->getReferencedPresentationLUTInstanceUID();
  return NULL; 
}

OFCondition DVPSImageBoxContent_PList::setAllImagesToDefault()
{
  OFCondition result = EC_Normal;
  OFListIterator(DVPSImageBoxContent *) first = list_.begin();
  OFListIterator(DVPSImageBoxContent *) last = list_.end();
  while (first != last)
  {
    result = (*first)->setDefault();
    ++first;
    if (EC_Normal != result) return result;
  }
  return result;  
}

OFCondition DVPSImageBoxContent_PList::getImageReference(size_t idx, const char *&studyUID, const char *&seriesUID, const char *&instanceUID)
{
  DVPSImageBoxContent *box = getImageBox(idx);
  if (box) return box->getImageReference(studyUID, seriesUID, instanceUID);
  return EC_IllegalCall; 
}

OFCondition DVPSImageBoxContent_PList::prepareBasicImageBox(size_t idx, DcmItem &dset)
{
  DVPSImageBoxContent *box = getImageBox(idx);
  if (box) return box->prepareBasicImageBox(dset);
  return EC_IllegalCall; 
}

OFBool DVPSImageBoxContent_PList::presentationLUTInstanceUIDisUsed(const char *uid)
{
  OFString uidS;
  if (uid) uidS = uid;
  const char *c;
  
  OFListIterator(DVPSImageBoxContent *) first = list_.begin();
  OFListIterator(DVPSImageBoxContent *) last = list_.end();  
  while (first != last)
  {
    c = (*first)->getReferencedPresentationLUTInstanceUID();
    if (c && (uidS == c)) return OFTrue;
    ++first;
  }
  return OFFalse;
}

const char *DVPSImageBoxContent_PList::haveSinglePresentationLUTUsed(const char *filmBox)
{
  OFList<char *> uidList;
  if (filmBox==NULL) filmBox = "";
  const char *c;
  OFString aString;
  OFBool found;
  OFListIterator(char *) uidfirst;
  OFListIterator(char *) uidlast;  
  
  OFListIterator(DVPSImageBoxContent *) first = list_.begin();
  OFListIterator(DVPSImageBoxContent *) last = list_.end();  
  while (first != last)
  {
    c = (*first)->getReferencedPresentationLUTInstanceUID();
    if ((c == NULL)||(strlen(c)==0)) c = filmBox;
    aString = c;  // aString now contains the UID of the P-LUT to be used for this image, if any.
    uidfirst = uidList.begin();
    uidlast = uidList.end();  
    found = OFFalse;
    while (uidfirst != uidlast)
    {
      if (aString == (*uidfirst)) 
      {
      	found = OFTrue;
        break;
      }
      ++uidfirst;
    }
    if (!found) uidList.push_back((char *)c);
    ++first;
  }

  if (uidList.size()==1) return *(uidList.begin()); // if there is only one LUT, return it
  return NULL;
}


OFBool DVPSImageBoxContent_PList::printSCPCreate(
  unsigned long numBoxes,
  DcmUniqueIdentifier& studyUID, 
  DcmUniqueIdentifier& seriesUID, 
  const char *aetitle)
{
  clear();
  DVPSImageBoxContent *box = NULL;
  char uid[100];
  for (unsigned long i=0; i<numBoxes; i++)
  {
    box = new DVPSImageBoxContent();
    if (box)
    {
      if ((EC_Normal == box->setSOPInstanceUID(dcmGenerateUniqueIdentifier(uid))) &&
          (EC_Normal == box->setUIDsAndAETitle(studyUID, seriesUID, aetitle)))
      {
        list_.push_back(box);
      }
      else
      {
      	delete box;
      	return OFFalse;
      }
    } else return OFFalse;
  }
  return OFTrue;
}


OFCondition DVPSImageBoxContent_PList::writeReferencedImageBoxSQ(DcmItem &dset)
{
  if (size()==0) return EC_IllegalCall; // can't write if sequence is empty

  OFCondition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;
  DcmUniqueIdentifier *uid=NULL;
  const char *instanceUID=NULL;
    
  dseq = new DcmSequenceOfItems(DCM_ReferencedImageBoxSequence);
  if (dseq)
  {
    OFListIterator(DVPSImageBoxContent *) first = list_.begin();
    OFListIterator(DVPSImageBoxContent *) last = list_.end();
    while (first != last)
    {
      if (result==EC_Normal)
      {
        ditem = new DcmItem();
        if (ditem)
        {
          uid = new DcmUniqueIdentifier(DCM_ReferencedSOPClassUID);
          if (uid) result = uid->putString(UID_BasicGrayscaleImageBoxSOPClass); else result = EC_MemoryExhausted; 
          if (EC_Normal == result) result = ditem->insert(uid, OFTrue /*replaceOld*/); else delete uid;
           
          uid = new DcmUniqueIdentifier(DCM_ReferencedSOPInstanceUID);
          instanceUID = (*first)->getSOPInstanceUID();
          if (uid && instanceUID) result = uid->putString(instanceUID); else result = EC_MemoryExhausted; 
          if (EC_Normal == result) result = ditem->insert(uid, OFTrue /*replaceOld*/); else delete uid;

          if (result==EC_Normal) dseq->insert(ditem); else delete ditem;
        } else result = EC_MemoryExhausted;
      }
      ++first;
    }
    if (result==EC_Normal) dset.insert(dseq, OFTrue /*replaceOld*/); else delete dseq;
  } else result = EC_MemoryExhausted;
  return result;
}


OFBool DVPSImageBoxContent_PList::matchesPresentationLUT(DVPSPrintPresentationLUTAlignment align) const
{
  OFBool result = OFTrue;
  OFListConstIterator(DVPSImageBoxContent *) first = list_.begin();
  OFListConstIterator(DVPSImageBoxContent *) last = list_.end();  
  while (first != last)
  {
    result = result && (*first)->matchesPresentationLUT(align);
    ++first;
  }  
  return result;
}


DVPSImageBoxContent *DVPSImageBoxContent_PList::duplicateImageBox(const char *uid)
{
  if (uid == NULL) return NULL;
  
  OFString aString(uid);
  OFListIterator(DVPSImageBoxContent *) first = list_.begin();
  OFListIterator(DVPSImageBoxContent *) last = list_.end();  
  while (first != last)
  {
    if (aString == (*first)->getSOPInstanceUID()) return (*first)->clone();
    ++first;
  }  
  return NULL;
}

OFBool DVPSImageBoxContent_PList::haveImagePositionClash(const char *uid, Uint16 position)
{
  if (uid == NULL) return OFFalse;
  
  OFString aString(uid);
  OFListIterator(DVPSImageBoxContent *) first = list_.begin();
  OFListIterator(DVPSImageBoxContent *) last = list_.end();  
  while (first != last)
  {
    if (((*first)->getImageBoxPosition() == position)&&(aString != (*first)->getSOPInstanceUID())) return OFTrue; //clash
    ++first;
  }  
  return OFFalse;
}


void DVPSImageBoxContent_PList::replace(DVPSImageBoxContent *newImageBox)
{
  if (! newImageBox) return;

  OFString aString(newImageBox->getSOPInstanceUID());
  OFListIterator(DVPSImageBoxContent *) first = list_.begin();
  OFListIterator(DVPSImageBoxContent *) last = list_.end();  
  while (first != last)
  {
    if (aString == (*first)->getSOPInstanceUID())
    {
      delete (*first);
      first = list_.erase(first);
    }
    else ++first;
  }  
  list_.push_back(newImageBox);
}

OFBool DVPSImageBoxContent_PList::emptyPageWarning()
{
  OFListIterator(DVPSImageBoxContent *) first = list_.begin();
  OFListIterator(DVPSImageBoxContent *) last = list_.end();  
  while (first != last)
  {
    if ((*first)->getImageBoxPosition() > 0) return OFFalse;
    ++first;
  }
  return OFTrue;
}
