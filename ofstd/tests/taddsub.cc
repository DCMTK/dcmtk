/*
 *
 *  Copyright (C) 2015, Open Connections GmbH
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
 *  Author:  Michael Onken
 *
 *  Purpose: test program for safe subtraction and addition
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofstd.h"


OFTEST(ofstd_safeSubtractAndAdd)
{
  // --------------- Subtraction ----------------

  unsigned int a = 1;
  unsigned int b = 2;
  // check whether underflow occurs (it should)
  OFCHECK(OFStandard::safeSubtract(a, b, a) == OFFalse);
  // check whether a has not been modified
  OFCHECK_EQUAL(a, 1);

  a = OFnumeric_limits<unsigned int>::max();
  b = OFnumeric_limits<unsigned int>::max()-1;
  // check whether no underflow occured (it shouldnt)
  OFCHECK(OFStandard::safeSubtract(a, b, a) == OFTrue);
  // check whether the result a was computed as expected
  OFCHECK_EQUAL(a, 1);

  // --------------- Addition ----------------

  a = OFnumeric_limits<unsigned int>::max()-1;
  b = OFnumeric_limits<unsigned int>::max()-1;
  // check whether overflow occured (it should)
  OFCHECK(OFStandard::safeAdd(a, b, a) == OFFalse);
  // check whether a has not been modified
  OFCHECK_EQUAL(a, OFnumeric_limits<unsigned int>::max()-1);

  b = 1; // a still equals max-1
  // check whether no overflow occured (it shouldnt)
  OFCHECK(OFStandard::safeAdd(a, b, a) == OFTrue);
  // check whether the result a was computed as expected.
  // dividing and then multiplying by 2 is required since max maybe be an
  // odd number so that max/2 is rounded to the floor number.
  OFCHECK_EQUAL(a, OFnumeric_limits<unsigned int>::max());
}
