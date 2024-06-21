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
 *
 */

#ifndef _CITRUS_STDENC_H_
#define _CITRUS_STDENC_H_

#include "dcmtk/config/osconfig.h"

struct _citrus_stdenc;
struct _citrus_stdenc_ops;
struct _citrus_stdenc_traits;

#define _CITRUS_STDENC_SDID_GENERIC     0
struct _citrus_stdenc_state_desc
{
    union {
        struct {
            int state;
#define _CITRUS_STDENC_SDGEN_UNKNOWN        0
#define _CITRUS_STDENC_SDGEN_INITIAL        1
#define _CITRUS_STDENC_SDGEN_STABLE     2
#define _CITRUS_STDENC_SDGEN_INCOMPLETE_CHAR    3
#define _CITRUS_STDENC_SDGEN_INCOMPLETE_SHIFT   4
        } generic;
    } u;
};

#include "citrus_stdenc_local.h"

BEGIN_EXTERN_C
int  _citrus_stdenc_open(struct _citrus_stdenc * * ,
        char const * , const void * , size_t);
void     _citrus_stdenc_close(struct _citrus_stdenc *);
END_EXTERN_C

static __inline int
_citrus_stdenc_init_state(struct _citrus_stdenc * ce,
    void * ps)
{

    return ((*ce->ce_ops->eo_init_state)(ce, ps));
}

static __inline int
_citrus_stdenc_mbtocs(struct _citrus_stdenc * ce,
    _citrus_csid_t * csid, _citrus_index_t * idx,
    char ** s, size_t n, void * ps,
    size_t * nresult, struct iconv_hooks *hooks)
{

    return ((*ce->ce_ops->eo_mbtocs)(ce, csid, idx, s, n, ps, nresult,
        hooks));
}

static __inline int
_citrus_stdenc_cstomb(struct _citrus_stdenc * ce,
    char * s, size_t n, _citrus_csid_t csid, _citrus_index_t idx,
    void * ps, size_t * nresult,
    struct iconv_hooks *hooks)
{

    return ((*ce->ce_ops->eo_cstomb)(ce, s, n, csid, idx, ps, nresult,
        hooks));
}

static __inline int
_citrus_stdenc_wctomb(struct _citrus_stdenc * ce,
    char * s, size_t n, _citrus_wc_t wc, void * ps,
    size_t * nresult, struct iconv_hooks *hooks)
{

    return ((*ce->ce_ops->eo_wctomb)(ce, s, n, wc, ps, nresult, hooks));
}

static __inline int
_citrus_stdenc_put_state_reset(struct _citrus_stdenc * ce,
    char * s, size_t n, void * ps,
    size_t * nresult)
{

    return ((*ce->ce_ops->eo_put_state_reset)(ce, s, n, ps, nresult));
}

static __inline size_t
_citrus_stdenc_get_state_size(struct _citrus_stdenc *ce)
{

    return (ce->ce_traits->et_state_size);
}

static __inline size_t
_citrus_stdenc_get_mb_cur_min(struct _citrus_stdenc *ce)
{

       return (ce->ce_traits->et_mb_cur_min);
}

static __inline size_t
_citrus_stdenc_get_mb_cur_max(struct _citrus_stdenc *ce)
{

       return (ce->ce_traits->et_mb_cur_max);
}

static __inline int
_citrus_stdenc_get_state_desc(struct _citrus_stdenc * ce,
    void * ps, int id,
    struct _citrus_stdenc_state_desc * d)
{

    return ((*ce->ce_ops->eo_get_state_desc)(ce, ps, id, d));
}
#endif
