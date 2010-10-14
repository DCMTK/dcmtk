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
 *  CVS/RCS Revision: $Revision: 1.21 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpspll.h"
#include "dcmtk/dcmpstat/dvpspl.h"      /* for DVPSImageBoxContent */
#include "dcmtk/dcmpstat/dvpshlp.h"     /* for class DVPSHelper */
#include "dcmtk/dcmpstat/dvpsibl.h"     /* for class DVPSImageBoxContent_PList */
#include "dcmtk/dcmimgle/diluptab.h"    /* for class DiLookupTable */
#include "dcmtk/dcmpstat/dvpsdef.h"
#include "dcmtk/dcmpstat/dvpsib.h"      /* for DVPSImageBoxContent, needed by MSVC5 with STL */

/* --------------- class DVPSImageBoxContent_PList --------------- */

DVPSPresentationLUT_PList::DVPSPresentationLUT_PList()
: list_()
{
}

DVPSPresentationLUT_PList::DVPSPresentationLUT_PList(const DVPSPresentationLUT_PList &arg)
: list_()
{
  OFListConstIterator(DVPSPresentationLUT *) first = arg.list_.begin();
  OFListConstIterator(DVPSPresentationLUT *) last = arg.list_.end();
  while (first != last)
  {     
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSPresentationLUT_PList::~DVPSPresentationLUT_PList()
{
  clear();
}

void DVPSPresentationLUT_PList::clear()
{
  OFListIterator(DVPSPresentationLUT *) first = list_.begin();
  OFListIterator(DVPSPresentationLUT *) last = list_.end();
  while (first != last)
  {     
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSPresentationLUT_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DVPSPresentationLUT *newLUT = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;
  
  if (EC_Normal == dset.search(DCM_RETIRED_PresentationLUTContentSequence, stack, ESM_fromHere, OFFalse))
  {
    dseq=(DcmSequenceOfItems *)stack.top();
    if (dseq)
    {
      unsigned long numItems = dseq->card();
      for (unsigned int i=0; i<numItems; i++)
      {
        ditem = dseq->getItem(i);
        newLUT = new DVPSPresentationLUT();
        if (newLUT && ditem)
        {
          result = newLUT->read(*ditem, OFTrue);
          list_.push_back(newLUT);
        } else result = EC_MemoryExhausted;
      }
    }
  }    
  
  return result;
}

OFCondition DVPSPresentationLUT_PList::write(DcmItem &dset)
{
  if (size()==0) return EC_Normal; // don't write if sequence is empty

  OFCondition result = EC_Normal;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;
  
  dseq = new DcmSequenceOfItems(DCM_RETIRED_PresentationLUTContentSequence);
  if (dseq)
  {
    OFListIterator(DVPSPresentationLUT *) first = list_.begin();
    OFListIterator(DVPSPresentationLUT *) last = list_.end();
    while (first != last)
    {
      if (result==EC_Normal)
      {
        ditem = new DcmItem();
        if (ditem)
        {
          result = (*first)->write(*ditem, OFTrue);
          if (result==EC_Normal) dseq->insert(ditem); else delete ditem;
        } else result = EC_MemoryExhausted;
      }
      ++first;
    }
    if (result==EC_Normal) dset.insert(dseq, OFTrue /*replaceOld*/); else delete dseq;
  } else result = EC_MemoryExhausted;
  return result;
}

void DVPSPresentationLUT_PList::cleanup(const char *filmBox, DVPSImageBoxContent_PList& imageBoxes)
{
  OFString aFilmbox;
  if (filmBox) aFilmbox = filmBox;
  const char *uid;
  OFListIterator(DVPSPresentationLUT *) first = list_.begin();
  OFListIterator(DVPSPresentationLUT *) last = list_.end();
  while (first != last)
  {
    uid = (*first)->getSOPInstanceUID();
    if (uid && ((aFilmbox == uid)||(imageBoxes.presentationLUTInstanceUIDisUsed(uid)))) ++first;
    else
    {
      delete (*first);
      first = list_.erase(first);
    }
  }	
  return; 
}

DVPSPresentationLUT *DVPSPresentationLUT_PList::findPresentationLUT(const char *instanceUID)
{
  if (instanceUID==NULL) return NULL;
  OFString instance(instanceUID);
  OFListIterator(DVPSPresentationLUT *) first = list_.begin();
  OFListIterator(DVPSPresentationLUT *) last = list_.end();
  const char *c;
  while (first != last)
  {
    c = (*first)->getSOPInstanceUID();
    if (c && (instance == c)) return (*first);
    ++first;
  }
  return NULL;
}

const char *DVPSPresentationLUT_PList::addPresentationLUT(DVPSPresentationLUT *newLUT, OFBool inversePLUT)
{
  if (newLUT == NULL) return NULL;
  
  DiLookupTable *diLUT = NULL;
  const char *result = NULL;  

  // 'INVERSE' LUT shape is undefined for Print and has already 
  // been rendered into the bitmap at this stage.
  DVPSPresentationLUTType lutType = newLUT->getType();
  if (lutType == DVPSP_inverse) lutType = DVPSP_identity; 

  DVPSPresentationLUT *myLUT = newLUT->clone();
  if (myLUT)
  {
    // make sure that we don't copy an inverse LUT shape
    if (myLUT->getType() == DVPSP_inverse) myLUT->setType(DVPSP_identity);
    if (lutType == DVPSP_table)
    {
      if (inversePLUT) myLUT->invert();  	 
      diLUT = myLUT->createDiLookupTable();
    }
  } else return NULL;

  // see if myLUT is already somewhere in the list
  OFListIterator(DVPSPresentationLUT *) first = list_.begin();
  OFListIterator(DVPSPresentationLUT *) last = list_.end();
  while (first != last)
  {
    if ((*first)->getType() == lutType)
    {
      if (lutType == DVPSP_table)
      {
   	if ((*first)->compareDiLookupTable(diLUT))
      	{
      	  result = (*first)->getSOPInstanceUID(); 
      	  break;
        }
      } else {
        result = (*first)->getSOPInstanceUID();
        break;
      }
    }
    ++first;
  }
  delete diLUT;
  
  if (result)
  {
    delete myLUT;
    return result;
  }
    
  // no match, store new LUT
  char uid[100];
  dcmGenerateUniqueIdentifier(uid);
  myLUT->setSOPInstanceUID(uid);
  list_.push_back(myLUT);
  result = myLUT->getSOPInstanceUID();

  return result;
}


void DVPSPresentationLUT_PList::printSCPDelete(T_DIMSE_Message& rq, T_DIMSE_Message& rsp)
{
  OFListIterator(DVPSPresentationLUT *) first = list_.begin();
  OFListIterator(DVPSPresentationLUT *) last = list_.end();
  OFBool found = OFFalse;
  OFString theUID(rq.msg.NDeleteRQ.RequestedSOPInstanceUID);
  while ((first != last) && (!found))
  {     
    if (theUID == (*first)->getSOPInstanceUID()) found = OFTrue;
    else ++first;
  }

  if (found)
  {
    delete (*first);
    list_.erase(first);
  } else {
    // presentation LUT does not exist or wrong instance UID
    DCMPSTAT_INFO("cannot delete presentation LUT with instance UID '" << rq.msg.NDeleteRQ.RequestedSOPInstanceUID << "': object does not exist.");
    rsp.msg.NDeleteRSP.DimseStatus = STATUS_N_NoSuchObjectInstance;
  }
}


/*
 *  $Log: dvpspll.cc,v $
 *  Revision 1.21  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.20  2009-11-24 14:12:59  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.19  2008-04-30 12:38:43  meichel
 *  Fixed compile errors due to changes in attribute tag names
 *
 *  Revision 1.18  2006/08/15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.17  2005/12/08 15:46:40  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.16  2004/02/04 15:57:49  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
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
 *  Revision 1.12  2002/01/08 10:35:46  joergr
 *  Corrected spelling of function dcmGenerateUniqueIdentifier().
 *
 *  Revision 1.11  2001/11/28 13:56:58  joergr
 *  Check return value of DcmItem::insert() statements where appropriate to
 *  avoid memory leaks when insert procedure fails.
 *
 *  Revision 1.10  2001/09/26 15:36:30  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.9  2001/06/01 15:50:35  meichel
 *  Updated copyright header
 *
 *  Revision 1.8  2000/06/07 13:17:07  meichel
 *  now using DIMSE status constants and log facilities defined in dcmnet
 *
 *  Revision 1.7  2000/06/02 16:01:04  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.6  2000/05/31 12:58:16  meichel
 *  Added initial Print SCP support
 *
 *  Revision 1.5  2000/03/08 16:29:08  meichel
 *  Updated copyright header.
 *
 *  Revision 1.4  2000/03/03 14:14:02  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.3  2000/02/29 12:16:20  meichel
 *  Fixed bug in dcmpstat library that caused Monochrome1 images
 *    to be printed inverse if a Presentation LUT was applied.
 *
 *  Revision 1.2  1999/10/19 14:48:24  meichel
 *  added support for the Basic Annotation Box SOP Class
 *    as well as access methods for Max Density and Min Density.
 *
 *  Revision 1.1  1999/10/07 17:22:00  meichel
 *  Reworked management of Presentation LUTs in order to create tighter
 *    coupling between Softcopy and Print.
 *
 *
 */
