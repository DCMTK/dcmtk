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
 *  Author:  Gerd Ehlers, Andreas Barth, Joerg Riesmeier
 *
 *  Purpose: Implementation of class DcmDateTime
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-20 16:44:17 $
 *  CVS/RCS Revision: $Revision: 1.33 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
                                                 const OFBool createMissingPart)
{
    /* call the real function, required to make Sun CC 2.0.1 happy (see header file) */
    return getISOFormattedDateTime(formattedDateTime, pos, seconds, fraction, timeZone,
                                   createMissingPart, " " /*dateTimeSeparator*/);
}


OFCondition DcmDateTime::getISOFormattedDateTime(OFString &formattedDateTime,
                                                 const unsigned long pos,
                                                 const OFBool seconds,
                                                 const OFBool fraction,
                                                 const OFBool timeZone,
                                                 const OFBool createMissingPart,
                                                 const OFString &dateTimeSeparator)
{
    OFString dicomDateTime;
    OFCondition l_error = getOFString(dicomDateTime, pos);
    if (l_error.good())
    {
        l_error = getISOFormattedDateTimeFromString(dicomDateTime, formattedDateTime, seconds, fraction,
                                                    timeZone, createMissingPart, dateTimeSeparator);
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
        l_error = putString(dicomDateTime.c_str());
    return l_error;
}


OFCondition DcmDateTime::setOFDateTime(const OFDateTime &dateTimeValue)
{
    OFString dicomDateTime;
    /* convert OFDateTime value to DICOM DT format and set the element value */
    OFCondition l_error = getDicomDateTimeFromOFDateTime(dateTimeValue, dicomDateTime);
    if (l_error.good())
        l_error = putString(dicomDateTime.c_str());
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
                                                           const OFBool createMissingPart)
{
    return getISOFormattedDateTimeFromString(dicomDateTime, formattedDateTime, seconds, fraction, timeZone,
                                             createMissingPart, " " /*dateTimeSeparator*/);
}


OFCondition DcmDateTime::getISOFormattedDateTimeFromString(const OFString &dicomDateTime,
                                                           OFString &formattedDateTime,
                                                           const OFBool seconds,
                                                           const OFBool fraction,
                                                           const OFBool timeZone,
                                                           const OFBool createMissingPart,
                                                           const OFString &dateTimeSeparator)
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
                formattedDateTime += dateTimeSeparator;
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
        } else {
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


/*
** CVS/RCS Log:
** $Log: dcvrdt.cc,v $
** Revision 1.33  2010-10-20 16:44:17  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.32  2010-10-14 13:14:10  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.31  2010-04-23 14:30:34  joergr
** Added new method to all VR classes which checks whether the stored value
** conforms to the VR definition and to the specified VM.
**
** Revision 1.30  2009-08-07 14:35:49  joergr
** Enhanced isEmpty() method by checking whether the data element value consists
** of non-significant characters only.
**
** Revision 1.29  2009-08-03 09:03:00  joergr
** Added methods that check whether a given string value conforms to the VR and
** VM definitions of the DICOM standards.
**
** Revision 1.28  2008-07-17 10:31:32  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.27  2007-06-29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.26  2005/12/08 15:41:51  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.25  2004/04/16 12:50:45  joergr
** Restructured code to avoid default parameter values for "complex types" like
** OFString. Required for Sun CC 2.0.1.
**
** Revision 1.24  2004/01/16 13:55:40  joergr
** Introduced new parameter "dateTimeSeparator" in getISOFormattedXXX() methods
** to support ISO 8601 as format required by XML Schema type "dateTime".
**
** Revision 1.23  2002/12/06 13:20:50  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.22  2002/11/27 12:06:56  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.21  2002/08/27 16:55:59  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.20  2002/06/20 12:06:17  meichel
** Changed toolkit to use OFStandard::atof instead of atof, strtod or
**   sscanf for all string to double conversions that are supposed to
**   be locale independent
**
** Revision 1.19  2002/04/25 10:29:13  joergr
** Removed getOFStringArray() implementation.
**
** Revision 1.18  2002/04/11 12:31:34  joergr
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
