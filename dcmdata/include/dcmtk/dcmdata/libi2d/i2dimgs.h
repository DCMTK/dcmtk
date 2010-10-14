/*
 *
 *  Copyright (C) 2001-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:46 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef I2DIMGS_H
#define I2DIMGS_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dcxfer.h"

class I2DImgSource
{

public:

  /** Constructor
   */
  I2DImgSource() : m_imageFile() {}

  /** Returns identifier for the image input format the plugin imports.
   *  @return A short identifier (e. g. "JPEG")
   */
  virtual OFString inputFormat() const =0;

  /** Extracts the raw JPEG pixel data stream from a JPEG file and returns some
   *  image information about this pixel data.
   *  Raw means here that all APP markers (e.g. JFIF information) are removed from the JPEG stream.
   *  The pixel data returned is a JPEG stream in JPEG interchange format.
   *  This function allocates memory for the pixel data returned to the user. The caller of this
   *  function is responsible for deleting the memory buffer
   *  @param rows - [out] Rows of image
   *  @param cols - [out] Columns of image
   *  @param samplesPerPixel - [out] Number of components per pixel
   *  @param photoMetrInt - [out] The DICOM color model used for the compressed data
   *  @param bitsAlloc - [out] Bits Allocated for one sample
   *  @param bitsStored - [out] Bits Stored, number of bits stored within Bits Allocated
   *  @param highBit - [out] High Bit, hightest bit position set within Bits Allocated
   *  @param pixelRepr - [out] Pixel Representation (0=unsigned, 1=signed)
   *  @param planConf - [out] Planar Configuration
   *  @param pixAspectH - [out] Horizontal value of pixel aspect ratio
   *  @param pixAspectV - [out] Vertical value of pixel aspect ratio
   *  @param pixData - [out] Pointer to the pixel data in JPEG Interchange Format (but without APPx markers).
   *  @param length - [out] Length of pixel data
   *  @param ts - [out] The transfer syntax imposed by the imported pixel pixel data.
                        This is necessary for the JPEG importer that needs to report
                        which TS must be used for the imported JPEG data (ie. baseline, progressive, ...).
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

  /* After reading of pixel data, this function can be used for getting
   * information about lossy compression parameters.
   * @param srcEncodingLossy - [out] Denotes, whether the encoding of the pixel
   *                           data read was lossy (OFtrue) or lossless (OFFalse)
   * @param srcLossyComprMethod - [out] Denotes the lossy compression method used
   *                              in source if there is one (srcEncodingLossy = OFTrue).
   *                              Should use defined terms of attribute Lossy Compression Method.
   * @return EC_Normal if information is available, error otherwise
   */
  virtual OFCondition getLossyComprInfo(OFBool& srcEncodingLossy,
                                        OFString& srcLossyComprMethod) const =0;

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

#endif // #ifndef I2DIMGS_H

/*
 * CVS/RCS Log:
 * $Log: i2dimgs.h,v $
 * Revision 1.9  2010-10-14 13:15:46  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.8  2010-02-22 11:39:54  uli
 * Remove some unneeded includes.
 *
 * Revision 1.7  2009-11-04 09:58:08  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.6  2009-09-30 08:05:25  uli
 * Stop including dctk.h in libi2d's header files.
 *
 * Revision 1.5  2009-07-16 14:22:19  onken
 * Changed comment (and thus semantic of parameter for output transfer syntax).
 *
 * Revision 1.4  2009-03-31 13:06:09  onken
 * Changed implementation of lossy compression attribute detection and writing.
 *
 * Revision 1.2  2009-01-16 09:51:55  onken
 * Completed doxygen documentation for libi2d.
 *
 * Revision 1.1  2008-01-16 15:08:38  onken
 * Moved library "i2dlib" from /dcmdata/libsrc/i2dlib to /dcmdata/libi2d
 *
 * Revision 1.2  2008-01-11 14:17:53  onken
 * Added various options to i2dlib. Changed logging to use a configurable
 * logstream. Added output plugin for the new Multiframe Secondary Capture SOP
 * Classes. Added mode for JPEG plugin to copy exsiting APPn markers (except
 * JFIF). Changed img2dcm default behaviour to invent type1/type2 attributes (no
 * need for templates any more). Added some bug fixes.
 *
 * Revision 1.1  2007/11/08 15:58:55  onken
 * Initial checkin of img2dcm application and corresponding library i2dlib.
 *
 *
 */

