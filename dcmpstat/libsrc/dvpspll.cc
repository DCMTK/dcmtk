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
 *    classes: DVPSImageBoxContent_PList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-01-08 10:35:46 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpspll.h"
#include "dvpspl.h"      /* for DVPSImageBoxContent */
#include "dvpshlp.h"     /* for class DVPSHelper */
#include "dvpsibl.h"     /* for class DVPSImageBoxContent_PList */
#include "diluptab.h"    /* for class DiLookupTable */
#include "dvpsdef.h"

/* --------------- class DVPSImageBoxContent_PList --------------- */

DVPSPresentationLUT_PList::DVPSPresentationLUT_PList()
: OFList<DVPSPresentationLUT *>()
, logstream(&ofConsole)
, verboseMode(OFFalse)
, debugMode(OFFalse)
{
}

DVPSPresentationLUT_PList::DVPSPresentationLUT_PList(const DVPSPresentationLUT_PList &arg)
: OFList<DVPSPresentationLUT *>()
, logstream(arg.logstream)
, verboseMode(arg.verboseMode)
, debugMode(arg.debugMode)
{
  OFListIterator(DVPSPresentationLUT *) first = arg.begin();
  OFListIterator(DVPSPresentationLUT *) last = arg.end();
  while (first != last)
  {     
    push_back((*first)->clone());
    ++first;
  }
}

DVPSPresentationLUT_PList::~DVPSPresentationLUT_PList()
{
  clear();
}

void DVPSPresentationLUT_PList::clear()
{
  OFListIterator(DVPSPresentationLUT *) first = begin();
  OFListIterator(DVPSPresentationLUT *) last = end();
  while (first != last)
  {     
    delete (*first);
    first = erase(first);
  }
}

OFCondition DVPSPresentationLUT_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DVPSPresentationLUT *newLUT = NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;
  
  if (EC_Normal == dset.search(DCM_PresentationLUTContentSequence, stack, ESM_fromHere, OFFalse))
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
          newLUT->setLog(logstream, verboseMode, debugMode);
          result = newLUT->read(*ditem, OFTrue);
          push_back(newLUT);
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
  
  dseq = new DcmSequenceOfItems(DCM_PresentationLUTContentSequence);
  if (dseq)
  {
    OFListIterator(DVPSPresentationLUT *) first = begin();
    OFListIterator(DVPSPresentationLUT *) last = end();
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

void DVPSPresentationLUT_PList::setLog(OFConsole *stream, OFBool verbMode, OFBool dbgMode)
{
  if (stream) logstream = stream; else logstream = &ofConsole;
  verboseMode = verbMode;
  debugMode = dbgMode;
  OFListIterator(DVPSPresentationLUT *) first = begin();
  OFListIterator(DVPSPresentationLUT *) last = end();
  while (first != last)
  {
    (*first)->setLog(logstream, verbMode, dbgMode);
    ++first;
  }	
}

void DVPSPresentationLUT_PList::cleanup(const char *filmBox, DVPSImageBoxContent_PList& imageBoxes)
{
  OFString aFilmbox;
  if (filmBox) aFilmbox = filmBox;
  const char *uid;
  OFListIterator(DVPSPresentationLUT *) first = begin();
  OFListIterator(DVPSPresentationLUT *) last = end();
  while (first != last)
  {
    uid = (*first)->getSOPInstanceUID();
    if (uid && ((aFilmbox == uid)||(imageBoxes.presentationLUTInstanceUIDisUsed(uid)))) ++first;
    else
    {
      delete (*first);
      first = erase(first);
    }
  }	
  return; 
}

DVPSPresentationLUT *DVPSPresentationLUT_PList::findPresentationLUT(const char *instanceUID)
{
  if (instanceUID==NULL) return NULL;
  OFString instance(instanceUID);
  OFListIterator(DVPSPresentationLUT *) first = begin();
  OFListIterator(DVPSPresentationLUT *) last = end();
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
  OFListIterator(DVPSPresentationLUT *) first = begin();
  OFListIterator(DVPSPresentationLUT *) last = end();
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
  push_back(myLUT);
  result = myLUT->getSOPInstanceUID();

  return result;
}


void DVPSPresentationLUT_PList::printSCPDelete(T_DIMSE_Message& rq, T_DIMSE_Message& rsp)
{
  OFListIterator(DVPSPresentationLUT *) first = begin();
  OFListIterator(DVPSPresentationLUT *) last = end();
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
    erase(first);
  } else {
    // presentation LUT does not exist or wrong instance UID
    if (verboseMode)
    {
      logstream->lockCerr() << "error: cannot delete presentation LUT with instance UID '" << rq.msg.NDeleteRQ.RequestedSOPInstanceUID << "': object does not exist." << endl;
      logstream->unlockCerr();
    }
    rsp.msg.NDeleteRSP.DimseStatus = STATUS_N_NoSuchObjectInstance;
  }
}


/*
 *  $Log: dvpspll.cc,v $
 *  Revision 1.12  2002-01-08 10:35:46  joergr
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

