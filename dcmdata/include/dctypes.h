/*
 *
 * Author: Gerd Ehlers      Created:  03-20-94
 *         Andrew Hewett    Modified: 04-11-95
 *
 * Module: dctypes.h
 *
 * Purpose:
 * This file contains global type definitions
 *
 *
 * Last Update:   $Author: andreas $
 * Revision:      $Revision: 1.5 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCTYPES_H
#define DCTYPES_H 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

typedef int BOOL;


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


