/*
 *
 *  Copyright (C) 1994-99, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andrew Hewett
 *
 *  Purpose: global type definitions
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-03-31 09:24:51 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dctypes.h,v $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCTYPES_H
#define DCTYPES_H 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "oftypes.h"

typedef unsigned char 	Uint8;
typedef signed char 	Sint8;
typedef Uint8		BYTE;
typedef Sint8		SBYTE;

#if SIZEOF_LONG == 8
typedef signed int	Sint32;
typedef unsigned int	Uint32;
#else
typedef signed long	Sint32;
typedef unsigned long	Uint32;
#endif

typedef signed short	Sint16;
typedef unsigned short	Uint16;

typedef float		Float32;	/* 32 Bit Floating Point Single */
typedef double		Float64;	/* 64 Bit Floating Point Double */

/*
** Enumerated Types
*/


typedef enum {
    EET_ExplicitLength = 0,
    EET_UndefinedLength = 1
} E_EncodingType;


typedef enum {
    EGL_noChange = 0,  // no change of GL values, WARNING: DO NOT USE FOR WRITE
    EGL_withoutGL = 1, // remove group length tags
    EGL_withGL = 2,    // add group length tags for every group
    EGL_recalcGL = 3   // recalculate values for existing group length tags
} E_GrpLenEncoding;

typedef enum {
    EPD_noChange = 0,        // no change of padding tags
    EPD_withoutPadding = 1,  // remove all padding tags
    EPD_withPadding = 2      // add padding tags
} E_PaddingEncoding;


typedef enum {
    ESM_fromHere = 0,
    ESM_fromStackTop = 1,
    ESM_afterStackTop = 2
} E_SearchMode;


typedef enum {
    ERW_init = 0,
    ERW_ready = 1,
    ERW_inWork = 2,
    ERW_notInitialized = 3
} E_TransferState;


// Undefined Length Identifier
const Uint32 DCM_UndefinedLength = 0xffffffff;

#endif /* !DCTYPES_H */


/*
 * CVS/RCS Log:
 * $Log: dctypes.h,v $
 * Revision 1.7  1999-03-31 09:24:51  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
