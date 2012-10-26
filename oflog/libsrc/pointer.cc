// Module:  Log4CPLUS
// File:    pointer.cxx
// Created: 6/2001
// Author:  Tad E. Smith
//
//
// Copyright 2001-2010 Tad E. Smith
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "dcmtk/oflog/streams.h"
#include "dcmtk/oflog/helpers/pointer.h"
#include "dcmtk/oflog/thread/threads.h"
#include "dcmtk/oflog/thread/impl/syncimpl.h"
#include "dcmtk/oflog/config/windowsh.h"
#include <cassert>
#if defined (DCMTK_LOG4CPLUS_HAVE_INTRIN_H)
#include <intrin.h>
#endif


namespace dcmtk {
namespace log4cplus { namespace helpers {


///////////////////////////////////////////////////////////////////////////////
// log4cplus::helpers::SharedObject dtor
///////////////////////////////////////////////////////////////////////////////

SharedObject::~SharedObject()
{
    assert(count == 0);
}



///////////////////////////////////////////////////////////////////////////////
// log4cplus::helpers::SharedObject public methods
///////////////////////////////////////////////////////////////////////////////

void
SharedObject::addReference() const
{
#if defined (DCMTK_LOG4CPLUS_SINGLE_THREADED)
    ++count;

#elif defined (DCMTK_LOG4CPLUS_HAVE_CXX11_ATOMICS)
    STD_NAMESPACE atomic_fetch_add_explicit (&count, 1u,
        STD_NAMESPACE memory_order_relaxed);

#elif defined (DCMTK_LOG4CPLUS_HAVE___SYNC_ADD_AND_FETCH)
    __sync_add_and_fetch (&count, 1);

#elif defined (_WIN32) && defined (DCMTK_LOG4CPLUS_HAVE_INTRIN_H)
    _InterlockedIncrement (&count);

#elif defined (_WIN32)
    InterlockedIncrement (&count);

#else
    thread::MutexGuard guard (access_mutex);
    ++count;

#endif
}


void
SharedObject::removeReference() const
{
    assert (count > 0);
    bool destroy;

#if defined (DCMTK_LOG4CPLUS_SINGLE_THREADED)
    destroy = --count == 0;

#elif defined (DCMTK_LOG4CPLUS_HAVE_CXX11_ATOMICS)
    destroy = STD_NAMESPACE atomic_fetch_sub_explicit (&count, 1u,
        STD_NAMESPACE memory_order_release) == 1;
    if (DCMTK_LOG4CPLUS_UNLIKELY (destroy))
        STD_NAMESPACE atomic_thread_fence (STD_NAMESPACE memory_order_acquire);

#elif defined (DCMTK_LOG4CPLUS_HAVE___SYNC_SUB_AND_FETCH)
    destroy = __sync_sub_and_fetch (&count, 1) == 0;

#elif defined (_WIN32) && defined (DCMTK_LOG4CPLUS_HAVE_INTRIN_H)
    destroy = _InterlockedDecrement (&count) == 0;

#elif defined (_WIN32)
    destroy = InterlockedDecrement (&count) == 0;

#else
    {
        thread::MutexGuard guard (access_mutex);
        destroy = --count == 0;
    }

#endif
    if (DCMTK_LOG4CPLUS_UNLIKELY (destroy))
        delete this;
}


} } // namespace log4cplus { namespace helpers
} // end namespace dcmtk
