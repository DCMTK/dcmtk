/*
 *
 *  Copyright (C) 2002-2026, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier, Harald Roesen
 *
 *  Purpose: Class for time functions (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oftime.h"
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/oflimits.h"
#include "dcmtk/ofstd/ofmath.h"

#include <ctime>
#include <cmath>

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


/*------------------------*
 *  constant definitions  *
 *------------------------*/

// use "Not a Number" (NaN) if available, an invalid value otherwise
#ifdef HAVE_CXX11
const double OFTime::unspecifiedSecond = OFnumeric_limits<double>::quiet_NaN();
const double OFTime::unspecifiedTimeZone = OFnumeric_limits<double>::quiet_NaN();
#else
const double OFTime::unspecifiedSecond = OFnumeric_limits<double>::max();
const double OFTime::unspecifiedTimeZone = OFnumeric_limits<double>::max();
#endif


/*------------------*
 *  implementation  *
 *------------------*/

OFTime::OFTime()
  : Hour(0),
    Minute(0),
    Second(unspecifiedSecond),
    TimeZone(unspecifiedTimeZone)
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
#ifndef __i386__
    return (getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/) == timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/));
#else
    volatile double lhs = getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/);
    volatile double rhs = timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/);
    return lhs == rhs;
#endif
}


OFBool OFTime::operator!=(const OFTime &timeVal) const
{
#ifndef __i386__
    return (getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/) != timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/));
#else
    volatile double lhs = getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/);
    volatile double rhs = timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/);
    return lhs != rhs;
#endif
}


OFBool OFTime::operator<(const OFTime &timeVal) const
{
#ifndef __i386__
    return (getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/) < timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/));
#else
    volatile double lhs = getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/);
    volatile double rhs = timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/);
    return lhs < rhs;
#endif
}


OFBool OFTime::operator<=(const OFTime &timeVal) const
{
#ifndef __i386__
    return (getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/) <= timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/));
#else
    volatile double lhs = getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/);
    volatile double rhs = timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/);
    return lhs <= rhs;
#endif
}


OFBool OFTime::operator>=(const OFTime &timeVal) const
{
#ifndef __i386__
    return (getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/) >= timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/));
#else
    volatile double lhs = getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/);
    volatile double rhs = timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/);
    return lhs >= rhs;
#endif
}


OFBool OFTime::operator>(const OFTime &timeVal) const
{
#ifndef __i386__
    return (getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/) > timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/));
#else
    volatile double lhs = getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/);
    volatile double rhs = timeVal.getTimeInSeconds(OFTrue /*useTimeZone*/, OFTrue /*normalize*/);
    return lhs > rhs;
#endif
}


void OFTime::clear()
{
    Hour = 0;
    Minute = 0;
    clearSecond();
    clearTimeZone();
}


void OFTime::clearSecond()
{
    Second = unspecifiedSecond;
}


void OFTime::clearTimeZone()
{
    TimeZone = unspecifiedTimeZone;
}


OFBool OFTime::isValid() const
{
    /* check current time settings */
    return isTimeValid(Hour, Minute, Second, TimeZone);
}


OFBool OFTime::hasSecond() const
{
    return isSecondSpecified(Second);
}


OFBool OFTime::hasFractionOfSecond() const
{
    return hasSecond() && (floor(Second) != Second);
}


OFBool OFTime::hasTimeZone() const
{
    return isTimeZoneSpecified(TimeZone);
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
    if (isTimeZoneValid(timeZone, OFFalse /*acceptUnspecified*/))
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


OFBool OFTime::setLocalTimeZone()
{
    /* set local time zone (if available) */
    return setTimeZone(getLocalTimeZone());
}


OFBool OFTime::setTimeInSeconds(const double seconds,
                                const double timeZone,
                                const OFBool normalize)
{
    OFBool status = OFFalse;
    /* first, normalize the value to the valid range of [0.0,86400.0[ */
    double normalizedSeconds = seconds;
    if (normalize)
    {
        normalizedSeconds = fmod(normalizedSeconds, 86400.0);
        if (normalizedSeconds < 0.0)
            normalizedSeconds += 86400.0;
        /* avoid floating-point rounding issues (e.g. normalizedSeconds == 86400.0) */
        if (normalizedSeconds >= 86400.0)
            normalizedSeconds = 0.0;
    }
    /* only change if the new time is valid (leap second is not supported!) */
    if ((normalizedSeconds >= 0) && (normalizedSeconds < 86400.0))
    {
        /* compute time from given number of seconds since "00:00:00" */
        unsigned int newHour = OFstatic_cast(unsigned int, normalizedSeconds / 3600.0);
        unsigned int newMinute = OFstatic_cast(unsigned int, (normalizedSeconds - OFstatic_cast(double, newHour) * 3600.0) / 60);
        double newSecond = normalizedSeconds - OFstatic_cast(double, newHour) * 3600.0 - OFstatic_cast(double, newMinute) * 60;
        /* avoid floating-point rounding issues (e.g. newSecond == 60.0) */
        if (newSecond >= 60.0)
        {
            newSecond = 0.0;
            if (++newMinute >= 60)
            {
                newMinute = 0;
                newHour = (newHour + 1) % 24;
            }
        }
        status = setTime(newHour, newMinute, newSecond, timeZone);
    }
    return status;
}


OFBool OFTime::setTimeInHours(const double hours,
                              const double timeZone,
                              const OFBool normalize)
{
    /* convert hours to seconds and delegate to setTimeInSeconds() */
    return setTimeInSeconds(hours * 3600.0, timeZone, normalize);
}


OFBool OFTime::setCurrentTime()
{
    /* get the current system time and call the "real" function */
    return setCurrentTime(time(NULL), OFTrue /*includeFraction*/);
}


OFBool OFTime::setCurrentTime(const time_t &tt,
                              const OFBool includeFraction)
{
    OFBool status = OFFalse;
#ifdef HAVE_LOCALTIME_R
    /* use localtime_r instead of localtime */
    struct tm ltBuf;
    struct tm *lt = localtime_r(&tt, &ltBuf);
#else
    struct tm *lt = localtime(&tt);
#endif
    if (lt != NULL)
    {
        /* store retrieved time */
        const unsigned int hours = lt->tm_hour;
        const unsigned int minutes = lt->tm_min;
        double seconds = lt->tm_sec;
        double timeZone = unspecifiedTimeZone;
#ifdef HAVE_GMTIME_R
        /* use gmtime_r instead of gmtime */
        struct tm gtBuf;
        struct tm *gt = gmtime_r(&tt, &gtBuf);
#else
        /* avoid overwriting of local time structure by calling gmtime() */
        struct tm ltBuf = *lt;
        lt = &ltBuf;
        struct tm *gt = gmtime(&tt);
#endif
        if (gt != NULL)
        {
            /* determine day difference between local time and UTC */
            int dayDiff = 0;
            if (lt->tm_year > gt->tm_year)
                dayDiff = 1;
            else if (lt->tm_year < gt->tm_year)
                dayDiff = -1;
            else if (lt->tm_yday > gt->tm_yday)
                dayDiff = 1;
            else if (lt->tm_yday < gt->tm_yday)
                dayDiff = -1;
            /* retrieve and store the time zone (supports range [-12.0, +14.0]) */
            timeZone = (lt->tm_hour - gt->tm_hour + dayDiff * 24) + OFstatic_cast(double, lt->tm_min - gt->tm_min) / 60.0;
        }
        /* add fractional part of a second (if requested) */
        if (includeFraction)
        {
#ifdef HAVE_WINDOWS_H
            /* Windows: no microseconds available, use milliseconds instead */
            SYSTEMTIME timebuf;
            GetSystemTime(&timebuf);
            seconds += OFstatic_cast(double, timebuf.wMilliseconds) / 1000;
#else /* Unix */
            struct timeval tv;
            if (gettimeofday(&tv, NULL) == 0)
                seconds += OFstatic_cast(double, tv.tv_usec) / 1000000;
#endif
        }
        /* avoid invalid second value in case of leap second (tm_sec == 60) */
        if (seconds > 60.0)
            seconds = 60.0;
        /* set the determined time information (if valid) */
        status = setTime(hours, minutes, seconds, timeZone);
    }
    return status;
}


OFBool OFTime::setISOFormattedTime(const OFString &formattedTime)
{
    const size_t length = formattedTime.length();
    if (length < 3) {
        /* not enough input to scan */
        return OFFalse;
    }

    size_t pos = 0;
    const char delim = formattedTime.at(pos + 2);
    const OFBool delimsUsed = !isdigit(OFstatic_cast(unsigned char, delim));

    unsigned int hours = 0;
    unsigned int minutes = 0;
    double seconds = OFTime::unspecifiedSecond;
    double timeZone = OFTime::unspecifiedTimeZone;

    {
        /* scan for HHMM or HH:MM - hours and minutes */
        OFString hhmmFormat("%2u");
        if (delimsUsed)
        {
            hhmmFormat += delim;
        }
        hhmmFormat += "%2u%n";

        const int nCharactersExpected = (delimsUsed ? 5 : 4);
        int nCharactersRead = 0;
        const int nReceivedArgs = sscanf(formattedTime.c_str() + pos, hhmmFormat.c_str(), &hours, &minutes, &nCharactersRead);
        if ((nReceivedArgs == 2) && (nCharactersRead == nCharactersExpected))
        {
            pos += nCharactersRead;
        }
        else
        {
            /* found no valid HH[:]MM 'pattern' */
            return OFFalse;
        }
    }

    /* is there still input available that starts with a digit or a delimiter (if used)? */
    if ((pos < length) && (isdigit(OFstatic_cast(unsigned char, formattedTime.at(pos))) || (delimsUsed && (formattedTime.at(pos) == delim))))
    {
        /* scan for SS or :SS - seconds */
        OFString ssFormat;
        if (delimsUsed)
        {
            ssFormat += delim;
        }
        ssFormat += "%2u%n";

        unsigned int secondsInt = 0;
        const int nCharactersExpected = (delimsUsed ? 3 : 2);
        int nCharactersRead = 0;
        const int nReceivedArgs = sscanf(formattedTime.c_str() + pos, ssFormat.c_str(), &secondsInt, &nCharactersRead);
        if ((nReceivedArgs == 1) && (nCharactersRead == nCharactersExpected))
        {
            pos += nCharactersRead;
            seconds = secondsInt;
        }
        else
        {
            /* found no valid [:]SS 'pattern' */
            return OFFalse;
        }
    }

    /* if seconds are used, is there still input available that starts with a dot ('.') character? */
    if (isSecondSpecified(seconds) && (pos < length) && (formattedTime.at(pos) == '.'))
    {
        /* scan for .FFFFFF - fractional seconds */
        char buffer[8] = {'\0'};
        buffer[0] = '.';
        ++pos; /* overread the dot ('.') character at front */
        int nCharactersRead = 0;
        const int nReceivedArgs = sscanf(formattedTime.c_str() + pos, "%6[0123456789]%n", (buffer + 1), &nCharactersRead);
        OFBool fsSuccess = OFFalse;
        const double fractionalSeconds = OFStandard::atof(buffer, &fsSuccess);
        if ((nReceivedArgs == 1) && fsSuccess)
        {
            pos += nCharactersRead;
            seconds += fractionalSeconds;
        }
        else
        {
            /* found no valid .FFFFFF 'pattern' */
            return OFFalse;
        }
    }

    /* skip whitespaces in case delimiter character has been used */
    if ((pos < length) && delimsUsed)
    {
        const OFBool hasSpaceBeenSkipped = isspace(formattedTime.at(pos));
        while ((pos < length) && isspace(formattedTime.at(pos)))
        {
           ++pos;
        }
        if ((pos == length) && hasSpaceBeenSkipped)
        {
          /* we have skipped trailing spaces but reached end of input,
           * this means that the input is/was malformed in the first place
           */
          return OFFalse;
        }
    }

    /* is there still input available that also starts with a plus ('+') or minus ('-') character?
     */
    if ((pos < length) && ((formattedTime.at(pos) == '+') || (formattedTime.at(pos) == '-')))
    {
        /* scan for &ZZZZ or &ZZ:ZZ - time zone */
        OFString tzFormat("%3d");
        if (delimsUsed)
        {
            tzFormat += delim;
        }
        tzFormat += "%2u%n";

        int tzHours;
        unsigned int tzMinutes;
        int nCharactersRead = 0;
        const int nReceivedArgs = sscanf(formattedTime.c_str() + pos, tzFormat.c_str(), &tzHours, &tzMinutes, &nCharactersRead);
        if (nReceivedArgs == 2)
        {
            timeZone = (tzHours < 0) ? tzHours - OFstatic_cast(double, tzMinutes) / 60
                                     : tzHours + OFstatic_cast(double, tzMinutes) / 60;
            pos += nCharactersRead;
        }
        else
        {
            /* found no valid &ZZ[:]ZZ 'pattern' */
            return OFFalse;
        }
    }

    /* is there still input available? */
    if (pos < length)
    {
        /* too much input */
        return OFFalse;
    }

    /* all 'eaten up' */
    return setTime(hours, minutes, seconds, timeZone);
}


OFBool OFTime::setISOFormattedTimeZone(const OFString &formattedTimeZone)
{
    OFBool status = OFFalse;
    const size_t length = formattedTimeZone.length();
    const OFBool hasDelimiter = (formattedTimeZone.find_first_not_of("0123456789", 1) != OFString_npos);
    /* format: +HHMM or -HHMM */
    if ((length == 5) && ((formattedTimeZone[0] == '+') || (formattedTimeZone[0] == '-')))
    {
        int tzHours;
        unsigned int tzMinutes;
        /* extract "HH", "MM" from time zone string */
        if (sscanf(formattedTimeZone.c_str(), "%03d%02u", &tzHours, &tzMinutes) == 2)
            status = setTimeZone(tzHours, tzMinutes);
    }
    /* format: +HH:MM or -HH:MM */
    else if ((length == 6) && hasDelimiter && ((formattedTimeZone[0] == '+') || (formattedTimeZone[0] == '-')))
    {
       int tzHours;
       unsigned int tzMinutes;
       /* ignore the delimiter "%c" */
       if (sscanf(formattedTimeZone.c_str(), "%03d%*c%02u", &tzHours, &tzMinutes) == 2)
          status = setTimeZone(tzHours, tzMinutes);
    }
    /* empty value */
    else if (length == 0)
    {
        TimeZone = unspecifiedTimeZone;
        status = OFTrue;
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
    return (hasSecond() ? OFstatic_cast(unsigned int, Second) : 0);
}


unsigned int OFTime::getMilliSecond() const
{
    return (hasSecond() ? OFstatic_cast(unsigned int, (Second - OFstatic_cast(unsigned int, Second)) * 1000) : 0);
}


unsigned int OFTime::getMicroSecond() const
{
    return (hasSecond() ? OFstatic_cast(unsigned int, (Second - OFstatic_cast(unsigned int, Second)) * 1000000) : 0);
}


double OFTime::getTimeZone() const
{
    // tbc: check for valid value?
    return TimeZone;
}


double OFTime::getTimeInSeconds(const OFBool useTimeZone,
                                const OFBool normalize) const
{
    /* delegate to static helper function */
    return getTimeInSeconds(Hour, Minute, Second, (useTimeZone) ? TimeZone : unspecifiedTimeZone, normalize);
}


double OFTime::getTimeInHours(const OFBool useTimeZone,
                              const OFBool normalize) const
{
    /* delegate to static helper function */
    return getTimeInHours(Hour, Minute, Second, (useTimeZone) ? TimeZone : unspecifiedTimeZone, normalize);
}


double OFTime::getTimeInSeconds(const unsigned int hour,
                                const unsigned int minute,
                                const double second,
                                const double timeZone,
                                const OFBool normalize)
{
    const double secondValue = isSecondSpecified(second) ? second : 0;
    const double timeZoneOffset = isTimeZoneSpecified(timeZone) ? timeZone : 0;
    /* compute number of seconds since 00:00:00 */
    double result = ((OFstatic_cast(double, hour) - timeZoneOffset) * 60 + OFstatic_cast(double, minute)) * 60 + secondValue;
    /* normalize the value to the valid range of [0.0,86400.0[ if requested */
    if (normalize)
    {
        result = fmod(result, 86400.0);
        if (result < 0.0)
            result += 86400.0;
        /* avoid floating-point rounding issues (e.g. result == 86400.0) */
        if (result >= 86400.0)
            result = 0.0;
    }
    return result;
}


double OFTime::getTimeInHours(const unsigned int hour,
                              const unsigned int minute,
                              const double second,
                              const double timeZone,
                              const OFBool normalize)
{
    /* compute seconds and convert to hours */
    return getTimeInSeconds(hour, minute, second, timeZone, normalize) / 3600.0;

}


OFTime OFTime::getCoordinatedUniversalTime() const
{
    /* create a new time object */
    OFTime timeVal;
    if (isValid())
    {
        /* convert time to UTC (use seconds to avoid double-conversion) */
        if (timeVal.setTimeInSeconds(getTimeInSeconds(OFTrue /*useTimeZone*/), 0 /*timeZone*/))
        {
            /* preserve the presence or absence of seconds */
            if (!hasSecond())
                timeVal.clearSecond();
        }
    }
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
                                   const OFBool createMissingPart,
                                   const OFString &timeZoneSeparator) const
{
    OFBool status = OFFalse;
    /* check for valid time first */
    if (isValid())
    {
        char buf[32];
        /* format: HH:MM */
        if (showDelimiter)
            OFStandard::snprintf(buf, sizeof(buf), "%02u:%02u", Hour, Minute);
        /* format: HHMM */
        else
            OFStandard::snprintf(buf, sizeof(buf), "%02u%02u", Hour, Minute);
        /* only show seconds if requested and a value is specified (or should be created) */
        if (showSeconds && (hasSecond() || createMissingPart))
        {
            const double secondValue = isSecondSpecified(Second) ? Second : 0;
            /* only show fractional part of a second if requested and a value is specified (or should be created) */
            if (showFraction && (hasFractionOfSecond() || createMissingPart))
            {
                char buf2[12];
                OFStandard::ftoa(buf2, sizeof(buf2), secondValue, OFStandard::ftoa_format_f | OFStandard::ftoa_zeropad, 9, 6);
                /* format: HH:MM:SS.FFFFFF */
                if (showDelimiter)
                    OFStandard::strlcat(buf, ":", sizeof(buf));
                OFStandard::strlcat(buf, buf2, sizeof(buf));
            } else {
                char buf2[12];
                /* format: HH:MM:SS */
                if (showDelimiter)
                    OFStandard::snprintf(buf2, sizeof(buf2), ":%02u", OFstatic_cast(unsigned int, secondValue));
                /* format: HHMMSS */
                else
                    OFStandard::snprintf(buf2, sizeof(buf2), "%02u", OFstatic_cast(unsigned int, secondValue));
                OFStandard::strlcat(buf, buf2, sizeof(buf));
            }
        }
        /* copy converted part so far to the result variable */
        formattedTime.assign(buf);
        /* only add the time zone if requested _and_ a value is specified */
        if (showTimeZone)
        {
            OFString formattedTimeZone;
            if (getISOFormattedTimeZone(formattedTimeZone, showDelimiter))
            {
                if (!formattedTimeZone.empty())
                {
                    if (showDelimiter)
                        formattedTime += timeZoneSeparator;
                    /* append time zone part to the result variable */
                    formattedTime += formattedTimeZone;
                }
            }
        }
        status = OFTrue;
    }
    return status;
}


OFBool OFTime::getISOFormattedTimeZone(OFString &formattedTimeZone,
                                       const OFBool showDelimiter) const
{
    OFBool status = OFFalse;
    /* first, check for valid time zone (either specified or unspecified) */
    if (isTimeZoneValid(TimeZone, OFTrue /*acceptUnspecified*/))
    {
        /* check whether time zone is specified */
        if (isTimeZoneSpecified(TimeZone))
        {
            char buf[32];
            /* convert time zone from hours and fraction of hours to hours and minutes */
            const char tzSign = (TimeZone < 0) ? '-' : '+';
            const double tzAbs = (TimeZone < 0) ? -TimeZone : TimeZone;
            const unsigned int tzHours = OFstatic_cast(unsigned int, tzAbs);
            const unsigned int tzMinutes = OFstatic_cast(unsigned int, (tzAbs - tzHours) * 60);
            /* format: +HH:MM or -HH:MM */
            if (showDelimiter)
                OFStandard::snprintf(buf, sizeof(buf), "%c%02u:%02u", tzSign, tzHours, tzMinutes);
            /* format: +HHMM or -HHMM */
            else
                OFStandard::snprintf(buf, sizeof(buf), "%c%02u%02u", tzSign, tzHours, tzMinutes);
            /* assign value to the result variable */
            formattedTimeZone.assign(buf);
        } else {
            /* clear result variable if time zone is unspecified */
            formattedTimeZone.clear();
        }
        status = OFTrue;
    }
    return status;
}


// -- static helper functions --

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
    double result = unspecifiedTimeZone;
    /* determine local time zone */
    OFTime timeVal;
    if (timeVal.setCurrentTime())
        result = timeVal.getTimeZone();
    return result;
}


OFBool OFTime::isTimeValid(const unsigned int hour,
                           const unsigned int minute,
                           const double second,
                           const double timeZone)
{
    /* check whether given time is valid (also support leap second) */
    return (hour < 24) && (minute < 60) && (!isSecondSpecified(second) || ((second >= 0) &&  (second <= 60))) &&
        isTimeZoneValid(timeZone, OFTrue /*acceptUnspecified*/);
}


OFBool OFTime::isSecondSpecified(const double second)
{
#ifdef HAVE_CXX11
    return !(OFMath::isnan)(second);
#else
    return (second != unspecifiedSecond);
#endif
}


OFBool OFTime::isTimeZoneValid(const double timeZone,
                               const OFBool acceptUnspecified)
{
    const OFBool timeZoneSpecified = isTimeZoneSpecified(timeZone);
    /* check whether given time zone is unspecified and/or within the valid range */
    return (acceptUnspecified && !timeZoneSpecified) || (timeZoneSpecified && (timeZone >= -12) && (timeZone <= 14));
}


OFBool OFTime::isTimeZoneSpecified(const double timeZone)
{
#ifdef HAVE_CXX11
    return !(OFMath::isnan)(timeZone);
#else
    return (timeZone != unspecifiedTimeZone);
#endif
}


// -- output operator --

STD_NAMESPACE ostream &operator<<(STD_NAMESPACE ostream &stream,
                                  const OFTime &timeVal)
{
    OFString tmpString;
    /* print the given time in ISO format to the stream */
    if (timeVal.getISOFormattedTime(tmpString, OFTrue /*showSeconds*/, OFTrue /*showFraction*/, OFTrue /*showTimeZone*/))
        stream << tmpString;
    return stream;
}
