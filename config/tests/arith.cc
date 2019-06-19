/*
 *
 *  Copyright (C) 2014-2019, OFFIS e.V.
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
 *  Module:  config
 *
 *  Author:  Jan Schlamelcher
 *
 *  Purpose: Query properties of fundamental arithmetic types
 *           for being used within oflimits.h.
 */

// Note: This depends on some files of ofstd and osconfig.h,
//       although it is part of configure testing itself.
//       Therefore, ensure osconfig.h has already been generated
//       before this program is used.
#define INCLUDE_CLIMITS
#define INCLUDE_CMATH
#define INCLUDE_CSETJMP
#define INCLUDE_CSIGNAL
#include "../math.cc"

#ifdef HAVE_FENV_H
// For controlling floating point exceptions on Unix like systems.
#include <fenv.h>
#endif

#ifdef HAVE_IEEEFP_H
// For controlling floating point exceptions on Unix like systems
// that don't have feenableexcept and fedisableexcept.
#include <ieeefp.h>
#endif

#ifdef __APPLE__
// For controlling floating point exceptions on OS X.
#include <xmmintrin.h>
#endif

#ifdef HAVE_SETJMP_H
// For setjmp for the SunPro compiler
#include <setjmp.h>
#endif

#ifdef HAVE_SIGNAL_H
// For signal for the SunPro compiler
#include <signal.h>
#endif

// hackish definition of cout, as we can't depend on
// OFConsole.
#define COUT STD_NAMESPACE cout

// define sigjmp_buf if it isn't already
#ifndef HAVE_SIGJMP_BUF
typedef jmp_buf sigjmp_buf;
#endif

// longjmp env (jump destination), ugly global var we
// need to recover from traps.
sigjmp_buf jbuf;

// signal handler that "jumps back in time" to before
// the error occurred and fixes what has gone wrong
// in 1985.
static void capture( int sig )
{
#ifndef _WIN32
    // Unix like systems support this natively
    siglongjmp( jbuf, sig );
#else
    // On Windows, we need to employ a workaround that
    // only works for floating point exceptions!
    _fpreset();
    longjmp( jbuf, sig );
#endif
}

static void register_signals()
{
    signal(SIGABRT, &capture);
    signal(SIGFPE,  &capture);
    signal(SIGILL,  &capture);
    signal(SIGINT,  &capture);
    signal(SIGSEGV, &capture);
    signal(SIGTERM, &capture);
}

// print to "arith.h"
static void print_define( STD_NAMESPACE ostream& out,
                          const char* fname, const char* name )
{
    out << "#define DCMTK_";
    // convert type name to uppercase and replace ' ' by underscore.
    for( const char* pch = name; *pch; ++pch )
        out.put( *pch <= 'z' && *pch >= 'a' ? *pch - 'a' + 'A' : *pch == ' ' ? '_' : *pch );
    out << '_' << fname;
}

// handles output to configure log and to "arith.h"
static int print_flag( STD_NAMESPACE ostream& out, int flag,
                       const char* fname, const char* name,
                       int width )
{
    COUT << STD_NAMESPACE setw(width) << STD_NAMESPACE internal << ( flag ? "yes" : "no" );
    print_define( out, fname, name );
    out << " OF" << ( flag ? "True" : "False" ) << '\n';
    return flag;
}

static const char all_one[8] = { '\377', '\377', '\377', '\377', '\377', '\377', '\377', '\377' };

template<typename T>
static int test_modulo()
{
    // test if signed, otherwise it is modulo anyway
    if( *OFreinterpret_cast( const T*, all_one ) > 0 )
        return 1;
    const T max = *OFreinterpret_cast( const T*, all_one ) & ~(T(1) << (sizeof(T) * 8 - 1));
    const T min = T( max + 1 );
    return min < max && min < 0 && max > 0;
}

template<typename FN>
static int test_trap( const FN& fn )
{
    register_signals();
#ifndef _WIN32
    // On Unix like systems, we use the
    // longjmp-module specifically designed for
    // signals.
    if( sigsetjmp( jbuf, 1 ) )
#else
    // On Windows, we use the normal longjmp
    // that destroys everything and try to fix
    // what is left.
    if( setjmp( jbuf ) )
#endif
    {
        // if a jump occurred, this means fn()
        // caused a trap.
        return 1;
    }
#ifdef _MSC_VER
    // On Visual Studio, we use their built-in
    // SEH things, they consider to be C++.
    __try
#else
    // The rest of the world hopefully only
    // has normal exceptions.
    try
#endif
    {
        // Call the function. Did it trap?
        fn();
    }
#ifdef _MSC_VER
    // On Windows, we return 3 if any SEH exception
    // occurred. Shall we consider this a trap,
    // although it was really an exception?
    // I don't know, for now it's yes.
    __except(1){return 3;}
#else
    // The rest of the world returns 2, which
    // means "normal exception".
    catch(...){return 2;}
#endif
    // Nothing happened, so we return 0, for
    // "nothing happened". Note: since MinGW's
    // crazy SEH things are crazy, MinGW will always
    // execute what is written in the brackets following
    // __except1, and NEVER see this return statement.
    return 0;
}

template<typename T>
static void divide_by_zero()
{
    // Note: we can't just do 1 / 0, since
    //       some compilers might optimize that
    //       away or emit compile-time errors.
    volatile T t0 = 1;
    volatile T t1 = 0;
    t0 /= t1;
}

// gathers and prints information for integer types
template<typename T>
static void inspect( STD_NAMESPACE ostream& out, const char* name )
{
    COUT << "--   " << STD_NAMESPACE left << STD_NAMESPACE setw(14) << name;
    print_flag( out, test_trap( &divide_by_zero<T> ), "TRAPS", name, 7 );
    print_flag( out, test_modulo<T>(), "MODULO", name, 7 );
    COUT << OFendl;
}

// print a constant's binary representation as a string, so it can be loaded
// instead of being calculated when needed
template<typename T>
static void print_binary( STD_NAMESPACE ostream& out, const char* pname, const char* name, const T& t )
{
    print_define( out, pname, name );
    out << " *OFreinterpret_cast( const " << name << "*, \"";
    for( unsigned ui = 0; ui < sizeof(T); ++ui )
        out << '\\' << STD_NAMESPACE oct << STD_NAMESPACE setw(3) << STD_NAMESPACE right << STD_NAMESPACE setfill( '0' )
            << unsigned(((unsigned char*)&t)[ui]) << STD_NAMESPACE dec;
    out << "\" )\n";
}

// try to create an infinite value and use whatever means
// to detect if it really is infinite
template<typename T>
static int test_inf( STD_NAMESPACE ostream& out, const char* name )
{
    const T t( HUGE_VAL );
    const int has_inf = print_flag
    (
        out,
        dcmtk_config_math::isinf(t),
        "HAS_INFINITY",
        name,
        7
    );
    print_binary( out, "INFINITY", name, t );
    return has_inf;
}

template<typename T>
static T guess_qnan()
{
#ifdef NAN
    // use NAN if available
    return T( NAN );
#else
    // else do a barrel roll
    T t( 0.0 );
    return t / t;
#endif
}

template<typename T>
static int test_qnan( STD_NAMESPACE ostream& out, const char* name )
{
    const T t = guess_qnan<T>();
    const int has_qnan = print_flag
    (
        out,
        dcmtk_config_math::isnan( t ),
        "HAS_QUIET_NAN",
        name,
        7
    );
    print_binary( out, "QUIET_NAN", name, t );
    return has_qnan;
}

template<typename T>
struct guess;

template<>
struct guess<float>
{
    // There is no way to create a signaling NaN employing only arithmetic
    // operations. So we assume we have IEC-559 floating point types
    // and convert the quiet NaNs we created to signaling NaNs by flipping
    // the bits that would need to be flipped if it really were IEC-559 floats.
    static const float& snan()
    {
        static float f = guess_qnan<float>();
        ((unsigned char*)&f)[2] &= 0xbf;
        ((unsigned char*)&f)[0] |= 0x01;
        return f;
    }

    static float denorm_min()
    {
        return float( ldexp( 1.0f, FLT_MIN_EXP - FLT_MANT_DIG ) );
    }

    static float denorm_max()
    {
        return FLT_MIN / 2;
    }

    static float epsilon()
    {
        return FLT_EPSILON;
    }
};

template<>
struct guess<double>
{
    // There is no way to create a signaling NaN employing only arithmetic
    // operations. So we assume we have IEC-559 floating point types
    // and convert the quiet NaNs we created to signaling NaNs by flipping
    // the bits that would need to be flipped if it really were IEC-559 floats.
    static const double& snan()
    {
        static double d = guess_qnan<double>();
        ((unsigned char*)&d)[6] &= 0xf7;
        ((unsigned char*)&d)[0] |= 0x01;
        return d;
    }

    static double denorm_min()
    {
        return double( ldexp( 1.0, DBL_MIN_EXP - DBL_MANT_DIG ) );
    }

    static double denorm_max()
    {
        return DBL_MIN / 2;
    }

    static double epsilon()
    {
        return DBL_EPSILON;
    }
};

template<typename T>
static void provoke_snan()
{
    // Enable floating point exceptions, Windows and Unix version.
#ifdef HAVE_WINDOWS_H
    _clearfp();
    _controlfp( _controlfp(0,0) & ~_EM_INVALID, _MCW_EM );
#elif defined(__APPLE__)
    _MM_SET_EXCEPTION_MASK( _MM_GET_EXCEPTION_MASK() & ~_MM_MASK_INVALID );
#elif defined(HAVE_FENV_H) && defined(HAVE_PROTOTYPE_FEENABLEEXCEPT)
    feenableexcept( FE_INVALID );
#elif defined(HAVE_IEEEFP_H)
    fp_except cw = fpgetmask();

#ifdef FP_X_DX
    // on some systems such as CygWin, the devide-by-zero flag is called FP_X_DX
    fpsetmask(cw | FP_X_INV | FP_X_DX | FP_X_OFL);
#else
    fpsetmask(cw | FP_X_INV | FP_X_DZ | FP_X_OFL);
#endif
#endif
    // Visual Studio will emit an exception the moment
    // we assign a signaling NaN to another float variable
    // and convert it to a quiet NaN.
    volatile T t = guess<T>::snan();
    // Other compilers will trigger only if we use
    // arithmetics.
    ++t;
}

template<typename T>
static int test_snan( STD_NAMESPACE ostream& out, const char* name )
{
    // Create signaling NaNs and test if they really are signaling NaNs
    const T& t = guess<T>::snan();
    int flag = dcmtk_config_math::isnan( t ) && test_trap( &provoke_snan<T> );
    // Disable floating point exceptions that have been enabled for the test,
    // Windows and Unix version.
#ifdef HAVE_WINDOWS_H
    _clearfp();
    _controlfp( _controlfp(0,0) | _EM_INVALID, _MCW_EM );
#elif defined(HAVE_FENV_H)
    feclearexcept( FE_INVALID );
#ifdef __APPLE__
    _MM_SET_EXCEPTION_MASK( _MM_GET_EXCEPTION_MASK() | _MM_MASK_INVALID );
#elif defined(HAVE_FENV_H) && defined(HAVE_PROTOTYPE_FEENABLEEXCEPT)
    fedisableexcept( FE_INVALID );
#elif defined(HAVE_IEEEFP_H)
    fp_except cw = fpgetmask();

#ifdef FP_X_DX
    // on some systems such as CygWin, the devide-by-zero flag is called FP_X_DX
    fpsetmask(cw & ~(FP_X_INV | FP_X_DX | FP_X_OFL));
#else
    fpsetmask(cw & ~(FP_X_INV | FP_X_DZ | FP_X_OFL));
#endif

#endif
#endif
    // Print and return the result
    print_flag
    (
        out,
        flag,
        "HAS_SIGNALING_NAN",
        name,
        7
    );
    print_binary( out, "SIGNALING_NAN", name, t );
    return flag;
}

template<typename T>
static void test_iec559( STD_NAMESPACE ostream& out, const char* name, int reqs )
{
    // Assume we have IEC-559 floats if the appropriate macro was defined,
    // or if infinity and both kinds of NaNs are supported and the characteristics
    // of double are what we expect them to be.
    print_flag
    (
        out,
#if defined(__STDC_IEC_559__) || (DBL_DIG == 15 && DBL_MANT_DIG == 53 && DBL_MAX_10_EXP == 308 && DBL_MAX_EXP == 1024 && DBL_MIN_10_EXP == -307 && DBL_MIN_EXP == -1021)
        reqs,
#else
        0,
#endif
        "IS_IEC559",
        name,
        7
    );
}

static int is_subnormal( float f )
{
// Wine does not implement _fpclassf, so don't use it when cross compiling for Windows
// (Wine would emit an exception at runtime, so it's hard to test for _fpclassf availability)
#if defined(HAVE_PROTOTYPE__FPCLASSF) && !( defined(DCMTK_CROSS_COMPILING) && defined(_WIN32) )
    return ( _fpclassf( f ) & ( _FPCLASS_ND | _FPCLASS_PD ) ) != 0;
#else
    return f != 0.0f && ( f < 0.0f ? -f < FLT_MIN : f < FLT_MIN );
#endif
}

static int is_subnormal( double d )
{
#ifdef _WIN32
    return ( _fpclass( d ) & ( _FPCLASS_ND | _FPCLASS_PD ) ) != 0;
#else
    return d != 0.0 && ( d < 0.0 ? -d < DBL_MIN : d < DBL_MIN );
#endif
}

template<typename T>
static void test_subnormal( STD_NAMESPACE ostream& out, const char* name )
{
#ifdef HAVE_WINDOWS_H
    _clearfp();
    _controlfp( _EM_DENORMAL | _EM_UNDERFLOW | _EM_INEXACT, _MCW_EM );
#endif
    const T denorm = guess<T>::denorm_min();
    const int flag = is_subnormal( denorm );
    COUT << STD_NAMESPACE setw(7) << STD_NAMESPACE internal << ( flag ? "yes" : "no" );
    print_define( out, "HAS_DENORM", name );
    out << " OFdenorm_" << ( flag ? "pre" : "ab" ) << "sent\n";
    print_binary( out, "DENORM_MIN", name, denorm );
}

template<typename T>
static void test_tinyness_before( STD_NAMESPACE ostream& out, const char* name )
{
    volatile const T denorm_max = guess<T>::denorm_max();
    volatile const T mult = T( 1.0 ) + guess<T>::epsilon();

#ifdef HAVE_WINDOWS_H
    _clearfp();
#elif defined(HAVE_FENV_H)
    feclearexcept( FE_ALL_EXCEPT );
#endif

    volatile T result = denorm_max * mult;
    (void)result;

    print_flag
    (
        out,
#ifdef HAVE_WINDOWS_H
        _statusfp() & _EM_UNDERFLOW,
#elif defined(HAVE_FENV_H)
        fetestexcept( FE_UNDERFLOW ),
#else
        0,
#endif
        "TINYNESS_BEFORE",
        name,
        7
    );
}

template<typename T>
static void test_denorm_loss( STD_NAMESPACE ostream& out, const char* name )
{
    volatile T t = guess<T>::denorm_min() * 3;

#ifdef HAVE_WINDOWS_H
    _clearfp();
#elif defined(HAVE_FENV_H)
    feclearexcept( FE_ALL_EXCEPT );
#endif

    t /= 2;

    print_flag
    (
        out,
#ifdef HAVE_WINDOWS_H
        _statusfp() & _EM_UNDERFLOW,
#elif defined(HAVE_FENV_H)
        fetestexcept( FE_UNDERFLOW ),
#else
        0,
#endif
        "HAS_DENORM_LOSS",
        name,
        7
    );
}

#ifdef HAVE_WINDOWS_H
// MinGW's crazy SEH stuff doesn't work on all MinGW "distributions"
// This is an even more ugly fallback implementation that seems to
// work for now
LONG WINAPI consume_seh_problems( struct _EXCEPTION_POINTERS* )
{
    _fpreset();
    _clearfp();
    longjmp( jbuf, 0 );
    return EXCEPTION_CONTINUE_SEARCH;
}
#endif

int main( int argc, char** argv )
{
#ifdef HAVE_WINDOWS_H
    // Activate the fallback workaround, it will only be used
    // if the SEH exceptions can't be caught "the right way"
    SetErrorMode( SEM_FAILCRITICALERRORS );
    SetUnhandledExceptionFilter( consume_seh_problems );
#endif

    COUT << "Inspecting fundamental arithmetic types... " << OFendl;
    if( argc != 2 )
    {
        STD_NAMESPACE cerr << "--   " << "Error: missing destination file "
                           << "to store collected information." << OFendl;
        return 1;
    }

    STD_NAMESPACE ofstream out( argv[1] );

    out << "#ifndef CONFIG_ARITH_H" << '\n';
    out << "#define CONFIG_ARITH_H" << '\n';
    out << '\n';

    // workaround to let digits10 be a compile time constant
    out << "#define DCMTK_SIGNED_CHAR_DIGITS10 " << OFstatic_cast( int, ( CHAR_BIT - 1 ) * .30102999566398119521373889472449 ) << OFendl;
    out << "#define DCMTK_UNSIGNED_CHAR_DIGITS10 " << OFstatic_cast( int, CHAR_BIT * .30102999566398119521373889472449 ) << OFendl;
    out << "#define DCMTK_SIGNED_SHORT_DIGITS10 " << OFstatic_cast( int, ( CHAR_BIT * sizeof( signed short ) - 1 ) * .30102999566398119521373889472449 ) << OFendl;
    out << "#define DCMTK_UNSIGNED_SHORT_DIGITS10 " << OFstatic_cast( int, CHAR_BIT * sizeof( unsigned short ) * .30102999566398119521373889472449 ) << OFendl;
    out << "#define DCMTK_SIGNED_INT_DIGITS10 " << OFstatic_cast( int, ( CHAR_BIT * sizeof( signed int ) - 1 ) * .30102999566398119521373889472449 ) << OFendl;
    out << "#define DCMTK_UNSIGNED_INT_DIGITS10 " << OFstatic_cast( int, CHAR_BIT * sizeof( unsigned int ) * .30102999566398119521373889472449 ) << OFendl;
    out << "#define DCMTK_SIGNED_LONG_DIGITS10 " << OFstatic_cast( int, ( CHAR_BIT * sizeof( signed long ) - 1 ) * .30102999566398119521373889472449 ) << OFendl;
    out << "#define DCMTK_UNSIGNED_LONG_DIGITS10 " << OFstatic_cast( int, CHAR_BIT * sizeof( unsigned long ) * .30102999566398119521373889472449 ) << OFendl;
    out << "#define DCMTK_FLOAT_MAX_DIGITS10 " << OFstatic_cast( int, FLT_MANT_DIG * .30102999566398119521373889472449 + 2 ) << OFendl;
    out << "#define DCMTK_DOUBLE_MAX_DIGITS10 " << OFstatic_cast( int, DBL_MANT_DIG * .30102999566398119521373889472449 + 2 ) << OFendl;

    COUT << "--" << OFendl;

    COUT << "-- " << STD_NAMESPACE setfill( ' ' ) << STD_NAMESPACE setw(17) << ' '
              << STD_NAMESPACE setw(7) << STD_NAMESPACE internal << "TRAPS"
              << STD_NAMESPACE setw(7) << STD_NAMESPACE internal << "MODULO" << OFendl;

    inspect<char>( out, "char" );
    inspect<signed char>( out, "signed char" );
    inspect<unsigned char>( out, "unsigned char" );
    inspect<signed short>( out, "signed short" );
    inspect<unsigned short>( out, "unsigned short" );
    inspect<signed int>( out, "signed int" );
    inspect<unsigned int>( out, "unsigned int" );
    inspect<signed long>( out, "signed long" );
    inspect<unsigned long>( out, "unsigned long" );

    COUT << "--" << OFendl;

    COUT << "-- " << STD_NAMESPACE setfill( ' ' ) << STD_NAMESPACE setw(18) << ' '
              << STD_NAMESPACE setw(7) << STD_NAMESPACE internal << "float"
              << STD_NAMESPACE setw(7) << STD_NAMESPACE internal << "double" << OFendl;

    COUT << STD_NAMESPACE setw(20) << STD_NAMESPACE left << "--   TRAPS";
    print_flag( out, test_trap( &divide_by_zero<float> ), "TRAPS", "float", 7 );
    print_flag( out, test_trap( &divide_by_zero<double> ), "TRAPS", "double", 7 );
    COUT << OFendl;

    COUT << STD_NAMESPACE setw(20) << STD_NAMESPACE left << "--   HAS INFINITY";
    const int finf = test_inf<float>( out, "float" );
    const int dinf = test_inf<double>( out, "double" );
    COUT << OFendl;

    COUT << STD_NAMESPACE setw(20) << STD_NAMESPACE left << "--   QUIET NAN";
    const int fqnan = test_qnan<float>( out, "float" );
    const int dqnan = test_qnan<double>( out, "double" );
    COUT << OFendl;

    COUT << STD_NAMESPACE setw(20) << STD_NAMESPACE left << "--   SIGNALING NAN";
    const int fsnan = test_snan<float>( out, "float" );
    const int dsnan = test_snan<double>( out, "double" );
    COUT << OFendl;

    COUT << STD_NAMESPACE setw(20) << STD_NAMESPACE left << "--   IEC-559";
    test_iec559<float>( out, "float", finf && fqnan && fsnan );
    test_iec559<double>( out, "double", dinf && dqnan && dsnan );
    COUT << OFendl;

    COUT << STD_NAMESPACE setw(20) << STD_NAMESPACE left << "--   HAS DENORM";
    test_subnormal<float>( out, "float" );
    test_subnormal<double>( out, "double" );
    COUT << OFendl;

    COUT << STD_NAMESPACE setw(20) << STD_NAMESPACE left << "--   TINYNESS BEFORE";
    test_tinyness_before<float>( out, "float" );
    test_tinyness_before<double>( out, "double" );
    COUT << OFendl;

    COUT << STD_NAMESPACE setw(20) << STD_NAMESPACE left << "--   DENORM LOSS";
    test_denorm_loss<float>( out, "float" );
    test_denorm_loss<double>( out, "double" );
    COUT << OFendl;

    COUT << "--";

    out << "#define DCMTK_ROUND_STYLE " << FLT_ROUNDS << '\n';
    out << '\n';
    out << "#endif // CONFIG_ARITH_H" << '\n';

    return 0;
}
