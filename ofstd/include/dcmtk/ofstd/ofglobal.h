/*
 *
 *  Copyright (C) 1997-2024, OFFIS e.V.
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
 *  Purpose: OFGlobal is a template class intended for the declaration
 *           of global objects, access to which is protected by a
 *           Read/Write Lock for multi-thread applications.
 *           class T must have copy constructor and assignment operator.
 *
 */

#ifndef OFGLOBAL_H
#define OFGLOBAL_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofthread.h"  /* for class OFBool */

#if defined(HAVE_STL_ATOMIC) && defined(WITH_THREADS)
#include <atomic>
#endif

/** Template class which allows to declare global objects that are
 *  protected by a Read/Write Lock if used in multi-thread applications.
 *  Must be compiled with -DWITH_THREADS for multi-thread-operation.
 *  Template class T must have copy constructor and assignment operator.
 */
template <class T> class OFGlobal
{
public:

  /** constructor.
   *  @param arg value to which this object is initialised
   */
  OFGlobal(const T &arg)
  : val(arg)
#ifdef WITH_THREADS
  , theRWLock()
#endif
  {
  }

  /** destructor.
   */
  virtual ~OFGlobal() { }

  /** assigns new value to this object. If compiled for multi-thread operation,
   *  access to the value of the object is protected by a Read/Write Lock.
   *  @param arg new value
   */
  void set(const T& arg)
  {
#ifdef WITH_THREADS
    theRWLock.wrlock();
#endif
    val = arg;
#ifdef WITH_THREADS
    theRWLock.wrunlock();
#endif
  }

  /** gets the value of this object. If compiled for multi-thread operation,
   *  access to the value of the object is protected by a Read/Write Lock.
   *  @param arg return value is assigned to this parameter.
   */
  void xget(T& arg)
  {
#ifdef WITH_THREADS
    theRWLock.rdlock();
#endif
    arg = val;
#ifdef WITH_THREADS
    theRWLock.rdunlock();
#endif
  }

  /** returns the value of this object. If compiled for multi-thread operation,
   *  access to the value of the object is protected by a Read/Write Lock. The result
   *  is returned by value, not by reference.
   *  @return value of this object.
   */
  T get()
  {
#ifdef WITH_THREADS
    theRWLock.rdlock();
#endif
    T result(val);
#ifdef WITH_THREADS
    theRWLock.rdunlock();
#endif
    return result;
  }

private:

  /** value of this object
   */
  T val;

#ifdef WITH_THREADS
  /** if compiled for multi-thread operation, the Read/Write Lock protecting
   *  access to the value of this object.
   */
  OFReadWriteLock theRWLock;
#endif

  /** unimplemented private default constructor */
  OFGlobal();

  /** unimplemented private copy constructor */
  OFGlobal(const OFGlobal<T>& arg);

  /** unimplemented private assignment operator */
  const OFGlobal<T>& operator=(const OFGlobal<T>& arg);

};

#if defined(HAVE_STL_ATOMIC) && defined(WITH_THREADS)
/** template specialization for OFGlobal<OFBool> that uses a std::atomic flag
 *  without read-write lock and is lock-free (and thus much more efficient) on many platforms.
 */
template <> class OFGlobal<OFBool>
{
public:

  /** constructor.
   *  @param arg value to which this object is initialised
   */
  OFGlobal(const OFBool &arg)
  : val(arg)
  {
  }

  /** destructor.
   */
  virtual ~OFGlobal() { }

  /** assigns new value to this object.
   *  @param arg new value
   */
  void set(const OFBool& arg)
  {
    val = arg;
  }

  /** gets the value of this object.
   *  @param arg return value is assigned to this parameter.
   */
  void xget(bool& arg)
  {
    arg = val;
  }


  /** returns the value of this object. The result
   *  is returned by value, not by reference.
   *  @return value of this object.
   */
  OFBool get()
  {
    OFBool result(val);
    return result;
  }

private:

  /** value of this object
   */
  std::atomic<OFBool> val;

  /** unimplemented private default constructor */
  OFGlobal();

  /** unimplemented private copy constructor */
  OFGlobal(const OFGlobal<OFBool>& arg);

  /** unimplemented private assignment operator */
  const OFGlobal<OFBool>& operator=(const OFGlobal<OFBool>& arg);

};
#endif

#endif
