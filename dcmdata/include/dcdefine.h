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
** Update Date:		$Date: 1996-03-22 13:33:09 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcdefine.h,v $
** CVS/RCS Revision:	$Revision: 1.2 $
** Status:		$State: Exp $
**
*/

#ifndef DCDEFINE_H
#define DCDEFINE_H

#if STDC_HEADERS
#  include <string.h>
#  undef memzero
#  define memzero(d, n) memset((d), 0, (n))
#else /*!STDC_HEADERS*/
#  ifndef HAVE_STRCHR
#    undef strchr
#    define strchr index
#    undef strrchr
#    define strrchr rindex
#  endif /*HAVE_STRCHR*/
#  ifndef HAVE_MEMCPY
#    undef memcpy
#    define memcpy(d, s, n) bcopy((s), (d), (n))
#    undef memmove
#    define memmove(d, s, n) bcopy ((s), (d), (n))
#    undef memzero
#    define memzero(d, n) bzero((d), (n))
#    undef memcmp
#    define memcmp(d, s, n) bcmp((s), (d), (n))
#  endif /*HAVE_MEMCPY*/
#endif /*!STDC_HEADERS*/

#endif
