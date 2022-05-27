/*
 *
 *  Copyright (C) 1997-2022, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Andreas Barth
 *
 *  Purpose: test program for class OFStack
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofstack.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"


OFTEST(ofstd_OFStack)
{
    OFStack<int> st;
    int i;
    st.push(1);
    st.push(2);
    st.push(3);

    OFStack<int> nst(st);

    OFCHECK_EQUAL(st.size(), 3);
    for (i = 3; i >= 1; i--)
    {
        OFCHECK(!st.empty());
        OFCHECK_EQUAL(i, st.top());
        st.pop();
    }

    OFCHECK_EQUAL(nst.size(), 3);
    for (i = 3; i >= 1; i--)
    {
        OFCHECK(!nst.empty());
        OFCHECK_EQUAL(i, nst.top());
        nst.pop();
    }
}
