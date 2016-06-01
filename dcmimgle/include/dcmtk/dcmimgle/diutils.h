/*
 *
 *  Copyright (C) 1996-2016, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Utilities (Header)
 *
 */


#ifndef DIUTILS_H
#define DIUTILS_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/oflog/oflog.h"

#include "dcmtk/dcmimgle/didefine.h"

extern DCMTK_DCMIMGLE_EXPORT OFLogger DCM_dcmimgleLogger;

#define DCMIMGLE_TRACE(msg) OFLOG_TRACE(DCM_dcmimgleLogger, msg)
#define DCMIMGLE_DEBUG(msg) OFLOG_DEBUG(DCM_dcmimgleLogger, msg)
#define DCMIMGLE_INFO(msg)  OFLOG_INFO(DCM_dcmimgleLogger, msg)
#define DCMIMGLE_WARN(msg)  OFLOG_WARN(DCM_dcmimgleLogger, msg)
#define DCMIMGLE_ERROR(msg) OFLOG_ERROR(DCM_dcmimgleLogger, msg)
#define DCMIMGLE_FATAL(msg) OFLOG_FATAL(DCM_dcmimgleLogger, msg)


// include this file in doxygen documentation

/** @file diutils.h
 *  @brief utilities for the dcmimgle/dcmimage module
 */


/*---------------------*
 *  const definitions  *
 *---------------------*/

/** @name configuration flags
 */

//@{

/// compatibility with old ACR-NEMA images
const unsigned long CIF_AcrNemaCompatibility         = 0x0000001;

/// accept wrong palette attribute tags
const unsigned long CIF_WrongPaletteAttributeTags    = 0x0000002;

/// element pixel data may be detached if it is no longer needed by DicomImage
const unsigned long CIF_MayDetachPixelData           = 0x0000004;

/// use presentation state instead of 'built-in' LUTs & overlays
const unsigned long CIF_UsePresentationState         = 0x0000008;

/// don't convert YCbCr (Full and Full 4:2:2) color images to RGB
const unsigned long CIF_KeepYCbCrColorModel          = 0x0000010;

/// take responsibility for the given external DICOM dataset, i.e. delete it on destruction
const unsigned long CIF_TakeOverExternalDataset      = 0x0000020;

/// ignore modality transformation (rescale slope/intercept or LUT) stored in the dataset
const unsigned long CIF_IgnoreModalityTransformation = 0x0000040;

/// ignore third value of the modality LUT descriptor, determine bit depth automatically
const unsigned long CIF_IgnoreModalityLutBitDepth    = 0x0000080;

/// check third value of the LUT descriptor, compare with with expected bit depth based on LUT data
const unsigned long CIF_CheckLutBitDepth             = 0x0000100;

/// use absolute (possible) pixel range for determining the internal representation (monochrome only)
const unsigned long CIF_UseAbsolutePixelRange        = 0x0000200;

/// use partial access to pixel data, i.e. without decompressing or loading a complete multi-frame image.
/// Please note that the use of this flag can cause another copy of the pixel data to be created in memory,
/// e.g. in case the pixel data element value has already been loaded or decompressed completely in memory.
const unsigned long CIF_UsePartialAccessToPixelData  = 0x0000400;

/// always decompress complete pixel data when processing an image, i.e. even if partial access is used
const unsigned long CIF_DecompressCompletePixelData  = 0x0000800;

/// never access embedded overlays since this requires to load and uncompress the complete pixel data
const unsigned long CIF_NeverAccessEmbeddedOverlays  = 0x0001000;
//@}


// / true color color mode (for monochrome images only)
const int MI_PastelColor = -1;


/*--------------------*
 *  type definitions  *
 *--------------------*/

/** constants for photometric interpretation
 */
enum EP_Interpretation
{
    /// unknown, undefined, invalid
    EPI_Unknown,
    /// no element value available
    EPI_Missing,
    /// monochrome 1
    EPI_Monochrome1,
    /// monochrome 2
    EPI_Monochrome2,
    /// palette color
    EPI_PaletteColor,
    /// RGB color
    EPI_RGB,
    /// HSV color (retired)
    EPI_HSV,
    /// ARGB color (retired)
    EPI_ARGB,
    /// CMYK color (retired)
    EPI_CMYK,
    /// YCbCr full
    EPI_YBR_Full,
    /// YCbCr full 4:2:2
    EPI_YBR_Full_422,
    /// YCbCr partial 4:2:2
    EPI_YBR_Partial_422
};


/** structure for photometric string and related constant
 */
struct DCMTK_DCMIMGLE_EXPORT SP_Interpretation
{
    /// string (name of the color model without spaces and underscores)
    const char *Name;
    /// defined term according to the DICOM standard
    const char *DefinedTerm;
    /// integer constant
    EP_Interpretation Type;
};


/** structure for BMP bitmap file header
 */
struct DCMTK_DCMIMGLE_EXPORT SB_BitmapFileHeader
{
    /// signature, must always be 'BM'
    char bfType[2];
    /// file size in bytes
    Uint32 bfSize;
    /// reserved, should be '0'
    Uint16 bfReserved1;
    /// reserved, should be '0'
    Uint16 bfReserved2;
    /// offset from the beginning of the file to the bitmap data (in bytes)
    Uint32 bfOffBits;
};


/** structure for BMP bitmap info header
 */
struct DCMTK_DCMIMGLE_EXPORT SB_BitmapInfoHeader
{
    /// size of the BitmapInfoHeader, usually '40'
    Uint32 biSize;
    /// width of the image (in pixels)
    Sint32 biWidth;
    /// height of the image (in pixels)
    Sint32 biHeight;
    /// number of planes, usually '1'
    Uint16 biPlanes;
    /// bits per pixel, supported values: 8 = color palette with 256 entries, 24 = true color
    Uint16 biBitCount;
    /// type of compression, support value: 0 = BI_RGB, no compression
    Uint32 biCompression;
    /// size of the image data (in bytes), might be set to '0' if image is uncompressed
    Uint32 biSizeImage;
    /// horizontal resolution: pixels/meter, usually set to '0'
    Sint32 biXPelsPerMeter;
    /// vertical resolution: pixels/meter, usually set to '0'
    Sint32 biYPelsPerMeter;
    /// number of actually used colors, if '0' the number of colors is calculated using 'biBitCount'
    Uint32 biClrUsed;
    /// number of important colors, '0' means all
    Uint32 biClrImportant;
};


/** internal representation of pixel data
 */
enum EP_Representation
{
    /// unsigned 8 bit integer
    EPR_Uint8, EPR_MinUnsigned = EPR_Uint8,
    /// signed 8 bit integer
    EPR_Sint8, EPR_MinSigned = EPR_Sint8,
    /// unsigned 16 bit integer
    EPR_Uint16,
    /// signed 16 bit integer
    EPR_Sint16,
    /// unsigned 32 bit integer
    EPR_Uint32, EPR_MaxUnsigned = EPR_Uint32,
    /// signed 32 bit integer
    EPR_Sint32, EPR_MaxSigned = EPR_Sint32
};


/** image status code
 */
enum EI_Status
{
    /// normal, no error
    EIS_Normal,
    /// data dictionary not found
    EIS_NoDataDictionary,
    /// invalid dataset/file
    EIS_InvalidDocument,
    /// mandatory attribute missing
    EIS_MissingAttribute,
    /// invalid value for an important attribute
    EIS_InvalidValue,
    /// specified value for an attribute not supported
    EIS_NotSupportedValue,
    /// memory exhausted etc.
    EIS_MemoryFailure,
    /// invalid image, internal error
    EIS_InvalidImage,
    /// other error
    EIS_OtherError
};


/** overlay modes.
 *  This mode is used to define how to display an overlay plane.
 */
enum EM_Overlay
{
    /// default mode, as stored in the dataset
    EMO_Default,
    /// replace mode
    EMO_Replace,
    /// graphics overlay
    EMO_Graphic = EMO_Replace,
    /// threshold replace
    EMO_ThresholdReplace,
    /// complement
    EMO_Complement,
    /// invert the overlay bitmap
    EMO_InvertBitmap,
    /// region of interest (ROI)
    EMO_RegionOfInterest,
    /// bitmap shutter, used for GSPS objects
    EMO_BitmapShutter
};


/** VOI LUT functions
 */
enum EF_VoiLutFunction
{
    /// default function (not explicitly set)
    EFV_Default,
    /// function LINEAR
    EFV_Linear,
    /// function SIGMOID
    EFV_Sigmoid
};


/** presentation LUT shapes
 */
enum ES_PresentationLut
{
    /// default shape (not explicitly set)
    ESP_Default,
    /// shape IDENTITY
    ESP_Identity,
    /// shape INVERSE
    ESP_Inverse,
    /// shape LIN OD
    ESP_LinOD
};


/** polarity
 */
enum EP_Polarity
{
    /// NORMAL
    EPP_Normal,
    /// REVERSE (opposite polarity)
    EPP_Reverse
};


/** bits per table entry modes.
 *  Specifies whether the given value in the LUT descriptor is used.
 */
enum EL_BitsPerTableEntry
{
    /// use given value
    ELM_UseValue,
    /// ignore given value, use auto detection
    ELM_IgnoreValue,
    /// check whether given value is consistent with LUT data
    ELM_CheckValue
};


/*----------------------------*
 *  constant initializations  *
 *----------------------------*/

const SP_Interpretation PhotometricInterpretationNames[] =
{
    {"MONOCHROME1",   "MONOCHROME1",     EPI_Monochrome1},
    {"MONOCHROME2",   "MONOCHROME2",     EPI_Monochrome2},
    {"PALETTECOLOR",  "PALETTE COLOR",   EPI_PaletteColor},        // space deleted to simplify detection
    {"RGB",           "RGB",             EPI_RGB},
    {"HSV",           "HSV",             EPI_HSV},
    {"ARGB",          "ARGB",            EPI_ARGB},
    {"CMYK",          "CMYK",            EPI_CMYK},
    {"YBRFULL",       "YBR_FULL",        EPI_YBR_Full},            // underscore deleted to simplify detection
    {"YBRFULL422",    "YBR_FULL_422",    EPI_YBR_Full_422},        // underscores deleted to simplify detection
    {"YBRPARTIAL422", "YBR_PARTIAL_422", EPI_YBR_Partial_422},     // underscores deleted to simplify detection
    {NULL,            NULL,              EPI_Unknown}
};


/*---------------------*
 *  macro definitions  *
 *---------------------*/

#define MAX_UINT Uint32
#define MAX_SINT Sint32

#define MAX_BITS 32
#define MAX_BITS_TYPE Uint32
#define MAX_RAWPPM_BITS 8
#define MAX_INTERPOLATION_BITS 16

#define bitsof(expr) (sizeof(expr) << 3)


/*----------------------*
 *  class declarations  *
 *----------------------*/

/** Class comprising several global functions and constants.
 *  introduced to avoid problems with naming convention
 */
class DCMTK_DCMIMGLE_EXPORT DicomImageClass
{

 public:

    /** calculate maximum value which could be stored in the specified number of bits
     *
     ** @param  mv_bits  number of bits
     *  @param  mv_pos   value subtracted from the maximum value (0 or 1)
     *
     ** @return maximum value
     */
    static inline unsigned long maxval(const int mv_bits,
                                       const unsigned long mv_pos = 1)
    {
        return (mv_bits < MAX_BITS) ?
            (OFstatic_cast(unsigned long, 1) << mv_bits) - mv_pos : OFstatic_cast(MAX_BITS_TYPE, -1);
    }

    /** calculate number of bits which are necessary to store the specified value
     *
     ** @param  tb_value  value to be stored
     *  @param  tb_pos    value subtracted from the value (0 or 1) before converting
     *
     ** @return number of bits
     */
    static inline unsigned int tobits(unsigned long tb_value,
                                      const unsigned long tb_pos = 1)
    {
        if (tb_value > 0)
            tb_value -= tb_pos;
        unsigned int tb_bits = 0;
        while (tb_value > 0)
        {
            ++tb_bits;
            tb_value >>= 1;
        }
        return tb_bits;
    }

    /** calculate number of bits which are necessary to store the specified value range
     *
     ** @param  minvalue  minimum value to be stored
     *  @param  maxvalue  maximum value to be stored
     *
     ** @return number of bits
     */
    static unsigned int rangeToBits(double minvalue,
                                    double maxvalue);

    /** determine whether integer representation is signed or unsigned
     *
     ** @param  repres  integer representation (enum) to be checked
     *
     ** @return true if representation is signed, false if unsigned
     */
    static int isRepresentationSigned(EP_Representation repres);

    /** determine number of bits used for a particular integer representation
     *
     ** @param  repres  integer representation (enum) to be checked
     *
     ** @return number of bits
     */
    static unsigned int getRepresentationBits(EP_Representation repres);

    /** determine integer representation which is necessary to store values in the specified range
     *
     ** @param  minvalue  minimum value to be stored
     *  @param  maxvalue  maximum value to be stored
     *
     ** @return integer representation (enum)
     */
    static EP_Representation determineRepresentation(double minvalue,
                                                     double maxvalue);

};


#endif
