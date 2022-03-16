/*
 *
 *  Copyright (C) 1999-2022, OFFIS e.V.
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
    DCMPSTAT_WARN("cannot delete presentation LUT with instance UID '" << rq.msg.NDeleteRQ.RequestedSOPInstanceUID << "': object does not exist.");
    rsp.msg.NDeleteRSP.DimseStatus = STATUS_N_NoSuchSOPInstance;
  }
}
