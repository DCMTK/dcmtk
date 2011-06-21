// Module:  Log4CPLUS
// File:    thread-config.h
// Created: 4/2003
// Author:  Tad E. Smith
//
//
// Copyright 2003-2009 Tad E. Smith
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

#ifndef DCMTK_LOG4CPLUS_HELPERS_THREAD_CONFIG_HEADER_
#define DCMTK_LOG4CPLUS_HELPERS_THREAD_CONFIG_HEADER_

#define INCLUDE_CSTDDEF               /* For NULL */
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef DCMTK_LOG4CPLUS_USE_PTHREADS
#   include <pthread.h>
#   include <semaphore.h>
#   define DCMTK_LOG4CPLUS_MUTEX_PTR_DECLARE pthread_mutex_t*
#   define DCMTK_LOG4CPLUS_MUTEX_CREATE ::dcmtk::log4cplus::thread::createNewMutex()
#   define DCMTK_LOG4CPLUS_MUTEX_LOCK(mutex) pthread_mutex_lock(mutex)
#   define DCMTK_LOG4CPLUS_MUTEX_UNLOCK(mutex) pthread_mutex_unlock(mutex)
#   define DCMTK_LOG4CPLUS_MUTEX_FREE(mutex) ::dcmtk::log4cplus::thread::deleteMutex(mutex)
#   define DCMTK_LOG4CPLUS_THREAD_HANDLE_TYPE  pthread_t
#   define DCMTK_LOG4CPLUS_THREAD_KEY_TYPE pthread_t
#   define DCMTK_LOG4CPLUS_GET_CURRENT_THREAD_NAME \
    ::dcmtk::log4cplus::thread::getCurrentThreadName()
#   define DCMTK_LOG4CPLUS_GET_CURRENT_THREAD pthread_self()
#   define DCMTK_LOG4CPLUS_THREAD_LOCAL_TYPE pthread_key_t*
#   define DCMTK_LOG4CPLUS_THREAD_LOCAL_INIT(cleanup) \
    ::dcmtk::log4cplus::thread::createPthreadKey(cleanup)
#   define DCMTK_LOG4CPLUS_GET_THREAD_LOCAL_VALUE(key) pthread_getspecific(*(key))
#   define DCMTK_LOG4CPLUS_SET_THREAD_LOCAL_VALUE(key, value) \
    pthread_setspecific(*(key), value)
#   define DCMTK_LOG4CPLUS_THREAD_LOCAL_CLEANUP(key) \
    do { pthread_key_t * pthkey (key); pthread_key_delete(*pthkey); \
    delete pthkey; } while(0)
namespace dcmtk {
namespace log4cplus {
    namespace thread {
        DCMTK_LOG4CPLUS_EXPORT DCMTK_LOG4CPLUS_MUTEX_PTR_DECLARE createNewMutex();
        DCMTK_LOG4CPLUS_EXPORT void deleteMutex(DCMTK_LOG4CPLUS_MUTEX_PTR_DECLARE);
        DCMTK_LOG4CPLUS_EXPORT DCMTK_LOG4CPLUS_THREAD_LOCAL_TYPE createPthreadKey(void (*) (void *));
    }
}
}

#elif defined(DCMTK_LOG4CPLUS_USE_WIN32_THREADS)
#   undef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>

#   define DCMTK_LOG4CPLUS_MUTEX_PTR_DECLARE CRITICAL_SECTION*
#   define DCMTK_LOG4CPLUS_MUTEX_CREATE ::dcmtk::log4cplus::thread::createNewMutex()
#   define DCMTK_LOG4CPLUS_MUTEX_LOCK(mutex)  EnterCriticalSection(mutex)
#   define DCMTK_LOG4CPLUS_MUTEX_UNLOCK(mutex)  LeaveCriticalSection(mutex)
#   define DCMTK_LOG4CPLUS_MUTEX_FREE(mutex) ::dcmtk::log4cplus::thread::deleteMutex(mutex)

#   define DCMTK_LOG4CPLUS_THREAD_HANDLE_TYPE  HANDLE
#   define DCMTK_LOG4CPLUS_THREAD_KEY_TYPE  DWORD
#   define DCMTK_LOG4CPLUS_GET_CURRENT_THREAD  GetCurrentThreadId()
#   define DCMTK_LOG4CPLUS_GET_CURRENT_THREAD_NAME \
    ::dcmtk::log4cplus::thread::getCurrentThreadName()
#   define DCMTK_LOG4CPLUS_THREAD_LOCAL_TYPE DWORD
#   define DCMTK_LOG4CPLUS_THREAD_LOCAL_INIT(cleanup) TlsAlloc()
#   define DCMTK_LOG4CPLUS_GET_THREAD_LOCAL_VALUE(key) TlsGetValue(key)
#   define DCMTK_LOG4CPLUS_SET_THREAD_LOCAL_VALUE(key, value) \
    TlsSetValue(key, OFstatic_cast(LPVOID, value))
#   define DCMTK_LOG4CPLUS_THREAD_LOCAL_CLEANUP(key) TlsFree(key)
#   if defined (_MSC_VER)
#     undef DCMTK_LOG4CPLUS_HAVE_TLS_SUPPORT
#     undef DCMTK_LOG4CPLUS_THREAD_LOCAL_VAR
// Comment out the following two lines if you do intend to use log4cplus.dll
// for loading using LoadLibrary(). The __declspec(thread) functionality is not
// compatible with such DLL use. For more information why is this necessary see
// <http://msdn.microsoft.com/en-us/library/2s9wt68x(vs.80).aspx>.
#     define DCMTK_LOG4CPLUS_HAVE_TLS_SUPPORT 1
#     define DCMTK_LOG4CPLUS_THREAD_LOCAL_VAR __declspec(thread)
#   endif


namespace dcmtk {
namespace log4cplus { namespace thread {

DCMTK_LOG4CPLUS_EXPORT DCMTK_LOG4CPLUS_MUTEX_PTR_DECLARE createNewMutex();
DCMTK_LOG4CPLUS_EXPORT void deleteMutex(DCMTK_LOG4CPLUS_MUTEX_PTR_DECLARE);

} } } // namespace dcmtk { namespace log4cplus { namespace thread { /

#elif defined(DCMTK_LOG4CPLUS_SINGLE_THREADED)
#   define DCMTK_LOG4CPLUS_MUTEX_PTR_DECLARE int*
#   define DCMTK_LOG4CPLUS_MUTEX_CREATE NULL
#   define DCMTK_LOG4CPLUS_MUTEX_LOCK(mutex)
#   define DCMTK_LOG4CPLUS_MUTEX_UNLOCK(mutex)
#   define DCMTK_LOG4CPLUS_MUTEX_FREE(mutex)
#   define DCMTK_LOG4CPLUS_THREAD_HANDLE_TYPE  void *
#   define DCMTK_LOG4CPLUS_THREAD_KEY_TYPE int
#   define DCMTK_LOG4CPLUS_GET_CURRENT_THREAD 1
#   define DCMTK_LOG4CPLUS_GET_CURRENT_THREAD_NAME \
    DCMTK_LOG4CPLUS_C_STR_TO_TSTRING("single")
#   define DCMTK_LOG4CPLUS_THREAD_LOCAL_TYPE void*
#   define DCMTK_LOG4CPLUS_THREAD_LOCAL_INIT(cleanup) NULL
#   define DCMTK_LOG4CPLUS_GET_THREAD_LOCAL_VALUE(key) (key)
#   define DCMTK_LOG4CPLUS_SET_THREAD_LOCAL_VALUE(key, value) \
    do { (key) = (value); } while (0)
#   define DCMTK_LOG4CPLUS_THREAD_LOCAL_CLEANUP(key) do { (key) = NULL; } while (0)
#   undef DCMTK_LOG4CPLUS_HAVE_TLS_SUPPORT
#   undef DCMTK_LOG4CPLUS_THREAD_LOCAL_VAR

#else
#   error "You Must define a Threading model"
#endif

/**
 * @def DCMTK_LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX(mutex)
 * Begin a block synchronized on a mutex.
 *
 * @see DCMTK_LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX
 *
 * @def DCMTK_LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX
 * End a block synchronized on a mutex.
 *
 * @see DCMTK_LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX
 */
#ifndef DCMTK_LOG4CPLUS_SINGLE_THREADED
#  define DCMTK_LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX(mutex) \
             do { ::dcmtk::log4cplus::thread::Guard _sync_guard_object(mutex);
#else
#  define DCMTK_LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX(mutex) do { (void)(mutex);
#endif

#define DCMTK_LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX } while (0)


#endif // DCMTK_LOG4CPLUS_HELPERS_THREAD_CONFIG_HEADER_

