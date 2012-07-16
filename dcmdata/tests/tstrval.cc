/*
 *
 *  Copyright (C) 2011, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: test program for various "string value" methods
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcelem.h"


OFTEST(dcmdata_determineVM)
{
    OFCHECK_EQUAL(DcmElement::determineVM(NULL, 0), 0);
    OFCHECK_EQUAL(DcmElement::determineVM("", 0), 0);
    OFCHECK_EQUAL(DcmElement::determineVM(" ", 1), 1);
    OFCHECK_EQUAL(DcmElement::determineVM("\\", 1), 2);
    OFCHECK_EQUAL(DcmElement::determineVM("1\\2", 3), 2);
    OFCHECK_EQUAL(DcmElement::determineVM("1\\2\\", 4), 3);
    OFCHECK_EQUAL(DcmElement::determineVM("\\2\\3\\", 5), 4);
    OFCHECK_EQUAL(DcmElement::determineVM("1\\\\\\4", 5), 4);
    /* also test with embedded NULL byte */
    OFCHECK_EQUAL(DcmElement::determineVM("1\\\\\\\04", 6), 4);
}

OFTEST(dcmdata_getValueFromString)
{
    OFString str;
    /* simple test */
    OFCHECK_EQUAL(DcmElement::getValueFromString("1\\2\\3\\4", 0, 7, str), 2);
    OFCHECK_EQUAL(str, "1");
    OFCHECK_EQUAL(DcmElement::getValueFromString("1\\2\\3\\4", 2, 7, str), 4);
    OFCHECK_EQUAL(str, "2");
    OFCHECK_EQUAL(DcmElement::getValueFromString("1\\2\\3\\4", 4, 7, str), 6);
    OFCHECK_EQUAL(str, "3");
    OFCHECK_EQUAL(DcmElement::getValueFromString("1\\2\\3\\4", 6, 7, str), 8);
    OFCHECK_EQUAL(str, "4");
    /* boundary test */
    OFCHECK_EQUAL(DcmElement::getValueFromString("\\aa\\bbb\\", 0, 8, str), 1);
    OFCHECK_EQUAL(str, "");
    OFCHECK_EQUAL(DcmElement::getValueFromString("\\aa\\bbb\\", 1, 8, str), 4);
    OFCHECK_EQUAL(str, "aa");
    OFCHECK_EQUAL(DcmElement::getValueFromString("\\aa\\bbb\\", 4, 8, str), 8);
    OFCHECK_EQUAL(str, "bbb");
    OFCHECK_EQUAL(DcmElement::getValueFromString("\\aa\\bbb\\", 8, 8, str), 8);
    OFCHECK_EQUAL(str, "");
    /* test with embedded NULL byte */
    OFCHECK_EQUAL(DcmElement::getValueFromString("\\aa\\b\0bb\\", 4, 9, str), 9);
    OFCHECK_EQUAL(str, OFString("b\0bb", 4));
}
