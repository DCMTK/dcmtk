/*
 *
 *  Copyright (C) 2012-2022, OFFIS e.V.
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
 *  Purpose: test program for functions and classes defined in
 *           ofmem.h (OFshared_ptr)
 *
 */

#include "dcmtk/config/osconfig.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/ofdiag.h"
#include DCMTK_DIAGNOSTIC_IGNORE_PESSIMIZING_MOVE_WARNING

OFrvalue<OFunique_ptr<int> > testMove()
{
    OFunique_ptr<int> pT( new int( 27 ) );
    return OFmove( pT );
}

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
    OFCHECK_EQUAL( OFstatic_cast( OFBool, p0 ), !p1 );
    // check the referred object is still valid.
    OFCHECK_EQUAL( *p1, 8 );

    // create a unique_ptr.
    OFunique_ptr<int> u0( new int );
    // check if operator bool works as expected.
    OFCHECK( u0 );
    // check if operator* works as expected.
    OFCHECK_EQUAL( *u0 = 42, 42 );
    // default construct a unique_ptr (referring to NULL/nullptr).
    OFunique_ptr<int> u1;
    // check if operator! works as expected.
    OFCHECK( !u1 );
    // do "release assignment".
    u1.reset( u0.release() );
    // destroy object owned by u0 (should do nothing).
    u0.reset();
    // check ownership has been removed.
    // The static cast is necessary since the c++11 version
    // of operator bool is explicit.
    OFCHECK_EQUAL( OFstatic_cast( OFBool, u0 ), !u1 );
    // check if u1 can still be dereferenced.
    OFCHECK_EQUAL( *u1.get() = 23, 23 );
    // move the object back
    u0 = OFmove( u1 );
    // destroy object owned by u1 (should do nothing).
    u1.reset();
    // check ownership has been removed.
    // The static cast is necessary since the c++11 version
    // of operator bool is explicit.
    OFCHECK_EQUAL( OFstatic_cast( OFBool, u1 ), !u0 );
    // check if u1 can still be dereferenced.
    OFCHECK_EQUAL( *u0.get() = 3, 3 );
    // test move out of function
    OFCHECK_EQUAL( *testMove(), 27 );
    OFunique_ptr<int> pi( testMove() );
}
