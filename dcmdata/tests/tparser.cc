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
 *  Author:  Uli Schlachter
 *
 *  Purpose: test program for reading DICOM datasets
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2011-08-23 14:24:31 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcistrmb.h"
#include "dcmtk/dcmdata/dcostrmb.h"


static OFLogger tparserLogger = OFLog::getLogger("dcmtk.test.tparser");

/* Macros for handling the endian */
#define LITTLE_ENDIAN_UINT16(w) ((w) & 0xff), ((w) >> 8)
#define LITTLE_ENDIAN_UINT32(w) LITTLE_ENDIAN_UINT16((w) & 0xffff), LITTLE_ENDIAN_UINT16((w) >> 16)

#define BIG_ENDIAN_UINT16(w) ((w) >> 8), ((w) & 0xff)
#define BIG_ENDIAN_UINT32(w) BIG_ENDIAN_UINT16((w) >> 16), BIG_ENDIAN_UINT16((w) & 0xffff)

/* Various definitions for "fake generating" DICOM datasets */
#define RESERVED_BYTES 0, 0
#define TAG(tag) ENDIAN_UINT16((tag).getGroup()), ENDIAN_UINT16((tag).getElement())
#define TAG_AND_LENGTH(tag, v, r, length) \
    TAG(tag), v, r, RESERVED_BYTES, ENDIAN_UINT32(length)
#define TAG_AND_LENGTH2(tag, v, r, length) \
    TAG(tag), v, r, ENDIAN_UINT16(length)
#define ITEM(length) \
    TAG(DCM_Item), ENDIAN_UINT32(length)

#define UNDEFINED_LENGTH  0xffffffff
#define UNDEFINED_LENGTH2 0xffff
#define VALUE 0x11

/* To switch between big and little endian, just change these macros */
#define TRANSFER_SYNTAX EXS_LittleEndianExplicit
#define ENDIAN_UINT16(w) LITTLE_ENDIAN_UINT16(w)
#define ENDIAN_UINT32(w) LITTLE_ENDIAN_UINT32(w)

static OFCondition readDataset(DcmDataset& dset, const Uint8* buffer, size_t length)
{
    DcmInputBufferStream stream;
    stream.setBuffer(buffer, length);
    stream.setEos();

    dset.clear();
    dset.transferInit();
    OFCondition cond = dset.read(stream, TRANSFER_SYNTAX);
    dset.transferEnd();

    return cond;
}

OFTEST(dcmdata_parser_missingSequenceDelimitationItem)
{
    const Uint8 data[] = {
        TAG_AND_LENGTH(DCM_PixelData, 'O', 'B', UNDEFINED_LENGTH),
        ITEM(4),
        VALUE, VALUE, VALUE, VALUE
        /* DCM_SequenceDelimitationItem missing */
    };
    DcmDataset dset;
    OFCondition cond;

    // This should assume the above sequence is complete
    dcmIgnoreParsingErrors.set(OFTrue);
    cond = readDataset(dset, data, sizeof(data));
    if (cond.bad())
    {
        OFCHECK_FAIL("Parsing should have worked, but got error: " << cond.text());
    }

    // This should complain about the missing item
    dcmIgnoreParsingErrors.set(OFFalse);
    cond = readDataset(dset, data, sizeof(data));
    if (cond != EC_DelimitationItemMissing)
    {
        OFCHECK_FAIL("Parsing should have failed with 'DelimitationItemMissing', but got: " << cond.text());
    }
}

static void testOddLengthPartialValue(const Uint8* data, size_t length)
{
    const unsigned int bytesToRead = 5;
    DcmFileFormat dfile;
    OFCondition cond;
    DcmElement *elem;
    Uint8 buf[bytesToRead];
    int randomInt = rand();
    char temp[60]; // should be enough also for ints with 128 bit...
    if (sprintf(temp, "dcmdata_tests%u.tmp", randomInt) == 0)
    {
      OFCHECK_FAIL("Unable to create temporary filename");
      return;
    }
    // Deferred value loading only works with files
    OFFile f;
    f.fopen(temp, "wb");
    f.fwrite(data, 1, length);
    f.fclose();

    // Everything larger than 1 byte won't be loaded now but only later
    cond = dfile.loadFile(temp, TRANSFER_SYNTAX, EGL_noChange, 1, ERM_dataset);
    if (cond.bad())
    {
        OFCHECK_FAIL(cond.text());
        unlink(temp);
        return;
    }

    cond = dfile.getDataset()->findAndGetElement(DCM_PixelData, elem);
    if (cond.bad())
    {
        OFCHECK_FAIL(cond.text());
        unlink(temp);
        return;
    }

    // This is what we are actually testing for. The bug was that
    // getPartialValue() tried to load one more byte than what was available.
    // This resulted in an "Invalid stream" error.
    cond = elem->getPartialValue(buf, 0, bytesToRead);
    if (cond.bad())
    {
        OFCHECK_FAIL(cond.text());
        unlink(temp);
        return;
    }

    for (unsigned int i = 0; i < bytesToRead; i++)
    {
        OFCHECK_EQUAL(buf[i], VALUE);
    }
    unlink(temp);
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
    TAG_AND_LENGTH2(wrongExplicitVRinDataset_unknownTag, 'P', 'N', 4),
    'A', 'B', 'C', 'D',
    /* This standard tag has a wrong VR ("ST" instead of "PN") */
    TAG_AND_LENGTH2(DCM_PatientName, 'S', 'T', 4),
    'A', 'B', 'C', 'D',
    /* This standard tag has a wrong VR ("UN") and uses a 4-byte length field */
    TAG_AND_LENGTH(DCM_PatientID, 'U', 'N', 4),
    '0', '8', '1', '5',
    /* This standard tag has a correct VR, no modification required */
    TAG_AND_LENGTH2(DCM_PatientSex, 'C', 'S', 2),
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
    cond = readDataset(dset, wrongExplicitVRinDataset_testData, sizeof(wrongExplicitVRinDataset_testData));
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
    cond = readDataset(dset, wrongExplicitVRinDataset_testData, sizeof(wrongExplicitVRinDataset_testData));
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
}


/*
 *
 * CVS/RCS Log:
 * $Log: tparser.cc,v $
 * Revision 1.7  2011-08-23 14:24:31  joergr
 * Fixed another inconsistent test name.
 *
 * Revision 1.6  2011-08-08 11:04:06  joergr
 * Added test cases for new parser flag "dcmPreferVRFromDataDictionary".
 *
 * Revision 1.5  2011-08-01 19:39:54  onken
 * Fixed CVS log at end of file.
 *
 * Revision 1.4  2011-08-01 19:36:40  onken
 * Added deletion of temporary files in case tests fail.
 *
 * Revision 1.3  2011-08-01 19:32:58  onken
 * Changed random file name generation to avoid warning about mktemp on
 * linux systems.
 *
 * Revision 1.2  2011-07-07 12:51:18  joergr
 * Added missing extern "C" declaration for mktemp(), e.g. required for MSVC++.
 *
 * Revision 1.1  2011-07-06 12:25:03  uli
 * Added tests for some bugs which we recently fixed.
 *
 *
 */
