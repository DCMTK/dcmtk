/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:16 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofconsol.h"

#define INCLUDE_CMATH
#include "dcmtk/ofstd/ofstdinc.h"


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
  {"1E3", 1000.0, OFTrue},
  {"1.e3", 1000.0, OFTrue},
  {"1.0E+003", 1000.0, OFTrue},
  {"10000.0E-001", 1000.0, OFTrue},
  {"1.23456E-0", 1.23456, OFTrue},
  {"9.87654321e-10", 9.87654321e-10, OFTrue},

  // big number
  {"1.7976931348623157E+308", 1.7976931348623157E+308, OFTrue},

  // since our precision is limited, everything else smaller then this may be converted to zero.
  {"2.2250738585072014E-292", 2.2250738585072014E-292, OFTrue},
  {"3.402823466E+38F", 3.402823466E+38F, OFTrue},
  {"1.175494351E-38F", 1.175494351E-38F, OFTrue},

  // overflow is reported as infinity
  {"1.7976931348623157E+1000", 1.0E300 * 1.0E300 * 1.0E300 * 1.0E100 /* infinity */, OFTrue},

  // underflow is reported as zero
  {"2.2250738585072014E-1000", 0.0, OFTrue},

  // conversions that should fail
  // {"NaN", 0.0, OFFalse}, // this test will fail if DISABLE_OFSTD_ATOF is defined
  {"BIGNUM", 0.0, OFFalse},
  {"_1.0", 0.0, OFFalse},
  {"", 0.0, OFFalse}
};

int main()
{
  unsigned long numVp = sizeof(vp)/sizeof(ValuePair);
  double d1, d2, delta;
  OFBool r = OFFalse;
  OFBool passed;
  unsigned long numPassed = 0;

  for (unsigned long i=0; i<numVp; i++)
  {
    passed = OFTrue;
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
          passed = OFFalse;
          CERR << "test #" << i << " failed: conversion error, atof=" << d2 << ", ref="
               << d1 << ", delta=" << delta << OFendl;
        }
      }
    }
    else
    {
      if (r != vp[i].r)
      {
        passed = OFFalse;
        if (r)
          CERR << "test #" << i << " failed: conversion did not flag error as expected, atof=" << d2 << OFendl;
        else
          CERR << "test #" << i << " failed: conversion did not succeed as expected" << OFendl;
      }
    }

    if (passed)
    {
      CERR << "test #" << i << " passed" << OFendl;
      numPassed++;
    }
  }

  if (numVp == numPassed)
  {
    CERR << "All tests passed." << OFendl;
    return 0;
  }
  else
  {
    CERR << "Failures: " << numPassed << " of " << numVp << " tests passed." << OFendl;
  }

  return 1;
}

/*
 *
 * CVS/RCS Log:
 * $Log: tstatof.cc,v $
 * Revision 1.6  2010-10-14 13:15:16  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.5  2006/08/14 16:42:48  meichel
 * Updated all code in module ofstd to correctly compile if the standard
 *   namespace has not included into the global one with a "using" directive.
 *
 * Revision 1.4  2005/12/08 15:49:07  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.3  2002/11/27 11:23:13  meichel
 * Adapted module ofstd to use of new header file ofstdinc.h
 *
 * Revision 1.2  2002/07/18 12:13:25  joergr
 * Changed value of double constant to avoid compilation error (out of range)
 * reported by Sun CC 2.0.1.
 *
 * Revision 1.1  2002/06/20 12:04:00  meichel
 * Created test application for OFStandard::atof()
 *
 *
 */
