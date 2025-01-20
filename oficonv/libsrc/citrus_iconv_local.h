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

#ifndef _CITRUS_ICONV_LOCAL_H_
#define _CITRUS_ICONV_LOCAL_H_

#include "dcmtk/config/osconfig.h"
#include "dcmtk/oficonv/iconv.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef HAVE_SYS_QUEUE_H
#include <sys/queue.h>
#else
#include "dcmtk/oficonv/queue.h"
#endif

#include "citrus_hash.h"
#include "citrus_types.h"
#include "citrus_module.h"

struct _citrus_iconv_shared;
struct _citrus_iconv_ops;
struct _citrus_iconv;

#define _CITRUS_ICONV_GETOPS_FUNC_BASE(_n_)             \
    int _n_(struct _citrus_iconv_ops *)
#define _CITRUS_ICONV_GETOPS_FUNC(_n_)                  \
    _CITRUS_ICONV_GETOPS_FUNC_BASE(_citrus_##_n_##_iconv_getops)

#define _CITRUS_ICONV_DECLS(_m_)                    \
static int   _citrus_##_m_##_iconv_init_shared          \
            (struct _citrus_iconv_shared * ,      \
            const char * , const char * );  \
static void  _citrus_##_m_##_iconv_uninit_shared            \
            (struct _citrus_iconv_shared *);            \
static int   _citrus_##_m_##_iconv_convert              \
            (struct _citrus_iconv * ,         \
            char * * ,       \
            size_t * ,                \
            char * * ,         \
            size_t * outbytes,           \
            uint32_t, size_t * );         \
static int   _citrus_##_m_##_iconv_init_context         \
            (struct _citrus_iconv *);               \
static void  _citrus_##_m_##_iconv_uninit_context           \
            (struct _citrus_iconv *)


#define _CITRUS_ICONV_DEF_OPS(_m_)                  \
extern struct _citrus_iconv_ops _citrus_##_m_##_iconv_ops;      \
struct _citrus_iconv_ops _citrus_##_m_##_iconv_ops = {          \
    /* io_init_shared */    &_citrus_##_m_##_iconv_init_shared, \
    /* io_uninit_shared */  &_citrus_##_m_##_iconv_uninit_shared,   \
    /* io_init_context */   &_citrus_##_m_##_iconv_init_context,    \
    /* io_uninit_context */ &_citrus_##_m_##_iconv_uninit_context,  \
    /* io_convert */    &_citrus_##_m_##_iconv_convert      \
}

typedef _CITRUS_ICONV_GETOPS_FUNC_BASE((*_citrus_iconv_getops_t));
typedef int (*_citrus_iconv_init_shared_t)
    (struct _citrus_iconv_shared * ,
    const char * , const char * );
typedef void (*_citrus_iconv_uninit_shared_t)
    (struct _citrus_iconv_shared *);
typedef int (*_citrus_iconv_convert_t)
    (struct _citrus_iconv * ,
    char ** , size_t * ,
    char * * , size_t * , uint32_t,
    size_t * );
typedef int (*_citrus_iconv_init_context_t)(struct _citrus_iconv *);
typedef void (*_citrus_iconv_uninit_context_t)(struct _citrus_iconv *);

struct _citrus_iconv_ops {
    _citrus_iconv_init_shared_t     io_init_shared;
    _citrus_iconv_uninit_shared_t   io_uninit_shared;
    _citrus_iconv_init_context_t    io_init_context;
    _citrus_iconv_uninit_context_t  io_uninit_context;
    _citrus_iconv_convert_t         io_convert;
};

struct _citrus_iconv_shared {
    struct _citrus_iconv_ops                 *ci_ops;
    void                                     *ci_closure;
    _CITRUS_HASH_ENTRY(_citrus_iconv_shared)  ci_hash_entry;
    TAILQ_ENTRY(_citrus_iconv_shared)         ci_tailq_entry;
    _citrus_module_t                          ci_module;
    unsigned int                              ci_used_count;
    char                                     *ci_convname;
};

struct _citrus_iconv {
    struct _citrus_iconv_shared *cv_shared;
    void                        *cv_closure;
    struct iconv_hooks          *ci_hooks;
    bool                         ci_discard_ilseq;
    bool                         ci_ilseq_invalid;
};

#endif
