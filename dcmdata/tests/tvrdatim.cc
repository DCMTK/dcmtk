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
 *  Module:  dcmdata
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: test program for classes DcmDate, DcmTime and DcmDateTime
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:07 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/dcmdata/dcvrdt.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/ofstd/ofconsol.h"


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
        COUT << "current date: " << dateVal << OFendl;
    else
        COUT << "current date: <invalid>" << OFendl;

    dcmTime.setCurrentTime();
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(timeVal).good())
        COUT << "current time: " << timeVal << OFendl;
    else
        COUT << "current time: <invalid>" << OFendl;

    dcmTime.putString("12");
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(timeVal).good())
        COUT << "valid time: " << timeVal << OFendl;
    dcmTime.putString("1203");
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(timeVal).good())
        COUT << "valid time: " << timeVal << OFendl;
    dcmTime.putString("120315");
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(timeVal).good())
        COUT << "valid time: " << timeVal << OFendl;
    dcmTime.putString("120301.99");
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(timeVal).good())
    {
        timeVal.getISOFormattedTime(string, OFTrue /*seconds*/, OFTrue /*fraction*/, OFTrue /*timeZone*/);
        COUT << "valid local time: " << string << OFendl;
    }

    dcmTime.putString("12:03");
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(timeVal).good())
        COUT << "valid time: " << timeVal << OFendl;
    dcmTime.putString("12:03:15");
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(timeVal).good())
        COUT << "valid time: " << timeVal << OFendl;

    if (DcmTime::getTimeZoneFromString("+1130", timeZone).good())
        COUT << "time zone: " << timeZone << OFendl;
    if (DcmTime::getTimeZoneFromString("-0100", timeZone).good())
        COUT << "time zone: " << timeZone << OFendl;

    dcmDateTime.putString("200204101203+0500");
    dcmDateTime.print(COUT);
    if (dcmDateTime.getOFDateTime(dateTime).good())
        COUT << "valid date/time: " << dateTime << OFendl;
    dcmDateTime.setCurrentDateTime(OFTrue /*seconds*/, OFTrue /*fraction*/, OFTrue /*timeZone*/);
    dcmDateTime.print(COUT);
    if (dcmDateTime.getOFDateTime(dateTime).good())
        COUT << "current date/time: " << dateTime << OFendl;
    if (dateTime.getISOFormattedDateTime(string, OFTrue /*seconds*/, OFTrue /*fraction*/, OFTrue /*timeZone*/, OFFalse /*delimiter*/))
        COUT << "current date/time: " << string << OFendl;

    dcmDateTime.putString("20020410");
    dcmDateTime.print(COUT);
    if (dcmDateTime.getOFDateTime(dateTime).good())
    {
        dateTime.getISOFormattedDateTime(string, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/);
        COUT << "valid local date/time: " << string << OFendl;
    }

    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: tvrdatim.cc,v $
 * Revision 1.8  2010-10-14 13:15:07  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.7  2006/08/15 15:50:10  meichel
 * Updated all code in module dcmdata to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.6  2005/12/08 15:42:14  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.5  2004/02/04 16:53:23  joergr
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
