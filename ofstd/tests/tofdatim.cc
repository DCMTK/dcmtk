/*
 *
 *  Copyright (C) 2002-2003, OFFIS
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2003-12-17 15:24:57 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofdate.h"
#include "oftime.h"
#include "ofdatime.h"
#include "ofconsol.h"


int main()
{
    OFDate date1, date2;
    OFTime time1, time2;
    OFDateTime dateTime1, dateTime2;
    OFString tmpString, tmpString2;

    COUT << "init date: " << date1 << endl;
    date1.setCurrentDate();
    COUT << "current date: " << date1 << endl;
    date1.setDate(1972, 5, 17);
    COUT << "birth date: " << date1 << endl;
    if (!date1.setDate(2000, 13, 1))
        COUT << "invalid date" << endl;
    else
        COUT << "valid date:" << date1 << endl;
    date1.setCurrentDate();
    date1.getISOFormattedDate(tmpString, OFFalse /*delimiter*/);
    COUT << "current date: " << tmpString << endl;

    COUT << "init time: " << time1 << endl;
    time1.setCurrentTime();
    COUT << "current time: " << time1 << endl;
    time1.setTime(0, 50, 01);
    COUT << "birth date: " << time1 << endl;
    if (!time1.setTime(24, 00, 00))
        COUT << "invalid time" << endl;
    else
        COUT << "valid time:" << time1 << endl;
    time1.setTime(0, 50, 01.99);
    time1.getISOFormattedTime(tmpString, OFFalse /*seconds*/);
    COUT << "current time: " << tmpString << endl;
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFTrue /*fraction*/);
    COUT << "current time: " << tmpString << endl;
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFTrue /*fraction*/, OFFalse /*timeZone*/, OFFalse /*delimiter*/);
    COUT << "current time: " << tmpString << endl;

    time1.setTime(10,  0,  0, -5);
    time2.setTime(23, 59, 59, -5);
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/);
    time2.getISOFormattedTime(tmpString2, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/);
    COUT << "compare time 1: " << tmpString << " < " << tmpString2 << " : " << ((time1 < time2) ? "true" : "false") << endl;
    COUT << "compare time 2: " << tmpString << " <= " << tmpString2 << " : " << ((time1 <= time2) ? "true" : "false") << endl;
    COUT << "compare time 3: " << tmpString << " >= " << tmpString2 << " : " << ((time1 >= time2) ? "true" : "false") << endl;
    COUT << "compare time 4: " << tmpString << " > " << tmpString2 << " : " << ((time1 > time2) ? "true" : "false") << endl;
    time2.setTime(11, 0, 0, -4);
    time2.getISOFormattedTime(tmpString2, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/);
    COUT << "compare time 5: " << tmpString << " == " << tmpString2 << " : " << ((time1 == time2) ? "true" : "false") << endl;

    date2 = date1;
    date2.setDay(date2.getDay() + 1);
    COUT << "compare date: " << date1 << " > " << date2 << " = " << ((date1 > date2) ? "true" : "false") << endl;
    time2 = time1;
    time2.setMinute(time2.getMinute() + 1);
    COUT << "compare time: " << time1 << " < " << time2 << " = " << ((time1 < time2) ? "true" : "false") << endl;

    COUT << "current date/time: " << OFDate::getCurrentDate() << " " << OFTime::getCurrentTime() << endl;

    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFTrue /*fraction*/, OFTrue /*timeZone*/, OFFalse /*delimiter*/);
    COUT << "with time zone: " << tmpString << endl;
    time1.setCurrentTime();
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/, OFTrue /*delimiter*/);
    COUT << "current time: " << tmpString << endl;
    time1.setTimeZone(-11, 30);
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/, OFTrue /*delimiter*/);
    COUT << "new time zone: " << tmpString << endl;
    COUT << "UTC time: " << time1.getCoordinatedUniversalTime() << endl;
    COUT << "local time: " << time1.getLocalTime() << endl;
    time1.setTimeZone(OFTime::getLocalTimeZone());
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/, OFTrue /*delimiter*/);
    COUT << "new time zone: " << tmpString << endl;
    COUT << "UTC time: " << time1.getCoordinatedUniversalTime() << endl;
    COUT << "local time: " << time1.getLocalTime() << endl;

    COUT << "current date/time with time zone: " << OFDate::getCurrentDate() << " " << OFTime::getCurrentTime().getCoordinatedUniversalTime() << endl;

    time1.setTimeInSeconds(12 * 3600 + 15 * 60 + 45.99);
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFTrue /*fraction*/);
    COUT << "time in seconds: " << tmpString << endl;
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFTrue /*fraction*/);
    time1.setTimeInHours(12 + 15 / 60 + 45.99 / 3600);
    COUT << "time in hours: " << tmpString << endl;

    dateTime1.setCurrentDateTime();
    COUT << "current date/time (OFDateTime): " << dateTime1 << endl;
    COUT << "current hour: " << dateTime1.getTime().getHour() << endl;
    COUT << "current minute: " << dateTime1.getTime().getMinute() << endl;
    COUT << "current second: " << dateTime1.getTime().getSecond() << endl;
    COUT << "current int second: " << dateTime1.getTime().getIntSecond() << endl;
    COUT << "current milli second: " << dateTime1.getTime().getMilliSecond() << endl;
    COUT << "current micro second: " << dateTime1.getTime().getMicroSecond() << endl;
    dateTime1.setDateTime(1999, 12, 31, 23, 59, 59, -1);
    dateTime2.setDateTime(2000,  1,  1,  1, 59, 59,  1);
    dateTime1.getISOFormattedDateTime(tmpString, OFTrue /*showSeconds*/, OFFalse /*showFraction*/, OFTrue /*showTimeZone*/);
    dateTime2.getISOFormattedDateTime(tmpString2, OFTrue /*showSeconds*/, OFFalse /*showFraction*/, OFTrue /*showTimeZone*/);
    COUT << "compare date/time: " << tmpString << " == " << tmpString2 << " : " << ((dateTime1 == dateTime2) ? "true" : "false") << endl;
    COUT << "compare date/time: " << tmpString << " != " << tmpString2 << " : " << ((dateTime1 != dateTime2) ? "true" : "false") << endl;

    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: tofdatim.cc,v $
 * Revision 1.6  2003-12-17 15:24:57  joergr
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
 * Thanks to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
 * contribution.
 *
 *
 */
