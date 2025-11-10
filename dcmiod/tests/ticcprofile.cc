/*
 *
 *  Copyright (C) 2024-2025, OFFIS e.V.
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
 *  Purpose: Tests for dcmiod's checks
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmiod/modiccprofile.h"
#include "dcmtk/ofstd/oftest.h"


static const Uint8 ICC_LENGTH = 100;
static const OFString COLOR_SPACE("SRGB");

static void fillICCProfile(IODICCProfileModule& mod);
static void checkICCProfile(IODICCProfileModule& mod);
static void checkReadWriteICCProfile(IODICCProfileModule& mod);

static OFLogger tLog = OFLog::getLogger("dcmtk.test.ticcprofile");


/** Make sure dictionary is loaded
 *  @return OFTrue if dictionary is loaded, OFFalse otherwise
 */
static OFBool checkDictionary()
{
     // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return OFFalse;
    }
    return OFTrue;
}

OFTEST(dcmiod_icc_profile_module)
{
    OFCHECK(checkDictionary());
    IODICCProfileModule mod;

    fillICCProfile(mod);
    checkICCProfile(mod);
    checkReadWriteICCProfile(mod);
}


static void fillICCProfile(IODICCProfileModule& mod)
{
    Uint8* iccProfile = new Uint8[ICC_LENGTH];
    for (Uint8 i = 0; i < ICC_LENGTH; i++)
    {
        iccProfile[i] = i;
    }
    OFCHECK(mod.setICCProfile(iccProfile, ICC_LENGTH, OFTrue /* check */).good());
    OFCHECK(mod.setColorSpace(COLOR_SPACE, OFTrue /* check */).good());

    delete[] iccProfile;
}


static void checkICCProfile(IODICCProfileModule& mod)
{
    const Uint8* iccProfile = NULL;
    Uint32 length = 0;
    OFCHECK(mod.getICCProfile(iccProfile, length).good());
    OFCHECK(length == ICC_LENGTH);
    for (unsigned long i = 0; i < ICC_LENGTH; i++)
    {
        OFCHECK(iccProfile[i] == i);
    }
    OFString colorSpace;
    OFCHECK(mod.getColorSpace(colorSpace).good());
    OFCHECK(colorSpace == COLOR_SPACE);
    delete [] iccProfile; // clean up, this is a copy
}


static void checkReadWriteICCProfile(IODICCProfileModule& mod)
{
    // Write to item and check all values using dcmdata API
    DcmItem item;
    OFCHECK(mod.write(item).good());
    const Uint8* profile = NULL;
    unsigned long count = 0;;
    OFCHECK(item.findAndGetUint8Array(DCM_ICCProfile, profile, &count).good());
    OFCHECK(count == ICC_LENGTH);
    for (unsigned long i = 0; i < ICC_LENGTH; i++)
    {
        OFCHECK(profile[i] == i);
    }
    OFString colorSpace;
    OFCHECK(item.findAndGetOFString(DCM_ColorSpace, colorSpace).good());
    OFCHECK(colorSpace == COLOR_SPACE);

    // Read from item and check all values from module class
    IODICCProfileModule mod2;
    OFCHECK(mod2.read(item).good());
    checkICCProfile(mod2);
}