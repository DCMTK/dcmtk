/*
 *
 *  Copyright (C) 1997-2005, OFFIS
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
 *  Purpose: Class for measurement of time (Header)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-12-08 16:06:10 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef OFTIMER_H
#define OFTIMER_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcast.h"

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#else /* UNIX */
#include <sys/time.h>
#endif


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** A class for measurement of time.
 *  Timer intervalls are represented as floating point values of seconds.
 */
class OFTimer
{

 public:

    /** constructor
     */
    OFTimer()
      : Start(getTime())
    {
    }
    
    /** reset start time
     */
    inline void reset()
    {
        Start = getTime();
    }
    
    /** get elapsed time.
     *  i.e. difference between current and start time
     *
     ** @return elapsed time in seconds
     */
    inline double getDiff() const
    {
        return getTime() - Start;
    }

    /** get difference between current time and specified time
     *
     ** @param  start  reference time (in seconds)
     *
     ** @return difference between the two times (in seconds)
     */
    inline static double getDiff(double start)
    {
        return getTime() - start;
    }

    /** get current time
     *
     ** @return current time in seconds
     */
    inline static double getTime()
    {
#ifdef HAVE_WINDOWS_H
        return OFstatic_cast(double, GetTickCount()) / 1000;
#else /* tested on solaris */
        timeval c_time;
        gettimeofday(&c_time, NULL);
        return OFstatic_cast(double, c_time.tv_sec) + OFstatic_cast(double, c_time.tv_usec) / 1000000;
#endif
    }


 private:

    /// reference/start time
    double Start; 
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: oftimer.h,v $
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
