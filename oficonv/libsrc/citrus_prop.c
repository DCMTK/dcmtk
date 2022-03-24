/*-
 * Copyright (c)2006 Citrus Project,
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

#include "dcmtk/config/osconfig.h"
#include "citrus_prop.h"

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "citrus_bcs.h"
#include "citrus_region.h"
#include "citrus_memstream.h"

typedef struct {
    _citrus_prop_type_t type;
    union {
        const char *str;
        int chr;
        bool boolean;
        uint64_t num;
    } u;
} _citrus_prop_object_t;

static __inline void
_citrus_prop_object_init(_citrus_prop_object_t *obj, _citrus_prop_type_t type)
{

    obj->type = type;
    memset(&obj->u, 0, sizeof(obj->u));
}

static __inline void
_citrus_prop_object_uninit(_citrus_prop_object_t *obj)
{

    if (obj->type == _CITRUS_PROP_STR)
        free(CITRUS_DECONST(void *, obj->u.str));
}

static const char *xdigit = "0123456789ABCDEF";

#define _CITRUS_PROP_READ_UINT_COMMON(_func_, _type_, _max_)        \
static int                              \
_citrus_prop_read_##_func_##_common(struct _citrus_memory_stream * ms,  \
    _type_ * result, int base, int neg)               \
{                                   \
    _type_ acc, cutoff;                     \
    int ch, cutlim, n;                      \
    char *p;                            \
                                    \
    acc = (_type_)0;                        \
    cutoff = _max_ / base;                      \
    cutlim = _max_ % base;                      \
    for (;;) {                          \
        ch = _citrus_memory_stream_getc(ms);               \
        p = strchr(xdigit, _citrus_bcs_toupper((uint8_t)ch));           \
        if (p == NULL || (n = (int)(p - xdigit)) >= base)        \
            break;                      \
        if (acc > cutoff || (acc == cutoff && n > cutlim))  \
            break;                      \
        acc *= base;                        \
        acc += n;                       \
    }                               \
    _citrus_memory_stream_ungetc(ms, ch);                  \
    *result = neg ? -acc : acc;                          \
    return (0);                         \
}
_CITRUS_PROP_READ_UINT_COMMON(chr, int, UCHAR_MAX)

/* suppress MSVC 'unary minus operator applied to unsigned type' warning  */
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4146)
#endif

_CITRUS_PROP_READ_UINT_COMMON(num, uint64_t, UINT64_MAX)

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#undef _CITRUS_PROP_READ_UINT_COMMON

#define _CITRUS_PROP_READ_INT(_func_, _type_)           \
static int                          \
_citrus_prop_read_##_func_(struct _citrus_memory_stream * ms,   \
    _citrus_prop_object_t * obj)         \
{                               \
    int base, ch, neg;                   \
                                \
    _citrus_memory_stream_skip_ws(ms);                 \
    ch = _citrus_memory_stream_getc(ms);               \
    neg = 0;                      \
    switch (ch) {                       \
    case '-':                       \
        neg = 1;                  \
    case '+':                       \
        ch = _citrus_memory_stream_getc(ms);           \
    }                           \
    base = 10;                      \
    if (ch == '0') {                    \
        base -= 2;                  \
        ch = _citrus_memory_stream_getc(ms);           \
        if (ch == 'x' || ch == 'X') {           \
            ch = _citrus_memory_stream_getc(ms);       \
            if (_citrus_bcs_isxdigit((uint8_t)ch) == 0) {       \
                _citrus_memory_stream_ungetc(ms, ch);  \
                obj->u._func_ = 0;      \
                return (0);         \
            }                   \
            base += 8;              \
        }                       \
    } else if (_citrus_bcs_isdigit((uint8_t)ch) == 0)           \
        return (EINVAL);                \
    _citrus_memory_stream_ungetc(ms, ch);              \
    return (_citrus_prop_read_##_func_##_common     \
        (ms, &obj->u._func_, base, neg));            \
}
_CITRUS_PROP_READ_INT(chr, int)
_CITRUS_PROP_READ_INT(num, uint64_t)
#undef _CITRUS_PROP_READ_INT

static int
_citrus_prop_read_character_common(struct _citrus_memory_stream * ms,
    int * result)
{
    int base, ch;

    ch = _citrus_memory_stream_getc(ms);
    if (ch != '\\')
        *result = ch;
    else {
        ch = _citrus_memory_stream_getc(ms);
        base = 16;
        switch (ch) {
        case 'a':
            *result = '\a';
            break;
        case 'b':
            *result = '\b';
            break;
        case 'f':
            *result = '\f';
            break;
        case 'n':
            *result = '\n';
            break;
        case 'r':
            *result = '\r';
            break;
        case 't':
            *result = '\t';
            break;
        case 'v':
            *result = '\v';
            break;
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
            _citrus_memory_stream_ungetc(ms, ch);
            base -= 8;
            /*FALLTHROUGH*/
        case 'x':
            return (_citrus_prop_read_chr_common(ms, result, base, 0));
            /*NOTREACHED*/
        default:
            /* unknown escape */
            *result = ch;
        }
    }
    return (0);
}

static int
_citrus_prop_read_character(struct _citrus_memory_stream * ms,
    _citrus_prop_object_t * obj)
{
    int ch, errnum;

    _citrus_memory_stream_skip_ws(ms);
    ch = _citrus_memory_stream_getc(ms);
    if (ch != '\'') {
        _citrus_memory_stream_ungetc(ms, ch);
        return (_citrus_prop_read_chr(ms, obj));
    }
    errnum = _citrus_prop_read_character_common(ms, &ch);
    if (errnum != 0)
        return (errnum);
    obj->u.chr = ch;
    ch = _citrus_memory_stream_getc(ms);
    if (ch != '\'')
        return (EINVAL);
    return (0);
}

static int
_citrus_prop_read_bool(struct _citrus_memory_stream * ms,
    _citrus_prop_object_t * obj)
{

    _citrus_memory_stream_skip_ws(ms);
    switch (_citrus_bcs_tolower((uint8_t) _citrus_memory_stream_getc(ms))) {
    case 't':
        if (_citrus_bcs_tolower((uint8_t) _citrus_memory_stream_getc(ms)) == 'r' &&
            _citrus_bcs_tolower((uint8_t) _citrus_memory_stream_getc(ms)) == 'u' &&
            _citrus_bcs_tolower((uint8_t) _citrus_memory_stream_getc(ms)) == 'e') {
            obj->u.boolean = true;
            return (0);
        }
        break;
    case 'f':
        if (_citrus_bcs_tolower((uint8_t) _citrus_memory_stream_getc(ms)) == 'a' &&
            _citrus_bcs_tolower((uint8_t) _citrus_memory_stream_getc(ms)) == 'l' &&
            _citrus_bcs_tolower((uint8_t) _citrus_memory_stream_getc(ms)) == 's' &&
            _citrus_bcs_tolower((uint8_t) _citrus_memory_stream_getc(ms)) == 'e') {
            obj->u.boolean = false;
            return (0);
        }
    }
    return (EINVAL);
}

static int
_citrus_prop_read_str(struct _citrus_memory_stream * ms,
    _citrus_prop_object_t * obj)
{
    int ch, errnum, quot;
    char *s, *t;
#define _CITRUS_PROP_STR_BUFSIZ 512
    size_t m, n;

    m = _CITRUS_PROP_STR_BUFSIZ;
    s = malloc(m);
    if (s == NULL)
        return (ENOMEM);
    n = 0;
    _citrus_memory_stream_skip_ws(ms);
    quot = _citrus_memory_stream_getc(ms);
    switch (quot) {
    case EOF:
        goto done;
        /*NOTREACHED*/
    case '\\':
        _citrus_memory_stream_ungetc(ms, quot);
        quot = EOF;
        /*FALLTHROUGH*/
    case '\"': case '\'':
        break;
    default:
        s[n] = (char)quot;
        ++n, --m;
        quot = EOF;
    }
    for (;;) {
        if (m < 1) {
            m = _CITRUS_PROP_STR_BUFSIZ;
            t = realloc(s, n + m);
            if (t == NULL) {
                free(s);
                return (ENOMEM);
            }
            s = t;
        }
        ch = _citrus_memory_stream_getc(ms);
        if (quot == ch || (quot == EOF &&
            (ch == ';' || _citrus_bcs_isspace((uint8_t)ch)))) {
done:
            s[n] = '\0';
            obj->u.str = (const char *)s;
            return (0);
        }
        _citrus_memory_stream_ungetc(ms, ch);
        errnum = _citrus_prop_read_character_common(ms, &ch);

        if (errnum != 0) {
            free(s);
            return (errnum);
        }
        s[n] = (char)ch;
        ++n, --m;
    }
    /* this code can never be reached */
    // free(s);
    //return (EINVAL);
#undef _CITRUS_PROP_STR_BUFSIZ
}

typedef int (*_citrus_prop_read_type_t)(struct _citrus_memory_stream * ,
    _citrus_prop_object_t * );

static const _citrus_prop_read_type_t readers[] = {
    _citrus_prop_read_bool,
    _citrus_prop_read_str,
    _citrus_prop_read_character,
    _citrus_prop_read_num,
};

static __inline int
_citrus_prop_read_symbol(struct _citrus_memory_stream * ms,
    char * s, size_t n)
{
    int ch;
    size_t m;

    for (m = 0; m < n; ++m) {
        ch = _citrus_memory_stream_getc(ms);
        if (ch != '_' && _citrus_bcs_isalnum((uint8_t)ch) == 0)
            goto name_found;
        s[m] = (char)ch;
    }
    ch = _citrus_memory_stream_getc(ms);
    if (ch == '_' || _citrus_bcs_isalnum((uint8_t)ch) != 0)
        return (EINVAL);

name_found:
    _citrus_memory_stream_ungetc(ms, ch);
    s[m] = '\0';

    return (0);
}

static int
_citrus_prop_parse_element(struct _citrus_memory_stream * ms,
    const _citrus_prop_hint_t * hints, void * context)
{
    int ch, errnum;
#define _CITRUS_PROP_HINT_NAME_LEN_MAX  255
    char name[_CITRUS_PROP_HINT_NAME_LEN_MAX + 1];
    const _citrus_prop_hint_t *hint;
    _citrus_prop_object_t ostart, oend;

    errnum = _citrus_prop_read_symbol(ms, name, sizeof(name));
    if (errnum != 0)
        return (errnum);
    for (hint = hints; hint->name != NULL; ++hint)
        if (_citrus_bcs_strcasecmp(name, hint->name) == 0)
            goto hint_found;
    return (EINVAL);

hint_found:
    _citrus_memory_stream_skip_ws(ms);
    ch = _citrus_memory_stream_getc(ms);
    if (ch != '=' && ch != ':')
        _citrus_memory_stream_ungetc(ms, ch);
    do {
        _citrus_prop_object_init(&ostart, hint->type);
        _citrus_prop_object_init(&oend, hint->type);
        errnum = (*readers[hint->type])(ms, &ostart);
        if (errnum != 0)
            return (errnum);
        _citrus_memory_stream_skip_ws(ms);
        ch = _citrus_memory_stream_getc(ms);
        switch (hint->type) {
        case _CITRUS_PROP_BOOL:
            /*FALLTHROUGH*/
        case _CITRUS_PROP_STR:
            break;
        default:
            if (ch != '-')
                break;
            errnum = (*readers[hint->type])(ms, &oend);
            if (errnum != 0)
                return (errnum);
            _citrus_memory_stream_skip_ws(ms);
            ch = _citrus_memory_stream_getc(ms);
        }
#define CALL0(_func_)                   \
do {                            \
    errnum = (*hint->cb._func_.func)(context,   \
        hint->name, ostart.u._func_);       \
} while (0)
#define CALL1(_func_)                   \
do {                            \
    errnum = (*hint->cb._func_.func)(context,   \
        hint->name, ostart.u._func_, oend.u._func_);\
} while (0)
        switch (hint->type) {
        case _CITRUS_PROP_BOOL:
            CALL0(boolean);
            break;
        case _CITRUS_PROP_STR:
            CALL0(str);
            break;
        case _CITRUS_PROP_CHR:
            CALL1(chr);
            break;
        case _CITRUS_PROP_NUM:
            CALL1(num);
            break;
        default:
            abort();
            /*NOTREACHED*/
        }
#undef CALL0
#undef CALL1
        _citrus_prop_object_uninit(&ostart);
        _citrus_prop_object_uninit(&oend);
        if (errnum != 0)
            return (errnum);
    } while (ch == ',');
    if (ch != ';')
        _citrus_memory_stream_ungetc(ms, ch);
    return (0);
}

int
_citrus_prop_parse_variable(const _citrus_prop_hint_t * hints,
    void * context, const void *var, size_t lenvar)
{
    struct _citrus_memory_stream ms;
    int ch, errnum;

    _citrus_memory_stream_bind_ptr(&ms, CITRUS_DECONST(void *, var), lenvar);
    for (;;) {
        _citrus_memory_stream_skip_ws(&ms);
        ch = _citrus_memory_stream_getc(&ms);
        if (ch == EOF || ch == '\0')
            break;
        _citrus_memory_stream_ungetc(&ms, ch);
        errnum = _citrus_prop_parse_element(&ms, hints, context);
        if (errnum != 0)
            return (errnum);
    }
    return (0);
}
