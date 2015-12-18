/*
 *
 *  Copyright (C) 2015, OFFIS e.V.
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
 *  Author:  Jan Schlamelcher
 *
 *  Purpose: Platform independent definition of basic functions declared
 *           in <math.h> resp. <cmath>.
 *
 */


#ifndef OFMATH_H
#define OFMATH_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"    /* for OFBool */
#include "dcmtk/ofstd/oftraits.h"   /* for OFenable_if, ... */

class DCMTK_OFSTD_EXPORT OFMath
{

public:

#ifndef DOXYGEN
    static OFBool (isnan) (float f);
    static OFBool (isnan) (double d);
    template<typename Integer>
    static inline OFTypename OFenable_if<OFis_integral<Integer>::value,OFBool>::type
    (isnan) ( const Integer i ) { return (isnan) ( OFstatic_cast( double, i ) ); }

    static OFBool (isinf) (float f);
    static OFBool (isinf) (double d);
    template<typename Integer>
    static inline OFTypename OFenable_if<OFis_integral<Integer>::value,OFBool>::type
    (isinf) ( const Integer i ) { return (isinf) ( OFstatic_cast( double, i ) ); }
#else
    /** Determines if the given floating point number is a not-a-number (NaN) value.
     *  @param f the floating point value to inspect.
     *  @return OFTrue if f is a NaN, OFFalse otherwise.
     */
    static OFBool isnan( float f );

    /** Determines if the given floating point number is a not-a-number (NaN) value.
     *  @param d the floating point value to inspect.
     *  @return OFTrue if d is a NaN, OFFalse otherwise.
     */
    static OFBool isnan( double d );

    /** Casts the argument to double and calls OFMath::isnan(double) on the result.
     *  @param i an integer, i.e. <kbd>OFis_integral<Integer>::value</kbd> equals <kbd>OFTrue</kbd>.
     *  @return OFMath::isnan(OFstatic_cast(double,i)).
     */
    template<typename Integer>
    static OFBool isnan( Integer i );

    /** Determines if the given floating point number is a positive or negative infinity.
     *  @param f the floating point value to inspect.
     *  @return OFTrue if f is infinite, OFFalse otherwise.
     */
    static OFBool isinf( float f );

    /** Determines if the given floating point number is a positive or negative infinity.
     *  @param d the floating point value to inspect.
     *  @return OFTrue if d is infinite, OFFalse otherwise.
     */
    static OFBool isinf( double d );

    /** Casts the argument to double and calls OFMath::isinf(double) on the result.
     *  @param i an integer, i.e. <kbd>OFis_integral<Integer>::value</kbd> equals <kbd>OFTrue</kbd>.
     *  @return OFMath::isinf(OFstatic_cast(double,i)).
     */
    template<typename Integer>
    static OFBool isinf( Integer i );
#endif
};

#endif // OFMATH_H
