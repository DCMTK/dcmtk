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
#include <stdio.h>
#include <string.h>

/*
 * case insensitive comparison between two C strings.
 */
int
_citrus_bcs_strcasecmp(const char * str1,
    const char * str2)
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
_citrus_bcs_strncasecmp(const char * str1,
    const char * str2, size_t sz)
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
_citrus_bcs_skip_ws(const char * p)
{

    while (*p && _citrus_bcs_isspace(*p))
        p++;

    return (p);
}

/*
 * skip non white space characters.
 */
const char *
_citrus_bcs_skip_nonws(const char * p)
{

    while (*p && !_citrus_bcs_isspace(*p))
        p++;

    return (p);
}

/*
 * skip white space characters with limitation of length.
 */
const char *
_citrus_bcs_skip_ws_len(const char * p, size_t * len)
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
_citrus_bcs_skip_nonws_len(const char * p, size_t * len)
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
_citrus_bcs_trunc_rws_len(const char * p, size_t * len)
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

/*
 * compute path to data file
 */
void get_data_path(char *path_out, size_t path_size, const char *dirname, const char *filename)
{
    const char *env;
    const char *separator1;
    const char *separator2;
    size_t len;
    char sep[3];
#ifdef HAVE_WINDOWS_H
    char buf[OFICONV_PATH_MAX+1];
    memset(buf, 0, sizeof(buf));
#endif

    // create a string containing the path separator
    snprintf(sep, sizeof(sep), "%c", PATH_SEPARATOR);

    // retrieve the DCMICONVPATH environment variable
    env = getenv(OFICONV_PATH_VARIABLE);

    // if the environment variable is not set, use DEFAULT_SUPPORT_DATA_DIR instead
    if (env == NULL) env = DEFAULT_SUPPORT_DATA_DIR;

#ifdef HAVE_WINDOWS_H
    (void) ExpandEnvironmentStringsA(env, buf, sizeof(buf));
    env = buf;
#endif

    len = strlen(env);
    if (len == 0) separator1 = ""; // empty path, don't add path separator
    else
    {
        if ((env[len-1] == PATH_SEPARATOR)||(env[len-1] == '/'))
            separator1 = ""; // path already ends with path separator
            else separator1 = sep; // add path separator
    }

    // use non-empty separator 2 only if filename is present and non-empty
    if (filename && (strlen(filename) > 0))
        separator2 = sep;
        else separator2 = "";

    // replace NULL filename with empty filename
    if (filename == NULL) filename = "";

    // finally print full path
    snprintf(path_out, path_size, "%s%s%s%s%s", env, separator1, dirname, separator2, filename);
}
