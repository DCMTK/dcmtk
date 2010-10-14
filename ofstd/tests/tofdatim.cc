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
 *  Purpose: test program for classes OFDate, OFTime and OFDateTime
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:15 $
 *  CVS/RCS Revision: $Revision: 1.11 $
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


void outputTestResult(const int number,
                      const OFBool result)
{
    COUT << "test #" << number << " ";
    if (result)
        COUT << "passed";
    else
        COUT << "failed!";
    COUT << OFendl;
}


int main()
{
    OFDate date1, date2;
    OFTime time1, time2;
    OFDateTime dateTime1, dateTime2;
    OFString tmpString, tmpString2;

    /* test OFDate */
    outputTestResult(1, !date1.isValid());
    outputTestResult(2, !date1.setDate(2000, 13, 1));
    outputTestResult(3, date1.setDate(2000, 12, 31));
    outputTestResult(4, date1.getYear() == 2000);
    outputTestResult(5, date1.getMonth() == 12);
    outputTestResult(6, date1.getDay() == 31);
    outputTestResult(7, date1 == OFDate(2000, 12, 31));
    date2 = date1;
    outputTestResult(8, date1 == date2);
    outputTestResult(9, !(date1 != date2));
    date2.clear();
    outputTestResult(10, !date2.isValid());
    outputTestResult(11, date1 < OFDate(2001, 1, 1));

    /* test OFTime */
    outputTestResult(12, time1.isValid());
    outputTestResult(13, !time1.setTime(24, 0, 0));
    outputTestResult(14, time1.setTime(12, 15, 30));
    outputTestResult(15, time1.getHour() == 12);
    outputTestResult(16, time1.getMinute() == 15);
    outputTestResult(17, time1.getSecond() == 30);
    outputTestResult(18, time1 == OFTime(12, 15, 30));
    time2 = time1;
    outputTestResult(19, time1 == time2);
    outputTestResult(20, !(time1 != time2));
    time2.clear();
    outputTestResult(21, time2.isValid());
    time1.setTime(12, 15, 30, -1);
    outputTestResult(22, time1 > OFTime(10, 0, 0, -1));
    time1.setTimeZone(2);
    outputTestResult(23, time1 < OFTime(10, 0, 0, -1));
    outputTestResult(24, time1 == OFTime(9, 15, 30, -1));
    outputTestResult(25, time1.getTimeZone() == 2);
    time2.setTime(12, 15, 00);
    outputTestResult(26, time2.getTimeInHours() == 12.25);
    outputTestResult(27, time1.getCoordinatedUniversalTime() == OFTime(10, 15, 30));
    time2.setSecond(30.1234);
    outputTestResult(28, time2.getSecond() == 30.1234);
    outputTestResult(29, time2.getIntSecond() == 30);

    /* test OFDateTime */
    outputTestResult(30, !dateTime1.isValid());
    dateTime1.setDateTime(date1, time1);
    outputTestResult(31, dateTime1.isValid());
    outputTestResult(32, !dateTime1.setDateTime(2000, 13, 1, 24, 0, 0));
    outputTestResult(33, !dateTime1.setDateTime(date2, time2));
    dateTime1.setDateTime(date1, time1);
    outputTestResult(34, dateTime1.getDate() == date1);
    outputTestResult(35, !(dateTime1.getTime() != time1));
    dateTime2 = dateTime1;
    outputTestResult(36, dateTime1 == dateTime2);
    /* "overflow" from one day to another is currently not handled by OFDateTime */
    outputTestResult(37, dateTime1 != OFDateTime(2001, 1, 1, 0, 15, 30, 12) /* should be equal */);
    dateTime1.getISOFormattedDateTime(tmpString);
    outputTestResult(38, tmpString == "2000-12-31 12:15:30");
    dateTime1.getISOFormattedDateTime(tmpString, OFTrue /*showSeconds*/, OFTrue /*showFraction*/, OFTrue /*showTimeZone*/, OFFalse /*showDelimiter*/, "" /*dateTimeSeparator*/);
    outputTestResult(39, tmpString == "20001231121530.000000+0200");
    dateTime2.setISOFormattedDateTime("2000-12-31 10:15:30" /*timeZone: 0*/);
    outputTestResult(40, dateTime1 == dateTime2);

    /* output current date */
    COUT << OFendl;
    date1.setCurrentDate();
    COUT << "current date (default): " << date1 << OFendl;
    date1.getISOFormattedDate(tmpString, OFFalse /*delimiter*/);
    COUT << "current date (YYYYMMDD): " << tmpString << OFendl;
    COUT << "current year: " << date1.getYear() << OFendl;
    COUT << "current month: " << date1.getMonth() << OFendl;
    COUT << "current day: " << date1.getDay() << OFendl;
    COUT << OFendl;

    /* output current time */
    time1.setCurrentTime();
    COUT << "current time (default): " << time1 << OFendl;
    time1.getISOFormattedTime(tmpString, OFFalse /*seconds*/);
    COUT << "current time (HH:MM): " << tmpString << OFendl;
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFFalse /*fraction*/);
    COUT << "current time (HH:MM:SS): " << tmpString << OFendl;
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/, OFTrue /*delimiter*/);
    COUT << "current time (HH:MM:SS&ZZ:ZZ): " << tmpString << OFendl;
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFTrue /*fraction*/, OFFalse /*timeZone*/, OFTrue /*delimiter*/);
    COUT << "current time (HH:MM:SS.FFFFFF): " << tmpString << OFendl;
    time1.getISOFormattedTime(tmpString, OFTrue /*seconds*/, OFTrue /*fraction*/, OFTrue /*timeZone*/, OFFalse /*delimiter*/);
    COUT << "current time (HHMMSS.FFFFFF&ZZZZ): " << tmpString << OFendl;
    COUT << "current time in UTC: " << time1.getCoordinatedUniversalTime() << OFendl;
    COUT << "current hour: " << time1.getHour() << OFendl;
    COUT << "current minute: " << time1.getMinute() << OFendl;
    COUT << "current second: " << time1.getSecond() << OFendl;
    COUT << "current int second: " << time1.getIntSecond() << OFendl;
    COUT << "current milli second: " << time1.getMilliSecond() << OFendl;
    COUT << "current micro second: " << time1.getMicroSecond() << OFendl;
    COUT << "current time zone: " << time1.getTimeZone() << OFendl;
    COUT << "current time in hours: " << time1.getTimeInHours() << OFendl;
    COUT << "current time in seconds: " << time1.getTimeInSeconds() << OFendl;
    COUT << OFendl;

    /* output current date/time */
    dateTime1.setDateTime(date1, time1);
    COUT << "current date/time (default): " << dateTime1 << OFendl;
    dateTime1.getISOFormattedDateTime(tmpString, OFTrue /*showSeconds*/, OFFalse /*showFraction*/, OFTrue /*showTimeZone*/);
    COUT << "current date/time (YYYY-MM-DD HH:MM:SS&ZZ:ZZ): " << tmpString << OFendl;
    dateTime1.getISOFormattedDateTime(tmpString, OFTrue /*showSeconds*/, OFTrue /*showFraction*/, OFTrue /*showTimeZone*/, OFFalse /*showDelimiter*/, "" /*dateTimeSeparator*/);
    COUT << "current date/time (YYYYMMDDHHMMSS.FFFFFF&ZZZZ): " << tmpString << OFendl;

    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: tofdatim.cc,v $
 * Revision 1.11  2010-10-14 13:15:15  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.10  2008-05-21 16:32:42  joergr
 * Reimplemented tests for OFDate, OFTime and OFDateTime classes.
 *
 * Revision 1.9  2006/08/14 16:42:48  meichel
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
