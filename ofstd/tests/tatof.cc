/*
 *
 *  Copyright (C) 1997-2024, OFFIS e.V.
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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Test code of ascii/double conversion methods in class OFStandard
 *
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/oftypes.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"

#include <cmath>
#include <clocale>

#if !defined(ENABLE_OLD_OFSTD_ATOF_IMPLEMENTATION) && !defined(ENABLE_IOSTREAM_BASED_ATOF_IMPLEMENTATION) && !defined(ENABLE_CSTDIO_BASED_ATOF_IMPLEMENTATION)
#ifdef _WIN32
// on Windows, the iostream-based implementation of atof is extremely slow,
// and we do have a locale independent version of sscanf. Use this version.
#define ENABLE_CSTDIO_BASED_ATOF_IMPLEMENTATION
#else
// on other platforms, we assume that the iobased-implementation, being the
// cleanest one, is appropriate. This is known to be the case for gcc and clang with glibc.
#define ENABLE_IOSTREAM_BASED_ATOF_IMPLEMENTATION
#endif
#endif

struct ValuePair
{
  const char *s;
  double d;
  OFBool r;
};

const ValuePair vp[] =
{
  // normal conversions
  {"0", 0.0, OFTrue},
  {"1", 1.0, OFTrue},
  {"     1", 1.0, OFTrue},
  {"\t\t1", 1.0, OFTrue},
  {"\n\n1", 1.0, OFTrue},
  {" \t\n1", 1.0, OFTrue},
  {"2.5", 2.5, OFTrue},
  {"  +2.5", 2.5, OFTrue},
  {"  -2.5", -2.5, OFTrue},
  {"20", 20.0, OFTrue},
  {"1E3", 1000.0, OFTrue},
  {"1.e3", 1000.0, OFTrue},
  {"1.0E+003", 1000.0, OFTrue},
  {"10000.0E-001", 1000.0, OFTrue},
  {"1.23456E-0", 1.23456, OFTrue},
  {"9.87654321e-10", 9.87654321e-10, OFTrue},

  // big number
  {"1.797693134862315E+308", 1.797693134862315e+308, OFTrue},

  // since our precision is limited, everything else smaller then this may be converted to zero.
  {"2.2250738585072014E-292", 2.2250738585072014E-292, OFTrue},
  {"3.402823466E+38", 3.402823466E+38F, OFTrue},
  {"1.175494351E-38", 1.175494351E-38F, OFTrue},

  // very large numbers that produce an overflow during conversion to double
  // cause undefined behavior in std::atof().
#ifdef ENABLE_OLD_OFSTD_ATOF_IMPLEMENTATION
  // Our old implementation (used in DCMTK 3.5.2 to 3.6.8) returns HUGE_VAL and success
  {"1.7976931348623157E+1000", HUGE_VAL, OFTrue},
#elif defined(ENABLE_IOSTREAM_BASED_ATOF_IMPLEMENTATION)
  // The iostream based implementation returns an error
  {"1.7976931348623157E+1000", 0.0, OFFalse},
#else
  // The sscanf based implementation returns infinitiy and success
  {"1.7976931348623157E+1000", OFnumeric_limits<double>::infinity(), OFTrue},
#endif

  // underflow should be reported as zero, but on some platforms that support
  // denormalized floating point numbers this test case fails.
  // {"2.2250738585072014E-1000", 0.0, OFTrue},

  {"NaN", OFnumeric_limits<double>::quiet_NaN(), OFTrue},
  {"INF", OFnumeric_limits<double>::infinity(), OFTrue},
  {"-INF", -OFnumeric_limits<double>::infinity(), OFTrue},
  // conversions that should fail
  {"BIGNUM", 0.0, OFFalse},
  {"_1.0", 0.0, OFFalse},
  {"", 0.0, OFFalse}
};

OFTEST(ofstd_atof)
{
  std::setlocale(LC_ALL, "C");

  size_t numVp = sizeof(vp)/sizeof(ValuePair);
  double d1, d2, delta;
  OFBool r = OFFalse;

  for (size_t i=0; i<numVp; i++)
  {
    d1 = vp[i].d;
    d2 = OFStandard::atof(vp[i].s, &r);
    if (r && vp[i].r)
    {
      if (d1 != d2)
      {
        delta = fabs(d1-d2);
        // fail if precision is less then 9 decimal digits
        if (delta * 1.0E9 > fabs(d1))
        {
          OFCHECK_FAIL("test #" << i << " failed: conversion error, atof=" << d2 << ", ref="
               << d1 << ", delta=" << delta);
        }
      }
    }
    else
    {
      if (r != vp[i].r)
      {
        if (r)
          OFCHECK_FAIL("test #" << i << " failed: conversion did not flag error as expected, atof=" << d2);
        else
          OFCHECK_FAIL("test #" << i << " failed: conversion did not succeed as expected: " << vp[i].s << ", " << d2);
      }
    }
  }

  // try to activate a locale that uses ',' as a decimal point.
  // If activating the locale fails, we still continue with the test
  std::setlocale(LC_ALL, "de_DE");
  d2 = OFStandard::atof("1.5", &r);
  OFCHECK_EQUAL(1.5, d2);

  // try to activate a locale that uses <U066B> as a decimal point.
  // If activating the locale fails, we still continue with the test
  std::setlocale(LC_ALL, "ps_AF");
  d2 = OFStandard::atof("1.5", &r);
  OFCHECK_EQUAL(1.5, d2);

  std::setlocale(LC_ALL, "C");
}
