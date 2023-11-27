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
#include "citrus_mapper_serial.h"

#ifdef HAVE_SYS_QUEUE_H
#include <sys/queue.h>
#else
#include "dcmtk/oficonv/queue.h"
#endif

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "citrus_bcs.h"
#include "citrus_types.h"
#include "citrus_module.h"
#include "citrus_region.h"
#include "citrus_memstream.h"
#include "citrus_mmap.h"
#include "citrus_hash.h"
#include "citrus_mapper.h"

/* ---------------------------------------------------------------------- */

_CITRUS_MAPPER_DECLS(mapper_serial);
_CITRUS_MAPPER_DEF_OPS(mapper_serial);

#define _citrus_mapper_parallel_mapper_init     \
    _citrus_mapper_serial_mapper_init
#define _citrus_mapper_parallel_mapper_uninit       \
    _citrus_mapper_serial_mapper_uninit
#define _citrus_mapper_parallel_mapper_init_state   \
    _citrus_mapper_serial_mapper_init_state
static int  _citrus_mapper_parallel_mapper_convert(
            struct _citrus_csmapper * , _citrus_index_t * ,
            _citrus_index_t, void * );
_CITRUS_MAPPER_DEF_OPS(mapper_parallel);
#undef _citrus_mapper_parallel_mapper_init
#undef _citrus_mapper_parallel_mapper_uninit
#undef _citrus_mapper_parallel_mapper_init_state


/* ---------------------------------------------------------------------- */

struct maplink {
    STAILQ_ENTRY(maplink)    ml_entry;
    struct _citrus_csmapper   *ml_mapper;
};
STAILQ_HEAD(maplist, maplink);

struct _citrus_mapper_serial {
    struct maplist       sr_mappers;
};

int
_citrus_mapper_serial_mapper_getops(struct _citrus_mapper_ops *ops)
{

    memcpy(ops, &_citrus_mapper_serial_mapper_ops,
        sizeof(_citrus_mapper_serial_mapper_ops));

    return (0);
}

int
_citrus_mapper_parallel_mapper_getops(struct _citrus_mapper_ops *ops)
{

    memcpy(ops, &_citrus_mapper_parallel_mapper_ops,
        sizeof(_citrus_mapper_parallel_mapper_ops));

    return (0);
}

static void
uninit(struct _citrus_mapper_serial *sr)
{
    struct maplink *ml;

    while ((ml = STAILQ_FIRST(&sr->sr_mappers)) != NULL) {
        STAILQ_REMOVE_HEAD(&sr->sr_mappers, ml_entry);
        _citrus_csmapper_close(ml->ml_mapper);
        free(ml);
    }
}

static int
parse_var(struct _citrus_mapper_area * ma,
    struct _citrus_mapper_serial *sr, struct _citrus_memory_stream *ms)
{
    struct _citrus_region r;
    struct maplink *ml;
    char mapname[OFICONV_PATH_MAX];
    int ret;

    STAILQ_INIT(&sr->sr_mappers);
    while (1) {
        /* remove beginning white spaces */
        _citrus_memory_stream_skip_ws(ms);
        if (_citrus_memory_stream_iseof(ms))
            break;
        /* cut down a mapper name */
        _citrus_memory_stream_chr(ms, &r, ',');
        snprintf(mapname, sizeof(mapname), "%.*s",
            (int)_citrus_region_size(&r), (char *)_citrus_region_head(&r));
        /* remove trailing white spaces */
        mapname[_citrus_bcs_skip_nonws(mapname)-mapname] = '\0';
        /* create a new mapper record */
        ml = malloc(sizeof(*ml));
        if (ml == NULL)
            return (errno);
        ret = _citrus_mapper_open(ma, &ml->ml_mapper, mapname);
        if (ret) {
            free(ml);
            return (ret);
        }
        /* support only 1:1 and stateless converter */
        if (_citrus_csmapper_get_src_max(ml->ml_mapper) != 1 ||
            _citrus_csmapper_get_dst_max(ml->ml_mapper) != 1 ||
            _citrus_csmapper_get_state_size(ml->ml_mapper) != 0) {
            free(ml);
            return (EINVAL);
        }
        STAILQ_INSERT_TAIL(&sr->sr_mappers, ml, ml_entry);
    }
    return (0);
}

static int
/*ARGSUSED*/
_citrus_mapper_serial_mapper_init(struct _citrus_mapper_area * ma ,
    struct _citrus_csmapper * cm, const char * dir ,
    const void * var, size_t lenvar,
    struct _citrus_mapper_traits * mt, size_t lenmt)
{
    struct _citrus_mapper_serial *sr;
    struct _citrus_memory_stream ms;
    struct _citrus_region r;
    (void) ma;
    (void) dir;

    if (lenmt < sizeof(*mt))
        return (EINVAL);

    sr = malloc(sizeof(*sr));
    if (sr == NULL)
        return (errno);

    _citrus_region_init(&r, CITRUS_DECONST(void *, var), lenvar);
    _citrus_memory_stream_bind(&ms, &r);
    if (parse_var(ma, sr, &ms)) {
        uninit(sr);
        free(sr);
        return (EINVAL);
    }
    cm->cm_closure = sr;
    mt->mt_src_max = mt->mt_dst_max = 1;    /* 1:1 converter */
    mt->mt_state_size = 0;          /* stateless */

    return (0);
}

static void
/*ARGSUSED*/
_citrus_mapper_serial_mapper_uninit(struct _citrus_csmapper *cm)
{

    if (cm && cm->cm_closure) {
        uninit(cm->cm_closure);
        free(cm->cm_closure);
    }
}

static int
/*ARGSUSED*/
_citrus_mapper_serial_mapper_convert(struct _citrus_csmapper * cm,
    _citrus_index_t * dst, _citrus_index_t src, void * ps )
{
    struct _citrus_mapper_serial *sr;
    struct maplink *ml;
    int ret;
    (void) ps;

    sr = cm->cm_closure;
    STAILQ_FOREACH(ml, &sr->sr_mappers, ml_entry) {
        ret = _citrus_csmapper_convert(ml->ml_mapper, &src, src, NULL);
        if (ret != _CITRUS_MAPPER_CONVERT_SUCCESS)
            return (ret);
    }
    *dst = src;
    return (_CITRUS_MAPPER_CONVERT_SUCCESS);
}

static int
/*ARGSUSED*/
_citrus_mapper_parallel_mapper_convert(struct _citrus_csmapper * cm,
    _citrus_index_t * dst, _citrus_index_t src, void * ps )
{
    struct _citrus_mapper_serial *sr;
    struct maplink *ml;
    _citrus_index_t tmp;
    int ret;
    (void) ps;

    sr = cm->cm_closure;
    STAILQ_FOREACH(ml, &sr->sr_mappers, ml_entry) {
        ret = _citrus_csmapper_convert(ml->ml_mapper, &tmp, src, NULL);
        if (ret == _CITRUS_MAPPER_CONVERT_SUCCESS) {
            *dst = tmp;
            return (_CITRUS_MAPPER_CONVERT_SUCCESS);
        } else if (ret == _CITRUS_MAPPER_CONVERT_ILSEQ)
            return (_CITRUS_MAPPER_CONVERT_ILSEQ);
    }
    return (_CITRUS_MAPPER_CONVERT_NONIDENTICAL);
}

static void
/*ARGSUSED*/
_citrus_mapper_serial_mapper_init_state(void)
{

}
