/*
 *
 *  Copyright (C) 2014-2024, OFFIS e.V.
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
 *  Purpose: unit test for OFnumeric_limits
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofdiag.h"
#include "dcmtk/ofstd/oflimits.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/oftraits.h"
#include "dcmtk/ofstd/ofmath.h"   // for isinf and isnan

// On some platforms, such as OpenIndiana, isinf is defined as a macro,
// and that interferes with the OFMath function of the same name.
#ifdef isinf
#undef isinf
#endif

#include DCMTK_DIAGNOSTIC_PUSH
#include DCMTK_DIAGNOSTIC_IGNORE_OVERFLOW
#include DCMTK_DIAGNOSTIC_IGNORE_IMPLICIT_CONVERSION
#include DCMTK_DIAGNOSTIC_IGNORE_UNSIGNED_UNARY_MINUS
template<typename T>
static void checkMinMax()
{
    // we need to declare some variables as volatile to prevent the
    // Clang compiler from optimizing away everything, leading to a test failure:wq

    volatile T max_( OFnumeric_limits<T>::max() );
    volatile T lowest;
    if (OFnumeric_limits<T>::is_integer) lowest = OFnumeric_limits<T>::min(); else lowest = -OFnumeric_limits<T>::max();
    volatile T max_plus_one = max_ + 1;
    volatile T lowest_minus_one = lowest - 1;
    OFCHECK
    (
        OFnumeric_limits<T>::max() >= max_plus_one ||
        ( OFnumeric_limits<T>::has_infinity && OFMath::isinf( max_plus_one ) )
    );
    OFCHECK
    (
        lowest <= lowest_minus_one ||
        ( OFnumeric_limits<T>::has_infinity && OFMath::isinf( lowest_minus_one ) )
    );
}
#include DCMTK_DIAGNOSTIC_POP

template<typename T>
static OFTypename OFenable_if<OFnumeric_limits<T>::has_quiet_NaN>::type checkNaN()
{
    (OFMath::isnan)( OFnumeric_limits<T>::quiet_NaN() );
}

template<typename T>
static OFTypename OFenable_if<!OFnumeric_limits<T>::has_quiet_NaN>::type checkNaN()
{

}

template<typename T>
static OFTypename OFenable_if<OFnumeric_limits<T>::has_infinity>::type checkInfinity()
{
    (OFMath::isinf)( OFnumeric_limits<T>::infinity() );
}

template<typename T>
static OFTypename OFenable_if<!OFnumeric_limits<T>::has_infinity>::type checkInfinity()
{

}

#include DCMTK_DIAGNOSTIC_PUSH
#include DCMTK_DIAGNOSTIC_IGNORE_OVERFLOW
template<typename T>
static void checkLimits()
{
    checkMinMax<T>();
    checkNaN<T>();
    checkInfinity<T>();
    // Only test overflow property if 'is_modulo' expands to OFTrue, since OFFalse may
    // also mean 'undefined'.
    if( OFnumeric_limits<T>::is_modulo )
        OFCHECK( OFstatic_cast( T, OFnumeric_limits<T>::max() + 1 ) == OFnumeric_limits<T>::min() );
}
#include DCMTK_DIAGNOSTIC_POP

OFTEST(ofstd_limits)
{
    checkLimits<char>();
    checkLimits<signed char>();
    checkLimits<unsigned char>();
    checkLimits<signed short>();
    checkLimits<unsigned short>();
    checkLimits<signed int>();
    checkLimits<unsigned int>();
    checkLimits<signed long>();
    checkLimits<unsigned long>();
    checkLimits<float>();
    checkLimits<double>();
}
