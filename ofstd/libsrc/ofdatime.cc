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
 *  Purpose: Class for date and time functions (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#define INCLUDE_CTIME
#define INCLUDE_CCTYPE
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>    /* for struct time_t */
#endif
END_EXTERN_C

#include "dcmtk/ofstd/ofdatime.h"


/*------------------*
 *  implementation  *
 *------------------*/

OFDateTime::OFDateTime()
  : Date(),
    Time()
{
}


OFDateTime::OFDateTime(const OFDateTime &dateTime)
  : Date(dateTime.Date),
    Time(dateTime.Time)
{
}


OFDateTime::OFDateTime(const OFDate &dateVal,
                       const OFTime &timeVal)
  : Date(dateVal),
    Time(timeVal)
{
}


OFDateTime::OFDateTime(const unsigned int year,
                       const unsigned int month,
                       const unsigned int day,
                       const unsigned int hour,
                       const unsigned int minute,
                       const double second,
                       const double timeZone)
  : Date(year, month, day),
    Time(hour, minute, second, timeZone)
{
}


OFDateTime::~OFDateTime()
{
}


OFDateTime &OFDateTime::operator=(const OFDateTime &dateTime)
{
    Date = dateTime.Date;
    Time = dateTime.Time;
    return *this;
}


OFBool OFDateTime::operator==(const OFDateTime &dateTime) const
{
    /* note that the "overflow" from one day to another is currently not handled */
    return (Date == dateTime.Date) && (Time == dateTime.Time);
}


OFBool OFDateTime::operator!=(const OFDateTime &dateTime) const
{
    /* note that the "overflow" from one day to another is currently not handled */
    return (Date != dateTime.Date) || (Time != dateTime.Time);
}


OFBool OFDateTime::operator<(const OFDateTime &dateTime) const
{
    /* note that the "overflow" from one day to another is currently not handled */
    return (Date < dateTime.Date) || ((Date == dateTime.Date) && (Time < dateTime.Time));
}


OFBool OFDateTime::operator<=(const OFDateTime &dateTime) const
{
    /* note that the "overflow" from one day to another is currently not handled */
    return (Date < dateTime.Date) || ((Date == dateTime.Date) && (Time <= dateTime.Time));
}


OFBool OFDateTime::operator>(const OFDateTime &dateTime) const
{
    return (dateTime < *this);
}


OFBool OFDateTime::operator>=(const OFDateTime &dateTime) const
{
    return (dateTime <= *this);
}


void OFDateTime::clear()
{
    Date.clear();
    Time.clear();
}


OFBool OFDateTime::isValid() const
{
    /* check current date settings */
    return Date.isValid() && Time.isValid();
}


OFBool OFDateTime::setDateTime(const unsigned int year,
                               const unsigned int month,
                               const unsigned int day,
                               const unsigned int hour,
                               const unsigned int minute,
                               const double second,
                               const double timeZone)
{
    OFBool result = OFFalse;
    /* check whether new date and time are valid */
    if (OFDate::isDateValid(year, month, day) && OFTime::isTimeValid(hour, minute, second, timeZone))
        result = Date.setDate(year, month, day) && Time.setTime(hour, minute, second, timeZone);
    return result;
}


OFBool OFDateTime::setDate(const OFDate &dateVal)
{
    OFBool result = OFFalse;
    /* check whether new date is valid */
    if (dateVal.isValid())
    {
        Date = dateVal;
        result = OFTrue;
    }
    return result;
}


OFBool OFDateTime::setTime(const OFTime &timeVal)
{
    OFBool result = OFFalse;
    /* check whether new time is valid */
    if (timeVal.isValid())
    {
        Time = timeVal;
        result = OFTrue;
    }
    return result;
}


OFBool OFDateTime::setDateTime(const OFDate &dateVal,
                               const OFTime &timeVal)
{
    const OFBool result = setDate(dateVal);
    /* make sure that time value is set even if date is invalid */
    return setTime(timeVal) && result;
}


OFBool OFDateTime::setCurrentDateTime()
{
    /* set current system date and time, use the same "time_t" struct to avoid inconsistencies */
    time_t tt = time(NULL);
    return Date.setCurrentDate(tt) && Time.setCurrentTime(tt);
}


OFBool OFDateTime::setISOFormattedDateTime(const OFString &formattedDateTime)
{
    const size_t length = formattedDateTime.length();

    size_t tm_pos = 0;
    size_t date_digits = 0;
    size_t date_size = 0;

    /* find end of YYYYMMDD or YYYY-MM-DD */
    while (tm_pos < length && date_digits < 8) {
        if (isdigit(formattedDateTime.at(tm_pos)))
            date_digits++;
        tm_pos++;
    }

    /* check if we found enough number of digits for date and symbols for time to continue parsing */
    if (date_digits < 8 || tm_pos == length)
        return OFFalse;

    date_size = tm_pos;

    if (formattedDateTime.at(tm_pos) == 'T') {
        /* dateTtime, skip T */
        tm_pos++;
    } else {
        /* skip heading spaces from time */
        while (tm_pos < length && formattedDateTime.at(tm_pos) == ' ')
            tm_pos++;
    }

    /* check if we have enough symbols for time to continue parsing */
    if (tm_pos == length)
        return OFFalse;

    /* parse ISO formatted date */
    if (!Date.setISOFormattedDate(formattedDateTime.substr(0, date_size)))
        return OFFalse;

    /* parse ISO formatted time */
    if (!Time.setISOFormattedTime(formattedDateTime.substr(tm_pos)))
        return OFFalse;

    return OFTrue;
}


const OFDate &OFDateTime::getDate() const
{
    return Date;
}


const OFTime &OFDateTime::getTime() const
{
    return Time;
}


OFBool OFDateTime::getISOFormattedDateTime(OFString &formattedDateTime,
                                           const OFBool showSeconds,
                                           const OFBool showFraction,
                                           const OFBool showTimeZone,
                                           const OFBool showDelimiter,
                                           const OFString &dateTimeSeparator,
                                           const OFString &timeZoneSeparator) const
{
    /* get formatted date first component */
    OFBool result = Date.getISOFormattedDate(formattedDateTime, showDelimiter);
    if (result)
    {
        /* ... then get the formatted time component */
        OFString timeString;
        if (Time.getISOFormattedTime(timeString, showSeconds, showFraction, showTimeZone, showDelimiter, timeZoneSeparator))
        {
            if (showDelimiter)
                formattedDateTime += dateTimeSeparator;
            formattedDateTime += timeString;
        }
    } else {
        /* clear result variable in case of error */
        formattedDateTime.clear();
    }
    return result;
}


OFDateTime OFDateTime::getCurrentDateTime()
{
    /* create a date/time object with the current system date/time set */
    OFDateTime dateTime;
    /* this call might fail! */
    dateTime.setCurrentDateTime();
    /* return by-value */
    return dateTime;
}


STD_NAMESPACE ostream& operator<<(STD_NAMESPACE ostream& stream, const OFDateTime &dateTime)
{
    OFString string;
    /* print the given date and time in ISO format to the stream */
    if (dateTime.getISOFormattedDateTime(string))
        stream << string;
    return stream;
}
