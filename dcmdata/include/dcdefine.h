/*
**
** Author: Andreas Barth    05.01.96 
** Kuratorium OFFIS e.V.
**
** Module: dcdefine.h
**
** Purpose:
** Implementation of common defines for configuration
**
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1998-02-09 14:04:31 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcdefine.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
*/

#ifndef DCDEFINE_H
#define DCDEFINE_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STRING_H
#include <string.h>
#endif

/* memzero */
#ifdef HAVE_MEMSET
#  undef memzero
#  define memzero(d, n) memset((d), 0, (n))
#else
#  ifdef HAVE_BZERO
#    undef memzero
#    define memzero(d, n) bzero((d), (n))
#  endif
#endif

/* memcpy */
#ifndef HAVE_MEMCPY
#  ifdef HAVE_BCOPY
#    undef memcpy
#    define memcpy(d, s, n) bcopy((s), (d), (n))
#  endif
#endif

/* memmove */
#ifndef HAVE_MEMMOVE
#  ifdef HAVE_BCOPY
#    undef memmove
#    define memmove(d, s, n) bcopy ((s), (d), (n))
#  endif
#endif

/* memcmp */
#ifndef HAVE_MEMCMP
#  ifdef HAVE_BCMP
#    undef memcmp
#    define memcmp(d, s, n) bcmp((s), (d), (n))
#  endif
#endif

/* strchr, strrchr */
#ifndef HAVE_STRCHR
#  ifdef HAVE_INDEX
#    undef strchr
#    define strchr index
#    undef strrchr
#    define strrchr rindex
#  endif
#endif

#endif
