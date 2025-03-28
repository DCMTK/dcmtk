/*-
 * Copyright (c)2002 Citrus Project,
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

/*-
 * Copyright (c)1999 Citrus Project,
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
#include "citrus_euctw.h"

#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
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
    int  chlen;
    char     ch[4];
} _EUCTWState;

typedef struct {
    int  dummy;
} _EUCTWEncodingInfo;

#define _SS2    0x008e
#define _SS3    0x008f

#define _CEI_TO_EI(_cei_)       (&(_cei_)->ei)
#define _CEI_TO_STATE(_cei_, _func_)    (_cei_)->states.s_##_func_

#define _FUNCNAME(m)            _citrus_EUCTW_##m
#define _ENCODING_INFO          _EUCTWEncodingInfo
#define _ENCODING_STATE         _EUCTWState
#define _ENCODING_MB_CUR_MAX(_ei_)  4
#define _ENCODING_IS_STATE_DEPENDENT    0
#define _STATE_NEEDS_EXPLICIT_INIT(_ps_)    0

static __inline int
_citrus_EUCTW_cs(unsigned int c)
{

    c &= 0xff;

    return ((c & 0x80) ? (c == _SS2 ? 2 : 1) : 0);
}

static __inline int
_citrus_EUCTW_count(int cs)
{

    switch (cs) {
    case 0:
        /*FALLTHROUGH*/
    case 1:
        /*FALLTHROUGH*/
    case 2:
        return (1 << cs);
    case 3:
        abort();
        /*NOTREACHED*/
    }
    return (0);
}

static __inline void
/*ARGSUSED*/
_citrus_EUCTW_init_state(_EUCTWEncodingInfo * ei ,
    _EUCTWState * s)
{
    (void) ei;
    memset(s, 0, sizeof(*s));
}

static int
/*ARGSUSED*/
_citrus_EUCTW_encoding_module_init(_EUCTWEncodingInfo * ei,
    const void * var , size_t lenvar )
{
    (void) var;
    (void) lenvar;
    memset((void *)ei, 0, sizeof(*ei));

    return (0);
}

static void
/*ARGSUSED*/
_citrus_EUCTW_encoding_module_uninit(_EUCTWEncodingInfo *ei )
{
    (void) ei;
}

static int
_citrus_EUCTW_mbrtowc_priv(_EUCTWEncodingInfo * ei,
    _citrus_wc_t * pwc, char ** s,
    size_t n, _EUCTWState * psenc, size_t * nresult)
{
    char *s0;
    _citrus_wc_t wchar;
    int c, chlenbak, cs;

    s0 = *s;

    if (s0 == NULL) {
        _citrus_EUCTW_init_state(ei, psenc);
        *nresult = 0; /* state independent */
        return (0);
    }

    chlenbak = psenc->chlen;

    /* make sure we have the first byte in the buffer */
    switch (psenc->chlen) {
    case 0:
        if (n < 1)
            goto restart;
        psenc->ch[0] = *s0++;
        psenc->chlen = 1;
        n--;
        break;
    case 1:
    case 2:
        break;
    default:
        /* illgeal state */
        goto ilseq;
    }

    c = _citrus_EUCTW_count(cs = _citrus_EUCTW_cs(psenc->ch[0] & 0xff));
    if (c == 0)
        goto ilseq;
    while (psenc->chlen < c) {
        if (n < 1)
            goto ilseq;
        psenc->ch[psenc->chlen] = *s0++;
        psenc->chlen++;
        n--;
    }

    wchar = 0;
    switch (cs) {
    case 0:
        if (psenc->ch[0] & 0x80)
            goto ilseq;
        wchar = psenc->ch[0] & 0xff;
        break;
    case 1:
        if (!(psenc->ch[0] & 0x80) || !(psenc->ch[1] & 0x80))
            goto ilseq;
        wchar = ((psenc->ch[0] & 0xff) << 8) | (psenc->ch[1] & 0xff);
        wchar |= 'G' << 24;
        break;
    case 2:
        if ((unsigned char)psenc->ch[1] < 0xa1 ||
            0xa7 < (unsigned char)psenc->ch[1])
            goto ilseq;
        if (!(psenc->ch[2] & 0x80) || !(psenc->ch[3] & 0x80))
            goto ilseq;
        wchar = ((psenc->ch[2] & 0xff) << 8) | (psenc->ch[3] & 0xff);
        wchar |= ('G' + psenc->ch[1] - 0xa1) << 24;
        break;
    default:
        goto ilseq;
    }

    *s = s0;
    psenc->chlen = 0;

    if (pwc)
        *pwc = wchar;
    *nresult = wchar ? c - chlenbak : 0;
    return (0);

ilseq:
    psenc->chlen = 0;
    *nresult = (size_t)-1;
    return (EILSEQ);

restart:
    *s = s0;
    *nresult = (size_t)-1;
    return (0);
}

static int
_citrus_EUCTW_wcrtomb_priv(_EUCTWEncodingInfo * ei ,
    char * s, size_t n, _citrus_wc_t wc,
    _EUCTWState * psenc , size_t * nresult)
{
    _citrus_wc_t cs, v;
    int clen, i, ret;
    size_t len;
    (void) ei;
    (void) psenc;

    cs = wc & 0x7f000080;
    clen = 1;
    if (wc & 0x00007f00)
        clen = 2;
    if ((wc & 0x007f0000) && !(wc & 0x00800000))
        clen = 3;

    if (clen == 1 && cs == 0x00000000) {
        /* ASCII */
        len = 1;
        if (n < len) {
            ret = E2BIG;
            goto err;
        }
        v = wc & 0x0000007f;
    } else if (clen == 2 && cs == ('G' << 24)) {
        /* CNS-11643-1 */
        len = 2;
        if (n < len) {
            ret = E2BIG;
            goto err;
        }
        v = wc & 0x00007f7f;
        v |= 0x00008080;
    } else if (clen == 2 && 'H' <= (cs >> 24) && (cs >> 24) <= 'M') {
        /* CNS-11643-[2-7] */
        len = 4;
        if (n < len) {
            ret = E2BIG;
            goto err;
        }
        *s++ = _SS2;
        *s++ = (cs >> 24) - 'H' + 0xa2;
        v = wc & 0x00007f7f;
        v |= 0x00008080;
    } else {
        ret = EILSEQ;
        goto err;
    }

    i = clen;
    while (i-- > 0)
        *s++ = (v >> (i << 3)) & 0xff;

    *nresult = len;
    return (0);

err:
    *nresult = (size_t)-1;
    return (ret);
}

static __inline int
/*ARGSUSED*/
_citrus_EUCTW_stdenc_wctocs(_EUCTWEncodingInfo * ei ,
    _citrus_csid_t * csid, _citrus_index_t * idx, _citrus_wc_t wc)
{
    (void) ei;

    *csid = (_citrus_csid_t)(wc >> 24) & 0xFF;
    *idx  = (_citrus_index_t)(wc & 0x7F7F);

    return (0);
}

static __inline int
/*ARGSUSED*/
_citrus_EUCTW_stdenc_cstowc(_EUCTWEncodingInfo * ei ,
    _citrus_wc_t * wc, _citrus_csid_t csid, _citrus_index_t idx)
{
    (void) ei;

    if (csid == 0) {
        if ((idx & ~0x7F) != 0)
            return (EINVAL);
        *wc = (_citrus_wc_t)idx;
    } else {
        if (csid < 'G' || csid > 'M' || (idx & ~0x7F7F) != 0)
            return (EINVAL);
        *wc = (_citrus_wc_t)idx | ((_citrus_wc_t)csid << 24);
    }

    return (0);
}

static __inline int
/*ARGSUSED*/
_citrus_EUCTW_stdenc_get_state_desc_generic(_EUCTWEncodingInfo * ei ,
    _EUCTWState * psenc, int * rstate)
{
    (void) ei;

    *rstate = (psenc->chlen == 0) ? _CITRUS_STDENC_SDGEN_INITIAL :
        _CITRUS_STDENC_SDGEN_INCOMPLETE_CHAR;
    return (0);
}

/* ----------------------------------------------------------------------
 * public interface for stdenc
 */

_CITRUS_STDENC_DECLS(EUCTW);
_CITRUS_STDENC_DEF_OPS(EUCTW);

#include "citrus_stdenc_template.h"
