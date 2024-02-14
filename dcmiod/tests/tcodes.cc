/*
 *
 *  Copyright (C) 2019-2024, OFFIS e.V.
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
 *  Module:  dcmect
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Tests for dcmiod's Code Sequence Macro implementation
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmiod/iodmacro.h"


static OFLogger tCodeLogger = OFLog::getLogger("dcmtk.test.t_codes");

static void clearCode(CodeSequenceMacro& c);

OFTEST(dcmiod_codes)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    // Most basic, classic "Code Value" variant
    CodeSequenceMacro c("Value", "99DCMTK", "Classic code", "Version 1");
    OFCHECK(c.check().good());
    OFString val;
    OFCHECK(c.getCodeValue(val, -1, OFTrue /* autoTag */).good());
    OFCHECK(val == "Value");
    OFCHECK(c.getCodeValue(val, -1, OFFalse /* autoTag off */).good());
    OFCHECK(val == "Value");
    OFCHECK(c.getCodingSchemeDesignator(val).good());
    OFCHECK(val == "99DCMTK");
    OFCHECK(c.getCodeMeaning(val).good());
    OFCHECK(val == "Classic code");
    OFCHECK(c.getCodingSchemeVersion(val).good());
    OFCHECK(val == "Version 1");
    OFCHECK(c.getLongCodeValue(val).bad());
    OFCHECK(c.getURNCodeValue(val).bad());
    OFCHECK(c.check().good());

    clearCode(c);
    val.clear();

    // "URN Code Value" variant
    OFCHECK(c.set("http://www.open-connections.de", "99DCMTK", "URN code", "Version 1", OFTrue, OFTrue /* autoTag */)
                .good());
    OFCHECK(c.check().good());
    OFCHECK(c.getCodeValue(val, -1, OFTrue /* autoTag */).good());
    OFCHECK(val == "http://www.open-connections.de");
    val.clear();
    OFCHECK(c.getCodeValue(val, -1, OFFalse /* autoTag off */).bad());
    OFCHECK(val.empty());
    OFCHECK(c.getCodingSchemeDesignator(val).good());
    OFCHECK(val == "99DCMTK");
    OFCHECK(c.getCodeMeaning(val).good());
    OFCHECK(val == "URN code");
    OFCHECK(c.getCodingSchemeVersion(val).good());
    OFCHECK(val == "Version 1");
    OFCHECK(c.getLongCodeValue(val).bad());
    OFCHECK(c.getURNCodeValue(val).good());
    OFCHECK(val == "http://www.open-connections.de");
    OFCHECK(c.check().good());

    clearCode(c);
    val.clear();

    // "Long Code Value" variant
    OFCHECK(c.set("Open Connections GmbH", "99DCMTK", "Long code", "Version 1", OFTrue, OFTrue /* autoTag */).good());
    OFCHECK(c.check().good());
    OFCHECK(c.getCodeValue(val, -1, OFTrue /* autoTag */).good());
    OFCHECK(val == "Open Connections GmbH");
    val.clear();
    OFCHECK(c.getCodeValue(val, -1, OFFalse /* autoTag off */).bad());
    OFCHECK(val.empty());
    OFCHECK(c.getCodingSchemeDesignator(val).good());
    OFCHECK(val == "99DCMTK");
    OFCHECK(c.getCodeMeaning(val).good());
    OFCHECK(val == "Long code");
    OFCHECK(c.getCodingSchemeVersion(val).good());
    OFCHECK(val == "Version 1");
    OFCHECK(c.getURNCodeValue(val).bad());
    OFCHECK(c.getLongCodeValue(val).good());
    OFCHECK(val == "Open Connections GmbH");
    OFCHECK(c.check().good());

    clearCode(c);
    val.clear();

    // Check only single code value is set internally (old values are deleted)
    OFCHECK(c.set("http://www.open-connections.de", "99DCMTK", "URN code", "Version 1", OFTrue, OFTrue /* autoTag */)
                .good());
    OFCHECK(c.set("Value", "99DCMTK", "Classic code", "Version 1").good());
    OFCHECK(c.getLongCodeValue(val).bad());
    OFCHECK(c.getURNCodeValue(val).bad());

    clearCode(c);
    val.clear();

    OFCHECK(c.set("http://www.open-connections.de", "99DCMTK", "URN code", "Version 1", OFTrue, OFTrue /* autoTag */)
                .good());
    OFCHECK(c.set("Open Connections GmbH", "99DCMTK", "Long code", "Version 1", OFTrue, OFTrue /* autoTag */).good());
    OFCHECK(c.getURNCodeValue(val).bad());
    OFCHECK(c.getCodeValue(val, -1, OFFalse /*autoTag off */).bad());

    clearCode(c);
    val.clear();

    // Failure: URN Code Value without autoTag
    OFCHECK(c.set("http://www.open-connections.de/this/url/is/over/64/characters/long",
                  "99DCMTK",
                  "URN code",
                  "Version 1",
                  OFTrue,
                  OFFalse /* autoTag off */)
                .bad());
    OFCHECK(c.check().bad());

    clearCode(c);
    val.clear();

    // Failure: Long Code Value without autoTag
    OFCHECK(c.set("Open Connections GmbH, Oldenburg, Germany, c/o Name over 64 characters long",
                  "99DCMTK",
                  "URN code",
                  "Version 1",
                  OFTrue,
                  OFFalse /* autoTag off */)
                .bad());
    OFCHECK(c.check().bad());
}

static void clearCode(CodeSequenceMacro& c)
{
    c.clearData();
    OFString val;
    c.getCodeValue(val);
    OFCHECK(val.empty());
    c.getURNCodeValue(val);
    OFCHECK(val.empty());
    c.getLongCodeValue(val);
    OFCHECK(val.empty());
    c.getCodingSchemeDesignator(val);
    OFCHECK(val.empty());
    c.getCodeMeaning(val);
    OFCHECK(val.empty());
    c.getCodingSchemeVersion(val);
    OFCHECK(val.empty());
}
