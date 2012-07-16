/*
 *
 *  Copyright (C) 1996-2011, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomObjectCounter (Header)
 *
 */


#ifndef DIOBJCOU_H
#define DIOBJCOU_H

#include "dcmtk/config/osconfig.h"

#ifdef WITH_THREADS
#include "dcmtk/ofstd/ofthread.h"
#endif

#include "dcmtk/dcmimgle/didefine.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to count number of instances (objects created from a certain class).
 *  used to manage more than one reference to an object in a secure way.
 */
class DCMTK_DCMIMGLE_EXPORT DiObjectCounter
{

 public:

    /** add a reference.
     *  Increase the internal counter by 1.
     */
    inline void addReference()
    {
#ifdef WITH_THREADS
        theMutex.lock();
#endif
        ++Counter;
#ifdef WITH_THREADS
        theMutex.unlock();
#endif
    }

    /** remove a reference.
     *  Decrease the internal counter by 1 and delete the object only if the counter is zero.
     */
    inline void removeReference()
    {
#ifdef WITH_THREADS
        theMutex.lock();
#endif
        if (--Counter == 0)
        {
#ifdef WITH_THREADS
            theMutex.unlock();
#endif
            delete this;
#ifdef WITH_THREADS
        } else {
            theMutex.unlock();
#endif
        }
    }


 protected:

    /** constructor.
     *  Internal counter is initialized with 1.
     */
    DiObjectCounter()
      : Counter(1)
#ifdef WITH_THREADS
       ,theMutex()
#endif
    {
    }

    /** destructor
     */
    virtual ~DiObjectCounter()
    {
    }


 private:

    /// internal counter
    unsigned long Counter;

#ifdef WITH_THREADS
    /** if compiled for multi-thread operation, the Mutex protecting
     *  access to the value of this object.
     */
    OFMutex theMutex;
#endif
};


#endif
