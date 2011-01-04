/*
 *
 *  Copyright (C) 2000-2011, OFFIS e.V.
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
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2011-01-04 14:47:11 $
 *  CVS/RCS Revision: $Revision: 1.22 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
#ifndef HAVE_PTHREAD_RWLOCK
// not all POSIX pthread implementations provide read/write locks
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
  CloseHandle((HANDLE)theThreadHandle);
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
  if (WaitForSingleObject((HANDLE)theThreadHandle, INFINITE) == WAIT_OBJECT_0) return 0;
  else return (int)GetLastError();
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
      NULL, (DWORD)code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &buf, 0, NULL);
    if (buf) description = (const char *)buf;
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
    if (*key == (DWORD) -1) delete key;
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
    if (0 == TlsSetValue(*((DWORD *)theKey), value)) return (int)GetLastError(); else return 0;
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
    value = TlsGetValue(*((DWORD *)theKey));
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
    NULL, (DWORD)code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &buf, 0, NULL);
  if (buf) description = (const char *)buf;
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

/* Mac OS X only permits named Semaphores. The code below compiles on Mac OS X
   but does not work. This will be corrected in the next snapshot. For now, the
   semaphore code is completely disabled for that OS (it is not used in other
   parts of the toolkit so far.
 */
#ifndef _DARWIN_C_SOURCE

#ifdef WINDOWS_INTERFACE
  const int OFSemaphore::busy = -1;
#elif defined(POSIX_INTERFACE)
  const int OFSemaphore::busy = EAGAIN;  // Posix returns EAGAIN instead of EBUSY in trywait.
#elif defined(SOLARIS_INTERFACE)
  const int OFSemaphore::busy = EBUSY;
#else
  const int OFSemaphore::busy = -2;
#endif


#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE)
OFSemaphore::OFSemaphore(unsigned int numResources)
#else
OFSemaphore::OFSemaphore(unsigned int /* numResources */ )
#endif
: theSemaphore(NULL)
{
#ifdef WINDOWS_INTERFACE
  theSemaphore = OFstatic_cast(void *, CreateSemaphore(NULL, numResources, numResources, NULL));
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
  CloseHandle((HANDLE)theSemaphore);
#elif defined(POSIX_INTERFACE)
  if (theSemaphore) sem_destroy(OFthread_cast(sem_t *, theSemaphore));
  delete OFthread_cast(sem_t *, theSemaphore);
#elif defined(SOLARIS_INTERFACE)
  if (theSemaphore) sema_destroy(OFthread_cast(sema_t *, theSemaphore));
  delete OFthread_cast(sema_t *, theSemaphore);
#else
#endif
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
  if (WaitForSingleObject((HANDLE)theSemaphore, INFINITE) == WAIT_OBJECT_0) return 0;
  else return (int)GetLastError();
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
  DWORD result = WaitForSingleObject((HANDLE)theSemaphore, 0);
  if (result == WAIT_OBJECT_0) return 0;
  else if (result == WAIT_TIMEOUT) return OFSemaphore::busy;
  else return (int)GetLastError();
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
  if (ReleaseSemaphore((HANDLE)theSemaphore, 1, NULL)) return 0; else return (int)GetLastError();
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

#if defined(WINDOWS_INTERFACE) || defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE)
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
      NULL, (DWORD)code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &buf, 0, NULL);
    if (buf) description = (const char *)buf;
    LocalFree(buf);
  }
#elif defined(POSIX_INTERFACE) || defined(SOLARIS_INTERFACE)
  char buf[256];
  const char *str = OFStandard::strerror(code, buf, sizeof(buf));
  if (str) description = str; else description.clear();
#else
  description = "error: semaphore not implemented";
#endif
  return;
}

#endif // _DARWIN_C_SOURCE

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
  theMutex = OFstatic_cast(void *, CreateMutex(NULL, FALSE, NULL));
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
  CloseHandle((HANDLE)theMutex);
#elif defined(POSIX_INTERFACE)
  if (theMutex) pthread_mutex_destroy(OFthread_cast(pthread_mutex_t *, theMutex));
  delete OFthread_cast(pthread_mutex_t *, theMutex);
#elif defined(SOLARIS_INTERFACE)
  if (theMutex) mutex_destroy(OFthread_cast(mutex_t *, theMutex));
  delete OFthread_cast(mutex_t *, theMutex);
#else
#endif
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
  if (WaitForSingleObject((HANDLE)theMutex, INFINITE) == WAIT_OBJECT_0) return 0;
  else return (int)GetLastError();
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
  DWORD result = WaitForSingleObject((HANDLE)theMutex, 0);
  if (result == WAIT_OBJECT_0) return 0;
  else if (result == WAIT_TIMEOUT) return OFMutex::busy;
  else return (int)GetLastError();
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
  if (ReleaseMutex((HANDLE)theMutex)) return 0; else return (int)GetLastError();
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
  if (code == OFSemaphore::busy) description = "mutex is already locked"; else
  {
    LPVOID buf;
    FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, (DWORD)code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &buf, 0, NULL);
    if (buf) description = (const char *)buf;
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
    OFReadWriteLockHelper *rwl = (OFReadWriteLockHelper *)theLock;
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
    OFReadWriteLockHelper *rwl = (OFReadWriteLockHelper *)theLock;
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
    OFReadWriteLockHelper *rwl = (OFReadWriteLockHelper *)theLock;
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
    OFReadWriteLockHelper *rwl = (OFReadWriteLockHelper *)theLock;
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
    OFReadWriteLockHelper *rwl = (OFReadWriteLockHelper *)theLock;
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
      NULL, (DWORD)code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &buf, 0, NULL);
    if (buf) description = (const char *)buf;
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

/*
 *
 * CVS/RCS Log:
 * $Log: ofthread.cc,v $
 * Revision 1.22  2011-01-04 14:47:11  onken
 * Disable and hide OFSemaphore class on Mac OS X since implementation is
 * broken on that OS (needs named semaphores instead).
 *
 * Revision 1.21  2010-10-14 13:14:53  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.20  2010-06-28 07:22:00  joergr
 * Introduced explicit type casts in order to compile with new gcc versions on
 * MinGW/MSYS. Use type cast macros (e.g. OFstatic_cast) where appropriate.
 *
 * Revision 1.19  2010-06-04 14:18:20  uli
 * Removed an outdated comment.
 *
 * Revision 1.18  2010-06-04 13:58:42  uli
 * Added class OFReadWriteLocker which simplifies unlocking OFReadWriteLocks.
 *
 * Revision 1.17  2010-06-03 10:27:04  joergr
 * Replaced calls to strerror() by new helper function OFStandard::strerror()
 * which results in using the thread safe version of strerror() if available.
 *
 * Revision 1.16  2005/12/08 15:49:02  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.15  2005/07/27 17:07:52  joergr
 * Fixed problem with pthread_t type cast and gcc 4.0.
 *
 * Revision 1.14  2004/08/03 16:44:17  meichel
 * Updated code to correctly handle pthread_t both as an integral integer type
 *   (e.g. Linux, Solaris) and as a pointer type (e.g. BSD, OSF/1).
 *
 * Revision 1.13  2004/04/22 10:45:33  joergr
 * Changed typecast from OFreinterpret_cast to OFstatic_cast to avoid compilation
 * error on Solaris with gcc 3.x.
 *
 * Revision 1.12  2003/12/02 16:20:12  meichel
 * Changed a few typecasts for static to reinterpret, required
 *   for NetBSD and OpenBSD
 *
 * Revision 1.11  2003/10/13 13:24:14  meichel
 * Changed order of include files, as a workaround for problem in Borland C++.
 *
 * Revision 1.10  2003/08/14 09:01:20  meichel
 * Adapted type casts to new-style typecast operators defined in ofcast.h
 *
 * Revision 1.9  2002/11/27 11:23:11  meichel
 * Adapted module ofstd to use of new header file ofstdinc.h
 *
 * Revision 1.8  2002/02/27 14:13:22  meichel
 * Changed initialized() methods to const. Fixed some return values when
 *   compiled without thread support.
 *
 * Revision 1.7  2001/06/01 15:51:40  meichel
 * Updated copyright header
 *
 * Revision 1.6  2001/01/17 13:03:29  meichel
 * Fixed problem that leaded to compile errors if compiled on Windows without
 *   multi-thread support.
 *
 * Revision 1.5  2000/12/19 12:18:19  meichel
 * thread related classes now correctly disabled when configured
 *   with --disable-threads
 *
 * Revision 1.4  2000/06/26 09:27:38  joergr
 * Replaced _WIN32 by HAVE_WINDOWS_H to avoid compiler errors using CygWin-32.
 *
 * Revision 1.3  2000/04/14 15:17:16  meichel
 * Adapted all ofstd library classes to consistently use ofConsole for output.
 *
 * Revision 1.2  2000/04/11 15:24:45  meichel
 * Changed debug output to COUT instead of CERR
 *
 * Revision 1.1  2000/03/29 16:41:26  meichel
 * Added new classes providing an operating system independent abstraction
 *   for threads, thread specific data, semaphores, mutexes and read/write locks.
 *
 *
 */
