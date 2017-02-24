/*
 *
 *  Copyright (C) 2002-2015, OFFIS e.V.
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
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofdate.h"
#include "dcmtk/ofstd/oftime.h"
#include "dcmtk/ofstd/ofdatime.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"


OFTEST(ofstd_OFDate)
{
    OFDate date1, date2;

    /* test OFDate */
    OFCHECK(!date1.isValid());
    OFCHECK(!date1.setDate(2000, 13, 1));
    OFCHECK(date1.setDate(2000, 12, 31));
    OFCHECK_EQUAL(date1.getYear(), 2000);
    OFCHECK_EQUAL(date1.getMonth(), 12);
    OFCHECK_EQUAL(date1.getDay(), 31);
    OFCHECK_EQUAL(date1, OFDate(2000, 12, 31));
    date2 = date1;
    OFCHECK_EQUAL(date1, date2);
    OFCHECK(!(date1 != date2));
    date2.clear();
    OFCHECK(!date2.isValid());
    OFCHECK(date1 < OFDate(2001, 1, 1));
    OFCHECK(date1.setISOFormattedDate("20001231"));
    OFCHECK(date2.setISOFormattedDate("2000-12-31"));
    OFCHECK_EQUAL(date1, date2);
    OFCHECK(date2.setISOFormattedDate("2000.12.31"));
}


OFTEST(ofstd_OFTime)
{
    OFTime time1, time2;

    /* test OFTime */
    OFCHECK(time1.isValid());
    OFCHECK(!time1.setTime(24, 0, 0));
    OFCHECK(time1.setTime(12, 15, 30));
    OFCHECK_EQUAL(time1.getHour(), 12);
    OFCHECK_EQUAL(time1.getMinute(), 15);
    OFCHECK_EQUAL(time1.getSecond(), 30);
    OFCHECK_EQUAL(time1, OFTime(12, 15, 30));
    time2 = time1;
    OFCHECK_EQUAL(time1, time2);
    OFCHECK(!(time1 != time2));
    time2.clear();
    OFCHECK(time2.isValid());
    OFCHECK(time1.setTime(12, 15, 30, -1));
    OFCHECK(time1 > OFTime(10, 0, 0, -1));
    OFCHECK(time1.setTimeZone(2));
    OFCHECK(!time1.setTimeZone(-20));
    OFCHECK(time1 < OFTime(10, 0, 0, -1));
    OFCHECK_EQUAL(time1, OFTime(9, 15, 30, -1));
    OFCHECK_EQUAL(time1.getTimeZone(), 2);
    OFCHECK(time2.setTime(12, 15, 00));
    OFCHECK_EQUAL(time2.getTimeInHours(), 12.25);
    OFCHECK_EQUAL(time1.getCoordinatedUniversalTime(), OFTime(10, 15, 30));
    OFCHECK(time2.setSecond(30.1234));
    OFCHECK_EQUAL(time2.getSecond(), 30.1234);
    OFCHECK_EQUAL(time2.getIntSecond(), 30);
    OFCHECK(time1.setISOFormattedTime("1215"));
    OFCHECK(time1.setISOFormattedTime("12:15"));
    OFCHECK(time1.setISOFormattedTime("121530"));
    OFCHECK(time1.setISOFormattedTime("12:15:30"));
    OFCHECK(time1.setISOFormattedTime("121530+0100"));
    OFCHECK_EQUAL(time1.getTimeZone(), 1.0);
    OFCHECK(time1.setISOFormattedTime("12:15:30 -02:30"));
    OFCHECK_EQUAL(time1.getTimeZone(), -2.5);
    /* the "seconds" part is mandatory if time zone is present */
    OFCHECK(!time2.setISOFormattedTime("10:15 -02:30"));
    OFCHECK(!time2.setISOFormattedTime("1015+0100"));
    /* check setting normalized time values */
    OFCHECK(time1.setTimeInSeconds(99999, 0, OFTrue /*normalized*/));
    OFCHECK_EQUAL(time1.getTimeInSeconds(OFTrue /*useTimeZone*/, OFFalse /*normalize*/), 13599);
    OFCHECK(time1.setTimeInHours(99, 0, OFTrue /*normalized*/));
    OFCHECK_EQUAL(time1.getTimeInHours(OFTrue /*useTimeZone*/, OFFalse /*normalize*/), 3);
}


OFTEST(ofstd_OFDateTime)
{
    const OFDate date1(2000, 12, 31);
    const OFDate date2;
    const OFTime time1(12, 15, 30, 2);
    const OFTime time2(12, 15, 30.1234);
    OFDateTime dateTime1, dateTime2;
    OFString tmpString;

    /* test OFDateTime */
    OFCHECK(!dateTime1.isValid());
    OFCHECK(dateTime1.setDateTime(date1, time1));
    OFCHECK(dateTime1.isValid());
    OFCHECK(!dateTime1.setDateTime(2000, 13, 1, 24, 0, 0));
    OFCHECK(!dateTime1.setDateTime(date2, time2));
    OFCHECK(dateTime1.setDateTime(date1, time1));
    OFCHECK_EQUAL(dateTime1.getDate(), date1);
    OFCHECK(!(dateTime1.getTime() != time1));
    dateTime2 = dateTime1;
    OFCHECK_EQUAL(dateTime1, dateTime2);
    /* "overflow" from one day to another is currently not handled by OFDateTime */
    OFCHECK(dateTime1 != OFDateTime(2001, 1, 1, 0, 15, 30, 12) /* should be equal */);
    OFCHECK(dateTime1 < OFDateTime(2001, 1, 1, 0, 15, 30, 2) /* should be less */);
    OFCHECK(dateTime1 <= OFDateTime(2001, 1, 1, 0, 15, 30, 12) /* should be less or equal */);
    OFCHECK(OFDateTime(2000, 12, 31, 12, 15, 30, -.5) > dateTime1 /* should be greater */);
    OFCHECK(OFDateTime(2000, 12, 31, 12, 15, 30, -.5) >= dateTime1 /* should be greater or equal */);
    OFCHECK(dateTime1 >= dateTime1 /* should be greater or equal */);
    OFCHECK(dateTime1.getISOFormattedDateTime(tmpString));
    OFCHECK_EQUAL(tmpString, "2000-12-31 12:15:30");
    OFCHECK(dateTime1.getISOFormattedDateTime(tmpString, OFTrue /*showSeconds*/, OFTrue /*showFraction*/,
        OFTrue /*showTimeZone*/, OFFalse /*showDelimiter*/, "" /*dateTimeSeparator*/, "" /*timeZoneSeparator*/));
    OFCHECK_EQUAL(tmpString, "20001231121530.000000+0200");
    OFCHECK(dateTime2.setISOFormattedDateTime("2000-12-31 10:15:30" /*timeZone: 0*/));
    OFCHECK_EQUAL(dateTime1, dateTime2);
    OFCHECK(dateTime2.setISOFormattedDateTime("2000.12.31  10-15-30" /*timeZone: 0*/));
    OFCHECK_EQUAL(dateTime1, dateTime2);
    OFCHECK(dateTime2.setISOFormattedDateTime("2000-12-31 10:15:30 +01:00"));
    OFCHECK(dateTime2.setISOFormattedDateTime("2000-12-31 10:15:30 -02:30"));
    OFCHECK(dateTime1.setISOFormattedDateTime("2000-12-31T10:15:30+01:00"));
    OFCHECK(dateTime2.setISOFormattedDateTime("20001231101530+0100"));
    OFCHECK_EQUAL(dateTime1, dateTime2);
    /* the "seconds" part is mandatory if time zone is present */
    OFCHECK(!dateTime2.setISOFormattedDateTime("2000-12-31 10:15 -02:30"));
    OFCHECK(!dateTime2.setISOFormattedDateTime("200012311015+0100"));
}


// These tests check that getting the current date and time works.
// The results for this cannot(?) be verified automatically.
#if 0
    OFDate date1;
    OFTime time1;
    OFDateTime dateTime1;
    OFString tmpString;

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
#endif
