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
 *  Purpose: Define classes for Semaphore, Mutex and Read/Write Lock
 *           as used by most multithread implementations
 *
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2011-08-19 12:07:02 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofthread.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstd.h"

#define BAILOUT(msg) do { \
    OFCHECK_FAIL(msg); \
    return; \
} while (0)

static OFMutex *mutex=NULL;
static int mtx_var=0;
static int mtx_cond1=0;
static int mtx_cond2=0;
static int mtx_cond3=0;
static const int wait_timeout = 100;


class MutexT1: public OFThread
{
public:
  MutexT1(): OFThread() {}
  ~MutexT1() {}

  virtual void run()
  {
    if (OFMutex::busy == mutex->trylock()) mtx_cond1=1; // trylock works
    if (0 == mutex->lock())
    {
      mtx_var = 1;
      OFStandard::milliSleep(wait_timeout); // since I've got the mutex, nobody should write to mtx_var
      if ((mtx_var == 1)&&(0 == mutex->unlock())) mtx_cond2 = 1;
    }
  }
};

class MutexT2: public OFThread
{
public:
  MutexT2(): OFThread() {}
  ~MutexT2() {}

  virtual void run()
  {
    if (0 == mutex->lock())
    {
      mtx_var = 2;
      OFStandard::milliSleep(wait_timeout); // since I've got the mutex, nobody should write to mtx_var
      if ((mtx_var == 2)&&(0 == mutex->unlock())) mtx_cond3 = 1;
    }
  }
};

static void mutex_test()
{
  OFString errmsg;
  mutex = new OFMutex();
  if ((!mutex)||(! mutex->initialized())) BAILOUT("creation of mutex failed");
  int condition = mutex->trylock();
  if (condition)
  {
    mutex->errorstr(errmsg, condition);
    errmsg = "mutex lock failed: " + errmsg;
    BAILOUT(errmsg);
  }

  mtx_var = -1;

  MutexT1 t1;
  if (0 != t1.start()) BAILOUT("unable to create thread, mutex test failed");

  MutexT2 t2;
  if (0 != t2.start()) BAILOUT("unable to create thread, mutex test failed");

  OFStandard::milliSleep(wait_timeout); // since I've got the mutex, nobody should write to mtx_var
  if (mtx_var != -1) BAILOUT("mutex test failed");

  int i=0;
  while ((i++<5) && (!mtx_cond1)) OFStandard::milliSleep(wait_timeout);
  if (!mtx_cond1) BAILOUT("mutex trylock test failed");

  if (0 != (condition = mutex->unlock()))
  {
    mutex->errorstr(errmsg, condition);
    errmsg =  "mutex unlock failed: " + errmsg;
    BAILOUT(errmsg);
  }

  while ((i++<5) && ((!mtx_cond2)||(!mtx_cond3))) OFStandard::milliSleep(wait_timeout);
  if ((!mtx_cond2) || (!mtx_cond3)) BAILOUT("mutex lock/unlock test failed");

  delete mutex;
}

/* Currently OFSemaphore is not working and disabled on Mac OS X. Thus,
 * it is not tested on Mac OS X.
 */
#ifndef _DARWIN_C_SOURCE

static OFSemaphore *semaphore=NULL;
static int sem_cond1=0;
static int sem_cond2=0;
static int sem_cond3=0;
static int sem_cond4=0;

class SemaT1: public OFThread
{
public:
  SemaT1(): OFThread() {}
  ~SemaT1() {}

  virtual void run()
  {
    if (0 == semaphore->wait())
    {
      sem_cond1 = 1; // acquired semaphore
      mutex->lock();
      mutex->unlock();
      if (0== semaphore->post()) sem_cond2=1;
    }
  }
};

class SemaT2: public OFThread
{
public:
  SemaT2(): OFThread() {}
  ~SemaT2() {}

  virtual void run()
  {
    if (OFSemaphore::busy == semaphore->trywait())
    {
      if (0 == semaphore->wait()) // block on semaphore
      {
        sem_cond3 = 1; // acquired semaphore
        if (0== semaphore->post()) sem_cond4=1;
      }
    }
  }
};

static void semaphore_test()
{
  OFString errmsg;
  mutex = new OFMutex();
  if ((!mutex)||(! mutex->initialized())) BAILOUT("creation of mutex failed");
  semaphore = new OFSemaphore(2);
  if ((!semaphore)||(! semaphore->initialized())) BAILOUT("creation of semaphore failed");
  int condition = mutex->trylock();
  if (condition)
  {
    mutex->errorstr(errmsg, condition);
    errmsg = "mutex lock failed: " + errmsg;
    BAILOUT(errmsg);
  }
  condition = semaphore->trywait();
  if (condition)
  {
    semaphore->errorstr(errmsg, condition);
    errmsg = "semaphore acquisition failed: " + errmsg;
    BAILOUT(errmsg);
  }

  SemaT1 t1;
  if (0 != t1.start()) BAILOUT("unable to create thread, semaphore test failed");

  int i=0;
  while ((i++<5) && (!sem_cond1)) OFStandard::milliSleep(wait_timeout);
  if (!sem_cond1) BAILOUT("semaphore lock/unlock test failed");

  SemaT2 t2;
  if (0 != t2.start()) BAILOUT("unable to create thread, semaphore test failed");

  OFStandard::milliSleep(wait_timeout);
  if (sem_cond3) BAILOUT("semaphore lock/unlock test failed"); // make sure T2 is really blocked
  mutex->unlock();

  i=0;
  while ((i++<5) && ((!sem_cond2)||(!sem_cond3)||(!sem_cond4))) OFStandard::milliSleep(wait_timeout);
  if ((!mtx_cond2) || (!mtx_cond3) || (!sem_cond4)) BAILOUT("semaphore lock/unlock test failed");

  delete mutex;
  delete semaphore;
}

#endif //_DARWIN_C_SOURCE

static OFReadWriteLock *rwlock=NULL;
static OFMutex *mutex2=NULL;
static int rw_cond1=0;
static int rw_cond2=0;
static int rw_cond3=0;
static int rw_cond4=0;
static int rw_cond5=0;
static int rw_cond6=0;
static int rw_cond7=0;

class RWLockT1: public OFThread
{
public:
  RWLockT1(): OFThread() {}
  ~RWLockT1() {}

  virtual void run()
  {
    if (0 == rwlock->rdlock())
    {
      rw_cond1 = 1; // acquired read lock
      mutex->lock();
      mutex->unlock();
      if (0== rwlock->unlock()) rw_cond2=1;
      mutex2->lock();
      mutex2->unlock();
      if (OFReadWriteLock::busy == rwlock->tryrdlock()) rw_cond3=1;
      if ((0 == rwlock->rdlock())&&(0==rwlock->unlock())) rw_cond4=1;
    }
    return;
  }
};

class RWLockT2: public OFThread
{
public:
  RWLockT2(): OFThread() {}
  ~RWLockT2() {}

  virtual void run()
  {
    if ((0==mutex2->trylock())&&(OFReadWriteLock::busy == rwlock->trywrlock())) rw_cond5=1;
    if (0 == rwlock->wrlock())
    {
      rw_cond6=1;
      mutex2->unlock();
      OFStandard::milliSleep(wait_timeout);
      if (0==rwlock->unlock()) rw_cond7=1;
    }
    return;
  }
};

static void rwlock_test()
{
  OFString errmsg;

  mutex = new OFMutex();
  if ((!mutex)||(! mutex->initialized())) BAILOUT("creation of mutex failed");
  mutex2 = new OFMutex();
  if ((!mutex2)||(! mutex2->initialized())) BAILOUT("creation of mutex failed");
  rwlock = new OFReadWriteLock();
  if ((!rwlock)||(! rwlock->initialized())) BAILOUT("creation of read/write lock failed");

  int condition = mutex->trylock();
  if (condition)
  {
    mutex->errorstr(errmsg, condition);
    errmsg = "mutex lock failed: " + errmsg;
    BAILOUT(errmsg);
  }

  condition = rwlock->tryrdlock();
  if (condition)
  {
    rwlock->errorstr(errmsg, condition);
    errmsg = "read lock failed: " + errmsg;
    BAILOUT(errmsg);
  }

  RWLockT1 t1;
  if (0 != t1.start()) BAILOUT("unable to create thread, semaphore test failed");

  RWLockT2 t2;
  if (0 != t2.start()) BAILOUT("unable to create thread, semaphore test failed");


  int i=0;
  while ((i++<5) && ((!rw_cond1)||(!rw_cond5))) OFStandard::milliSleep(wait_timeout);

  if ((!rw_cond1)||(!rw_cond5)) BAILOUT("read/write lock/unlock test failed");
  condition = rwlock->unlock();
  if (condition)
  {
    rwlock->errorstr(errmsg, condition);
    errmsg = "read lock failed: " + errmsg;
    BAILOUT(errmsg);
  }
  OFStandard::milliSleep(wait_timeout);
  if (rw_cond6) BAILOUT("read/write lock test failed");

  mutex->unlock();

  i=0;
  while ((i++<5) && ((!rw_cond2)||(!rw_cond3)||(!rw_cond4)||(!rw_cond5)||(!rw_cond6)||(!rw_cond7))) OFStandard::milliSleep(wait_timeout);
  if ((!rw_cond2)||(!rw_cond3)||(!rw_cond4)||(!rw_cond5)||(!rw_cond6)||(!rw_cond7)) BAILOUT("read/write lock/unlock test failed");

  delete mutex;
  delete mutex2;
  delete rwlock;
}

class RWLockerT1: public OFThread
{
public:
  RWLockerT1(): OFThread() {}
  ~RWLockerT1() {}

  virtual void run()
  {
    OFReadWriteLocker locker(*rwlock);
    if (0 == locker.rdlock())
    {
      rw_cond1 = 1; // acquired read lock
      mutex->lock();
      mutex->unlock();
      if (0== locker.unlock()) rw_cond2=1;
      mutex2->lock();
      mutex2->unlock();
      if (OFReadWriteLock::busy == locker.tryrdlock()) rw_cond3=1;
      if (0 == locker.rdlock()) rw_cond4=1;
      // Implicit unlock() at the end
    }
    return;
  }
};

class RWLockerT2: public OFThread
{
public:
  RWLockerT2(): OFThread() {}
  ~RWLockerT2() {}

  virtual void run()
  {
    OFReadWriteLocker locker(*rwlock);
    if ((0==mutex2->trylock())&&(OFReadWriteLock::busy == locker.trywrlock())) rw_cond5=1;
    if (0 == locker.wrlock())
    {
      rw_cond6=1;
      mutex2->unlock();
      OFStandard::milliSleep(wait_timeout);
      // Explicite unlock(), check if this causes one unlock() too much
      if (0==locker.unlock()) rw_cond7=1;
    }
    return;
  }
};

static void rwlocker_test()
{
  OFString errmsg;

  // Reset global state
  rwlock=NULL;
  mutex2=NULL;
  rw_cond1=0;
  rw_cond2=0;
  rw_cond3=0;
  rw_cond4=0;
  rw_cond5=0;
  rw_cond6=0;
  rw_cond7=0;

  mutex = new OFMutex();
  if ((!mutex)||(! mutex->initialized())) BAILOUT("creation of mutex failed");
  mutex2 = new OFMutex();
  if ((!mutex2)||(! mutex2->initialized())) BAILOUT("creation of mutex failed");
  rwlock = new OFReadWriteLock();
  if ((!rwlock)||(! rwlock->initialized())) BAILOUT("creation of read/write lock failed");

  int condition = mutex->trylock();
  if (condition)
  {
    mutex->errorstr(errmsg, condition);
    errmsg = "mutex lock failed: " + errmsg;
    BAILOUT(errmsg);
  }

  OFReadWriteLocker rwlockLocker(*rwlock);
  condition = rwlockLocker.tryrdlock();
  if (condition)
  {
    rwlock->errorstr(errmsg, condition);
    errmsg = "read lock failed: " + errmsg;
    BAILOUT(errmsg);
  }

  RWLockerT1 t1;
  if (0 != t1.start()) BAILOUT("unable to create thread, semaphore test failed");

  RWLockerT2 t2;
  if (0 != t2.start()) BAILOUT("unable to create thread, semaphore test failed");


  int i=0;
  while ((i++<5) && ((!rw_cond1)||(!rw_cond5))) OFStandard::milliSleep(wait_timeout);

  if ((!rw_cond1)||(!rw_cond5)) BAILOUT("read/write lock/unlock test failed");
  condition = rwlockLocker.unlock();
  if (condition)
  {
    rwlock->errorstr(errmsg, condition);
    errmsg = "read lock failed: " + errmsg;
    BAILOUT(errmsg);
  }
  OFStandard::milliSleep(wait_timeout);
  if (rw_cond6) BAILOUT("read/write lock test failed");

  mutex->unlock();

  i=0;
  while ((i++<5) && ((!rw_cond2)||(!rw_cond3)||(!rw_cond4)||(!rw_cond5)||(!rw_cond6)||(!rw_cond7))) OFStandard::milliSleep(wait_timeout);
  if ((!rw_cond2)||(!rw_cond3)||(!rw_cond4)||(!rw_cond5)||(!rw_cond6)||(!rw_cond7)) BAILOUT("read/write lock/unlock test failed");

  delete mutex;
  delete mutex2;
  delete rwlock;
}


static OFThreadSpecificData *tsdata=NULL;
static int tsd_cond1=0;
static int tsd_cond2=0;
static int tsd_cond3=0;
static int tsd_cond4=0;

class TSDataT1: public OFThread
{
public:
  TSDataT1(): OFThread() {}
  ~TSDataT1() {}

  virtual void run()
  {
    if (0 == tsdata->set(this))
    {
      tsd_cond1 = 1; // acquired read lock
      mutex->lock();
      mutex->unlock();
      void *result = NULL;
      if (0== tsdata->get(result))
      {
        if (result == OFstatic_cast(void *, this)) tsd_cond3 = 1;
      }
    }
  }
};

class TSDataT2: public OFThread
{
public:
  TSDataT2(): OFThread() {}
  ~TSDataT2() {}

  virtual void run()
  {
    if (0 == tsdata->set(this))
    {
      tsd_cond2 = 1; // acquired read lock
      mutex2->lock();
      mutex2->unlock();
      void *result = NULL;
      if (0== tsdata->get(result))
      {
        if (result == OFstatic_cast(void *, this)) tsd_cond4 = 1;
      }
    }
  }
};


static void tsdata_test()
{
  OFString errmsg;

  mutex = new OFMutex();
  if ((!mutex)||(! mutex->initialized())) BAILOUT("creation of mutex failed");
  mutex2 = new OFMutex();
  if ((!mutex2)||(! mutex2->initialized())) BAILOUT("creation of mutex failed");
  tsdata = new OFThreadSpecificData();
  if ((!tsdata)||(! tsdata->initialized())) BAILOUT("creation of thread specific data failed");

  int condition = mutex->trylock();
  if (condition)
  {
    mutex->errorstr(errmsg, condition);
    errmsg = "mutex lock failed: " + errmsg;
    BAILOUT(errmsg);
  }
  condition = mutex2->trylock();
  if (condition)
  {
    mutex2->errorstr(errmsg, condition);
    errmsg = "mutex lock failed: " + errmsg;
    BAILOUT(errmsg);
  }

  TSDataT1 t1;
  if (0 != t1.start()) BAILOUT("unable to create thread, thread specific data test failed");

  TSDataT2 t2;
  if (0 != t2.start()) BAILOUT("unable to create thread, thread specific data test failed");


  int i=0;
  while ((i++<5) && ((!tsd_cond1)||(!tsd_cond2))) OFStandard::milliSleep(wait_timeout);

  if ((!tsd_cond1)||(!tsd_cond2)) BAILOUT("thread specific data write test failed");

  condition = mutex->unlock();
  if (condition)
  {
    mutex->errorstr(errmsg, condition);
    errmsg = "mutex unlock failed: " + errmsg;
    BAILOUT(errmsg);
  }
  condition = mutex2->unlock();
  if (condition)
  {
    mutex2->errorstr(errmsg, condition);
    errmsg = "mutex unlock failed: " + errmsg;
    BAILOUT(errmsg);
  }

  i=0;
  while ((i++<5) && ((!tsd_cond3)||(!tsd_cond4))) OFStandard::milliSleep(wait_timeout);
  if ((!tsd_cond3)||(!tsd_cond4)) BAILOUT("thread specific data read test failed");

  delete mutex;
  delete mutex2;
  delete tsdata;
}


OFTEST(ofstd_thread)
{
  // This makes sure tests are executed in the expected order
  mutex_test();
#ifndef _DARWIN_C_SOURCE  
  semaphore_test(); // may assume that mutexes work correctly
#endif
  rwlock_test();    // may assume that mutexes and semaphores work correctly
  rwlocker_test();  // may assume that mutexes, semaphores and read/write locks work correctly
  tsdata_test();
}


/*
 *
 * CVS/RCS Log:
 * $Log: tthread.cc,v $
 * Revision 1.3  2011-08-19 12:07:02  uli
 * Speed up the thread test by polling more often for events.
 *
 * Revision 1.2  2011-06-15 07:31:25  uli
 * Made sure thread 2 can't run before thread 1 in the semaphore test.
 *
 * Revision 1.1  2011-05-25 10:05:59  uli
 * Imported oftest and converted existing tests to oftest.
 *
 * Revision 1.15  2011-01-05 12:31:30  uli
 * Remove all windows line endings that were recently introduced.
 *
 * Revision 1.14  2011-01-05 11:03:45  onken
 * Updated thread test in order to reflect missing OFSemaphore class
 * on Mac OS X.
 *
 * Revision 1.13  2010-10-14 13:15:16  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.12  2010-06-04 13:58:42  uli
 * Added class OFReadWriteLocker which simplifies unlocking OFReadWriteLocks.
 *
 * Revision 1.11  2006-08-14 16:42:48  meichel
 * Updated all code in module ofstd to correctly compile if the standard
 *   namespace has not included into the global one with a "using" directive.
 *
 * Revision 1.10  2005/12/08 15:49:12  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.9  2004/01/16 10:37:23  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.8  2003/08/14 09:01:20  meichel
 * Adapted type casts to new-style typecast operators defined in ofcast.h
 *
 * Revision 1.7  2003/06/06 09:44:14  meichel
 * Added static sleep function in class OFStandard. This replaces the various
 *   calls to sleep(), Sleep() and usleep() throughout the toolkit.
 *
 * Revision 1.6  2002/04/16 13:37:02  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.5  2002/04/11 12:17:19  joergr
 * Removed obsolete return statements to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.4  2001/09/28 12:42:39  joergr
 * Replaced "cerr" by "CERR".
 *
 * Revision 1.3  2001/06/05 10:32:21  joergr
 * Replaced some #ifdef _WIN32 statements by #ifdef HAVE_WINDOWS_H or #ifdef
 * __CYGWIN__ respectively to reflect the fact that the latest Cygwin/gcc
 * version does not define _WIN32 any more.
 *
 * Revision 1.2  2001/06/01 15:51:42  meichel
 * Updated copyright header
 *
 * Revision 1.1  2000/03/29 16:41:27  meichel
 * Added new classes providing an operating system independent abstraction
 *   for threads, thread specific data, semaphores, mutexes and read/write locks.
 *
 *
 */
