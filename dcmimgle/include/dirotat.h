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
 *  Update Date:      $Date: 1999-01-20 15:12:47 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dirotat.h,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
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

#ifdef DEBUG
 #include "oftimer.h"
#endif


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiRotateTemplate
  : public DiTransTemplate<T>
{

 public:

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

    DiRotateTemplate(const int planes,
                     const Uint16 src_cols,
                     const Uint16 src_rows,
                     const Uint16 dest_cols,
                     const Uint16 dest_rows,
                     const Uint32 frames)
      : DiTransTemplate<T>(planes, src_cols, src_rows, dest_cols, dest_rows, frames)
    {
    }

    virtual ~DiRotateTemplate()
    {
    }

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
                for (unsigned long f = 0; f < Frames; f++)
                {
                    r += count;
                    for (x = Dest_X; x > 0; x--)
                    {
                        q = r - x;
                        for (y = 0; y < Dest_Y; y++)
                        {
                            *q = *p++;
                            q -= Dest_X;
                        }
                    }
                }
            }
        }
    }

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
                for (unsigned long f = 0; f < Frames; f++)
                {
                    for (x = Dest_X; x > 0; x--)
                    {
                        q = r + x - 1;
                        for (y = 0; y < Dest_Y; y++)
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
                for (unsigned long f = 0; f < Frames; f++)
                {
                    q += count;
                    for (i = 0; i < count; i++)
                        *--q = *p++;
                    q += count;
                }
            }
        }
    }

 private:

    inline void rotateLeft(T *data[])
    {
        const unsigned long count = (unsigned long)Dest_X * (unsigned long)Dest_Y;
        T *temp = new T[count];
        if (temp != NULL)
        {
/*
            const Uint16 block_X = (Dest_X < 256) ? Dest_X : 256;
            const Uint16 block_Y = (Dest_Y < 256) ? Dest_Y : 256;
            Uint16 count_X = 0;
            Uint16 count_Y = 0;
            Uint16 copy_X = block_X;
            Uint16 copy_Y = block_Y;
*/
            register Uint16 x;
            register Uint16 y;
            register const T *p;
            register T *q;
            register T *r;
            for (int j = 0; j < Planes; j++)
            {
                r = data[j];
                for (unsigned long f = 0; f < Frames; f++)
                {
#ifdef DEBUG
 OFTimer timer;
#endif
                    OFBitmanipTemplate<T>::copyMem((const T *)r, temp, count);      // create temporary copy of current frame
#ifdef DEBUG
 cerr << "time for copyMem: " << timer.getDiff() << " s" << endl;
#endif
                    p = temp;
                    r += count;
                    for (x = Dest_X; x > 0; x--)
                    {
                        q = r - x;
                        for (y = 0; y < Dest_Y; y++)
                        {
                            *q = *p++;
                            q -= Dest_X;
                        }
                    }
/*
                    T *p_block = temp;
                    T *p_line;
                    T *q_block = data + count - Dest_X;
                    T *q_line;
                    while (copy_X == block_X)
                    {
                        for (x = 0; x < copy_X, x++)
                        {
                            p_line = p_block;
                            while (copy_Y == block_Y)
                            {
                                p = p_line;
                                q = q_line;
                                for (y = 0; y < copy_Y, y++)
                                {
                                    *q = *p++;
                                    q -= Dest_X;
                                }
                                p_line += Src_X;
                                q_line++;
                                count_Y += block_Y;
                                if (count_Y > Dest_Y)
                                    copy_Y = count_Y - Dest_Y;
                            }
                            p_block += block_X
                        }
                        q_block -= 
                        count_X += block_X;
                        if (count_X > Dest_X)
                            copy_X = count_X - Dest_X;
                    }
*/
#ifdef DEBUG
 cerr << "time for copy+rotate: " << timer.getDiff() << " s" << endl;
#endif
                }
            }
            delete[] temp;
        }
    }

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
                for (unsigned long f = 0; f < Frames; f++)
                {
                    OFBitmanipTemplate<T>::copyMem((const T *)r, temp, count);      // create temporary copy of current frame
                    p = temp;
                    for (x = Dest_X; x > 0; x--)
                    {
                        q = r + x - 1;
                        for (y = 0; y < Dest_Y; y++)
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
            for (unsigned long f = 0; f < Frames; f++)
            {               
                p = s;
                q = s + count;
                for (i = 0; i < count / 2; i++)
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
 * Revision 1.4  1999-01-20 15:12:47  joergr
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
