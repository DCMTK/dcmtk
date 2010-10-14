/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:26 $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIOBJCOU_H
#define DIOBJCOU_H

#include "dcmtk/config/osconfig.h"

#ifdef WITH_THREADS
#include "dcmtk/ofstd/ofthread.h"
#endif


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to count number of instances (objects created from a certain class).
 *  used to manage more than one reference to an object in a secure way.
 */
class DiObjectCounter
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


/*
 *
 * CVS/RCS Log:
 * $Log: diobjcou.h,v $
 * Revision 1.14  2010-10-14 13:16:26  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.13  2010-10-04 14:44:45  joergr
 * Replaced "#ifdef _REENTRANT" by "#ifdef WITH_THREADS" where appropriate (i.e.
 * in all cases where OFMutex, OFReadWriteLock, etc. are used).
 *
 * Revision 1.12  2010-03-01 09:08:47  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.11  2005-12-08 16:47:59  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.10  2003/12/23 15:53:22  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.9  2003/12/08 19:30:16  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.8  2001/06/01 15:49:48  meichel
 * Updated copyright header
 *
 * Revision 1.7  2000/07/12 12:47:47  joergr
 * Correct bug in destructor of ObjectCounter class.
 *
 * Revision 1.6  2000/05/25 10:35:23  joergr
 * Added member variable to member initialization list (avoid compiler
 * warnings).
 *
 * Revision 1.5  2000/04/28 12:30:51  joergr
 * ObjectCounter uses now class OFMutex to be MT-safe.
 *
 * Revision 1.4  2000/03/08 16:24:21  meichel
 * Updated copyright header.
 *
 * Revision 1.3  1999/09/17 12:44:08  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.2  1999/03/24 17:20:18  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.1  1998/11/27 15:40:30  joergr
 * Added copyright message.
 *
 * Revision 1.3  1998/05/11 14:53:24  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
