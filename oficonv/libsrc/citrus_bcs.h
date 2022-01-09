/* $FreeBSD$ */
/* $NetBSD: citrus_bcs.h,v 1.6 2009/01/11 02:46:24 christos Exp $ */

/*-
 * Copyright (c)2003 Citrus Project,
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/types.h>

/* BEGIN DCMTK modifications */
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#define __INT_MAX INT_MAX
#define __INT_MIN INT_MIN
#define __DECONST(type, var)    ((type)(uintptr_t)(const void *)(var))
#define __unused        __attribute__((__unused__))
#define __packed        __attribute__((__packed__))
#define __restrict      
#ifndef EFTYPE
#define EFTYPE EINVAL
#endif
#define _PATH_CSMAPPER  "/usr/share/i18n/csmapper"
#define _PATH_ESDB      "/usr/share/i18n/esdb"
#define _PATH_I18NMODULE "/usr/lib/i18n"
#define __isthreaded 1
#ifndef CHAR_MAX
#error Need to define CHAR_MAX
#endif
#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321    /* MSB first: 68000, ibm, net */
#endif
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN  1234    /* LSB first: i386, vax */
#endif

#ifndef BYTE_ORDER
    // Detect with GCC macro.
#   if defined(__BYTE_ORDER__)
#       if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#           define BYTE_ORDER LITTLE_ENDIAN
#       elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#           define BYTE_ORDER BIG_ENDIAN
#       else
#           error "Unknown machine byteorder endianness detected. User needs to define BYTE_ORDER."
#       endif
    // Detect with GLIBC's endian.h.
#   elif defined(__GLIBC__)
#       include <endian.h>
#       if (__BYTE_ORDER == __LITTLE_ENDIAN)
#           define BYTE_ORDER LITTLE_ENDIAN
#       elif (__BYTE_ORDER == __BIG_ENDIAN)
#           define BYTE_ORDER BIG_ENDIAN
#       else
#           error "Unknown machine byteorder endianness detected. User needs to define BYTE_ORDER."
#       endif
    // Detect with _LITTLE_ENDIAN and _BIG_ENDIAN macro.
#   elif defined(_LITTLE_ENDIAN) && !defined(_BIG_ENDIAN)
#       define BYTE_ORDER LITTLE_ENDIAN
#   elif defined(_BIG_ENDIAN) && !defined(_LITTLE_ENDIAN)
#       define BYTE_ORDER BIG_ENDIAN
    // Detect with architecture macros.
#   elif defined(__sparc) || defined(__sparc__) || defined(_POWER) || defined(__powerpc__) || defined(__ppc__) || defined(__hpux) || defined(__hppa) || defined(_MIPSEB) || defined(_POWER) || defined(__s390__)
#       define BYTE_ORDER BIG_ENDIAN
#   elif defined(__i386__) || defined(__alpha__) || defined(__ia64) || defined(__ia64__) || defined(_M_IX86) || defined(_M_IA64) || defined(_M_ALPHA) || defined(__amd64) || defined(__amd64__) || defined(_M_AMD64) || defined(__x86_64) || defined(__x86_64__) || defined(_M_X64) || defined(__bfin__)
#       define BYTE_ORDER LITTLE_ENDIAN
#   elif defined(_MSC_VER) && (defined(_M_ARM) || defined(_M_ARM64))
#       define BYTE_ORDER LITTLE_ENDIAN
#   else
#       error "Unknown machine byteorder endianness detected. User needs to define BYTE_ORDER."
#   endif
#endif

/* END DCMTK modifications */

#ifndef _CITRUS_BCS_H_
#define _CITRUS_BCS_H_

/*
 * predicate/conversion for basic character set.
 *
 * `Basic character set' is a term defined in the ISO C standard.
 * Citrus bcs is, if anything, close to `portable character set'
 * defined in the POSIX.
 */

#define _CITRUS_BCS_PRED(_name_, _cond_) \
static __inline int _citrus_bcs_##_name_(uint8_t c) { return (_cond_); }

/*
 * predicates.
 * Unlike predicates defined in ctype.h, these do not accept EOF.
 */
_CITRUS_BCS_PRED(isblank, c == ' ' || c == '\t')
_CITRUS_BCS_PRED(iseol, c == '\n' || c == '\r')
_CITRUS_BCS_PRED(isspace, _citrus_bcs_isblank(c) || _citrus_bcs_iseol(c) ||
    c == '\f' || c == '\v')
_CITRUS_BCS_PRED(isdigit, c >= '0' && c <= '9')
_CITRUS_BCS_PRED(isupper, c >= 'A' && c <= 'Z')
_CITRUS_BCS_PRED(islower, c >= 'a' && c <= 'z')
_CITRUS_BCS_PRED(isalpha, _citrus_bcs_isupper(c) || _citrus_bcs_islower(c))
_CITRUS_BCS_PRED(isalnum, _citrus_bcs_isdigit(c) || _citrus_bcs_isalpha(c))
_CITRUS_BCS_PRED(isxdigit, _citrus_bcs_isdigit(c) ||
    (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))

/*
 * transliterate between uppercase and lowercase.
 * Unlike transliterator defined in ctype.h, these do not accept EOF.
 */
static __inline uint8_t
_citrus_bcs_toupper(uint8_t c)
{

	return (_citrus_bcs_islower(c) ? (c - 'a' + 'A') : c);
}

static __inline uint8_t
_citrus_bcs_tolower(uint8_t c)
{

	return (_citrus_bcs_isupper(c) ? (c - 'A' + 'a') : c);
}

__BEGIN_DECLS
int		 _citrus_bcs_strcasecmp(const char * __restrict,
		    const char * __restrict);
int		 _citrus_bcs_strncasecmp(const char * __restrict,
		    const char * __restrict, size_t);
const char	*_citrus_bcs_skip_ws(const char * __restrict);
const char	*_citrus_bcs_skip_nonws(const char * __restrict);
const char	*_citrus_bcs_skip_ws_len(const char * __restrict,
		    size_t * __restrict);
const char	*_citrus_bcs_skip_nonws_len(const char * __restrict,
		    size_t * __restrict);
void		 _citrus_bcs_trunc_rws_len(const char * __restrict,
		    size_t * __restrict);
void		 _citrus_bcs_convert_to_lower(char *);
void		 _citrus_bcs_convert_to_upper(char *);

long int	 _citrus_bcs_strtol(const char * __restrict,
		    char ** __restrict, int);
unsigned long	 _citrus_bcs_strtoul(const char * __restrict,
		    char ** __restrict, int);
__END_DECLS

#endif
