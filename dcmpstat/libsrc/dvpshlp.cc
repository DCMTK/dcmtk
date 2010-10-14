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
 *  Module:  dcmpstat
 *
 *  Author:  Joerg Riesmeier, Marco Eichelberg
 *
 *  Purpose: DVPSHelper
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:32 $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpshlp.h"
#include "dcmtk/dcmpstat/dvpsdef.h"
#include "dcmtk/dcmnet/dcompat.h"     /* compatability routines */
#include "dcmtk/dcmdata/dctk.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CERRNO
#define INCLUDE_CTIME
#include "dcmtk/ofstd/ofstdinc.h"

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
    OFCondition result =  fileformat->loadFile(filename);

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
#elif HAVE_WAIT3
    struct rusage rusage;
#if defined(__NeXT__)
    /* some systems need a union wait as argument to wait3 */
    union wait status;
#else
    int        status;
#endif
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


/*
 *  CVS/RCS Log:
 *  $Log: dvpshlp.cc,v $
 *  Revision 1.19  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.18  2010-06-03 10:33:00  joergr
 *  Replaced calls to strerror() by new helper function OFStandard::strerror()
 *  which results in using the thread safe version of strerror() if available.
 *
 *  Revision 1.17  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.16  2009-09-30 10:42:38  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.15  2006-08-15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.14  2005/12/08 15:46:32  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.13  2002/11/27 15:48:11  meichel
 *  Adapted module dcmpstat to use of new header file ofstdinc.h
 *
 *  Revision 1.12  2002/08/20 12:22:03  meichel
 *  Adapted code to new loadFile and saveFile methods, thus removing direct
 *    use of the DICOM stream classes.
 *
 *  Revision 1.11  2002/04/11 13:13:45  joergr
 *  Replaced direct call of system routines by new standard date and time
 *  functions.
 *
 *  Revision 1.10  2001/11/28 13:56:55  joergr
 *  Check return value of DcmItem::insert() statements where appropriate to
 *  avoid memory leaks when insert procedure fails.
 *
 *  Revision 1.9  2001/09/26 15:36:27  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.8  2001/06/01 15:50:32  meichel
 *  Updated copyright header
 *
 *  Revision 1.7  2000/12/19 13:45:49  joergr
 *  Added #include statement to keep gcc 2.5.8 (NeXTSTEP) quiet.
 *
 *  Revision 1.6  2000/10/16 11:46:15  joergr
 *  Added check to avoid wrong warning messages when shutting down application
 *  externally.
 *
 *
 */
