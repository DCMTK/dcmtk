/*
 *
 *  Copyright (C) 2002-2004, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: test program for classes DcmDate, DcmTime and DcmDateTime
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2004-02-04 16:53:23 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrda.h"
#include "dcvrtm.h"
#include "dcvrdt.h"
#include "dcdeftag.h"
#include "ofconsol.h"


int main()
{
    double timeZone;
    OFDate dateVal;
    OFTime timeVal;
    OFDateTime dateTime;
    OFString string;
    DcmDate dcmDate(DCM_StudyDate);
    DcmTime dcmTime(DCM_StudyTime);
    DcmDateTime dcmDateTime(DCM_DateTime);

    dcmDate.setCurrentDate();
    dcmDate.print(COUT);
    if (dcmDate.getOFDate(dateVal).good())
        COUT << "current date: " << dateVal << endl;
    else
        COUT << "current date: <invalid>" << endl;

    dcmTime.setCurrentTime();
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(timeVal).good())
        COUT << "current time: " << timeVal << endl;
    else
        COUT << "current time: <invalid>" << endl;

    dcmTime.putString("12");
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(timeVal).good())
        COUT << "valid time: " << timeVal << endl;
    dcmTime.putString("1203");
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(timeVal).good())
        COUT << "valid time: " << timeVal << endl;
    dcmTime.putString("120315");
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(timeVal).good())
        COUT << "valid time: " << timeVal << endl;
    dcmTime.putString("120301.99");
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(timeVal).good())
    {
        timeVal.getISOFormattedTime(string, OFTrue /*seconds*/, OFTrue /*fraction*/, OFTrue /*timeZone*/);
        COUT << "valid local time: " << string << endl;
    }

    dcmTime.putString("12:03");
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(timeVal).good())
        COUT << "valid time: " << timeVal << endl;
    dcmTime.putString("12:03:15");
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(timeVal).good())
        COUT << "valid time: " << timeVal << endl;

    if (DcmTime::getTimeZoneFromString("+1130", timeZone).good())
        COUT << "time zone: " << timeZone << endl;
    if (DcmTime::getTimeZoneFromString("-0100", timeZone).good())
        COUT << "time zone: " << timeZone << endl;

    dcmDateTime.putString("200204101203+0500");
    dcmDateTime.print(COUT);
    if (dcmDateTime.getOFDateTime(dateTime).good())
        COUT << "valid date/time: " << dateTime << endl;
    dcmDateTime.setCurrentDateTime(OFTrue /*seconds*/, OFTrue /*fraction*/, OFTrue /*timeZone*/);
    dcmDateTime.print(COUT);
    if (dcmDateTime.getOFDateTime(dateTime).good())
        COUT << "current date/time: " << dateTime << endl;
    if (dateTime.getISOFormattedDateTime(string, OFTrue /*seconds*/, OFTrue /*fraction*/, OFTrue /*timeZone*/, OFFalse /*delimiter*/))
        COUT << "current date/time: " << string << endl;

    dcmDateTime.putString("20020410");
    dcmDateTime.print(COUT);
    if (dcmDateTime.getOFDateTime(dateTime).good())
    {
        dateTime.getISOFormattedDateTime(string, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/);
        COUT << "valid local date/time: " << string << endl;
    }

    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: tvrdatim.cc,v $
 * Revision 1.5  2004-02-04 16:53:23  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.4  2002/07/16 14:22:24  joergr
 * Added test case.
 *
 * Revision 1.3  2002/05/24 09:50:13  joergr
 * Renamed some parameters/variables to avoid ambiguities.
 *
 * Revision 1.2  2002/04/16 13:46:10  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 *
 */
