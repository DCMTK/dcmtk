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
 *  Purpose: Regression tests for DcmJSONReader bounds safety and URI decoding
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/offile.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/oftempf.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcjsonrd.h"


static OFCondition writeTempJson(const char *data, size_t len, OFString& outPath)
{
    OFCondition cond = OFTempFile::createFile(outPath);
    if (cond.bad()) return cond;

    OFFile f;
    if (!f.fopen(outPath, "wb"))
    {
        OFStandard::deleteFile(outPath);
        return EC_IllegalCall;
    }
    size_t written = f.fwrite(data, 1, len);
    f.fclose();
    if (written != len)
    {
        OFStandard::deleteFile(outPath);
        return EC_IllegalCall;
    }
    return EC_Normal;
}


OFTEST(dcmdata_json_malformed_token_oob)
{
    static const char kMalformedObj[] = "{\"\"{\"vr\"2";
    static const char kMalformedArr[] = "[{\"\"{\"vr\"2";

    const char *cases[] = { kMalformedObj, kMalformedArr };
    const size_t lens[] = { sizeof(kMalformedObj) - 1, sizeof(kMalformedArr) - 1 };

    for (size_t i = 0; i < 2; ++i)
    {
        OFString tmpPath;
        OFCHECK(writeTempJson(cases[i], lens[i], tmpPath).good());

        DcmJSONReader reader;
        reader.setIgnoreBulkdataURIPolicy(OFTrue);

        // Must fail cleanly without advancing tokenArray_ pointer out of bounds
        // even when stopOnErrorPolicy is disabled (the default).
        reader.setStopOnErrorPolicy(OFFalse);
        DcmFileFormat ff1;
        OFCondition cond1 = reader.readAndConvertJSONFile(ff1, tmpPath.c_str());
        OFCHECK(cond1 == EC_InvalidJSONContent);

        reader.setStopOnErrorPolicy(OFTrue);
        DcmFileFormat ff2;
        OFCondition cond2 = reader.readAndConvertJSONFile(ff2, tmpPath.c_str());
        OFCHECK(cond2 == EC_InvalidJSONContent);

        OFStandard::deleteFile(tmpPath);
    }
}


OFTEST(dcmdata_json_empty_vr_and_desync_bounds)
{
    // 1. Valid JSON with empty "vr" string followed by "Value" array
    {
        static const char kEmptyVr[] = "{\"00100020\":{\"vr\":\"\",\"Value\":[\"TEST\"]}}";
        OFString tmpPath;
        OFCHECK(writeTempJson(kEmptyVr, sizeof(kEmptyVr) - 1, tmpPath).good());

        DcmJSONReader reader;
        DcmFileFormat ff;
        OFCHECK(reader.readAndConvertJSONFile(ff, tmpPath.c_str()).good());
        OFString val;
        OFCHECK(ff.getDataset()->findAndGetOFString(DCM_PatientID, val).good());
        OFCHECK_EQUAL(val, "TEST");

        OFStandard::deleteFile(tmpPath);
    }

    // 2. Valid JSON with unknown attribute & malformed trailing element under stopOnErrorPolicy=OFFalse
    {
        static const char kUnknownAttr[] = "{\"00100010\":{\"unknown\":\"x\"},\"00100020\":1}";
        OFString tmpPath;
        OFCHECK(writeTempJson(kUnknownAttr, sizeof(kUnknownAttr) - 1, tmpPath).good());

        DcmJSONReader reader;
        reader.setStopOnErrorPolicy(OFFalse);
        DcmFileFormat ff;
        OFCHECK(reader.readAndConvertJSONFile(ff, tmpPath.c_str()).good());

        OFStandard::deleteFile(tmpPath);
    }

    // 3. Multi-dataset JSON array selection (both valid index 2 and out-of-range index 3)
    {
        static const char kMultiDs[] =
            "[{\"00100020\":{\"vr\":\"LO\",\"Value\":[\"A\"]}},"
            "{\"00100020\":{\"vr\":\"LO\",\"Value\":[\"B\"]}}]";
        OFString tmpPath;
        OFCHECK(writeTempJson(kMultiDs, sizeof(kMultiDs) - 1, tmpPath).good());

        DcmJSONReader reader;
        reader.setStopOnErrorPolicy(OFTrue);
        reader.setArrayHandlingPolicy(2);
        DcmFileFormat ff;
        OFCHECK(reader.readAndConvertJSONFile(ff, tmpPath.c_str()).good());
        OFString val;
        OFCHECK(ff.getDataset()->findAndGetOFString(DCM_PatientID, val).good());
        OFCHECK_EQUAL(val, "B");

        reader.setArrayHandlingPolicy(3);
        DcmFileFormat ffOob;
        OFCHECK(reader.readAndConvertJSONFile(ffOob, tmpPath.c_str()) == EC_InvalidJSONContent);

        OFStandard::deleteFile(tmpPath);
    }
}


OFTEST(dcmdata_json_urlDecode_truncated)
{
    DcmJSONReader reader;
    const char *badInputs[] = { "%", "%4", "abc%", "abc%4" };
    for (size_t i = 0; i < 4; ++i)
    {
        OFString s(badInputs[i]);
        OFCHECK(reader.urlDecode(s) == EC_UnsupportedURIType);
    }

    OFString valid("hello%20world");
    OFCHECK(reader.urlDecode(valid).good());
    OFCHECK_EQUAL(valid, "hello world");

    OFString pct("%2520");
    OFCHECK(reader.urlDecode(pct).good());
    OFCHECK_EQUAL(pct, "%20");
}
