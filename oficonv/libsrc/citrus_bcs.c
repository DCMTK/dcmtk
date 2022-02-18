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

#include "dcmtk/config/osconfig.h"
#include "citrus_bcs.h"

#include <stdlib.h>

/*
 * case insensitive comparison between two C strings.
 */
int
_citrus_bcs_strcasecmp(const char * __restrict str1,
    const char * __restrict str2)
{
    int c1, c2;

    c1 = c2 = 1;

    while (c1 && c2 && c1 == c2) {
        c1 = _citrus_bcs_toupper(*str1++);
        c2 = _citrus_bcs_toupper(*str2++);
    }

    return ((c1 == c2) ? 0 : ((c1 > c2) ? 1 : -1));
}

/*
 * case insensitive comparison between two C strings with limitation of length.
 */
int
_citrus_bcs_strncasecmp(const char * __restrict str1,
    const char * __restrict str2, size_t sz)
{
    int c1, c2;

    c1 = c2 = 1;

    while (c1 && c2 && c1 == c2 && sz != 0) {
        c1 = _citrus_bcs_toupper(*str1++);
        c2 = _citrus_bcs_toupper(*str2++);
        sz--;
    }

    return ((c1 == c2) ? 0 : ((c1 > c2) ? 1 : -1));
}

/*
 * skip white space characters.
 */
const char *
_citrus_bcs_skip_ws(const char * __restrict p)
{

    while (*p && _citrus_bcs_isspace(*p))
        p++;

    return (p);
}

/*
 * skip non white space characters.
 */
const char *
_citrus_bcs_skip_nonws(const char * __restrict p)
{

    while (*p && !_citrus_bcs_isspace(*p))
        p++;

    return (p);
}

/*
 * skip white space characters with limitation of length.
 */
const char *
_citrus_bcs_skip_ws_len(const char * __restrict p, size_t * __restrict len)
{

    while (*len > 0 && *p && _citrus_bcs_isspace(*p)) {
        p++;
        (*len)--;
    }

    return (p);
}

/*
 * skip non white space characters with limitation of length.
 */
const char *
_citrus_bcs_skip_nonws_len(const char * __restrict p, size_t * __restrict len)
{

    while (*len > 0 && *p && !_citrus_bcs_isspace(*p)) {
        p++;
        (*len)--;
    }

    return (p);
}

/*
 * truncate trailing white space characters.
 */
void
_citrus_bcs_trunc_rws_len(const char * __restrict p, size_t * __restrict len)
{

    while (*len > 0 && _citrus_bcs_isspace(p[*len - 1]))
        (*len)--;
}

/*
 * destructive transliterate to lowercase.
 */
void
_citrus_bcs_convert_to_lower(char *s)
{

    while (*s) {
        *s = _citrus_bcs_tolower(*s);
        s++;
    }
}

/*
 * destructive transliterate to uppercase.
 */
void
_citrus_bcs_convert_to_upper(char *s)
{

    while (*s) {
        *s = _citrus_bcs_toupper(*s);
        s++;
    }
}
