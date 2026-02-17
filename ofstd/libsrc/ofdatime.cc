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
 *  Purpose: Class for date and time functions (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

BEGIN_EXTERN_C
#include <sys/types.h>    /* for struct time_t */
END_EXTERN_C

#include "dcmtk/ofstd/ofdatime.h"
#include "dcmtk/ofstd/ofstdinc.h"
#include <ctime>


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
    /* try to split 'formattedDateTime' into a 'formattedDate' and a 'formattedTime' */
    const size_t length = formattedDateTime.length();

    /* a 'formattedDate' is at least eight characters long: YYYYMMDD */
    if (length < 8)
    {
        return OFFalse;
    }

    /* (Weak) check if a delimiter is used. If so, we'll assert that it
     * is always the same one, i.e. 'delim'.
     */
    const char delim = formattedDateTime.at(4);
    const OFBool delimsUsed = !isdigit(delim);

    /* where will 'formattedTime' start within 'formattedDateTime'? */
    size_t pos = 0;
    if (delimsUsed && (length >= 10))
    {
        /* a 'formattedDate' with delimiters is at least ten characters long: YYYY-MM-DD */
        pos = 10;
    }
    else if (!delimsUsed && (length >= 8))
    {
        /* a 'formattedDate' without delimiters is at least eight characters long: YYYYMMDD */
        pos = 8;
    }
    else
    {
        return OFFalse;
    }

    /* remember the current date in case setting the formatted time will fail */
    const OFDate preDate = getDate();

    /* try to set the formatted date */
    const OFString formattedDate = formattedDateTime.substr(0, pos);
    const OFBool setDateStatus = Date.setISOFormattedDate(formattedDate);
    /* setting the formatted date failed, time to return */
    if (!setDateStatus)
    {
        return OFFalse;
    }

    /* skip non digits in case delimiter character has been used */
    if (delimsUsed)
    {
        while ((pos < length) && !isdigit(formattedDateTime.at(pos)))
        {
            ++pos;
        }
    }
    const OFString formattedTime = formattedDateTime.substr(pos);
    const OFBool setTimeStatus = Time.setISOFormattedTime(formattedTime);
    if (!setTimeStatus)
    {
        /* setting the time failed, restore the former date */
        if (preDate.isValid())
        {
            setDate(preDate);
        }
        else
        {
            Date.clear();
        }
    }

    return setTimeStatus;
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


// -- static helper functions --

OFDateTime OFDateTime::getCurrentDateTime()
{
    /* create a date/time object with the current system date/time set */
    OFDateTime dateTime;
    /* this call might fail! */
    dateTime.setCurrentDateTime();
    /* return by-value */
    return dateTime;
}


// -- output operator --

STD_NAMESPACE ostream &operator<<(STD_NAMESPACE ostream &stream,
                                  const OFDateTime &dateTime)
{
    OFString string;
    /* print the given date and time in ISO format to the stream */
    if (dateTime.getISOFormattedDateTime(string))
        stream << string;
    return stream;
}
