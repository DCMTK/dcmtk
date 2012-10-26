// -*- C++ -*-
// Module:  Log4CPLUS
// File:    internal.h
// Created: 1/2009
// Author:  Vaclav Haisman
//
//
//  Copyright (C) 2009-2010, Vaclav Haisman. All rights reserved.
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

/** @file 
 * This header contains declaration internal to log4cplus. They must never be
 * visible from user accesible headers or exported in DLL/shared libray.
 */


#ifndef DCMTK_LOG4CPLUS_INTERNAL_INTERNAL_HEADER_
#define DCMTK_LOG4CPLUS_INTERNAL_INTERNAL_HEADER_

#include "dcmtk/oflog/config.h"

#if defined (DCMTK_LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#if ! defined (DCMTK_INSIDE_LOG4CPLUS)
#  error "This header must not be be used outside log4cplus' implementation files."
#endif

#include <memory>
#include "dcmtk/ofstd/ofvector.h"
#include <sstream>
#include <cstdio>
#include "dcmtk/oflog/tstring.h"
#include "dcmtk/oflog/streams.h"
#include "dcmtk/oflog/ndc.h"
#include "dcmtk/oflog/mdc.h"
#include "dcmtk/oflog/spi/logevent.h"
#include "dcmtk/oflog/thread/impl/tls.h"
#include "dcmtk/oflog/helpers/snprintf.h"


namespace dcmtk {
namespace log4cplus {

namespace internal {


//! Canonical empty string. It is used when the need to return empty string
//! by reference arises.
extern log4cplus::tstring const empty_str;


struct gft_scratch_pad
{
    gft_scratch_pad ();
    ~gft_scratch_pad ();
    
    void
    reset ()
    {
        uc_q_str_valid = false;
        q_str_valid = false;
        s_str_valid = false;
        ret.clear ();
    }
    
    log4cplus::tstring q_str;
    log4cplus::tstring uc_q_str;
    log4cplus::tstring s_str;
    log4cplus::tstring ret;
    log4cplus::tstring fmt;
    log4cplus::tstring tmp;
    OFVector<tchar> buffer;
    bool uc_q_str_valid;
    bool q_str_valid;
    bool s_str_valid;
};


struct appender_sratch_pad
{
    appender_sratch_pad ();
    ~appender_sratch_pad ();

    tostringstream oss;
    tstring str;
    STD_NAMESPACE string chstr;
};


//! Per thread data.
struct per_thread_data
{
    per_thread_data ();
    ~per_thread_data ();

    tostringstream macros_oss;
    tostringstream layout_oss;
    DiagnosticContextStack ndc_dcs;
    MappedDiagnosticContextMap mdc_map;
    log4cplus::tstring thread_name;
    log4cplus::tstring thread_name2;
    gft_scratch_pad gft_sp;
    appender_sratch_pad appender_sp;
    log4cplus::tstring faa_str;
    log4cplus::tstring ll_str;
    spi::InternalLoggingEvent forced_log_ev;
    FILE * fnull;
    log4cplus::helpers::snprintf_buf snprintf_buf;

private:
    per_thread_data (per_thread_data&);
    per_thread_data& operator=(const per_thread_data&);
};


per_thread_data * alloc_ptd ();

// TLS key whose value is pointer struct per_thread_data.
extern log4cplus::thread::impl::tls_key_type tls_storage_key;


#if ! defined (DCMTK_LOG4CPLUS_SINGLE_THREADED) \
    && defined (DCMTK_LOG4CPLUS_THREAD_LOCAL_VAR)

extern DCMTK_LOG4CPLUS_THREAD_LOCAL_VAR per_thread_data * ptd;


inline
void
set_ptd (per_thread_data * p)
{
    ptd = p;
}


//! The default value of the \param alloc is false for Win32 DLL builds
//! since per thread data are already initialized by DllMain().
inline
per_thread_data *
get_ptd (bool alloc
#if defined (_WIN32) && defined (DCMTK_LOG4CPLUS_BUILD_DLL)
         = false
#else
         = true
#endif
         )
{
    if (DCMTK_LOG4CPLUS_UNLIKELY (! ptd && alloc))
        return alloc_ptd ();

    // The assert() does not belong here. get_ptd() might be called by
    // cleanup code that can handle the returned NULL pointer.
    //assert (ptd);

    return ptd;
}


#else // defined (DCMTK_LOG4CPLUS_THREAD_LOCAL_VAR)


inline
void
set_ptd (per_thread_data * p)
{
    thread::impl::tls_set_value (tls_storage_key, p);
}


inline
per_thread_data *
get_ptd (bool alloc = true)
{
    per_thread_data * ptd
        = OFreinterpret_cast(per_thread_data *,
            thread::impl::tls_get_value (tls_storage_key));

    if (DCMTK_LOG4CPLUS_UNLIKELY (! ptd && alloc))
        return alloc_ptd ();

    return ptd;
}


#endif // defined (DCMTK_LOG4CPLUS_THREAD_LOCAL_VAR)


inline
tstring &
get_thread_name_str ()
{
    return get_ptd ()->thread_name;
}


inline
tstring &
get_thread_name2_str ()
{
    return get_ptd ()->thread_name2;
}


inline
gft_scratch_pad &
get_gft_scratch_pad ()
{
    return get_ptd ()->gft_sp;
}


inline
appender_sratch_pad &
get_appender_sp ()
{
    return get_ptd ()->appender_sp;
}


} // namespace internal {


} // namespace log4cplus { 
} // end namespace dcmtk


#endif // DCMTK_LOG4CPLUS_INTERNAL_INTERNAL_HEADER_
