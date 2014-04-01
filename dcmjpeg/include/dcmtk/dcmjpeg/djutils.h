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
 *  Module:  dcmjpeg
 *
 *  Author:  Norbert Olges, Marco Eichelberg
 *
 *  Purpose: enumerations, error constants and helper functions for dcmjpeg
 *
 */

#ifndef DJUTILS_H
#define DJUTILS_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcond.h"      /* for class OFCondition */
#include "dcmtk/dcmimgle/diutils.h"  /* for EP_Interpretation */
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmjpeg/djdefine.h"


// global definitions for logging mechanism provided by the oflog module

extern DCMTK_DCMJPEG_EXPORT OFLogger DCM_dcmjpegLogger;

#define DCMJPEG_TRACE(msg) OFLOG_TRACE(DCM_dcmjpegLogger, msg)
#define DCMJPEG_DEBUG(msg) OFLOG_DEBUG(DCM_dcmjpegLogger, msg)
#define DCMJPEG_INFO(msg)  OFLOG_INFO(DCM_dcmjpegLogger, msg)
#define DCMJPEG_WARN(msg)  OFLOG_WARN(DCM_dcmjpegLogger, msg)
#define DCMJPEG_ERROR(msg) OFLOG_ERROR(DCM_dcmjpegLogger, msg)
#define DCMJPEG_FATAL(msg) OFLOG_FATAL(DCM_dcmjpegLogger, msg)


// include this file in doxygen documentation

/** @file djutils.h
 *  @brief enumerations, error constants and helper functions for the dcmjpeg module
 */


// forward declarations

class DcmItem;


/** describes the different modes of operation of a JPEG codec
 */
enum EJ_Mode
{
  /// JPEG baseline
  EJM_baseline,

  /// JPEG extended sequential
  EJM_sequential,

  /// JPEG spectral selection
  EJM_spectralSelection,

  /// JPEG full progression
  EJM_progressive,

  /// JPEG lossless
  EJM_lossless
};

/** describes the different types of component sub-sampling
 *  to be used with lossy image compression.
 */
enum E_SubSampling
{
  /// 4:4:4 sampling (no subsampling)
  ESS_444,
  /// 4:2:2 sampling (horizontal subsampling of chroma components)
  ESS_422,
  /// 4:1:1 sampling (horizontal and vertical subsampling of chroma components)
  ESS_411
};

/** describes the condition under which a compressed or decompressed image
 *  receives a new SOP instance UID.
 */
enum E_UIDCreation
{
  /** Upon compression, assign new SOP instance UID if compression is lossy.
   *  Upon decompression never assign new SOP instance UID.
   */
  EUC_default,

  /// always assign new SOP instance UID on compression and decompression
  EUC_always,

  /// never assign new SOP instance UID
  EUC_never
};

/** describes how the decoder should handle planar configuration of
 *  decompressed color images.
 */
enum E_PlanarConfiguration
{
  /** automatically determine whether color-by-plane is required from
   *  the SOP Class UID and decompressed photometric interpretation
   */
  EPC_default,

  /// always create color-by-pixel planar configuration
  EPC_colorByPixel,

  /// always create color-by-plane planar configuration
  EPC_colorByPlane
};

/** describes how color space conversion should be handled
 *  during the conversion of an uncompressed DICOM image to
 *  a JPEG-compressed image
 */
enum E_CompressionColorSpaceConversion
{
  /** encode color images in YCbCr if lossy JPEG.
   *  If lossless JPEG, images are encoded as RGB unless the source
   *  image is YCbCr in which case no color conversion is performed.
   */
  ECC_lossyYCbCr,

  /** encode color images in RGB unless the source
   *  image is YCbCr in which case no color conversion is performed.
   */
  ECC_lossyRGB,

  /** convert color images to monochrome before compressing
   */
  ECC_monochrome
};

/** describes how color space conversion should be handled
 *  during the conversion of a JPEG-compressed DICOM image to
 *  an uncompressed image
 */
enum E_DecompressionColorSpaceConversion
{
  /** perform color space conversion from YCbCr to RGB if
   *  DICOM photometric interpretation indicates YCbCr.
   */
  EDC_photometricInterpretation,

  /** always perform color space conversion from YCbCr to
   *  RGB if JPEG data is color image and compression is lossy.
   */
  EDC_lossyOnly,

  /** always perform color space conversion from YCbCr to
   *  RGB if JPEG data is color image.
   */
  EDC_always,

  /** never perform any color space conversion.
   */
  EDC_never,

  /** perform color space conversion from YCbCr to RGB if
   *  JPEG data is color image, compression is lossy, and
   *  the underlying JPEG library "guesses" the color space
   *  to be YCbCr.
   */
  EDC_guessLossyOnly,

  /** perform color space conversion from YCbCr to RGB if
   *  JPEG data is color image, and the underlying JPEG
   *  library "guesses" the color space to be YCbCr.
   */
  EDC_guess
};


// CONDITION CONSTANTS

/// IJG codec suspension return
extern DCMTK_DCMJPEG_EXPORT const OFConditionConst EJ_Suspension;
/// Buffer for decompressed image (8 bits/sample) too small
extern DCMTK_DCMJPEG_EXPORT const OFConditionConst EJ_IJG8_FrameBufferTooSmall;
/// Buffer for decompressed image (12 bits/sample) too small
extern DCMTK_DCMJPEG_EXPORT const OFConditionConst EJ_IJG12_FrameBufferTooSmall;
/// Buffer for decompressed image (16 bits/sample) too small
extern DCMTK_DCMJPEG_EXPORT const OFConditionConst EJ_IJG16_FrameBufferTooSmall;
/// Codec does not support this PhotometricInterpretation
extern DCMTK_DCMJPEG_EXPORT const OFConditionConst EJ_UnsupportedPhotometricInterpretation;
/// Codec does not support this kind of color conversion
extern DCMTK_DCMJPEG_EXPORT const OFConditionConst EJ_UnsupportedColorConversion;

// reserved condition codes for IJG error messages
const unsigned short EJCode_IJG8_Compression    = 0x0100;
const unsigned short EJCode_IJG8_Decompression  = 0x0101;
const unsigned short EJCode_IJG12_Compression   = 0x0102;
const unsigned short EJCode_IJG12_Decompression = 0x0103;
const unsigned short EJCode_IJG16_Compression   = 0x0104;
const unsigned short EJCode_IJG16_Decompression = 0x0105;

/** helper class with static methods used from different dcmjpeg classes
 *  (in particular from the encoder and the decoder part).
 */
class DCMTK_DCMJPEG_EXPORT DcmJpegHelper
{
public:

  /** helper function that locates the photometric interpretation attribute
   *  in a DICOM dataset and returns a parsed value.
   *  @param item the dataset in which the element is searched
   *  @return photometric interpretation enum, EPI_Unknown if unknown string or attribute missing
   */
  static EP_Interpretation getPhotometricInterpretation(DcmItem *item);
};

#endif
