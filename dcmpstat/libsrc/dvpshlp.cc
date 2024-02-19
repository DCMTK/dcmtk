/*
 *
 *  Copyright (C) 1998-2024, OFFIS e.V.
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
 *  Module:  dcmpstat
 *
 *  Author:  Joerg Riesmeier, Marco Eichelberg
 *
 *  Purpose: DVPSHelper
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpshlp.h"
#include "dcmtk/dcmpstat/dvpsdef.h"
#include "dcmtk/dcmnet/dcompat.h"     /* compatibility routines */
#include "dcmtk/dcmdata/dctk.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>   /* for fork */
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>    /* for waitpid */
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>    /* for wait3 */
#endif
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h> /* for wait3 */
#endif
END_EXTERN_C

void DVPSHelper::currentDate(OFString &str)
{
  DcmDate::getCurrentDate(str);
}

void DVPSHelper::currentTime(OFString &str)
{
  DcmTime::getCurrentTime(str);
}

OFCondition DVPSHelper::loadFileFormat(const char *filename,
                                       DcmFileFormat *&fileformat)
{
    fileformat = new DcmFileFormat;
    OFCondition result =  fileformat->loadFile(OFFilename(filename,OFTrue));

    if (result.bad())
    {
      delete fileformat;
      fileformat = NULL;
    }

    return result;
}

OFCondition DVPSHelper::saveFileFormat(const char *filename,
                                       DcmFileFormat *fileformat, OFBool explicitVR)
{
    E_TransferSyntax xfer = EXS_LittleEndianImplicit;
    if (explicitVR) xfer = EXS_LittleEndianExplicit;

    return fileformat->saveFile(filename, xfer, EET_ExplicitLength, EGL_recalcGL, EPD_withoutPadding);
}

OFCondition DVPSHelper::putStringValue(DcmItem *item, DcmTagKey tag, const char *value)
{
    OFCondition result = EC_Normal;
    DcmTag localTag(tag);
    if (item)
    {
        DcmElement *elem = NULL;
        switch(localTag.getEVR())
        {
          case EVR_AE: elem = new DcmApplicationEntity(localTag); break;
          case EVR_AS: elem = new DcmAgeString(localTag); break;
          case EVR_CS: elem = new DcmCodeString(localTag); break;
          case EVR_DA: elem = new DcmDate(localTag); break;
          case EVR_DS: elem = new DcmDecimalString(localTag); break;
          case EVR_DT: elem = new DcmDateTime(localTag); break;
          case EVR_IS: elem = new DcmIntegerString(localTag); break;
          case EVR_TM: elem = new DcmTime(localTag); break;
          case EVR_UI: elem = new DcmUniqueIdentifier(localTag); break;
          case EVR_LO: elem = new DcmLongString(localTag); break;
          case EVR_LT: elem = new DcmLongText(localTag); break;
          case EVR_PN: elem = new DcmPersonName(localTag); break;
          case EVR_SH: elem = new DcmShortString(localTag); break;
          case EVR_ST: elem = new DcmShortText(localTag); break;
          default: result=EC_IllegalCall; break;
        }
        if (elem)
        {
          if (value) result = elem->putString(value);
        } else if (result==EC_Normal) result = EC_MemoryExhausted;
        if (EC_Normal==result) item->insert(elem, OFTrue);
    } else result = EC_IllegalCall;
    return result;
}


OFCondition DVPSHelper::putUint16Value(DcmItem *item, DcmTagKey tag, Uint16 value)
{
    OFCondition result = EC_Normal;
    DcmTag localTag(tag);
    if (item)
    {
        DcmElement *elem = new DcmUnsignedShort(localTag);
        if (elem) result = elem->putUint16(value); else result=EC_MemoryExhausted;
        if (EC_Normal==result) item->insert(elem, OFTrue);
    } else result = EC_IllegalCall;
    return result;
}

void DVPSHelper::cleanChildren()
{
#ifdef HAVE_WAITPID
    int stat_loc;
#elif defined(HAVE_WAIT3)
    struct rusage rusage;
    int        status;
#endif

#if defined(HAVE_WAITPID) || defined(HAVE_WAIT3)
    int child = 1;
    int options = WNOHANG;
    while (child > 0)
    {
#ifdef HAVE_WAITPID
      child = (int)(waitpid(-1, &stat_loc, options));
#elif defined(HAVE_WAIT3)
      child = wait3(&status, options, &rusage);
#endif
      if (child < 0)
      {
        if ((errno != ECHILD) && (errno != 0))
        {
          char buf[256];
          DCMPSTAT_ERROR("wait for child failed: " << OFStandard::strerror(errno, buf, sizeof(buf)));
        }
      }
    }
#endif
}

void DVPSHelper::setDefault(OFCondition& result, DcmElement& a_name, const char *a_value)
{
  if ((result==EC_Normal)&&(a_name.getLength()==0)) result = a_name.putString(a_value);
  return;
}

OFBool DVPSHelper::haveReferencedUIDItem(DcmSequenceOfItems& seq, const char *uid)
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
      aString.clear();
      refuid = (DcmUniqueIdentifier *)(stack.top());
      if (refuid) refuid->getOFString(aString,0);
      if (aString == uid) return OFTrue;
    }
  }
  return OFFalse;
}

OFCondition DVPSHelper::addReferencedUIDItem(DcmSequenceOfItems& seq, const char *uid)
{
  if (uid==NULL) return EC_IllegalCall;
  OFCondition result = EC_Normal;

  DcmElement *delem = new DcmUniqueIdentifier(DCM_ReferencedSOPClassUID);
  if (delem)
  {
    result = delem->putString(uid);
    if (EC_Normal==result)
    {
      DcmItem *ditem = new DcmItem();
      if (ditem)
      {
        ditem->insert(delem, OFTrue /*replaceOld*/);
        seq.insert(ditem);
      } else {
        delete delem;
        result=EC_MemoryExhausted;
      }
    }
  } else result=EC_MemoryExhausted;
  return result;
}
