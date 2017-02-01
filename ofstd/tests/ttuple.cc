/*
 *
 *  Copyright (C) 2014-2017, OFFIS e.V.
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
 *  Purpose: unit test for OFtuple
 *
 */

// Disable some warnings.
// We're testing if a tuple containing floats can be initialized from a tuple of
// ints, which is required by the standard. But this gives precision warnings
// on some compilers / at some settings and we don't want to see these warnings.
#ifdef __GNUG__
#pragma GCC diagnostic ignored "-Wconversion"
#elif defined(_MSC_VER)
#pragma warning(disable: 4244)
#endif

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/oftuple.h"
#include "dcmtk/ofstd/ofstring.h"

OFTEST(ofstd_tuple)
{
    OFtuple<int,OFBool,OFString> t0, t1( 3, OFFalse, "Hello World" ), t2, t3;

    OFCHECK( (OFtuple_size<OFtuple<int,OFBool,const char*> >::value) == 3 );
    OFCHECK( OFget<0>( t1 ) == 3 && !OFget<1>( t1 ) && OFget<2>( t1 ) == "Hello World" );
    t0 = t1;
    t2 = t3;
    OFswap( t1, t3 );
    OFCHECK( t3 == t0 && t1 == t2 );
    t1.swap( t0 );
    OFCHECK( t3 == t1 && t2 == t0 );

    int i = 42;
    OFBool b = OFTrue;
    const char* cstr( "Thomas Theisman" );

    t2 = OFtie( i, b, cstr );

    OFCHECK( OFget<0>( t2 ) == 42 && OFget<1>( t2 ) && OFget<2>( t2 ) == "Thomas Theisman" );

    OFString s;
    OFtie( i, b, s ) = OFmake_tuple( 23, OFFalse, "Edward Saganami" );

    OFCHECK( i == 23 && !b && s == "Edward Saganami" );

    OFtuple<float,float,float,float> t4( 5.2f, 3.1f, 2.4f, 9.1f ), t5( OFmake_tuple( 4, 6, 9, 27 ) );

    OFCHECK( t4 != t5 && t4 > t5 && t5 <= t4 && !( t4 == t5 ) );
    t4 = t5;
    OFCHECK( !( t4 < t5 ) && t4 <= t5 );

    OFtie( OFignore, s ) = OFMake_pair( OFString( "Hello World" ), OFString( "My name is John" ) );
    OFCHECK( s == "My name is John" );
}

