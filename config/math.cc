/*
 *
 *  Copyright (C) 2015-2021, OFFIS e.V.
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


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include <cfloat>
#include <cmath>
#include <climits>
#include <csignal>
#include <csetjmp>
#include <iostream>

#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofstream.h"

#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

/* Some MacOS X versions define isinf() and isnan() in <math.h> but not in <cmath> */
#if defined(__APPLE__) && defined(__MACH__) && !defined (__INTEL_COMPILER)
#undef HAVE_PROTOTYPE_ISINF
#undef HAVE_PROTOTYPE_ISNAN
#endif


// some systems don't properly define isnan()
#ifdef HAVE_ISNAN
#ifndef HAVE_PROTOTYPE_ISNAN
extern "C"
{
  int isnan(double value);
}
#endif
#endif


// some systems don't properly define finite()
#ifdef HAVE_FINITE
#ifndef HAVE_PROTOTYPE_FINITE
extern "C"
{
  int finite(double value);
}
#endif
#endif

#if !defined(HAVE_ISINF) && defined(HAVE_PROTOTYPE_ISINF)
#   define HAVE_ISINF 1
#endif

// some systems don't properly define isinf()
#ifdef HAVE_ISINF
#ifndef HAVE_PROTOTYPE_ISINF
extern "C"
{
  int isinf(double value);
}
#endif
#endif /* HAVE_ISINF */

struct dcmtk_config_math
{
  static inline OFBool isnan( float f )
  {
#ifdef HAVE_WINDOWS_H
    return _isnan(f) != 0;
#elif defined(HAVE_PROTOTYPE_STD__ISNAN)
    return STD_NAMESPACE isnan(f);
#else
    return ::isnan(f);
#endif
  }

  static inline OFBool isnan( double d )
  {
#ifdef HAVE_WINDOWS_H
    return _isnan(d) != 0;
#elif defined(HAVE_PROTOTYPE_STD__ISNAN)
    return STD_NAMESPACE isnan(d);
#else
    return ::isnan(d);
#endif
  }

  static inline OFBool isinf( float f )
  {
#ifdef HAVE_PROTOTYPE_STD__ISINF
    return STD_NAMESPACE isinf( f );
#elif defined(HAVE_ISINF)
    return ::isinf( f );
#else
    return dcmtk_config_math::isinf( OFstatic_cast( double, f ) );
#endif
  }

  static inline OFBool isinf( double d )
  {
#ifdef HAVE_PROTOTYPE_STD__ISINF
    return STD_NAMESPACE isinf( d );
#elif defined(HAVE_ISINF)
    return ::isinf( d );
#else
#ifdef HAVE_WINDOWS_H
    return (! _finite(d)) && (! _isnan(d));
#else
    // Solaris 2.5.1 has finite() and isnan() but not isinf().
    return (! finite(d)) && (! isnan(d));
#endif
#endif
  }
};
