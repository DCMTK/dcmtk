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
 *  Update Date:      $Date: 1999-09-17 13:07:20 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/discalet.h,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
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

/** Template class to scale images (on pixel data level).
 *  with and without interpolation
 */
template<class T>
class DiScaleTemplate
  : public DiTransTemplate<T>
{
    
 public:

    /** constructor, scale clipping area.
     *
     ** @param  planes     number of planes (1 or 3)
     *  @param  columns    width of source image
     *  @param  rows       height of source image
     *  @param  left       left coordinate of clipping area
     *  @param  top        top coordinate of clipping area
     *  @param  src_cols   width of clipping area
     *  @param  src_rows   height of clipping area
     *  @param  dest_cols  width of destination image (scaled image)
     *  @param  dest_rows  height of destination image
     *  @param  frames     number of frames 
     *  @param  bits       number of bits per plane/pixel
     */
    DiScaleTemplate(const int planes,
                    const Uint16 columns,           /* resolution of source image */
                    const Uint16 rows,
                    const signed long left,         /* origin of clipping area */
                    const signed long top,
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

    /** constructor, scale whole image.
     *
     ** @param  planes     number of planes (1 or 3)
     *  @param  src_cols   width of source image
     *  @param  src_rows   height of source image
     *  @param  dest_cols  width of destination image (scaled image)
     *  @param  dest_rows  height of destination image
     *  @param  frame      number of frames 
     *  @param  bits       number of bits per plane/pixel
     */
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

    /** destructor
     */
    virtual ~DiScaleTemplate()
    {
    }
    
    /** choose scaling/clipping algorithm depending on specified parameters.
     *
     ** @param  src          array of pointers to source image pixels
     *  @param  dest         array of pointers to destination image pixels
     *  @param  interpolate  interpolation algorithm (0 = no interpolation, 1 = pbmplus algorithm, 2 = c't algorithm)
     *  @param  value        value to be set outside the image boundaries (used for clipping, default: 0)
     */
    void scaleData(const T *src[],               // combined clipping and scaling UNTESTED for multi-frame images !!
                   T *dest[],
                   const int interpolate,
                   const T value = 0)
    {
        if ((src != NULL) && (dest != NULL))
        {
#ifdef DEBUG
            if (DicomImageClass::DebugLevel & DicomImageClass::DL_DebugMessages)
            {
                cout << "C/R: " << Columns << " " << Rows << endl;
                cout << "L/T: " << Left << " " << Top << endl;
                cout << "SX/Y: " << Src_X << " " << Src_Y << endl;
                cout << "DX/Y: " << Dest_X << " " << Dest_Y << endl;
            }
#endif
            if ((Left + (signed long)Src_X <= 0) || (Top + (signed long)Src_Y <= 0) ||            
                (Left >= Columns) || (Top >= Rows))
            {                                                                   // no image to be displayed
                if (DicomImageClass::DebugLevel & DicomImageClass::DL_Informationals)
                    cout << "INFO: clipping area is fully outside the image boundaries !" << endl;
                fillPixel(dest, value);                                         // ... fill bitmap
            }
            else if ((Src_X == Dest_X) && (Src_Y == Dest_Y))                    // no scaling
            {
                if ((Left == 0) && (Top == 0) && (Columns == Src_X) && (Rows == Src_Y))
                    copyPixel(src, dest);                                       // copying
                else if ((Left >= 0) && ((Uint16)(Left + Src_X) <= Columns) &&
                         (Top >= 0) && ((Uint16)(Top + Src_Y) <= Rows))
                    clipPixel(src, dest);                                       // clipping
                else
                    clipBorderPixel(src, dest, value);                          // clipping (with border)
            }
            else if ((interpolate == 1) && (Bits <= MAX_INTERPOLATION_BITS))    // interpolation (pbmplus)
                interpolatePixel(src, dest);
            else if ((interpolate == 2) && (Dest_X >= Src_X) && (Dest_Y >= Src_Y))    // interpolated expansion (c't)
                expandPixel(src, dest);
            else if ((interpolate == 2) && (Src_X >= Dest_X) && (Src_Y >= Dest_Y))    // interpolated reduction (c't)
                reducePixel(src, dest);
            else if ((Dest_X % Src_X == 0) && (Dest_Y % Src_Y == 0))            // replication
                replicatePixel(src, dest);
            else if ((Src_X % Dest_X == 0) && (Src_Y % Dest_Y == 0))            // supression
                suppressPixel(src, dest);
            else                                                                // general scaling
                scalePixel(src, dest);
        }
    }

 protected:

    /// left coordinate of clipping area
    const signed long Left;
    /// top coordinate of clipping area
    const signed long Top;
    /// width of source image
    const Uint16 Columns;
    /// height of source image
    const Uint16 Rows;


 private:

    /** clip image to specified area (only inside image boundaries).
     *  This is an optimization of the more general method clipBorderPixel().
     *
     ** @param  src   array of pointers to source image pixels
     *  @param  dest  array of pointers to destination image pixels
     */
    void clipPixel(const T *src[],
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
            for (unsigned long f = Frames; f != 0; f--)
            {
                for (y = Dest_Y; y != 0; y--)
                {
                    for (x = Dest_X; x != 0; x--)
                        *(q++) = *(p++);
                    p += x_feed;
                }
                p += y_feed;
            }
        }
    }
   
    /** clip image to specified area and add a border if necessary.
     *  NOT fully tested - UNTESTED for multi-frame and multi-plane images !!
     *
     ** @param  src    array of pointers to source image pixels
     *  @param  dest   array of pointers to destination image pixels
     *  @param  value  value to be set outside the image boundaries
     */
    void clipBorderPixel(const T *src[],
                         T *dest[],
                         const T value)
    {
        const Uint16 s_left = (Left > 0) ? Left : 0;
        const Uint16 s_top = (Top > 0) ? Top : 0;
        const Uint16 d_left = (Left < 0 ? (Uint16)(-Left) : 0);
        const Uint16 d_top = (Top < 0) ? (Uint16)(-Top) : 0;
        const Uint16 d_right = (Src_X + s_left < Columns + d_left) ? (Src_X - 1) : (Columns + d_left - s_left - 1);
        const Uint16 d_bottom = (Src_Y + s_top < Rows + d_top) ? (Src_Y - 1) : (Rows + d_top - s_top - 1);
        const Uint16 x_count = d_right - d_left + 1;
        const Uint16 y_count = d_bottom - d_top + 1;
        const unsigned long s_start = ((unsigned long)s_top * (unsigned long)Columns) + s_left;
        const unsigned long x_feed = Columns - x_count;
        const unsigned long y_feed = (unsigned long)(Rows - y_count) * Columns;
        const unsigned long t_feed = (unsigned long)d_top * (unsigned long)Src_X;
        const unsigned long b_feed = (unsigned long)(Src_Y - d_bottom - 1) * (unsigned long)Src_X;
        
        /*
         *  The approach is to divide the destination image in up to four areas outside the source image
         *  plus one area for the source image. The for and while loops are scanning linearly over the
         *  destination image and setting the appropriate value depending on the current area. This is
         *  different from most of the other algorithms in this toolkit where the source image is scanned
         *  linearly.
         */
        register Uint16 x;
        register Uint16 y;
        register unsigned long i;
        register const T *p;
        register T *q;
        for (int j = 0; j < Planes; j++)
        {
            p = src[j] + s_start;
            q = dest[j];
            for (unsigned long f = Frames; f != 0; f--)
            {
                for (i = t_feed; i != 0; i--)               // top
                    *(q++) = value;
                for (y = y_count; y != 0; y--)              // middle part:
                {
                    x = 0;
                    while (x < d_left)                      // - left
                    {
                        *(q++) = value;
                        x++;
                    }
                    while (x <= d_right)                    // - middle
                    {
                        *(q++) = *(p++);
                        x++;
                    }
                    while (x < Src_X)                       // - right
                    {
                        *(q++) = value;
                        x++;
                    }
                    p += x_feed;
                }
                for (i = b_feed; i != 0; i--)               // bottom
                    *(q++) = value;
                p += y_feed;
            }
        }
    }

    /** enlarge image by an integer factor.
     *  Pixels are replicated independently in both directions.
     *
     ** @param  src   array of pointers to source image pixels
     *  @param  dest  array of pointers to destination image pixels
     */
    void replicatePixel(const T *src[],
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
            for (Uint32 f = Frames; f != 0; f--)
            {
                for (y = Src_Y; y != 0; y--)
                {
                    for (dy = y_factor; dy != 0; dy--)
                    {
                        for (x = Src_X, p = sp; x != 0; x--)
                        {
                            value = *(p++);
                            for (dx = x_factor; dx != 0; dx--)
                                *(q++) = value;
                        }
                    }
                    sp += x_feed;
                }
                sp += y_feed;
            }
        }
    }
    
    /** shrink image by an integer divisor
     *  Pixels are suppressed independently in both directions.
     *
     ** @param  src   array of pointers to source image pixels
     *  @param  dest  array of pointers to destination image pixels
     */
    void suppressPixel(const T *src[],
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
            for (Uint32 f = Frames; f != 0; f--)
            {
                for (y = Dest_Y; y != 0; y--)
                {
                    for (x = Dest_X; x != 0; x--)
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
    
    /** free scaling method without interpolation.
     *  This algorithm is necessary for overlays (1 bpp).
     *
     ** @param  src   array of pointers to source image pixels
     *  @param  dest  array of pointers to destination image pixels
     */
    void scalePixel(const T *src[],
                    T *dest[])
    {
        const Uint16 xmin = (Dest_X < Src_X) ? Dest_X : Src_X;      // minimum width
        const Uint16 ymin = (Dest_Y < Src_Y) ? Dest_Y : Src_Y;      // minimum height
        Uint16 *x_step = new Uint16[xmin];
        Uint16 *y_step = new Uint16[ymin];
        Uint16 *x_fact = new Uint16[xmin];
        Uint16 *y_fact = new Uint16[ymin];

       /*
        *  Approach: If one pixel line has to be added or removed it is taken from the middle of the image (1/2).
        *  For two lines it is at 1/3 and 2/3 of the image and so on. It sounds easy but it was a hard job ;-)
        */

        if ((x_step != NULL) && (y_step != NULL) && (x_fact != NULL) && (y_fact != NULL))
        {
            register Uint16 x;
            register Uint16 y;
            if (Dest_X < Src_X)
                setScaleValues(x_step, Dest_X, Src_X);
            else if (Dest_X > Src_X)
                setScaleValues(x_fact, Src_X, Dest_X);
            if (Dest_X <= Src_X)
                OFBitmanipTemplate<Uint16>::setMem(x_fact, 1, xmin);  // initialize with default values
            if (Dest_X >= Src_X)
                OFBitmanipTemplate<Uint16>::setMem(x_step, 1, xmin);  // initialize with default values
            x_step[xmin - 1] += Columns - Src_X;                      // skip to next line
            if (Dest_Y < Src_Y)
                setScaleValues(y_step, Dest_Y, Src_Y);
            else if (Dest_Y > Src_Y)
                setScaleValues(y_fact, Src_Y, Dest_Y);
            if (Dest_Y <= Src_Y)
                OFBitmanipTemplate<Uint16>::setMem(y_fact, 1, ymin);  // initialize with default values
            if (Dest_Y >= Src_Y)
                OFBitmanipTemplate<Uint16>::setMem(y_step, 1, ymin);  // initialize with default values
            y_step[ymin - 1] += Rows - Src_Y;                         // skip to next frame
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


    /** free scaling method with interpolation.
     *
     ** @param  src   array of pointers to source image pixels
     *  @param  dest  array of pointers to destination image pixels
     */
    void interpolatePixel(const T *src[],
                          T *dest[])
    {
        if ((Src_X != Columns) || (Src_Y != Rows))
        {
            if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
            {
               cerr << "ERROR: interpolated scaling and clipping at the same time not implemented" << endl;
               cerr << "       ... ignoring clipping region !" << endl;
            }
            Src_X = Columns;            // temporarily removed 'const' for 'Src_X' in class 'DiTransTemplate'
            Src_Y = Rows;               //                             ... 'Src_Y' ...
        }

        /*
         *   based on scaling algorithm from "Extended Portable Bitmap Toolkit" (pbmplus10dec91)
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
            if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
                cerr << "ERROR: can't allocate temporary buffers for interpolation scaling !" << endl;
    
            const unsigned long count = (unsigned long)Dest_X * (unsigned long)Dest_Y * Frames; 
            for (int j = 0; j < Planes; j++)
                OFBitmanipTemplate<T>::zeroMem(dest[j], count);     // delete destination buffer
        }
        else
        { 
            for (int j = 0; j < Planes; j++)
            {
                fp = src[j];
                sq = dest[j];
                for (Uint32 f = Frames; f != 0; f--)
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
                            for (x = Src_X, p = sp, q = xtemp; x != 0; x--)
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
                            for (x = Dest_X, p = xtemp, q = sq; x != 0; x--)
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


    /** free scaling method with interpolation (only for expansion).
     *
     ** @param  src   array of pointers to source image pixels
     *  @param  dest  array of pointers to destination image pixels
     */
    void expandPixel(const T *src[],
                     T *dest[])
    {
        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Informationals)
            cerr << "INFO: expandPixel with interpolated c't algorithm" << endl;
        const double x_factor = (double)Src_X / (double)Dest_X;
        const double y_factor = (double)Src_Y / (double)Dest_Y;
        const unsigned long f_size = (unsigned long)Rows * (unsigned long)Columns;
        const T *sp;
        double bx, ex;
        double by, ey;
        int bxi, exi;
        int byi, eyi;
        unsigned long offset;
        double value, sum;
        double x_part, y_part;
        double l_factor, r_factor;
        double t_factor, b_factor;
        register Uint16 x, xi;
        register Uint16 y, yi;
        register const T *p;
        register T *q;

        /*
         *   based on scaling algorithm from "c't - Magazin fuer Computertechnik" (c't 11/94)
         *   (adapted to be used with signed pixel representation and inverse images - mono2)
         */

        for (int j = 0; j < Planes; j++)
        {
            sp = src[j] + (unsigned long)Top * (unsigned long)Columns + Left;
            q = dest[j];
            for (Uint32 f = 0; f < Frames; f++)
            {
                for (y = 0; y < Dest_Y; y++)
                {
                    by = y_factor * (double)y;
                    ey = y_factor * (double)(y + 1);
                    byi = (int)by;
                    eyi = (int)ey;
                    if ((double)eyi == ey) eyi--;
                    y_part = (double)eyi / y_factor;
                    b_factor = y_part - (double)y;
                    t_factor = (double)(y + 1) - y_part;
                    for (x = 0; x < Dest_X; x++)
                    {
                        value = 0;
                        bx = x_factor * (double)x;
                        ex = x_factor * (double)(x + 1);
                        bxi = (int)bx;
                        exi = (int)ex;
                        if ((double)exi == ex) exi--;
                        x_part = (double)exi / x_factor;
                        l_factor = x_part - (double)x;
                        r_factor = (double)(x + 1) - x_part;
                        offset = ((unsigned long)(byi - 1)) * (unsigned long)Columns;
                        for (yi = byi; yi <= eyi; yi++)
                        {
                            offset += Columns;
                            p = sp + offset + (unsigned long)bxi;
                            for (xi = bxi; xi <= exi; xi++)
                            {
                                sum = (double)*(p++);
                                if (bxi != exi)
                                {
                                    if (xi == bxi)
                                        sum *= l_factor;
                                    else
                                        sum *= r_factor;
                                }
                                if (byi != eyi)
                                {
                                    if (yi == byi)
                                        sum *= b_factor;
                                    else
                                        sum *= t_factor;
                                }
                                value += sum;
                            }
                        }
                        *(q++) = (T)(value + 0.5);
                    }
                }
                sp += f_size;                                        // skip to next frame start: UNTESTED
            }
        }
    }


    /** free scaling method with interpolation (only for reduction).
     *
     ** @param  src   array of pointers to source image pixels
     *  @param  dest  array of pointers to destination image pixels
     */
    void reducePixel(const T *src[],
                          T *dest[])
    {
        if (DicomImageClass::DebugLevel & (DicomImageClass::DL_Informationals | DicomImageClass::DL_Warnings))
            cerr << "INFO: reducePixel with interpolated c't algorithm ... still a little BUGGY !" << endl;
        const double x_factor = (double)Src_X / (double)Dest_X;
        const double y_factor = (double)Src_Y / (double)Dest_Y;
        const double xy_factor = x_factor * y_factor;
        const unsigned long f_size = (unsigned long)Rows * (unsigned long)Columns;
        const T *sp;
        double bx, ex;
        double by, ey;
        int bxi, exi;
        int byi, eyi;
        unsigned long offset;
        double value, sum;
        double l_factor, r_factor;
        double t_factor, b_factor;
        register Uint16 x, xi;
        register Uint16 y, yi;
        register const T *p;
        register T *q;

        /*
         *   based on scaling algorithm from "c't - Magazin fuer Computertechnik" (c't 11/94)
         *   (adapted to be used with signed pixel representation and inverse images - mono2)
         */

        for (int j = 0; j < Planes; j++)
        {
            sp = src[j] + (unsigned long)Top * (unsigned long)Columns + Left;
            q = dest[j];
            for (Uint32 f = 0; f < Frames; f++)
            {
                for (y = 0; y < Dest_Y; y++)
                {
                    by = y_factor * (double)y;
                    ey = y_factor * (double)(y + 1);
                    byi = (int)by;
                    eyi = (int)ey;
                    if ((double)eyi == ey) eyi--;
                    b_factor = 1 + (double)byi - by;
                    t_factor = ey - (double)eyi;
                    for (x = 0; x < Dest_X; x++)
                    {
                        value = 0;
                        bx = x_factor * (double)x;
                        ex = x_factor * (double)(x + 1);
                        bxi = (int)bx;
                        exi = (int)ex;
                        if ((double)exi == ex) exi--;
                        l_factor = 1 + (double)bxi - bx;
                        r_factor = ex - (double)exi;
                        offset = ((unsigned long)(byi - 1)) * (unsigned long)Columns;
                        for (yi = byi; yi <= eyi; yi++)
                        {
                            offset += Columns;
                            p = sp + offset + (unsigned long)bxi;
                            for (xi = bxi; xi <= exi; xi++)
                            {
                                sum = (double)*(p++) / xy_factor;
                                if (xi == bxi)
                                    sum *= l_factor;
                                else if (xi = exi)
                                    sum *= r_factor;
                                if (yi == byi)
                                    sum *= b_factor;
                                else if (yi == eyi)
                                    sum *= t_factor;
                                value += sum;
                            }
                        }
                        *(q++) = (T)(value + 0.5);
                    }
                }
                sp += f_size;                                        // skip to next frame start: UNTESTED
            }
        }
    }
};

#endif


/*
 *
 * CVS/RCS Log:
 * $Log: discalet.h,v $
 * Revision 1.10  1999-09-17 13:07:20  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.9  1999/08/25 16:41:55  joergr
 * Added new feature: Allow clipping region to be outside the image
 * (overlapping).
 *
 * Revision 1.8  1999/07/23 14:09:24  joergr
 * Added new interpolation algorithm for scaling.
 *
 * Revision 1.7  1999/04/28 14:55:05  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.6  1999/03/24 17:20:24  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.5  1999/02/11 16:42:10  joergr
 * Removed inline declarations from several methods.
 *
 * Revision 1.4  1999/02/03 17:35:14  joergr
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
