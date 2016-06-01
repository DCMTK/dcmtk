/*
 *
 *  Copyright (C) 1997-2016, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Template class for bit manipulations (Header)
 *
 */


#ifndef OFBMANIP_H
#define OFBMANIP_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofdefine.h"

#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

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

    /** copies specified number of elements from source to destination.
     *  Both src and dest must be aligned according to T's align requirements.
     *  These memory areas must not overlap!
     *
     ** @param  src    pointer to source memory
     *  @param  dest   pointer to destination memory
     *  @param  count  number of elements to be copied
     */
    static void copyMem(const T *src,
                        T *dest,
                        const size_t count)
    {
#ifdef HAVE_MEMCPY
        memcpy(OFstatic_cast(void *, dest), OFstatic_cast(const void *, src), count * sizeof(T));
#else
        size_t i;
        const T *p = src;
        T *q = dest;
        for (i = count; i != 0; --i)
            *q++ = *p++;
#endif
    }


    /** moves specified number of elements from source to destination.
     *  Both src and dest must be aligned according to T's align requirements.
     *  If src and dest are not equal, they must be at least sizeof(T) bytes
     *  apart. These memory areas may overlap.
     *
     ** @param  src    pointer to source memory
     *  @param  dest   pointer to destination memory
     *  @param  count  number of elements to be moved
     */
    static void moveMem(const T *src,
                        T *dest,
                        const size_t count)
    {
#ifdef HAVE_MEMMOVE
        memmove(OFstatic_cast(void *, dest), OFstatic_cast(const void *, src), OFstatic_cast(size_t, count) * sizeof(T));
#else
        if (src == dest)
            return;

        size_t i;
        const T *p = src;
        T *q = dest;
        if (src > dest)
        {
            // src is above dest in memory, we start copying from the start
            for (i = count; i != 0; --i)
                *q++ = *p++;
        }
        else
        {
            // src is below dest in memory, we start copying from the end
            q += count - 1;
            p += count - 1;
            for (i = count; i != 0; --i)
                *q-- = *p--;
        }
#endif
    }


    /** sets specified number of elements in destination memory to a defined value
     *
     ** @param  dest   pointer to destination memory
     *  @param  value  value to be set
     *  @param  count  number of elements to be set
     */
    static void setMem(T *dest,
                       const T value,
                       const size_t count)
    {
#ifdef HAVE_MEMSET
        if ((value == 0) || (sizeof(T) == sizeof(unsigned char)))
            memset(OFstatic_cast(void *, dest), OFstatic_cast(int, value), OFstatic_cast(size_t, count) * sizeof(T));
        else
#endif
        {
            size_t i;
            T *q = dest;
            for (i = count; i != 0; --i)
                *q++ = value;
        }
    }


    /** sets specified number of elements in destination memory to zero
     *
     ** @param  dest   pointer to destination memory
     *  @param  count  number of elements to be set to zero
     */
    static void zeroMem(T *dest,
                        const size_t count)
    {
#ifdef HAVE_MEMZERO
        memzero(dest, OFstatic_cast(size_t, count) * sizeof(T));
#else
        size_t i;
        T *q = dest;
        for (i = count; i != 0; --i)
            *q++ = 0;
#endif
    }
};


#endif
