/*
 *
 *  Copyright (C) 1994-2001, OFFIS
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
 *  Author:  Gerd Ehlers, Andreas Barth, Joerg Riesmeier
 *
 *  Purpose: class DcmDateTime
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-04-11 12:31:34 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrdt.cc,v $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrdt.h"
#include "dcvrda.h"
#include "dcvrtm.h"
#include "dcdebug.h"
#include "ofstring.h"


// ********************************


DcmDateTime::DcmDateTime(const DcmTag &tag, const Uint32 len)
: DcmByteString(tag, len)
{
    maxLength = 26;
}


// ********************************


DcmDateTime::DcmDateTime( const DcmDateTime &newDT )
: DcmByteString(newDT)
{
}


// ********************************


DcmDateTime::~DcmDateTime()
{
}


// ********************************


OFCondition
DcmDateTime::getOFString(
    OFString & str,
    const unsigned long pos,
    OFBool normalize)
{
    OFCondition l_error = DcmByteString::getOFString(str, pos, normalize);
    if (l_error.good() && normalize)
        normalizeString(str, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition
DcmDateTime::getOFStringArray(
    OFString & str,
    OFBool normalize)
{
    OFCondition l_error = DcmByteString::getOFStringArray(str, normalize);
    if (l_error.good() && normalize)
        normalizeString(str, MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition
DcmDateTime::getOFDateTime(
    OFDateTime &dateTimeValue,
    const unsigned long pos)
{
    OFString dicomDateTime;
    /* convert the current element value to OFDateTime format */
    OFCondition l_error = getOFString(dicomDateTime, pos);
    if (l_error.good())
        l_error = getOFDateTimeFromString(dicomDateTime, dateTimeValue);
    else
        dateTimeValue.clear();
    return l_error;
}


OFCondition
DcmDateTime::getISOFormattedDateTime(
    OFString &formattedDateTime,
    const unsigned long pos,
    const OFBool seconds,
    const OFBool fraction,
    const OFBool timeZone,
    const OFBool createMissingPart)
{
    OFString dicomDateTime;
    OFCondition l_error = getOFString(dicomDateTime, pos);
    if (l_error.good())
        l_error = getISOFormattedDateTimeFromString(dicomDateTime, formattedDateTime, seconds, fraction, timeZone, createMissingPart);
    else
        formattedDateTime.clear();
    return l_error;
}


OFCondition
DcmDateTime::setCurrentDateTime(
    const OFBool seconds,
    const OFBool fraction,
    const OFBool timeZone)
{
    OFString dicomDateTime;
    OFCondition l_error = getCurrentDateTime(dicomDateTime, seconds, fraction, timeZone);
    if (l_error.good())
        l_error = putString(dicomDateTime.c_str());
    return l_error;
}


OFCondition
DcmDateTime::setOFDateTime(const OFDateTime &dateTimeValue)
{
    OFString dicomDateTime;
    /* convert OFDateTime value to DICOM DT format and set the element value */
    OFCondition l_error = getDicomDateTimeFromOFDateTime(dateTimeValue, dicomDateTime);
    if (l_error.good())
        l_error = putString(dicomDateTime.c_str());
    return l_error;
}


// ********************************


OFCondition
DcmDateTime::getCurrentDateTime(
    OFString &dicomDateTime,
    const OFBool seconds,
    const OFBool fraction,
    const OFBool timeZone)
{
    OFCondition l_error = EC_IllegalCall;
    OFDateTime dateTimeValue;
    /* get the current system time */
    if (dateTimeValue.setCurrentDateTime())
    {
        /* format: YYYYMMDDHHMM[SS[.FFFFFF]] */
        if (dateTimeValue.getISOFormattedDateTime(dicomDateTime, seconds, fraction, timeZone, OFFalse /*showDelimiter*/))
            l_error = EC_Normal;
    }
    /* set default date/time if an error occurred */
    if (l_error.bad())
    {   
        /* format: YYYYMMDDHHMM */
        dicomDateTime = "190001010000";
        if (seconds)
        {
            /* format: SS */
            dicomDateTime += "00";
            if (fraction)
            {
                /* format: .FFFFFF */
                dicomDateTime += ".000000";
            }
        }
        if (timeZone)
        {
            /* format: CHHMM */
            dicomDateTime += "+0000";
        }
    }
    return l_error;
}


OFCondition
DcmDateTime::getDicomDateTimeFromOFDateTime(
    const OFDateTime &dateTimeValue,
    OFString &dicomDateTime,
    const OFBool seconds,
    const OFBool fraction,
    const OFBool timeZone)
{
    OFCondition l_error = EC_IllegalParameter;
    /* convert OFDateTime value to DICOM DT format */
    if (dateTimeValue.getISOFormattedDateTime(dicomDateTime, seconds, fraction, timeZone, OFFalse /*showDelimiter*/))
        l_error = EC_Normal;
    return l_error;
}


OFCondition
DcmDateTime::getOFDateTimeFromString(
    const OFString &dicomDateTime,
    OFDateTime &dateTimeValue)
{
    OFCondition l_error = EC_IllegalParameter;
    /* clear result variable */
    dateTimeValue.clear();
    /* minimal check for valid format: YYYYMMDD */
    if (dicomDateTime.length() >= 8)
    {
        OFString string;
        unsigned int year, month, day;
        unsigned int hour = 0;
        unsigned int minute = 0;
        double second = 0;
        double timeZone = 0;
        /* check whether optional time zone is present and extract the value if so */
        if (DcmTime::getTimeZoneFromString(dicomDateTime.substr(dicomDateTime.length() - 5), timeZone).good())
            string = dicomDateTime.substr(0, dicomDateTime.length() - 5);
        else
        {
            string = dicomDateTime;
            /* no time zone specified, therefore, use the local one */
            timeZone = OFTime::getLocalTimeZone();
        }
        /* extract remaining components from date/time string: YYYYMMDDHHMM[SS[.FFFFFF]] */
        if (sscanf(string.c_str(), "%04u%02u%02u%02u%02u%lf", &year, &month, &day, &hour, &minute, &second) >= 3)
        {
            if (dateTimeValue.setDateTime(year, month, day, hour, minute, second, timeZone))
                l_error = EC_Normal;
        }
    }
    return l_error;
}


OFCondition
DcmDateTime::getISOFormattedDateTimeFromString(
    const OFString &dicomDateTime,
    OFString &formattedDateTime,
    const OFBool seconds,
    const OFBool fraction,
    const OFBool timeZone,
    const OFBool createMissingPart)
{
    OFCondition l_error = EC_IllegalParameter;
    const size_t length = dicomDateTime.length();
    /* minimum DT format: YYYYMMDD */
    if (length >= 8)
    {
        OFString timeString;
        OFDate dateValue;
        /* get formatted date: YYYY-MM-DD */
        l_error = DcmDate::getOFDateFromString(dicomDateTime.substr(0, 8), dateValue, OFFalse /*supportOldFormat*/);
        if (l_error.good())
        {
            dateValue.getISOFormattedDate(formattedDateTime);
            /* get formatted time: [HH[:MM[:SS[.FFFFFF]]]] */
            const size_t posSign = dicomDateTime.find_first_of("+-", 8);
            OFString dicomTime = (posSign != OFString_npos) ? dicomDateTime.substr(8, posSign - 8) : dicomDateTime.substr(8);
            l_error = DcmTime::getISOFormattedTimeFromString(dicomTime, timeString, seconds, fraction, createMissingPart, OFFalse /*supportOldFormat*/);
            if (l_error.good())
            {
                /* add time string with separator */
                formattedDateTime += " ";
                formattedDateTime += timeString;
                /* add optional time zone: [+/-HH:MM] */
                if (timeZone)
                {
                    /* check whether optional time zone is present: &ZZZZ */
                    if ((posSign != OFString_npos) && (length >= posSign + 5))
                    {
                        formattedDateTime += " ";
                        formattedDateTime += dicomDateTime[posSign];
                        formattedDateTime += dicomDateTime.substr(posSign + 1, 2);
                        formattedDateTime += ":";
                        formattedDateTime += dicomDateTime.substr(posSign + 3, 2);
                    } else if (createMissingPart)
                        formattedDateTime += " +00:00";
                }
            }
        }
    }
    if (l_error.bad())
        formattedDateTime.clear();
    return l_error;
}


/*
** CVS/RCS Log:
** $Log: dcvrdt.cc,v $
** Revision 1.18  2002-04-11 12:31:34  joergr
** Enhanced DICOM date, time and date/time classes. Added support for new
** standard date and time functions.
**
** Revision 1.17  2001/12/19 09:59:31  meichel
** Added prototype declaration for gettimeofday() for systems like Ultrix
**   where the function is known but no prototype present in the system headers.
**
** Revision 1.16  2001/12/18 10:42:24  meichel
** Added typecasts to avoid warning on gcc 2.95.3 on OSF/1 (Alpha)
**
** Revision 1.15  2001/11/01 16:16:00  meichel
** Including <sys/time.h> if present, needed on Linux.
**
** Revision 1.14  2001/10/10 15:21:33  joergr
** Added new flag to date/time routines allowing to choose whether the old
** prior V3.0 format for the corresponding DICOM VRs is supported or not.
**
** Revision 1.13  2001/10/04 10:16:58  joergr
** Adapted new time/date routines to Windows systems.
**
** Revision 1.12  2001/10/01 15:04:44  joergr
** Introduced new general purpose functions to get/set person names, date, time
** and date/time.
**
** Revision 1.11  2001/09/25 17:19:56  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.10  2001/06/01 15:49:16  meichel
** Updated copyright header
**
** Revision 1.9  2000/03/08 16:26:46  meichel
** Updated copyright header.
**
** Revision 1.8  1999/03/31 09:25:50  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.7  1998/11/12 16:48:24  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.6  1997/08/29 13:11:45  andreas
** Corrected Bug in getOFStringArray Implementation
**
** Revision 1.5  1997/08/29 08:32:57  andreas
** - Added methods getOFString and getOFStringArray for all
**   string VRs. These methods are able to normalise the value, i. e.
**   to remove leading and trailing spaces. This will be done only if
**   it is described in the standard that these spaces are not relevant.
**   These methods do not test the strings for conformance, this means
**   especially that they do not delete spaces where they are not allowed!
**   getOFStringArray returns the string with all its parts separated by \
**   and getOFString returns only one value of the string.
**   CAUTION: Currently getString returns a string with trailing
**   spaces removed (if dcmEnableAutomaticInputDataCorrection == OFTrue) and
**   truncates the original string (since it is not copied!). If you rely on this
**   behaviour please change your application now.
**   Future changes will ensure that getString returns the original
**   string from the DICOM object (NULL terminated) inclusive padding.
**   Currently, if you call getOF... before calling getString without
**   normalisation, you can get the original string read from the DICOM object.
**
** Revision 1.4  1997/07/03 15:10:11  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/01/05 13:27:48  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
