/*
 *
 *  Copyright (C) 2017-2018, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Jan Schlamelcher
 *
 *  Purpose: unit test for DcmAttributeMatching
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcvr.h"
#include "dcmtk/dcmdata/dcmatch.h"

static OFBool test_singlevalue( const char* const query, const char* const candidate )
{
    return DcmAttributeMatching::singleValueMatching( query, strlen( query ), candidate, strlen( candidate ) );
}

static OFBool test_wildcards( const char* const query, const char* const candidate )
{
    return DcmAttributeMatching::wildCardMatching( query, strlen( query ), candidate, strlen( candidate ) );
}

static OFBool test_date( const char* const query )
{
    return DcmAttributeMatching::isDateQuery( query, strlen( query ) );
}

static OFBool test_date( const char* const query, const char* const candidate )
{
    return DcmAttributeMatching::rangeMatchingDate( query, strlen( query ), candidate, strlen( candidate ) );
}

static OFBool test_time( const char* const query )
{
    return DcmAttributeMatching::isTimeQuery( query, strlen( query ) );
}

static OFBool test_time( const char* const query, const char* const candidate )
{
    return DcmAttributeMatching::rangeMatchingTime( query, strlen( query ), candidate, strlen( candidate ) );
}

static OFBool test_datetime( const char* const query )
{
    return DcmAttributeMatching::isDateTimeQuery( query, strlen( query ) );
}

static OFBool test_datetime( const char* const query, const char* const candidate )
{
    return DcmAttributeMatching::rangeMatchingDateTime( query, strlen( query ), candidate, strlen( candidate ) );
}

static OFBool test_uids( const char* const query, const char* const candidate )
{
    return DcmAttributeMatching::listOfUIDMatching( query, strlen( query ), candidate, strlen( candidate ) );
}

OFTEST(dcmdata_attribute_matching)
{
    // wild card matching tests
    OFCHECK(test_wildcards( "", "hello world" ));
    OFCHECK(test_wildcards( "*", "hello world" ));
    OFCHECK(!test_wildcards( "?", "hello world" ));
    OFCHECK(test_wildcards( "?*", "hello world" ));
    OFCHECK(test_wildcards( "?ell*??l?", "hello world" ));
    OFCHECK(!test_wildcards( "?ell***?*?l??", "hello world" ));
    OFCHECK(test_wildcards( "?ell*?**?l?*", "hello world" ));
    // date
    OFCHECK(test_date( "17000101" ));
    OFCHECK(test_date( "30001010" ));
    OFCHECK(test_date( "-12000101" ));
    OFCHECK(test_date( "12451210-" ));
    OFCHECK(test_date( "20451210-29110114" ));
    OFCHECK(!test_date( "204512101-29110114" ));
    OFCHECK(!test_date( "20451210-29110134" ));
    OFCHECK(!test_date( "3124" ));
    OFCHECK(!test_date( "2017.05.12" ));
    OFCHECK(test_date( "", "20170224" ));
    OFCHECK(test_date( "1987.08.02", "19870802" ));
    OFCHECK(test_date( "-20000101", "20000101" ));
    OFCHECK(test_date( "-20000101", "19990531" ));
    OFCHECK(!test_date( "-20000101", "20010101" ));
    OFCHECK(test_date( "20000101-", "20010101" ));
    OFCHECK(!test_date( "20000101-", "19991231" ));
    OFCHECK(test_date( "19990101-20000305", "20000101" ));
    OFCHECK(!test_date( "19990101-20000305", "19980107"));
    OFCHECK(!test_date( "19990101-20000305", "20000306"));
    OFCHECK(!test_date( "19990101-20000305", "122713.114122"));
    // time
    OFCHECK(test_time( "12" ));
    OFCHECK(!test_time( "49" ));
    OFCHECK(!test_time( "12:00:00" ));
    OFCHECK(test_time( "122413.123456-13" ));
    OFCHECK(!test_time( "29-13" ));
    OFCHECK(!test_time( "04-25" ));
    OFCHECK(test_time( "-231211" ));
    OFCHECK(test_time( "124222-" ));
    OFCHECK(test_time( "", "120224" ));
    OFCHECK(test_time( "11:23:17.123456", "112317.123456" ));
    OFCHECK(test_time( "-12", "120000" ));
    OFCHECK(test_time( "-12", "113059.654321" ));
    OFCHECK(!test_time( "-12", "120000.000001" ));
    OFCHECK(test_time( "1200-", "120000.000000" ));
    OFCHECK(!test_time( "12-", "115959.999999" ));
    OFCHECK(test_time( "11-121428.234763", "12" ));
    OFCHECK(!test_time( "11-121428.234763", "105959.999999"));
    OFCHECK(!test_time( "11-121428.234763", "121428.234764"));
    OFCHECK(!test_time( "11-121428.234763", "20140909"));
    // datetime
    OFCHECK(test_datetime( "20170224120224+0100" ));
    OFCHECK(!test_datetime( "2017224120224+0100" ));
    OFCHECK(test_datetime( "2017-2018" ));
    OFCHECK(!test_datetime( "201713-2018" ));
    OFCHECK(!test_datetime( "2017-201813" ));
    OFCHECK(test_datetime( "2017-201812" ));
    OFCHECK(test_datetime( "191712-19991231235959.999999" ));
    OFCHECK(test_datetime( "19991231235959.999999-" ));
    OFCHECK(test_datetime( "-20000305173259.123456" ));
    OFCHECK(test_datetime( "", "20170224120224+0100" ));
    OFCHECK(test_datetime( "20170224113224.000000+0030", "20170224120224+0100" ));
    OFCHECK(test_datetime( "-2000", "20000101000000.000000" ));
    OFCHECK(test_datetime( "-2000", "19990531132417.231194" ));
    OFCHECK(!test_datetime( "-2000", "20000102000000.000001" ));
    OFCHECK(test_datetime( "200001-", "2001010100" ));
    OFCHECK(!test_datetime( "2000-", "19991231235959.999999" ));
    OFCHECK(!test_datetime( "1999-20000305173259.123456", "1998010712+0430"));
    OFCHECK(!test_datetime( "1999-20000305173259.123456", "20000306"));
    OFCHECK(!test_datetime( "1999-20000305173259.123456", "122713.114122"));
    // UIDs
    OFCHECK(test_uids("","123.456.789.10"));
    OFCHECK(test_uids("123.456.789.10","123.456.789.10"));
    OFCHECK(!test_uids("456.789.10","123.456.789.10"));
    OFCHECK(test_uids("456.789.10\\123.456.789.10","123.456.789.10"));
    OFCHECK(test_uids("456.789.10\\123.456.789.10\\456.123.789.10","123.456.789.10"));
    OFCHECK(!test_uids("456.789.10\\123.456.79.10\\456.123.789.10","123.456.789.10"));
    // single value matching
    OFCHECK(test_singlevalue( "", "Hello world!" ));
    OFCHECK(test_singlevalue( "Hello world!", "Hello world!" ));
    OFCHECK(test_singlevalue( "Hello*world!", "Hello*world!" ));
    OFCHECK(!test_singlevalue( "Hello*world!", "Hello world!" ));
    // high level API tests
    DcmAttributeMatching match( EVR_DA );
    OFCHECK(match);
    OFCHECK(!!match);
    OFCHECK(match( "20160101-20181231", 17, "20170713", 8 ));
    match = DcmAttributeMatching( EVR_TM );
    OFCHECK(match( "-12", 3, "11", 2 ));
    OFCHECK(!DcmAttributeMatching( EVR_PN )( "M?ller", 6, "^Martin", 7 ));
}
