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
 *  Purpose: Class for measurement of time (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2011-11-17 11:46:04 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oftimer.h"
#include "dcmtk/ofstd/ofcast.h"

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else /* UNIX */
#include <sys/time.h>
#endif


/*------------------*
 *  implementation  *
 *------------------*/

OFTimer::OFTimer()
  : Start(getTime())
{
}


void OFTimer::reset()
{
    Start = getTime();
}


double OFTimer::getDiff() const
{
    return getTime() - Start;
}


double OFTimer::getDiff(double start)
{
    return getTime() - start;
}


double OFTimer::getTime()
{
#ifdef HAVE_WINDOWS_H
    return OFstatic_cast(double, GetTickCount()) / 1000;
#else
    timeval c_time;
    gettimeofday(&c_time, NULL);
    return OFstatic_cast(double, c_time.tv_sec) + OFstatic_cast(double, c_time.tv_usec) / 1000000;
#endif
}


STD_NAMESPACE ostream &operator<<(STD_NAMESPACE ostream &stream, const OFTimer &timer)
{
    const double timeDiff = timer.getDiff();
    // output time difference in units depending on the value range
    if ((timeDiff < 1) && (timeDiff > -1))
        stream << (timeDiff * 1000) << " ms";
    else if ((timeDiff < 60) && (timeDiff > -60))
        stream << timeDiff << " s";
    else if ((timeDiff < 3600) && (timeDiff > -3600))
        stream << (timeDiff / 60) << " m";
    else
        stream << (timeDiff / 3600) << " h";
    return stream;
}


/*
 *
 * CVS/RCS Log:
 * $Log: oftimer.cc,v $
 * Revision 1.3  2011-11-17 11:46:04  joergr
 * Define WIN32_LEAN_AND_MEAN in order to avoid unneeded header file inclusions
 * caused by "windows.h".
 *
 * Revision 1.2  2011-10-10 09:25:58  joergr
 * Added output stream operator in order to use different time units depending
 * on the value range, i.e. "ms", "s", "m" and "h".
 *
 * Revision 1.1  2011-10-10 09:12:25  joergr
 * Moved implementation from header to source file in order to avoid unwanted
 * header file inclusion.
 *
 *
 */
