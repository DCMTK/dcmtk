// -*- C++ -*-
//  Copyright (C) 2010, Vaclav Haisman. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modifica-
//  tion, are permitted provided that the following conditions are met:
//
//  1. Redistributions of  source code must  retain the above copyright  notice,
//     this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
//  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
//  FITNESS  FOR A PARTICULAR  PURPOSE ARE  DISCLAIMED.  IN NO  EVENT SHALL  THE
//  APACHE SOFTWARE  FOUNDATION  OR ITS CONTRIBUTORS  BE LIABLE FOR  ANY DIRECT,
//  INDIRECT, INCIDENTAL, SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL  DAMAGES (INCLU-
//  DING, BUT NOT LIMITED TO, PROCUREMENT  OF SUBSTITUTE GOODS OR SERVICES; LOSS
//  OF USE, DATA, OR  PROFITS; OR BUSINESS  INTERRUPTION)  HOWEVER CAUSED AND ON
//  ANY  THEORY OF LIABILITY,  WHETHER  IN CONTRACT,  STRICT LIABILITY,  OR TORT
//  (INCLUDING  NEGLIGENCE OR  OTHERWISE) ARISING IN  ANY WAY OUT OF THE  USE OF
//  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef DCMTK_LOG4CPLUS_THREAD_IMPL_TLS_H
#define DCMTK_LOG4CPLUS_THREAD_IMPL_TLS_H

#include "dcmtk/oflog/config.h"

#if defined (DCMTK_LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include <new>
#include <cassert>

#if ! defined (DCMTK_INSIDE_LOG4CPLUS)
#  error "This header must not be be used outside log4cplus' implementation files."
#endif

#ifdef DCMTK_LOG4CPLUS_USE_PTHREADS
#  include <pthread.h>

#elif defined (DCMTK_LOG4CPLUS_USE_WIN32_THREADS)
#  include "dcmtk/oflog/config/windowsh.h"

#elif defined (DCMTK_LOG4CPLUS_SINGLE_THREADED)
#  include "dcmtk/ofstd/ofvector.h"

#endif

#ifdef __MINGW32__
/* MinGW does not support fiber local storage. Use Thread Local Storage instead. */
#define DCMTK_LOG4CPLUS_AVOID_WIN32_FLS
#endif

namespace dcmtk {
namespace log4cplus { namespace thread { namespace impl {


typedef void * tls_value_type;

#ifdef DCMTK_LOG4CPLUS_USE_PTHREADS
typedef pthread_key_t * tls_key_type;
typedef void (* tls_init_cleanup_func_type)(void *);

#elif defined (DCMTK_LOG4CPLUS_USE_WIN32_THREADS)
typedef DWORD tls_key_type;
typedef void (WINAPI * tls_init_cleanup_func_type)(void *);

#elif defined (DCMTK_LOG4CPLUS_SINGLE_THREADED)
typedef size_t tls_key_type;
typedef void (* tls_init_cleanup_func_type)(void *);

#endif


inline tls_value_type tls_get_value (tls_key_type);
inline void tls_set_value (tls_key_type, tls_value_type);
inline void tls_cleanup (tls_key_type);

#if defined (DCMTK_LOG4CPLUS_USE_PTHREADS)

inline tls_key_type tls_init (tls_init_cleanup_func_type);

tls_key_type
tls_init (tls_init_cleanup_func_type cleanupfunc)
{
    pthread_key_t * key = new pthread_key_t;
    pthread_key_create (key, cleanupfunc);
    return key;
}


tls_value_type
tls_get_value (tls_key_type key)
{
    return pthread_getspecific (*key);
}


void
tls_set_value (tls_key_type key, tls_value_type value)
{
    pthread_setspecific(*key, value);
}


void
tls_cleanup (tls_key_type key)
{
    pthread_key_delete (*key);
    delete key;
}


#elif defined (DCMTK_LOG4CPLUS_USE_WIN32_THREADS)

#ifdef DCMTK_LOG4CPLUS_AVOID_WIN32_FLS
inline tls_key_type tls_init (tls_init_cleanup_func_type);

tls_key_type
tls_init (tls_init_cleanup_func_type)
{
    return TlsAlloc();
}
#else
inline tls_key_type tls_init (PFLS_CALLBACK_FUNCTION);

tls_key_type
tls_init (PFLS_CALLBACK_FUNCTION cleanupfunc)
{
    return FlsAlloc(cleanupfunc);
}
#endif


tls_value_type tls_get_value (tls_key_type k)
{
#ifdef DCMTK_LOG4CPLUS_AVOID_WIN32_FLS
    return TlsGetValue(k);
#else
    return FlsGetValue(k);
#endif
}


void
tls_set_value (tls_key_type k, tls_value_type value)
{
#ifdef DCMTK_LOG4CPLUS_AVOID_WIN32_FLS
    TlsSetValue(k, value);
#else
    FlsSetValue(k, value);
#endif
}


void
tls_cleanup (tls_key_type k)
{
#ifdef DCMTK_LOG4CPLUS_AVOID_WIN32_FLS
    TlsFree(k);
#else
    FlsFree(k);
#endif
}


#elif defined (DCMTK_LOG4CPLUS_SINGLE_THREADED)
extern OFVector<tls_value_type> * tls_single_threaded_values;

inline tls_key_type tls_init (tls_init_cleanup_func_type);

tls_key_type
tls_init (tls_init_cleanup_func_type)
{
    if (! tls_single_threaded_values)
        tls_single_threaded_values = new OFVector<tls_value_type>;
    tls_key_type key = tls_single_threaded_values->size ();
    tls_single_threaded_values->resize (key + 1);
    return key;
}


tls_value_type
tls_get_value (tls_key_type k)
{
    assert (k < tls_single_threaded_values->size ());
    return (*tls_single_threaded_values)[k];
}


void
tls_set_value (tls_key_type k, tls_value_type val)
{
    assert (k < tls_single_threaded_values->size ());
    (*tls_single_threaded_values)[k] = val;
}


void
tls_cleanup (tls_key_type k)
{
    assert (k < tls_single_threaded_values->size ());
    (*tls_single_threaded_values)[k] = 0;
}

#endif


} } } // namespace log4cplus { namespace thread { namespace impl {
} // end namespace dcmtk

#endif // DCMTK_LOG4CPLUS_THREAD_IMPL_TLS_H
