/*
 *
 *  Copyright (C) 2000-2017, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Provides operating system independent abstractions for basic
 *           multi-thread concepts: threads, thread specific data,
 *           semaphores, mutexes and read/write locks. The implementation
 *           of these classes supports the Solaris, POSIX and Win32
 *           multi-thread APIs.
 *
 */

#include "dcmtk/config/osconfig.h"

/* if WITH_THREADS is undefined, we don't even attempt to implement a thread interface. */
#ifdef WITH_THREADS

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#define INCLUDE_CERRNO
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstd.h"

#ifdef _DARWIN_C_SOURCE
#define DARWIN_INTERFACE
extern "C" {
#include <dispatch/dispatch.h>
}
#endif /* _DARWIN_C_SOURCE */

#ifdef HAVE_WINDOWS_H
#define WINDOWS_INTERFACE

extern "C" {
#include <windows.h>
#include <process.h>
}

#elif defined(HAVE_THREAD_H) && defined(HAVE_SYNCH_H)
#define SOLARIS_INTERFACE
extern "C" {
#include <thread.h>
#include <synch.h>      /* for read/write locks, semaphores and mutexes */
}

#elif defined(HAVE_PTHREAD_H) && defined(HAVE_SEMAPHORE_H)
#define POSIX_INTERFACE
#if !defined(HAVE_PTHREAD_RWLOCK) || defined(__ANDROID__)
// not all POSIX pthread implementations provide read/write locks
// the android implementation seems to be insufficient
#define POSIX_INTERFACE_WITHOUT_RWLOCK
#endif
extern "C" {
#include <pthread.h>    /* for threads, read/write locks and mutexes*/
#include <semaphore.h>  /* for semaphores */
}
#endif

#endif /* WITH_THREADS */

#include "dcmtk/ofstd/ofthread.h"
#include "dcmtk/ofstd/ofconsol.h"
#include "dcmtk/ofstd/ofstring.h"

// The Posix interfaces are not always correctly declared as volatile,
// so we need a two-step cast from our internal representation
// as a volatile void * to the Posix representation such as pthread_key_t *

#define OFthread_cast(x,y) OFreinterpret_cast(x, OFconst_cast(void *, y))

/* ------------------------------------------------------------------------- */

/* Thread stub function - called by the thread creation system function.
 * Calls the run() method of the OFString class and returns.
 */


#ifdef HAVE_WINDOWS_H
unsigned int __stdcall thread_stub(void *arg)
{
  OFstatic_cast(OFThread *, arg)->run();
  return 0; // thread terminates
}
#else
void *thread_stub(void *arg)
{
  OFstatic_cast(OFThread *, arg)->run();
  return NULL; // thread terminates
}
#endif

/* ------------------------------------------------------------------------- */

#ifdef WINDOWS_INTERFACE
  const int OFThread::busy = -1;
#elif defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE)
  const int OFThread::busy = ESRCH;
#else
  const int OFThread::busy = -2;
#endif

OFThread::OFThread()
#ifdef WINDOWS_INTERFACE
: theThreadHandle(0)
, theThread(0)
#else
: theThread(0)
#endif
{
}

OFThread::~OFThread()
{
#ifdef WINDOWS_INTERFACE
  CloseHandle(OFreinterpret_cast(HANDLE, theThreadHandle));
#endif
}

int OFThread::start()
{
#ifdef WINDOWS_INTERFACE
  unsigned int tid = 0;
  theThreadHandle = _beginthreadex(NULL, 0, thread_stub, OFstatic_cast(void *, this), 0, &tid);
  if (theThreadHandle == 0) return errno; else
  {
#ifdef HAVE_POINTER_TYPE_PTHREAD_T
    theThread = OFreinterpret_cast(void *, tid);
#else
    theThread = tid;
#endif
    return 0;
  }
#elif defined(POSIX_INTERFACE)
  pthread_t tid = 0;
  int result = pthread_create(&tid, NULL, thread_stub, OFstatic_cast(void *, this));
#ifdef HAVE_POINTER_TYPE_PTHREAD_T
  if (0 == result) theThread = OFstatic_cast(void *, tid); else theThread = 0;
#else
  if (0 == result) theThread = OFstatic_cast(unsigned long, tid); else theThread = 0;
#endif
  return result;
#elif defined(SOLARIS_INTERFACE)
  thread_t tid=0;
  int result = thr_create(NULL, 0, thread_stub, OFstatic_cast(void *, this), 0, &tid);
  if (0 == result) theThread = OFstatic_cast(unsigned long, tid); else theThread = 0;
  return result;
#else
  return -1;
#endif
}

int OFThread::join()
{
#ifdef WINDOWS_INTERFACE
  if (WaitForSingleObject(OFreinterpret_cast(HANDLE, theThreadHandle), INFINITE) == WAIT_OBJECT_0) return 0;
  else return OFstatic_cast(int, GetLastError());
#elif defined(POSIX_INTERFACE)
  void *retcode=NULL;
  return pthread_join(OFstatic_cast(pthread_t, theThread), &retcode);
#elif defined(SOLARIS_INTERFACE)
  void *retcode=NULL;
  return thr_join(OFstatic_cast(thread_t, theThread), NULL, &retcode);
#else
  return -1;
#endif
}

unsigned long OFThread::threadID()
{
#ifdef HAVE_POINTER_TYPE_PTHREAD_T
  // dangerous - we cast a pointer type to unsigned long and hope that it
  // remains valid after casting back to a pointer type.
  return OFreinterpret_cast(unsigned long, theThread);
#else
  return theThread;
#endif
}

#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE)
OFBool OFThread::equal(unsigned long tID)
#else
OFBool OFThread::equal(unsigned long /* tID */ )
#endif
{
#ifdef WINDOWS_INTERFACE
#ifdef HAVE_POINTER_TYPE_PTHREAD_T
  if (theThread == OFreinterpret_cast(void *, tID)) return OFTrue; else return OFFalse;
#else
  if (theThread == tID) return OFTrue; else return OFFalse;
#endif
#elif defined(POSIX_INTERFACE)
#ifdef HAVE_POINTER_TYPE_PTHREAD_T
  // dangerous - we cast an unsigned long back to a pointer type and hope that it is still valid
  if (pthread_equal(OFstatic_cast(pthread_t, theThread), OFreinterpret_cast(pthread_t, tID))) return OFTrue; else return OFFalse;
#else
  if (pthread_equal(OFstatic_cast(pthread_t, theThread), OFstatic_cast(pthread_t, tID))) return OFTrue; else return OFFalse;
#endif
#elif defined(SOLARIS_INTERFACE)
  if (OFstatic_cast(thread_t, theThread) == OFstatic_cast(thread_t, tID)) return OFTrue; else return OFFalse;
#else
  return 0;
#endif
}

void OFThread::thread_exit()
{
#ifdef WINDOWS_INTERFACE
  _endthreadex(0);
#elif defined(POSIX_INTERFACE)
  pthread_exit(NULL);
#elif defined(SOLARIS_INTERFACE)
  thr_exit(NULL);
#else
#endif
  return; // will never be reached
}

unsigned long OFThread::self()
{
#ifdef WINDOWS_INTERFACE
  return OFstatic_cast(unsigned long, GetCurrentThreadId());
#elif defined(POSIX_INTERFACE)
#ifdef HAVE_POINTER_TYPE_PTHREAD_T
  return OFreinterpret_cast(unsigned long, pthread_self());
#else
  return OFstatic_cast(unsigned long, pthread_self());
#endif
#elif defined(SOLARIS_INTERFACE)
  return OFstatic_cast(unsigned long, thr_self());
#else
  return 0;
#endif
}

#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE)
void OFThread::errorstr(OFString& description, int code)
#else
void OFThread::errorstr(OFString& description, int /* code */ )
#endif
{
#ifdef WINDOWS_INTERFACE
  if (code == OFThread::busy) description = "another thread already waiting for join"; else
  {
    LPVOID buf;
    FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, OFstatic_cast(DWORD, code), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      OFreinterpret_cast(LPTSTR, &buf), 0, NULL);
    if (buf) description = OFstatic_cast(const char *, buf);
    LocalFree(buf);
  }
#elif defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE)
  char buf[256];
  const char *str = OFStandard::strerror(code, buf, sizeof(buf));
  if (str) description = str; else description.clear();
#else
  description = "error: threads not implemented";
#endif
  return;
}

/* ------------------------------------------------------------------------- */


OFThreadSpecificData::OFThreadSpecificData()
: theKey(NULL)
{
#ifdef WINDOWS_INTERFACE
  DWORD *key = new DWORD;
  if (key)
  {
    *key = TlsAlloc();
    if (*key == OFstatic_cast(DWORD, -1)) delete key;
    else theKey=key;
  }
#elif defined(POSIX_INTERFACE)
  pthread_key_t *key = new pthread_key_t;
  if (key)
  {
    if (pthread_key_create(key, NULL)) delete key;
    else theKey=key;
  }
#elif defined(SOLARIS_INTERFACE)
  thread_key_t *key = new thread_key_t;
  if (key)
  {
    if (thr_keycreate(key, NULL)) delete key;
    else theKey=key;
  }
#else
#endif
}

OFThreadSpecificData::~OFThreadSpecificData()
{
#ifdef WINDOWS_INTERFACE
  if (theKey) TlsFree(* OFthread_cast(DWORD *, theKey));
#elif defined(POSIX_INTERFACE)
  delete OFthread_cast(pthread_key_t *, theKey);
#elif defined(SOLARIS_INTERFACE)
  delete OFthread_cast(thread_key_t *, theKey);
#else
#endif
  theKey = NULL;
}

OFBool OFThreadSpecificData::initialized() const
{
#ifdef WITH_THREADS
  if (theKey) return OFTrue; else return OFFalse;
#else
  return OFFalse; // thread specific data is not supported if we are working in a single-thread environment
#endif
}

#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE)
int OFThreadSpecificData::set(void *value)
#else
int OFThreadSpecificData::set(void * /* value */ )
#endif
{
#ifdef WINDOWS_INTERFACE
  if (theKey)
  {
    if (0 == TlsSetValue(*(OFthread_cast(DWORD *, theKey)), value)) return OFstatic_cast(int, GetLastError()); else return 0;
  } else return ERROR_INVALID_HANDLE;
#elif defined(POSIX_INTERFACE)
  if (theKey) return pthread_setspecific(* OFthread_cast(pthread_key_t *, theKey), value); else return EINVAL;
#elif defined(SOLARIS_INTERFACE)
  if (theKey) return thr_setspecific(* OFthread_cast(thread_key_t *, theKey), value); else return EINVAL;
#else
  return -1;
#endif
}

int OFThreadSpecificData::get(void *&value)
{
#ifdef WINDOWS_INTERFACE
  if (theKey)
  {
    value = TlsGetValue(*(OFthread_cast(DWORD *, theKey)));
    return 0;
  } else {
    value = NULL;
    return ERROR_INVALID_HANDLE;
  }
#elif defined(POSIX_INTERFACE)
  if (theKey)
  {
    value = pthread_getspecific(* OFthread_cast(pthread_key_t *, theKey));
    return 0;
  } else {
    value = NULL;
    return EINVAL;
  }
#elif defined(SOLARIS_INTERFACE)
  value = NULL;
  if (theKey) return thr_getspecific(* OFthread_cast(thread_key_t *, theKey), &value); else return EINVAL;
#else
  value = NULL;
  return -1;
#endif
}

#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE)
void OFThreadSpecificData::errorstr(OFString& description, int code)
#else
void OFThreadSpecificData::errorstr(OFString& description, int /* code */ )
#endif
{
#ifdef WINDOWS_INTERFACE
  LPVOID buf;
  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL, OFstatic_cast(DWORD, code), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    OFreinterpret_cast(LPTSTR, &buf), 0, NULL);
  if (buf) description = OFstatic_cast(const char *, buf);
  LocalFree(buf);
#elif defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE)
  char buf[256];
  const char *str = OFStandard::strerror(code, buf, sizeof(buf));
  if (str) description = str; else description.clear();
#else
  description = "error: thread specific data not implemented";
#endif
  return;
}

/* ------------------------------------------------------------------------- */

#ifdef WINDOWS_INTERFACE
  const int OFSemaphore::busy = -1;
#elif defined(DARWIN_INTERFACE)
  const int OFSemaphore::busy = EAGAIN;
#elif defined(POSIX_INTERFACE)
  const int OFSemaphore::busy = EAGAIN;  // Posix returns EAGAIN instead of EBUSY in trywait.
#elif defined(SOLARIS_INTERFACE)
  const int OFSemaphore::busy = EBUSY;
#else
  const int OFSemaphore::busy = -2;
#endif


#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE) || defined(DARWIN_INTERFACE)
OFSemaphore::OFSemaphore(unsigned int numResources)
#else
OFSemaphore::OFSemaphore(unsigned int /* numResources */ )
#endif
: theSemaphore(NULL)
{
#ifdef WINDOWS_INTERFACE
  theSemaphore = OFstatic_cast(void *, CreateSemaphore(NULL, numResources, numResources, NULL));
#elif defined(DARWIN_INTERFACE)
  dispatch_semaphore_t *sem = new dispatch_semaphore_t;
  if (sem)
  {
    *sem = dispatch_semaphore_create(numResources);
    if (*sem == NULL) delete sem;
    else theSemaphore = sem;
  }
#elif defined(POSIX_INTERFACE)
  sem_t *sem = new sem_t;
  if (sem)
  {
    if (sem_init(sem, 0, numResources) == -1) delete sem;
    else theSemaphore = sem;
  }
#elif defined(SOLARIS_INTERFACE)
  sema_t *sem = new sema_t;
  if (sem)
  {
    if (sema_init(sem, numResources, USYNC_THREAD, NULL)) delete sem;
    else theSemaphore = sem;
  }
#else
#endif
}

OFSemaphore::~OFSemaphore()
{
#ifdef WINDOWS_INTERFACE
  CloseHandle(OFthread_cast(HANDLE, theSemaphore));
#elif defined(DARWIN_INTERFACE)
  delete OFthread_cast(dispatch_semaphore_t *, theSemaphore);
#elif defined(POSIX_INTERFACE)
  if (theSemaphore) sem_destroy(OFthread_cast(sem_t *, theSemaphore));
  delete OFthread_cast(sem_t *, theSemaphore);
#elif defined(SOLARIS_INTERFACE)
  if (theSemaphore) sema_destroy(OFthread_cast(sema_t *, theSemaphore));
  delete OFthread_cast(sema_t *, theSemaphore);
#else
#endif
  theSemaphore = NULL;
}

OFBool OFSemaphore::initialized() const
{
#ifdef WITH_THREADS
  if (theSemaphore) return OFTrue; else return OFFalse;
#else
  return OFTrue;
#endif
}

int OFSemaphore::wait()
{
#ifdef WINDOWS_INTERFACE
  if (WaitForSingleObject(OFthread_cast(HANDLE, theSemaphore), INFINITE) == WAIT_OBJECT_0) return 0;
  else return OFstatic_cast(int, GetLastError());
#elif defined(DARWIN_INTERFACE)
  if (theSemaphore)
  {
    // Always succeeds (returns zero) if the timeout is DISPATCH_TIME_FOREVER.
    return dispatch_semaphore_wait(*OFthread_cast(dispatch_semaphore_t *, theSemaphore), DISPATCH_TIME_FOREVER);
  } else return EINVAL;
#elif defined(POSIX_INTERFACE)
  if (theSemaphore)
  {
    if (sem_wait(OFthread_cast(sem_t *, theSemaphore))) return errno; else return 0;
  } else return EINVAL;
#elif defined(SOLARIS_INTERFACE)
  if (theSemaphore) return sema_wait(OFthread_cast(sema_t *, theSemaphore)); else return EINVAL;
#else
  return -1;
#endif
}

int OFSemaphore::trywait()
{
#ifdef WINDOWS_INTERFACE
  DWORD result = WaitForSingleObject(OFthread_cast(HANDLE, theSemaphore), 0);
  if (result == WAIT_OBJECT_0) return 0;
  else if (result == WAIT_TIMEOUT) return OFSemaphore::busy;
  else return OFstatic_cast(int, GetLastError());
#elif defined(DARWIN_INTERFACE)
  if (theSemaphore)
  {
    if (dispatch_semaphore_wait(*OFthread_cast(dispatch_semaphore_t *, theSemaphore), DISPATCH_TIME_NOW) != 0)
      return EAGAIN;
    else
      return 0;
  } else return EINVAL;
#elif defined(POSIX_INTERFACE)
  if (theSemaphore)
  {
    if (sem_trywait(OFthread_cast(sem_t *, theSemaphore))) return errno; else return 0; // may return EAGAIN
  } else return EINVAL;
#elif defined(SOLARIS_INTERFACE)
  if (theSemaphore) return sema_trywait(OFthread_cast(sema_t *, theSemaphore)); else return EINVAL; // may return EBUSY
#else
  return -1;
#endif
}

int OFSemaphore::post()
{
#ifdef WINDOWS_INTERFACE
  if (ReleaseSemaphore(OFthread_cast(HANDLE, theSemaphore), 1, NULL)) return 0; else return OFstatic_cast(int, GetLastError());
#elif defined(DARWIN_INTERFACE)
  if (theSemaphore)
  {
    // Always succeeds.
    dispatch_semaphore_signal(*OFthread_cast(dispatch_semaphore_t *, theSemaphore));
    return 0;
  } else return EINVAL;
#elif defined(POSIX_INTERFACE)
  if (theSemaphore)
  {
    if (sem_post(OFthread_cast(sem_t *, theSemaphore))) return errno; else return 0;
  } else return EINVAL;
#elif defined(SOLARIS_INTERFACE)
  if (theSemaphore) return sema_post(OFthread_cast(sema_t *, theSemaphore)); else return EINVAL;
#else
  return -1;
#endif
}

#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE) || defined(DARWIN_INTERFACE)
void OFSemaphore::errorstr(OFString& description, int code)
#else
void OFSemaphore::errorstr(OFString& description, int /* code */ )
#endif
{
#ifdef WINDOWS_INTERFACE
  if (code == OFSemaphore::busy) description = "semaphore is already locked"; else
  {
    LPVOID buf;
    FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, OFstatic_cast(DWORD, code), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      OFreinterpret_cast(LPTSTR, &buf), 0, NULL);
    if (buf) description = OFreinterpret_cast(const char *, buf);
    LocalFree(buf);
  }
#elif defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE) || defined(DARWIN_INTERFACE)
  char buf[256];
  const char *str = OFStandard::strerror(code, buf, sizeof(buf));
  if (str) description = str; else description.clear();
#else
  description = "error: semaphore not implemented";
#endif
  return;
}

/* ------------------------------------------------------------------------- */


#ifdef WINDOWS_INTERFACE
  const int OFMutex::busy = -1;
#elif defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE)
  const int OFMutex::busy = EBUSY;
#else
  const int OFMutex::busy = -2;
#endif

OFMutex::OFMutex()
: theMutex(NULL)
{
#ifdef WINDOWS_INTERFACE
#ifdef USE_WIN32_CREATE_MUTEX
  theMutex = OFstatic_cast(void *, CreateMutex(NULL, FALSE, NULL));
#else
  CRITICAL_SECTION *critSec = new CRITICAL_SECTION;
  InitializeCriticalSection(critSec);
  theMutex = OFstatic_cast(void *, critSec);
#endif
#elif defined(POSIX_INTERFACE)
  pthread_mutex_t *mtx = new pthread_mutex_t;
  if (mtx)
  {
    if (pthread_mutex_init(mtx, NULL)) delete mtx;
    else theMutex = mtx;
  }
#elif defined(SOLARIS_INTERFACE)
  mutex_t *mtx = new mutex_t;
  if (mtx)
  {
    if (mutex_init(mtx, USYNC_THREAD, NULL)) delete mtx;
    else theMutex = mtx;
  }
#else
#endif
}


OFMutex::~OFMutex()
{
#ifdef WINDOWS_INTERFACE
#ifdef USE_WIN32_CREATE_MUTEX
  CloseHandle(OFthread_cast(HANDLE, theMutex));
#else
  CRITICAL_SECTION *critSec = OFthread_cast(CRITICAL_SECTION *, theMutex);
  DeleteCriticalSection(critSec);
  delete critSec;
#endif
#elif defined(POSIX_INTERFACE)
  if (theMutex) pthread_mutex_destroy(OFthread_cast(pthread_mutex_t *, theMutex));
  delete OFthread_cast(pthread_mutex_t *, theMutex);
#elif defined(SOLARIS_INTERFACE)
  if (theMutex) mutex_destroy(OFthread_cast(mutex_t *, theMutex));
  delete OFthread_cast(mutex_t *, theMutex);
#else
#endif
  theMutex = NULL;
}


OFBool OFMutex::initialized() const
{
#ifdef WITH_THREADS
  if (theMutex) return OFTrue; else return OFFalse;
#else
  return OFTrue;
#endif
}


int OFMutex::lock()
{
#ifdef WINDOWS_INTERFACE
#ifdef USE_WIN32_CREATE_MUTEX
  if (WaitForSingleObject(OFthread_cast(HANDLE, theMutex), INFINITE) == WAIT_OBJECT_0) return 0;
  else return OFstatic_cast(int, GetLastError());
#else
  EnterCriticalSection(OFthread_cast(CRITICAL_SECTION *, theMutex));
  return 0;
#endif
#elif defined(POSIX_INTERFACE)
  if (theMutex) return pthread_mutex_lock(OFthread_cast(pthread_mutex_t *, theMutex)); else return EINVAL;
#elif defined(SOLARIS_INTERFACE)
  if (theMutex) return mutex_lock(OFthread_cast(mutex_t *, theMutex)); else return EINVAL;
#else
  return -1;
#endif
}

int OFMutex::trylock()
{
#ifdef WINDOWS_INTERFACE
#ifdef USE_WIN32_CREATE_MUTEX
  DWORD result = WaitForSingleObject(OFthread_cast(HANDLE, theMutex), 0);
  if (result == WAIT_OBJECT_0) return 0;
  else if (result == WAIT_TIMEOUT) return OFMutex::busy;
  else return OFstatic_cast(int, GetLastError());
#else
  if (TryEnterCriticalSection(OFthread_cast(CRITICAL_SECTION *, theMutex))) return 0;
  else return OFMutex::busy;
#endif
#elif defined(POSIX_INTERFACE)
  if (theMutex) return pthread_mutex_trylock(OFthread_cast(pthread_mutex_t *, theMutex)); else return EINVAL; // may return EBUSY.
#elif defined(SOLARIS_INTERFACE)
  if (theMutex) return mutex_trylock(OFthread_cast(mutex_t *, theMutex)); else return EINVAL; // may return EBUSY.
#else
  return -1;
#endif
}

int OFMutex::unlock()
{
#ifdef WINDOWS_INTERFACE
#ifdef USE_WIN32_CREATE_MUTEX
  if (ReleaseMutex(OFthread_cast(HANDLE, theMutex))) return 0; else return OFstatic_cast(int, GetLastError());
#else
  LeaveCriticalSection(OFthread_cast(CRITICAL_SECTION *, theMutex));
  return 0;
#endif
#elif defined(POSIX_INTERFACE)
  if (theMutex) return pthread_mutex_unlock(OFthread_cast(pthread_mutex_t *, theMutex)); else return EINVAL;
#elif defined(SOLARIS_INTERFACE)
  if (theMutex) return mutex_unlock(OFthread_cast(mutex_t *, theMutex)); else return EINVAL;
#else
  return -1;
#endif
}

#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE)
void OFMutex::errorstr(OFString& description, int code)
#else
void OFMutex::errorstr(OFString& description, int /* code */ )
#endif
{
#ifdef WINDOWS_INTERFACE
  if (code == OFMutex::busy) description = "mutex is already locked"; else
  {
    LPVOID buf;
    FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, OFstatic_cast(DWORD, code), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      OFreinterpret_cast(LPTSTR, &buf), 0, NULL);
    if (buf) description = OFreinterpret_cast(const char *, buf);
    LocalFree(buf);
  }
#elif defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE)
  char buf[256];
  const char *str = OFStandard::strerror(code, buf, sizeof(buf));
  if (str) description = str; else description.clear();
#else
  description = "error: mutex not implemented";
#endif
  return;
}


/* ------------------------------------------------------------------------- */

#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE_WITHOUT_RWLOCK)

class OFReadWriteLockHelper
{
public:
  OFReadWriteLockHelper(): accessMutex(), usageSemaphore(1), numReaders(0) {}
  ~OFReadWriteLockHelper() {}

  OFMutex accessMutex;
  OFSemaphore usageSemaphore;
  int numReaders;
private:
  /* undefined */ OFReadWriteLockHelper(const OFReadWriteLockHelper& arg);
  /* undefined */ OFReadWriteLockHelper& operator=(const OFReadWriteLockHelper& arg);
};

#endif

/* ------------------------------------------------------------------------- */


#if defined(WINDOWS_INTERFACE)
  const int OFReadWriteLock::busy = -1;
#elif defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE)
  const int OFReadWriteLock::busy = EBUSY;
#else
  const int OFReadWriteLock::busy = -2;
#endif

OFReadWriteLock::OFReadWriteLock()
: theLock(NULL)
{
#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE_WITHOUT_RWLOCK)
   OFReadWriteLockHelper *rwl = new OFReadWriteLockHelper();
   if ((rwl->accessMutex.initialized()) && (rwl->usageSemaphore.initialized())) theLock=rwl;
   else delete rwl;
#elif defined(POSIX_INTERFACE)
  pthread_rwlock_t *rwl = new pthread_rwlock_t;
  if (rwl)
  {
    if (pthread_rwlock_init(rwl, NULL)) delete rwl;
    else theLock = rwl;
  }
#elif defined(SOLARIS_INTERFACE)
  rwlock_t *rwl = new rwlock_t;
  if (rwl)
  {
    if (rwlock_init(rwl, USYNC_THREAD, NULL)) delete rwl;
    else theLock = rwl;
  }
#else
#endif
}

OFReadWriteLock::~OFReadWriteLock()
{
#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE_WITHOUT_RWLOCK)
  delete OFthread_cast(OFReadWriteLockHelper *, theLock);
#elif defined(POSIX_INTERFACE)
  if (theLock) pthread_rwlock_destroy(OFthread_cast(pthread_rwlock_t *, theLock));
  delete OFthread_cast(pthread_rwlock_t *, theLock);
#elif defined(SOLARIS_INTERFACE)
  if (theLock) rwlock_destroy(OFthread_cast(rwlock_t *, theLock));
  delete OFthread_cast(rwlock_t *, theLock);
#else
#endif
  theLock = NULL;
}

OFBool OFReadWriteLock::initialized() const
{
#ifdef WITH_THREADS
  if (theLock) return OFTrue; else return OFFalse;
#else
  return OFTrue;
#endif
}

int OFReadWriteLock::rdlock()
{
#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE_WITHOUT_RWLOCK)
  if (theLock)
  {
    OFReadWriteLockHelper *rwl = OFthread_cast(OFReadWriteLockHelper *, theLock);
    int result =0;
    while (1)
    {
      if (0 != (result = rwl->accessMutex.lock())) return result; // lock mutex
      if (rwl->numReaders >= 0) // we can grant the read lock
      {
        if (rwl->numReaders == 0)
        {
          if (0 != (result = rwl->usageSemaphore.wait()))
          {
            rwl->accessMutex.unlock();
            return result;
          }
        }
        (rwl->numReaders)++;
        return rwl->accessMutex.unlock();
      }
      // we cannot grant the read lock, block thread.
      if (0 != (result = rwl->accessMutex.unlock())) return result;
      if (0 != (result = rwl->usageSemaphore.wait())) return result;
      if (0 != (result = rwl->usageSemaphore.post())) return result;
    }
  } else return EINVAL;
#elif defined(POSIX_INTERFACE)
  if (theLock) return pthread_rwlock_rdlock(OFthread_cast(pthread_rwlock_t *, theLock)); else return EINVAL;
#elif defined(SOLARIS_INTERFACE)
  if (theLock) return rw_rdlock(OFthread_cast(rwlock_t *, theLock)); else return EINVAL;
#else
  return -1;
#endif
}

int OFReadWriteLock::wrlock()
{
#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE_WITHOUT_RWLOCK)
  if (theLock)
  {
    OFReadWriteLockHelper *rwl = OFthread_cast(OFReadWriteLockHelper *, theLock);
    int result =0;
    while (1)
    {
      if (0 != (result = rwl->accessMutex.lock())) return result; // lock mutex
      if (rwl->numReaders == 0) // we can grant the write lock
      {
        if (0 != (result = rwl->usageSemaphore.wait()))
        {
          rwl->accessMutex.unlock();
          return result;
        }
        rwl->numReaders = -1;
        return rwl->accessMutex.unlock();
      }

      // we cannot grant the write lock, block thread.
      if (0 != (result = rwl->accessMutex.unlock())) return result;
      if (0 != (result = rwl->usageSemaphore.wait())) return result;
      if (0 != (result = rwl->usageSemaphore.post())) return result;
    }
  } else return EINVAL;
#elif defined(POSIX_INTERFACE)
  if (theLock) return pthread_rwlock_wrlock(OFthread_cast(pthread_rwlock_t *, theLock)); else return EINVAL;
#elif defined(SOLARIS_INTERFACE)
  if (theLock) return rw_wrlock(OFthread_cast(rwlock_t *, theLock)); else return EINVAL;
#else
  return -1;
#endif
}


int OFReadWriteLock::tryrdlock()
{
#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE_WITHOUT_RWLOCK)
  if (theLock)
  {
    OFReadWriteLockHelper *rwl = OFthread_cast(OFReadWriteLockHelper *, theLock);
    int result =0;
    if (0 != (result = rwl->accessMutex.lock())) return result; // lock mutex
    if (rwl->numReaders >= 0) // we can grant the read lock
    {
      if (rwl->numReaders == 0)
      {
        if (0 != (result = rwl->usageSemaphore.wait()))
        {
          rwl->accessMutex.unlock();
          return result;
        }
      }
      (rwl->numReaders)++;
      return rwl->accessMutex.unlock();
    }
    result = rwl->accessMutex.unlock();
    if (result) return result; else return OFReadWriteLock::busy;
  } else return EINVAL;
#elif defined(POSIX_INTERFACE)
  if (theLock) return pthread_rwlock_tryrdlock(OFthread_cast(pthread_rwlock_t *, theLock)); else return EINVAL; // may return EBUSY.
#elif defined(SOLARIS_INTERFACE)
  if (theLock) return rw_tryrdlock(OFthread_cast(rwlock_t *, theLock)); else return EINVAL; // may return EBUSY.
#else
  return -1;
#endif
}

int OFReadWriteLock::trywrlock()
{
#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE_WITHOUT_RWLOCK)
  if (theLock)
  {
    OFReadWriteLockHelper *rwl = OFthread_cast(OFReadWriteLockHelper *, theLock);
    int result =0;
    if (0 != (result = rwl->accessMutex.lock())) return result; // lock mutex
    if (rwl->numReaders == 0) // we can grant the write lock
    {
      if (0 != (result = rwl->usageSemaphore.wait()))
      {
        rwl->accessMutex.unlock();
        return result;
      }
      rwl->numReaders = -1;
      return rwl->accessMutex.unlock();
    }
    result = rwl->accessMutex.unlock();
    if (result) return result; else return OFReadWriteLock::busy;
  } else return EINVAL;
#elif defined(POSIX_INTERFACE)
  if (theLock) return pthread_rwlock_trywrlock(OFthread_cast(pthread_rwlock_t *, theLock)); else return EINVAL; // may return EBUSY.
#elif defined(SOLARIS_INTERFACE)
  if (theLock) return rw_trywrlock(OFthread_cast(rwlock_t *, theLock)); else return EINVAL; // may return EBUSY.
#else
  return -1;
#endif
}


int OFReadWriteLock::unlock()
{
#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE_WITHOUT_RWLOCK)
  if (theLock)
  {
    OFReadWriteLockHelper *rwl = OFthread_cast(OFReadWriteLockHelper *, theLock);
    int result =0;
    if (0 != (result = rwl->accessMutex.lock())) return result; // lock mutex
    if (rwl->numReaders == -1) rwl->numReaders = 0; else (rwl->numReaders)--;
    if ((rwl->numReaders == 0) && (0 != (result = rwl->usageSemaphore.post())))
    {
      rwl->accessMutex.unlock();
      return result;
    }
    return rwl->accessMutex.unlock();
  } else return EINVAL;
#elif defined(POSIX_INTERFACE)
  if (theLock) return pthread_rwlock_unlock(OFthread_cast(pthread_rwlock_t *, theLock)); else return EINVAL;
#elif defined(SOLARIS_INTERFACE)
  if (theLock) return rw_unlock(OFthread_cast(rwlock_t *, theLock)); else return EINVAL;
#else
  return -1;
#endif
}

#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE)
void OFReadWriteLock::errorstr(OFString& description, int code)
#else
void OFReadWriteLock::errorstr(OFString& description, int /* code */ )
#endif
{
#ifdef WINDOWS_INTERFACE
  if (code == OFSemaphore::busy) description = "read/write lock is already locked"; else
  {
    LPVOID buf;
    FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, OFstatic_cast(DWORD, code), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      OFreinterpret_cast(LPTSTR, &buf), 0, NULL);
    if (buf) description = OFreinterpret_cast(const char *, buf);
    LocalFree(buf);
  }
#elif defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE)
  char buf[256];
  const char *str = OFStandard::strerror(code, buf, sizeof(buf));
  if (str) description = str; else description.clear();
#else
  description = "error: read/write lock not implemented";
#endif
  return;
}


OFReadWriteLocker::OFReadWriteLocker(OFReadWriteLock& lock)
    : theLock(lock), locked(OFFalse)
{
}

OFReadWriteLocker::~OFReadWriteLocker()
{
  if (locked)
    theLock.unlock();
}

#ifdef DEBUG
#define lockWarn(name, locked) \
  if (locked == OFTrue)        \
  {                            \
    ofConsole.lockCout() << "OFReadWriteLocker::" name "(): Already locked?!" << OFendl; \
    ofConsole.unlockCout();    \
  }
#else
#define lockWarn(name, locked)
#endif

#define OFReadWriteLockerFunction(name) \
int OFReadWriteLocker::name()           \
{                                       \
  lockWarn(#name, locked);              \
  int ret = theLock. name ();           \
  if (ret == 0)                         \
    locked = OFTrue;                    \
  return ret;                           \
}

OFReadWriteLockerFunction(rdlock)
OFReadWriteLockerFunction(wrlock)
OFReadWriteLockerFunction(tryrdlock)
OFReadWriteLockerFunction(trywrlock)

int OFReadWriteLocker::unlock()
{
#ifdef DEBUG
  if (locked == OFFalse)
  {
    ofConsole.lockCout() << "OFReadWriteLocker::unlock(): Nothing to unlock?!" << OFendl;
    ofConsole.unlockCout();
  }
#endif

  int ret = theLock.unlock();
  if (ret == 0)
    locked = OFFalse;
  return ret;
}
