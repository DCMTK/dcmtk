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
 *  Purpose: test program for the maximum element value length limit
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/oftempf.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcistrmb.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmdata/dcdeftag.h"


/** Helper: write an Implicit VR LE element header for PatientName
 *  (0010,0010) with the given value length into the given buffer.
 *  @param buf         target buffer, must hold at least 8 bytes
 *  @param valueLength value length to announce (little endian)
 *  @return number of bytes written (always 8)
 */
static size_t writeElementHeader(Uint8 *buf, Uint32 valueLength)
{
    buf[0] = 0x10; buf[1] = 0x00; buf[2] = 0x10; buf[3] = 0x00;
    buf[4] = OFstatic_cast(Uint8, valueLength & 0xff);
    buf[5] = OFstatic_cast(Uint8, (valueLength >> 8) & 0xff);
    buf[6] = OFstatic_cast(Uint8, (valueLength >> 16) & 0xff);
    buf[7] = OFstatic_cast(Uint8, (valueLength >> 24) & 0xff);
    return 8;
}


/** Helper: parse an Implicit VR LE dataset from a memory buffer with an
 *  optional custom element length limit.
 *  @param data             buffer holding the dataset
 *  @param length           number of bytes in the buffer
 *  @param dset             output dataset receiving the parsed result
 *  @param maxElementLength element length setting passed to
 *         setMaxElementLength(): 0 (default) = compile-time default
 *         (DCMTK_MAX_ELEMENT_LENGTH), > 0 = custom limit
 *  @param complete         if OFTrue (default), setEos() is called, i.e.
 *         the buffer holds the complete dataset
 *  @return the OFCondition returned by DcmDataset::read()
 */
static OFCondition parseFromBuffer(const Uint8 *data, size_t length,
                                   DcmDataset &dset, Uint32 maxElementLength,
                                   OFBool complete = OFTrue)
{
    DcmInputBufferStream inStream;
    inStream.setBuffer(data, OFstatic_cast(offile_off_t, length));
    if (complete)
        inStream.setEos();
    if (maxElementLength != 0)
        inStream.setMaxElementLength(maxElementLength);

    dset.clear();
    dset.transferInit();
    OFCondition cond = dset.read(inStream, EXS_LittleEndianImplicit);
    dset.transferEnd();
    return cond;
}


OFTEST(dcmdata_elementLengthLimit_exceeded)
{
    /* An element larger than the configured limit must be rejected with
     * EC_ElementLengthLimitExceeded, even though the complete element data
     * is available in the stream. */
    static const Uint32 elemLen = 8192;
    Uint8 data[8 + elemLen];
    writeElementHeader(data, elemLen);
    memset(data + 8, 'A', elemLen);

    DcmDataset dset;
    OFCondition cond = parseFromBuffer(data, sizeof(data), dset, 4096);
    if (cond != EC_ElementLengthLimitExceeded)
    {
        OFCHECK_FAIL("Expected EC_ElementLengthLimitExceeded for an 8192 byte element "
            "with maxElementLength=4096, but got: " << cond.text());
    }
}


OFTEST(dcmdata_elementLengthLimit_atLimit)
{
    /* An element of exactly the configured limit must still be accepted. */
    static const Uint32 elemLen = 8192;
    Uint8 data[8 + elemLen];
    writeElementHeader(data, elemLen);
    memset(data + 8, 'A', elemLen);

    DcmDataset dset;
    OFCondition cond = parseFromBuffer(data, sizeof(data), dset, elemLen);
    if (cond.bad())
    {
        OFCHECK_FAIL("Parsing an 8192 byte element with maxElementLength=8192 "
            "should succeed, but got: " << cond.text());
        return;
    }
    DcmElement *elem = NULL;
    OFCHECK(dset.findAndGetElement(DCM_PatientName, elem).good());
    if (elem != NULL)
        OFCHECK_EQUAL(elem->getLength(), elemLen);
}


OFTEST(dcmdata_elementLengthLimit_default)
{
    /* Without a custom limit, the compile-time default applies, which is
     * the maximum defined value length permitted in DICOM, i.e. the same
     * element parses fine. */
    static const Uint32 elemLen = 8192;
    Uint8 data[8 + elemLen];
    writeElementHeader(data, elemLen);
    memset(data + 8, 'A', elemLen);

    DcmDataset dset;
    OFCondition cond = parseFromBuffer(data, sizeof(data), dset, 0);
    if (cond.bad())
    {
        OFCHECK_FAIL("Parsing an 8192 byte element with the default limit "
            "should succeed, but got: " << cond.text());
    }
}


OFTEST(dcmdata_elementLengthLimit_incompleteStream)
{
    /* The limit must also protect a stream that may still receive more
     * data (setEos() not called, as for a network message that arrives in
     * multiple PDV fragments): without the limit, the parser allocates a
     * value buffer of the announced size up front.  Only the 8 byte
     * element header announcing a 1 GB value is provided. */
    Uint8 data[8];
    writeElementHeader(data, 0x40000000);

    DcmDataset dset;
    OFCondition cond = parseFromBuffer(data, sizeof(data), dset, 4096, OFFalse);
    if (cond != EC_ElementLengthLimitExceeded)
    {
        OFCHECK_FAIL("Expected EC_ElementLengthLimitExceeded for an element announcing "
            "1 GB on an incomplete stream with maxElementLength=4096, but got: " << cond.text());
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


OFTEST(dcmdata_elementLengthLimit_datasetAPI)
{
    /* Test DcmDataset::setMaxElementLength() with loadFile(). */
    DcmDataset srcDset;
    OFString tmpFile;
    OFCondition cond;

    /* save a dataset with an 8192 byte OB element as raw dataset (no meta header) */
    static const Uint32 elemLen = 8192;
    Uint8 elemData[elemLen];
    memset(elemData, 'A', elemLen);
    OFCHECK(srcDset.putAndInsertUint8Array(DCM_EncapsulatedDocument, elemData, elemLen).good());
    cond = makeTempFile(tmpFile);
    OFCHECK(cond.good());
    cond = srcDset.saveFile(tmpFile, EXS_LittleEndianExplicit);
    OFCHECK(cond.good());

    /* default limit: 8192 byte element should load fine */
    DcmDataset dset1;
    cond = dset1.loadFile(tmpFile, EXS_LittleEndianExplicit);
    if (cond.bad())
    {
        OFCHECK_FAIL("DcmDataset::loadFile() with an 8192 byte element should succeed "
            "with the default limit, but got: " << cond.text());
    }

    /* custom limit 4096: 8192 byte element should be rejected */
    DcmDataset dset2;
    dset2.setMaxElementLength(4096);
    cond = dset2.loadFile(tmpFile, EXS_LittleEndianExplicit);
    if (cond != EC_ElementLengthLimitExceeded)
    {
        OFCHECK_FAIL("DcmDataset::loadFile() with an 8192 byte element and "
            "maxElementLength=4096 should fail, but got: " << cond.text());
    }

    OFStandard::deleteFile(tmpFile);
}


OFTEST(dcmdata_elementLengthLimit_fileFormatAPI)
{
    /* Test DcmFileFormat::setMaxElementLength() with loadFile(). */
    DcmDataset srcDset;
    OFString tmpFile;
    OFCondition cond;

    /* save a file (with meta header) containing an 8192 byte OB element */
    static const Uint32 elemLen = 8192;
    Uint8 elemData[elemLen];
    memset(elemData, 'A', elemLen);
    OFCHECK(srcDset.putAndInsertUint8Array(DCM_EncapsulatedDocument, elemData, elemLen).good());
    cond = makeTempFile(tmpFile);
    OFCHECK(cond.good());
    DcmFileFormat srcFF(&srcDset);
    cond = srcFF.saveFile(tmpFile, EXS_LittleEndianExplicit);
    OFCHECK(cond.good());

    /* default limit: 8192 byte element should load fine */
    DcmFileFormat ff1;
    cond = ff1.loadFile(tmpFile);
    if (cond.bad())
    {
        OFCHECK_FAIL("DcmFileFormat::loadFile() with an 8192 byte element should succeed "
            "with the default limit, but got: " << cond.text());
    }

    /* custom limit 4096: 8192 byte element should be rejected */
    DcmFileFormat ff2;
    ff2.setMaxElementLength(4096);
    cond = ff2.loadFile(tmpFile);
    if (cond != EC_ElementLengthLimitExceeded)
    {
        OFCHECK_FAIL("DcmFileFormat::loadFile() with an 8192 byte element and "
            "maxElementLength=4096 should fail, but got: " << cond.text());
    }

    OFStandard::deleteFile(tmpFile);
}
