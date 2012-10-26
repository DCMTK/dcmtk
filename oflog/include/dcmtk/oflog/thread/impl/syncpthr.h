// -*- C++ -*-
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

//! @file
//! This file contains implementations of synchronization
//! primitives using the POSIX threads. It does not contain any
//! include guards because it is only a fragment to be included by
//! syncprims.h.

#include "dcmtk/oflog/thread/threads.h"
#include "dcmtk/ofstd/ofstream.h"
#include <limits>
#include <algorithm>


namespace dcmtk {
namespace log4cplus { namespace thread { namespace impl {


struct PthreadMutexAttr
{
    PthreadMutexAttr ()
        : attr()
    {
        int ret = pthread_mutexattr_init (&attr);
        if (ret != 0)
            DCMTK_LOG4CPLUS_THROW_RTE ("PthreadMutexAttr::PthreadMutexAttr");
    }


    ~PthreadMutexAttr ()
    {
        try
        {
            int ret = pthread_mutexattr_destroy (&attr);
            if (ret != 0)
                DCMTK_LOG4CPLUS_THROW_RTE ("PthreadMutexAttr::~PthreadMutexAttr");
        }
        catch (...)
        { }
    }


    void
    set_type (log4cplus::thread::Mutex::Type t)
    {
        int mutex_type;
        switch (t)
        {
        case log4cplus::thread::Mutex::RECURSIVE:
            mutex_type = PTHREAD_MUTEX_RECURSIVE;
            break;

        default:
            mutex_type = PTHREAD_MUTEX_DEFAULT;
        }

        int ret = pthread_mutexattr_settype (&attr, mutex_type);
        if (ret != 0)
            DCMTK_LOG4CPLUS_THROW_RTE ("PthreadMutexAttr::set_type");
    }


    pthread_mutexattr_t attr;
};


//
//
//

inline
Mutex::Mutex (log4cplus::thread::Mutex::Type t)
    : mtx()
{
    PthreadMutexAttr attr;
    attr.set_type (t);

    int ret = pthread_mutex_init (&mtx, &attr.attr);
    if (ret != 0)
        DCMTK_LOG4CPLUS_THROW_RTE ("Mutex::Mutex");
}


inline
Mutex::~Mutex ()
{
    try
    {
        int ret = pthread_mutex_destroy (&mtx);
        if (ret != 0)
            DCMTK_LOG4CPLUS_THROW_RTE ("Mutex::~Mutex");
    }
    catch (...)
    { }
}


inline
void
Mutex::lock () const
{
    int ret = pthread_mutex_lock (&mtx);
    if (ret != 0)
        DCMTK_LOG4CPLUS_THROW_RTE ("Mutex::lock");
}


inline
void
Mutex::unlock () const
{
    int ret = pthread_mutex_unlock (&mtx);
    if (ret != 0)
        DCMTK_LOG4CPLUS_THROW_RTE ("Mutex::unlock");
}


//
//
//

inline
Semaphore::Semaphore (unsigned max, unsigned initial)
    : sem()
{
    unsigned const sem_value_max =
#if defined (SEM_VALUE_MAX)
        SEM_VALUE_MAX
#else
        (STD_NAMESPACE numeric_limits<int>::max) ()
#endif
        ;

    unsigned const limited_max = (STD_NAMESPACE min) (max, sem_value_max);
    unsigned const limited_initial = (STD_NAMESPACE min) (initial, limited_max);
    int ret = 0;

#if defined (DCMTK_LOG4CPLUS_USE_NAMED_POSIX_SEMAPHORE)
    OFOStringStream oss;
    char size_check[2 * (OFstatic_cast(int, sizeof (STD_NAMESPACE ptrdiff_t))
                         - OFstatic_cast(int, sizeof (this))) + 1];
    (void)size_check;
    oss << getpid () << "-" << OFreinterpret_cast(STD_NAMESPACE ptrdiff_t, this);
    STD_NAMESPACE string name (oss.str ());

    sem = sem_open (name.c_str (), O_CREAT, S_IRWXU | S_IRWXG, limited_max);
    ret = sem == SEM_FAILED;
    if (ret != 0)
        DCMTK_LOG4CPLUS_THROW_RTE ("Semaphore::Semaphore");

    try
    {
        // Unlink the semaphore early to simulate anonymous semaphore.
        ret = sem_unlink (name.c_str ());
        if (ret != 0)
            DCMTK_LOG4CPLUS_THROW_RTE ("Semaphore::Semaphore");
    }
    catch (STD_NAMESPACE runtime_error const &)
    {
        ret = sem_close (sem);
        if (ret != 0)
            DCMTK_LOG4CPLUS_THROW_RTE ("Semaphore::~Semaphore");

        throw;
    }

#else
    ret = sem_init (&sem, 0, limited_max);
    if (ret != 0)
        DCMTK_LOG4CPLUS_THROW_RTE ("Semaphore::Semaphore");

#endif

    try
    {
        for (unsigned i = limited_initial; i < limited_max; ++i)
            lock ();
    }
    catch (STD_NAMESPACE runtime_error const &)
    {
#if defined (DCMTK_LOG4CPLUS_USE_NAMED_POSIX_SEMAPHORE)
        ret = sem_close (sem);
#else
        ret = sem_destroy (&sem);
#endif
        if (ret != 0)
            DCMTK_LOG4CPLUS_THROW_RTE ("Semaphore::~Semaphore");

        throw;
    }
}


inline
Semaphore::~Semaphore ()
{
    try
    {
        int ret = 0;
#if defined (DCMTK_LOG4CPLUS_USE_NAMED_POSIX_SEMAPHORE)
        ret = sem_close (sem);
#else
        ret = sem_destroy (&sem);
#endif
        if (ret != 0)
            DCMTK_LOG4CPLUS_THROW_RTE ("Semaphore::~Semaphore");
    }
    catch (...)
    { }
}


inline
void
Semaphore::unlock () const
{
#if defined (DCMTK_LOG4CPLUS_USE_NAMED_POSIX_SEMAPHORE)
    int ret = sem_post (sem);
#else
    int ret = sem_post (&sem);
#endif
    if (ret != 0)
        DCMTK_LOG4CPLUS_THROW_RTE ("Semaphore::unlock");
}


inline
void
Semaphore::lock () const
{
#if defined (DCMTK_LOG4CPLUS_USE_NAMED_POSIX_SEMAPHORE)
    int ret = sem_wait (sem);
#else
    int ret = sem_wait (&sem);
#endif
    if (ret != 0)
        DCMTK_LOG4CPLUS_THROW_RTE ("Semaphore::lock");
}


//
//
//


inline
FairMutex::FairMutex ()
    : sem (1, 1)
{ }


inline
FairMutex::~FairMutex ()
{ }


inline
void
FairMutex::lock () const
{
    sem.lock ();
}


inline
void
FairMutex::unlock () const
{
    sem.unlock ();
}


//
//
//

inline
ManualResetEvent::ManualResetEvent (bool sig)
    : cv ()
    , mtx (log4cplus::thread::Mutex::DEFAULT)
    , sigcount (0)
    , signaled (sig)
{
    int ret = pthread_cond_init (&cv, 0);
    if (ret != 0)
        DCMTK_LOG4CPLUS_THROW_RTE ("ManualResetEvent::ManualResetEvent");
}


inline
ManualResetEvent::~ManualResetEvent ()
{
    try
    {
        int ret = pthread_cond_destroy (&cv);
        if (ret != 0)
            DCMTK_LOG4CPLUS_THROW_RTE ("ManualResetEvent::~ManualResetEvent");
    }
    catch (...)
    { }
}


inline
void
ManualResetEvent::signal () const
{
    MutexGuard mguard (mtx);

    signaled = true;
    sigcount += 1;
    int ret = pthread_cond_broadcast (&cv);
    if (ret != 0)
        DCMTK_LOG4CPLUS_THROW_RTE ("ManualResetEVent::signal");

}


inline
void
ManualResetEvent::wait () const
{
    MutexGuard mguard (mtx);

    if (! signaled)
    {
        unsigned prev_count = sigcount;
        do
        {
            int ret = pthread_cond_wait (&cv, &mtx.mtx);
            if (ret != 0)
            {
                mguard.unlock ();
                mguard.detach ();
                DCMTK_LOG4CPLUS_THROW_RTE ("ManualResetEvent::wait");
            }
        }
        while (prev_count == sigcount);
    }
}


inline
bool
ManualResetEvent::timed_wait (unsigned long msec) const
{
    MutexGuard mguard (mtx);

    if (! signaled)
    {
        helpers::Time const wakeup_time (helpers::Time::gettimeofday ()
            + helpers::Time (msec / 1000, (msec % 1000) * 1000));
        struct timespec const ts = {wakeup_time.sec (),
            wakeup_time.usec () * 1000};
        unsigned prev_count = sigcount;
        do
        {
            int ret = pthread_cond_timedwait (&cv, &mtx.mtx, &ts);
            switch (ret)
            {
            case 0:
                break;

            case ETIMEDOUT:
                return false;

            default:
                mguard.unlock ();
                mguard.detach ();
                DCMTK_LOG4CPLUS_THROW_RTE ("ManualResetEvent::timed_wait");
            }
        }
        while (prev_count == sigcount);
    }

    return true;
}


inline
void
ManualResetEvent::reset () const
{
    MutexGuard mguard (mtx);

    signaled = false;
}


//
//
//

#if defined (DCMTK_LOG4CPLUS_POOR_MANS_SHAREDMUTEX)
#include "dcmtk/ofstd/thread/impl/syncpmsm.h"

#else
inline
SharedMutex::SharedMutex ()
    : rwl()
{
    int ret = pthread_rwlock_init (&rwl, 0);
    if (ret != 0)
        DCMTK_LOG4CPLUS_THROW_RTE ("SharedMutex::SharedMutex");
}


inline
SharedMutex::~SharedMutex ()
{
    try
    {
        int ret = pthread_rwlock_destroy (&rwl);
        if (ret != 0)
            DCMTK_LOG4CPLUS_THROW_RTE ("SharedMutex::~SharedMutex");
    }
    catch (...)
    { }
}


inline
void
SharedMutex::rdlock () const
{
    int ret;

    do
    {
        ret = pthread_rwlock_rdlock (&rwl);
        switch (ret)
        {
        case EAGAIN:
            // The read lock could not be acquired because the maximum
            // number of read locks for rwlock has been exceeded.

            log4cplus::thread::yield ();
            // Fall through.

        case 0:
            break;

        default:
            DCMTK_LOG4CPLUS_THROW_RTE ("SharedMutex::rdlock");

        }
    }
    while (ret != 0);
}


inline
void
SharedMutex::rdunlock () const
{
    unlock ();
}


inline
void
SharedMutex::wrlock () const
{
    int ret = pthread_rwlock_wrlock (&rwl);
    if (ret != 0)
        DCMTK_LOG4CPLUS_THROW_RTE ("SharedMutex::wrlock");
}


inline
void
SharedMutex::wrunlock () const
{
    unlock ();
}


inline
void
SharedMutex::unlock () const
{
    int ret = pthread_rwlock_unlock (&rwl);
    if (ret != 0)
        DCMTK_LOG4CPLUS_THROW_RTE ("SharedMutex::unlock");

}


#endif


} } } // namespace log4cplus { namespace thread { namespace impl {
} // end namespace dcmtk
