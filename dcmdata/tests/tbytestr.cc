/*
 *
 *  Copyright (C) 2024, OFFIS e.V.
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
 *  Author:  Michael Onken
 *
 *  Purpose: test program for DcmByteString
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcdeftag.h"

static void checkValue(DcmElement& elem, const int pos, const char *value, const int vm = -1)
{
    OFString str;
    OFCondition cond = elem.getOFString(str, pos);
    if (cond.bad())
        OFCHECK_FAIL("Failed to get value at pos " << pos << ": " << cond.text());
    if (str != value)
        OFCHECK_FAIL("Expected value (" << value << "), but got (" << str << ")");
    if (vm != -1 )
    {
        unsigned long usVm = OFstatic_cast(unsigned long, vm);
        if (usVm != elem.getVM())
        {
            OFCHECK_FAIL("Expected VM (" << vm << "), but got (" << elem.getVM() << ")");
        }
    }
}

OFTEST(dcmdata_putOFStringAtPos)
{
    DcmCodeString imageType(DCM_ImageType);
    imageType.putOFStringArray("One\\Two\\Three");
    checkValue(imageType, 0, "One");
    checkValue(imageType, 1, "Two");
    checkValue(imageType, 2, "Three", 3);

    // Replace first value and see if it worked
    OFCHECK(imageType.putOFStringAtPos("OneReplaced", 0).good());
    checkValue(imageType, 0, "OneReplaced");
    // Replace second value and see if it worked
    OFCHECK(imageType.putOFStringAtPos("TwoReplaced", 1).good());
    checkValue(imageType, 1, "TwoReplaced");
    // Replace third value and see if it worked
    OFCHECK(imageType.putOFStringAtPos("ThreeReplaced", 2).good());
    checkValue(imageType, 2, "ThreeReplaced");
    // Check if the other values are still there
    checkValue(imageType, 0, "OneReplaced");
    checkValue(imageType, 1, "TwoReplaced");
    checkValue(imageType, 2, "ThreeReplaced", 3);

    // Try to replace a value that doesn't exist
    OFCHECK(imageType.putOFStringAtPos("Four", 4).good());
    checkValue(imageType, 4, "Four", 5);

    // restart with "One\\Two\\Three"
    imageType.putOFStringArray("One\\Two\\Three");

    // Replace last value with an empty string
    OFCHECK(imageType.putOFStringAtPos("", 2).good());
    checkValue(imageType, 2, "");

    // Check whether other values are still there
    checkValue(imageType, 0, "One");
    checkValue(imageType, 1, "Two", 3);

    // Replace value 0 and 1 with empty strings as well
    OFCHECK(imageType.putOFStringAtPos("", 0).good());
    OFCHECK(imageType.putOFStringAtPos("", 1).good());
    checkValue(imageType, 0, "");
    checkValue(imageType, 1, "", 3);
}
