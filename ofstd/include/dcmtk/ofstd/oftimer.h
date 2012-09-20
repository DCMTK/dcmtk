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
 *  Purpose: Class for measurement of time (Header)
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
 *  The output format is "<numeric value> <SI unit>" where "SI unit" is:
 *  - "ms" for milliseconds if the time difference is less than 1 second
 *  - "s" for seconds if the time difference is less than 60 seconds
 *  - "min" for minutes if the time difference is less than 3600 seconds
 *  - "h" for hours otherwise (i.e. the difference is 3600 seconds or more)
 *  @param stream output stream
 *  @param timer OFTimer object to print
 *  @return reference to the output stream
 */
DCMTK_OFSTD_EXPORT STD_NAMESPACE ostream &operator<<(STD_NAMESPACE ostream &stream, const OFTimer &timer);


#endif
