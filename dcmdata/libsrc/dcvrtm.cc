/*
 *
 *  Copyright (C) 1994-2001, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: class DcmTime
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-11-01 16:16:01 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrtm.cc,v $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>    /* for struct time_t */
#endif
#ifdef HAVE_TIME_H
# include <time.h>         /* for time() */
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>      /* for struct timeval on Linux */
#endif
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
# include <windows.h>      /* for GetSystemTime() */
#endif

#include "dcvrtm.h"
#include "dcdebug.h"


// ********************************


DcmTime::DcmTime(const DcmTag &tag, const Uint32 len)
: DcmByteString(tag, len)
{
    maxLength = 16;
}


// ********************************


DcmTime::DcmTime(const DcmTime& old)
: DcmByteString(old)
{
}


// ********************************


DcmTime::~DcmTime(void)
{
}


// ********************************

OFCondition
DcmTime::getOFString(
    OFString & str,
    const unsigned long pos,
    OFBool normalize)
{
    OFCondition l_error = DcmByteString::getOFString(str, pos, normalize);
    if (l_error.good() && normalize)
        normalizeString(str, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}

// ********************************

OFCondition
DcmTime::getOFStringArray(
    OFString & str,
    OFBool normalize)
{
    OFCondition l_error = DcmByteString::getOFStringArray(str, normalize);
    if (l_error.good() && normalize)
        normalizeString(str, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition
DcmTime::getCurrentTime(
    OFString &dicomTime,
    const OFBool seconds,
    const OFBool fraction)
{
    OFCondition l_error = EC_IllegalCall;
    time_t tt = time(NULL);
#if defined(_REENTRANT) && !defined(_WIN32) && !defined(__CYGWIN__)
    // use localtime_r instead of localtime
    struct tm ltBuf;
    struct tm *lt = &ltBuf;
    localtime_r(&tt, lt);
#else
    struct tm *lt = localtime(&tt);
#endif
    if (lt != NULL)
    {
        char buf[32];
        /* format: HHMM */
        sprintf(buf, "%02d%02d", lt->tm_hour, lt->tm_min);
        if (seconds)
        {
            /* format: SS */
            sprintf(strchr(buf, 0), "%02d", lt->tm_sec);
            if (fraction)
            {
#ifdef HAVE_WINDOWS_H
                /* Windows: no microseconds available, use milliseconds instead */
                SYSTEMTIME timebuf;
                GetSystemTime(&timebuf);
                /* format: .FFF000 */
                sprintf(strchr(buf, 0), ".%03i000", timebuf.wMilliseconds);
#else /* Unix */
                struct timeval tv;
                if (gettimeofday(&tv, NULL) == 0)
                    /* format: .FFFFFF */
                    sprintf(strchr(buf, 0), ".%06li", tv.tv_usec);
                else
                    /* format: .FFFFFF */
                    strcat(buf, ".000000");
#endif
            }
        }
        dicomTime = buf;
        /* no error, since at least HHMM valid */
        l_error = EC_Normal;
    } else {
        /* format: HHMM */
        dicomTime = "0000";
        if (seconds)
        {
            /* format: SS */
            dicomTime += "00";
            if (fraction)
                /* format: .FFFFFF */
                dicomTime = ".000000";
        }
    }
    return l_error;
}


OFCondition
DcmTime::setCurrentTime(
    const OFBool seconds,
    const OFBool fraction)
{
    OFString dicomTime;
    OFCondition l_error = getCurrentTime(dicomTime, seconds, fraction);
    if (l_error.good())
        l_error = putString(dicomTime.c_str());
    return l_error;
}


// ********************************


OFCondition
DcmTime::getISOFormattedTime(
    OFString &formattedTime,
    const unsigned long pos,
    const OFBool seconds,
    const OFBool fraction,
    const OFBool createMissingPart,
    const OFBool supportOldFormat)
{
    OFString dicomTime;
    OFCondition l_error = getOFString(dicomTime, pos);
    if (l_error.good())
        l_error = getISOFormattedTimeFromString(dicomTime, formattedTime, seconds, fraction, createMissingPart, supportOldFormat);
    else
        formattedTime.clear();
    return l_error;
}


OFCondition
DcmTime::getISOFormattedTimeFromString(
    const OFString &dicomTime,
    OFString &formattedTime,
    const OFBool seconds,
    const OFBool fraction,
    const OFBool createMissingPart,
    const OFBool supportOldFormat)
{
    OFCondition result = EC_IllegalParameter;
    const size_t length = dicomTime.length();
    /* minimal check for valid format */
    if (supportOldFormat || (dicomTime.find(":") == OFString_npos))
    {
        /* check for prior V3.0 version of VR=TM: HH:MM:SS.frac */
        const size_t minPos = (supportOldFormat && (length > 2) && (dicomTime[2] == ':')) ? 3 : 2;
        const size_t secPos = (supportOldFormat && (length > minPos + 2) && (dicomTime[minPos + 2] == ':')) ? minPos + 3 : minPos + 2;
        /* decimal point for fractional seconds */
        const size_t decPoint = dicomTime.find(".");
        const size_t decLength = (decPoint != OFString_npos) ? decPoint : length;
        OFString hourStr, minStr, secStr, fracStr;
        /* hours */
        if (decLength >= 2)
            hourStr = dicomTime.substr(0, 2);
        else
            hourStr = "00";
        /* minutes */
        if (decLength >= minPos + 2)
            minStr = dicomTime.substr(minPos, 2);
        else
            minStr = "00";
        /* seconds */
        if (decLength >= secPos + 2)
            secStr = dicomTime.substr(secPos, 2);
        else if (createMissingPart)
            secStr = "00";
        /* fractional seconds */
        if ((length >= secPos + 4) && (decPoint == secPos + 2))
        {
            if (length < secPos + 9)
            {
                fracStr = dicomTime.substr(secPos + 3);
                fracStr.append(secPos + 9 - length, '0');
            } else
                fracStr = dicomTime.substr(secPos + 3, 6);
        } else if (createMissingPart)
            fracStr = "000000";
        /* concatenate time components */
        formattedTime = hourStr;
        formattedTime += ":";
        formattedTime += minStr;
        if (seconds && (secStr.length() > 0))
        {
            formattedTime += ":";
            formattedTime += secStr;
            if (fraction && (fracStr.length() > 0))
            {
                formattedTime += ".";
                formattedTime += fracStr;
            }
        }
        result = EC_Normal;
    } else
        formattedTime.clear();
    return result;
}


/*
** CVS/RCS Log:
** $Log: dcvrtm.cc,v $
** Revision 1.15  2001-11-01 16:16:01  meichel
** Including <sys/time.h> if present, needed on Linux.
**
** Revision 1.14  2001/10/10 15:20:42  joergr
** Added new flag to date/time routines allowing to choose whether the old
** prior V3.0 format for the corresponding DICOM VRs is supported or not.
**
** Revision 1.13  2001/10/04 10:16:59  joergr
** Adapted new time/date routines to Windows systems.
**
** Revision 1.12  2001/10/01 15:04:45  joergr
** Introduced new general purpose functions to get/set person names, date, time
** and date/time.
**
** Revision 1.11  2001/09/25 17:20:01  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.10  2001/06/01 15:49:21  meichel
** Updated copyright header
**
** Revision 1.9  2000/03/08 16:26:51  meichel
** Updated copyright header.
**
** Revision 1.8  1999/03/31 09:26:00  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.7  1998/11/12 16:48:30  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.6  1997/08/29 13:11:49  andreas
** Corrected Bug in getOFStringArray Implementation
**
** Revision 1.5  1997/08/29 08:33:01  andreas
** - Added methods getOFString and getOFStringArray for all
**   string VRs. These methods are able to normalise the value, i. e.
**   to remove leading and trailing spaces. This will be done only if
**   it is described in the standard that these spaces are not relevant.
**   These methods do not test the strings for conformance, this means
**   especially that they do not delete spaces where they are not allowed!
**   getOFStringArray returns the string with all its parts separated by \
**   and getOFString returns only one value of the string.
**   CAUTION: Currently getString returns a string with trailing
**   spaces removed (if dcmEnableAutomaticInputDataCorrection == OFTrue) and
**   truncates the original string (since it is not copied!). If you rely on this
**   behaviour please change your application now.
**   Future changes will ensure that getString returns the original
**   string from the DICOM object (NULL terminated) inclusive padding.
**   Currently, if you call getOF... before calling getString without
**   normalisation, you can get the original string read from the DICOM object.
**
** Revision 1.4  1997/07/03 15:10:19  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/01/05 13:27:54  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
