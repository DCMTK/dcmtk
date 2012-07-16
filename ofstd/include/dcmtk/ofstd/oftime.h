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
 *  Purpose: Class for time functions
 *
 */


#ifndef OFTIME_H
#define OFTIME_H

#include "dcmtk/config/osconfig.h"

#define INCLUDE_CTIME
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>    /* for struct time_t */
#endif
END_EXTERN_C

#include "dcmtk/ofstd/ofstring.h"      /* for class OFString */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** This class provides a collection of time functions
 */
class DCMTK_OFSTD_EXPORT OFTime
{
    // allow class OFDateTime to access protected class members
    friend class OFDateTime;


 public:

    /** default constructor.
     *  Initializes Hour, Minute, Second and TimeZone to 0.
     */
    OFTime();

    /** copy constructor
     *  @param timeVal time object to be copied
     */
    OFTime(const OFTime &timeVal);

    /** constructor with init values
     *  @param hour hour value to be set
     *  @param minute minute value to be set
     *  @param second second value to be set (incl. fraction of seconds)
     *  @param timeZone optional offset to Coordinated Universal Time (UTC) in hours
     */
    OFTime(const unsigned int hour,
           const unsigned int minute,
           const double second,
           const double timeZone = 0);

    /** destructor
     */
    virtual ~OFTime();

    /** assignment operator
     *  @param timeVal time value to be set
     *  @return reference to this object (with new value)
     */
    virtual OFTime &operator=(const OFTime &timeVal);

    /** comparison operator (equal).
     *  Please note that the time values are first transformed to the Coordinated Universal
     *  Time (UTC) before they are compared.
     *  @param timeVal time value compared with the current value
     *  @return OFTrue if given time is equal, OFFalse otherwise
     */
    virtual OFBool operator==(const OFTime &timeVal) const;

    /** comparison operator (unequal)
     *  Please note that the time values are first transformed to the Coordinated Universal
     *  Time (UTC) before they are compared.
     *  @param timeVal time value compared with the current value
     *  @return OFTrue if given time is unequal, OFFalse otherwise
     */
    virtual OFBool operator!=(const OFTime &timeVal) const;

    /** comparison operator (less than)
     *  Please note that the time values are first transformed to the Coordinated Universal
     *  Time (UTC) before they are compared.
     *  @param timeVal time value compared with the current value
     *  @return OFTrue if current time is earlier than the given value, OFFalse otherwise
     */
    virtual OFBool operator<(const OFTime &timeVal) const;

    /** comparison operator (less than or equal)
     *  Please note that the time values are first transformed to the Coordinated Universal
     *  Time (UTC) before they are compared.
     *  @param timeVal time value compared with the current value
     *  @return OFTrue if current time is earlier than or identical to the given value,
     *    OFFalse otherwise
     */
    virtual OFBool operator<=(const OFTime &timeVal) const;

    /** comparison operator (greater than or equal)
     *  Please note that the time values are first transformed to the Coordinated Universal
     *  Time (UTC) before they are compared.
     *  @param timeVal time value compared with the current value
     *  @return OFTrue if current time is later than or identical to the given value,
     *    OFFalse otherwise
     */
    virtual OFBool operator>=(const OFTime &timeVal) const;

    /** comparison operator (greater than)
     *  Please note that the time values are first transformed to the Coordinated Universal
     *  Time (UTC) before they are compared.
     *  @param timeVal time value compared with the current value
     *  @return OFTrue if current time is later than the given value, OFFalse otherwise
     */
    virtual OFBool operator>(const OFTime &timeVal) const;

    /** reset the time value.
     *  Sets the hour, minute, second and time zone to "0".
     */
    virtual void clear();

    /** check whether the currently stored time value is valid.
     *  Valid ranges: [0,24[ for 'hour', [0,60[ for 'minute', [0.0,60.0[ for 'second'
     *  and [-12.0,+14.0] for 'timeZone'
     *  @return OFTrue if the current value is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** set the time value to the specified time.
     *  Before the new value is set it is checked using the "isValid()" routine.
     *  @param hour new hour value to be set
     *  @param minute new minute value to be set
     *  @param second new second value to be set (incl. fraction of seconds)
     *  @param timeZone optional offset to Coordinated Universal Time (UTC) in hours
     *  @return OFTrue if the new value is valid and has been set, OFFalse otherwise
     */
    OFBool setTime(const unsigned int hour,
                   const unsigned int minute,
                   const double second,
                   const double timeZone = 0);

    /** set the time value to the specified hour.
     *  Before the new value is set it is checked using the "isValid()" routine.
     *  @param hour new hour value to be set
     *  @return OFTrue if the new value is valid and has been set, OFFalse otherwise
     */
    OFBool setHour(const unsigned int hour);

    /** set the time value to the specified minute.
     *  Before the new value is set it is checked using the "isValid()" routine.
     *  @param minute new minute value to be set
     *  @return OFTrue if the new value is valid and has been set, OFFalse otherwise
     */
    OFBool setMinute(const unsigned int minute);

    /** set the time value to the specified second.
     *  Before the new value is set it is checked using the "isValid()" routine.
     *  @param second new second value to be set (incl. fraction of seconds)
     *  @return OFTrue if the new value is valid and has been set, OFFalse otherwise
     */
    OFBool setSecond(const double second);

    /** set the time zone to the specified value.
     *  The time zone is given as the offset (in hours) to the Coordinated Universal
     *  Time (UTC). Valid values are for instance "+1.0" (plus one hour) and "-2.5"
     *  (minus two and a half hour, i.e. 2 hours and 30 minutes). Before the new value
     *  is set it is checked using the "isValid()" routine.
     *  @param timeZone new timeZone value to be set
     *  @return OFTrue if the new value is valid and has been set, OFFalse otherwise
     */
    OFBool setTimeZone(const double timeZone);

    /** set the time zone to the specified value.
     *  The time zone is given as the offset (in hours and minutes) to the Coordinated
     *  Universal Time (UTC). Before the new value is set it is checked using the
     *  "isValid()" routine.
     *  @param hour new hour value to be set for the time zone
     *  @param minute new minute value to be set for the time zone
     *  @return OFTrue if the new value is valid and has been set, OFFalse otherwise
     */
    OFBool setTimeZone(const signed int hour,
                       const unsigned int minute);

    /** set the time to the specified number of seconds
     *  @param seconds number of seconds since "00:00:00" specifying time to set
     *  @param timeZone optional offset to Coordinated Universal Time (UTC) in hours
     *  @param normalize if OFTrue the 'seconds' value is normalized to the valid range
     *    [0.0,86400.0[ otherwise the value is checked as is
     *  @return OFTrue if the new value is valid and has been set, OFFalse otherwise
     */
    OFBool setTimeInSeconds(const double seconds,
                            const double timeZone = 0,
                            const OFBool normalize = OFTrue);

    /** set the time to the specified number of hours
     *  @param hours number of hours since "00:00:00" specifying time to set
     *  @param timeZone optional offset to Coordinated Universal Time (UTC) in hours
     *  @param normalize if OFTrue the 'hours' value is normalized to the valid range
     *    [0.0,24.0[ otherwise the value is checked as is
     *  @return OFTrue if the new value is valid and has been set, OFFalse otherwise
     */
    OFBool setTimeInHours(const double hours,
                          const double timeZone = 0,
                          const OFBool normalize = OFTrue);

    /** set the time value to the current system time.
     *  This function uses operating system dependent routines. If they are unavailable
     *  for some reason the current value is not modified.
     *  @return OFTrue if the current system time has been set, OFFalse otherwise
     */
    OFBool setCurrentTime();

    /** set the time value to the given ISO formatted time string.
     *  The two ISO time formats supported by this function are
     *  - "HH:MM[:SS [&ZZ:ZZ]]" (with arbitrary delimiters) and
     *  - "HHMM[SS[&ZZZZ]]" (without delimiters)
     *  where the brackets enclose optional parts. Please note that the optional fractional
     *  part of a second ".FFFFFF" (see getISOFormattedTime()) is not yet supported.
     *  @param formattedTime ISO formatted time value to be set
     *  @return OFTrue if input is valid and result variable has been set, OFFalse otherwise
     */
    OFBool setISOFormattedTime(const OFString &formattedTime);

    /** get the currently stored hour value
     *  @return hour value (might be invalid, i.e. out of range)
     */
    unsigned int getHour() const;

    /** get the currently stored minute value
     *  @return minute value (might be invalid, i.e. out of range)
     */
    unsigned int getMinute() const;

    /** get the currently stored second value
     *  @return second value (might be invalid, i.e. out of range)
     */
    double getSecond() const;

    /** get the integral part of the currently stored second value
     *  @return second value (might be invalid, i.e. out of range)
     */
    unsigned int getIntSecond() const;

    /** get the milli second part of the currently stored second value
     *  @return milli second part (0..999)
     */
    unsigned int getMilliSecond() const;

    /** get the micro second part of the currently stored second value
     *  @return micro second part (0..999999)
     */
    unsigned int getMicroSecond() const;

    /** get the currently stored time zone value.
     *  The time zone is given as the offset (in hours) to the Coordinated Universal
     *  Time (UTC). Valid values are for instance "+1.0" (plus one hour) and "-2.5"
     *  (minus two and a half hour, i.e. 2 hours and 30 minutes).
     *  @return time zone value (might be invalid, i.e. out of range)
     */
    double getTimeZone() const;

    /** get the currently stored time in seconds
     *  @param useTimeZone use the currently set time zone to correct the result
     *  @param normalize if OFTrue results larger than or equal 86400.0 (seconds)
     *    are normalized to a valid range
     *  @return number of seconds since "00:00:00"
     */
    double getTimeInSeconds(const OFBool useTimeZone = OFFalse,
                            const OFBool normalize = OFTrue) const;

    /** get the currently stored time in hours.
     *  @param useTimeZone use the currently set time zone to correct the result
     *  @param normalize if OFTrue results larger than or equal 24.0 (hours)
     *    are normalized to a valid range
     *  @return number of hours since "00:00:00" (incl. fraction of hours)
     */
    double getTimeInHours(const OFBool useTimeZone = OFFalse,
                          const OFBool normalize = OFTrue) const;

    /** get the currently stored time as Coordinated Universal Time.
     *  The resulting time object always has a time zone of "+00:00" since the time is
     *  corrected regarding the Coordinated Universal Time (UTC).
     *  @return time object with the UTC corrected time set
     */
    OFTime getCoordinatedUniversalTime() const;

    /** get the currently stored time in local time.
     *  The resulting time object always has the local time zone.
     *  @return time object with the corrected time set
     */
    OFTime getLocalTime() const;

    /** get the current time value in ISO format.
     *  The two ISO time formats supported by this function are
     *  - "HH:MM[:SS[.FFFFFF]] [&ZZ:ZZ]" (with delimiters) and
     *  - "HHMM[SS[.FFFFFF]][&ZZZZ]" (without delimiters, useful for DICOM time type)
     *  where the brackets enclose optional parts.
     *  @param formattedTime reference to string variable where the result is stored
     *  @param showSeconds add optional seconds (":SS" or "SS") to the resulting string if OFTrue
     *  @param showFraction add optional fractional part of a second (".FFFFFF") if OFTrue.
     *    Requires parameter 'seconds' to be also OFTrue.
     *  @param showTimeZone add optional time zone ("&ZZ:ZZ" or "&ZZZZ") to the resulting string
     *    if OFTrue. The time zone indicates the offset from the Coordinated Universal Time (UTC)
     *    in hours and minutes. The "&" is a placeholder for the sign symbol ("+" or "-").
     *  @param showDelimiter flag, indicating whether to use delimiters (":") or not
     *  @param timeZoneSeparator separator between ISO time value and time zone. Only used if
     *    'showDelimiter' is true.
     *  @return OFTrue if result variable has been set, OFFalse otherwise
     */
    OFBool getISOFormattedTime(OFString &formattedTime,
                               const OFBool showSeconds = OFTrue,
                               const OFBool showFraction = OFFalse,
                               const OFBool showTimeZone = OFFalse,
                               const OFBool showDelimiter = OFTrue,
                               const OFString &timeZoneSeparator = " ") const;

    /* --- static helper functions --- */

    /** get the current system time
     *  @return time object with the current system time set
     */
    static OFTime getCurrentTime();

    /** get the local time zone.
     *  This function uses operating system dependent routines. If they are unavailable
     *  for some reason the Coordinated Universal Time is assumed (time zone offset = 0).
     *  Also please note that time zones in the range ]+12.0,+14.0] cannot be detected
     *  due to the internally used algorithm.
     *  @return local time zone if available, 0 otherwise
     */
    static double getLocalTimeZone();


 protected:

    /** set the time value to the current system time.
     *  This function uses operating system dependent routines. If they are unavailable
     *  for some reason the current value is not modified.
     *  @param tt current system time (as returned by the time() function )
     *  @return OFTrue if the current system time has been set, OFFalse otherwise
     */
    OFBool setCurrentTime(const time_t &tt);

    /* --- static helper functions --- */

    /** check whether the given time is valid.
     *  Valid ranges: [0,24[ for 'hour', [0,60[ for 'minute', [0.0,60.0[ for 'second'
     *  and [-12.0,+14.0] for 'timeZone'
     *  @param hour hour value to be checked
     *  @param minute minute value to be checked
     *  @param second second value to be checked
     *  @param timeZone offset to Coordinated Universal Time (UTC) in hours
     *  @return OFTrue if the current value is valid, OFFalse otherwise
     */
    static OFBool isTimeValid(const unsigned int hour,
                              const unsigned int minute,
                              const double second,
                              const double timeZone);

    /** get the given time in seconds.
     *  @param hour hour value of the given time
     *  @param minute minute value of the given time
     *  @param second second value of the given time
     *  @param timeZone offset to Coordinated Universal Time (UTC) in hours
     *  @param normalize if OFTrue results larger than or equal 86400.0 (seconds)
     *    are normalized to a valid range
     *  @return number of seconds since "00:00:00"
     */
    static double getTimeInSeconds(const unsigned int hour,
                                   const unsigned int minute,
                                   const double second,
                                   const double timeZone = 0,
                                   const OFBool normalize = OFTrue);

    /** get the given time in hours.
     *  @param hour hour value of the given time
     *  @param minute minute value of the given time
     *  @param second second value of the given time
     *  @param timeZone offset to Coordinated Universal Time (UTC) in hours
     *  @param normalize if OFTrue results larger than or equal 24.0 (hours)
     *    are normalized to a valid range
     *  @return number of hours since "00:00:00" (incl. fraction of hours)
     */
    static double getTimeInHours(const unsigned int hour,
                                 const unsigned int minute,
                                 const double second,
                                 const double timeZone = 0,
                                 const OFBool normalize = OFTrue);


 private:

    /// currently stored hour value
    unsigned int Hour;

    /// currently stored minute value
    unsigned int Minute;

    /// currently stored second value (incl. fraction of seconds)
    double Second;

    /// currently stored time zone value
    double TimeZone;
};


/** put the given time in ISO format on the output stream.
 *  If an error occurs nothing is printed.
 *  @param stream output stream
 *  @param timeVal OFTime object to print
 *  @return reference to the output stream
 */
DCMTK_OFSTD_EXPORT STD_NAMESPACE ostream& operator<<(STD_NAMESPACE ostream& stream, const OFTime &timeVal);


#endif
