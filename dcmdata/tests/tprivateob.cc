#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcprivateob.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dcistrmb.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcostrmb.h"

void compareBuffers(uint8_t* expected, uint8_t* actual, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (expected[i] != actual[i])
        {
            OFCHECK_FAIL("Unexpected difference in buffers at " << i << ". Expected: 0x" << std::hex << (uint16_t)expected[i] << " but got 0x" << (uint16_t)actual[i] << std::dec);
            break;
        }
    }
}

DcmPrivateOBTag createTag()
{
    DcmTag tagKey(DcmTagKey(0x7FE1, 0x1060), EVR_OB);
    DcmPrivateOBTag tag = DcmPrivateOBTag(tagKey);
    return tag;
}

OFCondition readFromInput(DcmPrivateOBTag& tag, uint8_t* buffer, int size, int verifyBuffer = -1)
{
    DcmInputBufferStream input;
    input.setBuffer(buffer, size);
    OFCondition readResult = tag.read(input, EXS_LittleEndianImplicit, EGL_noChange, DCM_MaxReadLength);

    if (verifyBuffer != -1)
    {
        OFCHECK_EQUAL(input.tell(), verifyBuffer);
    }
    return readResult;
}

OFTEST(dcmdata_dcprivateob_readExpected)
{
    // Create test input
    uint8_t testInput[] = {
        0xFE, 0xFF, 0x00, 0xE0, 0x01, 0x02, 0x03,       // some data bytes
        0xFE, 0xFF, 0xDD, 0xE0, 0x00, 0x00, 0x00, 0x00, // default delimiter, also part of the tag
        0x04, 0x05, 0x06,                               // some trailing data, should not be consumed
    };
    const int expectedLength = 15;

    // Create DcmPrivateOBTag object
    DcmPrivateOBTag tag = createTag();

    // Read from stream
    OFCondition readResult = readFromInput(tag, testInput, (int)sizeof(testInput), expectedLength);

    // Verify reading is successful, tag has expected number of bytes and stream position is correct
    if (!readResult.good())
    {
        OFCHECK_FAIL("Unexpected error reading from stream");
    }
    OFCHECK_EQUAL(tag.getLength(), expectedLength);
};

OFTEST(dcmdata_dcprivateob_readInsfficientData)
{
    // Create test input
    uint8_t testInput[] = {
        0xFE, 0xFF, 0x00, 0xE0, 0x01, 0x02, 0x03,       // some data bytes, less than default delimiter
    };

    // Create DcmPrivateOBTag object
    DcmPrivateOBTag tag = createTag();

    // Read from stream
    OFCondition readResult = readFromInput(tag, testInput, (int)sizeof(testInput), 0);

    // Verify reading is successful, tag has expected number of bytes and stream position is correct
    OFCHECK_EQUAL(readResult.status(), EC_StreamNotifyClient.theStatus);
    OFCHECK_EQUAL(readResult.code(), EC_StreamNotifyClient.theCode);
    OFCHECK_EQUAL(tag.getLength(), 0);
};

OFTEST(dcmdata_dcprivateob_readNoDelimiterInData)
{
    // Create test input
    uint8_t testInput[] = {
        0xFE, 0xFF, 0x00, 0xE0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // some data bytes, no delimiter
    };

    // Create DcmPrivateOBTag object
    DcmPrivateOBTag tag = createTag();

    // Read from stream
    OFCondition readResult = readFromInput(tag, testInput, (int)sizeof(testInput), (int)sizeof(testInput));

    // Verify reading is successful, tag has expected number of bytes and stream position is correct
    // (DcmInputBufferStream does not set eosflag when reading the last byte from buffer, so test for EC_InvalidStream instead of EC_EndOfStream)
    OFCHECK_EQUAL(readResult.status(), EC_InvalidStream.theStatus);
    OFCHECK_EQUAL(readResult.code(), EC_InvalidStream.theCode);
    OFCHECK_EQUAL(tag.getLength(), (int)sizeof(testInput));
};

OFTEST(dcmdata_dcprivateob_writeInsufficientBufferLengthForTag)
{
    // Create test input
    uint8_t testInput[] = {
        0xFE, 0xFF, 0x00, 0xE0, 0x01, 0x02, 0x03,       // some data bytes
        0xFE, 0xFF, 0xDD, 0xE0, 0x00, 0x00, 0x00, 0x00, // default delimiter, also part of the tag
    };

    // Create and populate DcmPrivateOBTag object
    DcmPrivateOBTag tag = createTag();
    readFromInput(tag, testInput, (int)sizeof(testInput));

    // Create test output buffer
    uint8_t testOutput[4]; // not enough capacity for tag
    DcmOutputBufferStream output(testOutput, sizeof(testOutput));

    OFCondition writeResult = tag.write(output, EXS_LittleEndianImplicit, EET_ExplicitLength, NULL);

    // Verify nothing was written and EC_StreamNotifyClient returned
    OFCHECK_EQUAL(output.tell(), 0);
    OFCHECK_EQUAL(writeResult.status(), EC_StreamNotifyClient.theStatus);
    OFCHECK_EQUAL(writeResult.code(), EC_StreamNotifyClient.theCode);
};

OFTEST(dcmdata_dcprivateob_writeInsufficientBufferLengthForData)
{
    // Create test input
    uint8_t testInput[] = {
        0xFE, 0xFF, 0x00, 0xE0, 0x01, 0x02, 0x03,       // some data bytes
        0xFE, 0xFF, 0xDD, 0xE0, 0x00, 0x00, 0x00, 0x00, // default delimiter, also part of the tag
    };

    // Create and populate DcmPrivateOBTag object
    DcmPrivateOBTag tag = createTag();
    readFromInput(tag, testInput, (int)sizeof(testInput));

    uint8_t expectedBuffer[] = {
        0xE1, 0x7F, 0x60, 0x10,             // tag
        0x0F, 0x00, 0x00, 0x00,             // length
        0xFE, 0xFF, 0x00, 0xE0, 0x01, 0x02, // data
    };
    const int expectedSize = (int)sizeof(expectedBuffer);

    // Create test output buffer
    uint8_t testOutput[14]; // enough capacity for tag (8 bytes), but not all the data
    DcmOutputBufferStream output(testOutput, sizeof(testOutput));

    OFCondition writeResult = tag.write(output, EXS_LittleEndianImplicit, EET_ExplicitLength, NULL);

    // Verify nothing was written and EC_StreamNotifyClient returned
    OFCHECK_EQUAL(output.tell(), expectedSize);
    OFCHECK_EQUAL(writeResult.status(), EC_StreamNotifyClient.theStatus);
    OFCHECK_EQUAL(writeResult.code(), EC_StreamNotifyClient.theCode);
    compareBuffers(testOutput, expectedBuffer, expectedSize);
};

OFTEST(dcmdata_dcprivateob_write)
{
    // Create test input
    uint8_t testInput[] = {
        0xFE, 0xFF, 0x00, 0xE0, 0x01, 0x02, 0x03,       // some data bytes
        0xFE, 0xFF, 0xDD, 0xE0, 0x00, 0x00, 0x00, 0x00, // default delimiter, also part of the tag
    };

    // Create and populate DcmPrivateOBTag object
    DcmPrivateOBTag tag = createTag();
    readFromInput(tag, testInput, (int)sizeof(testInput));

    uint8_t expectedBuffer[] = {
        0xE1, 0x7F, 0x60, 0x10, // tag
        0x0F, 0x00, 0x00, 0x00, // length
        0xFE, 0xFF, 0x00, 0xE0, 0x01, 0x02, 0x03, 0xFE, 0xFF, 0xDD, 0xE0, 0x00, 0x00, 0x00, 0x00, // data
    };
    const int expectedSize = (int)sizeof(expectedBuffer);

    // Create test output buffer
    uint8_t testOutput[30]; // enough capacity for tag (8 bytes) and all the data
    DcmOutputBufferStream output(testOutput, sizeof(testOutput));

    OFCondition writeResult = tag.write(output, EXS_LittleEndianImplicit, EET_ExplicitLength, NULL);

    // Verify nothing was written and EC_StreamNotifyClient returned
    OFCHECK_EQUAL(output.tell(), expectedSize);
    if (!writeResult.good())
    {
        OFCHECK_FAIL("Unexpected write result, should be successfull");
    }
    compareBuffers(testOutput, expectedBuffer, expectedSize);
};

OFTEST(dcmdata_dcprivateob_printDefault)
{
    const std::string expected = "  (7fe1,1060) OB fe\\ff\\dd\\e0\\00\\00\\00\\00 # 8, 1 Unknown Tag & Data\n";
    // Create test input
    uint8_t testInput[] = {
        0xFE, 0xFF, 0xDD, 0xE0, 0x00, 0x00, 0x00, 0x00, // default delimiter, also part of the tag
    };

    // Create and populate DcmPrivateOBTag object
    DcmPrivateOBTag tag = createTag();
    readFromInput(tag, testInput, (int)sizeof(testInput));

    // Create test output buffer
    std::ostringstream output;

    tag.print(output, 0, 2);

    // Verify nothing was written and EC_StreamNotifyClient returned
    OFCHECK_EQUAL(output.str(), expected);
};

OFTEST(dcmdata_dcprivateob_printTreestructure)
{
    const std::string expected = "| Unknown Tag & Data fe\\ff\\dd\\e0\\00\\00\\00\\00\n";
    // Create test input
    uint8_t testInput[] = {
        0xFE, 0xFF, 0xDD, 0xE0, 0x00, 0x00, 0x00, 0x00, // default delimiter, also part of the tag
    };

    // Create and populate DcmPrivateOBTag object
    DcmPrivateOBTag tag = createTag();
    readFromInput(tag, testInput, (int)sizeof(testInput));

    // Create test output buffer
    std::ostringstream output;

    tag.print(output, DCMTypes::PF_showTreeStructure, 2);

    // Verify nothing was written and EC_StreamNotifyClient returned
    OFCHECK_EQUAL(output.str(), expected);
};

OFTEST(dcmdata_dcprivateob_printShortenOutput)
{
    const std::string expected = "  (7fe1,1060) OB 0\\00\\00\\00\\00\\00\\00\\00\\00\\00\\00\\00\\00\\00\\00\\00\\00\\00\\00\\fe\\ff\\dd\\e0 ... # 27, 1 Unknown Tag & Data\n";
    // Create test input
    uint8_t testInput[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,// filler to get above 70 characters (~23 bytes)
        0xFE, 0xFF, 0xDD, 0xE0, 0x00, 0x00, 0x00, 0x00, // default delimiter, also part of the tag
    };

    // Create and populate DcmPrivateOBTag object
    DcmPrivateOBTag tag = createTag();
    readFromInput(tag, testInput, (int)sizeof(testInput));

    // Create test output buffer
    std::ostringstream output;

    tag.print(output, DCMTypes::PF_shortenLongTagValues, 2);

    // Verify nothing was written and EC_StreamNotifyClient returned
    OFCHECK_EQUAL(output.str(), expected);
};
