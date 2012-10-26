// -*- C++ -*-
// Module:  Log4CPLUS
// File:    threads.h
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

/** @file */

#ifndef DCMTK_LOG4CPLUS_IMPL_THREADS_IMPL_HEADER_
#define DCMTK_LOG4CPLUS_IMPL_THREADS_IMPL_HEADER_

#include "dcmtk/oflog/config.h"

#if defined (DCMTK_LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#if defined (_WIN32)
#include "dcmtk/oflog/config/windowsh.h"
#endif
#include "dcmtk/oflog/tstring.h"
#include "dcmtk/oflog/helpers/pointer.h"
#include "dcmtk/oflog/thread/threads.h"

#if ! defined (DCMTK_INSIDE_LOG4CPLUS)
#  error "This header must not be be used outside log4cplus' implementation files."
#endif


namespace dcmtk {
namespace log4cplus { namespace thread { namespace impl {


#if defined (DCMTK_LOG4CPLUS_USE_PTHREADS)

typedef pthread_t os_handle_type;
typedef pthread_t os_id_type;


inline
pthread_t
getCurrentThreadId ()
{
    return pthread_self ();
}


#elif defined (DCMTK_LOG4CPLUS_USE_WIN32_THREADS)

typedef HANDLE os_handle_type;
typedef DWORD os_id_type;


inline
DWORD
getCurrentThreadId ()
{
    return GetCurrentThreadId ();
}


#elif defined (DCMTK_LOG4CPLUS_SINGLE_THREADED)

typedef void * os_handle_type;
typedef int os_id_type;


inline
int
getCurrentThreadId ()
{
    return 1;
}


#endif


#ifndef DCMTK_LOG4CPLUS_SINGLE_THREADED


struct ThreadStart
{
#  ifdef DCMTK_LOG4CPLUS_USE_PTHREADS
static void* threadStartFuncWorker(void *);
#  elif defined(DCMTK_LOG4CPLUS_USE_WIN32_THREADS)
static unsigned threadStartFuncWorker(void *);
#  endif
};


/**
 * There are many cross-platform C++ Threading libraries.  The goal of
 * this class is not to replace (or match in functionality) those
 * libraries.  The goal of this class is to provide a simple Threading
 * class with basic functionality.
 */
class Thread
    : public ThreadImplBase
{
public:
    Thread();
    bool isRunning() const;
    os_id_type getThreadId() const;
    os_handle_type getThreadHandle () const;
    void start();
    void join ();

protected:
    // Force objects to be constructed on the heap
    virtual ~Thread();
    virtual void run() = 0;

private:
    // Friends.
    friend struct ThreadStart;

    enum Flags
    {
        fRUNNING  = 0x01,
        fJOINED   = 0x02
    };

    unsigned flags;

    os_handle_type handle;

#  if defined(DCMTK_LOG4CPLUS_USE_WIN32_THREADS)
    unsigned thread_id;
#  endif

    // Disallow copying of instances of this class.
    Thread(const Thread&);
    Thread& operator=(const Thread&);
};

typedef helpers::SharedObjectPtr<Thread> ThreadPtr;


#endif // DCMTK_LOG4CPLUS_SINGLE_THREADED


} } } // namespace log4cplus { namespace thread { namespace impl {
} // end namespace dcmtk


#endif // DCMTK_LOG4CPLUS_IMPL_THREADS_IMPL_HEADER_
