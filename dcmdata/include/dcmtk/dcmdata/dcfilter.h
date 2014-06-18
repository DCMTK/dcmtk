/*
 *
 *  Copyright (C) 2012-2014, OFFIS e.V.
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
 *  Purpose: Class for filtering DcmItem objects regarding specific attributes.
 *
 */


#ifndef DCFILTER_H
#define DCFILTER_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofalgo.h"
#include "dcmtk/dcmdata/dcfilefo.h"


/** Accept or reject DcmItem objects by comparing a specific attribute to a set of values.
 *  DcmAttributeFilter objects can be used to compare a specific attribute with one or more
 *  values - or to accept any DcmItem if no attribute is specified. DcmAttributeFilter
 *  objects can be used like any function taking a DcmItem& or a DcmFileFormat& object as
 *  parameter and returning a boolean value.
 */
class DCMTK_DCMDATA_EXPORT DcmAttributeFilter
{
    /// all actual filter implementations must inherit this interface.
    struct FilterInterface
    {
        /// enable virtual destructor.
        virtual inline ~FilterInterface() {}

        /** apply the actual filter.
         *  @param value the value to be compared with the range of values.
         *  @return OFTrue if value matches one element in the range of values,
         *    OFFalse otherwise.
         */
        virtual OFBool apply( const OFString& value ) = 0;
    };

    /** A range-based filter implementation that compares an attribute's value with a
     *  range of values defined by two iterators.
     *  @tparam Iterator a type matching the STL's ForwardIterator concept.
     */
    template<typename Iterator>
    class IteratorBasedFilter : public FilterInterface
    {
    public:
        /** construct an IteratorBasedFilter object from two iterators.
         *  @param begin an iterator referring to the first element in the range of values.
         *  @param end an iterator referring to one past the end in the range of values.
         */
        IteratorBasedFilter( const Iterator& begin, const Iterator& end )
        : m_Begin( begin ), m_End( end )
        {

        }

        /// @copydoc FilterInterface::apply
        virtual OFBool apply( const OFString& value )
        {
            return OFFind( Iterator, const OFString&, m_Begin, m_End, value ) != m_End;
        }

    private:
        /// disable copy constructor.
        IteratorBasedFilter(const IteratorBasedFilter&);

        /// disable copy assignment.
        IteratorBasedFilter& operator=(const IteratorBasedFilter&);

        /// an iterator referring to the first element in the range of values.
        Iterator const m_Begin;

        /// an iterator referring to one past the end in the range of values.
        Iterator const m_End;
    };

    /** helper function to deduce the Iterator type from a container's begin() and end()
     *  method.
     *  @tparam Iterator a type matching the STL's ForwardIterator concept.
     *  @param begin an iterator referring to the first element in the range of values.
     *  @param end an iterator referring to one past the end in the range of values.
     *  @return a pointer to a newly created IteratorBasedFilter object.
     */
    template<typename Iterator>
    static IteratorBasedFilter<Iterator>* createIteratorBasedFilter( const Iterator& begin,
                                                                     const Iterator& end )
    {
        return new IteratorBasedFilter<Iterator>( begin, end );
    }

public:
    /** construct a DcmAttributeFilter object from two iterators defining the range of
     *  values.
     *  @note each value should be comparable with an object of type OFString.
     *  @tparam Iterator a type matching the STL's ForwardIterator concept.
     *  @param tag a DcmTagKey object determining which attribute of a DcmItem should be
     *    compared with the range of values.
     *  @param begin an iterator referring to the first element in the range of values.
     *  @param end an iterator referring to one past the end in the range of values.
     */
    template<typename Iterator>
    DcmAttributeFilter( const DcmTagKey& tag, const Iterator& begin, const Iterator& end )
    : m_Tag( tag )
    , m_pFilter( createIteratorBasedFilter( begin, end ) )
    {

    }

    /** construct a DcmAttributeFilter object from a container containing the range of
     *  values.
     *  @note each value should be comparable with an object of type OFString.
     *  @tparam Container a class matching the STL's Container concept.
     *  @param tag a DcmTagKey object determining which attribute of a DcmItem should be
     *    compared with the range of values.
     *  @param container a container containing the range of values.
     */
    template<typename Container>
    DcmAttributeFilter( const DcmTagKey& tag, const Container& container )
    : m_Tag( tag )
    , m_pFilter( createIteratorBasedFilter( container.begin(), container.end() ) )
    {

    }

    /// construct a DcmAttributeFilter object that accepts any DcmItem.
    DcmAttributeFilter();

    /** construct a DcmAttributeFilter object that compares the attribute with one
     *  specific value.
     *  @param tag a DcmTagKey object determining which attribute of a DcmItem should be
     *    compared with the value.
     *  @param value an OFString that is to be compared with the actual value stored in
     *    the DcmItem the filter is invoked on.
     *
     */
    DcmAttributeFilter( const DcmTagKey& tag, const OFString& value );

    /** construct a DcmAttributeFilter object that compares the attribute with one
     *  specific value.
     *  @param tag a DcmTagKey object determining which attribute of a DcmItem should
     *    be compared with the value.
     *  @param value a character string that is to be compared with the actual value
     *    stored in the DcmItem the filter is invoked on.
     */
    DcmAttributeFilter( const DcmTagKey& tag, const char* const value );

    /// free all resources used by this object.
    ~DcmAttributeFilter();

    /** apply the filter on a DcmItem.
     *  @param item a DcmItem the filter should accept or reject.
     *  @return OFTrue if the filter accepts the item, OFFalse otherwise.
     */
    OFBool apply( DcmItem& item ) const;

    /** convenience function to enable using DcmAttributeFilter as a functor.
     *  @param item a DcmItem the filter should accept or reject.
     *  @return OFTrue if the filter accepts the item, OFFalse otherwise.
     */
    OFBool operator()( DcmItem& item ) const;

    /** apply the filter on a DICOM file (represented by a DcmFileFormat instance).
     *  @param dcmfile a DcmFileFormat object the filter should accept or reject.
     *  @return OFTrue if the filter accepts the DICOM file, OFFalse otherwise.
     */
    OFBool apply( DcmFileFormat& dcmfile ) const;

    /** convenience function to enable using DcmAttributeFilter as a functor.
     *  @param dcmfile a DcmFileFormat object the filter should accept or reject.
     *  @return OFTrue if the filter accepts the DICOM file, OFFalse otherwise.
     */
    OFBool operator()( DcmFileFormat& dcmfile ) const;

private:
    /// Helper class for storing single values in a DcmAttributeFilter.
    class SingleValueFilter;

    // disable copy constructor.
    DcmAttributeFilter( const DcmAttributeFilter& );

    // disable copy assignment.
    DcmAttributeFilter& operator=( const DcmAttributeFilter& );

    /** a DcmTagKey object determining which attribute of a DcmItem should be compared
     *  with the value.
     */
    const DcmTagKey m_Tag;

    /// a pointer to the actual filter.
    FilterInterface* const m_pFilter;
};

#endif // DCFILTER_H
