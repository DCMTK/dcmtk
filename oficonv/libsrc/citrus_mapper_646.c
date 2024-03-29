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
#include "citrus_mapper_646.h"

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

_CITRUS_MAPPER_DECLS(mapper_646);
_CITRUS_MAPPER_DEF_OPS(mapper_646);

/* ---------------------------------------------------------------------- */

#define ILSEQ   0xFFFFFFFE
#define INVALID 0xFFFFFFFF
#define SPECIALS(x)             \
    x(0x23)                 \
    x(0x24)                 \
    x(0x40)                 \
    x(0x5B)                 \
    x(0x5C)                 \
    x(0x5D)                 \
    x(0x5E)                 \
    x(0x60)                 \
    x(0x7B)                 \
    x(0x7C)                 \
    x(0x7D)                 \
    x(0x7E)

#define INDEX(x) INDEX_##x,

enum {
    SPECIALS(INDEX)
    NUM_OF_SPECIALS
};
struct _citrus_mapper_646 {
    _citrus_index_t     m6_map[NUM_OF_SPECIALS];
    int      m6_forward;
};

int
_citrus_mapper_646_mapper_getops(struct _citrus_mapper_ops *ops)
{

    memcpy(ops, &_citrus_mapper_646_mapper_ops,
        sizeof(_citrus_mapper_646_mapper_ops));

    return (0);
}

#define T_COMM '#'
static int
parse_file(struct _citrus_mapper_646 *m6, const char *path)
{
    struct _citrus_memory_stream ms;
    struct _citrus_region r;
    const char *p;
    char *pp;
    size_t len;
    char buf[OFICONV_PATH_MAX];
    int i, ret;

    ret = _citrus_map_file(&r, path);
    if (ret)
        return (ret);
    _citrus_memory_stream_bind(&ms, &r);
    for (i = 0; i < NUM_OF_SPECIALS; i++) {
retry:
        p = _citrus_memory_stream_getln(&ms, &len);
        if (p == NULL) {
            ret = EINVAL;
            break;
        }
        p = _citrus_bcs_skip_ws_len(p, &len);
        if (*p == T_COMM || len==0)
            goto retry;
        if (!_citrus_bcs_isdigit(*p)) {
            ret = EINVAL;
            break;
        }
        snprintf(buf, sizeof(buf), "%.*s", (int)len, p);
        pp = CITRUS_DECONST(void *, p);
        m6->m6_map[i] = strtoul(buf, (char **)&pp, 0);
        p = _citrus_bcs_skip_ws(buf);
        if (*p != T_COMM && !*p) {
            ret = EINVAL;
            break;
        }
    }
    _citrus_unmap_file(&r);

    return (ret);
};

static int
parse_var(struct _citrus_mapper_646 *m6, struct _citrus_memory_stream *ms,
    const char *dir)
{
    struct _citrus_region r;
    char path[OFICONV_PATH_MAX];

    memset(&r, 0, sizeof(struct _citrus_region ));
    m6->m6_forward = 1;
    _citrus_memory_stream_skip_ws(ms);
    /* whether backward */
    if (_citrus_memory_stream_peek(ms) == '!') {
        _citrus_memory_stream_getc(ms);
        m6->m6_forward = 0;
    }
    /* get description file path */
    _citrus_memory_stream_getregion(ms, &r, _citrus_memory_stream_remainder(ms));
    snprintf(path, sizeof(path), "%s/%.*s",
         dir, (int)_citrus_region_size(&r), (char *)_citrus_region_head(&r));
    /* remove trailing white spaces */
    path[_citrus_bcs_skip_nonws(path)-path] = '\0';
    return (parse_file(m6, path));
}

static int
/*ARGSUSED*/
_citrus_mapper_646_mapper_init(struct _citrus_mapper_area * ma ,
    struct _citrus_csmapper * cm, const char * dir,
    const void * var, size_t lenvar,
    struct _citrus_mapper_traits * mt, size_t lenmt)
{
    struct _citrus_mapper_646 *m6;
    struct _citrus_memory_stream ms;
    struct _citrus_region r;
    int ret;
    (void) ma;

    if (lenmt < sizeof(*mt))
        return (EINVAL);

    m6 = malloc(sizeof(*m6));
    if (m6 == NULL)
        return (errno);

    _citrus_region_init(&r, CITRUS_DECONST(void *, var), lenvar);
    _citrus_memory_stream_bind(&ms, &r);
    ret = parse_var(m6, &ms, dir);
    if (ret) {
        free(m6);
        return (ret);
    }

    cm->cm_closure = m6;
    mt->mt_src_max = mt->mt_dst_max = 1;    /* 1:1 converter */
    mt->mt_state_size = 0;          /* stateless */

    return (0);
}

static void
/*ARGSUSED*/
_citrus_mapper_646_mapper_uninit(struct _citrus_csmapper *cm)
{

    if (cm && cm->cm_closure)
        free(cm->cm_closure);
}

static int
/*ARGSUSED*/
_citrus_mapper_646_mapper_convert(struct _citrus_csmapper * cm,
    _citrus_index_t * dst, _citrus_index_t src, void * ps )
{
    struct _citrus_mapper_646 *m6;
    (void) ps;

    m6 = cm->cm_closure;
    if (m6->m6_forward) {
        /* forward */
        if (src >= 0x80)
            return (_CITRUS_MAPPER_CONVERT_ILSEQ);
#define FORWARD(x)                  \
if (src == (x)) {                   \
    if (m6->m6_map[INDEX_##x]==INVALID)     \
        return (_CITRUS_MAPPER_CONVERT_NONIDENTICAL);  \
    *dst = m6->m6_map[INDEX_##x];           \
    return (0);                 \
} else
        SPECIALS(FORWARD);
        *dst = src;
    } else {
        /* backward */
#define BACKWARD(x)                         \
if (m6->m6_map[INDEX_##x] != INVALID && src == m6->m6_map[INDEX_##x]) { \
    *dst = (x);                         \
    return (0);                         \
} else if (src == (x))                          \
    return (_CITRUS_MAPPER_CONVERT_ILSEQ);                 \
else
        SPECIALS(BACKWARD);
        if (src >= 0x80)
            return (_CITRUS_MAPPER_CONVERT_NONIDENTICAL);
        *dst = src;
    }

    return (_CITRUS_MAPPER_CONVERT_SUCCESS);
}

static void
/*ARGSUSED*/
_citrus_mapper_646_mapper_init_state(void)
{

}
