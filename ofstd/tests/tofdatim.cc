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
 *  Purpose: test program for classes OFDate, OFTime and OFDateTime
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-05-24 09:45:13 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/tests/tofdatim.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
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
    OFDateTime dateTime;
    OFString string;

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
    date1.getISOFormattedDate(string, OFFalse /*delimiter*/);
    COUT << "current date: " << string << endl;
    
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
    time1.getISOFormattedTime(string, OFFalse /*seconds*/);
    COUT << "current time: " << string << endl;
    time1.getISOFormattedTime(string, OFTrue /*seconds*/, OFTrue /*fraction*/);
    COUT << "current time: " << string << endl;
    time1.getISOFormattedTime(string, OFTrue /*seconds*/, OFTrue /*fraction*/, OFFalse /*timeZone*/, OFFalse /*delimiter*/);
    COUT << "current time: " << string << endl;

    date2 = date1;
    date2.setDay(date2.getDay() + 1);
    COUT << "compare date: " << date1 << " > " << date2 << " = " << (date1 > date2) << endl;
    time2 = time1;
    time2.setMinute(time2.getMinute() + 1);
    COUT << "compare time: " << time1 << " < " << time2 << " = " << (time1 < time2) << endl;
    
    COUT << "current date/time: " << OFDate::getCurrentDate() << " " << OFTime::getCurrentTime() << endl;
    
    time1.getISOFormattedTime(string, OFTrue /*seconds*/, OFTrue /*fraction*/, OFTrue /*timeZone*/, OFFalse /*delimiter*/);
    COUT << "time zone: " << string << endl;    
    time1.setCurrentTime();
    time1.getISOFormattedTime(string, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/, OFTrue /*delimiter*/);
    COUT << "current time: " << string << endl;    
    time1.setTimeZone(-11, 30);
    time1.getISOFormattedTime(string, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/, OFTrue /*delimiter*/);
    COUT << "new time zone: " << string << endl;    
    COUT << "UTC time: " << time1.getCoordinatedUniversalTime() << endl;    
    COUT << "local time: " << time1.getLocalTime() << endl;    
    time1.setTimeZone(OFTime::getLocalTimeZone());
    time1.getISOFormattedTime(string, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/, OFTrue /*delimiter*/);
    COUT << "new time zone: " << string << endl;    
    COUT << "UTC time: " << time1.getCoordinatedUniversalTime() << endl;    
    COUT << "local time: " << time1.getLocalTime() << endl;    
    
    COUT << "current date/time with time zone: " << OFDate::getCurrentDate() << " " << OFTime::getCurrentTime().getCoordinatedUniversalTime() << endl;
    
    time1.setTimeInSeconds(12 * 3600 + 15 * 60 + 45.99);
    time1.getISOFormattedTime(string, OFTrue /*seconds*/, OFTrue /*fraction*/);
    COUT << "time in seconds: " << string << endl;    
    time1.getISOFormattedTime(string, OFTrue /*seconds*/, OFTrue /*fraction*/);
    time1.setTimeInHours(12 + 15 / 60 + 45.99 / 3600);
    COUT << "time in hours: " << string << endl;    

    dateTime.setCurrentDateTime();
    COUT << "current date/time (OFDateTime): " << dateTime << endl;
    COUT << "current hour: " << dateTime.getTime().getHour() << endl;
    COUT << "current minute: " << dateTime.getTime().getMinute() << endl;
    COUT << "current second: " << dateTime.getTime().getSecond() << endl;
    COUT << "current int second: " << dateTime.getTime().getIntSecond() << endl;
    COUT << "current milli second: " << dateTime.getTime().getMilliSecond() << endl;
    COUT << "current micro second: " << dateTime.getTime().getMicroSecond() << endl;

    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: tofdatim.cc,v $
 * Revision 1.4  2002-05-24 09:45:13  joergr
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
