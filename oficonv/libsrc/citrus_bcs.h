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

#ifndef _CITRUS_BCS_H_
#define _CITRUS_BCS_H_

#include "dcmtk/config/osconfig.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#include <errno.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

#define CITRUS_DECONST(type, var)    ((type)(uintptr_t)(const void *)(var))

#ifdef __GNUC__
#define __packed __attribute__((__packed__))
#else
#define __packed
#endif

#ifndef LINE_MAX
#ifdef _POSIX2_LINE_MAX
#define LINE_MAX _POSIX2_LINE_MAX
#else
#define LINE_MAX 2048
#endif
#endif

#define OFICONV_PATH_MAX 4096

#ifndef EFTYPE
#define EFTYPE EINVAL
#endif

/* The original path to the citrus libiconv files is /usr/share/i18n,
 * we use DCMTK's DEFAULT_SUPPORT_DATA_DIR instead. */

#define OFICONV_PATH_VARIABLE "DCMICONVPATH"
#define OFICONV_CSMAPPER_DIR "csmapper"
#define OFICONV_ESDB_DIR "esdb"

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
    /* Detect with GCC macro. */
#   if defined(__BYTE_ORDER__)
#       if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#           define BYTE_ORDER LITTLE_ENDIAN
#       elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#           define BYTE_ORDER BIG_ENDIAN
#       else
#           error "Unknown machine byteorder endianness detected. User needs to define BYTE_ORDER."
#       endif
     /* Detect with GLIBC's endian.h. */
#   elif defined(__GLIBC__)
#       include <endian.h>
#       if (__BYTE_ORDER == __LITTLE_ENDIAN)
#           define BYTE_ORDER LITTLE_ENDIAN
#       elif (__BYTE_ORDER == __BIG_ENDIAN)
#           define BYTE_ORDER BIG_ENDIAN
#       else
#           error "Unknown machine byteorder endianness detected. User needs to define BYTE_ORDER."
#       endif
     /* Detect with _LITTLE_ENDIAN and _BIG_ENDIAN macro. */
#   elif defined(_LITTLE_ENDIAN) && !defined(_BIG_ENDIAN)
#       define BYTE_ORDER LITTLE_ENDIAN
#   elif defined(_BIG_ENDIAN) && !defined(_LITTLE_ENDIAN)
#       define BYTE_ORDER BIG_ENDIAN
     /* Detect with architecture macros. */
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

#if BYTE_ORDER == LITTLE_ENDIAN
#ifndef be16toh
#  define be16toh(x) ((uint16_t) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))
#endif
#ifndef be32toh
#  define be32toh(x) ((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >> 8) | (((x) & 0x0000ff00u) << 8) | (((x) & 0x000000ffu) << 24))
#endif
#else
#ifndef be16toh
#  define be16toh(x) ((uint32_t)(x))
#endif
#ifndef be32toh
#  define be32toh(x) ((uint16_t)(x))
#endif
#endif

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
_CITRUS_BCS_PRED(isspace, _citrus_bcs_isblank(c) || _citrus_bcs_iseol(c) || c == '\f' || c == '\v')
_CITRUS_BCS_PRED(isdigit, c >= '0' && c <= '9')
_CITRUS_BCS_PRED(isupper, c >= 'A' && c <= 'Z')
_CITRUS_BCS_PRED(islower, c >= 'a' && c <= 'z')
_CITRUS_BCS_PRED(isalpha, _citrus_bcs_isupper(c) || _citrus_bcs_islower(c))
_CITRUS_BCS_PRED(isalnum, _citrus_bcs_isdigit(c) || _citrus_bcs_isalpha(c))
_CITRUS_BCS_PRED(isxdigit, _citrus_bcs_isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))

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

BEGIN_EXTERN_C
int _citrus_bcs_strcasecmp(const char * , const char * );
int _citrus_bcs_strncasecmp(const char * , const char * , size_t);
const char *_citrus_bcs_skip_ws(const char * );
const char *_citrus_bcs_skip_nonws(const char * );
const char *_citrus_bcs_skip_ws_len(const char * , size_t * );
const char *_citrus_bcs_skip_nonws_len(const char * , size_t * );
void _citrus_bcs_trunc_rws_len(const char * , size_t * );
void _citrus_bcs_convert_to_lower(char *);
void _citrus_bcs_convert_to_upper(char *);
long int _citrus_bcs_strtol(const char * , char ** , int);
unsigned long _citrus_bcs_strtoul(const char * , char ** , int);

/** return path to data file or data directory. This function checks the value
 *  of the environment variable DCMICONVPATH and uses it as the base path, if present,
 *  and the hard-coded DEFAULT_SUPPORT_DATA_DIR otherwise.
 *  @param path_out full path to data file returned in this buffer
 *  @param size of buffer, should be MAX_PATH or larger
 *  @param dirname name of the data directory, e.g. "esdb" or "csmapper"
 *  @param filename name of the data file, may be NULL, in which case the name of the data directory is returned
 */
void get_data_path(char *path_out, size_t path_size, const char *dirname, const char *filename);
END_EXTERN_C

#endif
