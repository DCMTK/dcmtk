/*
 *
 *  Copyright (C) 2002-2006, OFFIS
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
 *  Purpose: test program for classes OFDate, OFTime and OFDateTime
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2006-08-14 16:42:48 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofdate.h"
#include "dcmtk/ofstd/oftime.h"
#include "dcmtk/ofstd/ofdatime.h"
#include "dcmtk/ofstd/ofconsol.h"


int main()
{
    OFDate date1, date2;
    OFTime time1, time2;
    OFDateTime dateTime1, dateTime2;
    OFString tmpString, tmpString2;

    COUT << "init date: " << date1 << OFendl;
    date1.setCurrentDate();
    COUT << "current date: " << date1 << OFendl;
    date1.setDate(1972, 5, 17);
    COUT << "birth date: " << date1 << OFendl;
    if (!date1.setDate(2000, 13, 1))
        COUT << "invalid date" << OFendl;
    else
        COUT << "valid date:" << date1 << OFendl;
    date1.setCurrentDate();
    date1.getISOFormattedDate(tmpString, OFTrue /*delimiter*/);
    COUT << "current date: " << tmpString << OFendl;
    date2.setISOFormattedDate(tmpString);
    COUT << "same date: " << date2 << OFendl;

    COUT << "init time: " << time1 << OFendl;
    time1.setCurrentTime();
    COUT << "current time: " << time1 << OFendl;
    time1.setTime(0, 50, 01);
    COUT << "birth date: " << time1 << OFendl;
    if (!time1.setTime(24, 00, 00))
        COUT << "invalid time" << OFendl;
    else
        COUT << "valid time:" << time1 << OFendl;
    time1.setTime(0, 50, 01.99);
    time1.getISOFormattedTime(tmpString, OFFalse /*seconds*/);
    COUT << "current time: " << tmpString << OFendl;
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFFalse /*fraction*/);
    COUT << "current time: " << tmpString << OFendl;
    time2.setISOFormattedTime(tmpString);
    COUT << "same time: " << time2 << OFendl;
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFTrue /*fraction*/, OFFalse /*timeZone*/, OFFalse /*delimiter*/);
    COUT << "current time: " << tmpString << OFendl;

    time1.setTime(10,  0,  0, -5);
    time2.setTime(23, 59, 59, -5);
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/);
    time2.getISOFormattedTime(tmpString2, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/);
    COUT << "compare time 1: " << tmpString << " < " << tmpString2 << " : " << ((time1 < time2) ? "true" : "false") << OFendl;
    COUT << "compare time 2: " << tmpString << " <= " << tmpString2 << " : " << ((time1 <= time2) ? "true" : "false") << OFendl;
    COUT << "compare time 3: " << tmpString << " >= " << tmpString2 << " : " << ((time1 >= time2) ? "true" : "false") << OFendl;
    COUT << "compare time 4: " << tmpString << " > " << tmpString2 << " : " << ((time1 > time2) ? "true" : "false") << OFendl;
    time2.setTime(11, 0, 0, -4);
    time2.getISOFormattedTime(tmpString2, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/);
    COUT << "compare time 5: " << tmpString << " == " << tmpString2 << " : " << ((time1 == time2) ? "true" : "false") << OFendl;

    date2 = date1;
    date2.setDay(date2.getDay() + 1);
    COUT << "compare date: " << date1 << " > " << date2 << " = " << ((date1 > date2) ? "true" : "false") << OFendl;
    time2 = time1;
    time2.setMinute(time2.getMinute() + 1);
    COUT << "compare time: " << time1 << " < " << time2 << " = " << ((time1 < time2) ? "true" : "false") << OFendl;

    COUT << "current date/time: " << OFDate::getCurrentDate() << " " << OFTime::getCurrentTime() << OFendl;

    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFTrue /*fraction*/, OFTrue /*timeZone*/, OFFalse /*delimiter*/);
    COUT << "with time zone: " << tmpString << OFendl;
    time1.setCurrentTime();
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/, OFTrue /*delimiter*/);
    COUT << "current time: " << tmpString << OFendl;
    time1.setTimeZone(-11, 30);
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/, OFTrue /*delimiter*/);
    COUT << "new time zone: " << tmpString << OFendl;
    COUT << "UTC time: " << time1.getCoordinatedUniversalTime() << OFendl;
    COUT << "local time: " << time1.getLocalTime() << OFendl;
    time1.setTimeZone(OFTime::getLocalTimeZone());
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/, OFTrue /*delimiter*/);
    COUT << "new time zone: " << tmpString << OFendl;
    COUT << "UTC time: " << time1.getCoordinatedUniversalTime() << OFendl;
    COUT << "local time: " << time1.getLocalTime() << OFendl;

    COUT << "current date/time with time zone: " << OFDate::getCurrentDate() << " " << OFTime::getCurrentTime().getCoordinatedUniversalTime() << OFendl;

    time1.setTimeInSeconds(12 * 3600 + 15 * 60 + 45.99);
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFTrue /*fraction*/);
    COUT << "time in seconds: " << tmpString << OFendl;
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFTrue /*fraction*/);
    time1.setTimeInHours(12 + 15 / 60 + 45.99 / 3600);
    COUT << "time in hours: " << tmpString << OFendl;

    dateTime1.setCurrentDateTime();
    COUT << "current date/time (OFDateTime): " << dateTime1 << OFendl;
    COUT << "current hour: " << dateTime1.getTime().getHour() << OFendl;
    COUT << "current minute: " << dateTime1.getTime().getMinute() << OFendl;
    COUT << "current second: " << dateTime1.getTime().getSecond() << OFendl;
    COUT << "current int second: " << dateTime1.getTime().getIntSecond() << OFendl;
    COUT << "current milli second: " << dateTime1.getTime().getMilliSecond() << OFendl;
    COUT << "current micro second: " << dateTime1.getTime().getMicroSecond() << OFendl;
    dateTime1.setDateTime(1999, 12, 31, 23, 59, 59, -1);
    dateTime2.setDateTime(2000,  1,  1,  1, 59, 59,  1);
    dateTime1.getISOFormattedDateTime(tmpString, OFTrue /*showSeconds*/, OFFalse /*showFraction*/, OFTrue /*showTimeZone*/);
    dateTime2.getISOFormattedDateTime(tmpString2, OFTrue /*showSeconds*/, OFFalse /*showFraction*/, OFTrue /*showTimeZone*/);
    COUT << "compare date/time: " << tmpString << " == " << tmpString2 << " : " << ((dateTime1 == dateTime2) ? "true" : "false") << OFendl;
    COUT << "compare date/time: " << tmpString << " != " << tmpString2 << " : " << ((dateTime1 != dateTime2) ? "true" : "false") << OFendl;

    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: tofdatim.cc,v $
 * Revision 1.9  2006-08-14 16:42:48  meichel
 * Updated all code in module ofstd to correctly compile if the standard
 *   namespace has not included into the global one with a "using" directive.
 *
 * Revision 1.8  2005/12/08 15:49:05  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.7  2004/01/16 10:37:09  joergr
 * Added setISOFormattedXXX() methods for Date, Time and DateTime.
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.6  2003/12/17 15:24:57  joergr
 * Added test cases for comparing both time and date/time values.
 *
 * Revision 1.5  2003/09/17 17:01:44  joergr
 * Renamed variable "string" to avoid name clash with STL class.
 *
 * Revision 1.4  2002/05/24 09:45:13  joergr
 * Renamed some parameters/variables to avoid ambiguities.
 *
 * Revision 1.3  2002/04/19 10:42:58  joergr
 * Added new helper routines to get the milli and micro seconds part as well as
 * the integral value of seconds.
 *
 * Revision 1.2  2002/04/16 13:37:00  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 *
 */
