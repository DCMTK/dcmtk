/*
 *
 *  Copyright (C) 1997-2007, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
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
 *  Module:  dcmjpls
 *
 *  Author:  Martin Willkomm
 *
 *  Purpose: enumerations, error constants and helper functions for dcmjp2k
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2009-07-29 14:46:46 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpls/include/dcmtk/dcmjpls/djlsutil.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DJLSUTILS_H
#define DJLSUTILS_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcond.h"   /* for class OFCondition */

/** describes the condition under which a compressed or decompressed image
 *  receives a new SOP instance UID.
 */
enum JLS_UIDCreation
{
  /** Upon compression, assign new SOP instance UID if compression is lossy.
   *  Upon decompression never assign new SOP instance UID.
   */
  EJLSUC_default,

  /// always assign new SOP instance UID on compression and decompression
  EJLSUC_always,

  /// never assign new SOP instance UID
  EJLSUC_never
};

/** describes how the decoder should handle planar configuration of
 *  decompressed color images.
 */
enum JLS_PlanarConfiguration
{
  /// restore planar configuration as indicated in data set
  EJLSPC_restore,

  /** automatically determine whether color-by-plane is required from
   *  the SOP Class UID and decompressed photometric interpretation
   */
  EJLSPC_auto,

  /// always create color-by-pixel planar configuration
  EJLSPC_colorByPixel,

  /// always create color-by-plane planar configuration
  EJLSPC_colorByPlane
};

/** describes how the encoder handles the image bit depth
 *  upon lossy compression.
 */
enum JLS_CompressionBitDepth
{
  /// keep original bit depth
  EJLSBD_original,

  /** limit bit depth to a certain value, i.e. scale down
   *  if the original image bit depth is larger
   */
  EJLSBD_limit,

  /** force bit depth to a certain value, i.e. scale up
   *  or scale down the original image to match the given
   *  bit depth.
   */
  EJLSBD_force
};


// CONDITION CONSTANTS

/// error condition constant: JPEG-LS Codec Error
extern const OFCondition EC_JLSCodecError;

/// error condition constant: Unsupported bit depth in JPEG-LS transfer syntax
extern const OFCondition EC_JLSUnsupportedBitDepth;

/// error condition constant: Cannot compute number of fragments for JPEG-LS frame
extern const OFCondition EC_JLSCannotComputeNumberOfFragments;

/// error condition constant: Error while writing temporary file
extern const OFCondition EC_JLSTempFileWriteError;

/// error condition constant: Error while reading temporary file
extern const OFCondition EC_JLSTempFileReadError;

/// error condition constant: Unknown PNM file format found in temporary file
extern const OFCondition EC_JLSUnknownPNMFormat;

/// error condition constant: Image data mismatch between DICOM header and JPEG-LS bitstream
extern const OFCondition EC_JLSImageDataMismatch;

/// error condition constant: Unsupported photometric interpretation for near-lossless JPEG-LS compression
extern const OFCondition EC_JLSUnsupportedPhotometricInterpretation;

/// error condition constant: Unsupported pixel representation for near-lossless JPEG-LS compression
extern const OFCondition EC_JLSUnsupportedPixelRepresentation;

/// error condition constant: Unsupported type of image for JPEG-LS compression
extern const OFCondition EC_JLSUnsupportedImageType;

/// error condition constant: Pixel data too short for uncompressed image
extern const OFCondition EC_JLSPixelDataTooShort;

#endif

/*
 * CVS/RCS Log:
 * $Log: djlsutil.h,v $
 * Revision 1.1  2009-07-29 14:46:46  meichel
 * Initial release of module dcmjpls, a JPEG-LS codec for DCMTK based on CharLS
 *
 * Revision 1.2  2007-06-20 12:37:37  meichel
 * Completed implementation of encoder, which now supports lossless
 *   "raw" and "cooked" and near-lossless "cooked" modes.
 *
 * Revision 1.1  2007/06/15 14:35:45  meichel
 * Renamed CMake project and include directory from dcmjpgls to dcmjpls
 *
 * Revision 1.4  2007/06/15 10:39:15  meichel
 * Completed implementation of decoder, which now correctly processes all
 *   of the NEMA JPEG-LS sample images, including fragmented frames.
 *
 * Revision 1.3  2007/06/14 12:36:14  meichel
 * Further code clean-up. Updated doxygen comments.
 *
 * Revision 1.2  2007/06/13 16:41:07  meichel
 * Code clean-up and removal of dead code
 *
 *
 */
