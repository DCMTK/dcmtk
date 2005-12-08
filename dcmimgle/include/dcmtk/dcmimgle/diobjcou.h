/*
 *
 *  Copyright (C) 1996-2005, OFFIS
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomObjectCounter (Header)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-12-08 16:47:59 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIOBJCOU_H
#define DIOBJCOU_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofthread.h"


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
#ifdef _REENTRANT
        theMutex.lock();
#endif
        ++Counter;
#ifdef _REENTRANT
        theMutex.unlock();
#endif
    }

    /** remove a reference.
     *  Decrease the internal counter by 1 and delete the object only if the counter is zero.
     */
    inline void removeReference()
    {
#ifdef _REENTRANT
        theMutex.lock();
#endif
        if (--Counter == 0)
        {
#ifdef _REENTRANT
            theMutex.unlock();
#endif
            delete this;
#ifdef _REENTRANT
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
#ifdef _REENTRANT
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

#ifdef _REENTRANT
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
