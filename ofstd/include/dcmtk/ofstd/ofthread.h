/*
 *
 *  Copyright (C) 1997-2014, OFFIS e.V.
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


#ifndef OFTHREAD_H
#define OFTHREAD_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"  /* for class OFBool */
#include "dcmtk/ofstd/ofstring.h" /* for class OFString */

/** Stub function with extern "C" linkage that is passed as a function pointer
 *  to the system function that creates the thread.
 *  @param arg pointer to the OFThread instance to be started, passed as void pointer.
 */
extern "C"
{
#ifdef HAVE_WINDOWS_H
  unsigned int __stdcall thread_stub(void *arg);
#else
  void *thread_stub(void *arg);
#endif
}


/** provides an operating system independent abstraction for threads.
 *  Threads are executed asynchronously in parallel to the main thread
 *  of the process. On multi-processor machines threads may run on
 *  different CPUs if the operating system permits.
 *  This class is abstract. Deriving classes must implement the run()
 *  method which contains the code executed by the thread.
 */
class DCMTK_OFSTD_EXPORT OFThread
{
public:

  /** default constructor. The new thread is not started immediately
   *  upon creation of the OFThread object. Calling the start()
   *  method causes the creation of the thread.
   */
  OFThread();

  /** destructor. Destruction of an OFThread object does not cause
   *  the referenced thread to be stopped and may result in undefined
   *  behaviour if the derived class maintains thread specific data
   *  in this object (which is not recommended).
   *  The join() method should be called prior to destruction
   *  of the thread object to make sure a thread has terminated.
   */
  virtual ~OFThread();

  /** adds a new thread of control to the  current process. The main()
   *  procedure itself is a single thread of control. Each thread executes
   *  simultaneously  with  all  the other  threads  within  the  calling
   *  process. A newly created thread shares all of  the  calling  process'
   *  global data with the other threads in this process except the
   *  execution stack.   The new thread executes the run() method and
   *  terminates upon return from this method or a call to thread_exit()
   *  from within the thread. This method should not be called if a thread
   *  is already running, otherwise a new thread will be started and the
   *  identifier of the old thread will be overwritten, making it
   *  impossible to call join() for the old thread.  It may also result in
   *  undefined behaviour if the derived class maintains thread specific
   *  data in this object (which is not recommended).
   *  @return 0 upon success, an error code otherwise.
   */
  int start();

  /** blocks the calling thread until the thread referenced by the OFThread
   *  object terminates. Several threads cannot wait for the same thread to
   *  complete; one  thread  will  complete the join() method successfully
   *  others may or may not return OFThread::busy. The method will not
   *  block the calling thread if the target thread has already terminated.
   *  @return 0 upon success, OFThread::busy if another thread is already
   *    waiting for the termination of the target thread,
   *    an error code otherwise.
   */
  int join();

  /** returns the thread identifier of the thread referenced by the
   *  OFThread object, if the thread has already been started.
   *  Otherwise returns 0. On certain platforms like OSF/1, a thread ID
   *  contains a pointer to a structure. Therefore, thread IDs should
   *  never be compared directly, but always using the equal() method
   *  provided in this class.
   *  @return thread ID of target thread if started, 0 otherwise.
   */
  unsigned long threadID();

  /** checks if the given thread ID matches the thread ID of the thread
   *  referenced by this object.
   *  @param tID thread ID to be compared
   *  @return OFTrue if equal, OFFalse otherwise.
   */
  OFBool equal(unsigned long tID);

  /** converts any of the error codes returned by the methods of this class
   *  into a textual description, which is written into the string object.
   *  @param description string object into which the error description is written.
   *  @param code error code
   */
  static void errorstr(OFString& description, int code);

  /** this constant is returned by the join() method if another thread
   *  is already waiting for termination of the thread referenced by the
   *  OFThread object. Since this value is operating system dependent,
   *  comparisons should always compare the return value of join()
   *  with this constant.
   */
  static const int busy;

protected:

  /** terminates the calling thread, in a similar way that exit()
   *  terminates the calling process. This method does not return.
   */
  static void thread_exit();

  /** returns the thread ID of the calling thread. For a running thread,
   *  this->threadID() and this->self() should return the same value,
   *  but self() is more robust and should be preferred.
   *  On certain platforms like OSF/1, a thread ID
   *  contains a pointer to a structure. Therefore, thread IDs should
   *  never be compared directly, but always using the equal() method
   *  provided in this class.
   *  @return thread ID of the calling thread.
   */
  static unsigned long self();

private:

  /** this method implements the thread that is run by calling the start
   *  method of the OFThread object. Defined as abstract method in OFThread.
   *  It is recommended that derived classes implementing this method
   *  do not rely on attributes of the OFThread object because this might
   *  result in undefined behaviour if another thread deletes or restarts the
   *  OFThread object before the thread has terminated.
   */
  virtual void run() = 0;

#ifdef HAVE_WINDOWS_H
  /** thread handle (Win32 only) */
  OFuintptr_t theThreadHandle;
#endif

  /** thread identifier */
#ifdef HAVE_POINTER_TYPE_PTHREAD_T
  void *theThread;
#else
  unsigned long theThread;
#endif

  /** unimplemented private copy constructor */
  OFThread(const OFThread& arg);

  /** unimplemented private assignment operator */
  OFThread& operator=(const OFThread& arg);

  /** thread stub must be friend to call run() */
#ifdef HAVE_WINDOWS_H
  friend unsigned int __stdcall thread_stub(void *arg);
#else
  friend DCMTK_OFSTD_EXPORT void *thread_stub(void *arg);
#endif
};


/** provides an operating system independent abstraction for thread
  * specific data. An instance of this class manages a key which is global
  * to all threads in the process but locates data specific to each thread.
  * Each thread can set a different value once the object has been created.
  * Upon creation, the value NULL is assigned for all threads. This class
  * does not perform any memory management on the objects pointed to.
  * Threads must ensure on their own that the data pointed to by the thread
  * specific data key is freed upon termination of the thread.
  */
class DCMTK_OFSTD_EXPORT OFThreadSpecificData
{
public:

  /** default constructor */
  OFThreadSpecificData();

  /** destructor. Deletes all thread specific key values (pointers), but
   *  not the objects pointed to.
   */
  ~OFThreadSpecificData();

  /** checks whether creation of the object was successful.
   *  @return OFTrue if the object was successfully created, OFFalse otherwise.
   */
  OFBool initialized() const;

  /** sets the thread specific value for this object. No attempt is made to
   *  automatically delete the object pointed to at the termination of the
   *  thread.
   *  @param value new pointer to thread-specific data for this object.
   *  @return 0 if successful, an error code otherwise.
   */
  int set(void *value);

  /** retrieves the thread specific value for this object. If no call to set()
   *  has been made for the calling thread before, NULL is returned.
   *  @param value new pointer to thread-specific data for this object
   *    returned in this parameter.
   *  @return 0 if successful, an error code otherwise.
   */
  int get(void *&value);

  /** converts any of the error codes returned by the methods of this class
   *  into a textual description, which is written into the string object.
   *  @param description string object into which the error description is written.
   *  @param code error code
   */
  static void errorstr(OFString& description, int code);

private:

  /** thread specific data key resource */
#ifdef HAVE_CXX_VOLATILE
  volatile
#endif
  void *theKey;

  /** unimplemented private copy constructor */
  OFThreadSpecificData(const OFThreadSpecificData& arg);

  /** unimplemented private assignment operator */
  OFThreadSpecificData& operator=(const OFThreadSpecificData& arg);
};


/* Mac OS X only permits named Semaphores. The code below compiles on Mac OS X
   but does not work. This will be corrected in the next snapshot. For now, the
   semaphore code is completely disabled for that OS (it is not used in other
   parts of the toolkit so far.
 */
#ifndef _DARWIN_C_SOURCE

/** provides an operating system independent abstraction for semaphores.
 *  A semaphore is a non-negative integer counter that is used
 *  to coordinate access to resources. The initial and maximum value of the counter
 *  is defined by the constructor. Each call to wait() decreases
 *  the counter by one and each call to post() increases the count by one.
 *  If a thread calls wait() while the counter is zero, the thread
 *  is blocked until another thread has increased the counter using post().
 */
class DCMTK_OFSTD_EXPORT OFSemaphore
{
public:

  /** constructor.
   *  @param numResources is the initial and maximum value for the semaphore.
   */
  OFSemaphore(unsigned int numResources);

  /** destructor */
  ~OFSemaphore();

  /** checks whether creation of the object was successful.
   *  @return OFTrue if the object was successfully created, OFFalse otherwise.
   */
  OFBool initialized() const;

  /** blocks the calling thread until the semaphore counter is greater than zero
   *  and then atomically decreases the counter.
   *  @return 0 upon success, an error code otherwise.
   */
  int wait();

  /** atomically decreases the counter if it is larger than zero,
   *  otherwise returns OFSemaphore::busy.
   *  @return 0 upon success, OFSemaphore::busy if the semaphore is already locked,
   *    an error code otherwise.
   */
  int trywait();

  /** atomically increases the counter. If threads are blocked on the semaphore,
   *  at least one of them is unblocked.
   *  @return 0 upon success, an error code otherwise.
   */
  int post();

  /** converts any of the error codes returned by the methods of this class
   *  into a textual description, which is written into the string object.
   *  @param description string object into which the error description is written.
   *  @param code error code
   */
  static void errorstr(OFString& description, int code);

  /** this constant is returned by the trywait() method if the semaphore
   *  is already locked. Since this value is operating system dependent,
   *  comparisons should always compare the return value of trywait()
   *  with this constant.
   */
  static const int busy;

private:
  /** semaphore resource */
#ifdef HAVE_CXX_VOLATILE
  volatile
#endif
  void * theSemaphore;

  /** unimplemented private copy constructor */
  OFSemaphore(const OFSemaphore& arg);

  /** unimplemented private assignment operator */
  OFSemaphore& operator=(const OFSemaphore& arg);
};


#endif // _DARWIN_C_SOURCE

/** provides an operating system independent abstraction for mutexes
 *  (mutual exclusion locks).
 *  Mutexes prevent multiple threads from simultaneously executing critical
 *  sections of code which access shared data. A successful call for a
 *  mutex lock by way of lock() will cause another thread that is also
 *  trying to lock the same mutex to block until the owner thread unlocks
 *  it by way of unlock().
 */
class DCMTK_OFSTD_EXPORT OFMutex
{
public:

  /** default constructor */
  OFMutex();

  /** destructor */
  ~OFMutex();

  /** checks whether creation of the object was successful.
   *  @return OFTrue if the object was successfully created, OFFalse otherwise.
   */
  OFBool initialized() const;

  /** locks the mutex object. If the mutex is already locked, the calling
   *  thread blocks until the mutex is freed; If the current owner of a
   *  mutex tries to relock the mutex, it may or may not result in
   *  deadlock.
   *  @return 0 upon success, an error code otherwise.
   */
  int lock();

  /** tries to lock the mutex object. If the mutex is already locked,
   *  returns OFMutex::busy.
   *  @return 0 upon success, OFMutex::busy if the mutex is already locked,
   *    an error code otherwise.
   */
  int trylock();

  /** releases the lock on the mutex object. The mutex must be locked and
   *  the calling thread must be the owner of the lock, otherwise the
   *  behaviour is undefined. If there are threads blocked on the mutex
   *  when unlock() is called, one of them is unblocked and receives
   *  ownership of the mutex lock.
   *  @return 0 upon success, an error code otherwise.
   */
  int unlock();

  /** converts any of the error codes returned by the methods of this class
   *  into a textual description, which is written into the string object.
   *  @param description string object into which the error description is written.
   *  @param code error code
   */
  static void errorstr(OFString& description, int code);

  /** this constant is returned by the trylock() method if the mutex
   *  is already locked. Since this value is operating system dependent,
   *  comparisons should always compare the return value of trylock()
   *  with this constant.
   */
  static const int busy;

private:
  /** mutex resource */
#ifdef HAVE_CXX_VOLATILE
  volatile
#endif
  void * theMutex;

  /** unimplemented private copy constructor */
  OFMutex(const OFMutex& arg);

  /** unimplemented private assignment operator */
  OFMutex& operator=(const OFMutex& arg);
};


/** provides an operating system independent abstraction for read/write
 *  locks. Many threads can have simultaneous read-only access to data,
 *  while only one thread can have write access at any given time.
 *  Multiple read access with single write access is controlled by
 *  read/write locks, which are generally used to protect data that is
 *  frequently searched.
 */
class DCMTK_OFSTD_EXPORT OFReadWriteLock
{
public:

  /** default constructor */
  OFReadWriteLock();

  /** destructor */
  ~OFReadWriteLock();

  /** checks whether creation of the object was successful.
   *  @return OFTrue if the object was successfully created, OFFalse otherwise.
   */
  OFBool initialized() const;

  /** gets a read lock. If the read/write lock is currently locked for
   *  writing, the calling thread blocks until the write lock is freed.
   *  Multiple threads may simultaneously hold a read lock on a read/write
   *  lock.
   *  @return 0 upon success, an error code otherwise.
   */
  int rdlock();

  /** gets a write lock. If the read/write lock is currently locked for
   *  reading or writing, the calling thread blocks until all the read and
   *  write locks are freed. At any given time, only one thread may have a
   *  write lock on a read/write lock.
   *  @return 0 upon success, an error code otherwise.
   */
  int wrlock();

  /** trys to get a read lock. If the read/write lock is locked for
   *  writing, returns OFReadWriteLock::busy.
   *  @return 0 upon success, OFReadWriteLock::busy if the read/write lock
   *    is already locked, an error code otherwise.
   */
  int tryrdlock();

  /** trys to get a write lock. If the read/write lock is currently locked
   *  for reading or writing, returns OFReadWriteLock::busy.
   *  @return 0 upon success, OFReadWriteLock::busy if the read/write lock
   *    is already locked, an error code otherwise.
   */
  int trywrlock();

  /** unlocks the read/write lock. The read/write lock must be locked and
   *  the calling thread must be the owner of the lock, otherwise the
   *  behaviour is undefined. One of the other threads that is waiting for
   *  the read/write lock to be freed will be unblocked, provided there are
   *  other waiting threads.
   *  @return 0 upon success, an error code otherwise.
   */
  int unlock();

  /** converts any of the error codes returned by the methods of this class
   *  into a textual description, which is written into the string object.
   *  @param description string object into which the error description is written.
   *  @param code error code
   */
  static void errorstr(OFString& description, int code);

  /** this constant is returned by the tryrdlock() and trywrlock() methods
   *  if the read/write lock is already locked. Since this value is operating
   *  system dependent, comparisons should always compare the return value
   *  of tryrdlock() and trywrlock() with this constant.
   */
  static const int busy;

private:
  /** read/write lock resource */
#ifdef HAVE_CXX_VOLATILE
  volatile
#endif
  void * theLock;

  /** unimplemented private copy constructor */
  OFReadWriteLock(const OFReadWriteLock& arg);

  /** unimplemented private assignment operator */
  OFReadWriteLock& operator=(const OFReadWriteLock& arg);
};

/** This class aims to provide an easy way for making sure OFReadWriteLocks
 *  are unlocked in an exception safe way. You can just create a local
 *  instance of this class and lock the OFReadWriteLock through it. When it
 *  is destructed it will make sure that the lock is unlocked if necessary.
 */
class DCMTK_OFSTD_EXPORT OFReadWriteLocker {
public:
  /** constructor
   *  @param lock the lock to associate this instance with
   */
  OFReadWriteLocker(OFReadWriteLock& lock);

  /** destructor, unlocks the mutex if necessary */
  ~OFReadWriteLocker();

  /** lock the lock for reading
   *  @return 0 upon success, an error code otherwise
   *  @see OFReadWriteLock::rdlock
   */
  int rdlock();

  /** lock the lock for writing
   *  @return 0 upon success, an error code otherwise
   *  @see OFReadWriteLock::wrlock
   */
  int wrlock();

  /** try to lock the lock for reading
   *  @return 0 upon success, OFReadWriteLock::busy if the read/write lock
   *    is already locked, an error code otherwise
   *  @see OFReadWriteLock::tryrdlock
   */
  int tryrdlock();

  /** try to lock the lock for writing
   *  @return 0 upon success, OFReadWriteLock::busy if the read/write lock
   *    is already locked, an error code otherwise
   *  @see OFReadWriteLock::trywrlock
   */
  int trywrlock();

  /** unlock the lock
   *  @return 0 upon success, an error code otherwise
   *  @see OFReadWriteLock::unlock
   */
  int unlock();

private:
  /** the lock on which we are operating */
  OFReadWriteLock& theLock;

  /** did we successfully lock the lock? */
  OFBool locked;

  /** unimplemented private copy constructor */
  OFReadWriteLocker(const OFReadWriteLocker& arg);

  /** unimplemented private assignment operator */
  OFReadWriteLocker& operator=(const OFReadWriteLocker& arg);
};

#endif
