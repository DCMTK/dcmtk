/*
 *
 *  Copyright (C) 2002, OFFIS
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
 *  Module:  ofstd
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Class for date functions (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-04-11 12:14:33 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/libsrc/ofdate.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

BEGIN_EXTERN_C
#include <stdio.h>         /* for sprintf() */
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>    /* for struct time_t */
#endif
#ifdef HAVE_TIME_H
# include <time.h>         /* for time() */
#endif
END_EXTERN_C

#include "ofdate.h"


/*------------------*
 *  implementation  *
 *------------------*/

OFDate::OFDate()
  : Year(0),
    Month(0),
    Day(0)
{
}


OFDate::OFDate(const OFDate &date)
  : Year(date.Year),
    Month(date.Month),
    Day(date.Day)
{
}


OFDate::OFDate(const unsigned int year,
               const unsigned int month,
               const unsigned int day)
  : Year(year),
    Month(month),
    Day(day)
{
}


OFDate::~OFDate()
{
}


OFDate &OFDate::operator=(const OFDate &date)
{
    Year = date.Year;
    Month = date.Month;
    Day = date.Day;
    return *this;
}


OFBool OFDate::operator==(const OFDate &date)
{
    return (Year == date.Year) && (Month == date.Month) && (Day == date.Day);
}


OFBool OFDate::operator!=(const OFDate &date)
{
    return (Year != date.Year) || (Month != date.Month) || (Day != date.Day);
}


OFBool OFDate::operator<(const OFDate &date)
{
    return (Year < date.Year) || ((Year == date.Year) && ((Month < date.Month) || ((Month == date.Month) && (Day < date.Day))));
}


OFBool OFDate::operator<=(const OFDate &date)
{
    return (Year < date.Year) || ((Year == date.Year) && ((Month < date.Month) || ((Month == date.Month) && (Day <= date.Day))));
}


OFBool OFDate::operator>=(const OFDate &date)
{
    return (Year > date.Year) || ((Year == date.Year) && ((Month > date.Month) || ((Month == date.Month) && (Day >= date.Day))));
}


OFBool OFDate::operator>(const OFDate &date)
{
    return (Year > date.Year) || ((Year == date.Year) && ((Month > date.Month) || ((Month == date.Month) && (Day > date.Day))));
}


void OFDate::clear()
{
    Year = 0;
    Month = 0;
    Day = 0;
}


OFBool OFDate::isValid() const
{
    /* check current date settings */
    return isDateValid(Year, Month, Day);
}


OFBool OFDate::isDateValid(const unsigned int /*year*/,
                           const unsigned int month,
                           const unsigned int day)
{
    /* this very simple validity check might be enhanced in the future */
    return (month >= 1) && (month <= 12) && (day >= 1) && (day <= 31);
}


OFBool OFDate::setDate(const unsigned int year,
                       const unsigned int month,
                       const unsigned int day)
{
    OFBool status = OFFalse;
    /* only change if the new date is valid */
    if (isDateValid(year, month, day))
    {
        Year = year;
        Month = month;
        Day = day;
        /* report that a new date has been set */
        status = OFTrue;
    }
    return status;
}


OFBool OFDate::setYear(const unsigned int year)
{
    OFBool status = OFFalse;
    /* only change if the new year is valid */
    if (isDateValid(year, Month, Day))
    {
        Year = year;
        /* report that a new year has been set */
        status = OFTrue;
    }
    return status;
}


OFBool OFDate::setMonth(const unsigned int month)
{
    OFBool status = OFFalse;
    /* only change if the new month is valid */
    if (isDateValid(Year, month, Day))
    {
        Month = month;
        /* report that a new month has been set */
        status = OFTrue;
    }
    return status;
}


OFBool OFDate::setDay(const unsigned int day)
{
    OFBool status = OFFalse;
    /* only change if the new day is valid */
    if (isDateValid(Year, Month, day))
    {
        Day = day;
        /* report that a new day has been set */
        status = OFTrue;
    }
    return status;
}


OFBool OFDate::setCurrentDate()
{
    /* get the current system date and call the "real" function */
    return setCurrentDate(time(NULL));
}


OFBool OFDate::setCurrentDate(const time_t &tt)
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
        /* store retrieved date */
        Year = 1900 + lt->tm_year;
        Month = lt->tm_mon + 1;
        Day = lt->tm_mday;
        /* report that current system date has been set */
        status = OFTrue;
    }
    return status;
}


unsigned int OFDate::getYear() const
{
    return Year;
}


unsigned int OFDate::getMonth() const
{
    return Month;
}


unsigned int OFDate::getDay() const
{
    return Day;
}


OFBool OFDate::getISOFormattedDate(OFString &formattedDate,
                                   const OFBool showDelimiter) const
{
    OFBool status = OFFalse;
    /* check for valid date first */
    if (isValid())
    {
        char buf[32];
        /* format: YYYY-MM-DD */
        if (showDelimiter)
            sprintf(buf, "%04u-%02u-%02u", Year, Month, Day);
        /* format: YYYYMMDD */
        else
            sprintf(buf, "%04u%02u%02u", Year, Month, Day);
        formattedDate = buf;
        status = OFTrue;
    }
    return status;
}


OFDate OFDate::getCurrentDate()
{
    /* create a date object with the current system date set */
    OFDate date;
    /* this call might fail! */
    date.setCurrentDate();
    /* return by-value */
    return date;
}


ostream& operator<<(ostream& stream, const OFDate &date)
{
    OFString string;
    /* print the given date in ISO format to the stream */
    if (date.getISOFormattedDate(string))
        stream << string;
    return stream;
}


/*
 *
 * CVS/RCS Log:
 * $Log: ofdate.cc,v $
 * Revision 1.1  2002-04-11 12:14:33  joergr
 * Introduced new standard classes providing date and time functions.
 *
 *
 */
