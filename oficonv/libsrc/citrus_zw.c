/*-
 * Copyright (c)2004, 2006 Citrus Project,
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
#include "citrus_zw.h"

#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "citrus_bcs.h"
#include "citrus_types.h"
#include "citrus_module.h"
#include "citrus_stdenc.h"

/* ----------------------------------------------------------------------
 * private stuffs used by templates
 */

typedef struct {
    int  dummy;
} _ZWEncodingInfo;

typedef enum {
    NONE, AMBIGIOUS, ASCII, GB2312
} _ZWCharset;

typedef struct {
    _ZWCharset   charset;
    int      chlen;
    char         ch[4];
} _ZWState;

#define _CEI_TO_EI(_cei_)       (&(_cei_)->ei)
#define _CEI_TO_STATE(_cei_, _func_)    (_cei_)->states.s_##_func_

#define _FUNCNAME(m)            _citrus_ZW_##m
#define _ENCODING_INFO          _ZWEncodingInfo
#define _ENCODING_STATE         _ZWState
#define _ENCODING_MB_CUR_MAX(_ei_)  MB_LEN_MAX
#define _ENCODING_IS_STATE_DEPENDENT        1
#define _STATE_NEEDS_EXPLICIT_INIT(_ps_)    ((_ps_)->charset != NONE)

static __inline void
/*ARGSUSED*/
_citrus_ZW_init_state(_ZWEncodingInfo * ei ,
    _ZWState * psenc)
{
    (void) ei;
    psenc->chlen = 0;
    psenc->charset = NONE;
}

static int
_citrus_ZW_mbrtowc_priv(_ZWEncodingInfo * ei,
    _citrus_wc_t * pwc, char ** s, size_t n,
    _ZWState * psenc, size_t * nresult)
{
    char *s0;
    _citrus_wc_t  wc;
    int ch, len;

    if (*s == NULL) {
        _citrus_ZW_init_state(ei, psenc);
        *nresult = (size_t)_ENCODING_IS_STATE_DEPENDENT;
        return (0);
    }
    s0 = *s;
    len = 0;

#define STORE               \
do {                    \
    if (n-- < 1) {          \
        *nresult = (size_t)-2;  \
        *s = s0;        \
        return (0);     \
    }               \
    ch = (unsigned char)*s0++;  \
    if (len++ > MB_LEN_MAX || ch > 0x7F)\
        goto ilseq;     \
    psenc->ch[psenc->chlen++] = (char) ch; \
} while (/*CONSTCOND*/0)

loop:
    switch (psenc->charset) {
    case ASCII:
        switch (psenc->chlen) {
        case 0:
            STORE;
            switch (psenc->ch[0]) {
            case '\0': case '\n':
                psenc->charset = NONE;
            }
        /*FALLTHROUGH*/
        case 1:
            break;
        default:
            return (EINVAL);
        }
        ch = (unsigned char)psenc->ch[0];
        if (ch > 0x7F)
            goto ilseq;
        wc = (_citrus_wc_t)ch;
        psenc->chlen = 0;
        break;
    case NONE:
        if (psenc->chlen != 0)
            return (EINVAL);
        STORE;
        ch = (unsigned char)psenc->ch[0];
        if (ch != 'z') {
            if (ch != '\n' && ch != '\0')
                psenc->charset = ASCII;
            wc = (_citrus_wc_t)ch;
            psenc->chlen = 0;
            break;
        }
        psenc->charset = AMBIGIOUS;
        psenc->chlen = 0;
    /* FALLTHROUGH */
    case AMBIGIOUS:
        if (psenc->chlen != 0)
            return (EINVAL);
        STORE;
        if (psenc->ch[0] != 'W') {
            psenc->charset = ASCII;
            wc = L'z';
            break;
        }
        psenc->charset = GB2312;
        psenc->chlen = 0;
    /* FALLTHROUGH */
    case GB2312:
        switch (psenc->chlen) {
        case 0:
            STORE;
            ch = (unsigned char)psenc->ch[0];
            if (ch == '\0') {
                psenc->charset = NONE;
                wc = (_citrus_wc_t)ch;
                psenc->chlen = 0;
                break;
            } else if (ch == '\n') {
                psenc->charset = NONE;
                psenc->chlen = 0;
                goto loop;
            }
        /*FALLTHROUGH*/
        case 1:
            STORE;
            if (psenc->ch[0] == ' ') {
                ch = (unsigned char)psenc->ch[1];
                wc = (_citrus_wc_t)ch;
                psenc->chlen = 0;
                break;
            } else if (psenc->ch[0] == '#') {
                ch = (unsigned char)psenc->ch[1];
                if (ch == '\n') {
                    psenc->charset = NONE;
                    wc = (_citrus_wc_t)ch;
                    psenc->chlen = 0;
                    break;
                } else if (ch == ' ') {
                    wc = (_citrus_wc_t)ch;
                    psenc->chlen = 0;
                    break;
                }
            }
            ch = (unsigned char)psenc->ch[0];
            if (ch < 0x21 || ch > 0x7E)
                goto ilseq;
            wc = (_citrus_wc_t)(ch << 8);
            ch = (unsigned char)psenc->ch[1];
            if (ch < 0x21 || ch > 0x7E) {
ilseq:
                *nresult = (size_t)-1;
                return (EILSEQ);
            }
            wc |= (_citrus_wc_t)ch;
            psenc->chlen = 0;
            break;
        default:
            return (EINVAL);
        }
        break;
    default:
        return (EINVAL);
    }
    if (pwc != NULL)
        *pwc = wc;

    *nresult = (size_t)(wc == 0 ? 0 : len);
    *s = s0;

    return (0);
}

static int
/*ARGSUSED*/
_citrus_ZW_wcrtomb_priv(_ZWEncodingInfo * ei ,
    char * s, size_t n, _citrus_wc_t wc,
    _ZWState * psenc, size_t * nresult)
{
    int ch;
    (void) ei;

    if (psenc->chlen != 0)
        return (EINVAL);
    if ((uint32_t)wc <= 0x7F) {
        ch = (unsigned char)wc;
        switch (psenc->charset) {
        case NONE:
            if (ch == '\0' || ch == '\n')
                psenc->ch[psenc->chlen++] = (char) ch;
            else {
                if (n < 4)
                    return (E2BIG);
                n -= 4;
                psenc->ch[psenc->chlen++] = 'z';
                psenc->ch[psenc->chlen++] = 'W';
                psenc->ch[psenc->chlen++] = ' ';
                psenc->ch[psenc->chlen++] = (char) ch;
                psenc->charset = GB2312;
            }
            break;
        case GB2312:
            if (n < 2)
                return (E2BIG);
            n -= 2;
            if (ch == '\0') {
                psenc->ch[psenc->chlen++] = '\n';
                psenc->ch[psenc->chlen++] = '\0';
                psenc->charset = NONE;
            } else if (ch == '\n') {
                psenc->ch[psenc->chlen++] = '#';
                psenc->ch[psenc->chlen++] = '\n';
                psenc->charset = NONE;
            } else {
                psenc->ch[psenc->chlen++] = ' ';
                psenc->ch[psenc->chlen++] = (char) ch;
            }
            break;
        default:
            return (EINVAL);
        }
    } else if ((uint32_t)wc <= 0x7E7E) {
        switch (psenc->charset) {
        case NONE:
            if (n < 2)
                return (E2BIG);
            n -= 2;
            psenc->ch[psenc->chlen++] = 'z';
            psenc->ch[psenc->chlen++] = 'W';
            psenc->charset = GB2312;
        /* FALLTHROUGH*/
        case GB2312:
            if (n < 2)
                return (E2BIG);
            n -= 2;
            ch = (wc >> 8) & 0xFF;
            if (ch < 0x21 || ch > 0x7E)
                goto ilseq;
            psenc->ch[psenc->chlen++] = (char) ch;
            ch = wc & 0xFF;
            if (ch < 0x21 || ch > 0x7E)
                goto ilseq;
            psenc->ch[psenc->chlen++] = (char) ch;
            break;
        default:
            return (EINVAL);
        }
    } else {
ilseq:
        *nresult = (size_t)-1;
        return (EILSEQ);
    }
    memcpy(s, psenc->ch, psenc->chlen);
    *nresult = psenc->chlen;
    psenc->chlen = 0;

    return (0);
}

static int
/*ARGSUSED*/
_citrus_ZW_put_state_reset(_ZWEncodingInfo * ei ,
    char * s, size_t n, _ZWState * psenc,
    size_t * nresult)
{
    (void) ei;

    if (psenc->chlen != 0)
        return (EINVAL);
    switch (psenc->charset) {
    case GB2312:
        if (n-- < 1)
            return (E2BIG);
        psenc->ch[psenc->chlen++] = '\n';
        psenc->charset = NONE;
    /*FALLTHROUGH*/
    case NONE:
        *nresult = psenc->chlen;
        if (psenc->chlen > 0) {
            memcpy(s, psenc->ch, psenc->chlen);
            psenc->chlen = 0;
        }
        break;
    default:
        return (EINVAL);
    }

    return (0);
}

static __inline int
/*ARGSUSED*/
_citrus_ZW_stdenc_get_state_desc_generic(_ZWEncodingInfo * ei ,
    _ZWState * psenc, int * rstate)
{
    (void) ei;

    switch (psenc->charset) {
    case NONE:
        if (psenc->chlen != 0)
            return (EINVAL);
        *rstate = _CITRUS_STDENC_SDGEN_INITIAL;
        break;
    case AMBIGIOUS:
        if (psenc->chlen != 0)
            return (EINVAL);
        *rstate = _CITRUS_STDENC_SDGEN_INCOMPLETE_SHIFT;
        break;
    case ASCII:
    case GB2312:
        switch (psenc->chlen) {
        case 0:
            *rstate = _CITRUS_STDENC_SDGEN_STABLE;
            break;
        case 1:
            *rstate = (psenc->ch[0] == '#') ?
                _CITRUS_STDENC_SDGEN_INCOMPLETE_SHIFT :
                _CITRUS_STDENC_SDGEN_INCOMPLETE_CHAR;
            break;
        default:
            return (EINVAL);
        }
        break;
    default:
        return (EINVAL);
    }
    return (0);
}

static __inline int
/*ARGSUSED*/
_citrus_ZW_stdenc_wctocs(_ZWEncodingInfo * ei ,
    _citrus_csid_t * csid, _citrus_index_t * idx, _citrus_wc_t wc)
{
    (void) ei;

    *csid = (_citrus_csid_t)(wc <= (_citrus_wc_t)0x7FU) ? 0 : 1;
    *idx = (_citrus_index_t)wc;

    return (0);
}

static __inline int
/*ARGSUSED*/
_citrus_ZW_stdenc_cstowc(_ZWEncodingInfo * ei ,
    _citrus_wc_t * wc, _citrus_csid_t csid, _citrus_index_t idx)
{
    (void) ei;

    switch (csid) {
    case 0: case 1:
        break;
    default:
        return (EINVAL);
    }
    *wc = (_citrus_wc_t)idx;

    return (0);
}

static void
/*ARGSUSED*/
_citrus_ZW_encoding_module_uninit(_ZWEncodingInfo *ei )
{
    (void) ei;
}

static int
/*ARGSUSED*/
_citrus_ZW_encoding_module_init(_ZWEncodingInfo * ei ,
    const void * var , size_t lenvar )
{
    (void) ei;
    (void) var;
    (void) lenvar;

    return (0);
}

/* ----------------------------------------------------------------------
 * public interface for stdenc
 */

_CITRUS_STDENC_DECLS(ZW);
_CITRUS_STDENC_DEF_OPS(ZW);

#include "citrus_stdenc_template.h"
