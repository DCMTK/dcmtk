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
 *  Purpose: Implementing attribute matching for being used in dcmqrdb and dcmwlm etc.
 *
 */


#ifndef DCMATCH_H
#define DCMATCH_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dcdefine.h"

// forward declaration of DcmVR
class DcmVR;

/** A class that implements the different kinds attribute matching.
 *  DcmAttributeMatching may either be used by directly calling the appropriate matching function,
 *  e.g. wildCardMatching() or by creating a DcmAttributeMatching object for a specific VR.
 *  In the latter case, DcmAttributeMatching will choose the appropriate matching function for
 *  the VR in its constructor and remember the result during its lifetime.
 */
class DCMTK_DCMDATA_EXPORT DcmAttributeMatching
{
public:

    /** Helper class for parsing value range pairs, as in date/time ranges.
     */
    struct Range
    {
        /** Separate the given value into two range components (if possible).
         *  @param data any data that might be a range of values.
         *  @param size the size in bytes of the value(s) passed as data.
         *  @param separator optional argument for using another separator than the default
         *    one (dash character '-').
         */
        Range( const void* const data, const size_t size, const char separator = '-' );

        /** Return the information whether *this refers to an actual range of values or
         *  just a single value.
         *  @return OFTrue if the given argument is a range of values (i.e. contains the
         *    given/default range separator character), OFFalse otherwise.
         */
        OFBool isRange() const;

        /** Return the information whether *this refers to an open range without a definite
         *  beginning.
         *  @return OFTrue if the given argument is an open range without a definite beginning
         *    OFFalse otherwise.
         */
        OFBool hasOpenBeginning() const;

        /** Return the information whether *this refers to an open range without a definite
         *  end.
         *  @return OFTrue if the given argument is an open range without a definite end
         *    OFFalse otherwise.
         */
        OFBool hasOpenEnd() const;

        /** The first value in the range, i.e. the beginning of the range.
         *  @note this will contain the whole value if the argument is not a range of values,
         *    use isRange() to retrieve that information.
         */
        const char* first;

        /** The size in bytes of the first value, will be zero if the argument is an open range
         *  with no definite beginning.
         */
        size_t firstSize;

        /** The second value in the range, e.g. the end of the range.
         *  @note this will also contain the whole value if the argument is not a range of values,
         *    use isRange() to retrieve that information.
         */
        const char* second;

        /** The size in bytes of the second value, will be zero if the argument is an open range
         *  with no definite end.
         */
        size_t secondSize;
    };

    /** Check whether the given query data conforms to the VR DA.
     *  @param queryData a pointer to some data.
     *  @param querySize the size (in bytes) of the data queryData refers to.
     *  @return OFTrue if the query is a valid Date or Date Range, OFFalse otherwise.
     */
    static OFBool isDateQuery( const void* queryData, const size_t querySize );

    /** Check whether the given query data conforms to the VR TM.
     *  @param queryData a pointer to some data.
     *  @param querySize the size (in bytes) of the data queryData refers to.
     *  @return OFTrue if the query is a valid Time or Time Range, OFFalse otherwise.
     */
    static OFBool isTimeQuery( const void* queryData, const size_t querySize );

    /** Check whether the given query data conforms to the VR DT.
     *  @param queryData a pointer to some data.
     *  @param querySize the size (in bytes) of the data queryData refers to.
     *  @return OFTrue if the query is a valid Date Time or Date Time Range, OFFalse otherwise.
     */
    static OFBool isDateTimeQuery( const void* queryData, const size_t querySize );

    /** Match the query data and the candidate using Single Value Matching, as defined by the DICOM standard.
     *  @param queryData a pointer to some data.
     *  @param querySize the size (in bytes) of the data queryData refers to.
     *  @param candidateData a pointer to some data.
     *  @param candidateSize the size (in bytes) of the data candidateData refers to.
     *  @return OFTrue if the query and the candidate match, OFFalse otherwise.
     *  @details
     *  If querySize equals zero, the result will be OFTrue, as appropriate for Universal Matching.
     *  Otherwise the result will only be OFTrue if both data pieces have the same size are bitwise
     *  equal.
     */
    static OFBool singleValueMatching( const void* queryData, const size_t querySize,
                                       const void* candidateData, const size_t candidateSize );

    /** Match the query and the candidate string using Wild Card Matching, as defined by the DICOM standard.
     *  @param queryData a pointer to a character string potentially containing Wild Card characters.
     *  @param querySize the size (in bytes) of the string queryData refers to.
     *  @param candidateData a pointer to a character string.
     *  @param candidateSize the size (in bytes) of the string candidateData refers to.
     *  @return OFTrue if the query and the candidate match, OFFalse otherwise.
     *  @details
     *  If querySize equals zero, the result will be OFTrue, as appropriate for Universal Matching.
     *  If the query contains no Wild Card characters ('?' and '*') the result will be the same
     *  as if singleValueMatching() was called.
     *  @note It is therefore not necessary to check for Wild Card characters and manually decided whether
     *    to use wildCardMatching() or singleValueMatching(), simply use wildCardMatching() if the query
     *    may <i>potentially</i> contain Wild Card characters.
     */
    static OFBool wildCardMatching( const void* queryData, const size_t querySize,
                                    const void* candidateData, const size_t candidateSize );

    /** Match the query date or date range and the candidate date.
     *  @param queryData a pointer to a character string that contains a DICOM Date (DA) or
     *    a date range.
     *  @param querySize the size (in bytes) of the string queryData refers to.
     *  @param candidateData a pointer to a character string that contains a DICOM Date (DA).
     *  @param candidateSize the size (in bytes) of the string candidateData refers to.
     *  @return OFTrue if the query and the candidate match, OFFalse otherwise.
     *  @details
     *  If querySize equals zero, the result will be OFTrue, as appropriate for Universal Matching.
     *  Otherwise query will be converted to an OFDate or a range defined by two OFDates and candidate
     *  will be converted to an OFDate. It will then be tested whether the candidate date lies within
     *  the range given by the query or is equal to the queried date as appropriate. If the conversion
     *  to OFDate is unsuccessful for any of the dates, the result will be OFFalse.
     */
    static OFBool rangeMatchingDate( const void* queryData, const size_t querySize,
                                     const void* candidateData, const size_t candidateSize );

    /** Match the query time or time range and the candidate time.
     *  @param queryData a pointer to a character string that contains a DICOM Time (TM) or
     *    a time range.
     *  @param querySize the size (in bytes) of the string queryData refers to.
     *  @param candidateData a pointer to a character string that contains a DICOM Time (TM).
     *  @param candidateSize the size (in bytes) of the string candidateData refers to.
     *  @return OFTrue if the query and the candidate match, OFFalse otherwise.
     *  @details
     *  If querySize equals zero, the result will be OFTrue, as appropriate for Universal Matching.
     *  Otherwise query will be converted to an OFTime or a range defined by two OFTimes and candidate
     *  will be converted to an OFTime. It will then be tested whether the candidate time lies within
     *  the range given by the query or is equal to the queried time as appropriate. If the conversion
     *  to OFTime is unsuccessful for any of the times, the result will be OFFalse.
     */
    static OFBool rangeMatchingTime( const void* queryData, const size_t querySize,
                                     const void* candidateData, const size_t candidateSize );

    /** Match the query date time or date time range and the candidate date time.
     *  @param queryData a pointer to a character string that contains a DICOM Date Time (DT) or
     *    a date time range.
     *  @param querySize the size (in bytes) of the string queryData refers to.
     *  @param candidateData a pointer to a character string that contains a DICOM Date Time (DT).
     *  @param candidateSize the size (in bytes) of the string candidateData refers to.
     *  @return OFTrue if the query and the candidate match, OFFalse otherwise.
     *  @details
     *  If querySize equals zero, the result will be OFTrue, as appropriate for Universal Matching.
     *  Otherwise query will be converted to an OFDateTime or a range defined by two OFDateTimes and
     *  candidate will be converted to an OFDateTime. It will then be tested whether the candidate date
     *  time lies within the range given by the query or is equal to the queried date time as appropriate.
     *  If the conversion to OFDateTime is unsuccessful for any of the date times, the result will be
     *  OFFalse.
     */
    static OFBool rangeMatchingDateTime( const void* queryData, const size_t querySize,
                                         const void* candidateData, const size_t candidateSize );

    /** Match the query date and time or date and time range and the candidate date and time using
     *  combined datetime matching.
     *  @param dateQueryData a pointer to a character string that contains a DICOM Date (DA) or
     *    a date range.
     *  @param dateQuerySize the size (in bytes) of the string dateQueryData refers to.
     *  @param timeQueryData a pointer to a character string that contains a DICOM Time (TM) or
     *    a time range.
     *  @param timeQuerySize the size (in bytes) of the string timeQueryData refers to.
     *  @param dateCandidateData a pointer to a character string that contains a DICOM Date (DA).
     *  @param dateCandidateSize the size (in bytes) of the string dateCandidateData refers to.
     *  @param timeCandidateData a pointer to a character string that contains a DICOM Time (TM).
     *  @param timeCandidateSize the size (in bytes) of the string timeCandidateData refers to.
     *  @return OFTrue if the query and the candidate match, OFFalse otherwise.
     *  @details
     *  If dateQuerySize and timeQuerySize equal zero, the result will be OFTrue, as appropriate for
     *  Universal Matching.
     *  Otherwise the query will be converted to an OFDateTime or a range defined by two OFDateTimes and
     *  candidate will be converted to an OFDateTime as defined by combined datetime matching.
     *  It will then be tested whether the candidate date time lies within the range given by the query or is
     *  equal to the queried date time as appropriate.
     *  If the conversion to OFDateTime is unsuccessful for any of the dates or times, the result will
     *  be OFFalse.
     */
    static OFBool rangeMatchingDateTime( const void* dateQueryData, const size_t dateQuerySize,
                                         const void* timeQueryData, const size_t timeQuerySize,
                                         const void* dateCandidateData, const size_t dateCandidateSize,
                                         const void* timeCandidateData, const size_t timeCandidateSize );

    /** Match the query list of UIDs with the candidate UID.
     *  @param queryData a pointer to a character string that contains a backslash separated list of UIDs.
     *  @param querySize the size (in bytes) of the string queryData refers to.
     *  @param candidateData a pointer to a character string that contains an UID.
     *  @param candidateSize the size (in bytes) of the string candidateData refers to.
     *  @return OFTrue if at least one of the UIDs from the query match the candidate, OFFalse otherwise.
     *  @details
     *  If querySize equals zero, the result will be OFTrue, as appropriate for Universal Matching.
     */
    static OFBool listOfUIDMatching( const void* queryData, const size_t querySize,
                                     const void* candidateData, const size_t candidateSize );

    /** Default construct an empty DcmAttributeMatching object.
     */
    DcmAttributeMatching();

    /** Construct an DcmAttributeMatching object that is appropriate for the given VR.
     *  @param vr the DICOM Value Representation of the data that this object will be
     *    used on for matching.
     */
    DcmAttributeMatching( const DcmVR vr );

    /** Test whether this object may be used for matching, i.e.\ whether it is not empty.
     *  @return OFTrue if this object may be used for matching, OFFalse otherwise.
     */
#ifdef HAVE_CXX11
    explicit
#endif
    operator OFBool() const;

    /** Test whether this object cannot be used for matching, i.e.\ whether it is empty.
     *  @return OFTrue if this object cannot be used for matching, OFFalse otherwise.
     */
    OFBool operator!() const;

    /** Match the given query and candidate using the internally stored matching function.
     *  @param queryData a pointer some DICOM data that uses the same VR that was given
     *    during construction of this object.
     *  @param querySize the size (in bytes) of the data queryData refers to.
     *  @param candidateData a pointer to some DICOM data that used the same VR that was
     *    given during construction of this object.
     *  @param candidateSize the size (in bytes) of the data candidateData refers to.
     *  @return compares the query data with the candidate data using the appropriate
     *    matching function for the given VR, returns OFTrue if they match, OFFalse
     *    if not or if an error occurred.
     *  @details
     *  @pre (*this) must have been constructed for a specific VR that matches the one
     *    of the query and candidate data, i.e.\ !(*this) must evaluate to OFFalse.
     *  @details
     *  <h3>Usage Example</h3>
     *  @code{.cpp}
     *  DcmAttributeMatching match( EVR_DA );
     *  match( "20160101-20181231", 17, "20170713", 8 ); // -> OFTrue
     *  match = DcmAttributeMatching( EVR_TM );
     *  match( "-12", 3, "11", 2 ) // -> OFTrue
     *  DcmAttributeMatching( EVR_PN )( "M?ller", 6, "^Martin", 7 ) // -> OFFalse
     *  @endcode
     */
    OFBool operator()( const void* queryData, const size_t querySize,
                       const void* candidateData, const size_t candidateSize ) const;

private:

    /// Helper class for implementing Wild Card Matching
    class WildCardMatcher;

    /** Helper function for generically implementing check functions operating on ranges.
     *  @param check a pointer to a function that checks a single value of the given query.
     *  @param queryData a pointer to some data.
     *  @param querySize the size (in bytes) of the data queryData refers to.
     *  @return logical AND of the results of the given check function applied to the
     *    beginning and the end of the range (as available).
     */
    static OFBool checkRangeQuery( OFBool (*check)(const char*,const size_t),
                                   const void* queryData, const size_t querySize );

    /** Helper template function for generically implementing range matching.
     *  @tparam T the type to parse the data int (e.g. OFDate), deduced automatically.
     *  @param parse a pointer to a function that parses a string as a T.
     *  @param query the already separated (but not parsed) query.
     *  @param candidate the already parsed candidate.
     *  @return OFTrue if parsing of the query was successful and candidate equals
     *    the query value or lies within the range defined by the query. OFFalse otherwise.
     */
    template<typename T>
    static OFBool rangeMatchingTemplate( OFCondition (*parse)(const char*,const size_t,T&),
                                         const Range& query, const T& candidate );

    /** Helper template function for generically implementing range matching.
     *  @tparam T the type to parse the data int (e.g. OFDate), deduced automatically.
     *  @param parse a pointer to a function that parses a string as a T.
     *  @param queryData a pointer to a character string that can be parsed as a T or
     *    a dash separated range of two Ts.
     *  @param querySize the size (in bytes) of the string queryData refers to.
     *  @param candidateData a pointer to a character string that can be parsed as a T
     *    for comparing it with the query.
     *  @param candidateSize the size (in bytes) of the string candidateData refers to.
     *  @return OFTrue if parsing of the query and the candidate as a T was successful
     *    and the T value represented by candidateData equals the one from the query or
     *    lies within the range defined by the query. OFFalse otherwise.
     */
    template<typename T>
    static OFBool rangeMatchingTemplate( OFCondition (*parse)(const char*,const size_t,T&),
                                         const void* queryData, const size_t querySize,
                                         const void* candidateData, const size_t candidateSize);

    /// A pointer to one of the matching functions that is appropriate for the given VR.
    OFBool (*m_pMatch)(const void*,const size_t,const void*,const size_t);
};

#endif // DCMATCH_H
