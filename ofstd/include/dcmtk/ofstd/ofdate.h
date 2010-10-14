/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Purpose: Class for date functions
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:50 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef OFDATE_H
#define OFDATE_H

#include "dcmtk/config/osconfig.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>    /* for struct time_t */
#endif
END_EXTERN_C

#include "dcmtk/ofstd/ofstring.h"      /* for class OFString */
#include "dcmtk/ofstd/ofstream.h"      /* for ostream */


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

    /** default constructor.
     *  Initializes Year, Month and Day to 0.
     */
    OFDate();

    /** copy constructor
     *  @param dateVal date object to be copied
     */
    OFDate(const OFDate &dateVal);

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
     *  @param dateVal date value to be set
     *  @return reference to this object (with new value)
     */
    virtual OFDate &operator=(const OFDate &dateVal);

    /** comparison operator (equal)
     *  @param dateVal date value compared with the current value
     *  @return OFTrue if given date is equal, OFFalse otherwise
     */
    virtual OFBool operator==(const OFDate &dateVal) const;

    /** comparison operator (unequal)
     *  @param dateVal date value compared with the current value
     *  @return OFTrue if given date is unequal, OFFalse otherwise
     */
    virtual OFBool operator!=(const OFDate &dateVal) const;

    /** comparison operator (less than)
     *  @param dateVal date value compared with the current value
     *  @return OFTrue if current date is earlier than the given value,
     *    OFFalse otherwise
     */
    virtual OFBool operator<(const OFDate &dateVal) const;

    /** comparison operator (less than or equal)
     *  @param dateVal date value compared with the current value
     *  @return OFTrue if current date is earlier than or identical to the given value,
     *    OFFalse otherwise
     */
    virtual OFBool operator<=(const OFDate &dateVal) const;

    /** comparison operator (greater than or equal)
     *  @param dateVal date value compared with the current value
     *  @return OFTrue if current date is later than or identical to the given value,
     *    OFFalse otherwise
     */
    virtual OFBool operator>=(const OFDate &dateVal) const;

    /** comparison operator (greater than)
     *  @param dateVal date value compared with the current value
     *  @return OFTrue if current date is later than the given value,
     *    OFFalse otherwise
     */
    virtual OFBool operator>(const OFDate &dateVal) const;

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

    /** set the date value to the given ISO formatted date string.
     *  The two ISO date formats supported by this function are "YYYY-MM-DD" (with arbitrary
     *  delimiters) and "YYYYMMDD" (without delimiters, useful for DICOM date type).
     *  @param formattedDate ISO formatted date value to be set
     *  @return OFTrue if input is valid and result variable has been set, OFFalse otherwise
     */
    OFBool setISOFormattedDate(const OFString &formattedDate);

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
 *  @param dateVal OFDate object to print
 *  @return reference to the output stream
 */
STD_NAMESPACE ostream& operator<<(STD_NAMESPACE ostream& stream, const OFDate &dateVal);


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: ofdate.h,v $
 * Revision 1.9  2010-10-14 13:15:50  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.8  2006/08/14 16:42:26  meichel
 * Updated all code in module ofstd to correctly compile if the standard
 *   namespace has not included into the global one with a "using" directive.
 *
 * Revision 1.7  2005/12/08 16:05:54  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.6  2004/01/16 10:30:39  joergr
 * Added setISOFormattedXXX() methods for Date, Time and DateTime.
 *
 * Revision 1.5  2003/09/15 12:12:56  joergr
 * Fixed incorrect/improper comments of the comparision operators. Enhanced
 * comment of the default constructor. Made comparison operators const.
 *
 * Revision 1.4  2003/07/04 13:30:37  meichel
 * Added include for ofstream.h, to make sure ofstream is correctly defined
 *
 * Revision 1.3  2002/05/24 09:43:04  joergr
 * Renamed some parameters/variables to avoid ambiguities.
 *
 * Revision 1.2  2002/04/15 09:38:58  joergr
 * Added "include <sys/types.h>" for struct time_t (required for MSVC).
 *
 * Revision 1.1  2002/04/11 12:12:23  joergr
 * Introduced new standard classes providing date and time functions.
 *
 *
 */
