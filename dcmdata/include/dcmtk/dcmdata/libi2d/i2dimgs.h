/*
 *
 *  Copyright (C) 2007-2014, OFFIS e.V.
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
 *  Purpose: Base Class for plugins extracting pixel data from standard
 *           image files
 *
 */

#ifndef I2DIMGS_H
#define I2DIMGS_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/libi2d/i2define.h"

/** Base Class for plugins extracting pixel data from standard image files
 */
class DCMTK_I2D_EXPORT I2DImgSource
{

public:

  /** Constructor
   */
  I2DImgSource() : m_imageFile() {}

  /** Returns identifier for the image input format the plugin imports.
   *  @return A short identifier of the input format
   */
  virtual OFString inputFormat() const = 0;

  /** Extracts the raw pixel data stream from image file and returns some further
   *  information about this pixel data.
   *  This function allocates memory for the pixel data returned to the user.
   *  The caller of this function is responsible for deleting the memory buffer
   *  @param rows - [out] Rows of image
   *  @param cols - [out] Columns of image
   *  @param samplesPerPixel - [out] Number of components per pixel
   *  @param photoMetrInt - [out] The DICOM color model used for the compressed data
   *  @param bitsAlloc - [out] Bits Allocated for one sample
   *  @param bitsStored - [out] Bits Stored, number of bits stored within Bits Allocated
   *  @param highBit - [out] High Bit, highest bit position set within Bits Allocated
   *  @param pixelRepr - [out] Pixel Representation (0=unsigned, 1=signed)
   *  @param planConf - [out] Planar Configuration
   *  @param pixAspectH - [out] Horizontal value of pixel aspect ratio
   *  @param pixAspectV - [out] Vertical value of pixel aspect ratio
   *  @param pixData - [out] Pointer to the pixel data
   *  @param length - [out] Length of pixel data
   *  @param ts - [out] The transfer syntax imposed by the imported pixel pixel data
   *  @return EC_Normal, if successful, error otherwise
   */
  virtual OFCondition readPixelData( Uint16& rows,
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
                                     E_TransferSyntax& ts) =0;

  /** After reading of pixel data, this function can be used for getting
   *  information about lossy compression parameters.
   *  @param srcEncodingLossy - [out] Denotes, whether the encoding of the pixel
   *                            data read was lossy (OFTrue) or lossless (OFFalse)
   *  @param srcLossyComprMethod - [out] Denotes the lossy compression method used
   *                               in source if there is one (srcEncodingLossy = OFTrue).
   *                               Should use defined terms of attribute Lossy
   *                               Compression Method.
   *  @return EC_Normal if information is available, error otherwise
   */
  virtual OFCondition getLossyComprInfo(OFBool& srcEncodingLossy,
                                        OFString& srcLossyComprMethod) const = 0;

  /** Sets the input image file to read.
   *  @param filename - [in] The filename of the image input file
   *  @return none
   */
  void setImageFile(const OFString& filename) { m_imageFile = filename; };

  /** Returns the input image file that currently associated with plugin
   *  @return The filename of the image input file
   */
  OFString getImageFile() { return m_imageFile; };

  /** Virtual Destructor
   */
  virtual ~I2DImgSource() {}

protected:

  /// The input file
  OFString m_imageFile;
};

#endif // I2DIMGS_H
