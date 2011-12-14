/*
 *
 *  Copyright (C) 1999-2011, OFFIS e.V.
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
 *  Purpose: Class for measurement of time (Header)
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2011-12-14 08:54:01 $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef OFTIMER_H
#define OFTIMER_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofdefine.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** A class for measurement of time.
 *  Timer intervals are represented as floating point values of seconds.
 */
class DCMTK_OFSTD_EXPORT OFTimer
{

 public:

    /** constructor
     */
    OFTimer();

    /** reset start time
     */
    void reset();

    /** get elapsed time.
     *  i.e. difference between current and start time
     *
     ** @return elapsed time in seconds
     */
    double getDiff() const;

    /** get difference between current time and specified time
     *
     ** @param  start  reference time (in seconds)
     *
     ** @return difference between the two times (in seconds)
     */
    static double getDiff(double start);

    /** get current time
     *
     ** @return current time in seconds
     */
    static double getTime();


 private:

    /// reference/start time
    double Start;
};


/** output the current time difference to the given stream.
 *  The output format is "<numeric value> <unit>" where "unit" is:
 *  - "ms" for milliseconds if the time difference is less than 1 second
 *  - "s" for seconds if the time difference is less than 60 seconds
 *  - "m" for minutes if the time difference is less than 3600 seconds
 *  - "h" for hours otherwise (i.e. the difference is 3600 seconds or more)
 *  @param stream output stream
 *  @param timer OFTimer object to print
 *  @return reference to the output stream
 */
DCMTK_OFSTD_EXPORT STD_NAMESPACE ostream &operator<<(STD_NAMESPACE ostream &stream, const OFTimer &timer);


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: oftimer.h,v $
 * Revision 1.16  2011-12-14 08:54:01  uli
 * Make it possible to correctly build ofstd as a DLL.
 *
 * Revision 1.15  2011-10-10 09:25:56  joergr
 * Added output stream operator in order to use different time units depending
 * on the value range, i.e. "ms", "s", "m" and "h".
 *
 * Revision 1.14  2011-10-10 09:12:23  joergr
 * Moved implementation from header to source file in order to avoid unwanted
 * header file inclusion.
 *
 * Revision 1.13  2010-10-14 13:15:50  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.12  2010-03-01 09:08:51  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.11  2005-12-08 16:06:10  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.10  2003/12/05 10:37:41  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright date where appropriate.
 *
 * Revision 1.9  2003/07/09 13:57:43  meichel
 * Adapted type casts to new-style typecast operators defined in ofcast.h
 *
 * Revision 1.8  2001/06/01 15:51:36  meichel
 * Updated copyright header
 *
 * Revision 1.7  2000/03/08 16:36:03  meichel
 * Updated copyright header.
 *
 * Revision 1.6  2000/02/02 10:56:25  joergr
 * Removed space characters before preprocessor directives.
 *
 * Revision 1.5  1999/04/29 13:45:00  joergr
 * Added DOC++ comments.
 *
 * Revision 1.4  1999/04/21 13:01:44  meichel
 * ofstd/include/oftimer.h
 *
 * Revision 1.3  1999/02/05 14:07:23  joergr
 * Introduced new preprocessor definition HAVE_WINDOWS_H.
 *
 * Revision 1.2  1999/01/20 15:56:12  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options).
 *
 * Revision 1.1  1999/01/20 14:27:02  joergr
 * Added class for measurement of time.
 *
 *
 */
