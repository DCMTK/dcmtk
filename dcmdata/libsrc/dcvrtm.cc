/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-20 16:44:18 $
 *  CVS/RCS Revision: $Revision: 1.34 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/ofstd/ofstd.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"


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
        l_error = putString(dicomTime.c_str());
    return l_error;
}


OFCondition DcmTime::setOFTime(const OFTime &timeValue)
{
    OFString dicomTime;
    /* convert OFTime value to DICOM TM format and set the element value */
    OFCondition l_error = getDicomTimeFromOFTime(timeValue, dicomTime);
    if (l_error.good())
        l_error = putString(dicomTime.c_str());
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
                                         OFTime &timeValue,
                                         const OFBool supportOldFormat)
{
    OFCondition l_error = EC_IllegalParameter;
    /* clear result variable */
    timeValue.clear();
    /* minimal check for valid format */
    if (supportOldFormat || (dicomTime.find(":") == OFString_npos))
    {
        unsigned int hour, minute = 0;
        double second = 0.0;
        /* normalize time format (remove ":" chars) */
        OFString string = dicomTime;
        if ((string.length() > 5) && (string[5] == ':'))
            string.erase(5, 1);
        if ((string.length() > 2) && (string[2] == ':'))
            string.erase(2, 1);
        /* extract components from time string: HH[MM[SS[.FFFFFF]]] */
       	/* scan seconds using OFStandard::atof to avoid locale issues */
        if (sscanf(string.c_str(), "%02u%02u", &hour, &minute) >= 1)
        {
            if (string.length() > 4)
            {
                /* get optional seconds part */
            	string.erase(0, 4);
            	second = OFStandard::atof(string.c_str());
            }
            /* always use the local time zone */
            if (timeValue.setTime(hour, minute, second, OFTime::getLocalTimeZone()))
                l_error = EC_Normal;
        }
    }
    return l_error;
}


OFCondition DcmTime::getISOFormattedTimeFromString(const OFString &dicomTime,
                                                   OFString &formattedTime,
                                                   const OFBool seconds,
                                                   const OFBool fraction,
                                                   const OFBool createMissingPart,
                                                   const OFBool supportOldFormat)
{
    OFCondition result = EC_IllegalParameter;
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
        if (seconds && (secStr.length() > 0))
        {
            formattedTime += ":";
            formattedTime += secStr;
            if (fraction && (fracStr.length() > 0))
            {
                formattedTime += ".";
                formattedTime += fracStr;
            }
        }
        result = EC_Normal;
    } else
        formattedTime.clear();
    return result;
}


OFCondition DcmTime::getTimeZoneFromString(const OFString &dicomTimeZone,
                                           double &timeZone)
{
    OFCondition result = EC_IllegalParameter;
    /* init return value */
    timeZone = 0;
    /* minimal check for valid format */
    if ((dicomTimeZone.length() == 5) && ((dicomTimeZone[0] == '+') || (dicomTimeZone[0] == '-')))
    {
        signed int hour;
        unsigned int minute;
        /* extract components from time zone string */
        if (sscanf(dicomTimeZone.c_str(), "%03i%02u", &hour, &minute) == 2)
        {
            timeZone = OFstatic_cast(double, hour) + OFstatic_cast(double, minute) / 60;
            result = EC_Normal;
        }
    }
    return result;
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
        } else {
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


/*
** CVS/RCS Log:
** $Log: dcvrtm.cc,v $
** Revision 1.34  2010-10-20 16:44:18  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.33  2010-10-14 13:14:11  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.32  2010-04-23 14:30:35  joergr
** Added new method to all VR classes which checks whether the stored value
** conforms to the VR definition and to the specified VM.
**
** Revision 1.31  2009-08-07 14:35:49  joergr
** Enhanced isEmpty() method by checking whether the data element value consists
** of non-significant characters only.
**
** Revision 1.30  2009-08-03 09:03:00  joergr
** Added methods that check whether a given string value conforms to the VR and
** VM definitions of the DICOM standards.
**
** Revision 1.29  2008-07-17 10:31:32  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.28  2007-06-29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.27  2005/12/08 15:42:05  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.26  2004/01/16 13:46:38  joergr
** Adapted type casts to new-style typecast operators defined in ofcast.h.
**
** Revision 1.25  2002/12/06 13:20:52  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.24  2002/11/27 12:06:59  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.23  2002/08/27 16:56:00  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.22  2002/07/16 14:33:08  joergr
** Fixed bug in DcmTime::getCurrentTime().
**
** Revision 1.21  2002/07/16 14:21:29  joergr
** Fixed bug in DcmTime::getOFTimeFromString().
**
** Revision 1.20  2002/06/20 12:06:18  meichel
** Changed toolkit to use OFStandard::atof instead of atof, strtod or
**   sscanf for all string to double conversions that are supposed to
**   be locale independent
**
** Revision 1.19  2002/04/25 10:34:35  joergr
** Removed getOFStringArray() implementation.
**
** Revision 1.18  2002/04/11 12:31:35  joergr
** Enhanced DICOM date, time and date/time classes. Added support for new
** standard date and time functions.
**
** Revision 1.17  2001/12/19 09:59:31  meichel
** Added prototype declaration for gettimeofday() for systems like Ultrix
**   where the function is known but no prototype present in the system headers.
**
** Revision 1.16  2001/12/18 10:42:25  meichel
** Added typecasts to avoid warning on gcc 2.95.3 on OSF/1 (Alpha)
**
** Revision 1.15  2001/11/01 16:16:01  meichel
** Including <sys/time.h> if present, needed on Linux.
**
** Revision 1.14  2001/10/10 15:20:42  joergr
** Added new flag to date/time routines allowing to choose whether the old
** prior V3.0 format for the corresponding DICOM VRs is supported or not.
**
** Revision 1.13  2001/10/04 10:16:59  joergr
** Adapted new time/date routines to Windows systems.
**
** Revision 1.12  2001/10/01 15:04:45  joergr
** Introduced new general purpose functions to get/set person names, date, time
** and date/time.
**
** Revision 1.11  2001/09/25 17:20:01  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.10  2001/06/01 15:49:21  meichel
** Updated copyright header
**
** Revision 1.9  2000/03/08 16:26:51  meichel
** Updated copyright header.
**
** Revision 1.8  1999/03/31 09:26:00  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.7  1998/11/12 16:48:30  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.6  1997/08/29 13:11:49  andreas
** Corrected Bug in getOFStringArray Implementation
**
** Revision 1.5  1997/08/29 08:33:01  andreas
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
** Revision 1.4  1997/07/03 15:10:19  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/01/05 13:27:54  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
