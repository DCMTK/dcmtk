/*
 *
 *  Copyright (C) 2012, OFFIS e.V.
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
 *  Purpose: Implementation of DcmAttributeFilter
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcfilter.h"


class DcmAttributeFilter::SingleValueFilter : public FilterInterface
{
public:
    SingleValueFilter( const OFString& value )
    : m_Value( value )
    {

    }

    virtual OFBool apply( const OFString& value )
    {
        return m_Value == value;
    }

private:
    const OFString m_Value;
};

DcmAttributeFilter::DcmAttributeFilter()
: m_Tag(), m_pFilter( NULL )
{

}

DcmAttributeFilter::DcmAttributeFilter( const DcmTagKey& tag, const OFString& value )
: m_Tag( tag ), m_pFilter( new SingleValueFilter( value ) )
{

}

DcmAttributeFilter::DcmAttributeFilter( const DcmTagKey& tag, const char* const value )
: m_Tag( tag ), m_pFilter( new SingleValueFilter( value ) )
{

}

DcmAttributeFilter::~DcmAttributeFilter()
{
    delete m_pFilter;
}

OFBool DcmAttributeFilter::apply( DcmItem& item ) const
{
    if( m_pFilter )
    {
        OFString value;
        return item.findAndGetOFStringArray( m_Tag, value ).good()
            && m_pFilter->apply( value );
    }
    else return true;
}

OFBool DcmAttributeFilter::operator()( DcmItem& item ) const
{
    return apply( item );
}

OFBool DcmAttributeFilter::apply( DcmFileFormat& dcmfile ) const
{
    DcmDataset* const dataset = dcmfile.getDataset();
    return dataset && apply( *dataset );
}

OFBool DcmAttributeFilter::operator()( DcmFileFormat& dcmfile ) const
{
    return apply( dcmfile );
}
