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
 *    classes: DVPSImageBoxContent_PList
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
#include "dvpsibl.h"
#include "dvpsib.h"      /* for DVPSImageBoxContent */

/* --------------- class DVPSImageBoxContent_PList --------------- */

DVPSImageBoxContent_PList::DVPSImageBoxContent_PList()
: OFList<DVPSImageBoxContent *>()
{
}

DVPSImageBoxContent_PList::DVPSImageBoxContent_PList(const DVPSImageBoxContent_PList &arg)
: OFList<DVPSImageBoxContent *>()
{
  OFListIterator(DVPSImageBoxContent *) first = arg.begin();
  OFListIterator(DVPSImageBoxContent *) last = arg.end();
  while (first != last)
  {     
    push_back((*first)->clone());
    ++first;
  }
}

DVPSImageBoxContent_PList::~DVPSImageBoxContent_PList()
{
  clear();
}

void DVPSImageBoxContent_PList::clear()
{
  OFListIterator(DVPSImageBoxContent *) first = begin();
  OFListIterator(DVPSImageBoxContent *) last = end();
  while (first != last)
  {     
    delete (*first);
    first = erase(first);
  }
}

E_Condition DVPSImageBoxContent_PList::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmStack stack;
  DVPSImageBoxContent *newImage = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;
  
  if (EC_Normal == dset.search(DCM_ImageBoxContentSequence, stack, ESM_fromHere, OFFalse))
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
          result = newImage->read(*ditem);
          push_back(newImage);
        } else result = EC_MemoryExhausted;
      }
    }
  }    
  
  return result;
}

E_Condition DVPSImageBoxContent_PList::write(DcmItem &dset, size_t numItems)
{
  if (size()==0) return EC_IllegalCall; // can't write if sequence is empty

  E_Condition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;
  OFBool working = OFTrue;
  
  dseq = new DcmSequenceOfItems(DCM_ImageBoxContentSequence);
  if (dseq)
  {
    OFListIterator(DVPSImageBoxContent *) first = begin();
    OFListIterator(DVPSImageBoxContent *) last = end();
    while ((first != last) && working)
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
      if (numItems && (--numItems==0)) working=OFFalse;
    }
    if (result==EC_Normal) dset.insert(dseq); else delete dseq;
  } else result = EC_MemoryExhausted;
  return result;
}

E_Condition DVPSImageBoxContent_PList::createDefaultValues(OFBool renumber)
{
  if (size()==0) return EC_IllegalCall; // can't write if sequence is empty
  E_Condition result = EC_Normal;
  unsigned long counter = 1;
  
  OFListIterator(DVPSImageBoxContent *) first = begin();
  OFListIterator(DVPSImageBoxContent *) last = end();
  while ((first != last)&&(EC_Normal == result))
  {     
    result = (*first)->createDefaultValues(renumber, counter++);
    ++first;
  }
  return result;
}

OFBool DVPSImageBoxContent_PList::haveReferencedUIDItem(DcmSequenceOfItems& seq, const char *uid)
{
  if (uid==NULL) return OFFalse;

  DcmItem *item = NULL;
  DcmUniqueIdentifier *refuid = NULL;
  unsigned long numItems = seq.card();
  DcmStack stack;
  OFString aString;
  for (unsigned long i=0; i<numItems; i++)
  {
    item = seq.getItem(i);
    stack.clear();
    if (EC_Normal == item->search(DCM_ReferencedSOPClassUID, stack, ESM_fromHere, OFFalse))
    {
      refuid = (DcmUniqueIdentifier *)(stack.top());
      if (refuid) refuid->getOFString(aString,0);
      if (aString == uid) return OFTrue;
    }
  }    
  return OFFalse;
}

E_Condition DVPSImageBoxContent_PList::addReferencedUIDItem(DcmSequenceOfItems& seq, const char *uid)
{
  if (uid==NULL) return EC_IllegalCall;
  E_Condition result = EC_Normal;

  DcmElement *delem = new DcmUniqueIdentifier(DCM_ReferencedSOPClassUID);
  if (delem)
  {
    result = delem->putString(uid);
    if (EC_Normal==result)
    {
      DcmItem *ditem = new DcmItem();
      if (ditem)
      {
        ditem->insert(delem);
        seq.insert(ditem);
      } else {
      	delete delem;
      	result=EC_MemoryExhausted;
      }
    }
  } else result=EC_MemoryExhausted;
  return result;  
}


E_Condition DVPSImageBoxContent_PList::addImageSOPClasses(DcmSequenceOfItems& seq, size_t numItems)
{
  E_Condition result = EC_Normal;
  OFBool working = OFTrue;
  const char *c = NULL;
  OFListIterator(DVPSImageBoxContent *) first = begin();
  OFListIterator(DVPSImageBoxContent *) last = end();
  while ((first != last) && working)
  {
    if (EC_Normal == result)
    {
      c = (*first)->getSOPClassUID();
      if (c && (! haveReferencedUIDItem(seq, c))) result = addReferencedUIDItem(seq, c);
    }
    ++first;
    if (numItems && (--numItems==0)) working=OFFalse;
  }
  return result;
}


E_Condition DVPSImageBoxContent_PList::addImage(DcmItem &image,char *aETitle)
{
	E_Condition result;
	DVPSImageBoxContent *box;
	box=new DVPSImageBoxContent();
	result = box->addImage(image,aETitle,size()+1);
	if (result==EC_Normal)
		result = addImageBox(box);
	return (result);

}	

E_Condition DVPSImageBoxContent_PList::addImageBox(DVPSImageBoxContent * box)
{
   
   push_back(box);
   return(EC_Normal);
	 
}

/*
 *  $Log: dvpsibl.cc,v $
 *  Revision 1.2  1999-08-26 09:29:48  thiel
 *  Extensions for the usage of the StoredPrint
 *
 *  Revision 1.1  1999/07/30 13:34:57  meichel
 *  Added new classes managing Stored Print objects
 *
 *
 */

