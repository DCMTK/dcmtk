/*
 *
 *  Copyright (C) 1998-2021, OFFIS e.V.
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
 *    classes: DVPSReferencedImage
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmpstat/dvpsri.h"
#include "dcmtk/dcmpstat/dvpsdef.h"   /* for constants and macros */

/* --------------- class DVPSReferencedImage --------------- */

DVPSReferencedImage::DVPSReferencedImage()
: referencedSOPClassUID(DCM_ReferencedSOPClassUID)
, referencedSOPInstanceUID(DCM_ReferencedSOPInstanceUID)
, referencedFrameNumber(DCM_ReferencedFrameNumber)
, frameCache(NULL)
, frameCacheEntries(0)
{
}

DVPSReferencedImage::DVPSReferencedImage(const DVPSReferencedImage& copy)
: referencedSOPClassUID(copy.referencedSOPClassUID)
, referencedSOPInstanceUID(copy.referencedSOPInstanceUID)
, referencedFrameNumber(copy.referencedFrameNumber)
, frameCache(NULL) // we don't copy the frame cache
, frameCacheEntries(0)
{
}

DVPSReferencedImage::~DVPSReferencedImage()
{
  if (frameCache) delete[] frameCache;
}

OFCondition DVPSReferencedImage::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;

  flushCache();
  
  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, referencedSOPClassUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, referencedSOPInstanceUID)
  READ_FROM_DATASET(DcmIntegerString, EVR_IS, referencedFrameNumber)
  
  /* Now perform basic sanity checks */

  if (referencedSOPClassUID.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a referenced image SQ item with referencedSOPClassUID absent or empty");
  }
  else if (referencedSOPClassUID.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a referenced image SQ item with referencedSOPClassUID VM != 1");
  }

  if (referencedSOPInstanceUID.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a referenced image SQ item with referencedSOPInstanceUID absent or empty");
  }
  else if (referencedSOPInstanceUID.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a referenced image SQ item with referencedSOPInstanceUID VM != 1");
  }

  return result;
}

OFCondition DVPSReferencedImage::write(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmElement *delem=NULL;
  
  ADD_TO_DATASET(DcmUniqueIdentifier, referencedSOPClassUID)
  ADD_TO_DATASET(DcmUniqueIdentifier, referencedSOPInstanceUID)
  if (referencedFrameNumber.getLength() >0) { ADD_TO_DATASET(DcmIntegerString, referencedFrameNumber) }

  return result;
}

OFBool DVPSReferencedImage::validateSOPClassUID(OFString& sopclassuid)
{
  OFBool result = OFTrue;
  if (sopclassuid.empty()) referencedSOPClassUID.getOFString(sopclassuid, 0);
  else 
  {
    OFString currentUID;
    referencedSOPClassUID.getOFString(currentUID, 0);
    if (currentUID != sopclassuid)
    {
      result = OFFalse;
      DCMPSTAT_WARN("images of different SOP classes referenced in presentation state");
    }
  }
  return result;
}

void DVPSReferencedImage::setSOPClassUID(const char *uid)
{
  if (uid) referencedSOPClassUID.putString(uid); else referencedSOPClassUID.clear();
  return;
}

void DVPSReferencedImage::setSOPInstanceUID(const char *uid)
{
  if (uid) referencedSOPInstanceUID.putString(uid); else referencedSOPInstanceUID.clear();
  return;
}

void DVPSReferencedImage::setFrameNumbers(const char *frames)
{
  if (frames) referencedFrameNumber.putString(frames); else referencedFrameNumber.clear();
  flushCache();
  return;
}

OFBool DVPSReferencedImage::isSOPInstanceUID(const char *uid)
{
  OFString aString;
  if (uid && (EC_Normal == referencedSOPInstanceUID.getOFString(aString,0)))
  {
    if (aString == uid) return OFTrue;
  }
  return OFFalse;
}

OFCondition DVPSReferencedImage::getImageReference(
    OFString& sopclassUID,
    OFString& instanceUID, 
    OFString& frames)
{
  OFCondition result = referencedSOPClassUID.getOFString(sopclassUID,0);
  if (EC_Normal == result) result = referencedSOPInstanceUID.getOFString(instanceUID,0);
  if (EC_Normal == result) result = referencedFrameNumber.getOFStringArray(frames);
  return result;
}

void DVPSReferencedImage::flushCache()
{
  if (frameCache) delete[] frameCache;
  frameCache = NULL;
  frameCacheEntries = 0;
}

OFBool DVPSReferencedImage::appliesToAllFrames()
{
  if (referencedFrameNumber.getLength() == 0) return OFTrue;
  if (referencedFrameNumber.getVM() == 0) return OFTrue;
  return OFFalse;
}

void DVPSReferencedImage::updateCache()
{
  Sint32 val=0;
  unsigned long i;
  if (frameCache==NULL)
  {
    frameCacheEntries = (Uint32) referencedFrameNumber.getVM();
    if (frameCacheEntries > 0)
    {
      frameCache = new Sint32[frameCacheEntries];
      if (frameCache)
      {
        for (i=0; i<frameCacheEntries; i++)
        {
          if (EC_Normal == referencedFrameNumber.getSint32(val, i)) frameCache[i]=val; else frameCache[i]=0;
        }
      } else frameCacheEntries=0; // out of memory
    }
  }
}

OFBool DVPSReferencedImage::appliesToFrame(unsigned long frame)
{
  if (referencedFrameNumber.getLength()==0) return OFTrue;
  Sint32 val=0;
  unsigned long i;
  updateCache();
  if (frameCache)
  {
    val = (Sint32) frame;
    for (i=0; i<frameCacheEntries; i++) if (val == frameCache[i]) return OFTrue;
    return OFFalse;    
  }
  return OFTrue; // referencedFrameNumber seems to contain garbage.
}

OFBool DVPSReferencedImage::appliesOnlyToFrame(unsigned long frame)
{
  Sint32 val=0;
  if (referencedFrameNumber.getVM() == 1)
  {
    if (EC_Normal == referencedFrameNumber.getSint32(val, 0))
    {
      if (frame == (unsigned long)val) return OFTrue;
    }
  }
  return OFFalse;
}

void DVPSReferencedImage::removeFrameReference(unsigned long frame, unsigned long numberOfFrames)
{
  unsigned long i;
  char str[20];
  OFString aString;
  
  updateCache();
  referencedFrameNumber.clear();
  if (frameCache)
  {
    for (i=0; i<frameCacheEntries; i++) 
    {
      if (frameCache[i] != (Sint32)frame) 
      {
      	if (aString.size() ==0) sprintf(str, "%ld", (long)(frameCache[i])); else sprintf(str, "\\%ld", (long)(frameCache[i]));
      	aString += str;
      }
    }
  } else {
    for (i=1; i<=numberOfFrames; i++) 
    {
      if (i != frame)
      {
      	if (aString.size() ==0) sprintf(str, "%ld", i); else sprintf(str, "\\%ld", i);
      	aString += str;
      }
    }
  }
  referencedFrameNumber.putString(aString.c_str());
  flushCache();
}
