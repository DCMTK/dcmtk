/*
 *
 *  Copyright (C) 1997-2012, OFFIS e.V.
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
 *  Module:  dcmjpls
 *
 *  Author:  Martin Willkomm, Uli Schlachter
 *
 *  Purpose: enumerations, error constants and helper functions for dcmjp2k
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2012-02-15 14:50:42 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMJPLS_DJLSUTILS_H
#define DCMJPLS_DJLSUTILS_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcond.h"   /* for class OFCondition */
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmjpls/dldefine.h"


#define DCMJPLS_CHARLS_VERSION_STRING "CharLS, Revision 55020 (modified)"


// global definitions for logging mechanism provided by the oflog module

extern DCMTK_DCMJPLS_EXPORT OFLogger DCM_dcmjplsLogger;

#define DCMJPLS_TRACE(msg) OFLOG_TRACE(DCM_dcmjplsLogger, msg)
#define DCMJPLS_DEBUG(msg) OFLOG_DEBUG(DCM_dcmjplsLogger, msg)
#define DCMJPLS_INFO(msg)  OFLOG_INFO(DCM_dcmjplsLogger, msg)
#define DCMJPLS_WARN(msg)  OFLOG_WARN(DCM_dcmjplsLogger, msg)
#define DCMJPLS_ERROR(msg) OFLOG_ERROR(DCM_dcmjplsLogger, msg)
#define DCMJPLS_FATAL(msg) OFLOG_FATAL(DCM_dcmjplsLogger, msg)


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

/// error condition constant: Too small buffer used for image data (internal error)
extern DCMTK_DCMJPLS_EXPORT const OFConditionConst EC_JLSUncompressedBufferTooSmall;

/// error condition constant: Too small buffer used for compressed image data (internal error)
extern DCMTK_DCMJPLS_EXPORT const OFConditionConst EC_JLSCompressedBufferTooSmall;

/// error condition constant: The image uses some features which the codec does not support
extern DCMTK_DCMJPLS_EXPORT const OFConditionConst EC_JLSCodecUnsupportedImageType;

/// error condition constant: The codec was fed with invalid parameters (e.g. height = -1)
extern DCMTK_DCMJPLS_EXPORT const OFConditionConst EC_JLSCodecInvalidParameters;

/// error condition constant: The codec was fed with unsupported parameters (e.g. 32 bit per sample)
extern DCMTK_DCMJPLS_EXPORT const OFConditionConst EC_JLSCodecUnsupportedValue;

/// error condition constant: The compressed image is invalid
extern DCMTK_DCMJPLS_EXPORT const OFConditionConst EC_JLSInvalidCompressedData;

/// error condition constant: The images' color transformation is not supported in this bit depth
extern DCMTK_DCMJPLS_EXPORT const OFConditionConst EC_JLSUnsupportedBitDepthForTransform;

/// error condition constant: The images' color transformation is not supported
extern DCMTK_DCMJPLS_EXPORT const OFConditionConst EC_JLSUnsupportedColorTransform;

/// error condition constant: Unsupported bit depth in JPEG-LS transfer syntax
extern DCMTK_DCMJPLS_EXPORT const OFConditionConst EC_JLSUnsupportedBitDepth;

/// error condition constant: Cannot compute number of fragments for JPEG-LS frame
extern DCMTK_DCMJPLS_EXPORT const OFConditionConst EC_JLSCannotComputeNumberOfFragments;

/// error condition constant: Image data mismatch between DICOM header and JPEG-LS bitstream
extern DCMTK_DCMJPLS_EXPORT const OFConditionConst EC_JLSImageDataMismatch;

/// error condition constant: Unsupported photometric interpretation for near-lossless JPEG-LS compression
extern DCMTK_DCMJPLS_EXPORT const OFConditionConst EC_JLSUnsupportedPhotometricInterpretation;

/// error condition constant: Unsupported pixel representation for near-lossless JPEG-LS compression
extern DCMTK_DCMJPLS_EXPORT const OFConditionConst EC_JLSUnsupportedPixelRepresentation;

/// error condition constant: Unsupported type of image for JPEG-LS compression
extern DCMTK_DCMJPLS_EXPORT const OFConditionConst EC_JLSUnsupportedImageType;

/// error condition constant: Trailing data after image
extern DCMTK_DCMJPLS_EXPORT const OFConditionConst EC_JLSTooMuchCompressedData;

#endif

/*
 * CVS/RCS Log:
 * $Log: djlsutil.h,v $
 * Revision 1.11  2012-02-15 14:50:42  uli
 * Removed dependency on static initialization order from OFCondition.
 * All static condition objects are now created via makeOFConditionConst()
 * in a way that doesn't need a constructor to run. This should only break
 * code which defines its own condition objects, all other changes are
 * backwards compatible.
 *
 * Revision 1.10  2011-12-14 10:55:35  uli
 * Add some changes which were missing in the previous commit.
 *
 * Revision 1.9  2011-12-14 10:44:09  uli
 * Make it possible to rightly build dcmjpls as a DLL
 *
 * Revision 1.8  2011-04-18 07:00:59  uli
 * Use global variables for the logger objects. This removes the thread-unsafe
 * static local variables which were used before.
 *
 * Revision 1.7  2011-02-25 11:54:03  uli
 * Move the CharLS version string to a central place.
 *
 * Revision 1.6  2010-10-14 13:17:19  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.5  2010-03-01 10:35:28  uli
 * Renamed include guards to avoid name clash with e.g. dcmjpeg.
 *
 * Revision 1.4  2010-02-25 08:50:38  uli
 * Updated to latest CharLS version.
 *
 * Revision 1.3  2009-10-07 13:16:47  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.2  2009-07-31 09:05:43  meichel
 * Added more detailed error messages, minor code clean-up
 *
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
