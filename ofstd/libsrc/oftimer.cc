/*
 *
 *  Copyright (C) 1999-2012, OFFIS e.V.
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
    // according to MSDN: "The resolution of the GetTickCount function is limited to the resolution
    // of the system timer, which is typically in the range of 10 milliseconds to 16 milliseconds."
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
        stream << (timeDiff / 60) << " min";
    else
        stream << (timeDiff / 3600) << " h";
    return stream;
}
