/*
 *
 *  Copyright (C) 1994-2002, OFFIS
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
 *  Purpose: Implementation of class DcmDate
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-12-06 13:20:49 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrda.cc,v $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcvrda.h"

#define INCLUDE_CSTDIO
#include "ofstdinc.h"


// ********************************


DcmDate::DcmDate(const DcmTag &tag,
                 const Uint32 len)
  : DcmByteString(tag, len)
{
    maxLength = 10;
}


DcmDate::DcmDate(const DcmDate &old)
  : DcmByteString(old)
{
}


DcmDate::~DcmDate()
{
}


DcmDate &DcmDate::operator=(const DcmDate &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmDate::ident() const
{
    return EVR_DA;
}


// ********************************


OFCondition DcmDate::getOFString(OFString &stringVal,
                                 const unsigned long pos,
                                 OFBool normalize)
{
    OFCondition l_error = DcmByteString::getOFString(stringVal, pos, normalize);
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition DcmDate::getOFDate(OFDate &dateValue,
                               const unsigned long pos,
                               const OFBool supportOldFormat)
{
    OFString dicomDate;
    /* convert the current element value to OFDate format */
    OFCondition l_error = getOFString(dicomDate, pos);
    if (l_error.good())
        l_error = getOFDateFromString(dicomDate, dateValue, supportOldFormat);
    else
        dateValue.clear();
    return l_error;
}


OFCondition DcmDate::getISOFormattedDate(OFString &formattedDate,
                                         const unsigned long pos,
                                         const OFBool supportOldFormat)
{
    OFString dicomDate;
    /* get current element value and convert to ISO formatted date */
    OFCondition l_error = getOFString(dicomDate, pos);
    if (l_error.good())
        l_error = getISOFormattedDateFromString(dicomDate, formattedDate, supportOldFormat);
    else
        formattedDate.clear();
    return l_error;
}


OFCondition DcmDate::setCurrentDate()
{
    OFString dicomDate;
    /* set the element value to the current system date */
    OFCondition l_error = getCurrentDate(dicomDate);
    if (l_error.good())
        l_error = putString(dicomDate.c_str());
    return l_error;
}


OFCondition DcmDate::setOFDate(const OFDate &dateValue)
{
    OFString dicomDate;
    /* convert OFDate value to DICOM DA format and set the element value */
    OFCondition l_error = getDicomDateFromOFDate(dateValue, dicomDate);
    if (l_error.good())
        l_error = putString(dicomDate.c_str());
    return l_error;
}


// ********************************


OFCondition DcmDate::getCurrentDate(OFString &dicomDate)
{
    OFCondition l_error = EC_IllegalCall;
    OFDate dateValue;
    /* get the current system date */
    if (dateValue.setCurrentDate())
    {
        /* format: YYYYMMDD */
        if (dateValue.getISOFormattedDate(dicomDate, OFFalse /*showDelimiter*/))
            l_error = EC_Normal;
    }
    /* set default date if an error occurred */
    if (l_error.bad())
    {
        /* format: YYYYMMDD */
        dicomDate = "19000101";
    }
    return l_error;
}


OFCondition DcmDate::getDicomDateFromOFDate(const OFDate &dateValue,
	                                        OFString &dicomDate)
{
    OFCondition l_error = EC_IllegalParameter;
    /* convert OFDate value to DICOM DA format */
    if (dateValue.getISOFormattedDate(dicomDate, OFFalse /*showDelimiter*/))
        l_error = EC_Normal;
    return l_error;
}


OFCondition DcmDate::getOFDateFromString(const OFString &dicomDate,
                                         OFDate &dateValue,
                                         const OFBool supportOldFormat)
{
    OFCondition l_error = EC_IllegalParameter;
    /* clear result variable */
    dateValue.clear();
    /* fixed length (8 or 10 bytes) required by DICOM part 5 */
    if ((dicomDate.length() == 8) && (dicomDate.find('.') == OFString_npos))
    {
        unsigned int year, month, day;
        /* extract components from date string */
        if (sscanf(dicomDate.c_str(), "%04u%02u%02u", &year, &month, &day) == 3)
        {
            if (dateValue.setDate(year, month, day))
                l_error = EC_Normal;
        }
    }
    /* old prior V3.0 version of VR=DA: YYYY.MM.DD */
    else if (supportOldFormat && (dicomDate.length() == 10) && (dicomDate[4] == '.') && (dicomDate[7] == '.'))
    {
        unsigned int year, month, day;
        /* extract components from date string */
        if (sscanf(dicomDate.c_str(), "%04u.%02u.%02u", &year, &month, &day) == 3)
        {
            if (dateValue.setDate(year, month, day))
                l_error = EC_Normal;
        }
    }
    return l_error;
}


OFCondition DcmDate::getISOFormattedDateFromString(const OFString &dicomDate,
                                                   OFString &formattedDate,
                                                   const OFBool supportOldFormat)
{
    OFDate dateValue;
    /* convert string to OFDate */
    OFCondition l_error = getOFDateFromString(dicomDate, dateValue, supportOldFormat);
    if (l_error.good())
    {
        /* convert OFDate to ISO formatted date */
        if (!dateValue.getISOFormattedDate(formattedDate))
            l_error = EC_CorruptedData;
    }
    /* in case of error clear result variable */
    if (l_error.bad())
        formattedDate.clear();
    return l_error;
}


/*
** CVS/RCS Log:
** $Log: dcvrda.cc,v $
** Revision 1.15  2002-12-06 13:20:49  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.14  2002/11/27 12:06:55  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.13  2002/08/27 16:55:58  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.12  2002/04/11 12:31:34  joergr
** Enhanced DICOM date, time and date/time classes. Added support for new
** standard date and time functions.
**
** Revision 1.11  2001/10/10 15:20:41  joergr
** Added new flag to date/time routines allowing to choose whether the old
** prior V3.0 format for the corresponding DICOM VRs is supported or not.
**
** Revision 1.10  2001/10/04 10:16:58  joergr
** Adapted new time/date routines to Windows systems.
**
** Revision 1.9  2001/10/01 15:04:43  joergr
** Introduced new general purpose functions to get/set person names, date, time
** and date/time.
**
** Revision 1.8  2001/06/01 15:49:15  meichel
** Updated copyright header
**
** Revision 1.7  2000/03/08 16:26:46  meichel
** Updated copyright header.
**
** Revision 1.6  1999/03/31 09:25:49  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.5  1998/11/12 16:48:23  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.4  1997/07/03 15:10:10  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/01/05 13:27:47  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
