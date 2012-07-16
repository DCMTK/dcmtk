/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *           of global objects, access to which is protected by a Mutex
 *           for multi-thread applications.
 *           class T must have copy constructor and assignment operator.
 *
 *
 */


#ifndef OFGLOBAL_H
#define OFGLOBAL_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofthread.h"  /* for class OFBool */

/** Template class which allows to declare global objects that are
 *  protected by a Mutex if used in multi-thread applications.
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
  , theMutex()
#endif
  {
  }

  /** destructor.
   */
  virtual ~OFGlobal() { }

  /** assigns new value to this object. If compiled for multi-thread operation,
   *  access to the value of the object is protected by a Mutex.
   *  @param arg new value
   */
  void set(const T& arg)
  {
#ifdef WITH_THREADS
    theMutex.lock();
#endif
    val = arg;
#ifdef WITH_THREADS
    theMutex.unlock();
#endif
  }

  /** gets the value of this object. If compiled for multi-thread operation,
   *  access to the value of the object is protected by a Mutex.
   *  @param arg return value is assigned to this parameter.
   */
  void xget(T& arg)
  {
#ifdef WITH_THREADS
    theMutex.lock();
#endif
    arg = val;
#ifdef WITH_THREADS
    theMutex.unlock();
#endif
  }

  /** returns the value of this object. If compiled for multi-thread operation,
   *  access to the value of the object is protected by a Mutex. The result
   *  is returned by value, not by reference.
   *  @return value of this object.
   */
  T get()
  {
#ifdef WITH_THREADS
    theMutex.lock();
#endif
    T result(val);
#ifdef WITH_THREADS
    theMutex.unlock();
#endif
    return result;
  }

private:

  /** value of this object
   */
  T val;

#ifdef WITH_THREADS
  /** if compiled for multi-thread operation, the Mutex protecting
   *  access to the value of this object.
   */
  OFMutex theMutex;
#endif

  /** unimplemented private default constructor */
  OFGlobal();

  /** unimplemented private copy constructor */
  OFGlobal(const OFGlobal<T>& arg);

  /** unimplemented private assignment operator */
  const OFGlobal<T>& operator=(const OFGlobal<T>& arg);

};


#endif
