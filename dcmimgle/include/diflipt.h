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
 *  Purpose: DicomFlipTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-12-16 16:27:54 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diflipt.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIFLIPT_H
#define __DIFLIPT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dipixel.h"
#include "ditranst.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiFlipTemplate
  : public DiTransTemplate<T>
{

 public:

    DiFlipTemplate(DiPixel *pixel,
                   const Uint16 columns,
                   const Uint16 rows,
                   const Uint32 frames,
                   const int horz,
                   const int vert)
      : DiTransTemplate<T>((pixel != NULL) ? pixel->getPlanes() : 0, columns, rows, columns, rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0) && (Planes > 0))
        {
            if (horz && vert)
                flipHorzVert((T **)pixel->getDataPtr());
            else if (horz)
                flipHorz((T **)pixel->getDataPtr());
            else if (vert)
                flipVert((T **)pixel->getDataPtr());
        }
    }

    DiFlipTemplate(const int planes,
                   const Uint16 columns,
                   const Uint16 rows,
                   const Uint32 frames)
      : DiTransTemplate<T>(planes, columns, rows, columns, rows, frames)
    {
    }

    virtual ~DiFlipTemplate()
    {
    }

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
            for (int j = 0; j < Planes; j++)
            {
                p = src[j];
                r = dest[j];
                for (unsigned long f = 0; f < Frames; f++)
                {               
                    for (y = 0; y < Src_Y; y++)
                    {
                        q = r + Dest_X;
                        for (x = 0; x < Src_X; x++)
                            *--q = *p++;
                        r += Dest_X;
                    }
                }
            }
        }
    }
 
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
            const unsigned long count = (unsigned long)Dest_X * (unsigned long)Dest_Y;
            for (int j = 0; j < Planes; j++)
            {
                p = src[j];
                r = dest[j];
                for (unsigned long f = 0; f < Frames; f++)
                {          
                    r += count;     
                    for (y = 0; y < Src_Y; y++)
                    {
                        q = r - Dest_X;
                        for (x = 0; x < Src_X; x++)
                            *q++ = *p++;
                        r -= Dest_X;
                    }
                    r += count;
                }
            }
        }
    }

    inline void flipHorzVert(const T *src[],
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

    inline void flipHorz(T *data[])
    {
        register Uint16 x;
        register Uint16 y;
        register T *p;
        register T *q;
        register T t;
        T *r;
        for (int j = 0; j < Planes; j++)
        {
            r = data[j];
            for (unsigned long f = 0; f < Frames; f++)
            {               
                for (y = 0; y < Src_Y; y++)
                {
                    p = r;
                    r += Dest_X;
                    q = r;
                    for (x = 0; x < Src_X / 2; x++)
                    {
                        t = *p;
                        *p++ = *--q;
                        *q = t;
                    }
                }
            }
        }
    }

    inline void flipVert(T *data[])
    {
        register Uint16 x;
        register Uint16 y;
        register T *p;
        register T *q;
        register T *r;
        register T t;
        T *s;
        const unsigned long count = (unsigned long)Dest_X * (unsigned long)Dest_Y;
        for (int j = 0; j < Planes; j++)
        {
            s = data[j];
            for (unsigned long f = 0; f < Frames; f++)
            {              
                p = s; 
                s += count;
                r = s; 
                for (y = 0; y < Src_Y / 2; y++)
                {
                    r -= Dest_X;
                    q = r;
                    for (x = 0; x < Src_X; x++)
                    {
                        t = *p;
                        *p++ = *q;
                        *q++ = t;
                    }
                }
            }
        }
    }

    inline void flipHorzVert(T *data[])
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
**
** CVS/RCS Log:
** $Log: diflipt.h,v $
** Revision 1.2  1998-12-16 16:27:54  joergr
** Added additional case to copy pixels.
**
** Revision 1.1  1998/11/27 14:57:46  joergr
** Added copyright message.
** Added methods and classes for flipping and rotating, changed for
** scaling and clipping.
**
**
**
*/
