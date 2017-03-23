/*
 *
 *  Copyright (C) 2017, OFFIS e.V.
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
 *  Purpose: Implementing attribute matching for being used in dcmqrdb and dcmwlm etc.
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/ofdiag.h"
#include "dcmtk/dcmdata/dcmatch.h"
#include "dcmtk/dcmdata/dcvr.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrdt.h"
#include "dcmtk/dcmdata/dcvrtm.h"

class DcmAttributeMatching::WildCardMatcher
{
public:
    // constructor, remembering the end of the query and candidate strings
    WildCardMatcher( const char* queryDataEnd, const char* candidateDataEnd )
    : queryDataEnd( queryDataEnd )
    , candidateDataEnd( candidateDataEnd )
    {

    }

    // the actual match function, taking two pointers to the beginning of
    // the query and the candidate string
    OFBool match( const char* queryData, const char* candidateData ) const
    {
        // matches all regular chars and '?' wildcard with the candidate string
        while( queryData != queryDataEnd && candidateData != candidateDataEnd && *queryData != '*' )
        {
            if( *queryData == '?' || *queryData == *candidateData )
            {
                ++queryData;
                ++candidateData;
            }
            else
            {
                return OFFalse;
            }
        }
        // if the end of the query is reached, there was no '*' wildcard
        // therefor it is either a match (if the end of the candidate was
        // also reached) or not
        if( queryData == queryDataEnd )
            return candidateData == candidateDataEnd;
        // if the current char in the query is not the '*' wildcard, the
        // values don't match, since all other chars would have been
        // matched by the previous while loop
        if( *queryData != '*' )
            return OFFalse;
        // skip all '*' wildcard characters, because even a string like "****"
        // equals the sematics of '*'. If the end of the query is reached
        // any remaining part of the candidate is a match, therefore return
        // OFTrue
        do if( ++queryData == queryDataEnd )
            return OFTrue;
        while( *queryData == '*' );
        // If this part of the code is reached, at least one non wildcard
        // character exists in the query after the previously skipped
        // wildcards. Search for a match of the remaining query characters
        // in the remaining candidate characters, by recursively calling
        // match.
        while( candidateData != candidateDataEnd )
        {
            if( !match( queryData, candidateData ) )
                ++candidateData;
            else
                return OFTrue;
        }
        // if the end of the candidate is reached, both strings don't match.
        return OFFalse;
    }

private:
    // the ends of both the query and the candidate string, will remain
    // constant per match operation
    const char* const queryDataEnd;
    const char* const candidateDataEnd;
};

struct DcmAttributeMatching::DashSeparated
{
    DashSeparated( const void* data, const size_t size )
    : first( OFreinterpret_cast( const char*, data ) )
    , firstSize( 0 )
    , second( first )
    , secondSize( size )
    {
        while( firstSize != secondSize && first[firstSize] != '-' )
            ++firstSize;
        if( firstSize != secondSize )
        {
            secondSize = secondSize - firstSize - 1;
            second = second + firstSize + 1;
        }
    }
    const char* first;
    size_t firstSize;
    const char* second;
    size_t secondSize;
};

OFBool DcmAttributeMatching::singleValueMatching( const void* queryData, const size_t querySize,
                                                  const void* candidateData, const size_t candidateSize )
{
    return !querySize || ( querySize == candidateSize && !memcmp( queryData, candidateData, querySize ) );
}

OFBool DcmAttributeMatching::wildCardMatching( const void* queryData, const size_t querySize,
                                               const void* candidateData, const size_t candidateSize )
{
    return !querySize || WildCardMatcher
    (
        OFreinterpret_cast( const char*, queryData ) + querySize,
        OFreinterpret_cast( const char*, candidateData ) + candidateSize
    )
    .match
    (
        OFreinterpret_cast( const char*, queryData ),
        OFreinterpret_cast( const char*, candidateData )
    );
}

template<typename T>
OFBool DcmAttributeMatching::rangeMatchingTemplate( OFCondition (*parse)(const char*,const size_t,T&),
                                                    const void* queryData, const size_t querySize,
                                                    const void* candidateData, const size_t candidateSize )
{
    if( !querySize )
        return OFTrue;
    T candidate;
    if( parse( OFreinterpret_cast( const char*, candidateData ), candidateSize, candidate ).bad() )
        return OFFalse;
    const DashSeparated query( queryData, querySize );
    T first;
    if( !query.firstSize || parse( query.first, query.firstSize, first ).good() )
    {
        // test whether it's a single date/time and not a range
        if( query.first == query.second )
            return query.firstSize && first == candidate;
        T second;
        if( !query.secondSize || parse( query.second, query.secondSize, second ).good() )
            return ( !query.firstSize || first <= candidate ) && ( !query.secondSize || second >= candidate );
    }
    return OFFalse;
}

OFBool DcmAttributeMatching::rangeMatchingDate( const void* queryData, const size_t querySize,
                                                const void* candidateData, const size_t candidateSize )
{
    return rangeMatchingTemplate( &DcmDate::getOFDateFromString, queryData, querySize, candidateData, candidateSize );
}

OFBool DcmAttributeMatching::rangeMatchingTime( const void* queryData, const size_t querySize,
                                                const void* candidateData, const size_t candidateSize )
{
    return rangeMatchingTemplate( &DcmTime::getOFTimeFromString, queryData, querySize, candidateData, candidateSize );
}

OFBool DcmAttributeMatching::rangeMatchingDateTime( const void* queryData, const size_t querySize,
                                                    const void* candidateData, const size_t candidateSize )
{
    return rangeMatchingTemplate( &DcmDateTime::getOFDateTimeFromString, queryData, querySize, candidateData, candidateSize );
}

OFBool DcmAttributeMatching::rangeMatchingDateTime( const void* dateQueryData, const size_t dateQuerySize,
                                                    const void* timeQueryData, const size_t timeQuerySize,
                                                    const void* dateCandidateData, const size_t dateCandidateSize,
                                                    const void* timeCandidateData, const size_t timeCandidateSize )
{
    if( !dateQuerySize )
        return rangeMatchingTime( timeQueryData, timeQuerySize, timeCandidateData, timeCandidateSize );
    OFDateTime candidate;
    if( DcmDate::getOFDateFromString( OFreinterpret_cast( const char*, dateCandidateData ), dateCandidateSize, candidate.Date ).bad() )
        return OFFalse;
    if( timeCandidateSize && DcmTime::getOFTimeFromString( OFreinterpret_cast( const char*, timeCandidateData ), timeCandidateSize, candidate.Time ).bad() )
        return OFFalse;
    const DashSeparated dateQuery( dateQueryData, dateQuerySize );
    const DashSeparated timeQuery( timeQueryData, timeQuerySize );
    // check that both date/time ranges have the same structure
    if( ( dateQuery.first == dateQuery.second ) != ( timeQuery.first != timeQuery.second ) )
        return OFFalse;
    if( ( !dateQuery.firstSize && timeQuery.firstSize ) || ( !dateQuery.secondSize && timeQuery.secondSize ) )
        return OFFalse;
    OFDateTime first;
    // parse the first date/time
    if( dateQuery.firstSize )
    {
        if( DcmDate::getOFDateFromString( dateQuery.first, dateQuery.firstSize, first.Date ).bad() )
            return OFFalse;
        if( timeQuery.firstSize && DcmTime::getOFTimeFromString( timeQuery.first, timeQuery.firstSize, first.Time ).bad() )
            return OFFalse;
    }
    // test whether it's a single date/time and not a range
    if( dateQuery.first == dateQuery.second )
        return dateQuery.firstSize && first == candidate;
    OFDateTime second;
    // parse the second date/time
    if( dateQuery.secondSize )
    {
        if( DcmDate::getOFDateFromString( dateQuery.second, dateQuery.secondSize, second.Date ).bad() )
            return OFFalse;
        if( timeQuery.secondSize && DcmTime::getOFTimeFromString( timeQuery.second, timeQuery.secondSize, second.Time ).bad() )
            return OFFalse;
    }
    // compare candidate with the date/time range
    return ( !dateQuery.firstSize || first <= candidate ) && ( !dateQuery.secondSize || second >= candidate );
}

OFBool DcmAttributeMatching::listOfUIDMatching( const void* queryData, const size_t querySize,
                                                const void* candidateData, const size_t candidateSize )
{
    if( !querySize )
        return OFTrue;
    const char* pQuery = OFreinterpret_cast( const char*, queryData );
    const char* const pQueryEnd = pQuery + querySize;
    const char* pCandidate = OFreinterpret_cast( const char*, candidateData );
    const char* const pCandidateEnd = pCandidate + candidateSize;
    // character wise match both strings, reset candidate pointer whenever a
    // '\\' character is encountered within a multi-valued query.
    while( pQuery != pQueryEnd )
    {
        if( pCandidate != pCandidateEnd && *pQuery == *pCandidate )
        {
            ++pQuery;
            ++pCandidate;
        }
        else
        {
            // test whether the candidate matches with the current value from the query
            if( pCandidate == pCandidateEnd && *pQuery == '\\' )
                return OFTrue;
            // mismatch, search for a '\\' char to try again with the next value from the query,
            // return OFFalse if none can be found, i.e. this was the last value.
            while( *pQuery != '\\' )
                if( ++pQuery == pQueryEnd )
                    return OFFalse;
            // skip the '\\' character
            ++pQuery;
            // reset candidate pointer to the beginning of the candidate
            pCandidate = OFreinterpret_cast( const char*, candidateData );
        }
    }
    // the query is at its end, we have a match if the candidate is also
    return pCandidate == pCandidateEnd;
}

DcmAttributeMatching::DcmAttributeMatching()
: m_pMatch( OFnullptr )
{

}

DcmAttributeMatching::DcmAttributeMatching( const DcmVR vr )
: m_pMatch( OFnullptr )
{
    switch( vr.getEVR() )
    {
    default:
        m_pMatch = &DcmAttributeMatching::singleValueMatching;
        break;

    case EVR_AE:
    case EVR_CS:
    case EVR_LO:
    case EVR_LT:
    case EVR_PN:
    case EVR_SH:
    case EVR_ST:
    case EVR_UC:
    case EVR_UR:
    case EVR_UT:
        m_pMatch = &DcmAttributeMatching::wildCardMatching;
        break;

    case EVR_DA:
        m_pMatch = &DcmAttributeMatching::rangeMatchingDate;
        break;

    case EVR_TM:
        m_pMatch = &DcmAttributeMatching::rangeMatchingTime;
        break;

    case EVR_DT:
        m_pMatch = &DcmAttributeMatching::rangeMatchingDateTime;
        break;

    case EVR_UI:
        m_pMatch = &DcmAttributeMatching::listOfUIDMatching;
        break;
    }
}

DcmAttributeMatching::operator OFBool() const
{
#include DCMTK_DIAGNOSTIC_PUSH
#include DCMTK_DIAGNOSTIC_IGNORE_VISUAL_STUDIO_PERFORMANCE_WARNING
    return m_pMatch;
#include DCMTK_DIAGNOSTIC_POP
}

OFBool DcmAttributeMatching::operator!() const
{
    return !m_pMatch;
}

OFBool DcmAttributeMatching::operator()( const void* queryData, const size_t querySize,
                                         const void* candidateData, const size_t candidateSize ) const
{
    assert( m_pMatch );
    return m_pMatch( queryData, querySize, candidateData, candidateSize );
}
