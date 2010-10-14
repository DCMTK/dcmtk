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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:27 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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


/*
 *
 * CVS/RCS Log:
 * $Log: ditranst.h,v $
 * Revision 1.17  2010-10-14 13:16:27  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.16  2010-03-01 09:08:47  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.15  2008-05-20 10:07:31  joergr
 * Added new method that clears the destination image (set pixel values to 0).
 *
 * Revision 1.14  2005/12/08 16:48:11  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.13  2003/12/23 15:53:22  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.12  2003/12/08 18:51:26  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.11  2003/06/02 17:08:07  joergr
 * Added include statement for "diutils.h".
 *
 * Revision 1.10  2001/06/01 15:49:52  meichel
 * Updated copyright header
 *
 * Revision 1.9  2000/03/08 16:24:25  meichel
 * Updated copyright header.
 *
 * Revision 1.8  2000/03/02 12:51:37  joergr
 * Rewrote variable initialization in class contructors to avoid warnings
 * reported on Irix.
 *
 * Revision 1.7  1999/09/17 13:08:12  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.6  1999/08/25 16:41:56  joergr
 * Added new feature: Allow clipping region to be outside the image
 * (overlapping).
 *
 * Revision 1.5  1999/05/03 11:09:32  joergr
 * Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.4  1999/03/24 17:20:27  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.3  1999/02/11 16:42:52  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.2  1998/12/22 14:42:23  joergr
 * Removed const declaration (as long as interpolated scaling isn't completed).
 *
 * Revision 1.1  1998/11/27 15:48:10  joergr
 * Added copyright message.
 * Added support for new bit manipulation class.
 *
 * Revision 1.2  1998/05/11 14:53:30  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
