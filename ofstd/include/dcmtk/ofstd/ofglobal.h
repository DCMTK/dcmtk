/*
 *
 *  Copyright (C) 1997-2005, OFFIS
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
 *  Update Date:      $Date: 2005-12-08 16:05:57 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef OFGLOBAL_H
#define OFGLOBAL_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofthread.h"  /* for class OFBool */

/** Template class which allows to declare global objects that are
 *  protected by a Mutex if used in multi-thread applications.
 *  Must be compiled with -D_REENTRANT for multi-thread-operation.
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
#ifdef _REENTRANT
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
 * Revision 1.6  2005-12-08 16:05:57  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.5  2003/12/05 10:37:41  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright date where appropriate.
 *
 * Revision 1.4  2001/06/01 15:51:34  meichel
 * Updated copyright header
 *
 * Revision 1.3  2000/10/10 12:01:21  meichel
 * Created/updated doc++ comments
 *
 * Revision 1.2  2000/05/30 17:03:38  meichel
 * Added default constructor for Mutex to initializer list in OFGlobal.
 *
 * Revision 1.1  2000/04/14 15:17:48  meichel
 * Created new templace class OFGlobal which allows to easily implement
 *   mutex protected global flags.
 *
 *
 */
