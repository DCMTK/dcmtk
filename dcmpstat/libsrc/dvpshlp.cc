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
 *  Module:  dcmpstat
 *
 *  Author:  Joerg Riesmeier, Marco Eichelberg
 *
 *  Purpose: DVPSHelper
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-09-24 11:13:51 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpshlp.h"
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
  char buf[32];
  time_t tt = time(NULL);
  struct tm *ts = localtime(&tt);
  if (ts)
  {
    int year = 1900 + ts->tm_year;
    sprintf(buf, "%04d%02d%02d", year, ts->tm_mon + 1, ts->tm_mday);
    str = buf;
  } else str = "19000101";
  return;
}

void DVPSHelper::currentTime(OFString &str)
{
  char buf[32];
  time_t tt = time(NULL);
  struct tm *ts = localtime(&tt);
  if (ts)
  {
    sprintf(buf, "%02d%02d%02d", ts->tm_hour, ts->tm_min, ts->tm_sec);
    str = buf;
  } else str = "000000";
  return;
}

E_Condition DVPSHelper::loadFileFormat(const char *filename,
                                        DcmFileFormat *&fileformat)
{
    DcmFileStream stream(filename, DCM_ReadMode);
    if (!stream.Fail())
    {
        fileformat = new DcmFileFormat;
        if (fileformat != NULL)
        {
            E_Condition status;
            fileformat->transferInit();
            if ((status = fileformat->read(stream)) == EC_Normal)
                fileformat->transferEnd();
            return status;
        }
        return EC_MemoryExhausted;
    }
    return stream.GetError();
}

E_Condition DVPSHelper::saveFileFormat(const char *filename,
                                        DcmFileFormat *fileformat, OFBool explicitVR)
{
    E_TransferSyntax xfer = EXS_LittleEndianImplicit;
    if (explicitVR) xfer = EXS_LittleEndianExplicit;
    
    DcmFileStream stream(filename, DCM_WriteMode);
    if (!stream.Fail())
    {
        if (fileformat != NULL)
        {
            E_Condition status;
            fileformat->transferInit();
            status = fileformat->write(stream, xfer, EET_ExplicitLength, EGL_recalcGL, EPD_withoutPadding);
            fileformat->transferEnd();
            return status;
        }
        return EC_IllegalCall;
    }
    return stream.GetError();
}

E_Condition DVPSHelper::putStringValue(DcmItem *item, DcmTagKey tag, const char *value)
{
    E_Condition result = EC_Normal;
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


E_Condition DVPSHelper::putUint16Value(DcmItem *item, DcmTagKey tag, Uint16 value)
{
    E_Condition result = EC_Normal;
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
       if (errno != ECHILD) cerr << "wait for child failed: " << strerror(errno) << endl;
    }
    }
#endif
}

void DVPSHelper::setDefault(E_Condition& result, DcmElement& a_name, const char *a_value)
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

E_Condition DVPSHelper::addReferencedUIDItem(DcmSequenceOfItems& seq, const char *uid)
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


/*
 *  CVS/RCS Log:
 *
 */
