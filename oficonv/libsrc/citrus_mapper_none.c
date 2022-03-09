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
#include "citrus_mapper_none.h"

#ifdef HAVE_SYS_QUEUE_H
#include <sys/queue.h>
#else
#include "dcmtk/oficonv/queue.h"
#endif

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "citrus_bcs.h"
#include "citrus_types.h"
#include "citrus_module.h"
#include "citrus_hash.h"
#include "citrus_mapper.h"

/* ---------------------------------------------------------------------- */

_CITRUS_MAPPER_DECLS(mapper_none);
_CITRUS_MAPPER_DEF_OPS(mapper_none);


/* ---------------------------------------------------------------------- */

int
_citrus_mapper_none_mapper_getops(struct _citrus_mapper_ops *ops)
{

    memcpy(ops, &_citrus_mapper_none_mapper_ops,
        sizeof(_citrus_mapper_none_mapper_ops));

    return (0);
}

static int
/*ARGSUSED*/
_citrus_mapper_none_mapper_init(struct _citrus_mapper_area * ma ,
    struct _citrus_csmapper * cm, const char * dir ,
    const void * var , size_t lenvar ,
    struct _citrus_mapper_traits * mt, size_t lenmt)
{
    (void) ma;
    (void) dir;
    (void) var;
    (void) lenvar;

    if (lenmt < sizeof(*mt))
        return (EINVAL);

    cm->cm_closure = NULL;
    mt->mt_src_max = mt->mt_dst_max = 1;    /* 1:1 converter */
    mt->mt_state_size = 0;          /* stateless */

    return (0);
}

static void
/*ARGSUSED*/
_citrus_mapper_none_mapper_uninit(struct _citrus_csmapper *cm )
{
    (void) cm;
}

static int
/*ARGSUSED*/
_citrus_mapper_none_mapper_convert(struct _citrus_csmapper * cm ,
    _citrus_index_t * dst, _citrus_index_t src,
    void * ps )
{
    (void) cm;
    (void) ps;

    *dst = src;
    return (_CITRUS_MAPPER_CONVERT_SUCCESS);
}

static void
/*ARGSUSED*/
_citrus_mapper_none_mapper_init_state(void)
{

}
