/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Template class for bit manipulations (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 12:29:20 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofbmanip.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __OFBMANIP_H
#define __OFBMANIP_H

#include "osconfig.h"

#ifdef HAVE_STRING_H
 #include <string.h>
#endif

#ifdef HAVE_STRINGS_H
 #include <strings.h>
#endif


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** A template class for bit manipulations.
 *  This class is used to perform platform independent operations on typed memory areas.
 */
template<class T>
class OFBitmanipTemplate
{

 public:
 
    /** copies specified number of elements from source to destination
     *
     ** @param  src    pointer to source memory
     *  @param  dest   pointer to destination memory
     *  @param  count  number of elements to be copied
     */
    static void copyMem(const T *src,
                        T *dest,
                        const unsigned long count)
    {
#ifdef HAVE_MEMCPY
        memcpy((void *)dest, (const void *)src, (size_t)count * sizeof(T));
#elif HAVE_BCOPY
        bcopy((const void *)src, (void *)dest, (size_t)count * sizeof(T));
#else
        register unsigned long i;
        register const T *p = src;
        register T *q = dest;
        for (i = 0; i < count; i++)
            *q++ = *p++;
#endif
    }


    /** sets specified number of elements in destination memory to defined value
     *
     ** @param  dest   pointer to destination memory
     *  @param  value  value to be set
     *  @param  count  number of elements to be set
     */
    static void setMem(T *dest, const T value, const unsigned long count)
    {
#ifdef HAVE_MEMSET
        memset((void *)dest, 0, (size_t)count * sizeof(T));
#else
        register unsigned long i;
        register T *q = dest;
        for (i = 0; i < count; i++)
            *q++ = 0;
#endif
    }


    /** sets specified number of elements in destination memory to zero
     *
     ** @param  dest   pointer to destination memory
     *  @param  count  number of elements to be set to zero
     */
    static void zeroMem(T *dest, const unsigned long count)
    {
#ifdef HAVE_BZERO
        bzero((void *)dest, (size_t)count * sizeof(T));
#else        
        setMem(dest, 0, count);
#endif        
    }
};


#endif


/*
**
** CVS/RCS Log:
** $Log: ofbmanip.h,v $
** Revision 1.1  1998-11-27 12:29:20  joergr
** First release of class for plaform independant memory operations.
**
**
**
*/
