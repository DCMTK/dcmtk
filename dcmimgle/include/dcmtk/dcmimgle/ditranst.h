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
 *  Purpose: DicomTransTemplate (Header)
 *
 */


#ifndef DITRANST_H
#define DITRANST_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/ofstd/ofbmanip.h"

#include "dcmtk/dcmimgle/diutils.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class building the base for other transformations.
 *  (e.g. scaling, flipping)
 */
template<class T>
class DiTransTemplate
{

 protected:

    /** constructor
     *
     ** @param  planes     number of planes
     *  @param  src_x      width of source image
     *  @param  src_y      height of source image
     *  @param  dest_x     width of destination image (after transformation)
     *  @param  dest_y     height of destination image
     *  @param  frames     number of frames
     *  @param  bits       number of bits per plane/pixel (optional)
     */
    DiTransTemplate(const int planes,
                    const Uint16 src_x,
                    const Uint16 src_y,
                    const Uint16 dest_x,
                    const Uint16 dest_y,
                    const Uint32 frames,
                    const int bits = 0)
      : Planes(planes),
        Src_X(src_x),
        Src_Y(src_y),
        Dest_X(dest_x),
        Dest_Y(dest_y),
        Frames(frames),
        Bits(((bits < 1) || (bits > OFstatic_cast(int, bitsof(T)))) ? OFstatic_cast(int, bitsof(T)) : bits)
    {
    }

    /** destructor
     */
    virtual ~DiTransTemplate()
    {
    }

    /** copy pixel data
     *
     ** @param  src   array of pointers to source image pixels
     *  @param  dest  array of pointers to destination image pixels
     */
    inline void copyPixel(const T *src[],
                          T *dest[])
    {
        const unsigned long count = OFstatic_cast(unsigned long, Dest_X) * OFstatic_cast(unsigned long, Dest_Y) * Frames;
        for (int j = 0; j < Planes; ++j)
            OFBitmanipTemplate<T>::copyMem(src[j], dest[j], count);
    }

    /** fill pixel data with specified value
     *
     ** @param  dest   array of pointers to destination image pixels
     *  @param  value  value to be filled in destination array
     */
    inline void fillPixel(T *dest[],
                          const T value)
    {
        const unsigned long count = OFstatic_cast(unsigned long, Dest_X) * OFstatic_cast(unsigned long, Dest_Y) * Frames;
        for (int j = 0; j < Planes; ++j)
            OFBitmanipTemplate<T>::setMem(dest[j], value, count);
    }

    /** clear pixel data (set values to 0)
     *
     ** @param  dest  array of pointers to destination image pixels
     */
    inline void clearPixel(T *dest[])
    {
        const unsigned long count = OFstatic_cast(unsigned long, Dest_X) * OFstatic_cast(unsigned long, Dest_Y) * Frames;
        for (int j = 0; j < Planes; ++j)
            OFBitmanipTemplate<T>::zeroMem(dest[j], count);
    }


    /// number of planes
    /*const*/ int Planes;                       // allow later changing to avoid warnings on Irix

    /// width of source image
    /*const*/ Uint16 Src_X;                     // add 'const' when interpolated scaling with clipping is fully implemented
    /// height of source image
    /*const*/ Uint16 Src_Y;                     // ... dito ...
    /// width of destination image
    const Uint16 Dest_X;
    /// height of destination image
    const Uint16 Dest_Y;

    /// number of frames
    const Uint32 Frames;
    /// number of bits per plane/pixel
    const int Bits;
};


#endif
