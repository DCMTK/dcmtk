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
 *    classes: DVPSReferencedImage_PList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:32 $
 *  CVS/RCS Revision: $Revision: 1.23 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmpstat/dvpsril.h"
#include "dcmtk/dcmpstat/dvpsri.h"      /* for DVPSReferencedImage */
#include "dcmtk/dcmpstat/dvpsrsl.h"     /* DVPSReferencedSeries_PList */
#include "dcmtk/dcmpstat/dvpsrs.h"      /* for DVPSReferencedSeries, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsdef.h"


DVPSReferencedImage_PList::DVPSReferencedImage_PList()
: list_()
{
}

DVPSReferencedImage_PList::DVPSReferencedImage_PList(const DVPSReferencedImage_PList &arg)
: list_()
{
  OFListConstIterator(DVPSReferencedImage *) first = arg.list_.begin();
  OFListConstIterator(DVPSReferencedImage *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSReferencedImage_PList::~DVPSReferencedImage_PList()
{
  clear();
}

void DVPSReferencedImage_PList::clear()
{
  OFListIterator(DVPSReferencedImage *) first = list_.begin();
  OFListIterator(DVPSReferencedImage *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSReferencedImage_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DVPSReferencedImage *newImage = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  if (EC_Normal == dset.search(DCM_ReferencedImageSequence, stack, ESM_fromHere, OFFalse))
  {
    dseq=(DcmSequenceOfItems *)stack.top();
    if (dseq)
    {
      unsigned long numItems = dseq->card();
      for (unsigned int i=0; i<numItems; i++)
      {
        ditem = dseq->getItem(i);
        newImage = new DVPSReferencedImage();
        if (newImage && ditem)
        {
          result = newImage->read(*ditem);
          list_.push_back(newImage);
        } else result = EC_MemoryExhausted;
      }
    }
  }

  return result;
}

OFCondition DVPSReferencedImage_PList::write(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  dseq = new DcmSequenceOfItems(DCM_ReferencedImageSequence);
  if (dseq)
  {
    OFListIterator(DVPSReferencedImage *) first = list_.begin();
    OFListIterator(DVPSReferencedImage *) last = list_.end();
    while (first != last)
    {
      if (result==EC_Normal)
      {
        ditem = new DcmItem();
        if (ditem)
        {
          result = (*first)->write(*ditem);
          if (result==EC_Normal) dseq->insert(ditem); else delete ditem;
        } else result = EC_MemoryExhausted;
      }
      ++first;
    }
    if (result==EC_Normal) dset.insert(dseq, OFTrue /*replaceOld*/); else delete dseq;
  } else result = EC_MemoryExhausted;
  return result;
}

OFBool DVPSReferencedImage_PList::isValid(OFString& sopclassuid)
{
  if (size() == 0)
  {
    DCMPSTAT_INFO("referenced image SQ contains empty item in presentation state");
    return OFFalse;
  }
  OFBool result = OFTrue;
  OFListIterator(DVPSReferencedImage *) first = list_.begin();
  OFListIterator(DVPSReferencedImage *) last = list_.end();
  while ((result == OFTrue) && (first != last))
  {
    result = (*first)->validateSOPClassUID(sopclassuid);
    ++first;
  }
  return result;
}

DVPSReferencedImage *DVPSReferencedImage_PList::findImageReference(const char *sopinstanceuid)
{
  OFListIterator(DVPSReferencedImage *) first = list_.begin();
  OFListIterator(DVPSReferencedImage *) last = list_.end();
  while (first != last)
  {
    if ((*first)->isSOPInstanceUID(sopinstanceuid)) return *first;
    ++first;
  }
  return NULL;
}

void DVPSReferencedImage_PList::removeFrameReference(const char *sopinstanceuid, unsigned long frame, unsigned long numberOfFrames)
{
  if ((frame<1)||(numberOfFrames<frame)) return;
  OFListIterator(DVPSReferencedImage *) first = list_.begin();
  OFListIterator(DVPSReferencedImage *) last = list_.end();
  while (first != last)
  {
    if ((*first)->isSOPInstanceUID(sopinstanceuid))
    {
      (*first)->removeFrameReference(frame, numberOfFrames);
      if ((*first)->appliesToAllFrames())
      {
        delete (*first);
        first = list_.erase(first);
      } else ++first;
    } else ++first;
  }
  return;
}

void DVPSReferencedImage_PList::removeImageReference(const char *sopinstanceuid)
{
  OFListIterator(DVPSReferencedImage *) first = list_.begin();
  OFListIterator(DVPSReferencedImage *) last = list_.end();
  while (first != last)
  {
    if ((*first)->isSOPInstanceUID(sopinstanceuid))
    {
      delete (*first);
      first = list_.erase(first);
    } else ++first;
  }
  return;
}

OFCondition DVPSReferencedImage_PList::addImageReference(
    const char *sopclassUID,
    const char *instanceUID,
    const char *frames)
{
  OFCondition result = EC_Normal;

  /* make sure that we don't create two references to the same image */
  if (findImageReference(instanceUID)) result = EC_IllegalCall;
  else
  {
    DVPSReferencedImage *image = new DVPSReferencedImage();
    if (image)
    {
      image->setSOPClassUID(sopclassUID);
      image->setSOPInstanceUID(instanceUID);
      if (frames) image->setFrameNumbers(frames);
      list_.push_back(image);
    } else result = EC_MemoryExhausted;
  }
  return result;
}

OFCondition DVPSReferencedImage_PList::addImageReference(
    const char *sopclassUID,
    const char *instanceUID,
    unsigned long frame,
    DVPSObjectApplicability applicability)
{
  if ((sopclassUID==NULL)||(instanceUID==NULL)||(applicability==DVPSB_allImages)) return EC_IllegalCall;
  const char *framenumber=NULL;
  if (applicability==DVPSB_currentFrame)
  {
    char frameString[100];
    sprintf(frameString, "%ld", frame);
    framenumber = frameString;
  }
  return addImageReference(sopclassUID, instanceUID, framenumber);
}


OFCondition DVPSReferencedImage_PList::getImageReference(
    size_t idx,
    OFString& sopclassUID,
    OFString& instanceUID,
    OFString& frames)
{
  if (size() <= idx) return EC_IllegalCall;
  OFListIterator(DVPSReferencedImage *) first = list_.begin();
  OFListIterator(DVPSReferencedImage *) last = list_.end();
  while (first != last)
  {
    if (idx==0) return (*first)->getImageReference(sopclassUID, instanceUID, frames); else
    {
      idx--;
      ++first;
    }
  }
  return EC_IllegalCall;
}

void DVPSReferencedImage_PList::removeImageReference(
    DVPSReferencedSeries_PList& allReferences,
    const char *instanceUID,
    unsigned long frame,
    unsigned long numberOfFrames,
    DVPSObjectApplicability applicability)
{

  if (applicability == DVPSB_allImages)
  {
    clear();
    return;
  }

  // if list of image references is empty, add all existing references
  if (size() == 0)
  {
    OFString seriesUID;
    OFString sopclassUID;
    OFString sopinstanceUID;
    OFString frames;
    OFString aetitle;
    OFString filesetID;
    OFString filesetUID;
    const char *cframes=NULL;

    size_t numberOfReferences = allReferences.numberOfImageReferences();
    for (size_t i=0; i<numberOfReferences; i++)
    {
      sopclassUID.clear();
      sopinstanceUID.clear();
      frames.clear();
      if (EC_Normal == allReferences.getImageReference(i, seriesUID, sopclassUID, sopinstanceUID, frames,
        aetitle, filesetID, filesetUID))
      {
      	if (frames.size() > 0) cframes=frames.c_str(); else cframes=NULL;
      	addImageReference(sopclassUID.c_str(), sopinstanceUID.c_str(), cframes);
      }
    }
  }
  if (applicability == DVPSB_currentImage) removeImageReference(instanceUID);
  else removeFrameReference(instanceUID, frame, numberOfFrames);
  return;
}

OFBool DVPSReferencedImage_PList::isApplicable(const char *instanceUID, unsigned long frame)
{
  if (size() == 0) return OFTrue; // if no image references exist, the object is valid "globally".
  DVPSReferencedImage *imageRef = findImageReference(instanceUID);
  if (imageRef) return imageRef->appliesToFrame(frame);
  return OFFalse;
}

OFBool DVPSReferencedImage_PList::matchesApplicability(const char *instanceUID, unsigned long frame, DVPSObjectApplicability applicability)
{
  DVPSReferencedImage *imageRef = NULL;
  switch (applicability)
  {
    case DVPSB_currentFrame:
      // we match if referenced image SQ contains exactly one item
      // referencing only the current frame of the current image
      if (size() == 1)
      {
      	imageRef = findImageReference(instanceUID);
      	if (imageRef) return imageRef->appliesOnlyToFrame(frame);
      }
      break;
    case DVPSB_currentImage:
      // we match if referenced image SQ contains exactly one item
      // referencing all frames of the current image
      if (size() == 1)
      {
      	imageRef = findImageReference(instanceUID);
      	if (imageRef) return imageRef->appliesToAllFrames();
      }
      break;
    case DVPSB_allImages:
      // applicability matches if referenced image SQ is empty
      if (size() == 0) return OFTrue;
      break;
  }
  return OFFalse;
}


/*
 *  $Log: dvpsril.cc,v $
 *  Revision 1.23  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.22  2009-11-24 14:12:59  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.21  2009-09-30 10:42:39  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.20  2006-08-15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.19  2005/12/08 15:46:44  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.18  2004/02/04 15:57:49  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.17  2003/10/15 16:57:14  meichel
 *  Updated error messages generated while parsing presentation states
 *
 *  Revision 1.16  2003/09/05 08:37:46  meichel
 *  Fixed minor issue that caused certain error messages during the
 *    parse process on a GSPS object to be "swallowed".
 *
 *  Revision 1.15  2003/06/12 18:23:11  joergr
 *  Modified code to use const_iterators where appropriate (required for STL).
 *
 *  Revision 1.14  2003/06/04 12:30:28  meichel
 *  Added various includes needed by MSVC5 with STL
 *
 *  Revision 1.13  2003/06/04 10:18:07  meichel
 *  Replaced private inheritance from template with aggregation
 *
 *  Revision 1.12  2001/11/28 13:56:59  joergr
 *  Check return value of DcmItem::insert() statements where appropriate to
 *  avoid memory leaks when insert procedure fails.
 *
 *  Revision 1.11  2001/09/26 15:36:31  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.10  2001/06/01 15:50:36  meichel
 *  Updated copyright header
 *
 *  Revision 1.9  2000/06/29 13:56:21  joergr
 *  Fixed bug causing a non-terminating "while" loop.
 *
 *  Revision 1.8  2000/06/02 16:01:05  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.7  2000/03/08 16:29:09  meichel
 *  Updated copyright header.
 *
 *  Revision 1.6  2000/03/06 18:24:08  joergr
 *  Renamed local variable hiding a member variable (reported by Sun CC 4.2).
 *
 *  Revision 1.5  2000/03/03 14:14:04  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.4  1999/07/22 16:40:01  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *  Revision 1.3  1999/01/15 17:32:56  meichel
 *  added methods to DVPresentationState allowing to access the image
 *    references in the presentation state.  Also added methods allowing to
 *    get the width and height of the attached image.
 *
 *  Revision 1.2  1998/12/14 16:10:46  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:46  meichel
 *  Initial Release.
 *
 *
 */
