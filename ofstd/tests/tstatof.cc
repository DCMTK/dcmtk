/*
 *
 *  Copyright (C) 1997-2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Test code of ascii/double conversion methods in class OFStandard
 *
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-06-20 12:04:00 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/tests/tstatof.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "ofstd.h"
#include "oftypes.h"
#include "ofconsol.h"

BEGIN_EXTERN_C
#include <math.h>
END_EXTERN_C

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
  {"1.7976931348623157E+1000", 1.0E999 /* infinity */, OFTrue},

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
               << d1 << ", delta=" << delta << endl;
        }
      }      
    }
    else
    {
      if (r != vp[i].r)
      {
        passed = OFFalse;
        if (r)
          CERR << "test #" << i << " failed: conversion did not flag error as expected, atof=" << d2 << endl;
        else 
          CERR << "test #" << i << " failed: conversion did not succeed as expected" << endl;
      }
    }

    if (passed) 
    {
      CERR << "test #" << i << " passed" << endl;
      numPassed++;
    }
  }

  if (numVp == numPassed)
  {
    CERR << "All tests passed." << endl;
    return 0;
  }
  else
  {
    CERR << "Failures: " << numPassed << " of " << numVp << " tests passed." << endl;
  }

  return 1;
}

/*
 *
 * CVS/RCS Log:
 * $Log: tstatof.cc,v $
 * Revision 1.1  2002-06-20 12:04:00  meichel
 * Created test application for OFStandard::atof()
 *
 *
 */
