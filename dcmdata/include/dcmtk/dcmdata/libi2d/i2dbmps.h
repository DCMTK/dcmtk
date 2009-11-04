/*
 *
 *  Copyright (C) 2001-2007, OFFIS
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
 *  Purpose: Class to extract pixel data and meta information from BMP file
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2009-11-04 09:58:08 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef I2DBMPS_H
#define I2DBMPS_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/libi2d/i2dimgs.h"
#include "dcmtk/ofstd/offile.h"

class I2DBmpSource : public I2DImgSource
{

public:

  /** Constructor, initializes member variables
   *  @return none
   */
  I2DBmpSource();

  /** Returns format of input image. For this class "BMP" is returned.
   *  @return Returns format of input image, i. e. "BMP".
   */
  OFString inputFormat() const;

  /** Extracts the raw BMP pixel data stream from a BMP file and returns some
   *  image information about this pixel data.
   *  Raw means that any header information is removed from the BMP stream.
   *  This function allocates memory for the pixel data returned to the user. The caller of this
   *  function is responsible for deleting the memory buffer
   *  @param rows - [out] Rows of image
   *  @param cols - [out] Columns of image
   *  @param samplesPerPixel - [out] Number of components per pixel
   *  @param photoMetrInt - [out] The DICOM color model used for the compressed data
   *  @param bitsAlloc - [out] Bits Allocated for one sample
   *  @param bitsStored - [out] Bits Stored, Number of bits actually stored within Bits Allocated
   *  @param highBit - [out] High Bit, Highest stored in bit within Bits Allocated
   *  @param pixelRepr - [out] Pixel Representation (0=unsigned, 1=signed)
   *  @param planConf - [out] Planar Configuration
   *  @param pixAspectH - [out] Horizontal value of pixel aspect ratio
   *  @param pixAspectV - [out] Vertical value of pixel aspect ratio
   *  @param pixData - [out] Pointer to the pixel data in BMP Interchange Format (but without APPx markers).
   *  @param length - [out] Length of pixel data
   *  @param ts - [out] The transfer syntax imposed by the imported pixel pixel data.
                        This is necessary for the BMP importer that needs to report
                        which TS must be used for the imported BMP data (ie. baseline, progressive, ...).
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition readPixelData( Uint16& rows,
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
                             E_TransferSyntax& ts);

  /** After reading of pixel data, this function can be used for getting
   *  information about lossy compression parameters.
   *  @param srcEncodingLossy - [out] Denotes, whether the encoding of the pixel
   *                            data read was lossy (OFtrue) or lossless (OFFalse)
   *  @param srcLossyComprMethod - [out] Denotes the lossy compression method used
   *                               in source if there is one (srcEncodingLossy = OFTrue).
   *                               Should use defined terms of attribute Lossy Compression Method.
   *  @return EC_Normal if information is available, error otherwise
   */
  virtual OFCondition getLossyComprInfo(OFBool& srcEncodingLossy,
                                        OFString& srcLossyComprMethod) const
  {
    srcEncodingLossy = OFFalse;
    return EC_Normal;
  }

  /** Destructor, frees some memory.
   *  @return none
   */
  virtual ~I2DBmpSource();

protected:

  /** Opens the BMP file specified by the given filename.
   *  @param filename - [in] The file to be opened
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition openFile(const OFString& filename);

  /** Closes BMP file.
   *  @return none
   */
  void closeFile();

  /** Reads and checks the BMP file header.
   *  @param offset - [out] Offset to the BMP data from beginning of file
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition readFileHeader(Uint32 &offset);

  /** Reads and checks the BMP bitmap header.
   *  After this function, the current read position is in the middle of the
   *  header, use fseek() to get to a known offset!
   *  @param width - [out] width of the image in pixel
   *  @param height - [out] height of the image in pixel
   *  @param isTopDown - [out] OFTrue if this is a top down bitmap
   *                     (height was read as negative value).OFFalse otherwise.
   *  @param bpp - [out] bits per pixel of the image.
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition readBitmapHeader(Uint16 &width     /*out*/,
                               Uint16 &height    /*out*/,
                               Uint16 &bpp       /*out*/,
                               OFBool &isTopDown /*out*/);

  /** Read the bitmap data.
   *  This assumes the current read position is at the start of the image data.
   *  @param width - [in] width of the image in pixel
   *  @param height - [in] height of the image in pixel
   *  @param bpp - [in] Image's bits per pixel.
   *  @param isTopDown - [in] If true, this is a top down bitmap
   *  @param data - [out] Image data
   *  @param length - [out] Length of data
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition readBitmapData(const Uint16 width,
                             const Uint16 height,
                             const Uint16 bpp,
                             const OFBool isTopDown,
                             char*& pixData /*out*/,
                             Uint32& length /*out*/);

  /** Parse a single 24bpp row of bmp data.
   *  @param row - [in] The row of data to parse.
   *  @param width - [in] The length in pixel of the row.
   *  @param pixData - [out] The buffer to write the data to (in "RGB" format).
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition parse24BppRow(const char *row,
                            const Uint16 width,
                            char *pixData /*out*/) const;

  /** Read 4 bytes from the byte stream and interpret it as a signed integer.
   *  @param result - [out] The result
   *  @return 0, if successful, EOF if rest of stream does not contain enough bytes
   */
  int readLong(Sint32& result);

  /** Read 4 bytes from the byte stream.
   *  @param result - [out] The result
   *  @return 0, if successful, EOF if rest of stream does not contain enough bytes
   */
  int readDWord(Uint32& result);

  /** Read 2 bytes from the byte stream.
   *  @param result - [out] The result
   *  @return 0, if successful, EOF if rest of stream does not contain enough bytes
   */
  int readWord(Uint16& result);

  /// The BMP file, if opened
  OFFile bmpFile;
};

#endif // #ifndef I2DBMPS_H

/*
 * CVS/RCS Log:
 * $Log: i2dbmps.h,v $
 * Revision 1.3  2009-11-04 09:58:08  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.2  2009-09-30 08:05:25  uli
 * Stop including dctk.h in libi2d's header files.
 *
 * Revision 1.1  2009-07-16 14:25:28  onken
 * Added img2dcm input plugin for the BMP graphics format (at the moment only
 * support for 24 Bit RGB).
 *
 *
 */

