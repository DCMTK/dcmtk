/*
 *
 *  Copyright (C) 2002-2017, OFFIS e.V.
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
#define WIN32_LEAN_AND_MEAN
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
    return (hour < 24) && (minute < 60) && (second >= 0) && (second < 60) && (timeZone >= -12) && (timeZone <= 14);
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
        const double normalSeconds = (normalize) ? seconds - OFstatic_cast(double, OFstatic_cast(signed long, seconds / 86400) * 86400) : seconds;
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
        const double normalHours = (normalize) ? hours - OFstatic_cast(double, OFstatic_cast(signed long, hours / 24) * 24) : hours;
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
#ifdef HAVE_LOCALTIME_R
    /* use localtime_r instead of localtime */
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
#ifdef HAVE_GMTIME_R
        /* use gmtime_r instead of gmtime */
        struct tm gtBuf;
        struct tm *gt = &gtBuf;
        gmtime_r(&tt, gt);
#else
        /* avoid overwriting of local time structure by calling gmtime() */
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
            else if (TimeZone > 12)   // cannot detect time zones in the range ]+12.0,+14.0]
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
    const size_t firstSep = formattedTime.find_first_not_of("0123456789");
    const OFBool separators = (firstSep != OFString_npos);
    unsigned int hours, minutes, seconds;
    /* check for supported formats: HHMM */
    if ((length == 4) && !separators)
    {
        /* extract "HH" and "MM" components from time string */
        if (sscanf(formattedTime.c_str(), "%02u%02u", &hours, &minutes) == 2)
            status = setTime(hours, minutes, 0 /*seconds*/);
    }
    /* HH:MM */
    else if ((length == 5) && separators)
    {
        /* extract "HH" and "MM" components from time string */
        if (sscanf(formattedTime.c_str(), "%02u%*c%02u", &hours, &minutes) == 2)
            status = setTime(hours, minutes, 0 /*seconds*/);
    }
    /* HHMMSS */
    else if ((length == 6) && !separators)
    {
        /* extract "HH", "MM" and "SS" components from time string */
        if (sscanf(formattedTime.c_str(), "%02u%02u%02u", &hours, &minutes, &seconds) == 3)
            status = setTime(hours, minutes, seconds);
    }
    /* HH:MM:SS */
    else if ((length == 8) && separators)
    {
        /* extract "HH", "MM" and "SS" components from time string */
        if (sscanf(formattedTime.c_str(), "%02u%*c%02u%*c%02u", &hours, &minutes, &seconds) == 3)
            status = setTime(hours, minutes, seconds);
    }
    /* HHMMSS&ZZZZ */
    else if ((length == 11) && (firstSep == 6) && ((formattedTime[6] == '+') || (formattedTime[6] == '-')))
    {
        int tzHours;
        unsigned int tzMinutes;
        /* extract "HH", "MM", "SS" and "&ZZZZ" components from time string */
        if (sscanf(formattedTime.c_str(), "%02u%02u%02u%03d%02u", &hours, &minutes, &seconds, &tzHours, &tzMinutes) == 5)
        {
            const double timeZone = (tzHours < 0) ? tzHours - OFstatic_cast(double, tzMinutes) / 60
                                                  : tzHours + OFstatic_cast(double, tzMinutes) / 60;
            status = setTime(hours, minutes, seconds, timeZone);
        }
    }
    /* HH:MM:SS &ZZ:ZZ */
    else if ((length >= 14) && separators)
    {
        /* first, extract "HH", "MM" and "SS" components from time string */
        if (sscanf(formattedTime.c_str(), "%02u%*c%02u%*c%02u", &hours, &minutes, &seconds) == 3)
        {
            size_t pos = 8;
            /* then search for the first digit of the time zone value (skip arbitrary separators) */
            while ((pos < length) && !isdigit(OFstatic_cast(unsigned char, formattedTime.at(pos))))
                ++pos;
            if (pos < length)
            {
                /* and finally, extract the time zone component from the time string */
                int tzHours;
                unsigned int tzMinutes;
                if (sscanf(formattedTime.c_str() + pos - 1, "%03d%*c%02u", &tzHours, &tzMinutes) == 2)
                {
                    const double timeZone = (tzHours < 0) ? tzHours - OFstatic_cast(double, tzMinutes) / 60
                                                          : tzHours + OFstatic_cast(double, tzMinutes) / 60;
                    status = setTime(hours, minutes, seconds, timeZone);
                }
            }
        }
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
        result -= OFstatic_cast(double, OFstatic_cast(unsigned long, result / 86400) * 86400);
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
        result -= OFstatic_cast(double, OFstatic_cast(unsigned long, result / 24) * 24);
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
                                   const OFBool showDelimiter,
                                   const OFString &timeZoneSeparator) const
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
                /* format: HH:MM:SS.FFFFFF */
                if (showDelimiter)
                    strcat(buf, ":");
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
        /* copy converted part so far to the result variable */
        formattedTime = buf;
        if (showTimeZone)
        {
            /* convert time zone from hours and fraction of hours to hours and minutes */
            const char zoneSign = (TimeZone < 0) ? '-' : '+';
            const double zoneAbs = (TimeZone < 0) ? -TimeZone : TimeZone;
            const unsigned int zoneHour = OFstatic_cast(unsigned int, zoneAbs);
            const unsigned int zoneMinute = OFstatic_cast(unsigned int, (zoneAbs - zoneHour) * 60);
            /* format: ...+HH:MM or -HH:MM */
            if (showDelimiter)
            {
                formattedTime += timeZoneSeparator;
                sprintf(buf, "%c%02u:%02u", zoneSign, zoneHour, zoneMinute);
            }
            /* format: ...+HHMM or -HHMM */
            else
                sprintf(buf, "%c%02u%02u",  zoneSign, zoneHour, zoneMinute);
            /* append time zone part to the result variable */
            formattedTime += buf;
        }
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
