/*
 *
 *  Copyright (C) 1998-2001, OFFIS
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
 *    classes: DVPSReferencedImage_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:50:36 $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsril.h"
#include "dvpsri.h"      /* for DVPSReferencedImage */
#include "dvpsrsl.h"     /* DVPSReferencedSeries_PList */


DVPSReferencedImage_PList::DVPSReferencedImage_PList()
: OFList<DVPSReferencedImage *>()
, logstream(&ofConsole)
, verboseMode(OFFalse)
, debugMode(OFFalse)

{
}

DVPSReferencedImage_PList::DVPSReferencedImage_PList(const DVPSReferencedImage_PList &arg)
: OFList<DVPSReferencedImage *>()
, logstream(arg.logstream)
, verboseMode(arg.verboseMode)
, debugMode(arg.debugMode)
{
  OFListIterator(DVPSReferencedImage *) first = arg.begin();
  OFListIterator(DVPSReferencedImage *) last = arg.end();
  while (first != last)
  {     
    push_back((*first)->clone());
    ++first;
  }
}

DVPSReferencedImage_PList::~DVPSReferencedImage_PList()
{
  clear();
}

void DVPSReferencedImage_PList::clear()
{
  OFListIterator(DVPSReferencedImage *) first = begin();
  OFListIterator(DVPSReferencedImage *) last = end();
  while (first != last)
  {     
    delete (*first);
    first = erase(first);
  }
}

E_Condition DVPSReferencedImage_PList::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
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
          push_back(newImage);
        } else result = EC_MemoryExhausted;
      }
    }
  }    
  
  return result;
}

E_Condition DVPSReferencedImage_PList::write(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;

  dseq = new DcmSequenceOfItems(DCM_ReferencedImageSequence);
  if (dseq)
  {
    OFListIterator(DVPSReferencedImage *) first = begin();
    OFListIterator(DVPSReferencedImage *) last = end();
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
    if (result==EC_Normal) dset.insert(dseq); else delete dseq;
  } else result = EC_MemoryExhausted;
  return result;
}

OFBool DVPSReferencedImage_PList::isValid(OFString& sopclassuid)
{
  if (size() == 0) 
  {
    if (verboseMode)
    {
      logstream->lockCerr() << "referenced image SQ contains empty item in presentation state" << endl;
      logstream->unlockCerr();
    }
    return OFFalse;
  }
  OFBool result = OFTrue;
  OFListIterator(DVPSReferencedImage *) first = begin();
  OFListIterator(DVPSReferencedImage *) last = end();
  while ((result == OFTrue) && (first != last))
  {
    result = (*first)->validateSOPClassUID(sopclassuid);
    ++first;
  }
  return result;
}

DVPSReferencedImage *DVPSReferencedImage_PList::findImageReference(const char *sopinstanceuid)
{
  OFListIterator(DVPSReferencedImage *) first = begin();
  OFListIterator(DVPSReferencedImage *) last = end();
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
  OFListIterator(DVPSReferencedImage *) first = begin();
  OFListIterator(DVPSReferencedImage *) last = end();
  while (first != last)
  {
    if ((*first)->isSOPInstanceUID(sopinstanceuid))
    {
      (*first)->removeFrameReference(frame, numberOfFrames);
      if ((*first)->appliesToAllFrames())
      {
        delete (*first);
        first = erase(first);
      } else ++first;
    } else ++first;
  }
  return;
}

void DVPSReferencedImage_PList::removeImageReference(const char *sopinstanceuid)
{
  OFListIterator(DVPSReferencedImage *) first = begin();
  OFListIterator(DVPSReferencedImage *) last = end();
  while (first != last)
  {
    if ((*first)->isSOPInstanceUID(sopinstanceuid))
    {
      delete (*first);
      first = erase(first);     
    } else ++first;
  }
  return;
}

E_Condition DVPSReferencedImage_PList::addImageReference(
    const char *sopclassUID,
    const char *instanceUID, 
    const char *frames)
{
  E_Condition result = EC_Normal;

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
      push_back(image);
    } else result = EC_MemoryExhausted;
  }
  return result;
}

E_Condition DVPSReferencedImage_PList::addImageReference(
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


E_Condition DVPSReferencedImage_PList::getImageReference(
    size_t idx,
    OFString& sopclassUID,
    OFString& instanceUID, 
    OFString& frames)
{
  if (size() <= idx) return EC_IllegalCall;
  OFListIterator(DVPSReferencedImage *) first = begin();
  OFListIterator(DVPSReferencedImage *) last = end();
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


void DVPSReferencedImage_PList::setLog(OFConsole *stream, OFBool verbMode, OFBool dbgMode)
{
  if (stream) logstream = stream; else logstream = &ofConsole;
  verboseMode = verbMode;
  debugMode = dbgMode;
  OFListIterator(DVPSReferencedImage *) first = begin();
  OFListIterator(DVPSReferencedImage *) last = end();
  while (first != last)
  {
    (*first)->setLog(logstream, verbMode, dbgMode);
    ++first;
  }	
}

/*
 *  $Log: dvpsril.cc,v $
 *  Revision 1.10  2001-06-01 15:50:36  meichel
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

