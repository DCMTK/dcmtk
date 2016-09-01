#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofvriant.h"

struct int_visitor
{
    void operator()( double d )
    {
        // Avoid compiler warning on unused parameter
        (void)d;
        OFCHECK_FAIL( "wrong alternative <double> selected" );
    }

    void operator()( int i )
    {
        // Avoid compiler warning on unused parameter
        (void)i;
        OFCHECK( i == 42 );
    }

    void operator()( char c )
    {
        // Avoid compiler warning on unused parameter
        (void)c;
        OFCHECK_FAIL( "wrong alternative <char> selected" );
    }
};

struct double_visitor
{
    bool operator()( double d )
    {
        // Avoid compiler warning on unused parameter
        (void)d;
        return d == 27.912456f;
    }

    bool operator()( int i )
    {
        // Avoid compiler warning on unused parameter
        (void)i;
        OFCHECK_FAIL( "wrong alternative <int> selected" );
        return false;
    }

    bool operator()( char c )
    {
        // Avoid compiler warning on unused parameter
        (void)c;
        OFCHECK_FAIL( "wrong alternative <char> selected" );
        return false;
    }
};

OFTEST(ofstd_variant)
{
    // copy construction from alternative
    OFvariant<int,float,bool> v0( true );

    // default construction
    OFvariant<int,float,bool> v1;

    OFCHECK( OFget<bool>( &v0 ) );
    OFCHECK( *OFget<bool>( &v0 ) );
    OFCHECK( OFget<int>( &v1 ) );

    // alternative changing assignment
    v0 = 3.1415f;

    // same alternative assignment
    v1 = 7;

    OFCHECK( !OFget<bool>( &v0 ) );
    OFCHECK( OFget<float>( &v0 ) );
    OFCHECK( *OFget<float>( &v0 ) == 3.1415f );
    OFCHECK( OFget<int>( &v1 ) );
    OFCHECK( *OFget<int>( &v1 ) == 7 );

    // alternative changing assignment and copy construction
    OFvariant<int,float,bool> v2( v0 = v1 );

    OFCHECK( OFget<int>( &v0 ) );
    OFCHECK( *OFget<int>( &v0 ) == 7 );
    OFCHECK( OFget<int>( &v1 ) );
    OFCHECK( *OFget<int>( &v1 ) == 7 );
    OFCHECK( OFget<int>( &v2 ) );
    OFCHECK( *OFget<int>( &v2 ) == 7 );

    // same alternative assignment
    v0 = 42;

    OFCHECK( OFget<int>( &v0 ) );
    OFCHECK( *OFget<int>( &v0 ) == 42 );

    // same alternative assignment
    v1 = v0;

    OFCHECK( OFget<int>( &v0 ) );
    OFCHECK( *OFget<int>( &v0 ) == 42 );
    OFCHECK( OFget<int>( &v1 ) );
    OFCHECK( *OFget<int>( &v1 ) == 42 );

    v2 = 27.912456f;

    // test visitation
    OFvisit<void>( int_visitor(), v0 );
    OFCHECK( OFvisit<bool>( double_visitor(), const_cast<const OFvariant<int,float,bool>&>( v2 ) ) );
}
