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
 *  Purpose: Class for date and time functions (Source)
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-10-20 07:41:37 $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
    OFBool result = OFFalse;
    const size_t length = formattedDateTime.length();
    /* check for supported formats: YYYYMMDDHHMM[SS] */
    if ((length == 12) || (length == 14))
    {
        if (Date.setISOFormattedDate(formattedDateTime.substr(0, 8)) && Time.setISOFormattedTime(formattedDateTime.substr(8)))
            result = OFTrue;
    }
    /* YYYY-MM-DD HH:MM[:SS] */
    else if (length >= 16)
    {
        if (Date.setISOFormattedDate(formattedDateTime.substr(0, 10)))
        {
            size_t pos = 10;
            /* search for first digit of the time value (skip arbitrary separators) */
            while ((pos < length) && !isdigit(OFstatic_cast(unsigned char, formattedDateTime.at(pos))))
                ++pos;
            if ((pos < length) && Time.setISOFormattedTime(formattedDateTime.substr(pos)))
                result = OFTrue;
        }
    }
    return result;
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
                                           const OFBool showDelimiter) const
{
    /* call the real function, required to make Sun CC 2.0.1 happy (see header file) */
    return getISOFormattedDateTime(formattedDateTime, showSeconds, showFraction, showTimeZone, showDelimiter, " " /*dateTimeSeparator*/);
}


OFBool OFDateTime::getISOFormattedDateTime(OFString &formattedDateTime,
                                           const OFBool showSeconds,
                                           const OFBool showFraction,
                                           const OFBool showTimeZone,
                                           const OFBool showDelimiter,
                                           const OFString &dateTimeSeparator) const
{
    /* get formatted date first component */
    OFBool result = Date.getISOFormattedDate(formattedDateTime, showDelimiter);
    if (result)
    {
        /* ... then get the formatted time component */
        OFString timeString;
        if (Time.getISOFormattedTime(timeString, showSeconds, showFraction, showTimeZone, showDelimiter))
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


/*
 *
 * CVS/RCS Log:
 * $Log: ofdatime.cc,v $
 * Revision 1.14  2010-10-20 07:41:37  uli
 * Made sure isalpha() & friends are only called with valid arguments.
 *
 * Revision 1.13  2010-10-14 13:14:53  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.12  2009-08-19 11:55:45  meichel
 * Added additional includes needed for Sun Studio 11 on Solaris.
 *
 * Revision 1.11  2008-05-21 16:29:45  joergr
 * Added new constructor and new setDateTime() method.
 *
 * Revision 1.10  2006/12/12 11:56:05  joergr
 * Fixed wrong default value for the "dateTimeSeparator" parameter.
 *
 * Revision 1.9  2006/08/14 16:42:46  meichel
 * Updated all code in module ofstd to correctly compile if the standard
 *   namespace has not included into the global one with a "using" directive.
 *
 * Revision 1.8  2005/12/08 15:48:56  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.7  2004/04/16 12:44:20  joergr
 * Restructured code to avoid default parameter values for "complex types" like
 * OFString. Required for Sun CC 2.0.1.
 *
 * Revision 1.6  2004/01/16 10:35:18  joergr
 * Added setISOFormattedXXX() methods for Date, Time and DateTime.
 *
 * Revision 1.5  2003/12/17 15:27:21  joergr
 * Added note to the comparison operators that the "day overflow" is not yet
 * handled correctly.
 *
 * Revision 1.4  2003/09/15 12:15:07  joergr
 * Made comparison operators const.
 *
 * Revision 1.3  2002/11/27 15:09:39  meichel
 * Adapted module ofstd to use of new header file ofstdinc.h
 *
 * Revision 1.2  2002/05/24 09:44:26  joergr
 * Renamed some parameters/variables to avoid ambiguities.
 *
 * Revision 1.1  2002/04/11 12:14:33  joergr
 * Introduced new standard classes providing date and time functions.
 *
 *
 */
