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
 *  Module:  ofstd
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Combined class for date and time functions
 *
 */


#ifndef OFDATIME_H
#define OFDATIME_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofstring.h"   /* for class OFString */
#include "dcmtk/ofstd/ofdate.h"     /* for class OFDate */
#include "dcmtk/ofstd/oftime.h"     /* for class OFTime */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** This class is a combination of OFDate and OFTime
 */
class DCMTK_OFSTD_EXPORT OFDateTime
{

 public:

    /** default constructor.
     *  Initializes Date and Time to 0.
     */
    OFDateTime();

    /** copy constructor
     *  @param dateTime date/time object to be copied
     */
    OFDateTime(const OFDateTime &dateTime);

    /** copy constructor
     *  @param dateVal date object to be copied
     *  @param timeVal time object to be copied
     */
    OFDateTime(const OFDate &dateVal,
               const OFTime &timeVal);

    /** constructor
     *  @param year year value to be set
     *  @param month month value to be set
     *  @param day new value to be set
     *  @param hour hour value to be set
     *  @param minute minute value to be set
     *  @param second second value to be set (incl. fraction of seconds)
     *  @param timeZone optional offset to Coordinated Universal Time (UTC) in hours
     */
    OFDateTime(const unsigned int year,
               const unsigned int month,
               const unsigned int day,
               const unsigned int hour,
               const unsigned int minute,
               const double second,
               const double timeZone = 0);

    /** destructor
     */
    virtual ~OFDateTime();

    /** assignment operator
     *  @param dateTime date/time value to be set
     *  @return reference to this object (with new value)
     */
    virtual OFDateTime &operator=(const OFDateTime &dateTime);

    /** comparison operator (equal).
     *  Please note that the time values are first transformed to the Coordinated Universal
     *  Time (UTC) before they are compared. However, the special case that there is an
     *  "overflow" from one day to another is currently not handled.
     *  @param dateTime date and time value compared with the current value
     *  @return OFTrue if given date and time is equal, OFFalse otherwise
     */
    virtual OFBool operator==(const OFDateTime &dateTime) const;

    /** comparison operator (unequal)
     *  Please note that the time values are first transformed to the Coordinated Universal
     *  Time (UTC) before they are compared. However, the special case that there is an
     *  "overflow" from one day to another is currently not handled.
     *  @param dateTime date and time value compared with the current value
     *  @return OFTrue if given date or time is unequal, OFFalse otherwise
     */
    virtual OFBool operator!=(const OFDateTime &dateTime) const;

    /** reset the date/time value.
     *  Sets all date and all time components to '0'. NB: Date becomes invalid.
     */
    virtual void clear();

    /** check whether the currently stored date and time value is valid.
     *  See classes OFDate and OFTime for details.
     *  @return OFTrue if the current value is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** set the date/time value.
     *  Before the new value is set it is checked using the "isValid()" routine.
     *  @param year new year value to be set
     *  @param month new month value to be set
     *  @param day new day value to be set
     *  @param hour new hour value to be set
     *  @param minute new minute value to be set
     *  @param second new second value to be set (incl. fraction of seconds)
     *  @param timeZone optional offset to Coordinated Universal Time (UTC) in hours
     *  @return OFTrue if the new value is valid and has been set, OFFalse otherwise
     */
    OFBool setDateTime(const unsigned int year,
                       const unsigned int month,
                       const unsigned int day,
                       const unsigned int hour,
                       const unsigned int minute,
                       const double second,
                       const double timeZone = 0);

    /** set the date component to the specified date.
     *  Before the new value is set it is checked using the "isValid()" routine.
     *  @param dateVal new date value to be set
     *  @return OFTrue if the new value is valid and has been set, OFFalse otherwise
     */
    OFBool setDate(const OFDate &dateVal);

    /** set the time component to the specified time.
     *  Before the new value is set it is checked using the "isValid()" routine.
     *  @param timeVal new time value to be set
     *  @return OFTrue if the new value is valid and has been set, OFFalse otherwise
     */
    OFBool setTime(const OFTime &timeVal);

    /** set the date and time component to the specified date/time.
     *  Before the new value is set it is checked using the "isValid()" routine.
     *  @param dateVal new date value to be set
     *  @param timeVal new time value to be set
     *  @return OFTrue if the new value is valid and has been set, OFFalse otherwise
     */
    OFBool setDateTime(const OFDate &dateVal,
                       const OFTime &timeVal);

    /** set the date and time value to the current system date and time.
     *  This function uses operating system dependent routines. If the date or time function
     *  (or both) are unavailable for some reason the corresponding value is not modified.
     *  @return OFTrue if the current system date and time has been set, OFFalse otherwise
     */
    OFBool setCurrentDateTime();

    /** set the date/time value to the given ISO formatted date/time string.
     *  The two ISO date/time formats supported by this function are
     *  - "YYYY-MM-DD HH:MM[:SS [&ZZ:ZZ]]" (with arbitrary delimiters) and
     *  - "YYYYMMDDHHMM[SS[&ZZZZ]]" (without delimiters, useful for DICOM datetime type)
     *  where the brackets enclose optional parts. Please note that the optional fractional
     *  part of a second ".FFFFFF" (see getISOFormattedDateTime()) is not yet supported.
     *  @param formattedDateTime ISO formatted date/time value to be set
     *  @return OFTrue if input is valid and result variable has been set, OFFalse otherwise
     */
    OFBool setISOFormattedDateTime(const OFString &formattedDateTime);

    /** get the current date component
     *  @return reference to date object
     */
    const OFDate &getDate() const;

    /** get the current time component
     *  @return reference to time object
     */
    const OFTime &getTime() const;

    /** get the current date/time value in ISO format.
     *  The two ISO time formats supported by this function are
     *  - "YYYY-MM-DD HH:MM[:SS[.FFFFFF]] [&ZZ:ZZ]" (with delimiters) and
     *  - "YYYYMMDDHHMM[SS[.FFFFFF]][&ZZZZ]" (without delimiters, useful for DICOM datetime type)
     *  where the brackets enclose optional parts.
     *  @param formattedDateTime reference to string variable where the result is stored
     *  @param showSeconds add optional seconds (":SS" or "SS") to the resulting string if OFTrue
     *  @param showFraction add optional fractional part of a second (".FFFFFF") if OFTrue.
     *    Requires parameter 'seconds' to be also OFTrue.
     *  @param showTimeZone add optional time zone ("&ZZ:ZZ" or "&ZZZZ") to the resulting string
     *    if OFTrue. The time zone indicates the offset from the Coordinated Universal Time (UTC)
     *    in hours and minutes. The "&" is a placeholder for the sign symbol ("+" or "-").
     *  @param showDelimiter flag, indicating whether to use delimiters ("-", ":" and " ") or not
     *  @param dateTimeSeparator separator between ISO date and time value. Only used if
     *    'showDelimiter' is true.
     *  @param timeZoneSeparator separator between ISO time value and time zone. Only used if
     *    'showDelimiter' is true.
     *  @return OFTrue if result variable has been set, OFFalse otherwise
     */
    OFBool getISOFormattedDateTime(OFString &formattedDateTime,
                                   const OFBool showSeconds = OFTrue,
                                   const OFBool showFraction = OFFalse,
                                   const OFBool showTimeZone = OFFalse,
                                   const OFBool showDelimiter = OFTrue,
                                   const OFString &dateTimeSeparator = " ",
                                   const OFString &timeZoneSeparator = " ") const;

    /* --- static helper functions --- */

    /** get the current system date and time
     *  @return date/time object with the current system date and time set
     */
    static OFDateTime getCurrentDateTime();


 private:

    /// currently stored date value
    OFDate Date;

    /// currently stored time value
    OFTime Time;
};


/** put the given date/time in ISO format on the output stream.
 *  If an error occurs nothing is printed.
 *  @param stream output stream
 *  @param time OFDateTime object to print
 *  @return reference to the output stream
 */
DCMTK_OFSTD_EXPORT STD_NAMESPACE ostream& operator<<(STD_NAMESPACE ostream& stream, const OFDateTime &dateTime);


#endif
