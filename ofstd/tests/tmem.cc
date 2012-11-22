/*
 *
 *  Copyright (C) 2009-2011, OFFIS e.V.
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
 *  Purpose: test programm for functions and classes defined in
 *           ofmem.h (OFshared_ptr)
 *  
 */

#include "dcmtk/config/osconfig.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofmem.h"

OFTEST(ofstd_memory)
{
    // create a shared_ptr.
    OFshared_ptr<int> p0( new int );
    // check if operator bool works as expected.
    OFCHECK( p0 );
    // check if operator* works as expected.
    OFCHECK_EQUAL( *p0 = 7, 7 );
    // default construct a shared_ptr (referring to NULL/nullptr).
    OFshared_ptr<int> p1;
    // check if operator! works as expected.
    OFCHECK( !p1 );
    // do assignment.
    p1 = p0;
    // deref the copy assigned shared_ptr.
    ++*p1;
    // check if both shared_ptrs refer to the same object.
    OFCHECK_EQUAL( *p0, 8 );
    // remove the reference from the first shared_ptr.
    p0.reset();
    // check destruction of reference from p1.
    // The static cast is necessary since the c++11 version
    // of operator bool is explicit.
    OFCHECK_EQUAL( static_cast<bool>( p0 ), !p1 );
    // check the refered object is still valid.
    OFCHECK_EQUAL( *p1, 8 );
}
