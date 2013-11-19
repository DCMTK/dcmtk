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
 *  Purpose: Implementation of class DcmDate
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrda.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"


// ********************************


DcmDate::DcmDate(const DcmTag &tag,
                 const Uint32 len)
  : DcmByteString(tag, len)
{
    setMaxLength(10);
    setNonSignificantChars("\\");
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


OFCondition DcmDate::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmDate &, rhs);
  }
  return EC_Normal;
}



// ********************************


DcmEVR DcmDate::ident() const
{
    return EVR_DA;
}


OFCondition DcmDate::checkValue(const OFString &vm,
                                const OFBool oldFormat)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
        l_error = DcmDate::checkStringValue(strVal, vm, oldFormat);
    return l_error;
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
        l_error = putOFStringArray(dicomDate);
    return l_error;
}


OFCondition DcmDate::setOFDate(const OFDate &dateValue)
{
    OFString dicomDate;
    /* convert OFDate value to DICOM DA format and set the element value */
    OFCondition l_error = getDicomDateFromOFDate(dateValue, dicomDate);
    if (l_error.good())
        l_error = putOFStringArray(dicomDate);
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
    OFCondition l_error = EC_Normal;
    if (!dicomDate.empty())
    {
        OFDate dateValue;
        /* convert string to OFDate */
        l_error = getOFDateFromString(dicomDate, dateValue, supportOldFormat);
        if (l_error.good())
        {
            /* convert OFDate to ISO formatted date */
            if (!dateValue.getISOFormattedDate(formattedDate))
                l_error = EC_CorruptedData;
        }
        /* clear the result variable in case of error */
        if (l_error.bad())
            formattedDate.clear();
    } else {
        /* input string is empty, so is the result string */
        formattedDate.clear();
    }
    return l_error;
}


// ********************************


OFCondition DcmDate::checkStringValue(const OFString &value,
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
            if (dcmEnableVRCheckerForStringValues.get())
            {
                /* check value representation */
                const int vrID = DcmElement::scanValue(value, "da", posStart, length);
                if ((vrID != 2) && (!oldFormat || (vrID != 3)) && (vrID != 17))
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
