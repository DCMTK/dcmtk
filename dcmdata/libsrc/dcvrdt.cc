/*
 *
 *  Copyright (C) 1994-2013, OFFIS e.V.
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
 *  Author:  Gerd Ehlers, Andreas Barth, Joerg Riesmeier
 *
 *  Purpose: Implementation of class DcmDateTime
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrdt.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstd.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"


#define MAX_DT_LENGTH 26


// ********************************


DcmDateTime::DcmDateTime(const DcmTag &tag,
                         const Uint32 len)
  : DcmByteString(tag, len)
{
    setMaxLength(MAX_DT_LENGTH);
    setNonSignificantChars(" \\");
}

DcmDateTime::DcmDateTime(const DcmDateTime &old)
  : DcmByteString(old)
{
}


DcmDateTime::~DcmDateTime()
{
}


DcmDateTime &DcmDateTime::operator=(const DcmDateTime &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


OFCondition DcmDateTime::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmDateTime &, rhs);
  }
  return EC_Normal;
}

// ********************************


DcmEVR DcmDateTime::ident() const
{
    return EVR_DT;
}


OFCondition DcmDateTime::checkValue(const OFString &vm,
                                    const OFBool /*oldFormat*/)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
        l_error = DcmDateTime::checkStringValue(strVal, vm);
    return l_error;
}


// ********************************


OFCondition DcmDateTime::getOFString(OFString &stringVal,
                                     const unsigned long pos,
                                     OFBool normalize)
{
    OFCondition l_error = DcmByteString::getOFString(stringVal, pos, normalize);
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition DcmDateTime::getOFDateTime(OFDateTime &dateTimeValue,
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


OFCondition DcmDateTime::getISOFormattedDateTime(OFString &formattedDateTime,
                                                 const unsigned long pos,
                                                 const OFBool seconds,
                                                 const OFBool fraction,
                                                 const OFBool timeZone,
                                                 const OFBool createMissingPart,
                                                 const OFString &dateTimeSeparator,
                                                 const OFString &timeZoneSeparator)
{
    OFString dicomDateTime;
    /* get current element value and convert to ISO formatted date/time */
    OFCondition l_error = getOFString(dicomDateTime, pos);
    if (l_error.good())
    {
        l_error = getISOFormattedDateTimeFromString(dicomDateTime, formattedDateTime, seconds, fraction,
            timeZone, createMissingPart, dateTimeSeparator, timeZoneSeparator);
    } else
        formattedDateTime.clear();
    return l_error;
}


OFCondition DcmDateTime::setCurrentDateTime(const OFBool seconds,
                                            const OFBool fraction,
                                            const OFBool timeZone)
{
    OFString dicomDateTime;
    OFCondition l_error = getCurrentDateTime(dicomDateTime, seconds, fraction, timeZone);
    if (l_error.good())
        l_error = putOFStringArray(dicomDateTime);
    return l_error;
}


OFCondition DcmDateTime::setOFDateTime(const OFDateTime &dateTimeValue)
{
    OFString dicomDateTime;
    /* convert OFDateTime value to DICOM DT format and set the element value */
    OFCondition l_error = getDicomDateTimeFromOFDateTime(dateTimeValue, dicomDateTime);
    if (l_error.good())
        l_error = putOFStringArray(dicomDateTime);
    return l_error;
}


// ********************************


OFCondition DcmDateTime::getCurrentDateTime(OFString &dicomDateTime,
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


OFCondition DcmDateTime::getDicomDateTimeFromOFDateTime(const OFDateTime &dateTimeValue,
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


OFCondition DcmDateTime::getOFDateTimeFromString(const OFString &dicomDateTime,
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
        /* scan seconds using OFStandard::atof to avoid locale issues */
        if (sscanf(string.c_str(), "%04u%02u%02u%02u%02u", &year, &month, &day, &hour, &minute) >= 3)
        {
            if (string.length() > 12)
            {
                string.erase(0, 12);
                second = OFStandard::atof(string.c_str());
            }
            if (dateTimeValue.setDateTime(year, month, day, hour, minute, second, timeZone))
                l_error = EC_Normal;
        }
    }
    return l_error;
}


OFCondition DcmDateTime::getISOFormattedDateTimeFromString(const OFString &dicomDateTime,
                                                           OFString &formattedDateTime,
                                                           const OFBool seconds,
                                                           const OFBool fraction,
                                                           const OFBool timeZone,
                                                           const OFBool createMissingPart,
                                                           const OFString &dateTimeSeparator,
                                                           const OFString &timeZoneSeparator)
{
    OFCondition l_error = EC_Normal;
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
                formattedDateTime += dateTimeSeparator;
                formattedDateTime += timeString;
                /* add optional time zone: [+/-HH:MM] */
                if (timeZone)
                {
                    /* check whether optional time zone is present: &ZZZZ */
                    if ((posSign != OFString_npos) && (length >= posSign + 5))
                    {
                        formattedDateTime += timeZoneSeparator;
                        formattedDateTime += dicomDateTime[posSign];
                        formattedDateTime += dicomDateTime.substr(posSign + 1, 2);
                        formattedDateTime += ":";
                        formattedDateTime += dicomDateTime.substr(posSign + 3, 2);
                    }
                    else if (createMissingPart)
                    {
                        formattedDateTime += timeZoneSeparator;
                        formattedDateTime += "+00:00";
                    }
                }
            }
        }
    }
    else if (length == 0)
    {
        /* an empty input string is no error ... */
        formattedDateTime.clear();
    } else {
        /* ... but all other formats are (if not handled before) */
        l_error = EC_IllegalParameter;
    }
    /* clear result variable in case of error */
    if (l_error.bad())
        formattedDateTime.clear();
    return l_error;
}


// ********************************


OFCondition DcmDateTime::checkStringValue(const OFString &value,
                                          const OFString &vm)
{
    OFCondition result = EC_Normal;
    const size_t valLen = value.length();
    if (valLen > 0)
    {
        size_t posStart = 0;
        unsigned long vmNum = 0;
        /* iterate over all value components */
        while (posStart != OFString_npos)
        {
            ++vmNum;
            /* search for next component separator */
            const size_t posEnd = value.find('\\', posStart);
            const size_t length = (posEnd == OFString_npos) ? valLen - posStart : posEnd - posStart;
            /* check length of current value component */
            if (length > MAX_DT_LENGTH)
            {
                result = EC_MaximumLengthViolated;
                break;
            }
            else if (dcmEnableVRCheckerForStringValues.get())
            {
                /* check value representation */
                const int vrID = DcmElement::scanValue(value, "dt", posStart, length);
                if ((vrID != 7) && (vrID != 18))
                {
                    result = EC_ValueRepresentationViolated;
                    break;
                }
            }
            posStart = (posEnd == OFString_npos) ? posEnd : posEnd + 1;
        }
        if (result.good() && !vm.empty())
        {
            /* check value multiplicity */
            result = DcmElement::checkVM(vmNum, vm);
        }
    }
    return result;
}
