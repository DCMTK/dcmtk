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
 *    classes: DVPSReferencedImage_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-11-27 14:50:46 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsril.h"
#include "dvpsri.h"      /* for DVPSReferencedImage */


DVPSReferencedImage_PList::DVPSReferencedImage_PList()
: OFList<DVPSReferencedImage *>()
{
}

DVPSReferencedImage_PList::DVPSReferencedImage_PList(const DVPSReferencedImage_PList &arg)
: OFList<DVPSReferencedImage *>()
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
#ifdef DEBUG
    cerr << "referenced image SQ contains empty item in presentation state" << endl;
#endif
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
    Sint32 frame)
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
      if (frame>0) image->setFrameNumber(frame);
      push_back(image);
    } else result = EC_MemoryExhausted;
  }
  return result;
}


/*
 *  $Log: dvpsril.cc,v $
 *  Revision 1.1  1998-11-27 14:50:46  meichel
 *  Initial Release.
 *
 *
 */

