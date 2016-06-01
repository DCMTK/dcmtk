/*
 *
 *  Copyright (C) 1996-2016, OFFIS e.V.
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
 *  Purpose: DicomRotateTemplate (Header)
 *
 */


#ifndef DIROTAT_H
#define DIROTAT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/dipixel.h"
#include "dcmtk/dcmimgle/ditranst.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to rotate images (on pixel data level).
 *  by steps of 90 degrees
 */
template<class T>
class DiRotateTemplate
  : public DiTransTemplate<T>
{

 public:

    /** constructor.
     *  This method is used to rotate an image and store the result in the same storage area.
     *
     ** @param  pixel      pointer to object where the pixel data are stored
     *  @param  src_cols   original width of the image
     *  @param  src_rows   original height of the image
     *  @param  dest_cols  new width of the image
     *  @param  dest_rows  new height of the image
     *  @param  frames     number of frames
     *  @param  degree     angle by which the image should be rotated
     */
    DiRotateTemplate(DiPixel *pixel,
                     const Uint16 src_cols,
                     const Uint16 src_rows,
                     const Uint16 dest_cols,
                     const Uint16 dest_rows,
                     const Uint32 frames,
                     const int degree)
      : DiTransTemplate<T>(0, src_cols, src_rows, dest_cols, dest_rows, frames)
    {
        if (pixel != NULL)
        {
            this->Planes = pixel->getPlanes();
            if ((pixel->getCount() > 0) && (this->Planes > 0) &&
                (pixel->getCount() == OFstatic_cast(unsigned long, src_cols) * OFstatic_cast(unsigned long, src_rows) * frames))
            {
                if (degree == 90)
                    rotateRight(OFstatic_cast(T **, pixel->getDataArrayPtr()));
                else if (degree == 180)
                    rotateTopDown(OFstatic_cast(T **, pixel->getDataArrayPtr()));
                else if (degree == 270)
                    rotateLeft(OFstatic_cast(T **, pixel->getDataArrayPtr()));
            } else {
                DCMIMGLE_WARN("could not rotate image ... corrupted data");
            }
        }
    }

    /** constructor.
     *  This method is used to perform only the preparation and to start rotation later with method 'rotateData()'
     *
     ** @param  planes     number of planes (1 or 3)
     *  @param  src_cols   original width of the image
     *  @param  src_rows   original height of the image
     *  @param  dest_cols  new width of the image
     *  @param  dest_rows  new height of the image
     *  @param  frames     number of frames
     */
    DiRotateTemplate(const int planes,
                     const Uint16 src_cols,
                     const Uint16 src_rows,
                     const Uint16 dest_cols,
                     const Uint16 dest_rows,
                     const Uint32 frames)
      : DiTransTemplate<T>(planes, src_cols, src_rows, dest_cols, dest_rows, frames)
    {
    }

    /** destructor
     */
    virtual ~DiRotateTemplate()
    {
    }

    /** choose algorithm depending on rotation angle
     *
     ** @param  src     array of pointers to source image pixels
     *  @param  dest    array of pointers to destination image pixels
     *  @param  degree  angle by which the image should be rotated
     */
    inline void rotateData(const T *src[],
                           T *dest[],
                           const int degree)
    {
        if (degree == 90)
            rotateRight(src, dest);
        else if (degree == 180)
            rotateTopDown(src, dest);
        else if (degree == 270)
            rotateLeft(src, dest);
        else
            this->copyPixel(src, dest);
    }


 protected:

   /** rotate source image left and store result in destination image
    *
    ** @param  src   array of pointers to source image pixels
    *  @param  dest  array of pointers to destination image pixels
    */
    inline void rotateLeft(const T *src[],
                           T *dest[])
    {
        if ((src != NULL) && (dest != NULL))
        {
            Uint16 x;
            Uint16 y;
            const T *p;
            T *q;
            T *r;
            const unsigned long count = OFstatic_cast(unsigned long, this->Dest_X) * OFstatic_cast(unsigned long, this->Dest_Y);
            for (int j = 0; j < this->Planes; ++j)
            {
                p = src[j];
                r = dest[j];
                for (unsigned long f = this->Frames; f != 0; --f)
                {
                    r += count;
                    for (x = this->Dest_X; x != 0; --x)
                    {
                        q = r - x;
                        for (y = this->Dest_Y; y != 0; --y)
                        {
                            *q = *p++;
                            q -= this->Dest_X;
                        }
                    }
                }
            }
        }
    }

   /** rotate source image right and store result in destination image
    *
    ** @param  src   array of pointers to source image pixels
    *  @param  dest  array of pointers to destination image pixels
    */
    inline void rotateRight(const T *src[],
                            T *dest[])
    {
        if ((src != NULL) && (dest != NULL))
        {
            Uint16 x;
            Uint16 y;
            const T *p;
            T *q;
            T *r;
            const unsigned long count = OFstatic_cast(unsigned long, this->Dest_X) * OFstatic_cast(unsigned long, this->Dest_Y);
            for (int j = 0; j < this->Planes; ++j)
            {
                p = src[j];
                r = dest[j];
                for (unsigned long f = this->Frames; f != 0; --f)
                {
                    for (x = this->Dest_X; x != 0; --x)
                    {
                        q = r + x - 1;
                        for (y = this->Dest_Y; y != 0; --y)
                        {
                            *q = *p++;
                            q += this->Dest_X;
                        }
                    }
                    r += count;
                }
            }
        }
    }

   /** rotate source image top-down and store result in destination image
    *
    ** @param  src   array of pointers to source image pixels
    *  @param  dest  array of pointers to destination image pixels
    */
    inline void rotateTopDown(const T *src[],
                              T *dest[])
    {
        if ((src != NULL) && (dest != NULL))
        {
            unsigned long i;
            const T *p;
            T *q;
            const unsigned long count = OFstatic_cast(unsigned long, this->Dest_X) * OFstatic_cast(unsigned long, this->Dest_Y);
            for (int j = 0; j < this->Planes; ++j)
            {
                p = src[j];
                q = dest[j];
                for (unsigned long f = this->Frames; f != 0; --f)
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

   /** rotate image left and store result in the same storage area
    *
    ** @param  data  array of pointers to source/destination image pixels
    */
    inline void rotateLeft(T *data[])
    {
        const unsigned long count = OFstatic_cast(unsigned long, this->Dest_X) * OFstatic_cast(unsigned long, this->Dest_Y);
        T *temp = new T[count];
        if (temp != NULL)
        {
            Uint16 x;
            Uint16 y;
            const T *p;
            T *q;
            T *r;
            for (int j = 0; j < this->Planes; ++j)
            {
                r = data[j];
                for (unsigned long f = this->Frames; f != 0; --f)
                {
                    OFBitmanipTemplate<T>::copyMem(OFstatic_cast(const T *, r), temp, count);  // create temporary copy of current frame
                    p = temp;
                    r += count;
                    for (x = this->Dest_X; x != 0; --x)
                    {
                        q = r - x;
                        for (y = this->Dest_Y; y != 0; --y)
                        {
                            *q = *p++;
                            q -= this->Dest_X;
                        }
                    }
                }
            }
            delete[] temp;
        }
    }

   /** rotate image right and store result in the same storage area
    *
    ** @param  data  array of pointers to source/destination image pixels
    */
    inline void rotateRight(T *data[])
    {
        const unsigned long count = OFstatic_cast(unsigned long, this->Dest_X) * OFstatic_cast(unsigned long, this->Dest_Y);
        T *temp = new T[count];
        if (temp != NULL)
        {
            Uint16 x;
            Uint16 y;
            const T *p;
            T *q;
            T *r;
            for (int j = 0; j < this->Planes; ++j)
            {
                r = data[j];
                for (unsigned long f = this->Frames; f != 0; --f)
                {
                    OFBitmanipTemplate<T>::copyMem(OFstatic_cast(const T *, r), temp, count);  // create temporary copy of current frame
                    p = temp;
                    for (x = this->Dest_X; x != 0; --x)
                    {
                        q = r + x - 1;
                        for (y = this->Dest_Y; y != 0; --y)
                        {
                            *q = *p++;
                            q += this->Dest_X;
                        }
                    }
                    r += count;
                }
            }
            delete[] temp;
        }
    }

   /** rotate image top-down and store result in the same storage area
    *
    ** @param  data  array of pointers to source/destination image pixels
    */
    inline void rotateTopDown(T *data[])
    {
        unsigned long i;
        T *p;
        T *q;
        T t;
        T *s;
        const unsigned long count = OFstatic_cast(unsigned long, this->Dest_X) * OFstatic_cast(unsigned long, this->Dest_Y);
        for (int j = 0; j < this->Planes; ++j)
        {
            s = data[j];
            for (unsigned long f = this->Frames; f != 0; --f)
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
