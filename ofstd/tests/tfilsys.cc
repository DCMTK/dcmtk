/*
 *
 *  Copyright (C) 2018, OFFIS e.V.
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
 *  Module: ofstd
 *
 *  Author: Jan Schlamelcher
 *
 *  Purpose: test program for OFpath and related stuff.
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/offilsys.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/ofstd/oftraits.h"
#include "dcmtk/ofstd/ofassert.h"

OFTEST(ofstd_filesystem)
{
    OFstatic_assert( (OFis_same<OFdirectory_iterator::difference_type,STD_NAMESPACE ptrdiff_t>::value), "OFdirectory_iterator has correctly defined difference type" );
    OFstatic_assert( (OFis_same<OFdirectory_iterator::value_type,OFdirectory_entry>::value), "OFdirectory_iterator has correctly defined value type" );
    OFstatic_assert( (OFis_same<OFdirectory_iterator::pointer,const OFdirectory_entry*>::value), "OFdirectory_iterator has correctly defined pointer type" );
    OFstatic_assert( (OFis_same<OFdirectory_iterator::reference,const OFdirectory_entry&>::value), "OFdirectory_iterator has correctly defined reference type" );
#if _WIN32
    const OFBool onWindows = OFTrue;
    OFCHECK_EQUAL( OFpath::preferred_separator, '\\' );
#else
    const OFBool onWindows = OFFalse;
    OFCHECK_EQUAL( OFpath::preferred_separator, '/' );
#endif
    OFCHECK( OFpath().empty() );
    OFCHECK_EQUAL( OFpath( "C:" ).is_absolute(), onWindows );
    OFCHECK_EQUAL( OFpath( OFString( 1, OFpath::preferred_separator ) ).is_absolute(), !onWindows );
#if _WIN32
    OFCHECK_EQUAL( OFpath( "C:\\test" ).root_name(), "C:" );
#else
    OFCHECK( !OFpath( "C:\\test" ).has_root_name() );
#endif
    OFString expected = OFString( "path" ) + OFpath::preferred_separator + "to" + OFpath::preferred_separator + "file";
    OFCHECK_EQUAL( "path" / ( OFString( "to" ) + OFpath::preferred_separator ) / "file", expected );
    OFCHECK_EQUAL( OFpath() / "path" / ( OFString( "to" ) + OFpath::preferred_separator ) / "file", expected );
    OFCHECK( OFpath( "bla.txt" ).has_extension() );
    OFCHECK( OFpath( "bla." ).has_extension() );
    OFCHECK( !OFpath( "bla" ).has_extension() );
    OFCHECK( !OFpath( ".bla" ).has_extension() );
    OFCHECK( !OFpath( "." ).has_extension() );
    OFCHECK( !OFpath( ".." ).has_extension() );
    OFCHECK( OFpath( "..." ).has_extension() );
    OFCHECK( OFpath( "..a" ).has_extension() );
    OFCHECK( OFpath( ":..a" ).has_extension() );
    OFCHECK_EQUAL( OFpath( "c:.bla" ).has_extension(), !onWindows );
    OFCHECK_EQUAL( OFpath( "c:.." ).has_extension(), !onWindows );
    OFCHECK_EQUAL( ( OFpath( "test" ) / ".." / "bla.txt" ).extension(), ".txt" );
    OFCHECK_EQUAL( ( OFpath( "test" ) / ".." / "bla." ).extension(), "." );
    OFCHECK_EQUAL( ( OFpath( "test" ) / "." / ".bla" ).extension(), "" );
    OFCHECK_EQUAL( ( OFpath( "test" ) / "..bla" ).extension(), ".bla" );
    OFCHECK_EQUAL( ( OFpath( "test" ) / ".." ).extension(), "" );
    OFCHECK_EQUAL( ( OFpath( "test" ) / "..." ).extension(), "." );
    OFCHECK( OFdirectory_iterator() == OFdirectory_iterator() );
    OFCHECK( OFdirectory_iterator( "./#$aninvalidpathihope--~~~" ) == OFdirectory_iterator() );
    OFCHECK( OFdirectory_iterator( "." ) != OFdirectory_iterator() );
    OFCHECK( OFStandard::createDirectory( "tehtestdire", "" ).good() );
    OFVector<OFString> files;
    files.push_back( "tesfile" );
    files.push_back( "anotherone" );
    files.push_back( "bites" );
    files.push_back( "the" );
    files.push_back( "heh" );
    files.push_back( "guess you didnt expect that huh" );
    OFFile f;
    for( OFVector<OFString>::const_iterator it = files.begin(); it != files.end(); ++it )
        OFCHECK( f.fopen( ( "tehtestdire" / *it ).native(), "w" ) );
    for( OFdirectory_iterator it( "tehtestdire" ); it != OFdirectory_iterator(); ++it )
    {
        OFBool found = OFFalse;
        for( OFVector<OFString>::iterator jt = files.begin(); jt != files.end(); ++jt )
        {
            if( "tehtestdire" / *jt == *it && *jt == it->path().filename() )
            {
                found = OFTrue;
                files.erase( jt );
                break;
            }
        }
        OFCHECK( found );
    }
    OFCHECK( files.empty() );
}
