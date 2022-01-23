/*-
 * Copyright (c) 2003 Citrus Project,
 * Copyright (c) 2009, 2010 Gabor Kovesdan <gabor@FreeBSD.org>
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
 *
 */

#ifndef OFICONV_H
#define OFICONV_H

#include "dcmtk/config/osconfig.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <wchar.h>

#ifdef __cplusplus
typedef bool __iconv_bool;
#elif __STDC_VERSION__ >= 199901L
typedef _Bool __iconv_bool;
#else
typedef int __iconv_bool;
#endif

struct __tag_iconv_t;
typedef struct __tag_iconv_t *iconv_t;

BEGIN_EXTERN_C

/**
 * open a converter from the codeset srcname to the codeset dstname and
 * returns its descriptor. The arguments srcname and dstname accept "" and
 * "char", which refer to the current locale encoding.
 */
iconv_t OFiconv_open(const char *, const char *);

/**
 *  convert the string in the buffer *src of length *srcleft bytes and
 *  stores the converted string in the buffer *dst of size *dstleft bytes.
 *  After calling iconv(), the values pointed to by src, srcleft, dst, and
 *  dstleft are updated as follows:
 *
 *  *src: Pointer to the byte just after the last character fetched.
 *  *srcleft: Number of remaining bytes in the source buffer.
 *  *dst: Pointer to the byte just after the last character stored.
 *  *dstleft: Number of remainder bytes in the destination buffer.
 *
 *  If the string pointed to by *src contains a byte sequence which is not a
 *  valid character in the source codeset, the conversion stops just after
 *  the last successful conversion. If the output buffer is too small to
 *  store the converted character, the conversion also stops in the same
 *  way. In these cases, the values pointed to by src, srcleft, dst, and
 *  dstleft are updated to the state just after the last successful
 *  conversion.
 *
 *  If the string pointed to by *src contains a character which is valid
 *  under the source codeset but can not be converted to the destination
 *  codeset, the character is replaced by an “invalid character” which
 *  depends on the destination codeset, e.g., ‘?’, and the conversion is
 *  continued. iconv() returns the number of such “invalid conversions”.
 */
size_t OFiconv(iconv_t cd, char ** __restrict src, size_t * __restrict srcleft, char ** __restrict dst, size_t * __restrict dstleft);

/**
 * close the specified converter cd
 */
int OFiconv_close(iconv_t cd);

/*
 * non-portable interfaces for iconv
 */

/**
 * obtain a list of character encodings that are supported by the OFiconv()
 * call. The list of the encoding names will be stored in names and the
 * number of the entries is stored in count. If the paired variable is
 * true, the list will be arranged into canonical/alias name pairs.
 */
int OF__iconv_get_list(char ***, size_t *, __iconv_bool);

/**
 * free the allocated memory during the call of OF__iconv_get_list().
 */
void OF__iconv_free_list(char **, size_t);

/**
 * The OF__iconv() function works just like iconv() but if iconv() fails,
 * the invalid character count is lost there. This is a not bug, rather a
 * limitation of IEEE Std 1003.1-2008 (POSIX.1), so __iconv() is
 * provided as an alternative but non-standard interface. It also has a
 * flags argument, where currently the following flags can be passed:
 *
 * __ICONV_F_HIDE_INVALID
 *     Skip invalid characters, instead of returning with an error.
 */
size_t  OF__iconv(iconv_t, char **, size_t *, char **, size_t *, __uint32_t, size_t *);

#define __ICONV_F_HIDE_INVALID  0x0001

/*
 * GNU interfaces for iconv
 */

/* We have iconvctl() */
#define _ICONV_VERSION  0x0108
extern int _iconv_version;

#define _libiconv_version       _iconv_version
#define _LIBICONV_VERSION       _ICONV_VERSION

typedef struct {
    void *spaceholder[64];
} iconv_allocation_t;


/**
 * create a conversion descriptor on a preallocated space. The
 * iconv_allocation_t is used as a spaceholder type when allocating such
 * space. The dstname and srcname arguments are the same as in the case of
 * iconv_open(). The ptr argument is a pointer of iconv_allocation_t to the
 * preallocated space.
 */
int OFiconv_open_into(const char *, const char *, iconv_allocation_t *);

/*
 * OFiconvctl() request macros
 */

/*
 * the argument is an int * variable, which is set to 1 if the
 * encoding is trivial one, i.e. the input and output encodings are the
 * same. Otherwise, the variable will be 0.
 */
#define ICONV_TRIVIALP          0

/*
 * Determines if transliteration is enabled. The answer is stored in
 * argument, which is of int *. It will be set to 1 if this feature is
 * enabled or set to 0 otherwise.
 */
#define ICONV_GET_TRANSLITERATE 1

/*
 * Enables transliteration if argument, which is of int * set to 1 or
 * disables it if argument is set to 0.
 */
#define ICONV_SET_TRANSLITERATE 2

/*
 * Determines if illegal sequences are discarded or not. The answer is
 * stored in argument, which is of int *. It will be set to 1 if this
 * feature is enabled or set to 0 otherwise.
 */
#define ICONV_GET_DISCARD_ILSEQ 3

/*
 * Sets whether illegal sequences are discarded or not. argument, which is
 * of int * set to 1 or disables it if argument is set to 0.
 */
#define ICONV_SET_DISCARD_ILSEQ 4

/*
 * Sets callback functions, which will be called back after successful
 * conversions. The callback functions are stored in a struct iconv_hooks
 * variable, which is passed to iconvctl via argument by its address.
 */
#define ICONV_SET_HOOKS         5

/*
 * Currently unsupported, will always return an error
 */
#define ICONV_SET_FALLBACKS     6

/*
 * Determines if a character in the input buffer that is valid, but for
 * which an identical character does not exist in the target codeset
 * returns EILSEQ or not. The answer is stored in argument, which is of
 * int *. It will be set to 1 if this feature is enabled or set to 0 otherwise.
 */
#define ICONV_GET_ILSEQ_INVALID 128

/*
 * Sets whether a character in the input buffer that is valid, but for
 * which an identical character does not exist in the target codeset
 * returns EILSEQ or not. If argument, which is of int * is set to 1 it
 * will be enabled, and if argument is set to 0 it will be disabled.
 */

#define ICONV_SET_ILSEQ_INVALID 129

typedef void (*iconv_unicode_char_hook) (unsigned int mbr, void *data);
typedef void (*iconv_wide_char_hook) (wchar_t wc, void *data);

struct iconv_hooks {
    iconv_unicode_char_hook uc_hook;
    iconv_wide_char_hook    wc_hook;
    void                   *data;
};

/**
 * The iconvlist() function obtains a list of character encodings that are
 * supported by the OFiconv() call. The do_one() callback function will be
 * called, where the count argument will be set to the number of the
 * encoding names found, the names argument will be the list of the
 * supported encoding names and the arg argument will be the arg argument
 * of the iconvlist() function. This argument can be used to interchange
 * custom data between the caller of iconvlist() and the callback function.
 * If an error occurs, names will be NULL when calling do_one().
 */
void OFiconvlist(int (*do_one) (unsigned int, const char * const *, void *), void *);

/**
 * resolve the character encoding name specified by the name argument
 * to its canonical form.
 */
const char  *OFiconv_canonicalize(const char *);

/**
 *  This function can retrieve or set specific conversion setting from the
 *  cd conversion descriptor. The request parameter specifies the operation
 *  to accomplish and argument is an operation-specific argument.
 */
int OFiconvctl(iconv_t cd, int request, void *argument);

/**
 *  Determine the current locale's character encoding, and canonicalize it.
 *  The result must not be freed; it is statically allocated.
 *  If the canonical name cannot be determined, the result is a non-canonical name.
 */
const char *OFlocale_charset();

END_EXTERN_C

#endif /* !OFICONV_H */
