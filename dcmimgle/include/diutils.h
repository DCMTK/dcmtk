/*
 *
 *  Copyright (C) 1996-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Utilities (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2001-11-09 16:25:59 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diutils.h,v $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIUTILS_H
#define __DIUTILS_H

#include "osconfig.h"
#include "dctypes.h"

#include "ofglobal.h"

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

BEGIN_EXTERN_C
#ifdef HAVE_LIBC_H
#include <libc.h>
#endif
#include <stdio.h>
END_EXTERN_C


/*---------------------*
 *  const definitions  *
 *---------------------*/

/** @name configuration flags
 */

//@{

/// compatibility with old ACR-NEMA images
const unsigned long CIF_AcrNemaCompatibility      = 0x0000001;

/// accept wrong palette attribute tags
const unsigned long CIF_WrongPaletteAttributeTags = 0x0000002;

/// element pixel data may be detached if it is no longer needed by dcmimage
const unsigned long CIF_MayDetachPixelData        = 0x0000004;

/// use presentation state instead of 'built-in' LUTs & overlays
const unsigned long CIF_UsePresentationState      = 0x0000008;

/// don't convert YCbCr (Full and Full 4:2:2) color images to RGB
const unsigned long CIF_KeepYCbCrColorModel       = 0x0000010;

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
    /// monochrome 1
    EPI_Monochrome1,
    /// monochrome 2
    EPI_Monochrome2,
    /// palette color
    EPI_PaletteColor,
    /// RGB color
    EPI_RGB,
    /// HSV color
    EPI_HSV,
    /// ARGB color
    EPI_ARGB,
    /// CMYK color
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
struct SP_Interpretation
{
    /// string
    const char *Name;
    /// constant
    EP_Interpretation Type;
};


/** structure for BMP bitmap file header
 */
struct SB_BitmapFileHeader
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
struct SB_BitmapInfoHeader
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
    /// complement, inverse
    EMO_Complement,
    /// region of interest (ROI)
    EMO_RegionOfInterest,
    /// bitmap shutter
    EMO_BitmapShutter
};


/** presentation LUT shapes
 */
enum ES_PresentationLut
{
    /// default shape (not expicitly set)
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


/*----------------------------*
 *  constant initializations  *
 *----------------------------*/

const SP_Interpretation PhotometricInterpretationNames[] =
{
    {"MONOCHROME1",   EPI_Monochrome1},
    {"MONOCHROME2",   EPI_Monochrome2},
    {"PALETTECOLOR",  EPI_PaletteColor},        // space deleted to simplify detection
    {"RGB",           EPI_RGB},
    {"HSV",           EPI_HSV},
    {"ARGB",          EPI_ARGB},
    {"CMYK",          EPI_CMYK},
    {"YBRFULL",       EPI_YBR_Full},            // underscore deleted to simplify detection
    {"YBRFULL422",    EPI_YBR_Full_422},        // underscores deleted to simplify detection
    {"YBRPARTIAL422", EPI_YBR_Partial_422},     // underscores deleted to simplify detection
    {NULL,            EPI_Unknown}
};


/*---------------------*
 *  macro definitions  *
 *---------------------*/

#define MAX_UINT Uint32
#define MAX_SINT Sint32

#define MAX_BITS 32
#define MAX_RAWPPM_BITS 8
#define MAX_INTERPOLATION_BITS 16

#define bitsof(expr) (sizeof(expr) << 3)


/*----------------------*
 *  class declarations  *
 *----------------------*/

/** Class comprising several global functions and constants.
 *  introduced to avoid problems with naming convention
 */
class DicomImageClass
{

 public:

    /** calculate maximum value which could be stored in the specified number of bits
     *
     ** @param  mv_bits  number of bits
     *  @param  mv_pos   value substracted from the maximum value (0 or 1)
     *
     ** @return maximum value
     */
    static inline unsigned long maxval(const int mv_bits,
                                       const unsigned long mv_pos = 1)
    {
        return (mv_bits < MAX_BITS) ? ((unsigned long)1 << mv_bits) - mv_pos : (unsigned long)-1;
    }

    /** calculate number of bits which are necessary to store the specified value
     *
     ** @param  tb_value  value to be stored
     *  @param  tb_pos    value substracted from the value (0 or 1) before converting
     *
     ** @return number of bits
     */
    static inline unsigned int tobits(unsigned long tb_value,
                                      const unsigned long tb_pos = 1)
    {
        if (tb_value > 0)
            tb_value -= tb_pos;
        register unsigned int tb_bits = 0;
        while (tb_value > 0)
        {
            tb_bits++;
            tb_value >>= 1;
        }
        return tb_bits;
    }

    /** determine integer representation which is necessary to store values in the specified range
     *
     ** @param  minvalue  minimum value to be stored
     *  @param  maxvalue  maximum value to be stored
     *
     ** @return integer representation (enum)
     */
    static EP_Representation determineRepresentation(double minvalue,
                                                     double maxvalue);

    /** set the debug level to the specified value
     *
     ** @param  level  debug level to be set
     */
    static void setDebugLevel(const int level)
    {
        DebugLevel.set(level);
    }
    
    /** get the current debug level
     *
     ** @return  current debug level
     */
    static int getDebugLevel()
    {
        return DebugLevel.get();
    }
    
    /** check whether specified debug level is set
     *
     ** @param  level  debug levelto be checked
     *
     ** @return true if debug level is set, false (0) otherwise
     */
    static int checkDebugLevel(const int level)
    {
        return DebugLevel.get() & level;
    }


    /// debug level: display no messages
    static const int DL_NoMessages;
    /// debug level: display error messages
    static const int DL_Errors;
    /// debug level: display warning messages
    static const int DL_Warnings;
    /// debug level: display informational messages
    static const int DL_Informationals;
    /// debug level: display debug messages
    static const int DL_DebugMessages;


  private:
  
    /// debug level defining the verboseness of the image toolkit
    static OFGlobal<int> DebugLevel;
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diutils.h,v $
 * Revision 1.19  2001-11-09 16:25:59  joergr
 * Added support for Window BMP file format.
 *
 * Revision 1.18  2001/09/28 13:11:00  joergr
 * Added new flag (CIF_KeepYCbCrColorModel) which avoids conversion of YCbCr
 * color models to RGB.
 *
 * Revision 1.17  2001/06/01 15:49:52  meichel
 * Updated copyright header
 *
 * Revision 1.16  2000/07/07 13:40:31  joergr
 * Added support for LIN OD presentation LUT shape.
 *
 * Revision 1.15  2000/06/07 14:30:28  joergr
 * Added method to set the image polarity (normal, reverse).
 *
 * Revision 1.14  2000/04/28 12:32:33  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.13  2000/03/08 16:24:25  meichel
 * Updated copyright header.
 *
 * Revision 1.12  2000/02/23 15:12:16  meichel
 * Corrected macro for Borland C++ Builder 4 workaround.
 *
 * Revision 1.11  2000/02/01 10:52:38  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.10  1999/09/17 13:08:13  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.9  1999/07/23 14:16:16  joergr
 * Added flag to avoid color space conversion for color images (not yet
 * implemented).
 *
 * Revision 1.8  1999/04/30 16:33:19  meichel
 * Now including stdio.h in diutils.h, required on SunOS
 *
 * Revision 1.7  1999/04/28 14:55:41  joergr
 * Added experimental support to create grayscale images with more than 256
 * shades of gray to be displayed on a consumer monitor (use pastel colors).
 *
 * Revision 1.6  1999/03/24 17:20:28  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.5  1999/02/03 17:36:06  joergr
 * Moved global functions maxval() and determineRepresentation() to class
 * DicomImageClass (as static methods).
 * Added BEGIN_EXTERN_C and END_EXTERN_C to some C includes.
 *
 * Revision 1.4  1999/01/20 15:13:12  joergr
 * Added new overlay plane mode for bitmap shutters.
 *
 * Revision 1.3  1998/12/23 11:38:08  joergr
 * Introduced new overlay mode item EMO_Graphic (= EMO_Replace).
 *
 * Revision 1.2  1998/12/16 16:40:15  joergr
 * Some layouting.
 *
 * Revision 1.1  1998/11/27 15:51:45  joergr
 * Added copyright message.
 * Introduced global debug level for dcmimage module to control error output.
 * Moved type definitions to diutils.h.
 * Added methods to support presentation LUTs and shapes.
 * Introduced configuration flags to adjust behaviour in different cases.
 *
 * Revision 1.5  1998/06/25 08:50:10  joergr
 * Added compatibility mode to support ACR-NEMA images and wrong
 * palette attribute tags.
 *
 * Revision 1.4  1998/05/11 14:53:30  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
