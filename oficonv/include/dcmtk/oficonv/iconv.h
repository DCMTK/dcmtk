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
#include "dcmtk/oficonv/oidefine.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <wchar.h>
#include <stdint.h>


// include this file in doxygen documentation

/** @file iconv.h
 *  @brief Type definitions, constants and functions for the oficonv module
 */


#ifdef __cplusplus
typedef bool __iconv_bool;
#elif __STDC_VERSION__ >= 199901L
typedef _Bool __iconv_bool;
#else
typedef int __iconv_bool;
#endif

struct __tag_iconv_t;
typedef struct __tag_iconv_t *iconv_t;

#ifndef OFICONV_CITRUS_WC_T_DEFINED
#define OFICONV_CITRUS_WC_T_DEFINED
typedef uint32_t _citrus_wc_t;
#endif

BEGIN_EXTERN_C

/** open a converter from the codeset srcname to the codeset dstname and
 *  returns its descriptor. The arguments srcname and dstname accept "" and
 *  "char", which refer to the current locale encoding.
 *  @param dstname name of the destination codeset
 *  @param srcname name of the source codeset
 *  @return a freshly allocated conversion descriptor.
 *    In case of error, errno is set and the function returns (iconv_t)(-1).
 */
DCMTK_OFICONV_EXPORT iconv_t OFiconv_open(const char *dstname, const char *srcname);

/**  convert the string in the buffer *src of length *srcleft bytes and
 *   stores the converted string in the buffer *dst of size *dstleft bytes.
 *   After calling iconv(), the values pointed to by src, srcleft, dst, and
 *   dstleft are updated as follows:
 *
 *   - *src: Pointer to the byte just after the last character fetched.
 *   - *srcleft: Number of remaining bytes in the source buffer.
 *   - *dst: Pointer to the byte just after the last character stored.
 *   - *dstleft: Number of remainder bytes in the destination buffer.
 *
 *   If the string pointed to by *src contains a byte sequence which is not a
 *   valid character in the source codeset, the conversion stops just after
 *   the last successful conversion. If the output buffer is too small to
 *   store the converted character, the conversion also stops in the same
 *   way. In these cases, the values pointed to by src, srcleft, dst, and
 *   dstleft are updated to the state just after the last successful
 *   conversion.
 *
 *   If the string pointed to by *src contains a character which is valid
 *   under the source codeset but can not be converted to the destination
 *   codeset, the character is replaced by an "invalid character" which
 *   depends on the destination codeset, e.g., '?', and the conversion is
 *   continued. iconv() returns the number of such "invalid conversions".
 *
 *   @param cd conversion descriptor returned by OFiconv_open()
 *   @param src address of pointer to first byte in source buffer
 *   @param srcleft address of variable containing the number of bytes left in the source buffer
 *   @param src address of pointer to first byte in destination buffer
 *   @param srcleft address of variable containing the number of bytes left in the destination buffer
 *   @return number of characters converted to "invalid character" upon success,
 *     or (size_t)(-1) in case of error, in which case errno is set
 */
DCMTK_OFICONV_EXPORT size_t OFiconv(iconv_t cd, char ** src, size_t * srcleft, char ** dst, size_t * dstleft);

/** close the specified converter cd
 *  @param cd converter ID returned by OFiconv_open()
 *  @return 0 if successful, -1 otherwise, in which case errno is set
 */
DCMTK_OFICONV_EXPORT int OFiconv_close(iconv_t cd);

/*
 * BSD specific interfaces for iconv
 */

/** obtain a list of character encodings that are supported by the OFiconv()
 *  call. The list of the encoding names will be stored in names and the
 *  number of the entries is stored in count.
 *  @param names address of char pointer array returned in this parameter
 *  @param count number of entries in char pointer array returned in this parameter
 *  @param paired if true, the list will be arranged into canonical/alias name pairs.
 *  @return 0 if successful, -1 otherwise, in which case errno is set
 */
DCMTK_OFICONV_EXPORT int OF__iconv_get_list(char ***names, size_t * count, __iconv_bool paired);

/** free the allocated memory during the call of OF__iconv_get_list().
 *  @param names address of char pointer array created by OF__iconv_get_list()
 *  @param count number of entries in char pointer array
 */
DCMTK_OFICONV_EXPORT void OF__iconv_free_list(char **names, size_t  count);

#define __ICONV_F_HIDE_INVALID  0x0001

/** The OF__iconv() function works just like iconv() but if iconv() fails,
 *  the invalid character count is lost there. This is a not bug, rather a
 *  limitation of IEEE Std 1003.1-2008 (POSIX.1), so OF__iconv() is
 *  provided as an alternative but non-standard interface.
 *   @param cd conversion descriptor returned by OFiconv_open()
 *   @param src address of pointer to first byte in source buffer
 *   @param srcleft address of variable containing the number of bytes left in the source buffer
 *   @param src address of pointer to first byte in destination buffer
 *   @param srcleft address of variable containing the number of bytes left in the destination buffer
 *   @param flags conversion flags. Currently only zero (default) or __ICONV_F_HIDE_INVALID can be passed.
 *     If __ICONV_F_HIDE_INVALID is passed, invalid characters are skipped instead of returning with an error.
 *   @param invalids address of variable in which the number of conversions to "invalid character" is returned upon success or failure
 *   @return number of characters converted to "invalid character" upon success,
 *     or (size_t)(-1) in case of error, in which case errno is set
 */
DCMTK_OFICONV_EXPORT size_t OF__iconv(iconv_t cd, char **src, size_t *srcleft, char **dst, size_t *dstleft, uint32_t flags, size_t *invalids);

/*
 * GNU libiconv interfaces for iconv
 */

/// iconv library version constant, indicating that we have iconvctl()
#define _ICONV_VERSION  0x0108

/// iconv library version constant, value defaults to _ICONV_VERSION
extern int _iconv_version;

#define _libiconv_version       _iconv_version
#define _LIBICONV_VERSION       _ICONV_VERSION

/// space holder type for OFiconv_open_into().
typedef struct {
    void *spaceholder[64];
} iconv_allocation_t;

/** create a conversion descriptor on a preallocated space. The
 *  iconv_allocation_t is used as a spaceholder type when allocating such
 *  space. The dstname and srcname arguments are the same as in the case of
 *  iconv_open(). The ptr argument is a pointer of iconv_allocation_t to the
 *  preallocated space.
 *  @note This function is a GNU libiconv extension, and should be avoided
 *    since in this implementation it may keep shared resources unnecessarily
 *    allocated since there is no way to properly close the conversion descriptor.
 *  @param dstname name of the destination codeset
 *  @param srcname name of the source codeset
 *  @param ptr pointer to block of memory in which the conversion descriptor is allocated
 *    After successful completion of this function, ptr can be safely casted
 *    to iconv_t and used with OFiconv(). The conversion descriptor must be closed
 *    using OFiconv_close_in(), not with OFiconv_close().
 *  @return 0 if successful, -1 otherwise, in which case errno is set
 */
DCMTK_OFICONV_EXPORT int OFiconv_open_into(const char *dstname, const char *srcname, iconv_allocation_t *ptr);

/** close the converter cd allocated in ptr
 *  @param ptr pointer to conversion descriptor created with OFiconv_open_into()
 *  @return 0 if successful, -1 otherwise, in which case errno is set
 */
DCMTK_OFICONV_EXPORT int OFiconv_close_in(iconv_allocation_t *ptr);

/*
 * OFiconvctl() request macros
 */

/** OFiconvctl() request macro.
 *  The argument is an int * variable, which is set to 1 if the
 *  encoding is trivial one, i.e. the input and output encodings are the
 *  same. Otherwise, the variable will be 0.
 */
#define ICONV_TRIVIALP          0

/** OFiconvctl() request macro.
 *  Determines if transliteration is enabled. The answer is stored in
 *  argument, which is of int *. It will be set to 1 if this feature is
 *  enabled or set to 0 otherwise.
 */
#define ICONV_GET_TRANSLITERATE 1

/** OFiconvctl() request macro.
 *  Enables transliteration if argument, which is of int * set to 1 or
 *  disables it if argument is set to 0.
 */
#define ICONV_SET_TRANSLITERATE 2

/** OFiconvctl() request macro.
 *  Determines if illegal sequences are discarded or not. The answer is
 *  stored in argument, which is of int *. It will be set to 1 if this
 *  feature is enabled or set to 0 otherwise.
 */
#define ICONV_GET_DISCARD_ILSEQ 3

/** OFiconvctl() request macro.
 *  Sets whether illegal sequences are discarded or not. argument, which is
 *  of int * set to 1 or disables it if argument is set to 0.
 */
#define ICONV_SET_DISCARD_ILSEQ 4

/** OFiconvctl() request macro.
 *  Sets callback functions, which will be called back after successful
 *  conversions. The callback functions are stored in a struct iconv_hooks
 *  variable, which is passed to iconvctl via argument by its address.
 */
#define ICONV_SET_HOOKS         5

/** OFiconvctl() request macro.
 *  Currently unsupported, will always return an error
 */
#define ICONV_SET_FALLBACKS     6

/** OFiconvctl() request macro.
 *  Determines if a character in the input buffer that is valid, but for
 *  which an identical character does not exist in the target codeset
 *  returns EILSEQ or not. The answer is stored in argument, which is of
 *  int *. It will be set to 1 if this feature is enabled or set to 0 otherwise.
 */
#define ICONV_GET_ILSEQ_INVALID 128

/** OFiconvctl() request macro.
 *  Sets whether a character in the input buffer that is valid, but for
 *  which an identical character does not exist in the target codeset
 *  returns EILSEQ or not. If argument, which is of int * is set to 1 it
 *  will be enabled, and if argument is set to 0 it will be disabled.
 */
#define ICONV_SET_ILSEQ_INVALID 129

/// callback function type for unicode char hook
typedef void (*iconv_unicode_char_hook) (unsigned int mbr, void *data);

/// callback function type for wide char hook
typedef void (*iconv_wide_char_hook) (_citrus_wc_t wc, void *data);

/// callback data struct for OFiconvctl() with ICONV_SET_HOOKS operation
struct iconv_hooks {
    iconv_unicode_char_hook uc_hook;
    iconv_wide_char_hook    wc_hook;
    void                   *data;
};

/** The iconvlist() function obtains a list of character encodings that are
 *  supported by the OFiconv() call. The do_one() callback function will be
 *  called, where the count argument will be set to the number of the
 *  encoding names found, the names argument will be the list of the
 *  supported encoding names and the arg argument will be the arg argument
 *  of the iconvlist() function. This argument can be used to interchange
 *  custom data between the caller of iconvlist() and the callback function.
 *  If an error occurs, names will be NULL when calling do_one().
 *  @param do_one callback function
 *  @param arg opaque pointer that will be passed to the callback function
 */
DCMTK_OFICONV_EXPORT void OFiconvlist(int (*do_one) (unsigned int count, const char * const *names, void *arg), void *arg);

/** resolve the character encoding name specified by the name argument
 *  to its canonical form.
 *  @param name encoding name
 *  @return canonical encoding name, NULL if unknown
 */
DCMTK_OFICONV_EXPORT const char *OFiconv_canonicalize(const char *name);

/** This function can retrieve or set specific conversion setting from the
 *  cd conversion descriptor. The request parameter specifies the operation
 *  to accomplish and argument is an operation-specific argument.
 *  @param cd conversion description
 *  @param request OFiconvctl() request macro defining the operation to be performed
 *  @param argument pointer to data for the the requested operation, see description of request macros
 *  @return 0 if successful, -1 otherwise, in which case errno is set
 */
DCMTK_OFICONV_EXPORT int OFiconvctl(iconv_t cd, int request, void *argument);

/// space holder type for OFlocale_charset().
typedef struct {
    char spaceholder[20];
} iconv_locale_allocation_t;

/** Determine the current locale's character encoding, and canonicalize it.
 *  The result must not be freed; it is statically allocated.
 *  If the canonical name cannot be determined, the result is a non-canonical name.
 *  @param buf buffer to which the current encoding is written on Windows
 *  @return current locale's character encoding (on Windows, this is the current output code page)
 */
DCMTK_OFICONV_EXPORT const char *OFlocale_charset(iconv_locale_allocation_t *buf);

/** logger callback function pointer type
 *  @param level log level, 0=trace, 1=debug, 2=info, 3=warn, 4=error, 5=fatal
 *  @param text1 first part of the string to be logged, must not be NULL
 *  @param text2 second part of the string to be logged, must not be NULL
 *  @param text3 third part of the string to be logged, must not be NULL
 */
typedef void (*oficonv_logger_callback_t)(int level, const char *text1, const char *text2, const char *text3);

/** set the logger callback to be used by the oficonv module.
 *  This function is thread UNSAFE.
 *  @param callback pointer to callback function, NULL for direct logging to stderr
 */
DCMTK_OFICONV_EXPORT void set_oficonv_logger_callback(oficonv_logger_callback_t callback);

/** get the current logger callback used by the oficonv module
 *  @return pointer to logger function, may be NULL
 */
DCMTK_OFICONV_EXPORT oficonv_logger_callback_t get_oficonv_logger_callback();

/** set the log level to be used as long as direct logging to stderr is active
 *  @param level log level, 0=trace, 1=debug, 2=info, 3=warn, 4=error, 5=fatal (default: 3=warn)
 */
DCMTK_OFICONV_EXPORT void set_oficonv_log_level(int level);

END_EXTERN_C

#endif /* !OFICONV_H */
