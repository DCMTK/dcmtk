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
 *  Update Date:      $Date: 2011-10-10 09:12:25 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oftimer.h"
#include "dcmtk/ofstd/ofcast.h"

#ifdef HAVE_WINDOWS_H
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


/*
 *
 * CVS/RCS Log:
 * $Log: oftimer.cc,v $
 * Revision 1.1  2011-10-10 09:12:25  joergr
 * Moved implementation from header to source file in order to avoid unwanted
 * header file inclusion.
 *
 *
 */
