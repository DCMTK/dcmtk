/*
 *
 *  Copyright (C) 2010, OFFIS e.V.
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
 *  Purpose: test program for class I2DBmpSource
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-12-16 12:52:34 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/libi2d/i2dbmps.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */

#define OFFIS_CONSOLE_APPLICATION "i2dbmp"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
    OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

static OFLogger i2dbmpLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

enum offset {
    BMP_IMAGE_OFFSET = 10,
    BMP_WIDTH        = 18,
    BMP_HEIGHT       = 22,
    // BPP is only 2 bytes wide!
    BMP_BPP          = 28,
    BMP_COLORS       = 46
};

// 0 unless there were errors
static int status = 0;
// temporary file which will be used
static const char *temporaryFile = "i2dbmp.tmp";

static Uint8 bmpHeader[] = {
    // Offset 0, Magic identification number
    'B', 'M',
    // 2, File size, most likely wrong anyway
    42, 0x42, 13, 37,
    // 6, Reservered
    0x13, 0x37, 0xde, 0xad,
    // 10, Offset of the image data from start of data, this is IMPORTANT
    0, 0, 0, 0,
    // 14, value is 40, the size of the BITMAPINFOHEADER struct (the following data)
    40, 0, 0, 0,
    // 18, Width of the image
    0, 0, 0, 0,
    // 22, height of image, if this is negative it's a top-down image
    0, 0, 0, 0,
    // 26, number of planes, always 1 in BMP
    1, 0,
    // 28, Bit depth
    24, 0,
    // 30, compression info
    0, 0, 0, 0,
    // 34, zero or size of image data in bytes
    12, 11, 10, 9,
    // 38, X pixels per meter
    8, 7, 6, 5,
    // 42, X pixels per meter
    4, 3, 2, 1,
    // 46, colors used, 0 means the default value
    5, 0, 0, 0,
    // 50, number of important colors, this field is useless
    1, 2, 3, 4,

    // Now comes the color table, this one gives us just some colors (the fourth
    // byte is unused, let's write some garbage into it)
    // color 0, black
    0x00, 0x00, 0x00, 18,
    // color 1, blue
    0xff, 0x00, 0x00, 97,
    // color 2, green
    0x00, 0xff, 0x00, 123,
    // color 3, red
    0x00, 0x00, 0xff, 93,
    // color 4, white
    0xff, 0xff, 0xff, 30,

    // Now you'd get the actual pixel data
};

#define LOG_WARN(arg)  OFLOG_WARN(i2dbmpLogger, arg)
#define LOG_INFO(arg)  OFLOG_INFO(i2dbmpLogger, arg)
#define LOG_DEBUG(arg) OFLOG_DEBUG(i2dbmpLogger, arg)
#define TEST(arg)      LOG_INFO(arg << "...")

#define EXPECT_SUCCESS(b) do {     \
    if (!(b)) {                    \
        status |= 1;               \
        LOG_WARN("!! Test failed in line " << __LINE__); \
    }                              \
} while (false)
#define EXPECT_FAIL(b) EXPECT_SUCCESS(!(b))

static void setDWord(int offset, int width, bool sign = false)
{
    Uint32 w = width;
    if (sign)
        w = ~w + 1;

    for (int i = 0; i < 4; i++)
    {
        bmpHeader[offset++] = OFstatic_cast(Uint8, w & 0xff);
        w >>= 8;
    }
}

static bool writeImage(const void *pixelData, unsigned int pixelDataLength)
{
    OFFile file;
    if (!file.fopen(temporaryFile, "wb"))
    {
        LOG_WARN("Failed to open temporary file " << temporaryFile);
        return false;
    }

    file.fwrite(bmpHeader, 1, sizeof(bmpHeader));
    if (pixelData != NULL)
        file.fwrite(pixelData, 1, pixelDataLength);
    file.fclose();

    return true;
}

static bool readFile(char *&pixData, Uint32 &outputLength, Uint16 &rows, Uint16 &cols)
{
    I2DBmpSource source;
    Uint16 samplesPerPixel, bitsAlloc, bitsStored;
    Uint16 highBit, pixelRepr, planConf, pixAspectH, pixAspectV;
    OFString photoMetrInt;
    E_TransferSyntax ts;

    source.setImageFile(temporaryFile);
    OFCondition res = source.readPixelData(rows, cols, samplesPerPixel, photoMetrInt,
                                           bitsAlloc, bitsStored, highBit, pixelRepr,
                                           planConf, pixAspectH, pixAspectV, pixData,
                                           outputLength, ts);
    if (res.bad())
    {
        LOG_DEBUG("Reading file failed: " << res.text());
        return false;
    }

    return true;
}

static bool canHandleImage(const void *pixelData, unsigned int pixelDataLength)
{
    if (!writeImage(pixelData, pixelDataLength))
        return false;

    char *pixData;
    Uint32 length;
    Uint16 rows, cols;

    if (!readFile(pixData, length, rows, cols))
        return false;

    return true;
}

static bool compareImage(const void *pixelData, unsigned int pixelDataLength,
                         void *expectedOutput, unsigned int expectedOutputLength,
                         Uint16 width, Uint16 height)
{
    if (!writeImage(pixelData, pixelDataLength))
        return false;

    char *pixData;
    Uint32 length;
    Uint16 rows, cols;

    if (!readFile(pixData, length, rows, cols))
        return false;

    if (expectedOutputLength != length)
    {
        LOG_DEBUG("Output length does not match!");
        delete[] pixData;
        return false;
    }
    if (memcmp(expectedOutput, pixData, expectedOutputLength) != 0)
    {
        LOG_DEBUG("Output data does not match!");
        delete[] pixData;
        return false;
    }

    delete[] pixData;

    if (width != cols)
    {
        LOG_DEBUG("Output width does not match!");
        return false;
    }
    if (height != rows)
    {
        LOG_DEBUG("Output height does not match!");
        return false;
    }

    return true;
}

static void testImages()
{
    // This pixel data is used for all tests with at least 16bpp
    Uint8 pixelData1[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
    Uint8 expected1a[] = { 0x77, 0x66, 0x55, 0x33, 0x22, 0x11 }; // 32/24bpp, 1x2, bottom-up
    Uint8 expected1b[] = { 0x33, 0x22, 0x11, 0x77, 0x66, 0x55 }; // 32/24bpp, 1x2, top-down
    Uint8 expected1c[] = { 0x33, 0x22, 0x11, 0x66, 0x55, 0x44 }; // 24bpp,    2x1, bottom-up
    Uint8 expected1d[] = { 0xc8, 0x90, 0xa8,  0x10, 0x18, 0xb8,
                           0x40, 0x80, 0x88,  0x88, 0x08, 0x98,
                         }; // 16bpp, 2x2, top-down

    // These are the values of the color table
#define BLACK 0x00, 0x00, 0x00
#define BLUE  0x00, 0x00, 0xff
#define GREEN 0x00, 0xff, 0x00
#define RED   0xff, 0x00, 0x00
#define WHITE 0xff, 0xff, 0xff

    Uint8 pixelData2[] = { 0, 1, 2, 3, 4, 3, 2, 1 };
    Uint8 expected2[]  = { BLACK, BLUE, GREEN, WHITE, RED, GREEN }; // 8bpp, 3x2, top-down

    // Only byte 0 and 4 are actually used
    Uint8 pixelData3[] = { 0x40, 0xff, 0xff, 0xff, 0x32, 0xff, 0xff, 0xff };
    Uint8 expected3[]  = { RED, GREEN, WHITE, BLACK }; // 4bpp, 2x2, bottom-up

    /* Only first three *bits* of byte 0 and 4 are important,
     * thanks to the 4 byte alignment requirement for rows.
     */
    Uint8 pixelData4[] = { 0xaf, 0xff, 0x00, 0xad, 0x80, 0xde, 0xad, 0xab };
    Uint8 expected4[]  = { BLUE, BLACK, BLACK, BLUE, BLACK, BLUE }; // 1bpp, 3x2, bottom-up

#undef BLACK
#undef BLUE
#undef GREEN
#undef RED
#undef WHITE

    setDWord(BMP_IMAGE_OFFSET, sizeof(bmpHeader));

    TEST("Trying to read a 0x0 image");
    setDWord(BMP_WIDTH, 0);
    setDWord(BMP_HEIGHT, 0);
    EXPECT_FAIL(canHandleImage(pixelData1, sizeof(pixelData1)));
    setDWord(BMP_WIDTH, 1);
    setDWord(BMP_HEIGHT, 2);

    TEST("Checking large bpp values");
    bmpHeader[BMP_BPP    ] = 0;
    bmpHeader[BMP_BPP + 1] = 0xef;
    EXPECT_FAIL(canHandleImage(pixelData1, sizeof(pixelData1)));
    bmpHeader[BMP_BPP    ] = 32;
    bmpHeader[BMP_BPP + 1] = 0;

    TEST("Checking large number of colors");
    setDWord(BMP_COLORS, 1 << 31);
    EXPECT_FAIL(canHandleImage(pixelData1, sizeof(pixelData1)));
    setDWord(BMP_COLORS, 5);

    TEST("Checking premature end of file");
    EXPECT_FAIL(canHandleImage(pixelData1, sizeof(pixelData1) - 1));

    TEST("Decoding a 32bpp image");
    EXPECT_SUCCESS(compareImage(pixelData1, sizeof(pixelData1), expected1a, sizeof(expected1a), 1, 2));

    TEST("Decoding a 24bpp image");
    // Because rows are aligned to 4-bytes boundaries, this can use the
    // same pixel data as the 32bpp image
    bmpHeader[BMP_BPP] = 24;
    EXPECT_SUCCESS(compareImage(pixelData1, sizeof(pixelData1), expected1a, sizeof(expected1a), 1, 2));

    TEST("Decoding a 24bpp top-down image");
    setDWord(BMP_HEIGHT, -2);
    EXPECT_SUCCESS(compareImage(pixelData1, sizeof(pixelData1), expected1b, sizeof(expected1b), 1, 2));

    TEST("Checking correct handling of row alignment");
    setDWord(BMP_WIDTH, 2);
    setDWord(BMP_HEIGHT, 1);
    EXPECT_SUCCESS(compareImage(pixelData1, sizeof(pixelData1), expected1c, sizeof(expected1c), 2, 1));

    TEST("Decoding a 16bpp image");
    bmpHeader[BMP_BPP] = 16;
    setDWord(BMP_WIDTH, 2);
    setDWord(BMP_HEIGHT, 2);
    EXPECT_SUCCESS(compareImage(pixelData1, sizeof(pixelData1), expected1d, sizeof(expected1d), 2, 2));

    // Everything starting from 8bpp uses color tables!
    TEST("Decoding a 8bpp top-down image");
    setDWord(BMP_WIDTH, 3);
    setDWord(BMP_HEIGHT, -2);
    bmpHeader[BMP_BPP] = 8;
    EXPECT_SUCCESS(compareImage(pixelData2, sizeof(pixelData2), expected2, sizeof(expected2), 3, 2));

    TEST("Decoding a 4bpp image");
    bmpHeader[BMP_BPP] = 4;
    setDWord(BMP_WIDTH, 2);
    setDWord(BMP_HEIGHT, 2);
    EXPECT_SUCCESS(compareImage(pixelData3, sizeof(pixelData3), expected3, sizeof(expected3), 2, 2));

    TEST("Decoding a 1bpp image");
    bmpHeader[BMP_BPP] = 1;
    setDWord(BMP_WIDTH, 3);
    setDWord(BMP_HEIGHT, 2);
    EXPECT_SUCCESS(compareImage(pixelData4, sizeof(pixelData4), expected4, sizeof(expected4), 3, 2));

    TEST("Decoding an image with too large color table");
    setDWord(BMP_COLORS, -1);
    EXPECT_FAIL(canHandleImage(pixelData4, sizeof(pixelData4)));

    TEST("Decoding an image with too small color table");
    setDWord(BMP_COLORS, 1);
    EXPECT_FAIL(canHandleImage(pixelData4, sizeof(pixelData4)));
}

#define SHORTCOL 3
#define LONGCOL 21

int main(int argc, char *argv[])
{
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Test libi2d's BMP support", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",    "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--version",           "print version information and exit", OFCommandLine::AF_Exclusive);
      OFLog::addOptions(cmd);

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
    {
      /* check exclusive options first */
      if (cmd.hasExclusiveOption())
      {
        if (cmd.findOption("--version"))
        {
          app.printHeader(OFTrue /*print host identifier*/);
          return 0;
        }
      }
    }

    /* options */
    OFLog::configureFromCommandLine(cmd, app);

    /* print resource identifier */
    OFLOG_DEBUG(i2dbmpLogger, rcsid << OFendl);

    testImages();

    /* Remove our temporary file */
    unlink(temporaryFile);

    if (status)
    {
        LOG_WARN("!!! Some tests failed!");
    } else {
        COUT << "*** All tests successful!" << OFendl;
    }
    return status;
}


/*
 *
 * CVS/RCS Log:
 * $Log: i2dbmp.cc,v $
 * Revision 1.4  2010-12-16 12:52:34  joergr
 * Added explicit type cast in order to keep VisualStudio 2005 quiet.
 *
 * Revision 1.3  2010-12-06 11:26:43  uli
 * Fixed the i2dbmp test case for windows by opening files in binary mode.
 *
 * Revision 1.2  2010-10-14 13:15:05  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.1  2010-06-11 09:34:17  uli
 * Added test for i2d's BMP code.
 *
 *
 *
 */
