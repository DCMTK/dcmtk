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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomRotateTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-09-17 13:07:20 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dirotat.h,v $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIROTAT_H
#define __DIROTAT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dipixel.h"
#include "ditranst.h"


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
      : DiTransTemplate<T>((pixel != NULL) ? pixel->getPlanes() : 0, src_cols, src_rows, dest_cols, dest_rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0) && (Planes > 0))
        {
            if (degree == 90)
                rotateRight((T **)pixel->getDataPtr());
            else if (degree == 180)
                rotateTopDown((T **)pixel->getDataPtr());
            else if (degree == 270)
                rotateLeft((T **)pixel->getDataPtr());
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
            copyPixel(src, dest);
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
            register Uint16 x;
            register Uint16 y;
            register const T *p;
            register T *q;
            register T *r;
            const unsigned long count = (unsigned long)Dest_X * (unsigned long)Dest_Y;
            for (int j = 0; j < Planes; j++)
            {
                p = src[j];
                r = dest[j];
                for (unsigned long f = Frames; f != 0; f--)
                {
                    r += count;
                    for (x = Dest_X; x != 0; x--)
                    {
                        q = r - x;
                        for (y = Dest_Y; y != 0; y--)
                        {
                            *q = *p++;
                            q -= Dest_X;
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
            register Uint16 x;
            register Uint16 y;
            register const T *p;
            register T *q;
            register T *r;
            const unsigned long count = (unsigned long)Dest_X * (unsigned long)Dest_Y;
            for (int j = 0; j < Planes; j++)
            {
                p = src[j];
                r = dest[j];
                for (unsigned long f = Frames; f != 0; f--)
                {
                    for (x = Dest_X; x != 0; x--)
                    {
                        q = r + x - 1;
                        for (y = Dest_Y; y != 0; y--)
                        {
                            *q = *p++;
                            q += Dest_X;
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
            register unsigned long i;
            register const T *p;
            register T *q;
            const unsigned long count = (unsigned long)Dest_X * (unsigned long)Dest_Y;
            for (int j = 0; j < Planes; j++)
            {
                p = src[j];
                q = dest[j];
                for (unsigned long f = Frames; f != 0; f--)
                {
                    q += count;
                    for (i = count; i != 0; i--)
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
        const unsigned long count = (unsigned long)Dest_X * (unsigned long)Dest_Y;
        T *temp = new T[count];
        if (temp != NULL)
        {
            register Uint16 x;
            register Uint16 y;
            register const T *p;
            register T *q;
            register T *r;
            for (int j = 0; j < Planes; j++)
            {
                r = data[j];
                for (unsigned long f = Frames; f != 0; f--)
                {
                    OFBitmanipTemplate<T>::copyMem((const T *)r, temp, count);      // create temporary copy of current frame
                    p = temp;
                    r += count;
                    for (x = Dest_X; x != 0; x--)
                    {
                        q = r - x;
                        for (y = Dest_Y; y != 0; y--)
                        {
                            *q = *p++;
                            q -= Dest_X;
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
        const unsigned long count = (unsigned long)Dest_X * (unsigned long)Dest_Y;
        T *temp = new T[count];
        if (temp != NULL)
        {
            register Uint16 x;
            register Uint16 y;
            register const T *p;
            register T *q;
            register T *r;
            for (int j = 0; j < Planes; j++)
            {
                r = data[j];
                for (unsigned long f = Frames; f != 0; f--)
                {
                    OFBitmanipTemplate<T>::copyMem((const T *)r, temp, count);      // create temporary copy of current frame
                    p = temp;
                    for (x = Dest_X; x != 0; x--)
                    {
                        q = r + x - 1;
                        for (y = Dest_Y; y != 0; y--)
                        {
                            *q = *p++;
                            q += Dest_X;
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
        register unsigned long i;
        register T *p;
        register T *q;
        register T t;
        T *s;
        const unsigned long count = (unsigned long)Dest_X * (unsigned long)Dest_Y;
        for (int j = 0; j < Planes; j++)
        {
            s = data[j];
            for (unsigned long f = Frames; f != 0; f--)
            {               
                p = s;
                q = s + count;
                for (i = count / 2; i != 0; i--)
                {
                    t = *p;
                    *p++ = *--q;
                    *q = t;
                }
            }
        }
    }
};


#endif
                        

/*
 *
 * CVS/RCS Log:
 * $Log: dirotat.h,v $
 * Revision 1.6  1999-09-17 13:07:20  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.5  1999/03/24 17:17:20  joergr
 * Removed debug code.
 * Added/Modified comments and formatting.
 *
 * Revision 1.4  1999/01/20 15:12:47  joergr
 * Added debug code to measure time of some routines.
 *
 * Revision 1.3  1998/12/16 16:38:49  joergr
 * Added additional case to copy pixels.
 *
 * Revision 1.2  1998/12/14 17:30:49  joergr
 * Added (missing) implementation of methods to rotate images/frames without
 * creating a new DicomImage.
 *
 * Revision 1.1  1998/11/27 14:57:46  joergr
 * Added copyright message.
 * Added methods and classes for flipping and rotating, changed for
 * scaling and clipping.
 *
 *
 */
