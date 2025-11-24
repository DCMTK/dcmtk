/*
 *
 *  Copyright (C) 2025, OFFIS e.V.
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
 *  Module:  dcmiod
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Tests for dcmiod macros
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmiod/iodmacro.h"


OFTEST(dcmiod_content_identification_macro)
{
    ContentIdentificationMacro macro;
    OFCHECK((macro.check(OFTrue /* quiet */).bad()));

    OFCHECK(macro.setContentCreatorName("OC").good());
    OFCHECK(macro.setContentDescription("Test Description").good());
    OFCHECK(macro.setContentLabel("TEST_LABEL").good());
    OFCHECK(macro.setInstanceNumber("42").good());

    OFCHECK(macro.check(OFTrue /* quiet */).good());
    OFString str;

    // Check all strings
    OFCHECK(macro.getContentCreatorName(str).good());
    OFCHECK(str == "OC");
    OFCHECK(macro.getContentDescription(str).good());
    OFCHECK(str == "Test Description");
    OFCHECK(macro.getContentLabel(str).good());
    OFCHECK(str == "TEST_LABEL");
    OFCHECK(macro.getInstanceNumber(str).good());
    OFCHECK(str == "42");

    // Write and re-read
    DcmItem item;
    OFCHECK(macro.write(item).good());
    ContentIdentificationMacro macro2;
    OFCHECK(macro2.read(item).good());
    OFCHECK(macro2.check(OFTrue /* quiet */).good());

    // Check strings in macro2
    OFCHECK(macro2.getContentCreatorName(str).good());
    OFCHECK(str == "OC");
    OFCHECK(macro2.getContentDescription(str).good());
    OFCHECK(str == "Test Description");
    OFCHECK(macro2.getContentLabel(str).good());
    OFCHECK(str == "TEST_LABEL");
    OFCHECK(macro2.getInstanceNumber(str).good());
    OFCHECK(str == "42");
}
