/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-02-05 14:07:23 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/oftimer.h,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __OFTIMER_H
#define __OFTIMER_H

#include "osconfig.h"

#ifdef HAVE_WINDOWS_H
 #include "windows.h"
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

    OFTimer()
      : Start(getTime())
    {
    }
    
    inline void reset()
    {
        Start = getTime();
    }
    
    inline double getDiff() const
    {
        return getTime() - Start;
    }

    inline static double getDiff(double start)
    {
        return getTime() - start;
    }

    inline static double getTime()
    {
#ifdef HAVE_WINDOWS_H
        return (double)GetTickCount() / 1000;
#else /* tested on solaris */
        timeval c_time;
        gettimeofday(&c_time, NULL);
        return (double)c_time.tv_sec + (double)c_time.tv_usec / 1000000;
#endif
    }


 private:

    double Start; 
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: oftimer.h,v $
 * Revision 1.3  1999-02-05 14:07:23  joergr
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
