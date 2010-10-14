/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Class for time functions (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:53 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CTIME
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>     /* for struct timeval on Linux */
#endif

#ifndef HAVE_WINDOWS_H
#ifndef HAVE_PROTOTYPE_GETTIMEOFDAY
 /* Ultrix has gettimeofday() but no prototype in the header files */
 int gettimeofday(struct timeval *tp, void *);
#endif
#endif
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
#include <windows.h>      /* for Windows time functions */
#endif

#include "dcmtk/ofstd/oftime.h"
#include "dcmtk/ofstd/ofstd.h"


/*------------------*
 *  implementation  *
 *------------------*/

OFTime::OFTime()
  : Hour(0),
    Minute(0),
    Second(0),
    TimeZone(0)
{
}


OFTime::OFTime(const OFTime &timeVal)
  : Hour(timeVal.Hour),
    Minute(timeVal.Minute),
    Second(timeVal.Second),
    TimeZone(timeVal.TimeZone)
{
}


OFTime::OFTime(const unsigned int hour,
               const unsigned int minute,
               const double second,
               const double timeZone)
  : Hour(hour),
    Minute(minute),
    Second(second),
    TimeZone(timeZone)
{
}


OFTime::~OFTime()
{
}


OFTime &OFTime::operator=(const OFTime &timeVal)
{
    Hour = timeVal.Hour;
    Minute = timeVal.Minute;
    Second = timeVal.Second;
    TimeZone = timeVal.TimeZone;
    return *this;
}


OFBool OFTime::operator==(const OFTime &timeVal) const
{
    return (getTimeInSeconds(OFTrue /*useTimeZone*/, OFFalse /*normalize*/) == timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFFalse /*normalize*/));
}


OFBool OFTime::operator!=(const OFTime &timeVal) const
{
    return (getTimeInSeconds(OFTrue /*useTimeZone*/, OFFalse /*normalize*/) != timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFFalse /*normalize*/));
}


OFBool OFTime::operator<(const OFTime &timeVal) const
{
    return (getTimeInSeconds(OFTrue /*useTimeZone*/, OFFalse /*normalize*/) < timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFFalse /*normalize*/));
}


OFBool OFTime::operator<=(const OFTime &timeVal) const
{
    return (getTimeInSeconds(OFTrue /*useTimeZone*/, OFFalse /*normalize*/) <= timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFFalse /*normalize*/));
}


OFBool OFTime::operator>=(const OFTime &timeVal) const
{
    return (getTimeInSeconds(OFTrue /*useTimeZone*/, OFFalse /*normalize*/) >= timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFFalse /*normalize*/));
}


OFBool OFTime::operator>(const OFTime &timeVal) const
{
    return (getTimeInSeconds(OFTrue /*useTimeZone*/, OFFalse /*normalize*/) > timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFFalse /*normalize*/));
}


void OFTime::clear()
{
    Hour = 0;
    Minute = 0;
    Second = 0;
    TimeZone = 0;
}


OFBool OFTime::isValid() const
{
    /* check current time settings */
    return isTimeValid(Hour, Minute, Second, TimeZone);
}


OFBool OFTime::isTimeValid(const unsigned int hour,
                           const unsigned int minute,
                           const double second,
                           const double timeZone)
{
    /* check whether given time is valid */
    return (hour < 24) && (minute < 60) && (second >= 0) && (second < 60) && (timeZone >= -12) && (timeZone <= 12);
}


OFBool OFTime::setTime(const unsigned int hour,
                       const unsigned int minute,
                       const double second,
                       const double timeZone)
{
    OFBool status = OFFalse;
    /* only change if the new time is valid */
    if (isTimeValid(hour, minute, second, timeZone))
    {
        Hour = hour;
        Minute = minute;
        Second = second;
        TimeZone = timeZone;
        /* report that a new time has been set */
        status = OFTrue;
    }
    return status;
}


OFBool OFTime::setHour(const unsigned int hour)
{
    OFBool status = OFFalse;
    /* only change the currently stored value if the new hour is valid */
    if (isTimeValid(hour, Minute, Second, TimeZone))
    {
        Hour = hour;
        /* report that a new hour has been set */
        status = OFTrue;
    }
    return status;
}


OFBool OFTime::setMinute(const unsigned int minute)
{
    OFBool status = OFFalse;
    /* only change the currently stored value if the new minute is valid */
    if (isTimeValid(Hour, minute, Second, TimeZone))
    {
        Minute = minute;
        /* report that a new minute has been set */
        status = OFTrue;
    }
    return status;
}


OFBool OFTime::setSecond(const double second)
{
    OFBool status = OFFalse;
    /* only change the currently stored value if the new second is valid */
    if (isTimeValid(Hour, Minute, second, TimeZone))
    {
        Second = second;
        /* report that a new second has been set */
        status = OFTrue;
    }
    return status;
}


OFBool OFTime::setTimeZone(const double timeZone)
{
    OFBool status = OFFalse;
    /* only change the currently stored value if the new time zone is valid */
    if (isTimeValid(Hour, Minute, Second, timeZone))
    {
        TimeZone = timeZone;
        /* report that a new time zone has been set */
        status = OFTrue;
    }
    return status;
}


OFBool OFTime::setTimeZone(const signed int hour,
                           const unsigned int minute)
{
    /* convert hour and minute values to one time zone value */
    const double timeZone = (hour < 0) ? OFstatic_cast(double, hour) - OFstatic_cast(double, minute) / 60 : OFstatic_cast(double, hour) + OFstatic_cast(double, minute) / 60;
    /* only change the currently stored value if the new time zone is valid */
    return setTimeZone(timeZone);
}


OFBool OFTime::setTimeInSeconds(const double seconds,
                                const double timeZone,
                                const OFBool normalize)
{
    OFBool status = OFFalse;
    /* only change if the new time is valid */
    if (normalize || ((seconds >= 0) && (seconds < 86400)))
    {
        /* first normalize the value first to the valid range of [0.0,86400.0[ */
        const double normalSeconds = (normalize) ? seconds + OFstatic_cast(signed long, seconds / 86400) * 86400 : seconds;
        /* compute time from given number of seconds since "00:00:00" */
        const unsigned int newHour = OFstatic_cast(unsigned int, normalSeconds / 3600);
        const unsigned int newMinute = OFstatic_cast(unsigned int, (normalSeconds - OFstatic_cast(double, newHour) * 3600) / 60);
        const double newSecond = normalSeconds - OFstatic_cast(double, newHour) * 3600 - OFstatic_cast(double, newMinute) * 60;
        status = setTime(newHour, newMinute, newSecond, timeZone);
    }
    return status;
}


OFBool OFTime::setTimeInHours(const double hours,
                              const double timeZone,
                              const OFBool normalize)
{
    OFBool status = OFFalse;
    /* only change if the new time is valid */
    if (normalize || ((hours >= 0) && (hours < 24)))
    {
        /* first normalize the value to the valid range of [0.0,24.0[ */
        const double normalHours = (normalize) ? hours + OFstatic_cast(signed long, hours / 24) * 24 : hours;
        /* compute time from given number of hours since "00:00:00" */
        const unsigned int newHour = OFstatic_cast(unsigned int, normalHours);
        const unsigned int newMinute = OFstatic_cast(unsigned int, (normalHours - OFstatic_cast(double, newHour)) * 60);
        const double newSecond = (normalHours - OFstatic_cast(double, newHour)) * 3600 - OFstatic_cast(double, newMinute) * 60;
        status = setTime(newHour, newMinute, newSecond, timeZone);
    }
    return status;
}


OFBool OFTime::setCurrentTime()
{
    /* get the current system time and call the "real" function */
    return setCurrentTime(time(NULL));
}


OFBool OFTime::setCurrentTime(const time_t &tt)
{
    OFBool status = OFFalse;
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
        /* store retrieved time */
        Hour = lt->tm_hour;
        Minute = lt->tm_min;
        Second = lt->tm_sec;
#if defined(_REENTRANT) && !defined(_WIN32) && !defined(__CYGWIN__)
        // use gmtime_r instead of gmtime
        struct tm gtBuf;
        struct tm *gt = &gtBuf;
        gmtime_r(&tt, gt);
#else
        // avoid overwriting of local time structure by calling gmtime()
        struct tm ltBuf = *lt;
        lt = &ltBuf;
        struct tm *gt = gmtime(&tt);
#endif
        if (gt != NULL)
        {
            /* retrieve and store the time zone */
            TimeZone = (lt->tm_hour - gt->tm_hour) + OFstatic_cast(double, lt->tm_min - gt->tm_min) / 60;
            /* correct for "day overflow" */
            if (TimeZone < -12)
                TimeZone += 24;
            else if (TimeZone > 12)
                TimeZone -= 24;
        } else {
            /* could not retrieve the time zone */
            TimeZone = 0;
        }
#ifdef HAVE_WINDOWS_H
        /* Windows: no microseconds available, use milliseconds instead */
        SYSTEMTIME timebuf;
        GetSystemTime(&timebuf);
        Second += OFstatic_cast(double, timebuf.wMilliseconds) / 1000;
#else /* Unix */
        struct timeval tv;
        if (gettimeofday(&tv, NULL) == 0)
            Second += OFstatic_cast(double, tv.tv_usec) / 1000000;
#endif
        /* report that current system time has been set */
        status = OFTrue;
    }
    return status;
}


OFBool OFTime::setISOFormattedTime(const OFString &formattedTime)
{
    OFBool status = OFFalse;
    const size_t length = formattedTime.length();
    unsigned int hours, minutes, seconds;
    /* check for supported formats: HHMM */
    if (length == 4)
    {
        /* extract components from time string */
        if (sscanf(formattedTime.c_str(), "%02u%02u", &hours, &minutes) == 2)
            status = setTime(hours, minutes, 0 /*seconds*/);
    }
    /* HH:MM */
    else if (length == 5)
    {
        /* extract components from time string */
        if (sscanf(formattedTime.c_str(), "%02u%*c%02u", &hours, &minutes) == 2)
            status = setTime(hours, minutes, 0 /*seconds*/);
    }
    /* HHMMSS */
    else if (length == 6)
    {
        /* extract components from time string */
        if (sscanf(formattedTime.c_str(), "%02u%02u%02u", &hours, &minutes, &seconds) == 3)
            status = setTime(hours, minutes, seconds);
    }
    /* HH:MM:SS */
    else if (length == 8)
    {
        /* extract components from time string */
        if (sscanf(formattedTime.c_str(), "%02u%*c%02u%*c%02u", &hours, &minutes, &seconds) == 3)
            status = setTime(hours, minutes, seconds);
    }
    return status;
}


unsigned int OFTime::getHour() const
{
    return Hour;
}


unsigned int OFTime::getMinute() const
{
    return Minute;
}


double OFTime::getSecond() const
{
    return Second;
}


unsigned int OFTime::getIntSecond() const
{
    /* return integral value of seconds */
    return OFstatic_cast(unsigned int, Second);
}


unsigned int OFTime::getMilliSecond() const
{
    return OFstatic_cast(unsigned int, (Second - OFstatic_cast(unsigned int, Second)) * 1000);
}


unsigned int OFTime::getMicroSecond() const
{
    return OFstatic_cast(unsigned int, (Second - OFstatic_cast(unsigned int, Second)) * 1000000);
}


double OFTime::getTimeZone() const
{
    return TimeZone;
}


double OFTime::getTimeInSeconds(const OFBool useTimeZone,
                                const OFBool normalize) const
{
    return getTimeInSeconds(Hour, Minute, Second, (useTimeZone) ? TimeZone : 0, normalize);
}


double OFTime::getTimeInHours(const OFBool useTimeZone,
                              const OFBool normalize) const
{
    return getTimeInHours(Hour, Minute, Second, (useTimeZone) ? TimeZone : 0, normalize);
}


double OFTime::getTimeInSeconds(const unsigned int hour,
                                const unsigned int minute,
                                const double second,
                                const double timeZone,
                                const OFBool normalize)
{
    /* compute number of seconds since 00:00:00 */
    double result = ((OFstatic_cast(double, hour) - timeZone) * 60 + OFstatic_cast(double, minute)) * 60 + second;
    /* normalize the result to the range [0.0,86400.0[ */
    if (normalize)
        result -= OFstatic_cast(unsigned long, result / 86400) * 86400;
    return result;
}


double OFTime::getTimeInHours(const unsigned int hour,
                              const unsigned int minute,
                              const double second,
                              const double timeZone,
                              const OFBool normalize)
{
    /* compute number of hours since 00:00:00 (incl. fraction of hours) */
    double result = OFstatic_cast(double, hour) - timeZone + (OFstatic_cast(double, minute) + second / 60) / 60;
    /* normalize the result to the range [0.0,24.0[ */
    if (normalize)
        result -= OFstatic_cast(unsigned long, result / 24) * 24;
    return result;
}


OFTime OFTime::getCoordinatedUniversalTime() const
{
    /* create a new time object */
    OFTime timeVal;
    /* convert time to UTC */
    timeVal.setTimeInHours(getTimeInHours(OFTrue /*useTimeZone*/), 0 /*timeZone*/);
    /* return by-value */
    return timeVal;
}


OFTime OFTime::getLocalTime() const
{
    /* create a new time object */
    OFTime timeVal;
    const double localTimeZone = getLocalTimeZone();
    /* convert time to local time */
    if (TimeZone != localTimeZone)
        timeVal.setTimeInHours(getTimeInHours(OFTrue /*useTimeZone*/) + localTimeZone, localTimeZone);
    else
    {
        /* same time zone, return currently stored time */
        timeVal = *this;
    }
    /* return by-value */
    return timeVal;
}


OFBool OFTime::getISOFormattedTime(OFString &formattedTime,
                                   const OFBool showSeconds,
                                   const OFBool showFraction,
                                   const OFBool showTimeZone,
                                   const OFBool showDelimiter) const
{
    OFBool status = OFFalse;
    /* check for valid time first */
    if (isValid())
    {
        char buf[32];
        /* format: HH:MM */
        if (showDelimiter)
            sprintf(buf, "%02u:%02u", Hour, Minute);
        /* format: HHMM */
        else
            sprintf(buf, "%02u%02u", Hour, Minute);
        if (showSeconds)
        {
            if (showFraction)
            {
                char buf2[12];
                OFStandard::ftoa(buf2, sizeof(buf2), Second,
                  OFStandard::ftoa_format_f | OFStandard::ftoa_zeropad, 9, 6);

                if (showDelimiter)
                    strcat(buf, ":");  /* format: HH:MM:SS.FFFFFF */
                strcat(buf, buf2);
            } else {
                /* format: HH:MM:SS*/
                if (showDelimiter)
                    sprintf(strchr(buf, 0), ":%02u", OFstatic_cast(unsigned int, Second));
                /* format: HHMMSS */
                else
                    sprintf(strchr(buf, 0), "%02u", OFstatic_cast(unsigned int, Second));
            }
        }
        if (showTimeZone)
        {
            /* convert time zone from hours and fraction of hours to hours and minutes */
            const char zoneSign = (TimeZone < 0) ? '-' : '+';
            const double zoneAbs = (TimeZone < 0) ? -TimeZone : TimeZone;
            const unsigned int zoneHour = OFstatic_cast(unsigned int, zoneAbs);
            const unsigned int zoneMinute = OFstatic_cast(unsigned int, (zoneAbs - zoneHour) * 60);
            /* format: ...+HH:MM or -HH:MM */
            if (showDelimiter)
                sprintf(strchr(buf, 0), "%c%02u:%02u", zoneSign, zoneHour, zoneMinute);
            /* format: ...+HHMM or -HHMM */
            else
                sprintf(strchr(buf, 0), "%c%02u%02u",  zoneSign, zoneHour, zoneMinute);
        }
        formattedTime = buf;
        status = OFTrue;
    }
    return status;
}


OFTime OFTime::getCurrentTime()
{
    /* create a time object with the current system time set */
    OFTime timeVal;
    /* this call might fail! */
    timeVal.setCurrentTime();
    /* return by-value */
    return timeVal;
}


double OFTime::getLocalTimeZone()
{
    double result = 0;
    /* determine local time zone */
    OFTime timeVal;
    if (timeVal.setCurrentTime())
        result = timeVal.getTimeZone();
    return result;
}


STD_NAMESPACE ostream& operator<<(STD_NAMESPACE ostream& stream, const OFTime &timeVal)
{
    OFString tmpString;
    /* print the given time in ISO format to the stream */
    if (timeVal.getISOFormattedTime(tmpString))
        stream << tmpString;
    return stream;
}


/*
 *
 * CVS/RCS Log:
 * $Log: oftime.cc,v $
 * Revision 1.17  2010-10-14 13:14:53  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.16  2009-08-19 11:55:45  meichel
 * Added additional includes needed for Sun Studio 11 on Solaris.
 *
 * Revision 1.15  2006-08-14 16:42:46  meichel
 * Updated all code in module ofstd to correctly compile if the standard
 *   namespace has not included into the global one with a "using" directive.
 *
 * Revision 1.14  2005/12/08 15:49:03  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.13  2004/01/16 10:35:18  joergr
 * Added setISOFormattedXXX() methods for Date, Time and DateTime.
 *
 * Revision 1.12  2003/12/18 16:28:22  joergr
 * Fixed bug in timezone calculation (difference of local time from UTC).
 *
 * Revision 1.11  2003/12/17 15:23:18  joergr
 * Fixed bug/inconsistency in comparison operators of class OFTime. Now the
 * "time overflow" is handled correctly.
 *
 * Revision 1.10  2003/09/17 17:01:11  joergr
 * Renamed variable "string" to avoid name clash with STL class.
 *
 * Revision 1.9  2003/09/15 12:15:07  joergr
 * Made comparison operators const.
 *
 * Revision 1.8  2003/07/09 13:58:04  meichel
 * Adapted type casts to new-style typecast operators defined in ofcast.h
 *
 * Revision 1.7  2002/12/04 10:40:50  meichel
 * Changed toolkit to use OFStandard::ftoa instead of sprintf for all
 *   double to string conversions that are supposed to be locale independent
 *
 * Revision 1.6  2002/11/27 11:23:12  meichel
 * Adapted module ofstd to use of new header file ofstdinc.h
 *
 * Revision 1.5  2002/07/18 12:14:20  joergr
 * Corrected typos.
 *
 * Revision 1.4  2002/05/24 09:44:27  joergr
 * Renamed some parameters/variables to avoid ambiguities.
 *
 * Revision 1.3  2002/04/19 10:42:55  joergr
 * Added new helper routines to get the milli and micro seconds part as well as
 * the integral value of seconds.
 *
 * Revision 1.2  2002/04/15 09:41:52  joergr
 * Removed "include <sys/types.h>" from implementation file.
 * Added "include <windows.h>" for Windows systems.
 *
 * Revision 1.1  2002/04/11 12:14:34  joergr
 * Introduced new standard classes providing date and time functions.
 *
 *
 */
