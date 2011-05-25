/*
 *
 *  Copyright (C) 2002-2011, OFFIS e.V.
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
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2011-05-25 10:05:56 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/dcmdata/dcvrdt.h"
#include "dcmtk/dcmdata/dcdeftag.h"

#define CHECK_EQUAL(string) do { \
    strstream << OFStringStream_ends; \
    OFSTRINGSTREAM_GETOFSTRING(strstream, res); \
    OFCHECK_EQUAL(res, string); \
    strstream.clear(); \
    strstream.str(""); \
} while (0)
#define CHECK_STREAM_EQUAL(val, string) do { \
    strstream << val; \
    CHECK_EQUAL(string); \
    strstream.clear(); \
    strstream.str(""); \
} while(0)

OFTEST(dcmdata_dateTime)
{
    double timeZone;
    OFDate dateVal;
    OFTime timeVal;
    OFDateTime dateTime;
    OFString string;
    DcmDate dcmDate(DCM_StudyDate);
    DcmTime dcmTime(DCM_StudyTime);
    DcmDateTime dcmDateTime(DCM_DateTime);
    OFOStringStream strstream;

    // Determine the local time zone, needed because setOFTime loses the timezone
    timeVal.setCurrentTime();
    const double localTimeZone = timeVal.getTimeZone();

    OFDate curDateVal(2011, 5, 9);
    OFTime curTimeVal(10, 35, 20, localTimeZone);
    OFDateTime curDateTimeVal(curDateVal, curTimeVal);

    dcmDate.setOFDate(curDateVal);
    dcmDate.print(strstream);
    CHECK_EQUAL("(0008,0020) DA [20110509]                               #   8, 1 StudyDate\n");
    OFCHECK(dcmDate.getOFDate(dateVal).good());
    OFCHECK_EQUAL(dateVal, curDateVal);

    dcmTime.setOFTime(curTimeVal);
    dcmTime.print(strstream);
    CHECK_EQUAL("(0008,0030) TM [103520]                                 #   6, 1 StudyTime\n");
    OFCHECK(dcmTime.getOFTime(timeVal).good());
    OFCHECK_EQUAL(timeVal, curTimeVal);

    dcmDateTime.setOFDateTime(curDateTimeVal);
    dcmDateTime.print(strstream);
    CHECK_EQUAL("(0040,a120) DT [20110509103520]                         #  14, 1 DateTime\n");
    OFCHECK(dcmDateTime.getOFDateTime(dateTime).good());
    OFCHECK_EQUAL(dateTime, curDateTimeVal);
    OFCHECK(dateTime.getISOFormattedDateTime(string, OFTrue /*seconds*/, OFTrue /*fraction*/, OFFalse /*timeZone*/, OFFalse /*delimiter*/));
    OFCHECK_EQUAL(string, "20110509103520.000000");

    dcmTime.putString("12");
    dcmTime.print(strstream);
    CHECK_EQUAL("(0008,0030) TM [12]                                     #   2, 1 StudyTime\n");
    OFCHECK(dcmTime.getOFTime(timeVal).good());
    CHECK_STREAM_EQUAL(timeVal, "12:00:00");

    dcmTime.putString("1203");
    dcmTime.print(strstream);
    CHECK_EQUAL("(0008,0030) TM [1203]                                   #   4, 1 StudyTime\n");
    OFCHECK(dcmTime.getOFTime(timeVal).good());
    CHECK_STREAM_EQUAL(timeVal, "12:03:00");

    dcmTime.putString("120315");
    dcmTime.print(strstream);
    CHECK_EQUAL("(0008,0030) TM [120315]                                 #   6, 1 StudyTime\n");
    OFCHECK(dcmTime.getOFTime(timeVal).good());
    CHECK_STREAM_EQUAL(timeVal, "12:03:15");
    dcmTime.putString("120301.99");
    dcmTime.print(strstream);
    CHECK_EQUAL("(0008,0030) TM [120301.99]                              #  10, 1 StudyTime\n");
    OFCHECK(dcmTime.getOFTime(timeVal).good());
    timeVal.getISOFormattedTime(string, OFTrue /*seconds*/, OFTrue /*fraction*/, OFFalse /*timeZone*/);
    OFCHECK_EQUAL(string, "12:03:01.990000");

    dcmTime.putString("12:03");
    dcmTime.print(strstream);
    CHECK_EQUAL("(0008,0030) TM [12:03]                                  #   6, 1 StudyTime\n");
    OFCHECK(dcmTime.getOFTime(timeVal).good());
    CHECK_STREAM_EQUAL(timeVal, "12:03:00");
    dcmTime.putString("12:03:15");
    dcmTime.print(strstream);
    CHECK_EQUAL("(0008,0030) TM [12:03:15]                               #   8, 1 StudyTime\n");
    OFCHECK(dcmTime.getOFTime(timeVal).good());
    CHECK_STREAM_EQUAL(timeVal, "12:03:15");

    OFCHECK(DcmTime::getTimeZoneFromString("+1130", timeZone).good());
    OFCHECK_EQUAL(timeZone, 11.5);
    OFCHECK(DcmTime::getTimeZoneFromString("-0100", timeZone).good());
    OFCHECK_EQUAL(timeZone, -1);

    dcmDateTime.putString("200204101203+0500");
    dcmDateTime.print(strstream);
    CHECK_EQUAL("(0040,a120) DT [200204101203+0500]                      #  18, 1 DateTime\n");
    OFCHECK(dcmDateTime.getOFDateTime(dateTime).good());
    CHECK_STREAM_EQUAL(dateTime, "2002-04-10 12:03:00");

    dcmDateTime.putString("20020410");
    dcmDateTime.print(strstream);
    CHECK_EQUAL("(0040,a120) DT [20020410]                               #   8, 1 DateTime\n");
    OFCHECK(dcmDateTime.getOFDateTime(dateTime).good());
    dateTime.getISOFormattedDateTime(string, OFTrue /*seconds*/, OFFalse /*fraction*/, OFFalse /*timeZone*/);
    OFCHECK_EQUAL(string, "2002-04-10 00:00:00");
}


/*
 *
 * CVS/RCS Log:
 * $Log: tvrdatim.cc,v $
 * Revision 1.9  2011-05-25 10:05:56  uli
 * Imported oftest and converted existing tests to oftest.
 *
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
