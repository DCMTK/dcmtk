/*
 *
 *  Copyright (C) 2012-2020, OFFIS e.V.
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
 *  Purpose: test program for class DcmAttributeFilter
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcfilter.h"
#include "dcmtk/dcmdata/dcdict.h"


OFTEST( dcmdata_attribute_filter )
{
    // make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    DcmAttributeFilter ct_filter( DCM_SOPClassUID, UID_CTImageStorage );
    DcmItem item;

    item.putAndInsertString( DCM_SOPClassUID, UID_CTImageStorage );
    OFCHECK( ct_filter( item ) ); // test filter matches
    item.putAndInsertString( DCM_SOPClassUID, UID_ComputedRadiographyImageStorage );
    OFCHECK( !ct_filter( item ) ); // test filter fails
    // test anonymous filter object
    OFCHECK( DcmAttributeFilter( DCM_SOPClassUID, UID_ComputedRadiographyImageStorage )( item ) );

#if !defined(_MSC_VER) || _MSC_VER > 1200 // iterator based filters not supported for VS <= 6.0.
    // test iterator (array) based range
    const char* filter_range[3] =
    {
        UID_ComputedRadiographyImageStorage,
        UID_CTImageStorage,
        UID_MRImageStorage
    };

    // create filter from range (iterators)
    DcmAttributeFilter cr_ct_mr_filter( DCM_SOPClassUID, filter_range + 0, filter_range + 3 );

    // test container-based (OFVector) range
    OFVector<OFString> filter_vector;
    filter_vector.push_back( UID_CTImageStorage );
    filter_vector.push_back( UID_MRImageStorage );

    // create filter from range (container)
    DcmAttributeFilter ct_mr_filter( DCM_SOPClassUID, filter_vector );

    // test all range-based filters
    OFCHECK( cr_ct_mr_filter( item ) );
    OFCHECK( !ct_mr_filter( item ) );
    item.putAndInsertString( DCM_SOPClassUID, UID_CTImageStorage );
    OFCHECK( cr_ct_mr_filter( item ) );
    OFCHECK( ct_mr_filter( item ) );
    item.putAndInsertString( DCM_SOPClassUID, UID_MRImageStorage );
    OFCHECK( cr_ct_mr_filter( item ) );
    OFCHECK( ct_mr_filter( item ) );
    item.putAndInsertString( DCM_SOPClassUID, UID_EnhancedCTImageStorage );
    OFCHECK( !cr_ct_mr_filter( item ) );
#endif

    OFCHECK( DcmAttributeFilter()( item ) ); // test default constructed (allow any) filter.
}
