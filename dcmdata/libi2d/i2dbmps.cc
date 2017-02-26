/*
 *
 *  Copyright (C) 2009-2017, OFFIS e.V.
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
 *  Purpose: Class to extract pixel data and meta information from BMP file
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/libi2d/i2dbmps.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmdata/libi2d/i2doutpl.h"

#ifndef UINT16_MAX
/// Maximum value a Uint16 can hold
#define UINT16_MAX 65535
#endif

I2DBmpSource::I2DBmpSource() : bmpFile()
{
  DCMDATA_LIBI2D_DEBUG("I2DBmpSource: Plugin instantiated");
}


OFString I2DBmpSource::inputFormat() const
{
  return "BMP";
}


OFCondition I2DBmpSource::openFile(const OFString &filename)
{
  DCMDATA_LIBI2D_DEBUG("I2DBmpSource: Opening BMP file: " << filename);
  OFCondition cond;
  if (filename.empty())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "No BMP filename specified");

  // Try to open BMP file
  if ((bmpFile.fopen(filename.c_str(), "rb")) == OFFalse)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to open BMP file");

  return cond;
}


OFCondition I2DBmpSource::readPixelData(Uint16& rows,
                                        Uint16& cols,
                                        Uint16& samplesPerPixel,
                                        OFString& photoMetrInt,
                                        Uint16& bitsAlloc,
                                        Uint16& bitsStored,
                                        Uint16& highBit,
                                        Uint16& pixelRepr,
                                        Uint16& planConf,
                                        Uint16& pixAspectH,
                                        Uint16& pixAspectV,
                                        char*&  pixData,
                                        Uint32& length,
                                        E_TransferSyntax &ts)
{
  DCMDATA_LIBI2D_DEBUG("I2DBmpSource: Importing BMP pixel data");
  OFCondition cond = openFile(m_imageFile);
  // return error if file is not open
  if (cond.bad())
  {
    closeFile();
    return cond;
  }

  /* Read both file headers */
  Uint32 dataOffset;
  cond = readFileHeader(dataOffset);
  if (cond.bad())
  {
    closeFile();
    return cond;
  }

  Uint16 width, height;
  Uint16 bpp, colors;
  OFBool isTopDown = OFFalse; /* Most BMPs are stored bottom-up */
  cond = readBitmapHeader(width, height, bpp, isTopDown, colors);
  if (cond.bad())
  {
    closeFile();
    return cond;
  }

  Uint32 *palette = NULL;
  cond = readColorPalette(colors, palette);
  if (cond.bad())
  {
    closeFile();
    return cond;
  }

  /* Now jump to the bitmap data... */
  if (bmpFile.fseek(dataOffset, SEEK_SET) != 0)
    return EC_EndOfStream;

  /* ...and read the "real" image data */
  char *data;
  Uint32 data_length;
  cond = readBitmapData(width, height, bpp, isTopDown, colors, palette, data, data_length);

  if (palette)
    delete[] palette;

  if (cond.bad())
  {
    closeFile();
    return cond;
  }

  /* Now we got all the info that we need, return it to caller */

  rows = height;
  cols = width;
  samplesPerPixel = 3;    /* 24 bpp */

  bitsAlloc = 8;
  bitsStored = 8;
  highBit = 7;
  photoMetrInt = "RGB";
  planConf = 0;           /* For each pixel we save rgb in that order */
  pixData = data;
  length = data_length;
  pixAspectH = pixAspectV = 1;
  pixelRepr = 0;
  ts = EXS_LittleEndianExplicit;

  return cond;
}


OFCondition I2DBmpSource::readFileHeader(Uint32 &offset)
{
  Uint16 magic;
  if (readWord(magic) != 0)
    return EC_EndOfStream;

  /* ASCII "BM", decimal 19778 */
  if (magic != 19778)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Not a BMP file - invalid header");

  /* Skip over two uninteresting entries */
  if (bmpFile.fseek(8, SEEK_CUR) != 0)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Not a BMP file - invalid header");

  if (readDWord(offset) != 0)
    return EC_EndOfStream;

  DCMDATA_LIBI2D_DEBUG("I2DBmpSource: BMP data at file offset: " << offset);

  return EC_Normal;
}


OFCondition I2DBmpSource::readBitmapHeader(Uint16 &width,
                                           Uint16 &height,
                                           Uint16 &bpp,
                                           OFBool &isTopDown,
                                           Uint16 &colors)
{
  Uint16 tmp_word;
  Uint32 tmp_dword;

  /* 40 is the size of the bitmap info header */
  if (readDWord(tmp_dword) != 0 || tmp_dword != 40)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Not a BMP file - invalid bitmap header");

  Sint32 tmp_width, tmp_height;
  if (readLong(tmp_width) != 0)
    return EC_EndOfStream;

  if (readLong(tmp_height) != 0)
    return EC_EndOfStream;

  if (tmp_height < 0) /* Is this a top down bitmap? */
  {
    isTopDown = OFTrue;
    tmp_height = -tmp_height;
  }
  else
    isTopDown = OFFalse;
  height = OFstatic_cast(Uint16, tmp_height);

  // Check if we got a valid value here which fits into a Uint16
  // (height < 0 can happen because -(INT_MIN) == INT_MIN).
  if (tmp_height <= 0 || tmp_height > UINT16_MAX)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unsupported BMP file - height too large or zero");

  if (tmp_width < 0) /* Width also can be signed, but no semantic */
  {
    tmp_width = -tmp_width;
  }
  width = OFstatic_cast(Uint16, tmp_width);
  if (tmp_width <= 0 || tmp_width > UINT16_MAX)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unsupported BMP file - width too large or zero");

  /* Some older standards used this, always 1 for BMP (number of planes) */
  if (readWord(tmp_word) != 0 || tmp_word != 1)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Not a BMP file - invalid number of planes");

  /* Color depth in bpp */
  if (readWord(tmp_word) != 0)
    return EC_EndOfStream;
  bpp = tmp_word;
  DCMDATA_LIBI2D_DEBUG("I2DBmpSource: BMP bpp: " << OFstatic_cast(int, bpp));

  /* Compression info */
  if (readDWord(tmp_dword) != 0)
    return EC_EndOfStream;
  /* We don't support compressed BMPs */
  if (tmp_dword != 0)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unsupported BMP file - compressed");

  /* We don't care about the next three fields of the bitmap info header:
   *  DWord: Size of image data or 0 (yes, that's what the standard says!).
   *  Long:  Horizontal resolution in pixel per meter, mostly set to 0.
   *  Long:  Vertical resolution in pixel per meter, mostly set to 0:
   */
  if (bmpFile.fseek(12, SEEK_CUR) != 0)
    return EC_EndOfStream;

  /* Number of entries in color table, 0 means "use default" */
  if (readDWord(tmp_dword) != 0)
    return EC_EndOfStream;

  /* A BMP file can only have 256 color table entries */
  if (tmp_dword > 256)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "invalid BMP file - color table too big");

  colors = OFstatic_cast(Uint16, tmp_dword);
  if (colors == 0) {
    // In this case, 1, 4 and 8 bpp get 2**bpp colors in the color table,
    // others get no color table at all.
    switch (bpp) {
    case 1:
      colors = 2;
      break;
    case 4:
      colors = 16;
      break;
    case 8:
      colors = 256;
      break;
    default:
      colors = 0;
      break;
    }
  }

  /* Skip another uninteresting entry (number of important colors) */
  if (bmpFile.fseek(4, SEEK_CUR) != 0)
    return EC_EndOfStream;

  DCMDATA_LIBI2D_DEBUG("I2DBmpSource: BMP width: " << width);
  DCMDATA_LIBI2D_DEBUG("I2DBmpSource: BMP height: " << height);
  DCMDATA_LIBI2D_DEBUG("I2DBmpSource: BMP stored as top down: " << (isTopDown ? "Yes" : "No"));
  DCMDATA_LIBI2D_DEBUG("I2DBmpSource: Colortable entries: " << colors);

  return EC_Normal;
}


OFCondition I2DBmpSource::readColorPalette(Uint16 colors,
                                           Uint32*& palette)
{
  if (colors == 0)
    // Nothing to do;
    return EC_Normal;

  if (colors > 256)
    // BMPs can not have more than 256 color table entries
    return EC_IllegalCall;

  // Read the color palette
  palette = new Uint32[colors];
  for (int i = 0; i < colors; i++) {
    Uint32 tmp;

    // Each item is 32-bit BGRx entry, this function reads that data
    if (readDWord(tmp) != 0) {
      delete[] palette;
      palette = NULL;
      return EC_EndOfStream;
    }

    // Converting this BGRx into RGB is done elsewhere
    palette[i] = tmp;
  }

  return EC_Normal;
}


OFCondition I2DBmpSource::readBitmapData(const Uint16 width,
                                         const Uint16 height,
                                         const Uint16 bpp,
                                         const OFBool isTopDown,
                                         const Uint16 colors,
                                         const Uint32* palette,
                                         char*& pixData,
                                         Uint32& length)
{
  /* row_length = width * bits_per_pixel / 8 bits_per_byte.
     row_length must be rounded *up* to a 4-byte boundary:
     row_length = (row_length + 3) & ~3
   */

  const Uint32 row_length = ((width * bpp + 31) / 32) * 4;
  Uint8 *row_data;
  Uint32 y;
  OFBool positive_direction;
  Uint32 max;

  // "palette" may only be NULL if colors is 0 and vice versa
  if ((palette == NULL) != (colors == 0))
    return EC_IllegalCall;

  // These bit depths always need a color palette
  if (colors == 0 && (bpp == 1 || bpp == 4 || bpp == 8))
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "invalid BMP file - missing color palette");

  if (isTopDown)
  {
    /* This is a top-down BMP, we start at the first row and work our way down */
    y = 1;
    positive_direction = OFTrue;
    max = height + 1;
  }
  else
  {
    /* Bottom-up BMP, we start with the last row and work our way up */
    y = height;
    positive_direction = OFFalse;
    max = 0;
  }

  length = width * height * 3;

  DCMDATA_LIBI2D_DEBUG("I2DBmpSource: Starting to read bitmap data");

  row_data = new Uint8[row_length];
  pixData = new char[length];

  if (!row_data || !pixData)
  {
    delete[] row_data;
    delete[] pixData;
    return EC_MemoryExhausted;
  }

  /* Go through each row of the image */
  for (; y != max; (positive_direction ? ++y : --y))
  {
    /* Calculate posData for this line, it is the index of the first byte for
     * this line. ( -1 because we start at index 1, but C at index 0)
     */
    Uint32 posData = (y - 1) * width * 3;

    if (bmpFile.fread(row_data, 1, row_length) < row_length)
    {
      delete[] row_data;
      delete[] pixData;
      return EC_EndOfStream;
    }

    OFCondition cond;
    switch (bpp)
    {
      case 1:
      case 4:
      case 8:
        cond = parseIndexedColorRow(row_data, width, bpp, colors, palette, &pixData[posData]);
        break;
      case 16:
        cond = parse16BppRow(row_data, width, &pixData[posData]);
        break;
      case 24:
      case 32:
        cond = parse24_32BppRow(row_data, width, bpp, &pixData[posData]);
        break;
      default:
        cond = makeOFCondition(OFM_dcmdata, 18, OF_error, "unsupported BMP file - invalid bpp");
        break;
    }
    if (cond.bad())
    {
      delete[] row_data;
      delete[] pixData;
      return cond;
    }
  }

  DCMDATA_LIBI2D_DEBUG("I2DBmpSource: Done reading bitmap data");

  delete[] row_data;

  return EC_Normal;
}


OFCondition I2DBmpSource::parse24_32BppRow(const Uint8 *row,
                                           const Uint16 width,
                                           const int bpp,
                                           char *pixData) const
{
  /* We now must convert this line of the bmp file into the kind of data that
   * our caller expects. Each pixel consists of three bytes: blue, green, red
   * (notice the order!) pixel value. We convert this into "standard" RGB.
   */
  Uint32 x;
  Uint32 pos_a = 0;
  Uint32 pos_b = 0;

  /* 32bpp images really are 24bpp images with a null byte prepended in front of
   * each pixel. Some apps use that as an alpha channel, but that's not allowed.
   */
  int offset = 0;
  if (bpp == 32)
    offset = 1;

  for (x = 0; x < width; x++)
  {
    Uint8 r = row[pos_a + 2];
    Uint8 g = row[pos_a + 1];
    Uint8 b = row[pos_a];

    pixData[pos_b]     = r;
    pixData[pos_b + 1] = g;
    pixData[pos_b + 2] = b;

    pos_a += 3 + offset;
    pos_b += 3;
  }
  return EC_Normal;
}


OFCondition I2DBmpSource::parse16BppRow(const Uint8 *row,
                                        const Uint16 width,
                                        char *pixData) const
{
  /* We now must convert this line of the bmp file into the kind of data that
   * our caller expects. Each pixel consists of three bytes: blue, green, red
   * (notice the order!) pixel value. We convert this into "standard" RGB.
   */
  Uint32 x;
  Uint32 pos = 0;

  for (x = 0; x < width; x++)
  {
    // Assemble one pixel value from the input data
    Uint16 pixel = 0;
    pixel = OFstatic_cast(Uint16, pixel | (row[2*x + 1] << 8));
    pixel = OFstatic_cast(Uint16, pixel | row[2*x + 0]);

    // Each colors has 5 bit, we convert that into 8 bit
    Uint8 r = OFstatic_cast(Uint8, (pixel >> 10) << 3);
    Uint8 g = OFstatic_cast(Uint8, (pixel >>  5) << 3);
    Uint8 b = OFstatic_cast(Uint8, (pixel >>  0) << 3);

    pixData[pos]     = r;
    pixData[pos + 1] = g;
    pixData[pos + 2] = b;

    pos += 3;
  }
  return EC_Normal;
}


OFCondition I2DBmpSource::parseIndexedColorRow(const Uint8 *row,
                                               const Uint16 width,
                                               const int bpp,
                                               const Uint16 colors,
                                               const Uint32* palette,
                                               char *pixData /*out*/) const
{
  // data that is still left from reading the last pixel
  Uint8 data = 0;
  // Number of valid bits in data
  Uint8 bitsLeft = 0;

  Uint32 pos = 0;
  Uint32 pos_input = 0;
  for (Uint32 x = 0; x < width; x++)
  {
    // Check if we need to go to the next byte of input data
    if (bitsLeft == 0)
    {
      bitsLeft = 8;
      data = row[pos_input++];
    }

    // Get the left-most bpp bits from data
    Uint8 index = OFstatic_cast(Uint8, data >> (bitsLeft - bpp));
    // The right-most bpp bits in "index" now contain the data we want,
    // clear all the higher bits.
    // (1 << bpp) gives us in binary: 00001000 (with bpp zero bits) if we
    // substract 1, only the right-most bpp bits will be 1.
    index = OFstatic_cast(Uint8, index & ((1 << bpp) - 1));
    bitsLeft = OFstatic_cast(Uint8, bitsLeft - bpp);

    // Check if we are still in the color palette
    if (index >= colors)
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "unsupported BMP file - access beyond end of color table");

    // Get the colors
    Uint32 pixel = palette[index];

    // And save it in the resulting image, this implicitly converts the BGR we
    // got from the color table into RGB.
    pixData[pos]     = OFstatic_cast(Uint8, pixel >> 16);
    pixData[pos + 1] = OFstatic_cast(Uint8, pixel >>  8);
    pixData[pos + 2] = OFstatic_cast(Uint8, pixel >>  0);

    pos += 3;
  }
  return EC_Normal;
}


/* Read 2 bytes, convert to unsigned int */
/* All 2-byte quantities in BMP are little-endian */
int I2DBmpSource::readWord(Uint16& result)
{
  int c1, c2;
  c1 = bmpFile.fgetc();
  if (c1 == EOF)
    return EOF;
  c2 = bmpFile.fgetc();
  if (c2 == EOF)
    return EOF;
  result = OFstatic_cast(Uint16, (OFstatic_cast(Uint16, c2) << 8) + OFstatic_cast(Uint16, c1));
  return 0;
}


/* Read 4 bytes, convert to unsigned int */
/* All 4-byte quantities in BMP are little-endian */
int I2DBmpSource::readDWord(Uint32& result)
{
  int c1, c2, c3, c4;
  c1 = bmpFile.fgetc();
  if (c1 == EOF)
    return EOF;
  c2 = bmpFile.fgetc();
  if (c2 == EOF)
    return EOF;
  c3 = bmpFile.fgetc();
  if (c3 == EOF)
    return EOF;
  c4 = bmpFile.fgetc();
  if (c4 == EOF)
    return EOF;
  result = (OFstatic_cast(Uint32, c4) << 24) +
           (OFstatic_cast(Uint32, c3) << 16) +
           (OFstatic_cast(Uint32, c2) <<  8) +
            OFstatic_cast(Uint32, c1);
  return 0;
}


int I2DBmpSource::readLong(Sint32& result)
{
  Uint32 tmp;

  /* First read, it as a "normal" double word */
  if (readDWord(tmp) != 0)
    return EOF;

  /* tmp is a two's complement (signed integer) and we have to convert it into that */
  if (tmp & (1UL << 31))
  {
    /* If the highest bit is set, it is a negative number, convert it */
    result = -(OFstatic_cast(Sint32, ~(tmp - 1)));
  }
  else
  {
    /* It is a positive number, no conversion necessary */
    result = tmp;
  }

  return 0;
}


// closes underlying BMP file
void I2DBmpSource::closeFile()
{
  bmpFile.fclose();
}


// close file and free dynamically allocated memory
I2DBmpSource::~I2DBmpSource()
{
  DCMDATA_LIBI2D_DEBUG("I2DBmpSource: Closing BMP file and cleaning up memory");
  closeFile();
}
