/*
 *
 *  Copyright (C) 2026, OFFIS e.V.
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
 *  Purpose: test program for sequence nesting depth limit
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/oftempf.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcistrmb.h"
#include "dcmtk/dcmdata/dcostrmb.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcwcache.h"


/** Helper: build a DcmDataset containing a chain of nested sequences to the
 *  specified depth using the high-level dcmdata API.  Each nesting level
 *  consists of a DcmSequenceOfItems with one DcmItem, and the innermost
 *  item is empty.
 *  @param depth the number of nested sequence levels to generate
 *  @param dset  output dataset (cleared before use)
 */
static void buildNestedDataset(Uint32 depth, DcmDataset& dset)
{
    dset.clear();
    /* start with the dataset as the outermost item */
    DcmItem* currentItem = &dset;
    for (Uint32 i = 0; i < depth; ++i)
    {
        DcmSequenceOfItems* seq = new DcmSequenceOfItems(DCM_ReferencedSeriesSequence);
        currentItem->insert(seq);
        DcmItem* item = new DcmItem();
        seq->insert(item);
        currentItem = item;
    }
}


/** Helper: serialize a dataset to a byte buffer and parse it back using a
 *  fresh DcmInputStream, with an optional custom nesting depth limit.
 *  @param srcDset        the dataset to serialize
 *  @param dstDset        output dataset to receive the parsed result
 *  @param maxNestingDepth nesting depth setting passed to setMaxNestingDepth():
 *         0 (default) = compile-time default (DCMTK_MAX_SEQUENCE_NESTING), -1 = unlimited, > 0 = custom limit
 *  @return the OFCondition returned by DcmDataset::read()
 */
static OFCondition serializeAndParse(DcmDataset& srcDset, DcmDataset& dstDset,
                                     Sint32 maxNestingDepth = 0)
{
    /* write the dataset to a memory buffer */
    const size_t bufLen = 1024 * 1024;
    Uint8* buf = new Uint8[bufLen];

    DcmOutputBufferStream outStream(buf, bufLen);
    srcDset.transferInit();
    DcmWriteCache wcache;
    OFCondition cond = srcDset.write(outStream, EXS_LittleEndianExplicit, EET_UndefinedLength, &wcache);
    srcDset.transferEnd();
    if (cond.bad())
    {
        delete[] buf;
        return cond;
    }
    offile_off_t bytesWritten = 0;
    void* writtenBuf = NULL;
    outStream.flushBuffer(writtenBuf, bytesWritten);

    /* parse it back with nesting depth limit */
    DcmInputBufferStream inStream;
    inStream.setBuffer(buf, bytesWritten);
    inStream.setEos();
    inStream.setMaxNestingDepth(maxNestingDepth);

    dstDset.clear();
    dstDset.transferInit();
    cond = dstDset.read(inStream, EXS_LittleEndianExplicit);
    dstDset.transferEnd();

    delete[] buf;
    return cond;
}


OFTEST(dcmdata_nestingDepthLimit_exceeded)
{
    /* One level beyond the default limit (DCMTK_MAX_SEQUENCE_NESTING) must be rejected. */
    DcmDataset srcDset, dstDset;
    buildNestedDataset(DCMTK_MAX_SEQUENCE_NESTING + 1, srcDset);

    OFCondition cond = serializeAndParse(srcDset, dstDset);
    if (cond != EC_NestingDepthLimitExceeded)
    {
        OFCHECK_FAIL("Expected EC_NestingDepthLimitExceeded for " << (DCMTK_MAX_SEQUENCE_NESTING + 1)
            << " levels of nesting, but got: " << cond.text());
    }
}


OFTEST(dcmdata_nestingDepthLimit_atLimit)
{
    /* Exactly DCMTK_MAX_SEQUENCE_NESTING levels must still pass. */
    DcmDataset srcDset, dstDset;
    buildNestedDataset(DCMTK_MAX_SEQUENCE_NESTING, srcDset);

    OFCondition cond = serializeAndParse(srcDset, dstDset);
    if (cond.bad())
    {
        OFCHECK_FAIL("Parsing " << DCMTK_MAX_SEQUENCE_NESTING
            << " levels of nesting should succeed, but got: " << cond.text());
    }
}


OFTEST(dcmdata_nestingDepthLimit_wellBelow)
{
    /* 5 levels of nesting (typical real-world depth) must parse fine. */
    DcmDataset srcDset, dstDset;
    buildNestedDataset(5, srcDset);

    OFCondition cond = serializeAndParse(srcDset, dstDset);
    if (cond.bad())
    {
        OFCHECK_FAIL("Parsing 5 levels of nesting should succeed, but got: " << cond.text());
    }
}


OFTEST(dcmdata_nestingDepthLimit_customLimit)
{
    /* Test setMaxNestingDepth(): set the limit to 10.
     * 10 levels must succeed, 11 levels must fail. */
    DcmDataset srcDset, dstDset;
    OFCondition cond;

    buildNestedDataset(10, srcDset);
    cond = serializeAndParse(srcDset, dstDset, 10);
    if (cond.bad())
    {
        OFCHECK_FAIL("Parsing 10 levels with maxNestingDepth=10 should succeed, but got: " << cond.text());
    }

    buildNestedDataset(11, srcDset);
    cond = serializeAndParse(srcDset, dstDset, 10);
    if (cond != EC_NestingDepthLimitExceeded)
    {
        OFCHECK_FAIL("Expected EC_NestingDepthLimitExceeded for 11 levels with maxNestingDepth=10, but got: " << cond.text());
    }
}


OFTEST(dcmdata_nestingDepthLimit_disabled)
{
    /* Test that setMaxNestingDepth(-1) disables the limit entirely.
     * Even 200 levels of nesting must succeed. */
    DcmDataset srcDset, dstDset;
    buildNestedDataset(200, srcDset);

    OFCondition cond = serializeAndParse(srcDset, dstDset, -1);
    if (cond.bad())
    {
        OFCHECK_FAIL("Parsing 200 levels with maxNestingDepth=-1 (unlimited) should succeed, but got: " << cond.text());
    }
}


/** Helper: create a temporary file path.
 *  @param tmpFile receives the temporary file path
 *  @return EC_Normal if successful
 */
static OFCondition makeTempFile(OFString& tmpFile)
{
    return OFTempFile::createFile(tmpFile, NULL /* fd_out */, O_RDWR,
        "" /* dir */, "" /* prefix */, ".dcm" /* postfix */);
}


OFTEST(dcmdata_nestingDepthLimit_datasetAPI)
{
    /* Test DcmDataset::setMaxNestingDepth() with loadFile(). */
    DcmDataset srcDset;
    OFString tmpFile;
    OFCondition cond;

    /* save as raw dataset (no meta header) */
    buildNestedDataset(11, srcDset);
    cond = makeTempFile(tmpFile);
    OFCHECK(cond.good());
    cond = srcDset.saveFile(tmpFile, EXS_LittleEndianExplicit);
    OFCHECK(cond.good());

    /* default limit (DCMTK_MAX_SEQUENCE_NESTING): 11 levels should succeed */
    DcmDataset dset1;
    cond = dset1.loadFile(tmpFile, EXS_LittleEndianExplicit);
    if (cond.bad())
    {
        OFCHECK_FAIL("DcmDataset::loadFile() with 11 levels should succeed with default limit, but got: " << cond.text());
    }

    /* custom limit 10: 11 levels should fail */
    DcmDataset dset2;
    dset2.setMaxNestingDepth(10);
    cond = dset2.loadFile(tmpFile, EXS_LittleEndianExplicit);
    if (cond != EC_NestingDepthLimitExceeded)
    {
        OFCHECK_FAIL("DcmDataset::loadFile() with 11 levels and maxNestingDepth=10 should fail, but got: " << cond.text());
    }

    OFStandard::deleteFile(tmpFile);
}


OFTEST(dcmdata_nestingDepthLimit_fileFormatAPI)
{
    /* Test DcmFileFormat::setMaxNestingDepth() with loadFile(). */
    DcmDataset srcDset;
    OFString tmpFile;
    OFCondition cond;

    /* save as DcmFileFormat (with meta header) */
    buildNestedDataset(11, srcDset);
    cond = makeTempFile(tmpFile);
    OFCHECK(cond.good());
    DcmFileFormat srcFF(&srcDset);
    cond = srcFF.saveFile(tmpFile, EXS_LittleEndianExplicit);
    OFCHECK(cond.good());

    /* default limit (DCMTK_MAX_SEQUENCE_NESTING): 11 levels should succeed */
    DcmFileFormat ff1;
    cond = ff1.loadFile(tmpFile);
    if (cond.bad())
    {
        OFCHECK_FAIL("DcmFileFormat::loadFile() with 11 levels should succeed with default limit, but got: " << cond.text());
    }

    /* custom limit 10: 11 levels should fail */
    DcmFileFormat ff2;
    ff2.setMaxNestingDepth(10);
    cond = ff2.loadFile(tmpFile);
    if (cond != EC_NestingDepthLimitExceeded)
    {
        OFCHECK_FAIL("DcmFileFormat::loadFile() with 11 levels and maxNestingDepth=10 should fail, but got: " << cond.text());
    }

    OFStandard::deleteFile(tmpFile);
}
