/*
 *
 *  Copyright (C) 2011-2013, OFFIS e.V.
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
 *  Author:  Uli Schlachter
 *
 *  Purpose: test program for reading DICOM datasets
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/oftempf.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcistrmb.h"
#include "dcmtk/dcmdata/dcostrmb.h"
#include "dctmacro.h"


/* To switch between big and little endian, just change these macros */
#define TRANSFER_SYNTAX EXS_LittleEndianExplicit
#define ENDIAN_UINT16(w) LITTLE_ENDIAN_UINT16(w)
#define ENDIAN_UINT32(w) LITTLE_ENDIAN_UINT32(w)


static OFLogger tparserLogger = OFLog::getLogger("dcmtk.test.tparser");

OFTEST(dcmdata_parser_missingDelimitationItems)
{
    const Uint8 data[] = {
        TAG_AND_LENGTH(DCM_IconImageSequence, 'S', 'Q', UNDEFINED_LENGTH),
        ITEM(UNDEFINED_LENGTH),
        TAG_AND_LENGTH(DCM_PixelData, 'O', 'B', 4),
        VALUE, VALUE, VALUE, VALUE,
        /* Delimitation Items missing at the end of the stream */
    };
    DcmDataset dset;
    OFCondition cond;

    // This should assume the above sequence is complete
    dcmIgnoreParsingErrors.set(OFTrue);
    cond = readDataset(dset, data, sizeof(data), TRANSFER_SYNTAX);
    if (cond.bad())
    {
        OFCHECK_FAIL("Parsing should have worked, but got error: " << cond.text());
    }

    // This should complain about the missing item
    dcmIgnoreParsingErrors.set(OFFalse);
    cond = readDataset(dset, data, sizeof(data), TRANSFER_SYNTAX);
    if (cond != EC_SequDelimitationItemMissing)
    {
        OFCHECK_FAIL("Parsing should have failed with 'Sequence Delimitation Item missing', but got: " << cond.text());
    }
}

OFTEST(dcmdata_parser_missingSequenceDelimitationItem_1)
{
    const Uint8 data[] = {
        TAG_AND_LENGTH(DCM_PixelData, 'O', 'B', UNDEFINED_LENGTH),
        ITEM(4),
        VALUE, VALUE, VALUE, VALUE
        /* SequenceDelimitationItem missing at the end of the stream */
    };
    DcmDataset dset;
    OFCondition cond;

    // This should assume the above sequence is complete
    dcmIgnoreParsingErrors.set(OFTrue);
    cond = readDataset(dset, data, sizeof(data), TRANSFER_SYNTAX);
    if (cond.bad())
    {
        OFCHECK_FAIL("Parsing should have worked, but got error: " << cond.text());
    }

    // This should complain about the missing item
    dcmIgnoreParsingErrors.set(OFFalse);
    cond = readDataset(dset, data, sizeof(data), TRANSFER_SYNTAX);
    if (cond != EC_SequDelimitationItemMissing)
    {
        OFCHECK_FAIL("Parsing should have failed with 'Sequence Delimitation Item missing', but got: " << cond.text());
    }
}

OFTEST(dcmdata_parser_missingSequenceDelimitationItem_2)
{
    const Uint8 data[] = {
        TAG_AND_LENGTH(DCM_IconImageSequence, 'S', 'Q', UNDEFINED_LENGTH),
        ITEM(UNDEFINED_LENGTH),
        TAG_AND_LENGTH(DCM_PixelData, 'O', 'B', 4),
        VALUE, VALUE, VALUE, VALUE,
        ITEM_END,
        /* SequenceDelimitationItem missing, but there's further data after the sequence */
        TAG_AND_LENGTH(DCM_DataSetTrailingPadding, 'O', 'B', 4),
        VALUE, VALUE, VALUE, VALUE
    };
    DcmDataset dset;
    OFCondition cond;

    // This should fail with a specific error code
    dcmIgnoreParsingErrors.set(OFFalse);
    cond = readDataset(dset, data, sizeof(data), TRANSFER_SYNTAX);
    if (cond != EC_SequDelimitationItemMissing)
    {
        OFCHECK_FAIL("Parsing should have failed with 'Sequence Delimitation Item missing', but got: " << cond.text());
    }
}

OFTEST(dcmdata_parser_wrongDelimitationItemForSequence)
{
    const Uint8 data[] = {
        TAG_AND_LENGTH(DCM_IconImageSequence, 'S', 'Q', UNDEFINED_LENGTH),
        ITEM(UNDEFINED_LENGTH),
        TAG_AND_LENGTH(DCM_PixelData, 'O', 'B', 4),
        VALUE, VALUE, VALUE, VALUE,
        ITEM_END,
        /* wrong delimitation item */
        ITEM_END,
        TAG_AND_LENGTH(DCM_DataSetTrailingPadding, 'O', 'B', 4),
        VALUE, VALUE, VALUE, VALUE
    };
    DcmDataset dset;
    OFCondition cond;

    // This should complain about the wrong delimitation item
    dcmIgnoreParsingErrors.set(OFFalse);
    dcmReplaceWrongDelimitationItem.set(OFFalse);
    cond = readDataset(dset, data, sizeof(data), TRANSFER_SYNTAX);
    if (cond != EC_SequDelimitationItemMissing)
    {
        OFCHECK_FAIL("Parsing should have failed with 'Sequence Delimitation Item missing', but got: " << cond.text());
    }

    // This should assume the above sequence encoding is correct
    dcmIgnoreParsingErrors.set(OFTrue);
    cond = readDataset(dset, data, sizeof(data), TRANSFER_SYNTAX);
    if (cond.good())
    {
        OFLOG_DEBUG(tparserLogger, DcmObject::PrintHelper(dset));
        if (dset.card() != 1)
            OFCHECK_FAIL("There should be exactly 1 element on the main dataset level, but " << dset.card() << " found");
    } else {
        OFCHECK_FAIL("Parsing should have worked, but got error: " << cond.text());
    }

    // This should replace the wrong delimitation item
    dcmIgnoreParsingErrors.set(OFFalse);
    dcmReplaceWrongDelimitationItem.set(OFTrue);
    cond = readDataset(dset, data, sizeof(data), TRANSFER_SYNTAX);
    if (cond.good())
    {
        OFLOG_DEBUG(tparserLogger, DcmObject::PrintHelper(dset));
        if (dset.card() != 2)
            OFCHECK_FAIL("There should be exactly 2 elements on the main dataset level, but " << dset.card() << " found");
    } else {
        OFCHECK_FAIL("Parsing should have worked, but got error: " << cond.text());
    }
}

OFTEST(dcmdata_parser_wrongDelimitationItemForItem)
{
    const Uint8 data[] = {
        TAG_AND_LENGTH(DCM_IconImageSequence, 'S', 'Q', UNDEFINED_LENGTH),
        ITEM(UNDEFINED_LENGTH),
        TAG_AND_LENGTH(DCM_PixelData, 'O', 'B', 4),
        VALUE, VALUE, VALUE, VALUE,
        /* wrong delimitation item */
        SEQUENCE_END,
        SEQUENCE_END,
        TAG_AND_LENGTH(DCM_DataSetTrailingPadding, 'O', 'B', 4),
        VALUE, VALUE, VALUE, VALUE
    };
    DcmDataset dset;
    OFCondition cond;

    // This should complain about the wrong delimitation item
    dcmIgnoreParsingErrors.set(OFFalse);
    dcmReplaceWrongDelimitationItem.set(OFFalse);
    cond = readDataset(dset, data, sizeof(data), TRANSFER_SYNTAX);
    if (cond != EC_ItemDelimitationItemMissing)
    {
        OFCHECK_FAIL("Parsing should have failed with 'Item Delimitation Item missing', but got: " << cond.text());
    }

    // This should assume the above item encoding is correct
    dcmIgnoreParsingErrors.set(OFTrue);
    cond = readDataset(dset, data, sizeof(data), TRANSFER_SYNTAX);
    if (cond.good())
    {
        OFLOG_DEBUG(tparserLogger, DcmObject::PrintHelper(dset));
        if (dset.card() != 2)
            OFCHECK_FAIL("There should be exactly 2 elements on the main dataset level, but " << dset.card() << " found");
    } else {
        OFCHECK_FAIL("Parsing should have worked, but got error: " << cond.text());
    }

    // This should replace the wrong delimitation item
    dcmIgnoreParsingErrors.set(OFFalse);
    dcmReplaceWrongDelimitationItem.set(OFTrue);
    cond = readDataset(dset, data, sizeof(data), TRANSFER_SYNTAX);
    if (cond.good())
    {
        OFLOG_DEBUG(tparserLogger, DcmObject::PrintHelper(dset));
        if (dset.card() != 2)
            OFCHECK_FAIL("There should be exactly 2 elements on the main dataset level, but " << dset.card() << " found");
    } else {
        OFCHECK_FAIL("Parsing should have worked, but got error: " << cond.text());
    }
}

OFTEST(dcmdata_parser_explicitItemLengthTooLarge)
{
    /* Produce some test data: Sequence with single item, containing a single element. */
    const Uint8 data[] = {
        IMPLICIT_TAG_AND_LENGTH(DCM_RequestAttributesSequence, UNDEFINED_LENGTH),
        ITEM(12), // 4 for tag, 4 for length, and 4 bytes extra that should produce the error
        IMPLICIT_TAG_AND_LENGTH(DCM_CodeValue, 0),
        SEQUENCE_END
    };
    DcmDataset dset;
    OFCondition cond;

    // This should complain about the item being too large
    dcmIgnoreParsingErrors.set(OFFalse);
    cond = readDataset(dset, data, sizeof(data), EXS_LittleEndianImplicit);
    if (cond != EC_PrematureSequDelimitationItem)
    {
        OFCHECK_FAIL("Parsing should have failed with 'Sequence Delimitation Item occured before Item was completely read', but got: " << cond.text());
    }

    // This should ignore the error during parsing
    dcmIgnoreParsingErrors.set(OFTrue);
    cond = readDataset(dset, data, sizeof(data), EXS_LittleEndianImplicit);
    if (cond.bad())
    {
        OFCHECK_FAIL("Parsing should have worked, but got error: " << cond.text());
    }
}

static void testOddLengthPartialValue(const Uint8* data, size_t length)
{
    const unsigned int bytesToRead = 5;
    DcmFileFormat dfile;
    OFCondition cond;
    DcmElement *elem;
    Uint8 buf[bytesToRead];
    OFTempFile temp;

    if (temp.getStatus().bad())
    {
        OFCHECK_FAIL("Could not create temporary file: " << temp.getStatus().text());
        return;
    }

    // Deferred value loading only works with files
    OFFile f;
    f.fopen(temp.getFilename(), "wb");
    f.fwrite(data, 1, length);
    f.fclose();

    // Everything larger than 1 byte won't be loaded now but only later
    cond = dfile.loadFile(temp.getFilename(), TRANSFER_SYNTAX, EGL_noChange, 1, ERM_dataset);
    if (cond.bad())
    {
        OFCHECK_FAIL(cond.text());
        return;
    }

    cond = dfile.getDataset()->findAndGetElement(DCM_PixelData, elem);
    if (cond.bad())
    {
        OFCHECK_FAIL(cond.text());
        return;
    }

    // This is what we are actually testing for. The bug was that
    // getPartialValue() tried to load one more byte than what was available.
    // This resulted in an "Invalid stream" error.
    cond = elem->getPartialValue(buf, 0, bytesToRead);
    if (cond.bad())
    {
        OFCHECK_FAIL(cond.text());
        return;
    }

    for (unsigned int i = 0; i < bytesToRead; i++)
    {
        OFCHECK_EQUAL(buf[i], VALUE);
    }
}

OFTEST(dcmdata_parser_oddLengthPartialValue_lastItem)
{
    const Uint8 data[] = {
        /* This needs an odd length and OW for the bug that we are testing */
        TAG_AND_LENGTH(DCM_PixelData, 'O', 'W', 5),
        VALUE, VALUE, VALUE, VALUE, VALUE
    };

    testOddLengthPartialValue(data, sizeof(data));
}

OFTEST(dcmdata_parser_oddLengthPartialValue_notLastItem)
{
    const Uint8 data[] = {
        /* This needs an odd length and OW for the bug that we are testing */
        TAG_AND_LENGTH(DCM_PixelData, 'O', 'W', 5),
        VALUE, VALUE, VALUE, VALUE, VALUE,
        /* This time something else is behind the tested element */
        TAG_AND_LENGTH(DCM_DataSetTrailingPadding, 'O', 'B', 4),
        VALUE, VALUE, VALUE, VALUE
    };

    testOddLengthPartialValue(data, sizeof(data));
}

static const DcmTagKey wrongExplicitVRinDataset_unknownTag1(0x0006, 0x0006);
static const DcmTagKey wrongExplicitVRinDataset_unknownTag2(0x0006, 0x0008);

#define WRONG_EXPLICIT_VR_COMMON \
    /* This non-standard tag cannot be corrected (not in data dictionary, short length field) */ \
    TAG_AND_LENGTH_SHORT(wrongExplicitVRinDataset_unknownTag1, 'P', 'N', 4), \
    'A', 'B', 'C', 'D', \
    /* This non-standard tag cannot be corrected (not in data dictionary, long length field) */ \
    TAG_AND_LENGTH(wrongExplicitVRinDataset_unknownTag2, 'U', 'T', 4), \
    'E', 'F', 'G', 'H', \
    /* This standard tag has a wrong VR ("ST" instead of "PN") */ \
    TAG_AND_LENGTH_SHORT(DCM_PatientName, 'S', 'T', 4), \
    'I', 'J', 'K', 'L', \
    /* This standard tag has a correct VR, no modification required */ \
    TAG_AND_LENGTH_SHORT(DCM_PatientSex, 'C', 'S', 2), \
    'O', ' '

static const Uint8 wrongExplicitVRinDataset_default_testData[] = {
    WRONG_EXPLICIT_VR_COMMON,
    /* This standard tag has a wrong VR ("UN") and does not use the "PN" length field size */
    TAG_AND_LENGTH(DCM_PatientBirthName, 'U', 'N', 4),
    '0', '8', '1', '5',
    /* This standard tag has a wrong VR ("LO") and does not use the "UT" length field size */
    TAG_AND_LENGTH_SHORT(DCM_PixelDataProviderURL, 'L', 'O', 4),
    '4', '2', '4', '2',
};

static const Uint8 wrongExplicitVRinDataset_dictLen_testData[] = {
    WRONG_EXPLICIT_VR_COMMON,
    /* This standard tag has a wrong VR ("UN") and uses the "PN" length field size */
    TAG_AND_LENGTH_SHORT(DCM_PatientBirthName, 'U', 'N', 4),
    '0', '8', '1', '5',
    /* This standard tag has a wrong VR ("LO") and uses the "UT" length field size */
    TAG_AND_LENGTH(DCM_PixelDataProviderURL, 'L', 'O', 4),
    '4', '2', '4', '2',
};
#undef WRONG_EXPLICIT_VR_COMMON

static void testForExpectedVR(DcmDataset &dset, const DcmTagKey &tag, const DcmEVR vr)
{
    DcmElement *elem;
    OFCondition cond = dset.findAndGetElement(tag, elem);
    if (cond.good())
    {
        /* check whether the VR generated by the parser is the expected one */
        OFCHECK_EQUAL(elem->getVR(), vr);
    } else {
        OFCHECK_FAIL(cond.text());
    }
}

static void testExplicitVRinDataset(OFBool useDictionaryVR, OFBool useDictionaryVRLen)
{
    DcmDataset dset;
    OFCondition cond;
    const OFConditionConst *expectedError;
    const Uint8 *broken, *working;
    size_t brokenLength, workingLength;

    if (useDictionaryVRLen) {
        broken = wrongExplicitVRinDataset_default_testData;
        brokenLength = sizeof(wrongExplicitVRinDataset_default_testData);
        working = wrongExplicitVRinDataset_dictLen_testData;
        workingLength = sizeof(wrongExplicitVRinDataset_dictLen_testData);
        // This gets the length field wrong and interprets an element's value as
        // the beginning of the following element; then complains about the length
        expectedError = NULL;
    } else {
        working = wrongExplicitVRinDataset_default_testData;
        workingLength = sizeof(wrongExplicitVRinDataset_default_testData);
        broken = wrongExplicitVRinDataset_dictLen_testData;
        brokenLength = sizeof(wrongExplicitVRinDataset_dictLen_testData);
        // This gets the length field wrong and tries to allocate a huge element
        expectedError = &EC_MemoryExhausted;
    }

    dcmPreferLengthFieldSizeFromDataDictionary.set(useDictionaryVRLen);
    dcmPreferVRFromDataDictionary.set(useDictionaryVR);

    // The broken version shouldn't be parseable
    cond = readDataset(dset, broken, brokenLength, TRANSFER_SYNTAX);
    if (cond != EC_InvalidStream && (expectedError == NULL || cond != *expectedError))
    {
        OFCHECK_FAIL("Dataset should fail, but got: " << cond.text());
        if (cond.good())
            OFLOG_DEBUG(tparserLogger, DcmObject::PrintHelper(dset));
    }

    // The good version should work
    cond = readDataset(dset, working, workingLength, TRANSFER_SYNTAX);

    // Reset to the default values
    dcmPreferLengthFieldSizeFromDataDictionary.set(OFFalse);
    dcmPreferVRFromDataDictionary.set(OFFalse);
    if (cond.bad()) {
        OFCHECK_FAIL(cond.text());
        return;
    }
    OFLOG_DEBUG(tparserLogger, DcmObject::PrintHelper(dset));
    testForExpectedVR(dset, wrongExplicitVRinDataset_unknownTag1, EVR_PN);
    testForExpectedVR(dset, wrongExplicitVRinDataset_unknownTag2, EVR_UT);
    testForExpectedVR(dset, DCM_PatientName, useDictionaryVR ? EVR_PN : EVR_ST);
    testForExpectedVR(dset, DCM_PatientBirthName, useDictionaryVR ? EVR_PN : EVR_UN);
    testForExpectedVR(dset, DCM_PixelDataProviderURL, useDictionaryVR ? EVR_UT : EVR_LO);
}

OFTEST(dcmdata_parser_wrongExplicitVRinDataset_default)
{
    testExplicitVRinDataset(OFFalse, OFFalse);
}

OFTEST(dcmdata_parser_wrongExplicitVRinDataset_defaultVR_dictLen)
{
    testExplicitVRinDataset(OFFalse, OFTrue);
}

OFTEST(dcmdata_parser_wrongExplicitVRinDataset_dictVR_defaultLen)
{
    testExplicitVRinDataset(OFTrue, OFFalse);
}

OFTEST(dcmdata_parser_wrongExplicitVRinDataset_preferDataDict)
{
    testExplicitVRinDataset(OFTrue, OFTrue);
}

OFTEST(dcmdata_parser_undefinedLengthUNSequence)
{
    const Uint8 data[] = {
        // Sequence with undefined length and VR UN => gets read as implicit TS
        TAG_AND_LENGTH(DCM_IconImageSequence, 'U', 'N', UNDEFINED_LENGTH),
        ITEM(UNDEFINED_LENGTH),
        IMPLICIT_TAG_AND_LENGTH(DCM_PatientName, 4),
        'A', 'B', 'C', 'D',
        IMPLICIT_TAG_AND_LENGTH(DCM_PixelData, 2),
        VALUE, VALUE,
        ITEM_END,
        SEQUENCE_END
    };
    DcmDataset dset;
    OFCondition cond;

    cond = readDatasetTwice(dset, data, sizeof(data), EXS_JPEGProcess14, EXS_JPEGProcess14);
    if (cond.good())
    {
        OFLOG_DEBUG(tparserLogger, DcmObject::PrintHelper(dset));
        OFCHECK_EQUAL(1, dset.card());

        DcmElement *elem = NULL;
        cond = dset.findAndGetElement(DCM_PatientName, elem, OFTrue);
        if (cond.good())
            OFCHECK(elem != NULL);
        else
            OFCHECK_FAIL("There should have been a PatientName item, but: " << cond.text());
    } else {
        OFCHECK_FAIL("Parsing should have worked, but got error: " << cond.text());
        return;
    }
}
