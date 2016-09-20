/*
 *
 *  Copyright (C) 2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    test program for class DSRListOfItems
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"

#include "dcmtk/dcmsr/dsrtlist.h"


OFTEST(dcmsr_addItems)
{
    // disable test case when building shared libraries because of linker errors
#ifndef DCMTK_SHARED
    /* prepare test data */
    OFVector<Uint16> vec;
    for (Uint16 i = 0; i < 10; ++i)
        vec.push_back(i);
    /* add data to list */
    DSRListOfItems<Uint16> lst;
    lst.addItems(vec);
    /* and check result */
    OFCHECK_EQUAL(lst.getNumberOfItems(), 10);
    for (Uint16 j = 0; j < 10; ++j)
        OFCHECK_EQUAL(lst.getItem(j + 1), j);
    OFCHECK(lst.isElement(7));
    OFCHECK(!lst.isElement(10));
#endif
}


OFTEST(dcmsr_getItems)
{
    // disable test case when building shared libraries because of linker errors
#ifndef DCMTK_SHARED
    DSRListOfItems<Uint16> lst;
    /* add data to list */
    for (Uint16 i = 0; i < 10; ++i)
        lst.addItem(i);
    /* and check result */
    OFCHECK_EQUAL(lst.getNumberOfItems(), 10);
    OFVector<Uint16> vec;
    OFCHECK(lst.getItems(vec).good());
    OFCHECK_EQUAL(vec.size(), 10);
    for (Uint16 j = 0; j < 10; ++j)
        OFCHECK_EQUAL(vec.at(j), j);
#endif
}
