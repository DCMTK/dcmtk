/*
 *
 *  Copyright (C) 1994-2017, OFFIS e.V.
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
 *  Author:  Gerd Ehlers, Joerg Riesmeier
 *
 *  Purpose: Implementation of class DcmTime
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/ofstd/ofstd.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/dcmdata/dcmatch.h"


#define MAX_TM_LENGTH 16


// ********************************


DcmTime::DcmTime(const DcmTag &tag,
                 const Uint32 len)
  : DcmByteString(tag, len)
{
    setMaxLength(MAX_TM_LENGTH);
    setNonSignificantChars("\\");
}


DcmTime::DcmTime(const DcmTime &old)
  : DcmByteString(old)
{
}


DcmTime::~DcmTime()
{
}


DcmTime &DcmTime::operator=(const DcmTime &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


OFCondition DcmTime::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmTime &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmTime::ident() const
{
    return EVR_TM;
}


OFCondition DcmTime::checkValue(const OFString &vm,
                                const OFBool oldFormat)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
        l_error = DcmTime::checkStringValue(strVal, vm, oldFormat);
    return l_error;
}


// ********************************


OFCondition DcmTime::getOFString(OFString &stringVal,
                                 const unsigned long pos,
                                 OFBool normalize)
{
    OFCondition l_error = DcmByteString::getOFString(stringVal, pos, normalize);
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition DcmTime::getOFTime(OFTime &timeValue,
                               const unsigned long pos,
                               const OFBool supportOldFormat)
{
    OFString dicomTime;
    /* convert the current element value to OFTime format */
    OFCondition l_error = getOFString(dicomTime, pos);
    if (l_error.good())
        l_error = getOFTimeFromString(dicomTime, timeValue, supportOldFormat);
    else
        timeValue.clear();
    return l_error;
}


OFCondition DcmTime::getISOFormattedTime(OFString &formattedTime,
                                         const unsigned long pos,
                                         const OFBool seconds,
                                         const OFBool fraction,
                                         const OFBool createMissingPart,
                                         const OFBool supportOldFormat)
{
    OFString dicomTime;
    /* get current element value and convert to ISO formatted time */
    OFCondition l_error = getOFString(dicomTime, pos);
    if (l_error.good())
        l_error = getISOFormattedTimeFromString(dicomTime, formattedTime, seconds, fraction, createMissingPart, supportOldFormat);
    else
        formattedTime.clear();
    return l_error;
}


OFCondition DcmTime::setCurrentTime(const OFBool seconds,
                                    const OFBool fraction)
{
    OFString dicomTime;
    /* set the element value to the current system time */
    OFCondition l_error = getCurrentTime(dicomTime, seconds, fraction);
    if (l_error.good())
        l_error = putOFStringArray(dicomTime);
    return l_error;
}


OFCondition DcmTime::setOFTime(const OFTime &timeValue)
{
    OFString dicomTime;
    /* convert OFTime value to DICOM TM format and set the element value */
    OFCondition l_error = getDicomTimeFromOFTime(timeValue, dicomTime);
    if (l_error.good())
        l_error = putOFStringArray(dicomTime);
    return l_error;
}


// ********************************


OFCondition DcmTime::getCurrentTime(OFString &dicomTime,
                                    const OFBool seconds,
                                    const OFBool fraction)
{
    OFCondition l_error = EC_IllegalCall;
    OFTime timeValue;
    /* get the current system time */
    if (timeValue.setCurrentTime())
    {
        /* format: HHMM[SS[.FFFFFF]] */
        if (timeValue.getISOFormattedTime(dicomTime, seconds, fraction, OFFalse /*timeZone*/, OFFalse /*showDelimiter*/))
            l_error = EC_Normal;
    }
    /* set default time if an error occurred */
    if (l_error.bad())
    {
        /* if the current system time cannot be retrieved create a valid default time */
        if (seconds)
        {
            if (fraction)
            {
                /* format: HHMMSS.FFFFFF */
                dicomTime = "000000.000000";
            } else {
                /* format: HHMMS */
                dicomTime = "000000";
            }
        } else {
            /* format: HHMM */
            dicomTime = "0000";
        }
    }
    return l_error;
}


OFCondition DcmTime::getDicomTimeFromOFTime(const OFTime &timeValue,
                                            OFString &dicomTime,
                                            const OFBool seconds,
                                            const OFBool fraction)
{
    OFCondition l_error = EC_IllegalParameter;
    /* convert OFTime value to DICOM TM format */
    if (timeValue.getISOFormattedTime(dicomTime, seconds, fraction, OFFalse /*timeZone*/, OFFalse /*showDelimiter*/))
        l_error = EC_Normal;
    return l_error;
}

OFCondition DcmTime::getOFTimeFromString(const OFString &dicomTime,
                                         OFTime &timeValue)
{
    return getOFTimeFromString(dicomTime.c_str(), dicomTime.size(), timeValue, OFTrue);
}

OFCondition DcmTime::getOFTimeFromString(const OFString &dicomTime,
                                         OFTime &timeValue,
                                         const OFBool supportOldFormat)
{
    return getOFTimeFromString(dicomTime.c_str(), dicomTime.size(), timeValue, supportOldFormat);
}

OFCondition DcmTime::getOFTimeFromString(const OFString &dicomTime,
                                         OFTime &timeValue,
                                         const OFBool supportOldFormat,
                                         const double timeZone)
{
    return getOFTimeFromString(dicomTime.c_str(), dicomTime.size(), timeValue, supportOldFormat, timeZone);
}

OFCondition DcmTime::getOFTimeFromString(const char *dicomTime,
                                         const size_t dicomTimeSize,
                                         OFTime &timeValue)
{
    return getOFTimeFromString(dicomTime, dicomTimeSize, timeValue, OFTrue);
}

OFCondition DcmTime::getOFTimeFromString(const char *dicomTime,
                                         const size_t dicomTimeSize,
                                         OFTime &timeValue,
                                         const OFBool supportOldFormat)
{
    return getOFTimeFromString(dicomTime, dicomTimeSize, timeValue, supportOldFormat, OFTime::getLocalTimeZone());
}

OFCondition DcmTime::getOFTimeFromString(const char *dicomTime,
                                         const size_t dicomTimeSize,
                                         OFTime &timeValue,
                                         const OFBool supportOldFormat,
                                         const double timeZone)
{
    // clear result variable
    timeValue.clear();
    // do checks for any valid DICOM time format, before performing any extraction
    if (dicomTimeSize < 2 || !OFStandard::checkDigits<2>(dicomTime))
        return EC_IllegalParameter;
    unsigned int minutes = 0;
    double seconds = 0;
    // test for HH[MM[SS[.FFFFFF]]] format
    switch (dicomTimeSize)
    {
    default:
        if (dicomTimeSize < 7 || dicomTime[6] != '.' || !parseFragment(dicomTime + 7, dicomTimeSize - 7, seconds))
            break;
    case 6:
        if (OFStandard::checkDigits<2>(dicomTime + 4))
            seconds += OFStandard::extractDigits<unsigned int,2>(dicomTime + 4);
        else
            break;
    case 4:
        if (OFStandard::checkDigits<2>(dicomTime + 2))
            minutes = OFStandard::extractDigits<unsigned int,2>(dicomTime + 2);
        else
            break;
    case 2:
        if (timeValue.setTime(OFStandard::extractDigits<unsigned int,2>(dicomTime), minutes, seconds, timeZone))
            return EC_Normal;
        else
            return EC_IllegalParameter;
    }
    // test for legacy time format HH[:MM[:SS[.FFFFFF]]], if enabled
    if (supportOldFormat && dicomTimeSize >= 5 && dicomTime[2] == ':' && OFStandard::checkDigits<2>(dicomTime + 3))
    {
        seconds = 0;
        switch (dicomTimeSize)
        {
        default:
            if (dicomTimeSize < 9 || dicomTime[8] != '.' || !parseFragment(dicomTime + 9, dicomTimeSize - 9, seconds))
                break;
        case 8:
            if (dicomTime[5] == ':' && OFStandard::checkDigits<2>(dicomTime + 6))
                seconds += OFStandard::extractDigits<unsigned int,2>(dicomTime + 6);
            else
                break;
        case 5:
            if
            (
                timeValue.setTime
                (
                    OFStandard::extractDigits<unsigned int,2>(dicomTime),
                    OFStandard::extractDigits<unsigned int,2>(dicomTime + 3),
                    seconds,
                    timeZone
                )
            )
            {
                return EC_Normal;
            }
            break;
        }
    }
    return EC_IllegalParameter;
}


OFCondition DcmTime::getISOFormattedTimeFromString(const OFString &dicomTime,
                                                   OFString &formattedTime,
                                                   const OFBool seconds,
                                                   const OFBool fraction,
                                                   const OFBool createMissingPart,
                                                   const OFBool supportOldFormat)
{
    OFCondition result = EC_Normal;
    if (!dicomTime.empty())
    {
        /* minimal check for valid format */
        if (supportOldFormat || (dicomTime.find(":") == OFString_npos))
        {
            const size_t length = dicomTime.length();
            /* check for prior V3.0 version of VR=TM: HH:MM:SS.frac */
            const size_t minPos = (supportOldFormat && (length > 2) && (dicomTime[2] == ':')) ? 3 : 2;
            const size_t secPos = (supportOldFormat && (length > minPos + 2) && (dicomTime[minPos + 2] == ':')) ? minPos + 3 : minPos + 2;
            /* decimal point for fractional seconds */
            const size_t decPoint = dicomTime.find(".");
            const size_t decLength = (decPoint != OFString_npos) ? decPoint : length;
            OFString hourStr, minStr, secStr, fracStr;
            /* hours */
            if (decLength >= 2)
                hourStr = dicomTime.substr(0, 2);
            else
                hourStr = "00";
            /* minutes */
            if (decLength >= minPos + 2)
                minStr = dicomTime.substr(minPos, 2);
            else
                minStr = "00";
            /* seconds */
            if (decLength >= secPos + 2)
                secStr = dicomTime.substr(secPos, 2);
            else if (createMissingPart)
                secStr = "00";
            /* fractional seconds */
            if ((length >= secPos + 4) && (decPoint == secPos + 2))
            {
                if (length < secPos + 9)
                {
                    fracStr = dicomTime.substr(secPos + 3);
                    fracStr.append(secPos + 9 - length, '0');
                } else
                    fracStr = dicomTime.substr(secPos + 3, 6);
            } else if (createMissingPart)
                fracStr = "000000";
            /* concatenate time components */
            formattedTime = hourStr;
            formattedTime += ":";
            formattedTime += minStr;
            if (seconds && !secStr.empty())
            {
                formattedTime += ":";
                formattedTime += secStr;
                if (fraction && !fracStr.empty())
                {
                    formattedTime += ".";
                    formattedTime += fracStr;
                }
            }
            result = EC_Normal;
        } else {
            /* invalid input format */
            result = EC_IllegalParameter;
        }
        /* clear the result variable in case of error */
        if (result.bad())
            formattedTime.clear();
    } else {
        /* input string is empty, so is the result string */
        formattedTime.clear();
    }
    return result;
}

OFCondition DcmTime::getTimeZoneFromString(const OFString &dicomTimeZone,
                                           double &timeZone)
{
    return getTimeZoneFromString(dicomTimeZone.c_str(), dicomTimeZone.size(), timeZone);
}

OFCondition DcmTime::getTimeZoneFromString(const char *dicomTimeZone,
                                           const size_t dicomTimeZoneSize,
                                           double &timeZone)
{
    /* init return value */
    timeZone = 0;
    /* minimal check for valid format */
    if (dicomTimeZoneSize == 5 && (dicomTimeZone[0] == '+' || dicomTimeZone[0] == '-') && OFStandard::checkDigits<4>(dicomTimeZone + 1))
    {
        timeZone = OFstatic_cast(double, (OFStandard::extractDigits<unsigned,2>(dicomTimeZone + 1)))
            + OFstatic_cast(double, (OFStandard::extractDigits<unsigned,2>(dicomTimeZone + 3))) / 60;
        if (dicomTimeZone[0] == '-')
            timeZone = -timeZone;
        return EC_Normal;
    }
    return EC_IllegalParameter;
}


// ********************************


OFCondition DcmTime::checkStringValue(const OFString &value,
                                      const OFString &vm,
                                      const OFBool oldFormat)
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
            if (length > MAX_TM_LENGTH)
            {
                result = EC_MaximumLengthViolated;
                break;
            }
            else if (dcmEnableVRCheckerForStringValues.get())
            {
                /* check value representation */
                const int vrID = DcmElement::scanValue(value, "tm", posStart, length);
                if ((vrID != 4) && (!oldFormat || (vrID != 5)))
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

OFBool DcmTime::parseFragment(const char* const string, const size_t size, double& result)
{
    const char* p = string + size - 1;
    if (p >= string && OFStandard::checkDigits<1>( p ))
    {
        result = OFStandard::extractDigits<int,1>( p ) / 10.0;
        while (--p >= string && OFStandard::checkDigits<1>( p ))
            result = ( result + OFStandard::extractDigits<int,1>( p ) ) / 10.0;
        return p < string;
    }
    return OFFalse;
}


OFBool DcmTime::matches(const OFString& key,
                        const OFString& candidate,
                        const OFBool enableWildCardMatching) const
{
  OFstatic_cast(void,enableWildCardMatching);
  return DcmAttributeMatching::rangeMatchingTime(key.c_str(), key.length(), candidate.c_str(), candidate.length());
}
