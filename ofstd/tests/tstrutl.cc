/*
 *
 *  Copyright (C) 2019, OFFIS e.V.
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
 *  Author:  Michael Onken
 *
 *  Purpose: Tests for string utilities
 *
 */

#include "dcmtk/config/osconfig.h"     /* include OS specific configuration first */

#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstrutl.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"


OFTEST(ofstd_OFStringUtil_replace_all)
{
    // Pattern found

    OFString s("Jon");
    OFStringUtil::replace_all(s, "Jon", "Doe");
    OFCHECK_EQUAL(s, "Doe");

    s = "Jon Doe";
    OFStringUtil::replace_all(s, "Jon", "Doe");
    OFCHECK_EQUAL(s, "Doe Doe");

    s = "blablabla";
    OFStringUtil::replace_all(s, "b", "p");
    OFCHECK_EQUAL(s, "plaplapla");

    s = "plaplapla";
    OFStringUtil::replace_all(s, "pla", "");
    OFCHECK_EQUAL(s, "");

    s = "blablabla";
    OFStringUtil::replace_all(s, "bl", "b");
    OFCHECK_EQUAL(s, "bababa");

    s = "blablabla";
    OFStringUtil::replace_all(s, "bla", "bla");
    OFCHECK_EQUAL(s, "blablabla");

    s = "blablabla";
    OFStringUtil::replace_all(s, "b", "bo");
    OFCHECK_EQUAL(s, "bolabolabola");

    // Pattern found and part of replacement

    s = "blabla";
    OFStringUtil::replace_all(s, "bla", "blabla");
    OFCHECK_EQUAL(s, "blablablabla");

    // Pattern not found

    s = "Jon";
    OFStringUtil::replace_all(s, "Doe", "Doe");
    OFCHECK_EQUAL(s, "Jon");

    s = "";
    OFStringUtil::replace_all(s, "Doe", "Doe");
    OFCHECK_EQUAL(s, "");

    s = "blablabla";
    OFStringUtil::replace_all(s, "Doe", "");
    OFCHECK_EQUAL(s, "blablabla");

    // Empty pattern or source (should have no effect)
    s = "blablabla";
    OFStringUtil::replace_all(s, "", "notfound");
    OFCHECK_EQUAL(s, "blablabla");

    s = "";
    OFStringUtil::replace_all(s, "", "notfound");
    OFCHECK_EQUAL(s, "");

    s = "This is the home of the brother of my brother";
    OFStringUtil::replace_all(s, "brother", "sister");
    OFCHECK_EQUAL(s, "This is the home of the sister of my sister");

}
