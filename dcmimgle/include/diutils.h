/*
 *
 *  Copyright (C) 1996-99, OFFIS
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
 *  Update Date:      $Date: 1999-03-24 17:20:28 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diutils.h,v $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIUTILS_H
#define __DIUTILS_H

#include "osconfig.h"
#include "dctypes.h"

BEGIN_EXTERN_C
 #ifdef HAVE_LIBC_H
  #include <libc.h>
 #else
  #include <stdlib.h>
 #endif
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

//@}


/*--------------------*
 *  type definitions  *
 *--------------------*/

/** constants for photometric interpretation
 */
enum EP_Interpretation
{
    EPI_Unknown,
    EPI_Monochrome1,
    EPI_Monochrome2,
    EPI_PaletteColor,
    EPI_RGB,
    EPI_HSV,
    EPI_ARGB,
    EPI_CMYK,
    EPI_YBR_Full,
    EPI_YBR_Full_422,
    EPI_YBR_Partial_422
};


/** structure for photometric string and related constant
 */
struct SP_Interpretation
{
    const char *Name;
    EP_Interpretation Type;
};


/** internal representation of pixel data
 */
enum EP_Representation 
{
    EPR_Uint8, EPR_MinUnsigned = EPR_Uint8,
    EPR_Sint8, EPR_MinSigned = EPR_Sint8,
    EPR_Uint16,
    EPR_Sint16,
    EPR_Uint32, EPR_MaxUnsigned = EPR_Uint32,
    EPR_Sint32, EPR_MaxSigned = EPR_Sint32
};


/** status code
 */
enum EI_Status 
{
    EIS_Normal,
    EIS_NoDataDictionary,
    EIS_InvalidDocument,
    EIS_MissingAttribute,
    EIS_InvalidValue,
    EIS_NotSupportedValue,
    EIS_MemoryFailure,
    EIS_InvalidImage,
    EIS_OtherError
};


/** overlay modes
 */
enum EM_Overlay
{
    EMO_Default,
    EMO_Replace,
    EMO_Graphic = EMO_Replace,
    EMO_ThresholdReplace,
    EMO_Complement,
    EMO_RegionOfInterest,
    EMO_BitmapShutter
};


/** presentation LUT shapes
 */
enum ES_PresentationLut
{
    ESP_Identity,
    ESP_Inverse
};


/*----------------------------*
 *  constant initializations  *
 *----------------------------*/

/** definition of photometric strings and related constants
 */
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

    static inline unsigned long maxval(const int mv_bits,
                                       const unsigned long mv_pos = 1)
    {
        return (mv_bits < MAX_BITS) ? ((unsigned long)1 << mv_bits) - mv_pos : (unsigned long)-1;
    }

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

    static EP_Representation determineRepresentation(double minvalue,
                                                     double maxvalue);

    static const int DL_NoMessages;
    static const int DL_Errors;
    static const int DL_Warnings;
    static const int DL_Informationals;
    static const int DL_DebugMessages;
    
    static int DebugLevel;    
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diutils.h,v $
 * Revision 1.6  1999-03-24 17:20:28  joergr
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
