/*
 *
 *  Copyright (C) 1996-99, OFFIS
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
 *  Purpose: DicomScaleTemplates (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-02-03 17:35:14 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/discalet.h,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DISCALET_H
#define __DISCALET_H

#include "osconfig.h"
#include "dctypes.h"

#include "ditranst.h"
#include "dipxrept.h"
#include "diutils.h"

#include <iostream.h>


#define SCALE 4096
#define HALFSCALE 2048


/*
 *   help function to set scaling values
 */

static inline void setScaleValues(Uint16 data[],
                                  const Uint16 min,
                                  const Uint16 max)
{
    register Uint16 remainder = max % min;
    Uint16 step0 = max / min;
    Uint16 step1 = max / min;
    if (remainder > (Uint16)(min / 2))
    {
        remainder = min - remainder;
        step0++;
    }
    else
        step1++;
    const double count = (double)min / ((double)remainder + 1);
    register Uint16 i;
    register double c = count;
    for (i = 0; i < min; i++)
    {
        if ((i >= (Uint16)c) && (remainder > 0))
        {
            remainder--;
            c += count;
            data[i] = step1;
        }
        else 
            data[i] = step0;
    }
}


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiScaleTemplate
  : public DiTransTemplate<T>
{
    
 public:

    DiScaleTemplate(const int planes,
                    const Uint16 columns,           /* resolution of source image */
                    const Uint16 rows,
                    const Uint16 left,              /* origin of clipping area */
                    const Uint16 top,
                    const Uint16 src_cols,          /* extension of clipping area */
                    const Uint16 src_rows,
                    const Uint16 dest_cols,         /* extension of destination image */
                    const Uint16 dest_rows,
                    const Uint32 frames,            /* number of frames */
                    const int bits = 0)
      : DiTransTemplate<T>(planes, src_cols, src_rows, dest_cols, dest_rows, frames, bits),
        Left(left),
        Top(top),
        Columns(columns),
        Rows(rows)
    {
    }

    DiScaleTemplate(const int planes,
                    const Uint16 src_cols,          /* resolution of source image */
                    const Uint16 src_rows,
                    const Uint16 dest_cols,         /* resolution of destination image */
                    const Uint16 dest_rows,
                    const Uint32 frames,            /* number of frames */
                    const int bits = 0)
      : DiTransTemplate<T>(planes, src_cols, src_rows, dest_cols, dest_rows, frames, bits),
        Left(0),
        Top(0),
        Columns(src_cols),
        Rows(src_rows)
    {
    }

    virtual ~DiScaleTemplate()
    {
    }
    
    inline void scaleData(const T *src[],               // combined clipping and scaling UNTESTED for multi-frame images !!
                          T *dest[],
                          const int interpolate)
    {
        if ((src != NULL) && (dest != NULL))
        {
#ifdef DEBUG
            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_DebugMessages)
            {
                cout << "C/R: " << Columns << " " << Rows << endl;
                cout << "L/T: " << Left << " " << Top << endl;
                cout << "SX/Y: " << Src_X << " " << Src_Y << endl;
                cout << "DX/Y: " << Dest_X << " " << Dest_Y << endl;
            }
#endif
            if ((Src_X == Dest_X) && (Src_Y == Dest_Y))                         // no scaling
            {
                if ((Left == 0) && (Top == 0) && (Columns == Src_X) && (Rows == Src_Y))
                    copyPixel(src, dest);                                       // copying
                else
                    clipPixel(src, dest);                                       // clipping
            }
            else if ((Dest_X % Src_X == 0) && (Dest_Y % Src_Y == 0))            // replication
                replicatePixel(src, dest);
            else if ((Src_X % Dest_X == 0) && (Src_Y % Dest_Y == 0))            // supression
                suppressPixel(src, dest);
            else if (interpolate && (Bits <= MAX_INTERPOLATION_BITS))           // interpolation (ignore flag for replication/suppresion)
                interpolatePixel(src, dest);
            else                                                                // general scaling
                scalePixel(src, dest);
        }
    }

 protected:

    const Uint16 Left;
    const Uint16 Top;
    const Uint16 Columns;
    const Uint16 Rows;


 private:

   /*
    *   clip image to specified area
    */

    inline void clipPixel(const T *src[],
                          T *dest[])
    {
        const unsigned long x_feed = Columns - Src_X;
        const unsigned long y_feed = (unsigned long)(Rows - Src_Y) * (unsigned long)Columns;
        register Uint16 x;
        register Uint16 y;
        register const T *p;
        register T *q;
        for (int j = 0; j < Planes; j++)
        {
            p = src[j] + (unsigned long)Top * (unsigned long)Columns + Left;
            q = dest[j];
            for (unsigned long f = 0; f < Frames; f++)
            {
                for (y = 0; y < Dest_Y; y++)
                {
                    for (x = 0; x < Dest_X; x++)
                        *(q++) = *(p++);
                    p += x_feed;
                }
                p += y_feed;
            }
        }
    }


   /*
    *   enlarge image by an integer factor
    */

    inline void replicatePixel(const T *src[],
                               T *dest[])
    {
        const Uint16 x_factor = Dest_X / Src_X;
        const Uint16 y_factor = Dest_Y / Src_Y;
        const unsigned long x_feed = Columns;
        const unsigned long y_feed = (unsigned long)(Rows - Src_Y) * (unsigned long)Columns;
        const T *sp;
        register Uint16 x;
        register Uint16 y;
        register Uint16 dx;
        register Uint16 dy;
        register const T *p;
        register T *q;
        register T value;
        for (int j = 0; j < Planes; j++)
        {
            sp = src[j] + (unsigned long)Top * (unsigned long)Columns + Left;
            q = dest[j];
            for (Uint32 f = 0; f < Frames; f++)
            {
                for (y = 0; y < Src_Y; y++)
                {
                    for (dy = 0; dy < y_factor; dy++)
                    {
                        for (x = 0, p = sp; x < Src_X; x++)
                        {
                            value = *(p++);
                            for (dx = 0; dx < x_factor; dx++)
                                *(q++) = value;
                        }
                    }
                    sp += x_feed;
                }
                sp += y_feed;
            }
        }
    }
    

   /*
    *   shrink image by an integer divisor
    */

    inline void suppressPixel(const T *src[],
                              T *dest[])
    {
        const unsigned int x_divisor = Src_X / Dest_X;
        const unsigned long x_feed = (unsigned long)(Src_Y / Dest_Y) * (unsigned long)Columns - Src_X;
        const unsigned long y_feed = (unsigned long)(Rows - Src_Y) * (unsigned long)Columns;
        register Uint16 x;
        register Uint16 y;
        register const T *p;
        register T *q;
        for (int j = 0; j < Planes; j++)
        {
            p = src[j] + (unsigned long)Top * (unsigned long)Columns + Left;
            q = dest[j];
            for (Uint32 f = 0; f < Frames; f++)
            {
                for (y = 0; y < Dest_Y; y++)
                {
                    for (x = 0; x < Dest_X; x++)
                    {
                        *(q++) = *p;
                        p += x_divisor;
                    }
                    p += x_feed;
                }
                p += y_feed;
            }
        }
    }
    

   /*
    *   free scaling method without interpolation (necessary for overlays) ... it was a hard job ;-)
    */

    inline void scalePixel(const T *src[],
                           T *dest[])
    {
        const Uint16 xmin = (Dest_X < Src_X) ? Dest_X : Src_X;      // minimum width
        const Uint16 ymin = (Dest_Y < Src_Y) ? Dest_Y : Src_Y;      // minimum height
        Uint16 *x_step = new Uint16[xmin];
        Uint16 *y_step = new Uint16[ymin];
        Uint16 *x_fact = new Uint16[xmin];
        Uint16 *y_fact = new Uint16[ymin];
        if ((x_step != NULL) && (y_step != NULL) && (x_fact != NULL) && (y_fact != NULL))
        {
            register Uint16 x;
            register Uint16 y;
            if (Dest_X < Src_X)
                setScaleValues(x_step, Dest_X, Src_X);
            else if (Dest_X > Src_X)
                setScaleValues(x_fact, Src_X, Dest_X);
            if (Dest_X <= Src_X)
            {
                for (x = 0; x < xmin; x++)                      // initialize with default values
                    x_fact[x] = 1;
            }
            if (Dest_X >= Src_X)
            {
                for (x = 0; x < xmin; x++)                      // initialize with default values
                    x_step[x] = 1;
            }
            x_step[xmin - 1] += Columns - Src_X;                // skip to next line
            if (Dest_Y < Src_Y)
                setScaleValues(y_step, Dest_Y, Src_Y);
            else if (Dest_Y > Src_Y)
                setScaleValues(y_fact, Src_Y, Dest_Y);
            if (Dest_Y <= Src_Y)
            {
                for (y = 0; y < ymin; y++)                      // initialize with default values
                    y_fact[y] = 1;
            }
            if (Dest_Y >= Src_Y)
            {
                for (y = 0; y < ymin; y++)                      // initialize with default values
                    y_step[y] = 1;
            }
            y_step[ymin - 1] += Rows - Src_Y;                   // skip to next frame
            const T *sp;
            register Uint16 dx;
            register Uint16 dy;
            register const T *p;
            register T *q;
            register T value;
            for (int j = 0; j < Planes; j++)
            {
                sp = src[j] + (unsigned long)Top * (unsigned long)Columns + Left;
                q = dest[j];
                for (Uint32 f = 0; f < Frames; f++)
                {
                    for (y = 0; y < ymin; y++)
                    {
                        for (dy = 0; dy < y_fact[y]; dy++)
                        {
                            for (x = 0, p = sp; x < xmin; x++)
                            {
                                value = *p;
                                for (dx = 0; dx < x_fact[x]; dx++)
                                    *(q++) = value;
                                p += x_step[x];
                            }
                        }
                        sp += (unsigned long)y_step[y] * (unsigned long)Columns;
                    }
                }
            }
        }
        delete[] x_step;
        delete[] y_step;
        delete[] x_fact;
        delete[] y_fact;
    }


    inline void interpolatePixel(const T *src[],
                                 T *dest[])
    {
        if ((Src_X != Columns) || (Src_Y != Rows))
        {
            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Errors)
            {
               cerr << "ERROR: interpolated scaling and clipping at the same time not implemented" << endl;
               cerr << "       ... ignoring clipping region !" << endl;
            }
            Src_X = Columns;            // temporarily removed 'const' for 'Src_X' in class 'DiTransTemplate'
            Src_Y = Rows;               //                             ... 'Src_Y' ...
        }

        /*
         *   based on scaling algorithm of "Extended Portable Bitmap Toolkit" (pbmplus10dec91)
         *   (adapted to be used with signed pixel representation and inverse images - mono2)
         */

        register Uint16 x;
        register Uint16 y;
        register const T *p;
        register T *q;
        T const *sp = NULL;                         // initialization avoids compiler warning
        T const *fp;
        T *sq;

        const unsigned long sxscale = (unsigned long)(((double)Dest_X / (double)Src_X) * SCALE);
        const unsigned long syscale = (unsigned long)(((double)Dest_Y / (double)Src_Y) * SCALE);
        DiPixelRepresentationTemplate<T> rep;
        const signed long maxvalue = DicomImageClass::maxval(Bits - rep.isSigned());

        T *xtemp = new T[Src_X];
        signed long *xvalue = new signed long[Src_X];

        if ((xtemp == NULL) || (xvalue == NULL))
        {
            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Errors)
                cerr << "ERROR: can't allocate temporary buffers for interpolation scaling !" << endl;
    
            const unsigned long count = (unsigned long)Dest_X * (unsigned long)Dest_Y * Frames; 
            register unsigned long i;
            for (int j = 0; j < Planes; j++)
            {
                register T *qq = dest[j];
                for (i = 0; i < count; i++)                 // delete destination buffer
                    *(qq++) = 0;
            }
        }
        else
        { 
            for (int j = 0; j < Planes; j++)
            {
                fp = src[j];
                sq = dest[j];
                for (Uint32 f = 0; f < Frames; f++)
                {
                    for (x = 0; x < Src_X; x++)
                        xvalue[x] = HALFSCALE;
                    register unsigned long yfill = SCALE;
                    register unsigned long yleft = syscale;
                    register int yneed = 1;
                    int ysrc = 0;
                    for (y = 0; y < Dest_Y; y++)
                    {
                        if (Src_Y == Dest_Y)
                            {
                            sp = fp;
                            for (x = 0, p = sp, q = xtemp; x < Src_X; x++)
                                *(q++) = *(p++);
                            fp += Src_X;
                        }
                        else
                        {
                            while (yleft < yfill)
                            {
                                if (yneed && (ysrc < (int)Src_Y))
                                {
                                    sp = fp;
                                    fp += Src_X;
                                    ysrc++;
                                }
                                for (x = 0, p = sp; x < Src_X; x++)
                                    xvalue[x] += yleft * (signed long)(*(p++));
                                yfill -= yleft;
                                yleft = syscale;
                                yneed = 1;
                            }
                            if (yneed && (ysrc < (int)Src_Y))
                            {
                                sp = fp;
                                fp += Src_X;
                                ysrc++;
                                yneed = 0;
                            }
                            for (x = 0, p = sp, q = xtemp; x < Src_X; x++)
                            {
                                register signed long v = xvalue[x] + yfill * (signed long)(*(p++));
                                v /= SCALE;
                                *(q++) = (T)((v > maxvalue) ? maxvalue : v);
                                xvalue[x] = HALFSCALE;
                            }
                            yleft -= yfill;
                            if (yleft == 0)
                            {
                                yleft = syscale;
                                yneed = 1;
                            }
                            yfill = SCALE;
                        }
                        if (Src_X == Dest_X)
                        {
                            for (x = 0, p = xtemp, q = sq; x < Dest_X; x++)
                                *(q++) = *(p++);
                            sq += Dest_X;
                        }
                        else
                        {
                            register signed long v = HALFSCALE;
                            register unsigned long xfill = SCALE;
                            register unsigned long xleft;
                            register int xneed = 0;
                            q = sq;
                            for (x = 0, p = xtemp; x < Src_X; x++, p++)
                            {
                                xleft = sxscale;
                                while (xleft >= xfill)
                                {
                                    if (xneed)
                                    {
                                        q++;
                                        v = HALFSCALE;
                                    }
                                    v += xfill * (signed long)(*p);
                                    v /= SCALE;
                                    *q = (T)((v > maxvalue) ? maxvalue : v);
                                    xleft -= xfill;
                                    xfill = SCALE;
                                    xneed = 1;
                                }
                                if (xleft > 0)
                                {
                                    if (xneed)
                                    {
                                        q++;
                                        v = HALFSCALE;
                                        xneed = 0;
                                    }
                                    v += xleft * (signed long)(*p);
                                    xfill -= xleft;
                                }
                            }
                            if (xfill > 0)
                                v += xfill * (signed long)(*(--p));
                            if (!xneed)
                            {
                                v /= SCALE;
                                *q = (T)((v > maxvalue) ? maxvalue : v);
                            }
                            sq += Dest_X;
                        }
                    }
                }
            }
        }
        delete[] xtemp;
        delete[] xvalue;
    }

};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: discalet.h,v $
 * Revision 1.4  1999-02-03 17:35:14  joergr
 * Moved global functions maxval() and determineRepresentation() to class
 * DicomImageClass (as static methods).
 *
 * Revision 1.3  1998/12/22 14:39:44  joergr
 * Added some preparation to enhance interpolated scaling (clipping and
 * scaling) in the future.
 *
 * Revision 1.2  1998/12/16 16:39:45  joergr
 * Implemented combined clipping and scaling for pixel replication and
 * suppression.
 *
 * Revision 1.1  1998/11/27 15:47:11  joergr
 * Added copyright message.
 * Combined clipping and scaling methods.
 *
 * Revision 1.4  1998/05/11 14:53:29  joergr
 * Added CVS/RCS header to each file.
 *
 * 
 */
