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
 *  Purpose: Class for date functions
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-04-11 12:12:23 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofdate.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __OFDATE_H
#define __OFDATE_H

#include "osconfig.h"

#include "ofstring.h"   /* for class OFString */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** This class provides a collection of date functions
 */
class OFDate
{
    // allow class OFDateTime to access protected class members
    friend class OFDateTime;


 public:

    /** constructor
     */
    OFDate();

    /** copy constructor
     *  @param date date object to be copied
     */
    OFDate(const OFDate &date);

    /** constructor with init values
     *  @param year year value to be set
     *  @param month month value to be set
     *  @param day day value to be set
     */
    OFDate(const unsigned int year,
           const unsigned int month,
           const unsigned int day);

    /** destructor
     */
    virtual ~OFDate();

    /** assignment operator
     *  @param date date value to be set
     *  @return reference to this object (with new value)
     */
    virtual OFDate &operator=(const OFDate &date);

    /** comparison operator (equal)
     *  @param date date value compared with the current value
     *  @return OFTrue if given date is equal, OFFalse otherwise
     */
    virtual OFBool operator==(const OFDate &date);

    /** comparison operator (unequal)
     *  @param date date value compared with the current value
     *  @return OFTrue if given date is unequal, OFFalse otherwise
     */
    virtual OFBool operator!=(const OFDate &date);

    /** comparison operator (less than)
     *  @param date date value compared with the current value
     *  @return OFTrue if given date is earlier than the current value, OFFalse otherwise
     */
    virtual OFBool operator<(const OFDate &date);

    /** comparison operator (less than or equal)
     *  @param date date value compared with the current value
     *  @return OFTrue if given date is earlier or on the same day, OFFalse otherwise
     */
    virtual OFBool operator<=(const OFDate &date);

    /** comparison operator (greater than or equal)
     *  @param date date value compared with the current value
     *  @return OFTrue if given date is later or on the same day, OFFalse otherwise
     */
    virtual OFBool operator>=(const OFDate &date);

    /** comparison operator (greater than)
     *  @param date date value compared with the current value
     *  @return OFTrue if given date is later than the current value, OFFalse otherwise
     */
    virtual OFBool operator>(const OFDate &date);

    /** reset the date value.
     *  Sets the year, month and day to "0". NB: Date becomes invalid.
     */
    virtual void clear();

    /** check whether the currently stored date value is valid.
     *  Currently, this method only checks whether the month is within the range of 1 to 12
     *  and the day is within the range of 1 to 31. In the future this routine might be
     *  enhanced.
     *  @return OFTrue if the current value is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** set the date value to the specified date.
     *  Before the new value is set it is checked using the "isValid()" routine.
     *  @param year new year value to be set
     *  @param month new month value to be set
     *  @param day new day value to be set
     *  @return OFTrue if the new value is valid and has been set, OFFalse otherwise
     */
    OFBool setDate(const unsigned int year,
                   const unsigned int month,
                   const unsigned int day);

    /** set the date value to the specified year.
     *  Before the new value is set it is checked using the "isValid()" routine.
     *  @param year new year value to be set
     *  @return OFTrue if the new value is valid and has been set, OFFalse otherwise
     */
    OFBool setYear(const unsigned int year);

    /** set the date value to the specified month.
     *  Before the new value is set it is checked using the "isValid()" routine.
     *  @param month new month value to be set
     *  @return OFTrue if the new value is valid and has been set, OFFalse otherwise
     */
    OFBool setMonth(const unsigned int month);

    /** set the date value to the specified day.
     *  Before the new value is set it is checked using the "isValid()" routine.
     *  @param day new day value to be set
     *  @return OFTrue if the new value is valid and has been set, OFFalse otherwise
     */
    OFBool setDay(const unsigned int day);

    /** set the date value to the current system date.
     *  This function uses operating system dependent routines. If they are unavailable
     *  for some reason the current value is not modified.
     *  @return OFTrue if the current system date has been set, OFFalse otherwise
     */
    OFBool setCurrentDate();

    /** get the currently stored year value
     *  @return year value (might be invalid, i.e. out of range)
     */
    unsigned int getYear() const;

    /** get the currently stored month value
     *  @return month value (might be invalid, i.e. out of range)
     */
    unsigned int getMonth() const;

    /** get the currently stored day value
     *  @return day value (might be invalid, i.e. out of range)
     */
    unsigned int getDay() const;

    /** get the current date value in ISO format.
     *  The two ISO date formats supported by this function are "YYYY-MM-DD" (with
     *  delimiters) and "YYYYMMDD" (without delimiters, useful for DICOM date type).
     *  @param formattedDate reference to string variable where the result is stored
     *  @param showDelimiter flag, indicating whether to use delimiters ("-") or not
     *  @return OFTrue if result variable has been set, OFFalse otherwise
     */
    OFBool getISOFormattedDate(OFString &formattedDate,
                               const OFBool showDelimiter = OFTrue) const;

    /* --- static helper functions --- */

    /** get the current system date
     *  @return date object with the current system date set
     */
    static OFDate getCurrentDate();


 protected:

    /** set the date value to the current system date.
     *  This function uses operating system dependent routines. If they are unavailable
     *  for some reason the current value is not modified.
     *  @param tt current system time (as returned by the time() function )
     *  @return OFTrue if the current system date has been set, OFFalse otherwise
     */
    OFBool setCurrentDate(const time_t &tt);

    /* --- static helper functions --- */

    /** check whether the given date is valid.
     *  Currently, this method only checks whether the month is within the range of 1 to 12
     *  and the day is within the range of 1 to 31. In the future this routine might be
     *  enhanced.
     *  @param year year value to be checked
     *  @param month month value to be checked
     *  @param day day value to be checked
     *  @return OFTrue if the current value is valid, OFFalse otherwise
     */
    static OFBool isDateValid(const unsigned int year,
                              const unsigned int month,
                              const unsigned int day);


 private:

    /// currently stored year value
    unsigned int Year;

    /// currently stored month value
    unsigned int Month;

    /// currently stored day value
    unsigned int Day;
};


/** put the given date in ISO format on the output stream.
 *  If an error occurs nothing is printed.
 *  @param stream output stream
 *  @param date OFDate object to print
 *  @return reference to the output stream
 */
ostream& operator<<(ostream& stream, const OFDate &date);


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: ofdate.h,v $
 * Revision 1.1  2002-04-11 12:12:23  joergr
 * Introduced new standard classes providing date and time functions.
 *
 *
 */
