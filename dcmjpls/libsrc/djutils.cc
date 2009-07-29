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
 *  Purpose: Support code for dcmjpls
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2009-07-29 14:46:48 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpls/libsrc/djutils.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpls/djlsutil.h"
#include "dcmtk/dcmdata/dcerror.h"

const OFConditionConst ECC_JLSCodecError                           ( OFM_dcmjpls,  1, OF_error, "JPEG-LS codec error");
const OFConditionConst ECC_JLSUnsupportedBitDepth                  ( OFM_dcmjpls,  2, OF_error, "Unsupported bit depth in JPEG-LS transfer syntax");
const OFConditionConst ECC_JLSCannotComputeNumberOfFragments       ( OFM_dcmjpls,  3, OF_error, "Cannot compute number of fragments for JPEG-LS frame");
const OFConditionConst ECC_JLSTempFileWriteError                   ( OFM_dcmjpls,  4, OF_error, "Error while writing temporary file");
const OFConditionConst ECC_JLSTempFileReadError                    ( OFM_dcmjpls,  5, OF_error, "Error while reading temporary file");
const OFConditionConst ECC_JLSUnknownPNMFormat                     ( OFM_dcmjpls,  6, OF_error, "Unknown PNM file format found in temporary file");
const OFConditionConst ECC_JLSImageDataMismatch                    ( OFM_dcmjpls,  7, OF_error, "Image data mismatch between DICOM header and JPEG-LS bitstream");
const OFConditionConst ECC_JLSUnsupportedPhotometricInterpretation ( OFM_dcmjpls,  8, OF_error, "Unsupported photometric interpretation for near-lossless JPEG-LS compression");
const OFConditionConst ECC_JLSUnsupportedPixelRepresentation       ( OFM_dcmjpls,  9, OF_error, "Unsupported pixel representation for near-lossless JPEG-LS compression");
const OFConditionConst ECC_JLSUnsupportedImageType                 ( OFM_dcmjpls, 10, OF_error, "Unsupported type of image for JPEG-LS compression");
const OFConditionConst ECC_JLSPixelDataTooShort                    ( OFM_dcmjpls, 11, OF_error, "Pixel data too short for uncompressed image");

const OFCondition      EC_JLSCodecError(                           ECC_JLSCodecError                     );
const OFCondition      EC_JLSUnsupportedBitDepth(                  ECC_JLSUnsupportedBitDepth            );
const OFCondition      EC_JLSCannotComputeNumberOfFragments(       ECC_JLSCannotComputeNumberOfFragments );
const OFCondition      EC_JLSTempFileWriteError(                   ECC_JLSTempFileWriteError             );
const OFCondition      EC_JLSTempFileReadError(                    ECC_JLSTempFileReadError              );
const OFCondition      EC_JLSUnknownPNMFormat(                     ECC_JLSUnknownPNMFormat               );
const OFCondition      EC_JLSImageDataMismatch(                    ECC_JLSImageDataMismatch              );
const OFCondition      EC_JLSUnsupportedPhotometricInterpretation( ECC_JLSUnsupportedPhotometricInterpretation );
const OFCondition      EC_JLSUnsupportedPixelRepresentation(       ECC_JLSUnsupportedPixelRepresentation );
const OFCondition      EC_JLSUnsupportedImageType(                 ECC_JLSUnsupportedImageType );
const OFCondition      EC_JLSPixelDataTooShort(                    ECC_JLSPixelDataTooShort );

/*
 * CVS/RCS Log:
 * $Log: djutils.cc,v $
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
