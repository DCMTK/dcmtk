/*
 *
 *  Copyright (C) 1997-2000, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
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
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-04-14 15:17:48 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofglobal.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __OFGLOBAL_H
#define __OFGLOBAL_H

#include "osconfig.h"
#include "ofthread.h"  /* for class OFBool */

/** Template class which allows to declare global objects that are 
 *  protected by a Mutex if used in multi-thread applications (i.e. 
 *  compiled with -D_REENTRANT). Class T must have copy constructor and 
 *  assignment operator.
 */
template <class T> class OFGlobal
{
public:

  /** constructor.
   *  @param arg value to which this object is initialised
   */
  OFGlobal(const T &arg)
  : val(arg)
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
#ifdef _REENTRANT
    theMutex.lock();
#endif
    val = arg;
#ifdef _REENTRANT
    theMutex.unlock();
#endif
  }

  /** gets the value of this object. If compiled for multi-thread operation,
   *  access to the value of the object is protected by a Mutex.
   *  @param arg return value is assigned to this parameter.
   */
  void xget(T& arg)
  {
#ifdef _REENTRANT
    theMutex.lock();
#endif
    arg = val;
#ifdef _REENTRANT
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
#ifdef _REENTRANT
    theMutex.lock();
#endif
    T result(val);
#ifdef _REENTRANT
    theMutex.unlock();
#endif
    return result;
  }
  
private:

  /** value of this object
   */
  T val;
  
#ifdef _REENTRANT
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

/*
 *
 * CVS/RCS Log:
 * $Log: ofglobal.h,v $
 * Revision 1.1  2000-04-14 15:17:48  meichel
 * Created new templace class OFGlobal which allows to easily implement
 *   mutex protected global flags.
 *
 *
 */
