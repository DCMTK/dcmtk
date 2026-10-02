/*
 *
 *  Copyright (C) 2011-2026, OFFIS e.V.
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
        OFCHECK_FAIL("Parsing should have failed with 'Sequence Delimitation Item occurred before Item was completely read', but got: " << cond.text());
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
    testForExpectedVR(dset, DCM_PixelDataProviderURL, useDictionaryVR ? EVR_UR : EVR_LO);
}

OFTEST(dcmdata_parser_wrongExplicitVRinDataset_default)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    testExplicitVRinDataset(OFFalse, OFFalse);
}

OFTEST(dcmdata_parser_wrongExplicitVRinDataset_defaultVR_dictLen)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    testExplicitVRinDataset(OFFalse, OFTrue);
}

OFTEST(dcmdata_parser_wrongExplicitVRinDataset_dictVR_defaultLen)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    testExplicitVRinDataset(OFTrue, OFFalse);
}

OFTEST(dcmdata_parser_wrongExplicitVRinDataset_preferDataDict)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    testExplicitVRinDataset(OFTrue, OFTrue);
}

OFTEST(dcmdata_parser_undefinedLengthUNSequence)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

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

OFTEST(dcmdata_parser_handleAmbiguousVR_fromDictionary)
{
    /* enable setting VR from data dictionary */
    dcmPreferVRFromDataDictionary.set(OFTrue);
    /* create waveform data for OW */
    const Uint8 data[] = {
        TAG_AND_LENGTH_SHORT(DCM_WaveformBitsAllocated, 'U', 'S', 2),
        '\x00', '\x10',
        TAG_AND_LENGTH(DCM_WaveformData, 'O', 'W', 8),
        '\x00', '\x01', '\x00', '\x02', '\x00', '\x03', '\x00', '\x04'
    };
    DcmDataset dset;
    OFCHECK(readDataset(dset, data, sizeof(data), EXS_LittleEndianExplicit).good());
    const Uint16 *uintVals = OFnullptr;
    unsigned long numUints = 0;
    /* check that the data can be accessed as a 16-bit array (regression test, used to fail with EC_IllegalCall) */
    OFCHECK(dset.findAndGetUint16Array(DCM_WaveformData, uintVals, &numUints).good());
    OFCHECK_EQUAL(numUints, 4);
}

/* Test data for the memory exhaustion issue reported in DCMTK issues #148
 * and #1192: a 276-byte DICOM file whose meta-information header specifies
 * Transfer Syntax 1.2.840.10008.1.2 (Implicit VR Little Endian).  The dataset
 * contains a single PatientName element with declared length 0xFFFFFFFE
 * (~4 GB) but only 1 byte of actual value data.  The last five bytes are the
 * length field and the single value byte. */
static const Uint8 hugeLengthPoC[] = {
        /* 128-byte DICOM preamble */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* "DICM" magic */
        0x44, 0x49, 0x43, 0x4d,
        /* (0002,0000) UL File Meta Information Group Length = 123 */
        0x02, 0x00, 0x00, 0x00, 0x55, 0x4c, 0x04, 0x00, 0x7b, 0x00, 0x00, 0x00,
        /* (0002,0001) OB File Meta Information Version */
        0x02, 0x00, 0x01, 0x00, 0x4f, 0x42, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x00, 0x01,
        /* (0002,0002) UI Media Storage SOP Class UID */
        0x02, 0x00, 0x02, 0x00, 0x55, 0x49, 0x1a, 0x00, 0x31, 0x2e,
        0x32, 0x2e, 0x38, 0x34, 0x30, 0x2e, 0x31, 0x30, 0x30, 0x30, 0x38, 0x2e,
        0x35, 0x2e, 0x31, 0x2e, 0x34, 0x2e, 0x31, 0x2e, 0x31, 0x2e, 0x37, 0x00,
        /* (0002,0003) UI Media Storage SOP Instance UID */
        0x02, 0x00, 0x03, 0x00, 0x55, 0x49, 0x12, 0x00, 0x31, 0x2e, 0x32, 0x2e,
        0x33, 0x2e, 0x34, 0x2e, 0x35, 0x2e, 0x36, 0x2e, 0x37, 0x2e, 0x38, 0x2e,
        0x39, 0x00,
        /* (0002,0010) UI Transfer Syntax UID = 1.2.840.10008.1.2 (Implicit VR LE) */
        0x02, 0x00, 0x10, 0x00, 0x55, 0x49, 0x12, 0x00, 0x31, 0x2e,
        0x32, 0x2e, 0x38, 0x34, 0x30, 0x2e, 0x31, 0x30, 0x30, 0x30, 0x38, 0x2e,
        0x31, 0x2e, 0x32, 0x00,
        /* (0002,0012) UI Implementation Class UID (odd length, triggers a warning) */
        0x02, 0x00, 0x12, 0x00, 0x55, 0x49, 0x0f, 0x00,
        0x31, 0x2e, 0x32, 0x2e, 0x33, 0x2e, 0x34, 0x2e, 0x35, 0x2e, 0x36, 0x2e,
        0x37, 0x2e, 0x38,
        /* Dataset (Implicit VR LE):
         * (0010,0010) PatientName, declared length 0xFFFFFFFE (~4 GB), value 'A' */
        0x10, 0x00, 0x10, 0x00, 0xfe, 0xff, 0xff, 0xff, 0x41
};

OFTEST(dcmdata_parser_implicitVR_hugeLengthInBufferStream)
{
    /* Regression test for the memory exhaustion issue reported in DCMTK
     * issues #148 and #1192: an element that declares a huge length while
     * the stream only contains a few bytes of data must not cause a huge
     * memory allocation.
     *
     * When parsed via DcmInputBufferStream (as DCMTK network services do),
     * DcmInputBufferStream::newFactory() returns NULL, so DcmElement::read()
     * proceeds to loadValue().  Since the stream cannot deliver the declared
     * number of value bytes right away, the value is loaded incrementally
     * into a staging buffer that only grows with the data that actually
     * arrives.  The parser therefore allocates no more than a few kilobytes
     * before it detects the premature end of the stream and rejects the
     * data with EC_InvalidStream. */
    DcmInputBufferStream bufstream;
    bufstream.setBuffer(hugeLengthPoC, sizeof(hugeLengthPoC));
    bufstream.setEos();

    DcmFileFormat fileformat;
    fileformat.transferInit();
    OFCondition cond = fileformat.read(bufstream);
    fileformat.transferEnd();

    /* The parser must return EC_InvalidStream because the declared element
     * length far exceeds the available stream data. */
    if (cond != EC_InvalidStream)
    {
        OFCHECK_FAIL("Expected EC_InvalidStream (huge element length in buffer stream), "
                     "but got: " << cond.text());
    }

    /* The incompletely received value must have been discarded: the element
     * must not expose a value buffer that is smaller than its declared length. */
    DcmElement *elem = NULL;
    if (fileformat.getDataset()->findAndGetElement(DCM_PatientName, elem).good() && (elem != NULL))
        OFCHECK(!elem->valueLoaded());
}

/** reads a truncated PatientName element (declared length 1 MiB, 4 bytes of
 *  data) from a DcmInputBufferStream into the given dataset and checks the
 *  outcome.  Reading must fail with EC_InvalidStream in any case; whether the
 *  element keeps a (full-length, partially filled) value buffer depends on
 *  whether the value was loaded incrementally (value discarded) or allocated
 *  up front (historic behavior, value buffer kept).
 *  @param dset dataset to read into (may carry an incremental load threshold)
 *  @param streamThreshold incremental load threshold to set on the stream
 *  @param expectValueLoaded expected result of DcmElement::valueLoaded()
 */
static void readTruncatedElement(DcmDataset &dset,
                                 const Sint32 streamThreshold,
                                 const OFBool expectValueLoaded)
{
    /* Implicit VR LE: (0010,0010) PatientName, declared length 0x00100000 (1 MiB), 4 bytes of data */
    static const Uint8 data[] = {
        0x10, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x41, 0x42, 0x43, 0x44
    };
    DcmInputBufferStream bufstream;
    bufstream.setIncrementalLoadThreshold(streamThreshold);
    bufstream.setBuffer(data, sizeof(data));
    bufstream.setEos();
    dset.transferInit();
    const OFCondition cond = dset.read(bufstream, EXS_LittleEndianImplicit);
    dset.transferEnd();
    if (cond != EC_InvalidStream)
    {
        OFCHECK_FAIL("Expected EC_InvalidStream (truncated element in buffer stream), but got: " << cond.text());
        return;
    }
    DcmElement *elem = NULL;
    if (dset.findAndGetElement(DCM_PatientName, elem).bad() || (elem == NULL))
    {
        OFCHECK_FAIL("PatientName element not found after truncated read");
        return;
    }
    OFCHECK_EQUAL(elem->getLengthField(), 0x00100000u);
    OFCHECK_EQUAL(elem->valueLoaded(), expectValueLoaded);
}

OFTEST(dcmdata_parser_incrementalLoadThreshold_disabled)
{
    /* Verify the runtime configuration of the incremental value loading:
     * with the default threshold (and with any threshold >= 0) a truncated
     * value is discarded, with a threshold of -1 the historic behavior is
     * selected, i.e. a buffer of the full declared length is allocated up
     * front and kept.  The setting can be made on the stream, on the dataset
     * (forwarded to the stream) and on the file format object (forwarded to
     * the dataset). */

    /* default threshold: value discarded */
    {
        DcmDataset dset;
        readTruncatedElement(dset, 0, OFFalse);
    }
    /* explicit (small) threshold on the stream: value discarded */
    {
        DcmDataset dset;
        readTruncatedElement(dset, 16, OFFalse);
    }
    /* incremental loading disabled on the stream: historic behavior */
    {
        DcmDataset dset;
        readTruncatedElement(dset, -1, OFTrue);
    }
    /* incremental loading disabled on the dataset: forwarded to the stream */
    {
        DcmDataset dset;
        dset.setIncrementalLoadThreshold(-1);
        OFCHECK_EQUAL(dset.getIncrementalLoadThreshold(), -1);
        readTruncatedElement(dset, 0, OFTrue);
    }
    /* dataset setting overrides the stream setting */
    {
        DcmDataset dset;
        dset.setIncrementalLoadThreshold(-1);
        readTruncatedElement(dset, 16, OFTrue);
    }
    /* file format object: setting is forwarded to the dataset.  Use the
     * PoC file with the declared length patched down to 1 MiB, so that the
     * up-front allocation of the historic behavior stays small. */
    {
        Uint8 data[sizeof(hugeLengthPoC)];
        memcpy(data, hugeLengthPoC, sizeof(data));
        /* length field (little endian) is located in front of the last byte */
        data[sizeof(data) - 5] = 0x00;
        data[sizeof(data) - 4] = 0x00;
        data[sizeof(data) - 3] = 0x10;
        data[sizeof(data) - 2] = 0x00;

        DcmFileFormat fileformat;
        OFCHECK_EQUAL(fileformat.getIncrementalLoadThreshold(), 0);
        fileformat.setIncrementalLoadThreshold(-1);
        OFCHECK_EQUAL(fileformat.getIncrementalLoadThreshold(), -1);
        OFCHECK_EQUAL(fileformat.getDataset()->getIncrementalLoadThreshold(), -1);

        DcmInputBufferStream bufstream;
        bufstream.setBuffer(data, sizeof(data));
        bufstream.setEos();
        fileformat.transferInit();
        const OFCondition cond = fileformat.read(bufstream);
        fileformat.transferEnd();
        if (cond != EC_InvalidStream)
            OFCHECK_FAIL("Expected EC_InvalidStream (truncated element in buffer stream), but got: " << cond.text());
        DcmElement *elem = NULL;
        if (fileformat.getDataset()->findAndGetElement(DCM_PatientName, elem).good() && (elem != NULL))
            OFCHECK(elem->valueLoaded());
        else
            OFCHECK_FAIL("PatientName element not found after truncated read");
    }
}

OFTEST(dcmdata_parser_incrementalBufferStreamRead)
{
    /* Verify that a legitimate element whose declared length exceeds the
     * bytes currently available in a DcmInputBufferStream can be read
     * incrementally, as DCMTK network services (DIMSE) do when receiving
     * data in multiple PDV fragments.
     *
     * The element's declared length (8191 bytes, odd on purpose to exercise
     * the padding logic) is larger than the first PDV fragment (2048 bytes).
     * The first read stages the data that is available and suspends with
     * EC_StreamNotifyClient.  After releaseBuffer(), the remaining data is
     * provided via setBuffer() + setEos(), and the second read completes
     * successfully. */

    /* Build an Implicit VR LE dataset: PatientName with 8191 bytes of data. */
    static const Uint32 elemLen = 8191;
    Uint8 data[8 + elemLen];
    /* tag (0010,0010) little-endian */
    data[0] = 0x10; data[1] = 0x00;
    data[2] = 0x10; data[3] = 0x00;
    /* length 8191 = 0x00001FFF little-endian */
    data[4] = 0xff; data[5] = 0x1f; data[6] = 0x00; data[7] = 0x00;
    /* value */
    memset(data + 8, 'A', elemLen);

    const size_t totalLen = sizeof(data);

    /* First "PDV fragment": 2048 bytes.  After the 8-byte tag+length header is
     * consumed by the parser, 2040 bytes of value data remain in the buffer.
     * This exceeds the 1024-byte backup buffer inside DcmInputBufferStream. */
    const size_t firstChunk = 2048;

    DcmInputBufferStream bufstream;
    bufstream.setBuffer(data, firstChunk);
    /* NOT calling setEos() — more fragments will follow. */

    DcmDataset dset;
    dset.transferInit();
    OFCondition cond = dset.read(bufstream, EXS_LittleEndianImplicit);

    /* The first read delivers only a partial PDV, so the parser suspends
     * with EC_StreamNotifyClient — more data is expected via setBuffer(). */
    if (cond != EC_StreamNotifyClient)
    {
        OFCHECK_FAIL("First read should return EC_StreamNotifyClient, "
                     "but got: " << cond.text());
        dset.transferEnd();
        return;
    }

    /* Simulate what the DIMSE loop does between PDV fragments:
     * releaseBuffer() saves unconsumed bytes (up to 1024) into the backup
     * buffer so the next setBuffer() can be called. */
    bufstream.releaseBuffer();

    /* Verify that releaseBuffer() succeeds, i.e. the first read consumed
     * enough data that the unconsumed remainder fits in the 1024-byte backup
     * buffer. */
    if (bufstream.status().bad())
    {
        OFCHECK_FAIL("releaseBuffer() failed after first read: "
                     << bufstream.status().text()
                     << " (unconsumed data exceeded backup buffer)");
        dset.transferEnd();
        return;
    }

    /* Feed the remaining data and signal end-of-stream. */
    bufstream.setBuffer(data + firstChunk, totalLen - firstChunk);
    bufstream.setEos();

    cond = dset.read(bufstream, EXS_LittleEndianImplicit);
    dset.transferEnd();
    if (cond.bad())
    {
        OFCHECK_FAIL("Second read should succeed, but got: " << cond.text());
        return;
    }

    /* Verify that the value arrived intact: 8191 'A' characters, correctly
     * terminated (the terminating zero byte is written when the incrementally
     * staged value is completed). */
    const char *str = NULL;
    OFCHECK(dset.findAndGetString(DCM_PatientName, str).good());
    if (str != NULL)
    {
        OFCHECK_EQUAL(strlen(str), 8191);
        OFCHECK(str[0] == 'A' && str[8190] == 'A');
    }
    else
        OFCHECK_FAIL("PatientName value should be readable");
}

/** reads a 64 KiB element that arrives in many small fragments through a
 *  DcmInputBufferStream and verifies that it round-trips unchanged.
 *  @param threshold incremental load threshold to set on the stream
 *    (0 = default, i.e. the value is allocated at once; small values force
 *    the staging buffer to grow several times)
 */
static void readFragmentedElement(const Sint32 threshold)
{
    /* Build an Implicit VR LE dataset: (0042,0011) EncapsulatedDocument (OB)
     * with 65536 bytes of patterned data. */
    static const Uint32 elemLen = 65536;
    static Uint8 data[8 + elemLen];
    /* tag (0042,0011) little-endian */
    data[0] = 0x42; data[1] = 0x00;
    data[2] = 0x11; data[3] = 0x00;
    /* length 65536 = 0x00010000 little-endian */
    data[4] = 0x00; data[5] = 0x00; data[6] = 0x01; data[7] = 0x00;
    /* patterned value (detects any misplaced copy while the buffer grows) */
    for (Uint32 i = 0; i < elemLen; ++i)
        data[8 + i] = OFstatic_cast(Uint8, (i * 31 + 7) & 0xff);

    const size_t totalLen = sizeof(data);
    const size_t fragmentSize = 1024;

    DcmInputBufferStream bufstream;
    bufstream.setIncrementalLoadThreshold(threshold);
    DcmDataset dset;
    dset.transferInit();

    /* feed the data fragment by fragment, as the DIMSE layer does */
    OFCondition cond = EC_Normal;
    size_t offset = 0;
    while (offset < totalLen)
    {
        const size_t chunk = (totalLen - offset < fragmentSize) ? (totalLen - offset) : fragmentSize;
        bufstream.setBuffer(data + offset, chunk);
        offset += chunk;
        if (offset >= totalLen)
            bufstream.setEos();
        cond = dset.read(bufstream, EXS_LittleEndianImplicit);
        if (cond != EC_StreamNotifyClient)
            break;
        bufstream.releaseBuffer();
        if (bufstream.status().bad())
        {
            OFCHECK_FAIL("releaseBuffer() failed: " << bufstream.status().text());
            dset.transferEnd();
            return;
        }
    }
    dset.transferEnd();

    if (cond.bad())
    {
        OFCHECK_FAIL("Reading the fragmented dataset (threshold " << threshold << ") should succeed, but got: " << cond.text());
        return;
    }

    /* verify that the element round-tripped unchanged */
    const Uint8 *readData = NULL;
    unsigned long readLen = 0;
    OFCHECK(dset.findAndGetUint8Array(DCM_EncapsulatedDocument, readData, &readLen).good());
    OFCHECK_EQUAL(readLen, elemLen);
    if (readData != NULL && readLen == elemLen)
        OFCHECK(memcmp(readData, data + 8, elemLen) == 0);
}

OFTEST(dcmdata_parser_manyFragmentsBufferStreamRead)
{
    /* Verify that a large element that arrives in many small fragments is
     * staged correctly: with a small incremental load threshold the staging
     * buffer has to grow several times, and the data must be copied to the
     * correct offsets each time. */

    /* default threshold (64 KiB): the complete value is allocated at once */
    readFragmentedElement(0);
    /* threshold 1 KiB: the staging buffer grows 1 KiB -> 4 KiB -> 16 KiB -> 64 KiB */
    readFragmentedElement(1024);
    /* minimum threshold: the first allocation is driven by the available data only */
    readFragmentedElement(1);
}

#ifdef WITH_ZLIB

OFTEST(dcmdata_parser_deflatedBufferStreamRead)
{
    /* Verify that a legitimate deflated dataset that is completely available
     * in a DcmInputBufferStream (setEos() has been called) is parsed
     * correctly even when it contains an element that is larger than the
     * decompressor's output buffer.
     *
     * With a compression filter installed, avail() only reports the bytes
     * currently held in the decompressor's output buffer (at most 4 KB),
     * not the total remaining data.  Elements larger than that are therefore
     * read via the incremental staging buffer, which grows as the
     * decompressor delivers more data. */

    /* Build a dataset with an 8192-byte OB element and write it deflated. */
    static const Uint32 elemLen = 8192;
    Uint8 elemData[elemLen];
    memset(elemData, 'A', elemLen);

    DcmDataset dsetOut;
    OFCHECK(dsetOut.putAndInsertUint8Array(DCM_EncapsulatedDocument, elemData, elemLen).good());

    Uint8 outBuf[16384];
    DcmOutputBufferStream outStream(outBuf, sizeof(outBuf));
    dsetOut.transferInit();
    OFCondition cond = dsetOut.write(outStream, EXS_DeflatedLittleEndianExplicit,
        EET_ExplicitLength, NULL);
    dsetOut.transferEnd();
    if (cond.bad())
    {
        OFCHECK_FAIL("Writing the deflated dataset failed: " << cond.text());
        return;
    }
    /* flush the stream including the embedded compression codec, then
     * retrieve the compressed data (which empties the stream's buffer) */
    outStream.flush();
    void *compData = NULL;
    offile_off_t compLen = 0;
    outStream.flushBuffer(compData, compLen);
    OFCHECK(outStream.isFlushed());
    /* the deflated dataset must be complete and smaller than the original */
    OFCHECK(compLen > 0 && compLen < OFstatic_cast(offile_off_t, elemLen));

    /* Read it back from a buffer stream that has all data and EOS set,
     * as a network service would do for a single-PDV deflated dataset. */
    DcmInputBufferStream bufstream;
    bufstream.setBuffer(compData, compLen);
    bufstream.setEos();

    DcmDataset dsetIn;
    dsetIn.transferInit();
    cond = dsetIn.read(bufstream, EXS_DeflatedLittleEndianExplicit);
    dsetIn.transferEnd();
    if (cond.bad())
    {
        OFCHECK_FAIL("Reading the deflated dataset should succeed, but got: " << cond.text());
        return;
    }

    /* verify that the element round-tripped unchanged */
    const Uint8 *readData = NULL;
    unsigned long readLen = 0;
    OFCHECK(dsetIn.findAndGetUint8Array(DCM_EncapsulatedDocument, readData, &readLen).good());
    OFCHECK_EQUAL(readLen, elemLen);
    if (readData != NULL && readLen == elemLen)
        OFCHECK(memcmp(readData, elemData, elemLen) == 0);
}

#endif /* WITH_ZLIB */
