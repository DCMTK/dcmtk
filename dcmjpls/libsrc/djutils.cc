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
 *  Purpose: Support code for dcmjpls
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpls/djlsutil.h"
#include "dcmtk/dcmdata/dcerror.h"

OFLogger DCM_dcmjplsLogger = OFLog::getLogger("dcmtk.dcmjpls");

#define MAKE_DCMJPLS_ERROR(number, name, description)  \
makeOFConditionConst(EC_ ## name, OFM_dcmjpls, number, OF_error, description)

MAKE_DCMJPLS_ERROR( 1, JLSUncompressedBufferTooSmall, "Uncompressed pixel data too short for uncompressed image");
MAKE_DCMJPLS_ERROR( 2, JLSCompressedBufferTooSmall, "Allocated too small buffer for compressed image data");
MAKE_DCMJPLS_ERROR( 3, JLSCodecUnsupportedImageType, "Codec does not support this JPEG-LS image");
MAKE_DCMJPLS_ERROR( 4, JLSCodecInvalidParameters, "Codec received invalid compression parameters");
MAKE_DCMJPLS_ERROR( 5, JLSCodecUnsupportedValue, "Codec received unsupported compression parameters");
MAKE_DCMJPLS_ERROR( 6, JLSInvalidCompressedData, "Invalid compressed image data");
MAKE_DCMJPLS_ERROR( 7, JLSUnsupportedBitDepthForTransform, "Codec does not support the image's color transformation with this bit depth");
MAKE_DCMJPLS_ERROR( 8, JLSUnsupportedColorTransform, "Codec does not support the image's color transformation");
MAKE_DCMJPLS_ERROR( 9, JLSUnsupportedBitDepth, "Unsupported bit depth in JPEG-LS transfer syntax");
MAKE_DCMJPLS_ERROR(10, JLSCannotComputeNumberOfFragments, "Cannot compute number of fragments for JPEG-LS frame");
MAKE_DCMJPLS_ERROR(11, JLSImageDataMismatch, "Image data mismatch between DICOM header and JPEG-LS bitstream");
MAKE_DCMJPLS_ERROR(12, JLSUnsupportedPhotometricInterpretation, "Unsupported photometric interpretation for near-lossless JPEG-LS compression");
MAKE_DCMJPLS_ERROR(13, JLSUnsupportedPixelRepresentation, "Unsupported pixel representation for near-lossless JPEG-LS compression");
MAKE_DCMJPLS_ERROR(14, JLSUnsupportedImageType, "Unsupported type of image for JPEG-LS compression");
MAKE_DCMJPLS_ERROR(15, JLSTooMuchCompressedData, "Too much compressed data, trailing data after image");
