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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:32 $
 *  CVS/RCS Revision: $Revision: 1.31 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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

/*
 *  $Log: dvpsibl.cc,v $
 *  Revision 1.31  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.30  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.29  2008-04-30 12:38:43  meichel
 *  Fixed compile errors due to changes in attribute tag names
 *
 *  Revision 1.28  2005/12/08 15:46:34  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.27  2004/02/04 15:57:49  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.26  2003/06/12 18:23:11  joergr
 *  Modified code to use const_iterators where appropriate (required for STL).
 *
 *  Revision 1.25  2003/06/04 10:18:07  meichel
 *  Replaced private inheritance from template with aggregation
 *
 *  Revision 1.24  2002/01/08 10:35:45  joergr
 *  Corrected spelling of function dcmGenerateUniqueIdentifier().
 *
 *  Revision 1.23  2001/11/28 13:56:57  joergr
 *  Check return value of DcmItem::insert() statements where appropriate to
 *  avoid memory leaks when insert procedure fails.
 *
 *  Revision 1.22  2001/09/26 15:36:28  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.21  2001/06/01 15:50:33  meichel
 *  Updated copyright header
 *
 *  Revision 1.20  2000/07/04 16:06:47  joergr
 *  Added support for overriding the presentation LUT settings made for the
 *  image boxes.
 *
 *  Revision 1.19  2000/06/14 11:29:00  joergr
 *  Added methods to access the attributes Polarity and Requested Image Size.
 *
 *  Revision 1.18  2000/06/08 10:44:35  meichel
 *  Implemented Referenced Presentation LUT Sequence on Basic Film Session level.
 *    Empty film boxes (pages) are not written to file anymore.
 *
 *  Revision 1.17  2000/06/07 14:26:46  joergr
 *  Added methods to access the image polarity attribute.
 *
 *  Revision 1.16  2000/06/02 16:01:02  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.15  2000/05/31 12:58:15  meichel
 *  Added initial Print SCP support
 *
 *  Revision 1.14  2000/03/08 16:29:07  meichel
 *  Updated copyright header.
 *
 *  Revision 1.13  2000/03/07 16:23:37  joergr
 *  Removed type specifier 'const' to make Sun CC 2.0.1 happy.
 *
 *  Revision 1.12  2000/03/06 15:53:24  meichel
 *  Fixed unassigned variable usage problem
 *
 *  Revision 1.11  2000/03/03 14:14:00  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.10  1999/10/07 17:21:59  meichel
 *  Reworked management of Presentation LUTs in order to create tighter
 *    coupling between Softcopy and Print.
 *
 *  Revision 1.9  1999/09/24 15:24:07  meichel
 *  Print spooler (dcmprtsv) now logs diagnostic messages in log files
 *    when operating in spool mode.
 *
 *  Revision 1.8  1999/09/17 14:33:52  meichel
 *  Completed print spool functionality including Supplement 22 support
 *
 *  Revision 1.7  1999/09/15 17:43:34  meichel
 *  Implemented print job dispatcher code for dcmpstat, adapted dcmprtsv
 *    and dcmpsprt applications.
 *
 *  Revision 1.6  1999/09/10 12:46:56  meichel
 *  Added implementations for a number of print API methods.
 *
 *  Revision 1.5  1999/09/01 16:15:08  meichel
 *  Added support for requested image size to print routines
 *
 *  Revision 1.4  1999/08/31 14:09:27  meichel
 *  Added get/set methods for stored print attributes
 *
 *  Revision 1.3  1999/08/27 15:57:49  meichel
 *  Added methods for saving hardcopy images and stored print objects
 *    either in file or in the local database.
 *
 *  Revision 1.2  1999/08/26 09:29:48  thiel
 *  Extensions for the usage of the StoredPrint
 *
 *  Revision 1.1  1999/07/30 13:34:57  meichel
 *  Added new classes managing Stored Print objects
 *
 *
 */
