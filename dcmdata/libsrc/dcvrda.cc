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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: class DcmDate
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2001-10-04 10:16:58 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrda.cc,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
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
END_EXTERN_C

#include "dcvrda.h"
#include "dcdebug.h"


// ********************************


DcmDate::DcmDate(const DcmTag &tag, const Uint32 len)
: DcmByteString(tag, len)
{
    maxLength = 10;
}


// ********************************


DcmDate::DcmDate( const DcmDate &newDA )
: DcmByteString(newDA)
{
}


// ********************************


DcmDate::~DcmDate()
{
}


// ********************************


OFCondition
DcmDate::getCurrentDate(OFString &dicomDate)
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
        /* format: YYYYMMDD */
        sprintf(buf, "%04d%02d%02d", 1900 + lt->tm_year, lt->tm_mon + 1, lt->tm_mday);
        dicomDate = buf;
        l_error = EC_Normal;
    } else {
        /* format: YYYYMMDD */
        dicomDate = "19000101";
    }
    return l_error;
}


OFCondition
DcmDate::setCurrentDate()
{
    OFString dicomDate;
    OFCondition l_error = getCurrentDate(dicomDate);
    if (l_error == EC_Normal)
        l_error = putString(dicomDate.c_str());
    return l_error;
}


// ********************************


OFCondition
DcmDate::getISOFormattedDate(
    OFString &formattedDate,
    const unsigned long pos)
{
    OFString dicomDate;
    OFCondition l_error = getOFString(dicomDate, pos);
    if (l_error == EC_Normal)
        l_error = getISOFormattedDateFromString(dicomDate, formattedDate);
    else
        formattedDate.clear();
    return l_error;
}


OFCondition
DcmDate::getISOFormattedDateFromString(
    const OFString &dicomDate,
    OFString &formattedDate)
{
    OFCondition l_error = EC_IllegalCall;
    const size_t length = dicomDate.length();
    /* fixed length (8 or 10 bytes) required by DICOM part 5 */
    if (length == 8)
    {
        /* year: YYYY */
        formattedDate = dicomDate.substr(0, 4);
        formattedDate += '-';
        /* month: MM */
        formattedDate += dicomDate.substr(4, 2);
        formattedDate += '-';
        /* day: DD */
        formattedDate += dicomDate.substr(6, 2);
        l_error = EC_Normal;
    }
    /* old prior V3.0 version of VR=DA: YYYY.MM.DD */
    else if ((length == 10) && (dicomDate[4] == '.') && (dicomDate[7] == '.'))
    {
        /* year: YYYY */
        formattedDate = dicomDate.substr(0, 4);
        formattedDate += '-';
        /* month: MM, skip '.' */
        formattedDate += dicomDate.substr(5, 2);
        formattedDate += '-';
        /* day: DD, skip '.' */
        formattedDate += dicomDate.substr(8, 2);
        l_error = EC_Normal;
    } else
        formattedDate = dicomDate;
    return l_error;
}


/*
** CVS/RCS Log:
** $Log: dcvrda.cc,v $
** Revision 1.10  2001-10-04 10:16:58  joergr
** Adapted new time/date routines to Windows systems.
**
** Revision 1.9  2001/10/01 15:04:43  joergr
** Introduced new general purpose functions to get/set person names, date, time
** and date/time.
**
** Revision 1.8  2001/06/01 15:49:15  meichel
** Updated copyright header
**
** Revision 1.7  2000/03/08 16:26:46  meichel
** Updated copyright header.
**
** Revision 1.6  1999/03/31 09:25:49  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.5  1998/11/12 16:48:23  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.4  1997/07/03 15:10:10  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/01/05 13:27:47  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
