/*
 *
 *  Copyright (C) 2011-2012, OFFIS e.V.
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

static const DcmTagKey wrongExplicitVRinDataset_unknownTag(0x0006, 0x0006);

static const Uint8 wrongExplicitVRinDataset_testData[] = {
    /* This non-standard tag cannot be corrected (not in data dictionary) */
    TAG_AND_LENGTH_SHORT(wrongExplicitVRinDataset_unknownTag, 'P', 'N', 4),
    'A', 'B', 'C', 'D',
    /* This standard tag has a wrong VR ("ST" instead of "PN") */
    TAG_AND_LENGTH_SHORT(DCM_PatientName, 'S', 'T', 4),
    'A', 'B', 'C', 'D',
    /* This standard tag has a wrong VR ("UN") and uses a 4-byte length field */
    TAG_AND_LENGTH(DCM_PatientID, 'U', 'N', 4),
    '0', '8', '1', '5',
    /* This standard tag has a correct VR, no modification required */
    TAG_AND_LENGTH_SHORT(DCM_PatientSex, 'C', 'S', 2),
    'O', ' '
};

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

OFTEST(dcmdata_parser_wrongExplicitVRinDataset_default)
{
    DcmDataset dset;
    OFCondition cond;

    // This should use the VR from the dataset (default)
    dcmPreferVRFromDataDictionary.set(OFFalse);
    cond = readDataset(dset, wrongExplicitVRinDataset_testData, sizeof(wrongExplicitVRinDataset_testData), TRANSFER_SYNTAX);
    if (cond.good())
    {
        OFLOG_DEBUG(tparserLogger, DcmObject::PrintHelper(dset));
        testForExpectedVR(dset, wrongExplicitVRinDataset_unknownTag, EVR_PN);
        testForExpectedVR(dset, DCM_PatientName, EVR_ST);
        testForExpectedVR(dset, DCM_PatientID, EVR_UN);
        testForExpectedVR(dset, DCM_PatientSex, EVR_CS);
    } else {
        OFCHECK_FAIL(cond.text());
    }
}

OFTEST(dcmdata_parser_wrongExplicitVRinDataset_preferDataDict)
{
    DcmDataset dset;
    OFCondition cond;

    // This should use the VR from the data dictionary
    dcmPreferVRFromDataDictionary.set(OFTrue);
    cond = readDataset(dset, wrongExplicitVRinDataset_testData, sizeof(wrongExplicitVRinDataset_testData), TRANSFER_SYNTAX);
    if (cond.good())
    {
        OFLOG_DEBUG(tparserLogger, DcmObject::PrintHelper(dset));
        testForExpectedVR(dset, wrongExplicitVRinDataset_unknownTag, EVR_PN);
        testForExpectedVR(dset, DCM_PatientName, EVR_PN);
        testForExpectedVR(dset, DCM_PatientID, EVR_LO);
        testForExpectedVR(dset, DCM_PatientSex, EVR_CS);
    } else {
        OFCHECK_FAIL(cond.text());
    }
    // Reset to the default value
    dcmPreferVRFromDataDictionary.set(OFFalse);
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
