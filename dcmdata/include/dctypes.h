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
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCTYPES_H
#define DCTYPES_H 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */

/* Define a simple boolean type */

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

typedef int BOOL;


/* Define datatypes of fixed sizes for DICOM I/O */

#if SIZEOF_CHAR == 1
typedef unsigned char 	Uint8;
typedef signed char 	Sint8;
typedef Uint8		BYTE;
#else
#error "DCMTK ERROR: cannot declare a 1 byte datatype"
#endif

#if SIZEOF_LONG == 4
typedef signed long	Sint32;
typedef unsigned long	Uint32;
#elif SIZEOF_INT == 4
typedef signed int	Sint32;
typedef unsigned int	Uint32;
#else
#error "DCMTK ERROR: cannot declare a 4 byte integer datatype"
#endif

#if SIZEOF_SHORT == 2
typedef signed short	Sint16;
typedef unsigned short	Uint16;
#else
#error "DCMTK ERROR: cannot declare a 2 byte integer datatype"
#endif

#if SIZEOF_FLOAT == 4
typedef float		Float32;	/* 32 Bit Floating Point Single */
#else
#error "DCMTK ERROR: cannot declare a 4 byte floating point datatype"
#endif

#if SIZEOF_DOUBLE == 8
typedef double		Float64;	/* 64 Bit Floating Point Double */
#else
#error "DCMTK ERROR: cannot declare an 8 byte floating point datatype"
#endif

/*
** The following T_VR_?? types should be avoided
** Only here for backwards compatibility
** Use the Sint16, Uint16, Sint32, Uint32 types instead.
*/

typedef Uint32		T_VR_UL;	/* DICOM Value Representation UL */
typedef Sint32		T_VR_SL;	/* DICOM Value Representation SL */
typedef Uint16		T_VR_US;	/* DICOM Value Representation US */
typedef Sint16		T_VR_SS;	/* DICOM Value Representation SS */
typedef Float32		T_VR_FL;	/* DICOM Value Representation FL */
typedef Float64		T_VR_FD;	/* DICOM Value Representation FD */

/*
** Enumerated Types
*/


typedef enum {
    EET_ExplicitLength = 0,
    EET_UndefinedLength = 1
} E_EncodingType;


typedef enum {
    EGL_withoutGL = 0,
    EGL_withGL = 1
} E_GrpLenEncoding;


typedef enum {
    ESM_fromHere = 0,
    ESM_fromStackTop = 1,
    ESM_afterStackTop = 2
} E_SearchMode;


typedef enum {
    ERW_init = 0,
    ERW_ready = 1,
    ERW_inWork = 2
} E_ReadWriteState;


#endif /* !DCTYPES_H */

