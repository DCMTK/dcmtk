// Module:  Log4CPLUS
// File:    threads.cxx
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

#include "dcmtk/oflog/config.h"

#include <exception>
#include <ostream>
#include <cerrno>

#ifdef DCMTK_LOG4CPLUS_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef DCMTK_LOG4CPLUS_HAVE_SYS_SYSCALL_H
#include <sys/syscall.h>
#endif

#ifdef DCMTK_LOG4CPLUS_HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef DCMTK_LOG4CPLUS_HAVE_UNISTD_H
#include <unistd.h>
#endif

#if defined(DCMTK_LOG4CPLUS_USE_PTHREADS)
#  include <pthread.h>
#  include <sched.h>
#  include <signal.h>
#elif defined (DCMTK_LOG4CPLUS_USE_WIN32_THREADS)
#  include <process.h>
#endif
#include "dcmtk/oflog/config/windowsh.h"
#include "dcmtk/oflog/thread/syncpub.h"
#include "dcmtk/oflog/tstring.h"
#include "dcmtk/oflog/internal/cygwin32.h"
#include "dcmtk/oflog/streams.h"

#ifndef DCMTK_LOG4CPLUS_SINGLE_THREADED

#include "dcmtk/oflog/thread/threads.h"
#include "dcmtk/oflog/thread/impl/thredimp.h"
#include "dcmtk/oflog/thread/impl/tls.h"
#include "dcmtk/oflog/ndc.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/helpers/strhelp.h"
#include "dcmtk/oflog/helpers/timehelp.h"
#include "dcmtk/oflog/internal/internal.h"

#endif // DCMTK_LOG4CPLUS_SINGLE_THREADED


namespace dcmtk {
namespace log4cplus { namespace thread {

DCMTK_LOG4CPLUS_EXPORT
void
blockAllSignals()
{
#if defined (DCMTK_LOG4CPLUS_USE_PTHREADS)
    // Block all signals.
    sigset_t signal_set;
    sigfillset (&signal_set);
    pthread_sigmask (SIG_BLOCK, &signal_set, 0);
#endif
}


DCMTK_LOG4CPLUS_EXPORT
void
yield()
{
#if defined(DCMTK_LOG4CPLUS_USE_PTHREADS)
    sched_yield();
#elif defined(_WIN32)
#if !defined(_MSC_VER) || _MSC_VER > 1200 /* MSC 6 doesn't know this */
    if (! SwitchToThread ())
#endif
        Sleep (0);
#endif
}


DCMTK_LOG4CPLUS_EXPORT
log4cplus::tstring const &
getCurrentThreadName()
{
#if ! defined (DCMTK_LOG4CPLUS_SINGLE_THREADED)
    log4cplus::tstring & name = log4cplus::internal::get_thread_name_str ();
    if (name.empty ())
    {
        log4cplus::tostringstream tmp;
        tmp << impl::getCurrentThreadId ();
        // tmp.str ().swap (name);
        name = OFString(tmp.str().c_str(), tmp.str().length());
    }
#else
    static log4cplus::tstring const name (DCMTK_LOG4CPLUS_TEXT ("single"));
#endif

    return name;
}


#ifndef DCMTK_LOG4CPLUS_SINGLE_THREADED

namespace
{

static
bool
get_current_thread_name_alt (log4cplus::tostream * s)
{
    log4cplus::tostream & os = *s;

#if defined (DCMTK_LOG4CPLUS_USE_PTHREADS) && defined (__linux__) \
    && defined (DCMTK_LOG4CPLUS_HAVE_GETTID)
    pid_t tid = OFstatic_cast(pid_t, syscall (SYS_gettid));
    os << tid;

#elif defined (__CYGWIN__)
    unsigned long tid = cygwin::get_current_win32_thread_id ();
    os << tid;

#else
    os << getCurrentThreadName ();

#endif

    return true;
}


} // namespace

#endif // DCMTK_LOG4CPLUS_SINGLE_THREADED


DCMTK_LOG4CPLUS_EXPORT
log4cplus::tstring const &
getCurrentThreadName2()
{
#if ! defined (DCMTK_LOG4CPLUS_SINGLE_THREADED)
    log4cplus::tstring & name = log4cplus::internal::get_thread_name2_str ();
    if (name.empty ())
    {
        log4cplus::tostringstream tmp;
        get_current_thread_name_alt (&tmp);
        name = OFString(tmp.str().c_str(), tmp.str().length());
    }

#else
    static log4cplus::tstring const name (getCurrentThreadName ());

#endif

    return name;
}


} } // namespace log4cplus { namespace thread {
} // end namespace dcmtk


#ifndef DCMTK_LOG4CPLUS_SINGLE_THREADED

namespace
{

#  ifdef DCMTK_LOG4CPLUS_USE_PTHREADS
extern "C" void* threadStartFunc(void * param)
#  elif defined(DCMTK_LOG4CPLUS_USE_WIN32_THREADS)
static unsigned WINAPI threadStartFunc(void * param)
#  endif
{
    return dcmtk::log4cplus::thread::impl::ThreadStart::threadStartFuncWorker (param);
}

} // namespace


namespace dcmtk {
namespace log4cplus { namespace thread { namespace impl {


#if defined(DCMTK_LOG4CPLUS_USE_PTHREADS)
void*
ThreadStart::threadStartFuncWorker(void * arg)
#elif defined(DCMTK_LOG4CPLUS_USE_WIN32_THREADS)
unsigned
ThreadStart::threadStartFuncWorker(void * arg)
#endif
{
    blockAllSignals ();
    helpers::LogLog * loglog = helpers::LogLog::getLogLog();
    if (! arg)
        loglog->error(DCMTK_LOG4CPLUS_TEXT("threadStartFunc()- arg is NULL"));
    else
    {
        Thread * ptr = OFstatic_cast(Thread *, arg);
        ThreadPtr thread(ptr);

        // Decrease reference count increased by Thread::start().
        ptr->removeReference ();

        try
        {
            thread->run();
        }
        catch(STD_NAMESPACE exception const & e)
        {
            tstring err = DCMTK_LOG4CPLUS_TEXT("threadStartFunc()- run() terminated with an exception: ");
            err += DCMTK_LOG4CPLUS_C_STR_TO_TSTRING(e.what());
            loglog->warn(err);
        }
        catch(...)
        {
            loglog->warn(DCMTK_LOG4CPLUS_TEXT("threadStartFunc()- run() terminated with an exception."));
        }

        thread::MutexGuard guard (thread->access_mutex);
        thread->flags &= ~Thread::fRUNNING;
    }

    threadCleanup ();

    return 0;
}


Thread::Thread()
    : flags (0)
#if defined(DCMTK_LOG4CPLUS_USE_WIN32_THREADS)
    , handle (INVALID_HANDLE_VALUE)
    , thread_id (0)
#else
    , handle ()
#endif
{
}


Thread::~Thread()
{
#if defined(DCMTK_LOG4CPLUS_USE_PTHREADS)
    if ((flags & fJOINED) == 0)
        pthread_detach (handle);

#elif defined(DCMTK_LOG4CPLUS_USE_WIN32_THREADS)
    if (handle != INVALID_HANDLE_VALUE)
        ::CloseHandle (handle);

#endif
}


void
Thread::start()
{
    flags |= fRUNNING;

    // Increase reference count here. It will be lowered by the running
    // thread itself.
    addReference ();

#if defined(DCMTK_LOG4CPLUS_USE_PTHREADS)
    if (pthread_create(&handle, NULL, threadStartFunc, this) )
    {
        removeReference ();
        flags &= ~fRUNNING;
        log4cplus::helpers::LogLog::getLogLog ()->error (
            DCMTK_LOG4CPLUS_TEXT ("Thread creation was not successful"), true);
    }
#elif defined(DCMTK_LOG4CPLUS_USE_WIN32_THREADS)
    HANDLE h = handle;
    handle = INVALID_HANDLE_VALUE;
    if (h != INVALID_HANDLE_VALUE)
        ::CloseHandle (h);

    h = OFreinterpret_cast(HANDLE,
        ::_beginthreadex (0, 0, threadStartFunc, this, 0, &thread_id));
    handle = h;
    if (! h)
    {
        removeReference ();
        flags &= ~fRUNNING;
        log4cplus::helpers::LogLog::getLogLog ()->error (
            DCMTK_LOG4CPLUS_TEXT ("Thread creation was not successful"), true);
    }
#endif
}


bool
Thread::isRunning() const
{
    thread::MutexGuard guard (access_mutex);
    return (flags & fRUNNING) != 0;
}


os_id_type
Thread::getThreadId () const
{
#if defined(DCMTK_LOG4CPLUS_USE_PTHREADS)
    return handle;
#elif defined(DCMTK_LOG4CPLUS_USE_WIN32_THREADS)
    return thread_id;
#endif
}


os_handle_type
Thread::getThreadHandle () const
{
    return handle;
}


void
Thread::join ()
{
#if defined(DCMTK_LOG4CPLUS_USE_PTHREADS)
    pthread_join (handle, 0);
#elif defined(DCMTK_LOG4CPLUS_USE_WIN32_THREADS)
    ::WaitForSingleObject (handle, INFINITE);
#endif
    flags |= fJOINED;
}


} // namespace impl {


//
//
//

ThreadImplBase::~ThreadImplBase ()
{ }


//
//
//


namespace
{


class ThreadImpl
    : public impl::Thread
{
public:
    ThreadImpl ()
        : abs_thread (0)
    { }

    virtual ~ThreadImpl ()
    { }

    virtual
    void
    run ()
    {
        abs_thread->run ();
    }

    void
    set_abs_thread (AbstractThread * at)
    {
        abs_thread = at;
    }

protected:
    AbstractThread * abs_thread;

private:
    ThreadImpl(const ThreadImpl&);
    ThreadImpl& operator=(const ThreadImpl&);
};


} // namespace


//
//
//

AbstractThread::AbstractThread ()
    : thread (new ThreadImpl)
{
    OFstatic_cast(ThreadImpl *, thread.get ())->set_abs_thread (this);
}


bool
AbstractThread::isRunning() const
{
    return OFstatic_cast(ThreadImpl *, thread.get ())->isRunning ();
}


void
AbstractThread::start()
{
    OFstatic_cast(ThreadImpl *, thread.get ())->start ();
}


void
AbstractThread::join () const
{
    OFstatic_cast(ThreadImpl *, thread.get ())->join ();
}


AbstractThread::~AbstractThread()
{ }


} } // namespace log4cplus { namespace thread {
} // end namespace dcmtk


#endif // DCMTK_LOG4CPLUS_SINGLE_THREADED
