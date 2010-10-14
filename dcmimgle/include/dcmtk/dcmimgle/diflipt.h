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
 *  Purpose: DicomFlipTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:26 $
 *  CVS/RCS Revision: $Revision: 1.23 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIFLIPT_H
#define DIFLIPT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/dipixel.h"
#include "dcmtk/dcmimgle/ditranst.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to flip images (on pixel data level).
 *  horizontally and vertically
 */
template<class T>
class DiFlipTemplate
  : public DiTransTemplate<T>
{

 public:

    /** constructor.
     *  This method is used to flip an image and store the result in the same storage area.
     *
     ** @param  pixel    pointer to object where the pixel data are stored
     *  @param  columns  width of the image
     *  @param  rows     height of the image
     *  @param  frames   number of frames
     *  @param  horz     flags indicating whether to flip horizontally or not
     *  @param  vert     flags indicating whether to flip vertically or not
     */
    DiFlipTemplate(DiPixel *pixel,
                   const Uint16 columns,
                   const Uint16 rows,
                   const Uint32 frames,
                   const int horz,
                   const int vert)
      : DiTransTemplate<T>(0, columns, rows, columns, rows, frames)
    {
        if (pixel != NULL)
        {
            this->Planes = pixel->getPlanes();
            if ((pixel->getCount() > 0) && (this->Planes > 0) &&
                (pixel->getCount() == OFstatic_cast(unsigned long, columns) * OFstatic_cast(unsigned long, rows) * frames))
            {
                if (horz && vert)
                    flipHorzVert(OFstatic_cast(T **, pixel->getDataArrayPtr()));
                else if (horz)
                    flipHorz(OFstatic_cast(T **, pixel->getDataArrayPtr()));
                else if (vert)
                    flipVert(OFstatic_cast(T **, pixel->getDataArrayPtr()));
            } else {
                DCMIMGLE_WARN("could not flip image ... corrupted data");
            }
        }
    }

    /** constructor.
     *  This method is used to perform only the preparation and to start flipping later with method 'flipData()'
     *
     ** @param  planes   number of planes (1 or 3)
     *  @param  columns  width of the image
     *  @param  rows     height of the image
     *  @param  frames   number of frames
     */
    DiFlipTemplate(const int planes,
                   const Uint16 columns,
                   const Uint16 rows,
                   const Uint32 frames)
      : DiTransTemplate<T>(planes, columns, rows, columns, rows, frames)
    {
    }

    /** destructor
     */
    virtual ~DiFlipTemplate()
    {
    }

    /** choose algorithm depending on flipping mode
     *
     ** @param  src   array of pointers to source image pixels
     *  @param  dest  array of pointers to destination image pixels
     *  @param  horz  flags indicating whether to flip horizontally or not
     *  @param  vert  flags indicating whether to flip vertically or not
     */
    inline void flipData(const T *src[],
                         T *dest[],
                         const int horz,
                         const int vert)
    {
        if ((src != NULL) && (dest != NULL))
        {
            if (horz && vert)
                flipHorzVert(src, dest);
            else if (horz)
                flipHorz(src, dest);
            else if (vert)
                flipVert(src, dest);
            else
                copyPixel(src, dest);
        }
    }


 protected:

   /** flip source image horizontally and store result in destination image
    *
    ** @param  src   array of pointers to source image pixels
    *  @param  dest  array of pointers to destination image pixels
    */
    inline void flipHorz(const T *src[],
                         T *dest[])
    {
        if ((src != NULL) && (dest != NULL))
        {
            register Uint16 x;
            register Uint16 y;
            register const T *p;
            register T *q;
            register T *r;
            for (int j = 0; j < this->Planes; ++j)
            {
                p = src[j];
                r = dest[j];
                for (Uint32 f = this->Frames; f != 0; --f)
                {
                    for (y = this->Src_Y; y != 0; --y)
                    {
                        q = r + this->Dest_X;
                        for (x = this->Src_X; x != 0; --x)
                            *--q = *p++;
                        r += this->Dest_X;
                    }
                }
            }
        }
    }

   /** flip source image vertically and store result in destination image
    *
    ** @param  src   array of pointers to source image pixels
    *  @param  dest  array of pointers to destination image pixels
    */
    inline void flipVert(const T *src[],
                         T *dest[])
    {
        if ((src != NULL) && (dest != NULL))
        {
            register Uint16 x;
            register Uint16 y;
            register const T *p;
            register T *q;
            register T *r;
            const unsigned long count = OFstatic_cast(unsigned long, this->Dest_X) * OFstatic_cast(unsigned long, this->Dest_Y);
            for (int j = 0; j < this->Planes; ++j)
            {
                p = src[j];
                r = dest[j];
                for (Uint32 f = this->Frames; f != 0; --f)
                {
                    r += count;
                    for (y = this->Src_Y; y != 0; --y)
                    {
                        q = r - this->Dest_X;
                        for (x = this->Src_X; x != 0; --x)
                            *q++ = *p++;
                        r -= this->Dest_X;
                    }
                    r += count;
                }
            }
        }
    }

   /** flip source image horizontally and vertically and store result in destination image
    *
    ** @param  src   array of pointers to source image pixels
    *  @param  dest  array of pointers to destination image pixels
    */
    inline void flipHorzVert(const T *src[],
                             T *dest[])
    {
       if ((src != NULL) && (dest != NULL))
       {
            register unsigned long i;
            register const T *p;
            register T *q;
            const unsigned long count = OFstatic_cast(unsigned long, this->Dest_X) * OFstatic_cast(unsigned long, this->Dest_Y);
            for (int j = 0; j < this->Planes; ++j)
            {
                p = src[j];
                q = dest[j];
                for (Uint32 f = this->Frames; f != 0; --f)
                {
                    q += count;
                    for (i = count; i != 0; --i)
                        *--q = *p++;
                    q += count;
                }
            }
        }
    }

 private:

   /** flip image horizontally and store result in the same storage area
    *
    ** @param  data  array of pointers to source/destination image pixels
    */
    inline void flipHorz(T *data[])
    {
        register Uint16 x;
        register Uint16 y;
        register T *p;
        register T *q;
        register T t;
        T *r;
        for (int j = 0; j < this->Planes; ++j)
        {
            r = data[j];
            for (Uint32 f = this->Frames; f != 0; --f)
            {
                for (y = this->Src_Y; y != 0; --y)
                {
                    p = r;
                    r += this->Dest_X;
                    q = r;
                    for (x = this->Src_X / 2; x != 0; --x)
                    {
                        t = *p;
                        *p++ = *--q;
                        *q = t;
                    }
                }
            }
        }
    }

   /** flip image vertically and store result in the same storage area
    *
    ** @param  data  array of pointers to source/destination image pixels
    */
    inline void flipVert(T *data[])
    {
        register Uint16 x;
        register Uint16 y;
        register T *p;
        register T *q;
        register T *r;
        register T t;
        T *s;
        const unsigned long count = OFstatic_cast(unsigned long, this->Dest_X) * OFstatic_cast(unsigned long, this->Dest_Y);
        for (int j = 0; j < this->Planes; ++j)
        {
            s = data[j];
            for (Uint32 f = this->Frames; f != 0; --f)
            {
                p = s;
                s += count;
                r = s;
                for (y = this->Src_Y / 2; y != 0; --y)
                {
                    r -= this->Dest_X;
                    q = r;
                    for (x = this->Src_X; x != 0; --x)
                    {
                        t = *p;
                        *p++ = *q;
                        *q++ = t;
                    }
                }
            }
        }
    }

   /** flip image horizontally and vertically and store result in the same storage area
    *
    ** @param  data  array of pointers to source/destination image pixels
    */
    inline void flipHorzVert(T *data[])
    {
        register unsigned long i;
        register T *p;
        register T *q;
        register T t;
        T *s;
        const unsigned long count = OFstatic_cast(unsigned long, this->Dest_X) * OFstatic_cast(unsigned long, this->Dest_Y);
        for (int j = 0; j < this->Planes; ++j)
        {
            s = data[j];
            for (Uint32 f = this->Frames; f != 0; --f)
            {
                p = s;
                q = s + count;
                for (i = count / 2; i != 0; --i)
                {
                    t = *p;
                    *p++ = *--q;
                    *q = t;
                }
                s += count;
            }
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diflipt.h,v $
 * Revision 1.23  2010-10-14 13:16:26  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.22  2010-03-01 09:08:46  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.21  2009-10-28 14:38:16  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.20  2009-10-28 09:53:40  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.19  2006-08-15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.18  2005/12/08 16:47:39  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.17  2005/06/15 08:25:18  joergr
 * Fixed bug which prevented flipHorzVert() from flipping multi-frame images
 * correctly (only the first frame was actually flipped).
 *
 * Revision 1.16  2004/04/21 10:00:36  meichel
 * Minor modifications for compilation with gcc 3.4.0
 *
 * Revision 1.15  2004/02/06 11:07:50  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.14  2003/12/23 15:53:22  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.13  2003/12/08 18:55:45  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.12  2001/06/01 15:49:41  meichel
 * Updated copyright header
 *
 * Revision 1.11  2000/09/12 10:04:44  joergr
 * Corrected bug: wrong parameter for attribute search routine led to crashes
 * when multiple pixel data attributes were contained in the dataset (e.g.
 * IconImageSequence). Added new checking routines to avoid crashes when
 * processing corrupted image data.
 *
 * Revision 1.10  2000/03/08 16:24:15  meichel
 * Updated copyright header.
 *
 * Revision 1.9  2000/03/02 12:51:36  joergr
 * Rewrote variable initialization in class contructors to avoid warnings
 * reported on Irix.
 *
 * Revision 1.8  1999/09/17 12:10:55  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.7  1999/05/03 11:09:28  joergr
 * Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.6  1999/04/28 14:46:54  joergr
 * Removed debug code.
 *
 * Revision 1.5  1999/03/24 17:20:00  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.4  1999/02/03 17:01:16  joergr
 * Removed some debug code.
 *
 * Revision 1.3  1999/01/20 14:59:05  joergr
 * Added debug code to measure time of some routines.
 *
 * Revision 1.2  1998/12/16 16:27:54  joergr
 * Added additional case to copy pixels.
 *
 * Revision 1.1  1998/11/27 14:57:46  joergr
 * Added copyright message.
 * Added methods and classes for flipping and rotating, changed for
 * scaling and clipping.
 *
 *
 */
