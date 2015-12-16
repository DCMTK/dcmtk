#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/oflimits.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/oftraits.h"
#include "dcmtk/ofstd/ofmath.h"   // for isinf and isnan

template<typename T>
static void checkMinMax()
{
    const T max_plus_one( OFnumeric_limits<T>::max() + 1 );
    const T lowest_minus_one( OFnumeric_limits<T>::lowest() - 1 );
    OFCHECK
    (
        OFnumeric_limits<T>::max() >= max_plus_one ||
        ( OFnumeric_limits<T>::has_infinity && OFMath::isinf( max_plus_one ) )
    );
    OFCHECK
    (
        OFnumeric_limits<T>::lowest() <= lowest_minus_one ||
        ( OFnumeric_limits<T>::has_infinity && OFMath::isinf( lowest_minus_one ) )
    );
    OFCHECK( ( OFnumeric_limits<T>::lowest() == OFnumeric_limits<T>::min() ) || !OFnumeric_limits<T>::is_integer );
}

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
