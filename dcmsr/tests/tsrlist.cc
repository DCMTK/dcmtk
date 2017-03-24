/*
 *
 *  Copyright (C) 2016-2017, J. Riesmeier, Oldenburg, Germany
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
#include "dcmtk/ofstd/ofdiag.h"

#include "dcmtk/dcmsr/dsrtlist.h"


// small dummy struct to use as list element, ensuring no other instance
// of DSRListOfItems with this element type exists as a workaround for
// Visual Studio not understanding C++ templates (again)
struct Elem
{
#include DCMTK_DIAGNOSTIC_PUSH
#include DCMTK_DIAGNOSTIC_IGNORE_SHADOW
    Elem(int i = 0) : i(i) {}
#include DCMTK_DIAGNOSTIC_POP
    OFBool operator==(const Elem& rhs) const {return i == rhs.i;}
    int i;
};


// provide EmptyItem for instantiation
template<>
const Elem& DSRgetEmptyItem<Elem>()
{
    static const Elem e;
    return e;
}


OFTEST(dcmsr_addItems)
{
    /* prepare test data */
    OFVector<Elem> vec;
    for (int i = 0; i < 10; ++i)
        vec.push_back(i);
    /* add data to list */
    DSRListOfItems<Elem> lst;
    lst.addItems(vec);
    /* and check result */
    OFCHECK_EQUAL(lst.getNumberOfItems(), 10);
    for (int j = 0; j < 10; ++j)
        OFCHECK_EQUAL(lst.getItem(j + 1).i, j);
    OFCHECK(lst.isElement(7));
    OFCHECK(!lst.isElement(10));
}


OFTEST(dcmsr_getItems)
{
    DSRListOfItems<Elem> lst;
    /* add data to list */
    for (int i = 0; i < 10; ++i)
        lst.addItem(i);
    /* and check result */
    OFCHECK_EQUAL(lst.getNumberOfItems(), 10);
    OFVector<Elem> vec;
    OFCHECK(lst.getItems(vec).good());
    OFCHECK_EQUAL(vec.size(), 10);
    for (int j = 0; j < 10; ++j)
        OFCHECK_EQUAL(vec.at(j).i, j);
}
