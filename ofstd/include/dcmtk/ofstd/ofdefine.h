/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Andreas Barth
 *
 *  Purpose: common defines for configuration
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2012-02-01 09:49:18 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef OFDEFINE_H
#define OFDEFINE_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofexport.h"

#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


#ifdef ofstd_EXPORTS
#define DCMTK_OFSTD_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_OFSTD_EXPORT DCMTK_DECL_IMPORT
#endif


#ifdef HAVE_BZERO
#ifndef HAVE_PROTOTYPE_BZERO
BEGIN_EXTERN_C
extern void bzero(char* s, int len);
END_EXTERN_C
#endif
#endif


/* memzero */
#ifdef HAVE_MEMSET
#  undef memzero
#  define memzero(d, n) memset((d), 0, (n))
#  define HAVE_MEMZERO /* This makes using this easier */
#else
#  ifdef HAVE_BZERO
#    undef memzero
// some platforms, e.g. OSF1, require the first parameter to be char *.
#    define memzero(d, n) bzero(OFstatic_cast(char *, d), (n))
#    define HAVE_MEMZERO /* This makes using this easier */
#  endif
#endif

/* memcpy */
#ifndef HAVE_MEMCPY
#  ifdef HAVE_BCOPY
#    undef memcpy
#    define memcpy(d, s, n) bcopy((s), (d), (n))
#    define HAVE_MEMCPY /* This makes using this easier */
#  endif
#endif

/* memmove */
#ifndef HAVE_MEMMOVE
#  ifdef HAVE_BCOPY
#    undef memmove
#    define memmove(d, s, n) bcopy ((s), (d), (n))
#    define HAVE_MEMMOVE /* This makes using this easier */
#  endif
#endif

/* memcmp */
#ifndef HAVE_MEMCMP
#  ifdef HAVE_BCMP
#    undef memcmp
#    define memcmp(d, s, n) bcmp((s), (d), (n))
#    define HAVE_MEMCMP /* This makes using this easier */
#  endif
#endif

/* strchr, strrchr */
#ifndef HAVE_STRCHR
#  ifdef HAVE_INDEX
#    undef strchr
#    define strchr index
#    undef strrchr
#    define strrchr rindex
#    define HAVE_STRCHR  /* This makes using this easier */
#    define HAVE_STRRCHR /* This makes using this easier */
#  endif
#endif

#endif

/*
 * CVS/RCS Log:
 * $Log: ofdefine.h,v $
 * Revision 1.5  2012-02-01 09:49:18  uli
 * Moved some definitions to a file that can be included from C.
 *
 * Revision 1.4  2012-01-18 09:33:57  uli
 * Added support for building with hidden visibility.
 *
 * Revision 1.3  2011-12-14 08:54:00  uli
 * Make it possible to correctly build ofstd as a DLL.
 *
 * Revision 1.2  2010-10-14 13:15:50  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.1  2009-09-28 12:19:02  joergr
 * Moved general purpose definition file from module dcmdata to ofstd, and
 * added new defines in order to make the usage easier.
 *
 * Revision 1.8  2005/12/08 16:28:05  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.7  2002/11/27 12:07:21  meichel
 * Adapted module dcmdata to use of new header file ofstdinc.h
 *
 * Revision 1.6  2001/06/01 15:48:35  meichel
 * Updated copyright header
 *
 * Revision 1.5  2000/03/08 16:26:12  meichel
 * Updated copyright header.
 *
 * Revision 1.4  1999/03/31 09:24:33  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
