/*
 *
 *  Copyright (C) 2009, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Uli Schlachter
 *
 *
 *  Purpose: Class to extract pixel data and meta information from BMP file
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2009-11-04 09:58:08 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/libi2d/i2dbmps.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmdata/libi2d/i2doutpl.h"


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
  if (filename.length() == 0)
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
  Uint16 bpp;
  OFBool isTopDown = OFFalse; /* Most BMPs are stored bottom-up */
  cond = readBitmapHeader(width, height, bpp, isTopDown);
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
  cond = readBitmapData(width, height, bpp, isTopDown, data, data_length);
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
                                           OFBool &isTopDown)
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
  if (tmp_width < 0) /* Width also can be signed, but no semantic */
  {
    tmp_width = -tmp_width;
  }
  width = OFstatic_cast(Uint16, tmp_width);

  /* Some older standards used this, always 1 for BMP (number of planes) */
  if (readWord(tmp_word) != 0 || tmp_word != 1)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Not a BMP file - invalid bitmap header");

  /* Color depth in bpp */
  if (readWord(tmp_word) != 0)
    return EC_EndOfStream;
  bpp = tmp_word;
  DCMDATA_LIBI2D_DEBUG("I2DBmpSource: BMP bpp: " << (int) bpp);

  /* Compression info */
  if (readDWord(tmp_dword) != 0)
    return EC_EndOfStream;
  /* We don't support compressed BMPs */
  if (tmp_dword != 0)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unsupported BMP file - compressed");

  /* We don't care about the rest of the bitmap info header, fields are:
   *  DWord: Size of image data or 0 (yes, that's what the standard says!).
   *  Long:  Horizontal resolution in pixel per meter, mostly set to 0.
   *  Long:  Vertical resolution in pixel per meter, mostly set to 0:
   *  DWord: Size of color table, unused for our supported bpp values.
   *  DWord: Used colors count, 0 for our supported bpp values.
   */

  DCMDATA_LIBI2D_DEBUG("I2DBmpSource: BMP width: " << width);
  DCMDATA_LIBI2D_DEBUG("I2DBmpSource: BMP height: " << height);
  DCMDATA_LIBI2D_DEBUG("I2DBmpSource: BMP stored as top down: " << (isTopDown ? "Yes" : "No"));

  return EC_Normal;
}


OFCondition I2DBmpSource::readBitmapData(const Uint16 width,
                                         const Uint16 height,
                                         const Uint16 bpp,
                                         const OFBool isTopDown,
                                         char*& pixData,
                                         Uint32& length)
{
  /* row_length = width * bits_per_pixel / 8 bits_per_byte.
     row_length must be rounded *up* to a 4-byte boundary:
     row_length = (row_length + 3) & ~3
   */

  const Uint32 row_length = (width * bpp / 8 + 3) & ~3;
  char *row_data;
  Uint32 y;
  Sint32 direction;
  Uint32 max;

  if (isTopDown)
  {
    /* This is a top-down BMP, we start at the first row and work our way down */
    y = 1;
    direction = 1;
    max = height + 1;
  }
  else
  {
    /* Bottom-up BMP, we start with the last row and work our way up */
    y = height;
    direction = -1;
    max = 0;
  }

  length = width * height * 3;

  DCMDATA_LIBI2D_DEBUG("I2DBmpSource: Starting to read bitmap data");

  row_data = new char[row_length];
  pixData = new char[length];

  if (!row_data || !pixData)
  {
    delete[] row_data;
    delete[] pixData;
    return EC_MemoryExhausted;
  }

  /* From bottom-most row up */
  for (; y != max; y += direction)
  {
    /* Calculate posData for this line, it is the index of the first byte for
     * this line. ( -1 because we start at index 1, but C at index 0)
     */
    Uint32 posData = (y - 1) * width * 3;

    if (bmpFile.fread(row_data, 1, row_length) < 1)
    {
      delete[] row_data;
      delete[] pixData;
      return EC_EndOfStream;
    }

    OFCondition cond;
    switch (bpp)
    {
      case 24:
        cond = parse24BppRow(row_data, width, &pixData[posData]);
        break;
      default:
        cond = makeOFCondition(OFM_dcmdata, 18, OF_error, "unsupported BMP file - wrong bpp");
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


OFCondition I2DBmpSource::parse24BppRow(const char *row,
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
    Uint8 r = row[pos + 2];
    Uint8 g = row[pos + 1];
    Uint8 b = row[pos];

    pixData[pos]     = r;
    pixData[pos + 1] = g;
    pixData[pos + 2] = b;

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
  result = (((Uint16) c2) << 8) + ((Uint16) c1);
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
  result = (((Uint32) c4) << 24) +
           (((Uint32) c3) << 16) +
           (((Uint32) c2) <<  8) +
            ((Uint32) c1);
  return 0;
}


int I2DBmpSource::readLong(Sint32& result)
{
  Uint32 tmp;

  /* First read, it as a "normal" double word */
  if (readDWord(tmp) != 0)
    return EOF;

  /* tmp is a two's complement (signed integer) and we have to convert it into that */

  if (tmp & (1 << 31))
  {
    /* If the highest bit is set, it is a negative number, convert it */
    result = -((Sint32) ~(tmp - 1));
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

/*
 * CVS/RCS Log:
 * $Log: i2dbmps.cc,v $
 * Revision 1.4  2009-11-04 09:58:08  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.3  2009-09-30 08:05:25  uli
 * Stop including dctk.h in libi2d's header files.
 *
 * Revision 1.2  2009-08-13 09:00:20  onken
 * Fixed minor formatting issues and bug that caused some images being
 * converted upside down.
 *
 * Revision 1.1  2009-07-16 14:25:38  onken
 * Added img2dcm input plugin for the BMP graphics format (at the moment only
 * support for 24 Bit RGB).
 *
 */
