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
 *  Module:  dcmpstat
 *
 *  Author:  Joerg Riesmeier, Marco Eichelberg
 *
 *  Purpose: DVPSHelper
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-04-11 13:13:45 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpshlp.h"
#include "dcompat.h"     /* compatability routines */
#include "dcvrda.h"
#include "dcvrtm.h"
#include <stdio.h>
#include <errno.h>

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
#ifdef HAVE_TIME_H
#include <time.h>
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
    DcmFileStream stream(filename, DCM_ReadMode);
    if (!stream.Fail())
    {
        fileformat = new DcmFileFormat;
        if (fileformat != NULL)
        {
            OFCondition status = EC_Normal;
            fileformat->transferInit();
            if ((status = fileformat->read(stream)) == EC_Normal)
                fileformat->transferEnd();
            return status;
        }
        return EC_MemoryExhausted;
    }
    return stream.GetError();
}

OFCondition DVPSHelper::saveFileFormat(const char *filename,
                                       DcmFileFormat *fileformat, OFBool explicitVR)
{
    E_TransferSyntax xfer = EXS_LittleEndianImplicit;
    if (explicitVR) xfer = EXS_LittleEndianExplicit;

    DcmFileStream stream(filename, DCM_WriteMode);
    if (!stream.Fail())
    {
        if (fileformat != NULL)
        {
            OFCondition status = EC_Normal;
            fileformat->transferInit();
            status = fileformat->write(stream, xfer, EET_ExplicitLength, EGL_recalcGL, EPD_withoutPadding);
            fileformat->transferEnd();
            return status;
        }
        return EC_IllegalCall;
    }
    return stream.GetError();
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

void DVPSHelper::cleanChildren(OFConsole *logconsole)
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
            if (logconsole)
            {
              logconsole->lockCerr() << "wait for child failed: " << strerror(errno) << endl;
              logconsole->unlockCerr();
            }
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
 *  Revision 1.11  2002-04-11 13:13:45  joergr
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
