/*
 *
 *  Copyright (C) 1997-2005, OFFIS
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
 *  Module:  ofstd
 *
 *  Author:  Andreas Barth
 *
 *  Purpose:
 *      Defines some C++ standard types that are not consistently 
 *      supported by all C++ Compilers
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-05-25 10:02:36 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/dcmtk/ofstd/oftypes.h,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef OFTYPES_H
#define OFTYPES_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>                /* Needed for int64_t */
#endif

#ifdef __CHAR_UNSIGNED__
typedef signed char     Sint8;
#else 
typedef char            Sint8;
#endif

typedef unsigned char   Uint8;

#if SIZEOF_LONG == 8
typedef signed int      Sint32;
typedef unsigned int    Uint32;
#else
typedef signed long     Sint32;
typedef unsigned long   Uint32;
#endif

typedef signed short    Sint16;
typedef unsigned short  Uint16;

typedef float           Float32;    /* 32 Bit Floating Point Single */
typedef double          Float64;    /* 64 Bit Floating Point Double */

#ifdef HAVE_LONGLONG
typedef longlong        Sint64;
#elif defined(__GNUC__)
/* GNU C defines long long */
typedef long long       Sint64;
#elif defined(_WIN32)
/* Win32 (MSVC) defines __int64 */
typedef __int64         Sint64;
#elif defined(HAVE_INT64_T)
/* many platforms define int64_t in <stdint.h> */
typedef int64_t         Sint64;
#elif SIZEOF_LONG == 8
/* on some platforms, long is 64 bits */
typedef long            Sint64;
#else
/* again on some other platforms (including gcc), long long is defined
 * this is our last resort - if this breaks, we have not found any
 * 64-bit int type and there is not much we can do anyway.
 */
typedef long long       Sint64;
#endif

#ifdef HAVE_ULONGLONG
typedef ulonglong       Uint64;
#endif
#ifdef __GNUC__
/* GNU C defines long long */
typedef unsigned long long Uint64;
#elif defined(_WIN32)
/* Win32 (MSVC) defines __int64 */
typedef unsigned __int64 Uint64;
#elif defined(HAVE_INT64_T)
/* many platforms define uint64_t in <stdint.h> */
typedef uint64_t        Uint64;
#elif SIZEOF_LONG == 8
/* on some platforms, long is 64 bits */
typedef unsigned long   Uint64;
#else
/* again on some other platforms (including gcc), long long is defined
 * this is our last resort - if this breaks, we have not found any
 * 64-bit int type and there is not much we can do anyway.
 */
typedef unsigned long long Uint64;
#endif

// Definition of type OFBool

#ifdef HAVE_CXX_BOOL

#define OFBool bool
#define OFTrue true
#define OFFalse false

#else

/** the boolean type used throughout the DCMTK project. Mapped to the 
 *  built-in type "bool" if the current C++ compiler supports it. Mapped 
 *  to int for old-fashioned compilers which do not yet support bool.
 */
typedef int OFBool;

#ifndef OFTrue 
#define OFTrue (1)
#endif

#ifndef OFFalse 
#define OFFalse (0)
#endif

#endif

#if defined(HAVE_TYPENAME)
#define OFTypename typename
#else
#define OFTypename
#endif

#endif

/*
 * CVS/RCS Log:
 * $Log: oftypes.h,v $
 * Revision 1.10  2010-05-25 10:02:36  uli
 * Added a missing include before the use of int64_t.
 *
 * Revision 1.9  2010-05-07 11:12:29  uli
 * Add new define OFTypename which only expands to "typename"
 * if "HAVE_TYPENAME" is defined.
 *
 * Revision 1.8  2010-03-09 12:14:20  uli
 * Added Sint64 and Uint64 typedefs.
 *
 * Revision 1.7  2005-12-08 16:06:11  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.6  2002/07/10 11:45:26  meichel
 * Moved definitions for Uint8, Sint8 ... Float64 from dcmdata to ofstd
 *   since these types are not DICOM specific
 *
 * Revision 1.5  2001/06/01 15:51:36  meichel
 * Updated copyright header
 *
 * Revision 1.4  2000/10/10 12:01:22  meichel
 * Created/updated doc++ comments
 *
 * Revision 1.3  2000/03/08 16:36:03  meichel
 * Updated copyright header.
 *
 * Revision 1.2  1998/11/27 12:42:53  joergr
 * Added copyright message to source files and changed CVS header.
 *
 * Revision 1.1  1997/07/02 11:51:16  andreas
 * - Preliminary release of the OFFIS Standard Library.
 *   In the future this library shall contain a subset of the
 *   ANSI C++ Library (Version 3) that works on a lot of different
 *   compilers. Additionally this library shall include classes and
 *   functions that are often used. All classes and functions begin
 *   with OF... This library is independent of the DICOM development and
 *   shall contain no DICOM specific stuff.
 *
 */

