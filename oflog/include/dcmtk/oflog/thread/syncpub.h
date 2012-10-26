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

#ifndef DCMTK_LOG4CPLUS_THREAD_SYNCPRIMS_PUB_IMPL_H
#define DCMTK_LOG4CPLUS_THREAD_SYNCPRIMS_PUB_IMPL_H

#include "dcmtk/oflog/config.h"

#if defined (DCMTK_LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#if (defined (DCMTK_LOG4CPLUS_INLINES_ARE_EXPORTED)           \
    && defined (DCMTK_LOG4CPLUS_BUILD_DLL))                   \
    || defined (DCMTK_LOG4CPLUS_ENABLE_SYNCPRIMS_PUB_IMPL)
#include "dcmtk/oflog/thread/syncprim.h"

#if defined (DCMTK_LOG4CPLUS_SINGLE_THREADED)
#  define DCMTK_LOG4CPLUS_THREADED(x)
#else
#  include "dcmtk/oflog/thread/impl/syncimpl.h"
#  define DCMTK_LOG4CPLUS_THREADED(x) (x)
#endif


namespace dcmtk {
namespace log4cplus { namespace thread {


//
//
//

DCMTK_LOG4CPLUS_INLINE_EXPORT
MutexImplBase::~MutexImplBase ()
{ }


//
//
//

DCMTK_LOG4CPLUS_INLINE_EXPORT
Mutex::Mutex (Mutex::Type t)
    : mtx (DCMTK_LOG4CPLUS_THREADED (new impl::Mutex (t)) + 0)
{ }


DCMTK_LOG4CPLUS_INLINE_EXPORT
Mutex::~Mutex ()
{
    DCMTK_LOG4CPLUS_THREADED (delete OFstatic_cast(impl::Mutex *, mtx));
}


DCMTK_LOG4CPLUS_INLINE_EXPORT
void
Mutex::lock () const
{
    DCMTK_LOG4CPLUS_THREADED (OFstatic_cast(impl::Mutex *, mtx)->lock ());
}


DCMTK_LOG4CPLUS_INLINE_EXPORT
void
Mutex::unlock () const
{
    DCMTK_LOG4CPLUS_THREADED (OFstatic_cast(impl::Mutex *, mtx)->unlock ());
}


//
//
//

DCMTK_LOG4CPLUS_INLINE_EXPORT
SemaphoreImplBase::~SemaphoreImplBase ()
{ }


//
//
//

DCMTK_LOG4CPLUS_INLINE_EXPORT
Semaphore::Semaphore (unsigned DCMTK_LOG4CPLUS_THREADED (max),
    unsigned DCMTK_LOG4CPLUS_THREADED (initial))
    : sem (DCMTK_LOG4CPLUS_THREADED (new impl::Semaphore (max, initial)) + 0)
{ }


DCMTK_LOG4CPLUS_INLINE_EXPORT
Semaphore::~Semaphore ()
{
    DCMTK_LOG4CPLUS_THREADED (delete OFstatic_cast(impl::Semaphore *, sem));
}


DCMTK_LOG4CPLUS_INLINE_EXPORT
void
Semaphore::lock () const
{
    DCMTK_LOG4CPLUS_THREADED (OFstatic_cast(impl::Semaphore *, sem)->lock ());
}


DCMTK_LOG4CPLUS_INLINE_EXPORT
void
Semaphore::unlock () const
{
    DCMTK_LOG4CPLUS_THREADED (OFstatic_cast(impl::Semaphore *, sem)->unlock ());
}


//
//
//

DCMTK_LOG4CPLUS_INLINE_EXPORT
FairMutexImplBase::~FairMutexImplBase ()
{ }


//
//
//

DCMTK_LOG4CPLUS_INLINE_EXPORT
FairMutex::FairMutex ()
    : mtx (DCMTK_LOG4CPLUS_THREADED (new impl::FairMutex) + 0)
{ }


DCMTK_LOG4CPLUS_INLINE_EXPORT
FairMutex::~FairMutex ()
{
    DCMTK_LOG4CPLUS_THREADED (delete OFstatic_cast(impl::FairMutex *, mtx));
}


DCMTK_LOG4CPLUS_INLINE_EXPORT
void
FairMutex::lock () const
{
    DCMTK_LOG4CPLUS_THREADED (OFstatic_cast(impl::FairMutex *, mtx)->lock ());
}


DCMTK_LOG4CPLUS_INLINE_EXPORT
void
FairMutex::unlock () const
{
    DCMTK_LOG4CPLUS_THREADED (OFstatic_cast(impl::FairMutex *, mtx)->unlock ());
}


//
//
//

DCMTK_LOG4CPLUS_INLINE_EXPORT
ManualResetEventImplBase::~ManualResetEventImplBase ()
{ }


//
//
//

DCMTK_LOG4CPLUS_INLINE_EXPORT
ManualResetEvent::ManualResetEvent (bool DCMTK_LOG4CPLUS_THREADED (sig))
    : ev (DCMTK_LOG4CPLUS_THREADED (new impl::ManualResetEvent (sig)) + 0)
{ }


DCMTK_LOG4CPLUS_INLINE_EXPORT
ManualResetEvent::~ManualResetEvent ()
{
    DCMTK_LOG4CPLUS_THREADED (delete OFstatic_cast(impl::ManualResetEvent *, ev));
}


DCMTK_LOG4CPLUS_INLINE_EXPORT
void
ManualResetEvent::signal () const
{
    DCMTK_LOG4CPLUS_THREADED (OFstatic_cast(impl::ManualResetEvent *, ev)->signal ());
}


DCMTK_LOG4CPLUS_INLINE_EXPORT
void
ManualResetEvent::wait () const
{
    DCMTK_LOG4CPLUS_THREADED (OFstatic_cast(impl::ManualResetEvent *, ev)->wait ());
}


DCMTK_LOG4CPLUS_INLINE_EXPORT
bool
ManualResetEvent::timed_wait (unsigned long DCMTK_LOG4CPLUS_THREADED (msec)) const
{
#if defined (DCMTK_LOG4CPLUS_SINGLE_THREADED)
    return true;
#else
    return OFstatic_cast(impl::ManualResetEvent *, ev)->timed_wait (msec);
#endif
}


DCMTK_LOG4CPLUS_INLINE_EXPORT
void
ManualResetEvent::reset () const
{
    DCMTK_LOG4CPLUS_THREADED (OFstatic_cast(impl::ManualResetEvent *, ev)->reset ());
}


//
//
//

DCMTK_LOG4CPLUS_INLINE_EXPORT
SharedMutexImplBase::~SharedMutexImplBase ()
{ }


//
//
//

DCMTK_LOG4CPLUS_INLINE_EXPORT
SharedMutex::SharedMutex ()
    : sm (DCMTK_LOG4CPLUS_THREADED (new impl::SharedMutex) + 0)
{ }


DCMTK_LOG4CPLUS_INLINE_EXPORT
SharedMutex::~SharedMutex ()
{ }


DCMTK_LOG4CPLUS_INLINE_EXPORT
void
SharedMutex::rdlock () const
{
    DCMTK_LOG4CPLUS_THREADED (OFstatic_cast(impl::SharedMutex *, sm)->rdlock ());
}


DCMTK_LOG4CPLUS_INLINE_EXPORT
void
SharedMutex::wrlock () const
{
    DCMTK_LOG4CPLUS_THREADED (OFstatic_cast(impl::SharedMutex *, sm)->wrlock ());
}


DCMTK_LOG4CPLUS_INLINE_EXPORT
void
SharedMutex::rdunlock () const
{
    DCMTK_LOG4CPLUS_THREADED (OFstatic_cast(impl::SharedMutex *, sm)->rdunlock ());
}


DCMTK_LOG4CPLUS_INLINE_EXPORT
void
SharedMutex::wrunlock () const
{
    DCMTK_LOG4CPLUS_THREADED (OFstatic_cast(impl::SharedMutex *, sm)->wrunlock ());
}


} } // namespace log4cplus { namespace thread { 
} // end namespace dcmtk

#endif // DCMTK_LOG4CPLUS_ENABLE_SYNCPRIMS_PUB_IMPL

#endif // DCMTK_LOG4CPLUS_THREAD_SYNCPRIMS_PUB_IMPL_H
