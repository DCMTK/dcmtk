/*
 *
 *  Copyright (C) 2002, OFFIS
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
 *  Module:  ofstd
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Combined class for date and time functions
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-05-24 09:43:04 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofdatime.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __OFDATIME_H
#define __OFDATIME_H

#include "osconfig.h"

#include "ofstring.h"   /* for class OFString */
#include "ofdate.h"     /* for class OFDate */
#include "oftime.h"     /* for class OFTime */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** This class is a combination of OFDate and OFTime
 */
class OFDateTime
{

 public:

    /** constructor
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
     *  Time (UTC) before they are compared.
     *  @param dateTime date and time value compared with the current value
     *  @return OFTrue if given date and time is equal, OFFalse otherwise
     */
    virtual OFBool operator==(const OFDateTime &dateTime);

    /** comparison operator (unequal)
     *  Please note that the time values are first transformed to the Coordinated Universal
     *  Time (UTC) before they are compared.
     *  @param dateTime date and time value compared with the current value
     *  @return OFTrue if given date or time is unequal, OFFalse otherwise
     */
    virtual OFBool operator!=(const OFDateTime &dateTime);

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
    
    /** set the date and time value to the current system date and time.
     *  This function uses operating system dependent routines. If the date or time function
     *  (or both) are unavailable for some reason the corresponding value is not modified.
     *  @return OFTrue if the current system date and time has been set, OFFalse otherwise
     */
    OFBool setCurrentDateTime();
    
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
     *    "YYYY-MM-DD HH:MM[:SS[.FFFFFF]][&ZZ:ZZ]" (with delimiters) and
     *    "YYYYMMDDHHMM[SS[.FFFFFF]][&ZZZZ]" (without delimiters, useful for DICOM date/time type)
     *  where the brackets enclose optional parts.
     *  @param formattedDateTime reference to string variable where the result is stored
     *  @param showSeconds add optional seconds (":SS" or "SS") to the resulting string if OFTrue
     *  @param showFraction add optional fractional part of a second (".FFFFFF") if OFTrue.
     *    Requires parameter 'seconds' to be also OFTrue.
     *  @param showTimeZone add optional time zone ("&ZZ:ZZ" or "&ZZZZ") to the resulting string
     *    if OFTrue. The time zone indicates the offset from the Coordinated Universal Time (UTC)
     *    in hours and minutes. The "&" is a placeholder for the sign symbol ("+" or "-").
     *  @param showDelimiter flag, indicating whether to use delimiters ("-", ":" and " ") or not
     *  @return OFTrue if result variable has been set, OFFalse otherwise
     */
    OFBool getISOFormattedDateTime(OFString &formattedDateTime,
                                   const OFBool showSeconds = OFTrue,
                                   const OFBool showFraction = OFFalse,
                                   const OFBool showTimeZone = OFFalse,
                                   const OFBool showDelimiter = OFTrue) const;
    
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
ostream& operator<<(ostream& stream, const OFDateTime &dateTime);


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: ofdatime.h,v $
 * Revision 1.2  2002-05-24 09:43:04  joergr
 * Renamed some parameters/variables to avoid ambiguities.
 *
 * Revision 1.1  2002/04/11 12:12:23  joergr
 * Introduced new standard classes providing date and time functions.
 *
 *
 */
