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
 *  Module:  dcmect
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Tests for dcmiod's checks
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmiod/iodcommn.h"


static OFLogger tCodeLogger = OFLog::getLogger("dcmtk.dcmiod.tests.t_checks");

/** Setup that is run for all test cases.
 *  It creates an IODCommon instance and sets all mandatory attributes besides Modality.
 *  Some missing attribute values are invented when writing (such as Instance UIDs).
 *  @param  iod The IOD to be setup
 *  @return OFTrue if setup was successful, OFFalse otherwise
 *
 */
static OFBool setupTest(DcmIODCommon& iod)
{
     // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return OFFalse;
    }
    // Set mandatory attributes
    if (iod.getSOPCommon().setSOPClassUID(UID_CTImageStorage).bad())
    {
        OFCHECK_FAIL("Cannot set SOP Class UID");
        return OFFalse;
    }
    char forUID[100];
    if (iod.getFrameOfReference().setFrameOfReferenceUID(dcmGenerateUniqueIdentifier(forUID)).bad())
    {
        OFCHECK_FAIL("Cannot set SOP Instance UID");
        return OFFalse;
    }
    return OFTrue;
}

OFTEST(dcmiod_component_check_vr_violated)
{
    DcmIODCommon iod;
    DcmItem destination;
    if (!setupTest(iod))
    {
        return;
    }

    // ----------------------------------------------------------
    // The following test checks whether the write() method of IODCommon fails if attribute value is invalid.
    // Therefore, insert Modality with an invalid value for CS value representation.
    destination.clear();
    OFCHECK(iod.getSeries().setModality("Not Valid", OFFalse /* dont check on insertion */).good());
    OFCHECK(iod.write(destination) == EC_ValueRepresentationViolated);

    // ----------------------------------------------------------
    // The following test checks whether the write() method of IODCommon succeeds
    // if attribute value is violates the Value Reprsentation but checking is turned off.
    // Modality value is still set to "Not valid".
    destination.clear();
    iod.setValueCheckOnWrite(OFFalse);
    OFCHECK(iod.write(destination).good());
}

OFTEST(dcmiod_component_check_vm_violated)
{
    DcmIODCommon iod;
    DcmItem destination;
    if (!setupTest(iod))
    {
        return;
    }

    // ----------------------------------------------------------
    // The following test checks whether the write() method of IODCommon fails with an error
    // if attribute value is violates the Value Multiplicity.
    destination.clear();
    OFCHECK(iod.getSeries().setModality("CT\\MR", OFFalse /* dont check on insertion */).good());
    iod.setValueCheckOnWrite(OFTrue);
    OFCHECK(iod.write(destination) == EC_ValueMultiplicityViolated);

    // ----------------------------------------------------------
    // The following test checks whether the write() method of IODCommon succeeds
    // if attribute value is violates the Value Multiplicity but checking is turned off.
    // Modality value is still set to "CT\\MR".
    destination.clear();
    iod.setValueCheckOnWrite(OFFalse);
    OFCHECK(iod.write(destination).good());
}


OFTEST(dcmiod_component_check_max_length_violated)
{
    DcmIODCommon iod;
    DcmItem destination;
    if (!setupTest(iod))
    {
        return;
    }

    // ----------------------------------------------------------
    // The following test checks whether the write() method of IODCommon fails with an error
    // if attribute value is violates the maximum length of the VR.
    OFCHECK(iod.getSeries().setModality("THIS_EXCEEDS_THE_MAXIMUM_LENGTH", OFFalse /* dont check on insertion */).good());
    iod.setValueCheckOnWrite(OFTrue);
    OFCHECK(iod.write(destination) == EC_MaximumLengthViolated);

    // ----------------------------------------------------------
    // The following test checks whether the write() method of IODCommon succeeds
    // if attribute value is violates the maximum length of the VR but checking is turned off.
    // Modality value is still set to "THIS_EXCEEDS_THE_MAXIMUM_LENGTH".
    destination.clear();
    iod.setValueCheckOnWrite(OFFalse);
    OFCHECK(iod.write(destination).good());
}

OFTEST(dcmiod_component_check_invalid_chars)
{
    DcmIODCommon iod;
    DcmItem destination;
    if (!setupTest(iod))
    {
        return;
    }

    // ----------------------------------------------------------
    // The following test checks whether the write() method of IODCommon fails with an error
    // if attribute value contains invalid characters.
    OFCHECK(iod.getSeries().setModality("ÜS", OFFalse /* dont check on insertion */).good());
    iod.setValueCheckOnWrite(OFTrue);
    OFCHECK(iod.write(destination) == EC_InvalidCharacter);

    // ----------------------------------------------------------
    // The following test checks whether the write() method of IODCommon succeeds
    // if attribute value contains invalid characters but checking is turned off.
    // Modality value is still set to "ÜS".
    destination.clear();
    iod.setValueCheckOnWrite(OFFalse);
    OFCHECK(iod.write(destination).good());
}

OFTEST(dcmiod_component_check_missing_value)
{
    DcmIODCommon iod;
    DcmItem destination;
    if (!setupTest(iod))
    {
        return;
    }

    // ----------------------------------------------------------
    // The following test checks whether the write() method of IODCommon fails with an error
    // if attribute value contains invalid characters.
    OFCHECK(iod.getSeries().setModality("", OFFalse /* dont check on insertion */).good());
    iod.setValueCheckOnWrite(OFTrue);
    OFCHECK(iod.write(destination) == EC_InvalidValue);

    // ----------------------------------------------------------
    // The following test checks whether the write() method of IODCommon succeeds
    // if attribute value contains invalid characters but checking is turned off.
    // Modality value is still set to "".
    destination.clear();
    iod.setValueCheckOnWrite(OFFalse);
    OFCHECK(iod.write(destination) == EC_InvalidValue);
}
