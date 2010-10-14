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
 *    classes: DVPSReferencedImage
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:32 $
 *  CVS/RCS Revision: $Revision: 1.20 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmpstat/dvpsri.h"
#include "dcmtk/dcmpstat/dvpsdef.h"   /* for constants and macros */

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"


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
  
  READ_FROM_DATASET(DcmUniqueIdentifier, referencedSOPClassUID)
  READ_FROM_DATASET(DcmUniqueIdentifier, referencedSOPInstanceUID)
  READ_FROM_DATASET(DcmIntegerString, referencedFrameNumber)
  
  /* Now perform basic sanity checks */

  if (referencedSOPClassUID.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a referenced image SQ item with referencedSOPClassUID absent or empty");
  }
  else if (referencedSOPClassUID.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a referenced image SQ item with referencedSOPClassUID VM != 1");
  }

  if (referencedSOPInstanceUID.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a referenced image SQ item with referencedSOPInstanceUID absent or empty");
  }
  else if (referencedSOPInstanceUID.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_INFO("presentation state contains a referenced image SQ item with referencedSOPInstanceUID VM != 1");
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
  if (sopclassuid.length() == 0) referencedSOPClassUID.getOFString(sopclassuid, 0);
  else 
  {
    OFString currentUID;
    referencedSOPClassUID.getOFString(currentUID, 0);
    if (currentUID != sopclassuid)
    {
      result = OFFalse;
      DCMPSTAT_INFO("images of different SOP classes referenced in presentation state");
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


/*
 *  $Log: dvpsri.cc,v $
 *  Revision 1.20  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.19  2009-11-24 14:12:59  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.18  2009-09-30 10:42:39  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.17  2006-08-15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.16  2005/12/08 15:46:43  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.15  2003/10/15 16:57:14  meichel
 *  Updated error messages generated while parsing presentation states
 *
 *  Revision 1.14  2002/11/27 15:48:12  meichel
 *  Adapted module dcmpstat to use of new header file ofstdinc.h
 *
 *  Revision 1.13  2002/08/20 12:41:01  meichel
 *  Added explicit includes for header files included implicitly
 *    via dcstream before.
 *
 *  Revision 1.12  2001/09/26 15:36:31  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.11  2001/06/01 15:50:36  meichel
 *  Updated copyright header
 *
 *  Revision 1.10  2000/06/02 16:01:05  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.9  2000/05/31 13:02:38  meichel
 *  Moved dcmpstat macros and constants into a common header file
 *
 *  Revision 1.8  2000/03/08 16:29:09  meichel
 *  Updated copyright header.
 *
 *  Revision 1.7  2000/03/06 16:08:08  meichel
 *  Changed a couple of definitions that implied that Uint32 or size_t are long
 *
 *  Revision 1.6  2000/03/03 14:14:03  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.5  1999/07/30 13:34:59  meichel
 *  Added new classes managing Stored Print objects
 *
 *  Revision 1.4  1999/07/22 16:40:00  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *  Revision 1.3  1999/01/15 17:32:56  meichel
 *  added methods to DVPresentationState allowing to access the image
 *    references in the presentation state.  Also added methods allowing to
 *    get the width and height of the attached image.
 *
 *  Revision 1.2  1998/12/14 16:10:45  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:45  meichel
 *  Initial Release.
 *
 *
 */

