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
 *  Purpose: Class for date functions (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:53 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofdate.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CTIME
#include "dcmtk/ofstd/ofstdinc.h"


/*------------------*
 *  implementation  *
 *------------------*/

OFDate::OFDate()
  : Year(0),
    Month(0),
    Day(0)
{
}


OFDate::OFDate(const OFDate &dateVal)
  : Year(dateVal.Year),
    Month(dateVal.Month),
    Day(dateVal.Day)
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


OFDate &OFDate::operator=(const OFDate &dateVal)
{
    Year = dateVal.Year;
    Month = dateVal.Month;
    Day = dateVal.Day;
    return *this;
}


OFBool OFDate::operator==(const OFDate &dateVal) const
{
    return (Year == dateVal.Year) && (Month == dateVal.Month) && (Day == dateVal.Day);
}


OFBool OFDate::operator!=(const OFDate &dateVal) const
{
    return (Year != dateVal.Year) || (Month != dateVal.Month) || (Day != dateVal.Day);
}


OFBool OFDate::operator<(const OFDate &dateVal) const
{
    return (Year < dateVal.Year) || ((Year == dateVal.Year) && ((Month < dateVal.Month) || ((Month == dateVal.Month) && (Day < dateVal.Day))));
}


OFBool OFDate::operator<=(const OFDate &dateVal) const
{
    return (Year < dateVal.Year) || ((Year == dateVal.Year) && ((Month < dateVal.Month) || ((Month == dateVal.Month) && (Day <= dateVal.Day))));
}


OFBool OFDate::operator>=(const OFDate &dateVal) const
{
    return (Year > dateVal.Year) || ((Year == dateVal.Year) && ((Month > dateVal.Month) || ((Month == dateVal.Month) && (Day >= dateVal.Day))));
}


OFBool OFDate::operator>(const OFDate &dateVal) const
{
    return (Year > dateVal.Year) || ((Year == dateVal.Year) && ((Month > dateVal.Month) || ((Month == dateVal.Month) && (Day > dateVal.Day))));
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


OFBool OFDate::setISOFormattedDate(const OFString &formattedDate)
{
    OFBool status = OFFalse;
    const size_t length = formattedDate.length();
    unsigned int year, month, day;
    /* we expect the following formats: YYYY-MM-DD with arbitrary delimiters ... */
    if (length == 10)
    {
        /* extract components from date string */
        if (sscanf(formattedDate.c_str(), "%04u%*c%02u%*c%02u", &year, &month, &day) == 3)
            status = setDate(year, month, day);
    }
    /* ... or YYYYMMDD (without delimiters) */
    else if (length == 8)
    {
        /* extract components from date string */
        if (sscanf(formattedDate.c_str(), "%04u%02u%02u", &year, &month, &day) == 3)
            status = setDate(year, month, day);
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
    OFDate dateVal;
    /* this call might fail! */
    dateVal.setCurrentDate();
    /* return by-value */
    return dateVal;
}


STD_NAMESPACE ostream& operator<<(STD_NAMESPACE ostream& stream, const OFDate &dateVal)
{
    OFString string;
    /* print the given date in ISO format to the stream */
    if (dateVal.getISOFormattedDate(string))
        stream << string;
    return stream;
}


/*
 *
 * CVS/RCS Log:
 * $Log: ofdate.cc,v $
 * Revision 1.9  2010-10-14 13:14:53  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.8  2006/08/14 16:42:46  meichel
 * Updated all code in module ofstd to correctly compile if the standard
 *   namespace has not included into the global one with a "using" directive.
 *
 * Revision 1.7  2005/12/08 15:48:55  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.6  2004/01/16 10:35:18  joergr
 * Added setISOFormattedXXX() methods for Date, Time and DateTime.
 *
 * Revision 1.5  2003/09/15 12:15:07  joergr
 * Made comparison operators const.
 *
 * Revision 1.4  2002/11/27 11:23:10  meichel
 * Adapted module ofstd to use of new header file ofstdinc.h
 *
 * Revision 1.3  2002/05/24 09:44:26  joergr
 * Renamed some parameters/variables to avoid ambiguities.
 *
 * Revision 1.2  2002/04/15 09:40:47  joergr
 * Removed "include <sys/types.h>" from implementation file.
 *
 * Revision 1.1  2002/04/11 12:14:33  joergr
 * Introduced new standard classes providing date and time functions.
 *
 *
 */
