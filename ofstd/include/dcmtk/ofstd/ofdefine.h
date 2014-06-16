/*
 *
 *  Copyright (C) 1994-2014, OFFIS e.V.
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

/* define OFconstexpr to 'constexpr' or '' if not supported */
#ifdef DCMTK_USE_CXX11_STL
#define OFconstexpr constexpr
#else // C++11
#define OFconstexpr
#endif // NOT C++11

#endif
