/*
 *
 *  Copyright (C) 1997-2014, OFFIS e.V.
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
 *  Purpose: enumerations, error constants and helper functions for dcmjpls
 *
 */

#ifndef DCMJPLS_DJLSUTILS_H
#define DCMJPLS_DJLSUTILS_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcond.h"   /* for class OFCondition */
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmjpls/dldefine.h"


#define DCMJPLS_CHARLS_VERSION_STRING "CharLS, Version 1.0 (modified)"


// global definitions for logging mechanism provided by the oflog module

extern DCMTK_DCMJPLS_EXPORT OFLogger DCM_dcmjplsLogger;

#define DCMJPLS_TRACE(msg) OFLOG_TRACE(DCM_dcmjplsLogger, msg)
#define DCMJPLS_DEBUG(msg) OFLOG_DEBUG(DCM_dcmjplsLogger, msg)
#define DCMJPLS_INFO(msg)  OFLOG_INFO(DCM_dcmjplsLogger, msg)
#define DCMJPLS_WARN(msg)  OFLOG_WARN(DCM_dcmjplsLogger, msg)
#define DCMJPLS_ERROR(msg) OFLOG_ERROR(DCM_dcmjplsLogger, msg)
#define DCMJPLS_FATAL(msg) OFLOG_FATAL(DCM_dcmjplsLogger, msg)


// include this file in doxygen documentation

/** @file djlsutil.h
 *  @brief enumerations, error constants and helper functions for the dcmjpls module
 */


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
