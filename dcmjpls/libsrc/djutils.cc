/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:24 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpls/djlsutil.h"
#include "dcmtk/dcmdata/dcerror.h"

OFLogger DCM_dcmjplsGetLogger()
{
    // We don't just use a global variable, because constructors of globals are
    // executed in random order. This guarantees that the OFLogger is constructed
    // before first use.
    static OFLogger DCM_dcmjplsLogger = OFLog::getLogger("dcmtk.dcmjpls");
    return DCM_dcmjplsLogger;
}

#define MAKE_DCMJPLS_ERROR(number, name, description)  \
const OFConditionConst ECC_ ## name (OFM_dcmjpls, number, OF_error, description); \
const OFCondition      EC_  ## name (ECC_ ## name)

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

/*
 * CVS/RCS Log:
 * $Log: djutils.cc,v $
 * Revision 1.6  2010-10-14 13:14:24  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.5  2010-10-05 10:15:19  uli
 * Fixed all remaining warnings from -Wall -Wextra -pedantic.
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
 * Revision 1.1  2009-07-29 14:46:48  meichel
 * Initial release of module dcmjpls, a JPEG-LS codec for DCMTK based on CharLS
 *
 * Revision 1.6  2007-06-20 12:37:37  meichel
 * Completed implementation of encoder, which now supports lossless
 *   "raw" and "cooked" and near-lossless "cooked" modes.
 *
 * Revision 1.5  2007/06/15 14:35:45  meichel
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
